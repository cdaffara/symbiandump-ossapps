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
#include "ut_logseventparser.h"
#include "logseventparser.h"
#include "logsevent.h"
#include "logseventdata.h"
#include <logcli.h>
#include <LogsApiConsts.h>

#include <QtTest/QtTest>

_LIT( KTestUnknown, "Unknown" );

void UT_LogsEventParser::initTestCase()
{

}

void UT_LogsEventParser::cleanupTestCase()
{
}


void UT_LogsEventParser::init()
{
    mStrings.iUnKnownRemote = DESC_TO_QSTRING( KTestUnknown() ); 
    mEvent = new LogsEvent();
}

void UT_LogsEventParser::cleanup()
{
    delete mEvent;
    mEvent = 0;
}

void UT_LogsEventParser::testConstructor()
{
    QVERIFY( mEvent );
}

void UT_LogsEventParser::testInitializeEventL()
{
	CLogEvent* logEvent = CLogEvent::NewL();
	CleanupStack::PushL( logEvent );
	
	// Test number
	_LIT( eventTestNumber, "123444555" );
	QString strEventTestNumber = DESC_TO_QSTRING( eventTestNumber() );
	logEvent->SetNumber( eventTestNumber() );
	mEvent->initializeEventL(*logEvent, mStrings);
    QVERIFY( mEvent->mNumber == strEventTestNumber );
    
    // Test direction
    mStrings.iInDirection = "dir_in";
    logEvent->SetDirection( _L("dir_in") );
    mEvent->initializeEventL(*logEvent, mStrings);
    QVERIFY( mEvent->mDirection == LogsEvent::DirIn );
    
    mStrings.iOutDirection = "dir_out";
    logEvent->SetDirection( _L("dir_out") );
    mEvent->initializeEventL(*logEvent, mStrings);
    QVERIFY( mEvent->mDirection == LogsEvent::DirOut );
    
    mStrings.iMissedDirection = "dir_missed";
    logEvent->SetDirection( _L("dir_missed") );
    mEvent->initializeEventL(*logEvent, mStrings);
    QVERIFY( mEvent->mDirection == LogsEvent::DirMissed );
    
    logEvent->SetDirection( _L("not_known") );
    mEvent->initializeEventL(*logEvent, mStrings);
    QVERIFY( mEvent->mDirection == LogsEvent::DirUndefined );
    
    // Test event states
    //
    LogsEvent event;
    logEvent->SetId(100);
    
    // If no log id, event state is "added"
    event.initializeEventL(*logEvent, mStrings);
    QVERIFY( event.eventState() == LogsEvent::EventAdded );
    
    // If log id has changed, event state is "added"
    logEvent->SetId(101);
    event.initializeEventL(*logEvent, mStrings);
    QVERIFY( event.eventState() == LogsEvent::EventAdded );
    
    // If log id is same and data has changed, event state is "updated"
    _LIT( eventTestNumber2, "223444555" );
    logEvent->SetNumber( eventTestNumber2() );
    event.initializeEventL(*logEvent, mStrings);
    QVERIFY( event.eventState() == LogsEvent::EventUpdated );
    
    // If log id is same but no data has change, event state is "not updated"
    event.initializeEventL(*logEvent, mStrings);
    QVERIFY( event.eventState() == LogsEvent::EventNotUpdated );
    
    LogsEvent unknowEvent;
    logEvent->SetRemoteParty( KTestUnknown );
    logEvent->SetNumber( _L("") );
    unknowEvent.initializeEventL(*logEvent, mStrings);
    QVERIFY( unknowEvent.isRemotePartyUnknown() );
    QVERIFY( unknowEvent.number().isEmpty() );
    logEvent->SetNumber( _L("1234") );
    unknowEvent.initializeEventL(*logEvent, mStrings);
    QVERIFY( !unknowEvent.isRemotePartyUnknown() );
    QVERIFY( !unknowEvent.number().isEmpty() );
      
    LogsEvent privateEvent;
    logEvent->SetRemoteParty( KLogsPrivateText );
    logEvent->SetNumber( _L("") );
    privateEvent.initializeEventL(*logEvent, mStrings);
    QVERIFY( privateEvent.isRemotePartyPrivate() );
    QVERIFY( privateEvent.number().isEmpty() );
    
    LogsEvent payphoneEvent;
    logEvent->SetRemoteParty( KLogsPayphoneText );
    logEvent->SetNumber( _L("1112222") );
    payphoneEvent.initializeEventL(*logEvent, mStrings);
    QVERIFY( !payphoneEvent.isRemotePartyPrivate() );
    QVERIFY( !payphoneEvent.isRemotePartyUnknown() );
    QVERIFY( !payphoneEvent.number().isEmpty() );
    
    CleanupStack::PopAndDestroy( logEvent );
}

void UT_LogsEventParser::eventTypeSpecificParsing()
{
    // Voip event
    LogsEvent event;
    event.setEventType(LogsEvent::TypeVoIPCall);
    LogsEventData* eventData = new LogsEventData;
    eventData->mRemoteUrl = "jep@1.2.3.4";
    event.setLogsEventData(eventData);
    event.setRemoteParty("Jep");
    LogsEventParser::eventTypeSpecificParsing(event);
    QVERIFY( event.remoteParty() == "Jep" );

    // Voip event with unknown remote party, remote party is cleared
    event.setRemoteParty(eventData->mRemoteUrl);
    LogsEventParser::eventTypeSpecificParsing(event);
    QVERIFY( event.remoteParty().isEmpty() );
    
    // Voice call event, nop
    event.setEventType(LogsEvent::TypeVoIPCall);
    LogsEventParser::eventTypeSpecificParsing(event);
    QVERIFY( event.remoteParty().isEmpty() );
}

void UT_LogsEventParser::testResolveEventType()
{
    // No event data
    LogsEventParser::resolveEventType(*mEvent);
    QVERIFY( mEvent->eventType() == LogsEvent::TypeVoiceCall );
    
    // Event data, type not changed
    LogsEventData* eventData = new LogsEventData();
    mEvent->setLogsEventData(eventData);
    QVERIFY( !LogsEventParser::resolveEventType(*mEvent) );
    QVERIFY( mEvent->eventType() == LogsEvent::TypeVoiceCall );

    eventData->mIsVoIP = true;
    QVERIFY( LogsEventParser::resolveEventType(*mEvent) );
    QVERIFY( mEvent->eventType() == LogsEvent::TypeVoIPCall );
    
    eventData->mIsVoIP = false;
    eventData->mIsVT = true;
    QVERIFY( LogsEventParser::resolveEventType(*mEvent) );
    QVERIFY( mEvent->eventType() == LogsEvent::TypeVideoCall );
    
    // PoC not supported
    eventData->mIsVoIP = false;
    eventData->mIsVT = false;
    eventData->mIsPoC = true;
    QVERIFY( LogsEventParser::resolveEventType(*mEvent) );
    QVERIFY( mEvent->eventType() == LogsEvent::TypeUndefined );
    
    // Msgs not supported
    eventData->mIsVoIP = false;
    eventData->mIsVT = false;
    eventData->mIsPoC = false;
    eventData->mMsgPartsNumber = 2;
    QVERIFY( !LogsEventParser::resolveEventType(*mEvent) );
    QVERIFY( mEvent->eventType() == LogsEvent::TypeUndefined );
}
