/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is used by db plugin for all database related
*                functionality. The main responsibilities are:
*
*/


// INCLUDE FILES
#include <bautils.h>
#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#include <pathinfo.h>
#endif //RD_MULTIPLE_DRIVE

#include <mpxcollectiondbhgres.rsg>
#include <mpxmediageneraldefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxmediaaudiodefs.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxcollectionpath.h>
#include <mpxlog.h>

#include "mpxresource.h"
#include "mpxdbcommonutil.h"

#include "mpxdbutil.h"
#include "mpxcollectiondbdef.h"
#include "mpxdbpluginqueries.h"
#include "mpxcollectiondbmanager.h"
#include "mpxdbplaylist.h"
#include "mpxdbplaylistsongs.h"
#include "mpxdbcategory.h"
#include "mpxdbauxiliary.h"
#include "mpxdbautoplaylist.h"
#include "mpxdbhandler.h"
#include "mpxdbartist.h"
#include "mpxdbalbum.h"
#include "mpxdbgenre.h"
#include "mpxdbcomposer.h"

// CONSTANTS
_LIT(KMPXVirtualPlaylistExt, ".vir");
static const TInt KMaxOpInTransaction = 100;

const TInt KSqlDbCorrupted = -321;

#if defined (__MTP_PROTOCOL_SUPPORT)

#include <centralrepository.h>

// MTP CenRep Key UID
const TUid KMPXMtpSettings = {0x101FFC53};
// MTP CenRep Key for Delete contents
const TUint32 KMPXMtpSaveDeletedRecordFlag = 0x00000001;

#endif

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbHandler* CMPXDbHandler::NewL(
    RFs& aFs,
    CMPXResource& aResource)
    {
    MPX_FUNC("CMPXDbHandler::NewL");
    CMPXDbHandler* self = CMPXDbHandler::NewLC(aFs, aResource);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbHandler* CMPXDbHandler::NewLC(
    RFs& aFs,
    CMPXResource& aResource)
    {
    MPX_FUNC("CMPXDbHandler::NewLC");
    CMPXDbHandler* self = new (ELeave) CMPXDbHandler(aFs, aResource);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CMPXDbHandler::~CMPXDbHandler()
    {
    MPX_FUNC("CMPXDbHandler::~CMPXDbHandler");

    delete iAutoPlaylist;

    delete iDbMusic;
    delete iDbPlaylist;
    delete iDbArtist;
    delete iDbAlbum;
    delete iDbGenre;
    delete iDbComposer;
    delete iDbAuxiliary;
    delete iDbManager;
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
    delete iDbAbstractAlbum;
#endif // ABSTRACTAUDIOALBUM_INCLUDED
    delete iMimeTypes;
    delete iExtensions;
    delete iExtensionsMime;
    delete iExtensionsDrm;

    iDbDrives.Close();
    }

// ----------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave
// ----------------------------------------------------------------------------
//
CMPXDbHandler::CMPXDbHandler(
    RFs& aFs,
    CMPXResource& aResource) :
    iFs(aFs),
    iResource(aResource)
    {
    MPX_FUNC("CMPXDbHandler::CMPXDbHandler");
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::ConstructL()
    {
    MPX_FUNC("CMPXDbHandler::ConstructL");

    iMimeTypes = iResource.ReadDesCArrayL(R_MC_MIME_TYPES);
    iExtensions = iResource.ReadDesCArrayL(R_MC_MUSIC_FILE_EXTENSIONS);
    iExtensionsMime = iResource.ReadDesCArrayL(R_MC_FILE_EXTENSIONS_MIME);
    iExtensionsDrm = iResource.ReadDesCArrayL(R_MC_FILE_EXTENSIONS_DRM);

    // make sure all databases are created and valid
    iDbManager = CMPXCollectionDbManager::NewL(iFs);

    CDesCArrayFlat* musicFolders =
#ifdef RD_MULTIPLE_DRIVE
        GetMusicFoldersL();
#else
        iResource.ReadDesCArrayL(R_MC_DEFAULT_MUSIC_FOLDERS);
#endif

    // create the music folders and initialize iDbDrives
    CleanupStack::PushL(musicFolders);
    ProcessMusicFoldersL(*musicFolders);
    CleanupStack::PopAndDestroy(musicFolders);

    // Create the db infrastructure,
    //
    iDbMusic = CMPXDbMusic::NewL(*iDbManager, iResource, *this);
    iDbPlaylist = CMPXDbPlaylist::NewL(*iDbManager, *this);
    iDbArtist = CMPXDbArtist::NewL(*iDbManager, EMPXArtist);
    iDbAlbum = CMPXDbAlbum::NewL(*iDbManager, EMPXAlbum, *this);
    iDbGenre = CMPXDbGenre::NewL(*iDbManager, EMPXGenre);
    iDbComposer = CMPXDbComposer::NewL(*iDbManager, EMPXComposer);
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
    iDbAbstractAlbum = CMPXDbAbstractAlbum::NewL(*iDbManager, EMPXAbstractAlbum, iFs);
#endif // ABSTRACTAUDIOALBUM_INCLUDED
    iAutoPlaylist = CMPXDbAutoPlaylist::NewL(*iDbManager, iFs, iResource);
    iDbAuxiliary = CMPXDbAuxiliary::NewL(*iDbManager);

    MPX_TRAPD(err, iDbManager->InitDatabasesL(iDbDrives));

    // If KErrCorrupt is returned, a database file was found to be corrupted
    // and was replaced with a new one.  The db plugin can ignore this error and continue
    // because a new db file was successfully created in a subsequent retry.
    if ((err != KErrNone) && (err != KErrCorrupt) && (err != KErrDiskFull))
        {
        // leave to signal the caller that there was an error why creating and opening
        // one or more of the databases
        User::Leave(err);
        }
    else if (err == KErrDiskFull)
        {
        iOutOfDisk = ETrue;
        }
    else
        {
        // do nothing
        }

    // Verify the volume ids of each drive matches the database
    MPX_TRAP(err,VerifyVolumeIdL());
    if ((err != KErrNone) && (err != KErrDiskFull))
        {
        // leave to signal the caller that there was an error why creating and opening
        // one or more of the databases
        User::Leave(err);
        }
    else if (err == KErrDiskFull)
        {
        iOutOfDisk = ETrue;
        }

//#ifdef _DEBUG
//    iDbManager->PrintDatabaseL();    // PREQ2536 the files sqlrowsetutil.h and sqlrowsetutil.cpp has been removed
//#endif

    MPX_DEBUG2("CMPXDbHandler::ConstructL DbCount[%d]", iDbManager->DatabaseCount());
    }

// ----------------------------------------------------------------------------
// Add song to collection
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbHandler::AddSongL(
    const CMPXMedia& aMedia,
    CMPXMessageArray* aMessageArray)
    {
    MPX_FUNC("CMPXDbHandler::AddSongL");

    BeginTransactionL();
    TUint32 songId(0);
    MPX_TRAPD(err, songId = DoAddSongL(aMedia,aMessageArray));

    if (iOutOfDisk && (err == KErrNotFound))
        {
        err = KErrDiskFull;
        }
    EndTransactionL(err);

    return songId;
    }

// ----------------------------------------------------------------------------
// Add song to collection with no database transaction
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbHandler::AddSongWithNoTransactionL(
    const CMPXMedia& aMedia,
    CMPXMessageArray* aMessageArray)
    {
    MPX_FUNC("CMPXDbHandler::AddSongWithNoTransactionL");

    TUint32 songId(0);
    MPX_TRAPD(err, songId = DoAddSongL(aMedia,aMessageArray));

    if (iOutOfDisk && (err == KErrNotFound))
        {
        err = KErrDiskFull;
        }
    User::LeaveIfError(err);

    return songId;
    }

// ----------------------------------------------------------------------------
// Add playlist to collection
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbHandler::AddPlaylistL(
    const CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbHandler::AddPlaylistL");

    BeginTransactionL();
    TUint32 playlistId(0);
    MPX_TRAPD(err, playlistId = DoAddPlaylistL(aMedia));
    EndTransactionL(err);

    return playlistId;
    }

// ----------------------------------------------------------------------------
// Add song to playlist
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbHandler::AddSongToPlaylistL(
    const CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbHandler::AddSongToPlaylistL");

    BeginTransactionL();
    TUint32 playlistId(0);
    MPX_TRAPD(err, playlistId = DoAddSongToPlaylistL(aMedia));
    EndTransactionL(err);

    return playlistId;
    }

#ifdef ABSTRACTAUDIOALBUM_INCLUDED
// ----------------------------------------------------------------------------
// Add AbstractAlbum to collection
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbHandler::AddAbstractAlbumL(
    const CMPXMedia& aMedia,
    CMPXMessageArray* aMessageArray)
    {
    MPX_FUNC("CMPXDbHandler::AddAbstractAlbumL");

    BeginTransactionL();
    TBool newRecord(EFalse);
    TInt err(KErrNone);

    TDriveUnit drive(aMedia.ValueText(KMPXMediaGeneralUri));
    TPtrC uri(aMedia.ValueText(KMPXMediaGeneralUri).Left(KMCMaxTextLen));
    TPtrC name(aMedia.ValueText(KMPXMediaGeneralTitle).Left(KMCMaxTextLen));
    TPtrC albumartist(aMedia.ValueText(KMPXMediaMusicAlbumArtist).Left(KMCMaxTextLen));

    //only insert to AbstractAlbum table when it is new item
    TUint32 abstractAlbumId(MPXDbCommonUtil::GenerateUniqueIdL(iFs, EMPXAbstractAlbum, uri, EFalse));

    newRecord = !iDbAbstractAlbum->CategoryItemExistsL(drive, abstractAlbumId);

    if (newRecord)
        {
        MPX_TRAP(err, abstractAlbumId = iDbAbstractAlbum->AddItemL(uri, name, albumartist, drive, newRecord, EFalse));
        if (iOutOfDisk && (err == KErrNotFound))
            {
            err = KErrDiskFull;
            }
        if (aMessageArray)
            {
            MPXDbCommonUtil::AddItemChangedMessageL(*aMessageArray, abstractAlbumId, EMPXItemInserted,
            EMPXAbstractAlbum, KDBPluginUid);
            }
        }
    EndTransactionL(err);

    return abstractAlbumId;
    }

// ----------------------------------------------------------------------------
// Update abstractalbum info to AbstractAlbum table and all songs which associate 
// with AbstractAlbum in the collection
// ----------------------------------------------------------------------------
//
CMPXDbActiveTask::TChangeVisibility CMPXDbHandler::UpdateAbstractAlbumL(
    const CMPXMedia& aMedia,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbHandler::UpdateAbstractAlbumL");

    CMPXDbActiveTask::TChangeVisibility visibleChange(CMPXDbActiveTask::ENotVisibile);
    TUint32 itemId(0);
    if (aMedia.IsSupported(KMPXMediaGeneralUri))
        {
        const TDesC& uri(aMedia.ValueText (KMPXMediaGeneralUri));
        TDriveUnit drive(aMedia.ValueText(KMPXMediaGeneralUri));

        //get Id based on new uri
        itemId = MPXDbCommonUtil::GenerateUniqueIdL(iFs, EMPXAbstractAlbum, uri, EFalse);
        if (!itemId)
            {
            User::Leave(KErrNotSupported);
            }

        //if updating to new item, need do renaming for AbstractAlbum: delete old one and insert a new entry to AbstractAlbum table
        //if a new item, need update songs associated and renaming albstractalbum table
        if (aMedia.IsSupported(KMPXMediaGeneralId))
            {
            MPX_DEBUG1("CMPXDbHandler::UpdateAbstractAlbumL, rename case");
            
            BeginTransactionL();
            //get old id, for renaming
            TInt err(KErrNone);
            TUint32 oldId = (aMedia.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId)).iId2;
            if (oldId && (oldId != itemId) )  //pass the old UID for renaming, do not care if new itemId exist or not
                {
                //get old Uri based on old Id
                HBufC* oldUri = iDbAbstractAlbum->GetUriL(oldId);
                CleanupStack::PushL(oldUri);

                //add and update new item to AbstractAlbum table
                //old existing item field values need to be saved and added when adding new item                        
                MPX_TRAP(err, itemId = iDbAbstractAlbum->AddUpdatedItemL(oldId, uri ));

                if (iOutOfDisk && (err == KErrNotFound))
                    {
                    err = KErrDiskFull;
                    }

                MPXDbCommonUtil::AddItemChangedMessageL(aItemChangedMessages, itemId, EMPXItemInserted,
                EMPXAbstractAlbum, KDBPluginUid);

                //find and update all songs associated with oldId
                RArray<TMPXAttribute> songAttributes;
                CleanupClosePushL(songAttributes);
                songAttributes.AppendL(KMPXMediaGeneralId);

                CMPXMediaArray* mediaArray = CMPXMediaArray::NewL();
                CleanupStack::PushL(mediaArray);

                //get all songs associated
                MPX_TRAP(err, iDbMusic->GetAllSongsForAbstractAlbumL(oldId, songAttributes.Array(), *mediaArray));
                if (err == KErrNotFound)
                    {
                    // Leave with KErrNotFound
                    MPX_DEBUG1("CMPXDbHandler::UpdateAbstractAlbumL, leave with not found");
                    User::Leave(KErrNotFound);
                    }

                TInt count(mediaArray->Count());
                MPX_DEBUG2("CMPXDbHandler::UpdateAbstractAlbumL, [%d] songs associated", count);
                //go through all songs for updating
                for (TInt i = 0; i < count; i++)
                    {
                    CMPXMedia* song = mediaArray->AtL(i);
                    song->SetTextValueL(KMPXMediaMusicAlbumArtFileName, uri );
                    visibleChange = UpdateSongL(*song, aItemChangedMessages);
                    }
                CleanupStack::PopAndDestroy(mediaArray);
                CleanupStack::PopAndDestroy(&songAttributes);

                //rename TN
                iDbAbstractAlbum->HandleTNL(*oldUri, uri, 0);
                CleanupStack::PopAndDestroy(oldUri);
                }
            EndTransactionL(err);
            }//renaming

        //check if abstractAlbum exist in ABSTRACTALBUM table before update it
        else if (iDbAbstractAlbum->CategoryItemExistsL(drive, itemId))
            {
            //support updating Name, AlbumArtist fields for AbstractAlbum table
            iDbAbstractAlbum->UpdateItemL(itemId, aMedia, drive, &aItemChangedMessages);
            }
        }
    return visibleChange;
    }

// ----------------------------------------------------------------------------
// Update all songs which associate with AbstractAlbum to new AbstractAlbum info 
// in the collection
// ----------------------------------------------------------------------------
//
CMPXDbActiveTask::TChangeVisibility CMPXDbHandler::UpdateSongsAbstractAlbumInfoL(
    const CMPXMedia& aMedia,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbHandler::UpdateSongsAbstractAlbumInfoL");
    const TDesC& uri(aMedia.ValueText (KMPXMediaGeneralUri));
    //need to update songs information to music table
    CMPXMediaArray* mediaArray = aMedia.Value<CMPXMediaArray>(KMPXMediaArrayContents);
    User::LeaveIfNull(mediaArray);
    TInt count(mediaArray->Count());
    CMPXDbActiveTask::TChangeVisibility visibleChange(CMPXDbActiveTask::ENotVisibile);
    for (TInt i = 0; i < count; i++)
        {
        CMPXMedia* mediaSong = mediaArray->AtL(i);
        mediaSong->SetTextValueL(KMPXMediaMusicAlbumArtFileName, uri );
        visibleChange = UpdateSongL(*mediaSong, aItemChangedMessages);
        }
    return visibleChange;
    }
#endif // ABSTRACTAUDIOALBUM_INCLUDED

// ----------------------------------------------------------------------------
// Update a song in the collection
// ----------------------------------------------------------------------------
//
CMPXDbActiveTask::TChangeVisibility CMPXDbHandler::UpdateSongL(
    const CMPXMedia& aMedia,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbHandler::UpdateSongL");

    BeginTransactionL();
    CMPXDbActiveTask::TChangeVisibility visibleChange(CMPXDbActiveTask::ENotVisibile);
    MPX_TRAPD(err, visibleChange = DoUpdateSongL(aMedia, aItemChangedMessages));
    EndTransactionL(err);
    return visibleChange;
    }

// ----------------------------------------------------------------------------
// Update a playlist in the collection
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::UpdatePlaylistL(
    const CMPXMedia& aMedia,
    CMPXMessageArray& aMessageArray)
    {
    MPX_FUNC("CMPXDbHandler::UpdatePlaylistL");

    BeginTransactionL();
    MPX_TRAPD(err, DoUpdatePlaylistL(aMedia, aMessageArray));
    EndTransactionL(err);
    }

// ----------------------------------------------------------------------------
// Updates the playlist songs
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::UpdatePlaylistSongsL(
    const CMPXMedia& aMedia,
    CMPXMessage& aMessage)
    {
    MPX_FUNC("CMPXDbHandler::UpdatePlaylistSongsL");

    BeginTransactionL();
    MPX_TRAPD(err, DoUpdatePlaylistSongsL(aMedia, aMessage));
    EndTransactionL(err);
    }

// ----------------------------------------------------------------------------
// Reorder a song in a playlist
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::ReorderPlaylistL(
    const TMPXItemId& aPlaylistId,
    const TMPXItemId& aSongId,
    TUint aOriginalOrdinal,
    TUint aNewOrdinal,
    CMPXMessage& aMessage)
    {
    MPX_DEBUG5("-->CMPXDbHandler::ReorderPlaylistL(0x%x, 0x%x, %d, %d)",
               aPlaylistId.iId2, aSongId.iId2, aOriginalOrdinal, aNewOrdinal);

    BeginTransactionL();
    MPX_TRAPD(err, DoReorderPlaylistL(aPlaylistId, aSongId, aOriginalOrdinal, aNewOrdinal, aMessage));
    EndTransactionL(err);
    MPX_DEBUG2("<--CMPXDbHandler::ReorderPlaylistL() error=%d", err);
    }

// ----------------------------------------------------------------------------
// Remove the entire music collection database
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::RemoveEntireCollectionL()
    {
    MPX_FUNC("CMPXDbHandler::RemoveEntireCollectionL");
    BeginTransactionL();
    MPX_TRAPD(err, iDbManager->RecreateAllDatabasesL());
    EndTransactionL(err);
    }

// ----------------------------------------------------------------------------
// Delete a song from collection
// The function notifies collection model to perform deletion
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::RemoveSongL(
    TUint32 aSongId,
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages,
    TBool aDeleteRecord)
    {
    MPX_FUNC("CMPXDbHandler::RemoveSongL");

    MPX_TRAPD(err, DoRemoveSongL(aSongId, aUriArray, aItemChangedMessages, aDeleteRecord));

    MPX_TRAP(err, DoRemoveSongFromPlaylistL(aSongId,aItemChangedMessages));

    }

// ----------------------------------------------------------------------------
// Removes a category of songs from the music collection,
// and its corresponding category in the lookup table
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::RemoveSongsMatchingCategoryL(
    TMPXGeneralCategory aCategory,
    TUint32 aCategoryId,
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbHandler::RemoveSongsMatchingCategoryL");

    BeginTransactionL();
    MPX_TRAPD(err, DoRemoveSongsMatchingCategoryL(aCategory, aCategoryId, aUriArray, aItemChangedMessages));
    EndTransactionL(err);
    }

// ----------------------------------------------------------------------------------------------------------
// Delete songs for the specified artist and album from collection
// The function notifies collection model to perform deletion
// ----------------------------------------------------------------------------------------------------------
//
void CMPXDbHandler::RemoveSongsMatchingArtistAndAlbumL(
    TUint32 aArtistId,
    TUint32 aAlbumId,
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbHandler::RemoveSongsMatchingArtistAndAlbumL");

    BeginTransactionL();
    MPX_TRAPD(err, DoRemoveSongsMatchingArtistAndAlbumL(aArtistId, aAlbumId, aUriArray,
        aItemChangedMessages));
    EndTransactionL(err);
    }

// ----------------------------------------------------------------------------
// Remove all playlists from collection
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::RemoveAllPlaylistsL()
    {
    MPX_FUNC("CMPXDbHandler::RemoveAllPlaylistsL");

    BeginTransactionL();
    MPX_TRAPD(err, DoRemoveAllPlaylistsL());
    EndTransactionL(err);
    }

// ----------------------------------------------------------------------------
// Remove specified playlist
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::RemovePlaylistL(
    TUint32 aPlaylistId,
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbHandler::RemovePlaylistL");

    BeginTransactionL();
    MPX_TRAPD(err, DoRemovePlaylistL(aPlaylistId, aUriArray, aItemChangedMessages));
    EndTransactionL(err);
    }

// ----------------------------------------------------------------------------
// Remove song from playlist songs table
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::RemoveSongFromPlaylistL(
    TUint32 aPlaylistId,
    const TMPXItemId& aSongId,
    TInt aOrdinal,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbHandler::RemoveSongFromPlaylistL");
    MPX_DEBUG5("CMPXDbHandler::RemoveSongFromPlaylistL(playlist 0x%x, songId [0x%x,0x%x], ordinal %d)",
        aPlaylistId, aSongId.iId1, aSongId.iId2, aOrdinal);

    MPX_TRAPD(err, DoRemoveSongFromPlaylistL(aPlaylistId, aSongId, aOrdinal, aItemChangedMessages));
    if ( err && InTransaction() )
        {
        // only end transaction if there's an error
        EndTransactionL( err );
        }
    }

#ifdef ABSTRACTAUDIOALBUM_INCLUDED
// ----------------------------------------------------------------------------
// Remove specified abstractalbum
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::RemoveAbstractAlbumL(
    TUint32 aAbstractAlbumId,
    CMPXMessageArray& aItemChangedMessages, TBool aFileDeleted)
    {
    MPX_FUNC("CMPXDbHandler::RemoveAbstractAlbumL");
    iDbAbstractAlbum->RemoveAbstractAlbumL(aAbstractAlbumId, aItemChangedMessages, aFileDeleted);
    }
#endif // ABSTRACTAUDIOALBUM_INCLUDED

// ----------------------------------------------------------------------------
// Cleanup records marked as deleted. This is designated for MTP to clean up records marked as deleted
// at the end of its session.
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::CleanupDeletedRecordsL()
    {
    MPX_FUNC("CMPXDbHandler::CleanupDeletedRecordsL");

    BeginTransactionL();
    MPX_TRAPD(err, DoCleanupDeletedRecordsL());
    EndTransactionL(err);
    }

// ----------------------------------------------------------------------------
//  Read all songs and cache them into an array ordered by song name
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::GetAllSongsL(
    CMPXMediaArray* aMediaArray,
    const TArray<TMPXAttribute>& aAttrs)
    {
    MPX_FUNC("CMPXDbHandler::GetAllSongsL");
    iDbMusic->GetAllSongsL(aAttrs, *aMediaArray);
    }

// ----------------------------------------------------------------------------
//  Read a limited # of songs and cache them into an array ordered by song name
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::GetAllSongsLimitedL(const TArray<TMPXAttribute>& aAttrs,
                                        CMPXMediaArray& aMediaArray, TInt aLimit)
    {
    MPX_FUNC("CMPXDbHandler::GetAllSongsLimitedL");
    iDbMusic->GetAllSongsLimitedL( aAttrs, aMediaArray, aLimit );
    }

// ----------------------------------------------------------------------------
//  Read all songs and cache them into an array ordered by song name
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::GetSongsInBlockL(
    CMPXMediaArray* aMediaArray,
    const TArray<TMPXAttribute>& aAttrs,
    TPtrC aTitle,
    TUint aNumOfSongs,
    TBool aAsc)
    {
    MPX_FUNC("CMPXDbHandler::GetSongsInBlockL");
    iDbMusic->GetSongsInBlockL(aAttrs, *aMediaArray, aTitle, aNumOfSongs, aAsc);
    }

// ----------------------------------------------------------------------------
//  Read songs at a particular offset
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::GetSongsAtOffsetL( CMPXMediaArray* aMediaArray,
                                       const TArray<TMPXAttribute>& aAttrs,
                                       TInt aOffset,
                                       TInt aCount )
    {
    MPX_DEBUG1("CMPXDbHandler::GetSongsAtOffsetL <--");
    iDbMusic->GetSongsAtOffsetL( *aMediaArray, aAttrs, aOffset, aCount );
    }

// ----------------------------------------------------------------------------
// Get all songs matching the given artist ID
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::GetSongsMatchingArtistL(
    TUint aArtistId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray* aMediaArray)
    {
    MPX_FUNC("CMPXDbHandler::GetSongsMatchingArtistL");
    iDbMusic->GetSongsForArtistL(aArtistId, aAttrs, *aMediaArray);
    }

// ----------------------------------------------------------------------------
// Get all songs matching the given album ID
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::GetSongsMatchingAlbumL(
    TUint aAlbumId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray* aMediaArray)
    {
    MPX_FUNC("CMPXDbHandler::GetSongsMatchingAlbumL");
    iDbMusic->GetSongsForAlbumL(aAlbumId, aAttrs, *aMediaArray);
    }

// ----------------------------------------------------------------------------------------------------------
// Get all songs matching the given artist and album ID
// ----------------------------------------------------------------------------------------------------------
//
void CMPXDbHandler::GetSongsMatchingArtistAndAlbumL(
    TUint aArtistId,
    TUint aAlbumId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray* aMediaArray)
    {
    MPX_FUNC("CMPXDbHandler::GetSongsMatchingArtistAndAlbumL");
    iDbMusic->GetSongsForArtistAndAlbumL(aArtistId, aAlbumId, aAttrs, *aMediaArray);
    }

// ----------------------------------------------------------------------------
// Get all songs matching the given genre ID
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::GetSongsMatchingGenreL(
    TUint aGenreId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray* aMediaArray)
    {
    MPX_FUNC("CMPXDbHandler::GetSongsMatchingGenreL");
    iDbMusic->GetSongsForGenreL(aGenreId, aAttrs, *aMediaArray);
    }

// ----------------------------------------------------------------------------
// Get all songs matching the given composer ID
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::GetSongsMatchingComposerL(
    TUint aComposerId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray* aMediaArray)
    {
    MPX_FUNC("CMPXDbHandler::GetSongsMatchingComposerL");
    iDbMusic->GetSongsForComposerL(aComposerId, aAttrs, *aMediaArray);
    }

// ----------------------------------------------------------------------------
// Get all songs that belong to the given playlist
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::GetSongsMatchingPlaylistL(
    TUint aPlaylistId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray* aMediaArray)
    {
    MPX_FUNC("CMPXDbHandler::GetSongsMatchingPlaylistL");
    GetPlaylistSongsL(aPlaylistId, aAttrs, *aMediaArray);
    }

// ----------------------------------------------------------------------------
// Get song matching the given song ID
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::GetSongL(
    TUint32 aSongId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbHandler::GetSongL");
    iDbMusic->GetSongL(aSongId, aAttrs, aMedia);
    }

// ----------------------------------------------------------------------------
// GetSongL
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::GetSongL(
    TUint32 aSongId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbHandler::GetSongL");

    CMPXMedia* media = CMPXMedia::NewL();
    CleanupStack::PushL(media);

    GetSongL(aSongId, aAttrs, *media);
    aMediaArray.AppendL(*media);

    CleanupStack::PopAndDestroy(media);
    }

// ----------------------------------------------------------------------------
// Get song matching the given song ID and playlist ID
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::GetPlaylistSongL(
    TUint32 aSongId,
    TUint32 aPlaylistId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aMedia)
    {
    MPX_DEBUG3("-->CMPXDbHandler::GetPlaylistSongL songId[0x%x] playlistId[0x%x]", aSongId, aPlaylistId);

    // complete the song information from the Music table
    MPX_TRAPD(err, iDbMusic->GetSongL(aSongId, aAttrs, aMedia));

    //
    // song not found in Music table
    //
    if (err == KErrNotFound)
        {
        //
        // Leave with KErrNotFound if one of the following is true:
        // 1) the requested song is in an auto playlist. Since auto-playlist isn't
        //    stored in playlist tables, we won't be able to retrieve info elsewhere
        // 2) the requested song is in a user playlist but we cannot find the song
        //    info from playlist tables either
        //
        if (EMPXNoAutoPlaylist != iAutoPlaylist->AutoPlaylistTypeL(aPlaylistId) ||
            !iDbPlaylist->Songs().GetSongL(aPlaylistId, aSongId, aAttrs, aMedia))
            {
            User::Leave(KErrNotFound);
            }
        }

    //
    // Unable to read information from Music table
    //
    else
        {
        // ignore the error if KErrNotFound
        User::LeaveIfError(err);
        }
    MPX_DEBUG1("<--CMPXDbHandler::GetPlaylistSongL");
    }

// ----------------------------------------------------------------------------
// GetPlaylistSongL
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::GetPlaylistSongL(
    TUint32 aSongId,
    TUint32 aPlaylistId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbHandler::GetPlaylistSongL");

    CMPXMedia* media = CMPXMedia::NewL();
    CleanupStack::PushL(media);

    GetPlaylistSongL(aSongId, aPlaylistId, aAttrs, *media);
    aMediaArray.AppendL(*media);

    CleanupStack::PopAndDestroy(media);
    }

// ----------------------------------------------------------------------------
// Get song matching the given URI
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbHandler::GetSongIdMatchingUriL(
    const TDesC& aUri)
    {
    MPX_FUNC("CMPXDbHandler::GetSongIdMatchingUriL");
    return MPXDbCommonUtil::GenerateUniqueIdL(iFs, EMPXCollection, aUri, EFalse);
    }

#ifdef ABSTRACTAUDIOALBUM_INCLUDED
// ----------------------------------------------------------------------------
// Get abstractalbum Id matching the given URI
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbHandler::GetAbstractAlbumIdMatchingUriL(
    const TDesC& aUri)
    {
    MPX_FUNC("CMPXDbHandler::GetAbstractAlbumIdMatchingUriL");
    return MPXDbCommonUtil::GenerateUniqueIdL(iFs, EMPXAbstractAlbum, aUri, EFalse);
    }
#endif // ABSTRACTAUDIOALBUM_INCLUDED

// ----------------------------------------------------------------------------
// Get all artists
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::GetAllArtistsL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray* aMediaArray)
    {
    MPX_FUNC("CMPXDbHandler::GetAllArtistsL");
    iDbArtist->GetAllCategoryItemsL(aAttrs, *aMediaArray);
    }

// ----------------------------------------------------------------------------
// Get all albums
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::GetAllAlbumsL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray* aMediaArray)
    {
    MPX_FUNC("CMPXDbHandler::GetAllAlbumsL");
    iDbAlbum->GetAllCategoryItemsL(aAttrs, *aMediaArray);
    }

// ----------------------------------------------------------------------------
// Get all albums for Media Wall
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::GetAllAlbumsMediaWallL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray* aMediaArray)
    {
    MPX_FUNC("CMPXDbHandler::GetAllAlbumsL");
    iDbAlbum->GetAllCategoryItemsMediaWallL(aAttrs, *aMediaArray);
    }

// ----------------------------------------------------------------------------
// Get all albums for the given artist ID
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::GetAlbumsMatchingArtistL(
    TUint aArtistId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbHandler::GetAlbumsMatchingArtistL");
    RArray<TMPXAttribute> attributes;
    CleanupClosePushL( attributes );

    TBool tryGetSongCount = EFalse;
    TInt attrCount(aAttrs.Count());
    TInt i = 0;
    for (i = 0; i < attrCount; i++)
        {
        TInt contentId(aAttrs[i].ContentId());
        TUint attributeId(aAttrs[i].AttributeId());

        if (contentId == KMPXMediaIdGeneral && attributeId & EMPXMediaGeneralCount)
            {
            MPX_DEBUG1("    EMPXMediaGeneralCount");

            attributes.AppendL(TMPXAttribute(KMPXMediaIdGeneral, attributeId & ~EMPXMediaGeneralCount));

            tryGetSongCount = ETrue;
            break;
            }

        attributes.AppendL(aAttrs[i]);
        }

    for (TInt j = i+1; j < attrCount; j++)
        {
        attributes.AppendL(aAttrs[j]);
        }
    iDbAlbum->GetSubCategoryItemsL(EMPXArtist, aArtistId, attributes.Array(), aMediaArray);
    CleanupStack::PopAndDestroy(&attributes);

    TInt pPath(0);
    if (aMediaArray.Count())
        {
        CMPXMedia* pMedia = aMediaArray[0];
        if (pMedia->IsSupported(KMPXMediaGeneralValue))
            {
            pPath = pMedia->ValueTObjectL<TInt>(KMPXMediaGeneralValue);
            MPX_ASSERT(pPath);
            }
        }

    TInt albumCount(aMediaArray.Count());
    if (albumCount)
        {
        if ( tryGetSongCount )
            {
            TInt startIndex = pPath ? 1 : 0;

            for (TInt i = startIndex; i < albumCount; i++)
                {
                CMPXMedia* media = aMediaArray[i];
                TUint32 albumId((media->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId)));

                TInt songCount = iDbAlbum->GetSongsCountInAlbumMatchingArtistL(aArtistId, albumId);

                media->SetTObjectValueL<TInt>(KMPXMediaGeneralCount, songCount );
                MPX_DEBUG2("    SongCount[%d]", songCount );
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// Get all genres
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::GetAllGenresL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray* aMediaArray)
    {
    MPX_FUNC("CMPXDbHandler::GetAllGenresL");
    iDbGenre->GetAllCategoryItemsL(aAttrs, *aMediaArray);
    }

// ----------------------------------------------------------------------------
// Get all composers
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::GetAllComposersL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray* aMediaArray)
    {
    MPX_FUNC("CMPXDbHandler::GetAllComposersL");
    iDbComposer->GetAllCategoryItemsL(aAttrs, *aMediaArray);
    }

// ----------------------------------------------------------------------------
// Get the playlist ID of the playlist that matches the given URI
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbHandler::GetPlaylistIdMatchingUriL(
    const TDesC& aUri)
    {
    MPX_FUNC("CMPXDbHandler::GetPlaylistIdMatchingUriL");
    return iDbPlaylist->GetIdL(aUri);
    }

// ----------------------------------------------------------------------------
// CMPXDbHandler::IsAutoPlaylistL
// ----------------------------------------------------------------------------
//
TBool CMPXDbHandler::IsAutoPlaylistL(
    TUint32 aPlaylistId)
    {
    MPX_FUNC("CMPXDbHandler::IsAutoPlaylistL");
    return (iAutoPlaylist->AutoPlaylistTypeL(aPlaylistId) != EMPXNoAutoPlaylist);
    }

// ----------------------------------------------------------------------------
// Get all user playlist names
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::GetAllPlaylistsL(
    CMPXMediaArray* aMediaArray,
    const TArray<TMPXAttribute>& aAttrs)
    {
    MPX_FUNC("CMPXDbHandler::GetAllPlaylistsL");
    iDbPlaylist->GetAllPlaylistsL(aAttrs, *aMediaArray);
    }

// ----------------------------------------------------------------------------
// Get all system playlist names
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::GetAllSystemPlaylistNamesL(
    CMPXMediaArray* aMediaArray)
    {
    MPX_FUNC("CMPXDbHandler::GetAllSystemPlaylistNamesL()");
    iAutoPlaylist->GetAllPlaylistsL(*aMediaArray);
    }

// ----------------------------------------------------------------------------
// Get the name of the row matching the given ID
// ----------------------------------------------------------------------------
//
HBufC* CMPXDbHandler::GetNameMatchingIdL(
    const TUint32 aId) const
    {
    MPX_FUNC("CMPXDbHandler::GetNameMatchingIdL()");

    HBufC* name(NULL);
    TMPXGeneralCategory category(MPX_ITEM_CATEGORY(aId));
    switch (category)
        {
        case EMPXCollection:
            {
            // song name
            name = iDbMusic->GetNameL(aId);
            break;
            }
        case EMPXPlaylist:
            {
            // playlist name
            if (iAutoPlaylist->AutoPlaylistTypeL(aId) != EMPXNoAutoPlaylist)
                {
                name = iAutoPlaylist->AutoPlaylistNameL(aId).AllocL();
                }
            else
                {
                name = iDbPlaylist->GetNameL(aId);
                }
            break;
            }
        default:
            {
            // category name
            name = DbCategoryL(category)->GetNameL(aId);
            break;
            }
        }

    return name;
    }

// ----------------------------------------------------------------------------
// Get the URI of the row matching the given ID
// ----------------------------------------------------------------------------
//
HBufC* CMPXDbHandler::GetUriMatchingIdL(
    const TUint32 aId) const
    {
    MPX_FUNC("CMPXDbHandler::GetUriMatchingIdL");

    HBufC* uri(NULL);
    switch (MPX_ITEM_CATEGORY(aId))
        {
        case EMPXCollection:
            {
            // song URI
            uri = iDbMusic->GetUriL(aId);
            break;
            }
        case EMPXPlaylist:
            {
            // playlist URI
            uri = iDbPlaylist->GetUriL(aId);
            break;
            }
        default:
            User::Leave(KErrNotSupported);
        }

    return uri;
    }

// ----------------------------------------------------------------------------
// Get category
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::GetCategoryL(
    const TUint32 aCategoryId,
    TMPXGeneralCategory aCategory,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia* aMedia)
    {
    MPX_FUNC("CMPXDbHandler::GetCategoryL");

    switch (aCategory)
        {
        case EMPXPlaylist:
            {
            if (iAutoPlaylist->AutoPlaylistTypeL(aCategoryId) != EMPXNoAutoPlaylist)
                {
                iAutoPlaylist->GetPlaylistL(aCategoryId, aAttrs, *aMedia);
                }
            else
                {
                iDbPlaylist->GetPlaylistL(aCategoryId, aAttrs, *aMedia);
                }
            break;
            }
        default:
            {
            DbCategoryL(aCategory)->GetCategoryItemL(aCategoryId, aAttrs, *aMedia);
            break;
            }
        }
    }

// ----------------------------------------------------------------------------
//  Get the duration for all songs
// ----------------------------------------------------------------------------
//
TInt CMPXDbHandler::GetAllSongsDurationL()
    {
    MPX_FUNC("CMPXDbHandler::GetAllSongsDurationL");
    return iDbMusic->AllSongsDurationL();
    }

// ----------------------------------------------------------------------------
//  Get the duration for an artist
// ----------------------------------------------------------------------------
//
TInt CMPXDbHandler::GetArtistDurationL(
    TInt aArtistId)
    {
    MPX_FUNC("CMPXDbHandler::GetArtistDurationL");
    return iDbMusic->ArtistDurationL(aArtistId);
    }

// ----------------------------------------------------------------------------
//  Get the duration for an album
// ----------------------------------------------------------------------------
//
TInt CMPXDbHandler::GetAlbumDurationL(
    TInt aAlbumId)
    {
    MPX_FUNC("CMPXDbHandler::GetAlbumDurationL");
    return iDbMusic->AlbumDurationL(aAlbumId);
    }

// ----------------------------------------------------------------------------
//  Get the duration for an artist/album combination
// ----------------------------------------------------------------------------
//
TInt CMPXDbHandler::GetArtistAlbumDurationL(
    TInt aArtistId,
    TInt aAlbumId)
    {
    MPX_FUNC("CMPXDbHandler::GetArtistAlbumDurationL");
    return iDbMusic->ArtistAlbumDurationL(aArtistId, aAlbumId);
    }

// ----------------------------------------------------------------------------
//  Get the duration for a composer
// ----------------------------------------------------------------------------
//
TInt CMPXDbHandler::GetComposerDurationL(
    TInt aComposerId)
    {
    MPX_FUNC("CMPXDbHandler::GetComposerDurationL");
    return iDbMusic->ComposerDurationL(aComposerId);
    }

// ----------------------------------------------------------------------------
//  Get the duration for a genre
// ----------------------------------------------------------------------------
//
TInt CMPXDbHandler::GetGenreDurationL(
    TInt aGenreId)
    {
    MPX_FUNC("CMPXDbHandler::GetGenreDurationL");
    return iDbMusic->GenreDurationL(aGenreId);
    }

// ----------------------------------------------------------------------------
//  Get the duration for a user playlist
// ----------------------------------------------------------------------------
//
TInt CMPXDbHandler::GetUserPlaylistDurationL(
    TInt aPlaylistId)
    {
    MPX_FUNC("CMPXDbHandler::GetUserPlaylistDurationL");

    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);
    attributes.AppendL(KMPXMediaGeneralId);
    attributes.AppendL(TMPXAttribute(KMPXMediaIdGeneral, EMPXMediaGeneralDuration));

    CMPXMediaArray* mediaArray = CMPXMediaArray::NewL();
    CleanupStack::PushL(mediaArray);

    GetPlaylistSongsL(aPlaylistId, attributes.Array(), *mediaArray);

    TInt duration(0);
    TInt count(mediaArray->Count());
    for (TInt index = 0; index < count; ++index)
        {
        CMPXMedia* media((*mediaArray)[index]);
        if (media->IsSupported(KMPXMediaGeneralDuration))
            {
            duration += media->ValueTObjectL<TInt>(KMPXMediaGeneralDuration);
            }
        }

    CleanupStack::PopAndDestroy(mediaArray);
    CleanupStack::PopAndDestroy(&attributes);

    return duration;
    }

// ----------------------------------------------------------------------------
//  Get the duration for a playlist
// ----------------------------------------------------------------------------
//
TInt CMPXDbHandler::GetPlaylistDurationL(
    TInt aPlaylistId)
    {
    MPX_FUNC("CMPXDbHandler::GetPlaylistDurationL");

    TInt duration(0);

    if (aPlaylistId == iAutoPlaylist->AutoPlaylistIdL(EMPXRecentlyPlayedPlaylist))
        {
        duration = iDbMusic->RecentlyPlayedDurationL();
        }
    else if (aPlaylistId == iAutoPlaylist->AutoPlaylistIdL(EMPXMostPlayedPlaylist))
        {
        duration = iDbMusic->MostPlayedDurationL();
        }
    else if (aPlaylistId == iAutoPlaylist->AutoPlaylistIdL(EMPXRecentlyAddedPlaylist))
        {
        duration = iDbMusic->RecentlyAddedDurationL();
        }
    else
        {
        duration = GetUserPlaylistDurationL(aPlaylistId);
        }

    return duration;
    }

// ----------------------------------------------------------------------------
// Find the number of items of a specified type
// ----------------------------------------------------------------------------
//
TInt CMPXDbHandler::NumberOfItemsL(
    TMPXGeneralCategory aCategory)
    {
    MPX_FUNC("CMPXDbHandler::NumberOfItemsL");

    TInt count(0);
    switch(aCategory)
        {
        case EMPXSong:
            {
            count = iDbMusic->CountL();
            break;
            }
        case EMPXPlaylist:
            {
            // return the total number of playlists, including the system ones
            count = iDbPlaylist->CountL() + EMPXAutoPlaylistCount;
            break;
            }
        default:
            {
            count = DbCategoryL(aCategory)->CountL();
            break;
            }
        }

    return count;
    }

// ----------------------------------------------------------------------------
// CMPXDbHandler::FindAllLC
// ----------------------------------------------------------------------------
//
CMPXMedia* CMPXDbHandler::FindAllLC(
    const CMPXMedia& aCriteria,
    const TArray<TMPXAttribute>& aAttrs)
    {
    MPX_FUNC("CMPXDbHandler::FindAllLC");

    // leave if the given media doesn't contain the following attributes
    if (!aCriteria.IsSupported(KMPXMediaGeneralType) ||
        !aCriteria.IsSupported(KMPXMediaGeneralCategory))
        {
        User::Leave(KErrArgument);
        }

    RArray<TInt> supportedIds;
    CleanupClosePushL(supportedIds);
    supportedIds.AppendL(KMPXMediaIdContainer);
    MPXDbCommonUtil::FillInSupportedUIDsL(aAttrs, supportedIds);

    CMPXMedia* entries = CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PopAndDestroy(&supportedIds);
    CleanupStack::PushL(entries);

    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);

    FindAllL(aCriteria, aAttrs, array);

    entries->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXGroup);
    entries->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory,
        aCriteria.ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory));
    entries->SetCObjectValueL(KMPXMediaArrayContents, array);
    entries->SetTObjectValueL<TInt>(KMPXMediaArrayCount, array->Count());

    CleanupStack::PopAndDestroy(array);
    return entries;
    }

// ----------------------------------------------------------------------------
// Set the last refreshed time into the collection
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::SetLastRefreshedTimeL(
    TTime aTime)
    {
    MPX_FUNC("CMPXDbHandler::SetLastRefreshedTimeL");

    BeginTransactionL();
    MPX_TRAPD(err, iDbAuxiliary->SetLastRefreshedTimeL(aTime));
    EndTransactionL(err);
    }

// ----------------------------------------------------------------------------
// Get the last refreshed time from the collection
// ----------------------------------------------------------------------------
//
TTime CMPXDbHandler::GetLastRefreshedTimeL()
    {
    MPX_FUNC("CMPXDbHandler::GetLastRefreshedTimeL");
    return iDbAuxiliary->LastRefreshedTimeL();
    }

// ----------------------------------------------------------------------------
// Set the db corrupted state for all drives
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::SetDBCorruptedL(
    TBool aCorrupted)
    {
    MPX_FUNC("CMPXDbHandler::SetDBCorruptedL");

    BeginTransactionL();
    MPX_TRAPD(err, iDbAuxiliary->SetDBCorruptedL(aCorrupted));
    EndTransactionL(err);
    }

// ----------------------------------------------------------------------------
// Gets the db corrupted state for all drives
// ----------------------------------------------------------------------------
//
TBool CMPXDbHandler::IsDBCorruptedL()
    {
    MPX_FUNC("CMPXDbHandler::IsDBCorruptedL");
    return iDbAuxiliary->DBCorruptedL();
    }

// ----------------------------------------------------------------------------
// Have the databases been created?
// ----------------------------------------------------------------------------
//
TBool CMPXDbHandler::DatabaseCreated()
    {
    MPX_FUNC("CMPXDbHandler::DatabaseCreatedL");

    TBool AuxilaryDbIsRefreshed(EFalse);
    TRAP_IGNORE(AuxilaryDbIsRefreshed = iDbAuxiliary->IsRefreshedL());
    // If none of the databases were available, ie out of disk we return EFalse
    return iDbManager->IsInitialized() && AuxilaryDbIsRefreshed;
    }

// ----------------------------------------------------------------------------
// Open a database
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::OpenDatabaseL(
    TInt aDrive)
    {
    MPX_FUNC("CMPXDbHandler::OpenDatabaseL");
    MPX_DEBUG2( "CMPXDbHandler::OpenDatabaseL: %i", aDrive);
    iDbManager->OpenDatabaseL(aDrive);

    // Verify the volume ID after a remount event
    VerifyVolumeIdL( aDrive );
    }

// ----------------------------------------------------------------------------
// Close a database
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::CloseDatabaseL(
    TInt aDrive)
    {
    MPX_FUNC("CMPXDbHandler::CloseDatabaseL");
    MPX_DEBUG2( "CMPXDbHandler::CloseDatabaseL drive: %i", aDrive );
    iDbManager->CloseDatabaseL(aDrive);
    }

// ----------------------------------------------------------------------------
// Re-create all databases
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::ReCreateDatabasesL()
    {
    MPX_FUNC("CMPXDbHandler::ReCreateDatabasesL");
    iDbManager->RecreateAllDatabasesL();
    }

// ----------------------------------------------------------------------------
// Set handler refresh status
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::RefreshStartL()
    {
    MPX_FUNC("CMPXDbHandler::RefreshStartL");

    iOutOfDisk = EFalse;
    // Re-open databases
    // This is needed for the case where we were OOD before, but user
    // has cleared some space but now try to refresh
    MPX_TRAPD(err, iDbManager->InitDatabasesL(iDbDrives));

    if(err == KErrDiskFull)
        {
            iOutOfDisk = ETrue;
        }

    User::LeaveIfError( err );
    	
    if(!iOutOfDisk)
    {
        MPX_TRAP(err,CheckDiskSpaceOnDrivesL());

        if(err == KErrDiskFull)
            {
            iOutOfDisk = ETrue;
            }
    }
#ifdef __RAMDISK_PERF_ENABLE
	iDbManager->CopyDBsToRamL();
#endif //__RAMDISK_PERF_ENABLE

    BeginTransactionL();
    iRefresh = ETrue;
    }

// ----------------------------------------------------------------------------
// Re-set handler refresh status
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::RefreshEndL()
    {
    MPX_FUNC("CMPXDbHandler::RefreshEndL");
    if ( iRefresh )
        { 
        iRefresh = EFalse;
        EndTransactionL(KErrNone);
        if (!iOutOfDisk)
            {
            // Write last refreshed time as current time
            // This also sets corrupt = 0
            TTime curTime;
            curTime.HomeTime();
            SetLastRefreshedTimeL(curTime);
            }

#ifdef ABSTRACTAUDIOALBUM_INCLUDED
    //for AbstractAlbum garbage collection
    TRAP_IGNORE( iDbAbstractAlbum->AbstractAlbumCleanUpL() );
#endif
   
#ifdef __RAMDISK_PERF_ENABLE
        iDbManager->CopyDBsFromRamL();
#endif //__RAMDISK_PERF_ENABLE
        }
    }

// ----------------------------------------------------------------------------
// Notification of Mtp status change
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::MtpStartL()
    {
    MPX_DEBUG1("-->CMPXDbHandler::MtpStartL");
    if(!iMtpInUse)
        {
        iMtpInUse = ETrue;
        iOpOnDbCount = 0;
    
    #ifdef __RAMDISK_PERF_ENABLE
        TRAPD(err, iDbManager->CopyDBsToRamL(iMtpInUse));
        if ( err != KErrNone )
            {
            MPX_DEBUG2("CMPXDbHandler::MtpStartL error=%d", err);
            }
    #endif //__RAMDISK_PERF_ENABLE
    
        iDbManager->BeginL();
        }
    //create Thumbnail manager session for cleanup for abstractalbum when MTP end.
    //because when MTP end comes, in case of mode switching, we need to do it as fast as possible, 
    //hence we don’t want the delay happens on that time.    
#ifdef RD_MPX_TNM_INTEGRATION           
    iDbAbstractAlbum->CreateTNMSessionL();
#endif  //RD_MPX_TNM_INTEGRATION
    MPX_DEBUG1("<--CMPXDbHandler::MtpStartL");
    }

// ----------------------------------------------------------------------------
// Notification of Mtp status change
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::MtpEndL()
    {
    MPX_DEBUG1("-->CMPXDbHandler::MtpEndL");
    iMtpInUse = EFalse;
    iOpOnDbCount = 0;
    iDbManager->CommitL();
    TInt err = KErrNone;


#ifdef __RAMDISK_PERF_ENABLE
    TRAP(err, iDbManager->CopyDBsFromRamL());
    if ( err != KErrNone )
        {
        MPX_DEBUG2("CMPXDbHandler::MtpEndL error=%d", err);
        }
#endif //__RAMDISK_PERF_ENABLE

#ifdef ABSTRACTAUDIOALBUM_INCLUDED
    BeginTransactionL();
    TRAP(err, iDbAbstractAlbum->RemoveAbstractAlbumsWithNoSongL());
    if ( err != KErrNone )
        {
        MPX_DEBUG2("CMPXDbHandler::MtpEndL error happened when cleanup albstractalbum with no songs association[%d]", err);
        }
    EndTransactionL(err);
#endif
    MPX_DEBUG1("<--CMPXDbHandler::MtpEndL");
    }

// ----------------------------------------------------------------------------
// Get all records count for music
// ----------------------------------------------------------------------------
//
TUint CMPXDbHandler::GetMusicCountL(TInt aDrive)
    {
    MPX_FUNC("CMPXDbHandler::GetMusicCountL");
    TUint total(0);

    //music
    total += iDbMusic->GetDriveTrackCountL(aDrive);

    return total;
    }

// ----------------------------------------------------------------------------
// Get all records count for playlists
// ----------------------------------------------------------------------------
//
TUint CMPXDbHandler::GetPlaylistCountL(TInt aDrive)
    {
    MPX_FUNC("CMPXDbHandler::GetPlaylistCountL");
    TUint total(0);

    //playlist
    total += iDbPlaylist->GetDrivePlaylistCountL(aDrive);

    return total;
    }

// ----------------------------------------------------------------------------
// Get all records count for music and playlists
// ----------------------------------------------------------------------------
//
TUint CMPXDbHandler::GetTotalCountL(TInt aDrive)
    {
    MPX_FUNC("CMPXDbHandler::GetTotalCountL");
    TUint total(0);

    //music
    total += iDbMusic->GetDriveTrackCountL(aDrive);
    //playlist
    total += iDbPlaylist->GetDrivePlaylistCountL(aDrive);

    return total;
    }

// ----------------------------------------------------------------------------
// Get all records count for music and playlists
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::GetMusicUriArrayL(TInt aDrive, TInt aFromID, TInt aRecords,
                                      CDesCArray& aUriArr, TInt& aLastID)
    {
    MPX_FUNC("CMPXDbHandler::GetMusicUriArrayL");

    iDbMusic->GetMusicUriArrayL(aDrive,aFromID,aRecords,aUriArr,aLastID);
    }

// ----------------------------------------------------------------------------
// Get all records count for music and playlists
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::GetPlaylistUriArrayL(TInt aDrive, TInt aFromID, TInt aRecords,
                                         CDesCArray& aUriArr, TInt& aLastID)
    {
    MPX_FUNC("CMPXDbHandler::GetPlaylistUriArrayL");

    iDbPlaylist->GetPlaylistUriArrayL(aDrive,aFromID,aRecords,aUriArr,aLastID);
    }

// ----------------------------------------------------------------------------
// Starts a transaction on all databases
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::BeginTransactionL()
    {
    MPX_FUNC("CMPXDbHandler::BeginTransactionL");

#ifdef __RAMDISK_PERF_ENABLE
    // EnsureRamSpaceL will copy dbs from ram if ram space is low or dbs exceeded
    // max space.
	TRAPD(err, iDbManager->EnsureRamSpaceL());
	if (err)
		{
		//error but continue
		}
#endif //__RAMDISK_PERF_ENABLE

    if(!iMtpInUse)
        {
        iDbManager->BeginL();
        }
    }

// ----------------------------------------------------------------------------
// Ends a transaction on all databases
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::EndTransactionL(
    TInt aError)
    {
    MPX_FUNC("CMPXDbHandler::EndTransactionL");

    if(iMtpInUse)
        {
        if (aError)
            {
            iOpOnDbCount = 0;
            iDbManager->RollbackL();
            User::Leave(aError);
            }

        if(iOpOnDbCount >= KMaxOpInTransaction)
            {
            MPX_DEBUG2("CMPXDbHandler::EndTransactionL - %d>KMaxOpInTransaction Commiting",iOpOnDbCount);
            iOpOnDbCount = 0;
            iDbManager->CommitL();
            iDbManager->BeginL();
            }
        }
    else
        {
        if (aError)
            {
            MPX_DEBUG2("CMPXDbHandler::EndTransactionL - Rollback [%d]", aError);

            iDbManager->RollbackL();

            // KSqlDbCorrupted indicates DB corrupted, need to recreate.
            if ( aError != KSqlDbCorrupted )
                {
                User::Leave(aError);
                }
            }
        else
            {
            iDbManager->CommitL();
            }
        }
    }

// ----------------------------------------------------------------------------
// Checks if the database is currently in a transaction
// ----------------------------------------------------------------------------
//
TBool CMPXDbHandler::InTransaction()
    {
    MPX_FUNC("CMPXDbHandler::InTransaction");
    return iDbManager->InTransaction();
    }

// ----------------------------------------------------------------------------
// Notifies the handler that the collection will be closed.
// It is called before closing the collection.
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::PreCloseCollectionL()
    {
    MPX_FUNC("CMPXDbHandler::PreCloseCollectionL");
    // Complete pending transaction and set the latest refresh time
    // before closing the databases if collection close event occurs
    // before the end of the refresh operation
    if(iRefresh)
        {
        EndTransactionL(KErrNone);
        if (!iOutOfDisk)
            {
            // Write last refreshed time as current time
            // This also sets corrupt = 0
            TTime curTime;
            curTime.HomeTime();
            SetLastRefreshedTimeL(curTime);
            }
        }
    }

// ----------------------------------------------------------------------------
// Add song to collection
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbHandler::DoAddSongL(
    const CMPXMedia& aMedia,
    CMPXMessageArray* aMessageArray)
    {
    MPX_FUNC("CMPXDbHandler::DoAddSongL");

    if (!aMedia.IsSupported(KMPXMediaGeneralUri))
        {
        User::Leave(KErrArgument);
        }

    // add the song to the Music table
    TDriveUnit drive(aMedia.ValueText(KMPXMediaGeneralUri));
    TUint32 songId(iDbMusic->AddSongL(aMedia, drive,
                                      iRefresh?NULL:aMessageArray));

    // update the playlist tables
    // make sure the song db flags are reset and not just updated
    iDbPlaylist->UpdateSongL(aMedia, ETrue);

    if(iMtpInUse)
        {
        ++iOpOnDbCount;
        }

    return songId;
    }

// ----------------------------------------------------------------------------
// Add playlist to collection
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbHandler::DoAddPlaylistL(
    const CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbHandler::DoAddPlaylistL");

    CMPXMedia* media = CMPXMedia::NewL(aMedia);
    CleanupStack::PushL(media);

    const TDesC& playlistUri = media->ValueText(KMPXMediaGeneralUri);

    //
    // client has asked to create a virtual playlist if file name is not specified
    // in the URI. In this case, generate URI for virtual playlist specified as
    // follows:
    //          <dir path>\timestamp.vir
    //
    TParsePtrC parser(playlistUri);

    if (!parser.NameOrExtPresent() ||
        parser.IsNameWild() ||
        parser.IsExtWild())
        {
        HBufC* newUri = HBufC::NewLC(parser.DriveAndPath().Length() + KMCMaxTextLen +
            KMPXVirtualPlaylistExt().Length());

        TPtr ptr = newUri->Des();
        ptr.Append(parser.DriveAndPath());
        TTime time;
        time.HomeTime();
        ptr.AppendNum(time.Int64());
        ptr.Append(KMPXVirtualPlaylistExt);

        // overwrite the old uri with the new one with full path and playlist
        // playlist filename
        media->SetTextValueL(KMPXMediaGeneralUri, *newUri);

        CleanupStack::PopAndDestroy(newUri);

        // set DbFlags to indicate that this is a virtual playlist
        media->SetTObjectValueL<TUint>(KMPXMediaGeneralFlags,
            KMPXMediaGeneralFlagsSetOrUnsetBit | KMPXMediaGeneralFlagsIsVirtual);
        }

    // complete the song attributes from the music table
    UpdatePlaylistSongInfoL(*media);

    // add playlist to the database
    TUint32 playlistId = iDbPlaylist->AddPlaylistL(*media);

    CleanupStack::PopAndDestroy(media);
    return playlistId;
    }

// ----------------------------------------------------------------------------
// Add song to playlist
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbHandler::DoAddSongToPlaylistL(
    const CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbHandler::DoAddSongToPlaylistL");

    CMPXMedia* media = CMPXMedia::NewL(aMedia);
    CleanupStack::PushL(media);

    // complete the song attributes from the music table
    UpdatePlaylistSongInfoL(*media);

    // add the songs to the playlist
    TUint32 playlistId((media->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId)).iId2);

    CMPXMediaArray* ary( aMedia.Value<CMPXMediaArray>(KMPXMediaArrayContents) );
    User::LeaveIfNull( ary );
    iDbPlaylist->AddSongsL(playlistId,*ary);

    CleanupStack::PopAndDestroy(media);
    return playlistId;
    }

// ----------------------------------------------------------------------------
// Update a song in the collection
// ----------------------------------------------------------------------------
//
CMPXDbActiveTask::TChangeVisibility CMPXDbHandler::DoUpdateSongL(
    const CMPXMedia& aMedia,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbHandler::DoUpdateSongL");

    CMPXDbActiveTask::TChangeVisibility visibleChange(CMPXDbActiveTask::ENotVisibile);

    TUint32 songId(0);

    if (aMedia.IsSupported(KMPXMediaGeneralId))
        {
        songId = (aMedia.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId)).iId2;
        }
    if (aMedia.IsSupported(KMPXMediaGeneralUri))
        {
        const TDesC& uri = aMedia.ValueText(KMPXMediaGeneralUri);
        songId = MPXDbCommonUtil::GenerateUniqueIdL(iFs, EMPXCollection, uri, EFalse);
        }
    if (!songId)
        {
        User::Leave(KErrNotSupported);
        }

    // Update the Music table
    TRAPD(err, visibleChange = iDbMusic->UpdateSongL(songId, aMedia, aItemChangedMessages));

    // do not leave if song is not found in Music table
    // leave for other errors such as disk full
    if(err != KErrNone && err != KErrNotFound)
        {
        User::Leave(err);
        }

    // Update the Playlist table
    TBool visible = EFalse;

    TRAP( err, visible = iDbPlaylist->UpdateSongL(aMedia, EFalse, &aItemChangedMessages));

    // do not leave if song is not found in Playlist table
    // leave for other errors such as disk full
    if(err != KErrNone && err != KErrNotFound)
        {
        User::Leave(err);
        }

    // make it visible if either table is updated
    if (visible)
        {
        visibleChange = CMPXDbActiveTask::EAllVisible;
        }
    if ( aMedia.ValueTObjectL<TInt>( KMPXMediaMusicAlbumArtChanged )== 1 )
        {
        ( const_cast<CMPXMedia*>( &aMedia ) 
        		)->SetTObjectValueL<TInt>( KMPXMediaMusicAlbumArtChanged, 0 );
        }

    return visibleChange;
    }

// ----------------------------------------------------------------------------
// Update a playlist in the collection
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::DoUpdatePlaylistL(
    const CMPXMedia& aMedia,
    CMPXMessageArray& aMessageArray)
    {
    MPX_FUNC("CMPXDbHandler::DoUpdatePlaylistL");

    TUint32 playlistId(0);
    TInt drive(0);

    CMPXMedia* media = CMPXMedia::NewL(aMedia);
    CleanupStack::PushL(media);


    ProcessPlaylistMediaL(*media, playlistId, drive);

    CMPXMessage* m1 = CMPXMessage::NewL();
    CleanupStack::PushL(m1);
    CMPXMessage* m2 = CMPXMessage::NewL();
    CleanupStack::PushL(m2);

    // send 2 messages to notify the playlist change & to refresh the display (update playlist name)
    MPXDbCommonUtil::FillItemChangedMessageL(*m1, playlistId, EMPXItemModified,
            EMPXPlaylist, KDBPluginUid);

    MPXDbCommonUtil::FillItemChangedMessageL(*m2, EBrowsePlaylist, EMPXItemModified,
                EMPXPlaylist, KDBPluginUid);

    iDbPlaylist->UpdatePlaylistL(*media, *m1, drive);

    aMessageArray.AppendL(*m1);
    aMessageArray.AppendL(*m2);


    CleanupStack::PopAndDestroy(m2);
    CleanupStack::PopAndDestroy(m1);
    CleanupStack::PopAndDestroy(media);
    }

// ----------------------------------------------------------------------------
// Update a playlist in the collection
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::DoUpdatePlaylistSongsL(
    const CMPXMedia& aMedia,
    CMPXMessage& aMessage)
    {
    MPX_FUNC("CMPXDbHandler::DoUpdatePlaylistSongsL");

    CMPXMedia* media = CMPXMedia::NewL(aMedia);
    CleanupStack::PushL(media);

    TUint32 playlistId(0);
    TInt drive(0);

    // get the playlist ID and drive ID
    ProcessPlaylistMediaL(*media, playlistId, drive);
    MPXDbCommonUtil::FillItemChangedMessageL(aMessage, playlistId, EMPXItemModified,
        EMPXPlaylist, KDBPluginUid);

    // complete the song attributes from the Music table
    UpdatePlaylistSongInfoL(*media);

    // delete existing songs for the playlist first
    iDbPlaylist->Songs().DeleteSongsL(playlistId, drive);

    // add new songs to the playlist
    CMPXMediaArray* ary( media->Value<CMPXMediaArray>(KMPXMediaArrayContents ) );
    User::LeaveIfNull( ary );
    iDbPlaylist->AddSongsL(playlistId, *ary);

    CleanupStack::PopAndDestroy(media);
    }

// ----------------------------------------------------------------------------
// Reorder a song in a playlist
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::DoReorderPlaylistL(
    const TMPXItemId& aPlaylistId,
    const TMPXItemId& aSongId,
    TUint aOriginalOrdinal,
    TUint aNewOrdinal,
    CMPXMessage& aMessage)
    {
    MPX_DEBUG1("-->CMPXDbHandler::DoReorderPlaylistL()");

    if (aOriginalOrdinal != aNewOrdinal)
        {
        iDbPlaylist->Songs().ReorderSongL(aPlaylistId, aSongId, aOriginalOrdinal, aNewOrdinal);

        MPXDbCommonUtil::FillItemChangedMessageL(aMessage, aPlaylistId.iId2, EMPXItemModified,
            EMPXPlaylist, KDBPluginUid);
        }

    MPX_DEBUG1("<--CMPXDbHandler::DoReorderPlaylistL()");
    }

// ----------------------------------------------------------------------------
// Delete a song from collection
// The function notifies collection model to perform deletion
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::DoRemoveSongL(
    TUint32 aSongId,
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages,
    TBool aDeleteRecord)
    {
    MPX_FUNC("CMPXDbHandler::DoRemoveSongL");

    // Get the song drive
    TUint32 artistID(0);
    TUint32 albumID(0);
    TUint32 genreID(0);
    TUint32 composerID(0);
    HBufC*  art(NULL);
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
    TUint32 abstractAlbumID(0);
#endif // ABSTRACTAUDIOALBUM_INCLUDED
    TInt drive(0);

// Get information from the Music table first
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
    HBufC* uri = iDbMusic->GetSongInfoL(aSongId, artistID, albumID, genreID, composerID, abstractAlbumID, drive, art);
#else
    HBufC* uri = iDbMusic->GetSongInfoL(aSongId, artistID, albumID, genreID, composerID, drive, art);
#endif // ABSTRACTAUDIOALBUM_INCLUDED

    CleanupStack::PushL(art);
    // add the URI to the return array
    CleanupStack::PushL(uri);
    aUriArray.AppendL(*uri);
    CleanupStack::PopAndDestroy(uri);

    // Update the category records
    TBool categoryExist( EFalse );
    iDbArtist->DecrementSongsForCategoryL(artistID, drive, &aItemChangedMessages, categoryExist);
    iDbAlbum->DecrementSongsForAlbumL(albumID, drive, &aItemChangedMessages, categoryExist, *art);
    iDbGenre->DecrementSongsForCategoryL(genreID, drive, &aItemChangedMessages, categoryExist);
    iDbComposer->DecrementSongsForCategoryL(composerID, drive, &aItemChangedMessages, categoryExist);
    CleanupStack::PopAndDestroy(art);
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
    if (abstractAlbumID)
        {
        iDbAbstractAlbum->DecrementSongsForCategoryL(abstractAlbumID, drive, &aItemChangedMessages, categoryExist, iMtpInUse);
        }
#endif // ABSTRACTAUDIOALBUM_INCLUDED
    // Update the music table
    TBool deleteRecord(ETrue);

#if defined (__MTP_PROTOCOL_SUPPORT)
    // Mark the song record as deleted if the following is true; otherwise, delete the
    // song record.
    //
    // A client other than MTP has initiated this song deletion (aDeleteRecord is EFalse)
    // and MTP has turned on its cenrep key to save deleted records and current number of
    // saved deleted records has not exceeded its maximum, KMCMaxSavedDeletedRecords.
    //
    // Songs are marked as deleted in order to support auto-sync. MTP will delete these
    // marked records at the end of each session via CleanupDeletedRecordsL.
    //
    // For performance consideration, if the number of saved records exceeds its maximum,
    // song record will be deleted.
    if (!aDeleteRecord && SaveDeletedSongs())
        {
        TUint32 savedDeletedRecordCount(iDbAuxiliary->SaveDeletedRecordCountL());
        MPX_DEBUG2("Current number of saved deleted record count is %d", savedDeletedRecordCount);

        if (savedDeletedRecordCount < KMCMaxSavedDeletedRecords)
            {
            deleteRecord = EFalse;
            TUint32 savedDeletedDriveRecordCount(iDbAuxiliary->SaveDeletedRecordCountL(drive));
            iDbAuxiliary->SetSaveDeletedRecordCountL(drive,++savedDeletedDriveRecordCount);
            }
        }
#endif

    // delete the song from the Music table
    iDbMusic->DeleteSongL(aSongId, drive, deleteRecord);

    // add the item changed message
    MPXDbCommonUtil::AddItemChangedMessageL(aItemChangedMessages, aSongId, EMPXItemDeleted,
        EMPXSong, KDBPluginUid);


    if(iMtpInUse)
        {
        ++iOpOnDbCount;
        }
    }

// ----------------------------------------------------------------------------
// Delete a song from playlist tables
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::DoRemoveSongFromPlaylistL(TUint32 aSongId,CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbHandler::DoRemoveSongFromPlaylistL");
    // delete song from the playlist tables on all drives
    iDbPlaylist->DeleteSongL(aSongId, aItemChangedMessages);
    }

// ----------------------------------------------------------------------------
// Removes a category of songs from the music collection,
// and its corresponding category in the lookup table
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::DoRemoveSongsMatchingCategoryL(
    TMPXGeneralCategory aCategory,
    TUint32 aCategoryId,
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbHandler::DoRemoveSongsMatchingCategoryL");

    // get the songs for the specified category
    CMPXMediaArray* songs = CMPXMediaArray::NewL();
    CleanupStack::PushL(songs);

    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);
    attributes.AppendL(KMPXMediaGeneralId);

    switch (aCategory)
        {
        case EMPXArtist:
            {
            iDbMusic->GetSongsForArtistL(aCategoryId, attributes.Array(), *songs);
            break;
            }
        case EMPXAlbum:
            {
            iDbMusic->GetSongsForAlbumL(aCategoryId, attributes.Array(), *songs);
            break;
            }
        case EMPXGenre:
            {
            iDbMusic->GetSongsForGenreL(aCategoryId, attributes.Array(), *songs);
            break;
            }
        case EMPXComposer:
            {
            iDbMusic->GetSongsForComposerL(aCategoryId, attributes.Array(), *songs);
            break;
            }
        default:
            User::Leave(KErrNotSupported);
        }

    CleanupStack::PopAndDestroy(&attributes);

    // iterate the songs and remove them one by one
    // so records in the category tables can also be updated
    TInt count(songs->Count());
    for (TInt index = 0; index < count; ++index)
        {
        CMPXMedia* song = (*songs)[index];
        if (song->IsSupported(KMPXMediaGeneralId))
            {
            DoRemoveSongL((song->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId)).iId2,
                aUriArray, aItemChangedMessages, EFalse);
            DoRemoveSongFromPlaylistL((song->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId)).iId2,aItemChangedMessages);
            }
        }

    CleanupStack::PopAndDestroy(songs);
    }

// ----------------------------------------------------------------------------------------------------------
// Delete songs for the specified artist and album from collection
// ----------------------------------------------------------------------------------------------------------
//
void CMPXDbHandler::DoRemoveSongsMatchingArtistAndAlbumL(
    TUint32 aArtistId,
    TUint32 aAlbumId,
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbHandler::RemoveSongsMatchingArtistAndAlbumL");

    // get the songs for the specified artist and album
    CMPXMediaArray* songs = CMPXMediaArray::NewL();
    CleanupStack::PushL(songs);

    RArray<TMPXAttribute> attributes;
    CleanupClosePushL (attributes);
    attributes.AppendL(KMPXMediaGeneralId);

    iDbMusic->GetSongsForArtistAndAlbumL(aArtistId, aAlbumId, attributes.Array(), *songs);
    CleanupStack::PopAndDestroy(&attributes);

    // iterate the songs and remove them one by one
    // so records in the category tables can also be updated
    TInt count(songs->Count());
    for (TInt index = 0; index < count; ++index)
        {
        CMPXMedia* song = (*songs)[index];
        if (song->IsSupported(KMPXMediaGeneralId))
            {
            DoRemoveSongL( song->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId),
                aUriArray, aItemChangedMessages, EFalse);
            DoRemoveSongFromPlaylistL(song->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId),aItemChangedMessages);
            }
        }

    CleanupStack::PopAndDestroy(songs);
    }

// ----------------------------------------------------------------------------
// Remove all playlists from collection
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::DoRemoveAllPlaylistsL()
    {
    MPX_FUNC("CMPXDbHandler::DoRemoveAllPlaylistsL");
    iDbPlaylist->DeleteAllPlaylistsL();
    }

// ----------------------------------------------------------------------------
// Remove specified playlist
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::DoRemovePlaylistL(
    TUint32 aPlaylistId,
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbHandler::DoRemovePlaylistL");

    HBufC* uri(iDbPlaylist->DeletePlaylistL(aPlaylistId));
    if (uri)
        {
        CleanupStack::PushL(uri);
        aUriArray.AppendL(*uri);
        CleanupStack::PopAndDestroy(uri);
        }

    MPXDbCommonUtil::AddItemChangedMessageL(aItemChangedMessages, aPlaylistId, EMPXItemDeleted,
        EMPXPlaylist, KDBPluginUid);
    }

// ----------------------------------------------------------------------------
// Remove song from playlist songs table
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::DoRemoveSongFromPlaylistL(
    TUint32 aPlaylistId,
    const TMPXItemId& aSongId,
    TInt aOrdinal,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbHandler::DoRemoveSongFromPlaylistL");
    MPX_DEBUG5("CMPXDbHandler::DoRemoveSongFromPlaylistL(playlist 0x%x, songId [0x%x,0x%x], ordinal %d)",
        aPlaylistId, aSongId.iId1, aSongId.iId2, aOrdinal);

    // delete the song
    iDbPlaylist->DeleteSongL(aPlaylistId, aSongId.iId2, aOrdinal);

    // Send a playlist modified message
    MPXDbCommonUtil::AddItemChangedMessageL(aItemChangedMessages, aPlaylistId, EMPXItemModified,
        EMPXPlaylist, KDBPluginUid);

    // Send a message on the song in the playlist that is deleted
    MPXDbCommonUtil::AddItemChangedMessageL(aItemChangedMessages, aSongId, EMPXItemDeleted,
        EMPXSong, KDBPluginUid);
    }
    

// ----------------------------------------------------------------------------
// CMPXDbHandler::DoCleanupDeletedRecordsL
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::DoCleanupDeletedRecordsL()
    {
    MPX_FUNC("CMPXDbHandler::DoCleanupDeletedRecordsL");

    // delete all marked records from the Music table
    iDbMusic->CleanupL();

    // reset the count in the Auxiliary table
    iDbAuxiliary->SetSaveDeletedRecordCountL(KDbManagerAllDrives,0);
    }

// ----------------------------------------------------------------------------
// FindAllL
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::FindAllL(
    const CMPXMedia& aCriteria,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray* aMediaArray)
    {
    MPX_FUNC("CMPXDbHandler::FindAllL");

    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);
    MPXUser::MergeAttributeL(aAttrs, attributes);

    TMPXGeneralCategory category = aCriteria.ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory);
    switch (category)
        {
        case EMPXPlaylist:
            {
            TUint32 playlistId(0);
            if (aCriteria.IsSupported(KMPXMediaGeneralId))
                {
                playlistId = (aCriteria.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId)).iId2;
                }

            if (iAutoPlaylist->AutoPlaylistTypeL(playlistId) != EMPXNoAutoPlaylist)
                {
                CMPXMedia* media = CMPXMedia::NewL();
                CleanupStack::PushL(media);

                iAutoPlaylist->GetPlaylistL(playlistId, aAttrs, *media);

                aMediaArray->AppendL(*media);
                CleanupStack::PopAndDestroy(media);
                }
            else
                {
                iDbPlaylist->FindAllL(aCriteria, attributes.Array(), *aMediaArray);
                }

            break;
            }
        case EMPXSong:
            {
            FindSongL(aCriteria, attributes.Array(), *aMediaArray);
            break;
            }
        default:
            {
            DbCategoryL(category)->FindAllL(aCriteria, attributes.Array(), *aMediaArray);
            break;
            }
        }

    CleanupStack::PopAndDestroy(&attributes);
    }

// ----------------------------------------------------------------------------
// Get song(s) from the music table that match the given criteria
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::FindSongL(
    const CMPXMedia& aCriteria,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbCollection::FindSongL");

    TMPXGeneralType type = aCriteria.ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType);

    TUint32 id(0);
    if (aCriteria.IsSupported(KMPXMediaGeneralId))
        {
        id = (aCriteria.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId)).iId2;
        }

    TUint32 containerId(0);
    if (aCriteria.IsSupported(KMPXMediaGeneralContainerId))
        {
        containerId = aCriteria.ValueTObjectL<TUint32>(KMPXMediaGeneralContainerId);
        }

    //////////////////////////////////////////////////////////////////////
    // Find songs in the specified playlist
    //////////////////////////////////////////////////////////////////////
    TMPXGeneralCategory cat(MPX_ITEM_CATEGORY(id));

    if (type == EMPXGroup &&
        (cat == EMPXPlaylist ||
        MPX_ITEM_CATEGORY(containerId) == EMPXPlaylist))
        {
        TUint32 playlistId = (cat == EMPXPlaylist) ?
            id : (containerId & KMCCategoryMask);

        GetPlaylistSongsL(playlistId, aAttrs, aMediaArray);
        }
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
    else if (type == EMPXGroup &&
        (cat == EMPXAbstractAlbum ||
        MPX_ITEM_CATEGORY(containerId) == EMPXAbstractAlbum))
        {
        TUint32 abstractAlbumId = (cat == EMPXAbstractAlbum) ?
            id : (containerId & KMCCategoryMask);
        TInt attrCount(aAttrs.Count());
        if ( attrCount > 1 || (attrCount == 1 && !(aAttrs[0] == KMPXMediaGeneralId)) )
            {
            MPX_TRAPD(err, iDbMusic->GetAllSongsForAbstractAlbumL( abstractAlbumId, aAttrs, aMediaArray));
        if (err == KErrNotFound)
            {
            //
            // Leave with KErrNotFound
            User::Leave(KErrNotFound);
            }
        else
            {
            // Leave if error
            User::LeaveIfError(err);
            }
        }
    }
#endif // ABSTRACTAUDIOALBUM_INCLUDED
    //////////////////////////////////////////////////////////////////////
    // Find a particular song in the specified playlist. This fills the
    // song with info from Playlist table first then overwrites it with
    // info from Songs table if Songs table where this song is located
    // is present in order to support the display of song titles in a
    // playlist when memory card is removed if the playlist refers to
    // songs on the memory card. Caller of this scenario is OpenL/MediaL.
    // When user attempts to play a track in an auto-playlist, we will
    // find the song from Songs table directly since auto-playlists are
    // not stored in the Playlist table. Auto-playlists are query-based,
    // therefore, when memory card is removed, songs on the memory card
    // will not be shown in the auto-playlist; hence they do not exhibit
    // the same challenge as user created playlists.
    //////////////////////////////////////////////////////////////////////
    else if (type == EMPXItem &&
        cat == EMPXCollection &&
        MPX_ITEM_CATEGORY(containerId) == EMPXPlaylist)
        {
        if (iAutoPlaylist->AutoPlaylistTypeL(containerId) != EMPXNoAutoPlaylist)
            {
            // auto playlist song, get the song details from the music table
            iDbMusic->FindSongsL(id, 0, type, aCriteria, aAttrs, aMediaArray);
            }
        else
            {
            GetPlaylistSongL(id, containerId, aAttrs, aMediaArray);
            }
        }

    //////////////////////////////////////////////////////////////////////
    // Find all songs, all songs in a particular album and/or artist, or
    // a particular song
    //////////////////////////////////////////////////////////////////////
    else
        {
        TBool sortByTrackOrder = EFalse;
		
        // construct the sort order depending on category. Albums are always sorted by track,
        // then name, except for unknown album. Songs are sorted by name for unknown album.
        // NULL track number is stored as KMaxTInt so that they will be sorted to the end
        if ((type == EMPXGroup) && (MPX_ITEM_CATEGORY(id) == EMPXAlbum) && !iDbAlbum->IsUnknownAlbumL(id))
            {
            sortByTrackOrder = ETrue;
            }
        iDbMusic->FindSongsL(id, containerId, type, aCriteria, aAttrs, aMediaArray, sortByTrackOrder);
        }
    }

// ----------------------------------------------------------------------------
// Get song(s) in the specified playlist
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::GetPlaylistSongsL(
    TUint32 aPlaylistId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbHandler::GetPlaylistSongsL");
    MPX_DEBUG2("CMPXDbHandler::GetPlaylistSongsL(0x%x)", aPlaylistId);

    // check the auto playlists first
    if (aPlaylistId == iAutoPlaylist->AutoPlaylistIdL(EMPXRecentlyPlayedPlaylist))
        {
        iDbMusic->GetRecentlyPlayedSongsL(aAttrs, aMediaArray);
        }
    else if (aPlaylistId == iAutoPlaylist->AutoPlaylistIdL(EMPXMostPlayedPlaylist))
        {
        iDbMusic->GetMostPlayedSongsL(aAttrs, aMediaArray);
        }
    else if (aPlaylistId == iAutoPlaylist->AutoPlaylistIdL(EMPXRecentlyAddedPlaylist))
        {
        iDbMusic->GetRecentlyAddedSongsL(aAttrs, aMediaArray);
        }
    else
        {
        TInt attrCount(aAttrs.Count());
        if ( attrCount > 1 || (attrCount == 1 && !(aAttrs[0] == KMPXMediaGeneralId)) )
            {
	        TInt plDrive(iDbPlaylist->GetDriveIdL(aPlaylistId));
	        MPX_TRAPD(err, iDbMusic->GetAllSongsL(plDrive, aPlaylistId, aAttrs, aMediaArray));

	        // song not found in Music table
	        if (err == KErrNotFound)
	            {
	            //
	            // Leave with KErrNotFound if one of the following is true:
	            // 1) the requested song is in an auto playlist. Since auto-playlist isn't
	            //    stored in playlist tables, we won't be able to retrieve info elsewhere
	            // 2) the requested song is in a user playlist but we cannot find the song
	            //    info from playlist tables either
	            //
	           if (EMPXNoAutoPlaylist != iAutoPlaylist->AutoPlaylistTypeL(aPlaylistId) ||
	                !iDbPlaylist->Songs().GetSongsL(aPlaylistId, aAttrs, aMediaArray))
	               {
	               User::Leave(KErrNotFound);
	               }
	            }
	        else
	            {
	            // ignore the error if KErrNotFound
	            User::LeaveIfError(err);
	            }
            }
        else
            {
            // get ids of the songs in the playlist
            iDbPlaylist->Songs().GetSongsL(aPlaylistId, aMediaArray);
            }
        }
    }

// ----------------------------------------------------------------------------
// Find all albums or the albums for a specified artist
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::FindAlbumL(
    const CMPXMedia& aCriteria,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbHandler::FindAlbumL");

    TMPXGeneralType type = aCriteria.ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType);

    TUint32 id(0);
    if (aCriteria.IsSupported(KMPXMediaGeneralId))
        {
        id = aCriteria.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
        }

    if ((type == EMPXGroup) && (MPX_ITEM_CATEGORY(id) == EMPXArtist))
        {
        // get all the albums for the artist
        GetAlbumsMatchingArtistL(id, aAttrs, aMediaArray);
        }
    else
        {
        // look up all albums from album table
        iDbAlbum->FindAllL(aCriteria, aAttrs, aMediaArray);
        }
    }

// ----------------------------------------------------------------------------
// Extracts the playlist ID and drive ID from a playlist media instance
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::ProcessPlaylistMediaL(
    CMPXMedia& aMedia,
    TUint32& aPlaylistId,
    TInt& aPlaylistDriveId)
    {
    MPX_FUNC("CMPXDbHandler::ProcessPlaylistMediaL");

    if (aMedia.IsSupported(KMPXMediaGeneralId))
        {
        aPlaylistId = aMedia.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);

        if (aMedia.IsSupported(KMPXMediaGeneralUri))
            {
            // find drive id of the playlist
            aPlaylistDriveId = TDriveUnit(aMedia.ValueText(KMPXMediaGeneralUri));
            }
        else
            {
            // Find drive Id(s) of corresponding Playlist Id
            aPlaylistDriveId = iDbPlaylist->GetDriveIdL(aPlaylistId);
            }
        }
    else if (aMedia.IsSupported(KMPXMediaGeneralUri))
        {
        const TDesC& playlistUri = aMedia.ValueText(KMPXMediaGeneralUri);
        // find drive id of the playlist
        aPlaylistDriveId = TDriveUnit(playlistUri);

        // aMedia does not have an ID, make sure the add it
        aPlaylistId = GetPlaylistIdMatchingUriL(playlistUri);
        aMedia.SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, aPlaylistId);
        }
    else
        {
        User::Leave(KErrArgument);
        }
    }

// ----------------------------------------------------------------------------
// Makes sure that all songs in the specified playlist have the ID, title and URI attributes
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::UpdatePlaylistSongInfoL(
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbHandler::UpdatePlaylistSongInfoL");

    CMPXMediaArray* mediaArray = aMedia.Value<CMPXMediaArray>(KMPXMediaArrayContents);
    User::LeaveIfNull(mediaArray);

    // make sure each song has Id, Uri, and Title before they can be added to playlist
    TInt count(mediaArray->Count());
    for (TInt i = 0; i < count; ++i)
        {
        CMPXMedia* element = mediaArray->AtL(i);

        // copy each song to deal w/ global heap issues
        CMPXMedia* entry = CMPXMedia::NewL(*element);
        CleanupStack::PushL(entry);

        // song has everything, go to next song
        if (entry->IsSupported(KMPXMediaGeneralUri) &&
            entry->IsSupported(KMPXMediaGeneralId) &&
            entry->IsSupported(KMPXMediaGeneralTitle))
            {
            // pop entry to maintain CleanupStack
            CleanupStack::PopAndDestroy(entry);
            continue;
            }

        // songs must contain (at minimum) an Uri or an Id
        if (!entry->IsSupported(KMPXMediaGeneralUri) &&
            !entry->IsSupported(KMPXMediaGeneralId))
            {
            User::Leave(KErrArgument);
            }

        // get Id
        if (!entry->IsSupported(KMPXMediaGeneralId))
            {
            // fill in the ID if not present
            TParsePtrC parser(entry->ValueText(KMPXMediaGeneralUri));
            entry->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId,
                MPXDbCommonUtil::GenerateUniqueIdL(iFs, EMPXCollection, parser.FullName(), EFalse));
            }

        CMPXMedia* song(NULL);

        // update songs info
        TInt error(iDbMusic->GetSongL(*entry, song));
        TBool result (ETrue);

        // error can only be KErrNone or KErrNotFound
        // from CMPXDbMusic::GetSongL
        // if not found in Music, get info from PlaylistSongs (PlaylistSongs & PlaylistSongInfo) DB
        if (error == KErrNotFound)
            {
            RArray<TMPXAttribute> attributes;
            CleanupClosePushL(attributes);
            attributes.AppendL(TMPXAttribute(KMPXMediaIdGeneral,
                EMPXMediaGeneralId | EMPXMediaGeneralTitle | EMPXMediaGeneralUri | EMPXMediaGeneralFlags));

            // this song doesn't exist in Music table. This song is either a broken link or
            // is of an unsupported song type that exists in the file system. Broken links
            // have already been marked as such during playlist import.
            result = iDbPlaylist->Songs().GetSongL(entry->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId), attributes.Array(), song);
            if (!result)
                {
                // song is a broken link
                //TUint flags = KMPXMediaGeneralFlagsSetOrUnsetBit;
                //flags |= KMPXMediaGeneralFlagsIsInvalid; // set flag
                //t->SetTObjectValueL<TUint>( KMPXMediaGeneralFlags, flags );

                if (entry->IsSupported(KMPXMediaGeneralUri))
                    {
                    // no valid Id but has Uri, just verify Title is present
                    // this is the case if the song is a broken link or podcast
                    if (!entry->IsSupported(KMPXMediaGeneralTitle))
                        {
                        // does not have Title, make up the Title from file name
                        TParsePtrC parser(entry->ValueText(KMPXMediaGeneralUri));
                        entry->SetTextValueL(KMPXMediaGeneralTitle, parser.Name());
                        }
                    }
                else
                    {
                    // no valid Id & no Uri, bad argument
                    User::Leave(KErrArgument);
                    }
                }
            CleanupStack::PopAndDestroy(&attributes);
            }

        // update attributes
        CleanupStack::PushL(song);

        // song not found in Music or Playlist DB, update entry
        if(error == KErrNotFound && !result)
            {
            mediaArray->InsertL(*entry,i);
            }
        else  // found in DB, replace entry
            {
            mediaArray->InsertL(*song,i);
            }

        // replace element in the array
        CleanupStack::PopAndDestroy(song);
        CleanupStack::PopAndDestroy(entry);
        mediaArray->Remove(i+1);
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbHandler::ProcessMusicFoldersL
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::ProcessMusicFoldersL(
    const CDesCArray& aFolders)
    {
    MPX_FUNC("CMPXDbHandler::ProcessMusicFoldersL");

    TInt count(aFolders.MdcaCount());
    for (TInt i = 0; i < count; ++i)
        {
        TPtrC16 folder = aFolders.MdcaPoint(i);

        // check if disk is inserted and act accordingly
        TDriveUnit driveUnit(folder);
        if (!iFs.IsValidDrive(driveUnit))
            {
            User::Leave(KErrArgument);
            }

        // append the drive to the drive list
        iDbDrives.AppendL(driveUnit);
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbHandler::DbCategoryL
// ----------------------------------------------------------------------------
//
CMPXDbCategory* CMPXDbHandler::DbCategoryL(
    TMPXGeneralCategory aCategory) const
    {
    MPX_FUNC("CMPXDbHandler::DbCategoryL");

    CMPXDbCategory* dbCategory(NULL);
    switch (aCategory)
        {
        case EMPXArtist:
            {
            dbCategory = (CMPXDbCategory*)iDbArtist;
            break;
            }
        case EMPXAlbum:
            {
            dbCategory = (CMPXDbCategory*)iDbAlbum;
            break;
            }
        case EMPXGenre:
            {
            dbCategory = (CMPXDbCategory*)iDbGenre;
            break;
            }
        case EMPXComposer:
            {
            dbCategory = (CMPXDbCategory*)iDbComposer;
            break;
            }
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
        case EMPXAbstractAlbum:
            {
            dbCategory = (CMPXDbCategory*)iDbAbstractAlbum;
            break;
            }
#endif // ABSTRACTAUDIOALBUM_INCLUDED
        default:
            User::Leave(KErrNotSupported);
        }

    return dbCategory;
    }

// ----------------------------------------------------------------------------
// Verifies that the volume ID of the database matches the drive
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::VerifyVolumeIdL( TInt aDrive )
    {
    MPX_DEBUG1("CMPXDbHandler::VerifyVolumeIdL <--");
        if( iDbManager->IsOpen( aDrive ) )
            {
            TVolumeInfo volInfo;
            iFs.Volume(volInfo, aDrive );
            TUint curId(volInfo.iUniqueID);

            TInt volId = iDbAuxiliary->IdL( aDrive );

            // New database, no volume id set, mask out top bit because this is an uint
            //
            MPX_DEBUG3("CMPXDBHandler::VerifyVolumeIdL drive:%i db:%i", curId, volId);
            if( volId == 0 && (curId&0x7FFFFFFF) )
                {
                MPX_DEBUG1("CMPXDbHandler::VerifyVolumeIdL -- New ID");
                BeginTransactionL();
                TRAPD( err, iDbAuxiliary->SetIdL( aDrive, curId&0x7FFFFFFF ) );
                EndTransactionL( err );

                // KSqlDbCorrupted indicates DB corrupted, need to recreate.
                if ( err == KSqlDbCorrupted )
                    {
                    MPX_DEBUG1("CMPXPodcastDbHandler::VerifyVolumeIdL -- Corrupted DB");
                    iDbManager->RecreateDatabaseL(aDrive);
                    BeginTransactionL();
                    TRAPD(err, iDbAuxiliary->SetDBCorruptedL( ETrue ) );
                    EndTransactionL( err );
                    }
                }
            // Unmatched volume id, mark db as corrupt and break
            //
            else if ( (curId&0x7FFFFFFF) != (volId&0x7FFFFFFFF) )
                {
                MPX_DEBUG1("CMPXDbHandler::VerifyVolumeIdL -- ID match FAILED");
                iDbManager->RecreateDatabaseL(aDrive);
                BeginTransactionL();
                TRAPD(err, iDbAuxiliary->SetDBCorruptedL( ETrue ) );
                EndTransactionL( err );
                }
            }

    MPX_DEBUG1("CMPXDbHandler::VerifyVolumeIdL -->");
    }

// ----------------------------------------------------------------------------
// Verifies that the volume ID of the database matches the drive
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::VerifyVolumeIdL()
    {
    MPX_DEBUG1("CMPXDbHandler::VerifyVolumeIdL <--");
    TInt count( iDbDrives.Count() );
    for( TInt i=0; i<count; ++i )
        {
        VerifyVolumeIdL(iDbDrives[i]);
        }
    MPX_DEBUG1("CMPXDbHandler::VerifyVolumeIdL -->");
    }


// ----------------------------------------------------------------------------
// Checks if there is a drive that has a low disk space
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::CheckDiskSpaceOnDrivesL()
    {
    MPX_DEBUG1("CMPXDbHandler::CheckDiskSpaceOnDrivesL <--");

    TInt count( iDbDrives.Count() );
    for( TInt index=0; index<count; ++index )
        {
        iDbManager->CheckDiskSpaceL(iDbDrives[index]);
        }
    MPX_DEBUG1("CMPXDbHandler::CheckDiskSpaceOnDrivesL -->");
    }

#if defined (__MTP_PROTOCOL_SUPPORT)

// ----------------------------------------------------------------------------
// CMPXDbHandler::SaveDeletedSongs
// ----------------------------------------------------------------------------
//
TBool CMPXDbHandler::SaveDeletedSongs()
    {
    MPX_FUNC("CMPXDbHandler::SaveDeletedSongs");

    TBool saveDeletedSongs(ETrue);
    CRepository* cenrep(NULL);
    MPX_TRAPD(error, cenrep = CRepository::NewL(KMPXMtpSettings));
    if (!error)
        {
        cenrep->Get(KMPXMtpSaveDeletedRecordFlag, saveDeletedSongs);
        delete cenrep;
        MPX_DEBUG2("MTP indicated to save deleted songs? %d", saveDeletedSongs);
        }

    return saveDeletedSongs;
    }

#endif

#ifdef RD_MULTIPLE_DRIVE

// ----------------------------------------------------------------------------------------------------------
// Retrieve all visible music folder locations
// ----------------------------------------------------------------------------------------------------------
//
CDesCArrayFlat* CMPXDbHandler::GetMusicFoldersL()
    {
    MPX_FUNC("CMPXDbHandler::GetMusicFoldersL()");
    TDriveList driveList;
    TInt driveCount(0);
    User::LeaveIfError(DriveInfo::GetUserVisibleDrives(iFs, driveList, driveCount));
    MPX_DEBUG2 ("CMPXDbHandler::GetMusicFoldersL() - driveCount = %d", driveCount);

    CDesCArrayFlat* folders = new (ELeave) CDesCArrayFlat(1); // granularity
    CleanupStack::PushL(folders);

    for (TInt i = EDriveA; i <= EDriveZ; i++)
        {
        if ((driveList[i]) && (!IsRemoteDrive(static_cast<TDriveNumber>(i))))
            {
            if (i == EDriveC)
                {
                // Append the default phone memory path to the list
                // of music folders
                TPtrC rootPath(PathInfo::PhoneMemoryRootPath());
                folders->AppendL(rootPath);
                MPX_DEBUG2("CMPXDbHandler::GetMusicFoldersL() - adding...%S", &rootPath);
                }
            else
                {
                // Get drive letter
                TChar driveChar;
                User::LeaveIfError(iFs.DriveToChar(i, driveChar));

                // Append visible drive to list of music folders
                TBuf<2> drive;
                drive.Append(driveChar);
                drive.Append(_L(":"));
                folders->AppendL(drive);
                MPX_DEBUG2 ("CMPXDbHandler::GetMusicFoldersL() - adding...%S", &drive);
                }
            }
        }

    CleanupStack::Pop(folders);
    return folders;
    }

#endif // RD_MULTIPLE_DRIVE

// ----------------------------------------------------------------------------
// CMPXDbHandler::AddAbstractAlbumItemL
// ----------------------------------------------------------------------------
//
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
TUint32 CMPXDbHandler::AddAbstractAlbumItemL(
    const TDesC& aName,
    TInt aDriveId,
    CMPXMessageArray* aItemChangedMessages,
    TBool& aItemExist,
    const TDesC& aUri,
    const TDesC& aAlbumArtist
    )
    {
    MPX_FUNC("CMPXDbHandler::AddAbstractAlbumItemL()");

    MPX_PERF_START(CMPXDbHandler_AddAbstractAlbumItemL);
    
    TBool newRecord(EFalse);
    TUint32 id(0);

    //AbstractAlbum is not case sensitive
    id =  iDbAbstractAlbum->AddItemL(aUri, aName, aAlbumArtist, aDriveId, newRecord, EFalse);
    MPX_PERF_END(CMPXDbHandler_AddAbstractAlbumItemL);
    
    return id;
    }
#endif

// ----------------------------------------------------------------------------
// CMPXDbHandler::AddCategoryItemL
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbHandler::AddCategoryItemL(
    TMPXGeneralCategory aCategory,
    const CMPXMedia& aMedia,
    TInt aDriveId,
    CMPXMessageArray* aItemChangedMessages,
    TBool& aItemExist)
    {
    MPX_FUNC("CMPXDbHandler::AddCategoryItemL()");
    
    MPX_PERF_START(CMPXDbHandler_AddCategoryItemL);

    ASSERT(aCategory != EMPXAbstractAlbum);
	
    TBool newRecord(EFalse);

    TUint32 id = DbCategoryL(aCategory)->AddItemL(aCategory, aMedia, aDriveId, newRecord, aCategory != EMPXGenre);

    if (newRecord && aItemChangedMessages)
        {
        MPXDbCommonUtil::AddItemChangedMessageL(*aItemChangedMessages, id, EMPXItemInserted,
            aCategory, KDBPluginUid);
        }
    // when the added item's category is Genre, Composer or Artist, and it is NOT a new record,
    // we should send the item number changed message
    else if ( ( aCategory == EMPXGenre || aCategory == EMPXComposer || aCategory == EMPXArtist) &&
            !newRecord && aItemChangedMessages )
        {
        MPXDbCommonUtil::AddItemChangedMessageL(*aItemChangedMessages, id, EMPXItemModified,
            aCategory, KDBPluginUid);
        }
    aItemExist = !newRecord;
    MPX_PERF_END(CMPXDbHandler_AddCategoryItemL);

    return id;    
    }

// ----------------------------------------------------------------------------
// CMPXDbHandler::UpdateCategoryItemL
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::UpdateCategoryItemL(
    TMPXGeneralCategory aCategory,
    TUint32 aCategoryId,
    const CMPXMedia& aMedia,
    TInt aDrive,
    CMPXMessageArray* aItemChangedMessages)
    {
    DbCategoryL(aCategory)->UpdateItemL(aCategoryId, aMedia, aDrive, aItemChangedMessages);
    }

// ----------------------------------------------------------------------------
// CMPXDbHandler::DeleteSongForCategoryL
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::DeleteSongForCategoryL(
    TMPXGeneralCategory aCategory,
    TUint32 aCategoryId,
    TInt aDriveId,
    CMPXMessageArray* aItemChangedMessages,
    TBool& aItemExist,
    const TDesC& aArt   
    )
    {
    MPX_FUNC("CMPXDbHandler::DeleteSongForCategoryL");
    switch(aCategory)
        {
        case EMPXAlbum:
            iDbAlbum->DecrementSongsForAlbumL(aCategoryId, aDriveId, aItemChangedMessages, aItemExist, aArt);
            break;
        default:
            DbCategoryL(aCategory)->DecrementSongsForCategoryL(aCategoryId, aDriveId,
                    aItemChangedMessages, aItemExist);
            break;
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbHandler::HandlePlayCountModifiedL
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::HandlePlayCountModifiedL(
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbHandler::HandlePlayCountModifiedL");

    TUint32 plId(iAutoPlaylist->AutoPlaylistIdL(EMPXMostPlayedPlaylist));

    MPXDbCommonUtil::AddItemChangedMessageL(aItemChangedMessages, plId, EMPXItemModified,
        EMPXSong, KDBPluginUid, plId);

    // Force the deprecated ID attribute
    aItemChangedMessages[aItemChangedMessages.Count() - 1]->
        SetTObjectValueL<TMPXItemId>(KMPXMessageMediaDeprecatedId, plId);
    }

// ----------------------------------------------------------------------------------------------------------
// CMPXDbHandler::HandlePlaybackTimeModifiedL
// ----------------------------------------------------------------------------------------------------------
//
void CMPXDbHandler::HandlePlaybackTimeModifiedL(
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbHandler::HandlePlaybackTimeModifiedL");

    TUint32 plId(iAutoPlaylist->AutoPlaylistIdL(EMPXRecentlyPlayedPlaylist));

    MPXDbCommonUtil::AddItemChangedMessageL(aItemChangedMessages, plId, EMPXItemModified,
        EMPXSong, KDBPluginUid, plId);

    // Force the deprecated ID attribute
    aItemChangedMessages[aItemChangedMessages.Count() - 1]->
        SetTObjectValueL<TMPXItemId>(KMPXMessageMediaDeprecatedId, plId);
    }

// ---------------------------------------------------------------------------
// CMPXDbHandler::IsRemoteDrive
// ---------------------------------------------------------------------------
//
TBool CMPXDbHandler::IsRemoteDrive(TDriveNumber aDrive)
    {
    return iDbManager->IsRemoteDrive(aDrive);
    }

TInt CMPXDbHandler::HandlePlaylistDurationL(TUint32 aPlaylistId)
    {
    return GetPlaylistDurationL(aPlaylistId);
    }
void CMPXDbHandler::HandlePlaylistInfoL(
    TUint32 aPlaylistId, 
    TInt& aCount, 
    TInt& aDuration)
    {
    MPX_FUNC("CMPXDbHandler::HandlePlaylistInfoL");
    MPX_DEBUG2("CMPXDbHandler::HandlePlaylistInfoL(0x%x)", aPlaylistId);

    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);
    attributes.AppendL(KMPXMediaGeneralId);    
    attributes.AppendL(TMPXAttribute(KMPXMediaIdGeneral, EMPXMediaGeneralDuration));

    CMPXMediaArray* mediaArray = CMPXMediaArray::NewL();
    CleanupStack::PushL(mediaArray);    
    
    GetPlaylistSongsL(aPlaylistId, attributes.Array(), *mediaArray);
    
    aCount = mediaArray->Count();
    for (TInt index = 0; index < aCount; ++index)
        {
        CMPXMedia* media((*mediaArray)[index]);
        if (media->IsSupported(KMPXMediaGeneralDuration))
            {
            aDuration += media->ValueTObjectL<TInt>(KMPXMediaGeneralDuration);
            }
        }

    CleanupStack::PopAndDestroy(mediaArray);
    CleanupStack::PopAndDestroy(&attributes);        
    }

// ---------------------------------------------------------------------------
// CMPXDbHandler::HandleAlbumartForAlbumL
// ---------------------------------------------------------------------------
//
HBufC*  CMPXDbHandler::HandleAlbumartForAlbumL(const TUint32 aAlbumId, TPtrC aArt)
    {
    return iDbMusic->AlbumartForAlbumL(aAlbumId, aArt);
    }

#ifdef ABSTRACTAUDIOALBUM_INCLUDED   
// ----------------------------------------------------------------------------------------------------------
// CMPXDbHandler::HandleGetAlbumNameForSongL
// ----------------------------------------------------------------------------------------------------------
//
HBufC* CMPXDbHandler::HandleGetAlbumNameFromIdL(TUint32 aId)
    {
    return iDbAbstractAlbum->GetUriL(aId);
    }
#endif // ABSTRACTAUDIOALBUM_INCLUDED

// ----------------------------------------------------------------------------
// CMPXDbHandler::DeleteAlbumForArtistL
// ----------------------------------------------------------------------------
//
void CMPXDbHandler::DeleteAlbumForArtistL(TUint32 aCategoryId,
    TInt aDrive, CMPXMessageArray* aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbHandler::DeleteAlbumForArtistL");
    iDbArtist->DecrementAlbumsForArtistL(aCategoryId, aDrive, aItemChangedMessages);
    }

// ----------------------------------------------------------------------------
// CMPXDbHandler::AddAlbumArtistL
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbHandler::AddAlbumArtistL(const TDesC& aName, const TDesC& aArt, TInt aDriveId)
    {
    MPX_FUNC("CMPXDbHandler::AddAlbumArtistL");
    return iDbArtist->AddAlbumArtistL(aName, aArt, aDriveId);
    }

// ----------------------------------------------------------------------------
// CMPXDbHandler::GenerateUniqueIdForAlbumL
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbHandler::GenerateUniqueIdForAlbumL(const CMPXMedia& aMedia)
    {
    return iDbAlbum->GenerateUniqueIdL(aMedia);
    }

// ----------------------------------------------------------------------------
// CMPXDbHandler::IsUnknownAlbumL
// ----------------------------------------------------------------------------
//
TBool CMPXDbHandler::IsUnknownAlbumL(const TUint32 aId)
    {
    return iDbAlbum->IsUnknownAlbumL(aId);
    }
// End of file
