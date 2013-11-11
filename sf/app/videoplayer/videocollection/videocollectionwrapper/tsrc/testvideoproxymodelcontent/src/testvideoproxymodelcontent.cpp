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
* Description:   tester for methods in VideoProxyModelContent
* 
*/

#include <QModelIndex>
#include <QModelIndexList>
#include <hbapplication.h>

#include "videoproxymodelgeneric.h"

#define private public
#include "videoproxymodelcontent.h"
#undef private

#include "videolistdatamodel.h"
#include "filterproxytester.h"
#include "videocollectioncommon.h"
#include "testvideoproxymodelcontent.h"

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    
    TestVideoProxyModelContent tv;

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
        pass[2] = "c:\\data\\testVideoProxyModelContent.txt";
        res = QTest::qExec(&tv, 3, pass);
    }    
    
    return res;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestVideoProxyModelContent::init()
{
    qRegisterMetaType<TMPXItemId>("TMPXItemId");
    
    mTestObject = new FilterProxyTester();
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestVideoProxyModelContent::cleanup()
{
    delete mTestObject;
    mTestObject = 0;
}

// ---------------------------------------------------------------------------
// testConnectSignals
// ---------------------------------------------------------------------------
//
void TestVideoProxyModelContent::testConnectSignals()
{
    // Base class connect returns false.
    VideoProxyModelGeneric::mConnectSignalsCallCount = 0;
    VideoProxyModelGeneric::mConnectSignalsReturnValue = false;
    QVERIFY(mTestObject->callConnectSignals() == false);
    QCOMPARE(VideoProxyModelGeneric::mConnectSignalsCallCount, 1);
    
    // No model.
    VideoProxyModelGeneric::mConnectSignalsCallCount = 0;
    VideoProxyModelGeneric::mConnectSignalsReturnValue = true;
    QVERIFY(mTestObject->callConnectSignals() == false);
    QCOMPARE(VideoProxyModelGeneric::mConnectSignalsCallCount, 1);

    // Model exists.
    VideoProxyModelGeneric::mConnectSignalsCallCount = 0;
    VideoProxyModelGeneric::mConnectSignalsReturnValue = true;
    VideoListDataModel *model = new VideoListDataModel();
    mTestObject->mModel = model;
    QVERIFY(mTestObject->callConnectSignals() == true);
    QCOMPARE(VideoProxyModelGeneric::mConnectSignalsCallCount, 1);
}

// ---------------------------------------------------------------------------
// testDisconnectSignals
// ---------------------------------------------------------------------------
//
void TestVideoProxyModelContent::testDisconnectSignals()
{
    // No model
    VideoProxyModelGeneric::mDisconnectSignalsCallCount = 0;
    mTestObject->callDisconnectSignals();
    QCOMPARE(VideoProxyModelGeneric::mDisconnectSignalsCallCount, 1);
    
    // Model exists.
    VideoProxyModelGeneric::mDisconnectSignalsCallCount = 0;
    VideoListDataModel *model = new VideoListDataModel();
    mTestObject->mModel = model;
    mTestObject->callDisconnectSignals();
    QCOMPARE(VideoProxyModelGeneric::mDisconnectSignalsCallCount, 1);
}

// ---------------------------------------------------------------------------
// testFilterAcceptsRow
// ---------------------------------------------------------------------------
//
void TestVideoProxyModelContent::testFilterAcceptsRow()
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
    
    // Doesn't belong to album
    VideoListDataModel::mBelongsToAlbumReturnValue = false;
    VideoListDataModel::mMediaIdAtIndexCallCount = 0;
    VideoListDataModel::mRowCountReturnValue = 1;
    QVERIFY(mTestObject->callFilterAcceptsRow(0, QModelIndex()) == false);
    QCOMPARE(VideoListDataModel::mMediaIdAtIndexCallCount, 1);

    // Good case
    VideoListDataModel::mBelongsToAlbumReturnValue = true;
    VideoListDataModel::mMediaIdAtIndexCallCount = 0;
    VideoListDataModel::mRowCountReturnValue = 1;
    QVERIFY(mTestObject->callFilterAcceptsRow(0, QModelIndex()) == true);
    QCOMPARE(VideoListDataModel::mMediaIdAtIndexCallCount, 1);
}

// ---------------------------------------------------------------------------
// testGetOpenItem
// ---------------------------------------------------------------------------
//
void TestVideoProxyModelContent::testGetOpenItem()
{
    TMPXItemId returned = TMPXItemId(1, 1);
    
    // No model
    VideoListDataModel::mAlbumInUseReturnValue = TMPXItemId(2, 2);
    VideoListDataModel *model = 0;
    mTestObject->mModel = model;
    returned = mTestObject->getOpenItem();
    QVERIFY(returned == TMPXItemId::InvalidId());
    
    // No collection client
    returned = TMPXItemId(1, 1);
    VideoListDataModel::mAlbumInUseReturnValue = TMPXItemId(2, 2);
    model = new VideoListDataModel();
    mTestObject->mModel = model;
    mTestObject->mCollectionClient = 0;
    returned = mTestObject->getOpenItem();
    QVERIFY(returned == TMPXItemId::InvalidId());

    // Good case.
    returned = TMPXItemId(1, 1);
    VideoListDataModel::mAlbumInUseReturnValue = TMPXItemId(2, 2); 
    mTestObject->mModel = model;
    mTestObject->mCollectionClient = 1;
    returned = mTestObject->getOpenItem();
    QVERIFY(returned == TMPXItemId(2, 2));
}

// ---------------------------------------------------------------------------
// testAlbumChangedSlot
// ---------------------------------------------------------------------------
//
void TestVideoProxyModelContent::testAlbumChangedSlot()
{
    VideoProxyModelGeneric::mLastSortRole = -1;
    mTestObject->mWantedSortRole = 5;
    mTestObject->mWantedSortOrder = Qt::DescendingOrder;
    VideoProxyModelGeneric::mLastSortedOrder = Qt::AscendingOrder;
            
    VideoListDataModel *model = new VideoListDataModel();
    mTestObject->mModel = model;
    mTestObject->callConnectSignals();
    emit model->emitAlbumChanged();
    mTestObject->callDisconnectSignals();
    QCOMPARE(VideoProxyModelGeneric::mInvalidateFilterCallCount, 1);
    QVERIFY(VideoProxyModelGeneric::mLastSortedOrder == Qt::DescendingOrder);
    QCOMPARE(VideoProxyModelGeneric::mLastSortRole, 5);
}

// End of file
