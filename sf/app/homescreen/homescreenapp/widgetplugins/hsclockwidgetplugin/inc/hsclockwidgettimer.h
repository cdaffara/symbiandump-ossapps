/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef HSCLOCKWIDGETTIMER_H
#define HSCLOCKWIDGETTIMER_H

#include <QObject>

#include <hstest_global.h>

HOMESCREEN_TEST_CLASS(TestClockWidget)

class QTimer;

class HsClockWidgetTimer : public QObject
{
    Q_OBJECT
    
public:    
    ~HsClockWidgetTimer();

    static HsClockWidgetTimer *instance();

signals:
    void tick();

protected:    
    void connectNotify(const char *signal);    
    void disconnectNotify(const char *signal);    

private:
    HsClockWidgetTimer(QObject *parent = 0);
    Q_DISABLE_COPY(HsClockWidgetTimer)

private slots:
    void onTick();

private:
    QTimer *mTimer;
    static HsClockWidgetTimer *mInstance;    

    HOMESCREEN_TEST_FRIEND_CLASS(TestClockWidget)
};

#endif // HSCLOCKWIDGETTIMER_H
