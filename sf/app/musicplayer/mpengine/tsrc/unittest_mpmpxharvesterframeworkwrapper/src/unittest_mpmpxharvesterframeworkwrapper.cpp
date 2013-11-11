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
* Description: Unit test for mpmpxharvesterframeworkwrapper
*
*/

#include <QSignalSpy>

#include <mpxmedia.h>
#include <mpxcollectionplaylist.h>

#include "unittest_mpmpxharvesterframeworkwrapper.h"
#include "stub/inc/mpxcollectionutility.h"
#include "stub/inc/mpxharvesterutility.h"

// Do this so we can access all member variables.
#define private public
#include "mpmpxharvesterframeworkwrapper.h"
#include "mpmpxharvesterframeworkwrapper_p.h"
#undef private

//This so we can test private functions
#include "mpmpxharvesterframeworkwrapper_p.cpp"

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    TestMpMpxHarvesterFrameworkWrapper tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpmpxharvesterframeworkwrapper.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpMpxHarvesterFrameworkWrapper::TestMpMpxHarvesterFrameworkWrapper()
    : mTest(0)
{
}

TestMpMpxHarvesterFrameworkWrapper::~TestMpMpxHarvesterFrameworkWrapper()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpMpxHarvesterFrameworkWrapper::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpMpxHarvesterFrameworkWrapper::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpMpxHarvesterFrameworkWrapper::init()
{
    mTest = new MpMpxHarvesterFrameworkWrapper();
    mTestPrivate = mTest->d_ptr;
}

/*!
 Called after every testfunction.
 */
void TestMpMpxHarvesterFrameworkWrapper::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Tests constructor.
 */
void TestMpMpxHarvesterFrameworkWrapper::testConstructor()
{
    QVERIFY( mTestPrivate->iCollectionUtility != 0 );
    QVERIFY( mTestPrivate->iHarvesterUtility != 0 );
    QCOMPARE( mTestPrivate->iHarvesterUtility->iCheckSystemEvents, TBool(ETrue) );
}

/*!
 Tests scan
 */
void TestMpMpxHarvesterFrameworkWrapper::testScan()
{
    mTest->scan();
    QCOMPARE( mTestPrivate->iHarvesterUtility->iScan, TBool(ETrue) );
}

/*!
 Tests cancelScan
 */
void TestMpMpxHarvesterFrameworkWrapper::testCancelScan()
{
    mTestPrivate->iHarvesterUtility->iScan = ETrue;
    mTest->cancelScan();
    QCOMPARE( mTestPrivate->iHarvesterUtility->iScan, TBool(EFalse) );
    
    mTestPrivate->iHarvesterUtility->iScan = ETrue;
    mTestPrivate->iHarvesterUtility->iLeave = ETrue;
    mTest->cancelScan();
    QVERIFY( mTestPrivate->iHarvesterUtility != 0 );
    QCOMPARE( mTestPrivate->iHarvesterUtility->iCheckSystemEvents, TBool(ETrue) );
    QCOMPARE( mTestPrivate->iHarvesterUtility->iScan, TBool(EFalse) );
}

/*!
 Tests handleCollectionMessage for Refreshing events. Part of private implementation.
 */
void TestMpMpxHarvesterFrameworkWrapper::testHandleCollectionMessageRefreshing()
{
    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds );
    supportedIds.AppendL( KMPXMessageContentIdGeneral );
    CMPXMedia* testMessage = CMPXMedia::NewL( supportedIds.Array() );
    CleanupStack::PopAndDestroy( &supportedIds );
    CleanupStack::PushL( testMessage );
    testMessage->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId, KMPXMessageGeneral );
        
    //Refresh ended
    QSignalSpy spy( mTest, SIGNAL( scanEnded( int, int ) ) );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralEvent, TMPXCollectionMessage::EBroadcastEvent );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralType, EMcMsgRefreshEnd );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralData, KErrCancel);
    mTestPrivate->DoHandleCollectionMessageL( *testMessage );
    QCOMPARE( mTestPrivate->iScanning, TBool( EFalse ) );   
    QCOMPARE( spy.count(), 1 );
    QList<QVariant> arguments = spy.takeFirst();
    TInt count = arguments.at(0).toInt();    
    QCOMPARE(count, mTestPrivate->iNumItemsAdded);    
    TInt error = arguments.at(1).toInt();
    QCOMPARE( error, KErrCancel );
        
    //Refresh started
    cleanup();
    init();
    QSignalSpy spy2(mTest, SIGNAL( scanStarted( void ) ) );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralEvent, TMPXCollectionMessage::EBroadcastEvent );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralType, EMcMsgRefreshStart );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralData, 0 );
    mTestPrivate->DoHandleCollectionMessageL( *testMessage );
    QCOMPARE( mTestPrivate->iScanning, TBool( ETrue ) );   
    QCOMPARE( spy2.count(), 1 );       

    //Items changed
    cleanup();
    init();
    QSignalSpy spy3( mTest, SIGNAL( scanCountChanged( int ) ) );
    mTest->scan();
    
    //Emulate Harvester started refreshing
    mTestPrivate->DoHandleCollectionMessageL( *testMessage );
    
    RArray<TInt> supportedIds2;
    CleanupClosePushL( supportedIds2 );
    supportedIds2.AppendL( KMPXMessageContentIdGeneral );
    supportedIds2.AppendL( KMPXMessageIdContainer );
    CMPXMedia* testMessage2 = CMPXMedia::NewL( supportedIds2.Array() );
    CleanupStack::PopAndDestroy( &supportedIds2 );
    CleanupStack::PushL( testMessage2 );
        
    CMPXMessageArray* array = CMPXMessageArray::NewL();
    CleanupStack::PushL(array);
    //EMPXNoCategory, EMPXCollection, EMPXPlaylist, EMPXSong, EMPXPodcast three possible and two extra items
    for (TInt i =0; i < 5; i++) {
        RArray<TInt> supportedIds3;
        CleanupClosePushL( supportedIds3 );
        supportedIds2.AppendL( KMPXMessageContentIdGeneral );
        CMPXMedia* media = CMPXMedia::NewL(supportedIds3.Array());
        CleanupStack::PopAndDestroy(&supportedIds3);
        CleanupStack::PushL(media);
        media->SetTObjectValueL<TMPXChangeEventType>(KMPXMessageChangeEventType, EMPXItemInserted);
        media->SetTObjectValueL<TMPXGeneralCategory>(KMPXMessageMediaGeneralCategory, TMPXGeneralCategory(i));
        media->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId, KMPXMessageIdItemChanged);
        array->AppendL( *media );
        CleanupStack::PopAndDestroy( media );
    }
    testMessage2->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId, KMPXMessageIdItemChanged); 
    testMessage2->SetCObjectValueL( KMPXMessageArrayContents, array );
    testMessage2->SetTObjectValueL<TInt>( KMPXMessageArrayCount, array->Count() );
    CleanupStack::PopAndDestroy( array );
    mTestPrivate->DoHandleCollectionMessageL( *testMessage2 );
    CleanupStack::PopAndDestroy( testMessage2 );
    QCOMPARE( mTestPrivate->iNumItemsAdded, 3 );
    QCOMPARE( spy3.count(), 1 );
    TInt itemsCount = qvariant_cast<int>( spy3.at( 0 ).at( 0 ) );
    QCOMPARE( itemsCount, mTestPrivate->iNumItemsAdded );
    
    CleanupStack::PopAndDestroy( testMessage );
}

/*!
 Tests handleCollectionMessage for Disk events. Part of private implementation.
 */
void TestMpMpxHarvesterFrameworkWrapper::testHandleCollectionMessageDisk()
{
    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds );
    supportedIds.AppendL( KMPXMessageContentIdGeneral );
    CMPXMedia* testMessage = CMPXMedia::NewL( supportedIds.Array() );
    CleanupStack::PopAndDestroy( &supportedIds );
    CleanupStack::PushL( testMessage );
    testMessage->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId, KMPXMessageGeneral );
        
    qRegisterMetaType<MpxDiskEvents>("MpxDiskEvents");

    //Format started
    QSignalSpy spy( mTest, SIGNAL( diskEvent( MpxDiskEvents ) ) );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralEvent, TMPXCollectionMessage::EBroadcastEvent );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralType, EMcMsgFormatStart );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralData, 0);
    mTestPrivate->DoHandleCollectionMessageL( *testMessage );
    QCOMPARE( spy.count(), 1 );
    TInt diskEvent = qvariant_cast<MpxDiskEvents>( spy.at( 0 ).at( 0 ) );
    QVERIFY( diskEvent == DiskFormatStarted );    
        
    //Format ended
    cleanup();
    init();
    QSignalSpy spy2( mTest, SIGNAL( diskEvent( MpxDiskEvents ) ) );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralEvent, TMPXCollectionMessage::EBroadcastEvent );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralType, EMcMsgFormatEnd );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralData, 0);
    mTestPrivate->DoHandleCollectionMessageL( *testMessage );
    QCOMPARE( spy2.count(), 1 );
    TInt diskEvent2 = qvariant_cast<MpxDiskEvents>( spy2.at( 0 ).at( 0 ) );
    QVERIFY( diskEvent2 == DiskFormatEnded );
    
    //Disk removed
    cleanup();
    init();
    QSignalSpy spy3( mTest, SIGNAL( diskEvent( MpxDiskEvents ) ) );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralEvent, TMPXCollectionMessage::EBroadcastEvent );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralType, EMcMsgDiskRemoved );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralData, 0);
    mTestPrivate->DoHandleCollectionMessageL( *testMessage );
    QCOMPARE( spy3.count(), 1 );
    TInt diskEvent3 = qvariant_cast<MpxDiskEvents>( spy3.at( 0 ).at( 0 ) );
    QVERIFY( diskEvent3 == DiskRemoved );
    
    //Disk inserted
    cleanup();
    init();
    QSignalSpy spy4( mTest, SIGNAL( diskEvent( MpxDiskEvents ) ) );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralEvent, TMPXCollectionMessage::EBroadcastEvent );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralType, EMcMsgDiskInserted );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralData, 0);
    mTestPrivate->DoHandleCollectionMessageL( *testMessage );
    QCOMPARE( spy4.count(), 1 );
    TInt diskEvent4 = qvariant_cast<MpxDiskEvents>( spy4.at( 0 ).at( 0 ) );
    QVERIFY( diskEvent4 == DiskInserted );
    
    CleanupStack::PopAndDestroy( testMessage );
}

/*!
 Tests handleCollectionMessage for USB events. Part of private implementation.
 */
void TestMpMpxHarvesterFrameworkWrapper::testHandleCollectionMessageUSB()
{
    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds );
    supportedIds.AppendL( KMPXMessageContentIdGeneral );
    CMPXMedia* testMessage = CMPXMedia::NewL( supportedIds.Array() );
    CleanupStack::PopAndDestroy( &supportedIds );
    CleanupStack::PushL( testMessage );
    testMessage->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId, KMPXMessageGeneral );
        
    qRegisterMetaType<MpxUsbEvents>("MpxUsbEvents");

    //USB Mass storage started
    QSignalSpy spy( mTest, SIGNAL( usbEvent( MpxUsbEvents ) ) );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralEvent, TMPXCollectionMessage::EBroadcastEvent );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralType, EMcMsgUSBMassStorageStart );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralData, 0);
    mTestPrivate->DoHandleCollectionMessageL( *testMessage );
    QCOMPARE( spy.count(), 1 );
    TInt usbEvent = qvariant_cast<MpxUsbEvents>( spy.at( 0 ).at( 0 ) );
    QVERIFY( usbEvent == UsbMassStorageStarted );    
        
    //USB Mass storage ended
    cleanup();
    init();
    QSignalSpy spy2( mTest, SIGNAL( usbEvent( MpxUsbEvents ) ) );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralEvent, TMPXCollectionMessage::EBroadcastEvent );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralType, EMcMsgUSBMassStorageEnd );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralData, 0);
    mTestPrivate->DoHandleCollectionMessageL( *testMessage );
    QCOMPARE( spy2.count(), 1 );
    TInt usbEvent2 = qvariant_cast<MpxUsbEvents>( spy2.at( 0 ).at( 0 ) );
    QVERIFY( usbEvent2 == UsbMassStorageEnded );
    
    //USB MTP started
    cleanup();
    init();
    QSignalSpy spy3( mTest, SIGNAL( usbEvent( MpxUsbEvents ) ) );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralEvent, TMPXCollectionMessage::EBroadcastEvent );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralType, EMcMsgUSBMTPStart );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralData, 0);
    mTestPrivate->DoHandleCollectionMessageL( *testMessage );
    QCOMPARE( spy3.count(), 1 );
    TInt usbEvent3 = qvariant_cast<MpxUsbEvents>( spy3.at( 0 ).at( 0 ) );
    QVERIFY( usbEvent3 == UsbMtpStarted );
    
    //USB MTP ended
    cleanup();
    init();
    QSignalSpy spy4( mTest, SIGNAL( usbEvent( MpxUsbEvents ) ) );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralEvent, TMPXCollectionMessage::EBroadcastEvent );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralType, EMcMsgUSBMTPEnd );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralData, 0);
    mTestPrivate->DoHandleCollectionMessageL( *testMessage );
    QCOMPARE( spy4.count(), 1 );
    TInt usbEvent4 = qvariant_cast<MpxUsbEvents>( spy4.at( 0 ).at( 0 ) );
    QVERIFY( usbEvent4 == UsbMtpEnded );
    
    //USB MTP NotActive
    cleanup();
    init();
    QSignalSpy spy5( mTest, SIGNAL( usbEvent( MpxUsbEvents ) ) );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralEvent, TMPXCollectionMessage::EBroadcastEvent );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralType, EMcMsgUSBMTPNotActive );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralData, 0);
    mTestPrivate->DoHandleCollectionMessageL( *testMessage );
    QCOMPARE( spy5.count(), 1 );
    TInt usbEvent5 = qvariant_cast<MpxUsbEvents>( spy5.at( 0 ).at( 0 ) );
    QVERIFY( usbEvent5 == UsbMtpNotActive );
    
    CleanupStack::PopAndDestroy( testMessage );
}

/*!
 Tests broadcastEventFilter. Part of private implementation.
 */
void TestMpMpxHarvesterFrameworkWrapper::testBroadcastEventFilter()
{
    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds );
    supportedIds.AppendL( KMPXMessageContentIdGeneral );
    CMPXMedia* testMessage = CMPXMedia::NewL( supportedIds.Array() );
    CleanupStack::PopAndDestroy( &supportedIds );
    CleanupStack::PushL( testMessage );
    testMessage->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId, KMPXMessageGeneral );
        
    qRegisterMetaType<MpxUsbEvents>("MpxUsbEvents");

    //Verify default values:
    QVERIFY( mTestPrivate->iPreviousBroadCastMsg == NoPreviousBroadCastMsg ); 
    
    //Send USB Mass storage started message, usbEvent signal should be emitted
    QSignalSpy spy( mTest, SIGNAL( usbEvent( MpxUsbEvents ) ) );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralEvent, TMPXCollectionMessage::EBroadcastEvent );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralType, EMcMsgUSBMassStorageStart );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralData, 0);
    mTestPrivate->DoHandleCollectionMessageL( *testMessage );    
    QVERIFY( mTestPrivate->iPreviousBroadCastMsg == EMcMsgUSBMassStorageStart );
    QCOMPARE( spy.count(), 1 );    
    TInt usbEvent = qvariant_cast<MpxUsbEvents>( spy.at( 0 ).at( 0 ) );
    QVERIFY( usbEvent == UsbMassStorageStarted );
    
    //Send for second time USB Mass storage started message, same message no new signal should be emitted
    mTestPrivate->DoHandleCollectionMessageL( *testMessage );
    QVERIFY( mTestPrivate->iPreviousBroadCastMsg == EMcMsgUSBMassStorageStart );
    QCOMPARE( spy.count(), 1 );    
        
    //Send USB Mass storage ended, different message new signal should be emitted
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralEvent, TMPXCollectionMessage::EBroadcastEvent );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralType, EMcMsgUSBMassStorageEnd );
    testMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralData, 0);
    mTestPrivate->DoHandleCollectionMessageL( *testMessage );
    QVERIFY( mTestPrivate->iPreviousBroadCastMsg == EMcMsgUSBMassStorageEnd );
    QCOMPARE( spy.count(), 2 );
    TInt usbEvent2 = qvariant_cast<MpxUsbEvents>( spy.at( 1 ).at( 0 ) );
    QVERIFY( usbEvent2 == UsbMassStorageEnded );
    
    CleanupStack::PopAndDestroy( testMessage );
}

/*!
 Tests testCheckForSystemEvents. Part of private implementation.
 */
void TestMpMpxHarvesterFrameworkWrapper::testCheckForSystemEvents()
{
    mTest->checkForSystemEvents();
    QVERIFY( mTestPrivate->iPreviousBroadCastMsg == NoPreviousBroadCastMsg );    
    QCOMPARE( mTestPrivate->iHarvesterUtility->iCheckSystemEvents, TBool(ETrue) );
}

//end of file
