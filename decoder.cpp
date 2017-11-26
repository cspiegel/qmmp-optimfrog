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

#include <memory>

#include <QIODevice>

#include <qmmp/decoder.h>

#include "decoder.h"

OFRDecoder::OFRDecoder(QIODevice *device) : Decoder(device)
{
}

bool OFRDecoder::initialize()
{
  if(!input()) return false;
  if(!input()->isOpen() && !input()->open(QIODevice::ReadOnly)) return false;

  try
  {
    frog = std::unique_ptr<FrogWrap>(new FrogWrap(input()));
  }
  catch(FrogWrap::InvalidFile)
  {
    return false;
  }

  enum Qmmp::AudioFormat format;
  switch(frog->depth())
  {
    case 8:
      format = Qmmp::PCM_S8;
      break;
    case 16:
      format = Qmmp::PCM_S16LE;
      break;
    default:
      return false;
  }

  configure(frog->rate(), frog->channels(), format);

  return true;
}

qint64 OFRDecoder::totalTime() const
{
  return frog->length();
}

int OFRDecoder::bitrate() const
{
  return frog->bitrate();
}

qint64 OFRDecoder::read(unsigned char *audio, qint64 max_size)
{
  return frog->read(audio, max_size);
}

void OFRDecoder::seek(qint64 pos)
{
  frog->seek(pos);
}
