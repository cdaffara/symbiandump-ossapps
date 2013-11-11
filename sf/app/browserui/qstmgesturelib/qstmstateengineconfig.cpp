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
#include "qstmstateengineconfig.h"
#include "qstmstateengine.h"
#include "qstmutils.h"

using namespace qstmUiEventEngine ;

QStm_StateEngineConfiguration::QStm_StateEngineConfiguration() :
	m_touchAreaShape(ERectangle),
	m_holdAreaShape(ERectangle)
{
	m_uiEventSender = new QStm_UiEventSender() ;
}

QStm_StateEngineConfiguration::~QStm_StateEngineConfiguration()
{
    delete m_uiEventSender ;
}

void QStm_StateEngineConfiguration::setTolerance(long fingersize_mm, QPoint& tolerance, QStm_AreaShape shape)
{
    long s = QStm_Utils::mm2Pixels(fingersize_mm) / 2;
    switch(shape)
    {
    case EEllipse:
        tolerance.setX((s * 2) / 3);
        tolerance.setY(s);
        break ;
    case ERectangle:
    case ECircle:
    default:
        tolerance.setX(s);
        tolerance.setY(s);
        break ;
    }
}

void QStm_StateEngineConfiguration::setTouchTimeArea(long fingersize_mm)
{
    setTolerance(fingersize_mm, m_touchTimeTolerance, m_touchAreaShape);

    // make sure that both touch areas are defined, so if the touch area is not yet set,
    // use the same as for touch time area.
    if (m_touchTolerance.x() == 0)
    {
        setTouchArea(fingersize_mm) ;
    }
}


void QStm_StateEngineConfiguration::setTouchArea(long fingersize_mm)
{
    setTolerance(fingersize_mm, m_touchTolerance, m_touchAreaShape);
}

void QStm_StateEngineConfiguration::setMoveTolerance(long fingersize_mm)
{
    setTolerance(fingersize_mm, m_moveTolerance, m_touchAreaShape);
}

QStm_AreaShape QStm_StateEngineConfiguration::getTouchAreaShape()
{
     return m_touchAreaShape ;
}

void QStm_StateEngineConfiguration::setTouchAreaShape(QStm_AreaShape aShape)
{
    this->m_touchAreaShape = aShape ;
}

unsigned int QStm_StateEngineConfiguration::getTouchTimeout()
{
    return m_touchTimerLimit ;
}

void QStm_StateEngineConfiguration::setTouchTimeout(unsigned int a)
{
    m_touchTimerLimit = a ;
}


void QStm_StateEngineConfiguration::setHoldArea(long fingersize_mm)
{
    setTolerance(fingersize_mm, m_holdTolerance, m_holdAreaShape);
}

QStm_AreaShape QStm_StateEngineConfiguration::getHoldAreaShape()
{
     return m_holdAreaShape ;
}


void QStm_StateEngineConfiguration::setHoldAreaShape(QStm_AreaShape aShape)
{
    m_holdAreaShape = aShape ;
}


unsigned int QStm_StateEngineConfiguration::getHoldTimeout()
{
    return m_holdTimerLimit ;
}

void QStm_StateEngineConfiguration::setHoldTimeout(unsigned int a)
{
    m_holdTimerLimit = a ;
}

unsigned int QStm_StateEngineConfiguration::getTouchSuppressTimeout()
{
    return m_suppressTimerLimit ;
}


void QStm_StateEngineConfiguration::setTouchSuppressTimeout(unsigned int a)
{
    m_suppressTimerLimit = a ;
}

unsigned int QStm_StateEngineConfiguration::getMoveSuppressTimeout()
{
    return m_moveSuppressTimerLimit ;
}

void QStm_StateEngineConfiguration::setMoveSuppressTimeout(unsigned int a)
{
    m_moveSuppressTimerLimit = a ;
}

void QStm_StateEngineConfiguration::enableLogging(bool a)
{
    m_enableLogging = a ;
    m_uiEventSender->setLogging(a) ;
}

bool QStm_StateEngineConfiguration::addUiEventObserver(QStm_UiEventObserverIf* observer)
{
    // The event sender handles the observers
    return m_uiEventSender->addObserver(observer) ;
}

bool QStm_StateEngineConfiguration::removeUiEventObserver(QStm_UiEventObserverIf* observer)
{
    // The event sender handles the observers
    return m_uiEventSender->removeObserver(observer) ;
}

