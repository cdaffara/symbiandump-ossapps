/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include <QTimer>
#include "hsclockwidgettimer.h"

/*!
    \class HsClockWidgetTimer
    \ingroup group_hsclockwidgetplugin
    \brief Common timer for all clock widgets that run in the HS process.
*/

/*!
    Constructs new timer with the given \a parent.
*/
HsClockWidgetTimer::HsClockWidgetTimer(QObject *parent)
  : QObject(parent),
    mTimer(0)
{
    mTimer = new QTimer(this);
    mTimer->setInterval(1000);    
    connect(mTimer, SIGNAL(timeout()), SLOT(onTick()));
}

/*!
    Destructor.
*/
HsClockWidgetTimer::~HsClockWidgetTimer()
{
    mTimer->stop();
}

/*!
    Return the static timer instance.
*/
HsClockWidgetTimer *HsClockWidgetTimer::instance()
{
    if (!mInstance) {
        mInstance = new HsClockWidgetTimer;
    }
    return mInstance;
}

/*!
    \internal
*/
void HsClockWidgetTimer::connectNotify(const char *signal)
{
    mTimer->start();
    QObject::connectNotify(signal);
}

/*!
    \internal
*/
void HsClockWidgetTimer::disconnectNotify(const char *signal)
{
    if (receivers(SIGNAL(tick())) == 0) {
        mTimer->stop();
    }
    QObject::disconnectNotify(signal);
}

/*!
    \internal
*/
void HsClockWidgetTimer::onTick()
{
    if (0 < receivers(SIGNAL(tick()))) {        
        emit tick();
    } else {        
        mTimer->stop();
    }    
}

/*!
    Static timer.
*/
HsClockWidgetTimer *HsClockWidgetTimer::mInstance = 0;
