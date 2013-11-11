/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __ZOOMMETADATA_H__
#define __ZOOMMETADATA_H__

#ifdef BEDROCK_TILED_BACKING_STORE
#include <QRectF>
#include <QString>
#endif

// To store the view port proeperties that affect zoom set through meta data. 
// This is available in the main frame of the page but to avoid parsing the meta
// data again, store it

struct ZoomMetaData{

public:

   /* These are currently not needed when we set the zoom again */
   /* 
   int viewportWidth;
   int viewportHeight;
   qreal initialScale;
   */ 
   qreal minScale;
   qreal maxScale;
   qreal zoomValue;
   bool userScalable;
#ifdef BEDROCK_TILED_BACKING_STORE
   qreal initialScale;
   QRectF rect;
   QRectF webViewSize;
   qreal scale;
   QSizeF viewportSize;
   QString m_specifiedWidth;
   QString m_specifiedHeight;
#endif
};
#endif  //__ZOOMMETADATA_H__
