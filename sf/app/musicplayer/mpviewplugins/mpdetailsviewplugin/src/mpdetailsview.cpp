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
* Description: Music Player details view.
*
*/
#include <QWebSettings>
#include <QGraphicsWidget>
#include <QGraphicsLayoutItem>
#include <QUrl>
#include <QDomElement>
#include <QList>
#include <QFile>
#include <QGraphicsLinearLayout>
#include <QSizeF>
#include <QDesktopServices>


#include <hbinstance.h>
#include <hbmainwindow.h>
#include <hbaction.h>
#include <hblabel.h>
#include <hbgroupbox.h>
#include <hbdocumentloader.h>
#include <hbwidget.h>
#include <hbpushbutton.h>
#include <hbpopup.h>
#include <hbdialog.h>
#include <hblistwidget.h>
#include <hblistwidgetitem.h>
#include <hblistviewitem.h>
#include <hbprogressbar.h>

#include "mpdetailsview.h"
#include "mpenginefactory.h"
#include "mpcommondefs.h"
#include "mpsongdata.h"
#include "mpsettingsmanager.h"
#include "mpquerymanager.h"
#include "mptrace.h"


const int KOneKiloByteInBytes = 1024;
const int KOneMegabyteInBytes = 1048576;        
const int KOneGigaByteInBytes = 134217728;        

/*!
 Constructor
 */
MpDetailsView::MpDetailsView()
    : mMpEngine( 0 ),
      mSongData( 0 ),
      mActivated( false ),
      mSoftKeyBack( 0 ),
      mSongText( 0 ),
      mAlbumText( 0 ),
      mArtistText( 0 ),
      mAlbumArt( 0 ),
      mSongDetailsGroupBox(0),
      mInspireMeGroupBox(0),
      mDocumentLoader( 0 ),
      mDetailList(0),
      mInspireList(0),
      mInspireMeProgressBar(0),
      mMpQueryManager( 0 ),
      mInspireMeQueryOngoing( false ),
      mInspireMeQueryRendered( false )
{
    TX_ENTRY
    bool widgetsOk = false;
    HbMainWindow *mainWindow = hbInstance->allMainWindows()[0];
    mDocumentLoader = new HbDocumentLoader();

    TX_LOG_ARGS("Document loader created")
    if(!mDocumentLoader) {
        TX_LOG_ARGS( "MpDetailsView() Error: HbDocumentLoader not constructed" );
        return;
    }

    TX_LOG_ARGS("Loading :/mpdetailsviewdocml/mpdetailsview.docml")
    mDocumentLoader->load( QString(":/mpdetailsviewdocml/mpdetailsview.docml"), &widgetsOk );
    if(!widgetsOk) {
        TX_LOG_ARGS( "MpDetailsView() Error: invalid mpdetailsview.docml" );
        return;
    }

    TX_LOG_ARGS("Loaded :/mpdetailsviewdocml/mpdetailsview.docml")
    QGraphicsWidget *widget = 0;

    widget = mDocumentLoader->findWidget( QString("content") );
    TX_LOG_ARGS( "MpDetailsView() mDocumentLoader->findWidget for <content>: " << (int)(widget) );
    setWidget(  qobject_cast<HbWidget *>(widget) );

    widget = mDocumentLoader->findWidget( QString("trackTitle") );
    mSongText = qobject_cast<HbLabel *>(widget);
    TX_LOG_ARGS("MpDetailsView() <trackTitle> widget found mSongText: " << (int)(mSongText) );

    widget = mDocumentLoader->findWidget( QString("artist") );
    mArtistText = qobject_cast<HbLabel *>(widget);
    TX_LOG_ARGS("MpDetailsView() <artist> widget found mArtistText: " << (int)(mArtistText) );

    widget = mDocumentLoader->findWidget( QString("albumName") );
    mAlbumText = qobject_cast<HbLabel *>(widget);
    TX_LOG_ARGS("MpDetailsView() <albumName> widget found mAlbumText: " << (int)(mAlbumText) );

    widget = mDocumentLoader->findWidget( QString("albumArt") );
    mAlbumArt = qobject_cast<HbLabel *>(widget);
    TX_LOG_ARGS("MpDetailsView() <albumArt> widget found mAlbumArt: " << (int)(mAlbumArt) );

    widget = mDocumentLoader->findWidget( QString("songDetails") );
    mSongDetailsGroupBox = qobject_cast<HbGroupBox *>(widget);
    TX_LOG_ARGS("MpDetailsView() <songDetails> widget found mSongDetailsGroupBox: " << (int)(mSongDetailsGroupBox) );

    widget = mDocumentLoader->findWidget( QString("songDetailsListWidget") );
    mDetailList = qobject_cast<HbListWidget *>(widget);
    TX_LOG_ARGS("MpDetailsView() <songDetailsListWidget> widget found mDetailList: " << (int)(mDetailList) );

    widget = mDocumentLoader->findWidget( QString("inspireMeGroupBox") );
    mInspireMeGroupBox = qobject_cast<HbGroupBox *>(widget);
    TX_LOG_ARGS("MpDetailsView() <inspireMeGroupBox> widget found mInspireMeGroupBox: " << (int)(mInspireMeGroupBox) );

    widget = mDocumentLoader->findWidget( QString("inspireListWidget") );
    mInspireList = qobject_cast<HbListWidget *>(widget);
    TX_LOG_ARGS("MpDetailsView() <inspireListWidget> widget found mInspireList: " << (int)(mInspireList) );

    widget = mDocumentLoader->findWidget( QString("inspireMeProgressBar") );
    mInspireMeProgressBar = qobject_cast<HbProgressBar *>(widget);
    TX_LOG_ARGS("MpDetailsView() <inspireMeProgressBar> widget found mInspireMeProgressBar: " << (int)(mInspireMeProgressBar) );
    
    // load section HideInspireMe if no ovi store suport
    bool loadingSectionOk = false;
    mDocumentLoader->load( QString(":/mpdetailsviewdocml/mpdetailsview.docml"), QString( "ShowInspireMe" ), &loadingSectionOk );    
    if( loadingSectionOk ) {
        TX_LOG_ARGS( "Loading ShowInspireMe section is successful." );
    }
    else {
        TX_LOG_ARGS( "Loading ShowInspireMe section fails." );
    }

    TX_EXIT
}

/*!
 Destructs the details view.
 */
MpDetailsView::~MpDetailsView()
{
    TX_ENTRY
    saveGroupBoxStates();
    delete mSoftKeyBack;
    delete mDocumentLoader;
    delete mMpQueryManager;

    TX_EXIT
}


/*!
 Initializes the details view. Allocates all resources needed by the view.
 */
void MpDetailsView::initializeView()
{
    TX_ENTRY

    mSoftKeyBack = new HbAction( Hb::BackNaviAction, this );

    mMpEngine = MpEngineFactory::sharedEngine();
    mSongData = mMpEngine->songData();
    mMpQueryManager = new MpQueryManager();

    connect( mSoftKeyBack, SIGNAL( triggered() ), this, SLOT( back() ) );
    connect( mSongData, SIGNAL( albumArtReady() ), this, SLOT( albumArtChanged() ) );
    connect( mSongData, SIGNAL( playbackInfoChanged() ), this, SLOT( handlePlaybackInfoChanged() ) );
    connect( mSongData, SIGNAL( songDetailInfoChanged() ), this, SLOT( songDetailInfoChanged() ) );
    connect( mDetailList, SIGNAL( pressed( HbListWidgetItem  * ) ), this, SLOT( handleListItemSelected( HbListWidgetItem  * ) ) );
    connect( mSongDetailsGroupBox, SIGNAL( toggled( bool ) ), this, SLOT( handleDetailsGroupBoxToggled( bool ) ) );
    connect( mInspireMeGroupBox, SIGNAL( toggled( bool ) ), this, SLOT( handleInspireMeGroupBoxToggled( bool ) ) );
    connect( mMpQueryManager, SIGNAL(inspireMeItemsMetadataRetrieved()), this, SLOT(renderInspireMeMetadata()));
    connect( mMpQueryManager, SIGNAL(inspireMeItemAlbumArtReady()), this, SLOT(renderInspireMeAlbumArts()));    
    connect( mMpQueryManager, SIGNAL(localMusicStoreRetrieved(bool)), this, SLOT(queryInspireMe(bool)), Qt::QueuedConnection);
	connect( mMpQueryManager, SIGNAL(localMusicStoreRetrievalError()), this, SLOT(abortInspireMeProcess()));
    connect( mMpQueryManager, SIGNAL(inspireMeItemsRetrievalError()), this, SLOT(queryLocalMusicStore()), Qt::QueuedConnection);

    TX_EXIT
}

void MpDetailsView::queryLocalMusicStore()
{
    TX_ENTRY
    mMpQueryManager->reset();
    mMpQueryManager->queryLocalMusicStore(); 
    TX_EXIT        
}

/*!
 Activates the details view.
 */
void MpDetailsView::activateView()
{
    TX_ENTRY
    setNavigationAction( mSoftKeyBack );

    mActivated = true;
    TX_LOG_ARGS( "InspireMeVal: " << MpSettingsManager::inspireMe() );
    TX_LOG_ARGS( "SongDetailsGbVal: " << MpSettingsManager::songDetailsGb() );
    mInspireMeGroupBox->setCollapsed(!MpSettingsManager::inspireMe());
    mSongDetailsGroupBox->setCollapsed(!MpSettingsManager::songDetailsGb());
    TX_EXIT
}

/*!
 Deactivates the details view.
 */
void MpDetailsView::deactivateView()
{
    TX_ENTRY
    saveGroupBoxStates();
    setNavigationAction( 0 );
    mActivated = false;
    TX_EXIT
}

void MpDetailsView::saveGroupBoxStates()
{
    TX_ENTRY
    if (mInspireMeGroupBox->isCollapsed() ) {
        MpSettingsManager::setInspireMe(false);
    }
    else {
        MpSettingsManager::setInspireMe(true);
    }

    if (mSongDetailsGroupBox->isCollapsed() ) {
        MpSettingsManager::setSongDetailsGb(false);
    }
    else {
        MpSettingsManager::setSongDetailsGb(true);
    }
    TX_EXIT      
}
 
/*!
 Slot to handle back command from softkey.
 */
void MpDetailsView::back()
{
    TX_ENTRY
    emit command( MpCommon::ActivatePreviousView );
    TX_EXIT
}

/*!
 Slot to handle Album art changed.
 */
void MpDetailsView::albumArtChanged( )
{
    TX_ENTRY
    HbIcon icon;
    mSongData->albumArt( icon );
    mAlbumArt->setIcon( icon );
    TX_EXIT
}

/*!
 Slot to handle network error.
 */
void MpDetailsView::abortInspireMeProcess( )
{
    TX_ENTRY
    mInspireMeProgressBar->hide();    
    mInspireMeGroupBox->setCollapsed( true );    
    mInspireMeQueryOngoing = false;
    mInspireMeQueryRendered = false;
    // ensure that we dont get callbacks from previous queries. Especially true,
    // if details view is actived with a quick song change again
    mMpQueryManager->reset();
    mInspireList->clear();
    TX_EXIT
}

/*!
 Render inspireme groupbox after album arts downloaded
 */
void MpDetailsView::renderInspireMeMetadata()
{
    TX_ENTRY
    mInspireMeQueryOngoing = false;
    mInspireMeQueryRendered = true;
    mInspireMeProgressBar->hide();
    if( mMpQueryManager->recommendationsCount() ) {
        TX_LOG_ARGS( "There are recommendations." )
        // we have recommendations
        for ( int i = 0; i < mMpQueryManager->recommendationsCount(); ++i ) {
            // configure the layout properties
            if(!mInspireList->count()) {
                // we havent configured the prototype before
                HbListViewItem *prototype = mInspireList->listItemPrototype();
                prototype->setGraphicsSize( HbListViewItem::LargeIcon );
            }
            // create the item
            HbListWidgetItem  *inspireMeItem = new HbListWidgetItem();
            inspireMeItem->setIcon( mMpQueryManager->recommendedAlbumArt( i ));
            inspireMeItem->setText( mMpQueryManager->recommendedSong( i ) );
            inspireMeItem->setSecondaryText( mMpQueryManager->recommendedArtist( i ) );
            mInspireList->addItem( inspireMeItem );
        }
    }
    else {
        TX_LOG_ARGS( "There are NO recommendations" )
        // we dont have recommendations
        // we havent configured the prototype before
        HbListViewItem *prototype = mInspireList->listItemPrototype();
        const int maxTextRowCount = 20;
        const int minTextRowCount = 1;
        prototype->setSecondaryTextRowCount(minTextRowCount,maxTextRowCount);

        HbListWidgetItem  *inspireMeItem = new HbListWidgetItem();
        inspireMeItem->setSecondaryText( hbTrId( "txt_mus_dblist_val_there_are_no_recommendations" ) );
        mInspireList->addItem( inspireMeItem );
    }

    TX_EXIT
}

void MpDetailsView::renderInspireMeAlbumArts()
{
    TX_ENTRY    
    for( int i = 0; i < mInspireList->count(); ++i) {
        mInspireList->item(i)->setIcon( mMpQueryManager->recommendedAlbumArt( i ) );
    }
    TX_EXIT    
}
   
bool MpDetailsView::isMetadata() const
{
    bool result = ( ( !mSongData->album().isEmpty() ) ||
                    ( !mSongData->artist().isEmpty() ) );
    TX_LOG_ARGS( "Inspire Me Query metadata available:" << result );
    return result;
}

/*!
 Slot to handle basic song information
 */
void MpDetailsView::handlePlaybackInfoChanged()
{
    TX_ENTRY
    // ensure that we dont get callbacks from previous queries. Especially true,
    // if details view is actived with a quick song change again
    mMpQueryManager->reset();
    mInspireList->clear();
    mInspireMeQueryRendered = false;

    // Clear the song data link until new query has been made.
    mSongData->setLink( "" );

    if ( !mSongData->title().isEmpty () ) {
        mSongText->setPlainText( mSongData->title() );
    }
    else {
        mSongText->setPlainText( mSongData->fileName() );
    }

    if ( !mSongData->album().isEmpty () ) {
        mAlbumText->setPlainText( mSongData->album() );
    }
    else {
        mAlbumText->setPlainText( hbTrId( "txt_mus_other_unknown7") );
    }

    if ( !mSongData->artist().isEmpty() ) {
        mArtistText->setPlainText( mSongData->artist() );
    }
    else {
        mArtistText->setPlainText( hbTrId( "txt_mus_other_unknown6") );
    }

    if(!mInspireMeGroupBox->isCollapsed()) {
        startInspireMe();
    }
    TX_EXIT
}

void MpDetailsView::startInspireMe()
{
    TX_ENTRY    
    if(isMetadata()) {
        // show progress bar, start store query or inspire me query process
        mInspireMeProgressBar->show();
        mInspireMeQueryOngoing = true;
        if( mMpQueryManager->isLocalMusicStore() ) {
            mMpQueryManager->queryInspireMeItems(mSongData->artist(),mSongData->album(),mSongData->title());
        }
        else {
            // no local store information present
            mMpQueryManager->queryLocalMusicStore();
        }
    }
    else {
        // no metadata. show no recommendations
        renderInspireMeMetadata();
    }
    TX_EXIT    
}
    
/*!
 Slot to handle detail song information
 */
void MpDetailsView::songDetailInfoChanged()
{
    TX_ENTRY
    mDetailList->clear();

    if ( !mSongData->albumTrack().isNull() ) {
        HbListWidgetItem *item = new HbListWidgetItem();
        item->setText( hbTrId( "txt_mus_dblist_song_number" ) );
        item->setSecondaryText( mSongData->albumTrack() );
        item->setEnabled( false );
        mDetailList->addItem( item );
    }

    if ( !mSongData->year().isNull() ) {
        HbListWidgetItem *item = new HbListWidgetItem();
        item->setText( hbTrId( "txt_mus_dblist_year" ) );
        item->setSecondaryText( mSongData->year() );
        item->setEnabled( false );
        mDetailList->addItem( item );
    }

    if ( !mSongData->genre().isNull() ) {
        HbListWidgetItem *item = new HbListWidgetItem();
        item->setText( hbTrId( "txt_mus_dblist_genre" ) );
        item->setSecondaryText( mSongData->genre() );
        item->setEnabled( false );
        mDetailList->addItem( item );
    }

    if ( !mSongData->composer().isNull() ) {
        HbListWidgetItem *item = new HbListWidgetItem();
        item->setText( hbTrId( "txt_mus_dblist_composer" ) );
        item->setSecondaryText( mSongData->composer() );
        item->setEnabled( false );
        mDetailList->addItem( item );
    }

    if ( !mSongData->fileName().isNull() ) {
        HbListWidgetItem *item = new HbListWidgetItem();
        item->setText( hbTrId( "txt_mus_dblist_file_name" ) );
        item->setSecondaryText( mSongData->fileName() );
        item->setEnabled( false );
        mDetailList->addItem( item );
    }

    if ( !mSongData->mimeType().isNull() ) {
        HbListWidgetItem *item = new HbListWidgetItem();
        item->setText( hbTrId( "txt_mus_dblist_format" ) );
        item->setSecondaryText( mSongData->mimeType() );
        item->setEnabled( false );
        mDetailList->addItem( item );
    }

    if ( !mSongData->duration().isNull() ) {
        HbListWidgetItem *item = new HbListWidgetItem();
        item->setText( hbTrId( "txt_mus_dblist_duration" ) );
        item->setSecondaryText( mSongData->duration() );
        item->setEnabled( false );
        mDetailList->addItem( item );
    }

    if ( !mSongData->bitRate().isNull() ) {
        HbListWidgetItem *item = new HbListWidgetItem();
        item->setText( hbTrId( "txt_mus_dblist_bitrate" ) );
        bool ok = false;
        int bitRate = mSongData->bitRate().toInt(&ok);
        TX_LOG_ARGS("Warning: If zero, bitrate is not read correctly. It wont show up in details" << ok);
        if(ok) {
            item->setSecondaryText( hbTrId("txt_mus_dblist_bitrate_val_ln_kbps", bitRate) );
        }
        item->setEnabled( false );
        mDetailList->addItem( item );
    }

    if ( !mSongData->sampleRate().isNull() ) {
        HbListWidgetItem *item = new HbListWidgetItem();
        item->setText( hbTrId( "txt_mus_dblist_sampling_rate" ) );
        bool ok = false;
        int samplingRate = mSongData->sampleRate().toInt(&ok);
        TX_LOG_ARGS("Warning: If zero, sampling rate is not read correctly. It wont show up in details" << ok);
        if(ok) {
            item->setSecondaryText(	hbTrId("txt_mus_dblist_sampling_rate_val_ln_hz", samplingRate) );
        }
        item->setEnabled( false );
        mDetailList->addItem( item );
    }

    if ( !mSongData->size().isNull() ) {
        HbListWidgetItem *item = new HbListWidgetItem();
        item->setText( hbTrId( "txt_mus_dblist_size" ) );
        bool ok = false;
        long size = mSongData->size().toLong(&ok); // in bytes
        TX_LOG_ARGS("WARNING: If zero, Cant get song data size right. You wont see the size in details groupbox correctly" << ok );
        if( size < KOneKiloByteInBytes) {
            // under 1 KB
            item->setSecondaryText( hbTrId("txt_mus_dblist_size_val_ln_b", size) );
        }
        else if( size < KOneMegabyteInBytes ) {
            // under 1 MB
            size /= KOneKiloByteInBytes; // turn size into KB
            item->setSecondaryText( hbTrId("txt_mus_dblist_size_val_ln_kb", size) );
        }
        else if( size < KOneGigaByteInBytes ) {
            // under 1 GB
            size /= KOneMegabyteInBytes; // turn size to MB
            item->setSecondaryText( hbTrId("txt_mus_dblist_size_val_ln_mb", size) );
        }
        else {
            // 1 GB or higher
            size /= KOneGigaByteInBytes; // turn size to GB
            item->setSecondaryText( hbTrId("txt_mus_dblist_size_val_ln_gb", size) );            
        }
        item->setEnabled( false );
        mDetailList->addItem( item );
    }

    if ( !mSongData->modified().isNull() ) {
        HbListWidgetItem *item = new HbListWidgetItem();
        item->setText( hbTrId( "txt_mus_dblist_modified" ) );
        item->setSecondaryText( mSongData->modified() );
        item->setEnabled( false );
        mDetailList->addItem( item );
    }

    if ( !mSongData->copyright().isNull() ) {
        HbListWidgetItem *item = new HbListWidgetItem();
        item->setText( hbTrId( "txt_mus_dblist_copyright" ) );
        item->setSecondaryText( mSongData->copyright() );
        item->setEnabled( false );
        mDetailList->addItem( item );
    }

    if ( !mSongData->musicURL().isNull() ) {
        HbListWidgetItem *item = new HbListWidgetItem();
        item->setText( hbTrId( "txt_mus_dblist_web_site" ) );
        item->setSecondaryText( mSongData->musicURL() );
        item->setEnabled( true );
        mDetailList->addItem( item );
    }

    if ( mSongData->isDrmProtected() ) {
        HbListWidgetItem *item = new HbListWidgetItem();
        item->setText( hbTrId( "txt_mus_dblist_licences" ) );
        item->setSecondaryText( hbTrId( "txt_mus_dblist_licences_val_click_for_details" ) );
        item->setEnabled( true );
        mDetailList->addItem( item );
    }

    TX_EXIT
}

void MpDetailsView::handleDetailsGroupBoxToggled(bool /*state*/)
{
    TX_ENTRY
    if ( !mSongDetailsGroupBox->isCollapsed() ) {
            mInspireMeGroupBox->setCollapsed( true );
    }
    TX_EXIT
}

void MpDetailsView::handleInspireMeGroupBoxToggled(bool /*state*/)
{
    TX_ENTRY
    if ( !mInspireMeGroupBox->isCollapsed() ) {
        TX_LOG_ARGS( "InspireMe is expanded." )
        mSongDetailsGroupBox->setCollapsed( true );
        if ( mInspireMeQueryOngoing ) {
            TX_LOG_ARGS( "Query is ongoing " )
            mInspireMeProgressBar->show();
        }
        else {
            TX_LOG_ARGS( "Query is NOT ongoing " )
            if ( mInspireMeQueryRendered ) {
                TX_LOG_ARGS( "InspireMe is rendered already. " )
                mInspireMeProgressBar->hide();
            }
            else {
                startInspireMe();
            }
        }
    }
    else {
        TX_LOG_ARGS( "InspireMe is collapsed." )
        mInspireMeProgressBar->hide();
    }
    TX_EXIT
}

/*!
 Slot to call when URL item in song details is pressed
*/
void MpDetailsView::handleListItemSelected( HbListWidgetItem  *item)
{
    TX_ENTRY_ARGS( "URL: " << item->secondaryText() );
    QDesktopServices::openUrl( item->secondaryText() );
    TX_EXIT
}

/*!
 Slot to call when MusicStore Info is Received
*/
void MpDetailsView::queryInspireMe(bool storeUpdated)
{
    TX_ENTRY
    // we asked query manager to update its local store information
    // if that store info didnt change (i.e. from empty string to "fi" or
    // from "bad old store" to "good new store") then we dont need to pursue a query    
    if( storeUpdated ) {
        mMpQueryManager->queryInspireMeItems(mSongData->artist(),mSongData->album(),mSongData->title());
        mInspireMeQueryOngoing = true;
    }
    else {
        mInspireMeQueryOngoing = false;
        mInspireMeProgressBar->hide();
    }
    TX_EXIT
}

