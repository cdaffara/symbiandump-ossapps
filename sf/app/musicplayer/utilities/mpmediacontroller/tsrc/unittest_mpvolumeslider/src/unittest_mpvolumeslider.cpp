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
* Description: Unit test for mpvolumeslider
*
*/

#include <QMetaType>

#include "stub/inc/mpenginefactory.h"
#include "stub/inc/hbvolumesliderpopup.h"
#include "unittest_mpvolumeslider.h"


// Do this so we can access all member variables.
#define private public
#include "mpvolumeslider.h"
#undef private

// Do this so private function can be accessed
#include <../../src/mpvolumeslider.cpp>

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TestMpVolumeSlider tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpvolumeslider.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpVolumeSlider::TestMpVolumeSlider()
    : mTest(0)
{
}

TestMpVolumeSlider::~TestMpVolumeSlider()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpVolumeSlider::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpVolumeSlider::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpVolumeSlider::init()
{
    mTest = new MpVolumeSlider();
}

/*!
 Called after every testfunction.
 */
void TestMpVolumeSlider::cleanup()
{
    if (mTest)
    {
        delete mTest;
        mTest = 0;

        MpEngineFactory::close();
    }
}

void TestMpVolumeSlider::testConstructor()
{
    QVERIFY( mTest );
    QCOMPARE(mTest->mMpEngine->iMediaPropertyRequested.value(0), PropVolumeLevelMax);
    QCOMPARE(mTest->mMpEngine->iMediaPropertyRequested.value(1), PropVolumeLevel);
    QCOMPARE(mTest->mMpEngine->iMediaPropertyRequested.value(2), PropVolumeMuteState);
}

void TestMpVolumeSlider::testShowVolumeSlider()
{
    mTest->mEngineVolumeMax = 100;
    mTest->mEngineMuted = true;
    mTest->mEngineVolumeLevel = 30;

    // While VolumeSlider is pressed showVolumeSlider() function does nothing.
    mTest->mSliderPressed = true;
    mTest->showVolumeSlider();
    QVERIFY( !mTest->mVolumeSliderPopup );

    // When VolumeSlider is not being pressed, showVolumeSlider() attempts to show the slider.
    mTest->mSliderPressed = false;
    mTest->showVolumeSlider();
    QVERIFY( mTest->mVolumeSliderPopup );
    QCOMPARE( mTest->mVolumeSliderPopup->mValue, 0 );    //Currently in mute state
    QVERIFY( mTest->mVolumeSliderPopup->isVisible() );

    // Clearing mute state
    mTest->mEngineMuted = false;
    mTest->showVolumeSlider();
    QVERIFY( mTest->mVolumeSliderPopup );
    QCOMPARE( mTest->mVolumeSliderPopup->mValue, 3 );
    QVERIFY( mTest->mVolumeSliderPopup->isVisible() );
}

void TestMpVolumeSlider::testHandleVolumeProperty()
{
    MpEngine *mpEngineInstance = MpEngineFactory::sharedEngine();
    connect( this, SIGNAL(triggerVolumePropertyChanged( MpCommon::MpVolumeProperty, int ) ),
             mpEngineInstance, SIGNAL( volumePropertyChanged( MpCommon::MpVolumeProperty, int ) ) );
    
    emit triggerVolumePropertyChanged( MpCommon::MaxVolume, 100 );
    QCOMPARE( mTest->mEngineDataCompleted, 0x01 );
    QCOMPARE( mTest->mEngineVolumeMax, 100 );
    QVERIFY( !mTest->mVolumeSliderPopup );

    emit triggerVolumePropertyChanged( MpCommon::Volume, 0 );
    QCOMPARE( mTest->mEngineDataCompleted, 0x03 );
    QCOMPARE( mTest->mEngineVolumeLevel, 0 );
    QVERIFY( mTest->mEngineMuted );
    QVERIFY( !mTest->mVolumeSliderPopup );
    emit triggerVolumePropertyChanged( MpCommon::Volume, 30 );
    QCOMPARE( mTest->mEngineDataCompleted, 0x03 );
    QCOMPARE( mTest->mEngineVolumeLevel, 30 );
    QVERIFY( !mTest->mEngineMuted );
    QVERIFY( !mTest->mVolumeSliderPopup );
    mTest->mEngineDataCompleted = 0x07;
    emit triggerVolumePropertyChanged( MpCommon::Volume, 40 );
    QCOMPARE( mTest->mEngineDataCompleted, 0x07 );
    QCOMPARE( mTest->mEngineVolumeLevel, 40 );
    QVERIFY( !mTest->mEngineMuted );
    QVERIFY( mTest->mVolumeSliderPopup->isVisible() );

    mTest->mEngineDataCompleted = 0x03;
    mTest->mVolumeSliderPopup->setVisible( false );
    emit triggerVolumePropertyChanged( MpCommon::MuteState, 1 );
    QCOMPARE( mTest->mEngineDataCompleted, 0x07 );
    QVERIFY( mTest->mEngineMuted );
    QVERIFY( !mTest->mVolumeSliderPopup->isVisible() );
    emit triggerVolumePropertyChanged( MpCommon::MuteState, 0 );
    QCOMPARE( mTest->mEngineDataCompleted, 0x07 );
    QVERIFY( !mTest->mEngineMuted );
    QVERIFY( mTest->mVolumeSliderPopup->isVisible() );
}

void TestMpVolumeSlider::testSliderPressed()
{
    mTest->mEngineMuted = false;
    mTest->mEngineVolumeLevel = 20;
    mTest->mEngineVolumeMax = 100;
    mTest->initializeVolumeSliderPopup();
    connect( this, SIGNAL( triggerSliderPressed() ), 
             mTest->mVolumeSliderPopup, SIGNAL( sliderPressed() ) );

    emit triggerSliderPressed();
    QVERIFY( mTest->mSliderPressed );
}

void TestMpVolumeSlider::testSliderReleased()
{
    mTest->mEngineMuted = false;
    mTest->mEngineVolumeLevel = 20;
    mTest->mEngineVolumeMax = 100;
    mTest->initializeVolumeSliderPopup();
    connect( this, SIGNAL( triggerSliderReleased() ), 
             mTest->mVolumeSliderPopup, SIGNAL( sliderReleased() ) );
    mTest->mSliderPressed = true;

    emit triggerSliderReleased();
    QVERIFY( !mTest->mSliderPressed );
}

void TestMpVolumeSlider::testMuteIconClicked()
{
    mTest->mEngineMuted = false;
    mTest->mEngineVolumeLevel = 20;
    mTest->mEngineVolumeMax = 100;
    mTest->initializeVolumeSliderPopup();
    connect( this, SIGNAL( triggerIconClicked() ), 
             mTest->mVolumeSliderPopup, SIGNAL( iconClicked() ) );

    mTest->mVolumeSliderPopup->setValue( 0 );
    emit triggerIconClicked();
    QCOMPARE( mTest->mMpEngine->iMediaCommandReceived, CmdMuteVolume );
    QVERIFY( mTest->mEngineMuted );

    mTest->mVolumeSliderPopup->setValue( 1 );
    emit triggerIconClicked();
    QCOMPARE( mTest->mMpEngine->iMediaCommandReceived, CmdUnmuteVolume );
    QVERIFY( !mTest->mEngineMuted );
}

void TestMpVolumeSlider::testVolumeSliderChanged()
{
    mTest->mEngineMuted = false;
    mTest->mEngineVolumeLevel = 20;
    mTest->mEngineVolumeMax = 100;
    mTest->initializeVolumeSliderPopup();
    connect( this, SIGNAL( triggerValueChanged( int ) ), 
             mTest->mVolumeSliderPopup, SIGNAL( valueChanged( int ) ) );

    mTest->mEngineVolumeLevel = 60;
    mTest->mEngineMuted = false;
    // Same values slider and engine
    emit triggerValueChanged( 6 );
    QTest::qWait(25);
    QCOMPARE( mTest->mMpEngine->iMediaCommandReceived, CmdNone );
    QCOMPARE( mTest->mMpEngine->iCommandValue, -1 );
    // Slider value smaller than engine
    mTest->mMpEngine->iMediaCommandReceived = CmdNone;
    mTest->mMpEngine->iCommandValue = -1;
    emit triggerValueChanged( 0 );
    QTest::qWait(25);
    QCOMPARE( mTest->mMpEngine->iMediaCommandReceived, CmdSetVolumeLevel );
    QCOMPARE( mTest->mMpEngine->iCommandValue, 0 );
    // Slider value bigger than engine
    mTest->mMpEngine->iMediaCommandReceived = CmdNone;
    mTest->mMpEngine->iCommandValue = -1;
    emit triggerValueChanged( 7 );
    QTest::qWait(25);
    QCOMPARE( mTest->mMpEngine->iMediaCommandReceived, CmdSetVolumeLevel );
    QCOMPARE( mTest->mMpEngine->iCommandValue, 70 );

    mTest->mEngineMuted = true;
    // Slider changes to 0 while engine in mute state.
    mTest->mMpEngine->iMediaCommandReceived = CmdNone;
    mTest->mMpEngine->iCommandValue = -1;
    emit triggerValueChanged( 0 );
    QTest::qWait(25);
    QCOMPARE( mTest->mMpEngine->iMediaCommandReceived, CmdNone );
    QCOMPARE( mTest->mMpEngine->iCommandValue, -1 );
    // Slider changes other than 0 while engine in mute state.
    mTest->mMpEngine->iMediaCommandReceived = CmdNone;
    mTest->mMpEngine->iCommandValue = -1;
    emit triggerValueChanged( 8 );
    QTest::qWait(25);
    QCOMPARE( mTest->mMpEngine->iMediaCommandReceived, CmdSetVolumeLevel );
    QCOMPARE( mTest->mMpEngine->iCommandValue, 80 );
}

void TestMpVolumeSlider::testInitializeVolumeSliderPopup()
{
    mTest->mEngineMuted = false;
    mTest->mEngineVolumeLevel = 20;
    mTest->mEngineVolumeMax = 100;

    mTest->initializeVolumeSliderPopup();
    QVERIFY( mTest->mVolumeSliderPopup );
    QCOMPARE( mTest->mVolumeSliderPopup->mMinRange, 0 );
    QCOMPARE( mTest->mVolumeSliderPopup->mMaxRange, 10 );
    QCOMPARE( mTest->mVolumeSliderPopup->mSingleStep, 1 );
    QCOMPARE( mTest->mVolumeSliderPopup->mSliderTickPosition, Hb::NoSliderTicks );
    QCOMPARE( mTest->mSliderStep, 10 );
}


// End of file
