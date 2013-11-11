/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, 
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/

#include "xbel.h"

#include <QtCore/QFile>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>
#include <QtCore/QVariant>
#include <QtCore/QBuffer>
#include <QtGui/QImage>
#include <QtGui/QImageWriter>
#include "bookmarks.h"

namespace WRT {

XbelReader::XbelReader()
{
}

BookmarkNode *XbelReader::read(const QString &fileName)
{
    QFile file(fileName);
    if (!file.exists()) {
        return new BookmarkNode(BookmarkNode::Root);
    }
    file.open(QFile::ReadOnly);
    return read(&file);
}

BookmarkNode *XbelReader::read(QIODevice *device)
{
    BookmarkNode *root = new BookmarkNode(BookmarkNode::Root);
    setDevice(device);
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            QString version = attributes().value(QLatin1String("version")).toString();
            if (name() == QLatin1String("xbel")
                && (version.isEmpty() || version == QLatin1String("1.0"))) {
                readXBEL(root);
            } else {
                raiseError(QObject::tr("The file is not an XBEL version 1.0 file."));
            }
        }
    }
    return root;
}

void XbelReader::readXBEL(BookmarkNode *parent)
{
    Q_ASSERT(isStartElement() && name() == QLatin1String("xbel"));

    while (!atEnd()) {
        readNext();
        if (isEndElement())
            break;

        if (isStartElement()) {
            if (name() == QLatin1String("folder"))
                readFolder(parent);
            else if (name() == QLatin1String("bookmark"))
                readBookmarkNode(parent);
            else if (name() == QLatin1String("separator"))
                readSeparator(parent);
            else
                skipUnknownElement();
        }
    }
}

void XbelReader::readFolder(BookmarkNode *parent)
{
    Q_ASSERT(isStartElement() && name() == QLatin1String("folder"));

    BookmarkNode *folder = new BookmarkNode(BookmarkNode::Folder, parent);
    folder->expanded = (attributes().value(QLatin1String("folded")) == QLatin1String("no"));

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement()) {
            if (name() == QLatin1String("title"))
                readTitle(folder);
            else if (name() == QLatin1String("date"))
                readDate(folder);
            else if (name() == QLatin1String("desc"))
                readDescription(folder);
            else if (name() == QLatin1String("folder"))
                readFolder(folder);
            else if (name() == QLatin1String("bookmark"))
                readBookmarkNode(folder);
            else if (name() == QLatin1String("separator"))
                readSeparator(folder);
            else
                skipUnknownElement();
        }
    }
}

void XbelReader::readTitle(BookmarkNode *parent)
{
    Q_ASSERT(isStartElement() && name() == QLatin1String("title"));
    parent->title = readElementText();
}

void XbelReader::readDate(BookmarkNode *parent)
{
    Q_ASSERT(isStartElement() && name() == QLatin1String("date"));
    parent->date = QDate::fromString(readElementText());
}

void XbelReader::readLastVisted(BookmarkNode *parent)
{
    Q_ASSERT(isStartElement() && name() == QLatin1String("visited"));
    parent->lastVisited = QTime::fromString(readElementText());
}

void XbelReader::readDescription(BookmarkNode *parent)
{
    Q_ASSERT(isStartElement() && name() == QLatin1String("desc"));
    parent->desc = readElementText();
}

void XbelReader::readFavicon(BookmarkNode *parent)
{
    Q_ASSERT(isStartElement() && name() == QLatin1String("favicon"));

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement()) {
            if (name() == QLatin1String("data")){
                QString format = attributes().value(QLatin1String("format")).toString();
                ulong len = attributes().value(QLatin1String("length")).toString().toULong();

                QString data = readElementText();

                QPixmap img;
                const int lengthOffset = 4;
                int baSize = data.length() / 2 + lengthOffset;
                uchar *ba = new uchar[ baSize ];
                for ( int i = lengthOffset; i < baSize; ++i ) {
                char h = data[ 2 * (i-lengthOffset) ].toLatin1();
                char l = data[ 2 * (i-lengthOffset) + 1 ].toLatin1();
                uchar r = 0;
                if ( h <= '9' )
                   r += h - '0';
                else
                   r += h - 'a' + 10;
                r = r << 4;
                if ( l <= '9' )
                   r += l - '0';
                else
                   r += l - 'a' + 10;
                ba[ i ] = r;
               }

               if ( format == "XPM.GZ" || format == "XBM.GZ" ) {

              if ( len < (ulong)data.length() * 5 )
                 len = data.length() * 5;
              // qUncompress() expects the first 4 bytes to be the expected length of
              // the uncompressed data
              ba[0] = ( len & 0xff000000 ) >> 24;
              ba[1] = ( len & 0x00ff0000 ) >> 16;
              ba[2] = ( len & 0x0000ff00 ) >> 8;
              ba[3] = ( len & 0x000000ff );
              QByteArray baunzip = qUncompress( ba, baSize );
              img.loadFromData( (const uchar*)baunzip.data(), baunzip.size(), format.left(format.indexOf('.')).toLatin1() );
               } else {
             img.loadFromData( (const uchar*)ba+lengthOffset, baSize-lengthOffset, format.toLatin1() );
               }
              delete [] ba;
              parent->favicon = QIcon(img);
            }
            else
                skipUnknownElement();
        }


        //parent->favicon = (QIcon)QByteArray::fromBase64(str);
    }
}

void XbelReader::readSeparator(BookmarkNode *parent)
{
    new BookmarkNode(BookmarkNode::Separator, parent);
    // empty elements have a start and end element
    readNext();
}

void XbelReader::readBookmarkNode(BookmarkNode *parent)
{
    Q_ASSERT(isStartElement() && name() == QLatin1String("bookmark"));
    BookmarkNode *bookmark = new BookmarkNode(BookmarkNode::Bookmark, parent);
    bookmark->url = attributes().value(QLatin1String("href")).toString();
    while (!atEnd()) {
        readNext();
        if (isEndElement())
            break;

        if (isStartElement()) {
            if (name() == QLatin1String("title"))
                readTitle(bookmark);
            else if (name() == QLatin1String("visited"))
                readLastVisted(bookmark);
            else if (name() == QLatin1String("desc"))
                readDescription(bookmark);
            else if (name() == QLatin1String("favicon"))
                readFavicon(bookmark);
            else
                skipUnknownElement();
        }
    }
    if (bookmark->title.isEmpty())
        bookmark->title = QObject::tr("Unknown title");
}
void XbelReader::skipUnknownElement()
{
    Q_ASSERT(isStartElement());

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement())
            skipUnknownElement();
    }
}


XbelWriter::XbelWriter()
{
    setAutoFormatting(true);
}

bool XbelWriter::write(const QString &fileName, const BookmarkNode *root)
{
    QFile file(fileName);
    if (!root || !file.open(QFile::WriteOnly))
        return false;
    return write(&file, root);
}

bool XbelWriter::write(QIODevice *device, const BookmarkNode *root)
{
    setDevice(device);

    writeStartDocument();
    writeDTD(QLatin1String("<!DOCTYPE xbel>"));
    writeStartElement(QLatin1String("xbel"));
    writeAttribute(QLatin1String("version"), QLatin1String("1.0"));
    if (root->type() == BookmarkNode::Root) {
        for (int i = 0; i < root->children().count(); ++i)
            writeItem(root->children()[i]);
    } else {
        writeItem(root);
    }

    writeEndDocument();
    return true;
}

void XbelWriter::writeItem(const BookmarkNode *parent)
{
    switch (parent->type()) {
    case BookmarkNode::Folder:
        writeStartElement(QLatin1String("folder"));
        writeAttribute(QLatin1String("folded"), parent->expanded ? QLatin1String("no") : QLatin1String("yes"));
        writeTextElement(QLatin1String("title"), parent->title);
        writeTextElement(QLatin1String("date"), parent->date.toString());
        for (int i = 0; i < parent->children().count(); ++i)
            writeItem(parent->children()[i]);
        writeEndElement();
        break;
    case BookmarkNode::Bookmark:
        writeStartElement(QLatin1String("bookmark"));
        if (!parent->url.isEmpty())
            writeAttribute(QLatin1String("href"), parent->url);
        writeTextElement(QLatin1String("title"), parent->title);
        if (!parent->desc.isEmpty())
            writeAttribute(QLatin1String("desc"), parent->desc);
        writeTextElement(QLatin1String("visited"), parent->lastVisited.toString());
        if (!parent->favicon.isNull()){
            writeStartElement(QLatin1String("favicon"));
            QIcon icon = parent->favicon;
            writeImageData(icon.pixmap(16,16).toImage());
            writeEndElement();
        }
        writeEndElement();
        break;
    case BookmarkNode::Separator:
        writeEmptyElement(QLatin1String("separator"));
        break;
    default:
        break;
    }
}

void XbelWriter::writeImageData(const QImage &img)
{
    QByteArray ba;
    QBuffer buf( &ba );
    buf.open( QIODevice::WriteOnly | QIODevice::Text );
    QByteArray format;
    bool compress = FALSE;
    if (img.hasAlphaChannel()){
        format = "PNG";
    } else {
        format = img.depth() > 1 ? "XPM" : "XBM";
        compress = TRUE;
    }
    QImageWriter iio ( &buf, format );
    iio.write( img );
    buf.close();
    QByteArray bazip = ba;
    int i = 0;
    if (compress){
        bazip = qCompress( ba );
        format += ".GZ";
        i = 4;
    }
    ulong len = bazip.size();
    writeStartElement(QLatin1String("data"));
    writeAttribute(QLatin1String("format"), format);
    QString siz;
    writeAttribute(QLatin1String("length"), siz.setNum(ba.size()));
    static const char hexchars[] = "0123456789abcdef";
    for (; i < (int)len; ++i) {
        uchar s = (uchar) bazip[i];
        QString chars;
        chars.append( hexchars[s >> 4] );
        chars.append( hexchars[s & 0x0f] );
        writeCharacters(chars);
    }
    writeEndElement();
}
}

