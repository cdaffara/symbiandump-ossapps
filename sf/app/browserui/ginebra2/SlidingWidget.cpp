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

#include "SlidingWidget.h"
#include "ChromeSnippet.h"

#include <QDebug>

//NB: Note that the current implementation only supports one snippet child

namespace GVA {

  SlidingWidget::SlidingWidget(QGraphicsItem* parent)
    : QGraphicsWidget(parent),
      m_top(0),
      m_window(0),
      m_bottom(0),
      m_slideMax(0),
      m_slidePos(0),
      m_shrinked(0),
      m_shrinkMax(0),
      m_windowSize(QSizeF())
  {
    //Clip child item so that it can "slide" out of view
    setFlags(QGraphicsItem::ItemClipsChildrenToShape);
    //setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
    m_layout = new QGraphicsAnchorLayout();
    m_layout->setContentsMargins(0,0,0,0);
    m_layout->setSpacing(0);
    setLayout(m_layout);
  }

  SlidingWidget:: ~SlidingWidget()
  {
    // delete m_layout;
  }

  void SlidingWidget::resizeEvent(QGraphicsSceneResizeEvent *ev)
  {
    if (m_window)
      m_window->resize(ev->newSize());
      m_windowSize = ev->newSize();
      m_shrinked = 0;
      //m_window->resize(ev->newSize().width(), m_window->size().height());
    QGraphicsWidget::resizeEvent(ev);
  }

  /* shrink the viewport at the bottom, to show content on top of toolbar */
  qreal SlidingWidget::shrink(qreal delta)
  {

      qreal newDelta = m_shrinked + delta;
      if (newDelta < -m_shrinkMax) {
          newDelta = -m_shrinkMax;
      }
      if (newDelta > 0) {
          newDelta = 0;
      }
      delta = newDelta - m_shrinked;
      if (delta != 0) {
          m_window->resize(m_window->size().width(), m_window->size().height() + delta);
          m_shrinked = newDelta;
      }
      return delta;

  }

  qreal SlidingWidget::slide(qreal delta)
  {

    qreal newPos = m_slidePos + delta;

    //qDebug() << "SlidingWidget::slide: delta: " << delta << " m_slidePos: " << m_slidePos << " newPos: " << newPos;

    if (newPos < 0) {
      if (m_slidePos == 0)
    return 0;
      delta = -m_slidePos;
      m_slidePos = 0;
    }

    else if (newPos > m_slideMax){
      if (m_slidePos == m_slideMax)
    return 0;
      delta = m_slideMax - m_slidePos;
      m_slidePos = m_slideMax;
    }

    else m_slidePos = newPos;

    //qDebug() << "calculated delta: " << delta << " new m_slidePos: " << m_slidePos;

    QTransform transform;
    transform.translate(0,delta);
    //m_window->setTransform(transform, true);
    //m_top->setTransform(transform, true);
    m_top->setPos(0, m_top->pos().y() + delta);
    m_window->setPos(0, m_window->pos().y() + delta);
    return delta;
  }

  void SlidingWidget::setWindow(QGraphicsWidget * window)
  {
    if (m_window)
      m_window->setParentItem(0);
    m_window = window;
    m_window->setParentItem(this);
    /* set the window size to saved value, before the viewport is shrinked */
    m_window->resize(m_windowSize);
    m_shrinked = 0;

    if (m_top) {
        slide(0);
        QGraphicsWidget* item=m_top;
        attachItem(item);
    }
    else
      m_window->setPos(0,0);
    //m_layout->addAnchors(m_window, m_layout, Qt::Horizontal);
  }

  void SlidingWidget::attachItem(QGraphicsWidget * item)
  {
    if (m_top)
      detachItem(m_top);
    m_top = item;
    m_top->setParentItem(this);
    m_top->setPos(0,0);
    m_slideMax =  m_top->size().height();
    m_slidePos = m_slideMax;
    if (m_window)
      m_window->setPos(0, m_top->size().height());
  }


  void SlidingWidget::detachItem(QGraphicsWidget * item)
  {
    if (m_top != item)
      return;
    m_top->setParentItem(0);
    m_top = 0;
    if (m_window)
      m_window->setPos(0,0);
  }


} // end of namespace GVA


