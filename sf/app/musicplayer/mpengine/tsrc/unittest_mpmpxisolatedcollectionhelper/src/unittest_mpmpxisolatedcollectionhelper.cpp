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
* Description: Unit test for CMpMpxIsolatedCollectionHelper
*
*/

#include <mpxmedia.h>
#include <mpxcollectionplaylist.h>
#include <mpxcollectionpath.h>

#include "unittest_mpmpxisolatedcollectionhelper.h"


//This so we can test private functions
#include "../../src/mpmpxisolatedcollectionhelper.cpp"


/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    TestCMpMpxIsolatedCollectionHelper tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpmpxisolatedcollectionhelper.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestCMpMpxIsolatedCollectionHelper::TestCMpMpxIsolatedCollectionHelper()
    : mTest( 0 ),
      iEntriesFromoOpenCallback ( 0 ),
      iErrorFromoOpenCallback( KErrNone ),
      iOpenCallbackCounter( 0 ),
      iOpenRestoreCallbackCounter( 0 )
{
}

TestCMpMpxIsolatedCollectionHelper::~TestCMpMpxIsolatedCollectionHelper()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestCMpMpxIsolatedCollectionHelper::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestCMpMpxIsolatedCollectionHelper::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestCMpMpxIsolatedCollectionHelper::init()
{
    mTest = CMpMpxIsolatedCollectionHelper::NewL( this );
}

/*!
 Called after every testfunction.
 */
void TestCMpMpxIsolatedCollectionHelper::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Tests constructor.
*/
void TestCMpMpxIsolatedCollectionHelper::testConstructor()
{
    QVERIFY(mTest->iIncrementalOpenUtil != 0);
    QCOMPARE(mTest->iIncrementalOpenUtil->iMode , KMcModeIsolated);
    QCOMPARE(mTest->iObserver , dynamic_cast<MMpMpxIsolatedCollectionHelperObserver*>(this));
}

/*!
 Tests OpenCollection.
 */
void TestCMpMpxIsolatedCollectionHelper::testOpenCollection()
{
    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    mTest->OpenCollectionL(*path);
    QCOMPARE(mTest->iIncrementalOpenUtil->iDelay, KIncrementalDelay);
    QCOMPARE(mTest->iIncrementalOpenUtil->iStop,TBool(ETrue));
    QCOMPARE(mTest->iIncrementalOpenUtil->iStart,TBool(ETrue));
    QCOMPARE(mTest->iFirstIncrementalOpen,TBool(ETrue));
    QCOMPARE(mTest->iIncrementalOpenUtil->iDirection,CMPXCollectionOpenUtility::EFetchDown);
    QCOMPARE(mTest->iIncrementalOpenUtil->iChunkSize, KIncrementalFetchBlockSize);
    QCOMPARE((int)mTest->iIncrementalOpenUtil->iPath, (int)path);
    QCOMPARE(mTest->iOpenMode, CMpMpxIsolatedCollectionHelper::DefaultMode );
    
    mTest->OpenCollectionL(*path,0,CMpMpxIsolatedCollectionHelper::RestorePathMode);
    QCOMPARE(mTest->iIncrementalOpenUtil->iDelay, KIncrementalDelay);
    QCOMPARE(mTest->iIncrementalOpenUtil->iStop,TBool(ETrue));
    QCOMPARE(mTest->iIncrementalOpenUtil->iStart,TBool(ETrue));
    QCOMPARE(mTest->iFirstIncrementalOpen,TBool(ETrue));
    QCOMPARE(mTest->iIncrementalOpenUtil->iDirection,CMPXCollectionOpenUtility::EFetchDown);
    QCOMPARE(mTest->iIncrementalOpenUtil->iChunkSize, KIncrementalFetchBlockSize);
    QCOMPARE((int)mTest->iIncrementalOpenUtil->iPath, (int)path);
    QCOMPARE(mTest->iOpenMode, CMpMpxIsolatedCollectionHelper::RestorePathMode );
    
    delete path;
}

/*!
 Tests HandleOpen.
 */
void TestCMpMpxIsolatedCollectionHelper::testHandleOpen()
{
     //Default Mode
     iEntriesFromoOpenCallback = 0;
     iErrorFromoOpenCallback = KErrArgument;
     iOpenCallbackCounter = 0;
     CMPXMedia *media = CMPXMedia::NewL();
     mTest->iFirstIncrementalOpen = ETrue;
     mTest->HandleOpenL(*media, 0, false, KErrNone);
     QCOMPARE((int)iEntriesFromoOpenCallback,(int)media);
     QCOMPARE(iErrorFromoOpenCallback,KErrNone);
     QCOMPARE(iOpenCallbackCounter,1);
     QCOMPARE(iOpenRestoreCallbackCounter,0);
     
     //RestorePath mode
     CMPXCollectionPath* path = CMPXCollectionPath::NewL();
     mTest->iFirstIncrementalOpen = ETrue;
     mTest->iOpenMode = CMpMpxIsolatedCollectionHelper::RestorePathMode;
     mTest->HandleOpenL(*media, 0, false, KErrNotFound);
     
     QCOMPARE(iErrorFromoOpenCallback,KErrNotFound);
     QCOMPARE(iOpenCallbackCounter,1);
     QCOMPARE(iOpenRestoreCallbackCounter,1);
     
}
 
/*!
 Used to keep track of the callback from the isolated collection helper.
 */
void TestCMpMpxIsolatedCollectionHelper::HandleIsolatedOpenL( const CMPXMedia& aEntries, TInt aError )
{
     iEntriesFromoOpenCallback = &aEntries;
     iErrorFromoOpenCallback = aError;
     iOpenCallbackCounter++;
}

/*!
 Used to keep track of the callback from the isolated collection helper.
 */
void TestCMpMpxIsolatedCollectionHelper::HandleIsolatedOpenRestorePathL( const CMPXCollectionPath& aPath, TInt aError )
{
    Q_UNUSED( aPath );
    iErrorFromoOpenCallback = aError;
    iOpenRestoreCallbackCounter++;
}
//end of file
