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
* Description:  Videoplayerengine test class implementation.
*
*/

// Version : %version:  1 %

// INCLUDES

#include <QtTest/QtTest>
#include <qdebug.h>
#include <hbapplication.h>

#include "mpxhbvideocommondefs.h"
#include "testvideoiadupdatewrapper.h"
#include "mpxvideo_debug.h"

#include "iaupdate.h"
#include "iaupdateparameters.h"
#include "iaupdateresult.h"

#define private public
#include "videoiadupdatewrapper.h"
#undef private

extern int IsFeatureSupported;

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);

    TestVideoIadUpdateWrapper tc;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\TestVideoIadUpdateWrapper.txt";

    return QTest::qExec(&tc, 3, pass);
}


TestVideoIadUpdateWrapper::TestVideoIadUpdateWrapper()
    : mTestObject( 0 )
{
    MPX_DEBUG(_L("TestVideoIadUpdateWrapper::TestVideoIadUpdateWrapper()"));
}

TestVideoIadUpdateWrapper::~TestVideoIadUpdateWrapper()
{
    MPX_ENTER_EXIT(_L("TestVideoIadUpdateWrapper::~TestVideoIadUpdateWrapper()"));
    
    delete mTestObject;
    mTestObject = 0;
}


void TestVideoIadUpdateWrapper::init()
{
    MPX_DEBUG(_L("TestVideoIadUpdateWrapper::init()"));
    
    mTestObject = new VideoIadUpdateWrapper();
}

void TestVideoIadUpdateWrapper::cleanup()
{
    MPX_ENTER_EXIT(_L("TestVideoIadUpdateWrapper::cleanup()"));

    delete mTestObject;
    mTestObject = 0;
}

void TestVideoIadUpdateWrapper::TestCreateDelete()
{
    MPX_DEBUG(_L("TestVideoIadUpdateWrapper::testCreateDelete()"));
    
    init();

    QVERIFY( mTestObject );
    QVERIFY( mTestObject->mUpdate == 0);
    QVERIFY( mTestObject->mParameters == 0);
    
    cleanup();

    QVERIFY( mTestObject == 0 );
}

void TestVideoIadUpdateWrapper::cleanupTestCase()
{
    MPX_DEBUG(_L("TestVideoIadUpdateWrapper::cleanupTestCase()"));
    // all common cleanup here
}

void TestVideoIadUpdateWrapper::TestCheckForUpdates()
{
    MPX_DEBUG(_L("TestVideoIadUpdateWrapper::testCheckForUpdates()"));

    init();

    QVERIFY( mTestObject );
    
    IsFeatureSupported = 1;
    
    mTestObject->checkForUpdates();

    QVERIFY( mTestObject );
    QVERIFY( mTestObject->mUpdate != 0);
    QVERIFY( mTestObject->mParameters != 0);
    
    cleanup();
}

void TestVideoIadUpdateWrapper::TestCheckUpdatesComplete()
{
    MPX_DEBUG(_L("TestVideoIadUpdateWrapper::TestCheckUpdatesComplete()"));

    init();

    QVERIFY( mTestObject );
    
    // error case: do nothing
    TInt errorCode = 1;
    TInt availableUpdates = 0;
    
    mTestObject->CheckUpdatesComplete(errorCode, availableUpdates);

    QVERIFY( mTestObject );
    QVERIFY( mTestObject->mUpdate == 0);
    QVERIFY( mTestObject->mParameters == 0);

    // update available
    errorCode=0;
    availableUpdates=1;
    
    mTestObject->mUpdate = CIAUpdate::NewL( *mTestObject );
    mTestObject->mParameters = CIAUpdateParameters::NewL();
    mTestObject->CheckUpdatesComplete(errorCode, availableUpdates);

    QVERIFY( mTestObject );
    QVERIFY( mTestObject->mUpdate != 0);
    QVERIFY( mTestObject->mParameters != 0);    
    
    // no update, everything should be deleted
    errorCode = 0;
    availableUpdates = 0;
    
    mTestObject->CheckUpdatesComplete(errorCode, availableUpdates);

    QVERIFY( mTestObject );
    QVERIFY( mTestObject->mUpdate == 0);
    QVERIFY( mTestObject->mParameters == 0);
    

    
    cleanup();
}

void TestVideoIadUpdateWrapper::TestUpdateComplete()
{
    MPX_DEBUG(_L("TestVideoIadUpdateWrapper::TestUpdateComplete()"));

    init();

    QVERIFY( mTestObject );
    TInt errorCode(0);
    CIAUpdateResult* resultDetails = new CIAUpdateResult;    
    mTestObject->UpdateComplete(errorCode, resultDetails);

    QVERIFY( mTestObject );
    QVERIFY( mTestObject->mUpdate == 0);
    QVERIFY( mTestObject->mParameters == 0);
    
    cleanup();
}

void TestVideoIadUpdateWrapper::TestUpdateQueryComplete()
{
    MPX_DEBUG(_L("TestVideoIadUpdateWrapper::TestUpdateQueryComplete()"));

    init();

    QVERIFY( mTestObject );
    
    // no error, no update: cleans up the members.
    
    TInt errorCode(0);
    TBool updateNow(0);

    mTestObject->mUpdate = CIAUpdate::NewL( *mTestObject );
    mTestObject->mParameters = CIAUpdateParameters::NewL();
    
    mTestObject->UpdateQueryComplete(errorCode, updateNow);
    
    QVERIFY( mTestObject );
    QVERIFY( mTestObject->mUpdate == 0);
    QVERIFY( mTestObject->mParameters == 0);
    
    // error case: does nothing
    
    errorCode = 1;
    updateNow = 0;
    
    mTestObject->UpdateQueryComplete(errorCode, updateNow);
    
    QVERIFY( mTestObject );
    QVERIFY( mTestObject->mUpdate == 0);
    QVERIFY( mTestObject->mParameters == 0);
    
    // the update case: should preserve members, strats the update.
    
    errorCode = 0;
    updateNow = 1;
    
    mTestObject->mUpdate = CIAUpdate::NewL(  *mTestObject  );
    mTestObject->mParameters = CIAUpdateParameters::NewL();
    
    mTestObject->UpdateQueryComplete(errorCode, updateNow);
    
    QVERIFY( mTestObject );
    QVERIFY( mTestObject->mUpdate != 0);
    QVERIFY( mTestObject->mParameters != 0);
    
    cleanup();
}

void TestVideoIadUpdateWrapper::TestDoCheckForUpdatesL()
{
    MPX_DEBUG(_L("TestVideoIadUpdateWrapper::TestdoCheckForUpdatesL()"));

    init();

    QVERIFY( mTestObject );
    
    // feature not supported

    IsFeatureSupported = 0;
    mTestObject->doCheckForUpdatesL();

    QVERIFY( mTestObject );
    QVERIFY( mTestObject->mUpdate == 0);
    QVERIFY( mTestObject->mParameters == 0);

    // feature  supported, initializes the data
    
    IsFeatureSupported = 1;
    mTestObject->doCheckForUpdatesL();

    QVERIFY( mTestObject );
    QVERIFY( mTestObject->mUpdate != 0);
    QVERIFY( mTestObject->mParameters != 0);
    
    cleanup();
}

void TestVideoIadUpdateWrapper::TestCleanup()
{
    MPX_DEBUG(_L("TestVideoIadUpdateWrapper::Testcleanup()"));

    init();

    QVERIFY( mTestObject );
    
    mTestObject->cleanup();

    QVERIFY( mTestObject );
    QVERIFY( mTestObject->mUpdate == 0);
    QVERIFY( mTestObject->mParameters == 0);
    
    cleanup();
}

// End of file

