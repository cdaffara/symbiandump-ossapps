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
* Description:  Main test class for test library.
*
*/

#include <QStateMachine>
#include <QSignalSpy>
#include <QDir>
#include <hbinstance.h>

#include "t_hsapplication.h"
#include "hsstatemachine.h"
#include "hsapp_defs.h"
#include <qservicemanager.h>

QTM_USE_NAMESPACE

const QString xmlPath = "./hsresources/plugins/stateplugins/";
const QString mockedStatePluginsXml = "mockstateplugins.xml";



// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplication::testConstruction()
{
    HsStateMachine* rt = new HsStateMachine();
    QVERIFY(rt);
    QVERIFY(rt->mContentService);    
    delete rt;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplication::testStartStop()
{
    HsStateMachine* rt = new HsStateMachine();
    QObject::connect(rt, SIGNAL(event_exit()), rt, SIGNAL(finished()), Qt::QueuedConnection);
    QSignalSpy startSpy(rt, SIGNAL(started()));
    QSignalSpy stopSpy(rt,SIGNAL(stopped()));
    rt->start();
    qApp->processEvents();
    rt->stop();
    qApp->processEvents();
	
    QVERIFY(startSpy.count());
    QVERIFY(stopSpy.count());
    
    delete rt;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplication::testOnIdleStateEntered()
{
    HsStateMachine rt;
    rt.onIdleStateEntered();
    QVERIFY(rt.mIdleStateActive);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplication::testOnIdleStateExited()
{
    HsStateMachine rt;
    rt.onIdleStateExited();
    QVERIFY(!rt.mIdleStateActive);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplication::testRuntimeEventFilter()
{
    HsStateMachine rt;
    QVERIFY(!rt.eventFilter(0,&QEvent(QEvent::ActionAdded)));

    QEvent *e = new QEvent(QEvent::ApplicationActivate);
    QVERIFY(!rt.eventFilter(0,e));
    
    rt.onIdleStateEntered();
    
    delete e;    

    e = new QEvent(QEvent::ApplicationDeactivate);
    QVERIFY(!rt.eventFilter(0,e));
    delete e;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplication::testActivityRequested()
{
    HsStateMachine rt;
    rt.activityRequested(QString());
    rt.activityRequested(Hs::groupAppLibRecentView);
}
