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
#include "ut_logscall.h"
#include "logscall.h"
#include "logsmodel.h"
#include "logsevent.h"
#include "logseventdata.h"
#include "qthighway_stub_helper.h"
#include "hbstubs_helper.h"

#include <QtTest/QtTest>

const char logsICallDial[] = "phoneui.com.nokia.symbian.ICallDial";

void UT_LogsCall::initTestCase()
{
}

void UT_LogsCall::cleanupTestCase()
{
}


void UT_LogsCall::init()
{
    mLogsEvent = new LogsEvent();
    mLogsEvent->setNumber(QString::number(12345));
    mLogsEvent->setEventType(LogsEvent::TypeVoiceCall);
    mLogsCall = new LogsCall(*mLogsEvent);
}

void UT_LogsCall::cleanup()
{
    delete mLogsCall;
    mLogsCall = 0;
    delete mLogsEvent;
    mLogsEvent = 0;
}

void UT_LogsCall::testConstructor()
{
    QVERIFY( mLogsCall );
    QVERIFY( mLogsCall->mDefaultCall == LogsCall::TypeLogsVoiceCall );
    QVERIFY( mLogsCall->mNumber == QString::number(12345) );
    QCOMPARE( mLogsCall->mContactId, (unsigned int)0 );
    
    LogsCall callWithContact(2, "2222" );
    QVERIFY( callWithContact.mDefaultCall == LogsCall::TypeLogsVoiceCall );
    QVERIFY( callWithContact.mNumber == "2222" );
    QCOMPARE( callWithContact.mContactId, (unsigned int)2 );
    
    LogsCall callWithContact2(2, "user@server.com" );
    QVERIFY( callWithContact2.mDefaultCall == LogsCall::TypeLogsVoiceCall );
    QVERIFY( callWithContact2.mNumber == "user@server.com" );
    QCOMPARE( callWithContact2.mContactId, (unsigned int)2 );
    
    LogsCall callWithContact3(3, "" );
    QVERIFY( callWithContact3.mDefaultCall == LogsCall::TypeLogsCallNotAvailable );
    QVERIFY( callWithContact3.mNumber.isEmpty() );
    QCOMPARE( callWithContact3.mContactId, (unsigned int)3 );
}

void UT_LogsCall::testallowedCallTypes()
{
    LogsEvent event;
    event.setNumber(QString::number(12345));
    event.setEventType(LogsEvent::TypeVoiceCall);
    LogsCall call1(event);
	QVERIFY(call1.allowedCallTypes().count() == 2);
	
	event.setEventType(LogsEvent::TypeVideoCall);
	LogsCall call2(event);
    QVERIFY(call2.allowedCallTypes().count() == 2);
    
    // CS compatible Voip event
    event.setEventType(LogsEvent::TypeVoIPCall);
    LogsEventData* eventData = new LogsEventData;
    eventData->mRemoteUrl = "23456667";
    event.setLogsEventData( eventData );
    LogsCall call3(event);
    QVERIFY(call3.allowedCallTypes().count() == 3);
    
    // Not CS compatible Voip event
    LogsEventData* eventData2 = new LogsEventData;
    eventData2->mRemoteUrl = "test@1.2.3.4";
    event.setLogsEventData( eventData2 );
    LogsCall call4(event);
    QVERIFY(call4.allowedCallTypes().count() == 1);
    
    // Event without number
    LogsEvent event2;
    event2.setEventType(LogsEvent::TypeVoiceCall);
    LogsCall call5(event2);
    QVERIFY(call5.allowedCallTypes().count() == 0);
    QVERIFY(call5.mDefaultCall == LogsCall::TypeLogsCallNotAvailable);
}

void UT_LogsCall::testisAllowedCallType()
{
	QVERIFY(mLogsCall->isAllowedCallType());
	delete mLogsCall;
	mLogsCall = 0;
	mLogsEvent->setEventType(LogsEvent::TypeVoIPCall);
	mLogsCall = new LogsCall(*mLogsEvent);
	QVERIFY(mLogsCall->isAllowedCallType());
	
	// Calling with voip event possible if having remote tel uri
	delete mLogsCall;
    mLogsCall = 0;
    mLogsEvent->setEventType(LogsEvent::TypeVoIPCall);
	LogsEventData* eventData = new LogsEventData;
    eventData->mRemoteUrl = "+22233344555";
    mLogsEvent->setLogsEventData( eventData );
    mLogsCall = new LogsCall(*mLogsEvent);
	QVERIFY(mLogsCall->isAllowedCallType());
}

void UT_LogsCall::testCall()
{  
    QtHighwayStubHelper::reset();
    mLogsCall->call(LogsCall::TypeLogsVoiceCall);
    QVERIFY( QtHighwayStubHelper::service() == logsICallDial );
    QVERIFY( QtHighwayStubHelper::message() == "dial(QString)" );
    QVERIFY( QtHighwayStubHelper::isRequestBg() );
    
    QtHighwayStubHelper::reset();
    mLogsCall->call(LogsCall::TypeLogsVideoCall);
    QVERIFY( QtHighwayStubHelper::service() == logsICallDial );
    QVERIFY( QtHighwayStubHelper::message() == "dialVideo(QString)" );
    QVERIFY( QtHighwayStubHelper::isRequestBg() );

    QtHighwayStubHelper::reset();
    mLogsCall->mServiceId = 3;
    mLogsCall->call(LogsCall::TypeLogsVoIPCall);
    QVERIFY( QtHighwayStubHelper::service() == logsICallDial );
    QVERIFY( QtHighwayStubHelper::message() == "dialVoipService(QString,int)" ); 
    QVERIFY( QtHighwayStubHelper::isRequestBg() );
    
    // Not supported calltype
    QtHighwayStubHelper::reset();
    mLogsCall->call(static_cast<LogsCall::CallType>(9999));
    QVERIFY( QtHighwayStubHelper::service().isEmpty() );
    QVERIFY( QtHighwayStubHelper::message().isEmpty() );
    QVERIFY( !QtHighwayStubHelper::isRequestBg() );
    
    // No number, call is anyway tried so that phone shows error note
    QtHighwayStubHelper::reset();
    HbStubHelper::reset();
    mLogsCall->mNumber.clear();
    mLogsCall->call(LogsCall::TypeLogsVoiceCall);
    QVERIFY( QtHighwayStubHelper::service() == logsICallDial );
    QVERIFY( QtHighwayStubHelper::message() == "dial(QString)" );
    QVERIFY( QtHighwayStubHelper::isRequestBg() );
    QVERIFY( HbStubHelper::notificationDialogTxt().isEmpty() );
    
    // No number for contact call, specific error note is shown
    QtHighwayStubHelper::reset();
    HbStubHelper::reset();
    mLogsCall->mContactId = 5;
    mLogsCall->call(LogsCall::TypeLogsVoiceCall);
    QVERIFY( QtHighwayStubHelper::service().isEmpty() );
    QVERIFY( QtHighwayStubHelper::message().isEmpty() );
    QVERIFY( HbStubHelper::notificationDialogTxt() == hbTrId("txt_dial_dpopinfo_no_saved_number_for_this_contact") );
}

void UT_LogsCall::testInitiateCallback()
{
    QtHighwayStubHelper::reset();
    mLogsCall->initiateCallback();
    QVERIFY( QtHighwayStubHelper::service() == logsICallDial );
    QVERIFY( QtHighwayStubHelper::message() == "dial(QString)" );
    QVERIFY( QtHighwayStubHelper::isRequestBg() );
    
    mLogsCall->mDefaultCall = LogsCall::TypeLogsVideoCall;
    QtHighwayStubHelper::reset();
    mLogsCall->initiateCallback();
    QVERIFY( QtHighwayStubHelper::service() == logsICallDial );
    QVERIFY( QtHighwayStubHelper::message() == "dialVideo(QString)" );
    QVERIFY( QtHighwayStubHelper::isRequestBg() );
    
    mLogsCall->mDefaultCall = LogsCall::TypeLogsVoIPCall;
    QtHighwayStubHelper::reset();
    mLogsCall->mServiceId = 3;
    mLogsCall->initiateCallback();
    QVERIFY( QtHighwayStubHelper::service() == logsICallDial );
    QVERIFY( QtHighwayStubHelper::message() == "dialVoipService(QString,int)" );  
    QVERIFY( QtHighwayStubHelper::isRequestBg() );
}
