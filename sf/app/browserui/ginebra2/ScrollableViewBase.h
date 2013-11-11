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

#ifndef ScrollableViewBase_h
#define ScrollableViewBase_h

#include "Kinetics/KineticScrollable.h"

#include <QGraphicsWidget>

namespace GVA {

class KineticScroller;

class ScrollableViewBase : public QGraphicsWidget, public KineticScrollable {
public:
    ScrollableViewBase(QGraphicsItem* parent = 0, Qt::WindowFlags wFlags = 0);
    ~ScrollableViewBase();

    void setWidget(QGraphicsWidget*);

protected:

    //KineticScrollable pure virtuals
    QSize viewportSize() const;
    QPoint maximumScrollPosition() const ;
    QPoint scrollPosition() const;
    void setScrollPosition(const QPoint& pos, const QPoint& overshootDelta);
    void stateChanged(KineticScrollable::State oldState, KineticScrollable::State newState);

    QGraphicsWidget* scrollWidget() const { return m_scrollWidget; }
    virtual void setScrollWidgetGeometry(const QRectF& r);
    QRectF adjustScrollWidgetRect(const QRectF&);

    //Helpers for adjusting scroll pos
    void setScrollWidgetPos(const QPointF& pos);
    QPointF scrollWidgetPos() const;

protected:
    KineticScroller* m_kineticScroller;

private:
    QGraphicsWidget* m_scrollWidget;
    QPointF m_overShoot;
    QPointF m_extraPos;
}; //ScrollableViewBase

} //namespace GVA

#endif //ScrollableViewBase_h
