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
* Description: Unit test for MpPlaybackWidget
*
*/
#include <QSignalSpy>
#include <QTime>
#include "unittest_mpplaybackwidget.h"
#include "mpplaybackdata.h"
#include "mpalbumcoverwidget.h"
#include <hblabel.h>
#include <hbprogressslider.h>

/*
 * formatDuration() cannot be tested as it is private slot
 */

// Do this so we can access all member variables.
#define private public
#include "mpplaybackwidget.h"
#undef private

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestMpPlaybackWidget tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpplaybackwidget.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpPlaybackWidget::TestMpPlaybackWidget()
    : mTest(0)
{
}

TestMpPlaybackWidget::~TestMpPlaybackWidget()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpPlaybackWidget::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpPlaybackWidget::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpPlaybackWidget::init()
{
    mTest = new MpPlaybackWidget( new MpPlaybackData() );
}

/*!
 Called after every testfunction.
 */
void TestMpPlaybackWidget::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 test repeatChanged
 */
void TestMpPlaybackWidget::testRepeatChanged() 
{
    mTest->repeatChanged( true );    
    QVERIFY( mTest->mRepeatIndicator->isVisible() == true );
    
    mTest->repeatChanged( false );    
    QVERIFY( mTest->mRepeatIndicator->isVisible() == false );
}

/*!
 test playbackInfoChanged
 */
void TestMpPlaybackWidget::testPlaybackInfoChanged()
{
    QSignalSpy spy( mTest, SIGNAL( signalPlaybackInfoChanged() ) );
    mTest->playbackInfoChanged();
    
    QVERIFY( mTest->mSongTitle->plainText() == "title" );
    QVERIFY( mTest->mArtistName->plainText() == "artist" );
    QVERIFY( mTest->mAlbumName->plainText() == "album" );
    QVERIFY( mTest->mRealAudioIndicator->isVisible() == false );
    QVERIFY( spy.count() == 1 );
    spy.clear();
}

/*!
 test durationChanged
 */
void TestMpPlaybackWidget::testDurationChanged()
{
    mTest->durationChanged();
    
    QVERIFY( mTest->mProgressBar->maximum() == 20 );
    QVERIFY( mTest->mProgressBar->progressValue() == 0 );
    QVERIFY( mTest->mProgressBar->sliderValue() == 0 );
    QVERIFY( mTest->mProgressBar->isEnabled() == true );
}

/*!
 test positionChanged
 */
void TestMpPlaybackWidget::testPositionChanged()
{
    mTest->positionChanged();
    mTest->mProgreesBarDragging = false;
    
    QVERIFY( mTest->mProgressBar->progressValue() == 10 );
    QVERIFY( mTest->mProgressBar->sliderValue() == 10 );
    // formatDuration is tested seperately 
}

/*!
 test albumArtChanged
 */
void TestMpPlaybackWidget::testAlbumArtChanged()
{
    mTest->albumArtChanged();
    
    QVERIFY( mTest->mAlbumArt->mIcon == mTest->mPlaybackData->mAlbumArt );
    QVERIFY( mTest->mAlbumArt->isEnabled() == true );
}

/*!
 test handleLcdLightStatus
 */
void TestMpPlaybackWidget::testHandleLcdLightStatus( )
{
    mTest->handleLcdLightStatus( true );
    testPlaybackInfoChanged();
	
}

/*!
 test handleCollectionPlaylistOpened
 */
void TestMpPlaybackWidget::testHandleCollectionPlaylistOpened()
{
    mTest->handleCollectionPlaylistOpened();
    QVERIFY( mTest->mSongTitle->plainText() == " " );
    QVERIFY( mTest->mArtistName->plainText() == " " );
    QVERIFY( mTest->mAlbumName->plainText() == " " );
}


// End of file
