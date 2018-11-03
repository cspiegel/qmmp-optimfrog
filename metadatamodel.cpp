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

#include <QFile>
#include <QHash>
#include <QObject>
#include <QString>

#include <qmmp/metadatamodel.h>

#include "metadatamodel.h"
#include "frogwrap.h"

OFRMetaDataModel::OFRMetaDataModel(const QString &path, QObject *parent) : MetaDataModel(parent)
{
  QFile file(path);

  if(file.open(QIODevice::ReadOnly))
  {
    try
    {
      FrogWrap frog(&file);

      ap.insert(tr("Bitrate"), tr("%1 kbps").arg(frog.bitrate()));
      ap.insert(tr("Sample rate"), tr("%1 Hz").arg(frog.rate()));
      ap.insert(tr("Channels"), QString::number(frog.channels()));
      ap.insert(tr("Version"), QString::number(frog.version()));
      ap.insert(tr("Compression ratio"), QString::number(frog.compression()));
    }
    catch(const FrogWrap::InvalidFile &)
    {
    }
  }
}

QHash<QString, QString> OFRMetaDataModel::audioProperties()
{
  return ap;
}
