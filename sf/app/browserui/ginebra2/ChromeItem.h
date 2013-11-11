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

#ifndef __GINEBRA_CHROMEITEM_H__
#define __GINEBRA_CHROMEITEM_H__

#include <QtGui>
#include "ChromeSnippet.h"


namespace GVA {


  class ChromeItem : public QGraphicsWidget
  {
    Q_OBJECT
  public:
    ChromeItem(ChromeSnippet * snippet=0, QGraphicsItem* parent=0);
    virtual ~ChromeItem();
    virtual ChromeSnippet * snippet() { return m_snippet;}
    virtual void  setSnippet(ChromeSnippet * s);

  Q_SIGNALS:
    void mouseEvent(QEvent::Type type);
  protected:
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

    ChromeSnippet * m_snippet;

  };

} // end of namespace GVA

#endif // __GINEBRA_CHROMEITEM_H__
