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
#include "ut_logsdbconnector.h"
#include "logsdbconnector.h"
#include "logsevent.h"
#include "logsmodel.h"
#include "logsreader.h"
#include "logscommondata.h"
#include "logsremove.h"
#include <xqsettingsmanager.h>

#include <QtTest/QtTest>

Q_DECLARE_METATYPE(QList<int>)

#define LOGS_TEST_CREATE_EVENT_WITHOUT_IDX(eventName, id, eventState ) \
LogsEvent* eventName = new LogsEvent; \
eventName->setLogId(id);\
eventName->setIsInView(true); \
eventName->mEventState = eventState; \
mDbConnector->mEvents.insert(id, eventName)

#define LOGS_TEST_CREATE_EVENT(eventName, index, eventState ) \
LogsEvent* eventName = new LogsEvent; \
eventName->setIndex(index); \
eventName->setLogId(index);\
eventName->setIsInView(true); \
eventName->mEventState = eventState; \
mDbConnector->mEvents.insert(index, eventName)

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

void UT_LogsDbConnector::initTestCase()
{

}

void UT_LogsDbConnector::cleanupTestCase()
{
}


void UT_LogsDbConnector::init()
{
    mDbConnector = new LogsDbConnector(mEvents);
}

void UT_LogsDbConnector::cleanup()
{
    delete mDbConnector;
    mDbConnector = 0;
}

void UT_LogsDbConnector::testConstructor()
{
    QVERIFY( mDbConnector );
}

void UT_LogsDbConnector::testInit()
{
    QVERIFY( !mDbConnector->mLogsRemove );
    QVERIFY( mDbConnector->init() == 0 );
    QVERIFY( mDbConnector->mReader );
    QVERIFY( mDbConnector->mLogsRemove );
    QVERIFY( mDbConnector->mLogEventStrings.iFetched.length() > 0 );
    QVERIFY( !mDbConnector->mCompressionEnabled );
    QVERIFY( LogsCommonData::getInstance().maxReadSize() == logsReadSizeUndefined );
    
    // Resource control enabled
    XQSettingsManager::mCurrentVal = logsDefaultMatchLength + 2;
    LogsDbConnector* connector = new LogsDbConnector(mEvents, false, true);
    QVERIFY( connector->init() == 0 );
    QVERIFY( connector->mReader );
    QVERIFY( connector->mLogsRemove );
    QVERIFY( connector->mCompressionEnabled );
    QVERIFY( LogsCommonData::getInstance().maxReadSize() == logsReadSizeCompressEnabled );
    QCOMPARE( LogsCommonData::getInstance().telNumMatchLen(), logsDefaultMatchLength + 2 );
    delete connector;
    LogsCommonData::getInstance().freeCommonData();
    
    // Match len not found, default is used
    XQSettingsManager::mCurrentVal = logsDefaultMatchLength + 2;
    XQSettingsManager::mFailed = true;
    connector = new LogsDbConnector(mEvents);
    QVERIFY( connector->init() == 0 );
    QVERIFY( connector->mReader );
    QVERIFY( connector->mLogsRemove );
    QCOMPARE( LogsCommonData::getInstance().telNumMatchLen(), logsDefaultMatchLength );
    
}

void  UT_LogsDbConnector::testClearList()
{
    QVERIFY( !mDbConnector->mLogsRemove );
    QVERIFY( !mDbConnector->clearList(LogsModel::TypeLogsClearAll) );
    
    mDbConnector->init();
    QVERIFY( mDbConnector->mLogsRemove );
    QVERIFY( mDbConnector->clearList(LogsModel::TypeLogsClearAll) );
}

void UT_LogsDbConnector::testClearEvents()
{
    QVERIFY( !mDbConnector->mLogsRemove );
    QList<LogsEvent*> events;
    ADD_EVENT_WITH_ID(events, 1);
    QVERIFY( !mDbConnector->clearEvents(events) ); // sync
    
    mDbConnector->init();
    QVERIFY( mDbConnector->mLogsRemove );
    mDbConnector->clearEvents(events); 
    QVERIFY( mDbConnector->mReader->mLocked );
    QVERIFY( !mDbConnector->clearEvents(events) ); // Already clearing
    QVERIFY( mDbConnector->mReader->mLocked );
    qDeleteAll(events);
    
    // Remove completed or removeError causes read lock release
    mDbConnector->removeCompleted();
    QVERIFY( !mDbConnector->mReader->mLocked );
    mDbConnector->mReader->mLocked = true;
    mDbConnector->logsRemoveErrorOccured(-1);
    QVERIFY( !mDbConnector->mReader->mLocked );
}

void UT_LogsDbConnector::testMarkEventsSeen()
{
    QList<LogsEvent*> events;
    QSignalSpy spy( mDbConnector, SIGNAL(markingCompleted(int)) );
    
    // Not ready
    mDbConnector->mEventsSeen.clear();
    mDbConnector->markEventsSeen(events);
    QVERIFY( mDbConnector->mEventsSeen.count() == 0 );
    
    // No events
    mDbConnector->init();
    mDbConnector->markEventsSeen(events);
    QVERIFY( mDbConnector->mEventsSeen.count() == 0 );
    
    // Marking missed as seen (one is already seen)
    LOGS_TEST_CREATE_EVENT(event, 0, LogsEvent::EventAdded );
    event->setDirection(LogsEvent::DirMissed);
    LOGS_TEST_CREATE_EVENT(event2, 1, LogsEvent::EventAdded );
    event2->setDirection(LogsEvent::DirMissed);
    LOGS_TEST_CREATE_EVENT(event3, 2, LogsEvent::EventAdded );
    event3->setDirection(LogsEvent::DirMissed);
    ADD_EVENT_WITH_ID(events, 0);
    ADD_EVENT_WITH_ID(events, 1);
    QVERIFY( mDbConnector->markEventsSeen(events) );
    QVERIFY( mDbConnector->mEventsSeen.count() == 2 );
    QVERIFY( mDbConnector->mReader->mCurrentEventId == 0 ); // Started modifying
    
    // Trying to clear missed again, id is appended to mark list, old modifying process in ongoing
    // and is not interrupted
    ADD_EVENT_WITH_ID(events, 2);
    QVERIFY( !mDbConnector->markEventsSeen(events) );
    QVERIFY( mDbConnector->mEventsSeen.count() == 3 );
    QVERIFY( mDbConnector->mReader->mCurrentEventId == 0 ); // Modifying still previous
    
    // Completed previous modifying, next one in queue is modified
    mDbConnector->mReader->Cancel();
    mDbConnector->eventModifyingCompleted();
    QVERIFY( mDbConnector->mEventsSeen.count() == 2 );
    QVERIFY( mDbConnector->mReader->mCurrentEventId == 1 ); // Started modifying next one
    QVERIFY( spy.count() == 0 ); // Not yet totally completed
    
    // Last pending gets completed
    mDbConnector->mEventsSeen.takeLast(); // Now only one pending left
    mDbConnector->eventModifyingCompleted();
    QVERIFY( mDbConnector->mEventsSeen.count() == 0 );
    QVERIFY( spy.count() == 1 ); // Completion was signaled with err 0
    QVERIFY( spy.takeFirst().at(0).toInt() == 0 );
    
    // Clearing all, ids are not appended as those are already in modification list
    mDbConnector->mEventsSeen.clear();
    ADD_EVENT_WITH_ID_2(mDbConnector->mEventsSeen, 0);
    ADD_EVENT_WITH_ID_2(mDbConnector->mEventsSeen, 1);
    ADD_EVENT_WITH_ID_2(mDbConnector->mEventsSeen, 2);
    QVERIFY( !mDbConnector->markEventsSeen(events) );
    QVERIFY( mDbConnector->mEventsSeen.count() == 3 );
    
    // Marked events contains merged duplicates which will be handled same way
    // as any other event
    mDbConnector->mEventsSeen.clear();
    qDeleteAll(events);
    events.clear();
    ADD_EVENT_WITH_ID(events, 8);
    ADD_EVENT_WITH_ID(events, 9);
    ADD_EVENT_WITH_ID_2( events.at(0)->mergedDuplicates(), 88 );
    ADD_EVENT_WITH_ID_2( events.at(0)->mergedDuplicates(), 9 ); // already exists in main event list
    QVERIFY( !mDbConnector->markEventsSeen(events) );
    QVERIFY( mDbConnector->mEventsSeen.count() == 3 );
    QVERIFY( mDbConnector->mEventsSeen.at(0).logId() == 8 );
    QVERIFY( mDbConnector->mEventsSeen.at(1).logId() == 88 );
    QVERIFY( mDbConnector->mEventsSeen.at(2).logId() == 9 );
    qDeleteAll(events);
}

void UT_LogsDbConnector::testReadDuplicates()
{
    QSignalSpy spy( mDbConnector, SIGNAL(duplicatesRead()) );
    
    // Not ready
    QVERIFY( mDbConnector->readDuplicates(2) != 0 );
    
    // Previous results are cleared when starting ok
    LogsEvent* event = new LogsEvent;
    mDbConnector->mDuplicatedEvents.append(event);
    mDbConnector->init();
    mDbConnector->readDuplicates(2);
    QVERIFY( mDbConnector->mDuplicatedEvents.count() == 0 );
    
    // Completes
    LogsEvent* event2 = new LogsEvent;
    LogsEvent* event3 = new LogsEvent;
    QList<LogsEvent*> duplicates;
    duplicates.append(event2);
    duplicates.append(event3);
    mDbConnector->duplicatesReadingCompleted(duplicates);
    QVERIFY( mDbConnector->mDuplicatedEvents.count() == 2 );
    QVERIFY( spy.count() == 1 );
    
    // Client reads those events
    QList<LogsEvent*> takenEvents = mDbConnector->takeDuplicates();
    QVERIFY( mDbConnector->mDuplicatedEvents.count() == 0 );
    QVERIFY( takenEvents.count() == 2 );
    
}

void UT_LogsDbConnector::testStart()
{
    // No reader, starting fails
    QVERIFY( mDbConnector->start() == -1 );
    
    // Starting ok
    mDbConnector->init();
    QVERIFY( mDbConnector->start() == 0 );
}

void UT_LogsDbConnector::testReadCompleted()
{
    qRegisterMetaType< QList<int> >("QList<int>");
    QSignalSpy spyAdded(mDbConnector, SIGNAL(dataAdded(QList<int>)));
    QSignalSpy spyRemoved(mDbConnector, SIGNAL(dataRemoved(QList<int>)));
    QSignalSpy spyUpdated(mDbConnector, SIGNAL(dataUpdated(QList<int>)));
    QSignalSpy spyReset(mDbConnector, SIGNAL(dataReset()));

    // No events, no signal
    mDbConnector->readCompleted();
    QVERIFY( spyAdded.count() == 0 );
    QVERIFY( spyRemoved.count() == 0 );
    QVERIFY( spyUpdated.count() == 0 );
    QVERIFY( spyReset.count() == 0 );
    
    // Events exists, their indexes are signaled, indexes are assigned at this phase
    LOGS_TEST_CREATE_EVENT_WITHOUT_IDX(event, 0, LogsEvent::EventAdded );
    QCOMPARE( event->index(), -1 );
    mDbConnector->readCompleted();
    QCOMPARE( event->index(), 0 );
    QVERIFY( spyAdded.count() == 1 );
    QList<int> addedIndexes = qvariant_cast< QList<int> >(spyAdded.at(0).at(0));
    QVERIFY( addedIndexes.count() == 1 );
    QVERIFY( addedIndexes.at(0) == 0 );
    QVERIFY( spyRemoved.count() == 0 );
    QVERIFY( spyUpdated.count() == 0 );
    QVERIFY( spyReset.count() == 0 );
    QVERIFY( mDbConnector->mEvents.count() == 1 );
    QVERIFY( mEvents.count() == 1 );
    
    // 2 more events added, their indexes are signaled
    event->mEventState = LogsEvent::EventNotUpdated;
    LOGS_TEST_CREATE_EVENT_WITHOUT_IDX(event2, 0, LogsEvent::EventAdded );
    LOGS_TEST_CREATE_EVENT_WITHOUT_IDX(event3, 1, LogsEvent::EventAdded );
    mDbConnector->readCompleted();
    QCOMPARE( event2->index(), 0 );
    QCOMPARE( event3->index(), 1 );
    QCOMPARE( event->index(), 2 );
    QVERIFY( spyAdded.count() == 2 );
    QList<int> addedIndexes2 = qvariant_cast< QList<int> >(spyAdded.at(1).at(0));
    QVERIFY( addedIndexes2.count() == 2 );
    QVERIFY( addedIndexes2.at(0) == 0 );
    QVERIFY( addedIndexes2.at(1) == 1 );
    QVERIFY( spyRemoved.count() == 0 );
    QVERIFY( spyUpdated.count() == 0 );
    QVERIFY( spyReset.count() == 0 );
    QVERIFY( mDbConnector->mEvents.count() == 3 );
    QVERIFY( mEvents.count() == 3 );
    
    // One of the events updated (index 1)
    event->mEventState = LogsEvent::EventNotUpdated;
    event2->mEventState = LogsEvent::EventNotUpdated;
    event3->mEventState = LogsEvent::EventUpdated;
    mDbConnector->readCompleted(); 
    QVERIFY( spyAdded.count() == 2 );
    QVERIFY( spyRemoved.count() == 0 );
    QVERIFY( spyUpdated.count() == 1 );
    QVERIFY( spyReset.count() == 0 );
    QList<int> updatedIndexes = qvariant_cast< QList<int> >(spyUpdated.at(0).at(0));
    QVERIFY( updatedIndexes.count() == 1 );
    QVERIFY( updatedIndexes.at(0) == 1 );
    QVERIFY( mDbConnector->mEvents.count() == 3 );
    QVERIFY( mEvents.count() == 3 );
    
    // One of the events removed (index 2)
    event->setIsInView(false);
    event3->mEventState = LogsEvent::EventNotUpdated;
    mDbConnector->readCompleted();
    QVERIFY( spyAdded.count() == 2 );
    QVERIFY( spyRemoved.count() == 1 );
    QVERIFY( spyUpdated.count() == 1 );
    QVERIFY( spyReset.count() == 0 );
    QList<int> removedIndexes = qvariant_cast< QList<int> >(spyRemoved.at(0).at(0));
    QVERIFY( removedIndexes.count() == 1 );
    QVERIFY( removedIndexes.at(0) == 2 ); // index 2
    QVERIFY( mDbConnector->mEvents.count() == 2 );
    QVERIFY( mEvents.count() == 2 );
    
    // Event added and removed, reset should be signaled
    mDbConnector->mEvents.at(0)->mEventState = LogsEvent::EventAdded;
    mDbConnector->mEvents.at(1)->mIsInView = false;
    mDbConnector->readCompleted();
    QVERIFY( spyAdded.count() == 2 );
    QVERIFY( spyRemoved.count() == 1 );
    QVERIFY( spyUpdated.count() == 1 );
    QVERIFY( spyReset.count() == 1 );
    QVERIFY( mDbConnector->mEvents.count() == 1 );
    QVERIFY( mEvents.count() == 1 );

    // Read completed when compression is enabled, reader is stopped
    QVERIFY( mDbConnector->init() == 0 );
    QVERIFY( mDbConnector->start() == 0 );
    QVERIFY( mDbConnector->mReader->mLogViewRecent != 0 );
    mDbConnector->mCompressionEnabled = true;
    mDbConnector->readCompleted();
    QVERIFY( !mDbConnector->mReader->mLogViewRecent );
    QVERIFY( !mDbConnector->mLogsRemove );
}

void UT_LogsDbConnector::testErrorOccurred()
{
    // If pending event modifying, completion is signaled
    QSignalSpy spy( mDbConnector, SIGNAL(markingCompleted(int)) );
    ADD_EVENT_WITH_ID_2(mDbConnector->mEventsSeen, 0);
    ADD_EVENT_WITH_ID_2(mDbConnector->mEventsSeen, 1);
    mDbConnector->errorOccurred(-3);
    QVERIFY( spy.count() == 1 ); // Completion was signaled with err -3
    QVERIFY( spy.takeFirst().at(0).toInt() == -3 );
    QVERIFY( mDbConnector->mEventsSeen.count() == 0 );
}

void UT_LogsDbConnector::testUpdateDetails()
{
    QSignalSpy spyUpdated(mDbConnector, SIGNAL(dataUpdated(QList<int>)));
    QVERIFY( !mDbConnector->mReader );
    QVERIFY( mDbConnector->updateDetails(false) == -1 );
    QVERIFY( spyUpdated.count() == 0 );
    
    mDbConnector->init();
    QVERIFY( mDbConnector->mReader );
    LOGS_TEST_CREATE_EVENT(event, 0, LogsEvent::EventUpdated );
    QVERIFY( mDbConnector->updateDetails(false) == 0 );
    QVERIFY( spyUpdated.count() == 0 ); // Will happen asynchronously
}

void UT_LogsDbConnector::testRefreshData()
{
    QVERIFY( mDbConnector->refreshData() != 0 );
    
    // Resource control enabled, no reader
    mDbConnector->mResourceControl = true;
    QVERIFY( mDbConnector->refreshData() == 0 );
    
    // Reader exists, not compressed before, nothing done
    mDbConnector->init();
    mDbConnector->mCompressionEnabled = false;
    QVERIFY( mDbConnector->refreshData() == 0 );
    QVERIFY( !mDbConnector->mReader->IsActive() );
    QVERIFY( LogsCommonData::getInstance().maxReadSize() == logsReadSizeUndefined );
    
    // Reader exists, compressed before, reading started
    delete mDbConnector->mLogsRemove;
    mDbConnector->mLogsRemove = 0;
    mDbConnector->mCompressionEnabled = true;
    QVERIFY( mDbConnector->refreshData() == 0 );
    QVERIFY( mDbConnector->mReader->IsActive() );
    QVERIFY( !mDbConnector->mCompressionEnabled );
    QVERIFY( LogsCommonData::getInstance().maxReadSize() == logsReadSizeUndefined );
    QVERIFY( mDbConnector->mLogsRemove );
    
    // Reading not started again as already active
    LogsCommonData::getInstance().configureReadSize(30, LogsEvent::DirUndefined);
    mDbConnector->mCompressionEnabled = true;
    QVERIFY( mDbConnector->refreshData() == 0 );
    QVERIFY( mDbConnector->mReader->IsActive() );
    QVERIFY( !mDbConnector->mCompressionEnabled );
    QVERIFY( LogsCommonData::getInstance().maxReadSize() == logsReadSizeUndefined );
    QVERIFY( mDbConnector->mLogsRemove );
}

void UT_LogsDbConnector::testCompressData()
{
    qRegisterMetaType< QList<int> >("QList<int>");
    QSignalSpy spyRemoved(mDbConnector, SIGNAL(dataRemoved(QList<int>)));
    
    QVERIFY( mDbConnector->compressData() != 0 );
    
    // Reader exists, zero events, nothing really done
    mDbConnector->mResourceControl = true;
    mDbConnector->init();
    mDbConnector->mCompressionEnabled = false;
    QVERIFY( mDbConnector->compressData() == 0 );
    QVERIFY( mDbConnector->mCompressionEnabled );
    QVERIFY( LogsCommonData::getInstance().maxReadSize() == logsReadSizeCompressEnabled );
    QVERIFY( spyRemoved.count() == 1 );
    QList<int> removedIndexes = qvariant_cast< QList<int> >(spyRemoved.at(0).at(0));
    QVERIFY( removedIndexes.count() == 0 );
    QVERIFY( !mDbConnector->mLogsRemove );
        
    // Less events than compression limit is, nothing really done
    int numEvents = 3;
    for ( int i = 0; i < numEvents; i++ ){
        LOGS_TEST_CREATE_EVENT(event, i, LogsEvent::EventAdded );
    }
    mDbConnector->mCompressionEnabled = false;
    QVERIFY( mDbConnector->compressData() == 0 );
    QVERIFY( mDbConnector->mCompressionEnabled );
    QVERIFY( LogsCommonData::getInstance().maxReadSize() == logsReadSizeCompressEnabled );
    QVERIFY( spyRemoved.count() == 2 );
    QList<int> removedIndexes2 = qvariant_cast< QList<int> >(spyRemoved.at(1).at(0));
    QVERIFY( removedIndexes2.count() == 0 );
    QVERIFY( mDbConnector->mModelEvents.count() == numEvents );
    
    // More events than compression limit is, extra events removed
    int numEventsMoreThanCompressLimit = ( numEvents + logsReadSizeCompressEnabled );
    for ( int i = numEvents; i < numEventsMoreThanCompressLimit; i++ ){
       LOGS_TEST_CREATE_EVENT(event, i, LogsEvent::EventAdded );
    }
    mDbConnector->mCompressionEnabled = false;
    QVERIFY( mDbConnector->compressData() == 0 );
    QVERIFY( mDbConnector->mCompressionEnabled );
    QVERIFY( LogsCommonData::getInstance().maxReadSize() == logsReadSizeCompressEnabled );
    QVERIFY( spyRemoved.count() == 3 );
    QList<int> removedIndexes3 = qvariant_cast< QList<int> >(spyRemoved.at(2).at(0));
    QVERIFY( removedIndexes3.count() == numEvents );
    // Check indexes of those removed events (3 oldest events)
    QVERIFY( removedIndexes3.at(0) == ( numEventsMoreThanCompressLimit - 3 ) ); 
    QVERIFY( removedIndexes3.at(1) == ( numEventsMoreThanCompressLimit - 2) );
    QVERIFY( removedIndexes3.at(2) == ( numEventsMoreThanCompressLimit - 1 ) );
    QVERIFY( mDbConnector->mModelEvents.count() == logsReadSizeCompressEnabled );
}
