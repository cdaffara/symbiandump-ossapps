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

// INCLUDES

#include "testvideodeleteworker.h"
#include "videocollectionclient.h"
#include "videocollectioncommon.h"
#include <QList>
#include <hbapplication.h>

#define private public
#include "videodeleteworker.h"
#undef private

// -----------------------------------------------------------------------------
// main
// -----------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    HbApplication app(argc, argv);
    TestVideoDeleteWorker tv;
  
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
        pass[2] = "c:\\data\\testvideodeleteworker.txt";
        res = QTest::qExec(&tv, 3, pass);
    }

    return res;
}

// -----------------------------------------------------------------------------
// initTestCase
// -----------------------------------------------------------------------------
//
void TestVideoDeleteWorker::initTestCase()
{
    qRegisterMetaType<QList<TMPXItemId> >("QList<TMPXItemId>");
    mTestObject = 0;
    mDummyCollection = new VideoCollectionClient();
}

// -----------------------------------------------------------------------------
// cleanupTestCase
// -----------------------------------------------------------------------------
//
void TestVideoDeleteWorker::cleanupTestCase()
{
    delete mTestObject;
    delete mDummyCollection;
}

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
//
void TestVideoDeleteWorker::init()
{
    mTestObject = new VideoDeleteWorker(*mDummyCollection);
}
 
// -----------------------------------------------------------------------------
// cleanup
// -----------------------------------------------------------------------------
//
void TestVideoDeleteWorker::cleanup()
{
    delete mTestObject;
    mTestObject = 0;   
}

// -----------------------------------------------------------------------------
// testRequestDelete
// -----------------------------------------------------------------------------
//
void TestVideoDeleteWorker::testRequestDelete()
{
    qRegisterMetaType<QList<TMPXItemId> >("QList<TMPXItemId>& ");
    
    mDummyCollection->mDeleteFails = false;
    QList<TMPXItemId> itemIds;
    // empty list
    mTestObject->requestDelete(itemIds);
    QVERIFY(mTestObject->mRemoveBuffer.count() == 0);
    
    itemIds.append(TMPXItemId(0,0));
    itemIds.append(TMPXItemId(1,0));
    itemIds.append(TMPXItemId(2,0));
    // list contains something
    mTestObject->requestDelete(itemIds);
    QVERIFY(mTestObject->mRemoveBuffer.count() == 3);
    
    itemIds.append(TMPXItemId(2,0));
    itemIds.append(TMPXItemId(3,0));
    itemIds.append(TMPXItemId(4,0));
    
    // second list, make sure data is united
    mTestObject->requestDelete(itemIds);
    QVERIFY(mTestObject->mRemoveBuffer.count() == 5);
    
    QCoreApplication::processEvents();
    QVERIFY(mTestObject->mRemoveBuffer.count() == 2);
    
    // delete -call fails
    
    QSignalSpy spysignal(mTestObject, SIGNAL(deleteStartupFailed(QList<TMPXItemId>&)));
    mDummyCollection->mDeleteFails = true;
    itemIds.clear();
    itemIds.append(TMPXItemId(0,0));
    mTestObject->requestDelete(itemIds);
    QVERIFY(mTestObject->mRemoveBuffer.count() == 3);
    
    QCoreApplication::processEvents();
    QCOMPARE(mTestObject->mRemoveBuffer.count(), 0);
    QVERIFY(spysignal.count() == 1);   
    
}

// -----------------------------------------------------------------------------
// testRemoveFromRequest
// -----------------------------------------------------------------------------
//
void TestVideoDeleteWorker::testRemoveFromRequest()
{
    QList<TMPXItemId> itemIds;    
    QVERIFY(mTestObject->mRemoveBuffer.count() == 0);
   
    itemIds.append(TMPXItemId(0,0));
    itemIds.append(TMPXItemId(1,0));
    mTestObject->requestDelete(itemIds);
    TMPXItemId id = TMPXItemId(3,0);
    QVERIFY(mTestObject->removeFromRequest(id) == 2);
    id = TMPXItemId(1,0);
    QVERIFY(mTestObject->removeFromRequest(id) == 1);
    id = TMPXItemId(2,0);
    QVERIFY(mTestObject->removeFromRequest(id) == 1);
    id = TMPXItemId(0,0);
    QVERIFY(mTestObject->removeFromRequest(id) == 0);

}

// -----------------------------------------------------------------------------
// testIsDeleting
// -----------------------------------------------------------------------------
//
void TestVideoDeleteWorker::testIsDeleting()
{
    QList<TMPXItemId> itemIds;
    
    itemIds.append(TMPXItemId(0,0));
    itemIds.append(TMPXItemId(1,0));
    itemIds.append(TMPXItemId(2,0));
    itemIds.append(TMPXItemId(3,0));
    // list contains something
    mTestObject->requestDelete(itemIds);
    QVERIFY(mTestObject->mRemoveBuffer.count() == 4);
    QVERIFY(mTestObject->isDeleting());
    QCoreApplication::processEvents();
    QVERIFY(mTestObject->mRemoveBuffer.count() == 1);
    QVERIFY(mTestObject->isDeleting());
    itemIds.clear();
    mTestObject->requestDelete(itemIds);
    QCoreApplication::processEvents();
    QVERIFY(mTestObject->mRemoveBuffer.count() == 0);
    QVERIFY(!mTestObject->isDeleting()); 
}

// -----------------------------------------------------------------------------
// testUpdateStatus
// -----------------------------------------------------------------------------
//
void TestVideoDeleteWorker::testUpdateStatus()
{
    int status = 50;
    QVariant statusData = QVariant();
    // invalid status is not setted
    mTestObject->updateStatus(50, statusData);
    status = mTestObject->getLastStatus(statusData);
    QVERIFY(status == 0);
    QVERIFY(!statusData.isValid());
    
    // if status is not setted and provided as succeed -> setted as succeed 
    status = VideoCollectionCommon::statusDeleteSucceed;
    mTestObject->updateStatus(status, statusData);
    status = mTestObject->getLastStatus(statusData);
    QVERIFY(status == VideoCollectionCommon::statusDeleteSucceed);
    QVERIFY(!statusData.isValid());

    // if status is not setted or succeed provided as statusSingleDeleteFail -> setted as statusSingleDeleteFail 
    status = VideoCollectionCommon::statusSingleDeleteFail;
    mTestObject->updateStatus(status, statusData);
    status = mTestObject->getLastStatus(statusData);
    QVERIFY(status == VideoCollectionCommon::statusSingleDeleteFail);
    QVERIFY(!statusData.isValid());
    
    // if there is failed -status and provided status is succeed, no update
    status = VideoCollectionCommon::statusDeleteSucceed;
    mTestObject->updateStatus(status, statusData);
    status = mTestObject->getLastStatus(statusData);
    QVERIFY(status == VideoCollectionCommon::statusSingleDeleteFail);
    QVERIFY(!statusData.isValid());
    
    // current status single failed, provided multiple, count should be increased
    status = VideoCollectionCommon::statusMultipleDeleteFail;
    statusData = 2;
    mTestObject->updateStatus(status, statusData);
    status = mTestObject->getLastStatus(statusData);
    QVERIFY(status == VideoCollectionCommon::statusMultipleDeleteFail);
    QVERIFY(statusData.toInt() == 3);
    
    // current status single failed, provided is single failed, status changes into
    // multiple failed and count becomes 2
    mTestObject->mLastStatus = VideoCollectionCommon::statusSingleDeleteFail;
    status = VideoCollectionCommon::statusSingleDeleteFail;
    mTestObject->updateStatus(status, statusData);
    status = mTestObject->getLastStatus(statusData);
    QVERIFY(status == VideoCollectionCommon::statusMultipleDeleteFail);
    QVERIFY(statusData.toInt() == 2);
    
    // is status is multiplefailed and provided is either single or multiple, 
    // status is updated into multiple
    status = VideoCollectionCommon::statusSingleDeleteFail;
    statusData = QVariant();
    mTestObject->updateStatus(status, statusData);
    status = mTestObject->getLastStatus(statusData);
    QVERIFY(status == VideoCollectionCommon::statusMultipleDeleteFail);
    QVERIFY(statusData.toInt() == 3);
    
    status = VideoCollectionCommon::statusMultipleDeleteFail;
    statusData = 2;
    mTestObject->updateStatus(status, statusData);
    status = mTestObject->getLastStatus(statusData);
    QVERIFY(status == VideoCollectionCommon::statusMultipleDeleteFail);
    QVERIFY(statusData.toInt() == 5);
    
    mTestObject->clearStatus();
    status = mTestObject->getLastStatus(statusData);
    QVERIFY(status == 0);
    QVERIFY(!statusData.isValid());

}

// -----------------------------------------------------------------------------
// testContinueSlot
// -----------------------------------------------------------------------------
//
void TestVideoDeleteWorker::testContinueSlot()
{
    QList<TMPXItemId> itemIds;   
    mTestObject->requestDelete(itemIds);
    QCoreApplication::processEvents();
    
    // no timer
    QTimer *tmp = mTestObject->mRequestWaitTimer;
    mTestObject->mRequestWaitTimer = 0;
    mTestObject->continueSlot();
    // nothing to really check here
    QVERIFY(mTestObject->mRemoveBuffer.count() == 0);
    mTestObject->mRequestWaitTimer = tmp;
    
    // buffer empty
    mTestObject->continueSlot();
    QVERIFY(!mTestObject->mRequestWaitTimer->isActive());
     
    itemIds.append(TMPXItemId(0,0));
    itemIds.append(TMPXItemId(1,0));
    itemIds.append(TMPXItemId(2,0));
    itemIds.append(TMPXItemId(3,0));
    mTestObject->requestDelete(itemIds);
    mTestObject->mRequestWaitTimer->stop();
    // buffer not empty
    mTestObject->continueSlot();
    QVERIFY(mTestObject->mRequestWaitTimer->isActive());
    
    // timer is active
    mTestObject->continueSlot();
    QVERIFY(mTestObject->mRequestWaitTimer->isActive());
}

// end of file
