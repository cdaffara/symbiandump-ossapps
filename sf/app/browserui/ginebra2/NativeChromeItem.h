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

#ifndef __GINEBRA_NATIVECHROMEITEM_H__
#define __GINEBRA_NATIVECHROMEITEM_H__

#include <QtGui>
#include <QWebElement>
#include "ChromeItem.h"


namespace GVA {

class ChromeSnippet;

  class NativeChromeItem : public ChromeItem
  {
  public:
    NativeChromeItem(ChromeSnippet * snippet,  QGraphicsItem* parent=0);
    virtual ~NativeChromeItem();
    static void CSSToQColor(QString cssColor, QColor & color);
  };

} // end of namespace GVA

#endif // __GINEBRA_NATIVECHROMEITEM_H__
