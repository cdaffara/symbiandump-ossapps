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

//USER
#include "ut_logsapplication.h"
#include "logsapplication.h"
#include "logsappsettings.h"
#include "qthighway_stub_helper.h"
#include "hbstubs_helper.h"
//SYSTEM
#include <QtTest/QtTest>
#include <hbmainwindow.h>
#include <QKeyEvent>

void UT_LogsApplication::initTestCase()
{

}

void UT_LogsApplication::cleanupTestCase()
{

}


void UT_LogsApplication::init()
{
    char* argv = 0;
    int argc = 0;
    mSettings = new LogsAppSettings(argc, &argv);
    mLogsApplication = new LogsApplication(argc, &argv, *mSettings);
    
}

void UT_LogsApplication::cleanup()
{
    delete mLogsApplication;
    mLogsApplication = 0;
    delete mSettings;
    mSettings = 0;
}

void UT_LogsApplication::testConstructor()
{
    QVERIFY( mLogsApplication->mTestFlags == Hb::DefaultApplicationFlags );
    
    delete mLogsApplication;
    mLogsApplication = 0;
    mSettings->mFeaturePreloadedEnabled = true;
    char* argv = 0;
    int argc = 0;
    mLogsApplication = new LogsApplication(argc, &argv, *mSettings);
    QVERIFY( mLogsApplication->mTestFlags == Hb::NoSplash );
}

void UT_LogsApplication::testtestLogsAppEngineReady()
{
    QSignalSpy spy( mLogsApplication, SIGNAL(applicationReady()) );
    // Not handled
    QVERIFY( mLogsApplication->mReadCompleted == false );
    QVERIFY( mLogsApplication->mViewReady == false );
    mLogsApplication->testLogsAppEngineReady();
    QVERIFY( spy.count() == 0 );
    
    QVERIFY( mLogsApplication->mReadCompleted == true );
    QVERIFY( mLogsApplication->mViewReady == false );
    // Not handled
    mLogsApplication->testLogsAppEngineReady();
    QVERIFY( spy.count() == 0 );
    
    // Handled
    mLogsApplication->mReadCompleted = false;
    mLogsApplication->mViewReady = true;
    mLogsApplication->testLogsAppEngineReady();
    QVERIFY( spy.count() == 1 );
    
    spy.clear();
    mLogsApplication->mViewReady = false;
    mLogsApplication->mReadCompleted = false;
    mLogsApplication->testLogsAppEngineReady();
    QVERIFY( spy.count() == 0 );
        
    mLogsApplication->testLogsHandleAppViewReady();
    QVERIFY( spy.count() == 1 );
    
}

void UT_LogsApplication::testtestLogsHandleAppViewReady()
{
    QSignalSpy spy( mLogsApplication, SIGNAL(applicationReady()) );
    // Not handled
    QVERIFY( mLogsApplication->mReadCompleted == false );
    QVERIFY( mLogsApplication->mViewReady == false );
    mLogsApplication->testLogsHandleAppViewReady();
    QVERIFY( spy.count() == 0 );
    
    QVERIFY( mLogsApplication->mReadCompleted == false );
    QVERIFY( mLogsApplication->mViewReady == true );
    // Not handled
    mLogsApplication->testLogsHandleAppViewReady();
    QVERIFY( spy.count() == 0 );
    
    // Handled
    mLogsApplication->mViewReady = false;
    mLogsApplication->mReadCompleted = true;
    mLogsApplication->testLogsHandleAppViewReady();
    QVERIFY( spy.count() == 1 );  
    
    spy.clear();
    mLogsApplication->mViewReady = false;
    mLogsApplication->mReadCompleted = false;
    mLogsApplication->testLogsHandleAppViewReady();
    QVERIFY( spy.count() == 0 );
    
    mLogsApplication->testLogsAppEngineReady();
    QVERIFY( spy.count() == 1 );
}

void UT_LogsApplication::testtestLogsResetAppReady()
{
    mLogsApplication->mViewReady = true;
    mLogsApplication->mReadCompleted = true;
    mLogsApplication->testLogsResetAppReady();
    QVERIFY( mLogsApplication->mViewReady );
    QVERIFY( !mLogsApplication->mReadCompleted );
    
}

