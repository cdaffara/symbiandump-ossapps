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
#include "ut_logsreader.h"
#include "logsreader.h"
#include "logsevent.h"
#include <logcli.h>

#include <QtTest/QtTest>

void UT_LogsReader::initTestCase()
{
    QT_TRAP_THROWING( mLogClient = CLogClient::NewL(mFs); )
}

void UT_LogsReader::cleanupTestCase()
{
    delete mLogClient;
}


void UT_LogsReader::init()
{
    mErrorOccurred = false;
    mError = 0;
    mReader = new LogsReader(mFs, *mLogClient, mStrings, mEvents, *this );
}

void UT_LogsReader::cleanup()
{
    delete mReader;
    while (!mEvents.isEmpty()){
        delete mEvents.takeFirst();
    }
    
}

void UT_LogsReader::testConstructor()
{
    QVERIFY( mReader );
    QVERIFY( mReader->mReadStates.count() > 0 );
    QVERIFY( mReader->mModifyingStates.count() == 0 );
    QVERIFY( mReader->mCurrentStateMachine == &mReader->mReadStates );
}


void UT_LogsReader::testStart()
{
    QVERIFY( !mReader->mLogViewRecent );
    QVERIFY( !mReader->mDuplicatesView );
    QVERIFY( mReader->start() == 0 );
    QVERIFY( mReader->IsActive() );
    QVERIFY( mReader->mLogViewRecent );
    QVERIFY( mReader->mDuplicatesView );
    QVERIFY( mReader->mCurrentStateMachine == &mReader->mReadStates );
    QVERIFY( mReader->isRecentView() );
    
    // Starting second time should go ok
    QVERIFY( mReader->start() == 0 );
    QVERIFY( mReader->IsActive() );
    QVERIFY( mReader->mLogViewRecent );
    
    // Starting when deleting is in progress should fail
    mReader->mCurrentStateMachine = &mReader->mModifyingStates;
    QVERIFY( mReader->start() == KErrInUse ); 
}

void UT_LogsReader::testStart2()
{
    qDeleteAll( mReader->mReadStates );
    mReader->mReadStates.clear();
    mReader->mReadAllEvents = true;
    
    QVERIFY( !mReader->mLogViewEvent );
    QVERIFY( !mReader->mDuplicatesView );
    QVERIFY( mReader->start() == 0 );
    QVERIFY( mReader->IsActive() );
    QVERIFY( mReader->mLogViewEvent );
    QVERIFY( mReader->mDuplicatesView );
    QVERIFY( mReader->mCurrentStateMachine == &mReader->mReadStates );
    QVERIFY( !mReader->isRecentView() );
    
    // Starting second time should go ok
    QVERIFY( mReader->start() == 0 );
    QVERIFY( mReader->IsActive() );
    QVERIFY( mReader->mLogViewEvent );
}

void UT_LogsReader::testStop()
{
    mReader->stop();
    QVERIFY( mReader->start() == 0 );
    QVERIFY( mReader->IsActive() );
    ContactCacheEntry contactEntry("name", 1);
    mReader->mContactCache.insert("12345", contactEntry);
    mReader->stop();
    QVERIFY( !mReader->IsActive() );
    QVERIFY( !mReader->mLogViewEvent );
    QVERIFY( !mReader->mDuplicatesView );
    QCOMPARE( mReader->mReadStates.count(), 0 );
    QCOMPARE( mReader->mModifyingStates.count(), 0 );
    QCOMPARE( mReader->mDuplicateReadingStates.count(), 0 );
    QCOMPARE( mReader->mContactCache.count(), 0 );
}

void UT_LogsReader::testMarkEventSeen()
{
    QVERIFY( !mReader->mLogViewRecent );
    QVERIFY( !mReader->mDuplicatesView );
    QVERIFY( mReader->markEventSeen(1) == 0 );
    QVERIFY( mReader->IsActive() );
    QVERIFY( mReader->mLogViewRecent );
    QVERIFY( mReader->mDuplicatesView );
    QVERIFY( mReader->mCurrentStateMachine == &mReader->mModifyingStates );
    QVERIFY( mReader->mModifyingStates.count() > 0 );
    
    //starting second time should fail
    QVERIFY( mReader->markEventSeen(1) == KErrInUse );
    
    //starting when reading is in progress should be ok
    mReader->mCurrentStateMachine = &mReader->mReadStates;
    QVERIFY( mReader->markEventSeen(1) == 0 );
}

void UT_LogsReader::testReadDuplicates()
{
    QVERIFY( !mReader->mLogViewRecent );
    QVERIFY( !mReader->mDuplicatesView );
    QVERIFY( mReader->readDuplicates(1) == 0 );
    QVERIFY( mReader->IsActive() );
    QVERIFY( mReader->mLogViewRecent );
    QVERIFY( mReader->mDuplicatesView );
    QVERIFY( mReader->mCurrentStateMachine == &mReader->mDuplicateReadingStates );
    QVERIFY( mReader->mDuplicateReadingStates.count() > 0 );
    
    //starting second time is ok
    QVERIFY( mReader->readDuplicates(1) == 0 );
    
    //starting when reading is in progress is not allowed
    mReader->mCurrentStateMachine = &mReader->mReadStates;
    QVERIFY( mReader->readDuplicates(1) != 0 );
}

void UT_LogsReader::testRunL()
{
    mReader->start();
    
    // Test ok scenario
    mReader->Cancel();
    mReader->iStatus = KErrNone;
    mReader->RunL();
    QVERIFY( !mErrorOccurred );
    QVERIFY( mError == 0 );
    QVERIFY( mReader->IsActive() );

    // Test failure
    mReader->Cancel();
    mReader->iStatus = KErrNotFound;
    TRAPD( err, mReader->RunL() );
    QVERIFY( err == KErrNotFound );
    
}

void UT_LogsReader::testRunError()
{
    // No fatal callback for access denied or in use errors
    TInt err = KErrAccessDenied;
    QVERIFY( mReader->RunError(err) == KErrNone );
    QVERIFY( !mErrorOccurred );
    QVERIFY( mError == KErrAccessDenied );
    
    err = KErrInUse;
    QVERIFY( mReader->RunError(err) == KErrNone );
    QVERIFY( !mErrorOccurred );
    QVERIFY( mError == KErrInUse );
    
    err = KErrNotFound;
    QVERIFY( mReader->RunError(err) == KErrNone );
    QVERIFY( mErrorOccurred );
    QVERIFY( mError == KErrNotFound );
}

void UT_LogsReader::testStateContext()
{
    mReader->mCurrentStateIndex = 0;
    QVERIFY( mReader->mCurrentStateMachine == &mReader->mReadStates );
    mReader->setCurrentState(*mReader->mReadStates[1]);
    QVERIFY( mReader->mCurrentStateIndex == 1 );
    
    //wrong state machine is set as current, state isn't changed
    LogsReaderStateBase* unknownState = new LogsReaderStateBase( *mReader, *mReader );
    mReader->mModifyingStates.append(unknownState);
    QVERIFY( mReader->mCurrentStateMachine == &mReader->mReadStates );
    mReader->setCurrentState(*mReader->mModifyingStates[0]);
    QVERIFY( mReader->mCurrentStateIndex == 1 );

    //once again with the correct state machine, state changed
    mReader->mCurrentStateMachine = &mReader->mModifyingStates;
    mReader->setCurrentState(*mReader->mModifyingStates[0]);
    QVERIFY( mReader->mCurrentStateIndex == 0 );
    
    mReader->mIndex = 1;
    int& index = mReader->index();
    QVERIFY( index == 1 );
    index++;
    QVERIFY( mReader->index() = index );
    
    QVERIFY( &mReader->events() == &mEvents );
    QVERIFY( &mReader->strings() == &mStrings );
    QVERIFY( &mReader->reqStatus() == &mReader->iStatus );

}

void UT_LogsReader::testViewChange()
{
    mReader->Cancel();
    mReader->HandleLogClientChangeEventL(KLogClientChangeEventLogCleared, 0, 0, 0);
    QVERIFY( mReader->IsActive() );
    
    mReader->Cancel();
    mReader->HandleLogClientChangeEventL(KNullUid, 0, 0, 0);
    QVERIFY( !mReader->IsActive() );
    
    mReader->Cancel();
    mReader->HandleLogViewChangeEventAddedL(0, 0, 0, 2);
    QVERIFY( mReader->IsActive() );
    
    mReader->Cancel();
    mReader->HandleLogViewChangeEventChangedL(0, 0, 0, 1);
    QVERIFY( mReader->IsActive() );
    
    // Deletion has optimization to reduce multiple actions for same deletion
    // patch
    mReader->Cancel();
    mReader->HandleLogViewChangeEventDeletedL(0, 0, 1, 2);
    QVERIFY( mReader->IsActive() );
    
    mReader->Cancel();
    mReader->HandleLogViewChangeEventDeletedL(0, 0, 0, 2);
    QVERIFY( !mReader->IsActive() );
    
    mReader->Cancel();
    mReader->HandleLogViewChangeEventDeletedL(0, 0, 0, 1);
    QVERIFY( mReader->IsActive() );
    
}

void UT_LogsReader::testUpdateDetails()
{
    // Full clearing is done always
    QVERIFY( !mReader->IsActive() );
    LogsEvent* ev = new LogsEvent;
    ev->setContactMatched(true);
    mReader->mEvents.append( ev);
    ContactCacheEntry contactEntry("name", 1);
    mReader->mContactCache.insert("12345", contactEntry);
    mReader->updateDetails(false);
    QVERIFY( mReader->IsActive() );
    QVERIFY( mReader->mContactCache.count() == 0 );
    
    mReader->mContactCache.insert("12345", contactEntry);
    ContactCacheEntry contactEntry2("name2", 1);
    mReader->mContactCache.insert("123456666", contactEntry2);
    mReader->updateDetails(true);
    QVERIFY( mReader->IsActive() );
    QVERIFY( mReader->mContactCache.count() == 0 );    
}

void UT_LogsReader::testLock()
{
    QVERIFY( !mReader->mLocked );
    
    // No pending read when lock is released
    mReader->lock(true);
    QVERIFY( !mReader->IsActive() );
    QVERIFY( mReader->mLocked );
    mReader->lock(false);
    QVERIFY( !mReader->mLocked );
    QVERIFY( !mReader->IsActive() );
    
    // Pending read exists when lock is released -> read starts
    mReader->lock(true);
    QVERIFY( !mReader->IsActive() );
    QVERIFY( mReader->mLocked );
    QVERIFY( mReader->start() == KErrAccessDenied );
    QVERIFY( !mReader->IsActive() );
    mReader->lock(false);
    QVERIFY( !mReader->mLocked );
    QVERIFY( mReader->IsActive() );
}


// ----------------------------------------------------------------------------
// From LogsReaderObserver
// ----------------------------------------------------------------------------
//
void UT_LogsReader::readCompleted()
{

}

// ----------------------------------------------------------------------------
// From LogsReaderObserver
// ----------------------------------------------------------------------------
//
void UT_LogsReader::errorOccurred(int err)
{
    mErrorOccurred = true;
    mError = err;
}

// ----------------------------------------------------------------------------
// From LogsReaderObserver
// ----------------------------------------------------------------------------
//
void UT_LogsReader::temporaryErrorOccurred(int err)
{
    mError = err;
}

// ----------------------------------------------------------------------------
// From LogsReaderObserver
// ----------------------------------------------------------------------------
//
void UT_LogsReader::eventModifyingCompleted()
{

}

// ----------------------------------------------------------------------------
// From LogsReaderObserver
// ----------------------------------------------------------------------------
//
void UT_LogsReader::duplicatesReadingCompleted(QList<LogsEvent*> duplicates)
{
    qDeleteAll(duplicates);
}
