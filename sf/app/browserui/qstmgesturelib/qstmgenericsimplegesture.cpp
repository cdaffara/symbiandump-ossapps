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

#include "qstmgenericsimplegesture.h"
#include "qstmfilelogger.h"
#include "qstmutils.h"

using namespace qstmGesture ;
using qstmUiEventEngine::QStm_UiEventSpeedIf;


QStm_GenericSimpleGesture::QStm_GenericSimpleGesture(
    QStm_GestureUid uid, const QPoint& loc, int type, const QStm_UiEventSpeedIf* speedIf) :
    m_location(loc), m_uid(uid), m_type(type), m_details(0), m_speed(speedIf), m_target(0)
{
}

QPoint QStm_GenericSimpleGesture::getLocation()  const
{
    return m_location ;
}

int QStm_GenericSimpleGesture::getType()  const
{
    return m_type ;
}

void* QStm_GenericSimpleGesture::getDetails() const
{
    return m_details ;
}

void QStm_GenericSimpleGesture::setName(const QString& name)
{
    m_name = name ;
}

QString QStm_GenericSimpleGesture::getGestureName()
{
    return m_name ;
}

QStm_DirectionalGesture::QStm_DirectionalGesture(
    QStm_GestureUid uid, const QPoint& loc, const QPoint& prevLoc,
    const QStm_UiEventSpeedIf* speedIf, bool logging) :
    QStm_GenericSimpleGesture(uid, loc, 0, speedIf), m_loggingEnabled(logging)
{
    setVector(loc, prevLoc);
}

void QStm_DirectionalGesture::setVector(const QPoint& last, const QPoint& previous)
{
    m_vector = QPoint(last.x()-previous.x(), previous.y()-last.y()) ;  // x > 0 => east, y > 0 => north
    if (m_loggingEnabled) {
        LOGARG("CHoveringGesture: x: %d , %d prev: %d, %d", last.x(), last.y(), previous.x(), previous.y()) ;
    }
}

QStm_GestureDirection QStm_DirectionalGesture::getDirection() const
{
    QStm_GestureDirection dir = ENorth ;
    int x = ABS(m_vector.x()) ;
    int y = ABS(m_vector.x()) ;
    
    if (y <= x/2) {
        if (m_vector.x() < 0)
            dir = EWest ;
        else
            dir = EEast ;
    }
    else if (y > x/2 && y <= (x+x/2)) {
        if (m_vector.x() < 0)  {
            if (m_vector.y() < 0 )
                dir = ESouthWest ;
            else
                dir = ENorthWest ;
        }
        else {
            if (m_vector.y() < 0 )
                dir = ESouthEast ;
            else
                dir = ENorthEast ;
        }
    }
    else if (y > x+x/2) {
        if (m_vector.y() < 0)
            dir = ESouth ;
        else
            dir = ENorth ;
    }
    
    if (m_loggingEnabled) {
        LOGARG("CHoveringGesture: x: %d y: %d direction: %d", m_vector.x(), m_vector.y(), dir) ;
    }
    
    return dir ;
}

QStm_TwoPointGesture::QStm_TwoPointGesture(QStm_GestureUid uid, const QPoint& loc, const QPoint& prevLoc) :
    QStm_DirectionalGesture(uid, loc, prevLoc, NULL),
    m_location2(prevLoc)
{
}


