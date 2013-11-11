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
* Description:  Responsible for interaction with the PlaylistSongs and
*                PlaylistSongInfo tables.
*
*/


// INCLUDE FILES
#include <f32file.h>
#include <mpxlog.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediamusicdefs.h>
#include "mpxdbcommonutil.h"

#include "mpxcollectiondbdef.h"
#include "mpxdbutil.h"
#include "mpxdbpluginqueries.h"
#include "mpxdbmanager.h"
#include "mpxdbplaylistsongs.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbPlaylistSongs* CMPXDbPlaylistSongs::NewL(
    CMPXDbManager& aDbManager)
    {
    MPX_FUNC("CMPXDbPlaylistSongs::NewL");

    CMPXDbPlaylistSongs* self = CMPXDbPlaylistSongs::NewLC(aDbManager);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbPlaylistSongs* CMPXDbPlaylistSongs::NewLC(
    CMPXDbManager& aDbManager)
    {
    MPX_FUNC("CMPXDbPlaylistSongs::NewLC");

    CMPXDbPlaylistSongs* self = new (ELeave) CMPXDbPlaylistSongs(aDbManager);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CMPXDbPlaylistSongs::~CMPXDbPlaylistSongs()
    {
    MPX_FUNC("CMPXDbPlaylistSongs::~CMPXDbPlaylistSongs");
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CMPXDbPlaylistSongs::CMPXDbPlaylistSongs(
    CMPXDbManager& aDbManager) :
    CMPXDbTable(aDbManager)
    {
    MPX_FUNC("CMPXDbPlaylistSongs::CMPXDbPlaylistSongs");
    }

// ----------------------------------------------------------------------------
// Second phase constructor.
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylistSongs::ConstructL()
    {
    MPX_FUNC("CMPXDbPlaylistSongs::ConstructL");
    BaseConstructL();
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylistSongs::AddSongsL
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylistSongs::AddSongsL(
    TUint32 aPlaylistId,
    const CMPXMediaArray& aMediaArray,
    TInt aDriveId)
    {
    MPX_FUNC("CMPXDbPlaylistSongs::AddSongsL");

    TInt count(aMediaArray.Count());
    for (TInt index = 0; index < count; ++index)
        {
        AddSongL(aPlaylistId, *(aMediaArray[index]), aDriveId);
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylistSongs::AddSongL
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbPlaylistSongs::AddSongL(
    TUint32 aPlaylistId,
    const CMPXMedia& aMedia,
    TInt aDriveId)
    {
    MPX_FUNC("CMPXDbPlaylistSongs::AddSongL");
    TInt count(0);
    if (aDriveId >= 0)
    	{
    	count = CountL(aPlaylistId, aDriveId);
    	}
    return AddSongL(aPlaylistId, count, aMedia, aDriveId);
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylistSongs::AddSongL
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbPlaylistSongs::AddSongL(
    TUint32 aPlaylistId,
    TInt aOrdinal,
    const CMPXMedia& aMedia,
    TInt aDriveId)
    {
    MPX_FUNC("CMPXDbPlaylistSongs::AddSongL");

    // the UniqueId field is AUTOINCREMENT and its value is going to be generated
    // automatically by the database - no need to supply it here
    TUint32 songId((aMedia.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId)).iId2);

    // insert PlaylistSongs record
    iDbManager.ExecuteQueryL(aDriveId, KQueryPlaylistSongsInsert, aPlaylistId, songId, aOrdinal);

    // if the song is not already in the PlaylistSongInfo table - add it
    if (!SongInfoExistsL(aDriveId, songId))
        {
        // add a new song info record
        HBufC* uri = MPXDbCommonUtil::ProcessSingleQuotesLC(aMedia.ValueText(KMPXMediaGeneralUri));
        HBufC* title = MPXDbCommonUtil::ProcessSingleQuotesLC(aMedia.ValueText(KMPXMediaGeneralTitle));

        TUint32 dbFlag(0);
        if (aMedia.IsSupported(KMPXMediaGeneralFlags))
            {
            dbFlag = (aMedia.ValueTObjectL<TUint>(KMPXMediaGeneralFlags));
            dbFlag = dbFlag & (~KMPXMediaGeneralFlagsDriveInfo); // clear drive info
            }

        // add the URI without the drive letter
        TPtrC uriPtr(uri->Mid(KMCPathStartPos));
        iDbManager.ExecuteQueryL(aDriveId, KQueryPlaylistSongInfoInsert, songId, &uriPtr, title,
            MPXDbCommonUtil::GetVolIdMatchDriveIdL(iDbManager.Fs(), TDriveUnit(*uri)), dbFlag);

        CleanupStack::PopAndDestroy(title);
        CleanupStack::PopAndDestroy(uri);
        }

    return songId;
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylistSongs::UpdateSongL
// ----------------------------------------------------------------------------
//
TBool CMPXDbPlaylistSongs::UpdateSongL(
    TUint32 aSongId,
    const CMPXMedia& aMedia,
    TBool aResetFlags,
    TBool& aUpdated)
    {
    MPX_FUNC("CMPXDbPlaylistSongs::UpdateSongL");

    aUpdated = EFalse;
    TBool visibleChange(EFalse);

    // get the current record
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryPlaylistSongInfoGet, aSongId));
    CleanupClosePushL(recordset);

    if (recordset.Next() == KSqlAtRow)
        {
        const TArray<TMPXAttribute> attributes = aMedia.Attributes();

        // stores the current song ID, which may change during execution
        TUint32 songId(aSongId);

        TInt attrCount(attributes.Count());
        for (TInt i = 0; i < attrCount; ++i)
            {
            TInt contentId(attributes[i].ContentId());
            TUint attributeId(attributes[i].AttributeId());

            switch(contentId)
                {
                case KMPXMediaIdGeneral:
                    {
                    if (attributeId & EMPXMediaGeneralTitle)
                        {
                        // existing title
                        TPtrC title(aMedia.ValueText(KMPXMediaGeneralTitle).Left(KMCMaxTextLen));

                        // compare with the old title
                        if (title.Compare(MPXDbCommonUtil::GetColumnTextL(recordset, EPlaylistSongInfoTitle)) != 0)
                            {
                            HBufC* titleProc = MPXDbCommonUtil::ProcessSingleQuotesLC(title);

                            // title has changed - update on all drives
                            iDbManager.ExecuteQueryL(KDbManagerAllDrives, KQueryPlaylistSongInfoUpdateTitle,
                                titleProc, songId);

                            CleanupStack::PopAndDestroy(titleProc);

                            visibleChange = ETrue;
                            aUpdated = ETrue;
                            MPX_DEBUG2("    Title[%S]", &title);
                            }
                        }

                    if (attributeId & EMPXMediaGeneralUri)
                        {
                        const TDesC& uriOrig(aMedia.ValueText(KMPXMediaGeneralUri));
                        songId = MPXDbCommonUtil::GenerateUniqueIdL(iDbManager.Fs(), EMPXCollection, uriOrig, EFalse);

                        if (aSongId != songId)
                            {
                            // URI of the song has been changed. This changes the Id of the song and both the
                            // PlaylistSongs and PlaylistSongInfo tables should be updated accordingly.

                            HBufC* uri = MPXDbCommonUtil::ProcessSingleQuotesLC(uriOrig);

                            // use the URI without the drive letter
                            TPtrC uriPtr(uri->Mid(KMCPathStartPos));

                            // update the PlaylistSongInfo table on all drives
                            iDbManager.ExecuteQueryL(KDbManagerAllDrives, KQueryPlaylistSongInfoUpdate, songId,
                                &uriPtr, MPXDbCommonUtil::GetVolIdMatchDriveIdL(iDbManager.Fs(), TDriveUnit(*uri)), aSongId);

                            // update the PlaylistSongs table on all drives
                            iDbManager.ExecuteQueryL(KDbManagerAllDrives, KQueryPlaylistSongsUpdate, songId,
                                aSongId);

                            aUpdated = ETrue;
                            MPX_DEBUG3("    CurrentSongId[0x%x] changed to [0x%x]", aSongId, songId);

                            CleanupStack::PopAndDestroy(uri);
                            }
                        }

                    if (attributeId & EMPXMediaGeneralFlags)
                        {
                        TUint flag(aMedia.ValueTObjectL<TUint>(KMPXMediaGeneralFlags));
                        TUint32 curFlag(recordset.ColumnInt64(EPlaylistSongInfoDbFlag));

                        if (flag & KMPXMediaGeneralFlagsSetOrUnsetBit)
                            {
                            if (aResetFlags)
                                {
                                visibleChange = ETrue;
                                curFlag = flag;
                                }
                            else
                                {
                                // Set flag, visible change is true only if the flag status is changing
                                visibleChange = visibleChange || ((curFlag^flag) & 0x7FFFFFFF);
                                curFlag |= flag;
                                }
                            }
                        else
                            {
                            // Clear flag, visible change is true only if the flag status is changing
                            visibleChange = visibleChange || (((curFlag^0xFFFFFFFF)^flag) & 0x7FFFFFFF);
                            curFlag &= (~flag);
                            }

                        if (visibleChange)
                            {
                            // update the flags on all drives
                            iDbManager.ExecuteQueryL(KDbManagerAllDrives, KQueryPlaylistSongInfoUpdateFlags,
                                curFlag, songId);
                            aUpdated = ETrue;
                            }

                        MPX_DEBUG2("    GeneralFlags[%b]", curFlag);
                        }
                    } // end case
                } // end switch
            } // end for
        if ( aMedia.IsSupported( KMPXMediaMusicAlbumArtChanged ) )
            {
            if ( aMedia.ValueTObjectL<TInt>( KMPXMediaMusicAlbumArtChanged )== 1 )
                {
                aUpdated = ETrue;                        
                }
            }
        }

    CleanupStack::PopAndDestroy(&recordset);
    return visibleChange;
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylistSongs::UpdateSongsL
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylistSongs::UpdateSongsL(
    TUint32 aPlaylistId,
    TUint32 aNewPlaylistId)
    {
    MPX_FUNC("CMPXDbPlaylistSongs::UpdateSongsL");

    // update the PlaylistSongs table on all drives
    iDbManager.ExecuteQueryL(KDbManagerAllDrives, KQueryPlaylistSongsUpdatePlaylistId,
        aNewPlaylistId, aPlaylistId);
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylistSongs::ReorderSongL
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylistSongs::ReorderSongL(
    const TMPXItemId& aPlaylistId,
    const TMPXItemId& aSongId,
    TUint aOriginalOrdinal,
    TUint aNewOrdinal)
    {
    MPX_DEBUG1("-->CMPXDbPlaylistSongs::ReorderSongL");
    MPX_DEBUG5("    playlist[0x%x, 0x%x], song[0x%x, 0x%x]",
               aPlaylistId.iId1, aPlaylistId.iId2, aSongId.iId1, aSongId.iId2);

    //
    // Move the song up. The rest of the songs in between the old ordinal and new ordinal
    // need to be moved down, i.e. their ordinals need to be incremented
    //
    if (aOriginalOrdinal > aNewOrdinal)
        {
        iDbManager.ExecuteQueryL(KDbManagerAllDrives, KQueryPlaylistSongsMoveSongUp,
            aPlaylistId.iId2, aNewOrdinal, aOriginalOrdinal);
        }

    //
    // Move the song down. The rest of the songs in between the old ordinal and new ordinal
    // need to be moved up, i.e. their ordinals need to be decremented
    //
    else if (aOriginalOrdinal < aNewOrdinal)
        {
        iDbManager.ExecuteQueryL(KDbManagerAllDrives, KQueryPlaylistSongsMoveSongDown,
            aPlaylistId.iId2, aNewOrdinal, aOriginalOrdinal);
        }

    //
    // Change the ordinal of the song itself. If the ordinal is unchanged, no update is
    // required
    //
    if (aOriginalOrdinal != aNewOrdinal)
        {
        iDbManager.ExecuteQueryL(KDbManagerAllDrives, KQueryPlaylistSongsUpdateSongOrdinal,
            aNewOrdinal, aSongId.iId1);
        }

    MPX_DEBUG1("<--CMPXDbPlaylistSongs::ReorderSongL");
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylistSongs::DeleteSongL
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylistSongs::DeleteSongL(
    TUint32 aSongId)
    {
    MPX_FUNC("CMPXDbPlaylistSongs::DeleteSongL");

    // delete from the PlaylistSongs table on all drives
    iDbManager.ExecuteQueryL(KDbManagerAllDrives, KQueryPlaylistSongsDelete, aSongId);

    // delete from the PlaylistSongInfo table on all drives
    iDbManager.ExecuteQueryL(KDbManagerAllDrives, KQueryPlaylistSongInfoDelete, aSongId);
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylistSongs::DeleteSongL
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylistSongs::DeleteSongL(
    TUint32 aPlaylistId,
    TUint32 aSongId,
    TInt aOrdinal,
    TInt aDriveId)
    {
    MPX_FUNC("CMPXDbPlaylist::DeleteSongL");

    // get the number of instances for the song in the playlist
    TInt count(SongInstanceCountL(aPlaylistId, aSongId));

    if (1 == count)
        {
        // just one instance with this ID in the playlist
        // delete it regardless of the ordinal
        iDbManager.ExecuteQueryL(aDriveId, KQueryPlaylistSongsDeleteSong, aPlaylistId, aSongId);

        // check how many instances of this song are left for all playlists
        if (!SongCountL(aSongId))
            {
            // delete the PlaylistSongInfo record
            iDbManager.ExecuteQueryL(aDriveId, KQueryPlaylistSongInfoDelete, aSongId);
            }
        }
    else if (count > 1)
        {
        // multiple songs with this id in the playlist
        // try to delete the one with the specified ordinal
        iDbManager.ExecuteQueryL(aDriveId, KQueryPlaylistSongsDeleteSongOrdinal, aPlaylistId,
            aSongId, aOrdinal);
        }
    else
        {
        // no such song in the playlist
        User::Leave(KErrCorrupt);
        }
    // adjust song ordinals for the songs after the deleted song
    iDbManager.ExecuteQueryL(aDriveId, KQueryPlaylistSongsUpdateSongOrdinalAfterDelete,
        aPlaylistId, aOrdinal);
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylistSongs::DeleteSongsL
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylistSongs::DeleteSongsL(
    TUint32 aPlaylistId,
    TInt aDriveId)
    {
    MPX_FUNC("CMPXDbPlaylist::DeleteSongsL");

    // delete the records from the PlaylistSongs table
    iDbManager.ExecuteQueryL(aDriveId, KQueryPlaylistSongsDeletePlaylist, aPlaylistId);

    // delete the unused records from the PlaylistSongInfo table
    CleanupSongInfoL(aDriveId);
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylistSongs::DeleteSongsForCategoryL
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylistSongs::DeleteSongsForCategoryL(
    TMPXGeneralCategory aCategory,
    TUint32 aCategoryId,
    TInt aDriveId)
    {
    MPX_FUNC("CMPXDbPlaylist::DeleteSongsForCategoryL");

    // get the category field name in the Music table
    TPtrC category = MPXDbUtil::MusicFieldNameForCategoryL(aCategory);

    iDbManager.ExecuteQueryL(aDriveId, KQueryPlaylistSongsDeleteForCategory,
        &category, aCategoryId);

    // delete the unused records from the PlaylistSongInfo table
    CleanupSongInfoL(aDriveId);
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylistSongs::DeleteSongsForArtistAndAlbumL
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylistSongs::DeleteSongsForArtistAndAlbumL(
    TUint32 aArtistId,
    TUint32 aAlbumId,
    TInt aDriveId)
    {
    MPX_FUNC("CMPXDbPlaylist::DeleteSongsForArtistAndAlbumL");

    // delete the songs in the PlaylisSongs table
    iDbManager.ExecuteQueryL(aDriveId, KQueryPlaylistSongsDeleteForArtistAlbum,
        aArtistId, aAlbumId);

    // delete the unused records from the PlaylistSongInfo table
    CleanupSongInfoL(aDriveId);
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylistSongs::DeleteAllSongsL
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylistSongs::DeleteAllSongsL()
    {
    MPX_FUNC("CMPXDbPlaylist::DeleteAllSongsL");

    // delete all records from the PlaylistSongs table, all drives
    iDbManager.ExecuteQueryL(KDbManagerAllDrives, KQueryPlaylistSongsDeleteAll);

    // delete all records from the PlaylistSongInfo table, all drives
    iDbManager.ExecuteQueryL(KDbManagerAllDrives, KQueryPlaylistSongInfoDeleteAll);
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylistSongs::GetSongsL
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylistSongs::GetSongsL(
    TUint32 aPlaylistId,
    CMPXMediaArray& aMediaArray)
    {
    MPX_DEBUG2("-->CMPXDbPlaylistSongs::GetSongsL(playlist 0x%x)", aPlaylistId);

    RArray<TMPXAttribute> attributes;
    CleanupClosePushL( attributes );

    attributes.AppendL(KMPXMediaGeneralType);
    attributes.AppendL(KMPXMediaGeneralCategory);
    attributes.AppendL(KMPXMediaGeneralId);

    // cannot execute a joined query to the music table
    // because the song records in the music table may be on a different drive
    ExecuteMediaQueryL(attributes.Array(), aMediaArray, KQueryPlaylistSongsGetSongs, aPlaylistId);

    CleanupStack::PopAndDestroy(&attributes);

    MPX_DEBUG1("<--CMPXDbPlaylistSongs::GetSongsL");
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylistSongs::GetSongsL
// ----------------------------------------------------------------------------
//
TBool CMPXDbPlaylistSongs::GetSongsL(
    TUint32 aPlaylistId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_DEBUG2("-->CMPXDbPlaylistSongs::GetSongsL(playlist 0x%x)", aPlaylistId);
    TBool valid(EFalse);

    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryPlaylistSongsGetSongsInfo,
    		aPlaylistId));
    CleanupClosePushL(recordset);

    TInt err(KErrNone);
    while ((err = recordset.Next()) == KSqlAtRow)
        {
        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL(media);

        UpdateMediaL(recordset, aAttrs, *media);

        aMediaArray.AppendL(*media);
        CleanupStack::PopAndDestroy(media);
        }
    CleanupStack::PopAndDestroy(&recordset);

    if (err!= KSqlAtEnd)
        {
        User::Leave(KErrCorrupt);
        }
    else
    	{
    	valid = ETrue;
    	}

    MPX_DEBUG1("<--CMPXDbPlaylistSongs::GetSongsL");
    return valid;
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylistSongs::GetSongL
// ----------------------------------------------------------------------------
//
TBool CMPXDbPlaylistSongs::GetSongL(
    TUint32 aPlaylistId,
    TUint32 aSongId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbPlaylist::GetSongL");

    // get the song
    TBool valid(EFalse);
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryPlaylistSongsGetSong,
        aPlaylistId, aSongId));
    CleanupClosePushL(recordset);

    if (recordset.Next() == KSqlAtRow)
        {
        // convert to media
        UpdateMediaL(recordset, aAttrs, aMedia);
        valid = ETrue;
        }

    CleanupStack::PopAndDestroy(&recordset);

    return valid;
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylistSongs::GetSongL
// ----------------------------------------------------------------------------
//
TBool CMPXDbPlaylistSongs::GetSongL(
    const TMPXItemId& aSongId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia*& aMedia)
    {
    MPX_FUNC("CMPXDbPlaylist::GetSongL");

    // get the song
    TBool valid(EFalse);
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryPlaylistSongsGetSongInfo,
        aSongId.iId1, aSongId.iId2));
    CleanupClosePushL(recordset);

    if (recordset.Next() == KSqlAtRow)
        {
        // convert to media
        aMedia = CMPXMedia::NewL();
        CleanupStack::PushL(aMedia);
        UpdateMediaL(recordset, aAttrs, *aMedia);
        CleanupStack::Pop(aMedia);
        valid = ETrue;
        }

    CleanupStack::PopAndDestroy(&recordset);

    return valid;
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylistSongs::CountL
// ----------------------------------------------------------------------------
//
TInt CMPXDbPlaylistSongs::CountL(
    TUint32 aPlaylistId,
    TInt aDriveId)
    {
    MPX_FUNC("CMPXDbPlaylist::CountL");
    return ExecuteSumExQueryL(KQueryPlaylistSongsCount, aPlaylistId, aDriveId);
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylistSongs::UpdateMediaL
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylistSongs::UpdateMediaL(
    RSqlStatement& aRecord,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbPlaylist::UpdateMediaL");

    aMedia.SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
    aMedia.SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXSong);
    aMedia.SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, TMPXItemId(
        aRecord.ColumnInt64(EPlaylistSongsUniqueId),
        aRecord.ColumnInt64(EPlaylistSongsSongId)));

/*
    aMedia.SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId,
        aRecord.ColumnInt64(EPlaylistSongsSongId));
*/
    TInt count(aAttrs.Count());
    for (TInt index = 0; index < count; ++index)
        {
        TInt contentId(aAttrs[index].ContentId());
        TUint attributeId(aAttrs[index].AttributeId());

        if (contentId == KMPXMediaIdGeneral)
            {
            if (attributeId & EMPXMediaGeneralCollectionId)
                {
                aMedia.SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralCollectionId,
                    aRecord.ColumnInt64(EPlaylistSongsPlaylistId));
                }
            if (attributeId & EMPXMediaGeneralTitle)
                {
                TPtrC title(MPXDbCommonUtil::GetColumnTextL(aRecord, EPlaylistSongsTitle));
                aMedia.SetTextValueL(KMPXMediaGeneralTitle, title);
                MPX_DEBUG2("    Title[%S]", &title);
                }
            if (attributeId & EMPXMediaGeneralUri)
                {
                TUint volId(aRecord.ColumnInt64(EPlaylistSongsVolumeId));
                HBufC* uri = MPXDbCommonUtil::CreateFullPathL(
                    MPXDbCommonUtil::GetDriveIdMatchVolIdL(iDbManager.Fs(), volId),
                    MPXDbCommonUtil::GetColumnTextL(aRecord, EPlaylistSongsUri));
                CleanupStack::PushL(uri);

                aMedia.SetTextValueL(KMPXMediaGeneralUri, *uri);

                MPX_DEBUG2("    URI[%S]", uri);
                CleanupStack::PopAndDestroy(uri);
                }
            if (attributeId & EMPXMediaGeneralFlags)
                {
                // assuming song details shouldn't be available for this song
                TUint32 dbFlags(aRecord.ColumnInt64(EPlaylistSongsDbFlag) |
                                KMPXMediaGeneralFlagsIsMissingDetails);
                TUint32 volId(aRecord.ColumnInt64(EPlaylistSongsVolumeId));
                TDriveUnit driveUnit(EDriveC);
                if (aMedia.IsSupported(KMPXMediaGeneralUri))
                    {
                    // ignore the return value
                    MPXDbCommonUtil::GetDriveL(aMedia.ValueText(KMPXMediaGeneralUri), driveUnit);
                    }
                else
                    {
                    driveUnit = MPXDbCommonUtil::GetDriveIdMatchVolIdL(iDbManager.Fs(), volId);
                    }

                // the volume the playlist song is located on may be removed
                if (~dbFlags & KMPXMediaGeneralFlagsIsInvalid)
                    {
                    if (((volId != 0) && (MPXDbCommonUtil::GetDriveIdMatchVolIdL(iDbManager.Fs(), volId) == KErrNotFound)) ||
                        ((volId == 0) && (MPXDbCommonUtil::GetVolIdMatchDriveIdL(iDbManager.Fs(), driveUnit) == 0)))
                        {
                        dbFlags |= KMPXMediaGeneralFlagsIsInvalid;
                        }
                    }

                TInt driveId(driveUnit & KMPXMediaGeneralFlagsDriveInfo);  // 5 bits
                aMedia.SetTObjectValueL<TUint>(KMPXMediaGeneralFlags, dbFlags | driveId);
                MPX_DEBUG2("    GeneralFlags[%b]", dbFlags | driveId);
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylistSongs::SongInstanceCountL
// ----------------------------------------------------------------------------
//
TInt CMPXDbPlaylistSongs::SongInstanceCountL(
    TUint32 aPlaylistId,
    TUint32 aSongId)
    {
    MPX_FUNC("CMPXDbPlaylistSongs::SongInstanceCountL");
    return ExecuteSumQueryL(KQueryPlaylistSongsPlaylistSongCount, aPlaylistId, aSongId);
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylistSongs::SongCountL
// ----------------------------------------------------------------------------
//
TInt CMPXDbPlaylistSongs::SongCountL(
    TUint32 aSongId)
    {
    MPX_FUNC("CMPXDbPlaylistSongs::SongCountL");
    return ExecuteSumQueryL(KQueryPlaylistSongsSongCount, aSongId);
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylistSongs::SongInfoExistsL
// The song info record must be in the same database as the corresponding
// PlaylistSongs record(s), otherwise when adding a duplicate of a song on a
// different drive this method will return true and the song info record won't
// be created.
// ----------------------------------------------------------------------------
//
TBool CMPXDbPlaylistSongs::SongInfoExistsL(
    TInt aDriveId,
    TUint32 aSongId)
    {
    MPX_FUNC("CMPXDbPlaylistSongs::SongInfoExistsL");

    RSqlStatement recordset(
        iDbManager.ExecuteSelectQueryL(aDriveId, KQueryPlaylistSongInfoExists, aSongId));
    TBool exists(recordset.Next() == KSqlAtRow);
    recordset.Close();
    return exists;
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylistSongs::CleanupSongInfoL
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylistSongs::CleanupSongInfoL(
    TInt aDriveId)
    {
    MPX_FUNC("CMPXDbPlaylistSongs::CleanupSongInfoL");
    iDbManager.ExecuteQueryL(aDriveId, KQueryPlaylistSongInfoCleanup);
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylistSongs::CreateTableL
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylistSongs::CreateTableL(
    RSqlDatabase& aDatabase,
    TBool /* aCorruptTable */)
    {
    MPX_FUNC("CMPXDbPlaylistSongs::CreateTableL");

    // create the PlaylistSongs table
    User::LeaveIfError(aDatabase.Exec(KPlaylistSongsCreateTable));

    // create the PlaylistSongInfo table
    User::LeaveIfError(aDatabase.Exec(KPlaylistSongInfoCreateTable));
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylistSongs::DropTableL
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylistSongs::DropTableL(
    RSqlDatabase& aDatabase)
    {
    MPX_FUNC("CMPXDbPlaylistSongs::DropTableL");

    // drop the PlaylistSongs table
    User::LeaveIfError(aDatabase.Exec(KPlaylistSongsDropTable));

    // drop the PlaylistSongInfo table
    User::LeaveIfError(aDatabase.Exec(KPlaylistSongInfoDropTable));
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylistSongs::CheckTableL
// ----------------------------------------------------------------------------
//
TBool CMPXDbPlaylistSongs::CheckTableL(
    RSqlDatabase& aDatabase)
    {
    MPX_FUNC("CMPXDbPlaylistSongs::CheckTableL");

    // check the PlaylistSongs table
    TBool check = DoCheckTable(aDatabase, KPlaylistSongsCheckTable);

    // check the PlaylistSongInfo table
    return check && DoCheckTable(aDatabase, KPlaylistSongInfoCheckTable);
    }

// End of File
