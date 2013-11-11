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
* Description:   TestVideoSettingsGroup class implementation
* 
*/

// INCLUDE FILES
#include <QtTest/QtTest>
#include <qdebug.h>
#include <hbapplication.h>
#include <hblineedit.h>
#include <hbinputeditorinterface.h>
#include <hbinputstandardfilters.h>
#include <MPSettEngPluginImplementationUIDs.hrh>

#include "testvideosettingsgroup.h"
#include "videosettingsaccesspointentry.h"
#include "stub/inc/mpsettingsmodel.h"
#include "hbdataformmodel.h"
#include "cpitemdatahelper.h"
#include "hbvalidator.h"

// trick to get access to protected/private members.
#define private public
#include "videosettingsgroup.h"
#undef private

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    
    TestVideoSettingsGroup tc;
    
    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testvideosettingsgroup.txt";

    return QTest::qExec(&tc, 3, pass);
}

// ---------------------------------------------------------------------------
// initTestCase
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::initTestCase()
{
    mItemHelper = new CpItemDataHelper;
}

// ---------------------------------------------------------------------------
// cleanupTestCase
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::cleanupTestCase()
{
    delete mItemHelper;
    mItemHelper = 0;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::init()
{
    CMPSettingsModel::mImplUid = TUid::Null();
    CMPSettingsModel::mConfigMode = -1;
    CMPSettingsModel::mProxyMode = 0;
    CMPSettingsModel::mHostName = QString("testhostname");
    CMPSettingsModel::mProxyPort = 4096;
    CMPSettingsModel::mMinUdpPort = 2048;
    CMPSettingsModel::mMaxUdpPort = 8080;
    CMPSettingsModel::mStoreSettingsCount = 0;
    CMPSettingsModel::mDestructorCount = 0;
    CMPSettingsModel::mApId = 5;
    HbEditorInterface::mFilter = 0;
    
    mTestObject = new VideoSettingsGroup(*mItemHelper);
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::cleanup()
{
    disconnect();
    delete mTestObject;
    mTestObject = 0;
}

// ---------------------------------------------------------------------------
// testSettingsConstruction
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testSettingsConstruction()
{
    QVERIFY( mTestObject->mSettingsModel != 0 );
    QCOMPARE( mTestObject->mSettingsModel->mConfigMode, (int)EConfigUser);
    QVERIFY( CMPSettingsModel::mImplUid == TUid::Uid(KMPSettEngImplUidROP) );
    
    // verify also the itemShown connection to form here.
    QCOMPARE( mItemHelper->mFormConnection.signal, SIGNAL(itemShown(const QModelIndex)) );
    QVERIFY( mItemHelper->mFormConnection.receiver == mTestObject );
    QCOMPARE( mItemHelper->mFormConnection.method, SLOT(itemShown(const QModelIndex)) );
    
    // and that group has correct label
    QCOMPARE( mTestObject->label(), hbTrId("txt_videos_subhead_video_streaming_settings") );
}

// ---------------------------------------------------------------------------
// testAccessPointConstruction
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testAccessPointConstruction()
{
    QVERIFY( mTestObject->mAccessPointItem != 0 );
    QVERIFY( mTestObject->indexOf(static_cast<HbDataFormModelItem*>(mTestObject->mAccessPointItem)) >= 0 );
    QCOMPARE( mTestObject->mAccessPointItem->mText, hbTrId("txt_videos_dblist_access_point") );
    QCOMPARE( mTestObject->mAccessPointItem->mApId, (uint)CMPSettingsModel::mApId );
}

// ---------------------------------------------------------------------------
// testLowestUdpPortConstruction
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testLowestUdpPortConstruction()
{
    QVERIFY( mTestObject->mLowestUDPPortItem != 0 );
    QVERIFY( mTestObject->indexOf(mTestObject->mLowestUDPPortItem) >= 0 );
    QCOMPARE( mTestObject->mLowestUDPPortItem->contentWidgetData(QString("maxRows")).toInt(), 1);
    QCOMPARE( mTestObject->mLowestUDPPortItem->contentWidgetData(QString("text")).toString(), 
        QString::number(CMPSettingsModel::mMinUdpPort) );
    
    ConnectionHolder connection = mItemHelper->mConnections[mTestObject->mLowestUDPPortItem];
    QCOMPARE( connection.signal, SIGNAL(editingFinished()) ); 
    QVERIFY( connection.receiver == mTestObject );
    QCOMPARE( connection.method, SLOT(lowestUdpPortEditingFinished()) );
}

// ---------------------------------------------------------------------------
// testHighestUdpPortConstruction
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testHighestUdpPortConstruction()
{
    QVERIFY( mTestObject->mHighestUDPPortItem != 0 );
    QVERIFY( mTestObject->indexOf(mTestObject->mHighestUDPPortItem) >= 0 );
    QCOMPARE( mTestObject->mHighestUDPPortItem->contentWidgetData(QString("maxRows")).toInt(), 1);
    QCOMPARE( mTestObject->mHighestUDPPortItem->contentWidgetData(QString("text")).toString(), 
        QString::number(CMPSettingsModel::mMaxUdpPort) );
    
    ConnectionHolder connection = mItemHelper->mConnections[mTestObject->mHighestUDPPortItem];
    QCOMPARE( connection.signal, SIGNAL(editingFinished()) ); 
    QVERIFY( connection.receiver == mTestObject );
    QCOMPARE( connection.method, SLOT(highestUdpPortEditingFinished()) );
}

// ---------------------------------------------------------------------------
// testUseProxyConstruction
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testUseProxyConstruction()
{
    QVERIFY( mTestObject->mUseProxyItem != 0 );
    QVERIFY( mTestObject->indexOf(mTestObject->mUseProxyItem) >= 0 );
    QCOMPARE( mTestObject->mUseProxyItem->contentWidgetData(QString("checked")).toBool(), 
        CMPSettingsModel::mProxyMode == 1 );
    
    // connection checking in the testItemShown.
}

// ---------------------------------------------------------------------------
// testProxyServerConstruction
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testProxyServerConstruction()
{
    QVERIFY( mTestObject->mProxyServerItem != 0 );
    QVERIFY( mTestObject->indexOf(mTestObject->mProxyServerItem) >= 0 );
    QCOMPARE( mTestObject->mProxyServerItem->contentWidgetData(QString("maxRows")).toInt(), 1);
    QCOMPARE( mTestObject->mProxyServerItem->contentWidgetData(QString("text")).toString(), 
        CMPSettingsModel::mHostName );
    QCOMPARE( mTestObject->mProxyServerItem->contentWidgetData(QString("placeholderText")).toString(),
        hbTrId("txt_videos_dblist_none") );

    ConnectionHolder connection = mItemHelper->mConnections[mTestObject->mProxyServerItem];
    QCOMPARE( connection.signal, SIGNAL(editingFinished()) ); 
    QVERIFY( connection.receiver == mTestObject );
    QCOMPARE( connection.method, SLOT(proxyServerEditingFinished()) );
}

// ---------------------------------------------------------------------------
// testProxyPortConstruction
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testProxyPortConstruction()
{
    QVERIFY( mTestObject->mProxyPortItem != 0 );
    QVERIFY( mTestObject->indexOf(mTestObject->mProxyPortItem) >= 0 );
    QCOMPARE( mTestObject->mProxyPortItem->contentWidgetData(QString("maxRows")).toInt(), 1);
    QCOMPARE( mTestObject->mProxyPortItem->contentWidgetData(QString("text")).toString(), 
        QString::number(CMPSettingsModel::mProxyPort) );

    ConnectionHolder connection = mItemHelper->mConnections[mTestObject->mProxyPortItem];
    QCOMPARE( connection.signal, SIGNAL(editingFinished()) ); 
    QVERIFY( connection.receiver == mTestObject );
    QCOMPARE( connection.method, SLOT(proxyPortEditingFinished()) );
}

// ---------------------------------------------------------------------------
// testDestructor
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testDestructor()
{
    delete mTestObject;
    mTestObject = 0;
    QCOMPARE( CMPSettingsModel::mStoreSettingsCount, 1 );
    QCOMPARE( CMPSettingsModel::mDestructorCount, 1 );
}

// ---------------------------------------------------------------------------
// testDestructor
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testGetAccessPointId()
{
    CMPSettingsModel::mApId = 4;
    
    QCOMPARE( mTestObject->getAccessPointId(), (uint)CMPSettingsModel::mApId );
}

// ---------------------------------------------------------------------------
// testDestructor
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testSetAccessPointId()
{
    QString string("test");
    CMPSettingsModel::mApId = -1;
    mTestObject->mUseProxyItem->setContentWidgetData(QString("checked"), true);
    mTestObject->mProxyServerItem->setContentWidgetData(QString("text"), string);
    CMPSettingsModel::mHostName = string;
    
    // given ap id is different from the one that is saved in settingsmodel.
    uint ap = 3;
    mTestObject->setAccessPointId(ap);
    QCOMPARE( CMPSettingsModel::mApId, (int)ap );
    QVERIFY( mTestObject->mUseProxyItem->contentWidgetData(QString("checked")).toBool() == false );
    QCOMPARE( mTestObject->mProxyServerItem->contentWidgetData(QString("text")).toString(), string );
    QCOMPARE( CMPSettingsModel::mHostName, string );
    
    mTestObject->mUseProxyItem->setContentWidgetData(QString("checked"), true);
    mTestObject->mProxyServerItem->setContentWidgetData(QString("text"), string);
    CMPSettingsModel::mHostName = string;
    
    // given ap id is the same than the one that is saved in settingsmodel.
    mTestObject->setAccessPointId(ap);
    QCOMPARE( CMPSettingsModel::mApId, (int)ap );
    QVERIFY( mTestObject->mUseProxyItem->contentWidgetData(QString("checked")).toBool() );
    QCOMPARE( mTestObject->mProxyServerItem->contentWidgetData(QString("text")).toString(), string );
    QCOMPARE( CMPSettingsModel::mHostName, string );
}

// ---------------------------------------------------------------------------
// testLowestUdpPortEditingFinished
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testLowestUdpPortEditingFinished()
{
    connect(this, SIGNAL(testTextSignal()),
        mTestObject, SLOT(lowestUdpPortEditingFinished()));
    
    // normal case where min port is less than max port and both are in acceptable range.
    QString highText("10000");
    mTestObject->mHighestUDPPortItem->setContentWidgetData(QString("text"), highText);
    
    QString testString("1024");
    mTestObject->mLowestUDPPortItem->setContentWidgetData(QString("text"), testString);
    
    emit testTextSignal();
    
    QCOMPARE( CMPSettingsModel::mMinUdpPort, testString.toInt() );
    
    // min port greater than max port, both are in acceptable range.
    CMPSettingsModel::mMinUdpPort = 0;
    mTestObject->mLowestUDPPortItem->setContentWidgetData(QString("text"), QString("10001"));
    
    emit testTextSignal();
    
    QCOMPARE( CMPSettingsModel::mMinUdpPort, highText.toInt() );
    
    // min port same as max port and in acceptable range.
    CMPSettingsModel::mMinUdpPort = 0;
    testString = "10000";
    mTestObject->mLowestUDPPortItem->setContentWidgetData(QString("text"), testString);
    
    emit testTextSignal();
    
    QCOMPARE( CMPSettingsModel::mMinUdpPort, testString.toInt() );
    
    // min port less than 1024
    CMPSettingsModel::mMinUdpPort = 0;
    mTestObject->mLowestUDPPortItem->setContentWidgetData(QString("text"), QString("1023"));
    
    emit testTextSignal();
    
    QCOMPARE( CMPSettingsModel::mMinUdpPort, 1024 );
    QCOMPARE( mTestObject->mLowestUDPPortItem->contentWidgetData(QString("text")).toString(), QString("1024") );
    
    // min port greater than 65535
    CMPSettingsModel::mMinUdpPort = 0;
    mTestObject->mHighestUDPPortItem->setContentWidgetData(QString("text"), QString("65536"));
    mTestObject->mLowestUDPPortItem->setContentWidgetData(QString("text"), QString("65536"));
    
    emit testTextSignal();
    
    QCOMPARE( CMPSettingsModel::mMinUdpPort, 65535 );
    QCOMPARE( mTestObject->mLowestUDPPortItem->contentWidgetData(QString("text")).toString(), QString("65535") );
    
    // min port field empty
    CMPSettingsModel::mMinUdpPort = 0;
    mTestObject->mLowestUDPPortItem->setContentWidgetData(QString("text"), QString(""));
    
    emit testTextSignal();
    
    QCOMPARE( CMPSettingsModel::mMinUdpPort, 1024 );
    QCOMPARE( mTestObject->mLowestUDPPortItem->contentWidgetData(QString("text")).toString(), QString("1024") );
}

// ---------------------------------------------------------------------------
// testHighestUdpPortEditingFinished
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testHighestUdpPortEditingFinished()
{
    connect(this, SIGNAL(testTextSignal()),
        mTestObject, SLOT(highestUdpPortEditingFinished()));
    
    // normal case where min port is less than max port and both are in acceptable range.
    QString lowText("2000");
    mTestObject->mLowestUDPPortItem->setContentWidgetData(QString("text"), lowText);
    
    QString testString("65535");
    mTestObject->mHighestUDPPortItem->setContentWidgetData(QString("text"), testString);
    
    emit testTextSignal();
    
    QCOMPARE( CMPSettingsModel::mMaxUdpPort, testString.toInt() );
    
    // max port less than min port, both in acceptable range.
    CMPSettingsModel::mMaxUdpPort = 0;
    mTestObject->mHighestUDPPortItem->setContentWidgetData(QString("text"), QString("1999"));
    
    emit testTextSignal();
    
    QCOMPARE( CMPSettingsModel::mMaxUdpPort, lowText.toInt() );
    
    // max port same as min port and in acceptable range.
    CMPSettingsModel::mMaxUdpPort = 0;
    testString = "2000";
    mTestObject->mHighestUDPPortItem->setContentWidgetData(QString("text"), testString);
    
    emit testTextSignal();
    
    QCOMPARE( CMPSettingsModel::mMaxUdpPort, testString.toInt() );

    // max port less than 1024
    CMPSettingsModel::mMaxUdpPort = 0;
    mTestObject->mHighestUDPPortItem->setContentWidgetData(QString("text"), QString("1023"));
    mTestObject->mLowestUDPPortItem->setContentWidgetData(QString("text"), QString("1023"));
    
    emit testTextSignal();
    
    QCOMPARE( CMPSettingsModel::mMaxUdpPort, 1024 );
    QCOMPARE( mTestObject->mHighestUDPPortItem->contentWidgetData(QString("text")).toString(), QString("1024") );
    
    // max port greater than 65535
    CMPSettingsModel::mMaxUdpPort = 0;
    mTestObject->mHighestUDPPortItem->setContentWidgetData(QString("text"), QString("65536"));
    
    emit testTextSignal();
    
    QCOMPARE( CMPSettingsModel::mMaxUdpPort, 65535 );
    QCOMPARE( mTestObject->mHighestUDPPortItem->contentWidgetData(QString("text")).toString(), QString("65535") );
    
    // max port field empty
    CMPSettingsModel::mMaxUdpPort = 0;
    mTestObject->mHighestUDPPortItem->setContentWidgetData(QString("text"), QString(""));
    
    emit testTextSignal();
    
    QCOMPARE( CMPSettingsModel::mMaxUdpPort, 1024 );
    QCOMPARE( mTestObject->mHighestUDPPortItem->contentWidgetData(QString("text")).toString(), QString("1024") );
}

// ---------------------------------------------------------------------------
// testUseProxyToggled
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testUseProxyToggled()
{
    connect(this, SIGNAL(testIntSignal(int)),
        mTestObject, SLOT(useProxyToggled(int)));
    
    emit testIntSignal(Qt::Unchecked);
    
    QVERIFY( mTestObject->mProxyServerItem->isEnabled() == false );
    QVERIFY( mTestObject->mProxyPortItem->isEnabled() == false );
    QCOMPARE( CMPSettingsModel::mProxyMode, 0);
    
    emit testIntSignal(Qt::Checked);
    
    QVERIFY( mTestObject->mProxyServerItem->isEnabled() );
    QVERIFY( mTestObject->mProxyPortItem->isEnabled() );
    QCOMPARE( CMPSettingsModel::mProxyMode, 1);
}

// ---------------------------------------------------------------------------
// testProxyServerEditingFinished
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testProxyServerEditingFinished()
{
    connect(this, SIGNAL(testTextSignal()),
        mTestObject, SLOT(proxyServerEditingFinished()));
    
    QString testString("testtesttest");
    mTestObject->mProxyServerItem->setContentWidgetData(QString("text"), testString);
    
    emit testTextSignal();
    QCOMPARE( CMPSettingsModel::mHostName, testString );
}

// ---------------------------------------------------------------------------
// testProxyPortEditingFinished
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testProxyPortEditingFinished()
{
    connect(this, SIGNAL(testTextSignal()),
        mTestObject, SLOT(proxyPortEditingFinished()));
    
    // proxy port at lowest accepted range.
    QString testString("1");
    mTestObject->mProxyPortItem->setContentWidgetData(QString("text"), testString);
    
    emit testTextSignal();
    QCOMPARE( CMPSettingsModel::mProxyPort, testString.toInt() );
    
    // proxy port at highest accepted range.
    testString = "65535";
    mTestObject->mProxyPortItem->setContentWidgetData(QString("text"), testString);
    
    emit testTextSignal();
    QCOMPARE( CMPSettingsModel::mProxyPort, testString.toInt() );
    
    // proxy port 0
    mTestObject->mProxyPortItem->setContentWidgetData(QString("text"), QString("0"));
    
    emit testTextSignal();
    QCOMPARE( CMPSettingsModel::mProxyPort, 1 );
    QCOMPARE( mTestObject->mProxyPortItem->contentWidgetData(QString("text")).toString(), QString("1") );
    
    // proxy port higher than accepted.
    mTestObject->mProxyPortItem->setContentWidgetData(QString("text"), QString("65536"));
    
    emit testTextSignal();
    QCOMPARE( CMPSettingsModel::mProxyPort, 65535 );
    QCOMPARE( mTestObject->mProxyPortItem->contentWidgetData(QString("text")).toString(), QString("65535") );
    
    // proxy port empty
    mTestObject->mProxyPortItem->setContentWidgetData(QString("text"), QString(""));
    
    emit testTextSignal();
    QCOMPARE( CMPSettingsModel::mProxyPort, 1 );
    QCOMPARE( mTestObject->mProxyPortItem->contentWidgetData(QString("text")).toString(), QString("1") );
}

// ---------------------------------------------------------------------------
// testItemShown
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testItemShown()
{
    connect(this, SIGNAL(testShownSignal(const QModelIndex&)), 
        mTestObject, SLOT(itemShown(const QModelIndex&)));
    
    QModelIndex index;
    
    HbLineEdit* editor = new HbLineEdit();
    mItemHelper->mWidgetReturnValue = editor;
    
    mItemHelper->mModelItemReturnValue = mTestObject->mLowestUDPPortItem;
    HbEditorInterface::mFilter = 0;
    emit testShownSignal(index);
    QVERIFY( HbEditorInterface::mFilter == HbDigitsOnlyFilter::instance() );
    QCOMPARE( editor->mMaxLength, 5 );

    mItemHelper->mModelItemReturnValue = mTestObject->mProxyPortItem;
    HbEditorInterface::mFilter = 0;
    emit testShownSignal(index);
    QVERIFY( HbEditorInterface::mFilter == HbDigitsOnlyFilter::instance() );
    QCOMPARE( editor->mMaxLength, 5 );
    
    mItemHelper->mModelItemReturnValue = mTestObject->mHighestUDPPortItem;
    HbEditorInterface::mFilter = 0;
    emit testShownSignal(index);
    QVERIFY( HbEditorInterface::mFilter == HbDigitsOnlyFilter::instance() );
    QCOMPARE( editor->mMaxLength, 5 );
    
    mItemHelper->mModelItemReturnValue = mTestObject->mProxyServerItem;
    HbEditorInterface::mFilter = 0;
    emit testShownSignal(index);
    QVERIFY( HbEditorInterface::mFilter == HbUrlFilter::instance() );
    
    // verify the use proxy connection:
    ConnectionHolder connection = mItemHelper->mConnections[mTestObject->mUseProxyItem];
    QCOMPARE( connection.signal, SIGNAL(stateChanged(int)) ); 
    QVERIFY( connection.receiver == mTestObject );
    QCOMPARE( connection.method, SLOT(useProxyToggled(int)) );
    
    delete editor;
}

// end of file
