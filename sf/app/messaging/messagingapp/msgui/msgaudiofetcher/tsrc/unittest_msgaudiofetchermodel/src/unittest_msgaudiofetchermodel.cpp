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

#include "unittest_msgaudiofetchermodel.h"
#include <QtTest/QtTest>
#include <HbMainWindow>
#include <HbListView>
#include "msguitest.h"

TestMsgAudioFetcherModel::TestMsgAudioFetcherModel():mTestMainWindow(NULL),mListView(NULL),mMsgAudioFetcherModel(NULL)
    {

    }

TestMsgAudioFetcherModel::~TestMsgAudioFetcherModel()
    {

    }

void TestMsgAudioFetcherModel::initTestCase()
    {
        mTestMainWindow = new HbMainWindow();
        mListView = new HbListView();
        mMsgAudioFetcherModel = new MsgAudioFetcherModel(this);
        mListView->setModel(mMsgAudioFetcherModel);
        mTestMainWindow->addView(mListView);        
    }
void TestMsgAudioFetcherModel::test_Constructor()
    {
        QVERIFY(mMsgAudioFetcherModel->mSelectionEngine != NULL);
    }

void TestMsgAudioFetcherModel::test_init()
    {
        QVERIFY(mMsgAudioFetcherModel->mSelectionEngine != NULL);    
    }

void TestMsgAudioFetcherModel::test_addToneFiles()
    {
        QDir dir("c:\\data\\testAudioFetcher");
        mMsgAudioFetcherModel->addToneFiles(dir);
        int rowCount = mMsgAudioFetcherModel->rowCount();
        QVERIFY(mMsgAudioFetcherModel->rowCount() != 0);
    }

void TestMsgAudioFetcherModel::test_insertIndex()
    {
        QDir dir("c:\\data\\testAudioFetcher");
        mMsgAudioFetcherModel->addToneFiles(dir);
        /*As already 4 file are present int the list, So index for file AAA should be 0 and ZZZ should be 4*/
        int index = mMsgAudioFetcherModel->insertIndex(0,mMsgAudioFetcherModel->rowCount(),"AAA");
        QVERIFY(index == 0);
        index = mMsgAudioFetcherModel->insertIndex(0,mMsgAudioFetcherModel->rowCount(),"zzz");
        QVERIFY(index == 4);
    }

void TestMsgAudioFetcherModel::test_clearAll()
{    

    QVERIFY(mMsgAudioFetcherModel->rowCount() != 0);
    /*test_addToneFiles is already called so this count should not be 0*/
    mMsgAudioFetcherModel->clearAll();
    
    QVERIFY(mMsgAudioFetcherModel->rowCount() == 0) ;
    QVERIFY(mMsgAudioFetcherModel->columnCount() == 0);
}

void TestMsgAudioFetcherModel::test_queryComplete()
{
    QStringList nameList;
    QStringList uriList;
    nameList.append("drm.amr");
    nameList.append("nondrm.amr");
    
    uriList.append("c:\\data\\testAudioFetcher\\drm.amr");
    uriList.append("c:\\data\\testAudioFetcher\\nondrm.amr");
    mMsgAudioFetcherModel->queryComplete(nameList,uriList);
    QVERIFY(mMsgAudioFetcherModel->rowCount() == 2);
}

void TestMsgAudioFetcherModel::test_isDRM()
    {
        QFETCH(QString, fileName);
        QFETCH(bool, expectedResult);
        bool result = mMsgAudioFetcherModel->isDRM(fileName);
        QVERIFY(result == expectedResult);
    }
void TestMsgAudioFetcherModel::test_isDRM_data()
    {
        QTest::addColumn<QString>("fileName");
        QTest::addColumn<bool>("expectedResult");
        QTest::newRow("NON DRM File")<< "c:\\data\\testAudioFetcher\\drm.amr" << false;
        QTest::newRow("NON DRM File")<< "c:\\data\\testAudioFetcher\\nondrm.amr" << false;
        QTest::newRow("DRM File")<< "c:\\data\\testAudioFetcher\\DRM_AMR.dcf" << true;
    } 

void TestMsgAudioFetcherModel::cleanupTestCase()
    {          
            if(mMsgAudioFetcherModel)
                delete mMsgAudioFetcherModel;
            if(mListView)
                delete mListView;
            if(mTestMainWindow)
                delete mTestMainWindow;
    }
MSGUI_TEST_MAIN(TestMsgAudioFetcherModel)
