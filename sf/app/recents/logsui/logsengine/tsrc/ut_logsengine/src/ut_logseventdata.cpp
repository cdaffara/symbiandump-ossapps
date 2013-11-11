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
#include <logeng.h>
#include <QtTest/QtTest>

#include "ut_logseventdata.h"
#include "logseventdata.h"
#include "logsapiconsts.h"


void UT_LogsEventData::initTestCase()
{

}

void UT_LogsEventData::cleanupTestCase()
{
}


void UT_LogsEventData::init()
{
    mEventData = new LogsEventData();
}

void UT_LogsEventData::cleanup()
{
    delete mEventData;
    mEventData = 0;
}

void UT_LogsEventData::testConstructor()
{
    QVERIFY( mEventData );
}

void UT_LogsEventData::testParse()
{
   
}

void UT_LogsEventData::testSetters()
{
   
}

void UT_LogsEventData::testGetters()
{
    QVERIFY( mEventData->remoteUrl().isEmpty() );
    QString remote( "te@1.2.3.4" );
    mEventData->mRemoteUrl = remote;
    QVERIFY( mEventData->remoteUrl() == remote );
    
    QVERIFY( mEventData->localUrl().isEmpty() );
    QString local( "tetetetet@2.2.3.4" );
    mEventData->mLocalUrl = local;
    QVERIFY( mEventData->localUrl() == local );
}

void UT_LogsEventData::testIsCsCompatible()
{
    // No url, compatible
    QVERIFY( mEventData->isCsCompatible() );
    
    // Url has only digits, compatible
    mEventData->mRemoteUrl = "1234555662";
    QVERIFY( mEventData->isCsCompatible() );
    
    // Url has only digits and + at beginning, compatible
    mEventData->mRemoteUrl = "+00634555662";
    QVERIFY( mEventData->isCsCompatible() );
    
    // Url has non-digit, not compatible
    mEventData->mRemoteUrl = "006345+55662";
    QVERIFY( !mEventData->isCsCompatible() );
    
    mEventData->mRemoteUrl = "006345@55662";
    QVERIFY( !mEventData->isCsCompatible() );
    
    mEventData->mRemoteUrl = "test";
    QVERIFY( !mEventData->isCsCompatible() );
    
    mEventData->mRemoteUrl = "test@1.2.3.4";
    QVERIFY( !mEventData->isCsCompatible() );
}

void UT_LogsEventData::testSerialization()
{
    
}
