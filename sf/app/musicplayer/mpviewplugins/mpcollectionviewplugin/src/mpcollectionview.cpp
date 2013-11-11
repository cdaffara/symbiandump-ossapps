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
* Description: Music Player collection view.
*
*/

// INCLUDE FILES
#include <time.h>
#include <cstdlib>
#include <QtCore>

#include <hbinstance.h>
#include <hbapplication.h>
#include <hbaction.h>
#include <hbtoolbar.h>
#include <hbmenu.h>
#include <hbmessagebox.h>
#include <hblabel.h>
#include <hblistview.h>
#include <hbscrollbar.h>
#include <hbstyleloader.h>

#include "mpcollectionview.h"
#include "mpcollectiondocumentloader.h"
#include "mpcollectioncontainerfactory.h"
#include "mpcollectioncontainer.h"
#include "mpcollectiondatamodel.h"
#include "mpenginefactory.h"
#include "mpmpxcollectiondata.h"
#include "mpnowplayingwidget.h"
#include "mpcommondefs.h"
#include "mptrace.h"
#include "mpsnapshotwidget.h"
#include "mpsettingsmanager.h"
#include "mpcollectionlistcontainer.h"
#include "mpcollectionpopuphandler.h"


const char*MUSIC_COLLECTION_DOCML = ":/docml/musiccollection.docml";

const char*CONTAINER_EFFECT_GROUP = "mpcontainer";

const char*SHOW_EFFECT = "show";
const char*HIDE_EFFECT = "hide";
const char*SHOW_BACK_EFFECT = "show_back";
const char*HIDE_BACK_EFFECT = "hide_back";

const char*SHOW_EFFECT_RESOURCE_NAME = "view_show_normal";
const char*HIDE_EFFECT_RESOURCE_NAME = "view_hide_normal";
const char*SHOW_BACK_EFFECT_RESOURCE_NAME = "view_show_back";
const char*HIDE_BACK_EFFECT_RESOURCE_NAME = "view_hide_back";

const char*EFFECT_TARGET_SNAPSHOT = "snapshot";
const char*EFFECT_TARGET_CONTAINER = "container";

const int KMainToolBarAll = 0;
const int KMainToolBarArtists = 1;
const int KMainToolBarAlbums = 2;
const int KMainToolBarPlaylists = 3;

const int KPlaylistToolBarAdd = 0;
const int KPlaylistToolBarRemove = 1;
const int KplaylistToolBarReorder = 2;

/*!
    \class MpCollectionView
    \brief Music Player collection view.

    Collection view provides access to music collection in the device.
    This class owns the menu and the toolbar. In addition, it is
    responsible for creating UI containers and the underlying engine
    components and connecting them to work together.
*/

/*!
    \fn void command( int command )

    This signal is emitted when the view issues a \a command to the
    application such as request to switch to a different view.
 */

/*!
 Constructs the collection view.
 */
MpCollectionView::MpCollectionView()
    : mCollectionContext( ECollectionContextUnknown ),
      mMpEngine( 0 ),
      mContainerFactory( 0 ),
      mCollectionContainer( 0 ),
      mCollectionDataModel( 0 ),
      mActivated( false ),
      mNowPlayingBanner( 0 ),
      mBannerAttached( false ),
      mSoftKeyQuit( 0 ),
      mSoftKeyBack( 0 ),
      mShuffleAction( 0 ),
      mShuffleEnabled( false ),
      mDocumentLoader( 0 ),
      mMainContainer( 0 ),
      mMainToolBar( 0 ),
      mPlaylistToolBar( 0 ),
      mSnapshot( 0 ),
      mActivationWaiting( false ),
      mMpPopupHandler( 0 ),
      mUsbBlocked( false )
{
    TX_LOG
}

/*!
 Destructs the collection view.
 */
MpCollectionView::~MpCollectionView()
{
    TX_ENTRY
    delete mSnapshot;
    delete mSoftKeyQuit;
    delete mSoftKeyBack;

    HbToolBar *toolBar = takeToolBar();
    if ( mMainToolBar != toolBar && mPlaylistToolBar != toolBar ) {
        delete toolBar;
    }
    if ( mMainToolBar ) {
        mMainToolBar->deleteLater();
    }
    if ( mPlaylistToolBar ) {
        mPlaylistToolBar->deleteLater();
    }

    delete mCollectionContainer;
    delete mContainerFactory;
    delete mCollectionDataModel;
    delete mDocumentLoader;
    TX_EXIT
}

/*!
 Initializes the collection view. Allocates all resources needed by the view.
 */
void MpCollectionView::initializeView()
{
    TX_ENTRY

    mWindow = mainWindow();

    // Create softkey actions
    mSoftKeyQuit = new HbAction( Hb::QuitNaviAction, this );
    connect( mSoftKeyQuit, SIGNAL( triggered() ), 
	         this, SLOT( back() ) );

    mSoftKeyBack = new HbAction( Hb::BackNaviAction, this );
    connect( mSoftKeyBack, SIGNAL( triggered() ), 
	         this, SLOT( back() ) );

    mMpEngine = MpEngineFactory::sharedEngine();

    connect( mMpEngine, SIGNAL( collectionPlaylistOpened() ),
            this, SLOT( startPlaybackView() ) );
    connect( mMpEngine, SIGNAL( playlistSaved( bool ) ),
            this, SLOT( playlistSaved( bool ) ) );
    connect( mMpEngine, SIGNAL( songsDeleted( bool ) ),
            this, SLOT( songsDeleted( bool ) ) );
    connect( mMpEngine, SIGNAL( playlistsRenamed( bool ) ),
            this, SLOT( playlistsRenamed( bool ) ) );
    connect( mMpEngine, SIGNAL( isolatedCollectionOpened( MpMpxCollectionData* ) ),
            this, SLOT( handleIsolatedCollectionOpened( MpMpxCollectionData* ) ) );

    mCollectionData = mMpEngine->collectionData();
    qRegisterMetaType<TCollectionContext>("TCollectionContext");
    connect( mCollectionData, SIGNAL( contextChanged( TCollectionContext ) ), 
             this, SLOT( setContext( TCollectionContext ) ), 
			 Qt::QueuedConnection );
    mCollectionDataModel = new MpCollectionDataModel( mCollectionData , mMpEngine->playbackData());
    
    connect( mCollectionDataModel, SIGNAL( dataReloaded() ),
             this, SLOT( containerDataChanged() ) );

    mDocumentLoader = new MpCollectionDocumentLoader();
    bool ok = false;
    mDocumentLoader->load( MUSIC_COLLECTION_DOCML, &ok );
    if ( ok ) {
        QGraphicsWidget *widget;

        widget = mDocumentLoader->findWidget( QString( "nowPlaying" ) );
        mNowPlayingBanner = qobject_cast<MpNowPlayingWidget*>( widget );

        if ( mViewMode == MpCommon::FetchView ) {
            // Banner is not needed since playback is stopped when returning
            // from playback preview. Disable the banner from updating.
            mNowPlayingBanner->setEnabled( false );
            attachNowPlayingBanner( false );
        }
        else {
            connect( mNowPlayingBanner, SIGNAL( clicked() ), 
				     this, SLOT( startPlaybackView() ) );
            connect( mNowPlayingBanner, SIGNAL( playbackAttachmentChanged( bool ) ),
                     this, SLOT( attachNowPlayingBanner( bool ) ) );
            attachNowPlayingBanner( mNowPlayingBanner->isBannerAttached() );
        }

        widget = mDocumentLoader->findWidget( QString( "mainContainer" ) );
        mMainContainer = qobject_cast<HbWidget*>( widget );

        setWidget( mMainContainer );

        HbEffect::add(
            QStringList() << CONTAINER_EFFECT_GROUP << CONTAINER_EFFECT_GROUP << CONTAINER_EFFECT_GROUP << CONTAINER_EFFECT_GROUP,
            QStringList() << SHOW_EFFECT_RESOURCE_NAME <<  HIDE_EFFECT_RESOURCE_NAME << SHOW_BACK_EFFECT_RESOURCE_NAME << HIDE_BACK_EFFECT_RESOURCE_NAME,
            QStringList() << SHOW_EFFECT << HIDE_EFFECT << SHOW_BACK_EFFECT << HIDE_BACK_EFFECT);


    }
    else {
        TX_LOG_ARGS( "Error: invalid xml file." );
        Q_ASSERT_X( ok, "MpCollectionView::initializeView", "invalid xml file" );
    }

    // Load custom tbone css here so we do it only once.
    HbStyleLoader::registerFilePath(":/css/tbonemediawall.css");
    HbStyleLoader::registerFilePath(":/css/tbonemediawall_color.css");
    HbStyleLoader::registerFilePath(":/css/tbonemediawall.hgmediawall.widgetml");
        
    mContainerFactory = new MpCollectionContainerFactory( this, mDocumentLoader );
    
    mMpPopupHandler = new MpCollectionPopupHandler( this );

    connect( mMpEngine, SIGNAL( usbBlocked(bool) ),
             this, SLOT( handleUsbBlocked(bool) ) );
    connect( mMpEngine, SIGNAL( libraryAboutToUpdate() ),
             this, SLOT( handleLibraryAboutToUpdate() ) );
    connect( mMpEngine, SIGNAL( libraryUpdated() ),
             this, SLOT( handleLibraryUpdated() ) );
    mUsbBlocked = mMpEngine->verifyUsbBlocking();

    if ( MpSettingsManager::firstStartup() ) {
        mActivationWaiting = true;
        mMpEngine->refreshLibrary( true );
    }

    TX_EXIT
}

/*!
 Activates the collection view.
 */
void MpCollectionView::activateView()
{
    if ( mActivationWaiting ) {
        return;
    }
    mActivated = true;
    TX_ENTRY_ARGS( "mCollectionContext=" << mCollectionContext );
    if ( mCollectionContext == ECollectionContextUnknown ) {
        // Open 'All Songs' by default
        if ( mCollectionData->context() == ECollectionContextAllSongs ){
            setContext( ECollectionContextAllSongs );
        }
        else {
            mMpEngine->openCollection( ECollectionContextAllSongs );
        }
    }
    else {
        // This true when returning from other views, e.g. playback view
        setSoftkey();
    }
    TX_EXIT
}

/*!
 Deactivates the collection view.
 */
void MpCollectionView::deactivateView()
{
    TX_ENTRY
    mActivated = false;

    closeActiveDialog( true );

    setNavigationAction( 0 );
    TX_EXIT
}

/*!
 Returns view activation status.
 */
bool MpCollectionView::isActivated()
{
    TX_LOG_ARGS( "View activated = " << mActivated );
    return mActivated;
}

/*!
 Opens or plays an item through collection container in order to keep track of the Artist / Album data used by infobar.
 */
void MpCollectionView::openItem( const QModelIndex &index )
{
    TX_ENTRY_ARGS( "index = " << index.row() );
    qobject_cast<MpCollectionListContainer*>(mCollectionContainer)->itemActivated( index );
    TX_EXIT
}

/*!
 Shows the detailed metadata information for the song with \a index.
 It activates details view.
 */
void MpCollectionView::showItemDetails( int index )
{
    TX_ENTRY_ARGS( "index = " << index );
    mMpEngine->retrieveSongDetails( index );
    emit command( MpCommon::ActivateDetailsView );
    TX_EXIT
}

/*!
 Slot to be called when collection context is changed as a result of Open
 operation.
 */
void MpCollectionView::setContext( TCollectionContext context )
{
    TX_ENTRY_ARGS( "context=" << context );
    if ( mActivated ) {
        startContainerTransition( mCollectionContext, context );
    }

    mCollectionContext = context;
    // Reset softkey and the menu
    if ( mActivated ) {
        setSoftkey();
    }

    // Close any possible popup already launched with previous context
    closeActiveDialog();

    updateToolBar();
    updateMenu();

    mCollectionContainer = mContainerFactory->createContainer( context );
    mCollectionContainer->setViewMode( mViewMode );
    mCollectionDataModel->refreshModel();
    mCollectionContainer->setDataModel( mCollectionDataModel );

    TX_EXIT
}

/*!
 Slot to be called when 'All Songs' action is triggered from the toolbar.
 */
void MpCollectionView::openSongs()
{
    TX_ENTRY
    if ( mCollectionContext != ECollectionContextAllSongs ) {
        mMpEngine->openCollection( ECollectionContextAllSongs );
    }
    TX_EXIT
}

/*!
 Slot to be called when 'Artists' action is triggered from the toolbar.
 */
void MpCollectionView::openArtists()
{
    TX_ENTRY
    if ( mCollectionContext != ECollectionContextArtists ) {
        mMpEngine->openCollection( ECollectionContextArtists );
    }
    TX_EXIT
}


/*!
 Slot to be called when 'Albums' action is triggered from the toolbar.
 */
void MpCollectionView::openAlbums()
{
    TX_ENTRY
    if ( mCollectionContext != ECollectionContextAlbums ) {
        mMpEngine->openCollection( ECollectionContextAlbums );
    }
    TX_EXIT
}

/*!
 Slot to be called when 'Playlists' action is triggered from the toolbar.
 */
void MpCollectionView::openPlaylists()
{
    TX_ENTRY
    if ( mCollectionContext != ECollectionContextPlaylists) {
        mMpEngine->openCollection( ECollectionContextPlaylists );
    }
    TX_EXIT
}

/*!
 Slot to be called when 'Find' action is triggered from the toolbar.
 */
void MpCollectionView::find()
{
    // Todo
    HbMessageBox *messageBox = new HbMessageBox( "Not ready!", HbMessageBox::MessageTypeInformation );
    messageBox->setAttribute( Qt::WA_DeleteOnClose );
    messageBox->show();
    updateToolBar();
}

/*!
 Slot to be called when 'Music Store' action is triggered from the toolbar.
 */
void MpCollectionView::openMusicStore()
{
    // Todo
    HbMessageBox *messageBox = new HbMessageBox( "Not ready!", HbMessageBox::MessageTypeInformation );
    messageBox->setAttribute( Qt::WA_DeleteOnClose );
    messageBox->show();
    updateToolBar();
}

/*!
 Slot to be called when an item is selected by the user.
 */
void MpCollectionView::openIndex( int index )
{
    TX_ENTRY_ARGS( "index=" << index );
    bool doOpen = true;
    if ( mViewMode == MpCommon::FetchView ) {
        QString songUri;
        switch ( mCollectionContext ) {
            case ECollectionContextAllSongs:
            case ECollectionContextArtistAllSongs:
            case ECollectionContextPlaylistSongs:
                doOpen = false;
                if (!mCollectionData->hasItemProperty(index, MpMpxCollectionData::Corrupted)) {
                    songUri = mCollectionData->itemData( index, MpMpxCollectionData::Uri );
                    emit songSelected( songUri );
                    }
                else {
                    showCorruptedNote();
                }
                break;
            case ECollectionContextArtistAlbumsTBone:
            case ECollectionContextAlbumsTBone:
                doOpen = false;
                if (!mCollectionData->hasAlbumSongProperty(index, MpMpxCollectionData::Corrupted)) {
                    songUri = mCollectionData->albumSongData( index, MpMpxCollectionData::Uri );
                    emit songSelected( songUri );
                    }
                else {
                    showCorruptedNote();
                }
                break;
            default:
                break;
        }
    }

    if ( doOpen ) {
        if ( mCollectionContext == ECollectionContextArtistAlbums ) {
            if ( (mCollectionData->count() > 1) && (index == 0) ) {
                mMpEngine->openCollectionItem( index );
            }
            else {
                // Artist-Album view going into T-bone. We don't actually open the
                // collection. Just fake an open.
                mCollectionData->setContext( ECollectionContextArtistAlbumsTBone );
            }
        }
        else if ( mCollectionContext == ECollectionContextAlbums ) {
            // Album view going into T-bone. We don't actually open the
            // collection. Just fake an open.
            mCollectionData->setContext( ECollectionContextAlbumsTBone );
        }
        else {
            switch ( mCollectionContext ) {
                case ECollectionContextAllSongs:
                case ECollectionContextArtistAllSongs:
                case ECollectionContextPlaylistSongs:
                    if (!mCollectionData->hasItemProperty(index, MpMpxCollectionData::Corrupted)) {
                        mMpEngine->openCollectionItem( index );
                        }
                    else {
                        showCorruptedNote();
                    }
                    break;
                case ECollectionContextArtists:
                case ECollectionContextPlaylists:
                    mMpEngine->openCollectionItem( index );
                default:
                    break;
            }
        }
    }
    TX_EXIT
}

/*!
 Slot to be called when an album is centered in T-Bone view and we need to
 find the songs beloging to album with \a index.
 */
void MpCollectionView::findAlbumSongs( int index )
{
    TX_ENTRY_ARGS( "index=" << index );
    mMpEngine->findAlbumSongs(index);
    TX_EXIT
}

/*!
 Slot to be called when a song is selected in T-Bone view and we need to
 play album with \a albumIndex starting with the songs with \a songIndex.
 */
void MpCollectionView::playAlbumSongs( int albumIndex, int songIndex )
{
    TX_ENTRY_ARGS( "albumIndex=" << albumIndex << "songIndex=" << songIndex );
    if (!mCollectionData->hasAlbumSongProperty(songIndex, MpMpxCollectionData::Corrupted)) {
        mMpEngine->playAlbumSongs(albumIndex, songIndex);
    }
    else {
        showCorruptedNote();
    }
    TX_EXIT
}

/*!
 Slot to be called when back/quit softkey is pressed.
 */
void MpCollectionView::back()
{
    TX_ENTRY_ARGS( "mCollectionContext=" << mCollectionContext );
    bool doExit( false );
    switch ( mCollectionContext ) {
        case ECollectionContextAllSongs:
        case ECollectionContextArtists: 
        case ECollectionContextAlbums:
        case ECollectionContextPlaylists:
            // Exit from these levels.
            doExit = true;
            break;
        case ECollectionContextArtistAlbums:
        case ECollectionContextArtistAllSongs:
        case ECollectionContextPlaylistSongs:
            mMpEngine->back();
            break;
        case ECollectionContextArtistAlbumsTBone:
            if ( mCollectionData->count() > 1 ) {
                // Going from T-Bone to Artist-Album view; Need to fake back.
                mCollectionData->setContext(ECollectionContextArtistAlbums);
            }
            else {
                // There only 1 album under this artist. Return to artist view.
                mMpEngine->back();
            }
            break;
        case ECollectionContextAlbumsTBone:
            // Going from T-Bone to Album view; Need to fake back.
            mCollectionData->setContext(ECollectionContextAlbums);
            break;
        default:
            doExit = true;
            break;
    }

    if ( doExit ) {
        if ( mViewMode == MpCommon::FetchView ) {
            // Send an empty string to indicate that user has cancelled
            // the fetch operation
            emit songSelected( "" );
        }
        else {
            emit command( MpCommon::SendToBackground );
        }
    }
    TX_EXIT
}

/*!
 Slot to be called to exit.
 */
void MpCollectionView::exit()
{
    TX_ENTRY
    emit command( MpCommon::Exit );
    TX_EXIT
}


/*!
 Slot to be called to activate playback view.
 */
void MpCollectionView::startPlaybackView()
{
    TX_LOG
    emit command( MpCommon::ActivatePlaybackView );
}

/*!
 Slot to be called when 'Now Playing Banner' state changes. If active, 'Now
 Playing Banner' is attached/shown in the collection view.
 */
void MpCollectionView::attachNowPlayingBanner( bool active )
{
    TX_ENTRY
    setBannerVisibility( active );
    mBannerAttached = active;
    updateMenu();
    TX_EXIT
}

/*!
 Slot for container transition end.
 */
void MpCollectionView::containerTransitionComplete( const HbEffect::EffectStatus &status )
{    
    if ( status.userData == EFFECT_TARGET_SNAPSHOT ) {
        qobject_cast<QGraphicsView *>( mWindow )->scene()->removeItem( mSnapshot );
        mSnapshot->deleteLater();
        mSnapshot = 0;
    }
}

/*!
 Slot to be called when 'Shuffle play all' is clicked by the user from the menu.
 */
void MpCollectionView::shufflePlayAll()
{
    TX_ENTRY
    mMpEngine->setShuffle( true );
    MpSettingsManager::setShuffle( true );
    int index = generateShuffleIndex();
    switch ( mCollectionContext ) {
        case ECollectionContextArtistAlbumsTBone:
        case ECollectionContextAlbumsTBone:
            playAlbumSongs( mCollectionData->currentAlbumIndex(), index );
            break;
        default:
            openIndex( index );
            break;
    }
    TX_EXIT
}

/*!
 Slot to be called when 'Add to playlist' is clicked by the user from the menu.
 */
void MpCollectionView::addToPlaylist()
{
    switch ( mCollectionContext ) {
        case ECollectionContextArtistAlbumsTBone:
        case ECollectionContextAlbumsTBone:
            mMpPopupHandler->openAddSongsToPlaylistFromTBone();    
            break;
        default:
            mMpPopupHandler->openAddSongsToPlaylist( mCollectionDataModel );
            break;
    }
}

/*!
 Slot to be called when 'New Playlist' is clicked by the user from the menu.
 */
void MpCollectionView::createNewPlaylist() 
{
    
    switch ( mCollectionContext ) {
        case ECollectionContextArtistAlbumsTBone:
        case ECollectionContextAlbumsTBone:
            mMpPopupHandler->openCreateNewPlaylistFromTBone();    
            break;
        default:
            //second parameter, means it is an not an isolated collection, 
            //ownership is  not transferred.
            mMpPopupHandler->openCreateNewPlaylist( mCollectionData, false );
            break;
    }
}

/*!
 Slot to be called when 'Delete' is clicked by the user from the menu.
 */
void MpCollectionView::deleteSongs()
{
    mMpPopupHandler->openDeleteSongs( mCollectionDataModel );
}

/*!
 Slot to be called when 'Rename Playlist' is clicked by the user from the menu.
 */
void MpCollectionView::renameCurrentPlaylistContainer()
{
    QString currentName;
    currentName = mCollectionData->collectionTitle();    
    mMpPopupHandler->openRenamePlaylistContainerDialog( currentName );
}


/*!
 Slot to be called when 'Add to playlist' operation has completed.
 */
void MpCollectionView::playlistSaved( bool success )
{
    if ( success
         && ( ECollectionContextPlaylists == mCollectionContext
              || ECollectionContextPlaylistSongs == mCollectionContext ) ) {
        mMpEngine->reopenCollection();
    }
}

/*!
 Slot to be called when 'Delete' operation has completed.
 */
void MpCollectionView::songsDeleted( bool success )
{
    // Update list if delete succeded or if delete interrupted by an USB MTP Event
    if ( success || mMpEngine->verifyUsbBlocking( true ) ) {
        mMpEngine->reopenCollection();
    }
}
/*!
 Slot to be called when 'Rename' operation has completed.
 */
void MpCollectionView::playlistsRenamed( bool success )
{
    if ( success ) {
        mMpEngine->reopenCollection();
    }
}

/*!
 Slot to be called to get ready for add to playlist using an isolated collection.
 */
void MpCollectionView::prepareToAddToPlaylist()
{
    TX_ENTRY
    mMpEngine->openIsolatedCollection( ECollectionContextAllSongs );
    TX_EXIT
}

/*!
 Slot to be called when isolated collection is oppened.
 */
void MpCollectionView::handleIsolatedCollectionOpened( MpMpxCollectionData* collectionData )
{
    TX_ENTRY
    if ( mActivated ) {

        if (ECollectionContextPlaylistSongs == mCollectionContext) {
            mMpPopupHandler->openAddToCurrentPlaylist( collectionData );
        }
        else if (ECollectionContextPlaylists == mCollectionContext) {
            //second parameter, means it is an isolated collection, ownership is transferred.
            mMpPopupHandler->openCreateNewPlaylist( collectionData, true );
        }
    }
    TX_EXIT
}

/*!
 Slot to be called to arrange songs in a playlist.
 */
void MpCollectionView::arrangeSongs( )
{
    mMpPopupHandler->openArrangeSongs();
}

/*!
 Slot to be called when an item is long pressed by the user.
 */
void MpCollectionView::openContextMenu( const QModelIndex &index, const QPointF &coords )
{
    TX_ENTRY_ARGS( "index=" << index.row() );
    switch ( mViewMode ) {
        case MpCommon::DefaultView:
            mMpPopupHandler->openDefaultViewContextMenu( index, coords );
            break;
        case MpCommon::FetchView:
            mMpPopupHandler->openFetchViewContextMenu( index, coords );
            break;
        default:
            break;
    }
    TX_EXIT
}

/*!
 Slot to be called when USB blocking status changes.
 */
void MpCollectionView::handleUsbBlocked( bool blocked )
{
    TX_ENTRY_ARGS( "blocked=" << blocked );
    mUsbBlocked = blocked;
        
    closeActiveDialog();
    
    updateMenu();
    if ( mCollectionContext == ECollectionContextPlaylistSongs ) {
         updateToolBar();
    }
    TX_EXIT
}

/*!
 Slot to be called when library is going to be updated.
 */
void MpCollectionView::handleLibraryAboutToUpdate()
{
    TX_ENTRY
    closeActiveDialog();
    TX_EXIT
}

/*!
 Slot to be called when refreshing completes or library has been updated.
 */
void MpCollectionView::handleLibraryUpdated()
{
    TX_ENTRY
    if ( mActivationWaiting ) {
        mActivationWaiting = false;
        activateView();
    }
    else {
        closeActiveDialog();

        // Update cache, even if collection is in background.
        // Library refreshing could be triggered at any point due USB/MMC events.
        if ( mCollectionContext == ECollectionContextAllSongs ) {
            mMpEngine->reopenCollection();
        }
		// We force to go to the all songs collection when the refresh is finished.
        else{
            openSongs();
        }
    }
    TX_EXIT
}

/*!
 Slot to be called when shuffle action status changes. This is called by the
 containers. Shuffle is only enabled when there is more than 1 song and the
 it can change when songs are deleted or when new songs are found during refresh.
 */
void MpCollectionView::setShuffleAction( bool enabled )
{
    TX_ENTRY
    mShuffleEnabled = enabled;
    if ( mShuffleAction ) {
        mShuffleAction->setEnabled(enabled);
    }
    TX_EXIT
}

/*!
 Slot to be called when a container data is changed/updated.
 */
void MpCollectionView::containerDataChanged()
{
    TX_ENTRY
    if ( mCollectionContext == ECollectionContextPlaylistSongs ) {
         updateToolBar();
    }
    TX_EXIT
}

/*!
 \internal
 Sets the main ( default ) toolbar for the view.
 */
void MpCollectionView::setMainToolBar()
{
    TX_ENTRY
    if ( !mMainToolBar ) {
        //Create the toolbar.
        mMainToolBar = new HbToolBar();
        mMainToolBar->setObjectName( "MainToolBar" );
        mMainToolBar->setOrientation( Qt::Horizontal );
        QActionGroup *actionsGroup = new QActionGroup( mMainToolBar );
        HbAction *action;

        // All Songs
        action = createToolBarAction( actionsGroup, "qtg_mono_songs_all", "AllSongsAction" );
        connect( action, SIGNAL( triggered( bool ) ), this, SLOT( openSongs() ) );
        mMainToolBar->addAction( action );
        
        // Artists
        action = createToolBarAction( actionsGroup, "qtg_mono_artists", "ArtistsAction" );
        connect( action, SIGNAL( triggered( bool ) ), this, SLOT( openArtists() ) );
        mMainToolBar->addAction( action );

        // Albums
        action = createToolBarAction( actionsGroup, "qtg_mono_music_albums", "AlbumsAction" );
        connect( action, SIGNAL( triggered( bool ) ), this, SLOT( openAlbums() ) );
        mMainToolBar->addAction( action );

        // Playlists
        action = createToolBarAction( actionsGroup, "qtg_mono_playlist", "PlaylistsAction" );
        connect( action, SIGNAL( triggered( bool ) ), this, SLOT( openPlaylists() ) );
        mMainToolBar->addAction( action );
        
        if ( mViewMode != MpCommon::FetchView ) {
            // Music Store
            action = createToolBarAction(actionsGroup, "qtg_mono_ovistore", "MusicStoreAction" );
            connect( action, SIGNAL( triggered( bool ) ), this, SLOT( openMusicStore() ) );
            mMainToolBar->addAction( action );
        }
    }
    HbAction* action = 0;
    switch ( mCollectionContext ) {
        case ECollectionContextAllSongs:
            action = qobject_cast<HbAction*>( mMainToolBar->actions()[KMainToolBarAll] );
            break;
        case ECollectionContextArtists:
            action = qobject_cast<HbAction*>( mMainToolBar->actions()[KMainToolBarArtists] );
            break;
        case ECollectionContextAlbums:
            action = qobject_cast<HbAction*>( mMainToolBar->actions()[KMainToolBarAlbums] );
            break;
        case ECollectionContextPlaylists:
            action = qobject_cast<HbAction*>( mMainToolBar->actions()[KMainToolBarPlaylists] );
            break;
    }
    if ( action ) {
        action->setChecked( true );
    }
    if ( toolBar() != mMainToolBar ) {
        HbToolBar *tmpToolBar = takeToolBar();
        if ( tmpToolBar && tmpToolBar->actions().empty() ) {
            tmpToolBar->deleteLater();
        }
        setToolBar( mMainToolBar );
    }
    TX_EXIT
}

/*!
 \internal
 Sets the playlist toolbar for the view.
 Used only on the playlist container.
 */
void MpCollectionView::setPlaylistToolBar()
{
    TX_ENTRY
    if ( !mPlaylistToolBar ) {
        mPlaylistToolBar = new HbToolBar();
        mPlaylistToolBar->setObjectName( "PlaylistToolBar" );
        mPlaylistToolBar->setOrientation( Qt::Horizontal );
        HbAction *action;

        action = new HbAction( this );
        action->setIcon( HbIcon( "qtg_mono_plus" ) );
        connect( action, SIGNAL( triggered( bool ) ), this, SLOT( prepareToAddToPlaylist() ) );
        action->setObjectName( "AddToPlaylistAction" );
        mPlaylistToolBar->addAction( action );

        action = new HbAction( this );
        action->setIcon( HbIcon( "qtg_mono_minus" ) );
        connect( action, SIGNAL( triggered( bool ) ), this, SLOT( deleteSongs() ) );
        action->setObjectName( "RemoveFromPlaylistAction" );
        mPlaylistToolBar->addAction( action );

        action = new HbAction( this );
        action->setIcon( HbIcon( "qtg_mono_organize" ) );
        connect( action, SIGNAL( triggered( bool ) ), this, SLOT( arrangeSongs() ) );
        action->setObjectName( "OrganizePlaylistAction" );
        mPlaylistToolBar->addAction( action );
    }

    int items = mCollectionData->count();

    //no use for add if usb blocked.
    mPlaylistToolBar->actions()[KPlaylistToolBarAdd]->setEnabled( !mUsbBlocked );

    //no use for remove if there are no items or usb blocked.
    mPlaylistToolBar->actions()[KPlaylistToolBarRemove]->setEnabled( items > 0 && !mUsbBlocked );

    //no use for reorder if there is no more than 1 item or usb blocked.
    mPlaylistToolBar->actions()[KplaylistToolBarReorder]->setEnabled( items > 1 && !mUsbBlocked );

    if ( toolBar() != mPlaylistToolBar ) {
        HbToolBar *tmpToolBar = takeToolBar();
        if ( tmpToolBar && tmpToolBar->actions().empty () ) {
            tmpToolBar->deleteLater();
        }
        setToolBar( mPlaylistToolBar );
    }
    TX_EXIT
}

/*!
 \internal
 Creates action associated with the action group for the toolbar.
 */
HbAction *MpCollectionView::createToolBarAction(
    QActionGroup *actionsGroup,
    const QString& icon,
    const QString& objectName )
{
    HbIcon actionIcon( icon );

    HbAction *action = new HbAction( actionsGroup );
    action->setIcon( actionIcon );
    action->setCheckable( true );
    action->setObjectName( objectName );
    return action;
}

/*!
 \internal
 Updates the options menu according to current context.
 */
void MpCollectionView::updateMenu()
{
    TX_ENTRY
    HbMenu *myMenu = new HbMenu();
    HbAction *menuAction;
    if ( mViewMode == MpCommon::DefaultView ) {
        int count = mCollectionData->count();
        switch ( mCollectionContext ) {
            case ECollectionContextAllSongs:
                mShuffleAction = myMenu->addAction( hbTrId( "txt_mus_opt_shuffle" ) );
                connect( mShuffleAction, SIGNAL( triggered() ), this, SLOT( shufflePlayAll() ) );
                if ( count <= 1 ) {
                    mShuffleAction->setDisabled( true );
                }
                if ( count && !mUsbBlocked ) {
                    menuAction = myMenu->addAction( hbTrId( "txt_mus_opt_add_to_playlist" ) );
                    connect( menuAction, SIGNAL( triggered() ), this, SLOT( addToPlaylist() ) );
                    menuAction = myMenu->addAction( hbTrId( "txt_mus_opt_new_playlist" ) );
                    connect( menuAction, SIGNAL( triggered() ), this, SLOT( createNewPlaylist() ) );
                }
                else {
                    menuAction = myMenu->addAction( hbTrId( "txt_mus_opt_add_to_playlist" ) );
                    menuAction->setDisabled( true );
                    menuAction = myMenu->addAction( hbTrId( "txt_mus_opt_new_playlist" ) );
                    menuAction->setDisabled( true );
                }
                addDefaultMenuOptions( myMenu, true, true );
                break;
            case ECollectionContextArtists:
            case ECollectionContextArtistAlbums:
            case ECollectionContextAlbums:
                addDefaultMenuOptions( myMenu, true, true );
                break;
            case ECollectionContextArtistAlbumsTBone:
            case ECollectionContextAlbumsTBone:
                mShuffleAction = myMenu->addAction( hbTrId( "txt_mus_opt_shuffle" ) );
                connect( mShuffleAction, SIGNAL( triggered() ), this, SLOT( shufflePlayAll() ) );
                if ( !mShuffleEnabled ) {
                    mShuffleAction->setDisabled( true );
                }
                if ( !mUsbBlocked ) {
                    menuAction = myMenu->addAction( hbTrId( "txt_mus_opt_add_to_playlist" ) );
                    connect( menuAction, SIGNAL( triggered() ), this, SLOT( addToPlaylist() ) );
                    menuAction = myMenu->addAction( hbTrId( "txt_mus_opt_new_playlist" ) );
                    connect( menuAction, SIGNAL( triggered() ), this, SLOT( createNewPlaylist() ) );
                }
                else {
                    menuAction = myMenu->addAction( hbTrId( "txt_mus_opt_add_to_playlist" ) );
                    menuAction->setDisabled( true );
                    menuAction = myMenu->addAction( hbTrId( "txt_mus_opt_new_playlist" ) );
                    menuAction->setDisabled( true );
                }
                addDefaultMenuOptions( myMenu, true, true );
                break;
            case ECollectionContextArtistAllSongs:
                mShuffleAction = myMenu->addAction( hbTrId( "txt_mus_opt_shuffle" ) );
                connect( mShuffleAction, SIGNAL( triggered() ), this, SLOT( shufflePlayAll() ) );
                if ( count <= 1 ) {
                    mShuffleAction->setDisabled( true );
                }
                if ( !mUsbBlocked ) {
                    menuAction = myMenu->addAction( hbTrId( "txt_mus_opt_add_to_playlist" ) );
                    connect( menuAction, SIGNAL( triggered() ), this, SLOT( addToPlaylist() ) );
                    menuAction = myMenu->addAction( hbTrId( "txt_mus_opt_new_playlist" ) );
                    connect( menuAction, SIGNAL( triggered() ), this, SLOT( createNewPlaylist() ) );
                }
                else {
                    menuAction = myMenu->addAction( hbTrId( "txt_mus_opt_add_to_playlist" ) );
                    menuAction->setDisabled( true );
                    menuAction = myMenu->addAction( hbTrId( "txt_mus_opt_new_playlist" ) );
                    menuAction->setDisabled( true );
                }
                addDefaultMenuOptions( myMenu, true, true );
                break;
            case ECollectionContextPlaylists:
                menuAction = myMenu->addAction( hbTrId( "txt_mus_opt_new_playlist" ) );
                if ( !mUsbBlocked ) {
                    connect( menuAction, SIGNAL( triggered() ), this, SLOT( prepareToAddToPlaylist() ) );
                }
                else {
                    menuAction->setDisabled( true );
                }
                addDefaultMenuOptions( myMenu, true, true );
                break;
            case ECollectionContextPlaylistSongs:
                mShuffleAction = myMenu->addAction( hbTrId( "txt_mus_opt_shuffle" ) );
                connect( mShuffleAction, SIGNAL( triggered() ), this, SLOT( shufflePlayAll() ) );
                if ( count <= 1 ) {
                    mShuffleAction->setDisabled( true );
                }
                if ( !mCollectionData->isAutoPlaylist() ) {
                menuAction = myMenu->addAction( hbTrId( "txt_common_opt_rename_item" ) );
                    if ( !mUsbBlocked ) {
                        connect( menuAction, SIGNAL( triggered() ), this, SLOT( renameCurrentPlaylistContainer() ) );
                    }
                    else {
                        menuAction->setDisabled( true );
                    }
                }
                addDefaultMenuOptions( myMenu, true, true );
                break;
            default:
                // Provide 'exit' if we accidentally fall in an unknown context
                addDefaultMenuOptions( myMenu, false, true );
                break;
        }
    }
    else if ( mViewMode == MpCommon::FetchView ) {
        addDefaultMenuOptions( myMenu, true, false );
    }

    setMenu( myMenu );
    TX_EXIT
}

/*!
 \internal
 Add default options to a specific menu.
 */
void MpCollectionView::addDefaultMenuOptions( HbMenu *menu, bool optRefresh, bool optExit )
{
    TX_ENTRY
    if ( menu ) {
        HbAction *menuAction;
        if( optRefresh ) {
            menuAction = menu->addAction( hbTrId( "txt_mus_opt_refresh_library" ) );
            if ( !mUsbBlocked ) {
                connect( menuAction, SIGNAL( triggered() ), mMpEngine, SLOT( refreshLibrary() ) );
            }
            else {
                menuAction->setDisabled( true );
                }
        }
        if ( optExit ) {
            connect( menu->addAction(hbTrId("txt_common_opt_exit")), SIGNAL( triggered() ), this, SLOT( exit() ) );
        }
    }
    TX_EXIT
}

/*!
 \internal
 Updates the Toolbar according to current context.
 */
void MpCollectionView::updateToolBar()
{
    TX_ENTRY

    switch ( mCollectionContext ) {
        case ECollectionContextPlaylistSongs:
            if ( !mCollectionData->isAutoPlaylist() ) {
                setPlaylistToolBar();
            }
            else if ( !toolBar()->actions().empty() ) {
                takeToolBar();
                setToolBar( new HbToolBar );
            }
            break;
        case ECollectionContextArtistAlbums:
        case ECollectionContextArtistAlbumsTBone:
        case ECollectionContextArtistAllSongs:
        case ECollectionContextAlbumsTBone:
            if ( !toolBar()->actions().empty() ) {
                takeToolBar();
                setToolBar( new HbToolBar );
            }
            break;
        default:
            setMainToolBar();
            break;
    }
    TX_EXIT
}

/*!
 \internal
 Updates the softkey according to current context.
 */
void MpCollectionView::setSoftkey()
{
    if ( mViewMode == MpCommon::FetchView ) {
        // 'Back' is used in all views in fetch mode because we must
        // appear as an embedded application.
        setNavigationAction( mSoftKeyBack );
    }
    else {
        switch ( mCollectionContext ) {
            case ECollectionContextAllSongs:
            case ECollectionContextArtists:
            case ECollectionContextAlbums:
            case ECollectionContextPlaylists:
                setNavigationAction( mSoftKeyQuit );
                break;
            default:
                setNavigationAction( mSoftKeyBack );
                break;
        }
    }
}

/*!
 \internal
 Sets the Now Playing Banner visibility based on \a visible.
 */
void MpCollectionView::setBannerVisibility( bool visible )
{
    bool ok = false;
    if ( visible ) {
        mDocumentLoader->load( MUSIC_COLLECTION_DOCML, "showBanner", &ok );
    }
    else {
        mDocumentLoader->load( MUSIC_COLLECTION_DOCML, "hideBanner", &ok );
    }

    if ( !ok ) {
        TX_LOG_ARGS( "Error: invalid xml file." );
        Q_ASSERT_X( ok, "MpCollectionView::setBannerVisibility", "invalid xml file" );
    }
}

/*!
 \internal
 Generates a random index for shuffle all. In case of All Songs, due to incremental open
 delay, the chosen index may not be available yet. In such case, narrow down the upper limit
 and regenerate until a valid index is found.
 */
int MpCollectionView::generateShuffleIndex()
{
    int low = 0;
    int high = 0;
    bool tbone;
    switch ( mCollectionContext ) {
        case ECollectionContextArtistAlbumsTBone:
        case ECollectionContextAlbumsTBone:
            tbone = true;
            high = mCollectionData->albumSongsCount();
            break;
        default:
            tbone = false;
            high = mCollectionData->count();
            break;
    }

    bool validIndex = false;
    int index = 0;
    int songId;
    time_t seconds;
    while ( !validIndex ) {
        time( &seconds );
        srand( ( unsigned int ) seconds );
        index = rand() % ( high - low + 1 ) + low;

        if ( tbone ) {
            songId = mCollectionData->albumSongId(index);
        }
        else {
            songId = mCollectionData->itemId(index);
        }

        if ( songId != -1 ) {
            // Got a valid index; exit the loop.
            validIndex = true;
        }
        else {
            // Invalid index; re-select an index, but narrow down the
            // upper range to increase the change of hitting a valid index.
            high = index;
        }
        TX_LOG_ARGS( "index=" << index << "valid=" << validIndex );
    }
    return index;
}

/*!
 \internal
 starts a transition of the main container with a decoy snapshot.
 */
void MpCollectionView::startContainerTransition( TCollectionContext contextFrom, TCollectionContext contextTo )
{
    if (contextFrom == contextTo) {
        return;
    }

    if ( !mSnapshot ) {
        mSnapshot = new MpSnapshotWidget();
    }
    mSnapshot->capture( mWindow, mMainContainer );
    mWindow->scene()->addItem( mSnapshot );


    if( ( contextFrom == ECollectionContextAlbumsTBone && contextTo == ECollectionContextAlbums ) ||
             ( contextFrom == ECollectionContextArtistAlbums && contextTo == ECollectionContextArtists ) ||
             ( contextFrom == ECollectionContextArtistAlbumsTBone && contextTo == ECollectionContextArtistAlbums ) ||
             ( contextFrom == ECollectionContextArtistAlbumsTBone && contextTo == ECollectionContextArtists ) ||
             ( contextFrom == ECollectionContextArtistAllSongs && contextTo == ECollectionContextArtistAlbums ) ||
             ( contextFrom == ECollectionContextPlaylistSongs && contextTo == ECollectionContextPlaylists ) ) {
        HbEffect::start( mSnapshot,
                QString( CONTAINER_EFFECT_GROUP ),
                QString( HIDE_BACK_EFFECT ),
                this,
                "containerTransitionComplete",
                QString( EFFECT_TARGET_SNAPSHOT) );

        HbEffect::start( mMainContainer,
                QString( CONTAINER_EFFECT_GROUP ),
                QString( SHOW_BACK_EFFECT ),
                this,
                "containerTransitionComplete",
                QString( EFFECT_TARGET_CONTAINER) );
    }
    else {
        HbEffect::start( mSnapshot,
                QString( CONTAINER_EFFECT_GROUP ),
                QString( HIDE_EFFECT ),
                this,
                "containerTransitionComplete",
                QString( EFFECT_TARGET_SNAPSHOT) );

        HbEffect::start( mMainContainer,
                QString( CONTAINER_EFFECT_GROUP ),
                QString( SHOW_EFFECT ),
                this,
                "containerTransitionComplete",
                QString( EFFECT_TARGET_CONTAINER ) );
    }
}

/*!
 \internal
 Closes any active dialog or menu.
 */
void MpCollectionView::closeActiveDialog( bool onlyContextMenu )
{
    mMpPopupHandler->cancelOngoingPopup( onlyContextMenu );
    menu()->close();
}

/*!
 \internal
 Shows Corrupted Note in collection view
 */
void MpCollectionView::showCorruptedNote()
{
    TX_ENTRY
    HbMessageBox *messageBox = new HbMessageBox( hbTrId( "txt_mus_info_file_is_corrupt" ), HbMessageBox::MessageTypeInformation );
    messageBox->setAttribute( Qt::WA_DeleteOnClose );
    messageBox->setIcon( HbIcon( QString("qtg_small_fail") ) ); 
    messageBox->show();
    TX_EXIT
}
