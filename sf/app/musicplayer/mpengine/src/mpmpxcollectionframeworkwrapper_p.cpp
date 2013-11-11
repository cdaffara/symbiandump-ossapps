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
* Description: Wrapper for mpx collection framework utilities - private implementation.
*
*/

#include <QStringList>
#include <QRegExp>
#include <QFileInfo>
#include <QDateTime>
#include <QTime>
#include <QDate>

#include <s32strm.h>
#include <s32mem.h>
#include <apgcli.h>
#include <apacmdln.h>

#include <mpxplaybackutility.h>
#include <mpxcollectionutility.h>
#include <mpxharvesterutility.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionmessage.h>
#include <mpxcollectionplaylist.h>
#include <mpxmessagecontainerdefs.h>
#include <mpxmediacollectiondetaildefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediaaudiodefs.h>
#include <mpxmediadrmdefs.h>
#include <mpxcollectionplugin.hrh>
#include <mpxmessagegeneraldefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxcollectionmessagedefs.h>
#include <mpxparameter.h>
#include <mpxcollectionhelperfactory.h>
#include <mpxcollectionuihelper.h>
#include <mpxcollectionopenutility.h>
#include <mpxmediaarray.h>
#include <mpxmedia.h>
#include <mpxlog.h>

#include "mpmpxcollectionframeworkwrapper_p.h"
#include "mpmpxcollectionframeworkwrapper.h"
#include "mpmpxcollectiondata.h"
#include "mpcommondefs.h"
#include "mptrace.h"
#include "mpsettingsmanager.h"
#include "mpsongdata.h"

const TInt KIncrementalDelay = 0;
const TInt KIncrementalFetchBlockSize = 1000;

const TInt KMPXChunkSize = 100;  // number of songs added in each chunk, IncAddL
_LIT( KPlaylistPath, "C:\\Data\\Playlists\\" ); // Todo

/*!
    \class MpMpxCollectionFrameworkWrapperPrivate
    \brief Wrapper for mpx framework utilities - private implementation.

    This is a private implementation of the mpx framework wrapper utilties interface.
*/

/*!
 \internal
 */
MpMpxCollectionFrameworkWrapperPrivate::MpMpxCollectionFrameworkWrapperPrivate( MpMpxCollectionFrameworkWrapper *wrapper )
    : q_ptr( wrapper ),
      iCollectionUtility( 0 ),
      iCollectionUiHelper( 0 ),
      iIncrementalOpenUtil( 0 ),
      iIsolatedCollectionHelper( 0 ),
      iPlaybackUtility( 0 ),
      iCollectionData( 0 ),
      iIsolatedCollectionData( 0 ),
      iFirstIncrementalOpen( EFalse ),
      iUserPlaylists( 0 ),
      iRepeatFeature( ETrue ),
      iShuffleFeature( ETrue ),
      iReopen( EFalse ),
      iShuffleAll( EFalse ),
      iRestoreDefaultPath( EFalse ),
      iRestorePathIndex( 0 ),
	  iSongData( 0 )
{
    TX_LOG
}

/*!
 \internal
 */
MpMpxCollectionFrameworkWrapperPrivate::~MpMpxCollectionFrameworkWrapperPrivate()
{
    TX_ENTRY
    delete iCollectionData;
    delete iIsolatedCollectionData;

    if ( iCollectionUtility ) {
        iCollectionUtility->Collection().CancelRequest();
        iCollectionUtility->Close();
    }

    if ( iCollectionUiHelper ) {
        iCollectionUiHelper->Close();
    }

    if ( iPlaybackUtility ) {
        iPlaybackUtility->Close();
    }

    delete iIncrementalOpenUtil;
    delete iIsolatedCollectionHelper;
    delete iUserPlaylists;

    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::init( quint32 clientSecureId, MpSongData *songData )
{
    TX_ENTRY
    iHostUid = TUid::Uid( clientSecureId );
    iSongData = songData;
    TRAPD( err, DoInitL() );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::openCollection( TCollectionContext context )
{
    TRAPD( err, DoOpenCollectionL( context ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::openCollectionItem( int index )
{
    TRAPD( err, DoOpenCollectionItemL( index ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::reopenCollection()
{
    TRAPD( err, DoReopenCollectionL() );
    if ( err == KErrNone ) {
        iReopen = ETrue;
    }
    else {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::back()
{
    TRAPD( err, DoBackL() );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}
/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::findPlaylists( QStringList &playlists )
{
    TRAPD( err, DoFindPlaylistsL( playlists ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::createPlaylist( QString &playlistName, QList<int> &selection, MpMpxCollectionData* collectionData )
{
    TRAPD( err, DoCreatePlaylistL( playlistName, selection, collectionData ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::saveToPlaylist( int playlistIndex, QList<int> &selection )
{
    TRAPD( err, DoSaveToPlaylistL( playlistIndex, selection ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::saveToCurrentPlaylist( QList<int> &selection, MpMpxCollectionData *collectionData )
{
    TRAPD( err, DoSaveToCurrentPlaylistL( selection, collectionData ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
} 

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::deleteSongs( QList<int> &selection )
{
    TCollectionContext context = iCollectionData->context();
    int err;
    if ( context == ECollectionContextArtistAlbumsTBone
         || context == ECollectionContextAlbumsTBone ) {
        TRAP( err, DoDeleteAlbumSongsL( selection ) );
    }
    else {
        TRAP( err, DoDeleteSongsL( selection ) );
    }
    if ( err == KErrNone ) {
        // send signal that deleting has started.
        emit q_ptr->deleteStarted(iCollectionData->context(), selection.count());
    }
    else{
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::renamePlaylist( QString &newName, int index )
{      
    TRAPD( err, DoRenamePlaylistL( newName, index ) );
    if ( err != KErrNone ) {
    TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::renamePlaylist( QString &newName )
{      
    TRAPD( err, DoRenamePlaylistL( newName ) );
    if ( err != KErrNone ) {
    TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}
/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::setShuffle( bool active )
{
    TRAPD( err, DoSetShuffleL( active ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::previewItem( int index )
{
    TCollectionContext context = iCollectionData->context();
    int err;
    if ( context == ECollectionContextArtistAlbumsTBone
         || context == ECollectionContextAlbumsTBone ) {
        TRAP( err, DoPreviewAlbumSongL( index ) );
    }
    else {
        TRAP( err, DoPreviewSongL( index ) );
    }
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
 void MpMpxCollectionFrameworkWrapperPrivate::openIsolatedCollection( TCollectionContext context )
{
    TRAPD( err, DoOpenIsolatedCollectionL( context ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::releaseIsolatedCollection()
{
    delete iIsolatedCollectionHelper;
    iIsolatedCollectionHelper = 0;
    delete iIsolatedCollectionData;
    iIsolatedCollectionData = 0;
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::reorderPlaylist( int playlistId, int songId, int originalOrdinal, int newOrdinal )
{
    TRAPD( err, DoReorderPlaylistL( playlistId, songId, originalOrdinal, newOrdinal ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::cancel()
{
    iCollectionUiHelper->Cancel();
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::findAlbumSongs( int index )
{
    TRAPD( err, DoFindAlbumSongsL( index ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::playAlbumSongs( int albumIndex, int songIndex, MpMpxCollectionData* collectionData  )
{
    TRAPD( err, DoPlayAlbumSongsL( albumIndex, songIndex, collectionData ? collectionData : iCollectionData ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
MpMpxCollectionData *MpMpxCollectionFrameworkWrapperPrivate::collectionData()
{
    return iCollectionData;
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::openShuffleAllSongsPath()
{
    TX_ENTRY
    TRAPD( err, DoOpenCollectionL(ECollectionContextAllSongs) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
    else {
        iShuffleAll = ETrue; 
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::retrieveSongDetails( int index )
{
    TX_ENTRY
    TRAPD( err, DoRetrieveSongDetailsL(index) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::savePath( QByteArray &data )
{
    TX_ENTRY
    TRAPD( err, DoSavePathL( data ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
    TX_EXIT
    
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::restorePath( const QByteArray &data )
{
    TX_ENTRY
    TRAPD( err, DoRestorePathL( data ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
    TX_EXIT
}


/*!
 \internal
 Result of open or re-open operation to the Collection Framework.
 */
void MpMpxCollectionFrameworkWrapperPrivate::HandleOpenL( 
    const CMPXMedia& aEntries,
    TInt aIndex,
    TBool aComplete,
    TInt aError )
{
    Q_UNUSED( aIndex );
    TX_UNUSED( aComplete );
    TX_ENTRY_ARGS( "aError=" << aError << "aComplete=" << aComplete );
    if ( aError == KErrNone ) {
        if ( iFirstIncrementalOpen ) {
            iFirstIncrementalOpen = EFalse;
            if( iShuffleAll ) {
                iShuffleAll = EFalse;
                TX_LOG_ARGS( "Path is ready" );
                DoPlayAllSongsPlaylistL();
            }
            iCollectionData->setMpxMedia( aEntries, iReopen );
            iReopen = EFalse;
        }
        else {
            iCollectionData->incrementalOpenUpdate();
        }
    }
    else {
        TX_LOG_ARGS( "Error: " << aError << "; should never get here." );
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::HandleOpenL( 
    const CMPXCollectionPlaylist& aPlaylist,
    TInt aError )
{
    TX_ENTRY_ARGS( "aError=" << aError );

    if ( aError == KErrNone ) {
        //TODO: all calls to playback utility should be done via the engine and trough the playback FW wrapper.
        createPlaybackUtilityL();
        iPlaybackUtility->InitL( aPlaylist, ETrue );
        emit q_ptr->collectionPlaylistOpened();
    }
    else {
        TX_LOG_ARGS( "Error: " << aError << "; should never get here." );
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::HandleCollectionMessage( 
    CMPXMessage* aMsg,
    TInt aErr )
{
    TX_ENTRY_ARGS( "aErr=" << aErr );
    if ( aErr == KErrNone && aMsg )
        {
        TRAP_IGNORE( DoHandleCollectionMessageL( *aMsg ) );
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::HandleCollectionMediaL( 
    const CMPXMedia& aMedia,
    TInt aError )
{
    TX_ENTRY
    if ( KErrNone != aError ){
        TX_LOG_ARGS( "Error: " << aError << "; should never get here." );
        TX_EXIT
        return;
    }
    iSongData->setMpxMedia( aMedia );
    TX_EXIT
}

/*!
 \internal
 Handles the completion of any collection helper event.
 */
void MpMpxCollectionFrameworkWrapperPrivate::HandleOperationCompleteL( 
    TCHelperOperation aOperation,
    TInt aErr,
    void* aArgument )
{
    TX_ENTRY_ARGS( "aErr=" << aErr );
    switch( aOperation ) {
    case EDeleteOp:
        emit q_ptr->songsDeleted( KErrNone == aErr );
        break;
    case EAddOp:
        emit q_ptr->playlistSaved( KErrNone == aErr );
        break;
    case ERenameOp:
        emit q_ptr->playlistsRenamed( KErrNone == aErr );
        break;
    default:
        break;
    }

    if ( aArgument ) {
        delete ( CBase* )aArgument;
    }
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::HandleIsolatedOpenL( const CMPXMedia& aEntries, TInt aError )
{
    TX_ENTRY_ARGS( "aError=" << aError );
    if ( aError == KErrNone ) {
        if ( iIsolatedCollectionData ) {
            delete iIsolatedCollectionData;
            iIsolatedCollectionData = 0;
        }  
        iIsolatedCollectionData = new MpMpxCollectionData();
        iIsolatedCollectionData->setMpxMedia( aEntries );
        emit q_ptr->isolatedCollectionOpened( iIsolatedCollectionData );
    }
    else {
        TX_LOG_ARGS( "Error: " << aError << "; should never get here." );
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::HandleIsolatedOpenRestorePathL( const CMPXCollectionPath& aPath, TInt aError )
{
    TX_ENTRY_ARGS( "aError=" << aError );
    if ( aError == KErrNone ) {
        CMPXCollectionPath* cpath = CMPXCollectionPath::NewL( aPath );
        CleanupStack::PushL( cpath );
        if ( cpath->Count() <= 0 ) {
            //There are no entries on the path
            if ( !iRestoreDefaultPath ) {
                //Try restoring default path if not already tried
                //This is a special case where a playlist's songs where on the MMC and it was removed
                iRestoreDefaultPath = ETrue;
                iRestorePathIndex = 0;
                CMPXCollectionPath* cpath = iCollectionUiHelper->MusicAllSongsPathL();
                CleanupStack::PushL( cpath );
                iIsolatedCollectionHelper->OpenCollectionL( *cpath, iRestorePathIndex, CMpMpxIsolatedCollectionHelper::RestorePathMode );
                CleanupStack::PopAndDestroy( cpath );
            }
            else {
                //emit signal to go back to collection view because there is no music
                emit q_ptr->restorePathFailed();
            }
        }
        else if ( iRestorePathIndex ) {
            //If RestorePathIndex equals zero there is no need to check with previous index
            //just go and create playlist below
            if ( iRestorePathIndex == cpath->IndexOfId( iRestorePathIndexId ) ) {
                //Song is in path and maintains previous position
                cpath->Set( iRestorePathIndex );
                CMPXCollectionPlaylist* playList = CMPXCollectionPlaylist::NewL( *cpath );
                CleanupStack::PushL( playList );
                createPlaybackUtilityL();
                iPlaybackUtility->InitL( *playList, EFalse );
                CleanupStack::PopAndDestroy( playList );
            }
            else { 
                //Re open path starting at the beginning of the collection
                iRestorePathIndex = 0;
                cpath->Back(); 
                iIsolatedCollectionHelper->OpenCollectionL( *cpath, iRestorePathIndex, CMpMpxIsolatedCollectionHelper::RestorePathMode );
            }
        }
        else {
            //We create a playlist right away since there is no need to check the index for the first element
            CMPXCollectionPlaylist* playList = CMPXCollectionPlaylist::NewL( *cpath );
            CleanupStack::PushL( playList );
            createPlaybackUtilityL();
            iPlaybackUtility->InitL( *playList, EFalse );
            CleanupStack::PopAndDestroy( playList );
        }
        CleanupStack::PopAndDestroy( cpath );
    }
    else if ( aError == KErrNotFound ) {
        //Path not found (e.g. MMC removed) try restoring default path
        iRestoreDefaultPath = ETrue;
        iRestorePathIndex = 0;
        CMPXCollectionPath* cpath = iCollectionUiHelper->MusicAllSongsPathL();
        CleanupStack::PushL( cpath );
        iIsolatedCollectionHelper->OpenCollectionL( *cpath, iRestorePathIndex, CMpMpxIsolatedCollectionHelper::RestorePathMode );
        CleanupStack::PopAndDestroy( cpath );
    }
    else {
        //Open path failed
        TX_LOG_ARGS( "Error: " << aError << "; should never get here." );
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::HandleFindAllL( const CMPXMedia& aResults, TBool aComplete, TInt aError )
{
    Q_UNUSED( aComplete );
    TX_ENTRY_ARGS( "aError=" << aError );

    if ( aError == KErrNone ) {
        iCollectionData->setAlbumContent( aResults );
    }
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoInitL()
{
    TX_ENTRY
    iCollectionUtility = MMPXCollectionUtility::NewL( this, iHostUid );
    iCollectionUiHelper = CMPXCollectionHelperFactory:: NewCollectionUiHelperL( iHostUid );
    iIncrementalOpenUtil = CMPXCollectionOpenUtility::NewL( this, iHostUid );
    iCollectionData = new MpMpxCollectionData();
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoOpenCollectionL( 
    TCollectionContext aContext )
{
    TX_ENTRY_ARGS( "aContext=" << aContext );

    switch ( aContext ) {
    case ECollectionContextAllSongs:
        {
        CMPXCollectionPath* cpath = iCollectionUiHelper->MusicAllSongsPathL();
        CleanupStack::PushL( cpath );
        iCollectionUtility->Collection().OpenL( *cpath );
        CleanupStack::PopAndDestroy( cpath );
        break;
        }
    case ECollectionContextArtists:
        {
        CMPXCollectionPath* cpath = iCollectionUiHelper->MusicMenuPathL();
        CleanupStack::PushL( cpath );
        cpath->AppendL( BrowseArtist );
        iCollectionUtility->Collection().OpenL( *cpath );
        CleanupStack::PopAndDestroy( cpath );
        break;
        }
    case ECollectionContextAlbums:
        {
        CMPXCollectionPath* cpath = iCollectionUiHelper->MusicMenuPathL();
        CleanupStack::PushL( cpath );
        cpath->AppendL( BrowseAlbum );
        iCollectionUtility->Collection().OpenL( *cpath );
        CleanupStack::PopAndDestroy( cpath );
        break;
        }
    case ECollectionContextPlaylists:
        {
        CMPXCollectionPath* cpath = iCollectionUiHelper->MusicPlaylistPathL();
        CleanupStack::PushL( cpath );
        iCollectionUtility->Collection().OpenL( *cpath );
        CleanupStack::PopAndDestroy( cpath );
        break;
        }
    case ECollectionContextAlbumsMediaWall:
        {
        CMPXCollectionPath* cpath = iCollectionUiHelper->MusicMenuPathL();
        CleanupStack::PushL( cpath );
        cpath->AppendL( BrowseAlbumMediaWall );
        iCollectionUtility->Collection().OpenL( *cpath );
        CleanupStack::PopAndDestroy( cpath );
        break;
        }
    default:
        TX_LOG_ARGS( "Error: Unexpected context; should never get here." );
        break;
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoOpenCollectionItemL( TInt aIndex )
    {
    TX_ENTRY_ARGS( "aIndex=" << aIndex );
    iCollectionUtility->Collection().OpenL( aIndex );
    TX_EXIT
    }

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoIncrementalOpenL()
{
    TX_ENTRY
    // Cancel any reads
    iIncrementalOpenUtil->Stop();

    // Start the read
    iFirstIncrementalOpen = ETrue;
    RArray<TMPXAttribute> attrs;
    CleanupClosePushL( attrs );
    TArray<TMPXAttribute> ary = attrs.Array();
    iIncrementalOpenUtil->SetDelay( KIncrementalDelay );
    iIncrementalOpenUtil->StartL( ary, KIncrementalFetchBlockSize,
            0, CMPXCollectionOpenUtility::EFetchDown );
    CleanupStack::PopAndDestroy( &attrs );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoReopenCollectionL()
{
    TX_ENTRY
    CMPXCollectionPath* cpath = iCollectionUtility->Collection().PathL();
    CleanupStack::PushL( cpath );
    cpath->Back();
    iCollectionUtility->Collection().OpenL( *cpath );
    CleanupStack::PopAndDestroy( cpath );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoBackL()
{
    TX_ENTRY
    iCollectionUtility->Collection().BackL();
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoFindPlaylistsL( QStringList &playlists )
{
    TX_ENTRY
    delete iUserPlaylists;
    iUserPlaylists = NULL;

    RArray<TMPXAttribute> attrs;
    CleanupClosePushL( attrs );
    attrs.AppendL( KMPXMediaGeneralId );
    attrs.AppendL( KMPXMediaGeneralTitle );

    CMPXMedia* criteria = CMPXMedia::NewL();
    CleanupStack::PushL( criteria );
    criteria->SetTObjectValueL<TMPXGeneralType>( 
        KMPXMediaGeneralType, EMPXGroup );
    criteria->SetTObjectValueL<TMPXGeneralCategory>( 
        KMPXMediaGeneralCategory, EMPXPlaylist );

    // Look up collection UID and set to criteria
    RArray<TUid> ary;
    CleanupClosePushL( ary );
    ary.AppendL( TUid::Uid( EMPXCollectionPluginMusic ) );
    TUid musicCollection = iCollectionUtility->CollectionIDL( ary.Array() );
    CleanupStack::PopAndDestroy( &ary );

    criteria->SetTObjectValueL<TUid>( KMPXMediaGeneralCollectionId, musicCollection );

    iUserPlaylists = iCollectionUtility->Collection().FindAllL( *criteria, attrs.Array() );
    CleanupStack::PopAndDestroy( criteria );
    CleanupStack::PopAndDestroy( &attrs );

    if ( iUserPlaylists ) {
        const CMPXMediaArray* mediaArray =
            iUserPlaylists->Value<CMPXMediaArray>( KMPXMediaArrayContents );
        User::LeaveIfNull( const_cast<CMPXMediaArray*>( mediaArray ) );
        TInt count = mediaArray->Count();
        for ( TInt i = 0; i < count; i++ ) {
            CMPXMedia* media( mediaArray->AtL( i ) );
            const TDesC& titleText = media->ValueText( KMPXMediaGeneralTitle );
            if ( titleText.Compare( KNullDesC ) != 0 ) {
            playlists += QString::fromUtf16( titleText.Ptr(), titleText.Length() );
            }
        }
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoCreatePlaylistL( QString &playlistName, QList<int> &selection, MpMpxCollectionData* collectionData )
{
    TX_ENTRY_ARGS( "playlistName=" << playlistName );
    CMPXMedia* tracks = CMPXMedia::NewL();
    CleanupStack::PushL( tracks );

    TPtrC ptr( reinterpret_cast<const TText*>( playlistName.constData() ) );
    tracks->SetTextValueL( KMPXMediaGeneralTitle, ptr );
    tracks->SetTextValueL( KMPXMediaGeneralUri, KPlaylistPath );
    //if collection data is not provided we use the browsing collection.
    PreparePlaylistMediaL( *tracks, selection, collectionData ? collectionData : iCollectionData );

    iCollectionUiHelper->IncAddL( *tracks, this, KMPXChunkSize );
    CleanupStack::PopAndDestroy( tracks );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoSaveToPlaylistL( TMPXItemId playlistId, QList<int> &selection, MpMpxCollectionData *collectionData )
{
    TX_ENTRY_ARGS( "playlistId=" << int( playlistId ) );
    
    CMPXMedia* tracks = CMPXMedia::NewL();
    CleanupStack::PushL( tracks );

    RArray<TUid> ary;
    CleanupClosePushL( ary );
    ary.AppendL( TUid::Uid( EMPXCollectionPluginMusic ) );
    TUid musicCollection = iCollectionUtility->CollectionIDL( ary.Array() );
    CleanupStack::PopAndDestroy( &ary );

    tracks->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, playlistId );
    tracks->SetTObjectValueL<TUid>( KMPXMediaGeneralCollectionId, musicCollection );
    PreparePlaylistMediaL( *tracks, selection, collectionData );

    iCollectionUiHelper->IncAddL( *tracks, this, KMPXChunkSize );
    CleanupStack::PopAndDestroy( tracks );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoSaveToPlaylistL( int playlistIndex, QList<int> &selection )
{
    TX_ENTRY_ARGS( "playlistIndex=" << playlistIndex );

    const CMPXMediaArray* mediaArray =  iUserPlaylists->Value<CMPXMediaArray>( KMPXMediaArrayContents );
    User::LeaveIfNull( const_cast<CMPXMediaArray*>( mediaArray ) );
    CMPXMedia* media( mediaArray->AtL( playlistIndex ) );
    TMPXItemId playlistId = media->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
    DoSaveToPlaylistL( playlistId, selection, iCollectionData );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoSaveToCurrentPlaylistL( QList<int> &selection, MpMpxCollectionData *collectionData )
{
    TX_ENTRY

    const CMPXMedia& container = iCollectionData->containerMedia();
    if ( container.ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType) != EMPXItem && 
        container.ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory) != EMPXPlaylist) {
        User::Leave( KErrArgument );
    }
    TMPXItemId playlistId( container.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
    DoSaveToPlaylistL( playlistId, selection, collectionData );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoDeleteSongsL( QList<int> &selection )
{
    int count = selection.count();
    TX_ENTRY_ARGS( "selection count=" << count );

    CMPXCollectionPath* path( iCollectionUtility->Collection().PathL() );
    CleanupStack::PushL( path );
    
    if (count > 1) {
        for ( TInt i = 0; i < count; i++ ){
            path->SelectL( selection.at( i ) );
        }
    }
    else {
        path->Set( selection.at(0) );
    }
        
    iCollectionUiHelper->DeleteL( *path, this );
    CleanupStack::PopAndDestroy( path );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoDeleteAlbumSongsL( QList<int> &selection )
{
    int count = selection.count();
    TX_ENTRY_ARGS( "selection count=" << count );

    CMPXCollectionPath* path( iCollectionUtility->Collection().PathL() );
    CleanupStack::PushL( path );
    MPX_DEBUG_PATH( *path );

    CMPXMediaArray *mediaArray;
    const CMPXMedia& container = iCollectionData->containerMedia();
    mediaArray = const_cast<CMPXMediaArray*>( container.Value<CMPXMediaArray>( KMPXMediaArrayContents ) );
    int currentAlbumIndex = iCollectionData->currentAlbumIndex();
    CMPXMedia* album( mediaArray->AtL( currentAlbumIndex ) );
    if ( album->IsSupported(KMPXMediaArrayContents) ) {
        const CMPXMediaArray* songs = album->Value<CMPXMediaArray>(KMPXMediaArrayContents);
        User::LeaveIfNull(const_cast<CMPXMediaArray*>(songs));
        int index = selection.at(0);
        CMPXMedia* song = songs->AtL(index);

        TMPXItemId id( song->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
        path->AppendL( id );   // Append the song ID to be deleted
        path->Set( 0 );        // Select 1st song

        iCollectionUiHelper->DeleteL( *path, this );
    }
    CleanupStack::PopAndDestroy( path );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoRenamePlaylistL( QString &newName, int index )
{
    TX_ENTRY
    CMPXMediaArray *mediaArray;  
    const CMPXMedia& container = iCollectionData->containerMedia();
    mediaArray = const_cast<CMPXMediaArray*>( container.Value<CMPXMediaArray>( KMPXMediaArrayContents ) );
    CMPXMedia* currentPlaylistMedia( mediaArray->AtL( index ) );
    TMPXItemId id( currentPlaylistMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
    DoRenamePlaylistL( id, newName );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoRenamePlaylistL( QString &newName )
{
    TX_ENTRY
    const CMPXMedia& container = iCollectionData->containerMedia();
    TMPXItemId id( container.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
    DoRenamePlaylistL( id, newName );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoRenamePlaylistL( TMPXItemId id, QString &newName )
 {
     CMPXMedia* media = CMPXMedia::NewL();
     CleanupStack::PushL( media );
     media->SetTObjectValueL<TMPXGeneralType>( 
     KMPXMediaGeneralType, EMPXItem );
     media->SetTObjectValueL<TMPXGeneralCategory>( 
     KMPXMediaGeneralCategory, EMPXPlaylist );
     media->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, id );
     TPtrC ptr( reinterpret_cast<const TText*>( newName.constData() ) );
     media->SetTextValueL( KMPXMediaGeneralTitle, ptr );
     iCollectionUiHelper->RenameL( *media, this );
     CleanupStack::PopAndDestroy( media );
 }     

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoSetShuffleL( bool active )
{
    TX_ENTRY
    if ( iPlaybackUtility ) {
        iPlaybackUtility->SetL( EPbPropertyRandomMode, active );
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoPreviewSongL( int index )
{
    TX_ENTRY
    
    // Get the current path
    CMPXCollectionPath* cpath = iCollectionUtility->Collection().PathL();
    CleanupStack::PushL( cpath );
    MPX_DEBUG_PATH( *cpath );
    cpath->Back();

    TMPXItemId id( iCollectionData->itemId(index) );
    cpath->AppendL( id ); // Top level items of songs
    cpath->Set( 0 ); // Select 1st song

    CMPXCollectionPlaylist* playList = CMPXCollectionPlaylist::NewL( *cpath );
    CleanupStack::PushL( playList );
    playList->SetToFirst();
    //TODO: all calls to playback utility should be done via the engine and trough the playback FW wrapper.
    createPlaybackUtilityL();
    iPlaybackUtility->InitL( *playList, ETrue );
    emit q_ptr->collectionPlaylistOpened();

    CleanupStack::PopAndDestroy( playList );
    CleanupStack::PopAndDestroy( cpath );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoPreviewAlbumSongL( int index )
{
    TX_ENTRY
    //TODO: all calls to playback utility should be done via the engine and trough the playback FW wrapper.
    if ( !iPlaybackUtility ) {
        iPlaybackUtility = MMPXPlaybackUtility::UtilityL( iHostUid );
    }

    // Get the current path
    CMPXCollectionPath* cpath = iCollectionUtility->Collection().PathL();
    CleanupStack::PushL( cpath );
    MPX_DEBUG_PATH( *cpath );

    CMPXMediaArray *mediaArray;
    const CMPXMedia& container = iCollectionData->containerMedia();
    mediaArray = const_cast<CMPXMediaArray*>( container.Value<CMPXMediaArray>( KMPXMediaArrayContents ) );
    int currentAlbumIndex = iCollectionData->currentAlbumIndex();
    CMPXMedia* album( mediaArray->AtL( currentAlbumIndex ) );
    if ( album->IsSupported(KMPXMediaArrayContents) ) {
        const CMPXMediaArray* songs = album->Value<CMPXMediaArray>(KMPXMediaArrayContents);
        User::LeaveIfNull(const_cast<CMPXMediaArray*>(songs));
        CMPXMedia* song = songs->AtL(index);

        TMPXItemId id( song->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
        cpath->AppendL( id );   // Top level items of songs
        cpath->Set( 0 );        // Select 1st song

        CMPXCollectionPlaylist* playList = CMPXCollectionPlaylist::NewL( *cpath );
        CleanupStack::PushL( playList );
        playList->SetToFirst();
        //TODO: all calls to playback utility should be done via the engine and trough the playback FW wrapper.
        iPlaybackUtility->InitL( *playList, ETrue );
        emit q_ptr->collectionPlaylistOpened();

        CleanupStack::PopAndDestroy( playList );
    }
    CleanupStack::PopAndDestroy( cpath );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoOpenIsolatedCollectionL( TCollectionContext context )
{

    if ( ECollectionContextAllSongs == context ) {
        CMPXCollectionPath* cpath = iCollectionUiHelper->MusicAllSongsPathL();
        CleanupStack::PushL( cpath );
        if ( !iIsolatedCollectionHelper ) {
            iIsolatedCollectionHelper = CMpMpxIsolatedCollectionHelper::NewL( this );
        }   
        iIsolatedCollectionHelper->OpenCollectionL( *cpath );
        CleanupStack::PopAndDestroy( cpath );
    }
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoReorderPlaylistL( int playlistId, int songId, int originalOrdinal, int newOrdinal )
{
    iCollectionUiHelper->ReorderPlaylistL( playlistId, songId, originalOrdinal, newOrdinal, this );
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoFindAlbumSongsL( int index )
{
    const CMPXMedia& container = iCollectionData->containerMedia();
    const CMPXMediaArray* mediaArray = container.Value<CMPXMediaArray>( KMPXMediaArrayContents );
    CMPXMedia* album( mediaArray->AtL( index ) );

	// Obtain the artistId from the container
    TMPXItemId artistId = container.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
	
    // Fetch the songs for the selected album and the artist
	// Specifying artistId is necessary to search for songs in the artist’s unknown album.
    TMPXItemId albumId = album->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
    CMPXMedia* findCriteria = CMPXMedia::NewL();
    CleanupStack::PushL( findCriteria );
    findCriteria->SetTObjectValueL<TMPXGeneralType>( KMPXMediaGeneralType, EMPXGroup );
    findCriteria->SetTObjectValueL<TMPXGeneralCategory>( KMPXMediaGeneralCategory, EMPXSong );
    findCriteria->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, albumId );
    findCriteria->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralContainerId, artistId );
    RArray<TMPXAttribute> attrs;
    CleanupClosePushL( attrs );
    attrs.AppendL( TMPXAttribute( KMPXMediaIdGeneral,
                                 EMPXMediaGeneralTitle |
								 EMPXMediaGeneralUri |
                                 EMPXMediaGeneralId |
                                 EMPXMediaGeneralType |
                                 EMPXMediaGeneralCategory |
                                 EMPXMediaGeneralFlags ) );
    attrs.AppendL( KMPXMediaMusicAlbumTrack );

    iCollectionUtility->Collection().FindAllL( *findCriteria, attrs.Array(), *this );
    CleanupStack::PopAndDestroy( &attrs );
    CleanupStack::PopAndDestroy( findCriteria );
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoPlayAlbumSongsL( int albumIndex, int songIndex, MpMpxCollectionData* collectionData  )
{
    TX_ENTRY

    RArray<TMPXItemId> ids;
    CleanupClosePushL(ids);

    CMPXCollectionPath* cpath;
    
    //Following check is for a use case for media wall, where it is required 
    //to play songs that are browsed by an isolated collection.
    if ( collectionData->context() ==  ECollectionContextAlbumsMediaWall ) {
        //Get the Media Wall path.
        cpath = iCollectionUiHelper->MusicMenuPathL();
        CleanupStack::PushL( cpath );
        //TODO: change to BrowseAlbumMediaWall befor when merging with olveras changes.
        cpath->AppendL( BrowseAlbum );
        const TMPXItemId albumId = collectionData->itemId( albumIndex );
        //Append the selected album.
        RArray<TMPXItemId> items;
        CleanupClosePushL(items);
        items.AppendL(albumId);
        cpath->AppendL(items.Array());
        CleanupStack::PopAndDestroy( &items );
        cpath->Set( 0 ); // Set the album.
        
    }
    else {
        // Get the current path
        cpath = iCollectionUtility->Collection().PathL();
        CleanupStack::PushL( cpath );
        cpath->Set(albumIndex); // Set the selected album
    }
    MPX_DEBUG_PATH( *cpath );

    CMPXMediaArray *mediaArray;
    const CMPXMedia& container = collectionData->containerMedia();
    mediaArray = const_cast<CMPXMediaArray*>( container.Value<CMPXMediaArray>( KMPXMediaArrayContents ) );
    CMPXMedia* album( mediaArray->AtL( albumIndex ) );
    const CMPXMediaArray* songs = album->Value<CMPXMediaArray>(KMPXMediaArrayContents);
    User::LeaveIfNull(const_cast<CMPXMediaArray*>(songs));

    TInt count = songs->Count();
    for ( TInt i = 0; i < count; ++i ) {
        CMPXMedia* song = songs->AtL(i);
        const TMPXItemId id = song->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
        ids.AppendL(id);
    }

    cpath->AppendL(ids.Array());    // Top level items of songs
    cpath->Set(songIndex);          // Set the selected song
    MPX_DEBUG_PATH(*cpath);

    CMPXCollectionPlaylist* playList = CMPXCollectionPlaylist::NewL( *cpath );
    CleanupStack::PushL( playList );
    
    //TODO: all calls to playback utility should be done via the engine and trough the playback FW wrapper.
    createPlaybackUtilityL();
    iPlaybackUtility->InitL( *playList, ETrue );
    emit q_ptr->collectionPlaylistOpened();

    CleanupStack::PopAndDestroy( playList );
    CleanupStack::PopAndDestroy( cpath );
    CleanupStack::PopAndDestroy( &ids );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::setRepeatFeatureEnabled( bool enable ) 
{
    iRepeatFeature = enable;
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::setShuffleFeatureEnabled( bool enable )
{
    iShuffleFeature = enable;
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoHandleCollectionMessageL( 
    const CMPXMessage& aMsg )
{
    TX_ENTRY
    TMPXMessageId id( aMsg.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id ) {
        TInt event( aMsg.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) );
        TInt type( aMsg.ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
        TInt data( aMsg.ValueTObjectL<TInt>( KMPXMessageGeneralData ) );
        TX_LOG_ARGS( "event=" << event << ", type=" << type << ", data=" << data );

        if ( event == TMPXCollectionMessage::EPathChanged &&
             type == EMcPathChangedByOpen &&
             data == EMcContainerOpened ) {
            // Incremental Open when browsing to the next level
            DoIncrementalOpenL();
        }
        else if ( event == TMPXCollectionMessage::EPathChanged &&
                  type == EMcPathChangedByOpen &&
                  data == EMcItemOpened ) {
            // Opened a song
            // This will result in HandleOpenL with CMPXCollectionPlaylist
            iCollectionUtility->Collection().OpenL();
        }
    }
    else if( id == KMPXMessageIdItemChanged ) {
        TInt eventType( aMsg.ValueTObjectL<TMPXChangeEventType>( KMPXMessageChangeEventType ) );

        if ( eventType == EMPXItemDeleted || eventType == EMPXItemInserted ) {
            emit q_ptr->containerContentsChanged();
        }
    }
    TX_EXIT
}


/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::PreparePlaylistMediaL( 
        CMPXMedia& aMedia,
        QList<int> &selection,
        MpMpxCollectionData *collectionData )
{
    int count = selection.count();
    TX_ENTRY_ARGS( "selection count=" << count );

    const CMPXMediaArray* mediaArray = 0;
    
    if ( ( collectionData->context() == ECollectionContextArtistAlbumsTBone ) || 
         ( collectionData->context() == ECollectionContextAlbumsTBone ) ) {
        //There is no need to find the track information, for TBone we cash it under the media itself.
        //Fetching song data from container media.
        CMPXMedia* album;
        const CMPXMediaArray* albumsArray;
        albumsArray = collectionData->containerMedia().Value<CMPXMediaArray>( KMPXMediaArrayContents );
        int index = collectionData->currentAlbumIndex();
        if ( index >= 0 ) {
            album = albumsArray->AtL( collectionData->currentAlbumIndex() );
            mediaArray = album->Value<CMPXMediaArray>( KMPXMediaArrayContents );
        }
    }
    else {
        const CMPXMedia& container = collectionData->containerMedia();
        mediaArray = container.Value<CMPXMediaArray>( KMPXMediaArrayContents );
    }
       
    User::LeaveIfNull( const_cast<CMPXMediaArray*>( mediaArray ) );
    CMPXMediaArray* tracksArray( CMPXMediaArray::NewL() );
    CleanupStack::PushL( tracksArray );

    CMPXCollectionPath* path( iCollectionUtility->Collection().PathL() );
    CleanupStack::PushL( path );
    TMPXItemId collectionId( path->Id( 0 ) );
    CleanupStack::PopAndDestroy( path );

    if ( collectionData->context() == ECollectionContextAlbums || 
         collectionData->context() == ECollectionContextArtistAlbums ||
         collectionData->context() == ECollectionContextArtists ) {
        //Container, it is possible to append multiple containers to a playlist. 
        //Currently we are just using single selection from the ui.
        for ( TInt i = 0; i < count; i++ ) {
            CMPXMedia* results;
            CMPXMedia* container( mediaArray->AtL( selection[i] ) );
            // Fetch the songs for the selected container
            TMPXItemId containerId = container->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
            CMPXMedia* findCriteria = CMPXMedia::NewL();
            CleanupStack::PushL( findCriteria );
            findCriteria->SetTObjectValueL<TMPXGeneralType>( KMPXMediaGeneralType, EMPXGroup );
            findCriteria->SetTObjectValueL<TMPXGeneralCategory>( KMPXMediaGeneralCategory, EMPXSong );
            findCriteria->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, containerId );
            RArray<TMPXAttribute> attrs;
            CleanupClosePushL( attrs );
            attrs.AppendL( TMPXAttribute( KMPXMediaIdGeneral,
                                         EMPXMediaGeneralTitle |
                                         EMPXMediaGeneralId ) );
            results = iCollectionUtility->Collection().FindAllL( *findCriteria, attrs.Array() );
            CleanupStack::PopAndDestroy( &attrs );
            CleanupStack::PopAndDestroy( findCriteria );
            CleanupStack::PushL( results );
            
            const CMPXMediaArray* resultsArray = results->Value<CMPXMediaArray>( KMPXMediaArrayContents );
            User::LeaveIfNull( resultsArray );
            for ( int j = 0 ; j < resultsArray->Count();j++ ) {
                CMPXMedia* media( resultsArray->AtL( j ) );
                CMPXMedia* entry = CMPXMedia::NewL();
                CleanupStack::PushL( entry );
                entry->SetTextValueL( KMPXMediaGeneralTitle,
                    media->ValueText( KMPXMediaGeneralTitle ) );
                entry->SetTObjectValueL( KMPXMediaGeneralType, EMPXItem );
                entry->SetTObjectValueL( KMPXMediaGeneralCategory, EMPXSong );
                entry->SetTObjectValueL( KMPXMediaGeneralId,
                    media->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
                entry->SetTObjectValueL( KMPXMediaGeneralCollectionId, collectionId );
                CleanupStack::Pop( entry );
                tracksArray->AppendL( entry );
            }
            CleanupStack::PopAndDestroy( results );
        }
        
    }
    else {
        //Tracks
        for ( TInt i = 0; i < count; i++ ) {
            CMPXMedia* media( mediaArray->AtL( selection.at( i ) ) );
            CMPXMedia* entry = CMPXMedia::NewL();
            CleanupStack::PushL( entry );
            entry->SetTextValueL( KMPXMediaGeneralTitle,
                media->ValueText( KMPXMediaGeneralTitle ) );
            entry->SetTObjectValueL( KMPXMediaGeneralType, EMPXItem );
            entry->SetTObjectValueL( KMPXMediaGeneralCategory, EMPXSong );
            entry->SetTObjectValueL( KMPXMediaGeneralId,
                media->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
            entry->SetTObjectValueL( KMPXMediaGeneralCollectionId, collectionId );
            CleanupStack::Pop( entry );
            tracksArray->AppendL( entry );
        }
    }
    aMedia.SetTObjectValueL<TMPXGeneralType>( KMPXMediaGeneralType, EMPXItem );
    aMedia.SetTObjectValueL<TMPXGeneralCategory>( KMPXMediaGeneralCategory, EMPXPlaylist );
    aMedia.SetCObjectValueL( KMPXMediaArrayContents, tracksArray );
    aMedia.SetTObjectValueL( KMPXMediaArrayCount, tracksArray->Count() );

    emit q_ptr->aboutToAddSongs( tracksArray->Count() );

    CleanupStack::PopAndDestroy( tracksArray );
    TX_EXIT
}

/*!
 \internal
 TODO: all calls to playback utility should be done via the engine and trough the playback FW wrapper.
 */
void MpMpxCollectionFrameworkWrapperPrivate::createPlaybackUtilityL() 
{
    if ( !iPlaybackUtility ) {

        iPlaybackUtility = MMPXPlaybackUtility::UtilityL( iHostUid );

        if ( iShuffleFeature ) {
            iPlaybackUtility->SetL( EPbPropertyRandomMode, MpSettingsManager::shuffle() ? ETrue : EFalse );
        }
        if ( iRepeatFeature ) {
            iPlaybackUtility->SetL( EPbPropertyRepeatMode, MpSettingsManager::repeat() ? EPbRepeatAll : EPbRepeatOff );
        }
    }
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoPlayAllSongsPlaylistL()
{
    TX_ENTRY
    CMPXCollectionPath* cpath = iCollectionUtility->Collection().PathL();
    CleanupStack::PushL( cpath );
    if ( cpath->Count() > 0 ) {
        CMPXCollectionPlaylist* playList = CMPXCollectionPlaylist::NewL( *cpath );
        CleanupStack::PushL( playList );
        playList->SetShuffleL( true, false );
        MpSettingsManager::setShuffle( true );
        createPlaybackUtilityL();
        iPlaybackUtility->InitL( *playList, ETrue );
        CleanupStack::PopAndDestroy( playList );
    }
    else {
        //There were no songs to be played so switch back to collection view
        emit q_ptr->restorePathFailed();
    }
    CleanupStack::PopAndDestroy( cpath );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoRetrieveSongDetailsL( int index )
{
    TX_ENTRY
    RArray<TMPXAttribute> requestedAttr;
    CleanupClosePushL( requestedAttr );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaGeneralTitle ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicArtist ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicAlbum ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaGeneralUri ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicAlbumArtFileName ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaGeneralMimeType ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicAlbumTrack ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicComposer ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicYear ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicGenre ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaGeneralDuration ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaAudioBitrate ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaAudioSamplerate ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaGeneralCopyright ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicURL ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaDrmProtected ) );

    CMPXCollectionPath* cpath = iCollectionUtility->Collection().PathL();
    CleanupStack::PushL( cpath );
    TCollectionContext context = iCollectionData->context();
    if ( context == ECollectionContextArtistAlbumsTBone
         || context == ECollectionContextAlbumsTBone ) {
        CMPXMediaArray *mediaArray;
        const CMPXMedia& container = iCollectionData->containerMedia();
        mediaArray = const_cast<CMPXMediaArray*>( container.Value<CMPXMediaArray>( KMPXMediaArrayContents ) );
        int currentAlbumIndex = iCollectionData->currentAlbumIndex();
        CMPXMedia* album( mediaArray->AtL( currentAlbumIndex ) );
        if ( album->IsSupported(KMPXMediaArrayContents) ) {
            const CMPXMediaArray* songs = album->Value<CMPXMediaArray>(KMPXMediaArrayContents);
            User::LeaveIfNull(const_cast<CMPXMediaArray*>(songs));
            CMPXMedia* song = songs->AtL(index);
            TMPXItemId id( song->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
            cpath->AppendL( id );   // Top level items of songs
            cpath->Set( 0 );        // Select 1st song
        }
    }
    else {
        cpath->Set( index );
    }
    iCollectionUtility->Collection().MediaL( *cpath, requestedAttr.Array() );
    CleanupStack::PopAndDestroy( cpath );
    CleanupStack::PopAndDestroy( &requestedAttr );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoSavePathL( QByteArray &data )
{
    TX_ENTRY
    CBufFlat* buffer = CBufFlat::NewL( 256 );
    CleanupStack::PushL( buffer );
    TBufBuf bufBuf;
    bufBuf.Set( *buffer, 0, TBufBuf::EWrite );
    
    RWriteStream writeStream( &bufBuf );
    writeStream.PushL();
    
    if ( iPlaybackUtility ) {
        MMPXSource* source = iPlaybackUtility->Source();
        CMPXCollectionPlaylist* playList( NULL );
        if( source ) {
        playList = source->PlaylistL();
            if ( playList ) {
                CleanupStack::PushL( playList );
                const CMPXCollectionPath& cpath = playList->Path();
                writeStream << cpath;
                writeStream.CommitL();
                CleanupStack::PopAndDestroy( playList );
            }
        }
    }
    data.append(reinterpret_cast<const char*>(buffer->Ptr(0).Ptr()) ,buffer->Ptr(0).Length());
    CleanupStack::PopAndDestroy( 2, buffer ); //writeStream and buffer
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoRestorePathL( const QByteArray &data )
{
    TX_ENTRY
    int dataSize = data.size();
    if ( dataSize > 0 ) {
        TPtrC8 activityDataDescriptor( reinterpret_cast<const unsigned char*> ( data.constData() ), data.size() );
         
        //Take a copy of the data
        CBufFlat* buffer = CBufFlat::NewL( dataSize );
        CleanupStack::PushL( buffer );
        buffer->InsertL( 0, activityDataDescriptor, dataSize );
        
        TBufBuf bufBuf;
        bufBuf.Set( *buffer, 0, TBufBuf::ERead );
        RReadStream readStream( &bufBuf );
        readStream.PushL();
        
        CMPXCollectionPath* cpath( NULL );
        cpath = CMPXCollectionPath::NewL(readStream);
        CleanupStack::PushL(cpath);
        iRestorePathIndex = 0;
        if( cpath->Levels() ) {
            iRestorePathIndexId = cpath->Id();
            iRestorePathIndex = cpath->Index();
            cpath->Back();
        }
        if ( !iIsolatedCollectionHelper ) {
            iIsolatedCollectionHelper = CMpMpxIsolatedCollectionHelper::NewL( this );
        }   
        iIsolatedCollectionHelper->OpenCollectionL( *cpath, iRestorePathIndex, CMpMpxIsolatedCollectionHelper::RestorePathMode );
          
        CleanupStack::PopAndDestroy( cpath );
        CleanupStack::PopAndDestroy( 2, buffer ); //readStream and buffer
    }
    TX_EXIT
}

//EOF
