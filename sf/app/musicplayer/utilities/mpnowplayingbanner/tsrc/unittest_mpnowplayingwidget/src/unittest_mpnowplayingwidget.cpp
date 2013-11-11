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
* Description: Unit test for mpnowplayingwidget
*
*/

#include <qnamespace.h>
#include <QSignalSpy>
#include <QMetaType>
#include <hblabel.h>
#include <hbevent.h>

#include "unittest_mpnowplayingwidget.h"
#include "stub/inc/mpplaybackdata.h"
#include "mpcommondefs.h"

// Do this so we can access all member variables.
#define private public
#include "mpnowplayingwidget.h"
#include "mpnowplayingwidget_p.h"
#undef private

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TestMpNowPlayingWidget tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpnowplayingwidget.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpNowPlayingWidget::TestMpNowPlayingWidget()
    : mTest(0)
{
}

TestMpNowPlayingWidget::~TestMpNowPlayingWidget()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpNowPlayingWidget::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpNowPlayingWidget::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpNowPlayingWidget::init()
{
    mTest = new MpNowPlayingWidget();
    mTestPrivate = mTest->d_ptr;
}

/*!
 Called after every testfunction.
 */
void TestMpNowPlayingWidget::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Tests constructor. Good case.
 */
void TestMpNowPlayingWidget::testConstructor()
{
    cleanup();
    MpPlaybackData::resetUpdateCounter();
    init();
    QVERIFY(mTestPrivate->mPrimaryText != 0);
    QVERIFY(mTestPrivate->mSecondaryText != 0);
    QVERIFY(mTestPrivate->mPlaybackData != 0);
    QVERIFY(mTestPrivate->mPlayIcon != 0);
    QVERIFY(mTestPrivate->mPauseIcon != 0);
    QVERIFY(mTestPrivate->mIcon != 0);
    QVERIFY(mTestPrivate->mDocumentLoader != 0);
    QCOMPARE(mTestPrivate->mState, MpPlaybackData::NotPlaying);
    QCOMPARE(MpPlaybackData::getUpdateCounter(), 1);
}

/*!
 Tests various state change combinations.
 */
void TestMpNowPlayingWidget::testSetState()
{
    QSignalSpy spy(mTest, SIGNAL(playbackAttachmentChanged(bool)));

    mTestPrivate->mState = MpPlaybackData::NotPlaying;
    mTestPrivate->mPlaybackData->triggerStateUpdate(MpPlaybackData::NotPlaying);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(mTestPrivate->mState, MpPlaybackData::NotPlaying);

    mTestPrivate->mState = MpPlaybackData::Playing;
    mTestPrivate->mPlaybackData->triggerStateUpdate(MpPlaybackData::NotPlaying);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), false);
    QCOMPARE(mTestPrivate->mState, MpPlaybackData::NotPlaying);

    spy.clear();
    mTestPrivate->mState = MpPlaybackData::Paused;
    mTestPrivate->mPlaybackData->triggerStateUpdate(MpPlaybackData::NotPlaying);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), false);
    QCOMPARE(mTestPrivate->mState, MpPlaybackData::NotPlaying);

    spy.clear();
    mTestPrivate->mState = MpPlaybackData::NotPlaying;
    mTestPrivate->mPlaybackData->triggerStateUpdate(MpPlaybackData::Playing);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), true);
    QCOMPARE(mTestPrivate->mState, MpPlaybackData::Playing);

    spy.clear();
    mTestPrivate->mState = MpPlaybackData::NotPlaying;
    mTestPrivate->mPlaybackData->triggerStateUpdate(MpPlaybackData::Paused);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), true);
    QCOMPARE(mTestPrivate->mState, MpPlaybackData::Paused);

    spy.clear();
    mTestPrivate->mState = MpPlaybackData::NotPlaying;
    mTestPrivate->mPlaybackData->triggerStateUpdate(MpPlaybackData::Stopped);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), true);
    QCOMPARE(mTestPrivate->mState, MpPlaybackData::Stopped);
    
    spy.clear();
    mTestPrivate->mState = MpPlaybackData::Playing;
    mTestPrivate->mPlaybackData->triggerStateUpdate(MpPlaybackData::Paused);
    mTestPrivate->mState = MpPlaybackData::Paused;
    mTestPrivate->mPlaybackData->triggerStateUpdate(MpPlaybackData::Playing);
    QCOMPARE(spy.count(), 0);
}

/*!
 Tests title and artist change
 */
void TestMpNowPlayingWidget::testBannerInfoChange()
{
    mTestPrivate->mPlaybackData->triggerLabelsChanged(QString("Title"), QString("Artist"));
    QCOMPARE(mTestPrivate->mPrimaryText->plainText(), QString("Title"));
    QCOMPARE(mTestPrivate->mSecondaryText->plainText(), QString("Artist"));
}


/*!
 Tests disabling/enabling the widget.
 Widget should stop observing events when disabled.
 */
void TestMpNowPlayingWidget::testSetEnabled()
{
    QSignalSpy spy(mTest, SIGNAL(playbackAttachmentChanged(bool)));
    mTestPrivate->mPrimaryText->setPlainText("Primary");
    mTestPrivate->mSecondaryText->setPlainText("Secondary");
    mTestPrivate->mState = MpPlaybackData::NotPlaying;

    // Disable the widget; Nothing should update.
    mTest->setEnabled(false);
    mTestPrivate->mPlaybackData->triggerStateUpdate(MpPlaybackData::Playing);
    QCOMPARE(mTestPrivate->mState, MpPlaybackData::NotPlaying);
    QCOMPARE(spy.count(), 0);

    mTestPrivate->mPlaybackData->triggerLabelsChanged(QString("Title"), QString("Artist"));
    QCOMPARE(mTestPrivate->mPrimaryText->plainText(), QString("Primary"));
    QCOMPARE(mTestPrivate->mSecondaryText->plainText(), QString("Secondary"));

    // Enable the widget. It should start updating again.
    mTestPrivate->mPlaybackData->triggerStateUpdate(MpPlaybackData::NotPlaying);
    mTest->setEnabled(true);
    mTestPrivate->mState = MpPlaybackData::Playing;
    mTestPrivate->mPlaybackData->triggerStateUpdate(MpPlaybackData::NotPlaying);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), false);
    QCOMPARE(mTestPrivate->mState, MpPlaybackData::NotPlaying);

    mTestPrivate->mPlaybackData->triggerLabelsChanged(QString("Title"), QString("Artist"));
    QCOMPARE(mTestPrivate->mPrimaryText->plainText(), QString("Title"));
    QCOMPARE(mTestPrivate->mSecondaryText->plainText(), QString("Artist"));
}

/*!
 Tests the return value based on the current playbackState
 */
void TestMpNowPlayingWidget::testIsBannerAttached()
{
    mTestPrivate->mState = MpPlaybackData::NotPlaying;
    QCOMPARE(mTestPrivate->isBannerAttached(), false);
    mTestPrivate->mState = MpPlaybackData::Playing;
    QCOMPARE(mTestPrivate->isBannerAttached(), true);
    mTestPrivate->mState = MpPlaybackData::NotPlaying;
    QCOMPARE(mTestPrivate->isBannerAttached(), false);
    mTestPrivate->mState = MpPlaybackData::Paused;
    QCOMPARE(mTestPrivate->isBannerAttached(), true);
    mTestPrivate->mState = MpPlaybackData::NotPlaying;
    QCOMPARE(mTestPrivate->isBannerAttached(), false);
    mTestPrivate->mState = MpPlaybackData::Stopped;
    QCOMPARE(mTestPrivate->isBannerAttached(), true);
    
}


// End of file
