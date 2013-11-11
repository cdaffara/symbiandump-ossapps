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
* Description: Unit test for mpengine
*
*/

#include <QSignalSpy>
#include <EqualizerConstants.h>

#include "unittest_mpengine.h"
#include "mpmediakeyhandler.h"
#include "mpmpxcollectionviewdefs.h"

#define private public
#include "mpengine.h"
#undef private

// help to test private function 
#include "../../src/mpengine.cpp"

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TestMpEngine tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpengine.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpEngine::TestMpEngine()
    : mTest(0)
{
}

TestMpEngine::~TestMpEngine()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpEngine::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpEngine::cleanupTestCase()
{
    MpEngineFactory::close();
}

/*!
 Called before each testfunction is executed.
 */
void TestMpEngine::init()
{

}

/*!
 Called after every testfunction.
 */
void TestMpEngine::cleanup()
{

}

/*!
 test initialize
 */
void TestMpEngine::testInitialize()
{
    mTest = MpEngineFactory::createIsolatedEngine( MpEngine::StandAlone );
    QVERIFY( mTest->mSongData != 0 );
    QVERIFY( mTest->mMpxHarvesterWrapper != 0 );
    QVERIFY( mTest->mMpxCollectionWrapper != 0 );
    QVERIFY( mTest->mEqualizerWrapper != 0 );
    QVERIFY( mTest->mMpxPlaybackWrapper != 0 );
    QVERIFY( mTest->mAudioEffectsWrapper != 0 );
    
    mTest = MpEngineFactory::createIsolatedEngine( MpEngine::Fetch );
    QVERIFY( mTest->mMpxHarvesterWrapper != 0 );
    QVERIFY( mTest->mMpxCollectionWrapper != 0 );
    QVERIFY( mTest->mMpxPlaybackWrapper != 0 );
    QVERIFY( mTest->mEqualizerWrapper == 0 );
    QVERIFY( mTest->mAudioEffectsWrapper == 0 );
    
    mTest = MpEngineFactory::createIsolatedEngine( MpEngine::MediaBrowsing );
    QVERIFY( mTest->mMpxCollectionWrapper != 0 );
    QVERIFY( mTest->mMpxHarvesterWrapper == 0 );
    QVERIFY( mTest->mMpxPlaybackWrapper == 0 );
    QVERIFY( mTest->mEqualizerWrapper == 0 );
    QVERIFY( mTest->mAudioEffectsWrapper == 0 );
    
    mTest = MpEngineFactory::createIsolatedEngine( MpEngine::Embedded );
    QVERIFY( mTest->mMpxPlaybackWrapper != 0 );
    QVERIFY( mTest->mSongData != 0 );
}

/*!
 test verifyUsbBlocking
 */
void TestMpEngine::testVerifyUsbBlocking() 
{
    bool result;
    QSignalSpy spy( mTest, SIGNAL( unableToCotinueDueUSB() ) );
    
    mTest->mUsbBlockingState = MpEngine::USB_Connected;
    result = mTest->verifyUsbBlocking( true );    
    QVERIFY( result == true );
    QVERIFY( spy.count() == 1 ); 
    spy.clear();
    
    mTest->mUsbBlockingState = MpEngine::USB_Connected;
    result = mTest->verifyUsbBlocking( false );
    QVERIFY( result == true );
    QVERIFY( spy.count() == 0 );
    spy.clear();
    
    mTest->mUsbBlockingState = MpEngine::USB_NotConnected;
    result = mTest->verifyUsbBlocking( true );
    QVERIFY( result == false );
    QVERIFY( spy.count() == 0 );
    spy.clear();
    
    mTest->mUsbBlockingState = MpEngine::USB_NotConnected;
    result = mTest->verifyUsbBlocking( false );
    QVERIFY( result == false );
    QVERIFY( spy.count() == 0 );
    spy.clear();
    
    mTest->mUsbBlockingState = MpEngine::USB_Synchronizing;
    result = mTest->verifyUsbBlocking( true );
    QVERIFY( result == false );
    QVERIFY( spy.count() == 0 );
    spy.clear();
    
    mTest->mUsbBlockingState = MpEngine::USB_Synchronizing;
    result = mTest->verifyUsbBlocking( false );
    QVERIFY( result == false );
    QVERIFY( spy.count() == 0 );
    spy.clear();    
}

/*!
 test songScanner
 */
void TestMpEngine::testSongScanner() 
{
    mTest = MpEngineFactory::createIsolatedEngine( MpEngine::StandAlone );
    QCOMPARE( mTest->mSongScanner, mTest->songScanner() );
}

/*!
 test refreshLibrary
 */
void TestMpEngine::testRefreshLibrary() 
{
    mTest = MpEngineFactory::createIsolatedEngine( MpEngine::StandAlone );
    QSignalSpy spy( mTest, SIGNAL( libraryAboutToUpdate() ) );
    
    mTest->mUsbBlockingState = MpEngine::USB_Connected;
    QVERIFY( mTest->verifyUsbBlocking() == true );
    mTest->refreshLibrary( true );
    QVERIFY( mTest->mSongScanner == 0 );
    QVERIFY( spy.count() == 0 );
    
    mTest->mUsbBlockingState = MpEngine::USB_NotConnected;
    QVERIFY( mTest->verifyUsbBlocking() == false );
    mTest->refreshLibrary( true );
    QVERIFY( mTest->mSongScanner != 0 );
    QVERIFY( spy.count() == 1 );
    spy.clear();
    
    mTest->mUsbBlockingState = MpEngine::USB_Synchronizing;
    QVERIFY( mTest->verifyUsbBlocking() == false );
    mTest->refreshLibrary( true );
    QVERIFY( mTest->mSongScanner != 0 );
    QVERIFY( spy.count() == 1 );
    spy.clear();
}

/*!
 test handleScanStarted 
 */
void TestMpEngine::testHandleScanStarted()
{
    mTest = MpEngineFactory::createIsolatedEngine( MpEngine::StandAlone );
    mTest->handleScanStarted();
    QVERIFY( mTest->mHandleMediaCommands == false );
}

/*!
 test handleScanEnded
 */
void TestMpEngine::testHandleScanEnded() 
{
    mTest = MpEngineFactory::createIsolatedEngine( MpEngine::StandAlone );
    QSignalSpy spy( mTest, SIGNAL( libraryUpdated() ) );
    
    mTest->handleScanEnded( 0, 0 );
    QVERIFY( mTest->mHandleMediaCommands == true );
    
    QVERIFY( spy.count() == 1 );
    spy.clear();    
}

/*!
 test handleDiskEvent
 */
void TestMpEngine::testHandleDiskEvent() 
{
    mTest = MpEngineFactory::createIsolatedEngine( MpEngine::StandAlone );
    QSignalSpy spy( mTest, SIGNAL( libraryUpdated() ) );
    QSignalSpy spyTwo( mTest, SIGNAL( libraryAboutToUpdate() ) );
    
    mTest->handleDiskEvent( DiskFormatStarted );
    QVERIFY( mTest->mHandleMediaCommands == false );
    
    mTest->handleDiskEvent( DiskFormatEnded );
    QVERIFY( mTest->mHandleMediaCommands == true );
    
    mTest->mUsbBlockingState = MpEngine::USB_Synchronizing;
    mTest->handleDiskEvent( DiskRemoved );
    QVERIFY( spy.count() == 0 );
    spy.clear();    
    
    mTest->mUsbBlockingState = MpEngine::USB_NotConnected;
    mTest->handleDiskEvent( DiskRemoved );
    QVERIFY( spy.count() == 1 );
    spy.clear();   
    
    mTest->mUsbBlockingState = MpEngine::USB_Connected;
    mTest->handleDiskEvent( DiskRemoved );
    QVERIFY( spy.count() == 1 );
    spy.clear();   
    
    mTest->mUsbBlockingState = MpEngine::USB_NotConnected;
    mTest->handleDiskEvent( DiskInserted );
    QVERIFY( mTest->verifyUsbBlocking() == false );
    QVERIFY( spyTwo.count() == 1 );
    spyTwo.clear();   
    
    mTest->mUsbBlockingState = MpEngine::USB_Connected;
    mTest->handleDiskEvent( DiskInserted );
    QVERIFY( spy.count() == 1 );
    spy.clear();      
}

/*!
 test HandleUsbEvent
 */
void TestMpEngine::testHandleUsbEvent()
{
    // no need to test here 
}

/*!
 test handleUsbMassStorageStartEvent
 */
void TestMpEngine::testHandleUsbMassStorageStartEvent()
{
    mTest = MpEngineFactory::createIsolatedEngine( MpEngine::StandAlone );
    QSignalSpy spy( mTest, SIGNAL( usbBlocked( bool ) ) );
    QSignalSpy spyTwo( mTest, SIGNAL( usbSynchronizationStarted() ) );
    
    mTest->handleUsbMassStorageStartEvent();
    QVERIFY( mTest->mHandleMediaCommands == false );
    QVERIFY( spy.count() == 1 );
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY( arguments.at(0).toBool() == true ); 
    
    QVERIFY( spyTwo.count() == 1 );
}

/*!
 test handleUsbMassStorageEndEvent
 */
void TestMpEngine::testHandleUsbMassStorageEndEvent() 
{
    mTest = MpEngineFactory::createIsolatedEngine( MpEngine::StandAlone );
    QSignalSpy spy( mTest, SIGNAL( usbBlocked( bool ) ) );
    QSignalSpy spyTwo( mTest, SIGNAL( usbSynchronizationFinished() ) );
    
    mTest->handleUsbMassStorageEndEvent();
    QVERIFY( mTest->mHandleMediaCommands == true );
    QVERIFY( spy.count() == 1 );
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toBool() == false ); 
    
    QVERIFY( spyTwo.count() == 1 );
}

/*!
 test handleUsbMtpStartEvent
 */
void TestMpEngine::testHandleUsbMtpStartEvent() 
{
    mTest = MpEngineFactory::createIsolatedEngine( MpEngine::StandAlone );
    QSignalSpy spy( mTest, SIGNAL( usbBlocked( bool ) ) );
    QSignalSpy spyTwo( mTest, SIGNAL( libraryAboutToUpdate() ) );
    QSignalSpy spyThree( mTest, SIGNAL( usbSynchronizationStarted() ) );
    
    mTest->handleUsbMtpStartEvent();
    QVERIFY( mTest->mHandleMediaCommands == false );
    QVERIFY( spy.count() == 1 );
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY( arguments.at(0).toBool() == true ); 
    QVERIFY( spyTwo.count() == 1 );
    QVERIFY( spyThree.count() == 1 );
}

/*!
 test handleUsbMtpEndEvent
 */
void TestMpEngine::testHandleUsbMtpEndEvent() 
{
    mTest = MpEngineFactory::createIsolatedEngine( MpEngine::StandAlone );
    QSignalSpy spy( mTest, SIGNAL( usbBlocked( bool ) ) );
    QSignalSpy spyTwo( mTest, SIGNAL( usbSynchronizationFinished() ) );
    QSignalSpy spyThree( mTest, SIGNAL( libraryUpdated() ) );
    QList<QVariant> arguments;
    
    mTest->mUsbBlockingState = MpEngine::USB_Synchronizing;
    mTest->handleUsbMtpEndEvent();
    QVERIFY( mTest->mHandleMediaCommands == true );
    QVERIFY( spy.count() == 1 );
    arguments = spy.takeFirst();
    QVERIFY( arguments.at(0).toBool() == false ); 
    QVERIFY( spyTwo.count() == 1 );
    QVERIFY( spyThree.count() == 1 );
    spyTwo.clear();
    spyThree.clear();
    arguments.clear();
    
    mTest->mPreviousUsbState = MpEngine::USB_NotConnected;
    mTest->handleUsbMtpEndEvent();
    QVERIFY( mTest->mHandleMediaCommands == true );
    QVERIFY( spy.count() == 1 );
    arguments = spy.takeFirst();
    QVERIFY( arguments.at(0).toBool() == false ); 
    QVERIFY( spyTwo.count() == 0 );
    QVERIFY( spyThree.count() == 0 );
    spyTwo.clear();
    spyThree.clear();
    arguments.clear();
    
    mTest->mPreviousUsbState = MpEngine::USB_Connected;
    mTest->handleUsbMtpEndEvent();
    QVERIFY( mTest->mHandleMediaCommands == true );
    QVERIFY( spy.count() == 1 );
    arguments = spy.takeFirst();
    QVERIFY( arguments.at(0).toBool() == false ); 
    QVERIFY( spyTwo.count() == 0 );
    QVERIFY( spyThree.count() == 0 );
    spyTwo.clear();
    spyThree.clear();
    arguments.clear();
}

/*!
 test handleUsbMtpNotActive
 */
void TestMpEngine::testHandleUsbMtpNotActive() 
{   
    mTest = MpEngineFactory::createIsolatedEngine( MpEngine::StandAlone );
    QSignalSpy spy( mTest, SIGNAL( usbBlocked( bool ) ) );
    
    mTest->handleUsbMtpNotActive();
    QVERIFY( spy.count() == 1 );
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY( arguments.at(0).toBool() == true ); 
}

/*!
 test changeUsbBlockingState
 */
void TestMpEngine::testChangeUsbBlockingState()
{
    mTest = MpEngineFactory::createIsolatedEngine( MpEngine::StandAlone );
    
    MpEngine::UsbBlockingState cState = mTest->mUsbBlockingState;
    mTest->changeUsbBlockingState( MpEngine::USB_Connected );
    QCOMPARE( mTest->mPreviousUsbState, cState );
    QCOMPARE( mTest->mUsbBlockingState, MpEngine::USB_Connected );
}

/*!
 test handleDeleteStarted
 */
void TestMpEngine::testHandleDeleteStarted() 
{
    mTest = MpEngineFactory::createIsolatedEngine( MpEngine::StandAlone );
    qRegisterMetaType<QModelIndex>("TCollectionContext");
    QSignalSpy spy( mTest, SIGNAL( deleteStarted( TCollectionContext, int ) ) );
    
    TCollectionContext context( ECollectionContextAllSongs );
    mTest->handleDeleteStarted( context, 0 );
    QVERIFY( mTest->mHandleMediaCommands == false );
    QVERIFY( spy.count() == 1 );
}

/*!
 test handleDeleteEnded
 */
void TestMpEngine::testHandleDeleteEnded() 
{
    mTest = MpEngineFactory::createIsolatedEngine( MpEngine::StandAlone );
    QSignalSpy spy( mTest, SIGNAL( songsDeleted( bool ) ) );
    QList<QVariant> arguments;
    
    mTest->handleDeleteEnded( true );
    QVERIFY( mTest->mHandleMediaCommands == true );
    QVERIFY( spy.count() == 1 );
    arguments = spy.takeFirst();
    QVERIFY( arguments.at(0).toBool() == true ); 
    spy.clear();
    arguments.clear();
    
    mTest->handleDeleteEnded( false );
    QVERIFY( mTest->mHandleMediaCommands == true );
    QVERIFY( spy.count() == 1 );
    arguments = spy.takeFirst();
    QVERIFY( arguments.at(0).toBool() == false );
    spy.clear();
    arguments.clear();
}

/*!
 test playbackData
 */
void TestMpEngine::testPlaybackData() 
{
    mTest = MpEngineFactory::createIsolatedEngine( MpEngine::StandAlone );
    QVERIFY( mTest->mMpxPlaybackWrapper->mPlaybackData == mTest->playbackData() );
}

/*!
 test songData
 */
void TestMpEngine::testSongData() 
{
    mTest = MpEngineFactory::createIsolatedEngine( MpEngine::StandAlone );
    QVERIFY( mTest->mSongData == mTest->songData() );
}

/*!
 test balance
 */
void TestMpEngine::testBalance() 
{
    mTest = MpEngineFactory::createIsolatedEngine( MpEngine::StandAlone );
    QVERIFY( mTest->mAudioEffectsWrapper->mBalance == mTest->balance() );
}

/*!
 test loudness
 */
void TestMpEngine::testLoudness()
{
    mTest = MpEngineFactory::createIsolatedEngine( MpEngine::StandAlone );
    QVERIFY( mTest->mAudioEffectsWrapper->mLoudness == mTest->loudness() );
}

/*!
 test setBalance
 */
void TestMpEngine::testSetBalance()
{
    // nothing to test here
}

/*!
 test setLoudness
 */
void TestMpEngine::testSetLoudness()
{
    // nothing to test here
}

/*!
 test applyPreset
 */
void TestMpEngine::testApplyPreset()
{
    mTest = MpEngineFactory::createIsolatedEngine( MpEngine::StandAlone );
    mTest->mCurrentPresetIndex = 2;
    
    mTest->applyPreset( 4 );
    QCOMPARE( mTest->mCurrentPresetIndex, 4 );
}

/*!
 test disableEqualizer
 */
void TestMpEngine::testDisableEqualizer()
{
    mTest = MpEngineFactory::createIsolatedEngine( MpEngine::StandAlone );
    mTest->disableEqualizer();
    
    QCOMPARE( mTest->mCurrentPresetIndex, 0 );
}

/*!
 test activePreset
 */
void TestMpEngine::testActivePreset()
{
    mTest = MpEngineFactory::createIsolatedEngine( MpEngine::StandAlone );
    
    QCOMPARE( mTest->mCurrentPresetIndex, mTest->activePreset() );
}

/*!
 test presetNames
 */
void TestMpEngine::testPresetNames() 
{
    mTest = MpEngineFactory::createIsolatedEngine( MpEngine::StandAlone );
    
    QCOMPARE( mTest->mEqualizerWrapper->mList, mTest->presetNames() );
}

/*!
 test handleEqualizerReady
 */
void TestMpEngine::testHandleEqualizerReady() 
{
    mTest = MpEngineFactory::createIsolatedEngine( MpEngine::StandAlone );
    QSignalSpy spy( mTest, SIGNAL( equalizerReady() ) );
    
    mTest->handleEqualizerReady();
    QCOMPARE( mTest->mCurrentPresetIndex, 0 );
    QVERIFY( spy.count() == 1 );
}


// End of file
