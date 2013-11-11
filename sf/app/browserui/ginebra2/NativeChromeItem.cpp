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

#include "NativeChromeItem.h"
#include "ChromeItem.h"
#include "ChromeSnippet.h"

namespace GVA {

  NativeChromeItem::NativeChromeItem(ChromeSnippet * snippet, QGraphicsItem* parent)
    : ChromeItem(snippet, parent)
  {
    //Set opacity from element CSS
    QString CSSOpacity = m_snippet->element().styleProperty("opacity", QWebElement::ComputedStyle);
    setOpacity(CSSOpacity.toFloat());
  }

  NativeChromeItem::~NativeChromeItem()
  {
  }

  void NativeChromeItem::CSSToQColor(QString cssColor, QColor & color){
    QStringList vals = cssColor.remove("rgb(").remove(")").split(", ");
    //qDebug() << vals[0] << ":" << vals[1] << ":" << vals[2];
    if (vals.size() == 4) {
        color.setRgb(vals[0].toInt(), vals[1].toInt(), vals[2].toInt(), vals[3].toInt());
    } else if  (vals.size() == 3) {
        color.setRgb(vals[0].toInt(), vals[1].toInt(), vals[2].toInt());
    }
    else {
        color.setNamedColor(cssColor);      
    }
  }

} // end of namespace GVA
