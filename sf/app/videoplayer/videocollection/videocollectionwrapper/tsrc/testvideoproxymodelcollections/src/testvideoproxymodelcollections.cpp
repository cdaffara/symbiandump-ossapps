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
* Description:   tester for methods in VideoProxyModelCollections
* 
*/

#include <QModelIndex>
#include <QModelIndexList>
#include <hbapplication.h>

#include <vcxmyvideosdefs.h>
#include "videoproxymodelgeneric.h"
#include "videolistdatamodel.h"

#define private public
#include "videoproxymodelcollections.h"
#undef private

#include "filterproxytester.h"
#include "videocollectioncommon.h"
#include "testvideoproxymodelcollections.h"

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    
    TestVideoProxyModelCollections tv;

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
        pass[2] = "c:\\data\\testVideoProxyModelCollections.txt";
        res = QTest::qExec(&tv, 3, pass);
    }    
    
    return res;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestVideoProxyModelCollections::init()
{
    qRegisterMetaType<TMPXItemId>("TMPXItemId");
    
    mTestObject = new FilterProxyTester();
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestVideoProxyModelCollections::cleanup()
{
    delete mTestObject;
    mTestObject = 0;
}

// ---------------------------------------------------------------------------
// testLessThan
// ---------------------------------------------------------------------------
//
void TestVideoProxyModelCollections::testLessThan()
{
    QModelIndex left;
    QModelIndex right;

    // No model.
    QVERIFY(mTestObject->callLessThan(left, right) == false);
    QCOMPARE(VideoListDataModel::mMediaIdAtIndexCallCount, 0);

    // Set model.
    VideoListDataModel *model = 0;
    model = new VideoListDataModel();
    mTestObject->mModel = model;
    
    // Default categories are always first in the following order:
    // Recently played (missing currently)
    // Captured
    // Downloaded
    // Podcasts (missing currently)    
    
    // Items are not categories.
    VideoProxyModelGeneric::mLessThanCallCount = 0;
    VideoListDataModel::mMediaIdAtIndexCallCount = 0;    
    VideoListDataModel::mMediaIdAtIndexReturnValues.append(TMPXItemId(0, KVcxMvcMediaTypeVideo));
    VideoListDataModel::mMediaIdAtIndexReturnValues.append(TMPXItemId(1, KVcxMvcMediaTypeAlbum));
    QVERIFY(mTestObject->callLessThan(left, right) == true);
    QCOMPARE(VideoListDataModel::mMediaIdAtIndexCallCount, 2);
    QCOMPARE(VideoProxyModelGeneric::mLessThanCallCount, 1);

    // Both are categories, left is captured
    VideoProxyModelGeneric::mLessThanCallCount = 0;
    VideoListDataModel::mMediaIdAtIndexCallCount = 0;
    VideoListDataModel::mMediaIdAtIndexReturnValues.append(
            TMPXItemId(KVcxMvcCategoryIdCaptured, KVcxMvcMediaTypeCategory));
    VideoListDataModel::mMediaIdAtIndexReturnValues.append(
            TMPXItemId(KVcxMvcCategoryIdDownloads, KVcxMvcMediaTypeCategory));
    QVERIFY(mTestObject->callLessThan(left, right) == true);
    QCOMPARE(VideoListDataModel::mMediaIdAtIndexCallCount, 2);
    QCOMPARE(VideoProxyModelGeneric::mLessThanCallCount, 0);

    // Both are categories, left is downloads, right some other
    VideoProxyModelGeneric::mLessThanCallCount = 0;
    VideoListDataModel::mMediaIdAtIndexCallCount = 0;
    VideoListDataModel::mMediaIdAtIndexReturnValues.append(
            TMPXItemId(KVcxMvcCategoryIdDownloads, KVcxMvcMediaTypeCategory));
    VideoListDataModel::mMediaIdAtIndexReturnValues.append(
            TMPXItemId(KVcxMvcCategoryIdOther, KVcxMvcMediaTypeCategory));
    QVERIFY(mTestObject->callLessThan(left, right) == true);
    QCOMPARE(VideoListDataModel::mMediaIdAtIndexCallCount, 2);
    QCOMPARE(VideoProxyModelGeneric::mLessThanCallCount, 0);

    // Both are categories, left is downloads, right captured
    VideoProxyModelGeneric::mLessThanCallCount = 0;
    VideoListDataModel::mMediaIdAtIndexCallCount = 0;
    VideoListDataModel::mMediaIdAtIndexReturnValues.append(
            TMPXItemId(KVcxMvcCategoryIdDownloads, KVcxMvcMediaTypeCategory));
    VideoListDataModel::mMediaIdAtIndexReturnValues.append(
            TMPXItemId(KVcxMvcCategoryIdCaptured, KVcxMvcMediaTypeCategory));
    QVERIFY(mTestObject->callLessThan(left, right) == false);
    QCOMPARE(VideoListDataModel::mMediaIdAtIndexCallCount, 2);
    QCOMPARE(VideoProxyModelGeneric::mLessThanCallCount, 0);

    // Only left is category
    VideoProxyModelGeneric::mLessThanCallCount = 0;
    VideoListDataModel::mMediaIdAtIndexCallCount = 0;
    VideoListDataModel::mMediaIdAtIndexReturnValues.append(
            TMPXItemId(KVcxMvcCategoryIdDownloads, KVcxMvcMediaTypeCategory));
    VideoListDataModel::mMediaIdAtIndexReturnValues.append(
            TMPXItemId(1, KVcxMvcMediaTypeVideo));
    QVERIFY(mTestObject->callLessThan(left, right) == true);
    QCOMPARE(VideoListDataModel::mMediaIdAtIndexCallCount, 2);
    QCOMPARE(VideoProxyModelGeneric::mLessThanCallCount, 0);

    // Only right is category
    VideoProxyModelGeneric::mLessThanCallCount = 0;
    VideoListDataModel::mMediaIdAtIndexCallCount = 0;
    VideoListDataModel::mMediaIdAtIndexReturnValues.append(
            TMPXItemId(1, KVcxMvcMediaTypeVideo));
    VideoListDataModel::mMediaIdAtIndexReturnValues.append(
            TMPXItemId(KVcxMvcCategoryIdDownloads, KVcxMvcMediaTypeCategory));
    QVERIFY(mTestObject->callLessThan(left, right) == false);
    QCOMPARE(VideoListDataModel::mMediaIdAtIndexCallCount, 2);
    QCOMPARE(VideoProxyModelGeneric::mLessThanCallCount, 0);
    
    // Sort order is descending. Both are categories, left is captured. 
    mTestObject->mWantedSortOrder = Qt::DescendingOrder;
    VideoProxyModelGeneric::mLessThanCallCount = 0;
    VideoListDataModel::mMediaIdAtIndexCallCount = 0;
    VideoListDataModel::mMediaIdAtIndexReturnValues.append(
            TMPXItemId(KVcxMvcCategoryIdCaptured, KVcxMvcMediaTypeCategory));
    VideoListDataModel::mMediaIdAtIndexReturnValues.append(
            TMPXItemId(KVcxMvcCategoryIdDownloads, KVcxMvcMediaTypeCategory));
    QVERIFY(mTestObject->callLessThan(left, right) == false);
    QCOMPARE(VideoListDataModel::mMediaIdAtIndexCallCount, 2);
    QCOMPARE(VideoProxyModelGeneric::mLessThanCallCount, 0);    
}

// ---------------------------------------------------------------------------
// testFilterAcceptsRow
// ---------------------------------------------------------------------------
//
void TestVideoProxyModelCollections::testFilterAcceptsRow()
{
    // No source model
    VideoListDataModel *model = 0;
    mTestObject->mModel = model;
    VideoListDataModel::mMediaIdAtIndexCallCount = 0;
    VideoListDataModel::mRowCountReturnValue = 0;
    QVERIFY(mTestObject->callFilterAcceptsRow(0, QModelIndex()) == false);
    QCOMPARE(VideoListDataModel::mMediaIdAtIndexCallCount, 0);
    
    // Source row < 0
    model = new VideoListDataModel();
    mTestObject->mModel = model;
    VideoListDataModel::mMediaIdAtIndexCallCount = 0;
    VideoListDataModel::mRowCountReturnValue = 0;
    QVERIFY(mTestObject->callFilterAcceptsRow(-1, QModelIndex()) == false);
    QCOMPARE(VideoListDataModel::mMediaIdAtIndexCallCount, 0);

    // Source row > model row count
    VideoListDataModel::mMediaIdAtIndexCallCount = 0;
    VideoListDataModel::mRowCountReturnValue = 0;
    QVERIFY(mTestObject->callFilterAcceptsRow(10, QModelIndex()) == false);
    QCOMPARE(VideoListDataModel::mMediaIdAtIndexCallCount, 0);
    
    // Current level is not category
    mTestObject->mLevel = VideoCollectionCommon::ELevelVideos;
    VideoListDataModel::mMediaIdAtIndexReturnValues.clear();
    VideoListDataModel::mMediaIdAtIndexReturnValues.append(TMPXItemId(0, KVcxMvcMediaTypeAlbum));
    VideoListDataModel::mMediaIdAtIndexCallCount = 0;
    VideoListDataModel::mRowCountReturnValue = 1;
    QVERIFY(mTestObject->callFilterAcceptsRow(0, QModelIndex()) == false);
    QCOMPARE(VideoListDataModel::mMediaIdAtIndexCallCount, 1);

    // Item id is not an album
    mTestObject->mLevel = VideoCollectionCommon::ELevelCategory;
    VideoListDataModel::mMediaIdAtIndexReturnValues.clear();
    VideoListDataModel::mMediaIdAtIndexReturnValues.append(TMPXItemId(0, KVcxMvcMediaTypeVideo));
    VideoListDataModel::mMediaIdAtIndexCallCount = 0;
    VideoListDataModel::mRowCountReturnValue = 1;
    QVERIFY(mTestObject->callFilterAcceptsRow(0, QModelIndex()) == false);
    QCOMPARE(VideoListDataModel::mMediaIdAtIndexCallCount, 1);

    // Item is album
    mTestObject->mLevel = VideoCollectionCommon::ELevelCategory;
    VideoListDataModel::mMediaIdAtIndexReturnValues.clear();
    VideoListDataModel::mMediaIdAtIndexReturnValues.append(TMPXItemId(0, KVcxMvcMediaTypeAlbum));
    VideoListDataModel::mMediaIdAtIndexCallCount = 0;
    VideoListDataModel::mRowCountReturnValue = 1;
    QVERIFY(mTestObject->callFilterAcceptsRow(0, QModelIndex()) == true);
    QCOMPARE(VideoListDataModel::mMediaIdAtIndexCallCount, 1);

    // Item is category
    mTestObject->mLevel = VideoCollectionCommon::ELevelCategory;
    VideoListDataModel::mMediaIdAtIndexReturnValues.clear();
    VideoListDataModel::mMediaIdAtIndexReturnValues.append(TMPXItemId(0, KVcxMvcMediaTypeCategory));
    VideoListDataModel::mMediaIdAtIndexCallCount = 0;
    VideoListDataModel::mRowCountReturnValue = 1;
    QVERIFY(mTestObject->callFilterAcceptsRow(0, QModelIndex()) == true);
    QCOMPARE(VideoListDataModel::mMediaIdAtIndexCallCount, 1);
}

// End of file
