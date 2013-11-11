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

#include <xqaiwdeclplat.h>
#include <QtTest/QtTest>

#include "test_calenviewerservice.h"

TestCalenViewerService::TestCalenViewerService()
{
    // No implementation yet
}

TestCalenViewerService::~TestCalenViewerService()
{
    // No implementation yet
}

void TestCalenViewerService::init()
{
    // No implementation yet
}

void TestCalenViewerService::cleanup()
{
    // No implementation yet
}

void TestCalenViewerService::testOpeningVCal()
{
    QTest::qWait(2);
    
    QFile file("z:\\data\\others\\meeting.vcs");

    XQApplicationManager mAppMgr;
    XQAiwRequest *request;
    QVERIFY(request = mAppMgr.create(file));
    
    // Set function parameters
    QList<QVariant> args;
    args << file.fileName();
    request->setArguments(args);
    request->setEmbedded(true);

    // Send the request
    bool res;
    QVERIFY(res = request->send());
    
    delete request;
}

void TestCalenViewerService::testOpeningICal()
{
    // TODO: Both the test cases cannot be run in sequence
    // due to some issues in Qt/Highway framework. Need to
    // re-check on newer version and then modify the test case
    // if required
    
    /*QTest::qWait(2);
    
    QFile file("z:\\data\\others\\meeting.ics");

    XQApplicationManager mAppMgr;
    XQAiwRequest *request;
    QVERIFY(request = mAppMgr.create(file));
    
    // Set function parameters
    QList<QVariant> args;
    args << file.fileName();
    request->setArguments(args);
    request->setEmbedded(true);

    // Send the request
    bool res;
    QVERIFY(res = request->send());
    
    delete request;*/
}
