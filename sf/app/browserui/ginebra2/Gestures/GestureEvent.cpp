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

#include "GestureEvent.h"

namespace GVA {

GestureEvent::GestureEvent()
    : m_type(GestureEvent::None)
    , m_pos(QPoint(-1,-1))
    , m_delta(QPoint(-1,-1))
    , m_velocity(QPointF(0,0))
{}

GestureEvent::GestureEvent(const GestureEvent& gesture)
{
    m_type = gesture.m_type;
    m_pos = gesture.m_pos;
    m_delta = gesture.m_delta;
    m_velocity = gesture.m_velocity;
}

GestureEvent& GestureEvent::operator=(const GestureEvent& gesture)
{
    m_type = gesture.m_type;
    m_pos = gesture.m_pos;
    m_delta = gesture.m_delta;
    m_velocity = gesture.m_velocity;

    return *this;
}

GestureEvent::~GestureEvent()
{ }

GestureEvent::Type GestureEvent::type() const
{
    return m_type;
}

void GestureEvent::setType(GestureEvent::Type type)
{
    m_type = type;
}

QPointF GestureEvent::position() const
{
    return m_pos;
}

void GestureEvent::setPosition(const QPointF& pos)
{
    m_pos = pos;
}


QPoint GestureEvent::delta() const
{
    return m_delta;
}

void GestureEvent::setDelta(const QPoint& delta)
{
    m_delta = delta;
}

QPointF GestureEvent::velocity() const
{
    return m_velocity;
}

void GestureEvent::setVelocity(const QPointF& velocity)
{
    m_velocity = velocity;
}

} //namespace GVA
