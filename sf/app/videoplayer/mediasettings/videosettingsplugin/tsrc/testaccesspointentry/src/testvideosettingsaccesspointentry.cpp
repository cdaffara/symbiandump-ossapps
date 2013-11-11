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
* Description:   TestVideoSettingsAccessPointEntry class implementation
* 
*/

// INCLUDE FILES
#include <QtTest/QtTest>
#include <qdebug.h>
#include <hbapplication.h>
#include <cpitemdatahelper.h>

#include "testvideosettingsaccesspointentry.h"
#include "videosettingsgroup.h"
#include "cmmanager_shim.h"
#include "cmconnectionmethod_shim.h"

// trick to get access to protected/private members.
#define private public
#include "videosettingsaccesspointentry.h"
#undef private

// ---------------------------------------------------------------------------
// initTestCase
// ---------------------------------------------------------------------------
//
void TestVideoSettingsAccessPointEntry::initTestCase()
{
    mItemHelper = new CpItemDataHelper;
    mGroup = new VideoSettingsGroup(*mItemHelper);
}

// ---------------------------------------------------------------------------
// cleanupTestCase
// ---------------------------------------------------------------------------
//
void TestVideoSettingsAccessPointEntry::cleanupTestCase()
{
    delete mGroup;
    mGroup = 0;
    delete mItemHelper;
    mItemHelper = 0;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestVideoSettingsAccessPointEntry::init()
{
    mTestObject = new VideoSettingsAccessPointEntry(*mItemHelper, QString("test"), mGroup);
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestVideoSettingsAccessPointEntry::cleanup()
{
    delete mTestObject;
    mTestObject = 0;
}

// ---------------------------------------------------------------------------
// testConstructor
// ---------------------------------------------------------------------------
//
void TestVideoSettingsAccessPointEntry::testConstructor()
{
    // verify clicked signal connection.
    ConnectionHolder connection = mItemHelper->mConnections[mTestObject];
    QCOMPARE( connection.signal, SIGNAL(clicked()) ); 
    QVERIFY( connection.receiver == mTestObject );
    QCOMPARE( connection.method, SLOT(openSelectionDialogSlot()) );
    
    // verify mApplSettings construction
    QVERIFY( mTestObject->mApplSettings );
    QVERIFY( disconnect(mTestObject->mApplSettings, SIGNAL(finished(uint)), mTestObject, SLOT(accessPointDialogFinished(uint))) );
    connect(mTestObject->mApplSettings, SIGNAL(finished(uint)), mTestObject, SLOT(accessPointDialogFinished(uint)));
    
    // verify mSelection initial values
    QVERIFY( mTestObject->mSelection.result == CmApplSettingsUi::SelectionTypeConnectionMethod );
    QVERIFY( mTestObject->mSelection.id == 0 );
    
    // verify CmManagerShim construction
    QVERIFY( mTestObject->mCmManager );
    
    // verify base class 'construction'
    QCOMPARE( mTestObject->mText, QString("test") );
    QVERIFY( mTestObject->mDescription.isNull() );
}

// ---------------------------------------------------------------------------
// testConstructor
// ---------------------------------------------------------------------------
//
void TestVideoSettingsAccessPointEntry::testDestructor()
{
    CmManagerShim::mDestructorCount = 0;
    cleanup();
    QCOMPARE( CmManagerShim::mDestructorCount, 1 );
}

// ---------------------------------------------------------------------------
// testSetIapId
// ---------------------------------------------------------------------------
//
void TestVideoSettingsAccessPointEntry::testSetIapId()
{
    // with empty cmArray
    mTestObject->setIapId(5);
    QCOMPARE( mTestObject->mDescription, hbTrId("txt_videos_dblist_none") );
    
    // with non-empty cmArray, but id is not found.
    mTestObject->mCmManager->mConnectionMethods[0] = new CmConnectionMethodShim(0, 4, QString("0"));
    mTestObject->mCmManager->mConnectionMethods[2] = new CmConnectionMethodShim(2, 6, QString("2"));
    mTestObject->setIapId(5);
    QCOMPARE( mTestObject->mDescription, hbTrId("txt_videos_dblist_none") );
    
    // with non-empty cmArray, and id is found, but CmManagerShim throws at that id.
    mTestObject->mCmManager->mConnectionMethods[1] = new CmConnectionMethodShim(1, 5, QString("1"));
    CmManagerShim::mConnectionMethodThrowsAtId = 1;
    mTestObject->setIapId(5);
    QCOMPARE( mTestObject->mDescription, hbTrId("txt_videos_dblist_none") );
    
    // with non-empty cmArray, and id is found.
    CmManagerShim::mConnectionMethodThrowsAtId = -1;
    mTestObject->setIapId(5);
    QCOMPARE( mTestObject->mDescription, QString("1") );
    
    // with non-empty cmArray, and id is found, but CmManagerShim throws at id before.
    CmManagerShim::mConnectionMethodThrowsAtId = 0;
    mTestObject->mDescription = QString();
    mTestObject->setIapId(5);
    QCOMPARE( mTestObject->mDescription, QString("1") );
    
    // with non-empty cmArray, and id is found, but CmManagerShim throws at id after.
    CmManagerShim::mConnectionMethodThrowsAtId = 2;
    mTestObject->mDescription = QString();
    mTestObject->setIapId(5);
    QCOMPARE( mTestObject->mDescription, QString("1") );
}

// ---------------------------------------------------------------------------
// testCreateSettingView
// ---------------------------------------------------------------------------
//
void TestVideoSettingsAccessPointEntry::testCreateSettingView()
{
    QVERIFY(mTestObject->callCreateSettingView() == 0 );
}

// ---------------------------------------------------------------------------
// testOpenSelectionDialogSlot
// ---------------------------------------------------------------------------
//
void TestVideoSettingsAccessPointEntry::testOpenSelectionDialogSlot()
{
    connect(this, SIGNAL(testSignal()), mTestObject, SLOT(openSelectionDialogSlot()));
    QSet<CmApplSettingsUi::BearerTypeFilter> filter;
    filter.insert(1);
    mTestObject->mApplSettings->mListItems = QFlags<CmApplSettingsUi::SelectionDialogItems>();
    mTestObject->mApplSettings->mBearerTypeFilter = filter;
    mTestObject->mApplSettings->mOpenCallAmount = 0; 
    
    emit testSignal();
    
    QVERIFY( mTestObject->mApplSettings->mListItems.testFlag(CmApplSettingsUi::ShowConnectionMethods) );
    QVERIFY( mTestObject->mApplSettings->mListItems.testFlag(CmApplSettingsUi::ShowDestinations) == false);
    QCOMPARE( mTestObject->mApplSettings->mBearerTypeFilter.count(), 0);
    QVERIFY( mTestObject->mApplSettings->mSelection.result == CmApplSettingsUi::SelectionTypeConnectionMethod );
    QCOMPARE( mTestObject->mApplSettings->mOpenCallAmount, 1 );
}

// ---------------------------------------------------------------------------
// testAccessPointDialogFinished
// ---------------------------------------------------------------------------
//
void TestVideoSettingsAccessPointEntry::testAccessPointDialogFinished()
{
    // error
    mTestObject->mApplSettings->mSelection.id = 5;
    mTestObject->mSelection.id = 0;
    mTestObject->mDescription = QString();
    mGroup->mApId = 0;
    
    mTestObject->mApplSettings->emitFinished(CmApplSettingsUi::ApplSettingsErrorCancel);
    
    QCOMPARE( mTestObject->mSelection.id, (uint)0 );
    
    // no error
    mTestObject->mDescription = QString();
    mTestObject->mCmManager->mConnectionMethods[5] = new CmConnectionMethodShim(5, 4, QString("5"));
    
    mTestObject->mApplSettings->emitFinished(CmApplSettingsUi::ApplSettingsErrorNone);
    
    QCOMPARE( mTestObject->mSelection.id, mTestObject->mApplSettings->mSelection.id );
    QCOMPARE( mGroup->mApId, (uint)4 );
    QCOMPARE( mTestObject->mDescription, QString("5") );
    
    // no error, but CmManagerShim throws at that id.
    mTestObject->mSelection.id = 0;
    mTestObject->mDescription = QString();
    mGroup->mApId = 0;
    CmManagerShim::mConnectionMethodThrowsAtId = 5;
    
    mTestObject->mApplSettings->emitFinished(CmApplSettingsUi::ApplSettingsErrorNone);
    
    QCOMPARE( mTestObject->mSelection.id, (uint)0 );
    QCOMPARE( mGroup->mApId, (uint)0 );
    QVERIFY( mTestObject->mDescription.isNull() );
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    
    TestVideoSettingsAccessPointEntry tc;
    
    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testvideosettingsaccesspointentry.txt";

    return QTest::qExec(&tc, 3, pass);
}

// end of file
