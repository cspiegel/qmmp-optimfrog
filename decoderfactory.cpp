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

#include <stdexcept>
#include <map>

#include <QFile>
#include <QIODevice>
#include <QList>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QTranslator>
#include <QtPlugin>

#include <qmmp/qmmp.h>

#include "decoderfactory.h"
#include "decoder.h"
#include "frogwrap.h"
#include "metadatamodel.h"

bool OFRDecoderFactory::canDecode(QIODevice *device) const
{
  try
  {
    FrogWrap frog(device);
    return true;
  }
  catch(const FrogWrap::InvalidFile &)
  {
    return false;
  }
}

DecoderProperties OFRDecoderFactory::properties() const
{
  DecoderProperties properties;

  properties.name = tr("OptimFROG Plugin");
  properties.shortName = "cas-ofr";
  properties.filters << "*.ofr" << "*.ofs";
  properties.description = tr("OptimFROG Files");
  properties.protocols << "file";
  properties.hasAbout = true;
  properties.hasSettings = false;
  properties.noInput = true;

  return properties;
}

Decoder *OFRDecoderFactory::create(const QString &, QIODevice *device)
{
  return new OFRDecoder(device);
}

QList<TrackInfo *> OFRDecoderFactory::createPlayList(const QString &filename, TrackInfo::Parts parts, QStringList *)
{
  QList<TrackInfo *> list;
  QFile file(filename);

  if(parts & (TrackInfo::MetaData | TrackInfo::Properties))
  {
    if(file.open(QIODevice::ReadOnly))
    {
      try
      {
        FrogWrap frog(&file);
        TrackInfo *file_info = new TrackInfo(filename);

        if(parts & TrackInfo::Properties)
        {
          file_info->setDuration(frog.length() / 1000);
        }

        if((parts & TrackInfo::MetaData) && frog.has_tags())
        {
          std::map<enum Qmmp::MetaData, std::string> tagmap =
          {
            { Qmmp::TITLE, "title" },
            { Qmmp::ARTIST, "artist" },
            { Qmmp::ALBUM, "album" },
            { Qmmp::COMMENT, "comment" },
            { Qmmp::GENRE, "genre" },
            { Qmmp::COMPOSER, "composer" },
            { Qmmp::YEAR, "year" },
            { Qmmp::TRACK, "track" },
          };

          for(auto &pair : tagmap)
          {
            try
            {
              QString value = QString::fromStdString(frog.get_tag(pair.second));
              file_info->setValue(pair.first, value.replace('\n', "<br>"));
            }
            catch(const std::out_of_range &)
            {
            }
          }
        }

        list << file_info;
      }
      catch(const FrogWrap::InvalidFile &)
      {
      }
    }
  }

  return list;
}

MetaDataModel *OFRDecoderFactory::createMetaDataModel(const QString &path, bool)
{
  return new OFRMetaDataModel(path);
}

void OFRDecoderFactory::showSettings(QWidget *)
{
}

void OFRDecoderFactory::showAbout(QWidget *parent)
{
  QString title = tr("About OptimFROG Audio Plugin");
  QString text = tr("OptimFROG Audio Plugin");
  text += "\n";
  text += tr("Written by: Chris Spiegel <cspiegel@gmail.com>");
  QMessageBox::about(parent, title, text);
}

QString OFRDecoderFactory::translation() const
{
  return QString(":/cas-optimfrog_plugin_");
}
