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

#include "ScrollableViewBase.h"


#include "Kinetics/KineticScroller.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>

namespace GVA {

ScrollableViewBase::ScrollableViewBase(QGraphicsItem* parent, Qt::WindowFlags wFlags)
    : QGraphicsWidget(parent, wFlags)
    , m_scrollWidget(0)
{
    //To get only the mouse events within the ScrollableViewBase.
    setFlag(QGraphicsItem::ItemClipsToShape, true);

    //To disable childrens painting outside the ViewportItem's rect.
    setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);

    //To speed up painting.
    setFlag(QGraphicsItem::ItemHasNoContents, true);
    setAttribute(Qt::WA_OpaquePaintEvent, true);

    m_kineticScroller = new KineticScroller(this);

    //Event filter on all children of the viewport, so that all mouse
    //events can be intercepted and used for kinetic scrolling.
    setFiltersChildEvents(true);
}

ScrollableViewBase::~ScrollableViewBase()
{}

void ScrollableViewBase::setWidget(QGraphicsWidget* view)
{
    if (view == m_scrollWidget)
        return;

    if (m_scrollWidget) {
        m_scrollWidget->setParentItem(0);
        delete m_scrollWidget;
    }

    m_scrollWidget = view;
    m_scrollWidget->setAttribute(Qt::WA_OpaquePaintEvent, true);
    m_scrollWidget->setParentItem(this);
}

QSize ScrollableViewBase::viewportSize() const
{
    return size().toSize();
}

QPoint ScrollableViewBase::maximumScrollPosition() const
{
    QSizeF contentsSize = m_scrollWidget->size();
    QSizeF viewportSize = size();
    QSize maxScrollSize = (contentsSize - viewportSize).toSize();

    return QPoint(qMax(0, maxScrollSize.width()), qMax(0, maxScrollSize.height()));
}

QPoint ScrollableViewBase::scrollPosition() const
{
    return (-scrollWidgetPos()).toPoint();
}

void ScrollableViewBase::setScrollPosition(const QPoint& pos, const QPoint& overShoot)
{
    m_overShoot = overShoot;
    setScrollWidgetPos(-pos);
}

void ScrollableViewBase::stateChanged(KineticScrollable::State oldState
                                      , KineticScrollable::State newState)
{
    Q_UNUSED(oldState);
    Q_UNUSED(newState);
}

void ScrollableViewBase::setScrollWidgetGeometry(const QRectF& geom)
{
    scrollWidget()->setGeometry(adjustScrollWidgetRect(geom));
}

QRectF ScrollableViewBase::adjustScrollWidgetRect(const QRectF& rect)
{
    //FIX ME : Stop animation of scroll widget before adjusting it here??
    QRectF newRect(rect);

    QSizeF widgetSize = rect.size();
    QSizeF viewportSize = size();

    qreal w = viewportSize.width() - widgetSize.width();
    qreal h = viewportSize.height() - widgetSize.height();

    if ( w > 0 ) {
        newRect.moveLeft(0);
        m_extraPos.setX(w/2);
      }
    else {
    	m_extraPos.setX(0);
        if (newRect.x() < w)
            newRect.moveLeft(w);
        if (newRect.x() > 0)
            newRect.moveLeft(0);
    }

    if ( h > 0 ) {
        m_extraPos.setY(h/2);
        newRect.moveTop(0);
      }
    else {
    	 m_extraPos.setY(0);
        if (newRect.y() < h)
            newRect.moveTop(h);
        if (newRect.y() > 0)
            newRect.moveTop(0);
    }
    //newRect.translate(m_extraPos);
    newRect.translate(m_overShoot);
    return newRect;
}

void ScrollableViewBase::setScrollWidgetPos(const QPointF& pos)
{
    setScrollWidgetGeometry(QRectF(pos, scrollWidget()->size()));
}

QPointF ScrollableViewBase::scrollWidgetPos() const
{
    return scrollWidget()->pos()  /*- m_extraPos*/ - m_overShoot;
}

} //namespace GVA
