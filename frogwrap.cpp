/*-
 * Copyright (c) 2015 Chris Spiegel
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include <QIODevice>

#include <OptimFROG/OptimFROG.h>

#include "frogwrap.h"

FrogWrap::FrogWrap(QIODevice *device)
{
  static ReadInterface rint =
  {
    ofr_close,
    ofr_read,
    ofr_eof,
    ofr_seekable,
    ofr_length,
    ofr_get_pos,
    ofr_seek,
  };
  OptimFROG_Tags ofr_tags;

  if(decoder == C_NULL) throw InvalidFile();

  if(!OptimFROG_openExt(decoder, &rint, device, C_TRUE))
  {
    OptimFROG_destroyInstance(decoder);
    throw InvalidFile();
  }

  OptimFROG_getInfo(decoder, &info);
  /* 24- and 32-bit audio is converted to 16-bit. */
  if(info.bitspersample > 16) info.bitspersample = 16;
  if(std::strncmp(info.sampleType, "SINT", 4) != 0 && std::strncmp(info.sampleType, "UINT", 4) != 0)
  {
    OptimFROG_destroyInstance(decoder);
    throw InvalidFile();
  }

  is_signed = info.sampleType[0] == 'S';

  OptimFROG_getTags(decoder, &ofr_tags);
  for(uInt32_t i = 0; i < ofr_tags.keyCount; i++)
  {
    tags.insert(std::pair<std::string, std::string>(ofr_tags.keys[i], ofr_tags.values[i]));
  }
  OptimFROG_freeTags(&ofr_tags);
}

FrogWrap::~FrogWrap()
{
  OptimFROG_close(decoder);
  OptimFROG_destroyInstance(decoder);
}

bool FrogWrap::can_play(std::string filename)
{
  OptimFROG_Info junk_info;

  return OptimFROG_infoFile(const_cast<char *>(filename.c_str()), &junk_info, C_NULL) == OptimFROG_NoError;
}

long FrogWrap::read(void *buf, long size)
{
  sInt32_t n;
  int bytes = depth() / 8;
  sInt32_t point_conversion = bytes * info.channels;

  n = OptimFROG_read(decoder, buf, size / point_conversion, C_TRUE);
  n = n > 0 ? n * point_conversion : 0;

  /* Qmmp doesn't support unsigned samples, so convert here. */
  if(!is_signed)
  {
    unsigned char *overlay = reinterpret_cast<unsigned char *>(buf);

    for(sInt32_t i = bytes - 1; i < n; i += bytes)
    {
      overlay[i] ^= 0x80;
    }
  }

  return n;
}

void FrogWrap::seek(int pos)
{
  if(OptimFROG_seekable(decoder)) OptimFROG_seekTime(decoder, pos);
}
