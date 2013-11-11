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
#include <e32base.h>
#include <logeng.h>
#include <logsmspdudata.h>
#include <QtTest/QtTest>

#include "ut_logseventdataparser.h"
#include "logseventdataparser.h"
#include "logseventdata.h"
#include "logsapiconsts.h"


void UT_LogsEventDataParser::initTestCase()
{

}

void UT_LogsEventDataParser::cleanupTestCase()
{
}


void UT_LogsEventDataParser::init()
{
    mEvent = CLogEvent::NewL();
    mEventData = new LogsEventData();
}

void UT_LogsEventDataParser::cleanup()
{
    delete mEvent;
    mEvent = 0;
    delete mEventData;
    mEventData = 0;
}

void UT_LogsEventDataParser::testConstructor()
{
    QVERIFY( mEventData );
}

void UT_LogsEventDataParser::testParseMessageParts()
{
    TPckgBuf<TLogSmsPduData> packedData;
    packedData().iTotal = 2;
    mEvent->SetDataL(packedData);
    mEvent->SetEventType(KLogShortMessageEventTypeUid);
    QVERIFY( mEventData->parse(*mEvent) == 0 );
    QVERIFY( mEventData->msgPartsNumber() == 2 );
}

void UT_LogsEventDataParser::testParseNonTagged()
{
    // No data
    mEvent->SetDataL(KNullDesC8);
    QVERIFY( mEventData->parse(*mEvent) == 0 );
    QVERIFY( mEventData->mDataReceived == 0 );
    QVERIFY( mEventData->mDataSent == 0 );
    
    // No numeric in beginning
    mEvent->SetDataL(_L8("abcdef"));
    QVERIFY( mEventData->parse(*mEvent) == 0 );
    QVERIFY( mEventData->mDataReceived == 0 );
    QVERIFY( mEventData->mDataSent == 0 );
    
    // No separator
    mEvent->SetDataL(_L8("123"));
    QVERIFY( mEventData->parse(*mEvent) == 0 );
    QVERIFY( mEventData->mDataReceived == 0 );
    QVERIFY( mEventData->mDataSent == 0 );
    
    // No received part
    mEvent->SetDataL(_L8("123,"));
    QVERIFY( mEventData->parse(*mEvent) == 0 );
    QVERIFY( mEventData->mDataReceived == 0 );
    QVERIFY( mEventData->mDataSent == 123 );
    
    // Both parts
    mEvent->SetDataL(_L8("2234,5567889"));
    QVERIFY( mEventData->parse(*mEvent) == 0 );
    QVERIFY( mEventData->mDataReceived == 5567889 );
    QVERIFY( mEventData->mDataSent == 2234 );
}

void UT_LogsEventDataParser::testParseTagged()
{
    // No data
    mEvent->SetDataL(KNullDesC8);
    QVERIFY( mEventData->parse(*mEvent) == 0 );
    QVERIFY( !mEventData->isVT() );
    
    // No field delim, VT
    mEvent->SetDataL(KLogsDataFldTag_VT);
    QVERIFY( mEventData->parse(*mEvent) == 0 );
    QVERIFY( mEventData->isVT() );
    
    // Field name delim, CNAP
    TBuf8<100> testData;
    testData.Copy(KLogsDataFldTag_CNAP);
    testData.Append(KLogsDataFldNameDelimiter);
    mEvent->SetDataL(testData);
    QVERIFY( mEventData->parse(*mEvent) == 0 );
    QVERIFY( mEventData->isCNAP() );
    
    // Field name delim, value delim, emergency
    testData.Copy(KLogsDataFldTag_Emergency);
    testData.Append(KLogsDataFldValueDelimiter);
    testData.Append(_L8("dummy val"));
    testData.Append(KLogsDataFldNameDelimiter);
    mEvent->SetDataL(testData);
    QVERIFY( mEventData->parse(*mEvent) == 0 );
    QVERIFY( mEventData->isEmerg() );
    
    // Voip, contact link and service id
    testData.Copy(KLogsDataFldTag_IP);
    testData.Append(KLogsDataFldNameDelimiter);
    testData.Append(KLogsDataFldTag_ServiceId);
    testData.Append(KLogsDataFldValueDelimiter);
    testData.Append(_L8("2"));
    testData.Append(KLogsDataFldNameDelimiter);
    testData.Append(KLogsDataFldTag_ContactLink);
    testData.Append(KLogsDataFldValueDelimiter);
    testData.Append(_L8("1"));
    mEvent->SetDataL(testData);
    QVERIFY( mEventData->parse(*mEvent) == 0 );
    QVERIFY( mEventData->isVoIP() );
    QVERIFY( mEventData->contactLocalId() == 1 );
    QVERIFY( mEventData->serviceId() == 2 );
    
    // Voip and incorrect contact link and service id
    testData.Copy(KLogsDataFldTag_IP);
    testData.Append(KLogsDataFldNameDelimiter);
    testData.Append(KLogsDataFldTag_ServiceId);
    testData.Append(KLogsDataFldValueDelimiter);
    testData.Append(_L8("a"));
    testData.Append(KLogsDataFldNameDelimiter);
    testData.Append(KLogsDataFldTag_ContactLink);
    testData.Append(KLogsDataFldValueDelimiter);
    testData.Append(_L8("a"));
    mEvent->SetDataL(testData);
    QVERIFY( mEventData->parse(*mEvent) == 0 );
    QVERIFY( mEventData->isVoIP() );
    QVERIFY( mEventData->contactLocalId() == 0 );
    QVERIFY( mEventData->serviceId() == 0 );
    
    // Voip and remote url
    testData.Copy(KLogsDataFldTag_IP);
    testData.Append(KLogsDataFldNameDelimiter);
    testData.Append(KLogsDataFldTag_URL);
    testData.Append(KLogsDataFldValueDelimiter);
    testData.Append(_L8("test@1.2.3.4"));
    testData.Append(KLogsDataFldNameDelimiter);
    testData.Append(KLogsDataFldTag_MA);
    mEvent->SetDataL(testData);
    QVERIFY( mEventData->parse(*mEvent) == 0 );
    QVERIFY( mEventData->isVoIP() );
    QVERIFY( mEventData->remoteUrl() == "test@1.2.3.4" );
    QVERIFY( mEventData->localUrl().isEmpty() );
    
    // Voip and remote url and local url
    testData.Append(KLogsDataFldValueDelimiter);
    testData.Append(_L8("test2@222.23.3.4"));
    testData.Append(KLogsDataFldNameDelimiter);
    mEvent->SetDataL(testData);
    QVERIFY( mEventData->parse(*mEvent) == 0 );
    QVERIFY( mEventData->isVoIP() );
    QVERIFY( mEventData->remoteUrl() == "test@1.2.3.4" );
    QVERIFY( mEventData->localUrl() == "test2@222.23.3.4"  );
    
    // PoC
    testData.Copy(KLogsDataFldTag_POC);
    mEvent->SetDataL(testData);
    QVERIFY( mEventData->parse(*mEvent) == 0 );
    QVERIFY( mEventData->isPoC() );
   
}
