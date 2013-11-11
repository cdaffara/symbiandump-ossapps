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

#ifndef __WEBPAGEDATA_H__
#define __WEBPAGEDATA_H__

#include <QImage>
#include <QPoint>
#include <QMetaType>
#include "BWFGlobal.h"

class BWF_EXPORT WebPageData : public QObject
{
    Q_OBJECT
public:
    WebPageData(){};
    WebPageData(QObject *parent) : QObject(parent) {};
    WebPageData(const WebPageData& o) :  m_thumbnail(o.m_thumbnail),m_zoomFactor(o.m_zoomFactor),m_contentsPos(o.m_contentsPos) {};
    QImage  m_thumbnail;
    qreal   m_zoomFactor;
    QPoint  m_contentsPos;
};
Q_DECLARE_METATYPE (WebPageData)
QDataStream &operator<<(QDataStream &out, const WebPageData &myObj);
QDataStream &operator>>(QDataStream &in, WebPageData &myObj);
#endif
