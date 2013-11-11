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
#ifndef __gva_slidingwidget_h__
#define __gva_slidingwidget_h__

#include <QtGui>

namespace GVA {

  class ChromeSnippet;

  class SlidingWidget : public QGraphicsWidget
  {
  public:
    SlidingWidget(QGraphicsItem* parent = 0);
    virtual ~SlidingWidget();
    void setWindow(QGraphicsWidget* window);
    qreal slide(qreal delta);
    qreal shrink(qreal delta);
    void setShrinkMax(qreal value) {m_shrinkMax = value;}
    void attachItem(QGraphicsWidget * item);
    void detachItem(QGraphicsWidget * item);
  protected:
    virtual void resizeEvent(QGraphicsSceneResizeEvent *ev);
  private:
    QGraphicsWidget *m_top;
    QGraphicsWidget *m_window;
    QGraphicsWidget *m_bottom;
    QGraphicsAnchorLayout *m_layout;
    qreal m_slidePos;
    qreal m_slideMax;
    qreal m_shrinked;
    qreal m_shrinkMax;
    QSizeF m_windowSize;
  };

} // end of namespace GVA

#endif

