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
* Description: Music Player Settings view.
*
*/

#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

#include <hbicon.h>
#include <hbaction.h>
#include <hbapplication.h>
#include <hbscrollbar.h>
#include <hblabel.h>
#include <hblistview.h>
#include <hbstyleloader.h>
#include <hbmessagebox.h>

#include <hgmediawall.h>

#include "mpmediawallview.h"
#include "mpmediawalldocumentloader.h"
#include "mpcommondefs.h"
#include "mpmpxcollectiondata.h"
#include "mpcollectiondatamodel.h"
#include "mpenginefactory.h"
#include "mpplaybackdata.h"
#include "mpalbumcoverwidget.h"
#include "mptracklistwidget.h"
#include "mpcollectiontbonelistdatamodel.h"
#include "mptrace.h"

const char*MUSIC_MEDIAWALL_DOCML = ":/mediawallviewdocml/mediawall.docml";

/*!
    \class MpMediaWallView
    \brief Music Player Settings view.

    Music player media wall view displays music players albums on an full screen view
*/

/*!
    \fn void command( int command )

    This signal is emitted when the view issues a \a command to the
    application such as request to switch to a different view.
 */


/*!
 Constructs the media wall view.
 */
MpMediaWallView::MpMediaWallView()
    : mEngine( 0 ),
      mCollectionData( 0 ),
      mModel( 0 ),
      mMediaWallWidget( 0 ),
      mNoMusic( 0 ),
      mDocumentLoader( 0 ),
      mLibraryUpdating( false ),
      mPlaybackData( 0 ),
      mListShowAnimationGroup( 0 ),
      mListShowListAnimation( 0 ),
      mCoverShowListAnimation( 0 ),
      mShowingSongsList( false ),
      mAlbumCover( 0 ),
      mTrackList( 0 )
{
    connect( MpEngineFactory::sharedEngine(), SIGNAL( libraryAboutToUpdate() ),
                    this, SLOT( libraryAboutToUpdate() ) );
    connect( MpEngineFactory::sharedEngine(), SIGNAL( libraryUpdated() ),
                    this, SLOT( libraryUpdated() ) );
    TX_LOG
}
    
/*!
 Destructs the media wall view.
 */
MpMediaWallView::~MpMediaWallView()
{
    TX_ENTRY
    //delete not parented objects
    delete mDocumentLoader;
    TX_EXIT
}

/*!
 Initializes the media wall view.
 */
void MpMediaWallView::initializeView()
{
    TX_ENTRY
    setTitleBarVisible( false );
    setStatusBarVisible( false );
       
    mDocumentLoader = new MpMediaWallDocumentLoader();  
    bool ok = false;
    mDocumentLoader->load( MUSIC_MEDIAWALL_DOCML, &ok );
    if ( ok ) {
        QGraphicsWidget *widget;
        
        
        mPlaybackData = MpEngineFactory::sharedEngine()->playbackData();
               
        //Get the main container and set as widget.
        widget = mDocumentLoader->findWidget( QString( "container" ) );
        setWidget( widget );
        
        mEngine = MpEngineFactory::createIsolatedEngine( MpEngine::MediaBrowsing );
        mCollectionData = mEngine->collectionData();
        mModel = new MpCollectionDataModel( mCollectionData, 0, this );
        
        connect( mCollectionData, SIGNAL( contextChanged( TCollectionContext ) ), 
                 this, SLOT( contextOpened( TCollectionContext ) ) );
        connect( mEngine, SIGNAL( containerContentsChanged() ),
                this, SLOT( containerContentsChanged() ) );
        if ( !mLibraryUpdating ) {
        mEngine->openCollection( ECollectionContextAlbumsMediaWall );
        }
    }
    else {
        TX_LOG_ARGS( "Error: invalid xml file." );
        Q_ASSERT_X( ok, "MpCollectionView::initializeView", "invalid xml file" );
    }
       
    mTrackList = new MpTrackListWidget( this );
    MpCollectionTBoneListDataModel *model = new MpCollectionTBoneListDataModel(mCollectionData, mPlaybackData, mTrackList );
    model->enablePlaybackIndicatorEnable(true);
    mTrackList->list()->setModel( model );
    mTrackList->hide();
    
    mAlbumCover = new MpAlbumCoverWidget( this );
    mAlbumCover->setDefaultIcon( HbIcon( "qtg_large_album_art" ) );
    mAlbumCover->hide();
    
    connect(mAlbumCover,SIGNAL(clicked()),this, SLOT(hideTracksList()));
    connect(mTrackList->list(), SIGNAL(activated(QModelIndex)), this, SLOT(listItemActivated(QModelIndex)));
    connect(mTrackList,SIGNAL(closed()),this, SLOT(hideTracksList()));

    HbStyleLoader::registerFilePath(":/css/mpcustommediawall.css");
    HbStyleLoader::registerFilePath(":/css/mpcustommediawall_color.css");
    HbStyleLoader::registerFilePath(":/css/mpcustommediawall.hgmediawall.widgetml");  
    
    TX_EXIT
}

/*!
 Activates the media wall view.
 */
void MpMediaWallView::activateView()
{
    TX_ENTRY
    scrollToNowPlaying();   
    TX_EXIT
}

/*!
 Deactivates the media wall view.
 */
void MpMediaWallView::deactivateView()
{
    TX_ENTRY
    dismissListClosingAnimation();
    TX_EXIT
}

/*!
 Slot to be called when the context is opened.
 */
void MpMediaWallView::contextOpened( TCollectionContext context )
{
    if ( context != ECollectionContextAlbumsMediaWall ) {
        //Force the context to Albums Media Wall, this to get the model and 
        //data to provide the right funtionality to media wall.
        mCollectionData->setContext( ECollectionContextAlbumsMediaWall );
        return;
    }
    if ( mCollectionData->count() ) {
        if (mNoMusic) {
            delete mNoMusic;
            mNoMusic = 0;
        }
        if ( mMediaWallWidget ) {
            mModel->refreshModel();
            scrollToDefault();
        } else {
            bool ok = false;
            mDocumentLoader->load( MUSIC_MEDIAWALL_DOCML, "mediaWall", &ok);
            if ( ok ) {
                QGraphicsWidget *widget;
                widget = mDocumentLoader->findWidget(QString("MPmediaWallWidget"));
                mMediaWallWidget = qobject_cast<HgMediawall*>(widget);
                //set these items as children of the media wall so they show behind the scrollbar.
                mModel->refreshModel();
                setUpMediaWallWidget();
            }
            else {
                TX_LOG_ARGS("Error: invalid xml file.");
                Q_ASSERT_X(ok, "MpMediaWallView::contextOpened", "invalid xml file");

            }          
        }
    }
    else {
        if ( mMediaWallWidget ) {
            //Take ownership of these items to prevent delete from media wall widget.
            delete mMediaWallWidget;
            mMediaWallWidget = 0;
            
        }
        if ( !mNoMusic ) {
            bool ok = false;
            mDocumentLoader->load( MUSIC_MEDIAWALL_DOCML, "empty", &ok);
            if ( ok ) {
                QGraphicsWidget *widget;
                widget = mDocumentLoader->findWidget(QString("noMusic"));
                mNoMusic = qobject_cast<HbLabel*>(widget);
            }
            else {
                TX_LOG_ARGS("Error: invalid xml file.");
                Q_ASSERT_X(ok, "MpMediaWallView::contextOpened", "invalid xml file");

            }
        }
    }
}

/*!
 Slot to be called when the container contents changed.
 */
void MpMediaWallView::containerContentsChanged() 
{
    if ( !mLibraryUpdating && mEngine ) {
        dismissListClosingAnimation();
        mEngine->openCollection( ECollectionContextAlbumsMediaWall );
    }
}

/*!
 Slot to be called when the container contents changed.
 */
void MpMediaWallView::libraryAboutToUpdate()
{
    mLibraryUpdating = true;
}

/*!
 Slot to be called when the container contents changed.
 */
void MpMediaWallView::libraryUpdated()
{
    mLibraryUpdating = false;
    if ( mEngine ) {
        mEngine->openCollection( ECollectionContextAlbumsMediaWall );
    }
}

/*!
 Slot called to start the album and list opening animation.
 */
void MpMediaWallView::showTrackList()
{
    if( mShowingSongsList ) {
        return;
    }
    mShowingSongsList = true;
    setUpListAnimation();
    mListShowAnimationGroup->setDirection(QAbstractAnimation::Forward);
    mListShowAnimationGroup->start();
    mModel->setItemVisibility( mMediaWallWidget->currentIndex(), false );
}

/*!
 Slot to be called to starts the album and list closing animation.
 */
void MpMediaWallView::hideTracksList()
{
    mListShowAnimationGroup->setDirection(QAbstractAnimation::Backward);
    connect(mListShowAnimationGroup, SIGNAL(finished()), this, SLOT(dismissListClosingAnimation()));
    mListShowAnimationGroup->start();
}

/*!
 Slot to be called when list closing animation is done or when we want to cancel the animation.
 */
void MpMediaWallView::dismissListClosingAnimation()
{
    if ( !mShowingSongsList ) {
        return;
    }
    mShowingSongsList = false;
    mAlbumCover->hide();
    mTrackList->hide();
    mModel->setItemVisibility( mMediaWallWidget->currentIndex(), true );
    mListShowAnimationGroup->stop();
    disconnect(mListShowAnimationGroup, SIGNAL(finished()), this, SLOT(dismissListClosingAnimation()));
}

/*!
 Slot to be called when a list item is selected by the user.
 */
void MpMediaWallView::listItemActivated( const QModelIndex &index )
{
    if (mCollectionData->hasAlbumSongProperty(index.row(), MpMpxCollectionData::Corrupted)) {
        //pop up corrupted note
        HbMessageBox *messageBox = new HbMessageBox( hbTrId( "txt_mus_info_file_is_corrupt" ), HbMessageBox::MessageTypeInformation );
        messageBox->setAttribute( Qt::WA_DeleteOnClose );
        messageBox->setIcon( HbIcon( QString("qtg_small_fail") ) ); 
        messageBox->show();   
    }
    else if	( (mPlaybackData->playbackState() != MpPlaybackData::NotPlaying ) 
        && ( mPlaybackData->id() == mCollectionData->albumSongId( index.row() ) ) ) {
         MpEngineFactory::sharedEngine()->playPause();
    }
    else {
        int albumIndex = mMediaWallWidget->currentIndex().row();
        //We are playing on the shared engine, but we pass the collection data that
        //points to albums on media wall, this is used to construct the playlist.
        MpEngineFactory::sharedEngine()->playAlbumSongs( albumIndex, index.row(), mCollectionData );
    }
}

/*!
 Slot to be called to fetch the songs for an album.
 */
void MpMediaWallView::fetchAlbumSongs(QModelIndex index)
{
    if ( !mCollectionData->setCurrentAlbum( index.row() ) ) {
        mEngine->findAlbumSongs( index.row() );
    }
}

/*!
 Loads the media wall widget.
 */
void MpMediaWallView::setUpMediaWallWidget()
{
    HbIcon defaultIcon( "qtg_large_album_art" );
    defaultIcon.setSize(mMediaWallWidget->itemSize());
    mMediaWallWidget->setDefaultImage( defaultIcon.pixmap().toImage() );
    mMediaWallWidget->enableReflections( false );
    mMediaWallWidget->setModel( mModel );
    scrollToDefault();
    mMediaWallWidget->setTitleFontSpec( HbFontSpec( HbFontSpec::Primary ) );
    mMediaWallWidget->setDescriptionFontSpec( HbFontSpec( HbFontSpec::Secondary ) );
    mMediaWallWidget->setScrollBarPolicy( HgWidget::ScrollBarAlwaysOn ); //HgWidget::ScrollBarAutoHide
    mMediaWallWidget->scrollBar()->setInteractive( true );
    mMediaWallWidget->setIndexFeedbackPolicy( HgWidget::IndexFeedbackSingleCharacter );
    connect(mMediaWallWidget, SIGNAL(animationAboutToEnd(QModelIndex)), SLOT(fetchAlbumSongs(QModelIndex)));
    connect(mMediaWallWidget, SIGNAL(activated(QModelIndex)), SLOT(showTrackList()));
    connect(mMediaWallWidget, SIGNAL(scrollingStarted()), SLOT(dismissListClosingAnimation()));
}
/*!
 Scrolls the media wall to a default position.
 */
void MpMediaWallView::scrollToDefault()
{
    //if something is playing go there, if not go to a balanced position.
    if ( mPlaybackData->playbackState() != MpPlaybackData::NotPlaying ) {
        scrollToNowPlaying();
    }
    else  if ( mModel->rowCount() >= 5 ) {
        /* 5 and more
        [0] [1] [X] [3] [4]...
        */
        QModelIndex index;
        index = mModel->index( 2 );
        mMediaWallWidget->scrollTo( index );
        fetchAlbumSongs( index );
        
    }
    else if ( mModel->rowCount() >=3  ) {
        /* 4 and 3
        __ [0] [X] [2] [3]
        __ [0] [X] [2] __
        */
        QModelIndex index;
        index = mModel->index( 1 );
        mMediaWallWidget->scrollTo( index );
        fetchAlbumSongs( index );
    }
    else {
        /* 2 and 1
        __ __ [X] [1] __
        __ __ [X] __ __
        */
        QModelIndex index;
        index = mModel->index( 0 );
        mMediaWallWidget->scrollTo( index );
        fetchAlbumSongs( index );
    }
}

/*!
 Scrolls the media wall to now playing position.
 */
void MpMediaWallView::scrollToNowPlaying()
{
    if ( mMediaWallWidget  && mPlaybackData->playbackState() != MpPlaybackData::NotPlaying ) {
        int albumId = mPlaybackData->albumId();
        QModelIndex index;
        index = mModel->index( mCollectionData->itemIndex( albumId ) );
        mMediaWallWidget->scrollTo( index );
        fetchAlbumSongs( index );
    }
}

/*!
 Sets up the album and list animation.
 */
void MpMediaWallView::setUpListAnimation()
{
    if ( !mListShowAnimationGroup ) {
        mListShowAnimationGroup = new QParallelAnimationGroup( this );
        mCoverShowListAnimation = new QPropertyAnimation(mAlbumCover, "geometry", this );
        mCoverShowListAnimation->setDuration( 400 );        
        mCoverShowListAnimation->setEasingCurve(QEasingCurve::InOutCubic);
        mListShowAnimationGroup->addAnimation(mCoverShowListAnimation);
        mListShowListAnimation = new QPropertyAnimation( mTrackList, "geometry", this );
        mListShowListAnimation->setDuration( 400 );
        mListShowListAnimation->setEasingCurve(QEasingCurve::InOutCubic);
        mListShowAnimationGroup->addAnimation(mListShowListAnimation);      
    }
    
    //Get the current album cover geometry.
    QPolygonF poly;
    QModelIndex index = mMediaWallWidget->currentIndex();
    if ( !mMediaWallWidget->getItemOutline( index, poly ) ) {
        return;
    }
    QRectF itemRect = poly.boundingRect();
    
    //Get the album cover icon.
    QVariant image = mModel->data(index, Qt::DecorationRole);
    if ( image.canConvert<HbIcon>() ) {
       mAlbumCover->setIcon(image.value<HbIcon>());
    }
    else {
        mAlbumCover->setIcon(HbIcon());
    }
    
    //Mapping form local media wall coords to window coors.
    itemRect.moveTo( mMediaWallWidget->geometry().topLeft() + itemRect.topLeft());   
    //Calculate start and end position of items for the animation.
    QRectF endRect = itemRect;
    endRect.moveTo( endRect.topLeft().x() - endRect.size().width() / 2.0, endRect.topLeft().y() );
    mCoverShowListAnimation->setStartValue(itemRect);
    mCoverShowListAnimation->setEndValue(endRect);
    endRect.moveTo( endRect.topRight() );
    mListShowListAnimation->setStartValue(itemRect);
    mListShowListAnimation->setEndValue(endRect);
    
    //set initial condition and show to prevent flicker.
    mAlbumCover->setPos(itemRect.topLeft());
    mTrackList->setPos(itemRect.topLeft());
    mTrackList->setGeometry(itemRect);
    mAlbumCover->show();
    mTrackList->show();
}

