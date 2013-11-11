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
#include "ut_logsforegroundwatcher.h"
#include "logsforegroundwatcher.h"

#include <QtTest/QtTest>
#include <coemain.h>
#include <apacmdln.h>

CCoeEnv* testEnv = 0;
bool testCommandlineExists = false;
bool testCommandLineCommandBg = false;
bool testCommandLineSetForProcess = false;

CCoeEnv* CCoeEnv::Static()
{
    if ( !testEnv ){
        testEnv = new CCoeEnv();
    }
    return testEnv;
}

CCoeEnv::CCoeEnv() : CActive(EPriorityNormal)
{
    
}

CCoeEnv::~CCoeEnv()
{
}

void CCoeEnv::AddForegroundObserverL(MCoeForegroundObserver& /*aForegroundObserver*/)
{
    
}

void CCoeEnv::RemoveForegroundObserver(MCoeForegroundObserver& /*aForegroundObserver*/)
{
    
}

TInt CApaCommandLine::GetCommandLineFromProcessEnvironment(CApaCommandLine*& aCommandLine)
{
    if ( testCommandlineExists ){
        aCommandLine = CApaCommandLine::NewL();
        if ( testCommandLineCommandBg ){
            TRAP_IGNORE( aCommandLine->SetCommandL(EApaCommandBackground) )
        } else {
            TRAP_IGNORE( aCommandLine->SetCommandL(EApaCommandRun) )
        }
    } else {
        aCommandLine = 0;
    }
    return KErrNone;
}

void CApaCommandLine::SetProcessEnvironmentL(RProcess& /*aProcess*/) const
{
    if ( Command() == EApaCommandBackground ){
        testCommandLineSetForProcess = true;
    }
}

void UT_LogsForegroundWatcher::initTestCase()
{
}

void UT_LogsForegroundWatcher::cleanupTestCase()
{
    delete testEnv;
    testEnv = 0;
}

void UT_LogsForegroundWatcher::init()
{
    mWatcher = new LogsForegroundWatcher();
}

void UT_LogsForegroundWatcher::cleanup()
{
    delete mWatcher;
}

void UT_LogsForegroundWatcher::testConstructor()
{
    QVERIFY( mWatcher );
}

void UT_LogsForegroundWatcher::testHandleLosingForeground()
{
    QSignalSpy spyGaining( mWatcher, SIGNAL(gainingForeground()) );
    QSignalSpy spyLosing( mWatcher, SIGNAL(losingForeground()) );
    mWatcher->HandleLosingForeground();
    QVERIFY( spyGaining.count() == 0 );
    QVERIFY( spyLosing.count() == 1 );
}

void UT_LogsForegroundWatcher::testHandleGainingForeground()
{
    QSignalSpy spyGaining( mWatcher, SIGNAL(gainingForeground()) );
    QSignalSpy spyLosing( mWatcher, SIGNAL(losingForeground()) );
    mWatcher->HandleGainingForeground();
    QVERIFY( spyGaining.count() == 1 );
    QVERIFY( spyLosing.count() == 0 );
}

void UT_LogsForegroundWatcher::testEnsureBackgroundStartup()
{
    // No commandline
    mWatcher->ensureBackgroundStartup();
    QVERIFY( !testCommandLineSetForProcess );
    
    // Command line already has bg command
    testCommandlineExists = true;
    testCommandLineCommandBg = true;
    mWatcher->ensureBackgroundStartup();
    QVERIFY( !testCommandLineSetForProcess );
    
    // Command line does not have bg command
    testCommandLineCommandBg = false;
    mWatcher->ensureBackgroundStartup();
    QVERIFY( testCommandLineSetForProcess );   
}

