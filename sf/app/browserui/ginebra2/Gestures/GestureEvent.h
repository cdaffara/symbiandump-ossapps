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

/*
 W A R N I N G
 -------------
 THIS IS A TEMPORARY GESTURE CODE. WOULD BE REPLACED WHEN BROWSER HAS ITS OWN GESTURE FRAMEWORK
 */

#ifndef GestureEvent_h
#define GestureEvent_h

#include <QObject>
#include <QPoint>
#include <QPointF>

namespace GVA {

class GestureEvent : public QObject {
    Q_OBJECT
    Q_PROPERTY(GestureEvent::Type type READ type WRITE setType)
    Q_PROPERTY(QPointF position READ position WRITE setPosition)
    Q_PROPERTY(QPoint delta READ delta WRITE setDelta)
    Q_PROPERTY(QPointF velocity READ velocity WRITE setVelocity)

public:
    enum Type {
        //FIX ME : We can use Tap for Touch & Release
        //But at present ginebra chrome elements such as
        //Bookmark/History needs separate touch and release
        //for drag and drop handling.
        None,
        Touch,
        Release,
        Pan,
        Flick,
        DoubleTap,
        LongTap
    };

    GestureEvent();
    GestureEvent(const GestureEvent&);
    GestureEvent& operator=(const GestureEvent&);
    ~GestureEvent();

    GestureEvent::Type type() const;
    void setType(GestureEvent::Type type);

    QPointF position() const;
    void setPosition(const QPointF& p);

    QPoint delta() const;
    void setDelta(const QPoint& p);

    QPointF velocity() const;
    void setVelocity(const QPointF& p);

private:
    GestureEvent::Type m_type;
    QPointF m_pos;
    QPoint m_delta;
    QPointF m_velocity;
};

} //namespace GVA
#endif //GestureEvent_h
