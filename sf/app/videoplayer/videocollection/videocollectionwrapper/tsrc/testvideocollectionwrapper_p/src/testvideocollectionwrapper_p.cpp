
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
* Description:   tester for methods in VideoCollectionWrapperPrivate
* 
*/


// INCLUDES
#include <qdebug.h>
#include <qabstractitemmodel.h>
#include <hbapplication.h>
#include "testvideocollectionwrapper_p.h"
#include "videocollectionwrapper.h"
#include "videocollectioncommon.h"
#include "videolistdatamodel.h"
#include "videoproxymodelgeneric.h"
#include "videocollectionclient.h"
#include "vcxmyvideosdefs.h"

#define private public
#include "videocollectionwrapper_p.h"
#undef private

// -----------------------------------------------------------------------------
// main
// -----------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    TestVideCollectionWrapper_p tv;
    HbApplication app(argc, argv);
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
        pass[2] = "c:\\data\\testvideocollectionwrapper_p.txt";
        res = QTest::qExec(&tv, 3, pass);
    }
    
    return res;
}


// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
//
void TestVideCollectionWrapper_p::init()
{
    mTestObject = new VideoCollectionWrapperPrivate();
    QVERIFY(mTestObject);
}

// -----------------------------------------------------------------------------
// cleanup
// -----------------------------------------------------------------------------
//
void TestVideCollectionWrapper_p::cleanup()
{
    delete mTestObject;  
    mTestObject = 0;
}

// -----------------------------------------------------------------------------
// testGetModelInitFail
// -----------------------------------------------------------------------------
//
void TestVideCollectionWrapper_p::testGetModelInitFail()
{
    QVERIFY(mTestObject);        
    
    VideoProxyModelGeneric *proxyGotten = 0;
    
    VideoListDataModel::mInitFails = true;
    // Test source model init fails
    
    proxyGotten = mTestObject->getGenericModel();
    QVERIFY(!proxyGotten);
    
    proxyGotten = mTestObject->getAllVideosModel();
    QVERIFY(!proxyGotten);
    
    proxyGotten = mTestObject->getCollectionsModel();
    QVERIFY(!proxyGotten);
    
    proxyGotten = mTestObject->getCollectionContentModel();
    QVERIFY(!proxyGotten);
}
 
// -----------------------------------------------------------------------------
// testGetModelSucceed
// -----------------------------------------------------------------------------
//
void TestVideCollectionWrapper_p::testGetModelSucceed()
{
    QVERIFY(mTestObject);        

    VideoListDataModel::mInitFails = false;
    
    VideoProxyModelGeneric *proxyGotten = 0;
    VideoProxyModelGeneric *proxyGotten2 = 0;

    proxyGotten = mTestObject->getAllVideosModel();
    QVERIFY(proxyGotten);
    
    QString name = proxyGotten->metaObject()->className();
    QString expectedName = QString("VideoProxyModelAllVideos");
    QVERIFY(name == expectedName);
    // again
    proxyGotten2 = mTestObject->getAllVideosModel();
    QCOMPARE(proxyGotten, proxyGotten2);
    delete proxyGotten;
    proxyGotten = 0;

    proxyGotten = mTestObject->getCollectionsModel();
    QVERIFY(proxyGotten);
    name = proxyGotten->metaObject()->className();
    expectedName = QString("VideoProxyModelCollections");
    QVERIFY(name == expectedName);
    // again
    proxyGotten2 = mTestObject->getCollectionsModel();
    QCOMPARE(proxyGotten, proxyGotten2);
    delete proxyGotten;
    proxyGotten = 0;
    
    proxyGotten = mTestObject->getCollectionContentModel();
    QVERIFY(proxyGotten);
    name = proxyGotten->metaObject()->className();
    expectedName = QString("VideoProxyModelContent");
    QVERIFY(name == expectedName);
    // again
    proxyGotten2 = mTestObject->getCollectionContentModel();
    QCOMPARE(proxyGotten, proxyGotten2);
    delete proxyGotten;
    proxyGotten = 0;

    proxyGotten = mTestObject->getGenericModel();
    QVERIFY(proxyGotten);
    name = proxyGotten->metaObject()->className();
    expectedName = QString("VideoProxyModelGeneric");
    QVERIFY(name == expectedName);
    // again
    proxyGotten2 = mTestObject->getGenericModel();
    QCOMPARE(proxyGotten, proxyGotten2);
    delete proxyGotten;
    proxyGotten = 0;
}

// -----------------------------------------------------------------------------
// testAboutToQuitSlot
// -----------------------------------------------------------------------------
//
void TestVideCollectionWrapper_p::testAboutToQuitSlot()
{
    QVERIFY(mTestObject);        
    connect(this, SIGNAL(testSignal()), mTestObject, SLOT(aboutToQuitSlot()));
    
    VideoProxyModelGeneric *proxyGotten = 0;
    proxyGotten = mTestObject->getAllVideosModel();
    QVERIFY(proxyGotten);
    QVERIFY(!mTestObject->mSourceModel.isNull());
    
    emit testSignal();
    QVERIFY(mTestObject->mSourceModel.isNull());
    proxyGotten = mTestObject->getAllVideosModel();
    QVERIFY(!proxyGotten);
    
    emit testSignal();
    QVERIFY(mTestObject->mSourceModel.isNull());
    proxyGotten = mTestObject->getAllVideosModel();
    QVERIFY(!proxyGotten);

    disconnect(this, SIGNAL(testSignal()), mTestObject, SLOT(aboutToQuitSlot()));
}

// End of file
    


