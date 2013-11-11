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
#include "ut_logsremove.h"
#include "logsremove.h"
#include "logsremoveobserver.h"
#include "logclient_stubs_helper.h"
#include "logsevent.h"
#include <logcli.h>

#include <QtTest/QtTest>

#define ADD_EVENT_WITH_ID( arr, id ) \
{\
LogsEvent* ev = new LogsEvent;\
ev->setLogId(id);\
arr.append(ev);\
}

#define ADD_EVENT_WITH_ID_2( arr, id ) \
{\
LogsEvent ev;\
ev.setLogId(id);\
arr.append(ev);\
}

void UT_LogsRemove::initTestCase()
{
}

void UT_LogsRemove::cleanupTestCase()
{
}

// ----------------------------------------------------------------------------
// UT_LogsRemove::removeCompleted
// ----------------------------------------------------------------------------
//
void UT_LogsRemove::removeCompleted()
{   
    mRemoveCompleted = true;
}

// ----------------------------------------------------------------------------
// UT_LogsRemove::logsRemoveErrorOccured
// ----------------------------------------------------------------------------
//
void UT_LogsRemove::logsRemoveErrorOccured(int err)
{
    Q_UNUSED(err)
}

void UT_LogsRemove::init()
{
    mRemoveCompleted = false;
    mErrorOccurred = false;
    mError = 0;
    mLogsRemove = new LogsRemove(*this, false);
}

void UT_LogsRemove::cleanup()
{
    delete mLogsRemove;
}

void UT_LogsRemove::testConstructor()
{
    QVERIFY( mLogsRemove );
    QVERIFY( !mLogsRemove->mFsSession );
    QVERIFY( !mLogsRemove->mLogClient );
}

void UT_LogsRemove::testclearList()
{
    //clearing doesn't start, since initialization failed
    LogClientStubsHelper::setLeaveWithError(KErrNoMemory);
    QVERIFY( !mLogsRemove->IsActive() );
    QVERIFY( !mLogsRemove->clearList(LogsModel::TypeLogsClearAll) );
    QVERIFY( !mLogsRemove->IsActive() );

    //clearing is ok
    LogClientStubsHelper::setLeaveWithError(KErrNone);
    QVERIFY( mLogsRemove->clearList(LogsModel::TypeLogsClearMissed) );
    QVERIFY( mLogsRemove->IsActive() );
    
    //clearing doesn't start, since already active
    QVERIFY( !mLogsRemove->clearList(LogsModel::TypeLogsClearAll) );
}

void UT_LogsRemove::testClearEvents()
{
    // Clearing with recent view (no ids)
    QList<LogsEvent*> events;
    bool async( false );
    QVERIFY( mLogsRemove->clearEvents(events, async) == 0 );
    QVERIFY( !async );
    QVERIFY( mLogsRemove->mRemovedEvents.count() == 0 );
    QVERIFY( !mLogsRemove->IsActive() );
    
    // Clearing with recent view (ids)
    ADD_EVENT_WITH_ID(events, 2);
    ADD_EVENT_WITH_ID(events, 100);
    QVERIFY( mLogsRemove->clearEvents(events, async) == 0 );
    QVERIFY( async );
    QVERIFY( mLogsRemove->mRemovedEvents.count() == 2 );
    QVERIFY( mLogsRemove->IsActive() );
    
    // Clearing with all events (no ids)
    qDeleteAll(events);
    events.clear();
    LogsRemove removeWithAllEvents(*this, true);
    QVERIFY( removeWithAllEvents.clearEvents(events, async) == 0 );
    QVERIFY( !async );
    QVERIFY( removeWithAllEvents.mRemovedEvents.count() == 0 );
    QVERIFY( !removeWithAllEvents.IsActive() );
    
    // Clearing with all events (ids)
    ADD_EVENT_WITH_ID(events, 99);
    ADD_EVENT_WITH_ID(events, 100);
    QVERIFY( removeWithAllEvents.clearEvents(events, async) == 0 );
    QVERIFY( async );
    QVERIFY( removeWithAllEvents.mRemovedEvents.count() == 2 );
    QVERIFY( removeWithAllEvents.mRemovedEvents.at(0).logId() == 99 );  
    QVERIFY( removeWithAllEvents.mRemovedEvents.at(1).logId() == 100 );   
    QVERIFY( removeWithAllEvents.IsActive() );
    
    // Clearing not allowed while previous is active
    ADD_EVENT_WITH_ID(events, 200);
    QVERIFY( removeWithAllEvents.clearEvents(events, async) != 0 );
    QVERIFY( mLogsRemove->mRemovedEvents.count() == 2 );
    QVERIFY( !async );
    QVERIFY( removeWithAllEvents.IsActive() );
    qDeleteAll(events);
}

void UT_LogsRemove::testClearEventsWithMergedDuplicates()
{
    QList<LogsEvent*> events;
    bool async( false );

    ADD_EVENT_WITH_ID(events, 2);
    ADD_EVENT_WITH_ID(events, 3);
    ADD_EVENT_WITH_ID_2(events.at(0)->mergedDuplicates(), 5);
    ADD_EVENT_WITH_ID_2(events.at(0)->mergedDuplicates(), 6);
    ADD_EVENT_WITH_ID_2(events.at(1)->mergedDuplicates(), 7);
    
    QVERIFY( mLogsRemove->clearEvents(events, async) == 0 );
    QVERIFY( async );
    QCOMPARE( mLogsRemove->mRemovedEvents.count(), 2 );
    QVERIFY( mLogsRemove->mRemovedEventDuplicates.count() == 3 );
    QVERIFY( mLogsRemove->IsActive() );
    QCOMPARE( mLogsRemove->mCurrentEventId, 2 );
    
    // Simulate completion, removing should continue with first item from duplicates list
    mLogsRemove->removeCompleted();
    QVERIFY( !mRemoveCompleted );
    QCOMPARE( mLogsRemove->mCurrentEventId, 5 );
    QVERIFY( mLogsRemove->mRemovedEventDuplicates.count() == 2 );
    mLogsRemove->removeCompleted();
    QVERIFY( !mRemoveCompleted );
    QCOMPARE( mLogsRemove->mCurrentEventId, 6 );
    QVERIFY( mLogsRemove->mRemovedEventDuplicates.count() == 1 );
    mLogsRemove->removeCompleted();
    QVERIFY( !mRemoveCompleted );
    QCOMPARE( mLogsRemove->mCurrentEventId, 7 );
    QVERIFY( mLogsRemove->mRemovedEventDuplicates.count() == 0 );
    mLogsRemove->removeCompleted();
    QVERIFY( mRemoveCompleted );
    
    qDeleteAll(events);
}

void UT_LogsRemove::testRunError()
{
    // No fatal callback for access denied error
    TInt err = KErrAccessDenied;
    QVERIFY( mLogsRemove->RunError(err) == KErrNone );
}

void UT_LogsRemove::testInit()
{
    //initialization failes
    LogClientStubsHelper::setLeaveWithError(KErrNoMemory);
    QVERIFY( mLogsRemove->init() == KErrNoMemory );
    QVERIFY( !mLogsRemove->mLogClient );
    LogClientStubsHelper::setLeaveWithError(KErrNone);
            
    //initializing for the first time
    QVERIFY( mLogsRemove->init() == KErrNone );
    QVERIFY( mLogsRemove->mLogClient );
    QVERIFY( mLogsRemove->mFsSession );
    CLogClient* oldClient = mLogsRemove->mLogClient;
    
    //2d time should be also ok
    QVERIFY( mLogsRemove->init() == KErrNone );
    QVERIFY( mLogsRemove->mLogClient );
    QVERIFY( mLogsRemove->mFsSession );
    QVERIFY( oldClient == mLogsRemove->mLogClient );    
}

