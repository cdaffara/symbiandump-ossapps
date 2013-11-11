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
#include "at_logsengine.h"
#include <logsmodel.h>
#include <logsevent.h>
#include <logsfilter.h>
#include <logscustomfilter.h>
#include <apacmdln.h>
#include <apgtask.h>
#include <apgcli.h>
#include <hbicon.h>

#include <QtTest/QtTest>

_LIT( KLogTestEvents, "at_logsengine" );
_LIT( KLogTestClientWriter, "logclienttest.exe" );
_LIT( KLogTestClientClearer, "clearlisttest.exe" );

const int logsTestAllEventsCount = 20;
const int logsTestMissedCallsCount = 10;
const int logsTestOtherThanMissedEventsCount = 10;
const int logsTestContact2EventsCount = 2;
const int logsTestContact3EventsCount = 3;

// Note, events are written from .dat file in order where last event is going to
// be first event at db
const QString logsTestAllEvent1Name = "deeppu";
const LogsEvent::LogsDirection logsTestAllEvent1Dir = LogsEvent::DirIn;

const QString logsTestContact2Event2Name = "Jeppis";
const LogsEvent::LogsDirection logsTestContact2Event2Dir = LogsEvent::DirIn;
const QString logsTestContact2Event1Name = "Jeppa";
const LogsEvent::LogsDirection logsTestContact2Event1Dir = LogsEvent::DirMissed;

const QString logsTestContact3Event3Name = "aeppu";
const LogsEvent::LogsDirection logsTestContact3Event3Dir = LogsEvent::DirOut;
const QString logsTestContact3Event2Name = "beppu";
const LogsEvent::LogsDirection logsTestContact3Event2Dir = LogsEvent::DirIn;
const QString logsTestContact3Event1Name = "deppu";
const LogsEvent::LogsDirection logsTestContact3Event1Dir = LogsEvent::DirOut;

const QString logsTestHomeScreenMissedCallerName = "Jeppa";

Q_DECLARE_METATYPE(LogsEvent *)

#define AT_LOGSENG_VERIFY(m, idx, remote, dir) { \
LogsEvent* event = qVariantValue<LogsEvent*>( m.data( m.index(idx, 0), LogsModel::RoleFullEvent ) ); \
QVERIFY( event && event->remoteParty() == remote && event->direction() == dir ); }

void AT_LogsEngine::initTestCase()
{
    TRAPD( err, clearEventsL() );
    Q_ASSERT( err == KErrNone );
    QTest::qWait(2000);
    TRAP( err, writeEventsL() );
    Q_ASSERT( err == KErrNone );
    QTest::qWait(2000);
    mModel = new LogsModel(LogsModel::LogsFullModel);
    QTest::qWait(5000); // wait model to be populated from db
}

void AT_LogsEngine::cleanupTestCase()
{
    delete mModel;
    mModel = 0;
}


void AT_LogsEngine::init()
{
}

void AT_LogsEngine::cleanup()
{

}

void AT_LogsEngine::testAllEvents()
{
    QVERIFY( mModel->rowCount(QModelIndex()) == logsTestAllEventsCount );
    AT_LOGSENG_VERIFY( (*mModel), 0, logsTestAllEvent1Name, logsTestAllEvent1Dir )
}

void AT_LogsEngine::testFilteredEvents()
{
    LogsCustomFilter filter;
    filter.setContactId(2);
    filter.setSourceModel(mModel);
    QVERIFY( filter.rowCount() == logsTestContact2EventsCount );
    AT_LOGSENG_VERIFY( filter, 0, logsTestContact2Event1Name, logsTestContact2Event1Dir )
    AT_LOGSENG_VERIFY( filter, 1, logsTestContact2Event2Name, logsTestContact2Event2Dir )
    
    filter.setContactId(3);
    filter.setSourceModel(0);
    filter.setSourceModel(mModel);
    QVERIFY( filter.rowCount() == logsTestContact3EventsCount );
    AT_LOGSENG_VERIFY( filter, 0, logsTestContact3Event1Name, logsTestContact3Event1Dir )
    AT_LOGSENG_VERIFY( filter, 1, logsTestContact3Event2Name, logsTestContact3Event2Dir )
    AT_LOGSENG_VERIFY( filter, 2, logsTestContact3Event3Name, logsTestContact3Event3Dir )
    
    filter.setContactId(10345624);
    filter.setSourceModel(0);
    filter.setSourceModel(mModel);
    QVERIFY( filter.rowCount() == 0 ); 
}

void AT_LogsEngine::testMarkEventsSeen()
{
    LogsCustomFilter filter;
    filter.setContactId(2);
    filter.setSourceModel(mModel);
    int unseenCount = 0;
    QVERIFY( filter.rowCount() == 2 );
    for ( int i = 0; i < filter.rowCount(); i++ ){
        LogsEvent* event = qVariantValue<LogsEvent*>( 
                filter.data( filter.index(i, 0), LogsModel::RoleFullEvent ) );
        if ( event && !event->isRead() ){
            unseenCount++;
        }
    }
    QVERIFY( unseenCount == 2 );
    QSignalSpy spy( &filter, SIGNAL( markingCompleted(int)));
    QVERIFY( filter.markEventsSeen() );
    QTest::qWait(1000); // wait marking completion
   
    QVERIFY( spy.count() == 1 );
    int seenCount = 0;
    for ( int i = 0; i < filter.rowCount(); i++ ){
        LogsEvent* event = qVariantValue<LogsEvent*>( 
                filter.data( filter.index(i, 0), LogsModel::RoleFullEvent ) );
        if ( event && event->isRead() ){
            seenCount++;
        }
    }
    QVERIFY( seenCount == 2 );
    
}

void AT_LogsEngine::testClearEvents()
{
    LogsCustomFilter filter;
    filter.setContactId(2);
    filter.setSourceModel(mModel);
    QVERIFY( filter.rowCount() == 2 );
    int unseenCount = 0;
    QSignalSpy spy( &filter, SIGNAL( clearingCompleted(int)));
    QVERIFY( filter.clearEvents() );
    QTest::qWait(1000); // wait clearing completion
    QVERIFY( filter.rowCount() == 0 );
    QVERIFY( spy.count() == 1 );
}

void AT_LogsEngine::testHomeScreenUsecase()
{
    // Homescreen missed calls notification plugin uses full model with
    // missed calls filter to find out how many sequential events was
    // received from one caller. If number of them is the same as missed call
    // counter, name of the caller is shown at HS.
    
    
    LogsFilter filter(LogsFilter::Missed);
    const int maxNumMissedCalls = 2;
    filter.setMaxSize(maxNumMissedCalls);
    LogsModel optimizedModel(LogsModel::LogsFullModel);   
    QTest::qWait(5000); // wait model to be populated from db
    filter.setSourceModel(&optimizedModel);
    
    // Test data has 10 missed calls from Jeppa but readsize has been configured to 2 so
    // reading stops when 2 missed calls has been read
    QCOMPARE( filter.rowCount(), maxNumMissedCalls );
    AT_LOGSENG_VERIFY( filter, 0, logsTestHomeScreenMissedCallerName, LogsEvent::DirMissed )
    AT_LOGSENG_VERIFY( filter, 1, logsTestHomeScreenMissedCallerName, LogsEvent::DirMissed )
    
    // Filter assigned to full model without read size has all those 10 missed calls
    LogsFilter filter2(LogsFilter::Missed);
    filter2.setSourceModel(mModel);
    QVERIFY( filter2.rowCount() == logsTestMissedCallsCount );
    
    // Verify decoration and display data
    QStringList displayData = filter.data( filter.index(0, 0), Qt::DisplayRole ).toStringList();
    QVERIFY( displayData.count() == 2 );
    QVERIFY( displayData.at(0) == logsTestHomeScreenMissedCallerName );
    HbIcon icon = qVariantValue<HbIcon>( filter.data( filter.index(0, 0), Qt::DecorationRole ).toList().at(0) );
    QVERIFY( !icon.isNull() );
    
    // Cenrep missed calls counter is not tested here
}

void AT_LogsEngine::clearEventsL()
{
    executeL(KLogTestClientClearer, KNullDesC);
}

void AT_LogsEngine::writeEventsL()
{
    TBuf<100> commandLine;
    // Run and write events named at_logsengine in the .dat
    commandLine.Append( KLogTestEvents );
    executeL(KLogTestClientWriter, commandLine);
}

void AT_LogsEngine::executeL(const TDesC& exeName, const TDesC& commandLine)
{
    RProcess process;
    CleanupClosePushL( process );
    User::LeaveIfError( process.Create( exeName, commandLine ) );
    process.Resume();
    CleanupStack::PopAndDestroy( &process );
}
