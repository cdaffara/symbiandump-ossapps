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
 * Description:  Item decorator of the message list view.
 *
 */

#include "unittest_msgaudioselectionengine.h"
#include <QtTest/QtTest>
#include <HbMainWindow>
#include "msguitest.h"
#include <XQConversions>

TestMsgAudioSelectionEngine::TestMsgAudioSelectionEngine()
    {

    }

TestMsgAudioSelectionEngine::~TestMsgAudioSelectionEngine()
    {
    
    }

void TestMsgAudioSelectionEngine::initTestCase()
    {
        mMsgAudioSelectionEngine = new MsgAudioSelectionEngine();
        QVERIFY(mMsgAudioSelectionEngine != NULL);
    }

void TestMsgAudioSelectionEngine::test_Constructor()
    {
        QVERIFY(mMsgAudioSelectionEngine->iQuery == NULL);
    }

void TestMsgAudioSelectionEngine::test_CreateMetaDataSession()
    {
        mMsgAudioSelectionEngine->CreateMetaDataSession();
        QVERIFY(mMsgAudioSelectionEngine->iSession != NULL);
    }

void TestMsgAudioSelectionEngine::test_QueryTones()
    {
        QTest::qWait(10000);
        mMsgAudioSelectionEngine->QueryTones();
        QVERIFY(mMsgAudioSelectionEngine->iQuery != NULL);
    
    }

void TestMsgAudioSelectionEngine::test_SetAttr()
    {
        mMsgAudioSelectionEngine->SetAttr(MsgAudioSelectionEngine::EAttrFileSize,20);
        QVERIFY(mMsgAudioSelectionEngine->iMaxFileSize == 20);
    }


void TestMsgAudioSelectionEngine::test_PropertyDefL()
    {
        CMdEPropertyDef& tempMdEPropertyDef = MsgAudioSelectionEngine::PropertyDefL(mMsgAudioSelectionEngine->iSession, MsgAudioSelectionEngine::EAttrFileName);
        QString str = XQConversions::s60DescToQString(tempMdEPropertyDef.Name());
        QVERIFY(str.isEmpty() != true); 
    }

void TestMsgAudioSelectionEngine::test_HandleSessionOpened()
    {
    QSignalSpy mdeSignalSessionError(mMsgAudioSelectionEngine, SIGNAL(mdeSessionError(int)));
    QSignalSpy mdeSignalSessionOpened(mMsgAudioSelectionEngine, SIGNAL(mdeSessionOpened()));
    CMdESession* lSession;
    
    QCOMPARE( mdeSignalSessionOpened.count(), 0);    
    mMsgAudioSelectionEngine->HandleSessionOpened(*lSession, KErrNone);
    QCOMPARE( mdeSignalSessionOpened.count(), 1); 
    
    QCOMPARE( mdeSignalSessionError.count(), 0 );    
    mMsgAudioSelectionEngine->HandleSessionOpened(*lSession, 1);
    QCOMPARE( mdeSignalSessionError.count(), 1);  
    }

void TestMsgAudioSelectionEngine::test_HandleObjectNotification()
    {
    CMdESession* lSession;
    RArray<TItemId> lItemID;
    QSignalSpy signalnotifyObjectChanged(mMsgAudioSelectionEngine, SIGNAL(notifyObjectChanged()));
    
    QCOMPARE( signalnotifyObjectChanged.count(), 0);
    mMsgAudioSelectionEngine->HandleObjectNotification(*lSession, ENotifyAdd,lItemID);
    QCOMPARE( signalnotifyObjectChanged.count(), 1);
    QTest::qWait(10000);
    }

void TestMsgAudioSelectionEngine::test_HandleObjectPresentNotification()
    {
    CMdESession* lSession;
    QSignalSpy signalnotifyObjectChanged(mMsgAudioSelectionEngine, SIGNAL(notifyObjectChanged()));
    RArray<TItemId> lItemID;
    lItemID.Append(10);
    lItemID.Append(20);
    lItemID.Append(30);    

    QCOMPARE( signalnotifyObjectChanged.count(), 0);
    mMsgAudioSelectionEngine->HandleObjectPresentNotification(*lSession,true,lItemID);
    QCOMPARE( signalnotifyObjectChanged.count(), 1);
    QTest::qWait(10000);
    }

void TestMsgAudioSelectionEngine::test_HandleSessionError()
    {
    QSignalSpy mdeSignalSessionError(mMsgAudioSelectionEngine, SIGNAL(mdeSessionError(int)));
    CMdESession* lSession;
    
    QCOMPARE( mdeSignalSessionError.count(), 0);
    mMsgAudioSelectionEngine->HandleSessionError(*lSession, 1);
    QCOMPARE( mdeSignalSessionError.count(), 1); 
    
    QVERIFY(mMsgAudioSelectionEngine->iSession == 0);
    QVERIFY(mMsgAudioSelectionEngine->iSessionOpen == EFalse);

    }

void TestMsgAudioSelectionEngine::cleanupTestCase()
    {
    if(mMsgAudioSelectionEngine)
        delete mMsgAudioSelectionEngine;
    }
    
MSGUI_TEST_MAIN(TestMsgAudioSelectionEngine)
