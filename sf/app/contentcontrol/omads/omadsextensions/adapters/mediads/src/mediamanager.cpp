/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CMediaManager class implementation
*
*/



#include "mediamanager.h"

#include <mpxmedia.h>
#include <mpxmediageneraldefs.h>
#include <mpxcollectionhelper.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediaarray.h>
#include <mpxcollectionhelperfactory.h>
#include <mpxcollectionhelper.h>
#include <utf.h>
#include <mpxplaylistenginedefs.h> // for EMPXPlaylistTypeM3U
#include <mpxcollectiontype.h>
#include <mpxmediamusicdefs.h>
#include <mpxcollectionplugin.hrh>
#include <PathInfo.h>
#include <mpxmediaaudiodefs.h>
#include <mpxmediamtpdefs.h>
#include <driveinfo.h>

#include "mediadsproviderdefs.h"
#include "playlistitem.h"
#include "logger.h"


const TUid KMpxLocalCollectionUid = { 0x101FFC3A };

_LIT( KPlaylistPath, "playlists\\" );
_LIT( KMPXVirtualPlaylistExt, ".vir" );

// Maximum amount of song items to be enumerated on one RunL.
const TInt iMaxSongItemsToEnumerate(100);
// Maximum amount of playlist items to be enumerated on one RunL.
const TInt iMaxPlaylistItemsToEnumerate(15);


CMediaManager::CMediaManager( RFs& aFs, MMediaManagerObserver* aObserver,
        TKeyArrayFix aKey, CMD5& aHasher  )
: CActive( EPriorityStandard ),
iObserver( aObserver ),
iKey( aKey ),
iHasher( aHasher ),
iFs( aFs )
    {
    TRACE_FUNC;
    CActiveScheduler::Add(this);
    }


void CMediaManager::ConstructL()
    {
    TRACE_FUNC_ENTRY;
    iHarvester = CMPXHarvesterFactory::NewL();
    LOGGER_WRITE("1");
    iCollection = MMPXCollectionUtility::NewL( iObserver );
    LOGGER_WRITE("2");
    iMediator = CMPXCollectionMediator::NewL( iCollection->Collection() );
    LOGGER_WRITE("3");
    iCollectionHelper = CMPXCollectionHelperFactory::NewCollectionCachedHelperL();   
    
    TRACE_FUNC_EXIT;
    }


CMediaManager* CMediaManager::NewL( RFs& aFs, MMediaManagerObserver* aObserver, TKeyArrayFix aKey, CMD5& aHasher )
    {
    CMediaManager* self = new( ELeave ) CMediaManager( aFs, aObserver, aKey, aHasher );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMediaManager::~CMediaManager()
    {
    TRACE_FUNC_ENTRY;
    Cancel();
    delete iFoundedMedia;
    if ( iCollectionHelper )
        {
        iCollectionHelper->Close();
        iCollectionHelper = NULL;
        }
    
    delete iMediator;
    
    if ( iCollection )
        {
        iCollection->Close();
        iCollection = NULL;
        }
    
    if ( iHarvester )
        {
        // For some unknown reason this may sometimes leave with error code -38
        TRAP_IGNORE( iHarvester->Close() );
        iHarvester = NULL;
        }
    
    TRACE_FUNC_EXIT;
    }

void CMediaManager::ScanL()
    {
    TRACE_FUNC_ENTRY;
    iHarvester->ScanL();
    TRACE_FUNC_EXIT;
    }

void CMediaManager::CancelScanL()
    {
    TRACE_FUNC_ENTRY;
    iHarvester->CancelScanL();
    TRACE_FUNC_EXIT;
    }

// --------------------------------------------------------------------------
// Starts registering all playlists. Calls RegisterAllPlayListsCompleted when ready
// --------------------------------------------------------------------------
// 
TInt CMediaManager::RegisterAllPlayLists()
    {
    TRACE_FUNC_ENTRY;
    if ( iCurrentState != EIdle || !iSnapshotArray)
        {
        LOGGER_WRITE("Not Idle or iSnapshotArray is NULL");
        return KErrNotReady;
        }
    iCurrentState = EReadingPlaylistCount;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    TRACE_FUNC_EXIT;
    return KErrNone;
    }

// --------------------------------------------------------------------------
// Starts registering all song items. Calls RegisterAllSongsCompleted when ready
// --------------------------------------------------------------------------
// 
TInt CMediaManager::RegisterAllSongs()
    {
    TRACE_FUNC_ENTRY;
    if ( iCurrentState != EIdle || !iSnapshotArray)
        {
        LOGGER_WRITE("Not Idle or iSnapshotArray is NULL");
        return KErrNotReady;
        }
    iCurrentState = EReadingSongCount;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    TRACE_FUNC_EXIT;
    return KErrNone;
    }

// --------------------------------------------------------------------------
// Registers playlist.
// --------------------------------------------------------------------------
// 
void CMediaManager::RegisterPlaylistL( const CMPXMedia& aMedia )
    {
    TRACE_FUNC_ENTRY;
    if ( !iSnapshotArray )
        {
        User::Leave( KErrGeneral );
        }
    
    if ( !aMedia.IsSupported(KMPXMediaGeneralId) )
        {
        LOGGER_WRITE("KMPXMediaGeneralId not supported");
        User::Leave(KErrNotSupported);
        }
     
     // Extract the playlist id from the found object
     TUint32 playlistId = *aMedia.Value<TMPXItemId>(KMPXMediaGeneralId);
     LOGGER_WRITE_1("KMPXMediaGeneralId: %d", playlistId);
     
     if ( !aMedia.IsSupported( KMPXMediaGeneralTitle ) )
          {
          LOGGER_WRITE("KMPXMediaGeneralTitle not supported");
          User::Leave(KErrNotSupported);
          }
     
     CPlaylistItem* playlist = CPlaylistItem::NewLC();
     // find the media object that contains a list of songs in the playlist
     
     RArray<TInt> contentIDs;
     CleanupClosePushL( contentIDs );
     contentIDs.AppendL(KMPXMediaIdGeneral);
     
     CMPXMedia* searchMedia = CMPXMedia::NewL(contentIDs.Array());
     CleanupStack::PopAndDestroy( &contentIDs );
     CleanupStack::PushL(searchMedia);
     
     searchMedia->SetTObjectValueL(KMPXMediaGeneralType, EMPXGroup);    
     searchMedia->SetTObjectValueL(KMPXMediaGeneralCategory, EMPXSong);
     searchMedia->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, playlistId);
     
     RArray<TMPXAttribute> songAttributes;
     CleanupClosePushL(songAttributes);
     songAttributes.AppendL(KMPXMediaGeneralId);
     songAttributes.AppendL(KMPXMediaGeneralUri);
     
     CMPXMedia* foundMedia = iCollectionHelper->FindAllL(*searchMedia, 
                                                         songAttributes.Array());
     CleanupStack::PopAndDestroy(&songAttributes);
     CleanupStack::PopAndDestroy(searchMedia);
     
     CleanupStack::PushL(foundMedia);
     
     
     if ( !foundMedia->IsSupported(KMPXMediaArrayCount) )
         {
         User::Leave(KErrNotSupported);
         }
     
     // Number of references
     TInt numOfRefs = *foundMedia->Value<TInt>(KMPXMediaArrayCount);
     if ( numOfRefs )
         {
         if (!foundMedia->IsSupported(KMPXMediaArrayContents))
             {
             User::Leave(KErrNotSupported);
             }
         
         const CMPXMediaArray* refArray =
             foundMedia->Value<CMPXMediaArray>(KMPXMediaArrayContents);
         
         // Import the references
         for (TInt i=0;i<numOfRefs;i++)
             {
             const CMPXMedia* refMedia = (*refArray)[i];
             
             if ( refMedia->IsSupported(KMPXMediaGeneralUri) )
                 {
                 const TDesC& uri = refMedia->ValueText(KMPXMediaGeneralUri);
                 playlist->AddItemL( uri );
                 }
             
             }
         }
    
    TSnapshotItem playlistItem(playlistId);
    
    const TDesC& title = aMedia.ValueText( KMPXMediaGeneralTitle );
    playlist->SetTitleL( title );
    
    const TDesC& uri = aMedia.ValueText( KMPXMediaGeneralUri );
    playlist->SetUrlL( uri );
    
    playlistItem.SetItemHash( *playlist, iHasher );
    
    playlistItem.SetParentId( KPlaylistsFolderId );
    TKeyArrayFix key( iKey );
    iSnapshotArray->InsertIsqL( playlistItem, key );
    
    CleanupStack::PopAndDestroy( foundMedia );
    CleanupStack::PopAndDestroy( playlist );
    
    TRACE_FUNC_EXIT;
    }

// --------------------------------------------------------------------------
// Populates CPlaylistItem
// --------------------------------------------------------------------------
// 
void CMediaManager::GetPlayListL( TInt aPlaylistId, CPlaylistItem& aList )
    {
    TRACE_FUNC_ENTRY;
    
    LOGGER_WRITE_1("aPlaylistId: %d", aPlaylistId);
    
    CMPXMedia* playlistItem = FindPlaylistL( aPlaylistId );
    CleanupStack::PushL( playlistItem );
    if ( !playlistItem->IsSupported(KMPXMediaGeneralId) )
        {
        LOGGER_WRITE("KMPXMediaGeneralId not supported");
        User::Leave(KErrNotSupported);
        }
    
    // Extract the playlist id from the found object
    TUint32 playlistId = *playlistItem->Value<TMPXItemId>(KMPXMediaGeneralId);
    LOGGER_WRITE_1("KMPXMediaGeneralId: %d", playlistId);
    aList.SetId( playlistId );
    
    if ( !playlistItem->IsSupported(KMPXMediaGeneralTitle) )
        {
        LOGGER_WRITE("KMPXMediaGeneralTitle not supported");
        User::Leave( KErrNotSupported );
        }
    else
        {
        const TDesC& title = playlistItem->ValueText(KMPXMediaGeneralTitle);
        LOGGER_WRITE_1("KMPXMediaGeneralTitle: %S", &title);
        aList.SetTitleL( title );
        }
    
    if ( playlistItem->IsSupported( KMPXMediaGeneralUri ) )
        {
        const TDesC& uri = playlistItem->ValueText( KMPXMediaGeneralUri );
        aList.SetUrlL( uri );
        }
    
    CleanupStack::PopAndDestroy( playlistItem );
    
    CMPXMedia* playlist = FetchPlaylistContentL( aPlaylistId );
    CleanupStack::PushL( playlist );
    
    if ( !playlist->IsSupported(KMPXMediaArrayCount) )
         {
         User::Leave(KErrNotSupported);
         }
     
    // Number of references
    TInt numOfRefs = *playlist->Value<TInt>(KMPXMediaArrayCount);
    LOGGER_WRITE_1("numOfRefs: %d", numOfRefs);
    if ( numOfRefs )
        {
        if (!playlist->IsSupported(KMPXMediaArrayContents))
            {
            User::Leave(KErrNotSupported);
            }
        
        const CMPXMediaArray* refArray =
        playlist->Value<CMPXMediaArray>(KMPXMediaArrayContents);
        
        // Import the references
        for (TInt i=0;i<numOfRefs;i++)
            {
            const CMPXMedia* refMedia = (*refArray)[i];
            
            if ( refMedia->IsSupported(KMPXMediaGeneralUri) )
                {
                aList.AddItemL( refMedia->ValueText(KMPXMediaGeneralUri) );
                }
            }
        }
 
    CleanupStack::PopAndDestroy( playlist );
    
    TRACE_FUNC_EXIT;
    }

// --------------------------------------------------------------------------
// Fetch a playlist content
// --------------------------------------------------------------------------
// 
CMPXMedia* CMediaManager::FetchPlaylistContentL( TInt aId )
    {
    TRACE_FUNC_ENTRY;
    TMPXItemId id(aId);

    CMPXMedia* criteria = CMPXMedia::NewL();
    CleanupStack::PushL( criteria );
    criteria->SetTObjectValueL<TUid>(
            KMPXMediaGeneralCollectionId, KMpxLocalCollectionUid );
    criteria->SetTObjectValueL<TMPXGeneralType>(
            KMPXMediaGeneralType, EMPXGroup );
    
    // set search keys
    criteria->SetTObjectValueL<TMPXGeneralCategory>(
            KMPXMediaGeneralCategory, EMPXSong );    
    criteria->SetTObjectValueL<TMPXItemId>(
            KMPXMediaGeneralId, id );
    
    // define attributes fetched
    RArray<TMPXAttribute> songAttributes;
    CleanupClosePushL(songAttributes);
    songAttributes.AppendL(KMPXMediaGeneralId);
    songAttributes.AppendL(KMPXMediaGeneralUri);
    
    // now find
    CMPXMedia* result = iCollectionHelper->FindAllL( *criteria , songAttributes.Array() );
    CleanupStack::PopAndDestroy( &songAttributes );
    CleanupStack::PopAndDestroy( criteria );
    
    TRACE_FUNC_EXIT;
    return result;
    }


 // --------------------------------------------------------------------------
 // Find a playlist
 // --------------------------------------------------------------------------
 //
 CMPXMedia* CMediaManager::FindPlaylistL( TInt aUid )
     {
     TRACE_FUNC_ENTRY;
     CMPXMedia* criteria = CMPXMedia::NewL();
     CleanupStack::PushL( criteria );
     
     criteria->SetTObjectValueL<TMPXGeneralType>(
                 KMPXMediaGeneralType, EMPXItem );
     criteria->SetTObjectValueL<TMPXGeneralCategory>(
                 KMPXMediaGeneralCategory, EMPXPlaylist );
     criteria->SetTObjectValueL<TMPXItemId>(
                 KMPXMediaGeneralId, aUid );
         
     // define attributes fetched
     RArray<TMPXAttribute> playlistAttributes;
     CleanupClosePushL(playlistAttributes);
     playlistAttributes.AppendL(KMPXMediaGeneralId);
     playlistAttributes.AppendL(KMPXMediaGeneralCollectionId); //test
     playlistAttributes.AppendL(KMPXMediaGeneralTitle);
     playlistAttributes.AppendL(KMPXMediaGeneralUri);
     
     // now find
     CMPXMedia* foundList = 0;
     foundList = iCollectionHelper->FindAllL( *criteria , playlistAttributes.Array() );
     CleanupStack::PopAndDestroy( &playlistAttributes );
     CleanupStack::PopAndDestroy( criteria );
     
     CleanupStack::PushL( foundList );
     
     if ( !foundList->IsSupported(KMPXMediaArrayCount) )
         {
         LOGGER_WRITE("KMPXMediaArrayCount not supported");
         User::Leave( KErrNotSupported );
         }
     
     TInt foundItemCount = *foundList->Value<TInt>(KMPXMediaArrayCount);
     
     CMPXMedia* playlist(NULL);
     if ( foundItemCount )
         {
         if ( !foundList->IsSupported(KMPXMediaArrayContents) )
             {
             LOGGER_WRITE("KMPXMediaArrayContents not supported");
             User::Leave(KErrNotSupported);
             }
             
         const CMPXMediaArray* foundArray = foundList->Value<CMPXMediaArray>(KMPXMediaArrayContents);
         
         // get the first founded item
         playlist = CMPXMedia::NewL(*(*foundArray)[0]);
         }
     else
         {
         User::Leave( KErrGeneral );
         }
     
     CleanupStack::PopAndDestroy( foundList );
     
     TRACE_FUNC_EXIT;
     return playlist;
     }

void CMediaManager::GetSongL( TInt aSongId, CSongItem& aSongItem )
    {
    TRACE_FUNC_ENTRY;
    TMPXItemId id(aSongId);

    RArray<TInt> contentIDs;
    CleanupClosePushL( contentIDs );
    contentIDs.AppendL( KMPXMediaIdMusic );
    contentIDs.AppendL( KMPXMediaIdGeneral );
    
    CMPXMedia* criteria = CMPXMedia::NewL(contentIDs.Array());
    CleanupStack::PopAndDestroy( &contentIDs );
    CleanupStack::PushL(criteria);
    
    // set search keys
    criteria->SetTObjectValueL<TMPXGeneralType>(
            KMPXMediaGeneralType, EMPXItem );
    criteria->SetTObjectValueL<TMPXGeneralCategory>(
            KMPXMediaGeneralCategory, EMPXSong );   
    criteria->SetTObjectValueL<TMPXItemId>(
            KMPXMediaGeneralId, id );
    
    // define attributes fetched
    RArray<TMPXAttribute> songAttributes;
    CleanupClosePushL(songAttributes);
    
    songAttributes.AppendL( KMPXMediaGeneralType );
    songAttributes.AppendL( KMPXMediaGeneralCategory );
    songAttributes.AppendL( KMPXMediaGeneralUri );
    songAttributes.AppendL( KMPXMediaGeneralId );
    songAttributes.AppendL( KMPXMediaGeneralTitle );
    songAttributes.AppendL( KMPXMediaGeneralMimeType );
    songAttributes.AppendL( KMPXMediaGeneralDuration );
    songAttributes.AppendL( KMPXMediaGeneralComment );
    
    songAttributes.AppendL( KMPXMediaMusicArtist );
    songAttributes.AppendL( KMPXMediaMusicAlbum );
    songAttributes.AppendL( KMPXMediaMusicYear );
    songAttributes.AppendL( KMPXMediaMusicAlbumTrack );
    songAttributes.AppendL( KMPXMediaMusicGenre );
    songAttributes.AppendL( KMPXMediaMusicComposer );
    
    // now find
    CMPXMedia* result = 0;
    TRAPD(err, result = iCollectionHelper->FindAllL( *criteria , songAttributes.Array() ));
    CleanupStack::PopAndDestroy( &songAttributes );
    CleanupStack::PopAndDestroy( criteria );
    
    if ( err )
        {
        LOGGER_WRITE_1("FindAllL err: %d", err);
        User::Leave( err );
        }
    
    CleanupStack::PushL( result );

    if ( !result->IsSupported(KMPXMediaArrayCount) )
         {
         LOGGER_WRITE("KMPXMediaArrayCount not supported");
         User::Leave(KErrNotSupported);
         }
    
    if ( !result->IsSupported( KMPXMediaArrayContents ) )
        {
        User::Leave(KErrNotSupported);
        }
    
    const CMPXMediaArray* songsArray =
    result->Value<CMPXMediaArray>(KMPXMediaArrayContents);
    if ( songsArray->Count() == 0 )
        {
        User::Leave( KErrNotFound );
        }
    const CMPXMedia* songMedia = (*songsArray)[0];
    
    ReadSongAttributesL( *songMedia, aSongItem, ETrue );
    
    CleanupStack::PopAndDestroy( result );
    
    TRACE_FUNC_EXIT;
    }

CMPXMedia* CMediaManager::CreatePlaylistMediaL( const CPlaylistItem& aPlaylist )
    {
    TRACE_FUNC_ENTRY;
    RArray<TInt> contentIDs;
    contentIDs.AppendL(KMPXMediaIdGeneral);
    
    CMPXMedia* playlist = CMPXMedia::NewL( contentIDs.Array() );
    CleanupStack::PushL( playlist );
    contentIDs.Close();
    // add playlist info into the playlist media
    playlist->SetTObjectValueL<TUid>(
        KMPXMediaGeneralCollectionId, KMpxLocalCollectionUid );
    
    // set playlist title
    playlist->SetTextValueL( KMPXMediaGeneralTitle, aPlaylist.Title() );
    
    // set type
    playlist->SetTObjectValueL( KMPXMediaGeneralType, EMPXItem );
     
    // set category
    playlist->SetTObjectValueL( KMPXMediaGeneralCategory, EMPXPlaylist ); 
    
    
    TFileName playlistfile;
    
    TInt drive;
    User::LeaveIfError( DriveInfo::GetDefaultDrive( DriveInfo::EDefaultMassStorage, drive) );
    TUint driveStatus( 0 );
    TInt err = DriveInfo::GetDriveStatus( iFs, drive, driveStatus );
    if ( !err && (driveStatus & DriveInfo::EDrivePresent) )
        {
        User::LeaveIfError( PathInfo::GetRootPath( playlistfile, drive ) );
        LOGGER_WRITE_1("mmc playlistfile: %S", &playlistfile);
        }
    else
        {
        playlistfile = PathInfo::PhoneMemoryRootPath();
        LOGGER_WRITE_1("phone playlistfile: %S", &playlistfile);
        }
    
    playlistfile.Append( KPlaylistPath );
    
    // set location drive
    TDriveUnit driveUnit( playlistfile );
    TPtrC driveName = driveUnit.Name();
    playlist->SetTextValueL( KMPXMediaGeneralDrive, driveName );
    
    
    TTime time;
    time.HomeTime();
    playlistfile.AppendNum(time.Int64());
    playlistfile.Append(KMPXVirtualPlaylistExt);
    
    LOGGER_WRITE_1("playlistfile: %S", &playlistfile);
    playlist->SetTextValueL(
             KMPXMediaGeneralUri, playlistfile );
    
    // set DbFlags to indicate that this is a virtual playlist
    playlist->SetTObjectValueL<TUint>(KMPXMediaGeneralFlags,
       KMPXMediaGeneralFlagsSetOrUnsetBit | KMPXMediaGeneralFlagsIsVirtual);

    // create songs
    CMPXMediaArray* medias = CreateMediaArrayLC( aPlaylist );
    // set medias
    playlist->SetCObjectValueL( KMPXMediaArrayContents, medias );
    /// set media count        
    playlist->SetTObjectValueL( KMPXMediaArrayCount, medias->Count() );
    
    CleanupStack::PopAndDestroy(medias);
    CleanupStack::Pop(playlist);
    
    
    TRACE_FUNC_EXIT;
    return playlist;
    }

CMPXMediaArray* CMediaManager::CreateMediaArrayLC( const CPlaylistItem& aList )
    {
    CMPXMediaArray* medias = CMPXMediaArray::NewL();
    CleanupStack::PushL( medias );
    //
    // create entries
    //
    TInt itemCount = aList.ItemCount();
    for ( TInt i=0; i<itemCount; i++)
        {
        TPtrC16 ptrUri = aList.ItemAt(i);
        LOGGER_WRITE_1("add uri: %S", &ptrUri);
        iCollectionHelper->AddL( ptrUri, this );
        iWaitOngoing = ETrue;
        LOGGER_WRITE("iWait.Start");
        iWait.Start();
        LOGGER_WRITE("continue..");
    
        RArray<TInt> contentIDs;
        contentIDs.AppendL(KMPXMediaIdGeneral);
        contentIDs.AppendL(KMPXMediaIdAudio);
        contentIDs.AppendL(KMPXMediaIdMusic);
        contentIDs.AppendL(KMPXMediaIdMTP); 
        
        CMPXMedia* entry = CMPXMedia::NewL( contentIDs.Array() );
        CleanupStack::PushL( entry );
        contentIDs.Close();
        
        entry->SetTObjectValueL<TUid>(
                KMPXMediaGeneralCollectionId, KMpxLocalCollectionUid );
        
        entry->SetTObjectValueL<TMPXGeneralType>(
                     KMPXMediaGeneralType, EMPXItem );
        
        entry->SetTObjectValueL<TMPXGeneralCategory>(
                     KMPXMediaGeneralCategory, EMPXSong );
        
        
        entry->SetTextValueL( KMPXMediaGeneralUri, ptrUri );
        
        // set location drive
        TDriveUnit driveUnit( ptrUri );
        TPtrC driveName = driveUnit.Name();
        entry->SetTextValueL( KMPXMediaGeneralDrive, driveName );
        
        medias->AppendL(entry);
        CleanupStack::Pop(entry);
        }
    return medias;
    }

void CMediaManager::CreateNewPlaylistL( TInt& aNewUid, CPlaylistItem& aList )
    {
    TRACE_FUNC_ENTRY;
    
    CMPXMedia* playlist = CreatePlaylistMediaL( aList );
    CleanupStack::PushL( playlist );
    iMediator->AddItemL( playlist );
    
    if ( !playlist->IsSupported(KMPXMediaGeneralId) )
        {
        LOGGER_WRITE("KMPXMediaGeneralId not supported");
        User::Leave( KErrNotSupported );
        }
    else
        {
        // Extract the playlist id from the found object
        TUint32 playlistId = *playlist->Value<TMPXItemId>(KMPXMediaGeneralId);
        LOGGER_WRITE_1("KMPXMediaGeneralId: %d", playlistId);
        aNewUid = playlistId;
        
        if ( !playlist->IsSupported( KMPXMediaGeneralUri ) )
            {
            // get url
            const TDesC& uri = playlist->ValueText( KMPXMediaGeneralUri );
            aList.SetUrlL( uri );
            }
        }
    
    CleanupStack::PopAndDestroy(playlist);
    
    TRACE_FUNC_EXIT;
    }

void CMediaManager::ReplacePlaylistL( TInt aPlaylistId, CPlaylistItem& aPlaylist )
    {
    TRACE_FUNC_ENTRY;
    CMPXMedia* playlist = CMPXMedia::NewL();
    CleanupStack::PushL( playlist );
    
    // add playlist info into the playlist media
    playlist->SetTObjectValueL<TUid>(
        KMPXMediaGeneralCollectionId, KMpxLocalCollectionUid );
    
    // set type
    playlist->SetTObjectValueL( KMPXMediaGeneralType, EMPXItem );
     
    // set category
    playlist->SetTObjectValueL( KMPXMediaGeneralCategory, EMPXPlaylist ); 
    
    
    
    // find playlist uri
    CMPXMedia* oldPlaylist = FindPlaylistL( aPlaylistId );
    CleanupStack::PushL( oldPlaylist );
    if ( !oldPlaylist->IsSupported(KMPXMediaGeneralUri) )
        {
        User::Leave( KErrNotSupported );
        }
    const TDesC& uri = oldPlaylist->ValueText( KMPXMediaGeneralUri );
    LOGGER_WRITE_1("uri: %S", &uri);
    
    playlist->SetTextValueL(
            KMPXMediaGeneralUri, uri );
    
	aPlaylist.SetUrlL( uri );
    
    const TDesC& title = oldPlaylist->ValueText( KMPXMediaGeneralTitle );
    if ( title.Compare( aPlaylist.Title() ) != 0 )
        {
        LOGGER_WRITE_1("title changed old: %S", &title);
        LOGGER_WRITE_1("new title: %S", &aPlaylist.Title());
        // set playlist title
        playlist->SetTextValueL( KMPXMediaGeneralTitle, aPlaylist.Title() );
        
        // update title first
        iMediator->SetItemL( playlist );
        }
    
    
    CleanupStack::PopAndDestroy( oldPlaylist );
    
    // update songs
    CMPXMediaArray* songArray = CreateMediaArrayLC( aPlaylist );
    // set medias
    playlist->SetCObjectValueL( KMPXMediaArrayContents, songArray );
    /// set media count        
    playlist->SetTObjectValueL( KMPXMediaArrayCount, songArray->Count() );
    
    iMediator->SetItemL( playlist );
    
    CleanupStack::PopAndDestroy( songArray );
    CleanupStack::PopAndDestroy( playlist );
    
    TRACE_FUNC_EXIT;
    }


void CMediaManager::DeletePlaylistL( TInt aPlaylistId )
    {
    TRACE_FUNC_ENTRY;
    
    CMPXMedia* playlist = FindPlaylistL( aPlaylistId );
    CleanupStack::PushL( playlist );
    iCollection->Collection().RemoveL( *playlist );
    CleanupStack::PopAndDestroy( playlist );
    
    TRACE_FUNC_EXIT;
    }

void CMediaManager::HandleAddFileCompleteL( TInt aErr )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1("aErr: %d", aErr);
    if ( iWaitOngoing )
        {
        LOGGER_WRITE("AsyncStop");
        iWaitOngoing = EFalse;
        iWait.AsyncStop();
        }
    TRACE_FUNC_EXIT;
    }

void CMediaManager::ReadSongAttributesL( const CMPXMedia& aMedia, CSongItem& aSongItem,
        TBool aReadFileDetails )
    {
    if ( !aMedia.IsSupported(KMPXMediaGeneralUri) )
        {
        LOGGER_WRITE("KMPXMediaGeneralUri not supported");
        User::Leave(KErrNotSupported);
        }    
    
    if ( !aMedia.IsSupported(KMPXMediaGeneralId) )
        {
        LOGGER_WRITE("KMPXMediaGeneralId not supported");
        User::Leave(KErrNotSupported);
        }
    
    aSongItem.SetId( *aMedia.Value<TMPXItemId>(KMPXMediaGeneralId) );
    
    const TDesC& uri = aMedia.ValueText(KMPXMediaGeneralUri);
    aSongItem.SetUriL( uri );
    
    if ( aMedia.IsSupported(KMPXMediaGeneralTitle) )
        {
        aSongItem.SetTitleL( aMedia.ValueText(KMPXMediaGeneralTitle) );
        }
    
    if ( aMedia.IsSupported(KMPXMediaGeneralMimeType) )
        {
        aSongItem.SetMimeTypeL( aMedia.ValueText(KMPXMediaGeneralMimeType) );
        }
    
    if ( aMedia.IsSupported(KMPXMediaGeneralDuration) )
        {
        aSongItem.SetDuration( aMedia.ValueTObjectL<TInt>(KMPXMediaGeneralDuration) );
        }
    
    if ( aMedia.IsSupported(KMPXMediaMusicArtist) )
        {
        aSongItem.SetArtistL( aMedia.ValueText(KMPXMediaMusicArtist) );
        }
    
    if ( aMedia.IsSupported(KMPXMediaMusicAlbum) )
        {
        aSongItem.SetAlbumL( aMedia.ValueText(KMPXMediaMusicAlbum) );
        }
    
    if ( aMedia.IsSupported(KMPXMediaMusicYear) )
        {
        TInt64 year = aMedia.ValueTObjectL<TInt64>( KMPXMediaMusicYear );
        TTime yearTime( year);
        aSongItem.SetYear( yearTime.DateTime().Year() );
        }
    
    if ( aMedia.IsSupported(KMPXMediaMusicAlbumTrack) )
        {
        aSongItem.SetAlbumTrack( *aMedia.Value<TInt>(KMPXMediaMusicAlbumTrack) );
        }
    
    if ( aMedia.IsSupported(KMPXMediaMusicGenre) )
        {
        const TDesC& genre = aMedia.ValueText(KMPXMediaMusicGenre);
        aSongItem.SetGenreL( genre );
        }
    
    if ( aMedia.IsSupported(KMPXMediaMusicComposer) )
        {
        aSongItem.SetComposerL( aMedia.ValueText(KMPXMediaMusicComposer) );
        }
    
    if ( aMedia.IsSupported(KMPXMediaGeneralComment) )
        {
        aSongItem.SetCommentL( aMedia.ValueText(KMPXMediaGeneralComment) );
        }
    
    // Reading file timestamp and size is slow, read only if needed
    if ( aReadFileDetails )
        {
        TEntry fileEntry;
        TInt err = iFs.Entry(uri,fileEntry);
        if ( !err )
            {
            aSongItem.SetModifiedTime( fileEntry.iModified );
            aSongItem.SetFileSize( fileEntry.iSize );
            }
        }
    
    }


void CMediaManager::DoCancel()
    {
    TRACE_FUNC_ENTRY;
    
    delete iFoundedMedia;
    iFoundedMedia = NULL;
    iReadedItems = 0;
    iItemsFound = 0;      
    TState state = iCurrentState;
    iCurrentState = EIdle;
    
    switch (state)
        {
        case EReadingPlaylistCount:
        case EEnumeratingPlaylists:
            iObserver->RegisterAllPlayListsCompleted( KErrCancel );
            break;
            
        case EReadingSongCount:
        case EEnumeratingSongs:
            iObserver->RegisterAllSongsCompleted( KErrCancel );
            break;
        default:
            break;
        }
    
    LOGGER_WRITE("signal cancel");
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrCancel );
    
    TRACE_FUNC_EXIT;
    }

void CMediaManager::RunL()
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1("iStatus.Int(): %d", iStatus.Int())
    TRequestStatus* status = &iStatus;
    switch (iCurrentState)
        {
        case EReadingPlaylistCount:
            //if ( !iCancel )
            //    {
                ReadPlaylistCountL();
                iReadedItems=0;
                // next state
                iCurrentState = EEnumeratingPlaylists;
                SetActive();
                User::RequestComplete( status, KErrNone );
            /*    }
            else
                {
                LOGGER_WRITE("cancel");
                iCancel = EFalse;
                delete iFoundedMedia; iFoundedMedia = NULL;
                iReadedItems = 0;
                iItemsFound = 0;
                iCurrentState = EIdle;
                iObserver->RegisterAllPlayListsCompleted( KErrCancel );
                }*/
            break;
            
        case EEnumeratingPlaylists:
            //if ( !iCancel )
            //    {
                ReadPlaylistItemsL();
                if ( iReadedItems == iItemsFound )
                    {
                    // all items readed
                    delete iFoundedMedia;
                    iFoundedMedia = NULL;
                    // next state
                    iCurrentState = EIdle;
                    iObserver->RegisterAllPlayListsCompleted( KErrNone );
                    }
                else
                    {
                    // not ready yet, continue same operation
                    SetActive();
                    User::RequestComplete( status, KErrNone );
                    }
             /*   }
            else
                {
                // cancel
                LOGGER_WRITE("cancel");
                iCancel = EFalse;
                delete iFoundedMedia; iFoundedMedia = NULL;
                iReadedItems = 0;
                iItemsFound = 0;
                iCurrentState = EIdle;
                iObserver->RegisterAllPlayListsCompleted( KErrCancel );
                }*/
            break;
            
        case EReadingSongCount:
            //if ( !iCancel )
            //    {
                ReadSongCountL();
                iReadedItems=0;
                // next state
                iCurrentState = EEnumeratingSongs;
                SetActive();
                User::RequestComplete( status, KErrNone );
            /*    }
            else
                {
                LOGGER_WRITE("cancel");
                iCancel = EFalse;
                delete iFoundedMedia; iFoundedMedia = NULL;
                iReadedItems = 0;
                iItemsFound = 0;
                iCurrentState = EIdle;
                iObserver->RegisterAllSongsCompleted( KErrCancel );
                }*/
            break;
            
        case EEnumeratingSongs:
//            if ( !iCancel )
              //  {
                ReadSongItemsL();
                if ( iReadedItems == iItemsFound )
                    {
                    // all items readed
                    delete iFoundedMedia;
                    iFoundedMedia = NULL;
                    // next state
                    iCurrentState = EIdle;
                    iObserver->RegisterAllSongsCompleted( KErrNone );
                    }
                else
                    {
                    // not ready yet, continue same operation
                    SetActive();
                    User::RequestComplete( status, KErrNone );
                    }
              /*  }
            else
                {
                LOGGER_WRITE("cancel");
                iCancel = EFalse;
                delete iFoundedMedia; iFoundedMedia = NULL;
                iReadedItems = 0;
                iItemsFound = 0;
                iCurrentState = EIdle;
                iObserver->RegisterAllSongsCompleted( KErrCancel );
                }*/
            break;
        default:
            break;
        }
    TRACE_FUNC_EXIT;
    }


TInt CMediaManager::RunError( TInt aError )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1("aError: %d", aError)
    switch (iCurrentState)
        {
        case EReadingPlaylistCount:
        case EEnumeratingPlaylists:
            iObserver->RegisterAllPlayListsCompleted( aError );
            break;
        case EReadingSongCount:
        case EEnumeratingSongs:
            iObserver->RegisterAllSongsCompleted( aError );
            break;
        default:
            LOGGER_WRITE("Unknown state!");
            break;
        }
    
    TRACE_FUNC_EXIT;
    return KErrNone;
    }

void CMediaManager::ReadPlaylistCountL()
    {
    TRACE_FUNC_ENTRY;
    // Find all playlist files in the playlist DB
    RArray<TInt> contentIDs;
    CleanupClosePushL( contentIDs );
    contentIDs.AppendL(KMPXMediaIdGeneral);
    
    CMPXMedia* searchMedia = CMPXMedia::NewL(contentIDs.Array());
    CleanupStack::PopAndDestroy( &contentIDs );
    CleanupStack::PushL(searchMedia);
    
    searchMedia->SetTObjectValueL(KMPXMediaGeneralType, EMPXItem);    
    searchMedia->SetTObjectValueL(KMPXMediaGeneralCategory, EMPXPlaylist);
    
    RArray<TMPXAttribute> playlistAttributes;
    CleanupClosePushL(playlistAttributes);
    playlistAttributes.AppendL( KMPXMediaGeneralId );
    playlistAttributes.AppendL( KMPXMediaGeneralTitle );
    playlistAttributes.AppendL( KMPXMediaGeneralUri );
    
    LOGGER_WRITE("find all");
    delete iFoundedMedia; iFoundedMedia=NULL;
    iFoundedMedia = iCollectionHelper->FindAllL(*searchMedia, playlistAttributes.Array());
    CleanupStack::PopAndDestroy(&playlistAttributes);
    CleanupStack::PopAndDestroy(searchMedia);

    if ( !iFoundedMedia->IsSupported(KMPXMediaArrayCount) )
        {
        delete iFoundedMedia; iFoundedMedia=NULL;
        User::Leave( KErrNotSupported );
        }
    
    iItemsFound = *iFoundedMedia->Value<TInt>(KMPXMediaArrayCount);
    
    LOGGER_WRITE_1("playlists: %d", iItemsFound);

    if ( iItemsFound )
        {
        if ( !iFoundedMedia->IsSupported(KMPXMediaArrayContents) )
            {
            delete iFoundedMedia; iFoundedMedia=NULL;
            iItemsFound = 0;
            User::Leave(KErrNotSupported);
            }
        }
    
    TRACE_FUNC_EXIT;
    }

void CMediaManager::ReadPlaylistItemsL()
    {
    TRACE_FUNC_ENTRY;
    TInt start = iReadedItems;
    TInt end = start+iMaxPlaylistItemsToEnumerate;
    if ( end >iItemsFound )
        {
        end = iItemsFound;
        }
    
    const CMPXMediaArray* foundArray = iFoundedMedia->Value<CMPXMediaArray>(KMPXMediaArrayContents);
            
    for (TInt i = start; i < end; i++)
        {
        LOGGER_WRITE_1("read playlistItem: %d", i);
        CMPXMedia* playlistMedia = CMPXMedia::NewL(*(*foundArray)[i]);
        CleanupStack::PushL( playlistMedia );
        
        // Append playlist to item array
        RegisterPlaylistL( *playlistMedia );
               
        CleanupStack::PopAndDestroy( playlistMedia );
        iReadedItems++;
        }
    
    TRACE_FUNC_EXIT;
    }

    

void CMediaManager::ReadSongCountL()
    {
    TRACE_FUNC_ENTRY;

    RArray<TInt> contentIDs;
    CleanupClosePushL( contentIDs );
    contentIDs.AppendL( KMPXMediaIdGeneral );
    contentIDs.AppendL( KMPXMediaIdMusic );
    
    CMPXMedia* criteria = CMPXMedia::NewL(contentIDs.Array());
    CleanupStack::PopAndDestroy( &contentIDs );
    CleanupStack::PushL(criteria);
    
    criteria->SetTObjectValueL( KMPXMediaGeneralType, EMPXItem );
    criteria->SetTObjectValueL( KMPXMediaGeneralCategory, EMPXSong );
    
    // define attributes fetched
    RArray<TMPXAttribute> songAttributes;
    CleanupClosePushL(songAttributes);
    
    songAttributes.AppendL( KMPXMediaGeneralType );
    songAttributes.AppendL( KMPXMediaGeneralCategory );
    songAttributes.AppendL( KMPXMediaGeneralUri );
    songAttributes.AppendL( KMPXMediaGeneralId );
    songAttributes.AppendL( KMPXMediaGeneralTitle );
    songAttributes.AppendL( KMPXMediaGeneralMimeType );
    songAttributes.AppendL( KMPXMediaGeneralDuration );
    songAttributes.AppendL( KMPXMediaGeneralComment );
    
    songAttributes.AppendL( KMPXMediaMusicArtist );
    songAttributes.AppendL( KMPXMediaMusicAlbum );
    songAttributes.AppendL( KMPXMediaMusicYear );
    songAttributes.AppendL( KMPXMediaMusicAlbumTrack );
    songAttributes.AppendL( KMPXMediaMusicGenre );
    songAttributes.AppendL( KMPXMediaMusicComposer );
    
    delete iFoundedMedia;
    iFoundedMedia = NULL;
    // now find
    LOGGER_WRITE("FindAllL");
    iFoundedMedia = iCollectionHelper->FindAllL( *criteria , songAttributes.Array() );
        
    LOGGER_WRITE("FindAllL ok");
    CleanupStack::PopAndDestroy( &songAttributes );
    CleanupStack::PopAndDestroy( criteria );
    

    if ( !iFoundedMedia->IsSupported(KMPXMediaArrayCount) )
         {
         LOGGER_WRITE("KMPXMediaArrayCount not supported");
         delete iFoundedMedia;
         iFoundedMedia = NULL;
         iItemsFound = 0;
         User::Leave(KErrNotSupported);
         }
     
    // Number of references
    iItemsFound = *iFoundedMedia->Value<TInt>(KMPXMediaArrayCount);
    LOGGER_WRITE_1("songs: %d", iItemsFound);
    if ( iItemsFound )
        {
        if (!iFoundedMedia->IsSupported(KMPXMediaArrayContents))
            {
            delete iFoundedMedia;
            iFoundedMedia = NULL;
            iItemsFound = 0;
            User::Leave(KErrNotSupported);
            }
        }
    
    TRACE_FUNC_EXIT;
    }

void CMediaManager::ReadSongItemsL()
    {
    TRACE_FUNC_ENTRY;
    
    TInt start = iReadedItems;
    TInt end = start+iMaxSongItemsToEnumerate;
    if ( end >iItemsFound )
        {
        end = iItemsFound;
        }
    
    const CMPXMediaArray* foundArray = iFoundedMedia->Value<CMPXMediaArray>(KMPXMediaArrayContents);
    
    TKeyArrayFix key( iKey );
    // Import the references
    for (TInt i = start; i < end; i++)
        {
        //LOGGER_WRITE_1("read songitem: %d",i);
        const CMPXMedia* refMedia = (*foundArray)[i];
        
        CSongItem* song = CSongItem::NewLC();
        
        ReadSongAttributesL( *refMedia, *song, EFalse );
        TSnapshotItem snapshotItem( song->Id(), KAllSongsFolderId );
        snapshotItem.SetItemHash( *song, iHasher );
        
        iSnapshotArray->InsertIsqL( snapshotItem, key );
        
        CleanupStack::PopAndDestroy( song );
        iReadedItems++;
        }
    TRACE_FUNC_EXIT;
    }
