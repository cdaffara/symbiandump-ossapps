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
* Description:  Main test class for hsutils library.
*
*/


#include "t_hsapplication.h"
#include "hshomescreen.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplication::testCreateHomescreen()
{
    HsHomeScreen hs;
    QVERIFY(hs.mRuntime);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplication::testHomescreenStart()
{

    {
        HsHomeScreen hs;
        QSignalSpy spy(&hs,SIGNAL(exit()));
        hs.start();
        qApp->processEvents();
        QVERIFY(spy.count() == 0);
    }
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplication::testHomescreenStop()
{
    {
        //Improves coverage test: return at the beginning of the stop()
        HsHomeScreen hs;
        hs.stop();
    }

    {
        HsHomeScreen hs;
        hs.start();
        qApp->processEvents();
        hs.stop();
    }
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplication::testOnRuntimeStarted()
{
    HsHomeScreen hs;
    hs.onRuntimeStarted();
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplication::testOnRuntimeStopped()
{
    HsHomeScreen hs;
    QSignalSpy spy(&hs,SIGNAL(exit()));
    hs.onRuntimeStopped();
    QVERIFY(spy.count() == 1);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplication::testEventFilter()
{
    HsHomeScreen hs;
    QVERIFY(!hs.eventFilter(0,&QEvent(QEvent::ActionAdded)));

    QEvent *e = new QEvent(QEvent::Close);
    QVERIFY(hs.eventFilter(0,e));
    delete e;
}
