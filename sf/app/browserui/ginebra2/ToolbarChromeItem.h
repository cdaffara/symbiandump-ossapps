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

#ifndef __GINEBRA_TOOLBARCHROMEITEM_H
#define __GINEBRA_TOOLBARCHROMEITEM_H

#include <QtGui>
#include "ChromeSnippet.h"
#include "NativeChromeItem.h"

namespace GVA {


  class ToolbarChromeItem : public ChromeItem
  {
    Q_OBJECT
  public:
    ToolbarChromeItem(ChromeSnippet* snippet, QGraphicsItem* parent = 0);
    virtual ~ToolbarChromeItem();
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* opt, QWidget* widget);
    virtual void setSnippet(ChromeSnippet* snippet);
    QPen pen() { return m_pen;}
    QLinearGradient grad() { return m_grad;}
    qreal opacity() { return m_opacity;}
    void setOpacity(qreal opacity) {m_opacity = opacity;}
    QPainterPath* partialbg() { return m_partialbg;}
    void removePartialbg() { delete m_partialbg; m_partialbg = NULL;}
    void addPartialbg() ;

  protected:
    virtual void resizeEvent(QGraphicsSceneResizeEvent * ev);

  private:
    void setProperties();

    QPainterPath* m_partialbg;
    QPen m_pen;
    QLinearGradient m_grad;
    qreal m_opacity;
    QColor m_borderColor;
    int m_padding;
    int m_borderWidth;

  };

} // end of namespace GVA

#endif // __GINEBRA_TOOLBARCHROMEITEM_H
