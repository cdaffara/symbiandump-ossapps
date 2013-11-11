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
#include "ut_logsservicehandler.h"
#include "logsservicehandler.h"
#include "logsservicehandlerold.h"
#include "qthighway_stub_helper.h"
#include "ut_logsviewmanager.h"
#include "logsviewmanager.h"
#include "logsmainwindow.h"
#include "logscomponentrepository.h"
#include "logsmatchesview.h"
#include "logsdefs.h"
#include "logsappsettings.h"
#include <xqaiwdecl.h>

//SYSTEM
#include <QtTest/QtTest>

Q_DECLARE_METATYPE(XQService::LogsViewIndex)

void UT_LogsServiceHandler::initTestCase()
{
}

void UT_LogsServiceHandler::cleanupTestCase()
{
}


void UT_LogsServiceHandler::init()
{
    mMainWindow =  new LogsMainWindow();
    mService = new LogsServiceHandler();
    mServiceOld = new LogsServiceHandlerOld();
    char* argv = 0;
    int argc = 0;
    mSettings = new LogsAppSettings(argc, &argv);
    mLogsViewManager = new LogsViewManager(*mMainWindow, *mService, *mServiceOld, *mSettings);
    
}

void UT_LogsServiceHandler::cleanup()
{
    delete mService;
    mService = 0;
    delete mServiceOld;
    mServiceOld = 0;
    delete mMainWindow;
    mMainWindow = 0;
    delete mLogsViewManager;
    mLogsViewManager = 0;
    delete mSettings;
    mSettings = 0;
    
    QtHighwayStubHelper::reset();
}

void UT_LogsServiceHandler::testConstructor()
{
    QVERIFY( mService );
    QVERIFY( !mService->mIsAppStartedUsingService );
    QVERIFY( !mService->isStartedUsingService() );

    QVERIFY( mServiceOld );
    QVERIFY( !mServiceOld->mIsAppStartedUsingService );
    QVERIFY( !mServiceOld->isStartedUsingService() );

    
    QtHighwayStubHelper::setIsService(true);
    LogsServiceHandler serviceHandler;
    QVERIFY( serviceHandler.mIsAppStartedUsingService );
    QVERIFY( serviceHandler.isStartedUsingService() );
    
    LogsServiceHandlerOld serviceHandlerOld;
    QVERIFY( serviceHandlerOld.mIsAppStartedUsingService );
    QVERIFY( serviceHandlerOld.isStartedUsingService() );
}

void UT_LogsServiceHandler::testStart()
{
    qRegisterMetaType< XQService::LogsViewIndex >("XQService::LogsViewIndex");
    QSignalSpy spy(mServiceOld, SIGNAL(activateView(XQService::LogsViewIndex, bool, QString)));
     
    // Wrong view
    QVERIFY( mServiceOld->start( 9999, true ) != 0 );
    QVERIFY( spy.count() == 0 );

    // Correct view
    QVERIFY( mServiceOld->start( (int)XQService::LogsViewReceived, true  ) == 0 );
    QVERIFY( spy.count() == 1 );
    XQService::LogsViewIndex view = 
        qvariant_cast< XQService::LogsViewIndex >(spy.at(0).at(0));
    QVERIFY( view == XQService::LogsViewReceived );
}

void UT_LogsServiceHandler::testStartWithNum()
{
    qRegisterMetaType< XQService::LogsViewIndex >("XQService::LogsViewIndex");
    QSignalSpy spy2(mServiceOld, SIGNAL(activateView(QString)));

    QVERIFY( mServiceOld->startWithNum( (int)XQService::LogsViewReceived, true,
            QString("+123456")  ) == 0 );
    QVERIFY( spy2.count() == 1 );
    QVERIFY( spy2.at(0).at(0).toString() == QString("+123456"));
}

void UT_LogsServiceHandler::testShow()
{
    qRegisterMetaType< XQService::LogsViewIndex >("XQService::LogsViewIndex");
    QSignalSpy spy(mService, SIGNAL(activateView(XQService::LogsViewIndex, bool, QString)));
    QSignalSpy spy2(mService, SIGNAL(activateView(QString)));
    QVariantMap map;

    // Wrong view
    map.insert(logsViewIndexParam, QVariant(9999));
    QVERIFY( mService->show(map) != 0 );
    QVERIFY( spy.count() == 0 );
    QVERIFY( spy2.count() == 0 );
    
    // Correct view, dialpad text is empty
    map.clear();
    map.insert(logsViewIndexParam, QVariant((int)XQService::LogsViewReceived));
    map.insert(logsShowDialpadParam, QVariant(true));
    QVERIFY( mService->show(map) == 0 );
    QVERIFY( spy.count() == 1 );
    QVERIFY( spy2.count() == 0 );
    XQService::LogsViewIndex view = 
            qvariant_cast< XQService::LogsViewIndex >(spy.at(0).at(0));
    QCOMPARE( view, XQService::LogsViewReceived );
    QCOMPARE( spy.at(0).at(1).toBool(), true );
    QCOMPARE( spy.at(0).at(2).toString(), QString(""));
    
    // Correct view, dialpad text not empty
    spy.clear();
    map.clear();
    map.insert(logsViewIndexParam, QVariant((int)XQService::LogsViewReceived));
    map.insert(logsDialpadTextParam, QVariant(QString("+123456")));
    QVERIFY( mService->show(map) == 0 );
    QVERIFY( spy.count() == 0 );
    QVERIFY( spy2.count() == 1 );
    QCOMPARE( spy2.at(0).at(0).toString(), QString("+123456"));
    
    // Some completly dummy parameters or no params at all
    spy.clear();
    spy2.clear();
    map.clear();
    map.insert("dummy_param", QVariant(666));
    QVERIFY( mService->show(map) == 0 );
    QVERIFY( spy.count() == 1 );
    QVERIFY( spy2.count() == 0 );
    view = qvariant_cast< XQService::LogsViewIndex >(spy.at(0).at(0));
    QCOMPARE( view, XQService::LogsViewAll );
    QCOMPARE( spy.at(0).at(1).toBool(), false );
    QCOMPARE( spy.at(0).at(2).toString(), QString(""));    
    
    // Both new and deprecated params present, new params will be used
    spy.clear();
    map.clear();
    map.insert(logsViewIndexParam, QVariant((int)XQService::LogsViewReceived));
    map.insert(logsShowDialpadParam, QVariant(false));
    map.insert(logsDialpadTextParam, QVariant(QString("+123456")));
    
    map.insert(XQLOGS_VIEW_INDEX, QVariant((int)XQService::LogsViewCalled));
    map.insert(XQLOGS_SHOW_DIALPAD, QVariant(true));
    map.insert(XQLOGS_DIALPAD_TEXT, QVariant(QString("")));
    QVERIFY( mService->show(map) == 0 );
    QVERIFY( spy.count() == 1 );
    QVERIFY( spy2.count() == 0 );
    view = qvariant_cast< XQService::LogsViewIndex >(spy.at(0).at(0));
    QCOMPARE( view, XQService::LogsViewCalled );
    QCOMPARE( spy.at(0).at(1).toBool(), true );
    QCOMPARE( spy.at(0).at(2).toString(), QString(""));
}
