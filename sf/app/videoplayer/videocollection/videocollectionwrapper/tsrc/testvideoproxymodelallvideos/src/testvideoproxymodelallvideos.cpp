/**
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
* Description:   tester for methods in VideoProxyModelAllVideos
* 
*/

#include <QModelIndex>
#include <QModelIndexList>
#include <hbapplication.h>

#include <vcxmyvideosdefs.h>
#include "videoproxymodelgeneric.h"

#define private public
#include "videoproxymodelallvideos.h"
#undef private

#include "videolistdatamodel.h"
#include "filterproxytester.h"
#include "videocollectioncommon.h"
#include "testvideoproxymodelallvideos.h"

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    
    TestVideoProxyModelAllVideos tv;

    int res;
    if(argc > 1)
    {   
        res = QTest::qExec(&tv, argc, argv);
    }
    else
    {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\testVideoProxyModelAllVideos.txt";
        res = QTest::qExec(&tv, 3, pass);
    }    
    
    return res;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestVideoProxyModelAllVideos::init()
{
    qRegisterMetaType<TMPXItemId>("TMPXItemId");
    
    mTestObject = new FilterProxyTester();
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestVideoProxyModelAllVideos::cleanup()
{
    delete mTestObject;
    mTestObject = 0;
}

// ---------------------------------------------------------------------------
// testFilterAcceptsRow
// ---------------------------------------------------------------------------
//
void TestVideoProxyModelAllVideos::testFilterAcceptsRow()
{
    // No source model
    VideoListDataModel *model = 0;
    mTestObject->mModel = model;
    VideoListDataModel::mMediaIdAtIndexReturnValue = TMPXItemId::InvalidId();
    VideoListDataModel::mMediaIdAtIndexCallCount = 0;
    VideoListDataModel::mRowCountReturnValue = 0;
    QVERIFY(mTestObject->callFilterAcceptsRow(0, QModelIndex()) == false);
    QCOMPARE(VideoListDataModel::mMediaIdAtIndexCallCount, 0);
    
    // Source row < 0
    model = new VideoListDataModel();
    mTestObject->mModel = model;
    VideoListDataModel::mMediaIdAtIndexReturnValue = TMPXItemId::InvalidId();
    VideoListDataModel::mMediaIdAtIndexCallCount = 0;
    VideoListDataModel::mRowCountReturnValue = 0;
    QVERIFY(mTestObject->callFilterAcceptsRow(-1, QModelIndex()) == false);
    QCOMPARE(VideoListDataModel::mMediaIdAtIndexCallCount, 0);

    // Source row > model row count
    VideoListDataModel::mMediaIdAtIndexReturnValue = TMPXItemId::InvalidId();
    VideoListDataModel::mMediaIdAtIndexCallCount = 0;
    VideoListDataModel::mRowCountReturnValue = 0;
    QVERIFY(mTestObject->callFilterAcceptsRow(10, QModelIndex()) == false);
    QCOMPARE(VideoListDataModel::mMediaIdAtIndexCallCount, 0);
    
    // Item id is not video
    VideoListDataModel::mMediaIdAtIndexReturnValue = TMPXItemId(0, KVcxMvcMediaTypeAlbum);
    VideoListDataModel::mMediaIdAtIndexCallCount = 0;
    VideoListDataModel::mRowCountReturnValue = 1;
    QVERIFY(mTestObject->callFilterAcceptsRow(0, QModelIndex()) == false);
    QCOMPARE(VideoListDataModel::mMediaIdAtIndexCallCount, 1);

    // Good case
    VideoListDataModel::mMediaIdAtIndexReturnValue = TMPXItemId(0, KVcxMvcMediaTypeVideo);
    VideoListDataModel::mMediaIdAtIndexCallCount = 0;
    VideoListDataModel::mRowCountReturnValue = 1;
    QVERIFY(mTestObject->callFilterAcceptsRow(0, QModelIndex()) == true);
    QCOMPARE(VideoListDataModel::mMediaIdAtIndexCallCount, 1);
}

// ---------------------------------------------------------------------------
// testGetOpenItem
// ---------------------------------------------------------------------------
//
void TestVideoProxyModelAllVideos::testGetOpenItem()
{
    TMPXItemId returned = TMPXItemId(1, 1);
    
    // No model
    VideoListDataModel *model = 0;
    mTestObject->mModel = model;
    returned = mTestObject->getOpenItem();
    QVERIFY(returned == TMPXItemId::InvalidId());
    
    // No collection client
    model = new VideoListDataModel();
    mTestObject->mModel = model;
    mTestObject->mCollectionClient = 0;
    returned = mTestObject->getOpenItem();
    QVERIFY(returned == TMPXItemId::InvalidId());

    // Good case.
    mTestObject->mModel = model;
    mTestObject->mCollectionClient = 1;
    returned = mTestObject->getOpenItem();
    QVERIFY(returned == TMPXItemId(KVcxMvcCategoryIdAll, KVcxMvcMediaTypeCategory));
}

// End of file
