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

#ifndef XBEL_H
#define XBEL_H

#include <QtCore/QXmlStreamReader>
#include <QtCore/QDateTime>
#include <QtGui/QIcon>
#include <QtCore/QUrl>
#include <QtCore/QXmlStreamWriter>

namespace WRT {

class BookmarkNode;

class XbelReader : public QXmlStreamReader
{
public:
    XbelReader();
    BookmarkNode *read(const QString &fileName);
    BookmarkNode *read(QIODevice *device);

private:
    void skipUnknownElement();
    void readXBEL(BookmarkNode *parent);
    void readTitle(BookmarkNode *parent);
    void readDate(BookmarkNode *parent);
    void readLastVisted(BookmarkNode *parent);
    void readDescription(BookmarkNode *parent);
    void readFavicon(BookmarkNode *parent);
    void readSeparator(BookmarkNode *parent);
    void readFolder(BookmarkNode *parent);
    void readBookmarkNode(BookmarkNode *parent);
};


class XbelWriter : public QXmlStreamWriter
{
public:
    XbelWriter();
    bool write(const QString &fileName, const BookmarkNode *root);
    bool write(QIODevice *device, const BookmarkNode *root);

private:
    void writeItem(const BookmarkNode *parent);
    void writeImageData(const QImage &img);
};
}
#endif // XBEL_H

