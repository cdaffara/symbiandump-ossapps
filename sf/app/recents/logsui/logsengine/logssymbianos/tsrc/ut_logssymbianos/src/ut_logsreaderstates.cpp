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
#include "ut_logsreaderstates.h"
#include "logsreaderstates.h"
#include "logsevent.h"
#include "logscommondata.h"
#include "logclient_stubs_helper.h"
#include "qtcontacts_stubs_helper.h"
#include <logcli.h>
#include <logview.h>

#include <QtTest/QtTest>

_LIT( KTestInDirection, "Incoming" );
_LIT( KTestOutDirection, "Outgoing" );
_LIT( KTestMissedDirection, "Missed call" );
_LIT( KTestUnknown, "Unknown" );

void UT_LogsReaderStates::initTestCase()
{
    int rfsDummy = 0;
    mLogClient = CLogClient::NewL( *((RFs*)&rfsDummy) );
    mStrings.iInDirection = DESC_TO_QSTRING( KTestInDirection() );
    mStrings.iOutDirection = DESC_TO_QSTRING( KTestOutDirection() );
    mStrings.iMissedDirection = DESC_TO_QSTRING( KTestMissedDirection() ); 
    mStrings.iUnKnownRemote = DESC_TO_QSTRING( KTestUnknown() ); 
}

void UT_LogsReaderStates::cleanupTestCase()
{
    delete mLogClient;
}


void UT_LogsReaderStates::init()
{
    reset();
    
    mCurrentState = 0;
    mIndex = 0;
    mLogView = 0;
    mLogViewEvent = 0;
    mIsRecentView = true;

    mLogView = CLogViewRecent::NewL(*mLogClient);
    mLogViewEvent = CLogViewEvent::NewL(*mLogClient);
    mDuplicatesView = CLogViewDuplicate::NewL(*mLogClient);
    
    const_cast<CLogEvent&>( mLogView->Event() ).SetDirection( KTestInDirection );
}

void UT_LogsReaderStates::cleanup()
{
    delete mLogView;
    mLogView = 0;
    delete mLogViewEvent;
    mLogViewEvent = 0;
    delete mDuplicatesView;
    mDuplicatesView = 0;
    while (!mEvents.isEmpty()){
        delete mEvents.takeFirst();
    }
    mContactCache.clear();
    qDeleteAll(mDuplicatedEvents);
    mDuplicatedEvents.clear();
    
}

void UT_LogsReaderStates::reset()
{
    mError = 0;
    mTemporaryError = 0;
    mReadCompleted = false;
    mModifyCompleted = false;
    mReadCount = 0;
    mDuplicatesReadingCompletedCount = 0;
}

void UT_LogsReaderStates::testStateBase()
{
    LogsReaderStateBase state(*this, *this);
    LogsReaderStateBase state2(*this, *this);
    QVERIFY( state.mNextState == 0 );
    QVERIFY( state2.mNextState == 0 );
    
    state.setNextState(state2);
    QVERIFY( state.mNextState == &state2 );
    QVERIFY( state2.mNextState == 0 );
    
    QVERIFY( !state.enterL() );
    QVERIFY( !state2.enterL() );
    
    QVERIFY( !state.continueL() );
    QVERIFY( !state2.continueL() );
    
    QVERIFY( !state2.enterNextStateL() );
    QVERIFY( mCurrentState == 0 );
    
    QVERIFY( !state.enterNextStateL() );
    QVERIFY( mCurrentState == &state2 );
    
    CLogEvent* logEvent = CLogEvent::NewL();
    CleanupStack::PushL(logEvent);
    logEvent->SetNumber( _L("1234") );
    logEvent->SetId( 100 );
    LogsEvent* logsEvent = new LogsEvent;
    int index( 0 );
    QVERIFY( state.constructAndInsertEventL(*logEvent, logsEvent, index, mEvents ) );
    logsEvent = 0;
    QVERIFY( mEvents.count() == 1 );
    QVERIFY( mEvents.at(0)->number() == "1234" );
    QVERIFY( mEvents.at(0)->isInView() );
    QVERIFY( index == 1 );
    
    logEvent->SetNumber( _L("2234") );
    logEvent->SetId( 101 );
    logsEvent = new LogsEvent;
    QVERIFY( state.constructAndInsertEventL(*logEvent, logsEvent, index, mEvents ) );
    logsEvent = 0;
    QVERIFY( mEvents.count() == 2 );
    QVERIFY( mEvents.at(1)->number() == "2234" );
    QVERIFY( mEvents.at(1)->index() == -1 ); // Not assigned at state machine
    QVERIFY( mEvents.at(1)->isInView() );
    QVERIFY( index == 2 );
    
    // Invalid event discarded
    logEvent->SetRemoteParty( _L("") );
    logEvent->SetNumber( _L("") );
    logEvent->SetId( 102 );
    logsEvent = new LogsEvent;
    QVERIFY( !state.constructAndInsertEventL(*logEvent, logsEvent, index, mEvents ) );
    QVERIFY( mEvents.count() == 2 );
    QVERIFY( index == 2 );
    
    logEvent->SetNumber( _L("2234") );
    logEvent->SetId( 101 );
    logsEvent = new LogsEvent;
    index = 0;
    QVERIFY( state.updateAndInsertEventL( *logEvent, logsEvent, index ) );
    QVERIFY( mEvents.count() == 3 );
    QVERIFY( index == 1 );
    
    logEvent->SetRemoteParty( _L("Private") );
    logEvent->SetNumber( _L("") );
    logEvent->SetId( 106 );
    logsEvent = new LogsEvent;
    index = 0;
    QVERIFY( state.updateAndInsertEventL( *logEvent, logsEvent, index ) );
    QVERIFY( mEvents.count() == 4 );
    QVERIFY( index == 1 );
    QVERIFY( mEvents.at(0)->isRemotePartyPrivate() );
    
    logEvent->SetRemoteParty( _L("Private") );
    logEvent->SetNumber( _L("123") );
    logEvent->SetId( 107 );
    logsEvent = new LogsEvent;
    index = 0;
    QVERIFY( state.updateAndInsertEventL( *logEvent, logsEvent, index ) );
    QVERIFY( mEvents.count() == 5 );
    QVERIFY( index == 1 );
    QVERIFY( mEvents.at(0)->isRemotePartyPrivate() );
    
    
    logEvent->SetRemoteParty( _L("Unknown") );
    logEvent->SetNumber( _L("123") );
    logEvent->SetId( 108 );
    logsEvent = new LogsEvent;
    index = 0;
    QVERIFY( state.updateAndInsertEventL( *logEvent, logsEvent, index ) );
    QVERIFY( mEvents.count() == 6 );
    QVERIFY( index == 1 );
    QVERIFY( !mEvents.at(0)->isRemotePartyUnknown() );
    
    
    logEvent->SetRemoteParty( _L("Unknown") );
    logEvent->SetNumber( _L("") );
    logEvent->SetId( 109 );
    logsEvent = new LogsEvent;
    index = 0;
    QVERIFY( state.updateAndInsertEventL( *logEvent, logsEvent, index ) );
    QVERIFY( mEvents.count() == 7 );
    QVERIFY( index == 1 );
    QVERIFY( mEvents.at(0)->isRemotePartyUnknown() );
    
    
    state.resetEvents();
    QVERIFY( !mEvents.at(0)->isInView() );
    QVERIFY( !mEvents.at(1)->isInView() );
    
    LogsEvent* event = state.takeMatchingEvent(*logEvent);
    QVERIFY( event );
    QVERIFY( mEvents.count() == 6 );
    delete event;
    logEvent->SetId( 200 );
    LogsEvent* event2 = state.takeMatchingEvent(*logEvent);
    QVERIFY( !event2 );
    QVERIFY( mEvents.count() == 6 );

    CleanupStack::PopAndDestroy( logEvent );    
        
    //event with Id not found
    QVERIFY( !mEvents.isEmpty() );
    QVERIFY( !state.eventById(0) );
    
    //event list is empty
    while ( !mEvents.isEmpty() ) {
        delete mEvents.takeFirst();
    }
    QVERIFY( !state.eventById(0) );
    
    //event with Id found
    LogsEvent* event3 = new LogsEvent;
    event3->setLogId( 300 );
    mEvents.append(event3);
    QVERIFY( state.eventById(300) == event3 );
    QVERIFY( !mEvents.isEmpty() );
    
}

void UT_LogsReaderStates::testStateInitReading()
{
    LogsReaderStateInitReading init(*this, *this);
    CLogEvent* logEvent = CLogEvent::NewL();
    CleanupStack::PushL(logEvent);
    logEvent->SetNumber( _L("1234") );
    logEvent->SetId( 100 );
    LogsEvent* logsEvent = new LogsEvent;
    logsEvent->setIsInView(true);
    int index( 0 );
    QVERIFY( init.constructAndInsertEventL(*logEvent, logsEvent, index, mEvents ) );
    init.enterL();
    QVERIFY( !logsEvent->isInView() ); 
    CleanupStack::PopAndDestroy( logEvent ); 
}

void UT_LogsReaderStates::testStateFiltering()
{
    LogsReaderStateFiltering state(*this, *this);
    QVERIFY( !state.mFilterList );
    
    // State can be used only with recent view
    mIsRecentView = false;
    TRAPD( err, state.enterL() );
    QVERIFY( err == KErrNotFound );
    mIsRecentView = true;
    
    // Recent list fetching starts ok
    QVERIFY( state.enterL() );
    QVERIFY( state.mFilterList );
    QVERIFY( state.mFilterList->Count() == 1 );
    
    // Recent list fetching does not start
    LogClientStubsHelper::setStubAsyncCallPossible(false);
    QVERIFY( !state.enterL() );
    LogClientStubsHelper::setStubAsyncCallPossible(true);
    
    // Doesn't continue as no next state
    QVERIFY( !state.continueL() );
    
    // Continues as there's next state
    LogsReaderStateFiltering state2(*this, *this);
    state.setNextState(state2);
    QVERIFY( state.continueL() );
}

void UT_LogsReaderStates::testStateFilteringAll()
{
    LogsReaderStateFilteringAll state(*this, *this);
    QVERIFY( !state.mFilterList );
    
    // State can be used only with full event view
    mIsRecentView = true;
    TRAPD( err, state.enterL() );
    QVERIFY( err == KErrNotFound );
    mIsRecentView = false;
    
    // Fetching starts ok
    QVERIFY( state.enterL() );
    QVERIFY( state.mFilterList );
    QVERIFY( state.mFilterList->Count() == 1 );
    
    // Recent list fetching does not start
    LogClientStubsHelper::setStubAsyncCallPossible(false);
    QVERIFY( !state.enterL() );
    LogClientStubsHelper::setStubAsyncCallPossible(true);
}

void UT_LogsReaderStates::testStateReading()
{
    LogsReaderStateReading state(*this, *this);
    
    // Reading starts ok
    QVERIFY( state.enterL() );
    
    // Reading doesn't start ok as no items in view
    LogClientStubsHelper::setViewCount(0);
    QVERIFY( !state.enterL() );
    
    // Reading continues as more events in view exist, new event is added
    QVERIFY( mEvents.count() == 0 );
    const_cast<CLogEvent&>( mLogView->Event() ).SetNumber( _L("12345") );
    const_cast<CLogEvent&>( mLogView->Event() ).SetId( 100 );
    LogClientStubsHelper::setViewCount(2);
    QVERIFY( state.continueL() );
    QVERIFY( mEvents.count() == 1 );
    QVERIFY( mIndex == 1 );
    
    // Reading does not continue as no more events in view
    const_cast<CLogEvent&>( mLogView->Event() ).SetId( 101 );
    QVERIFY( !state.continueL() );
    QVERIFY( mEvents.count() == 2 );
    QVERIFY( mIndex == 2 );
     
    // Test where view is re-read and mathing event is updated
    mIndex = 0;
    state.mEventIndex = 0;
    LogClientStubsHelper::setViewCount(1);
    QVERIFY( !state.continueL() );
    QVERIFY( mEvents.count() == 2 );
    QVERIFY( mIndex == 1 );
    
    // Test where searching duplicates for missed call event
    mIndex = 0;
    state.mEventIndex = 0;
    const_cast<CLogEvent&>( mLogView->Event() ).SetDirection( KTestMissedDirection );
    LogClientStubsHelper::setViewCount(1); // effective also for duplicates view
    
    // Duplicate finding activation fails
    LogClientStubsHelper::setStubAsyncCallPossible(false);
    QVERIFY( !state.continueL() );
    QVERIFY( !state.mCheckingMissed );
    QVERIFY( mEvents.count() > 0 );
    QVERIFY( mEvents.at(0)->duplicates() == 0 );
    
    // Duplicate finding activation ok
    mIndex = 0;
    state.mEventIndex = 0;
    LogClientStubsHelper::setStubAsyncCallPossible(true);
    QVERIFY( state.continueL() );
    QVERIFY( state.mCheckingMissed );
    QVERIFY( !state.continueL() );
    QVERIFY( !state.mCheckingMissed );
    QVERIFY( mEvents.count() > 0 );
    QVERIFY( mEvents.at(0)->duplicates() == 1 );
    
    // Duplicate finding not done for full event view
    qDeleteAll(mEvents);
    mEvents.clear();
    mIsRecentView = false;
    mIndex = 0;
    state.mEventIndex = 0;
    state.mCheckingMissed = false;
    LogClientStubsHelper::setStubAsyncCallPossible(true);
    QVERIFY( !state.continueL() ); // No next state
    QVERIFY( !state.mCheckingMissed );
}

void UT_LogsReaderStates::testStateReading2()
{
    // Test reading when max size has been defined
    mIndex = 0;
    LogsCommonData::getInstance().configureReadSize(3);
    LogClientStubsHelper::setViewCount(5);
    
    LogsReaderStateReading state(*this, *this);
    
    // Reading starts ok
    QVERIFY( state.enterL() );
    QVERIFY( state.mReadSizeCounter == 0 );
    
     // Reading continues as more events in view exist, new event is added
    QVERIFY( mEvents.count() == 0 );
    const_cast<CLogEvent&>( mLogView->Event() ).SetNumber( _L("12345") );
    const_cast<CLogEvent&>( mLogView->Event() ).SetId( 100 );
    QVERIFY( state.continueL() );
    QVERIFY( mEvents.count() == 1 );
    QVERIFY( mIndex == 1 );
    
    // Reading continues as max size has not been reached
    const_cast<CLogEvent&>( mLogView->Event() ).SetId( 101 );
    QVERIFY( state.continueL() );
    QVERIFY( mEvents.count() == 2 );
    QVERIFY( mIndex == 2 );
    
    // Reading does not continue as max size has been reached
    const_cast<CLogEvent&>( mLogView->Event() ).SetId( 102 );
    QVERIFY( !state.continueL() );
    QVERIFY( mEvents.count() == 3 );
    QVERIFY( mIndex == 3 );
    LogsCommonData::getInstance().configureReadSize(-1);
}

void UT_LogsReaderStates::testStateFillDetails()
{
    LogsReaderStateFillDetails state(*this, *this);
    int contactId = 2;
    
    // No events, nothing to check
    QVERIFY( !state.enterL() );
    QVERIFY( mContactCache.count() == 0 );
    
    // Remote name exists already
    LogsEvent* logsEvent = new LogsEvent;
    logsEvent->setIsInView(true);
    logsEvent->setRemoteParty( "remote" );
    LogsEvent* logsEvent2 = new LogsEvent;
    logsEvent2->setRemoteParty( "remote2" );
    logsEvent2->setIsInView(true);
    mEvents.append( logsEvent );
    mEvents.append( logsEvent2 );
    QVERIFY( !state.enterL() );
    QVERIFY( mContactCache.count() == 0 );
    QVERIFY(!logsEvent->contactMatched());
   
    // Some events, nothing yet in cache, match from phonebook not found
    QtContactsStubsHelper::setContactId( contactId );
    logsEvent->setRemoteParty( "" );
    logsEvent->setNumber( "99999999" );
    QVERIFY( !state.enterL() );
    QVERIFY( mContactCache.count() == 0 );
    QVERIFY( !logsEvent->contactLocalId() );
    QVERIFY(!logsEvent->contactMatched());
    
    
    // Some events, nothing yet in cache, match from phonebook found (international format)
    // Match is not used for event which has already remoteparty name
    QtContactsStubsHelper::setContactNames("first", "last");
    logsEvent->setNumber( "+3581112222" );
    logsEvent2->setNumber( "+3581112222" );
    QVERIFY( !state.enterL() );
    QVERIFY( mContactCache.count() == 1 );
    QVERIFY( logsEvent->contactLocalId() == contactId );
    QVERIFY(logsEvent->contactMatched());
    QCOMPARE(logsEvent->remoteParty(), QString("first last") );
    QVERIFY(!logsEvent2->contactMatched());
    QCOMPARE(logsEvent2->remoteParty(), QString("remote2") );
    
    // Some events, nothing yet in cache, match from phonebook found (local format)
    mContactCache.clear();
    logsEvent->setRemoteParty( "" );
    logsEvent->setNumber( "11112222" );
    logsEvent->setLogsEventData(NULL);
    QVERIFY( !state.enterL() );
    QVERIFY( mContactCache.count() == 1 );
    QVERIFY( logsEvent->contactLocalId() == contactId );
    QVERIFY(logsEvent->contactMatched());
    QCOMPARE(logsEvent->remoteParty(), QString("first last") );
    
    // Some events, matching info found from cache
    logsEvent->setLogsEventData(NULL);
    logsEvent->setRemoteParty( "" );
    QVERIFY( !logsEvent->contactLocalId() );
    QVERIFY( !state.enterL() );
    QVERIFY( mContactCache.count() == 1 );
    QVERIFY( logsEvent->remoteParty().length() > 0 );
    QVERIFY( logsEvent->contactLocalId() == contactId );
    
    QtContactsStubsHelper::setContactNames("updated", "last");
    QCOMPARE(logsEvent->remoteParty(), QString("first last") );
    mContactCache.clear();
    logsEvent->setContactMatched( false );
    logsEvent->setRemoteParty("");
    QVERIFY(!logsEvent->contactMatched());
    QVERIFY( mContactCache.count() == 0 );
    QVERIFY( logsEvent->remoteParty().length() == 0 );
    QVERIFY( !state.enterL() );
    QVERIFY( mContactCache.count() == 1 );
    QVERIFY( logsEvent->remoteParty().length() > 0 );
    QVERIFY(logsEvent->contactMatched());
    QVERIFY( logsEvent->remoteParty() == "updated last" );
    
    //matching info not found in cache
    QtContactsStubsHelper::reset();
    mContactCache.clear();
    logsEvent->setContactMatched( false );
    logsEvent->setRemoteParty("");
    logsEvent->setNumber( "5555" );
    QVERIFY( mContactCache.count() == 0 );
    QVERIFY( logsEvent->remoteParty().length() == 0 );
    QVERIFY( !state.enterL() );
    QVERIFY( mContactCache.count() == 0 );
    QVERIFY( logsEvent->remoteParty().length() == 0 );
    QVERIFY(!logsEvent->contactMatched());
    
    // Check that optimization for avoiding multiple searches
    // per one number works
    QtContactsStubsHelper::reset();
    mContactCache.clear();
    LogsEvent* logsEvent3 = new LogsEvent;
    logsEvent3->setNumber( "5555" );
    logsEvent3->setIsInView(true);
    mEvents.append( logsEvent3 );
    QCOMPARE( logsEvent->number(), logsEvent3->number() );
    QVERIFY( !state.enterL() );
    QVERIFY( mContactCache.count() == 0 );
    QVERIFY( logsEvent->remoteParty().length() == 0 );
    QVERIFY( logsEvent3->remoteParty().length() == 0 );
    QVERIFY(!logsEvent->contactMatched());
    QVERIFY(!logsEvent3->contactMatched());
    QCOMPARE( QtContactsStubsHelper::contactIdsMethodCallCount(), 1 );
    
}

void UT_LogsReaderStates::testStateFillDetails2()
{
    // Test duplicate lookup
    LogsReaderStateFillDetails state(*this, *this);
    
    // Event whithout remote party is not used in lookup
    LogsEvent* logsEvent = new LogsEvent;
    logsEvent->setIsInView(true);
    logsEvent->setNumber( "222333" );
    logsEvent->setDirection(LogsEvent::DirOut);
    mEvents.append( logsEvent );
    QVERIFY( !state.mDuplicateLookup.findDuplicate(*logsEvent) );
    QVERIFY( !state.enterL() );
    QVERIFY( !state.mDuplicateLookup.findDuplicate(*logsEvent) );
    QVERIFY( logsEvent->isInView() );
    
    // Event with remote party and number is added to lookup
    logsEvent->setRemoteParty( "remote" );
    logsEvent->setNumber( "11112222" );
    QVERIFY( !state.enterL() );
    QVERIFY( state.mDuplicateLookup.findDuplicate(*logsEvent) );
    QVERIFY( logsEvent->isInView() );
    QCOMPARE( logsEvent->mergedDuplicates().count(), 0 );
        
    // Event with contact matched remote party is ignored as being
    // duplicate for "historical" event
    QtContactsStubsHelper::setContactNames("remote", "");
    LogsEvent* logsEvent2 = new LogsEvent;
    logsEvent2->setIsInView(true);
    logsEvent2->setNumber( "11112222" );
    logsEvent2->setDirection(LogsEvent::DirOut);
    mEvents.append( logsEvent2 );
    QVERIFY( !state.enterL() );
    QVERIFY( state.mDuplicateLookup.findDuplicate(*logsEvent) );
    QVERIFY( logsEvent->isInView() );
    QVERIFY( !logsEvent2->isInView() );
    QCOMPARE( logsEvent->mergedDuplicates().count(), 1 );
       
    // Event with contact matched remote party is not ignored
    // because it is not in yet in lookup
    mContactCache.clear();
    QtContactsStubsHelper::setContactNames("otherremote", "party");
    LogsEvent* logsEvent3 = new LogsEvent;
    logsEvent3->setIsInView(true);
    logsEvent3->setNumber( "11112222" );
    logsEvent3->setDirection(LogsEvent::DirOut);
    mEvents.append( logsEvent3 );
    QVERIFY( !state.enterL() );
    QVERIFY( state.mDuplicateLookup.findDuplicate(*logsEvent) );
    QVERIFY( state.mDuplicateLookup.findDuplicate(*logsEvent3) );
    QVERIFY( logsEvent->isInView() );
    QVERIFY( logsEvent3->isInView() );
    QCOMPARE( logsEvent->mergedDuplicates().count(), 1 );
    QCOMPARE( logsEvent3->mergedDuplicates().count(), 0 );
}

void UT_LogsReaderStates::testStateDone()
{
    LogsReaderStateDone state(*this, *this);
    mIndex = 3;
    LogClientStubsHelper::setViewCount(3);
    QVERIFY( !state.enterL() );
    QVERIFY( mReadCompleted );
    
    // Reading hasn't gone through whole db view (e.g. maxsize has been defined)
    reset();
    mIndex = 2;
    QVERIFY( !state.enterL() );
    QVERIFY( mReadCompleted );
}

void UT_LogsReaderStates::testStateSearchingEvent()
{
    // Searching starts ok
    LogsStateSearchingEvent state(*this);
    mCurrentEventId = 10;
    QVERIFY( state.enterL() );
    
    // Searching doesn't start ok as no items in view
    LogClientStubsHelper::setViewCount(0);
    QVERIFY( !state.enterL() );
      
    // Searching event continues
    mIndex = 0;
    mCurrentEventId = -1;
    const_cast<CLogEvent&>( mLogView->Event() ).SetId( 100 );
    LogClientStubsHelper::setViewCount(2);
    QVERIFY( state.continueL() );

    // Last event handled, no target event found, entering next state
    mIndex = 2;
    QVERIFY( !state.continueL() );
    
     // Target event found, entering next state
    mIndex = 0;
    mCurrentEventId = 100;
    QVERIFY( !state.continueL() );
}


void UT_LogsReaderStates::testStateFindingDuplicates()
{
      // Finding starts ok
    LogsReaderStateFindingDuplicates state(*this, *this);
    QVERIFY( state.enterL() );
    
    // Duplicates cannot be searched for some reason
    LogClientStubsHelper::reset();
    LogClientStubsHelper::setStubAsyncCallPossible(false);
    QVERIFY( !state.enterL() );
    LogClientStubsHelper::setStubAsyncCallPossible(true);
    
    // Duplicates can be searched
    QVERIFY( state.enterL() );
    
    // Searching completes, no next state to enter
    QVERIFY( !state.continueL() );
}

void UT_LogsReaderStates::testStateMarkingDuplicates()
{
    // Marking does not start as no matching event in view
    LogsReaderStateMarkingDuplicates state(*this, *this);
    mCurrentEventId = 5;
    const_cast<CLogEvent&>( mLogView->Event() ).SetId( 100 );
    QVERIFY( !state.enterL() );
    QVERIFY( !state.mGettingDuplicates );
    QVERIFY( !(mLogView->Event().Flags() & KLogEventRead) );
    
    // Marking can start ok
    mCurrentEventId = 100;
    QVERIFY( state.enterL() );
    QVERIFY( !state.mGettingDuplicates );
    QVERIFY( mLogView->Event().Flags() & KLogEventRead );
    
    // Duplicates cannot be searched for some reason
    LogClientStubsHelper::reset();
    LogClientStubsHelper::setStubAsyncCallPossible(false);
    QVERIFY( !(LogClientStubsHelper::stubViewFlags() & KLogEventRead) );
    QVERIFY( !state.continueL() );
    QVERIFY( !(LogClientStubsHelper::stubViewFlags() & KLogEventRead) );
    QVERIFY( !state.mGettingDuplicates );
    LogClientStubsHelper::setStubAsyncCallPossible(true);
    
    // Duplicates searching starts ok
    QVERIFY( state.continueL() );
    QVERIFY( !(LogClientStubsHelper::stubViewFlags() & KLogEventRead) );
    QVERIFY( state.mGettingDuplicates );
    
    // Duplicates searching completes, view flags are set, no next state to enter
    QVERIFY( !state.continueL() );
    QVERIFY( LogClientStubsHelper::stubViewFlags() & KLogEventRead );
}

void UT_LogsReaderStates::testStateReadingDuplicates()
{
    // Duplicates view empty, cannot start
    LogClientStubsHelper::setViewCount(0);
    LogsReaderStateReadingDuplicates state(*this, *this);
    
    QVERIFY( !state.enterL() );
    
    // Starting ok
    LogClientStubsHelper::setViewCount(2);
    const_cast<CLogEvent&>( mDuplicatesView->Event() ).SetNumber( _L("12345") );
    const_cast<CLogEvent&>( mDuplicatesView->Event() ).SetId( 100 );

    QVERIFY( state.enterL() );
    QVERIFY( mDuplicatedEvents.count() == 0 );
    
    // Continue reading as more events in view
    QVERIFY( state.continueL() );
    QVERIFY( mDuplicatedEvents.count() == 1 );
    
    // Don't continue reading as no more events in view
    LogClientStubsHelper::setStubAsyncCallPossible(false);
    QVERIFY( !state.continueL() );
    QVERIFY( mDuplicatedEvents.count() == 2 );
    
}

void UT_LogsReaderStates::testStateMergingDuplicates()
{
    LogsReaderStateMergingDuplicates state(*this, *this);
    
    // No event
    LogsEvent* origDupl = new LogsEvent;
    origDupl->setDirection(LogsEvent::DirMissed);
    origDupl->setLogId(100);
    QDateTime dateTime5thJan = QDateTime::fromString("M1d5y9800:01:02","'M'M'd'd'y'yyhh:mm:ss");
    origDupl->setTime(dateTime5thJan);
    mDuplicatedEvents.append(origDupl);
    LogsEvent* origDupl2 = new LogsEvent;
    origDupl2->setDirection(LogsEvent::DirMissed);
    origDupl2->setLogId(101);
    QDateTime dateTime4thJan = QDateTime::fromString("M1d4y9800:01:02","'M'M'd'd'y'yyhh:mm:ss");
    origDupl2->setTime(dateTime4thJan);
    mDuplicatedEvents.append(origDupl2);
    QVERIFY( !state.enterL() );
    QCOMPARE( mDuplicatedEvents.count(), 2 );
    
    // No events to merge
    LogsEvent* logsEvent = new LogsEvent;
    logsEvent->setIsInView(true);
    logsEvent->setNumber( "222333" );
    logsEvent->setDirection(LogsEvent::DirMissed);
    logsEvent->setLogId(99);
    mCurrentEventId = 99;
    mEvents.append( logsEvent );
    QVERIFY( !state.enterL() );
    QCOMPARE( mDuplicatedEvents.count(), 2 );
    
    // No unseen events to merge
    LogsEvent mergedEv;
    mergedEv.setDirection(LogsEvent::DirMissed);
    mergedEv.setLogId(111);
    mergedEv.markedAsSeenLocally(true);
    logsEvent->mergedDuplicates().append(mergedEv);
    QVERIFY( !state.enterL() );
    QCOMPARE( mDuplicatedEvents.count(), 2 );
    
    // Unseen events to merge, check also time based ordering
    LogsEvent mergedEv2;
    mergedEv2.setDirection(LogsEvent::DirMissed);
    mergedEv2.setLogId(200);
    mergedEv2.markedAsSeenLocally(false);
    QDateTime dateTime4thJanLaterTime = QDateTime::fromString("M1d4y9800:02:02","'M'M'd'd'y'yyhh:mm:ss");
    mergedEv2.setTime(dateTime4thJanLaterTime);
    logsEvent->mergedDuplicates().append(mergedEv2);
    LogsEvent mergedEv3;
    mergedEv3.setDirection(LogsEvent::DirMissed);
    mergedEv3.setLogId(300);
    mergedEv3.markedAsSeenLocally(false);
    QDateTime dateTime6thJan = QDateTime::fromString("M1d6y9800:01:02","'M'M'd'd'y'yyhh:mm:ss");
    mergedEv3.setTime(dateTime6thJan);
    logsEvent->mergedDuplicates().append(mergedEv3);
    QVERIFY( !state.enterL() );
    QCOMPARE( mDuplicatedEvents.count(), 4 );
    QCOMPARE( mDuplicatedEvents.at(0)->logId(), mergedEv3.logId() );
    QCOMPARE( mDuplicatedEvents.at(1)->logId(), origDupl->logId() );
    QCOMPARE( mDuplicatedEvents.at(2)->logId(), mergedEv2.logId() );
    QCOMPARE( mDuplicatedEvents.at(3)->logId(), origDupl2->logId() );
}


void UT_LogsReaderStates::testStateReadingDuplicatesDone()
{
    LogsReaderStateReadingDuplicatesDone state(*this, *this);
    
    // No duplicates was found
    QVERIFY( !state.enterL() );
    QVERIFY( mDuplicatesReadingCompletedCount == 0 );
    
    // Duplicates were found, ownership transferred to observer
    LogsEvent* event = new LogsEvent;
    mDuplicatedEvents.append(event);
    LogsEvent* event2 = new LogsEvent;
    mDuplicatedEvents.append(event2);
    QVERIFY( !state.enterL() );
    QVERIFY( mDuplicatesReadingCompletedCount == 2 );
    QVERIFY( mDuplicatedEvents.count() == 0 );
}

void UT_LogsReaderStates::testStateModifyingDone()
{
    LogsReaderStateModifyingDone state(*this, *this);
    
    // Modified event not found for some reason
    LogsEvent* logsEvent = new LogsEvent;
    logsEvent->setIsInView(true);
    mCurrentEventId = 100;
    mEvents.append( logsEvent );
    QVERIFY( !mModifyCompleted );
    QVERIFY( !state.enterL() );
    QVERIFY( mModifyCompleted );
    QVERIFY( !logsEvent->isSeenLocally() );
    
    // Modified event found and is set to locally read
    mModifyCompleted = false;
    logsEvent->setLogId(100);
    QVERIFY( !state.enterL() );
    QVERIFY( mModifyCompleted );
    QVERIFY( logsEvent->isSeenLocally() );
}

// ----------------------------------------------------------------------------
// From LogsStateBaseContext
// ----------------------------------------------------------------------------
//
void UT_LogsReaderStates::setCurrentState(const LogsStateBase& state)
{
    mCurrentState = &state;
}

CLogView& UT_LogsReaderStates::logView()
{
    if ( mIsRecentView ){
        return *mLogView;
    }
    return *mLogViewEvent;
}

CLogViewDuplicate& UT_LogsReaderStates::duplicatesView()
{
    return *mDuplicatesView;
}

int& UT_LogsReaderStates::index()
{
    return mIndex;
}

TRequestStatus& UT_LogsReaderStates::reqStatus()
{
    return mReqStatus;
}


int UT_LogsReaderStates::currentEventId()
{
    return mCurrentEventId;
}

CLogClient& UT_LogsReaderStates::logClient()
{
    return *mLogClient;
}

bool UT_LogsReaderStates::isRecentView()
{
    return mIsRecentView;
}

// ----------------------------------------------------------------------------
// From LogsReaderStateContext
// ----------------------------------------------------------------------------
//

LogsEventStrings& UT_LogsReaderStates::strings()
{
    return mStrings;
}

LogsReaderObserver& UT_LogsReaderStates::observer()
{
    return *this;
}

QHash<QString, ContactCacheEntry>& UT_LogsReaderStates::contactCache()
{
    return mContactCache;
}

QList<LogsEvent*>& UT_LogsReaderStates::events()
{
    return mEvents;
}

QList<LogsEvent*>& UT_LogsReaderStates::duplicatedEvents()
{
    return mDuplicatedEvents;
}

// ----------------------------------------------------------------------------
// From LogsReaderObserver
// ----------------------------------------------------------------------------
//
void UT_LogsReaderStates::readCompleted()
{
    mReadCompleted = true;
}
void UT_LogsReaderStates::errorOccurred(int err)
{
    mError = err;
}
void UT_LogsReaderStates::temporaryErrorOccurred(int err)
{
    mTemporaryError = err;
}
void UT_LogsReaderStates::eventModifyingCompleted()
{
    mModifyCompleted = true;
}

void UT_LogsReaderStates::duplicatesReadingCompleted(QList<LogsEvent*> duplicates)
{
    mDuplicatesReadingCompletedCount = duplicates.count();
    qDeleteAll(duplicates);
}
