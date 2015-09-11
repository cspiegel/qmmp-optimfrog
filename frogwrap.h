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

#ifndef QMMP_OPTIMFROG_OPTIMFROGWRAP_H
#define QMMP_OPTIMFROG_OPTIMFROGWRAP_H

#include <exception>
#include <map>
#include <string>

#include <QIODevice>

#include <OptimFROG.h>

class FrogWrap
{
  public:
    class InvalidFile : public std::exception
    {
      public:
        InvalidFile() : std::exception() { }
    };

    explicit FrogWrap(QIODevice *);
    FrogWrap(const FrogWrap &) = delete;
    FrogWrap &operator=(const FrogWrap &) = delete;
    ~FrogWrap();

    static bool can_play(std::string);

    long read(void *, long);
    void seek(int);

    long rate() { return info.samplerate; }
    long channels() { return info.channels; }
    long depth() { return info.bitspersample; }
    long length() { return info.length_ms; }
    long bitrate() { return info.bitrate; }
    long version() { return info.version; }
    double compression() { return 1000.0 * bitrate() / rate() / channels() / depth(); }

    bool has_tags() { return !tags.empty(); }
    std::string get_tag(std::string tag) { return tags.at(tag); }

  private:
    void *decoder = OptimFROG_createInstance();
    OptimFROG_Info info;
    bool is_signed;

    std::map<std::string, std::string> tags;

    static QIODevice *VFS(void *instance) { return reinterpret_cast<QIODevice *>(instance); }

    static condition_t ofr_close(void *) { return C_TRUE; }
    static sInt32_t ofr_read(void *instance, void *buf, uInt32_t n) { return VFS(instance)->read(reinterpret_cast<char *>(buf), n); }
    static condition_t ofr_eof(void* instance) { return VFS(instance)->atEnd(); }
    static condition_t ofr_seekable(void* instance) { return !VFS(instance)->isSequential(); }
    static sInt64_t ofr_length(void* instance) { return VFS(instance)->size(); }
    static sInt64_t ofr_get_pos(void* instance) { return VFS(instance)->pos(); }
    static condition_t ofr_seek(void* instance, sInt64_t offset) { return VFS(instance)->seek(offset); }
};

#endif
