/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements the functions to manipulate the Podcast table
*
*/


// INCLUDE FILES
#include <pcres.rsg>
#include <sqldb.h>

#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxmediaaudiodefs.h>
#include <mpxmediadrmdefs.h>
#include <mpxpodcastdefs.h>
#include <mpxlog.h>

#include "mpxdbcommondef.h"
#include "mpxdbcommonstd.h"
#include "mpxresource.h"
#include "mpxdbcommonutil.h"
#include "mpxdbmanager.h"

#include "mpxdbutil.h"
#include "mpxpodcastcollectiondbdef.h"
#include "mpxpodcastdbpluginqueries.h"
#include "mpxdbpodcast.h"

//CONSTANTS
// UniqueID column in Uris requests
const TInt KColUniqueID = 0;
// URI column in Uris requests
const TInt KColUri = 1;

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbPodcast* CMPXDbPodcast::NewL(
    CMPXDbManager& aDbManager,
    CMPXResource& aResource,
    MMPXDbPodcastObserver& aObserver)
    {
    MPX_FUNC("CMPXDbPodcast::NewL");

    CMPXDbPodcast* self = CMPXDbPodcast::NewLC(aDbManager, aResource, aObserver);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbPodcast* CMPXDbPodcast::NewLC(
    CMPXDbManager& aDbManager,
    CMPXResource& aResource,
    MMPXDbPodcastObserver& aObserver)
    {
    MPX_FUNC("CMPXDbPodcast::NewLC");

    CMPXDbPodcast* self = new (ELeave) CMPXDbPodcast(aDbManager, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aResource);
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor.
// ----------------------------------------------------------------------------
//
CMPXDbPodcast::~CMPXDbPodcast()
    {
    MPX_FUNC("CMPXDbPodcast::~CMPXDbPodcast");
    delete iExtensionsDrm;
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CMPXDbPodcast::CMPXDbPodcast(
    CMPXDbManager& aDbManager,
    MMPXDbPodcastObserver& aObserver) :
    CMPXDbTable(aDbManager),
    iObserver(aObserver)
    {
    MPX_FUNC("CMPXDbPodcast::CMPXDbPodcast");
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::ConstructL(
    CMPXResource& aResource)
    {
    MPX_FUNC("CMPXDbPodcast::ConstructL");

    BaseConstructL();
    iExtensionsDrm = aResource.ReadDesCArrayL(R_MC_FILE_EXTENSIONS_DRM);
    }

// ----------------------------------------------------------------------------
// Add a episode to the podcast table
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbPodcast::AddEpisodeL(
    const CMPXMedia& aMedia,
    TInt aDrive)
    {
    MPX_FUNC("CMPXDbPodcast::AddEpisodeL");

    if (!aMedia.IsSupported(KMPXMediaGeneralUri))
        {
        User::Leave(KErrArgument);
        }

    TUint32 episodeId(MPXDbCommonUtil::GenerateUniqueIdL(iDbManager.Fs(), EMPXCollection,
        aMedia.ValueText(KMPXMediaGeneralUri), EFalse));
    if ( EpisodeExistsL( episodeId ))
        {
        // Return deleted file paths to caller
        CDesCArray* fp = new( ELeave ) CDesCArrayFlat( 1 );
        CleanupStack::PushL( fp );

        // a list of change event messages a result of the item being removed
        CMPXMessageArray* itemChangedMessages = CMPXMediaArray::NewL();
        CleanupStack::PushL( itemChangedMessages );
        
		//Isadd is passed as true as its called from AddEpisodeL
        //IsAdd true means dont decrement  the episode count from tables like
        //Artist Genere, Album, and Composer , which is not needed when 
        //DeleteEpisodeL is called as part of Add operation
        TRAPD( err, DeleteEpisodeL( episodeId, *fp, *itemChangedMessages, ETrue, ETrue ) );
        
        if ( err != KErrNone ) // EpisodeExistL has found a record from a previously deleted episode, cleanup it now
            {
            iDbManager.ExecuteQueryL( aDrive, KQueryPodcastDelete(), episodeId );
            }
        
        CleanupStack::PopAndDestroy( itemChangedMessages );
        CleanupStack::PopAndDestroy( fp );
        }
    // add the song
    DoAddEpisodeL(episodeId, aMedia, aDrive, NULL);

    

    return episodeId;
    }

// ----------------------------------------------------------------------------
// CMPXDbPodcast::UpdateEpisodeL
// ----------------------------------------------------------------------------
//
CMPXDbActiveTask::TChangeVisibility CMPXDbPodcast::UpdateEpisodeL(
    TUint32 aEpisodeId,
    const CMPXMedia& aMedia,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbPodcast::UpdateEpisodeL");
    return DoUpdateEpisodeL(aEpisodeId, aMedia, &aItemChangedMessages);
    }

// ----------------------------------------------------------------------------
// CMPXDbPodcast::CleanupL
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::CleanupL()
    {
    MPX_FUNC("CMPXDbPodcast::CleanupL");
    iDbManager.ExecuteQueryL(KDbManagerAllDrives, KQueryPodcastCleanup);
    }

// ----------------------------------------------------------------------------
// Remove a episode from the podcast table
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::DeleteEpisodeL(
    TUint32 aEpisodeId,
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages, TBool IsAdd,
    TBool aDeleteRecord)
    {
    MPX_FUNC("CMPXDbPodcast::DeleteEpisodeL");

    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryPodcastGetEpisode,
        aEpisodeId));
    CleanupClosePushL(recordset);
    if (recordset.Next() != KSqlAtRow)
        {
        User::Leave(KErrNotFound);
        }
    DoDeleteEpisodeL(recordset, aUriArray, aItemChangedMessages, IsAdd, aDeleteRecord);
    CleanupStack::PopAndDestroy(&recordset);
    }

// ----------------------------------------------------------------------------
// CMPXDbPodcast::DeleteCategoryL
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::DeleteCategoryL(
    TMPXGeneralCategory aCategory,
    TUint32 aCategoryId,
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbPodcast::DeleteCategoryL");

    TPtrC ptr(MPXDbUtil::PodcastFieldNameForCategoryL(aCategory));
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryPodcastCategory,
        &ptr, aCategoryId));
    CleanupClosePushL(recordset);
    DeleteRecordsetL(recordset, aUriArray, aItemChangedMessages);
    CleanupStack::PopAndDestroy(&recordset);
    }

// ----------------------------------------------------------------------------
// Remove all podcast episodes from the podcast table that are published today
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::DeleteTodayEpisodesL(
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbPodcast::DeleteTodayEpisodesL");

    HBufC* now = MPXDbCommonUtil::CurrentDateDesLC();
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryPodcastToday, now, now));
    CleanupClosePushL(recordset);
    DeleteRecordsetL(recordset, aUriArray, aItemChangedMessages);
    CleanupStack::PopAndDestroy(&recordset);
    CleanupStack::PopAndDestroy(now);
    }

// ----------------------------------------------------------------------------
// Remove all podcast episodes from the podcast table that are published yesterday
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::DeleteYesterdayEpisodesL(
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbPodcast::DeleteYesterdayEpisodesL");

    HBufC* now = MPXDbCommonUtil::CurrentDateDesLC();
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryPodcastYesterday, now, now));
    CleanupClosePushL(recordset);
    DeleteRecordsetL(recordset, aUriArray, aItemChangedMessages);
    CleanupStack::PopAndDestroy(&recordset);
    CleanupStack::PopAndDestroy(now);
    }

// ----------------------------------------------------------------------------
// Remove all podcast episodes from the podcast table that are published this week
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::DeleteThisWeekEpisodesL(
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbPodcast::DeleteThisWeekEpisodesL");

    TInt dayNo(MPXDbUtil::DayNoInWeek());
    dayNo++; // shifted by 1 day to get correct results
    HBufC* now = MPXDbCommonUtil::CurrentDateDesLC();
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryPodcastBetween,
        now, dayNo, now, 1));
    CleanupClosePushL(recordset);
    DeleteRecordsetL(recordset, aUriArray, aItemChangedMessages);
    CleanupStack::PopAndDestroy(&recordset);
    CleanupStack::PopAndDestroy(now);
    }

// ----------------------------------------------------------------------------
// Remove all podcast episodes from the podcast table that are published last week
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::DeleteLastWeekEpisodesL(
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbPodcast::DeleteLastWeekEpisodesL");

    TInt dayNo(MPXDbUtil::DayNoInWeek());
    dayNo++; // shifted by 1 day to get correct results
    HBufC* now = MPXDbCommonUtil::CurrentDateDesLC();
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryPodcastBetween,
        now, dayNo + 7, now, dayNo));
    CleanupClosePushL(recordset);
    DeleteRecordsetL(recordset, aUriArray, aItemChangedMessages);
    CleanupStack::PopAndDestroy(&recordset);
    CleanupStack::PopAndDestroy(now);
    }

// ----------------------------------------------------------------------------
// Remove all podcast episodes from the podcast table that are published 2 weeks ago
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::Delete2WeeksAgoEpisodesL(
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbPodcast::Delete2WeeksAgoEpisodesL");

    TInt dayNo(MPXDbUtil::DayNoInWeek());
    dayNo++; // shifted by 1 day to get correct results
    HBufC* now = MPXDbCommonUtil::CurrentDateDesLC();
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryPodcastBetween,
        now, dayNo + 14, now, dayNo + 7));
    CleanupClosePushL(recordset);
    DeleteRecordsetL(recordset, aUriArray, aItemChangedMessages);
    CleanupStack::PopAndDestroy(&recordset);
    CleanupStack::PopAndDestroy(now);
    }

// ----------------------------------------------------------------------------
// Remove all podcast episodes from the podcast table that are published 3 weeks ago
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::Delete3WeeksAgoEpisodesL(
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbPodcast::Delete3WeeksAgoEpisodesL");

    TInt dayNo(MPXDbUtil::DayNoInWeek());
    dayNo++; // shifted by 1 day to get correct results
    HBufC* now = MPXDbCommonUtil::CurrentDateDesLC();
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryPodcastBetween,
        now, dayNo + 21, now, dayNo + 14));
    CleanupClosePushL(recordset);
    DeleteRecordsetL(recordset, aUriArray, aItemChangedMessages);
    CleanupStack::PopAndDestroy(&recordset);
    CleanupStack::PopAndDestroy(now);
    }

// ----------------------------------------------------------------------------
// Remove all podcast episodes from the podcast table that are published last month
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::DeleteLastMonthEpisodesL(
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbPodcast::DeleteLastMonthEpisodesL");

    HBufC* now = MPXDbCommonUtil::CurrentDateDesLC();
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryPodcastLastMonth, now, now));
    CleanupClosePushL(recordset);
    DeleteRecordsetL(recordset, aUriArray, aItemChangedMessages);
    CleanupStack::PopAndDestroy(&recordset);
    CleanupStack::PopAndDestroy(now);
    }

// ----------------------------------------------------------------------------
// Remove all podcast episodes from the podcast table that are published earlier
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::DeleteEarlierEpisodesL(
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbPodcast::DeleteEarlierEpisodesL");

    HBufC* now = MPXDbCommonUtil::CurrentDateDesLC();
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryPodcastEarlier, now));
    CleanupClosePushL(recordset);
    DeleteRecordsetL(recordset, aUriArray, aItemChangedMessages);
    CleanupStack::PopAndDestroy(&recordset);
    CleanupStack::PopAndDestroy(now);
    }

// ----------------------------------------------------------------------------
// Remove all podcast episodes from the podcast table that do not have a published date
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::DeleteUnknownEpisodesL(
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbPodcast::DeleteUnknownEpisodesL");

    HBufC* now = MPXDbCommonUtil::CurrentDateDesLC();
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryPodcastUnknown, now));
    CleanupClosePushL(recordset);
    DeleteRecordsetL(recordset, aUriArray, aItemChangedMessages);
    CleanupStack::PopAndDestroy(&recordset);
    CleanupStack::PopAndDestroy(now);
    }

// ----------------------------------------------------------------------------
// Get the episode Id and duration
// ----------------------------------------------------------------------------
//
TInt CMPXDbPodcast::EpisodeDurationL(
    TUint32 aEpisodeId)
    {
    MPX_FUNC("CMPXDbPodcast::EpisodeDurationL");
    return ExecuteSumQueryL(KQueryPodcastEpisodeDuration, aEpisodeId);
    }

// ----------------------------------------------------------------------------
// CMPXDbPodcast::AllEpisodesDurationL
// ----------------------------------------------------------------------------
//
TInt CMPXDbPodcast::AllEpisodesDurationL()
    {
    MPX_FUNC("CMPXDbPodcast::AllEpisodesDurationL");
    return ExecuteSumQueryL(KQueryPodcastDurationAll);
    }

// ----------------------------------------------------------------------------
// CMPXDbPodcast::TitleDurationL
// ----------------------------------------------------------------------------
//
TInt CMPXDbPodcast::TitleDurationL(
    TUint32 aTitleId)
    {
    MPX_FUNC("CMPXDbPodcast::TitleDurationL");
    return ExecuteSumQueryL(KQueryPodcastDurationAlbum, aTitleId);
    }

// ----------------------------------------------------------------------------
// CMPXDbPodcast::NotPlayedDurationL
// ----------------------------------------------------------------------------
//
TInt CMPXDbPodcast::NotPlayedDurationL()
    {
    MPX_FUNC("CMPXDbPodcast::NotPlayedDurationL");
    return ExecuteSumQueryL(KQueryPodcastNotPlayedDuration);
    }

// ----------------------------------------------------------------------------
// CMPXDbPodcast::RecentlyAddedDurationL
// ----------------------------------------------------------------------------
//
TInt CMPXDbPodcast::RecentlyAddedDurationL()
    {
    MPX_FUNC("CMPXDbPodcast::RecentlyAddedDurationL");
    return ExecuteSumQueryL(KQueryPodcastRecentlyAddedDuration);
    }

// ----------------------------------------------------------------------------
// Get the name of the episode matching the given ID
// ----------------------------------------------------------------------------
//
HBufC* CMPXDbPodcast::GetNameL(
    TUint32 aEpisodeId)
    {
    MPX_FUNC("CMPXDbPodcast::GetNameL");

    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryPodcastGetTitle,
        aEpisodeId));
    CleanupClosePushL(recordset);

    if (recordset.Next() != KSqlAtRow)
        {
        User::Leave(KErrNotFound);
        }

    HBufC* name =  recordset.ColumnTextL(KMPXTableDefaultIndex).AllocL();
    CleanupStack::PopAndDestroy(&recordset);

    return name;
    }

// ----------------------------------------------------------------------------
// Get URI of the episode
// ----------------------------------------------------------------------------
//
HBufC* CMPXDbPodcast::GetUriL(
    TUint32 aEpisodeId)
    {
    MPX_FUNC("CMPXDbPodcast::GetUriL");

    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryPodcastGetEpisodeNoCat,
        aEpisodeId));
    CleanupClosePushL(recordset);

    if (recordset.Next() != KSqlAtRow)
        {
        User::Leave(KErrNotFound);
        }

    HBufC* uri = ConstructUriL(recordset);
    CleanupStack::PopAndDestroy(&recordset);

    return uri;
    }

// ----------------------------------------------------------------------------
// Find the drive Ids of corresponding unique Episode Id
// ----------------------------------------------------------------------------
//
TInt CMPXDbPodcast::GetDriveL(
    TUint32 aEpisodeId)
    {
    MPX_FUNC("CMPXDbPodcast::GetDriveL");
    return MPXDbCommonUtil::GetDriveIdMatchVolIdL(iDbManager.Fs(),
        ExecuteIntQueryL(KQueryPodcastVolume, aEpisodeId));
    }

// ----------------------------------------------------------------------------
// CMPXDbPodcast::GetEpisodeL
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::GetEpisodeL(
    TUint32 aEpisodeId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbPodcast::GetEpisodeL");
    ExecuteMediaQueryL(aAttrs, aMedia, ExtraFieldsRequired(aAttrs) ?
        KQueryPodcastGetEpisode() : KQueryPodcastGetEpisodeNoCat(), aEpisodeId);
    }

// ----------------------------------------------------------------------------
// CMPXDbPodcast::GetAllEpisodesL
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::GetAllEpisodesL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbPodcast::GetAllEpisodesL");
    ExecuteMediaQueryL(aAttrs, aMediaArray, ExtraFieldsRequired(aAttrs) ?
        KQueryPodcastGetAllEpisodes() : KQueryPodcastGetAllEpisodesNoCat());
    }

// ----------------------------------------------------------------------------
// CMPXDbPodcast::GetEpisodesForCategoryL
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::GetEpisodesForCategoryL(
    TMPXGeneralCategory aCategory,
    TUint32 aCategoryId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray,
    TBool aOrder )
    {
    MPX_FUNC("CMPXDbPodcast::GetEpisodesForCategoryL");
    TPtrC ptr(MPXDbUtil::PodcastFieldNameForCategoryL(aCategory));

    if( aOrder )
        {
        ExecuteMediaQueryL(aAttrs, aMediaArray, KQueryPodcastCategoryOrdered, ptr, aCategoryId);
        }
    else
        {
        ExecuteMediaQueryL(aAttrs, aMediaArray, KQueryPodcastCategory, ptr, aCategoryId);
        }
    }

// ----------------------------------------------------------------------------
// Retrieve the episode(s) with the specified title
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::GetEpisodesMatchingTitleL(
    TUint aTitleId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbPodcast::GetEpisodesMatchingTitleL");
    ExecuteMediaQueryL(aAttrs, aMediaArray, ExtraFieldsRequired(aAttrs) ?
        KQueryPodcastGetEpisodesForAlbum() : KQueryPodcastGetEpisodesForAlbumNoCat(),
        aTitleId);
    }

// ----------------------------------------------------------------------------
// Get all podcast episodes from the podcast table that have not been played
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::GetNotYetPlayedPlaylistEpisodesL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray,
    TInt& aIndexOfCurrentlyPlayingItem)
    {
    MPX_FUNC("CMPXDbPodcast::GetNotYetPlayedPlaylistEpisodesL");
    ExecuteMediaQueryL(aAttrs, aMediaArray, ExtraFieldsRequired(aAttrs) ?
        KQueryPodcastNotPlayed() : KQueryPodcastNotPlayedNoCat());

    aIndexOfCurrentlyPlayingItem = GetPlayingItemL(aMediaArray);
    }

// ----------------------------------------------------------------------------
// Get all podcast episodes from the podcast table were added within the last 7 days
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::GetRecentlyAddedPlaylistEpisodesL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray,
    TInt& aIndexOfCurrentlyPlayingItem)
    {
    MPX_FUNC("CMPXDbPodcast::GetRecentlyAddedPlaylistEpisodesL");
    ExecuteMediaQueryL(aAttrs, aMediaArray, ExtraFieldsRequired(aAttrs) ?
        KQueryPodcastRecentlyAdded() : KQueryPodcastRecentlyAddedNoCat());

    aIndexOfCurrentlyPlayingItem = GetPlayingItemL(aMediaArray);
    }

// ----------------------------------------------------------------------------
// Get all episodes from the podcast table that are published today
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::GetTodayEpisodesL(
    const TMPXItemId& aPublishDateCategoryId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbPodcast::GetTodayEpisodesL");

    HBufC* now = MPXDbCommonUtil::CurrentDateDesLC();
    ExecuteMediaQueryL(aAttrs, aMediaArray, ExtraFieldsRequired(aAttrs) ?
        KQueryPodcastToday() : KQueryPodcastTodayNoCat(), *now, *now);
    CleanupStack::PopAndDestroy(now);

    ProcessPublishDateCategoryL(aPublishDateCategoryId, aMediaArray);
    }

// ----------------------------------------------------------------------------
// Get all episodes from the podcast table that are published yesterday
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::GetYesterdayEpisodesL(
    const TMPXItemId& aPublishDateCategoryId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbPodcast::GetYesterdayEpisodesL");

    HBufC* now = MPXDbCommonUtil::CurrentDateDesLC();
    ExecuteMediaQueryL(aAttrs, aMediaArray, ExtraFieldsRequired(aAttrs) ?
        KQueryPodcastYesterday() : KQueryPodcastYesterdayNoCat(), *now, *now);
    CleanupStack::PopAndDestroy(now);

    ProcessPublishDateCategoryL(aPublishDateCategoryId, aMediaArray);
    }

// ----------------------------------------------------------------------------
// Get all episodes from the podcast table that are published this weeks
// This doesn't include Today or Yesterday's episodes
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::GetThisWeekEpisodesL(
    const TMPXItemId& aPublishDateCategoryId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbPodcast::GetThisWeekEpisodesL");

    TInt dayNo(MPXDbUtil::DayNoInWeek());
    dayNo++; // shifted by 1 day to get correct results
    HBufC* now = MPXDbCommonUtil::CurrentDateDesLC();
    ExecuteMediaQueryL(aAttrs, aMediaArray, ExtraFieldsRequired(aAttrs) ?
        KQueryPodcastBetween() : KQueryPodcastBetweenNoCat(),
        *now, dayNo, *now, 1);
    CleanupStack::PopAndDestroy(now);

    ProcessPublishDateCategoryL(aPublishDateCategoryId, aMediaArray);
    }

// ----------------------------------------------------------------------------
// Get all episodes from the podcast table that are published last weeks
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::GetLastWeekEpisodesL(
    const TMPXItemId& aPublishDateCategoryId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbPodcast::GetLastWeekEpisodesL");

    TInt dayNo(MPXDbUtil::DayNoInWeek());
    dayNo++; // shifted by 1 day to get correct results
    HBufC* now = MPXDbCommonUtil::CurrentDateDesLC();
    ExecuteMediaQueryL(aAttrs, aMediaArray, ExtraFieldsRequired(aAttrs) ?
        KQueryPodcastBetween() : KQueryPodcastBetweenNoCat(),
        *now, dayNo + 7, *now, dayNo);
    CleanupStack::PopAndDestroy(now);

    ProcessPublishDateCategoryL(aPublishDateCategoryId, aMediaArray);
    }

// ----------------------------------------------------------------------------
// Get all episodes from the podcast table that are published 2 weeks ago
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::Get2WeeksAgoEpisodesL(
    const TMPXItemId& aPublishDateCategoryId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbPodcast::Get2WeeksAgoEpisodesL");

    TInt dayNo(MPXDbUtil::DayNoInWeek());
    dayNo++; // shifted by 1 day to get correct results
    HBufC* now = MPXDbCommonUtil::CurrentDateDesLC();
    ExecuteMediaQueryL(aAttrs, aMediaArray, ExtraFieldsRequired(aAttrs) ?
        KQueryPodcastBetween() : KQueryPodcastBetweenNoCat(),
        *now, dayNo + 14, *now, dayNo + 7);

    CleanupStack::PopAndDestroy(now);
    ProcessPublishDateCategoryL(aPublishDateCategoryId, aMediaArray);
    }

// ----------------------------------------------------------------------------
// Get all episodes from the podcast table that are published 3 weeks ago
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::Get3WeeksAgoEpisodesL(
    const TMPXItemId& aPublishDateCategoryId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbPodcast::Get3WeeksAgoEpisodesL");

    TInt dayNo(MPXDbUtil::DayNoInWeek());
    dayNo++; // shifted by 1 day to get correct results
    HBufC* now = MPXDbCommonUtil::CurrentDateDesLC();
    ExecuteMediaQueryL(aAttrs, aMediaArray, ExtraFieldsRequired(aAttrs) ?
        KQueryPodcastBetween() : KQueryPodcastBetweenNoCat(),
        *now, dayNo + 21, *now, dayNo + 14);
    CleanupStack::PopAndDestroy(now);

    ProcessPublishDateCategoryL(aPublishDateCategoryId, aMediaArray);
    }

// ----------------------------------------------------------------------------
// Get all episodes from the podcast table that are published last month
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::GetLastMonthEpisodesL(
    const TMPXItemId& aPublishDateCategoryId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbPodcast::GetLastMonthEpisodesL");

    HBufC* now = MPXDbCommonUtil::CurrentDateDesLC();
    ExecuteMediaQueryL(aAttrs, aMediaArray, ExtraFieldsRequired(aAttrs) ?
        KQueryPodcastLastMonth() : KQueryPodcastLastMonthNoCat(), *now, *now);
    CleanupStack::PopAndDestroy(now);

    ProcessPublishDateCategoryL(aPublishDateCategoryId, aMediaArray);
    }

// ----------------------------------------------------------------------------
// Get all episodes from the podcast table that are published earlier
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::GetEarlierEpisodesL(
    const TMPXItemId& aPublishDateCategoryId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbPodcast::GetEarlierEpisodesL");

    HBufC* now = MPXDbCommonUtil::CurrentDateDesLC();
    ExecuteMediaQueryL(aAttrs, aMediaArray, ExtraFieldsRequired(aAttrs) ?
        KQueryPodcastEarlier() : KQueryPodcastEarlierNoCat(), *now);
    CleanupStack::PopAndDestroy(now);

    ProcessPublishDateCategoryL(aPublishDateCategoryId, aMediaArray);
    }

// ----------------------------------------------------------------------------
// Get all episodes from the podcast table that are published on unknown date
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::GetUnknownEpisodesL(
    const TMPXItemId& aPublishDateCategoryId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbPodcast::GetUnknownEpisodesL");

    HBufC* now = MPXDbCommonUtil::CurrentDateDesLC();
    ExecuteMediaQueryL(aAttrs, aMediaArray, ExtraFieldsRequired(aAttrs) ?
        KQueryPodcastUnknown() : KQueryPodcastUnknownNoCat(), *now);
    CleanupStack::PopAndDestroy(now);

    ProcessPublishDateCategoryL(aPublishDateCategoryId, aMediaArray);
    }

// ----------------------------------------------------------------------------
// Count the total number of items
// ----------------------------------------------------------------------------
//
TInt CMPXDbPodcast::CountL()
    {
    MPX_FUNC("CMPXDbPodcast::CountL");
    return ExecuteSumQueryL(KQueryPodcastCount);
    }

// ----------------------------------------------------------------------------
// Get number of episodes from the podcast table that are published today
// ----------------------------------------------------------------------------
//
TInt CMPXDbPodcast::GetTodayEpisodesCountL()
    {
    MPX_FUNC("CMPXDbPodcast::GetTodayEpisodesCountL");

    HBufC* now = MPXDbCommonUtil::CurrentDateDesLC();
    TInt count = ExecuteSumQueryL(KQueryPodcastTodayCount, *now, *now);
    CleanupStack::PopAndDestroy(now);

    return count;
    }

// ----------------------------------------------------------------------------
// Get number of episodes from the podcast table that are published yesterday
// ----------------------------------------------------------------------------
//
TInt CMPXDbPodcast::GetYesterdayEpisodesCountL()
    {
    MPX_FUNC("CMPXDbPodcast::GetYesterdayEpisodesCountL");

    HBufC* now = MPXDbCommonUtil::CurrentDateDesLC();
    TInt count = ExecuteSumQueryL(KQueryPodcastYesterdayCount, *now, *now);
    CleanupStack::PopAndDestroy(now);

    return count;
    }

// ----------------------------------------------------------------------------
// Get number of episodes from the podcast table that are published this weeks
// This doesn't include Today or Yesterday's episodes
// ----------------------------------------------------------------------------
//
TInt CMPXDbPodcast::GetThisWeekEpisodesCountL()
    {
    MPX_FUNC("CMPXDbPodcast::GetThisWeekEpisodesCountL");

    TInt dayNo(MPXDbUtil::DayNoInWeek());
    dayNo++; // shifted by 1 day to get correct results

    HBufC* now = MPXDbCommonUtil::CurrentDateDesLC();
    TInt count = ExecuteSumQueryL(KQueryPodcastBetweenCount, *now, dayNo, *now, 1);
    CleanupStack::PopAndDestroy(now);

    return count;
    }

// ----------------------------------------------------------------------------
// Get number of episodes from the podcast table that are published last weeks
// ----------------------------------------------------------------------------
//
TInt CMPXDbPodcast::GetLastWeekEpisodesCountL()
    {
    MPX_FUNC("CMPXDbPodcast::GetLastWeekEpisodesCountL");

    TInt dayNo(MPXDbUtil::DayNoInWeek());
    dayNo++; // shifted by 1 day to get correct results

    HBufC* now = MPXDbCommonUtil::CurrentDateDesLC();
    TInt count = ExecuteSumQueryL(KQueryPodcastBetweenCount, *now, dayNo + 7, *now, dayNo);
    CleanupStack::PopAndDestroy(now);

    return count;
    }

// ----------------------------------------------------------------------------
// Get number of episodes from the podcast table that are published 2 weeks ago
// ----------------------------------------------------------------------------
//
TInt CMPXDbPodcast::Get2WeeksAgoEpisodesCountL()
    {
    MPX_FUNC("CMPXDbPodcast::Get2WeeksAgoEpisodesCountL");

    TInt dayNo(MPXDbUtil::DayNoInWeek());
    dayNo++; // shifted by 1 day to get correct results

    HBufC* now = MPXDbCommonUtil::CurrentDateDesLC();
    TInt count = ExecuteSumQueryL(KQueryPodcastBetweenCount, *now, dayNo + 14, *now, dayNo + 7);
    CleanupStack::PopAndDestroy(now);

    return count;
    }

// ----------------------------------------------------------------------------
// Get number of episodes from the podcast table that are published 3 weeks ago
// ----------------------------------------------------------------------------
//
TInt CMPXDbPodcast::Get3WeeksAgoEpisodesCountL()
    {
    MPX_FUNC("CMPXDbPodcast::Get3WeeksAgoEpisodesCountL");

    TInt dayNo(MPXDbUtil::DayNoInWeek());
    dayNo++; // shifted by 1 day to get correct results

    HBufC* now = MPXDbCommonUtil::CurrentDateDesLC();
    TInt count = ExecuteSumQueryL(KQueryPodcastBetweenCount, *now, dayNo + 21, *now, dayNo + 14);
    CleanupStack::PopAndDestroy(now);

    return count;
    }

// ----------------------------------------------------------------------------
// Get number of episodes from the podcast table that are published last month
// ----------------------------------------------------------------------------
//
TInt CMPXDbPodcast::GetLastMonthEpisodesCountL()
    {
    MPX_FUNC("CMPXDbPodcast::GetLastMonthEpisodesCountL");

    HBufC* now = MPXDbCommonUtil::CurrentDateDesLC();
    TInt count = ExecuteSumQueryL(KQueryPodcastLastMonthCount, *now, *now);
    CleanupStack::PopAndDestroy(now);

    return count;
    }

// ----------------------------------------------------------------------------
// Get number of episodes from the podcast table that are published earlier
// ----------------------------------------------------------------------------
//
TInt CMPXDbPodcast::GetEarlierEpisodesCountL()
    {
    MPX_FUNC("CMPXDbPodcast::GetEarlierEpisodesCountL");

    HBufC* now = MPXDbCommonUtil::CurrentDateDesLC();
    TInt count = ExecuteSumQueryL(KQueryPodcastEarlierCount, *now);
    CleanupStack::PopAndDestroy(now);

    return count;
    }

// ----------------------------------------------------------------------------
// Get number of episodes from the podcast table that are published on unknown date
// ----------------------------------------------------------------------------
//
TInt CMPXDbPodcast::GetUnknownEpisodesCountL()
    {
    MPX_FUNC("CMPXDbPodcast::GetUnknownEpisodesCountL");

    HBufC* now = MPXDbCommonUtil::CurrentDateDesLC();
    TInt count = ExecuteSumQueryL(KQueryPodcastUnknownCount, *now);
    CleanupStack::PopAndDestroy(now);

    return count;
    }

// ----------------------------------------------------------------------------
// Count the number of episodes recently added
// ----------------------------------------------------------------------------
//
TInt CMPXDbPodcast::GetRecentlyAddedEpisodesCountL()
    {
    MPX_FUNC("CMPXDbPodcast::GetRecentlyAddedEpisodesCountL");
    return ExecuteSumQueryL(KQueryPodcastRecentlyAddedCount);
    }

// ----------------------------------------------------------------------------
// Get count of all podcast episodes from the Podcast table that have not been played
// ----------------------------------------------------------------------------
//
TInt CMPXDbPodcast::GetNotYetPlayedEpisodesCountL()
    {
    MPX_FUNC("CMPXDbPodcast::GetNotYetPlayedPlaylistEpisodes");
    return ExecuteSumQueryL(KQueryPodcastNotPlayedCount);
    }

// ----------------------------------------------------------------------------
// Get episode(s) from the podcast table that match the given criteria
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::FindEpisodesL(
    TUint32 aGeneralId,
    TUint32 aContainerId,
    TMPXGeneralType aType,
    TMPXPodcastType aPodcastType,
    const CMPXMedia& aCriteria,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbPodcast::FindEpisodesL");

    // set up the selection criteria
    HBufC* criteriaStr = GeneratePodcastMatchingCriteriaLC(aGeneralId, aContainerId,
        aType, aPodcastType, aCriteria);

    // iterate the results and append media objects to the destination array
    ExecuteMediaQueryL(aAttrs, aMediaArray, KQueryPodcastFindAll, *criteriaStr);

    CleanupStack::PopAndDestroy(criteriaStr);
    }

// ----------------------------------------------------------------------------
// CMPXDbPodcast::SetIsPlayingL
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::SetIsPlayingL(
    TUint32 aEpisodeId,
    TBool aIsPlaying)
    {
    MPX_FUNC("CMPXDbPodcast::SetIsPlayingL");
    TInt drive(GetDriveL(aEpisodeId));
    iDbManager.ExecuteQueryL(drive, KQueryPodcastSetIsPlaying, aIsPlaying, aEpisodeId);
    }

// ----------------------------------------------------------------------------
// CMPXDbPodcast::GetDrivePodcastCountL
// ----------------------------------------------------------------------------
//
TUint CMPXDbPodcast::GetDrivePodcastCountL(TInt aDrive)
    {
    TUint count(0);

    //podcast
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(aDrive,KQueryPodcastCount));
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
// CMPXDbPodcast::GetPodcastUriArrayL
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::GetPodcastUriArrayL(TInt aDrive, TInt aFromID, TInt aRecords,
                                        CDesCArray& aUriArr, TInt& aLastID)
    {
    MPX_FUNC("CMPXDbPodcast::GetPodcastUriArrayL");

    HBufC* query = NULL;
    if(aFromID == 0)
        {
        query = HBufC::NewLC(KQueryPodcastGetUris().Length()
                            + KMCIntegerLen);
        query->Des().Format(KQueryPodcastGetUris, aRecords);
        }
    else
        {
        query = HBufC::NewLC(KQueryPodcastGetUrisFrom().Length()
                            + 2*KMCIntegerLen);
        query->Des().Format(KQueryPodcastGetUrisFrom, aFromID, aRecords);
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
// CMPXDbPodcast::DoAddEpisodeL
// ----------------------------------------------------------------------------
//
TBool CMPXDbPodcast::DoAddEpisodeL(
    TUint32 aEpisodeId,
    const CMPXMedia& aMedia,
    TInt aDrive,
    CMPXMessageArray* aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbPodcast::DoAddEpisodeL");

    CDesCArrayFlat* fields = new (ELeave) CDesCArrayFlat(EPodcastFieldCount);
    CleanupStack::PushL(fields);
    CDesCArrayFlat* values = new (ELeave) CDesCArrayFlat(EPodcastFieldCount);
    CleanupStack::PushL(values);

    // add known fields
    MPXDbCommonUtil::AppendValueL(*fields, *values, KMCPodcastUniqueId, aEpisodeId);
    MPXDbCommonUtil::AppendValueL(*fields, *values, KMCPodcastDel, 0);

    HBufC* date = MPXDbCommonUtil::CurrentDateDesLC();
    MPXDbCommonUtil::AppendValueL(*fields, *values, KMCPodcastTimeAdded, *date);
    CleanupStack::PopAndDestroy(date);

    // process the media parameter and construct the fields and values array
    TBool visible(GeneratePodcastFieldsValuesL(aEpisodeId, aMedia, aItemChangedMessages,
        NULL, *fields, *values, aDrive));

    // create the fields and values strings
    HBufC* fieldStr = MPXDbCommonUtil::StringFromArrayLC(*fields, KMCCommaSign);
    HBufC* valueStr = MPXDbCommonUtil::StringFromArrayLC(*values, KMCCommaSign);

    // execute the query
    iDbManager.ExecuteQueryL(aDrive, KQueryPodcastInsert, fieldStr, valueStr);

    CleanupStack::PopAndDestroy(valueStr);
    CleanupStack::PopAndDestroy(fieldStr);
    CleanupStack::PopAndDestroy(values);
    CleanupStack::PopAndDestroy(fields);

    return visible;
    }

// ----------------------------------------------------------------------------
// CMPXDbPodcast::DoUpdateEpisodeL
// ----------------------------------------------------------------------------
//
CMPXDbActiveTask::TChangeVisibility CMPXDbPodcast::DoUpdateEpisodeL(
    TUint32 aEpisodeId,
    const CMPXMedia& aMedia,
    CMPXMessageArray* aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbPodcast::DoUpdateEpisodeL");

    // retrieve the existing record
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryPodcastGetEpisode, aEpisodeId));
    CleanupClosePushL(recordset);

    if (recordset.Next() != KSqlAtRow)
        {
        User::Leave(KErrNotFound);
        }

    TDriveUnit driveUnit(MPXDbCommonUtil::GetDriveIdMatchVolIdL(iDbManager.Fs(),
        recordset.ColumnInt64(EPodcastVolumeId)));
    CMPXDbActiveTask::TChangeVisibility visible(DoUpdateEpisodeL(aEpisodeId, aMedia, driveUnit, aItemChangedMessages,
        recordset));

    CleanupStack::PopAndDestroy(&recordset);

    return visible;
    }

// ----------------------------------------------------------------------------
// CMPXDbPodcast::DoUpdateEpisodeL
// ----------------------------------------------------------------------------
//
CMPXDbActiveTask::TChangeVisibility CMPXDbPodcast::DoUpdateEpisodeL(
    TUint32 aEpisodeId,
    const CMPXMedia& aMedia,
    TInt aDrive,
    CMPXMessageArray* aItemChangedMessages,
    RSqlStatement& aRecordset)
    {
    MPX_FUNC("CMPXDbPodcast::DoUpdateEpisodeL");

    CDesCArrayFlat* fields = new (ELeave) CDesCArrayFlat(EPodcastFieldCount);
    CleanupStack::PushL(fields);
    CDesCArrayFlat* values = new (ELeave) CDesCArrayFlat(EPodcastFieldCount);
    CleanupStack::PushL(values);

    // process the media parameter and construct the fields and values array
    CMPXDbActiveTask::TChangeVisibility visible(GeneratePodcastFieldsValuesL(aEpisodeId, aMedia, aItemChangedMessages,
        &aRecordset, *fields, *values, aDrive));

    // construct the SET string
    HBufC* setStr = MPXDbCommonUtil::StringFromArraysLC(*fields, *values,
        KMCEqualSign, KMCCommaSign);
    if (setStr->Length())
        {
        // execute the query
        iDbManager.ExecuteQueryL(aDrive, KQueryPodcastUpdate, setStr, aEpisodeId);
        }

    CleanupStack::PopAndDestroy(setStr);
    CleanupStack::PopAndDestroy(values);
    CleanupStack::PopAndDestroy(fields);

    return visible;
    }

// ----------------------------------------------------------------------------
// CMPXDbPodcast::DeleteRecordsetL
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::DeleteRecordsetL(
    RSqlStatement& aRecordset,
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages,
    TBool aDeleteRecord /* = EFalse */)
    {
    MPX_FUNC("CMPXDbPodcast::DeleteRecordsetL");

    TInt err(KErrNone);
    while ((err = aRecordset.Next()) == KSqlAtRow)
        {
        DoDeleteEpisodeL(aRecordset, aUriArray, aItemChangedMessages, aDeleteRecord);
        }

    if (err!= KSqlAtEnd)
        {
        User::Leave(KErrCorrupt);
        }
    }

// ----------------------------------------------------------------------------
// Remove a episode from the podcast table
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::DoDeleteEpisodeL(
    RSqlStatement& aRecordset,
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages, TBool IsAdd ,
    TBool aDeleteRecord)
    {
    MPX_FUNC("CMPXDbPodcast::DoDeleteEpisodeL");

    // add the full path to the URI array
    HBufC* uri = ConstructUriL(aRecordset);
    CleanupStack::PushL(uri);
    aUriArray.AppendL(*uri);
    TDriveUnit driveUnit(*uri);
    CleanupStack::PopAndDestroy(uri);
   
    //if adding then dont delete episode category 
    if(!IsAdd)
        {
        // process the author
        iObserver.DeleteEpisodeForCategoryL(EMPXArtist, aRecordset.ColumnInt64(EPodcastArtist),
            driveUnit, &aItemChangedMessages );
    
        // process the title
        iObserver.DeleteEpisodeForCategoryL(EMPXAlbum, aRecordset.ColumnInt64(EPodcastAlbum),
            driveUnit, &aItemChangedMessages);
    
        // process the genre
        iObserver.DeleteEpisodeForCategoryL(EMPXGenre, aRecordset.ColumnInt64(EPodcastGenre),
            driveUnit, &aItemChangedMessages);
    
        // process the composer
        iObserver.DeleteEpisodeForCategoryL(EMPXComposer, aRecordset.ColumnInt64(EPodcastComposer),
            driveUnit, &aItemChangedMessages);
        }
  


    // add a change event for removing the episode itself
    TUint32 episodeId(aRecordset.ColumnInt64(EPodcastUniqueId));
    MPXDbCommonUtil::AddItemChangedMessageL(aItemChangedMessages, episodeId,
        EMPXItemDeleted, EMPXPodcast, KDBPluginUid);

    // delete or update the episode
    iDbManager.ExecuteQueryL(driveUnit, aDeleteRecord ? KQueryPodcastDelete() :
        KQueryPodcastDeleteUpdate(), episodeId);

    MPX_DEBUG2(_L("CMPXDbPodcast::RemoveEpisodeL Removed episodeId[0x%x]"), episodeId);
    }

// ----------------------------------------------------------------------------
// CMPXDbPodcast::GeneratePodcastFieldsValuesL
// ----------------------------------------------------------------------------
//
CMPXDbActiveTask::TChangeVisibility CMPXDbPodcast::GeneratePodcastFieldsValuesL(
    TUint32 aEpisodeId,
    const CMPXMedia& aMedia,
    CMPXMessageArray* aItemChangedMessages,
    RSqlStatement* aPodcastTable,
    CDesCArray& aFields,
    CDesCArray& aValues,
    TInt aDrive)
    {
    MPX_FUNC("CMPXDbMusic::GeneratePodcastFieldsValuesL");

    CMPXDbActiveTask::TChangeVisibility visibleChange(CMPXDbActiveTask::ENotVisibile);
    TBool metaDataModified(EFalse);
    const TArray<TMPXAttribute> attributes = aMedia.Attributes();

    CMPXMessage* episodeChangedMessage(NULL);
    if (aItemChangedMessages)
        {
        episodeChangedMessage = CMPXMedia::NewL();
        CleanupStack::PushL(episodeChangedMessage);
        MPXDbCommonUtil::FillItemChangedMessageL(*episodeChangedMessage, aEpisodeId,
            aPodcastTable ? EMPXItemModified : EMPXItemInserted, EMPXPodcast, KDBPluginUid);
        }

    TInt attrCount(attributes.Count());
    for (TInt i = 0; i < attrCount; ++i)
        {
        TInt contentId(attributes[i].ContentId());
        TUint attributeId(attributes[i].AttributeId());

        switch (contentId)
            {
            case KMPXMediaIdGeneral:
                {
                if (attributeId & EMPXMediaGeneralTitle)
                    {
                    TBool titleChanged(NULL == aPodcastTable);

                    const TDesC& title = aMedia.ValueText(KMPXMediaGeneralTitle);
                    TPtrC truncatedTitle(title.Left(KMCMaxTextLen));
                    if (aPodcastTable)
                        {
                        // title of the song has been changed
                        if (truncatedTitle.Compare(aPodcastTable->ColumnTextL(EPodcastTitle)) != 0)
                            {
                            titleChanged = ETrue;
                            }
                        }

                    if (titleChanged)
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastName, truncatedTitle);
                        visibleChange = CMPXDbActiveTask::EAllVisible;
                        metaDataModified = ETrue;

                        MPX_DEBUG2("    Title[%S]", &truncatedTitle);
                        }
                    }

                if (attributeId & EMPXMediaGeneralUri)
                    {
                    const TDesC& uri = aMedia.ValueText(KMPXMediaGeneralUri);
                    const TDesC& uriTrunc(uri.Mid(KMCPathStartPos));

                    TDriveUnit driveUnit(uri);
                    TUint volId(MPXDbCommonUtil::GetVolIdMatchDriveIdL(iDbManager.Fs(), driveUnit));

                    if (!aPodcastTable || ((uriTrunc != aPodcastTable->ColumnTextL(EPodcastLocation)) ||
                        (volId != aPodcastTable->ColumnInt64(EPodcastVolumeId))))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastVolumeId, volId);
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastLocation, uri.Mid(KMCPathStartPos));
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastDRM, DRMTypeL(uri));

                        const TDesC& mimeTypeText = MPXDbCommonUtil::GetMimeTypeForUriL(uri).Des();
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastMimeType, mimeTypeText);

                        MPX_DEBUG3("    VolumeId[%u] Location[%S]", volId, &uri);
                        MPX_DEBUG2("    MimeType[%S]", &mimeTypeText);

                        if (!aPodcastTable && !aMedia.IsSupported(KMPXMediaGeneralTitle))
                            {
                            TParsePtrC parser(uri);
                            TPtrC title = parser.Name();
                            // use file name as song name
                            MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastName, title.Left(KMCMaxTextLen));
                            visibleChange = CMPXDbActiveTask::EAllVisible;

                            MPX_DEBUG2("    Title[%S]", &title);
                            }

                        // URI of the song has been changed. This changes the Id of the song
                        if (aPodcastTable)
                            {
                            TUint32 newEpisodeId = MPXDbCommonUtil::GenerateUniqueIdL(iDbManager.Fs(), EMPXCollection, uri, EFalse);
                            if (aEpisodeId != newEpisodeId)
                                {
                                MPX_DEBUG3("    CurrentEpisodeId[0x%x] changed to [0x%x]", aEpisodeId, newEpisodeId);

                                if (episodeChangedMessage)
                                    {
                                    episodeChangedMessage->SetTObjectValueL<TMPXItemId>(KMPXMessageMediaGeneralId, newEpisodeId);
                                    episodeChangedMessage->SetTObjectValueL<TMPXItemId>(KMPXMessageMediaDeprecatedId, aEpisodeId);
                                    }

                                MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastUniqueId, newEpisodeId);
                                }
                            }
                        }
                    }

                if (attributeId & EMPXMediaGeneralComment)
                    {
                    const TDesC& comment = aMedia.ValueText(KMPXMediaGeneralComment).Left(KMCMaxTextLen);

                    if (!aPodcastTable || (comment != aPodcastTable->ColumnTextL(EPodcastComment)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastComment, comment);
                        metaDataModified = ETrue;
                        }
                    MPX_DEBUG2("    Comment[%S]", &comment);
                    }

                if (attributeId & EMPXMediaGeneralSynchronized)
                    {
                    TBool synced(aMedia.ValueTObjectL<TBool>(KMPXMediaGeneralSynchronized));
                    if (!aPodcastTable || (synced != aPodcastTable->ColumnInt(EPodcastSync)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastSync, synced);
                        }
                    MPX_DEBUG2("    Synchronized[%d]", synced);
                    }

                if (attributeId & EMPXMediaGeneralDeleted)
                    {
                    TBool deleted(aMedia.ValueTObjectL<TBool>(KMPXMediaGeneralDeleted));
                    if (!aPodcastTable || (deleted != aPodcastTable->ColumnInt(EPodcastDeleted)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastDel, deleted);
                        }
                    MPX_DEBUG2("    Deleted[%d]", deleted);
                    }

                if (attributeId & EMPXMediaGeneralModified)
                    {
                    TBool modified(aMedia.ValueTObjectL<TBool>(KMPXMediaGeneralModified));
                    if (!aPodcastTable || (modified != aPodcastTable->ColumnInt(EPodcastModified)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastMod, modified);
                        }
                    MPX_DEBUG2("    Modified[%d]", modified);
                    }

                if (attributeId & EMPXMediaGeneralCopyright)
                    {
                    const TDesC& copyright = aMedia.ValueText(KMPXMediaGeneralCopyright).Left(KMCMaxTextLen);
                    if (!aPodcastTable || (copyright != aPodcastTable->ColumnTextL(EPodcastCopyright)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastCopyright, copyright);
                        metaDataModified = ETrue;
                        }
                    MPX_DEBUG2("    Copyright[%S]", &copyright);
                    }

                if (attributeId & EMPXMediaGeneralDuration)
                    {
                    TInt duration(aMedia.ValueTObjectL<TInt>(KMPXMediaGeneralDuration));
                    if (!aPodcastTable || (duration != aPodcastTable->ColumnInt(EPodcastDuration)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastDuration, duration);
                        visibleChange = CMPXDbActiveTask::EAllVisible;
                        metaDataModified = ETrue;
                        }
                    MPX_DEBUG2("    Duration[%d]", duration);
                    }

                if (attributeId & EMPXMediaGeneralFlags)
                    {
                    TUint flag(aMedia.ValueTObjectL<TUint>(KMPXMediaGeneralFlags));
                    TUint32 curFlag(0);
                    if (aPodcastTable)
                        {
                        curFlag = aPodcastTable->ColumnInt64(EPodcastDbFlag);
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
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastDbFlag, curFlag);
                        visibleChange = CMPXDbActiveTask::EAllVisible;
                        }
                     MPX_DEBUG2("    GeneralFlags[%b]", curFlag);
                    }

                if (attributeId & EMPXMediaGeneralPlayCount)
                    {
                    TInt increment(aMedia.ValueTObjectL<TInt>(KMPXMediaGeneralPlayCount));
                    TUint32 curCount(increment);

                    // Only add to the current number if the value received is positive
                    // otherwise reset
                    if (curCount && aPodcastTable)
                        {
                        curCount += aPodcastTable->ColumnInt(EPodcastPlayCount);
                        }

                    if (!aPodcastTable || (curCount != aPodcastTable->ColumnInt(EPodcastPlayCount)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastPlayCount, curCount);
                        visibleChange = CMPXDbActiveTask::EAllVisible;
                        }
                    MPX_DEBUG2("    PlayCount[%d]", curCount);
                    }

                if (attributeId & EMPXMediaGeneralLastPlaybackPosition)
                    {
                    TInt32 lastPlayPosition(aMedia.ValueTObjectL<TInt32>(KMPXMediaGeneralLastPlaybackPosition));
                    MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastIsPlaying, EFalse);

                    if (!aPodcastTable || (lastPlayPosition != aPodcastTable->ColumnInt(EPodcastLastPlayPosition)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastLastPlayPosition, lastPlayPosition);
                        visibleChange = CMPXDbActiveTask::EAllVisible;
                        }
                    MPX_DEBUG2("    Last Playback Position[%d]", lastPlayPosition);
                    }

                if (attributeId & EMPXMediaGeneralLastPlaybackTime)
                    {
                    HBufC* time = MPXDbCommonUtil::TTimeToDesLC(
                        TTime(aMedia.ValueTObjectL<TInt64>(KMPXMediaGeneralLastPlaybackTime)));
                    if (!aPodcastTable || (*time != aPodcastTable->ColumnTextL(EPodcastTimePlayed)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastTimePlayed, *time);
                        }
                    MPX_DEBUG2("    PlaybackTime[%S]", time);
                    CleanupStack::PopAndDestroy(time);
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
                        if (stringParser.Val(track) != KErrNone)
                            {
                            track = KMaxTInt;
                            }
                        }

                    if (!aPodcastTable || (track != aPodcastTable->ColumnInt(EPodcastAlbumTrack)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastTrackNumber, track);
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
                    if (!aPodcastTable || (*timeStr != aPodcastTable->ColumnTextL(EPodcastReleaseDate)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastReleaseDate, *timeStr);
                        metaDataModified = ETrue;
                        }
                    MPX_DEBUG2("    Music Year[%S]", timeStr);
                    CleanupStack::PopAndDestroy(timeStr);
                    }

                if (attributeId & EMPXMediaMusicRating)
                    {
                    TInt rating(aMedia.ValueTObjectL<TInt>(KMPXMediaMusicRating));

                    if (!aPodcastTable || (rating != aPodcastTable->ColumnInt(EPodcastRating)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastRating, rating);
                        metaDataModified = ETrue;
                        }
                    MPX_DEBUG2("    Rating[%d]", rating);
                    }

                if (attributeId & EMPXMediaMusicAlbumArtFileName)
                    {
                    const TDesC& albumArtFilename(aMedia.ValueText(KMPXMediaMusicAlbumArtFileName).Left(KMCMaxTextLen));
                    if (!aPodcastTable || (albumArtFilename != aPodcastTable->ColumnTextL(EPodcastArt)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastArt, albumArtFilename);
                        metaDataModified = ETrue;
                        }
                    MPX_DEBUG2("    Album Art Filename[%S]", &albumArtFilename);
                    }

                if (attributeId & EMPXMediaMusicURL)
                    {
                    const TDesC& url(aMedia.ValueText(KMPXMediaMusicURL).Left(KMCMaxTextLen));
                    if (!aPodcastTable || (url != aPodcastTable->ColumnTextL(EPodcastUrl)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastUrl, url);
                        metaDataModified = ETrue;
                        }
                    MPX_DEBUG2("    Music URL[%S]", &url);
                    }
                }
                break;

            case KMPXMediaIdAudio:
                {
                if (attributeId & EMPXMediaAudioSamplerate)
                    {
                    TInt samplerate(aMedia.ValueTObjectL<TInt>(KMPXMediaAudioSamplerate));
                    if (!aPodcastTable || (samplerate != aPodcastTable->ColumnInt(EPodcastSampleRate)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastSampleRate, samplerate);
                        metaDataModified = ETrue;
                        }
                    MPX_DEBUG2("    Sample Rate[%d]", samplerate);
                    }

                if (attributeId & EMPXMediaAudioBitrate)
                    {
                    TInt bitrate(aMedia.ValueTObjectL<TInt>(KMPXMediaAudioBitrate));
                    if (!aPodcastTable || (bitrate != aPodcastTable->ColumnInt(EPodcastBitRate)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastBitRate, bitrate);
                        metaDataModified = ETrue;
                        }
                    MPX_DEBUG2("    Bitrate[%d]", bitrate);
                    }

                if (attributeId & EMPXMediaAudioNumberOfChannels)
                    {
                    TUint32 val = aMedia.ValueTObjectL<TUint32>(KMPXMediaAudioNumberOfChannels);
                    if (!aPodcastTable || (val != aPodcastTable->ColumnInt(EPodcastNumChannels)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastNumChannels, val);
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
                    if (!aPodcastTable || (drmType != aPodcastTable->ColumnInt(EPodcastDRM)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastDRM, drmType);
                        }
                    MPX_DEBUG2("    DRM type[%d]", drmType);
                    }

                if (attributeId & KMPXMediaDrmRightsStatus.iAttributeId)
                    {
                    if (aPodcastTable)
                        {
                        TMPXMediaDrmRightsStatus status =
                                 aMedia.ValueTObjectL<TMPXMediaDrmRightsStatus>(KMPXMediaDrmRightsStatus);

                        //.Set the db flag
                        TUint32 curFlag(aPodcastTable->ColumnInt64(EPodcastDbFlag));

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
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastDbFlag, curFlag);
                        MPX_DEBUG2("    Rights Status[%d]", curFlag);
                        }
                    }

                break;
                }

            case KMPXMediaIdPodcast:
                {
                if (attributeId & EMPXMediaPodcastPubDate)
                    {
                    HBufC* time = MPXDbCommonUtil::TTimeToDesLC(
                        TTime(aMedia.ValueTObjectL<TInt64>(KMPXMediaPodcastPubDate)));
                    if (!aPodcastTable || (*time != aPodcastTable->ColumnTextL(EPodcastPublishDate)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastPublishDate, *time);
                        }
                    MPX_DEBUG2("    PublishDate[%S]", time);
                    CleanupStack::PopAndDestroy(time);
                    }

                if (attributeId & EMPXMediaPodcastIsPlaying)
                    {
                    TInt isPlaying = aMedia.ValueTObjectL<TBool>(KMPXMediaPodcastIsPlaying);
                    if (!aPodcastTable || (isPlaying != aPodcastTable->ColumnInt(EPodcastIsPlaying)))
                        {
                        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastIsPlaying, isPlaying);
                        }
                    MPX_DEBUG2("    IsPlaying[%d]", isPlaying);
                    }

                break;
                }

            default:
                break;
            } // end switch
        } // end for

    // get the current author/album/genre/composer
    // this is required because the recordset may be reused by the code below
    TUint32 artistId(0);
    TUint32 albumId(0);
    TUint32 genreId(0);
    TUint32 composerId(0);
    if (aPodcastTable)
        {
        artistId = aPodcastTable->ColumnInt64(EPodcastArtist);
        albumId = aPodcastTable->ColumnInt64(EPodcastAlbum);
        genreId = aPodcastTable->ColumnInt64(EPodcastGenre);
        composerId = aPodcastTable->ColumnInt64(EPodcastComposer);
        }

    // update the artist field
    TUint32 id(0);
    if (UpdateCategoryFieldL(EMPXArtist, aMedia, KMPXMediaMusicArtist, artistId,
        aDrive, aItemChangedMessages, id))
        {
        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastAuthor, id);
        metaDataModified = (aPodcastTable != NULL);
        visibleChange = CMPXDbActiveTask::EAllVisible;
        }

    // update the album field
    if (UpdateCategoryFieldL(EMPXAlbum, aMedia, KMPXMediaMusicAlbum, albumId,
        aDrive, aItemChangedMessages, id))
        {
        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastTitle, id);
        metaDataModified = (aPodcastTable != NULL);
        visibleChange = CMPXDbActiveTask::EAllVisible;
        }

    // update the genre field
    if (UpdateCategoryFieldL(EMPXGenre, aMedia, KMPXMediaMusicGenre, genreId,
        aDrive, aItemChangedMessages, id))
        {
        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastGenre, id);
        metaDataModified = (aPodcastTable != NULL);
        visibleChange = CMPXDbActiveTask::EAllVisible;
        }

    // update the composer field
    if (UpdateCategoryFieldL(EMPXComposer, aMedia, KMPXMediaMusicComposer, composerId,
        aDrive, aItemChangedMessages, id))
        {
        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastComposer, id);
        metaDataModified = (aPodcastTable != NULL);
        visibleChange = CMPXDbActiveTask::EAllVisible;
        }

#if defined (__MTP_PROTOCOL_SUPPORT)
    // Set Mod bit to ETrue if metadata has been updated and caller hasn't explicitly
    // set/reset it
    if (aPodcastTable &&
        !aMedia.IsSupported(KMPXMediaGeneralModified) &&
        metaDataModified)
        {
        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCPodcastMod, 1);
        MPX_DEBUG1("    Modified[1]");
        }
#endif

    if (aItemChangedMessages)
        {
        if (aFields.Count())
        	{
        	aItemChangedMessages->AppendL(*episodeChangedMessage);
        	}
        CleanupStack::PopAndDestroy(episodeChangedMessage);
        }

    return visibleChange;
    }

// ----------------------------------------------------------------------------
// CMPXDbMusic::UpdateCategoryFieldL
// ----------------------------------------------------------------------------
//
TBool CMPXDbPodcast::UpdateCategoryFieldL(
    TMPXGeneralCategory aCategory,
    const CMPXMedia& aMedia,
    const TMPXAttribute& aAttribute,
    TUint32 aOldId,
    TInt aDriveId,
    CMPXMessageArray* aItemChangedMessages,
    TUint32& aItemId)
    {
    MPX_FUNC("CMPXDbPodcast::UpdateCategoryFieldL");

    TBool updated(EFalse);

    // update category table and add category Id to the podcast table
    if (!aOldId || aMedia.IsSupported(aAttribute))
        {
        if (aMedia.IsSupported(aAttribute))
            {
            HBufC* name = aMedia.ValueText(aAttribute).AllocLC();
            name->Des().Trim();

            // only genre is not case sensitive
            aItemId = MPXDbCommonUtil::GenerateUniqueIdL(iDbManager.Fs(), aCategory, name->Left(KMCMaxTextLen),
                (aCategory != EMPXGenre));
            if (!aOldId || (aOldId != aItemId))
                {
                // only add if the ID changed,
                // otherwise the song was updated but the artist name was not

                // ignore the return value
                iObserver.AddCategoryItemL(aCategory, name->Left(KMCMaxTextLen), aDriveId,
                    aItemChangedMessages);
                updated = ETrue;
                }

            CleanupStack::PopAndDestroy(name);
            }
        else
            {
            // only genre is not case sensitive
            aItemId = MPXDbCommonUtil::GenerateUniqueIdL(iDbManager.Fs(), aCategory, KNullDesC,
                (aCategory != EMPXGenre));
            if (!aOldId || (aOldId != aItemId))
                {
                // ignore the return value
                iObserver.AddCategoryItemL(aCategory, KNullDesC, aDriveId, aItemChangedMessages);
                updated = ETrue;
                }
            }

        if (aOldId && (aOldId != aItemId))
            {
            iObserver.DeleteEpisodeForCategoryL(aCategory, aOldId, aDriveId, aItemChangedMessages);
            updated = ETrue;
            }
        }

    return updated;
    }

// ----------------------------------------------------------------------------
// Read episode detail info from DB into media object
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::UpdateMediaL(
    RSqlStatement& aPodcastTable,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbPodcast::UpdateMediaL");

    TInt count(aAttrs.Count());
    for (TInt i = 0; i < count; ++i)
        {
        switch (aAttrs[i].ContentId())
            {
            case KMPXMediaIdGeneral:
                {
                UpdateMediaGeneralL(aPodcastTable, aAttrs[i].AttributeId(), aMedia);
                break;
                }
            case KMPXMediaIdMusic:
                {
                UpdateMediaMusicL(aPodcastTable, aAttrs[i].AttributeId(), aMedia);
                break;
                }
            case KMPXMediaIdDrm:
                {
                // DRM is set by drm helper
                break;
                }
            case KMPXMediaIdAudio:
                {
                UpdateMediaAudioL(aPodcastTable, aAttrs[i].AttributeId(), aMedia);
                break;
                }
            case KMPXMediaIdPodcast:
                {
                UpdateMediaPodcastL(aPodcastTable, aAttrs[i].AttributeId(), aMedia);
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
// Set all the attributes in CMPXMedia corresponding to KMPXMediaIdGeneral
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::UpdateMediaGeneralL(
    RSqlStatement& aPodcastTable,
    TUint aAttrId,
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbPodcast::UpdateMediaGeneralL");

    aMedia.SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
    aMedia.SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXPodcast);

    // FIX ME, temporary always fetch item ID
    //if (aAttrId & EMPXMediaGeneralId)
    if (!aMedia.IsSupported(KMPXMediaGeneralId))
        {
        TUint32 episodeId(aPodcastTable.ColumnInt64(EPodcastUniqueId));
        aMedia.SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, episodeId);
        MPX_DEBUG2("    EpisodeId[%d]", episodeId);
        }
    // FIX ME temporary always fetch URI
    //if (aAttrId & EMPXMediaGeneralUri)
    if (!aMedia.IsSupported(KMPXMediaGeneralUri))
        {
        HBufC* uri = ConstructUriL(aPodcastTable);
        CleanupStack::PushL(uri);
        aMedia.SetTextValueL(KMPXMediaGeneralUri, *uri);

        MPX_DEBUG2("    Uri[%S]", uri);
        CleanupStack::PopAndDestroy(uri);
        }
    if (aAttrId & EMPXMediaGeneralDrive)
        {
        TDriveUnit driveUnit;
        if (aMedia.IsSupported(KMPXMediaGeneralUri))
            {
            driveUnit = aMedia.ValueText(KMPXMediaGeneralUri);
            }
        else
            {
            driveUnit = MPXDbCommonUtil::GetDriveIdMatchVolIdL(iDbManager.Fs(),
                aPodcastTable.ColumnInt64(EPodcastVolumeId));
            }

        TPtrC driveName(driveUnit.Name());
        aMedia.SetTextValueL(KMPXMediaGeneralDrive, driveName);
        MPX_DEBUG2("    Drive[%S]", &driveName);
        }
    if (aAttrId & EMPXMediaGeneralSize)
        {
        // to-do: store this in the DB
        HBufC* uri = ConstructUriL(aPodcastTable);
        CleanupStack::PushL(uri);
        aMedia.SetTextValueL(KMPXMediaGeneralUri, *uri);

        MPX_DEBUG2("    Uri[%S]", uri);
        
        TEntry entry;
        RFs fs;
        User::LeaveIfError(fs.Connect());
        CleanupClosePushL(fs);
        fs.Entry(*uri, entry);
        aMedia.SetTObjectValueL<TInt>(
               TMPXAttribute(KMPXMediaIdGeneral, EMPXMediaGeneralSize),
               entry.iSize);   
        
        MPX_DEBUG2("    Size[%d]", entry.iSize);
        
        CleanupStack::PopAndDestroy(&fs);
        CleanupStack::PopAndDestroy(uri);
        }
    if (aAttrId & EMPXMediaGeneralDuration)
        {
        TInt32 duration(aPodcastTable.ColumnInt(EPodcastDuration));
        aMedia.SetTObjectValueL<TInt>(KMPXMediaGeneralDuration, duration);
        MPX_DEBUG2("    Duration[%d]", duration);
        }
    if ((aAttrId & EMPXMediaGeneralTitle) && !aMedia.IsSupported(KMPXMediaGeneralTitle))
        {
        TPtrC title(MPXDbCommonUtil::GetColumnTextL(aPodcastTable, EPodcastTitle));
        aMedia.SetTextValueL(KMPXMediaGeneralTitle, title);
        MPX_DEBUG2("    Title[%S]", &title);
        }
    if (aAttrId & EMPXMediaGeneralDate)
        {
        const TDesC& dateStr(MPXDbCommonUtil::GetColumnTextL(aPodcastTable, EPodcastTimeAdded));
        if(dateStr.Compare(KNullDesC) != 0)
        	{
            TTime dateTime(MPXDbCommonUtil::DesToTTimeL(dateStr));
            aMedia.SetTObjectValueL<TInt64>(KMPXMediaGeneralDate, dateTime.Int64());
        	}
        MPX_DEBUG2("    Date[%S]", &dateStr);
        }
    if (aAttrId & EMPXMediaGeneralComment)
        {
        TPtrC comment(MPXDbCommonUtil::GetColumnTextL(aPodcastTable, EPodcastComment));
        aMedia.SetTextValueL(KMPXMediaGeneralComment, comment);
        MPX_DEBUG2("    Comment[%S]", &comment);
        }
    if (aAttrId & EMPXMediaGeneralMimeType)
        {
        TPtrC mimeType(MPXDbCommonUtil::GetColumnTextL(aPodcastTable, EPodcastMimeType));
        aMedia.SetTextValueL(KMPXMediaGeneralMimeType, mimeType);
        MPX_DEBUG2("    MimeType[%S]", &mimeType);
        }
    if (aAttrId & EMPXMediaGeneralSynchronized)
        {
        TInt sync(aPodcastTable.ColumnInt(EPodcastSync));
        aMedia.SetTObjectValueL<TBool>(KMPXMediaGeneralSynchronized, sync);
        MPX_DEBUG2("    Synchronized[%d]", sync);
        }
    if (aAttrId & EMPXMediaGeneralDeleted)
        {
        TInt del(aPodcastTable.ColumnInt(EPodcastDeleted));
        aMedia.SetTObjectValueL<TBool>(KMPXMediaGeneralDeleted, del);
        MPX_DEBUG2("    Deleted[%d]", del);
        }
    if (aAttrId & EMPXMediaGeneralModified)
        {
        TInt mod(aPodcastTable.ColumnInt(EPodcastModified));
        aMedia.SetTObjectValueL<TBool>(KMPXMediaGeneralModified, mod);
        MPX_DEBUG2("    Modified[%d]", mod);
        }
    if (aAttrId & EMPXMediaGeneralCount)
        {
        aMedia.SetTObjectValueL<TInt>(KMPXMediaGeneralCount, 1);
        }
    if (aAttrId & EMPXMediaGeneralCollectionId)
        {
        aMedia.SetTObjectValueL<TUid>(KMPXMediaGeneralCollectionId ,
            TUid::Uid(KDBPluginUid));
        }
    if (aAttrId & EMPXMediaGeneralCopyright)
        {
        TPtrC copyright(MPXDbCommonUtil::GetColumnTextL(aPodcastTable, EPodcastCopyright));
        aMedia.SetTextValueL(KMPXMediaGeneralCopyright, copyright);
        MPX_DEBUG2("    Copyright[%S]", &copyright);
        }
    if (aAttrId & EMPXMediaGeneralFlags)
        {
        TUint32 dbFlags(aPodcastTable.ColumnInt64(EPodcastDbFlag));
        TDriveUnit driveUnit;
        if (aMedia.IsSupported(KMPXMediaGeneralUri))
            {
            TParsePtrC parse( aMedia.ValueText(KMPXMediaGeneralUri) );
            if( parse.DrivePresent() )
                {
                driveUnit = parse.Drive();
                }
            else
                {
                driveUnit = MPXDbCommonUtil::GetDriveIdMatchVolIdL(iDbManager.Fs(),
                     aPodcastTable.ColumnInt64(EPodcastVolumeId));
                }
            }
        else
            {
            driveUnit = MPXDbCommonUtil::GetDriveIdMatchVolIdL(iDbManager.Fs(),
                aPodcastTable.ColumnInt64(EPodcastVolumeId));
            }

        TInt driveId = driveUnit & KMPXMediaGeneralFlagsDriveInfo;  // 5 bits
        aMedia.SetTObjectValueL<TUint>(KMPXMediaGeneralFlags, dbFlags | driveId);

        MPX_DEBUG2("    GeneralFlags[%b]", dbFlags | driveId);
        MPX_DEBUG2("    DriveId[%u]", driveId);
        }
    if (aAttrId & EMPXMediaGeneralLastPlaybackPosition)
        {
        TInt32 lastPlaybackPosition(aPodcastTable.ColumnInt(EPodcastLastPlayPosition));
        aMedia.SetTObjectValueL<TInt32>(KMPXMediaGeneralLastPlaybackPosition,
            lastPlaybackPosition);
        MPX_DEBUG2("    LastPlayPosition[%d]", lastPlaybackPosition);
        }
    if (aAttrId & EMPXMediaGeneralPlayCount)
        {
        TUint32 playcount(aPodcastTable.ColumnInt(EPodcastPlayCount));
        aMedia.SetTObjectValueL<TInt>(KMPXMediaGeneralPlayCount, playcount);
        MPX_DEBUG2("    PlayCount[%d]", playcount);
        }
    }

// ----------------------------------------------------------------------------
// Set all the attributes in CMPXMedia corresponding to KMPXMediaIdMusic
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::UpdateMediaMusicL(
    RSqlStatement& aPodcastTable,
    TUint aAttrId,
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbPodcast::UpdateMediaMusicL");

    if (aAttrId & EMPXMediaMusicAlbumArtFileName)
        {
        TPtrC art(MPXDbCommonUtil::GetColumnTextL(aPodcastTable, EPodcastArt));
        aMedia.SetTextValueL(KMPXMediaMusicAlbumArtFileName, art);
        MPX_DEBUG2("    Album Art File Name[%S]", &art);
        }
    if (aAttrId & EMPXMediaMusicArtist)
        {
        TPtrC artist(MPXDbCommonUtil::GetColumnTextL(aPodcastTable, EPodcastArtistName));
        aMedia.SetTextValueL(KMPXMediaMusicArtist, artist);
        MPX_DEBUG2("    Artist[%S]", &artist);
        }
    if (aAttrId & EMPXMediaMusicAlbum)
        {
        TPtrC album(MPXDbCommonUtil::GetColumnTextL(aPodcastTable, EPodcastAlbumName));
        aMedia.SetTextValueL(KMPXMediaMusicAlbum, album);
        MPX_DEBUG2("    Album[%S]", &album);
        }
    if (aAttrId & EMPXMediaMusicYear)
        {
        const TDesC& dateStr(MPXDbCommonUtil::GetColumnTextL(aPodcastTable, EPodcastReleaseDate));
        if(dateStr.Compare(KNullDesC) != 0)
        	{
            TTime dateTime(MPXDbCommonUtil::DesToTTimeL(dateStr));
            aMedia.SetTObjectValueL<TInt64>(KMPXMediaMusicYear, dateTime.Int64());
            MPX_DEBUG2("    Year[%d]", dateTime.Int64());
        	}
        MPX_DEBUG2("    ReleaseDate[%S]", &dateStr);
        }
    if (aAttrId & EMPXMediaMusicAlbumTrack)
        {
        TInt32 track(aPodcastTable.ColumnInt(EPodcastAlbumTrack));
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
        TPtrC genre(MPXDbCommonUtil::GetColumnTextL(aPodcastTable, EPodcastGenreName));
        aMedia.SetTextValueL(KMPXMediaMusicGenre, genre);
        MPX_DEBUG2("    Music Genre[%S]", &genre);
        }
    if (aAttrId & EMPXMediaMusicComposer)
        {
        TPtrC composer(MPXDbCommonUtil::GetColumnTextL(aPodcastTable, EPodcastComposerName));
        aMedia.SetTextValueL(KMPXMediaMusicComposer, composer);
        MPX_DEBUG2("    Music Composer[%S]", &composer);
        }
    if (aAttrId & EMPXMediaMusicRating)
        {
        TUint32 rating(aPodcastTable.ColumnInt(EPodcastRating));
        aMedia.SetTObjectValueL<TUint32>(KMPXMediaMusicRating, rating);
        MPX_DEBUG2("    Music Rating[%d]", rating);
        }
    if (aAttrId & EMPXMediaMusicURL)
        {
        TPtrC url(MPXDbCommonUtil::GetColumnTextL(aPodcastTable, EPodcastUrl));
        aMedia.SetTextValueL(KMPXMediaMusicURL, url);
        MPX_DEBUG2("    Music URL[%S]", &url);
        }
    if (aAttrId & EMPXMediaMusicOriginalAlbumArtFileName)
        {
        // Always set original album art to be file path
        // Maybe add a new column to db for future if services like rhapsody pushes jpgs to us
        if (aMedia.IsSupported(KMPXMediaGeneralUri))
            {
            const TDesC& uri(aMedia.ValueText(KMPXMediaGeneralUri));
            aMedia.SetTextValueL(KMPXMediaMusicOriginalAlbumArtFileName, uri);
            MPX_DEBUG2("    Music Original Album Art FullPath[%S]", &uri);
            }
        else
            {
            HBufC* fullPath = ConstructUriL(aPodcastTable);
            CleanupStack::PushL(fullPath);
            aMedia.SetTextValueL(KMPXMediaMusicOriginalAlbumArtFileName, *fullPath);
            MPX_DEBUG2("    Music Original Album Art FullPath[%S]", fullPath);
            CleanupStack::PopAndDestroy(fullPath);
            }

        }
    }

// ----------------------------------------------------------------------------
// Set all the attributes in CMPXMedia corresponding to KMPXMediaIdAudio
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::UpdateMediaAudioL(
    RSqlStatement& aPodcastTable,
    TUint aAttrId,
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbPodcast::UpdateMediaAudioL");

    if (aAttrId & EMPXMediaAudioBitrate)
        {
        TUint32 bitrate(aPodcastTable.ColumnInt(EPodcastBitRate));
        aMedia.SetTObjectValueL<TUint32>(KMPXMediaAudioBitrate, bitrate);
        MPX_DEBUG2("    Bitrate[%d]", bitrate);
        }
    if (aAttrId & EMPXMediaAudioSamplerate)
        {
        TUint32 samplerate(aPodcastTable.ColumnInt(EPodcastSampleRate));
        aMedia.SetTObjectValueL<TUint32>(KMPXMediaAudioSamplerate, samplerate);
        MPX_DEBUG2("    SampleRate[%d]", samplerate);
        }
    if (aAttrId & EMPXMediaAudioNumberOfChannels)
        {
        TUint32 numchannels(aPodcastTable.ColumnInt(EPodcastNumChannels));
        aMedia.SetTObjectValueL<TUint32>(KMPXMediaAudioNumberOfChannels, numchannels);
        MPX_DEBUG2("    Num of Channels[%d]", numchannels);
        }
    if (aAttrId & EMPXMediaAudioCodec)
        {
        TUint32 codec(aPodcastTable.ColumnInt(EPodcastCodec));
        aMedia.SetTObjectValueL<TUint32>(KMPXMediaAudioAudioCodec, codec);
        MPX_DEBUG2("    Audio Codec[%d]", codec);
        }
    }

// ----------------------------------------------------------------------------
// Set all the attributes in CMPXMedia corresponding to KMPXMediaIdPodcast
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::UpdateMediaPodcastL(
    RSqlStatement& aPodcastTable,
    TUint aAttrId,
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbPodcast::UpdateMediaPodcastL");

    //  This episode is currently being played so the playback
    //  flag needs to be toggled
    if (aAttrId & EMPXMediaPodcastSetIsPlayingTrue)
        {
        SetIsPlayingL(aMedia.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId), ETrue);
        }

    if (aAttrId & EMPXMediaPodcastCategory)
        {
        TPtrC genre(MPXDbCommonUtil::GetColumnTextL(aPodcastTable, EPodcastGenreName));
        aMedia.SetTextValueL(KMPXMediaPodcastCategory, genre);
        MPX_DEBUG2("    PodcastCategory[%S]", &genre);
        }
    if (aAttrId & EMPXMediaPodcastPubDate)
        {
        const TDesC& dateStr(MPXDbCommonUtil::GetColumnTextL(aPodcastTable, EPodcastPublishDate));
        if(dateStr.Compare(KNullDesC) != 0)
        	{
            TTime dateTime(MPXDbCommonUtil::DesToTTimeL(dateStr));
            aMedia.SetTObjectValueL<TInt64>(KMPXMediaPodcastPubDate, dateTime.Int64());
        	}
        MPX_DEBUG2("    Publish Date[%S]", &dateStr);
        }
    if (aAttrId & EMPXMediaPodcastIsPlaying)
        {
        TInt isPlaying(aPodcastTable.ColumnInt(EPodcastIsPlaying));
        aMedia.SetTObjectValueL<TBool>(KMPXMediaPodcastIsPlaying, isPlaying);
        MPX_DEBUG2("    IsPlaying[%d]", isPlaying);
        }

    TInt lastPlayPos(0);
    if (aMedia.IsSupported(KMPXMediaGeneralLastPlaybackPosition))
        {
        lastPlayPos = aMedia.ValueTObjectL<TInt>(KMPXMediaGeneralLastPlaybackPosition);
        }

    TUint32 playCount(0);
    if (aMedia.IsSupported(KMPXMediaGeneralPlayCount))
        {
        playCount = aMedia.ValueTObjectL<TUint32>(KMPXMediaGeneralPlayCount);
        }

    // playcount is incremented after an episode is played to completion
    if ((lastPlayPos != 0) && (playCount == 0))
        {
        aMedia.SetTObjectValueL(KMPXMediaPodcastCategoryGroup, EMPXPartlyPlayed);
        MPX_DEBUG1( "CMPXDbPodcast::DoSetMediaPodcastL() item is partly played" );
        }
    else if ((lastPlayPos == 0) && (playCount == 0))
        {
        aMedia.SetTObjectValueL(KMPXMediaPodcastCategoryGroup, EMPXNew);
        }
    else
        {
        aMedia.SetTObjectValueL(KMPXMediaPodcastCategoryGroup, EMPXCompletelyPlayed);
        }

    aMedia.SetTObjectValueL(KMPXMediaPodcastType, EMPXPodcastItem);
    }

// ----------------------------------------------------------------------------
//  Validate podcast file
// ----------------------------------------------------------------------------
//
TMCDrmType CMPXDbPodcast::DRMTypeL(
    const TDesC& aFile)
    {
    MPX_FUNC("CMPXDbPodcast::DRMTypeL");

    TMCDrmType drm = EMCDrmNone;
    TInt pos(0);
    TParsePtrC fullEntry(aFile);
    TPtrC theExt = fullEntry.Ext();

    if (iExtensionsDrm->Find(theExt,pos) == 0)
        {
        drm = MPXDbCommonUtil::GetDRMTypeL(aFile);
        }

    return drm;
    }

// ----------------------------------------------------------------------------
//  Generate viewing columns for podcast table from attributes
// ----------------------------------------------------------------------------
//
HBufC* CMPXDbPodcast::GeneratePodcastMatchingCriteriaLC(
    TUint32 aGeneralId,
    TUint32 aContainerId,
    TMPXGeneralType aType,
    TMPXPodcastType aPodcastType,
    const CMPXMedia& aCriteria)
    {
    MPX_FUNC("CMPXDbPodcast::GeneratePodcastMatchingCriteriaLC");

    const TArray<TMPXAttribute> criteria = aCriteria.Attributes();
    TInt criteriaCount(criteria.Count());

    CDesCArrayFlat* sqlCriteria = new (ELeave) CDesCArrayFlat(criteriaCount);
    CleanupStack::PushL(sqlCriteria);

    // If EMPXMediaGeneralDeleted is not defined, always unset the deleted bit for matching
    if (!aCriteria.IsSupported(KMPXMediaGeneralDeleted))
        {
        sqlCriteria->AppendL(KCriterionPodcastNotDeleted);
        }

    TBool volumeAdded(EFalse);
    for (TInt i = 0; i < criteriaCount; ++i)
        {
        const TMPXAttribute& criterion = criteria[i];

        if (criterion == KMPXMediaGeneralId)
            {
            TInt category(MPX_ITEM_CATEGORY(aGeneralId));

            // Set the type if no type is specified
            if (aType == EMPXNoType)
                {
                aType = (category == EMPXCollection) ? EMPXItem : EMPXGroup;
                }

            TPtrC ptr;
            if ((aType == EMPXItem) && (category == EMPXCollection))
                {
                MPXDbCommonUtil::AddSqlCriterionL(*sqlCriteria,
                    KCriterionPodcastUniqueId, aGeneralId);
                }
            else if ((aType == EMPXGroup) && (category == EMPXArtist))
                {
                MPXDbCommonUtil::AddSqlCriterionL(*sqlCriteria,
                    KCriterionPodcastAuthor, aGeneralId);
                }
            else if ((aType == EMPXGroup) && (category == EMPXAlbum))
                {
                MPXDbCommonUtil::AddSqlCriterionL(*sqlCriteria,
                    KCriterionPodcastAlbum, aGeneralId);
                }
            else if ((aType == EMPXGroup) && (category == EMPXGenre))
                {
                MPXDbCommonUtil::AddSqlCriterionL(*sqlCriteria,
                    KCriterionPodcastGenre, aGeneralId);
                }
            else if ((aType == EMPXGroup) && (category == EMPXComposer))
                {
                MPXDbCommonUtil::AddSqlCriterionL(*sqlCriteria,
                    KCriterionPodcastComposer, aGeneralId);
                }
            else if ((aPodcastType == EMPXPodcastGroup) && (category == EMPXTitle))
                {
                MPXDbCommonUtil::AddSqlCriterionL(*sqlCriteria,
                    KCriterionPodcastAlbum, aGeneralId);
                }
            else if ((aType == EMPXGroup) && (category == EMPXNoCategory))
                {
                HBufC* sqlCriterion = GeneratePublishDateCategoryCriteriaL(aGeneralId & 0x00FFFFFF);
                sqlCriteria->AppendL(*sqlCriterion);
                CleanupStack::PopAndDestroy(sqlCriterion);
                }
            else
                {
                User::Leave(KErrNotSupported);
                }
            }
        else if (criterion == KMPXMediaGeneralContainerId)
            {
            TInt containerCategory(MPX_ITEM_CATEGORY(aContainerId));

            if ((aType == EMPXGroup) && (containerCategory == EMPXArtist))
                {
                MPXDbCommonUtil::AddSqlCriterionL(*sqlCriteria,
                    KCriterionPodcastAuthor, aContainerId);
                }
            else if ((aType == EMPXGroup) && (containerCategory == EMPXAlbum))
                {
                MPXDbCommonUtil::AddSqlCriterionL(*sqlCriteria,
                    KCriterionPodcastAlbum, aContainerId);
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
            MPXDbCommonUtil::AddSqlCriterionL(*sqlCriteria, KCriterionPodcastTitle, *title);
            CleanupStack::PopAndDestroy(title);
            }
        else if (criterion == KMPXMediaGeneralUri)
            {
            const TDesC& uri(aCriteria.ValueText(KMPXMediaGeneralUri));

            // Only set the criteria if Uri is not null Des. a URI has to match to location and volume
            // columns together.
            if (uri != KNullDesC)
                {
                if (!volumeAdded)
                    {
	                TDriveUnit driveUnit(uri);
	                MPXDbCommonUtil::AddSqlCriterionL(*sqlCriteria, KCriterionPodcastVolume,
	                    MPXDbCommonUtil::GetVolIdMatchDriveIdL(iDbManager.Fs(), driveUnit));
	                volumeAdded = ETrue;
                    }
                MPXDbCommonUtil::AddSqlCriterionL(*sqlCriteria, KCriterionPodcastUri,
                    uri.Mid(KMCPathStartPos));
                }

            }
        else if (criterion == KMPXMediaGeneralDrive)
            {
            if (!volumeAdded)
                {
	            // validate the drive letter, TDriveUnit panics if given drive isn't between
	            // 'A' to 'Z'
	            TDriveUnit driveUnit(aCriteria.ValueText(KMPXMediaGeneralDrive));
	            MPXDbCommonUtil::AddSqlCriterionL(*sqlCriteria, KCriterionPodcastVolume,
	                MPXDbCommonUtil::GetVolIdMatchDriveIdL(iDbManager.Fs(), driveUnit));
                volumeAdded = ETrue;
                }
            }
        else if (criterion == KMPXMediaGeneralSynchronized)
            {
            MPXDbCommonUtil::AddSqlCriterionL(*sqlCriteria, KCriterionPodcastSynchronized,
                 aCriteria.ValueTObjectL<TBool>(KMPXMediaGeneralSynchronized));
            }
        else if (criterion == KMPXMediaGeneralDeleted)
            {
            MPXDbCommonUtil::AddSqlCriterionL(*sqlCriteria, KCriterionPodcastDeleted,
                 aCriteria.ValueTObjectL<TBool>(KMPXMediaGeneralDeleted));
            }
        else if (criterion == KMPXMediaGeneralModified)
            {
            MPXDbCommonUtil::AddSqlCriterionL(*sqlCriteria, KCriterionPodcastModified,
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
// CMPXDbPodcast::GeneratePublishDateCategoryCriteriaL
// ----------------------------------------------------------------------------
//
HBufC* CMPXDbPodcast::GeneratePublishDateCategoryCriteriaL(
    const TUint& aPublishDateCategoryID)
    {
    MPX_FUNC("CMPXDbPodcast::GeneratePublishDateCategoryCriteriaL");

    HBufC* criteriaBuffer(NULL);

    HBufC* now = MPXDbCommonUtil::CurrentDateDesLC();

    switch (aPublishDateCategoryID + KPublishTodayPlaylistUID)
        {
        case KPublishTodayPlaylistUID:
            {
            criteriaBuffer = MPXDbCommonUtil::SqlCriterionLC(KCriterionPodcastPublishDateToday(),
                *now, *now);
            break;
            }
        case KPublishYesterdayPlaylistUID:
            {
            criteriaBuffer = MPXDbCommonUtil::SqlCriterionLC(KCriterionPodcastPublishDateYesterday(),
                *now, *now);
            break;
            }
        case KPublishThisWeekPlaylistUID:
            {
            TInt dayNo(MPXDbUtil::DayNoInWeek());
            dayNo++; // shifted by 1 day to get correct results
            criteriaBuffer = MPXDbCommonUtil::SqlCriterionLC(KCriterionPodcastPublishDateBetween(),
                *now, dayNo,*now, 1);
            break;
            }
        case KPublishLastWeekPlaylistUID:
            {
            TInt dayNo(MPXDbUtil::DayNoInWeek());
            dayNo++; // shifted by 1 day to get correct results
            criteriaBuffer = MPXDbCommonUtil::SqlCriterionLC(KCriterionPodcastPublishDateBetween(),
                *now, dayNo + 7, *now, dayNo);
            break;
            }
        case KPublish2WeeksAgoPlaylistUID:
            {
            TInt dayNo(MPXDbUtil::DayNoInWeek());
            dayNo++; // shifted by 1 day to get correct results
            criteriaBuffer = MPXDbCommonUtil::SqlCriterionLC(KCriterionPodcastPublishDateBetween(),
                *now, dayNo + 14, *now, dayNo + 7);
            break;
            }
        case KPublish3WeeksAgoPlaylistUID:
            {
            TInt dayNo(MPXDbUtil::DayNoInWeek());
            dayNo++; // shifted by 1 day to get correct results
            criteriaBuffer = MPXDbCommonUtil::SqlCriterionLC(KCriterionPodcastPublishDateBetween(),
                *now, dayNo + 21, *now, dayNo + 14);
            break;
            }
        case KPublishLastMonthPlaylistUID:
            {
            criteriaBuffer = MPXDbCommonUtil::SqlCriterionLC(KCriterionPodcastPublishDateLastMonth(),
                *now, *now);
            break;
            }
        case KPublishEarlierPlaylistUID:
            {
            criteriaBuffer = MPXDbCommonUtil::SqlCriterionLC(KCriterionPodcastPublishDateEarlier(),
                *now);
            break;
            }
        case KPublishUnknownPlaylistUID:
            {
            criteriaBuffer = MPXDbCommonUtil::SqlCriterionLC(KCriterionPodcastPublishDateUnknown(),
                *now);
            break;
            }
        default:
            {
            User::Leave(KErrArgument);
            }
        }

    CleanupStack::Pop(criteriaBuffer);
    CleanupStack::PopAndDestroy(now);
    CleanupStack::PushL(criteriaBuffer);

    TPtrC ptr(criteriaBuffer->Left(248));
    MPX_DEBUG2("CriteriaBuffer: %S", &ptr);

    return criteriaBuffer;
    }

// ----------------------------------------------------------------------------
// CMPXDbPodcast::ConstructUriL
// ----------------------------------------------------------------------------
//
HBufC* CMPXDbPodcast::ConstructUriL(
    RSqlStatement& aRecordset)
    {
    MPX_FUNC("CMPXDbPodcast::ConstructUriL");

    return MPXDbCommonUtil::CreateFullPathL(
        MPXDbCommonUtil::GetDriveIdMatchVolIdL(iDbManager.Fs(), aRecordset.ColumnInt64(EPodcastVolumeId)),
        MPXDbCommonUtil::GetColumnTextL(aRecordset, EPodcastLocation));
    }

// ----------------------------------------------------------------------------
// CMPXDbPodcast::EpisodeExistsL
// ----------------------------------------------------------------------------
//
TBool CMPXDbPodcast::EpisodeExistsL(
    TUint32 aEpisodeId)
    {
    MPX_FUNC("CMPXDbPodcast::EpisodeExistsL");

    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryPodcastVolume, aEpisodeId));
    TBool found = (recordset.Next() == KSqlAtRow);
    recordset.Close();

    return found;
    }

// ----------------------------------------------------------------------------
// CMPXDbPodcast::ExtraFieldsRequired
// ----------------------------------------------------------------------------
//
TBool CMPXDbPodcast::ExtraFieldsRequired(
    const TArray<TMPXAttribute>& aAttrs)
    {
    MPX_FUNC("CMPXDbPodcast::ExtraFieldsRequired");

    TBool extraRequired(EFalse);
    TInt count(aAttrs.Count());
    for (TInt i = 0; i < count; ++i)
        {
        TUint attributeId(aAttrs[i].AttributeId());

        if ((KMPXMediaIdMusic == aAttrs[i].ContentId()) &&
            ((attributeId & EMPXMediaMusicArtist) ||
            (attributeId & EMPXMediaMusicAlbum) ||
            (attributeId & EMPXMediaMusicGenre) ||
            (attributeId & EMPXMediaMusicComposer)))
            {
            // fields from category tables are required
            extraRequired = ETrue;
            break;
            }
        }

    return extraRequired;
    }

// ----------------------------------------------------------------------------
// CMPXDbPodcast::ProcessPublishDateCategoryL
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::ProcessPublishDateCategoryL(
    const TMPXItemId& aPublishDateCategoryId,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbPodcast::ProcessPublishDateCategoryL");

    TInt count(aMediaArray.Count());
    if ((aPublishDateCategoryId.iId2 > 0) && (count > 0))
        {
        // last item within category, append category ID to aid in removal of
        // all episodes within a Publish Date View category
        aMediaArray[count - 1]->SetTObjectValueL<TMPXItemId>(KMPXMediaPodcastPubDateCategoryId,
            aPublishDateCategoryId);
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbPodcast::GetPlayingItemL
// ----------------------------------------------------------------------------
//
TInt CMPXDbPodcast::GetPlayingItemL(
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbPodcast::GetPlayingItemL");

    TInt index(KErrNotFound);
    TInt count(aMediaArray.Count());
    for (TInt i = 0; i < count; ++i)
        {
        CMPXMedia* media = aMediaArray[i];
        if (media->IsSupported(KMPXMediaPodcastIsPlaying) &&
            media->ValueTObjectL<TBool>(KMPXMediaPodcastIsPlaying))
            {
            index = i;
            break;
            }
        }

    return index;
    }

// ----------------------------------------------------------------------------
// CMPXDbPodcast::CreateTableL
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::CreateTableL(
    RSqlDatabase& aDatabase,
    TBool /* aCorruptTable */)
    {
    MPX_FUNC("CMPXDbPodcast::CreateTableL");

    // Create the table
    User::LeaveIfError(aDatabase.Exec(KPodcastCreateTable));
    }

// ----------------------------------------------------------------------------
// CMPXDbPodcast::DropTableL
// ----------------------------------------------------------------------------
//
void CMPXDbPodcast::DropTableL(
    RSqlDatabase& aDatabase)
    {
    MPX_FUNC("CMPXDbPodcast::DropTableL");
    User::LeaveIfError(aDatabase.Exec(KPodcastDropTable));
    }

// ----------------------------------------------------------------------------
// CMPXDbPodcast::CheckTableL
// ----------------------------------------------------------------------------
//
TBool CMPXDbPodcast::CheckTableL(
    RSqlDatabase& aDatabase)
    {
    MPX_FUNC("CMPXDbPodcast::CheckTableL");
    return DoCheckTable(aDatabase, KPodcastCheckTable);
    }

// End of file
