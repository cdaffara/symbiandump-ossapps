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
#include "ut_logscomponentrepository.h"
#include "logsrecentcallsview.h"
#include "logsdetailsview.h"
#include "logsmatchesview.h"
#include "logscomponentrepository.h"
#include "logsdefs.h"
#include "logsmainwindow.h"

//SYSTEM
#include <QtTest/QtTest>

void UT_LogsComponentRepository::initTestCase()
{
    mMainWindow = new LogsMainWindow();
    mViewManager = new LogsViewManagerStub(*mMainWindow);
}

void UT_LogsComponentRepository::cleanupTestCase()
{
    delete mMainWindow;
    delete mViewManager;
}


void UT_LogsComponentRepository::init()
{
    mRepository = new LogsComponentRepository(*mViewManager);
}

void UT_LogsComponentRepository::cleanup()
{
    delete mRepository;
    mRepository = 0;
}

void UT_LogsComponentRepository::testConstructor()
{
    QVERIFY( !mRepository->mRecentCallsView );
    QVERIFY( !mRepository->mDetailsView );
    QVERIFY( !mRepository->mMatchesView );
    QVERIFY( !mRepository->mDialpad );
    QVERIFY( !mRepository->mDialpadKeyHandler );
    QVERIFY( mRepository->mModel );
}

void UT_LogsComponentRepository::testRecentCallsView()
{
    //loading view
    QVERIFY( !mRepository->mRecentCallsView );
    QVERIFY( mRepository->mRecentViewComponents.count() == 0 );
    LogsRecentCallsView* view = mRepository->recentCallsView();
    QVERIFY( view );
    QCOMPARE( view->objectName(), QString( logsRecentCallsViewId ) );

    //view already loaded
    LogsRecentCallsView* view2 = mRepository->recentCallsView();
    QVERIFY( view == view2 );
    delete view;
}

void UT_LogsComponentRepository::testDetailsView()
{
    //loading view
    QVERIFY( !mRepository->mDetailsView );
    QVERIFY( mRepository->mDetailsViewComponents.count() == 0 );
    LogsDetailsView* view = mRepository->detailsView();
    QVERIFY( view );
    QCOMPARE( view->objectName(), QString( logsDetailsViewId ) );
    
    //view already loaded
    LogsDetailsView* view2 = mRepository->detailsView();
    QVERIFY( view == view2 );
    delete view;
}

void UT_LogsComponentRepository::testMatchesView()
{
    //loading view
    QVERIFY( !mRepository->mMatchesView );
    QVERIFY( mRepository->mMatchesViewComponents.count() == 0 );
    LogsMatchesView* view = mRepository->matchesView();
    QVERIFY( view );
    QCOMPARE( view->objectName(), QString( logsMatchesViewId ) );
    
    //view already loaded
    LogsMatchesView* view2 = mRepository->matchesView();
    QVERIFY( view == view2 );
    delete view;
}

void UT_LogsComponentRepository::testDialpad()
{
    //first call, dialpad and dialpadkeyhandler created as a result of the call
    QVERIFY( !mRepository->mDialpad );
    QVERIFY( !mRepository->mDialpadKeyHandler );
    Dialpad* dialpad = mRepository->dialpad();
    QVERIFY( dialpad );
    QVERIFY( dialpad == mRepository->mDialpad );
    QVERIFY( mRepository->mDialpadKeyHandler );
    DialpadKeyHandler* prevKeyHandler = mRepository->mDialpadKeyHandler;
    //all the next calls, return the same instance of the dialer
    QVERIFY( dialpad == mRepository->dialpad() );
    QVERIFY( prevKeyHandler == mRepository->mDialpadKeyHandler );
}

void UT_LogsComponentRepository::testLoadSection()
{
    //Unknow viewId, section is not loaded
    QVERIFY( !mRepository->loadSection(LogsUnknownViewId, "default") );
    
    //loading of non-existing section
    QVERIFY( !mRepository->loadSection(LogsDetailsViewId, "dummy") );

    //loading of existing section is ok
    mRepository->matchesView();
    QVERIFY( mRepository->loadSection(LogsMatchesViewId, "default") );    
    mRepository->recentCallsView();
    QVERIFY( mRepository->loadSection(LogsRecentViewId, "default") );
}

void UT_LogsComponentRepository::testAddToolbarToObjectList()
{
    //toolbar widget not found in docml
    QVERIFY( mRepository->mRecentViewComponents.count() == 0 );
    mRepository->addToolbarToObjectList( mRepository->mRecentViewComponents  );
    QVERIFY( mRepository->mRecentViewComponents.count() == 0 );
    
    //toolbar found and added to object list
    LogsRecentCallsView* view = mRepository->recentCallsView();
    mRepository->mRecentViewComponents.clear();
    mRepository->addToolbarToObjectList( mRepository->mRecentViewComponents  );
    QVERIFY( mRepository->mRecentViewComponents.count() == 1 );
    QVERIFY( mRepository->mRecentViewComponents.at(0)->objectName() == logsToolbarId );
    delete view;
}

void UT_LogsComponentRepository::testLazyInit()
{
    QObjectList dummyObjList;
    mRepository->mCurrentObjectTree = &dummyObjList;
    QVERIFY( !mRepository->mRecentCallsView );
    QVERIFY( !mRepository->mDetailsView );
    QVERIFY( !mRepository->mMatchesView );
    mRepository->lazyInit();
    QVERIFY( mRepository->mRecentCallsView );
    QVERIFY( mRepository->mDetailsView );
    QVERIFY( mRepository->mMatchesView );
    QVERIFY( mRepository->mCurrentObjectTree == &dummyObjList );
}
