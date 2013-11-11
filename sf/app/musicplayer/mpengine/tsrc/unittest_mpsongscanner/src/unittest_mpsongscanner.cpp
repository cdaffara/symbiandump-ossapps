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
* Description: Unit test for MpSongScanner
*
*/

#include <QSignalSpy>

#include "stub/inc/mpmpxharvesterframeworkwrapper.h"
#include "unittest_mpsongscanner.h"


// Do this so we can access all member variables.
#define private public
#define protected public
#include "mpsongscanner.h"
#undef protected
#undef private


/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestMpSongScanner tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpsongscanner.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpSongScanner::TestMpSongScanner()
    : mTest( 0 ),
      mHarvesterWrapper( 0 )
{
}

TestMpSongScanner::~TestMpSongScanner()
{
    delete mTest;
    delete mHarvesterWrapper;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpSongScanner::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpSongScanner::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpSongScanner::init()
{
    qRegisterMetaType<MpxDiskEvents>("MpxDiskEvents");
    mHarvesterWrapper = new MpMpxHarvesterFrameworkWrapper();
    mTest = new MpSongScanner( mHarvesterWrapper );
}

/*!
 Called after every testfunction.
 */
void TestMpSongScanner::cleanup()
{
    delete mTest;
    mTest = 0;
    delete mHarvesterWrapper;
    mHarvesterWrapper = 0;
}

/*!
 Tests constructor.
*/
void TestMpSongScanner::testConstructor()
{
    QVERIFY( mTest->mMpxHarvesterWrapper );
    QVERIFY( !mTest->mScanning );
    QVERIFY( mTest->mAutomaticScan );
}

/*!
 testScan.
 */
void TestMpSongScanner::testScan()
{
    // Manual Scan
    mTest->scan( false );
    QVERIFY( mTest->mScanning );
    QVERIFY( !mTest->mAutomaticScan );
    QVERIFY( mTest->mMpxHarvesterWrapper->mScanRequested );

    // Automatic Scan
    cleanup();
    init();
    mTest->scan( true );
    QVERIFY( mTest->mScanning );
    QVERIFY( mTest->mAutomaticScan );
    QVERIFY( mTest->mMpxHarvesterWrapper->mScanRequested );
}

/*!
 testIsAutomaticScan.
 */
void TestMpSongScanner::testIsAutomaticScan()
{
    mTest->mAutomaticScan = true;
    QVERIFY( mTest->isAutomaticScan() );
    mTest->mAutomaticScan = false;
    QVERIFY( !mTest->isAutomaticScan() );
}

/*!
 testCancelScan
 */
void TestMpSongScanner::testCancelScan( )
{
    mTest->scan( true );
    mTest->cancelScan();
    QVERIFY( !mTest->mScanning );
    QVERIFY( !mTest->mMpxHarvesterWrapper->mScanRequested );
}

/*!
 testScanStartedSignal
 */
void TestMpSongScanner::testScanStartedSignal( )
{
    QSignalSpy spy( mTest, SIGNAL( scanStarted() ) );
    mTest->scan( true );
    mTest->mMpxHarvesterWrapper->emitScanStarted();
    QTest::qWait(500);
    QCOMPARE( spy.count(), 1 );
}

/*!
 testHandleScanEnded
 */
void TestMpSongScanner::testHandleScanEnded( )
{
    int itemsAdded( -1 );
    int error( KErrNone );

    // Scand endend with error KErrDiskFull 1/4
    {
    QSignalSpy spyScanFinished( mTest, SIGNAL( scanFinished( int, int ) ) );
    itemsAdded = 10;
    error = KErrDiskFull;
    mTest->scan( true );
    mTest->mMpxHarvesterWrapper->emitScanEnded( itemsAdded, error );
    QTest::qWait(500);
    QCOMPARE( spyScanFinished.count(), 1 );
    QCOMPARE( spyScanFinished.at(0).at(0), QVariant( MpSongScanner::ScanErrorDiskFull ) );
    QCOMPARE( spyScanFinished.at(0).at(1), QVariant( 0 ) );
    QVERIFY( !mTest->mScanning );
    }

    // Any other error 2/4
    {
    cleanup();
    init();
    QSignalSpy spyScanFinished( mTest, SIGNAL( scanFinished( int, int ) ) );
    itemsAdded = 10;
    error = KErrUnknown;
    mTest->scan( true );
    mTest->mMpxHarvesterWrapper->emitScanEnded( itemsAdded, error );
    QTest::qWait(500);
    QCOMPARE( spyScanFinished.count(), 1 );
    QCOMPARE( spyScanFinished.at(0).at(0), QVariant( MpSongScanner::ScanGeneralError ) );
    QCOMPARE( spyScanFinished.at(0).at(1), QVariant( itemsAdded ) );
    QVERIFY( !mTest->mScanning );
    }

    // Normal scan finish (No error, no cancel) 3/4
    {
    cleanup();
    init();
    QSignalSpy spyScanFinished( mTest, SIGNAL( scanFinished( int, int ) ) );
    itemsAdded = 20;
    error = KErrNone;
    mTest->scan( true );
    mTest->mMpxHarvesterWrapper->emitScanEnded( itemsAdded, error );
    QTest::qWait(500);
    QCOMPARE( spyScanFinished.count(), 1 );
    QCOMPARE( spyScanFinished.at(0).at(0), QVariant( MpSongScanner::ScanErrorNone ) );
    QCOMPARE( spyScanFinished.at(0).at(1), QVariant( itemsAdded ) );
    QVERIFY( !mTest->mScanning );
    }

    // Scan canceled 4/4
    {
    cleanup();
    init();
    QSignalSpy spyScanFinished( mTest, SIGNAL( scanFinished( int, int ) ) );
    itemsAdded = 20;
    error = KErrNone;
    mTest->scan( true );
    mTest->cancelScan();
    mTest->mMpxHarvesterWrapper->emitScanEnded( itemsAdded, error );
    QTest::qWait(500);
    QCOMPARE( spyScanFinished.count(), 1 );
    QCOMPARE( spyScanFinished.at(0).at(0), QVariant( MpSongScanner::ScanGeneralError ) );
    QCOMPARE( spyScanFinished.at(0).at(1), QVariant( itemsAdded ) );
    QVERIFY( !mTest->mScanning );
    }
}

/*!
 testScanCountChangedSignal
 */
void TestMpSongScanner::testScanCountChangedSignal()
{
    int itemsAdded = 15;
    QSignalSpy spy( mTest, SIGNAL( scanCountChanged( int ) ) );
    mTest->scan( true );
    mTest->mMpxHarvesterWrapper->emitScanCountChanged( itemsAdded );
    QTest::qWait(500);
    QCOMPARE( spy.count(), 1 );
    QCOMPARE( spy.at(0).at(0), QVariant( itemsAdded ) );
}

/*!
 testHandleDiskEvent
 */
void TestMpSongScanner::testHandleDiskEvent()
{
    QSignalSpy spyScanFinished( mTest, SIGNAL( scanFinished( int, int ) ) );
    mTest->scan( true );
    mTest->mMpxHarvesterWrapper->emitDiskEvent();
    QTest::qWait(500);
    QCOMPARE( spyScanFinished.count(), 1 );
    QCOMPARE( spyScanFinished.at(0).at(0), QVariant( MpSongScanner::ScanInterrupted ) );
    QCOMPARE( spyScanFinished.at(0).at(1), QVariant( 0 ) );
    QVERIFY( !mTest->mScanning );
}

//end of file
