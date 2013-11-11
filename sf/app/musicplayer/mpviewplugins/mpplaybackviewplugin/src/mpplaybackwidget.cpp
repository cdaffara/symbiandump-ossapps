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
* Description: Playback widget for Music Player playback view.
*
*/

#include <QTime>

#include <hbprogressslider.h>
#include <hbinstance.h>
#include <hblabel.h>
#include <hbfontspec.h>
#include <hbstackedlayout.h>

#include "mpplaybackwidget.h"
#include "mpplaybackdata.h"
#include "mpalbumcoverwidget.h"
#include "mpplaybackdocumentloader.h"
#include "mptrace.h"
#include "mplightmonitor.h"

const unsigned int KMicroSecToMiliSec( 1000 );

/*!
    \class MpPlaybackWidget
    \brief Music Player playback widget.

    This widget displays playback information on the playback view.
*/

/*!
    \fn void setPlaybackPosition(int value)

    This signal is emitted when the user drags the progressbar to a new
    position, \a value indicates the position.
 */

 /*!
    \fn void signalPlaybackInfoChanged()

    This signal is emitted when playback information has changed. Added for MATTI testing support.
 */


/*!
 Constructs the collection view plugin.
 */
MpPlaybackWidget::MpPlaybackWidget(MpPlaybackData *data, QGraphicsItem *parent )
    : HbWidget(parent),
      mPlaybackData(data),
      mDocumentLoader(0),
      mLightMonitor(0),
      mProgreesBarDragging(false),
      mDuration(0)
{
    TX_ENTRY
    mLayout = new HbStackedLayout(this);
    mLayout->setContentsMargins( 0.0, 0.0, 0.0, 0.0 );
    mLayout->setMinimumSize( 0.0, 0.0 );
    setLayout( mLayout );
    bool widgetsOk = false;
    mDocumentLoader = new MpPlaybackDocumentLoader();
    HbMainWindow *mainWindow = hbInstance->allMainWindows()[0];

    if ( mDocumentLoader ) {
        mDocumentLoader->load( QString(":/playbackviewdocml/playbackwidget.docml"), &widgetsOk);
    }
    if ( widgetsOk ) {
        QGraphicsWidget *tmpWidgetPtr;
        tmpWidgetPtr = mDocumentLoader->findWidget(QString("playbackWidgetContainer"));
        tmpWidgetPtr->setParentItem(this);
        mLayout->addItem( qobject_cast<HbWidget*>( tmpWidgetPtr ) );
        tmpWidgetPtr = mDocumentLoader->findWidget(QString("albumText"));
        mAlbumName = qobject_cast<HbLabel*>(tmpWidgetPtr);
        tmpWidgetPtr = mDocumentLoader->findWidget(QString("artistText"));
        mArtistName = qobject_cast<HbLabel*>(tmpWidgetPtr);
        tmpWidgetPtr = mDocumentLoader->findWidget(QString("songText"));
        mSongTitle = qobject_cast<HbLabel*>(tmpWidgetPtr);
        tmpWidgetPtr = mDocumentLoader->findWidget(QString("albumArt"));
        mAlbumArt = qobject_cast<MpAlbumCoverWidget*>(tmpWidgetPtr);
        mAlbumArt->setEnabled( false );
        mAlbumArt->setDefaultIcon( HbIcon( "qtg_large_album_art" ) );
        tmpWidgetPtr = mDocumentLoader->findWidget(QString("realAudio"));
        mRealAudioIndicator = qobject_cast<HbLabel*>(tmpWidgetPtr);
        tmpWidgetPtr = mDocumentLoader->findWidget(QString("repeatIcon"));
        mRepeatIndicator = qobject_cast<HbLabel*>(tmpWidgetPtr);
                
        tmpWidgetPtr = mDocumentLoader->findWidget(QString("progressBar"));
        mProgressBar = qobject_cast<HbProgressSlider*>(tmpWidgetPtr);

        //TODO: move this to docml when supported.
        mProgressBar->setMinMaxTextVisible(true);
    }
    else {
        TX_LOG_ARGS("Error: invalid xml file.");
        Q_ASSERT_X(widgetsOk, "MpPlaybackWidget", "invalid xml file - widget");
    }

    mLightMonitor = new MpLightMonitor (this);
    connect( mLightMonitor, SIGNAL(lcdLightStatus(bool)), this, SLOT(handleLcdLightStatus(bool)) );

    connect( mProgressBar, SIGNAL(sliderPressed()), this, SLOT(handleSliderPressed()) );
    connect( mProgressBar, SIGNAL(sliderReleased()), this, SLOT(handleSliderReleased()) );
    connect( mProgressBar, SIGNAL(sliderMoved(int)), this, SLOT(handleSliderMoved(int)) );

    connectPlaybackDataSignals(true);
    // Update to set initial conditions if playback has already started.
    if ( mPlaybackData->playbackState() != MpPlaybackData::NotPlaying ) {
        updatePlaybackInfo();
    }
    TX_EXIT
}

/*!
 Constructs the collection view plugin.
 */
MpPlaybackWidget::~MpPlaybackWidget()
{
    TX_ENTRY
    delete mDocumentLoader;
    delete mLightMonitor;
    TX_EXIT
}

/*!
 Handle Repeat changed.
 */
void MpPlaybackWidget::repeatChanged( bool value )
{
    mRepeatIndicator->setVisible( value );
}

/*!
 Slot to handle playback info changed.
 */
void MpPlaybackWidget::playbackInfoChanged( )
{
    TX_ENTRY
    //Title
    mSongTitle->setPlainText( mPlaybackData->title() );
    // Artist
    QString data = mPlaybackData->artist();
    if ( !data.isEmpty() ) {
        mArtistName->setPlainText( data );
    }
    else {
        mArtistName->setPlainText( hbTrId( "txt_mus_other_unknown3" ) );
    }
    // Album
    data = mPlaybackData->album();
    if ( !data.isEmpty() ) {
        mAlbumName->setPlainText( data );
    }
    else {
        mAlbumName->setPlainText( hbTrId( "txt_mus_other_unknown4" ) );
    }
    // RealAudioIcon
    mRealAudioIndicator->setVisible( mPlaybackData->realAudio() );
    emit signalPlaybackInfoChanged();
    TX_EXIT
}

/*!
 Slot to handle playback duration changed.
 */
void MpPlaybackWidget::durationChanged()
{
    TX_ENTRY
    mDuration = mPlaybackData->duration();
    mDuration /= KMicroSecToMiliSec;

    mProgressBar->setRange(0, mDuration);
    mProgressBar->setProgressValue(0);
    mProgressBar->setSliderValue(0);
    mProgressBar->setEnabled(true);
    TX_EXIT
}

/*!
 Slot to handle playback position changed.
 */
void MpPlaybackWidget::positionChanged()
{
    TX_ENTRY
    int position = mPlaybackData->position();
    position /= KMicroSecToMiliSec;

    if ( !mProgreesBarDragging ) {
        mProgressBar->setProgressValue(position);
        mProgressBar->setSliderValue(position);
        mProgressBar->setMinText( formatDuration(position) );
        mProgressBar->setMaxText( formatDuration(mDuration  - position) );
    }
    TX_EXIT
}

/*!
 Slot to handle Album art changed.
 */
void MpPlaybackWidget::albumArtChanged( )
{
    TX_ENTRY
    HbIcon icon;
    mPlaybackData->albumArt( icon );
    mAlbumArt->setIcon( icon );
    mAlbumArt->setEnabled( true );
    TX_EXIT
}

/*!
 Slot to handle slider pressed.
 */
void MpPlaybackWidget::handleSliderPressed()
{
    TX_LOG
    mProgreesBarDragging = true;
}

/*!
 Slot to handle progressbar relesed.
 */
void MpPlaybackWidget::handleSliderReleased()
{
    TX_ENTRY
    mProgreesBarDragging = false;
    emit setPlaybackPosition( KMicroSecToMiliSec * mProgressBar->progressValue() );
    TX_EXIT
}


/*!
 Slot to handle progressbar moved.
 */
void MpPlaybackWidget::handleSliderMoved( int value )
{
    TX_ENTRY_ARGS( "value  =" << value )
    mProgressBar->setProgressValue(value);
    mProgressBar->setSliderValue(value);
    mProgressBar->setMinText( formatDuration(value) );
    mProgressBar->setMaxText( formatDuration(mDuration  - value) );
    mProgressBar->setSliderToolTip( formatDuration(value) );
    if ( !mProgreesBarDragging ) {
        // Click on the progress bar, not a drag.
        emit setPlaybackPosition( KMicroSecToMiliSec * value );
    }
    TX_EXIT
}

/*!
 Returns the formated duration defined in \a seconds.
 */
QString MpPlaybackWidget::formatDuration( int seconds )
{
    TX_ENTRY
    QTime z(0,0,0,0);
    QTime duration;
    duration = z.addSecs( seconds );
    int hour = seconds / 3600;
    if ( hour == 0 ) {
        TX_EXIT
        return duration.toString( "mm:ss" );
    }
    else {
        TX_EXIT
        return duration.toString( "hh:mm:ss" );
    }
}


/*!
 Slot to be called to handle LCD light status change.
 */
void MpPlaybackWidget::handleLcdLightStatus( bool lightOn )
{
    TX_ENTRY
    if ( lightOn ) {
        connectPlaybackDataSignals( true );
        updatePlaybackInfo();
    }
    else {
        connectPlaybackDataSignals( false );
    }
    TX_EXIT
}

/*!
 Connect/disconnect the signals from the Playback data according to /a connect.
 */
void MpPlaybackWidget::connectPlaybackDataSignals( bool connectSignal )
{
    TX_ENTRY
    if ( connectSignal ) {
        connect( mPlaybackData, SIGNAL(playbackInfoChanged()), this, SLOT(playbackInfoChanged()) );
        connect( mPlaybackData, SIGNAL(durationChanged()), this, SLOT(durationChanged()) );
        connect( mPlaybackData, SIGNAL(positionChanged()), this, SLOT(positionChanged()) );
        connect( mPlaybackData, SIGNAL(albumArtReady()), this, SLOT(albumArtChanged()) );
        connect( mPlaybackData, SIGNAL(collectionPlaylistOpened()), this, SLOT(handleCollectionPlaylistOpened()) );     
       }
    else {
        mPlaybackData->disconnect( this );
    }
    TX_EXIT
}

/*!
 Update playback information.
 */
void MpPlaybackWidget::updatePlaybackInfo()
{
    TX_ENTRY
    playbackInfoChanged();
    durationChanged();
    positionChanged();
    albumArtChanged();
    TX_EXIT
}

/*!
 Slot to be called to handle the selection of an item from collection view when playback
 view was previously activated with another item. We reset or hide the view items 
 in order to reduce the flickering.
 */
void MpPlaybackWidget::handleCollectionPlaylistOpened()
{
    TX_ENTRY
    QString blankSpace = " ";   
    mAlbumArt->setEnabled( false );
    mArtistName->setPlainText( blankSpace );
    mAlbumName->setPlainText( blankSpace );
    mSongTitle->setPlainText( blankSpace );
    mRealAudioIndicator->setVisible( false );
    mProgressBar->setProgressValue( 0 );
    mProgressBar->setSliderValue( 0 );
    mProgressBar->setMinText( formatDuration( 0 ) );
    mProgressBar->setMaxText( formatDuration( 0 ) );
    TX_EXIT
}

