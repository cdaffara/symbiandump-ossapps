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
* Description: Music Player now playing widget - Private.
*
*/
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include <hbstackedlayout.h>
#include <hblabel.h>
#include <hbicon.h>
#include <hbiconitem.h>
#include <hbdocumentloader.h>
#include <hbinstance.h>
#include <hbstyleloader.h>

#include "mpenginefactory.h"
#include "mpnowplayingwidget_p.h"
#include "mpnowplayingwidget.h"
#include "mptrace.h"

const char *NOW_PLAYING_XML = ":/xml/nowplaying.docml";
const char *NOW_PLAYING_CSS = ":/css/banner_color.css";

/*!
    \internal
    \class MpNowPlayingWidgetPrivate
    \brief Custom HB widget that displays currently playing song information.

    This is a composite widget made out of HB widgets, it displays information
    for the current playing song on the playback engine indicated at
    construction time.
*/

/*!
 \internal
 Constructs the now playing widget private.
 */
MpNowPlayingWidgetPrivate::MpNowPlayingWidgetPrivate( MpNowPlayingWidget *qq )
    : q_ptr( qq ),
      mPrimaryText(0),
      mSecondaryText(0),
      mState( MpPlaybackData::NotPlaying ),
      mPlaybackData(0),
      mMpEngine(0),
      mIcon(0),
      mDocumentLoader(0),
      mCurrentPressedState( BannerNone )
{
    TX_ENTRY_ARGS( " Q pointer=" << ( void * )qq )
    QGraphicsWidget *widget;
    bool widgetsOk = false;
    
    HbStyleLoader::registerFilePath(NOW_PLAYING_CSS);  
    
    mPlayIcon = new HbIcon( QString( "qtg_mono_play" ) );
    mPauseIcon = new HbIcon( QString( "qtg_mono_pause" ) );

    mDocumentLoader = new HbDocumentLoader();
    if (mDocumentLoader) {
            mDocumentLoader->load( NOW_PLAYING_XML, &widgetsOk);
    }

    if (widgetsOk) {

        // find the main widget container and all of its child items
        widget = mDocumentLoader->findWidget(QString("nowPlayingContainer"));
        HbWidget* container = qobject_cast<HbWidget*>(widget);
        container->setParentItem(q_ptr);

        widget = mDocumentLoader->findWidget(QString("primaryText"));
        mPrimaryText = qobject_cast<HbLabel*>(widget);
        mPrimaryText->setObjectName( "bannerLabel" );
        mPrimaryText->setProperty( "state", "normal" );
        

        widget = mDocumentLoader->findWidget(QString("secondaryText"));
        mSecondaryText = qobject_cast<HbLabel*>(widget);
        mSecondaryText->setObjectName( "bannerLabel" );
        mSecondaryText->setProperty( "state", "normal" );
        
        
        widget = mDocumentLoader->findWidget(QString("playPause"));
        mIcon = qobject_cast<HbLabel*>(widget);
        mIcon->setObjectName( "bannerLabel" );
        mIcon->setProperty( "state", "normal" );
        
        HbStackedLayout *mylayout;
        mylayout = new HbStackedLayout(q_ptr);
        mylayout->addItem( container );

        q_ptr->setLayout( mylayout );
    }
    else {
        TX_LOG_ARGS("Error: invalid xml file.");
        Q_ASSERT_X(widgetsOk, "MpNowPlayingWidgetPrivate", "invalid xml file");
    }

    mMpEngine = MpEngineFactory::sharedEngine();
    mPlaybackData = mMpEngine->playbackData();
    setEnabled(true);
    TX_EXIT
}


/*!
 \internal
 Destructs the now playing widget private.
 */
MpNowPlayingWidgetPrivate::~MpNowPlayingWidgetPrivate()
{
    TX_ENTRY
    delete mPlayIcon;
    delete mPauseIcon;
    delete mDocumentLoader;
    TX_EXIT
}

/*!
 If \a enabled is true, the item is enabled; otherwise, it is disabled.
 Item is enabled by default.
 */
void MpNowPlayingWidgetPrivate::setEnabled( bool enabled )
{
    if ( enabled ) {
        connect( mPlaybackData, SIGNAL( playbackStateChanged() ),
                 this, SLOT( setState() ) );

        connect( mPlaybackData, SIGNAL( playbackInfoChanged() ),
                 this, SLOT( updateBannerInfo() ) );

    setState();
    updateBannerInfo();
    }
    else {
        disconnect( mPlaybackData, SIGNAL( playbackStateChanged() ),
                 this, SLOT( setState() ) );
    
        disconnect( mPlaybackData, SIGNAL( playbackInfoChanged() ),
                 this, SLOT( updateBannerInfo() ) );
    }
}

/*!
 Return if banner is attached based on current playback state
 */
bool MpNowPlayingWidgetPrivate::isBannerAttached()
{
    return mState == MpPlaybackData::NotPlaying ? false : true;
}

/*!
 \internal
 Offers click \a event to the widget privated side, if event is consumed it returns true.
 */

bool MpNowPlayingWidgetPrivate::handleClickEvent(QGraphicsSceneMouseEvent *event)
{
    bool ret = true;
    if ( mCurrentPressedState != BannerNone ) {
        //Widget was previosly pressed, handle the event.
        QRectF geometry = q_ptr->rect();
        qreal delta = mIcon->windowFrameGeometry().top() - geometry.top();
        QRectF iconTouchRect( mIcon->windowFrameGeometry() );
        iconTouchRect.adjust( -delta, -delta, delta, delta );
        if ( iconTouchRect.contains( event->pos() ) ) {
            if ( mCurrentPressedState == BannerIcon ) {
                    // click on play/pause
                mMpEngine->playPause();
            }
        }
        else if ( mCurrentPressedState == BannerLabels && geometry.contains( event->pos() )){
            // click somewhere else on the widget.
            ret = false;
        }
        /*else {
        // click outside the widget.
        }*/
        mCurrentPressedState = BannerNone;
    }
    return ret;
}

/*!
 \internal
 Changes text and icon color when \a pressed
 */
void MpNowPlayingWidgetPrivate::handleMousePressEvent(QGraphicsSceneMouseEvent *event, bool pressed)
{   

    QRectF geometry = q_ptr->rect();
    qreal delta = mIcon->windowFrameGeometry().top() - geometry.top();
    QRectF iconTouchRect( mIcon->windowFrameGeometry() );
    iconTouchRect.adjust( -delta, -delta, delta, delta );
    
    if( iconTouchRect.contains( event->pos() ) &&  pressed ) {  
        if( mCurrentPressedState == BannerNone ) {
            mCurrentPressedState = BannerIcon;
        }
        mIcon->setProperty( "state", "pressed" );
    }
    else if( geometry.contains( event->pos() ) && pressed ){
        if( mCurrentPressedState == BannerNone ) {
            mCurrentPressedState = BannerLabels;
        }
        mPrimaryText->setProperty( "state", "pressed" );
        mSecondaryText->setProperty( "state", "pressed" );
    }
    else { 
        mIcon->setProperty( "state", "normal" );
        mPrimaryText->setProperty( "state", "normal" );
        mSecondaryText->setProperty( "state", "normal" );
    }
}

/*!
 \internal
 Changes text and icon color when moved
 */
void MpNowPlayingWidgetPrivate::handleMouseMoveEvent(QGraphicsSceneMouseEvent *event)
{   

    QRectF geometry = q_ptr->rect();
    qreal delta = mIcon->windowFrameGeometry().top() - geometry.top();
    QRectF iconTouchRect( mIcon->windowFrameGeometry() );
    iconTouchRect.adjust( -delta, -delta, delta, delta );
    
    if( iconTouchRect.contains( event->pos() ) ) {  
        if( mCurrentPressedState == BannerIcon ) {
            mIcon->setProperty( "state", "pressed" );
        }
        else {
            mIcon->setProperty( "state", "normal" );
        }
        mPrimaryText->setProperty( "state", "normal" );
        mSecondaryText->setProperty( "state", "normal" );
    }
    else if( geometry.contains( event->pos() ) &&  mCurrentPressedState == BannerLabels ){
        mPrimaryText->setProperty( "state", "pressed" );
        mSecondaryText->setProperty( "state", "pressed" );
        mIcon->setProperty( "state", "normal" );
    }
    else { 
        mIcon->setProperty( "state", "normal" );
        mPrimaryText->setProperty( "state", "normal" );
        mSecondaryText->setProperty( "state", "normal" );
    }
}

/*!
 \internal
 Sets the simplified playback \a state.
 */
void MpNowPlayingWidgetPrivate::setState( )
{
    MpPlaybackData::SimplifiedState state = mPlaybackData->playbackState();
    TX_ENTRY_ARGS( "State = " << state )
            
    switch ( state ) {
        case MpPlaybackData::NotPlaying:
            if ( mState != MpPlaybackData::NotPlaying )
                emit q_ptr->playbackAttachmentChanged( false );
            break;
        case MpPlaybackData::Playing:
            mIcon->setIcon( *mPauseIcon );
            mIcon->setProperty( "state", mIcon->property("state").toString() );
            if ( mState == MpPlaybackData::NotPlaying )
                emit q_ptr->playbackAttachmentChanged( true );
            break;
        case MpPlaybackData::Paused:
        case MpPlaybackData::Stopped:    
            mIcon->setIcon( *mPlayIcon );
            mIcon->setProperty( "state", mIcon->property("state").toString() );
            if ( mState == MpPlaybackData::NotPlaying )
                emit q_ptr->playbackAttachmentChanged( true );
            break;
        default:
            break;
    }
    mState = state;
    TX_EXIT
}

/*!
 \internal
 Sets the \a title 
 */
void MpNowPlayingWidgetPrivate::updateBannerInfo()
{
    mPrimaryText->setPlainText( mPlaybackData->title() );
    mPrimaryText->setProperty( "state", mPrimaryText->property("state").toString() );
    
    QString data = mPlaybackData->artist();
    if ( !data.isEmpty() ) {
        mSecondaryText->setPlainText( data );
    }
    else {
        mSecondaryText->setPlainText( hbTrId( "txt_mus_other_unknown" ) );
    }
    mSecondaryText->setProperty( "state", mSecondaryText->property("state").toString() );
}

