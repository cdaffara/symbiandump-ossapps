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
#include "ut_logsevent.h"
#include "logsevent.h"
#include "logseventdata.h"
#include "qtcontacts_stubs_helper.h"
#include <logcli.h>
#include <qcontactmanager.h>
#include <qcontactname.h>

#include <QtTest/QtTest>

void UT_LogsEvent::initTestCase()
{

}

void UT_LogsEvent::cleanupTestCase()
{
}


void UT_LogsEvent::init()
{
    mEvent = new LogsEvent();
}

void UT_LogsEvent::cleanup()
{
    delete mEvent;
    mEvent = 0;
}

void UT_LogsEvent::testConstructor()
{
    QVERIFY( mEvent );
}

void UT_LogsEvent::testInitializeEventL()
{
	CLogEvent* logEvent = 0;
	QT_TRAP_THROWING( logEvent = CLogEvent::NewL(); )
	
	// Test number
	_LIT( eventTestNumber, "123444555" );
	QString strEventTestNumber = DESC_TO_QSTRING( eventTestNumber() );
	logEvent->SetNumber( eventTestNumber() );
	QT_TRAP_THROWING( mEvent->initializeEventL(*logEvent, mStrings); )
    QVERIFY( mEvent->mNumber == strEventTestNumber );
}

void UT_LogsEvent::testSetters()
{
}

void UT_LogsEvent::testGetters()
{
}

void UT_LogsEvent::testValidate()
{
    // No num, remote party nor remote url, not valid
    QVERIFY( !mEvent->validate() );
    
    // Num, valid
    mEvent->mNumber = "123454555";
    QVERIFY( mEvent->validate() );
    
    // Remote party, valid
    mEvent->mNumber.clear();
    mEvent->mRemoteParty = "remotePart";
    QVERIFY( mEvent->validate() );
    
    // Remote url, valid
    mEvent->mRemoteParty.clear();
    LogsEventData* eventData = new LogsEventData;
    eventData->mRemoteUrl = "2343425";
    mEvent->setLogsEventData(eventData);
    QVERIFY( mEvent->validate() );
}

void UT_LogsEvent::testGetNumberForCalling()
{
    mEvent->mNumber = "123454555";
    QVERIFY( mEvent->getNumberForCalling() == "123454555" );
    mEvent->mNumber.clear();
    QVERIFY( mEvent->getNumberForCalling() == "" );
    mEvent->mEventType = LogsEvent::TypeVoIPCall;
    LogsEventData* eventData = new LogsEventData;
    eventData->mRemoteUrl = "2343425";
    mEvent->setLogsEventData(eventData);
    QVERIFY( mEvent->getNumberForCalling() == "2343425" );
    LogsEventData* eventData2 = new LogsEventData;
    eventData2->mRemoteUrl = "test@pr.fi";
    mEvent->setLogsEventData(eventData2);
    QVERIFY( mEvent->getNumberForCalling() == "test@pr.fi" );
}

void UT_LogsEvent::testSetContactLocalId()
{
    //event data hasn't been created yet
    QVERIFY( !mEvent->logsEventData() );
    mEvent->setContactLocalId( 2 );
    QVERIFY( mEvent->logsEventData() );
    QVERIFY( mEvent->logsEventData()->contactLocalId() == 2 );    
    QVERIFY( mEvent->contactLocalId() == 2 );
    
    //event data exists
    mEvent->setContactLocalId( 3 );
    QVERIFY( mEvent->logsEventData()->contactLocalId() == 3 );    
    QVERIFY( mEvent->contactLocalId() == 3 );
    
    //getting contact id with no event data
    mEvent->setLogsEventData(NULL);
    QVERIFY( mEvent->contactLocalId() == 0 );
    QVERIFY( !mEvent->logsEventData() );
}

void UT_LogsEvent::testUpdateRemotePartyFromContacts()
{
    // No search term
    unsigned int testContactId = 99;
    unsigned int testNoContactId = 0;
    QtContactsStubsHelper::setContactId( testContactId );
    QContactManager manager;
    LogsEvent event;
    QString remoteParty;   
    QVERIFY( !event.updateRemotePartyFromContacts(manager, remoteParty) );
    QVERIFY( remoteParty.isEmpty() );
    QVERIFY( event.remoteParty().length() == 0 );
    QCOMPARE( event.contactLocalId(), testNoContactId );
    
    // Number as search term, no match
    event.setNumber("12345");
    QVERIFY( !event.updateRemotePartyFromContacts(manager, remoteParty) );
    QVERIFY( remoteParty.isEmpty() );
    QVERIFY( event.remoteParty().length() == 0 );
    QCOMPARE( event.contactLocalId(), testNoContactId );
    
    // Number as search term, match
    QtContactsStubsHelper::setContactNames("first", "last");
    event.setNumber("11112222");
    QString newRemoteParty;
    QVERIFY( event.updateRemotePartyFromContacts(manager, newRemoteParty) );
    QVERIFY( !newRemoteParty.isEmpty() );
    QVERIFY( newRemoteParty == event.remoteParty() );
    QCOMPARE( event.contactLocalId(), testContactId );
    
    // Even if contact is unnamed, contact id is stored
    LogsEvent eventNoContactName;
    eventNoContactName.setNumber("11112222");
    QtContactsStubsHelper::setContactNames("", "");
    newRemoteParty.clear();
    QVERIFY( eventNoContactName.updateRemotePartyFromContacts(manager, newRemoteParty) );
    QVERIFY( newRemoteParty.isEmpty() );
    QVERIFY( newRemoteParty == eventNoContactName.remoteParty() );
    QCOMPARE( eventNoContactName.contactLocalId(), testContactId );
    
    // Voip address as search term, no match
    QtContactsStubsHelper::setContactNames("first", "last");
    LogsEvent event2;
    LogsEventData* eventData = new LogsEventData;
    eventData->mRemoteUrl = "1.2.3.4";
    event2.setLogsEventData(eventData);
    event2.setEventType(LogsEvent::TypeVoIPCall);
    QVERIFY( !event2.updateRemotePartyFromContacts(manager, remoteParty) );
    QVERIFY( remoteParty.isEmpty() );
    QVERIFY( event2.remoteParty().length() == 0 );
    QCOMPARE( event2.contactLocalId(), testNoContactId );
    
    // Voip address as search term, match
    eventData->mRemoteUrl = "11112222";
    newRemoteParty.clear();
    QVERIFY( event2.updateRemotePartyFromContacts(manager, newRemoteParty) );
    QVERIFY( !newRemoteParty.isEmpty() );
    QVERIFY( newRemoteParty == event.remoteParty() );
    QCOMPARE( event2.contactLocalId(), testContactId );
}

void UT_LogsEvent::testParseContactName()
{
    LogsEvent event;
    QtContactsStubsHelper::reset();
    QContactName name;
    QtContactsStubsHelper::setContactNames("firstname", "");
    QCOMPARE(event.parseContactName(name), QString("firstname"));
    
    QtContactsStubsHelper::setContactNames("firstname", "lastname");
    QCOMPARE(event.parseContactName(name), QString("firstname lastname"));
    
    QtContactsStubsHelper::setContactNames("", "lastname");
    QCOMPARE(event.parseContactName(name), QString("lastname"));
    
    QtContactsStubsHelper::setContactNames("", "");
    QCOMPARE(event.parseContactName(name), QString(""));
}

void UT_LogsEvent::testSerialization()
{
    // Serialize/deserialize, no event data
    LogsEvent event;
    event.mLogId = 3;        
    event.mDirection = LogsEvent::DirMissed;
    event.mEventType = LogsEvent::TypeVideoCall;
    event.mUid = 222222;
    event.mLogsEventData = 0;
    event.mRemoteParty = "remotePart";
    event.mNumber = "33333";
    event.mDuplicates = 2;                 
    event.mRingDuration = 3;
    event.mIsRead = true;  
    event.mIsALS = false;
    event.mDuration = 30;
    event.mIndex = 3;
    event.mIsInView = true;
    event.mEventState = LogsEvent::EventAdded;
    event.mIsLocallySeen = true;
    event.mIsPrivate = false;
    event.mIsUnknown = false;
    
    QByteArray serializedEvent;
    QDataStream stream(&serializedEvent, QIODevice::WriteOnly | QIODevice::Append);
    event.serialize(stream);
    
    QDataStream readStream(&serializedEvent, QIODevice::ReadOnly);
    
    LogsEvent deserializedEvent(readStream);
    QVERIFY( deserializedEvent.mLogId == 3 );        
    QVERIFY( deserializedEvent.mDirection == LogsEvent::DirMissed );
    QVERIFY( deserializedEvent.mEventType == LogsEvent::TypeVideoCall );
    QVERIFY( deserializedEvent.mUid == 222222 );
    QVERIFY( deserializedEvent.mLogsEventData == 0 );
    QVERIFY( deserializedEvent.mRemoteParty == "remotePart" );
    QVERIFY( deserializedEvent.mNumber == "33333" );
    QVERIFY( deserializedEvent.mDuplicates == 2 );                 
    QVERIFY( deserializedEvent.mRingDuration == 3 );
    QVERIFY( deserializedEvent.mIsRead == true );  
    QVERIFY( deserializedEvent.mIsALS == false );
    QVERIFY( deserializedEvent.mDuration == 30 );
    QVERIFY( deserializedEvent.mIndex == 3 );
    QVERIFY( deserializedEvent.mIsInView == true );
    QVERIFY( deserializedEvent.mEventState == LogsEvent::EventAdded );
    QVERIFY( deserializedEvent.mIsLocallySeen == true );
    QVERIFY( deserializedEvent.mIsPrivate == false );
    QVERIFY( deserializedEvent.mIsUnknown == false );
    QVERIFY( deserializedEvent.mMergedDuplicates.count() == 0 );
    
    // Serialize/deserialize, event data exists
    event.mLogsEventData = new LogsEventData();
    
    QByteArray serializedEvent2;
    QDataStream stream2(&serializedEvent2, QIODevice::WriteOnly | QIODevice::Append);
    LogsEvent merge1;
    merge1.setLogId(33);
    event.mMergedDuplicates.append( merge1 );
    LogsEvent merge2;
    merge2.setLogId(34);
    event.mMergedDuplicates.append( merge2 );
    event.serialize(stream2);  
    QDataStream readStream2(&serializedEvent2, QIODevice::ReadOnly);      
    LogsEvent deserializedEvent2(readStream2);
    QVERIFY( deserializedEvent2.mLogId == 3 );        
    QVERIFY( deserializedEvent2.mDirection == LogsEvent::DirMissed );
    QVERIFY( deserializedEvent2.mEventType == LogsEvent::TypeVideoCall );
    QVERIFY( deserializedEvent2.mUid == 222222 );
    QVERIFY( deserializedEvent2.mLogsEventData != 0 );
    QVERIFY( deserializedEvent2.mRemoteParty == "remotePart" );
    QVERIFY( deserializedEvent2.mNumber == "33333" );
    QVERIFY( deserializedEvent2.mDuplicates == 2 );                 
    QVERIFY( deserializedEvent2.mRingDuration == 3 );
    QVERIFY( deserializedEvent2.mIsRead == true );  
    QVERIFY( deserializedEvent2.mIsALS == false );
    QVERIFY( deserializedEvent2.mDuration == 30 );
    QVERIFY( deserializedEvent2.mIndex == 3 );
    QVERIFY( deserializedEvent2.mIsInView == true );
    QVERIFY( deserializedEvent2.mEventState == LogsEvent::EventAdded );
    QVERIFY( deserializedEvent2.mIsLocallySeen == true );
    QVERIFY( deserializedEvent2.mIsPrivate == false );
    QVERIFY( deserializedEvent2.mIsUnknown == false );
    QVERIFY( deserializedEvent2.mMergedDuplicates.count() == 2 );
    QVERIFY( deserializedEvent2.mMergedDuplicates.at(0).logId() == 33 );
    QVERIFY( deserializedEvent2.mMergedDuplicates.at(1).logId() == 34 );
    
    // Incorrect stream
    QByteArray serializedEvent3;
    QDataStream stream3(&serializedEvent3, QIODevice::ReadWrite | QIODevice::Append);
    LogsEvent deserializedEvent3(stream3);
    QVERIFY( deserializedEvent3.mLogId == 0 );        
    QVERIFY( deserializedEvent3.mDirection == LogsEvent::DirIn );
    QVERIFY( deserializedEvent3.mEventType == LogsEvent::TypeVoiceCall );
    QVERIFY( deserializedEvent3.mUid == 0 );
    QVERIFY( deserializedEvent3.mLogsEventData == 0 );
    QVERIFY( deserializedEvent3.mRemoteParty == "" );
    QVERIFY( deserializedEvent3.mNumber == "" );
    QVERIFY( deserializedEvent3.mDuplicates == 0 );                 
    QVERIFY( deserializedEvent3.mRingDuration == 0 );
    QVERIFY( deserializedEvent3.mIsRead == false );  
    QVERIFY( deserializedEvent3.mIsALS == false );
    QVERIFY( deserializedEvent3.mDuration == 0 );
    QVERIFY( deserializedEvent3.mIndex == 0 );
    QVERIFY( deserializedEvent3.mIsInView == false );
    QVERIFY( deserializedEvent3.mEventState == LogsEvent::EventAdded );
    QVERIFY( deserializedEvent3.mIsLocallySeen == false );
    QVERIFY( deserializedEvent3.mIsPrivate == false );
    QVERIFY( deserializedEvent3.mIsUnknown == false );
    QVERIFY( deserializedEvent3.mMergedDuplicates.count() == 0 );
}

void UT_LogsEvent::testMerge()
{
    LogsEvent ev;
    ev.setDirection(LogsEvent::DirOut);
    ev.setLogId(5);
    LogsEvent mergedEv;
    mergedEv.setDirection(LogsEvent::DirOut);
    mergedEv.setLogId(10);
    ev.merge(mergedEv);
    QCOMPARE( ev.mMergedDuplicates.count(), 1 );
    QCOMPARE( ev.mMergedDuplicates.at(0).logId(), 10 );
    
    // Adding again has no effect
    ev.merge(mergedEv);
    QCOMPARE( ev.mMergedDuplicates.count(), 1 );
    QCOMPARE( ev.mMergedDuplicates.at(0).logId(), 10 );
    
    // Merging unseen missed events
    ev.mMergedDuplicates.clear();
    ev.setDirection(LogsEvent::DirMissed);
    ev.setDuplicates(2);
    mergedEv.setDirection(LogsEvent::DirMissed);
    mergedEv.setDuplicates(0);
    ev.merge(mergedEv);
    QCOMPARE( ev.mMergedDuplicates.count(), 1 );
    QCOMPARE( ev.mMergedDuplicates.at(0).logId(), 10 );
    QCOMPARE( ev.duplicates(), 3 );
    
    // Simulate second reading round
    ev.setDuplicates(2);
    ev.merge(mergedEv);
    QCOMPARE( ev.duplicates(), 3 );
    
    // Merging seen events does not cause duplicate increase
    ev.markedAsSeenLocally(true);
    ev.setDuplicates(0);
    ev.merge(mergedEv);
    QCOMPARE( ev.mMergedDuplicates.count(), 1 );
    QCOMPARE( ev.mMergedDuplicates.at(0).logId(), 10 );
    QCOMPARE( ev.duplicates(), 0 );
}

