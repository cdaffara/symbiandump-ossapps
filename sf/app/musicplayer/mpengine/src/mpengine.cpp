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
* Description: Wrapper for mpx framework utilities.
*
*/

#include <xqsharablefile.h>
#include <EqualizerConstants.h>

#include "mpengine.h"
#include "mpmpxharvesterframeworkwrapper.h"
#include "mpmpxcollectionframeworkwrapper.h"
#include "mpmpxplaybackframeworkwrapper.h"
#include "mpaudioeffectsframeworkwrapper.h"
#include "mpequalizerframeworkwrapper.h"
#include "mptrace.h"
#include "mpsettingsmanager.h"
#include "mpsongscanner.h"
#include "mpsongdata.h"
#include "mpapplicationmonitor.h"
#include "mpplaybackdata.h"

/*!
    \class MpEngine
    \brief Engine for musicplayer - mpx framework utilities.

    MP Engine provides Qt style interface to the MPX framework
    utilities. Its implementation is hidden using private class data pattern.
*/

/*!
-------------------------------------------------------------------------------
    Harvesting related signals
-------------------------------------------------------------------------------
*/

/*!
    \fn void libraryAboutToUpdate()

    This signal is emitted when a scan operation has been requested or
    when MTP synchronization starts.

 */

/*!
    \fn void libraryUpdated()

    This signal is emitted when library has changed.
    
    \sa handleScanEnded()
    \sa handleDiskEvent()
    \sa handleUsbMtpEndEvent()

 */

/*!
    \fn void usbBlocked( bool blocked )

    This signal is emitted to update the usb blocking state
    due a previous usb event received.
    
    \sa handleUsbEvent()

 */

/*!
    \fn void unableToCotinueDueUSB()

    This signal is emitted if usb blocking verification returns true.
    
    \sa verifyUsbBlocking()

 */

/*!
    \fn void usbSynchronizationStarted()

    This signal is emitted when usb is connected in MassStorage mode or MTP mode and synchronizing.

 */

/*!
    \fn void usbSynchronizationFinished()

    This signal is emitted when usb in synchronizing state is disconnected.

 */


/*!
-------------------------------------------------------------------------------
    Collection related signals
-------------------------------------------------------------------------------
*/

/*!
    \fn void collectionPlaylistOpened()

    This signal is emitted when A new collection playlist is opened and 
    playback is initialized.

 */

/*!
    \fn void playlistSaved( bool success )

    This signal is emitted when playlist save operation is completed, it 
    indicates the operation \a success .

 */

/*!
    \fn void aboutToAddSongs( int count )

    Signal emitted up on a notification from MPX Collection wrapper, 
    when play list is prepared and about to be added.
    \a count Indicates number of songs to be added.

 */

/*!
    \fn void deleteStarted(TCollectionContext context, int Count)

    This signal is emitted when song delete operation has started.
    \a context The context where delete operation is taking place.
    \a Count Number of items to delete.

 */

/*!
    \fn void songsDeleted( bool success )

    This signal is emitted when song delete operation is completed, it 
    indicates the operation \a success .

 */

/*!
    \fn void playlistsRenamed( bool success )

    This signal is emitted when playlist rename operation is completed, it
    indicates the operation  \a success .

 */

/*!
    \fn void isolatedCollectionOpened( MpMpxCollectionData* collectionData )

    This signal is emitted when an isolated collection is opened on \a context

 */

/*!
    \fn void restorePathFailed()

    This signal is emitted when an the previous path cannot be restored 
    (i.e. no music in collection).

 */

/*!
    \fn void containerContentsChanged()

    This signal is emitted when items are removed or inserted on the current 
    container.

 */

/*!
-------------------------------------------------------------------------------
    Playback related signals
-------------------------------------------------------------------------------
*/

/*!
    \fn void volumePropertyChanged( MpCommon::MpVolumeProperty, int value );

    This signal is emitted when framework receives a volume related property notification,
    such as EPbPropertyVolume, EPbPropertyMaxVolume, EPbPropertyMute.
 */

/*!
    \fn void corruptedStop();
    
    This signal is emitted when framework determines the corrupted song is the last 
    song of the playlist.
*/

/*!
 Constructs music player engine.
 */
MpEngine::MpEngine()
    : mMpxHarvesterWrapper(0),
      mSongScanner(0),
      mMpxCollectionWrapper(0),
      mMpxPlaybackWrapper(0),
      mAudioEffectsWrapper(0),
      mEqualizerWrapper(0),
      mCurrentPresetIndex(0),
      mSongData(0),
      mApplicationMonitor(0),
      mUsbBlockingState(USB_NotConnected),
      mPreviousUsbState(USB_NotConnected),
      mHandleMediaCommands(true)
{
    TX_LOG
}

/*!
 Destructs music player engine.
 */
MpEngine::~MpEngine()
{
    TX_ENTRY
    if ( mMpxPlaybackWrapper ) {
        // Don't send stop cmd if mode=HomeScreen, because we don't want to stop playback when the music widget is deleted.
        if ( mEngineMode != HomeScreen ) {
            mMpxPlaybackWrapper->forceStop();
        }
        delete mMpxPlaybackWrapper;
    }
    
    delete mMpxHarvesterWrapper;
    delete mMpxCollectionWrapper;
    delete mAudioEffectsWrapper;
    delete mEqualizerWrapper;
    delete mSongScanner;
    delete mSongData;
    delete mApplicationMonitor;
    TX_EXIT
}

/*!
 Initialize engine
 */
void MpEngine::initialize( quint32 clientSecureId, EngineMode mode )
{
    TX_ENTRY_ARGS("clientSecureId=" << clientSecureId << ", mode=" << mode);

    mEngineMode = mode;
    if ( StandAlone == mode ) {
        mSongData = new MpSongData();

        // Harvesting Wrapper
        mMpxHarvesterWrapper = new MpMpxHarvesterFrameworkWrapper( clientSecureId );
        connect( mMpxHarvesterWrapper, SIGNAL( scanStarted() ),
                 this, SLOT( handleScanStarted() ), 
				 Qt::QueuedConnection );
        connect( mMpxHarvesterWrapper, SIGNAL( scanEnded(int, int) ),
                 this, SLOT( handleScanEnded(int, int) ), 
				 Qt::QueuedConnection );
        qRegisterMetaType<MpxDiskEvents>("MpxDiskEvents");
        connect( mMpxHarvesterWrapper, SIGNAL( diskEvent(MpxDiskEvents) ),
                 this, SLOT( handleDiskEvent(MpxDiskEvents) ), 
				 Qt::QueuedConnection );
        qRegisterMetaType<MpxUsbEvents>("MpxUsbEvents");
        connect( mMpxHarvesterWrapper, SIGNAL( usbEvent(MpxUsbEvents) ),
                 this, SLOT( handleUsbEvent(MpxUsbEvents) ), 
				 Qt::QueuedConnection );

        // Collection Wrapper
        mMpxCollectionWrapper = new MpMpxCollectionFrameworkWrapper( clientSecureId, mSongData );
        connect( mMpxCollectionWrapper, SIGNAL( collectionPlaylistOpened() ),
                 this, SLOT( handleCollectionPlaylistOpened() ), 
				 Qt::QueuedConnection );
        connect( mMpxCollectionWrapper, SIGNAL( aboutToAddSongs(int) ),
                 this, SIGNAL( aboutToAddSongs(int) ) );
        connect( mMpxCollectionWrapper, SIGNAL( playlistSaved(bool) ),
                 this, SIGNAL( playlistSaved(bool) ), 
				 Qt::QueuedConnection );
        connect( mMpxCollectionWrapper, SIGNAL( playlistsRenamed(bool) ),
                 this, SIGNAL( playlistsRenamed(bool) ), 
				 Qt::QueuedConnection );
        connect( mMpxCollectionWrapper, SIGNAL( isolatedCollectionOpened(MpMpxCollectionData*) ),
                 this, SIGNAL( isolatedCollectionOpened(MpMpxCollectionData*) ), 
				 Qt::QueuedConnection );
        connect( mMpxCollectionWrapper, SIGNAL( containerContentsChanged() ),
                 this, SIGNAL( containerContentsChanged() ), 
				 Qt::QueuedConnection );
        connect( mMpxCollectionWrapper, SIGNAL( deleteStarted(TCollectionContext, int) ),
                 this, SLOT( handleDeleteStarted(TCollectionContext, int) ) );
        connect( mMpxCollectionWrapper, SIGNAL( songsDeleted(bool) ),
                 this, SLOT( handleDeleteEnded(bool) ), 
				 Qt::QueuedConnection );
        connect( mMpxCollectionWrapper, SIGNAL( restorePathFailed() ),
                 this, SIGNAL( restorePathFailed() ),
                 Qt::QueuedConnection );
        // Equalizer wrapper; this needs to be created before playback wrapper.
        mEqualizerWrapper = new MpEqualizerFrameworkWrapper();
        connect( mEqualizerWrapper, SIGNAL( equalizerReady() ),
                 this, SLOT( handleEqualizerReady() ), 
				 Qt::QueuedConnection );

        // Playback Wrapper
        mMpxPlaybackWrapper = new MpMpxPlaybackFrameworkWrapper( clientSecureId, mSongData );
        connect( this, SIGNAL( libraryUpdated() ),
                 mMpxPlaybackWrapper, SLOT( closeCurrentPlayback() ) );
        connect( mMpxPlaybackWrapper, SIGNAL ( corruptedStop(bool) ),
                 this, SLOT( handleCorruptedStop(bool) ));
		connect( mMpxPlaybackWrapper, SIGNAL( volumePropertyChanged( MpCommon::MpVolumeProperty, int ) ),
                 this, SIGNAL( volumePropertyChanged( MpCommon::MpVolumeProperty, int ) ) );

        // AudioEffects wrapper
        mAudioEffectsWrapper = new MpAudioEffectsFrameworkWrapper();
    }
    else if ( Fetch == mode ) {
        // Harvesting Wrapper
        mMpxHarvesterWrapper = new MpMpxHarvesterFrameworkWrapper( clientSecureId );
        connect( mMpxHarvesterWrapper, SIGNAL( scanStarted() ),
                 this, SLOT( handleScanStarted() ), 
				 Qt::QueuedConnection );
        connect( mMpxHarvesterWrapper, SIGNAL( scanEnded(int, int) ),
                 this, SLOT( handleScanEnded(int, int) ), 
				 Qt::QueuedConnection );
        qRegisterMetaType<MpxDiskEvents>("MpxDiskEvents");
        connect( mMpxHarvesterWrapper, SIGNAL( diskEvent(MpxDiskEvents) ),
                 this, SLOT( handleDiskEvent(MpxDiskEvents) ), 
				 Qt::QueuedConnection );
        qRegisterMetaType<MpxUsbEvents>("MpxUsbEvents");
        connect( mMpxHarvesterWrapper, SIGNAL( usbEvent(MpxUsbEvents) ),
                 this, SLOT( handleUsbEvent(MpxUsbEvents) ), 
				 Qt::QueuedConnection );

        // Collection Wrapper
        mMpxCollectionWrapper = new MpMpxCollectionFrameworkWrapper( clientSecureId, 0 );
        connect( mMpxCollectionWrapper, SIGNAL( collectionPlaylistOpened() ),
                 this, SLOT( handleCollectionPlaylistOpened() ), 
				 Qt::QueuedConnection );
        // Disabling these since fetch mode plays only one song at a time.
        mMpxCollectionWrapper->setRepeatFeatureEnabled( false );
        mMpxCollectionWrapper->setShuffleFeatureEnabled( false );

        // Playback Wrapper
        mMpxPlaybackWrapper = new MpMpxPlaybackFrameworkWrapper( clientSecureId, 0 );
        connect( this, SIGNAL( libraryUpdated() ),
                 mMpxPlaybackWrapper, SLOT( closeCurrentPlayback() ) );
        connect( mMpxPlaybackWrapper, SIGNAL ( corruptedStop(bool) ),
                 this, SLOT( handleCorruptedStop(bool) ));				 
	    connect( mMpxPlaybackWrapper, SIGNAL( volumePropertyChanged( MpCommon::MpVolumeProperty, int ) ),
                 this, SIGNAL( volumePropertyChanged( MpCommon::MpVolumeProperty, int ) ) );

    }
    else if ( MediaBrowsing == mode ) {
        // Collection Wrapper
        mMpxCollectionWrapper = new MpMpxCollectionFrameworkWrapper( clientSecureId, 0 );
        connect( mMpxCollectionWrapper, SIGNAL( collectionPlaylistOpened() ),
                 this, SLOT( handleCollectionPlaylistOpened() ), 
				 Qt::QueuedConnection );
        connect( mMpxCollectionWrapper, SIGNAL( containerContentsChanged() ),
                 this, SIGNAL( containerContentsChanged() ), 
				 Qt::QueuedConnection );
    }
    else if ( Embedded == mode ) {
        mSongData = new MpSongData();
        // Playback Wrapper
        mMpxPlaybackWrapper = new MpMpxPlaybackFrameworkWrapper( clientSecureId, mSongData );
        connect( mMpxPlaybackWrapper, SIGNAL( volumePropertyChanged( MpCommon::MpVolumeProperty, int ) ),
                 this, SIGNAL( volumePropertyChanged( MpCommon::MpVolumeProperty, int ) ) );
        connect( mMpxPlaybackWrapper, SIGNAL ( corruptedStop(bool) ),
                 this, SLOT( handleCorruptedStop(bool) ));
    }
    else if ( HomeScreen == mode ) {
        // Harvesting Wrapper
        mMpxHarvesterWrapper = new MpMpxHarvesterFrameworkWrapper( clientSecureId );
        connect( mMpxHarvesterWrapper, SIGNAL( scanStarted() ),
                 this, SLOT( handleScanStarted() ),
                 Qt::QueuedConnection );
        connect( mMpxHarvesterWrapper, SIGNAL( scanEnded(int, int) ),
                 this, SLOT( handleScanEnded(int, int) ),
                 Qt::QueuedConnection );
        qRegisterMetaType<MpxDiskEvents>("MpxDiskEvents");
        connect( mMpxHarvesterWrapper, SIGNAL( diskEvent(MpxDiskEvents) ),
                 this, SLOT( handleDiskEvent(MpxDiskEvents) ),
                 Qt::QueuedConnection );
        qRegisterMetaType<MpxUsbEvents>("MpxUsbEvents");
        connect( mMpxHarvesterWrapper, SIGNAL( usbEvent(MpxUsbEvents) ),
                 this, SLOT( handleUsbEvent(MpxUsbEvents) ),
                 Qt::QueuedConnection );

        // Playback Wrapper
        mMpxPlaybackWrapper = new MpMpxPlaybackFrameworkWrapper( clientSecureId, mSongData );
    }
    TX_EXIT
}

/*!
-------------------------------------------------------------------------------
    Harvesting related
-------------------------------------------------------------------------------
*/

/*!
 \
 Used to verify if an action can be executed depending on USB blocking state.
 If not, a notification note might be displayed.
 */
bool MpEngine::verifyUsbBlocking( bool notify )
{
    TX_ENTRY
    bool result( false );
    if ( mUsbBlockingState == USB_Connected ) {
        result = true;
        if ( notify ) {
            emit unableToCotinueDueUSB();
        }
    }
    TX_EXIT
    return result;
}

/*!
 \
 Request Harvester to check if there are any system events active.
 */
void MpEngine::checkForSystemEvents()
{
    TX_ENTRY
    mMpxHarvesterWrapper->checkForSystemEvents();
    TX_EXIT
}

/*!
 \
 Returs the current songScanner instance
 */
MpSongScanner *MpEngine::songScanner()
{
    return mSongScanner;
}
/*!
 Slot to be called to start Refresh library process.
 If scanning is already ongoing, this request is ignored.
 */
void MpEngine::refreshLibrary( bool automaticRequest )
{
    TX_ENTRY
    if ( !verifyUsbBlocking( true ) ) {
        if ( !mSongScanner ) {
            mSongScanner = new MpSongScanner( mMpxHarvesterWrapper );
        }
        emit libraryAboutToUpdate();
        mSongScanner->scan( automaticRequest );
    }
    TX_EXIT
}

/*!
 Slot to be called when song scanning starts.
 */
void MpEngine::handleScanStarted() {
    TX_ENTRY
    mHandleMediaCommands = false;
    emit libraryUpdateStarted();
    TX_EXIT
}

/*!
 Slot to be called when song scanning ends.
 */
void MpEngine::handleScanEnded( int count, int error ) {
    TX_ENTRY
    Q_UNUSED( count );
    Q_UNUSED( error );
    mHandleMediaCommands = true;
    emit libraryUpdated();
    TX_EXIT
}

/*!
 Slot to be called when disk event is received from MPX framework.
 */
void MpEngine::handleDiskEvent( MpxDiskEvents event )
{
    TX_ENTRY_ARGS("event=" << event);
    switch ( event ) {
        case DiskFormatStarted:
            mHandleMediaCommands = false;
            break;
        case DiskFormatEnded:
            mHandleMediaCommands = true;
            break;
        case DiskRemoved:
            if ( mUsbBlockingState != USB_Synchronizing ) {
                emit libraryUpdated();
            }
            break;
        case DiskInserted:
            if ( mUsbBlockingState == USB_NotConnected ) {
                autoRefreshLibrary(true);
            }
            else if ( mUsbBlockingState == USB_Connected ) {
                emit libraryUpdated();
            }
            break;
        default:
            break;
    }
    TX_EXIT
}

/*!
 Slot to be called when USB event is received from MPX framework.
 */
void MpEngine::handleUsbEvent( MpxUsbEvents event )
{
    TX_ENTRY_ARGS("event=" << event);
    switch ( event ) {
        case UsbMassStorageStarted:
            handleUsbMassStorageStartEvent();
            break;
        case UsbMassStorageEnded:
            handleUsbMassStorageEndEvent();
            break;
        case UsbMtpStarted:
            handleUsbMtpStartEvent();
            break;
        case UsbMtpEnded:
            handleUsbMtpEndEvent();
            break;
        case UsbMtpNotActive:
            handleUsbMtpNotActive();
        default:
            break;
    }
    TX_EXIT
}

/*!
 To be called when EMcMsgUSBMassStorageStart event is received.
 */
void MpEngine::handleUsbMassStorageStartEvent()
{
    TX_ENTRY
    mHandleMediaCommands = false;

    changeUsbBlockingState( USB_Synchronizing );
    emit usbBlocked(true);
    emit usbSynchronizationStarted();

    TX_EXIT
}

/*!
 To be called when EMcMsgUSBMassStorageEnd event is received.
 */
void MpEngine::handleUsbMassStorageEndEvent()
{
    TX_ENTRY
    mHandleMediaCommands = true;

    changeUsbBlockingState( USB_NotConnected );
    emit usbBlocked(false);
    emit usbSynchronizationFinished();
    autoRefreshLibrary();

    TX_EXIT
}

/*!
 To be called when EMcMsgUSBMTPStart event is received.
 */
void MpEngine::handleUsbMtpStartEvent()
{
    TX_ENTRY
    mHandleMediaCommands = false;
    
    changeUsbBlockingState( USB_Synchronizing );
    emit usbBlocked(true);
    
    //Cancel any ongoing operation.
    emit libraryAboutToUpdate();
    
    emit usbSynchronizationStarted();
    
    TX_EXIT
}

/*!
 To be called when EMcMsgUSBMTPEnd event is received.
 */
void MpEngine::handleUsbMtpEndEvent()
{
    TX_ENTRY
    mHandleMediaCommands = true;

    changeUsbBlockingState( USB_NotConnected );
    emit usbBlocked(false);

    if ( mPreviousUsbState == USB_Synchronizing ) {
        emit usbSynchronizationFinished();
        emit libraryUpdated();
    }
    TX_EXIT
}

/*!
 To be called when EMcMsgUSBMTPNotActive event is received.
 */
void MpEngine::handleUsbMtpNotActive()
{
    TX_ENTRY
    changeUsbBlockingState( USB_Connected );
    emit usbBlocked(true);
    TX_EXIT
}

/*!
 Internal
 Update the new and previous usb blocking state
 */
void MpEngine::changeUsbBlockingState( UsbBlockingState state )
{
    TX_ENTRY
    mPreviousUsbState = mUsbBlockingState;
    mUsbBlockingState = state;
    TX_EXIT
}

/*!
 Internal
 */
void MpEngine::autoRefreshLibrary( bool automaticRequest )
{
    TX_ENTRY_ARGS( "mEngineMode=" << mEngineMode );
    if ( mEngineMode == StandAlone ) {
        refreshLibrary(automaticRequest);
    }
    else if ( mEngineMode == Fetch ) {
        if ( !mApplicationMonitor ) {
            mApplicationMonitor = new MpApplicationMonitor();
        }
        if ( !mApplicationMonitor->isApplicationRunning() ) {
            // In Fetcher mode, only perform library update when main Music
            // Player application is not running. Otherwise, let the main
            // application take care.
            refreshLibrary(automaticRequest);
        }
    }
    TX_EXIT
}

/*!
-------------------------------------------------------------------------------
    Collection related
-------------------------------------------------------------------------------
*/

/*!
 Opens the collection for the given \a context.

 \sa collectionOpened()
 */
void MpEngine::openCollection( TCollectionContext context )
{
    mMpxCollectionWrapper->openCollection( context );
}

/*!
 Opens the collection at a specific \a index.

 \sa collectionOpened(), playlistOpened()
 */
void MpEngine::openCollectionItem( int index )
{
    mMpxCollectionWrapper->openCollectionItem( index );
}

/*!
 Navigates back to the container of the current items.

 \sa collectionOpened()
 */
void MpEngine::back()
{
    mMpxCollectionWrapper->back();
}

/*!
 Loads the user created \a playLists.

 */
void MpEngine::findPlaylists( QStringList &playlists )
{
    mMpxCollectionWrapper->findPlaylists( playlists );
}

/*!
 Creates a new playlist with name \a playlistName and adds \a selection and optionally uses \a collectionData.
 */
void MpEngine::createPlaylist( QString &playlistName, QList<int> &selection, MpMpxCollectionData* collectionData )
{
    if ( !verifyUsbBlocking( true ) ) {
        mMpxCollectionWrapper->createPlaylist( playlistName, selection, collectionData );
    }
}

/*!
 Adds \a selection to the playlist specified in \a playlistIndex.
 */
void MpEngine::saveToPlaylist( int playlistIndex, QList<int> &selection )
{
    if ( !verifyUsbBlocking( true ) ) {
        mMpxCollectionWrapper->saveToPlaylist( playlistIndex, selection );
    }
}

/*!
 Rename a playlist by \a index with \a name.
 */
void MpEngine::renamePlaylist( QString &newName, int index )
{
    if ( !verifyUsbBlocking( true ) ) {
        mMpxCollectionWrapper->renamePlaylist( newName, index );
    }
}

/*!
 Adds \a selection to the current playlist from the specified \a collectionData.
 */
void MpEngine::saveToCurrentPlaylist( QList<int> &selection, MpMpxCollectionData *collectionData )
{
    if ( !verifyUsbBlocking( true ) ) {
        mMpxCollectionWrapper->saveToCurrentPlaylist( selection, collectionData );
    }
}

/*!
 Rename a playlist with \a name.
 */
void MpEngine::renamePlaylist( QString &newName )
{
    if ( !verifyUsbBlocking( true ) ) {
        mMpxCollectionWrapper->renamePlaylist( newName );
    }
}

/*!
 Deletes \a selection from the collection.
 */
void MpEngine::deleteSongs( QList<int> &selection )
{
    if ( !verifyUsbBlocking( true ) ) {
        mMpxCollectionWrapper->deleteSongs( selection );
    }
}



/*!
 Initiate a playback preview for the selected item.
 */
void MpEngine::previewItem( int index )
{
    mMpxCollectionWrapper->previewItem( index );
}


/*!
 Opens the an isolated collection with \a context.
 \sa isolatedCollectionOpened()
 */
void MpEngine::openIsolatedCollection( TCollectionContext context )
{
    if ( !verifyUsbBlocking( true ) ) {
        mMpxCollectionWrapper->openIsolatedCollection( context );
    }
}

/*!
 Releases the resources used for the isolated collection.
 */
void MpEngine::releaseIsolatedCollection()
{
    mMpxCollectionWrapper->releaseIsolatedCollection();
}

/*!
 Finds all songs beloging to the album specified by the \a index.
 */
void MpEngine::findAlbumSongs( int index )
{
    mMpxCollectionWrapper->findAlbumSongs(index);
}

/*!
 Plays album with \a albumIndex starting with the songs with \a songIndex.
 */
void MpEngine::playAlbumSongs( int albumIndex, int songIndex, MpMpxCollectionData* collectionData  )
{
    mMpxCollectionWrapper->playAlbumSongs(albumIndex, songIndex, collectionData);
}

/*!
 Returns pointer to MpMpxCollectionData, which is the collection data.
 */
MpMpxCollectionData *MpEngine::collectionData()
{
    return mMpxCollectionWrapper->collectionData();
}

/*!
 Slot to be called to reopen the collection in its current state.

 \sa collectionOpened()
 */
void MpEngine::reopenCollection()
{
    mMpxCollectionWrapper->reopenCollection();
}

/*!
 Slot to be called to request a reorder operation , indicates that 
 the item with \a playlistId , \a songId is to be moved from 
 \a originalOrdinal to \a newOrdinal.
 */
void MpEngine::reorderPlaylist( int playlistId, int songId, int originalOrdinal, int newOrdinal )
{
    mMpxCollectionWrapper->reorderPlaylist( playlistId, songId, originalOrdinal, newOrdinal );
}

/*!
 Slot to be called when song deleting starts.
 */
void MpEngine::handleDeleteStarted( TCollectionContext context, int count )
{
    TX_ENTRY
    mHandleMediaCommands = false;
    emit deleteStarted( context, count );
    TX_EXIT
}

/*!
 Slot to be called when song deleting ends.
 */
void MpEngine::handleDeleteEnded( bool success )
{
    TX_ENTRY
    mHandleMediaCommands = true;
    emit songsDeleted( success );
    TX_EXIT
}

/*!
 Slot to be called when song deleting ends.
 */
void MpEngine::cancelCollectionRequest()
{
    TX_ENTRY
    mMpxCollectionWrapper->cancelRequest();
    TX_EXIT
}

/*!
 Returns pointer to MpPlaybackData, which is the playback data.
 */
MpPlaybackData *MpEngine::playbackData()
{
    return mMpxPlaybackWrapper->playbackData();
}

/*!
 Requests playback of all songs with shuffle enabled
 */
void MpEngine::shuffleAll()
{
    mMpxCollectionWrapper->openShuffleAllSongsPath();
}

/*!
 Slot to be called to request embedded playback of item with Uri aFilename
 */

void MpEngine::playEmbedded( QString aFilename )
{
    mMpxPlaybackWrapper->play( aFilename );
}

/*!
 Slot to be called to request embedded playback of item with file handle
 */

void MpEngine::playEmbedded(const XQSharableFile& file )
{
    mMpxPlaybackWrapper->play( file );
}

/*!
 Slot to handle a play command
 */

void MpEngine::play()
{
    if ( mHandleMediaCommands ) {
        mMpxPlaybackWrapper->play();
    }
}

/*!
 Slot to handle a pause command.
 */
void MpEngine::pause()
{
    if ( mHandleMediaCommands ) {
        mMpxPlaybackWrapper->pause();
    }
}

/*!
 Slot to handle a play pause.
 */
void MpEngine::playPause()
{
    if ( mHandleMediaCommands ) {
        mMpxPlaybackWrapper->playPause();
    }
}

/*!
 Slot to handle a stop.
 */
void MpEngine::stop()
{
    if ( mHandleMediaCommands ) {
        mMpxPlaybackWrapper->stop();
    }
}

/*!
 Slot to handle a skip forward.
 */
void MpEngine::skipForward()
{
    if ( mHandleMediaCommands ) {
        mMpxPlaybackWrapper->skipForward();
    }
}

/*!
 Slot to handle  seek forward.
 */
void MpEngine::startSeekForward()
{
    if ( mHandleMediaCommands ) {
        mMpxPlaybackWrapper->startSeekForward();
    }
}

/*!
 Slot to handle stop seeking.
 */
void MpEngine::stopSeeking()
{
    if ( mHandleMediaCommands ) {
        mMpxPlaybackWrapper->stopSeeking();
    }
}

/*!
 Slot to handle a skip backwards.
 */
void MpEngine::skipBackward()
{
    if ( mHandleMediaCommands ) {
        mMpxPlaybackWrapper->skipBackward();
    }
}

/*!
 Slot to handle seek backwards.
 */
void MpEngine::startSeekBackward()
{
    if ( mHandleMediaCommands ) {
        mMpxPlaybackWrapper->startSeekBackward();
    }
}
/*!
 Slot to handle a request to change \a position.
 */
void MpEngine::setPosition( int position )
{
    mMpxPlaybackWrapper->setPosition( position );
}

/*!
 Slot to handle a request to change shuffle \a mode.
 */
void MpEngine::setShuffle( bool mode )
{
    mMpxPlaybackWrapper->setShuffle( mode );
}

/*!
 Slot to handle a request to change repeat \a mode.
 */
void MpEngine::setRepeat( bool mode )
{
    mMpxPlaybackWrapper->setRepeat( mode );
}

/*!
 Slot to handle a request to get the volume level Max.
 Response will be asynchronously sent through volumePropertyChanged() signal.
 */
void MpEngine::getMaxVolume( )
{
    mMpxPlaybackWrapper->getMaxVolume();
}

/*!
 Slot to handle a request to get the current volume level.
 Response will be asynchronously sent through volumePropertyChanged() signal.
 */
void MpEngine::getVolume( )
{
    mMpxPlaybackWrapper->getVolume();
}

/*!
 Slot to handle a request to increase volume.
 */
void MpEngine::increaseVolume()
{
    if ( mHandleMediaCommands ) {
        mMpxPlaybackWrapper->increaseVolume();
    }
}

/*!
 Slot to handle a request to decrease volume.
 */
void MpEngine::decreaseVolume()
{
    if ( mHandleMediaCommands ) {
        mMpxPlaybackWrapper->decreaseVolume();
    }
}

/*!
 Slot to handle a request to set the volume level.
 */
void MpEngine::setVolume( int value )
{
    if ( mHandleMediaCommands ) {
        mMpxPlaybackWrapper->setVolume( value );
    }
}

/*!
 Slot to handle a request to get the current mute state.
 Response will be asynchronously sent through volumePropertyChanged() signal.
 */
void MpEngine::getMuteState( )
{
    mMpxPlaybackWrapper->getMuteState();
}

/*!
 Slot to handle a request to mute.
 */
void MpEngine::mute()
{
    if ( mHandleMediaCommands ) {
        mMpxPlaybackWrapper->mute();
    }
}

/*!
 Slot to handle a request to unmute.
 */
void MpEngine::unmute()
{
    if ( mHandleMediaCommands ) {
        mMpxPlaybackWrapper->unmute();
    }
}

/*!
 Slot to be called when playback encounters a corrupted song. \a lastSong
    indicates whether this is the last song of the playlist.
 */
void MpEngine::handleCorruptedStop( bool lastSong )
{
    if ( lastSong ) {
        emit corruptedStop();
    }
    else {
        if ( mEngineMode == StandAlone ) {
            skipForward();
        }
    }
}

/*!
 Returns pointer to MpSongData, which is the song data for Details View.
 */
MpSongData *MpEngine::songData()
{
    return mSongData;
}

/*!
 Retrieves song details for the specified \a index.
 */
void MpEngine::retrieveSongDetails( int index )
{
    TX_ENTRY
    if ( index == -1 ) {
        // Retrieve song details of currently playing song
        mMpxPlaybackWrapper->retrieveSongDetails();
    }
    else {
        mMpxCollectionWrapper->retrieveSongDetails(index);
    }
    TX_EXIT
}

/*!
 Retrieve balance informatioin from audio effects
 */
int MpEngine::balance()
{
    return mAudioEffectsWrapper->balance();
}

/*!
 Retrieve loudness informatioin from audio effects
 */
bool MpEngine::loudness()
{
    return mAudioEffectsWrapper->loudness();
}

/*!
 Slot to handle a request to change \a balance in audio effects.
 */
void MpEngine::setBalance( int balance )
{
    TX_ENTRY
    
    if ( mAudioEffectsWrapper->balance() != balance ) {
        mAudioEffectsWrapper->setBalance( balance );
        mMpxPlaybackWrapper->setBalance( balance );
    }
    
    TX_EXIT
}

/*!
 Slot to handle a request to change loudness \a mode in audio effects.
 */
void MpEngine::setLoudness( bool mode )
{
    TX_ENTRY
    
    if( mAudioEffectsWrapper->loudness() != mode ) { //do not set same value twice
        mAudioEffectsWrapper->setLoudness( mode );
        mMpxPlaybackWrapper->applyAudioEffects();
    }
    
    TX_EXIT
}

/*!
 Apply the preset by giving \a presetIndex. The index is subtracted by 1 because
 index 0 represent "Off" at UI level.
 */
void MpEngine::applyPreset( int presetIndex )
{
    TX_ENTRY_ARGS( "presetIndex=" << presetIndex );
    
    TInt presetKey = mEqualizerWrapper->getPresetNameKey( presetIndex - 1 );
    
    if ( presetKey != KEqualizerPresetNone ) {
        TX_LOG_ARGS( "Preset key in engine =" << presetKey );
        // Store in CenRep file
        MpSettingsManager::setPreset( presetKey );
        // Notify playback framework of the change.
        mMpxPlaybackWrapper->applyEqualizer();
        mCurrentPresetIndex = presetIndex;
    }
    else {
        TX_LOG_ARGS("getPresetNameKey Error  = " << KEqualizerPresetNone);
    }
    
    TX_EXIT
}

/*!
 Disabling equalizer by setting the preset to -1 and apply it to 
 disable current preset.
 */
void MpEngine::disableEqualizer()
{
    TX_ENTRY

    mCurrentPresetIndex = 0;
    // Store in CenRep file
    MpSettingsManager::setPreset( KEqualizerPresetNone );
    // Notify playback framework of the change.
    mMpxPlaybackWrapper->applyEqualizer();

    TX_EXIT
}

/*!
 Return current preset index (UI)
 */
int MpEngine::activePreset()
{
    TX_LOG_ARGS("mCurrentPresetIndex = " << mCurrentPresetIndex );

    return mCurrentPresetIndex;
}

/*!
 Retrieve list of preset names from equalizer wrapper
 */
QStringList MpEngine::presetNames()
{
    return mEqualizerWrapper->presetNames(); 
}

/*!
 Save data needed to later restore state (activity)
 */
void MpEngine::saveActivityData( QByteArray &data )
{
    mMpxCollectionWrapper->savePath( data );
}

/*!
 Restore state from activity data 
 */
void MpEngine::loadActivityData( const QByteArray &data ) 
{
    mMpxCollectionWrapper->restorePath( data );
}

/*!
 Slot to handle equalizer ready signal from equalizer wrapper.
 */
void MpEngine::handleEqualizerReady()
{
    TX_ENTRY
    
    // Get preset id from cenrep
    TInt presetKey( MpSettingsManager::preset() );
    
    mCurrentPresetIndex = mEqualizerWrapper->getPresetIndex( presetKey );
    emit equalizerReady();
    
    TX_EXIT
}

/*!
 Slot to handle the open of a collecion playlist.
 */
void MpEngine::handleCollectionPlaylistOpened()
{
    TX_ENTRY
    
    playbackData()->handleCollectionPlaylistOpened();
    emit collectionPlaylistOpened();
    
    TX_EXIT
}
