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
* Description:  Responsible for interation with the music table.
*
*/


// INCLUDE FILES
#include <mpxlog.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxcollectionpath.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxmediaaudiodefs.h>
#include <mpxmediadrmdefs.h>
#include <mpxmediamtpdefs.h>
#include <mpxcollectiondbhgres.rsg>

#include "mpxdbcommondef.h"
#include "mpxdbcommonstd.h"
#include "mpxdbcommonutil.h"
#include "mpxresource.h"

#include "mpxcollectiondbdef.h"
#include "mpxcollectiondbstd.h"
#include "mpxdbpluginqueries.h"
#include "mpxdbutil.h"
#include "mpxdbmanager.h"
#include "mpxdbmusic.h"

// CONSTANTS

// UniqueID column in Uris requests
const TInt KColUniqueID = 0;
// URI column in Uris requests
const TInt KColUri = 1;
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
_LIT( KAbstractAlbumExt, ".alb" );
#endif // ABSTRACTAUDIOALBUM_INCLUDED

const TInt KAllSongsQueryResultGranularity = 250;

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbMusic* CMPXDbMusic::NewL(
    CMPXDbManager& aDbManager,
    CMPXResource& aResource,
    MMPXDbMusicObserver& aObserver)
    {
    MPX_FUNC("CMPXDbMusic::NewL");
    CMPXDbMusic* self = CMPXDbMusic::NewLC(aDbManager, aResource, aObserver);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbMusic* CMPXDbMusic::NewLC(
    CMPXDbManager& aDbManager,
    CMPXResource& aResource,
    MMPXDbMusicObserver& aObserver)
    {
    MPX_FUNC("CMPXDbMusic::NewLC");

    CMPXDbMusic* self = new (ELeave) CMPXDbMusic(aDbManager, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aResource);
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CMPXDbMusic::~CMPXDbMusic()
    {
    MPX_FUNC("CMPXDbMusic::~CMPXDbMusic");
    delete iExtensionsDrm;
    iAllSongsQueryResult.ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CMPXDbMusic::CMPXDbMusic(
    CMPXDbManager& aDbManager,
    MMPXDbMusicObserver& aObserver) :
    CMPXDbTable(aDbManager),
    iObserver(aObserver)
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
    ,iArtNeedUpdated(ETrue)
#endif
    ,iAllSongsQueryResult(KAllSongsQueryResultGranularity)
    {
    MPX_FUNC("CMPXDbMusic::CMPXDbMusic");
    }

// ----------------------------------------------------------------------------
// Second phase constructor.
// ----------------------------------------------------------------------------
//
void CMPXDbMusic::ConstructL(
    CMPXResource& aResource)
    {
    MPX_FUNC("CMPXDbMusic::ConstructL");

    BaseConstructL();
    iExtensionsDrm = aResource.ReadDesCArrayL(R_MC_FILE_EXTENSIONS_DRM);
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::AddSongL
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbMusic::AddSongL(
    const CMPXMedia& aMedia,
    TInt aDrive,
    CMPXMessageArray* aMessageArray)
    {
    MPX_FUNC("CMPXDbMusic::AddSongL");

    TUint32 songId(MPXDbCommonUtil::GenerateUniqueIdL(iDbManager.Fs(), EMPXCollection,
        aMedia.ValueText(KMPXMediaGeneralUri), EFalse));

    if (SongExistsL(songId))
        {
        // Delete the existing record first and start from scratch
        DeleteSongL(songId, aDrive, ETrue);
        }
    // add the song
    DoAddSongL(songId, aMedia, aDrive, aMessageArray);

    return songId;
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::DoAddSongL
// ----------------------------------------------------------------------------
//
TBool CMPXDbMusic::DoAddSongL(
    TUint32 aSongId,
    const CMPXMedia& aMedia,
    TInt aDrive,
    CMPXMessageArray* aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbMusic::DoAddSongL");

    CDesCArrayFlat* fields = new (ELeave) CDesCArrayFlat(EMusicFieldCount);
    CleanupStack::PushL(fields);
    CDesCArrayFlat* values = new (ELeave) CDesCArrayFlat(EMusicFieldCount);
    CleanupStack::PushL(values);

    // add known fields
    MPXDbCommonUtil::AppendValueL(*fields, *values, KMCMusicUniqueId, aSongId);
    MPXDbCommonUtil::AppendValueL(*fields, *values, KMCMusicDel, 0);

    TTime time;
    time.HomeTime();
    HBufC* timeAdded = MPXDbCommonUtil::TTimeToDesLC(time);
    MPXDbCommonUtil::AppendValueL(*fields, *values, KMCMusicTimeAdded, *timeAdded);
    CleanupStack::PopAndDestroy(timeAdded);

#ifdef ABSTRACTAUDIOALBUM_INCLUDED
    //update ContainEmbeddedArt
    const TDesC& albumArtFilename = aMedia.ValueText(KMPXMediaMusicAlbumArtFileName).Left(KMCMaxTextLen);
    if (albumArtFilename.Length() > 0)
        {
        MPXDbCommonUtil::AppendValueL(*fields, *values, KMCMusicContainEmbeddedArt, 1);
        }
#endif // ABSTRACTAUDIOALBUM_INCLUDED
    // process the media parameter and construct the fields and values array
    TBool visible(GenerateMusicFieldsValuesL(aSongId, aMedia, aItemChangedMessages,
        NULL, *fields, *values, aDrive));

    // create the fields and values strings
    HBufC* fieldStr = MPXDbCommonUtil::StringFromArrayLC(*fields, KMCCommaSign);
    HBufC* valueStr = MPXDbCommonUtil::StringFromArrayLC(*values, KMCCommaSign);

    // execute the query
    iDbManager.ExecuteQueryL(aDrive, KQueryMusicInsert, fieldStr, valueStr);

    CleanupStack::PopAndDestroy(valueStr);
    CleanupStack::PopAndDestroy(fieldStr);
    CleanupStack::PopAndDestroy(values);
    CleanupStack::PopAndDestroy(fields);

    return visible;
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::UpdateSongL
// ----------------------------------------------------------------------------
//
CMPXDbActiveTask::TChangeVisibility CMPXDbMusic::UpdateSongL(
    TUint32 aSongId,
    const CMPXMedia& aMedia,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbMusic::UpdateSongL");
    return DoUpdateSongL(aSongId, aMedia, &aItemChangedMessages);
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::DoUpdateSongL
// ----------------------------------------------------------------------------
//
CMPXDbActiveTask::TChangeVisibility CMPXDbMusic::DoUpdateSongL(
    TUint32 aSongId,
    const CMPXMedia& aMedia,
    CMPXMessageArray* aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbMusic::DoUpdateSongL");

    CMPXDbActiveTask::TChangeVisibility visible(CMPXDbActiveTask::ENotVisibile);
    if (IsSupported(aMedia))
        {
        // retrieve the existing record
        TInt oldSongId(0);
        oldSongId = (aMedia.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId)).iId2;
        if ( oldSongId <= 0 )
            {
            oldSongId = aSongId;
            }
        RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryMusicGetSong, oldSongId));
        CleanupClosePushL(recordset);

        if (recordset.Next() != KSqlAtRow)
            {
            User::Leave(KErrNotFound);
            }

#ifdef ABSTRACTAUDIOALBUM_INCLUDED        
        iArtNeedUpdated = ETrue;    // reset flag
#endif
        TDriveUnit driveUnit(MPXDbCommonUtil::GetDriveIdMatchVolIdL(iDbManager.Fs(),
          recordset.ColumnInt64(EMusicVolumeId)));
        visible = DoUpdateSongL(aSongId, aMedia, driveUnit, aItemChangedMessages,
          recordset);
        
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
        if(iArtNeedUpdated)
            {
#endif // ABSTRACTAUDIOALBUM_INCLUDED
	        // Update Album table
            if (aMedia.IsSupported(KMPXMediaMusicAlbumArtFileName))
	            {
	            TUint32 albumId = recordset.ColumnInt64(EMusicAlbum);
	            iObserver.UpdateCategoryItemL(EMPXAlbum, albumId, aMedia, driveUnit, aItemChangedMessages);
	            }
            
	        // Update Artist table
	        if ( aMedia.IsSupported(KMPXMediaMusicAlbumArtFileName) )
	            {
	            TUint32 artistId = recordset.ColumnInt64(EMusicArtist);
	            iObserver.UpdateCategoryItemL(EMPXArtist, artistId, aMedia, driveUnit, aItemChangedMessages);
	            }
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
            }
#endif // ABSTRACTAUDIOALBUM_INCLUDED
        CleanupStack::PopAndDestroy(&recordset);
        }

    return visible;
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::DoUpdateSongL
// ----------------------------------------------------------------------------
//
CMPXDbActiveTask::TChangeVisibility CMPXDbMusic::DoUpdateSongL(
    TUint32 aSongId,
    const CMPXMedia& aMedia,
    TInt aDrive,
    CMPXMessageArray* aItemChangedMessages,
    RSqlStatement& aRecordset)
    {
    MPX_FUNC("CMPXDbMusic::DoUpdateSongL");

    CDesCArrayFlat* fields = new (ELeave) CDesCArrayFlat(EMusicFieldCount);
    CleanupStack::PushL(fields);
    CDesCArrayFlat* values = new (ELeave) CDesCArrayFlat(EMusicFieldCount);
    CleanupStack::PushL(values);

    TInt oldSongId(0);
    oldSongId = (aMedia.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId)).iId2;
    if ( oldSongId <= 0 )
        {
        oldSongId = aSongId;
        }
    // process the media parameter and construct the fields and values array
    CMPXDbActiveTask::TChangeVisibility visible(GenerateMusicFieldsValuesL(oldSongId, aMedia, aItemChangedMessages,
        &aRecordset, *fields, *values, aDrive));

    // construct the SET string
    HBufC* setStr = MPXDbCommonUtil::StringFromArraysLC(*fields, *values, KMCEqualSign, KMCCommaSign);
    if (setStr->Length())
        {
        // execute the query
        iDbManager.ExecuteQueryL(aDrive, KQueryMusicUpdate, setStr, oldSongId);
        }

    CleanupStack::PopAndDestroy(setStr);
    CleanupStack::PopAndDestroy(values);
    CleanupStack::PopAndDestroy(fields);

    return visible;
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::DeleteSongL
// ----------------------------------------------------------------------------
//
void CMPXDbMusic::DeleteSongL(
    TUint32 aSongId,
    TInt aDrive,
    TBool aDeleteRecord /* = EFalse */)
    {
    MPX_FUNC("CMPXDbMusic::DeleteSongL");

    TPtrC query(aDeleteRecord ? KQueryMusicDelete() : KQueryMusicDeleteUpdate());
    iDbManager.ExecuteQueryL(aDrive, query, aSongId);
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::DeleteCategoryL
// ----------------------------------------------------------------------------
//
void CMPXDbMusic::DeleteCategoryL(
    TMPXGeneralCategory aCategory,
    TUint32 aCategoryId,
    CDesCArray& /* aUriArray */,
    CMPXMessageArray& /* aItemChangedMessages */,
    TInt aDrive)
    {
    MPX_FUNC("CMPXDbMusic::DeleteCategoryL");
    TPtrC ptr(MPXDbUtil::MusicFieldNameForCategoryL(aCategory));
    iDbManager.ExecuteQueryL(aDrive, KQueryMusicDeleteForCategory, &ptr, aCategoryId);
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::CleanupL
// ----------------------------------------------------------------------------
//
void CMPXDbMusic::CleanupL()
    {
    MPX_FUNC("CMPXDbMusic::CleanupL");
    iDbManager.ExecuteQueryL(KDbManagerAllDrives, KQueryMusicCleanup);
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::GetNameL
// ----------------------------------------------------------------------------
//
HBufC* CMPXDbMusic::GetNameL(
    TUint32 aSongId)
    {
    MPX_FUNC("CMPXDbMusic::GetNameL");
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryMusicGetTitle, aSongId));
    CleanupClosePushL(recordset);

    if (recordset.Next() != KSqlAtRow)
        {
        User::LeaveIfError(KErrNotFound);
        }

    HBufC* title = MPXDbCommonUtil::GetColumnTextL(recordset, KMPXTableDefaultIndex).AllocL();
    CleanupStack::PopAndDestroy(&recordset);

    return title;
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::GetUriL
// ----------------------------------------------------------------------------
//
HBufC* CMPXDbMusic::GetUriL(
    TUint32 aSongId)
    {
    MPX_FUNC("CMPXDbMusic::GetUriL");

    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryMusicGetUri, aSongId));
    CleanupClosePushL(recordset);

    if (recordset.Next() != KSqlAtRow)
        {
        User::LeaveIfError(KErrNotFound);
        }

    // query fields
    enum
        {
        EMusicUriLocation = 0,
        EMusicUriVolId
        };

    TUint volId(recordset.ColumnInt64(EMusicUriVolId));
    HBufC* fullUri = MPXDbCommonUtil::CreateFullPathL(
        MPXDbCommonUtil::GetDriveIdMatchVolIdL(iDbManager.Fs(), volId),
        MPXDbCommonUtil::GetColumnTextL(recordset, EMusicUriLocation));

    CleanupStack::PopAndDestroy(&recordset);

    return fullUri;
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::GetDriveL
// ----------------------------------------------------------------------------
//
TInt CMPXDbMusic::GetDriveL(
    TUint32 aSongId)
    {
    MPX_FUNC("CMPXDbMusic::GetDriveL");
    return MPXDbCommonUtil::GetDriveIdMatchVolIdL(iDbManager.Fs(),
        ExecuteIntQueryL(KQueryMusicVolume, aSongId));
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::GetSongInfoL
// ----------------------------------------------------------------------------
//
HBufC* CMPXDbMusic::GetSongInfoL(
    TUint32 aSongId,
    TUint32& aArtistId,
    TUint32& aAlbumId,
    TUint32& aGenreId,
    TUint32& aComposerId,
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
    TUint32& aAbstractAlbumId,
#endif // ABSTRACTAUDIOALBUM_INCLUDED
    TInt& aDriveId,
    HBufC*& aArt)
    {
    MPX_FUNC("CMPXDbMusic::GetSongInfoL");

    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryMusicInfo, aSongId));
    CleanupClosePushL(recordset);

    if (recordset.Next() != KSqlAtRow)
        {
        User::Leave(KErrNotFound);
        }

    aDriveId = MPXDbCommonUtil::GetDriveIdMatchVolIdL(iDbManager.Fs(),
        recordset.ColumnInt64(EMusicVolumeId));
    aArtistId = recordset.ColumnInt64(EMusicArtist);
    aAlbumId = recordset.ColumnInt64(EMusicAlbum);
    aGenreId = recordset.ColumnInt64(EMusicGenre);
    aComposerId = recordset.ColumnInt64(EMusicComposer);
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
    aAbstractAlbumId = recordset.ColumnInt64(EMusicAbstractAlbum);
#endif // ABSTRACTAUDIOALBUM_INCLUDED
    aArt = MPXDbCommonUtil::GetColumnTextL(recordset, EMusicArt).AllocL();
    HBufC* uri = ConstructUriL(recordset, aSongId);

    CleanupStack::PopAndDestroy(&recordset);

    return uri;
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::GetSongL
// ----------------------------------------------------------------------------
//
TInt CMPXDbMusic::GetSongL(
    const CMPXMedia& aCriteria,
    CMPXMedia*& aMedia)
    {
    MPX_FUNC("CMPXDbMusic::GetSongL");

    TUint32 songId(0);

    // find song Id, title, URI, and general flags from its Id and/or URI. Do not use "aCriteria"
    // because it may contain attributes other than Id and URI. We don't want to search the
    // song by fields other than the ID and URI because other attributes for the song may have
    // been changed in the collection since the song was added
    CMPXMedia* criteria = CMPXMedia::NewL();
    CleanupStack::PushL(criteria);
    if (aCriteria.IsSupported(KMPXMediaGeneralId))
        {
        songId = (aCriteria.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId)).iId2;
        criteria->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, songId);
        }
    else if (aCriteria.IsSupported(KMPXMediaGeneralUri))
        {
        criteria->SetTextValueL(KMPXMediaGeneralUri,
            aCriteria.ValueText(KMPXMediaGeneralUri));
        }

    // get the criteria string
    HBufC* criteriaStr = GenerateMusicMatchingCriteriaLC(songId, 0, EMPXItem, *criteria);

    // execute the query
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryMusicSong, criteriaStr));
    CleanupClosePushL(recordset);

    TInt err(KErrNone);
    if ((err = recordset.Next()) == KSqlAtRow)
        {
        RArray<TMPXAttribute> attributes;
        CleanupClosePushL(attributes);
        attributes.AppendL(TMPXAttribute(KMPXMediaIdGeneral,
            EMPXMediaGeneralId | EMPXMediaGeneralTitle | EMPXMediaGeneralUri | EMPXMediaGeneralFlags));

        aMedia = CMPXMedia::NewL();
        CleanupStack::PushL(aMedia);

        UpdateMediaL(recordset, attributes.Array(), *aMedia);

        CleanupStack::Pop(aMedia);
        CleanupStack::PopAndDestroy(&attributes);

        err = KErrNone;
        }
    else
        {
        err = KErrNotFound;
        }

    CleanupStack::PopAndDestroy(&recordset);
    CleanupStack::PopAndDestroy(criteriaStr);
    CleanupStack::PopAndDestroy(criteria);

    return err;
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::GetRecentlyPlayedSongsL
// ----------------------------------------------------------------------------
//
void CMPXDbMusic::GetRecentlyPlayedSongsL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbMusic::GetRecentlyPlayedSongsL");
    ExecuteMediaQueryL(aAttrs, aMediaArray, KQueryMusicGetRecentlyPlayed,
        KMPXMaxRecentlyPlayedSongs);
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::GetMostPlayedSongsL
// ----------------------------------------------------------------------------
//
void CMPXDbMusic::GetMostPlayedSongsL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbMusic::GetMostPlayedSongsL");
    ExecuteMediaQueryL(aAttrs, aMediaArray, ExtraFieldsRequired(aAttrs) ?
        KQueryMusicGetMostPlayed() : KQueryMusicGetMostPlayedNoCategories(),
        KMPXMaxMostPlayedSongs);
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::GetRecentlyAddedSongsL
// ----------------------------------------------------------------------------
//
void CMPXDbMusic::GetRecentlyAddedSongsL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbMusic::GetRecentlyAddedSongsL");
    ExecuteMediaQueryL(aAttrs, aMediaArray, ExtraFieldsRequired(aAttrs) ?
        KQueryMusicGetRecentlyAdded() : KQueryMusicGetRecentlyAddedNoCategories());
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::CountL
// ----------------------------------------------------------------------------
//
TInt CMPXDbMusic::CountL()
    {
    MPX_FUNC("CMPXDbMusic::CountL");
    return ExecuteSumQueryL(KQueryMusicCount);
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::GetAlbumsForArtistL
// ----------------------------------------------------------------------------
//
void CMPXDbMusic::GetAlbumsForArtistL(
    TUint32 aArtistId,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbMusic::GetAlbumsForArtistL");

    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryMusicAlbum, aArtistId));
    CleanupClosePushL(recordset);

    TInt err(KErrNone);
    while ((err = recordset.Next()) == KSqlAtRow)
        {
        TUint32 albumId(recordset.ColumnInt64(KMPXTableDefaultIndex));

        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL(media);

        media->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, albumId);
        media->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
        media->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXAlbum);

        aMediaArray.AppendL(*media);
        CleanupStack::PopAndDestroy(media);
        }

    CleanupStack::PopAndDestroy(&recordset);
    if (err != KSqlAtEnd)
        {
        User::LeaveIfError(KErrCorrupt);
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::GetSongL
// ----------------------------------------------------------------------------
//
void CMPXDbMusic::GetSongL(
    TInt aSongId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbMusic::GetSongL");
    ExecuteMediaQueryL(aAttrs, aMedia, ExtraFieldsRequired(aAttrs) ?
        KQueryMusicGetSong() : KQueryMusicGetSongNoCategories(), aSongId);
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::GetAllSongsL
// ----------------------------------------------------------------------------
//
void CMPXDbMusic::GetAllSongsL(
    TInt aDrive,
    TInt aPlaylistId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbMusic::GetAllSongsL");
    ExecuteMediaQueryL(aDrive, aAttrs, aMediaArray, KQueryMusicGetAllSongsInfobyPl(), aPlaylistId);
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::GetAllSongsL
// ----------------------------------------------------------------------------
//
void CMPXDbMusic::GetAllSongsL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbMusic::GetAllSongsL");

    // do not execute the joins if no category name is required
    ExecuteMediaQueryL(aAttrs, aMediaArray, ExtraFieldsRequired(aAttrs) ?
        KQueryMusicGetAllSongs() : KQueryMusicGetAllSongsNoCategories());
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::GetAllSongsLimitedL
// ----------------------------------------------------------------------------
//
void CMPXDbMusic::GetAllSongsLimitedL(const TArray<TMPXAttribute>& aAttrs,
                                      CMPXMediaArray& aMediaArray, TInt aLimit)
    {
    MPX_FUNC("CMPXDbMusic::GetAllSongsLimitedL");

	// Reset the previous query results. This will cause creation of query result
	// cache during actual incremental open stage (i.e. GetSongsAtOffsetL).
	if (iAllSongsQueryResult.Count())
		{
        iAllSongsQueryResult.ResetAndDestroy();
		}

    // Gets a subset of the data from all songs ordered by title
    HBufC* query = HBufC::NewLC( KQueryMusicGetAllSongsMinimumLimited().Length() + KMCIntegerLen );
    query->Des().Format( KQueryMusicGetAllSongsMinimumLimited, aLimit );
    ExecuteMediaQueryL(aAttrs, aMediaArray, *query);
    CleanupStack::PopAndDestroy( query );
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::ExecuteQueryAllSongsL
// ----------------------------------------------------------------------------
//
void CMPXDbMusic::ExecuteQueryAllSongsL(const TArray<TMPXAttribute>& aAttrs)
    {
    // Run query and add result media objects to the cache array.
    MPX_FUNC("CMPXDbMusic::ExecuteQueryAllSongsL");

    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryMusicGetAllSongsMinimum()));
    CleanupClosePushL(recordset);

    TInt err(KErrNone);
    while ((err = recordset.Next()) == KSqlAtRow)
        {
        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL(media);
        UpdateMediaL(recordset, aAttrs, *media);
        iAllSongsQueryResult.AppendL(media);
        CleanupStack::Pop(media);
        }

    CleanupStack::PopAndDestroy(&recordset);
    if (err!= KSqlAtEnd)
        {
        User::Leave(KErrCorrupt);
        }
    }


// ----------------------------------------------------------------------------
// CMPXDbMusic::GetSongsInBlockL
// ----------------------------------------------------------------------------
//
void CMPXDbMusic::GetSongsInBlockL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray,
    TPtrC aTitle,
    TUint aNumOfSongs,
    TBool aAsc)
    {
    MPX_FUNC("CMPXDbMusic::GetSongsInBlockL");

    if (aAsc)
        { 
        ExecuteMediaQueryL(aAttrs, aMediaArray, KQueryMusicGetSongsInBlockAsc,
                              aTitle, aNumOfSongs, ETrue, EAscQuery );    
        }
    else
        {
        ExecuteMediaQueryL(aAttrs, aMediaArray, KQueryMusicGetSongsInBlockDsc,
                           aTitle, aNumOfSongs, EFalse, EDscQuery );        
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::GetSongsAtOffsetL
// ----------------------------------------------------------------------------
//
void CMPXDbMusic::GetSongsAtOffsetL( CMPXMediaArray& aMediaArray,
                                     const TArray<TMPXAttribute>& aAttrs,
                                     TInt aOffset,
                                     TInt aCount )
    {
	MPX_DEBUG3("CMPXDbMusic::GetSongsAtOffsetL offset[%d], count[%d]", aOffset, aCount);

	if ( !iAllSongsQueryResult.Count() )
		{
		// If there's no cache, create a cache for the query results.
		ExecuteQueryAllSongsL(aAttrs);
		}

	TInt limit = aOffset + aCount > iAllSongsQueryResult.Count() ?
		iAllSongsQueryResult.Count() : aOffset + aCount;
	for ( TInt i = aOffset; i < limit; i++ )
		{
        CMPXMedia* m = iAllSongsQueryResult[i];
		aMediaArray.AppendL(*m);
		}

    MPX_DEBUG1("CMPXDbMusic::GetSongsAtOffsetL() -->");
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::GetSongsForArtistL
// ----------------------------------------------------------------------------
//
void CMPXDbMusic::GetSongsForArtistL(
    TUint aArtistId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbMusic::GetSongsForArtistL");
    ExecuteMediaQueryL(aAttrs, aMediaArray, ExtraFieldsRequired(aAttrs) ?
        KQueryMusicGetSongsForArtist() : KQueryMusicGetSongsForArtistNoCategories(),
        aArtistId);
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::GetSongsForAlbumL
// ----------------------------------------------------------------------------
//
void CMPXDbMusic::GetSongsForAlbumL(
    TUint aAlbumId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbMusic::GetSongsForAlbumL");

    TPtrC query;
    if (iObserver.IsUnknownAlbumL(aAlbumId))
        {
        query.Set(ExtraFieldsRequired(aAttrs) ? KQueryMusicGetSongsForUnknownAlbum() :
            KQueryMusicGetSongsForUnknownAlbumNoCategories());
        }
    else
        {
        query.Set(ExtraFieldsRequired(aAttrs) ? KQueryMusicGetSongsForAlbum() :
            KQueryMusicGetSongsForAlbumNoCategories());
        }

    ExecuteMediaQueryL(aAttrs, aMediaArray, query, aAlbumId);
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::GetSongsForArtistAndAlbumL
// ----------------------------------------------------------------------------
//
void CMPXDbMusic::GetSongsForArtistAndAlbumL(
    TUint aArtistId,
    TUint aAlbumId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbMusic::GetSongsForArtistAndAlbumL");
    ExecuteMediaQueryL(aAttrs, aMediaArray, ExtraFieldsRequired(aAttrs) ?
        KQueryMusicGetSongsForArtistAlbum() : KQueryMusicGetSongsForArtistAlbumNoCategories(),
        aArtistId, aAlbumId);
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::GetSongsForGenreL
// ----------------------------------------------------------------------------
//
void CMPXDbMusic::GetSongsForGenreL(
    TUint aGenreId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbMusic::GetSongsForGenreL");
    ExecuteMediaQueryL(aAttrs, aMediaArray, ExtraFieldsRequired(aAttrs) ?
        KQueryMusicGetSongsForGenre() : KQueryMusicGetSongsForGenreNoCategories(),
        aGenreId);
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::GetSongsForComposerL
// ----------------------------------------------------------------------------
//
void CMPXDbMusic::GetSongsForComposerL(
    TUint aComposerId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbMusic::GetSongsForComposerL");
    ExecuteMediaQueryL(aAttrs, aMediaArray, ExtraFieldsRequired(aAttrs) ?
        KQueryMusicGetSongsForComposer() : KQueryMusicGetSongsForComposerNoCategories(),
        aComposerId);
    }

#ifdef ABSTRACTAUDIOALBUM_INCLUDED
// ----------------------------------------------------------------------------
// CMPXDbMusic::GetAllSongsForAbstractAlbumL
// ----------------------------------------------------------------------------
//
void CMPXDbMusic::GetAllSongsForAbstractAlbumL(
    TUint aAbstractAlbumId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbMusic::GetAllSongsForAbstractAlbumL");
    ExecuteMediaQueryL(aAttrs, aMediaArray, KQueryMusicGetSongsForAbstractAlbum(), aAbstractAlbumId);
    }
#endif // ABSTRACTAUDIOALBUM_INCLUDED

// ----------------------------------------------------------------------------
// CMPXDbMusic::AllSongsDurationL
// ----------------------------------------------------------------------------
//
TInt CMPXDbMusic::AllSongsDurationL()
    {
    MPX_FUNC("CMPXDbMusic::AllSongsDurationL");
    return ExecuteSumQueryL(KQueryMusicDurationAll);
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::ArtistDurationL
// ----------------------------------------------------------------------------
//
TInt CMPXDbMusic::ArtistDurationL(
    TUint aArtistId)
    {
    MPX_FUNC("CMPXDbMusic::ArtistDurationL");
    return ExecuteSumQueryL(KQueryMusicDurationArtist, aArtistId);
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::AlbumDurationL
// ----------------------------------------------------------------------------
//
TInt CMPXDbMusic::AlbumDurationL(
    TUint aAlbumId)
    {
    MPX_FUNC("CMPXDbMusic::AlbumDurationL");
    return ExecuteSumQueryL(KQueryMusicDurationAlbum, aAlbumId);
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::ArtistAlbumDurationL
// ----------------------------------------------------------------------------
//
TInt CMPXDbMusic::ArtistAlbumDurationL(
    TUint aArtistId,
    TUint aAlbumId)
    {
    MPX_FUNC("CMPXDbMusic::ArtistAlbumDurationL");
    return ExecuteSumQueryL(KQueryMusicDurationArtistAlbum, aArtistId, aAlbumId);
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::GenreDurationL
// ----------------------------------------------------------------------------
//
TInt CMPXDbMusic::GenreDurationL(
    TUint aGenreId)
    {
    MPX_FUNC("CMPXDbMusic::GenreDurationL");
    return ExecuteSumQueryL(KQueryMusicDurationGenre, aGenreId);
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::ComposerDurationL
// ----------------------------------------------------------------------------
//
TInt CMPXDbMusic::ComposerDurationL(
    TUint aComposerId)
    {
    MPX_FUNC("CMPXDbMusic::ComposerDurationL");
    return ExecuteSumQueryL(KQueryMusicDurationComposer, aComposerId);
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::RecentlyPlayedDurationL
// ----------------------------------------------------------------------------
//
TInt CMPXDbMusic::RecentlyPlayedDurationL()
    {
    MPX_FUNC("CMPXDbMusic::RecentlyPlayedDurationL");
    return ExecuteSumQueryL(KQueryMusicDurationRecentlyPlayed, KMPXMaxRecentlyPlayedSongs);
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::MostPlayedDurationL
// ----------------------------------------------------------------------------
//
TInt CMPXDbMusic::MostPlayedDurationL()
    {
    MPX_FUNC("CMPXDbMusic::MostPlayedDurationL");
    return ExecuteSumQueryL(KQueryMusicDurationMostPlayed, KMPXMaxMostPlayedSongs);
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::RecentlyAddedDurationL
// ----------------------------------------------------------------------------
//
TInt CMPXDbMusic::RecentlyAddedDurationL()
    {
    MPX_FUNC("CMPXDbMusic::RecentlyAddedDurationL");
    return ExecuteSumQueryL(KQueryMusicDurationRecentlyAdded);
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::FindSongsL
// ----------------------------------------------------------------------------
//
void CMPXDbMusic::FindSongsL(
    TUint32 aGeneralId,
    TUint32 aContainerId,
    TMPXGeneralType aType,
    const CMPXMedia& aCriteria,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray,
    TBool aSortByTrackOrder   
    )
    {
    MPX_FUNC("CMPXDbMusic::FindSongsL");

    // get the selection criteria string
    HBufC* criteriaStr = GenerateMusicMatchingCriteriaLC(aGeneralId, aContainerId, aType,
        aCriteria);

    TPtrC sortOrder;  
    if (aSortByTrackOrder)
        {
        sortOrder.Set(KQueryMusicFindAllSortOrderTrack);
        }
    else
        {
        sortOrder.Set(KQueryMusicFindAllSortOrderTitle);
        }
        
    // construct the query
    HBufC* query = HBufC::NewLC(KQueryMusicFindAll().Length() + criteriaStr->Length() +
        sortOrder.Length());
    query->Des().Format(KQueryMusicFindAll, criteriaStr, &sortOrder);

    // iterate the results and append media objects to the destination array
    ExecuteMediaQueryL(aAttrs, aMediaArray, *query);

    CleanupStack::PopAndDestroy(query);
    CleanupStack::PopAndDestroy(criteriaStr);
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::GetDriveTrackCount
// ----------------------------------------------------------------------------
//
TUint CMPXDbMusic::GetDriveTrackCountL(TInt aDrive)
    {
    TUint count(0);

    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(aDrive,KQueryMusicCount));
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
// CMPXDbMusic::GetMusicUriArrayL
// ----------------------------------------------------------------------------
//
void CMPXDbMusic::GetMusicUriArrayL(TInt aDrive, TInt aFromID, TInt aRecords,
                                    CDesCArray& aUriArr, TInt& aLastID)
    {
    MPX_FUNC("CMPXDbMusic::GetMusicUriArrayL");

    HBufC* query = NULL;
    if(aFromID == 0)
        {
        query = HBufC::NewLC(KQueryMusicGetMusicUris().Length() + KMCIntegerLen);
        query->Des().Format(KQueryMusicGetMusicUris, aRecords);
        }
    else
        {
        query = HBufC::NewLC(KQueryMusicGetMusicUrisFrom().Length() + 2*KMCIntegerLen);
        query->Des().Format(KQueryMusicGetMusicUrisFrom, aFromID, aRecords);
        }

    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(aDrive,*query));

    CleanupStack::PopAndDestroy(query);

    CleanupClosePushL(recordset);

    TInt lastID = 0;
    TInt err(KErrNone);
    while((err = recordset.Next()) == KSqlAtRow)
        {
        HBufC* fullPath = MPXDbCommonUtil::CreateFullPathL(aDrive,
                MPXDbCommonUtil::GetColumnTextL(recordset, KColUri));
        CleanupStack::PushL(fullPath);
        aUriArr.AppendL(*fullPath);
        CleanupStack::PopAndDestroy(fullPath);

        lastID = recordset.ColumnInt(KColUniqueID);
        }
    CleanupStack::PopAndDestroy(&recordset);

    aLastID = lastID;

    if (err!= KSqlAtEnd)
        {
        User::Leave(KErrCorrupt);
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::AlbumartForAlbumL
// ----------------------------------------------------------------------------
//
HBufC* CMPXDbMusic::AlbumartForAlbumL(const TUint32 aAlbumId, TPtrC aArt)
    {
	MPX_FUNC("CMPXDbMusic::AlbumartForAlbumL");
	
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryMusicGetAlbumartForAlbum, aAlbumId));
    HBufC* albumart(NULL);
   
    CleanupClosePushL(recordset);
    TInt err(KErrNone);
    while ((err = recordset.Next()) == KSqlAtRow)
        {     
        TPtrC art(MPXDbCommonUtil::GetColumnTextL(recordset, KMPXTableDefaultIndex));		
        if (art.Length()>0 && art.Compare(KNullDesC)!=0 &&  art.CompareF(aArt)!=0 )
            {
            albumart = art.AllocL(); 
            break;
            }	
        }
       
    CleanupStack::PopAndDestroy(&recordset);
    return albumart;
    }
// ----------------------------------------------------------------------------
// CMPXDbMusic::SongExistsL
// ----------------------------------------------------------------------------
//
TBool CMPXDbMusic::SongExistsL(
    TUint32 aSongId)
    {
    MPX_FUNC("CMPXDbMusic::SongExistsL");

    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryMusicVolumeAll, aSongId));
    TBool found = (recordset.Next() == KSqlAtRow);
    recordset.Close();

    return found;
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::UpdateMediaL
// ----------------------------------------------------------------------------
//
void CMPXDbMusic::UpdateMediaL(
    RSqlStatement& aMusicTable,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbMusic::UpdateMediaL");

    TInt count(aAttrs.Count());
    for (TInt i = 0; i < count; ++i)
        {
        switch (aAttrs[i].ContentId())
            {
            case KMPXMediaIdGeneral:
                {
                UpdateMediaGeneralL(aMusicTable, aAttrs[i].AttributeId(), aMedia);
                break;
                }
            case KMPXMediaIdMusic:
                {
                UpdateMediaMusicL(aMusicTable, aAttrs[i].AttributeId(), aMedia);
                break;
                }
            case KMPXMediaIdDrm:
                {
                // DRM is set by drm helper
                break;
                }
            case KMPXMediaIdMTP:
                {
                // Only attribute stored in db is MTP drm status
                UpdateMediaMTPL(aMusicTable, aAttrs[i].AttributeId(), aMedia);
                break;
                }
            case KMPXMediaIdAudio:
                {
                UpdateMediaAudioL(aMusicTable, aAttrs[i].AttributeId(), aMedia);
                break;
                }
            default:
                // Do not leave. If this plugin doesn't support
                // the content id they want, just return what we have
                break;
            } // end switch
        } // end for
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::UpdateMediaGeneralL
// ----------------------------------------------------------------------------
//
void CMPXDbMusic::UpdateMediaGeneralL(
    RSqlStatement& aMusicTable,
    const TUint aAttrId,
    CMPXMedia& aMedia)
    {
    MPX_DEBUG1("-->CMPXDbMusic::UpdateMediaGeneralL");
    MPX_DEBUG2("    aAttrId=%b", aAttrId);

    aMedia.SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem );
    aMedia.SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXSong );

    // FIX ME, temporary always fetch item ID
    //if (aAttrId & EMPXMediaGeneralId)
    if (!aMedia.IsSupported(KMPXMediaGeneralId))
        {
        MPX_DEBUG1("    !aMedia.IsSupported(KMPXMediaGeneralId)");
        TUint32 songId(aMusicTable.ColumnInt64(EMusicUniqueId));
        TInt columnCount(aMusicTable.ColumnCount());
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
        if(columnCount == 40 && aMusicTable.ColumnIndex(_L("PlUId"))==38)
            {
            TUint32 pListUId(aMusicTable.ColumnInt64(38));
            aMedia.SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, TMPXItemId(pListUId, songId));
            }
#else
        if(columnCount == 37 && aMusicTable.ColumnIndex(_L("PlUId"))==35)
            {
            TUint32 pListUId(aMusicTable.ColumnInt64(35));
            aMedia.SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, TMPXItemId(pListUId, songId));
            }
#endif
        else
            {
            aMedia.SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, songId);
            }
        MPX_DEBUG2("    SongId[%d]", songId);
        }
    // FIX ME temporary always fetch URI
    if (aAttrId & EMPXMediaGeneralUri)
        {
        MPX_DEBUG1("    !aMedia.IsSupported(KMPXMediaGeneralUri)");
        HBufC* uri = ConstructUriL(aMusicTable, (aMedia.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId)).iId2);
        CleanupStack::PushL(uri);
        aMedia.SetTextValueL(KMPXMediaGeneralUri, *uri);
        MPX_DEBUG2("    FullPath[%S]", uri);
        CleanupStack::PopAndDestroy(uri);
        }
    if (aAttrId & EMPXMediaGeneralDrive)
        {
        MPX_DEBUG1("    EMPXMediaGeneralDrive");
        TDriveUnit driveUnit;
        if (aMedia.IsSupported(KMPXMediaGeneralUri))
            {
            MPX_DEBUG1("        aMedia.IsSupported(KMPXMediaGeneralUri)");
            driveUnit = aMedia.ValueText(KMPXMediaGeneralUri);
            MPX_DEBUG2("        driveUnit=%d", (TInt)driveUnit);
            }
        else
            {
            MPX_DEBUG1("        !aMedia.IsSupported(KMPXMediaGeneralUri)");
            driveUnit = MPXDbCommonUtil::GetDriveIdMatchVolIdL(iDbManager.Fs(),
                aMusicTable.ColumnInt64(EMusicVolumeId));
            MPX_DEBUG2("        driveUnit=%d", (TInt)driveUnit);
            }

        TPtrC driveName(driveUnit.Name());
        aMedia.SetTextValueL(KMPXMediaGeneralDrive, driveName);
        MPX_DEBUG2("    Drive[%S]", &driveName);
        }
    if (aAttrId & EMPXMediaGeneralSize)
        {
        MPX_DEBUG1("    EMPXMediaGeneralSize");
        // to-do: store this in the DB
        }
    if (aAttrId & EMPXMediaGeneralDuration)
        {
        MPX_DEBUG1("    EMPXMediaGeneralDuration");
        TInt32 duration(aMusicTable.ColumnInt(EMusicDuration));
        aMedia.SetTObjectValueL<TInt>(KMPXMediaGeneralDuration, duration);
        MPX_DEBUG2("    Duration[%d]", duration);
        }
    if ((aAttrId & EMPXMediaGeneralTitle) && !aMedia.IsSupported(KMPXMediaGeneralTitle))
        {
        MPX_DEBUG1("    EMPXMediaGeneralTitle");
        TPtrC title(MPXDbCommonUtil::GetColumnTextL(aMusicTable, EMusicTitle));
        aMedia.SetTextValueL(KMPXMediaGeneralTitle, title);
        MPX_DEBUG2("    Title[%S]", &title);
        }
    if ( aAttrId & EMPXMediaGeneralDate)
        {
        MPX_DEBUG1("    EMPXMediaGeneralDate");
        const TDesC& dateStr(MPXDbCommonUtil::GetColumnTextL (aMusicTable,
                EMusicTimeAdded));
        if ( dateStr.Compare (KNullDesC)!= 0)
            {
            TTime dateTime(MPXDbCommonUtil::DesToTTimeL (dateStr));
            aMedia.SetTObjectValueL<TInt64> (KMPXMediaGeneralDate,
                    dateTime.Int64 ());
            }
        MPX_DEBUG2("    Date[%S]", &dateStr);
        }
    if (aAttrId & EMPXMediaGeneralComment)
        {
        MPX_DEBUG1("    EMPXMediaGeneralComment");
        TPtrC comment(MPXDbCommonUtil::GetColumnTextL(aMusicTable, EMusicComment));
        aMedia.SetTextValueL(KMPXMediaGeneralComment, comment);
        MPX_DEBUG2("    Comment[%S]", &comment);
        }
    if (aAttrId & EMPXMediaGeneralMimeType)
        {
        MPX_DEBUG1("    EMPXMediaGeneralMimeType");
        TPtrC mimeType(MPXDbCommonUtil::GetColumnTextL(aMusicTable, EMusicMimeType));
        aMedia.SetTextValueL(KMPXMediaGeneralMimeType, mimeType);
        MPX_DEBUG2("    MimeType[%S]", &mimeType);
        }
    if (aAttrId & EMPXMediaGeneralSynchronized)
        {
        MPX_DEBUG1("    EMPXMediaGeneralSynchronized");
        TInt sync(aMusicTable.ColumnInt(EMusicSync));
        aMedia.SetTObjectValueL<TBool>(KMPXMediaGeneralSynchronized, sync);
        MPX_DEBUG2("    Synchronized[%d]", sync);
        }
    if (aAttrId & EMPXMediaGeneralDeleted)
        {
        MPX_DEBUG1("    EMPXMediaGeneralDeleted");
        TInt del(aMusicTable.ColumnInt(EMusicDeleted));
        aMedia.SetTObjectValueL<TBool>(KMPXMediaGeneralDeleted, del);
        MPX_DEBUG2("    Deleted[%d]", del);
        }
    if (aAttrId & EMPXMediaGeneralModified)
        {
        MPX_DEBUG1("    EMPXMediaGeneralModified");
        TInt mod(aMusicTable.ColumnInt(EMusicModified));
        aMedia.SetTObjectValueL<TBool>(KMPXMediaGeneralModified, mod);
        MPX_DEBUG2("    Modified[%d]", mod);
        }
    if (aAttrId & EMPXMediaGeneralCount)
        {
        MPX_DEBUG1("    EMPXMediaGeneralCount");
        aMedia.SetTObjectValueL<TInt>(KMPXMediaGeneralCount, 1);
        }
    if (aAttrId & EMPXMediaGeneralCollectionId)
        {
        MPX_DEBUG1("    EMPXMediaGeneralCollectionId");
        aMedia.SetTObjectValueL<TUid>(KMPXMediaGeneralCollectionId,
            TUid::Uid(KDBPluginUid));
        }
    if(aAttrId & EMPXMediaGeneralCopyright)
        {
        MPX_DEBUG1("    EMPXMediaGeneralCopyright");
        TPtrC copyright(MPXDbCommonUtil::GetColumnTextL(aMusicTable, EMusicCopyright));
        aMedia.SetTextValueL(KMPXMediaGeneralCopyright, copyright);
        MPX_DEBUG2("    Copyright[%S]", &copyright);
        }
    if (aAttrId & EMPXMediaGeneralFlags)
        {
        MPX_DEBUG1("    EMPXMediaGeneralFlags");
        TUint32 dbFlags(aMusicTable.ColumnInt64(EMusicDbFlag));
        MPX_DEBUG2("        dbFlags=%b", dbFlags);
        TDriveUnit driveUnit;
        if (aMedia.IsSupported(KMPXMediaGeneralUri))
            {
            MPX_DEBUG1("        aMedia.IsSupported(KMPXMediaGeneralUri)");
            TParsePtrC parse( aMedia.ValueText(KMPXMediaGeneralUri) );
            if( parse.DrivePresent() )
                {
                driveUnit = parse.Drive();
                }
            else
                {
                driveUnit = MPXDbCommonUtil::GetDriveIdMatchVolIdL(iDbManager.Fs(),
                     aMusicTable.ColumnInt64(EMusicVolumeId));
                }
            MPX_DEBUG2("        driveUnit=%d", (TInt)driveUnit);
            }
        else
            {
            MPX_DEBUG1("        !aMedia.IsSupported(KMPXMediaGeneralUri)");
            driveUnit = MPXDbCommonUtil::GetDriveIdMatchVolIdL(iDbManager.Fs(),
                aMusicTable.ColumnInt64(EMusicVolumeId));
            MPX_DEBUG2("        driveUnit=%d", (TInt)driveUnit);
            }

        TInt driveId = driveUnit & KMPXMediaGeneralFlagsDriveInfo;  // 5 bits
        aMedia.SetTObjectValueL<TUint>(KMPXMediaGeneralFlags, dbFlags | driveId);

        MPX_DEBUG2("    GeneralFlags[%b]", dbFlags | driveId);
        MPX_DEBUG2("    DriveId[%u]", driveId);
        }
    if (aAttrId & EMPXMediaGeneralPlayCount)
        {
        MPX_DEBUG1("    EMPXMediaGeneralPlayCount");
        TUint32 playcount(aMusicTable.ColumnInt(EMusicPlayCount));
        aMedia.SetTObjectValueL<TInt>(KMPXMediaGeneralPlayCount, playcount);
        MPX_DEBUG2("    PlayCount[%d]", playcount);
        }

    MPX_DEBUG1("<--CMPXDbMusic::UpdateMediaGeneralL");
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::UpdateMediaMusicL
// ----------------------------------------------------------------------------
//
void CMPXDbMusic::UpdateMediaMusicL(
    RSqlStatement& aMusicTable,
    const TUint aAttrId,
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbMusic::UpdateMediaMusicL");

    if (aAttrId & EMPXMediaMusicAlbumArtFileName)
        {
        TPtrC art(MPXDbCommonUtil::GetColumnTextL(aMusicTable, EMusicArt));
            aMedia.SetTextValueL(KMPXMediaMusicAlbumArtFileName, art);
            MPX_DEBUG2("    Album Art File Name[%S]", &art);
        }
    if (aAttrId & EMPXMediaMusicArtist)
        {
        TPtrC artist(MPXDbCommonUtil::GetColumnTextL(aMusicTable, EMusicArtistName));
        aMedia.SetTextValueL(KMPXMediaMusicArtist, artist);
        MPX_DEBUG2("    Artist[%S]", &artist);
        }
    if (aAttrId & EMPXMediaMusicAlbum)
        {
        TPtrC album(MPXDbCommonUtil::GetColumnTextL(aMusicTable, EMusicAlbumName));
        aMedia.SetTextValueL(KMPXMediaMusicAlbum, album);
        MPX_DEBUG2("    Album[%S]", &album);
        }
    if ( aAttrId & EMPXMediaMusicYear)
        {
        const TDesC& dateStr(MPXDbCommonUtil::GetColumnTextL (aMusicTable,
                EMusicReleaseDate));
        if ( dateStr.Compare (KNullDesC)!= 0)
            {
            TTime dateTime(MPXDbCommonUtil::DesToTTimeL (dateStr));
            aMedia.SetTObjectValueL<TInt64> (KMPXMediaMusicYear,
                    dateTime.Int64 ());
            MPX_DEBUG2("    Year[%d]", dateTime.Int64());
            }
        MPX_DEBUG2("    ReleaseDate[%S]", &dateStr);
        }
    if (aAttrId & EMPXMediaMusicAlbumTrack)
        {
        TInt32 track(aMusicTable.ColumnInt(EMusicAlbumTrack));
        HBufC* hbuf = HBufC::NewLC(KMCIntegerLen);
        if (track != KMaxTInt)
            {
            hbuf->Des().AppendFormat(_L("%d"), track);
            }
        aMedia.SetTextValueL(KMPXMediaMusicAlbumTrack, *hbuf);
        MPX_DEBUG3("    Album Track[%S][%d]", hbuf, track);
        CleanupStack::PopAndDestroy(hbuf);
        }
    if (aAttrId & EMPXMediaMusicGenre)
        {
        TPtrC genre(MPXDbCommonUtil::GetColumnTextL(aMusicTable, EMusicGenreName));
        aMedia.SetTextValueL(KMPXMediaMusicGenre, genre);
        MPX_DEBUG2("    Music Genre[%S]", &genre);
        }
    if (aAttrId & EMPXMediaMusicComposer)
        {
        TPtrC composer(MPXDbCommonUtil::GetColumnTextL(aMusicTable, EMusicComposerName));
        aMedia.SetTextValueL(KMPXMediaMusicComposer, composer);
        MPX_DEBUG2("    Music Composer[%S]", &composer);
        }
    if (aAttrId & EMPXMediaMusicRating)
        {
        TUint32 rating(aMusicTable.ColumnInt(EMusicRating));
        aMedia.SetTObjectValueL<TUint32>(KMPXMediaMusicRating, rating);
        MPX_DEBUG2("    Music Rating[%d]", rating);
        }
    if (aAttrId & EMPXMediaMusicURL)
        {
        TPtrC url(MPXDbCommonUtil::GetColumnTextL(aMusicTable, EMusicUrl));
        aMedia.SetTextValueL(KMPXMediaMusicURL, url);
        MPX_DEBUG2("    Music URL[%S]", &url);
        }
    if (aAttrId & EMPXMediaMusicOriginalAlbumArtFileName)
        {
        // Always set original album art to be file path
        // Maybe add a new column to db for future if services like rhapsody pushes jpgs to us
        if (aMedia.IsSupported(KMPXMediaGeneralUri))
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
            {    
            TUint32 abstractAlbumId(aMusicTable.ColumnInt64(EMusicAbstractAlbum));
            TInt containEmbeddedArt = aMusicTable.ColumnInt( EMusicContainEmbeddedArt);
            //embedded with art, no art
            if (containEmbeddedArt || (!containEmbeddedArt && !abstractAlbumId)) //embedded with art case, no art
                {
                const TDesC& art(aMedia.ValueText(KMPXMediaGeneralUri));
                aMedia.SetTextValueL(KMPXMediaMusicOriginalAlbumArtFileName, art);
                MPX_DEBUG2("    Music Original Album Art FullPath[%S]", &art);
                }
            //for abstractalbum case, originalAlbumArt from AbstractAlbum table
            else if ( abstractAlbumId )
                {
                HBufC* art = iObserver.HandleGetAlbumNameFromIdL(abstractAlbumId);
                CleanupStack::PushL(art);
                aMedia.SetTextValueL(KMPXMediaMusicOriginalAlbumArtFileName, *art);
                MPX_DEBUG2("    Music Original Album Art FullPath[%S]", art);
                CleanupStack::PopAndDestroy(art);
                }
            }
#else
            {
            const TDesC& uri(aMedia.ValueText(KMPXMediaGeneralUri));
            aMedia.SetTextValueL(KMPXMediaMusicOriginalAlbumArtFileName, uri);
            MPX_DEBUG2("    Music Original Album Art FullPath[%S]", &uri);
            }
#endif // ABSTRACTAUDIOALBUM_INCLUDED
       else
           {
           HBufC* fullPath = ConstructUriL(aMusicTable, (aMedia.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId)).iId2);
           CleanupStack::PushL(fullPath);
           aMedia.SetTextValueL(KMPXMediaMusicOriginalAlbumArtFileName, *fullPath);
           MPX_DEBUG2("    Music Original Album Art FullPath[%S]", fullPath);
           CleanupStack::PopAndDestroy(fullPath);
           }
      }
#ifdef ABSTRACTAUDIOALBUM_INCLUDED    
    if (aAttrId & EMPXMediaMusicAlbumArtist)
        {
        TPtrC albumartist(MPXDbCommonUtil::GetColumnTextL(aMusicTable, EMusicAlbumArtist));
        aMedia.SetTextValueL(KMPXMediaMusicAlbumArtist, albumartist);
        MPX_DEBUG2("    Music AlbumArtist[%S]", &albumartist);
        }
#endif // ABSTRACTAUDIOALBUM_INCLUDED
    if (aAttrId & EMPXMediaMusicAlbumId)
        {
        TUint32 albumId(aMusicTable.ColumnInt64(EMusicAlbum));
        aMedia.SetTObjectValueL<TMPXItemId>(KMPXMediaMusicAlbumId, albumId);
        MPX_DEBUG2("    AlbumId[%d]", albumId);
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::UpdateMediaAudioL
// ----------------------------------------------------------------------------
//
void CMPXDbMusic::UpdateMediaAudioL(
    RSqlStatement& aMusicTable,
    const TUint aAttrId,
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbMusic::UpdateMediaAudioL");

    if (aAttrId & EMPXMediaAudioBitrate)
        {
        TUint32 bitrate(aMusicTable.ColumnInt(EMusicBitRate));
        aMedia.SetTObjectValueL<TUint32>(KMPXMediaAudioBitrate, bitrate);
        MPX_DEBUG2("    Bitrate[%d]", bitrate);
        }
    if (aAttrId & EMPXMediaAudioSamplerate)
        {
        TUint32 samplerate(aMusicTable.ColumnInt(EMusicSampleRate));
        aMedia.SetTObjectValueL<TUint32>(KMPXMediaAudioSamplerate, samplerate);
        MPX_DEBUG2("    SampleRate[%d]", samplerate);
        }
    if (aAttrId & EMPXMediaAudioNumberOfChannels)
        {
        TUint32 numchannels(aMusicTable.ColumnInt(EMusicNumChannels));
        aMedia.SetTObjectValueL<TUint32>(KMPXMediaAudioNumberOfChannels, numchannels);
        MPX_DEBUG2("    Num of Channels[%d]", numchannels);
        }
    if (aAttrId & EMPXMediaAudioCodec)
        {
        TUint32 codec(aMusicTable.ColumnInt(EMusicCodec));
        aMedia.SetTObjectValueL<TUint32>(KMPXMediaAudioAudioCodec, codec);
        MPX_DEBUG2("    Audio Codec[%d]", codec);
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::UpdateMediaMTPL
// ----------------------------------------------------------------------------
//
void CMPXDbMusic::UpdateMediaMTPL(
    RSqlStatement& aMusicTable,
    const TUint aAttrId,
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbMusic::UpdateMediaMTPL");

    if (aAttrId & KMPXMediaMTPDrmStatus.iAttributeId)
        {
        TUint32 val(aMusicTable.ColumnInt(EMusicMTPDrmStatus));
        aMedia.SetTObjectValueL<TUint16>(KMPXMediaMTPDrmStatus, val);
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::GenerateMusicFieldsValuesL
// ----------------------------------------------------------------------------
//
CMPXDbActiveTask::TChangeVisibility CMPXDbMusic::GenerateMusicFieldsValuesL(
    TUint32 aSongId,
    const CMPXMedia& aMedia,
    CMPXMessageArray* aItemChangedMessages,
    RSqlStatement* aMusicTable,
    CDesCArray& aFields,
    CDesCArray& aValues,
    TInt aDrive)
    {
    MPX_FUNC("CMPXDbMusic::GenerateMusicFieldsValuesL");

    CMPXDbActiveTask::TChangeVisibility visibleChange(CMPXDbActiveTask::ENotVisibile);
    TBool metaDataModified(EFalse);
    const TArray<TMPXAttribute> attributes = aMedia.Attributes();

    TBool addSongChangedMessage(ETrue);
    CMPXMessage* songChangedMessage(NULL);
    if (aItemChangedMessages)
        {
        songChangedMessage = CMPXMedia::NewL();
        CleanupStack::PushL(songChangedMessage);
        MPXDbCommonUtil::FillItemChangedMessageL(*songChangedMessage, aSongId,
            aMusicTable ? EMPXItemModified : EMPXItemInserted, EMPXSong, KDBPluginUid);
        }

    // NOTE: Attributes being processed here should be listed in IsSupported()
    TInt attrCount(attributes.Count());
    for (TInt i = 0; i < attrCount; ++i)
        {
        TUint attributeId(attributes[i].AttributeId());

        switch (attributes[i].ContentId())
            {
            case KMPXMediaIdGeneral:
                {
                if (attributeId & EMPXMediaGeneralDeleted)
                    {
                    TBool deleted(aMedia.ValueTObjectL<TBool>(KMPXMediaGeneralDeleted));
                    if (!aMusicTable || (deleted != aMusicTable->ColumnInt(EMusicDeleted)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicDel, deleted);
                        }
                    MPX_DEBUG2("    Deleted[%d]", deleted);
                    }

                if (attributeId & EMPXMediaGeneralFlags)
                    {
                    TUint flag(aMedia.ValueTObjectL<TUint>(KMPXMediaGeneralFlags));
                    TUint32 curFlag(0);
                    if (aMusicTable)
                        {
                        curFlag = aMusicTable->ColumnInt64(EMusicDbFlag);
                        }
                    TUint32 oldFlag(curFlag);

                    if (flag & KMPXMediaGeneralFlagsSetOrUnsetBit)
                        {
                        // Set bits
                        curFlag |= flag;
                        }
                    else
                        {
                        // Clear bits
                        curFlag &= (~flag);
                        }

                    // The field is written ONLY if the flag value is changing
                    if (((curFlag ^ oldFlag) & 0x7FFFFFFF) != 0)
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicDbFlag, curFlag);
                        visibleChange = CMPXDbActiveTask::EAllVisible;
                        }
                    MPX_DEBUG2("    GeneralFlags[%b]", curFlag);
                    }

                if (attributeId & EMPXMediaGeneralTitle)
                    {
                    TBool titleChanged(NULL == aMusicTable);

                    const TDesC& title = aMedia.ValueText(KMPXMediaGeneralTitle);
                    TPtrC truncatedTitle(title.Left(KMCMaxTextLen));
                    if (aMusicTable)
                        {
                        // Title of the song has been changed
                        if (truncatedTitle.Compare(MPXDbCommonUtil::GetColumnTextL(*aMusicTable, EMusicTitle)) != 0)
                            {
                            titleChanged = ETrue;
                            }
                        }

                    if (titleChanged)
                        {
                        if ( title.Length() == 0 )
                            {
                            const TDesC& path = aMedia.ValueText( KMPXMediaGeneralUri );
                            TParsePtrC parse( path );
                            TPtrC truncatedParse( parse.Name().Left( KMCMaxTextLen ) );

                            MPXDbCommonUtil::AppendValueL( aFields, aValues, KMCMusicName, truncatedParse );
                            }
                        else
                            {
                            MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicName, truncatedTitle);
                            }
                        visibleChange = CMPXDbActiveTask::EAllVisible;
                        metaDataModified = ETrue;

                        MPX_DEBUG2("    Title[%S]", &truncatedTitle);
                        }
                    }

                if (attributeId & EMPXMediaGeneralMimeType)
                    {
                    const TDesC& mimeTypeText( aMedia.ValueText(KMPXMediaGeneralMimeType) );
                    MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicMimeType, mimeTypeText);
                    MPX_DEBUG2("    MimeType[%S]", &mimeTypeText);
                    }
                    
                if (attributeId & EMPXMediaGeneralUri)
                    {
                    const TDesC& uri = aMedia.ValueText(KMPXMediaGeneralUri);
                    const TDesC& uriTrunc(uri.Mid(KMCPathStartPos));

                    TDriveUnit driveUnit(uri);
                    TUint volId(MPXDbCommonUtil::GetVolIdMatchDriveIdL(iDbManager.Fs(), driveUnit));

                    if (!aMusicTable || ((uriTrunc != MPXDbCommonUtil::GetColumnTextL(*aMusicTable, EMusicLocation)) ||
                        (volId != aMusicTable->ColumnInt64(EMusicVolumeId))))
                        {
                        // only do the update something changed
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicVolumeId, volId);
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicLocation,
                            uri.Mid(KMCPathStartPos));
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicDRM, DRMTypeL(uri));
                        MPX_DEBUG3("    VolumeId[%u] Location[%S]", volId, &uri);

                        if (!aMedia.IsSupported(KMPXMediaGeneralMimeType))
                            {
                            TBuf< KMaxDataTypeLength > mimeTypeText(MPXDbCommonUtil::GetMimeTypeForUriL(uri).Des());
                            MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicMimeType, mimeTypeText);
                            MPX_DEBUG2("    MimeType[%S]", &mimeTypeText);
                            }

                        if (!aMusicTable && !aMedia.IsSupported(KMPXMediaGeneralTitle))
                            {
                            TParsePtrC parser(uri);
                            TPtrC title = parser.Name();
                            // use file name as song name
                            MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicName, title);

                            visibleChange = CMPXDbActiveTask::EAllVisible;
                            MPX_DEBUG2("    Title[%S]", &title);
                            }

                        // URI of the song has been changed. This changes the Id of the song
                        if (aMusicTable)
                            {
                            TUint32 newSongId = MPXDbCommonUtil::GenerateUniqueIdL(iDbManager.Fs(), EMPXCollection, uri, EFalse);
                            if (aSongId != newSongId)
                                {
                                MPX_DEBUG3("    CurrentSongId[0x%x] changed to [0x%x]", aSongId, newSongId);
                                if (songChangedMessage)
                                    {
                                    songChangedMessage->SetTObjectValueL<TMPXItemId>(KMPXMessageMediaGeneralId, newSongId);
                                    songChangedMessage->SetTObjectValueL<TMPXItemId>(KMPXMessageMediaDeprecatedId, aSongId);
                                    }

                                MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicUniqueId, newSongId);
                                }
                            }
                        }
                    }

                if (attributeId & EMPXMediaGeneralPlayCount)
                    {
                    TInt increment(aMedia.ValueTObjectL<TInt>(KMPXMediaGeneralPlayCount));
                    TUint32 curCount(increment);
                    if (aMusicTable)
                        {
                        curCount += aMusicTable->ColumnInt(EMusicPlayCount);
                        }

                    if (!aMusicTable || (curCount != aMusicTable->ColumnInt(EMusicPlayCount)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicPlayCount, curCount);

                        // this needs to be visible in order for Recently Played and Most Played
                        // playlists will be updated
                        visibleChange = CMPXDbActiveTask::ESingleVisible;

                        if (aItemChangedMessages)
                            {
                            iObserver.HandlePlayCountModifiedL(*aItemChangedMessages);
                            }
                        addSongChangedMessage = EFalse;
                        }

                    MPX_DEBUG2("    PlayCount[%d]", curCount);
                    }

                if (attributeId & EMPXMediaGeneralLastPlaybackTime)
                    {
                    HBufC* time = MPXDbCommonUtil::TTimeToDesLC(
                        TTime(aMedia.ValueTObjectL<TInt64>(KMPXMediaGeneralLastPlaybackTime)));
                    if (!aMusicTable || (*time != MPXDbCommonUtil::GetColumnTextL(*aMusicTable, EMusicTimePlayed)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicTimePlayed, *time);
                        // this needs to be visible in order for Recently Played playlist to be updated
                        visibleChange = CMPXDbActiveTask::ESingleVisible;

                        if (aItemChangedMessages)
                            {
                            iObserver.HandlePlaybackTimeModifiedL(*aItemChangedMessages);
                            }
                        addSongChangedMessage = EFalse;
                        }
                    MPX_DEBUG2("    PlaybackTime[%S]", time);
                    CleanupStack::PopAndDestroy(time);
                    }

                if (attributeId & EMPXMediaGeneralDuration)
                    {
                    TInt duration(aMedia.ValueTObjectL<TInt>(KMPXMediaGeneralDuration));
                    if (!aMusicTable || (duration != aMusicTable->ColumnInt(EMusicDuration)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicDuration, duration);
                        visibleChange = CMPXDbActiveTask::EAllVisible;
                        metaDataModified = ETrue;
                        }
                    MPX_DEBUG2("    Duration[%d]", duration);
                    }

                if (attributeId & EMPXMediaGeneralSynchronized)
                    {
                    TBool synced(aMedia.ValueTObjectL<TBool>(KMPXMediaGeneralSynchronized));
                    if (!aMusicTable || (synced != aMusicTable->ColumnInt(EMusicSync)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicSync, synced);
                        }
                    MPX_DEBUG2("    Synchronized[%d]", synced);
                    }

                if (attributeId & EMPXMediaGeneralModified)
                    {
                    TBool modified(aMedia.ValueTObjectL<TBool>(KMPXMediaGeneralModified));
                    if (!aMusicTable || (modified != aMusicTable->ColumnInt(EMusicModified)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicMod, modified);
                        }
                    MPX_DEBUG2("    Modified[%d]", modified);
                    }

                if (attributeId & EMPXMediaGeneralComment)
                    {
                    TPtrC comment = aMedia.ValueText(KMPXMediaGeneralComment).Left(KMCMaxTextLen);
                    if (!aMusicTable || (comment != MPXDbCommonUtil::GetColumnTextL(*aMusicTable, EMusicComment)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicComment, comment);
                        metaDataModified = ETrue;
                        }
                    MPX_DEBUG2("    Comment[%S]", &comment);
                    }

                if (attributeId & EMPXMediaGeneralCopyright)
                    {
                    const TDesC& copyright = aMedia.ValueText(KMPXMediaGeneralCopyright).
                        Left(KMCMaxTextLen);
                    if (!aMusicTable || (copyright != MPXDbCommonUtil::GetColumnTextL(*aMusicTable, EMusicCopyright)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicCopyright, copyright);
                        metaDataModified = ETrue;
                        }
                    MPX_DEBUG2("    Copyright[%S]", &copyright);
                    }
                }
                break;

            case KMPXMediaIdMusic:
                {
                if (attributeId & EMPXMediaMusicAlbumTrack)
                    {
                    const TDesC& trackNumber = aMedia.ValueText(KMPXMediaMusicAlbumTrack);

                    // KMaxTInt is used to represent null album track
                    TInt track(KMaxTInt);
                    if (trackNumber.Length())
                        {
                        TLex stringParser(trackNumber);

                        if ((stringParser.Val(track) != KErrNone) ||
                            (track == 0) || (track > 999))  // Limit track number to 3 characters
                            {
                            track = KMaxTInt;
                            }
                        }

                    if (!aMusicTable || (track != aMusicTable->ColumnInt(EMusicAlbumTrack)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicTrackNumber, track);
                        visibleChange = CMPXDbActiveTask::EAllVisible;
                        metaDataModified = ETrue;
                        }
                    MPX_DEBUG3("    Album Track[%S][%d]", &trackNumber, track);
                    }

                if (attributeId & EMPXMediaMusicYear)
                    {
                    TInt64 int64(aMedia.ValueTObjectL<TInt64>(KMPXMediaMusicYear));

                    TTime maxTime(0);
                    maxTime += TTimeIntervalYears(9999);    // Limit years to 4 characters
                    TTime time(int64);

                    if (time > maxTime)
                        {
                        time = Time::NullTTime();
                        }

                    HBufC* timeStr = MPXDbCommonUtil::TTimeToDesLC(time);
                    if (!aMusicTable || (*timeStr != MPXDbCommonUtil::GetColumnTextL(*aMusicTable, EMusicReleaseDate)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicReleaseDate, *timeStr);
                        metaDataModified = ETrue;
                        }
                    MPX_DEBUG2("    Music Year[%S]", timeStr);
                    CleanupStack::PopAndDestroy(timeStr);
                    }

                if (attributeId & EMPXMediaMusicRating)
                    {
                    TInt rating(aMedia.ValueTObjectL<TInt>(KMPXMediaMusicRating));
                    if (!aMusicTable || (rating != aMusicTable->ColumnInt(EMusicRating)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicRating, rating);
                        metaDataModified = ETrue;
                        }
                    MPX_DEBUG2("    Rating[%d]", rating);
                    }

                if (attributeId & EMPXMediaMusicAlbumArtFileName)
                    {
#ifdef ABSTRACTAUDIOALBUM_INCLUDED     
                    TInt containEmbeddedArt(0);                   
                    if (aMusicTable )
                        {                        
                        containEmbeddedArt = aMusicTable->ColumnInt(EMusicContainEmbeddedArt);                    
                        }
#endif // ABSTRACTAUDIOALBUM_INCLUDED
                    const TDesC& albumArtFilename = aMedia.ValueText(KMPXMediaMusicAlbumArtFileName).Left(KMCMaxTextLen);
#ifdef ABSTRACTAUDIOALBUM_INCLUDED 
                    TParsePtrC parse(albumArtFilename);
                    TPtrC ext(parse.Ext());
                    //set flag to false, so .alb will not overwrite art field in album, artist table 
                    // when song with embedded art
                    if ((ext.CompareF(KAbstractAlbumExt)== 0) && containEmbeddedArt)
                        {
                        iArtNeedUpdated = EFalse;
                        }
                          
                    if ( ((ext.CompareF(KAbstractAlbumExt)== 0) && !containEmbeddedArt) || (ext.CompareF(KAbstractAlbumExt)!= 0))                    
                        {
#endif // ABSTRACTAUDIOALBUM_INCLUDED
                        if (!aMusicTable || (albumArtFilename != MPXDbCommonUtil::GetColumnTextL(*aMusicTable, EMusicArt)))
                            {
                            MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicArt, albumArtFilename);
                            visibleChange = CMPXDbActiveTask::EAllVisible;
                            metaDataModified = ETrue;              
                            }
#ifdef ABSTRACTAUDIOALBUM_INCLUDED                                       
                        }
#endif // ABSTRACTAUDIOALBUM_INCLUDED                                         
                    MPX_DEBUG2("    Album Art Filename[%S]", &albumArtFilename);
                    }

                if (attributeId & EMPXMediaMusicURL)
                    {
                    const TDesC& url = aMedia.ValueText(KMPXMediaMusicURL).Left(KMCMaxTextLen);
                    if (!aMusicTable || (url != MPXDbCommonUtil::GetColumnTextL(*aMusicTable, EMusicUrl)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicUrl, url);
                        metaDataModified = ETrue;
                        }

                    MPX_DEBUG2("    Music URL[%S]", &url);
                    }
#ifdef ABSTRACTAUDIOALBUM_INCLUDED                    
               if (attributeId & EMPXMediaMusicAlbumArtist)
                    {
                    const TDesC& albumartist = aMedia.ValueText(KMPXMediaMusicAlbumArtist);
                    TPtrC truncatedAlbumartist(albumartist.Left(KMCMaxTextLen));
                    
                    if (!aMusicTable || (truncatedAlbumartist.Compare(MPXDbCommonUtil::GetColumnTextL(*aMusicTable, EMusicAlbumArtist)) != 0))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicAlbumArtist, truncatedAlbumartist);
                        visibleChange = CMPXDbActiveTask::EAllVisible;
                        metaDataModified = ETrue;
                        }                
                   }
#endif // ABSTRACTAUDIOALBUM_INCLUDED              
                }
                break;

            case KMPXMediaIdAudio:
                {
                if (attributeId & EMPXMediaAudioSamplerate)
                    {
                    TInt samplerate(aMedia.ValueTObjectL<TInt>(KMPXMediaAudioSamplerate));
                    if (!aMusicTable || (samplerate != aMusicTable->ColumnInt(EMusicSampleRate)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicSampleRate, samplerate);
                        metaDataModified = ETrue;
                        }

                    MPX_DEBUG2("    Sample Rate[%d]", samplerate);
                    }

                if (attributeId & EMPXMediaAudioBitrate)
                    {
                    TInt bitrate(aMedia.ValueTObjectL<TInt>(KMPXMediaAudioBitrate));
                    if (!aMusicTable || (bitrate != aMusicTable->ColumnInt(EMusicBitRate)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicBitRate, bitrate);
                        metaDataModified = ETrue;
                        }
                    MPX_DEBUG2("    Bitrate[%d]", bitrate);
                    }

                if (attributeId & EMPXMediaAudioNumberOfChannels)
                    {
                    TUint32 val = aMedia.ValueTObjectL<TUint32>(KMPXMediaAudioNumberOfChannels);
                    if (!aMusicTable || (val != aMusicTable->ColumnInt(EMusicNumChannels)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicNumChannels, val);
                        }
                    MPX_DEBUG2("    Num of Channels[%d]", val);
                    }
                }
                break;

            case KMPXMediaIdDrm:
                {
                if (attributeId & EMPXMediaDrmType)
                    {
                    TInt drmType(aMedia.ValueTObjectL<TInt>(KMPXMediaDrmType));
                    if (!aMusicTable || (drmType != aMusicTable->ColumnInt(EMusicDRM)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicDRM, drmType);
                        }
                    MPX_DEBUG2("    DRM type[%d]", drmType);
                    }

                if (attributeId & KMPXMediaDrmRightsStatus.iAttributeId)
                    {
                    if (aMusicTable)
                        {
                        TMPXMediaDrmRightsStatus status =
                                 aMedia.ValueTObjectL<TMPXMediaDrmRightsStatus>(KMPXMediaDrmRightsStatus);

                        //.Set the db flag
                        TUint32 curFlag(aMusicTable->ColumnInt64(EMusicDbFlag));
                        TUint32 oldFlag(curFlag);

                        if ((status != EMPXDrmRightsFull) && (status != EMPXDrmRightsRestricted))
                            {
                            // No rights
                            curFlag |= KMPXMediaGeneralFlagsIsDrmLicenceInvalid;
                            }
                        else
                            {
                            // Rights valid
                            curFlag &= (KMPXMediaGeneralFlagsIsDrmLicenceInvalid ^ 0xFFFFFFFF);
                            }

                        // The field is written ONLY if the flag value is changing
                        if (((curFlag ^ oldFlag) & 0x7FFFFFFF) != 0)
                        {
                            MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicDbFlag, curFlag);
                            visibleChange = CMPXDbActiveTask::EAllVisible;
                        }

                        MPX_DEBUG2("    Rights Status[%d]", curFlag);
                        }
                    }

                break;
                }

            case KMPXMediaIdMTP:
                {
                if (attributeId & KMPXMediaMTPDrmStatus.iAttributeId)
                    {
                    TUint16 drmStatus(aMedia.ValueTObjectL<TUint16>(KMPXMediaMTPDrmStatus));
                    if (!aMusicTable || (drmStatus != aMusicTable->ColumnInt(EMusicMTPDrmStatus)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicMTPDrmStatus, drmStatus);
                        }
                    MPX_DEBUG2("    MTP Drm Status[%d]", drmStatus);
                    }
                }
                break;

            default:
                break;
            } // end switch
        } // end for

    // get the current artist/album/genre/composer/abstractalbum
    // this is required because the recordset may be reused by the code below
    TUint32 artistId(0);
    TUint32 albumId(0);
    TUint32 genreId(0);
    TUint32 composerId(0);
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
    TUint32 abstractAlbumId(0);
#endif // ABSTRACTAUDIOALBUM_INCLUDED
    //need to get song art for updating art field in Album and Artist tables
    TPtrC art(KNullDesC);
    if (aMusicTable)
        {
        artistId = aMusicTable->ColumnInt64(EMusicArtist);
        albumId = aMusicTable->ColumnInt64(EMusicAlbum);
        genreId = aMusicTable->ColumnInt64(EMusicGenre);
        composerId = aMusicTable->ColumnInt64(EMusicComposer);
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
        abstractAlbumId = aMusicTable->ColumnInt64(EMusicAbstractAlbum);
#endif // ABSTRACTAUDIOALBUM_INCLUDED        
        art.Set(MPXDbCommonUtil::GetColumnTextL(*aMusicTable, EMusicArt));
        }

    // update the artist field
    TUint32 id(0);
    if (UpdateCategoryFieldL(EMPXArtist, aMedia, KMPXMediaMusicArtist, artistId,
        aDrive, aItemChangedMessages, id, art))
        {
        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicArtist, id);
        metaDataModified = (aMusicTable != NULL);
        visibleChange = CMPXDbActiveTask::EAllVisible;
        iArtNeedUpdated = EFalse;
        }

    // update the album field
    if (UpdateCategoryFieldL(EMPXAlbum, aMedia, KMPXMediaMusicAlbum, albumId,
        aDrive, aItemChangedMessages, id, art))
        {
        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicAlbum, id);
        metaDataModified = (aMusicTable != NULL);
        visibleChange = CMPXDbActiveTask::EAllVisible;

        //
        // added to handle error EALU-73WDJN. If the album name of the last song
        // in the album for the artist is changed to an existing album name from
        // artist view, a change message needs to be sent in order for UI to
        // correctly refresh.
        //
        // Fix for EDXU-7BBALS, remove check for HasOtherSongsInArtistAlbumL
        // Always send a Album Inserted message when Album change to get Artist updated
        if (aItemChangedMessages)
            {
            // send album added m essage instead of album deleted or modified
            // to avoid collection paths of other clients being modified
            MPXDbCommonUtil::AddItemChangedMessageL(*aItemChangedMessages, albumId,
                    EMPXItemInserted, EMPXAlbum, KDBPluginUid);
            }
        iArtNeedUpdated = EFalse;
        }

    // update the genre field
    if (UpdateCategoryFieldL(EMPXGenre, aMedia, KMPXMediaMusicGenre, genreId,
        aDrive, aItemChangedMessages, id))
        {
        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicGenre, id);
        metaDataModified = (aMusicTable != NULL);
        visibleChange = CMPXDbActiveTask::EAllVisible;
        }

    // update the composer field
    if (UpdateCategoryFieldL(EMPXComposer, aMedia, KMPXMediaMusicComposer, composerId,
        aDrive, aItemChangedMessages, id))
        {
        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicComposer, id);
        metaDataModified = (aMusicTable != NULL);
        visibleChange = CMPXDbActiveTask::EAllVisible;
        }
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
    if ( aMedia.IsSupported (KMPXMediaMusicAlbumArtFileName))
        {
        const TDesC& albumArtFilename = aMedia.ValueText(KMPXMediaMusicAlbumArtFileName).Left(KMCMaxTextLen);            
        TParsePtrC parse( albumArtFilename );
        TPtrC ext( parse.Ext() );
        if (ext.CompareF(KAbstractAlbumExt)== 0)   
            {
            if (UpdateCategoryFieldL(EMPXAbstractAlbum, aMedia, KMPXMediaMusicAlbumArtFileName, abstractAlbumId,
                aDrive, aItemChangedMessages, id))
                {
                MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicAbstractAlbum, id);
                metaDataModified = (aMusicTable != NULL);
                visibleChange = CMPXDbActiveTask::EAllVisible;
                }
            }                       
        }
#endif // ABSTRACTAUDIOALBUM_INCLUDED
#if defined (__MTP_PROTOCOL_SUPPORT)
    // Set Mod bit to ETrue if metadata has been updated and caller hasn't explicitly
    // set/reset it
    if (aMusicTable &&
        !aMedia.IsSupported(KMPXMediaGeneralModified) &&
        metaDataModified)
        {
        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicMod, 1);
        MPX_DEBUG1("    Modified[1]");
        }
#endif

    if (aItemChangedMessages)
        {
      if (aFields.Count() && addSongChangedMessage)
          {
          aItemChangedMessages->AppendL(*songChangedMessage);
          }
        CleanupStack::PopAndDestroy(songChangedMessage);
        }

    return visibleChange;
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::DRMTypeL
// ----------------------------------------------------------------------------
//
TMCDrmType CMPXDbMusic::DRMTypeL(
    const TDesC& aFile)
    {
    MPX_FUNC("CMPXDbMusic::DRMTypeL");

    TMCDrmType drm(EMCDrmNone);
    TInt pos(0);
    TParsePtrC fullEntry(aFile);
    TPtrC theExt = fullEntry.Ext();

    if (iExtensionsDrm->Find(theExt, pos) == 0)
        {
        drm = MPXDbCommonUtil::GetDRMTypeL(aFile);
        }

    return drm;
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::GenerateMusicMatchingCriteriaLC
// ----------------------------------------------------------------------------
//
HBufC* CMPXDbMusic::GenerateMusicMatchingCriteriaLC(
    TUint32 aGeneralId,
    TUint32 aContainerId,
    TMPXGeneralType aType,
    const CMPXMedia& aCriteria)
    {
    MPX_FUNC("CMPXDbMusic::GenerateMusicMatchingCriteriaLC");

    const TArray<TMPXAttribute> criteria = aCriteria.Attributes();
    TInt criteriaCount(criteria.Count());

    // construct an array of criteria strings
    CDesCArrayFlat* sqlCriteria = new (ELeave) CDesCArrayFlat(criteriaCount);
    CleanupStack::PushL(sqlCriteria);

    // If EMPXMediaGeneralDeleted is not defined, always unset the deleted bit for matching
    if (!aCriteria.IsSupported(KMPXMediaGeneralDeleted))
        {
        sqlCriteria->AppendL(KCriterionMusicNotDeleted);
        }

    TBool volumeAdded(EFalse);
    for (TInt i = 0; i < criteriaCount; ++i)
        {
        const TMPXAttribute& criterion = criteria[i];

        if (criterion == KMPXMediaGeneralId)
            {
            // Set the type if no type is specified
            TInt category(MPX_ITEM_CATEGORY(aGeneralId));

            if( aType == EMPXNoType )
                {
                aType = (category == EMPXCollection) ? EMPXItem : EMPXGroup;
                }

            TPtrC ptr;
            if (aType == EMPXItem && (category == EMPXCollection))
                {
                ptr.Set(KCriterionMusicUniqueId);
                }
            else if (aType == EMPXGroup && (category == EMPXArtist))
                {
                ptr.Set(KCriterionMusicArtist);
                }
            else if (aType == EMPXGroup && (category == EMPXAlbum))
                {
                ptr.Set(KCriterionMusicAlbum);
                }
            else if (aType == EMPXGroup && (category == EMPXGenre))
                {
                ptr.Set(KCriterionMusicGenre);
                }
            else if (aType == EMPXGroup && (category == EMPXComposer))
                {
                ptr.Set(KCriterionMusicComposer);
                }
            else
                {
                User::Leave(KErrNotSupported);
                }

            MPXDbCommonUtil::AddSqlCriterionL(*sqlCriteria, ptr, aGeneralId);
            }
        else if (criterion == KMPXMediaGeneralContainerId)
            {
            TInt containerCategory(MPX_ITEM_CATEGORY(aContainerId));

            if (aType == EMPXGroup && (containerCategory == EMPXArtist))
                {
                MPXDbCommonUtil::AddSqlCriterionL(*sqlCriteria,
                    KCriterionMusicArtist, aContainerId);
                }
            else if (aType == EMPXGroup && (containerCategory == EMPXAlbum))
                {
                MPXDbCommonUtil::AddSqlCriterionL(*sqlCriteria,
                    KCriterionMusicAlbum, aContainerId);
                }
            else
                {
                //User::Leave(KErrNotSupported);
                }
            }
        else if (criterion == KMPXMediaGeneralTitle)
            {
            HBufC* title = MPXDbCommonUtil::ProcessPatternCharsLC(
                aCriteria.ValueText(KMPXMediaGeneralTitle));
            MPXDbCommonUtil::AddSqlCriterionL(*sqlCriteria, KCriterionMusicTitle, *title);
            CleanupStack::PopAndDestroy(title);
            }
        else if (criterion == KMPXMediaGeneralUri)
            {
            // full URI from criteria
            const TDesC& uri = aCriteria.ValueText(KMPXMediaGeneralUri);
            TUint32 itemId = MPXDbCommonUtil::GenerateUniqueIdL(iDbManager.Fs(), EMPXCollection,
                                                                uri, EFalse);

            MPXDbCommonUtil::AddSqlCriterionL(*sqlCriteria, KCriterionMusicUniqueId, itemId);
            }
        else if (criterion == KMPXMediaGeneralDrive)
            {
            if (!volumeAdded)
                {
                // validate the drive letter, TDriveUnit panics if given drive isn't between
                // 'A' to 'Z'
                const TDesC& drive = aCriteria.ValueText(KMPXMediaGeneralDrive);
                TInt driveInt = 0;
                if( drive == KNullDesC || RFs::CharToDrive( drive[0], driveInt )!= KErrNone )
                    {
                    User::Leave(KErrArgument);
                    }               
                TDriveUnit driveUnit(drive);
                MPXDbCommonUtil::AddSqlCriterionL(*sqlCriteria, KCriterionMusicVolume,
                    MPXDbCommonUtil::GetVolIdMatchDriveIdL(iDbManager.Fs(), driveUnit));
                volumeAdded = ETrue;
                }
            }
        else if (criterion == KMPXMediaGeneralSynchronized)
            {
            MPXDbCommonUtil::AddSqlCriterionL(*sqlCriteria, KCriterionMusicSync,
                aCriteria.ValueTObjectL<TBool>(KMPXMediaGeneralSynchronized));
            }
        else if (criterion == KMPXMediaGeneralDeleted)
            {
            MPXDbCommonUtil::AddSqlCriterionL(*sqlCriteria, KCriterionMusicDeleted,
                aCriteria.ValueTObjectL<TBool>(KMPXMediaGeneralDeleted));
            }
        else if (criterion == KMPXMediaGeneralModified)
            {
            MPXDbCommonUtil::AddSqlCriterionL(*sqlCriteria, KCriterionMusicModified,
                aCriteria.ValueTObjectL<TBool>(KMPXMediaGeneralModified));
            }
         else
            {
            // to-do: provide searching ability on the rest of the fields
            }
        }

    // construct the final criteria string
    HBufC* criteriaStr = MPXDbCommonUtil::StringFromArrayLC(*sqlCriteria, KMCAndKeyword);

    CleanupStack::Pop(criteriaStr);
    CleanupStack::PopAndDestroy(sqlCriteria);
    CleanupStack::PushL(criteriaStr);

    return criteriaStr;
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::ConstructUriL
// ----------------------------------------------------------------------------
//
HBufC* CMPXDbMusic::ConstructUriL(
    RSqlStatement& aMusicTable,
    TUint32 aMediaId)
    {
    MPX_FUNC("CMPXDbMusic::ConstructUriL");

    TUint volId(aMusicTable.ColumnInt64(EMusicVolumeId));
    TPtrC location(MPXDbCommonUtil::GetColumnTextL(aMusicTable, EMusicLocation));
    if(!location.Length())
        {
        return GetUriL(aMediaId);
        }
    return MPXDbCommonUtil::CreateFullPathL(
        MPXDbCommonUtil::GetDriveIdMatchVolIdL(iDbManager.Fs(), volId), location);
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::CreateTableL
// ----------------------------------------------------------------------------
//
void CMPXDbMusic::CreateTableL(
    RSqlDatabase& aDatabase,
    TBool /* aCorruptTable */)
    {
    MPX_FUNC("CMPXDbMusic::CreateTableL");

    // Create the table
    User::LeaveIfError(aDatabase.Exec(KMusicCreateTable));

    // Do not create any other indexes than the one on UniqueId
    // as they only slow down the database overall
    User::LeaveIfError(aDatabase.Exec(KMusicDeletedTitleIndex));
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::DropTableL
// ----------------------------------------------------------------------------
//
void CMPXDbMusic::DropTableL(
    RSqlDatabase& aDatabase)
    {
    MPX_FUNC("CMPXDbMusic::DropTableL");
    User::LeaveIfError(aDatabase.Exec(KMusicDropTable));
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::CheckTableL
// ----------------------------------------------------------------------------
//
TBool CMPXDbMusic::CheckTableL(
    RSqlDatabase& aDatabase)
    {
    MPX_FUNC("CMPXDbMusic::CheckTableL");
    return DoCheckTable(aDatabase, KMusicCheckTable);
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::UpdateCategoryFieldL
// ----------------------------------------------------------------------------
//
TBool CMPXDbMusic::UpdateCategoryFieldL(
    TMPXGeneralCategory aCategory,
    const CMPXMedia& aMedia,
    const TMPXAttribute& aAttribute,
    TUint32 aOldId,
    TInt aDriveId,
    CMPXMessageArray* aItemChangedMessages,
    TUint32& aItemId, 
    const TDesC& aArt)
    {
    MPX_FUNC("CMPXDbMusic::UpdateCategoryFieldL");
    
	if (aCategory == EMPXAlbum)
	    {
	    return UpdateCategoryFieldForAlbumL(aMedia, aAttribute, aOldId, 
	        aDriveId, aItemChangedMessages, aItemId, aArt);
	    }
	
    TBool updated(EFalse);
    TBool itemNotRemoved( EFalse );
    TBool itemAdded( EFalse );

    // update category table and add category Id to the music table
    if (!aOldId || aMedia.IsSupported(aAttribute))
        {
        TInt changeMsgCount( 0 );
        if( aItemChangedMessages )
            {
            changeMsgCount = aItemChangedMessages->Count();
            }

        if (aMedia.IsSupported(aAttribute))
            {
            TPtrC name(aMedia.ValueText(aAttribute).Left(KMCMaxTextLen));

            // construct the new ID for the category record
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
            // genre and abstractalbum are not case sensitive
            TBool caseSensitive = ETrue;         
            if ((aCategory == EMPXGenre) || (aCategory == EMPXAbstractAlbum))
                caseSensitive = EFalse;
                      
            aItemId = MPXDbCommonUtil::GenerateUniqueIdL(iDbManager.Fs(), aCategory,
                name, caseSensitive);
#else
            // only genre is not case sensitive
            aItemId = MPXDbCommonUtil::GenerateUniqueIdL(iDbManager.Fs(), aCategory,
                name, (aCategory != EMPXGenre));
#endif
            if (!aOldId || (aOldId != aItemId))
                {
                // only add if the ID changed,
                // otherwise the song was updated but the artist name was not
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
                if(aCategory == EMPXAbstractAlbum)
                    {
                    if (aMedia.ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory) == EMPXSong )
                        {
                        iObserver.AddAbstractAlbumItemL(KNullDesC, aDriveId,
                            aItemChangedMessages, itemAdded, name, KNullDesC);                
                        }
                    else
                        {     
                        TPtrC albumartist(KNullDesC);
                        TPtrC abstractAlbumName(KNullDesC);
                        if (aMedia.IsSupported(KMPXMediaMusicAlbumArtist) )
                            {                         
                            albumartist.Set(aMedia.ValueText(KMPXMediaMusicAlbumArtist).Left(KMCMaxTextLen));
                            //get AlbumArt AbstractAlbum
                            MPX_DEBUG2("     albumartist[%S]", &albumartist);
                            }
                        if (aMedia.IsSupported(KMPXMediaGeneralTitle) )
                            {                         
                            abstractAlbumName.Set(aMedia.ValueText(KMPXMediaGeneralTitle).Left(KMCMaxTextLen));
                            MPX_DEBUG2("     abstractAlbumName[%S]", &abstractAlbumName);
                            }
                        
                        // ignore the return value                       
                        iObserver.AddAbstractAlbumItemL(abstractAlbumName, aDriveId,
                            aItemChangedMessages, itemAdded, name, albumartist);       
                        }
                    }
                else
#endif // ABSTRACTAUDIOALBUM_INCLUDED
                if (aCategory == EMPXArtist)
                    {
                    //for the update case, need to maintain art field for Artist/Album table.
                    if (aOldId && (aOldId != aItemId))
                        {
                        CMPXMedia* media = CMPXMedia::NewL(aMedia);
                        CleanupStack::PushL(media);
                        media->SetTextValueL(KMPXMediaMusicAlbumArtFileName, aArt);                        
                        iObserver.AddCategoryItemL(aCategory, *media, aDriveId, aItemChangedMessages, itemAdded);                        
                        CleanupStack::PopAndDestroy(media);
                        } 
                    else
                        {      
                        iObserver.AddCategoryItemL(aCategory, aMedia, aDriveId, aItemChangedMessages, itemAdded);
                        }
                    }
                else
                    {
                    // ignore the return value
                    iObserver.AddCategoryItemL(aCategory, aMedia, aDriveId,
                        aItemChangedMessages, itemAdded);
                    }
                updated = ETrue;
                }
            }
        else
            {
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
            // genre and abstractalbum are not case sensitive
            aItemId = MPXDbCommonUtil::GenerateUniqueIdL(iDbManager.Fs(), aCategory, KNullDesC,
                (aCategory != EMPXGenre)&&(aCategory != EMPXAbstractAlbum));
#else
            // only genre is not case sensitive
            aItemId = MPXDbCommonUtil::GenerateUniqueIdL(iDbManager.Fs(), aCategory, KNullDesC,
                (aCategory != EMPXGenre));
#endif
            if (!aOldId || (aOldId != aItemId))
                {
#ifdef ABSTRACTAUDIOALBUM_INCLUDED                                        
                if(aCategory == EMPXAbstractAlbum)
                    {                         
                    //get AlbumArt for AbstractAlbum
                    TPtrC albumartist(aMedia.ValueText(KMPXMediaMusicAlbumArtist).Left(KMCMaxTextLen));
                    MPX_DEBUG2("    Music albumartist[%S]", &albumartist);                       
                    TPtrC abstractAlbumName(aMedia.ValueText(KMPXMediaGeneralTitle).Left(KMCMaxTextLen));
                    MPX_DEBUG2("    Music abstractAlbumName[%S]", &abstractAlbumName);     
                    // ignore the return value
                    iObserver.AddAbstractAlbumItemL(abstractAlbumName, aDriveId,
                        aItemChangedMessages, itemAdded, KNullDesC, albumartist);     
                    }            
               else
#endif // ABSTRACTAUDIOALBUM_INCLUDED        
                   {
                   // ignore the return value
                   iObserver.AddCategoryItemL(aCategory, aMedia, aDriveId,
                       aItemChangedMessages, itemAdded);
                   }
               updated = ETrue;
               }
            }

        if (aOldId && (aOldId != aItemId))
            {
            iObserver.DeleteSongForCategoryL(aCategory, aOldId, aDriveId,
                aItemChangedMessages, itemNotRemoved);
            updated = ETrue;
            }

        // Special case where the item(s) has been renamed.
        // In this case, a new category is created +1 change msg
        //               a old category is removed +1 change msg
        // We merge these 2 change messages into one using the deprecated ID
        //
        if( aItemChangedMessages )
            {
            TInt newChangeMsgCount( aItemChangedMessages->Count() );
            if(  newChangeMsgCount - changeMsgCount > 0 )
                {
                TInt oldId = KErrNotFound;
                TInt newId = KErrNotFound;
                for( TInt i=0; i<newChangeMsgCount; ++i )
                    {
                    CMPXMessage& msg = *(*aItemChangedMessages)[i];

                    TMPXItemId id = msg.ValueTObjectL<TMPXItemId>(KMPXMessageMediaGeneralId);
                    TMPXChangeEventType changeType = msg.ValueTObjectL<TMPXChangeEventType>(KMPXMessageChangeEventType);

                    // Look for the added and deleted category IDs
                    //
                    if( id == aOldId && changeType == EMPXItemDeleted )
                        {
                        oldId = i;
                        }
                    else if( id == aItemId && changeType == EMPXItemInserted )
                        {
                        newId = i;
                        }
                    }

                if( oldId != KErrNotFound &&
                    newId != KErrNotFound )
                    {
                    aItemChangedMessages->Remove(oldId);  // category removed
                    aItemChangedMessages->Remove(newId);  // category added
                    MPXDbCommonUtil::AddItemChangedMessageL(*aItemChangedMessages, aItemId, EMPXItemModified,
                                                            aCategory, KDBPluginUid, aOldId );
                    }
                else if ( oldId !=KErrNotFound && itemAdded ) // old item removed, new item already exist
                    {
                    MPXDbCommonUtil::AddItemChangedMessageL(*aItemChangedMessages, aItemId, EMPXItemModified,
                                                            aCategory, KDBPluginUid, aOldId );
                    }
                else if ( newId !=KErrNotFound && itemNotRemoved ) // new item added, old item still exist
                    {
                    MPXDbCommonUtil::AddItemChangedMessageL(*aItemChangedMessages, aOldId, EMPXItemModified,
                                                            aCategory, KDBPluginUid, aItemId );
                    }
                }
            }
        }

    return updated;
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::ExtraFieldsRequired
// ----------------------------------------------------------------------------
//
TBool CMPXDbMusic::ExtraFieldsRequired(
    const TArray<TMPXAttribute>& aAttrs)
    {
    MPX_DEBUG1("-->CMPXDbMusic::ExtraFieldsRequired");

    // check if any extra fields are required
    TUint defaultFields(EMPXMediaGeneralId |
                        EMPXMediaGeneralType |
                        EMPXMediaGeneralCategory |
                        EMPXMediaGeneralTitle |
                        EMPXMediaGeneralUri |
                        EMPXMediaGeneralFlags);

    TBool extraRequired(EFalse);
    TInt count(aAttrs.Count());
    for (TInt i = 0; i < count; ++i)
        {
        TUint attributeId(aAttrs[i].AttributeId()|defaultFields);
        MPX_DEBUG2("    attribute content id 0x%x", aAttrs[i].ContentId());
        MPX_DEBUG3("    attribute id %b, original attribute id %b", attributeId, aAttrs[i].AttributeId());

        if (KMPXMediaIdGeneral != aAttrs[i].ContentId() ||
            attributeId != defaultFields)
            {
            MPX_DEBUG1("    extraRequired YES");
            extraRequired = ETrue;
            break;
            }
        }

    MPX_DEBUG2("<--CMPXDbMusic::ExtraFieldsRequired returns %d", extraRequired);
    return extraRequired;
    }

// ----------------------------------------------------------------------------------------------------------
// Test if the given media contains supported attributes
// ----------------------------------------------------------------------------------------------------------
//
TBool CMPXDbMusic::IsSupported(
  const CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbMusic::IsSupported");

    // this checklist should match the attributes processed in DoFillInDatabaseInfoL
    return aMedia.IsSupported(KMPXMediaGeneralTitle) ||
        aMedia.IsSupported(KMPXMediaGeneralUri) ||
        aMedia.IsSupported(KMPXMediaGeneralComment) ||
        aMedia.IsSupported(KMPXMediaGeneralSynchronized) ||
        aMedia.IsSupported(KMPXMediaGeneralDeleted) ||
        aMedia.IsSupported(KMPXMediaGeneralModified) ||
        aMedia.IsSupported(KMPXMediaGeneralCopyright) ||
        aMedia.IsSupported(KMPXMediaGeneralDuration) ||
        aMedia.IsSupported(KMPXMediaGeneralFlags) ||
        aMedia.IsSupported(KMPXMediaGeneralPlayCount) ||
        aMedia.IsSupported(KMPXMediaGeneralLastPlaybackTime) ||
        aMedia.IsSupported(KMPXMediaMusicAlbumTrack) ||
        aMedia.IsSupported(KMPXMediaMusicYear) ||
        aMedia.IsSupported(KMPXMediaMusicRating) ||
        aMedia.IsSupported(KMPXMediaMusicAlbumArtFileName) ||
        aMedia.IsSupported(KMPXMediaMusicURL) ||
        aMedia.IsSupported(KMPXMediaMusicArtist) ||
        aMedia.IsSupported(KMPXMediaMusicAlbum) ||
        aMedia.IsSupported(KMPXMediaMusicGenre) ||
        aMedia.IsSupported(KMPXMediaMusicComposer) ||
        aMedia.IsSupported(KMPXMediaAudioSamplerate) ||
        aMedia.IsSupported(KMPXMediaAudioBitrate) ||
        aMedia.IsSupported(KMPXMediaAudioNumberOfChannels) ||
        aMedia.IsSupported(KMPXMediaDrmType) ||
        aMedia.IsSupported(KMPXMediaDrmRightsStatus) ||
        aMedia.IsSupported(KMPXMediaMTPDrmStatus)
#ifdef ABSTRACTAUDIOALBUM_INCLUDED        
        ||aMedia.IsSupported(KMPXMediaMusicAlbumArtist)
#endif // ABSTRACTAUDIOALBUM_INCLUDED
        ;
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::UpdateCategoryFieldForAlbumL
// ----------------------------------------------------------------------------
//
TBool CMPXDbMusic::UpdateCategoryFieldForAlbumL(
    const CMPXMedia& aMedia,
    const TMPXAttribute& aAttribute,
    TUint32 aOldId,
    TInt aDriveId,
    CMPXMessageArray* aItemChangedMessages,
    TUint32& aItemId, 
    const TDesC& aArt)
    {
    MPX_FUNC("CMPXDbMusic::UpdateCategoryFieldForAlbumL");

    TBool updated(EFalse);
    TBool itemNotRemoved( EFalse );
    TBool itemAdded( EFalse );

    // update category table and add category Id to the music table
    if (!aOldId || aMedia.IsSupported(aAttribute))
        {
        TInt changeMsgCount( 0 );
        if( aItemChangedMessages )
            {
            changeMsgCount = aItemChangedMessages->Count();
            }
            
        // construct the new ID for the category record
        aItemId = iObserver.GenerateUniqueIdForAlbumL(aMedia);
            
        if (!aOldId || (aOldId != aItemId))
            {
            // only add if the ID changed,
            // otherwise the song was updated but the artist name was not

            //for the update case, need to maitain art field for Artist/Album table.
            if (aOldId && (aOldId != aItemId))
                {
                CMPXMedia* media = CMPXMedia::NewL(aMedia);
                CleanupStack::PushL(media);
                media->SetTextValueL(KMPXMediaMusicAlbumArtFileName, aArt);
                
                iObserver.AddCategoryItemL(EMPXAlbum, *media, aDriveId, aItemChangedMessages, itemAdded);
                CleanupStack::PopAndDestroy(media);
                }  
            else // !aOldId
                {
                iObserver.AddCategoryItemL(EMPXAlbum, aMedia, aDriveId, aItemChangedMessages, itemAdded);
                }                
            updated = ETrue;
            }

        if (aOldId && (aOldId != aItemId))
            {
            iObserver.DeleteSongForCategoryL(EMPXAlbum, aOldId, aDriveId, 
                aItemChangedMessages, itemNotRemoved, aArt);
            updated = ETrue;
            }

        // Special case where the item(s) has been renamed.
        // In this case, a new category is created +1 change msg
        //               a old category is removed +1 change msg
        // We merge these 2 change messages into one using the deprecated ID
        //
        if( aItemChangedMessages )
            {
            TInt newChangeMsgCount( aItemChangedMessages->Count() );
            if(  newChangeMsgCount - changeMsgCount > 0 )
                {
                TInt oldId = KErrNotFound;
                TInt newId = KErrNotFound;
                for( TInt i=0; i<newChangeMsgCount; ++i )
                    {
                    CMPXMessage& msg = *(*aItemChangedMessages)[i];

                    TMPXItemId id = msg.ValueTObjectL<TMPXItemId>(KMPXMessageMediaGeneralId);
                    TMPXChangeEventType changeType = msg.ValueTObjectL<TMPXChangeEventType>(KMPXMessageChangeEventType);

                    // Look for the added and deleted category IDs
                    //
                    if( id == aOldId && changeType == EMPXItemDeleted )
                        {
                        oldId = i;
                        }
                    else if( id == aItemId && changeType == EMPXItemInserted )
                        {
                        newId = i;
                        }
                    }// for

                if( oldId != KErrNotFound &&
                    newId != KErrNotFound )
                    {
                    aItemChangedMessages->Remove(oldId);  // category removed
                    aItemChangedMessages->Remove(newId);  // category added
                    MPXDbCommonUtil::AddItemChangedMessageL(*aItemChangedMessages, aItemId, EMPXItemModified,
                                                            EMPXAlbum, KDBPluginUid, aOldId );
                    }
                else if ( oldId !=KErrNotFound && itemAdded ) // old item removed, new item already exist
                    {
                    MPXDbCommonUtil::AddItemChangedMessageL(*aItemChangedMessages, aItemId, EMPXItemModified,
                                                            EMPXAlbum, KDBPluginUid, aOldId );
                    }
                else if ( newId !=KErrNotFound && itemNotRemoved ) // new item added, old item still exist
                    {
                    MPXDbCommonUtil::AddItemChangedMessageL(*aItemChangedMessages, aOldId, EMPXItemModified,
                                                            EMPXAlbum, KDBPluginUid, aItemId );
                    }
                }
            }// aItemChangedMessages
        }// if (!aOldId || aMedia.IsSupported(aAttribute))

    return updated;
    }

// End of File
