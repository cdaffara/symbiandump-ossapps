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
* Description:  Responsible for interaction with the playlist tables.
*
*/


// INCLUDE FILES
#include <f32file.h>
#include <mpxlog.h>
#include <mpxmediacontainerdefs.h>

#include "mpxdbcommonutil.h"
#include "mpxdbcommondef.h"
#include "mpxdbcommonstd.h"

#include "mpxcollectiondbdef.h"
#include "mpxdbmanager.h"
#include "mpxdbutil.h"
#include "mpxdbpluginqueries.h"
#include "mpxdbplaylistsongs.h"
#include "mpxdbplaylist.h"

// CONSTANTS
// UniqueID column in Uris requests
const TInt KColUniqueID = 0;
// URI column in Uris requests
const TInt KColUri = 1;

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbPlaylist* CMPXDbPlaylist::NewL(
    CMPXDbManager& aDbManager,
	MMPXDbPlaylistObserver& aObserver)
    {
    MPX_FUNC("CMPXDbPlaylist::NewL");

    CMPXDbPlaylist* self = CMPXDbPlaylist::NewLC(aDbManager, aObserver);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbPlaylist* CMPXDbPlaylist::NewLC(
    CMPXDbManager& aDbManager,
	MMPXDbPlaylistObserver& aObserver)
    {
    MPX_FUNC("CMPXDbPlaylist::NewLC");

	CMPXDbPlaylist* self = new (ELeave) CMPXDbPlaylist(aDbManager, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CMPXDbPlaylist::~CMPXDbPlaylist()
    {
    MPX_FUNC("CMPXDbPlaylist::~CMPXDbPlaylist");
    delete iPlaylistSongs;
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CMPXDbPlaylist::CMPXDbPlaylist(
    CMPXDbManager& aDbManager,
    MMPXDbPlaylistObserver& aObserver) :
    CMPXDbTable(aDbManager),
	iObserver(aObserver)
    {
    MPX_FUNC("CMPXDbPlaylist::CMPXDbPlaylist");
    }

// ----------------------------------------------------------------------------
// Second phase constructor.
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylist::ConstructL()
    {
    MPX_FUNC("CMPXDbPlaylist::ConstructL");

    BaseConstructL();
    iPlaylistSongs = CMPXDbPlaylistSongs::NewL(iDbManager);
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylist::AddPlaylistL
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbPlaylist::AddPlaylistL(
    const CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbPlaylist::AddPlaylistL");

    // make sure the playlist and the corresponding songs are deleted
    TUint32 playlistId(MPXDbCommonUtil::GenerateUniqueIdL(iDbManager.Fs(), EMPXPlaylist,
        aMedia.ValueText(KMPXMediaGeneralUri), EFalse));
    DeletePlaylistNoUriL(playlistId);

    // add the playlist
    return DoAddPlaylistL(aMedia, TDriveUnit(aMedia.ValueText(KMPXMediaGeneralUri)));
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylist::AddSongsL
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylist::AddSongsL(
    TUint32 aPlaylistId,
    const CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbPlaylist::AddSongsL");

    // get the drive ID of corresponding playlist
    TInt drive(GetDriveIdL(aPlaylistId));

    // add the songs
    iPlaylistSongs->AddSongsL(aPlaylistId, aMediaArray, drive);

    // update the time for the playlist
    UpdatePlaylistTimeL(aPlaylistId, drive);
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylist::UpdatePlaylistL
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylist::UpdatePlaylistL(
    const CMPXMedia& aMedia,
    CMPXMessage& aMessage,
    TInt aDriveId)
    {
    MPX_FUNC("CMPXDbPlaylist::UpdatePlaylistL");

    TUint32 playlistId((aMedia.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId)).iId2);
    DoUpdatePlaylistL(playlistId, aMedia, aDriveId, aMessage);
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylist::UpdateSongL
// ----------------------------------------------------------------------------
//
TBool CMPXDbPlaylist::UpdateSongL(
    const CMPXMedia& aMedia,
    TBool aResetFlags,
    CMPXMessageArray* aItemChangedMessages /* = NULL */)
    {
    MPX_FUNC("CMPXDbPlaylist::UpdateSongL");

    // find the song ID
    TInt oldSongId(0);
    TInt newSongId(0);
    if (aMedia.IsSupported(KMPXMediaGeneralId))
        {
        oldSongId = (aMedia.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId)).iId2;
        }
    if (aMedia.IsSupported(KMPXMediaGeneralUri))
        {
        newSongId = MPXDbCommonUtil::GenerateUniqueIdL(iDbManager.Fs(), EMPXCollection,
            aMedia.ValueText(KMPXMediaGeneralUri), EFalse);
        }
    if ( !aMedia.IsSupported( KMPXMediaGeneralId ) && !aMedia.IsSupported( KMPXMediaGeneralUri ))
        {
        User::Leave( KErrArgument );
        }

    if ( newSongId <= 0 )
        {
        newSongId = oldSongId;
        }

    if ( oldSongId <= 0 )
        {
        oldSongId = newSongId;
        }

    // update the PlaylistSongs and PlaylistSongInfo tables first
    TBool updated(EFalse);
    TBool visible(iPlaylistSongs->UpdateSongL( oldSongId, aMedia, aResetFlags, updated ));
    TBool bSongInPlaylists( EFalse );
    if (updated)
        {
        UpdatePlaylistsForSongL( newSongId, aItemChangedMessages,bSongInPlaylists );
        }

    return visible;
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylist::DeleteSongL
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylist::DeleteSongL(
    TUint32 aSongId,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbPlaylist::DeleteSongL");
    TBool bSongInPlaylists(EFalse);
    // add item changed messages for all playlists that contain the song
    UpdatePlaylistsForSongL (aSongId, &aItemChangedMessages, bSongInPlaylists);
    if (bSongInPlaylists)
        {
        // delete the song from the PlaylistSongs and PlaylistSongInfo tables
        iPlaylistSongs->DeleteSongL (aSongId);
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylist::DeleteSongL
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylist::DeleteSongL(
    TUint32 aPlaylistId,
    TUint32 aSongId,
    TInt aOrdinal)
    {
    MPX_FUNC("CMPXDbPlaylist::DeleteSongL");

    // get playlist drive
    TInt drive(GetDriveIdL(aPlaylistId));

    // delete the song from the PlaylistSongs / PlaylistSongInfo tables
    iPlaylistSongs->DeleteSongL(aPlaylistId, aSongId, aOrdinal, drive);

    // update the time for the playlist
    UpdatePlaylistTimeL(aPlaylistId, drive);
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylist::DeletePlaylistL
// ----------------------------------------------------------------------------
//
HBufC* CMPXDbPlaylist::DeletePlaylistL(
    TUint32 aPlaylistId)
    {
    MPX_FUNC("CMPXDbPlaylist::DeletePlaylistL");

    // get the uri
    HBufC* uri = GetUriL(aPlaylistId);
    if (uri)
        {
        CleanupStack::PushL(uri);
        TDriveUnit drive(*uri);

        // delete the songs from the PlaylistSongs table
        iPlaylistSongs->DeleteSongsL(aPlaylistId, drive);

        // delete the playlist record from the Playlist table
        iDbManager.ExecuteQueryL(drive, KQueryPlaylistDelete, aPlaylistId);

        CleanupStack::Pop(uri);
        }

    return uri;
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylist::DeletePlaylistNoUriL
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylist::DeletePlaylistNoUriL(
    TUint32 aPlaylistId)
    {
    MPX_FUNC("CMPXDbPlaylist::DeletePlaylistNoUriL");

    TInt drive(0);
    MPX_TRAPD(err, drive = GetDriveIdL(aPlaylistId));
    if (err != KErrNotFound)
        {
        User::LeaveIfError(err);

        // delete the songs from the PlaylistSongs table
        iPlaylistSongs->DeleteSongsL(aPlaylistId, drive);

        // delete the playlist record from the Playlist table
        iDbManager.ExecuteQueryL(drive, KQueryPlaylistDelete, aPlaylistId);
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylist::DeleteAllPlaylistsL
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylist::DeleteAllPlaylistsL()
    {
    MPX_FUNC("CMPXDbPlaylist::DeleteAllPlaylistsL");

    // delete the songs from the PlaylistSongs table
    iPlaylistSongs->DeleteAllSongsL();

    // delete all playlists
    iDbManager.ExecuteQueryL(KDbManagerAllDrives, KQueryPlaylistDeleteAll);
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylist::CountL
// ----------------------------------------------------------------------------
//
TInt CMPXDbPlaylist::CountL()
    {
    MPX_FUNC("CMPXDbPlaylist::CountL");
    return ExecuteSumQueryL(KQueryPlaylistCount);
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylist::GetPlaylistL
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylist::GetPlaylistL(
    TUint32 aPlaylistId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbPlaylist::GetPlaylistL");
    ExecuteMediaQueryL(aAttrs, aMedia, KQueryPlaylistGet, aPlaylistId);
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylist::GetAllPlaylistsL
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylist::GetAllPlaylistsL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbPlaylist::GetAllPlaylistsL");
    ExecuteMediaQueryL(aAttrs, aMediaArray, KQueryPlaylistGetAll);
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylist::GetNameL
// ----------------------------------------------------------------------------
//
HBufC* CMPXDbPlaylist::GetNameL(
    TUint32 aPlaylistId)
    {
    MPX_FUNC("CMPXDbPlaylist::GetNameL");

    RSqlStatement recordset(GetPlaylistRecordL(aPlaylistId));
    CleanupClosePushL(recordset);

    HBufC* name(NULL);
    TInt err(KErrNone);
    if ((err = recordset.Next()) == KSqlAtRow)
        {
        name = MPXDbCommonUtil::GetColumnTextL(recordset, EPlaylistName).AllocL();
        }
    CleanupStack::PopAndDestroy(&recordset);

    if (err != KSqlAtEnd)
        {
        User::LeaveIfError(err);
        }

    if (name == NULL)
        {
        User::LeaveIfError(KErrNotFound);
        }
    return name;
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylist::GetUriL
// ----------------------------------------------------------------------------
//
HBufC* CMPXDbPlaylist::GetUriL(
    TUint32 aPlaylistId)
    {
    MPX_FUNC("CMPXDbPlaylist::GetUriL");

    RSqlStatement recordset(GetPlaylistRecordL(aPlaylistId));
    CleanupClosePushL(recordset);

    HBufC* uri(NULL);
    TInt err(KErrNone);
    if ((err = recordset.Next()) == KSqlAtRow)
        {
        uri = MPXDbCommonUtil::CreateFullPathL(
            MPXDbCommonUtil::GetDriveIdMatchVolIdL(iDbManager.Fs(),
            recordset.ColumnInt64(EPlaylistVolumeId)),
            MPXDbCommonUtil::GetColumnTextL(recordset, EPlaylistUri));

        }
    CleanupStack::PopAndDestroy(&recordset);

    if (err != KSqlAtEnd)
        {
        User::LeaveIfError(err);
        }

    if (uri == NULL)
        {
        User::LeaveIfError(KErrNotFound);
        }

    return uri;
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylist::GetIdL
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbPlaylist::GetIdL(
    const TDesC& aUri)
    {
    MPX_FUNC("CMPXDbPlaylist::GetIdL");
    HBufC* uri = MPXDbCommonUtil::ProcessPatternCharsLC( aUri );
    TUint32 ret = ExecuteIntQueryL(
        KQueryLikePlaylistId, uri->Mid( KMCPathStartPos ) );

   	CleanupStack::PopAndDestroy( uri );

    return ret;
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylist::FindAllL
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylist::FindAllL(
    const CMPXMedia& aCriteria,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbPlaylist::FindAllL");

    // process the requested attributes
    // the UniqueId is always requested
    TBool titleRequested(EFalse);
    TBool counterRequested(EFalse);

    TInt viewingColumnCount(aAttrs.Count());
    for (TInt i = 0; (i < viewingColumnCount) && !(titleRequested && counterRequested); ++i)
        {
        if (aAttrs[i].ContentId() == KMPXMediaIdGeneral)
            {
            TUint attributeId(aAttrs[i].AttributeId());

            if (attributeId & EMPXMediaGeneralTitle)
                {
                titleRequested = ETrue;
                }
            if (attributeId & EMPXMediaGeneralCount)
                {
                counterRequested = ETrue;
                }
            }
        }

    TMPXGeneralType type = aCriteria.ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType);

    const TArray<TMPXAttribute> criteria = aCriteria.Attributes();
    TInt criteriaCount(criteria.Count());

    // process the criteria and construct the criteria string
    CDesCArrayFlat* criteriaArray = new (ELeave) CDesCArrayFlat(criteriaCount);
    CleanupStack::PushL(criteriaArray);

    TBool criteriaCounterSet(EFalse);
    TInt criteriaCounter(0);

    for (TInt i = 0; i < criteriaCount; ++i)
        {
        const TMPXAttribute& criterion = criteria[i];
        if (type == EMPXItem && criterion == KMPXMediaGeneralId)
            {
            TUint32 itemId((aCriteria.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId)).iId2);
            if (MPX_ITEM_CATEGORY(itemId) != EMPXPlaylist)
                {
                User::Leave(KErrNotSupported);
                }

            MPXDbCommonUtil::AddSqlCriterionL(*criteriaArray, KCriterionPlaylistUniqueId, itemId);
            }
        else if (criterion == KMPXMediaGeneralTitle)
            {
            HBufC* title = MPXDbCommonUtil::ProcessPatternCharsLC(
                aCriteria.ValueText(KMPXMediaGeneralTitle));
            if( type == EMPXOtherType )
                {
                MPXDbCommonUtil::AddSqlCriterionL(*criteriaArray, KCriterionPlaylistTitle, *title);
                }
            else
                {
            MPXDbCommonUtil::AddSqlCriterionL(*criteriaArray, KCriterionPlaylistLikeTitle, *title);
                }
            CleanupStack::PopAndDestroy(title);
            }
        else if (criterion == KMPXMediaGeneralUri)
            {
            MPXDbCommonUtil::AddSqlCriterionL(*criteriaArray, KCriterionPlaylistUniqueId,
                MPXDbCommonUtil::GenerateUniqueIdL(iDbManager.Fs(), EMPXPlaylist,
                aCriteria.ValueText(KMPXMediaGeneralUri), EFalse));
            }
        else if (criterion == KMPXMediaGeneralDrive)
            {
            const TDesC& drive(aCriteria.ValueText(KMPXMediaGeneralDrive));
            TDriveUnit driveUnit(drive);
            MPXDbCommonUtil::AddSqlCriterionL(*criteriaArray, KCriterionPlaylistVolumeId,
                MPXDbCommonUtil::GetVolIdMatchDriveIdL(iDbManager.Fs(), driveUnit));
            }
        else if (criterion == KMPXMediaGeneralSynchronized)
            {
            MPXDbCommonUtil::AddSqlCriterionL(*criteriaArray, KCriterionPlaylistSync,
                aCriteria.ValueTObjectL<TBool>(KMPXMediaGeneralSynchronized));
            }
        else if (criterion == KMPXMediaGeneralCount)
            {
            criteriaCounterSet = ETrue;
            criteriaCounter = aCriteria.ValueTObjectL<TInt>(KMPXMediaGeneralCount);
            }
        }

    // construct criteria string
    HBufC* criteriaStr = MPXDbCommonUtil::StringFromArrayLC(*criteriaArray, KMCAndKeyword);

    HBufC* query(NULL);
    if (criteriaStr->Length() > 0)
        {
        // construct and execute the query
        query = HBufC::NewLC(KQueryPlaylistItems().Length() + criteriaStr->Length());
        query->Des().Format(KQueryPlaylistItems, criteriaStr);
        }
    else
        {
        query = HBufC::NewLC(KQueryPlaylistGetAll().Length());
        query->Des().Format(KQueryPlaylistGetAll);
        }

    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(*query));

    CleanupStack::PopAndDestroy(query);
    CleanupStack::PopAndDestroy(criteriaStr);
    CleanupStack::PopAndDestroy(criteriaArray);

    CleanupClosePushL(recordset);

    TInt err(KErrNone);
    while ((err = recordset.Next()) == KSqlAtRow)
        {
        // Setup basic info - with first record of a group
        TUint32 playlistId(recordset.ColumnInt64(EPlaylistUniqueId));
        TUint32 volId(recordset.ColumnInt64(EPlaylistVolumeId));
        TInt driveId = MPXDbCommonUtil::GetDriveIdMatchVolIdL(iDbManager.Fs(), volId);

        TBool valid(ETrue);
        TInt songCount(-1);
        if (criteriaCounterSet)
            {
            if (driveId >= 0)
                {
                //valid disk
                songCount = iPlaylistSongs->CountL(playlistId, driveId);
                }
            valid = (criteriaCounter == songCount);
            }

        if (valid)
            {
            // start a new media object
            CMPXMedia* media = CMPXMedia::NewL();
            CleanupStack::PushL(media);

            UpdateMediaL(recordset, aAttrs, *media);

            aMediaArray.AppendL(*media);
            CleanupStack::PopAndDestroy(media);
            }
        }

    CleanupStack::PopAndDestroy(&recordset);

    if (err != KSqlAtEnd)
        {
        User::LeaveIfError(err);
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylist::GetDriveIdL
// ----------------------------------------------------------------------------
//
TInt CMPXDbPlaylist::GetDriveIdL(
    TUint32 aPlaylistId)
    {
    MPX_FUNC("CMPXDbPlaylist::GetDriveIdL");
    return MPXDbCommonUtil::GetDriveIdMatchVolIdL(iDbManager.Fs(),
        ExecuteIntQueryL(KQueryPlaylistDriveId, aPlaylistId));
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylist::Songs
// ----------------------------------------------------------------------------
//
CMPXDbPlaylistSongs& CMPXDbPlaylist::Songs()
    {
    return *iPlaylistSongs;
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylist::GetDrivePlaylistCount
// ----------------------------------------------------------------------------
//
TUint CMPXDbPlaylist::GetDrivePlaylistCountL(TInt aDrive)
    {
    TUint count(0);

    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(aDrive,KQueryPlaylistFileCount));
    CleanupClosePushL(recordset);

    if (recordset.Next() != KSqlAtRow)
        {
        User::Leave(KErrCorrupt);
        }

    count = TUint(recordset.ColumnInt64(KMPXTableDefaultIndex));
    CleanupStack::PopAndDestroy(&recordset);

    return count;
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylist::GetPlaylistUriArrayL
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylist::GetPlaylistUriArrayL(TInt aDrive, TInt aFromID, TInt aRecords,
                                          CDesCArray& aUriArr, TInt& aLastID)
    {
    MPX_FUNC("CMPXDbPlaylist::GetPlaylistUriArrayL");

    HBufC* query = NULL;
    if(aFromID == 0)
        {
        query = HBufC::NewLC(KQueryPlaylistGetFileUris().Length()
                            + KMCIntegerLen);
        query->Des().Format(KQueryPlaylistGetFileUris, aRecords);
        }
    else
        {
        query = HBufC::NewLC(KQueryPlaylistGetFileUrisFrom().Length()
                            + 2*KMCIntegerLen);
        query->Des().Format(KQueryPlaylistGetFileUrisFrom, aFromID, aRecords);
        }

    RSqlStatement playlist_rs(iDbManager.ExecuteSelectQueryL(aDrive,*query));

    CleanupStack::PopAndDestroy(query);

    CleanupClosePushL(playlist_rs);

    TInt lastID = 0;
    TInt err(KErrNone);
    while((err = playlist_rs.Next()) == KSqlAtRow)
        {
        HBufC* fullPath = MPXDbCommonUtil::CreateFullPathL(aDrive,
                MPXDbCommonUtil::GetColumnTextL(playlist_rs, KColUri));
        CleanupStack::PushL(fullPath);
        aUriArr.AppendL(*fullPath);
        CleanupStack::PopAndDestroy(fullPath);

        lastID = playlist_rs.ColumnInt(KColUniqueID);
        }
    CleanupStack::PopAndDestroy(&playlist_rs);

    aLastID = lastID;

    if (err!= KSqlAtEnd)
        {
        User::Leave(KErrCorrupt);
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylist::UpdateMediaL
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylist::UpdateMediaL(
    RSqlStatement& aRecord,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbPlaylist::UpdateMediaL");

    TBool countRequested(EFalse);
	TBool durationRequested(EFalse);
    TUint32 playlistId(aRecord.ColumnInt64(EPlaylistUniqueId));

    TInt count(aAttrs.Count());
    TUint32 volId(aRecord.ColumnInt64(EPlaylistVolumeId));
    TInt driveId = MPXDbCommonUtil::GetDriveIdMatchVolIdL(iDbManager.Fs(), volId);
    for (TInt i = 0; i < count; ++i)
        {
        TInt contentId(aAttrs[i].ContentId());
        TUint attributeId(aAttrs[i].AttributeId());

        if (contentId == KMPXMediaIdGeneral)
            {
            if (attributeId & EMPXMediaGeneralId)
                {
                aMedia.SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, playlistId);
                MPX_DEBUG2("    Playlist ID[%u]", playlistId);
                }
            if (attributeId & EMPXMediaGeneralCollectionId)
                {
                aMedia.SetTObjectValueL<TUid>(KMPXMediaGeneralCollectionId,
                    TUid::Uid(KDBPluginUid));
                }
            if (attributeId & EMPXMediaGeneralTitle)
                {
                TPtrC title(MPXDbCommonUtil::GetColumnTextL(aRecord, EPlaylistName));
                aMedia.SetTextValueL(KMPXMediaGeneralTitle, title);
                MPX_DEBUG2("    Title[%S]", &title);
                }
            if ((attributeId & EMPXMediaGeneralUri) ||
                (attributeId & EMPXMediaGeneralDrive) ||
                (attributeId & EMPXMediaGeneralFlags))
                {

                // LTAN-7GH6BZ, crash if eject memory card when adding song to existing playlist
                // due to special timing issue, it is possible drive number is -1 and create a
                // panic when use for TDriveUnit
                MPX_DEBUG3("volId = %d, driveId = %d", volId, driveId);

                // handle possibly delay from framework notification
                if (driveId < 0)
                    {
                    MPX_DEBUG1("invalid driveId, leave with KErrNotReady");
                    User::Leave(KErrNotReady);
                    }

                TDriveUnit driveUnit(driveId);

                if (attributeId & EMPXMediaGeneralUri)
                    {
                    HBufC* fullPath = MPXDbCommonUtil::CreateFullPathL(driveUnit,
                        MPXDbCommonUtil::GetColumnTextL(aRecord, EPlaylistUri));
                    CleanupStack::PushL(fullPath);
                    aMedia.SetTextValueL(KMPXMediaGeneralUri, *fullPath);
                    MPX_DEBUG2("    URI[%S]", fullPath);
                    CleanupStack::PopAndDestroy(fullPath);
                    }
                if (attributeId & EMPXMediaGeneralDrive)
                    {
                    aMedia.SetTextValueL(KMPXMediaGeneralDrive, driveUnit.Name());
                    }
                if (attributeId & EMPXMediaGeneralFlags)
                    {
                    TUint32 dbFlags(aRecord.ColumnInt64(EPlaylistDbFlag));
                    TInt driveId = driveUnit & KMPXMediaGeneralFlagsDriveInfo;  // 5 bits
                    aMedia.SetTObjectValueL<TUint>(KMPXMediaGeneralFlags, dbFlags | driveId);
                    }
                }
            if (attributeId & EMPXMediaGeneralSynchronized)
                {
                aMedia.SetTObjectValueL<TBool>(KMPXMediaGeneralSynchronized,
                    aRecord.ColumnInt(EPlaylistSync));
                }
            if (attributeId & EMPXMediaGeneralCount)
                {
                // make sure the PlaylistSongs query is executed after all fields
                // from the current record have been processed, otherwise the recordset
                // may point to something else
                countRequested = ETrue;
                }
            if (attributeId & EMPXMediaGeneralDate)
                {
                // convert the time from the internal DB string format
                // to the int64 format used by TTime
                aMedia.SetTObjectValueL<TInt64>(KMPXMediaGeneralDate,
                    MPXDbCommonUtil::DesToTTimeL(
                    MPXDbCommonUtil::GetColumnTextL(aRecord, EPlaylistTime)).Int64());
                }
			if ( attributeId & EMPXMediaGeneralDuration )
				{
				// make sure the PlaylistSongs query is executed after all fields
				// from the current record have been processed, otherwise the recordset
				// may point to something else

				durationRequested = ETrue;
				}
            } // end if contentId == KMPXMediaIdGeneral
        } // end for

    TInt plSongCount(0);
    TInt plSongDuration(0);
    if (countRequested)
        {
        if (driveId >= 0)
            {
            //valid disk
            iObserver.HandlePlaylistInfoL(playlistId, plSongCount, plSongDuration);
            }
        aMedia.SetTObjectValueL<TInt>(KMPXMediaGeneralCount,
        		plSongCount );
        
        MPX_DEBUG1("    EMPXMediaGeneralCount");
		MPX_DEBUG2("	Count[%d]", plSongCount);				
        }
	if (durationRequested)
		{
        aMedia.SetTObjectValueL<TInt>(KMPXMediaGeneralDuration, plSongDuration);
		
        MPX_DEBUG1("    EMPXMediaGeneralDuration");
        MPX_DEBUG2("	Duration[%d]", plSongDuration);				
		}

    aMedia.SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
    aMedia.SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXPlaylist);
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylist::GetPlaylistRecordL
// ----------------------------------------------------------------------------
//
RSqlStatement CMPXDbPlaylist::GetPlaylistRecordL(
    TUint32 aPlaylistId)
    {
    MPX_FUNC("CMPXDbPlaylist::GetPlaylistRecordL");
    return iDbManager.ExecuteSelectQueryL(KQueryPlaylistGet, aPlaylistId);
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylist::DoAddPlaylistL
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbPlaylist::DoAddPlaylistL(
    const CMPXMedia& aMedia,
    TInt aDriveId)
    {
    MPX_FUNC("CMPXDbPlaylist::DoAddPlaylistL");

    const TDesC& playlistName(aMedia.ValueText(KMPXMediaGeneralTitle));
    const TDesC& playlistUri(aMedia.ValueText(KMPXMediaGeneralUri));

    TUint32 playlistId(MPXDbCommonUtil::GenerateUniqueIdL(iDbManager.Fs(), EMPXPlaylist, playlistUri, EFalse));
    const CMPXMediaArray* mediaArray = aMedia.Value<CMPXMediaArray>(KMPXMediaArrayContents);
    if( !mediaArray )
        {
        User::Leave( KErrNoMemory );
        }
    // add the songs to the PlaylistSongs table
    TInt count(mediaArray->Count());
    for (TInt i = 0; i < count; ++i)
        {
        iPlaylistSongs->AddSongL(playlistId, i, *mediaArray->AtL(i), aDriveId);
        }

    // determine the value of DbFlag
    TUint dbflags(0);
    if (aMedia.IsSupported(KMPXMediaGeneralFlags))
        {
        TUint flag(aMedia.ValueTObjectL<TUint>(KMPXMediaGeneralFlags));
        if (flag & KMPXMediaGeneralFlagsSetOrUnsetBit )
            {
            // Set flag
            dbflags |= flag;
            MPX_DEBUG2("    GeneralFlags[%b]", dbflags);
            }
        }

    // add playlist record
    HBufC* time = MPXDbCommonUtil::CurrentTimeDesLC();
    HBufC* name = MPXDbCommonUtil::ProcessSingleQuotesLC(playlistName);
    HBufC* uri = MPXDbCommonUtil::ProcessSingleQuotesLC(playlistUri);
    TPtrC uriPtr(uri->Mid(KMCPathStartPos));
    TPtrC namePtr(name->Left(KMCMaxTextLen));

    iDbManager.ExecuteQueryL(aDriveId, KQueryPlaylistInsert, playlistId, &namePtr,
        &uriPtr, MPXDbCommonUtil::GetVolIdMatchDriveIdL(iDbManager.Fs(), aDriveId),
        dbflags, time);

    CleanupStack::PopAndDestroy(uri);
    CleanupStack::PopAndDestroy(name);
    CleanupStack::PopAndDestroy(time);

    return playlistId;
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylist::DoUpdatePlaylistL
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylist::DoUpdatePlaylistL(
    TUint32 aPlaylistId,
    const CMPXMedia& aMedia,
    TInt aDriveId,
    CMPXMessage& aMessage)
    {
    MPX_FUNC("CMPXDbPlaylist::DoUpdatePlaylistL");

    // construct the criteria array
    const TArray<TMPXAttribute> attributes = aMedia.Attributes();

    CDesCArrayFlat* criteriaArray = new (ELeave) CDesCArrayFlat(attributes.Count());
    CleanupStack::PushL(criteriaArray);

    TInt attrCount(attributes.Count());
    for (TInt index = 0; index < attrCount; ++index)
        {
        TInt contentId(attributes[index].ContentId());
        TUint attributeId(attributes[index].AttributeId());

        switch(contentId)
            {
            case KMPXMediaIdGeneral:
                {
                if (attributeId & EMPXMediaGeneralTitle)
                    {
                    MPXDbCommonUtil::AddSqlCriterionL(*criteriaArray, KCriterionPlaylistTitle,
                        aMedia.ValueText(KMPXMediaGeneralTitle));
                    }
                if (attributeId & EMPXMediaGeneralUri)
                    {
                    const TDesC& uri(aMedia.ValueText (KMPXMediaGeneralUri));

                    // determine if we are changing URI of an existing playlist.
                    // if so, update playlist URI + its Id
                    TUint32 newId(MPXDbCommonUtil::GenerateUniqueIdL(iDbManager.Fs(), EMPXPlaylist, uri, EFalse));

                    if (aPlaylistId != newId)
                        {
                        aMessage.SetTObjectValueL<TMPXItemId>(KMPXMessageMediaGeneralId, newId);
                        aMessage.SetTObjectValueL<TMPXItemId>(KMPXMessageMediaDeprecatedId, aPlaylistId);

                        // Change event handling for renaming a playlist should be like a song
                        // The item focus should goto the new name of the playlist
                        // to-do: this should be abstracted from the plugin. framework should
                        //        have enough info to deal with this scenario, if not, add more
                        //        info in the message passing back to framework
                        aMessage.SetTObjectValueL<TMPXGeneralCategory>(KMPXMessageMediaGeneralCategory,
                            EMPXSong);

                        // update the PlaylistSongs to reflect playlist id change
                        iPlaylistSongs->UpdateSongsL(aPlaylistId, newId);

                        // this takes care of processing the single quotes in the URI
                        MPXDbCommonUtil::AddSqlCriterionL(*criteriaArray, KCriterionPlaylistUri,
                            uri.Mid(KMCPathStartPos));
                        MPXDbCommonUtil::AddSqlCriterionL(*criteriaArray, KCriterionPlaylistVolumeId,
                            MPXDbCommonUtil::GetVolIdMatchDriveIdL(iDbManager.Fs(), TDriveUnit(uri)));
                        MPXDbCommonUtil::AddSqlCriterionL(*criteriaArray, KCriterionPlaylistUniqueId,
                            newId);
                        }
                    }
                if (attributeId & EMPXMediaGeneralSynchronized)
                    {
                    MPXDbCommonUtil::AddSqlCriterionL(*criteriaArray, KCriterionPlaylistSync,
                        aMedia.ValueTObjectL<TBool>(KMPXMediaGeneralSynchronized));
                    }
                }
                break;

            default:
                break;

            } // end switch
        } // end for

    // update the time field to the current time
    HBufC* time = MPXDbCommonUtil::CurrentTimeDesLC();
    MPXDbCommonUtil::AddSqlCriterionL(*criteriaArray, KCriterionPlaylistTime, *time);
    CleanupStack::PopAndDestroy(time);

    // construct a string from all criteria
    HBufC* criteria = MPXDbCommonUtil::StringFromArrayLC(*criteriaArray, KMCCommaSign);

    // execute the query
    iDbManager.ExecuteQueryL(aDriveId, KQueryPlaylistUpdate, criteria, aPlaylistId);

    CleanupStack::PopAndDestroy(criteria);
    CleanupStack::PopAndDestroy(criteriaArray);
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylist::UpdatePlaylistsForSongL
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylist::UpdatePlaylistsForSongL(
    TUint32 aSongId,
    CMPXMessageArray* aItemChangedMessages, TBool& aSongInPlaylists)
    {
    MPX_FUNC("CMPXDbPlaylist::UpdatePlaylistsForSongL");

    aSongInPlaylists = EFalse;
    if (aItemChangedMessages)
        {
        // get all playlists for the song
        RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryPlaylistGetForSong, aSongId));
        CleanupClosePushL(recordset);

        // ignore the errors
        while (recordset.Next() == KSqlAtRow)
            {
            aSongInPlaylists = ETrue;
            // add item changed messages for all of them
            MPXDbCommonUtil::AddItemChangedMessageL(*aItemChangedMessages,
                recordset.ColumnInt64(KMPXTableDefaultIndex), EMPXItemModified,
                EMPXPlaylist, KDBPluginUid);
            }

        CleanupStack::PopAndDestroy(&recordset);
        }
    if (aSongInPlaylists)
        {
        // update the time for all playlists that contain this song
        HBufC* time = MPXDbCommonUtil::CurrentTimeDesLC();
        iDbManager.ExecuteQueryL (KDbManagerAllDrives,
                KQueryPlaylistUpdateTimeForSong, time, aSongId);
        CleanupStack::PopAndDestroy (time);
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylist::CreateTableL
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylist::UpdatePlaylistTimeL(
    TUint32 aPlaylistId,
    TInt aDrive)
    {
    MPX_FUNC("CMPXDbPlaylist::UpdatePlaylistTimeL");

    HBufC* time = MPXDbCommonUtil::CurrentTimeDesLC();
    iDbManager.ExecuteQueryL(aDrive, KQueryPlaylistUpdateTime, time, aPlaylistId);
    CleanupStack::PopAndDestroy(time);
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylist::GetDrivePlaylistDuration
// ----------------------------------------------------------------------------
//
TInt CMPXDbPlaylist::GetDrivePlaylistDuration(TUint32 /*aPlaylistId*/)
    {
    return 0;
    }
// ----------------------------------------------------------------------------
// CMPXDbPlaylist::CreateTableL
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylist::CreateTableL(
    RSqlDatabase& aDatabase,
    TBool /* aCorruptTable */)
    {
    MPX_FUNC("CMPXDbPlaylist::CreateTableL");

    // create the table
    User::LeaveIfError(aDatabase.Exec(KPlaylistCreateTable));

    // create the index on the Name field
    User::LeaveIfError(aDatabase.Exec(KPlaylistNameIndex));
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylist::DropTableL
// ----------------------------------------------------------------------------
//
void CMPXDbPlaylist::DropTableL(
    RSqlDatabase& aDatabase)
    {
    MPX_FUNC("CMPXDbPlaylist::DropTableL");
    User::LeaveIfError(aDatabase.Exec(KPlaylistDropTable));
    }

// ----------------------------------------------------------------------------
// CMPXDbPlaylist::CheckTableL
// ----------------------------------------------------------------------------
//
TBool CMPXDbPlaylist::CheckTableL(
    RSqlDatabase& aDatabase)
    {
    MPX_FUNC("CMPXDbPlaylist::CheckTableL");
    return DoCheckTable(aDatabase, KPlaylistCheckTable);
    }

// End of File
