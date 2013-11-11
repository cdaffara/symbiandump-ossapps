/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Unit test for mpplaybackview
*
*/

#include <QMetaType>
#include <QtGui>
#include <hbradiobuttonlist.h>
#include <QTranslator>
#include <QLocale>

#include "stub/inc/mpengine.h"
#include "stub/inc/mpenginefactory.h"
#include "stub/inc/mpequalizerwidget.h"
#include "stub/inc/mpplaybackdata.h"
#include "stub/inc/mpplaybackwidget.h"
#include "stub/inc/mpsettingsmanager.h"
#include "stub/inc/mpviewbase.h"
#include "mpcommondefs.h"
#include "unittest_mpplaybackview.h"

// Do this so we can access all member variables.
#define private public
#include "mpplaybackview.h"
#undef private

//This so we can test private functions
#include "../../src/mpplaybackview.cpp"



/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestMpPlaybackView tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpplaybackview.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpPlaybackView::TestMpPlaybackView()
        : mTest(0)
{

}

TestMpPlaybackView::~TestMpPlaybackView()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpPlaybackView::initTestCase()
{

}

/*!
 Called after the last testfunction was executed.
 */
void TestMpPlaybackView::cleanupTestCase()
{
    QCoreApplication::processEvents();
}

/*!
 Called before each testfunction is executed.
 */
void TestMpPlaybackView::init()
{
    mTest = new MpPlaybackView();
    mTest->initializeView();
}

/*!
 Called after every testfunction.
 */
void TestMpPlaybackView::cleanup()
{
    delete mTest;
    mTest = 0;
    MpEngineFactory::close();
}

/*!
Test initializeView
 */
void TestMpPlaybackView::testInitializeView()
{
    QVERIFY( mTest->mSoftKeyBack );
    QVERIFY( mTest->mMpEngine );
    QVERIFY( mTest->mPlaybackData );
    QVERIFY( mTest->mPlaybackWidget );
}

/*!
Test ActivateView
 */
void TestMpPlaybackView::testActivateView()
{
    QCOMPARE( mTest->mActivated, false );
    mTest->activateView();
    QCOMPARE( mTest->mActivated, true );
    QVERIFY( mTest->navigationAction() == mTest->mSoftKeyBack );
}

/*!
 Test DeactivateView
 */
void TestMpPlaybackView::testDeactivateView()
{
    mTest->mActivated = true;
    mTest->deactivateView();
    QCOMPARE( mTest->mActivated, false );
}

/*!
 Test ShuffleEnabled
 */
void TestMpPlaybackView::testShuffleEnabled()
{
    mTest->mShuffle = false;
    QCOMPARE(mTest->shuffleEnabled(), false);
    mTest->mShuffle = true;
    QCOMPARE(mTest->shuffleEnabled(), true);
}

/*!
 Test RepeatEnabled
  */
void TestMpPlaybackView::testRepeatEnabled()
{
    mTest->mRepeat = false;
    QCOMPARE(mTest->repeatEnabled(), false);
    mTest->mRepeat = true;
    QCOMPARE(mTest->repeatEnabled(), true);
}

/*!
  Test PlaybackStatus
  */
void TestMpPlaybackView::testPlaybackStatus()
{
    mTest->mPlaybackData->setPlaybackState(MpPlaybackData::NotPlaying);
    QCOMPARE(mTest->playbackStatus(),QString("NotPlaying"));

    mTest->mPlaybackData->setPlaybackState(MpPlaybackData::Playing);
    QCOMPARE(mTest->playbackStatus(),QString("Playing"));

    mTest->mPlaybackData->setPlaybackState(MpPlaybackData::Paused);
    QCOMPARE(mTest->playbackStatus(),QString("Paused"));

    mTest->mPlaybackData->setPlaybackState(MpPlaybackData::Stopped);
    QCOMPARE(mTest->playbackStatus(),QString("Stopped"));
}

/*!
  Test StartSettingsView
  */
void TestMpPlaybackView::testStartSettingsView()
{
    QSignalSpy spy(mTest, SIGNAL(command(int)));

    mTest->startSettingsView();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0), QVariant(MpCommon::ActivateSettingsView));
}

/*!
  Test Back
  */
void TestMpPlaybackView::testBack()
{
    //test Stop() gets called in MpEngine
    mTest->mMpEngine->mStopCount=0;
    mTest->mViewMode = MpCommon::EmbeddedView;
    mTest->back();
    QCOMPARE(mTest->mMpEngine->mStopCount,1);

    mTest->mViewMode = MpCommon::FetchView;
    mTest->back();
    QCOMPARE(mTest->mMpEngine->mStopCount,2);

    QSignalSpy spy(mTest, SIGNAL(command(int)));
    mTest->mMpEngine->mStopCount = 0;   
    mTest->mViewMode = MpCommon::DefaultView;

    mTest->back();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0), QVariant(MpCommon::ActivateCollectionView));
    QCOMPARE(mTest->mMpEngine->mStopCount,0);
}

/*!
  Test Exit
  */
void TestMpPlaybackView::testExit()
{
    QSignalSpy spy(mTest, SIGNAL(command(int)));
    mTest->exit();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0), QVariant(MpCommon::Exit));
}

/*!
  Test PlaybackStateChanged
  */
void TestMpPlaybackView::testPlaybackStateChanged()
{
    //test signal slot connection
    mTest->mMpEngine->mStopCount=0;
    mTest->mViewMode = MpCommon::FetchView;
    mTest->mPlaybackData->setPlaybackState(MpPlaybackData::Stopped);

    mTest->mPlaybackData->triggerStateUpdate(MpPlaybackData::Stopped);
    QCOMPARE(mTest->mMpEngine->mStopCount,1);
}

/*!
  Test Flip
  */
void TestMpPlaybackView::testFlip()
{        
    QSignalSpy spy(mTest, SIGNAL(command(int)));
    mTest->flip();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0), QVariant(MpCommon::ActivateDetailsView));
    QCOMPARE(mTest->mMpEngine->mRetrieveSongDetailsCount, 1);
}

/*!
  Test ToggleShuffle and Shuffle changed
  */
void TestMpPlaybackView::testShuffle()
{
    mTest->mShuffle = false;
    QCOMPARE(mTest->shuffleEnabled(), false);

    mTest->toggleShuffle();
    mTest->shuffleChanged(true); //simulate signal
    QCOMPARE(mTest->shuffleEnabled(), true);
    QVERIFY(mTest->mShuffleAction->isChecked());

    mTest->toggleShuffle();
    mTest->shuffleChanged(false);
    QCOMPARE(mTest->shuffleEnabled(), false);
    QVERIFY(!mTest->mShuffleAction->isChecked());
}

/*!
  Test ToggleRepeat and RepeatChanged
  */
void TestMpPlaybackView::testRepeat()
{
    mTest->mRepeat = false;
    QCOMPARE(mTest->repeatEnabled(),false);

    mTest->toggleRepeat();
    mTest->repeatChanged(true);
    QCOMPARE(mTest->repeatEnabled(),true);    
    QCOMPARE(mTest->mRepeatAction->text(),hbTrId( "txt_mus_opt_repeat_off" ));

    mTest->toggleRepeat();
    mTest->repeatChanged(false);
    QCOMPARE(mTest->repeatEnabled(),false);
    QCOMPARE(mTest->mRepeatAction->text(),hbTrId( "txt_mus_opt_repeat_on" ));
}

/*!
  Test HandleSongSelected
  */
void TestMpPlaybackView::testHandleSongSelected()
{
    mTest->mMpEngine->mStopCount=0;
    mTest->mPlaybackData->setUri("testUri");

    QSignalSpy spy(mTest, SIGNAL(songSelected(QString)));
    mTest->handleSongSelected();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0), QVariant("testUri"));
    QCOMPARE(mTest->mMpEngine->mStopCount,1);
}

/*!
  Test ShowEqualizerDialog
  */
void TestMpPlaybackView::testShowEqualizerDialog()
{
    mTest->showEqualizerDialog();
    QCOMPARE(mTest->mEqualizerWidget->showEqualizerWidgetCount,1);
}

/*!
    TestCloseEqualizerDialog
  */
void TestMpPlaybackView::testCloseEqualizerDialog()
{
    // Equalizer no displayed before, closeEqualizerDialog should do nothing.
    mTest->closeEqualizerDialog();
    QVERIFY(!mTest->mEqualizerWidget);

    // Equalizer previously displyaed
    mTest->showEqualizerDialog();
    mTest->closeEqualizerDialog();
    QCOMPARE(mTest->mEqualizerWidget->closeEqualizerWidgetCount,1);
}

/*!
    Test showCorruptedNote
  */
void TestMpPlaybackView::testShowCorruptedNote()
{
    mTest->showCorruptedNote();
    QCOMPARE(mTest->mMpEngine->mStopCount, 1);
}

/*!
    Test StartRewindTimer and startSeekRewind
  */
void TestMpPlaybackView::testSeekRewind()
{    
    mTest->mMpEngine->mStartSeekBackwardCount = 0;
    mTest->mSeeking = false;

    mTest->startRewindTimer();
    QTest::qWait(2000); // wait for timer to fire and call startSeekRewind()

    QCOMPARE(mTest->mMpEngine->mStartSeekBackwardCount,1);
    QCOMPARE(mTest->mSeeking,true);
}

/*!
    Test StartForwardTimer and startSeekForward
  */
void TestMpPlaybackView::testSeekForward()
{
    mTest->mMpEngine->mStartSeekForwardCount = 0;
    mTest->mSeeking = false;

    mTest->startForwardTimer();
    QTest::qWait(2000); // wait for timer to fire and call startSeekForward()

    QCOMPARE(mTest->mMpEngine->mStartSeekForwardCount,1);
    QCOMPARE(mTest->mSeeking,true);
}

/*!
    Test EndRewind
  */
void TestMpPlaybackView::testEndRewind()
{
    mTest->mSeeking = true;
    mTest->endRewind();
    QCOMPARE(mTest->mMpEngine->mStopSeekingCount,1);
    QCOMPARE(mTest->mSeeking,false);
    
    mTest->endRewind();
    QCOMPARE(mTest->mMpEngine->mSkipBackwardCount,1);
    QCOMPARE(mTest->mSeeking,false);
}

/*!
    Test EndForward
  */
void TestMpPlaybackView::testEndForward()
{
    mTest->mSeeking = true;
    mTest->endForward();
    QCOMPARE(mTest->mMpEngine->mStopSeekingCount,1);
    QCOMPARE(mTest->mSeeking,false);
    
    mTest->endForward();
    QCOMPARE(mTest->mMpEngine->mSkipForwardCount,1);
    QCOMPARE(mTest->mSeeking,false);    
}

// End of file
