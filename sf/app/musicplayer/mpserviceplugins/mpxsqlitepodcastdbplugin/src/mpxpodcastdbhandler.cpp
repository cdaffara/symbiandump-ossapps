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
* Description:  This class is used by db plugin to add episodes into DB or
*                remove episodes from DB
*
*/


// INCLUDE FILES
#include <pcres.rsg>
#include <bautils.h>
#include <MetaDataUtility.h>
#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#include <pathinfo.h>
#endif //RD_MULTIPLE_DRIVE

#include <mpxmediageneraldefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxmediaaudiodefs.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxcollectionpath.h>
#include <mpxlog.h>

#include "mpxdbcommondef.h"
#include "mpxresource.h"
#include "mpxdbcommonutil.h"

#include "mpxdbutil.h"
#include "mpxpodcastcollectiondbdef.h"
#include "mpxpodcastdbmanager.h"
#include "mpxdbcategory.h"
#include "mpxdbauxiliary.h"
#include "mpxpodcastdbhandler.h"
#include "mpxpodcastdbpluginqueries.h"

// CONSTANTS
#if defined (__MTP_PROTOCOL_SUPPORT)
#include <centralrepository.h>

const TUid KMPXMtpSettings = {0x101FFC53};    // MTP CenRep Key UID
const TUint32 KMPXMtpSaveDeletedRecordFlag = 0x00000001;      // MTP CenRep Key for Delete contents
#endif
const TInt KSqlDbCorrupted = -321;

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXPodcastDbHandler* CMPXPodcastDbHandler::NewL(
    RFs& aFs,
    CMPXResource& aResource)
    {
    MPX_FUNC("CMPXPodcastDbHandler::NewL");

    CMPXPodcastDbHandler* self = CMPXPodcastDbHandler::NewLC(aFs, aResource);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXPodcastDbHandler* CMPXPodcastDbHandler::NewLC(
    RFs& aFs,
    CMPXResource& aResource)
    {
    MPX_FUNC("CMPXPodcastDbHandler::NewLC");

    CMPXPodcastDbHandler* self = new (ELeave) CMPXPodcastDbHandler(aFs, aResource);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CMPXPodcastDbHandler::~CMPXPodcastDbHandler()
    {
    MPX_FUNC("CMPXPodcastDbHandler::~CMPXPodcastDbHandler");

    delete iDbPodcast;
    delete iDbArtist;
    delete iDbAlbum;
    delete iDbGenre;
    delete iDbComposer;
    delete iDbAuxiliary;
    delete iDbManager;

    delete iPodcastPublishDateCat;
    iPodcastPublishDateIds.Close();
    iDbDrives.Close();
    }

// ----------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave
// ----------------------------------------------------------------------------
//
CMPXPodcastDbHandler::CMPXPodcastDbHandler(
    RFs& aFs,
    CMPXResource& aResource) :
    iFs(aFs),
    iResource(aResource)
    {
    MPX_FUNC("CMPXPodcastDbHandler::CMPXPodcastDbHandler");
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::ConstructL()
    {
    MPX_FUNC("CMPXPodcastDbHandler::ConstructL");

    iPodcastPublishDateCat = iResource.ReadMenuArrayL(R_MPX_QTN_NMP_PUBLISH_DATE_ARRAY,
        iPodcastPublishDateIds);
    
    //create db manager
    iDbManager = CMPXPodcastDbManager::NewL(iFs);
    CDesCArrayFlat* podcastFolders =
#ifdef RD_MULTIPLE_DRIVE
		GetPodcastFoldersL();
#else
    	iResource.ReadDesCArrayL(R_MC_DEFAULT_MUSIC_FOLDERS);
#endif

    // create the podcast folders and initialize iDbDrives
    CleanupStack::PushL(podcastFolders);
    ProcessPodcastFoldersL(*podcastFolders);
    CleanupStack::PopAndDestroy(podcastFolders);

    // create the db infrastructure
    iDbPodcast = CMPXDbPodcast::NewL(*iDbManager, iResource, *this);
    iDbArtist = CMPXDbCategory::NewL(*iDbManager, EMPXArtist);
    iDbAlbum = CMPXDbCategory::NewL(*iDbManager, EMPXAlbum);
    iDbGenre = CMPXDbCategory::NewL(*iDbManager, EMPXGenre);
    iDbComposer = CMPXDbCategory::NewL(*iDbManager, EMPXComposer);
    iDbAuxiliary = CMPXDbAuxiliary::NewL(*iDbManager);

    // make sure all databases are created and valid
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
    // Verify the volume ID Matches
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

    // iDbManager->PrintDatabaseL();

    MPX_DEBUG2("CMPXDbHandler::ConstructL DbCount[%d]", iDbManager->DatabaseCount());
    }

// ----------------------------------------------------------------------------
// Add episode into collection
// ----------------------------------------------------------------------------
//
TUint32 CMPXPodcastDbHandler::AddEpisodeL(
    const CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXPodcastDbHandler::AddEpisodeL");

    BeginTransactionL();

    TUint32 episodeId(0);
    MPX_TRAPD(err, episodeId = DoAddEpisodeL(aMedia));

    if (iOutOfDisk && (err == KErrNotFound))
        {
        err = KErrDiskFull;
        }
    EndTransactionL(err);

    return episodeId;
    }

// ----------------------------------------------------------------------------
// Update a episode in the collection
// ----------------------------------------------------------------------------
//
CMPXDbActiveTask::TChangeVisibility CMPXPodcastDbHandler::UpdateEpisodeL(
    const CMPXMedia& aMedia,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXPodcastDbHandler::UpdateEpisodeL");

    BeginTransactionL();
    CMPXDbActiveTask::TChangeVisibility visibleChange(CMPXDbActiveTask::ENotVisibile);
    MPX_TRAPD(err, visibleChange = DoUpdateEpisodeL(aMedia, aItemChangedMessages));
    EndTransactionL(err);
    return visibleChange;
    }

// ----------------------------------------------------------------------------
// Remove the entire podcast collection database
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::RemoveEntireCollectionL()
    {
    MPX_FUNC("CMPXPodcastDbHandler::RemoveEntireCollectionL");

    BeginTransactionL();
    MPX_TRAPD(err, iDbManager->RecreateAllDatabasesL());
    EndTransactionL(err);
    }

// ----------------------------------------------------------------------------
// Delete a episode from collection
// The function notifies collection model to perform deletion
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::RemoveEpisodeL(
    TUint32 aEpisodeId,
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages,
    TBool aDeleteRecord)
    {
    MPX_FUNC("CMPXPodcastDbHandler::RemoveEpisodeL");

    BeginTransactionL();
    MPX_TRAPD(err, DoRemoveEpisodeL(aEpisodeId, aUriArray, aItemChangedMessages, aDeleteRecord));
    EndTransactionL(err);
    }

// ----------------------------------------------------------------------------
// Removes a category of episodes from the podcast collection, and its
// corresponding category in the lookup table
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::RemoveEpisodesMatchingCategoryL(
    TMPXGeneralCategory aCategory,
    TUint32 aCategoryId,
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXPodcastDbHandler::RemoveEpisodesMatchingCategoryL");

    BeginTransactionL();
    MPX_TRAPD(err, DoRemoveEpisodesMatchingCategoryL(aCategory, aCategoryId,
        aUriArray, aItemChangedMessages));
    EndTransactionL(err);
    }

// ----------------------------------------------------------------------------
// Delete episode(s) from collection belonging to specified publish date category
// The function notifies collection model to perform deletion
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::RemoveEpisodesMatchingPublishDateCategoryL(
    TUint32 aCategoryId,
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXPodcastDbHandler::RemoveEpisodesMatchingPublishDateCategoryL");

    BeginTransactionL();
    MPX_TRAPD(err, DoRemoveEpisodesMatchingPublishDateCategoryL(aCategoryId,
        aUriArray, aItemChangedMessages));
    EndTransactionL(err);
    }

// ----------------------------------------------------------------------------
// Cleanup records marked as deleted. This is designated for MTP to clean up
// records marked as deleted at the end of its session.
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::CleanupDeletedRecordsL()
    {
    MPX_FUNC("CMPXPodcastDbHandler::CleanupDeletedRecordsL");

    BeginTransactionL();
    MPX_TRAPD(err, DoCleanupDeletedRecordsL());
    EndTransactionL(err);
    }

// ----------------------------------------------------------------------------
//  Read all episodes and cache them into an array ordered by episode name
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::GetAllEpisodesL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXPodcastDbHandler::GetAllEpisodesL");
    iDbPodcast->GetAllEpisodesL(aAttrs, aMediaArray);
    }

// ----------------------------------------------------------------------------
// Retrieve the episode(s) with the specified title
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::GetEpisodesMatchingTitleL(
    TUint aTitleId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXPodcastDbHandler::GetEpisodesMatchingTitleL");
    iDbPodcast->GetEpisodesForCategoryL(EMPXAlbum, aTitleId, aAttrs,
                                        aMediaArray, ETrue);
    }

// ----------------------------------------------------------------------------
// Retrieve the episode(s) belonging to the specified playlist
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::GetEpisodesMatchingPlaylistL(
    TUint aPlaylistId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray,
    TInt& aIndexOfCurrentlyPlayingItem)
    {
    MPX_FUNC("CMPXPodcastDbHandler::GetEpisodesMatchingPlaylistL");

    switch (aPlaylistId)
        {
        case KRecentlyAddedPlaylistUID:
            {
            iDbPodcast->GetRecentlyAddedPlaylistEpisodesL(aAttrs, aMediaArray,
                aIndexOfCurrentlyPlayingItem);
            break;
            }
        case KNotPlayedPlaylistUID:
            {
            iDbPodcast->GetNotYetPlayedPlaylistEpisodesL(aAttrs, aMediaArray,
                aIndexOfCurrentlyPlayingItem);
            break;
            }
        default:
            {
            User::Leave(KErrNotSupported);
            }
        }
    }

// ----------------------------------------------------------------------------
// Retrieve the episode with the specified ID
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::GetEpisodeL(
    TUint32 aEpisodeId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXPodcastDbHandler::GetEpisodeL");

    CMPXMedia* media = CMPXMedia::NewL();
    CleanupStack::PushL(media);

    GetEpisodeL(aEpisodeId, aAttrs, *media);
    aMediaArray.AppendL(*media);

    CleanupStack::PopAndDestroy(media);
    }

// ----------------------------------------------------------------------------
// Retrieve the episode with the specified ID
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::GetEpisodeL(
    TUint32 aEpisodeId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXPodcastDbHandler::GetEpisodeL");
    iDbPodcast->GetEpisodeL(aEpisodeId, aAttrs, aMedia);
    }

// ----------------------------------------------------------------------------
// Retrieve the episode with the specified URI
// ----------------------------------------------------------------------------
//
TUint32 CMPXPodcastDbHandler::GetEpisodeIdMatchingUriL(
    const TDesC& aUri)
    {
    MPX_FUNC("CMPXPodcastDbHandler::GetEpisodeIdMatchingUriL");
    return MPXDbCommonUtil::GenerateUniqueIdL(iFs, EMPXCollection, aUri, EFalse);
    }

// ----------------------------------------------------------------------------
// Return all podcast title names
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::GetAllPodcastTitlesL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXPodcastDbHandler::GetAllPodcastTitlesL");
    iDbAlbum->GetAllCategoryItemsL(aAttrs, aMediaArray);
    }

// ----------------------------------------------------------------------------
// Retrieve episodes matching the specified publish playlist id
// ----------------------------------------------------------------------------
//
TInt CMPXPodcastDbHandler::GetEpisodesMatchingPublishPlaylistL(
    TUint aPlaylistId,
    const TArray<TMPXAttribute>& aAttrs,
    TBool aPendingCategoryDeletionFlag,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXPodcastDbHandler::GetEpisodesMatchingPublishPlaylistL");

    TInt numEpisodes(0);
    TMPXItemId podcastPublishDateCatId = 0;

    // the deletion helper is retrieving all the media within this category
    // to delete it so need to record category id within last item so
    // that the HandleChangeL callback can cause OpenL to be called after the
    // last item within the category is deleted
    if (aPendingCategoryDeletionFlag)
        {
        TInt categoryID(EMPXOther);

        // pass in the category Id so that the helper method can add
        // the category Id to the last item in this category to aid
        // in deletion of a category
        podcastPublishDateCatId = (aPlaylistId - KPublishTodayPlaylistUID) | (categoryID << 28);
        }

    switch (aPlaylistId)
        {
        case KPublishAllPlaylistUID:
            {
            TInt itemCount(iPodcastPublishDateCat->Count());
            for (TInt i = 0; i < itemCount; ++i)
                {
                TMPXItemId itemId = iPodcastPublishDateIds[i];
                TInt count(0);

                switch (iPodcastPublishDateIds[i] + KPublishTodayPlaylistUID)
                    {
                    case KPublishTodayPlaylistUID:
                        count = iDbPodcast->GetTodayEpisodesCountL();
                        break;
                    case KPublishYesterdayPlaylistUID:
                        count = iDbPodcast->GetYesterdayEpisodesCountL();
                        break;
                    case KPublishThisWeekPlaylistUID:
                        count = iDbPodcast->GetThisWeekEpisodesCountL();
                        break;
                    case KPublishLastWeekPlaylistUID:
                        count = iDbPodcast->GetLastWeekEpisodesCountL();
                        break;
                    case KPublish2WeeksAgoPlaylistUID:
                        count = iDbPodcast->Get2WeeksAgoEpisodesCountL();
                        break;
                    case KPublish3WeeksAgoPlaylistUID:
                        count = iDbPodcast->Get3WeeksAgoEpisodesCountL();
                        break;
                    case KPublishLastMonthPlaylistUID:
                        count = iDbPodcast->GetLastMonthEpisodesCountL();
                        break;
                    case KPublishEarlierPlaylistUID:
                        count = iDbPodcast->GetEarlierEpisodesCountL();
                        break;
                    case KPublishUnknownPlaylistUID:
                        count = iDbPodcast->GetUnknownEpisodesCountL();
                        break;
                    default:
                        User::Leave(KErrNotFound);
                        break;
                    };

                if (count > 0)
                    {
                    numEpisodes += count;
                    MPXDbUtil::AppendMediaL(aMediaArray, iPodcastPublishDateCat->MdcaPoint(i),
                        EMPXGroup, EMPXPlaylist, EMPXPodcastGroup, EMPXPubDate, itemId);
                    }
                }
            break;
            }

        case KPublishTodayPlaylistUID:
            MPX_DEBUG1 (_L("CMPXPodcastDbHandler::GetEpisodesMatchingPublishPlaylistL: Published Today"));
            iDbPodcast->GetTodayEpisodesL(podcastPublishDateCatId, aAttrs, aMediaArray);
            break;

        case KPublishYesterdayPlaylistUID:
            MPX_DEBUG1 (_L("CMPXPodcastDbHandler::GetEpisodesMatchingPublishPlaylistL: Published Yesterday"));
            iDbPodcast->GetYesterdayEpisodesL(podcastPublishDateCatId, aAttrs, aMediaArray);
            break;

        case KPublishThisWeekPlaylistUID:
            MPX_DEBUG1 (_L("CMPXPodcastDbHandler::GetEpisodesMatchingPublishPlaylistL: Published This Week"));
            iDbPodcast->GetThisWeekEpisodesL(podcastPublishDateCatId, aAttrs, aMediaArray);
            break;

        case KPublishLastWeekPlaylistUID:
            MPX_DEBUG1 (_L("CMPXPodcastDbHandler::GetEpisodesMatchingPublishPlaylistL: Published Last Week"));
            iDbPodcast->GetLastWeekEpisodesL(podcastPublishDateCatId, aAttrs, aMediaArray);
            break;

        case KPublish2WeeksAgoPlaylistUID:
            MPX_DEBUG1 (_L("CMPXPodcastDbHandler::GetEpisodesMatchingPublishPlaylistL: Published 2 Weeks Ago"));
            iDbPodcast->Get2WeeksAgoEpisodesL(podcastPublishDateCatId, aAttrs, aMediaArray);
            break;

        case KPublish3WeeksAgoPlaylistUID:
            MPX_DEBUG1 (_L("CMPXPodcastDbHandler::GetEpisodesMatchingPublishPlaylistL: Published 3 Weeks Ago"));
            iDbPodcast->Get3WeeksAgoEpisodesL(podcastPublishDateCatId, aAttrs, aMediaArray);
            break;

        case KPublishLastMonthPlaylistUID:
            MPX_DEBUG1 (_L("CMPXPodcastDbHandler::GetEpisodesMatchingPublishPlaylistL: Published Last Month"));
            iDbPodcast->GetLastMonthEpisodesL(podcastPublishDateCatId, aAttrs, aMediaArray);
            break;

        case KPublishEarlierPlaylistUID:
            MPX_DEBUG1 (_L("CMPXPodcastDbHandler::GetEpisodesMatchingPublishPlaylistL: Published Earlier"));
            iDbPodcast->GetEarlierEpisodesL(podcastPublishDateCatId, aAttrs, aMediaArray);
            break;

        case KPublishUnknownPlaylistUID:
            MPX_DEBUG1 (_L("CMPXPodcastDbHandler::GetEpisodesMatchingPublishPlaylistL: Published Unknown"));
            iDbPodcast->GetUnknownEpisodesL(podcastPublishDateCatId, aAttrs, aMediaArray);
            break;

        default:
            MPX_DEBUG2 (_L("CMPXPodcastDbHandler::GetEpisodesMatchingPublishPlaylistL: Invalid publish ID [%d]"), aPlaylistId);
            User::Leave(KErrNotFound);
            break;
        }

    if (aPlaylistId != KPublishAllPlaylistUID)
        {
        numEpisodes = aMediaArray.Count();
        }

    return numEpisodes;
    }

// ----------------------------------------------------------------------------
// Get the name of the row matching the given ID
// ----------------------------------------------------------------------------
//
HBufC* CMPXPodcastDbHandler::GetTitleNameMatchingIdL(
    const TUint32 aId)
    {
    MPX_FUNC("CMPXPodcastDbHandler::GetTitleNameMatchingIdL");
    return iDbAlbum->GetNameL(aId);
    }

// ----------------------------------------------------------------------------
// Get the URI of the row matching the given ID
// ----------------------------------------------------------------------------
//
HBufC* CMPXPodcastDbHandler::GetUriMatchingIdL(
    const TUint32 aId)
    {
    MPX_FUNC("CMPXPodcastDbHandler::GetUriMatchingIdL");

    return iDbPodcast->GetUriL(aId);
    }

// ----------------------------------------------------------------------------
// Find episode(s) that satisfy the specified criteria
// ----------------------------------------------------------------------------
//
CMPXMedia* CMPXPodcastDbHandler::FindAllLC(
    const CMPXMedia& aCriteria,
    const TArray<TMPXAttribute>& aAttrs)
    {
    MPX_FUNC("CMPXPodcastDbHandler::FindAllLC");

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

    FindAllL(aCriteria, aAttrs, *array);

    entries->SetTObjectValueL(KMPXMediaGeneralType, EMPXGroup);
    entries->SetTObjectValueL(KMPXMediaGeneralCategory,
         aCriteria.ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory));
    entries->SetCObjectValueL(KMPXMediaArrayContents, array);
    entries->SetTObjectValueL(KMPXMediaArrayCount, array->Count());

    CleanupStack::PopAndDestroy(array);
    return entries;
    }

// ----------------------------------------------------------------------------
// Find the number of items in the database
// ----------------------------------------------------------------------------
//
TInt CMPXPodcastDbHandler::NumberOfItemsL(
    TMPXPodcastCategory aCategory)
    {
    MPX_FUNC("CMPXPodcastDbHandler::NumberOfItemsL");

    TInt numOfItems(0);

    switch(aCategory)
        {
        case EMPXAll:
        case EMPXEpisode:
            {
            numOfItems = iDbPodcast->CountL();
            break;
            }
        case EMPXRecentlyAdded:
            {
            numOfItems = iDbPodcast->GetRecentlyAddedEpisodesCountL();
            break;
            }
        case EMPXNotYetPlayed:
            {
            numOfItems = iDbPodcast->GetNotYetPlayedEpisodesCountL();
            break;
            }
        case EMPXTitle:
            {
            numOfItems = iDbPodcast->GetNotYetPlayedEpisodesCountL();
            break;
            }
        default:
            {
            User::Leave(KErrNotSupported);
            }
        }

    return numOfItems;
    }

// ----------------------------------------------------------------------------
// Has the database been created?
// ----------------------------------------------------------------------------
//
TBool CMPXPodcastDbHandler::DatabaseCreated()
    {
    MPX_FUNC("CMPXPodcastDbHandler::DatabaseCreated");

    TBool AuxilaryDbIsRefreshed(EFalse);
    TRAP_IGNORE(AuxilaryDbIsRefreshed = iDbAuxiliary->IsRefreshedL());
    // If none of the databases were available, ie out of disk
    // we return EFalse
    return iDbManager->IsInitialized() && AuxilaryDbIsRefreshed;
    }

// ----------------------------------------------------------------------------
//  CMPXPodcastDbHandler::GetAllEpisodesDurationL
// ----------------------------------------------------------------------------
//
TInt CMPXPodcastDbHandler::GetAllEpisodesDurationL()
    {
    MPX_FUNC("CMPXPodcastDbHandler::GetAllEpisodesDurationL");
    return iDbPodcast->AllEpisodesDurationL();
    }

// ----------------------------------------------------------------------------
//  CMPXPodcastDbHandler::GetTitleDurationL
// ----------------------------------------------------------------------------
//
TInt CMPXPodcastDbHandler::GetTitleDurationL(
    TUint32 aTitleId)
    {
    MPX_FUNC("CMPXPodcastDbHandler::GetTitleDurationL");
    return iDbPodcast->TitleDurationL(aTitleId);
    }

// ----------------------------------------------------------------------------
//  CMPXPodcastDbHandler::GetNotPlayedDurationL
// ----------------------------------------------------------------------------
//
TInt CMPXPodcastDbHandler::GetNotPlayedDurationL()
    {
    MPX_FUNC("CMPXPodcastDbHandler::GetNotPlayedDurationL");
    return iDbPodcast->NotPlayedDurationL();
    }

// ----------------------------------------------------------------------------
//  CMPXPodcastDbHandler::GetRecentlyAddedDurationL
// ----------------------------------------------------------------------------
//
TInt CMPXPodcastDbHandler::GetRecentlyAddedDurationL()
    {
    MPX_FUNC("CMPXPodcastDbHandler::GetRecentlyAddedDurationL");
    return iDbPodcast->RecentlyAddedDurationL();
    }

// ----------------------------------------------------------------------------
// Set the last refreshed time into the collection
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::SetLastRefreshedTimeL(
    TTime aTime)
    {
    MPX_FUNC("CMPXPodcastDbHandler::SetLastRefreshedTimeL");

    BeginTransactionL();
    MPX_TRAPD(err, iDbAuxiliary->SetLastRefreshedTimeL(aTime));
    EndTransactionL(err);
    }

// ----------------------------------------------------------------------------
// Get the last refreshed time into the collection
// ----------------------------------------------------------------------------
//
TTime CMPXPodcastDbHandler::GetLastRefreshedTimeL()
    {
    MPX_FUNC("CMPXPodcastDbHandler::GetLastRefreshedTimeL");
    return iDbAuxiliary->LastRefreshedTimeL();
    }

// ----------------------------------------------------------------------------
// Set the db corrupted state for all drives
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::SetDBCorruptedL(
    TBool aCorrupted)
    {
    MPX_FUNC("CMPXPodcastDbHandler::SetDBCorruptedL");

    BeginTransactionL();
    MPX_TRAPD(err, iDbAuxiliary->SetDBCorruptedL(aCorrupted));
    EndTransactionL(err);
    }

// ----------------------------------------------------------------------------
// Gets the db corrupted state for all drives
// ----------------------------------------------------------------------------
//
TBool CMPXPodcastDbHandler::IsDBCorruptedL()
    {
    MPX_FUNC("CMPXPodcastDbHandler::IsDBCorruptedL");
    return iDbAuxiliary->DBCorruptedL();
    }

// ----------------------------------------------------------------------------
// Opens a database
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::OpenDatabaseL(
    TInt aDrive)
    {
    MPX_FUNC("CMPXPodcastDbHandler::OpenDatabaseL");
    iDbManager->OpenDatabaseL(aDrive);

    // Verify the volume ID after a remount event
    VerifyVolumeIdL( aDrive );
    }

// ----------------------------------------------------------------------------
// Close a database
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::CloseDatabaseL(
    TInt aDrive)
    {
    MPX_FUNC("CMPXPodcastDbHandler::CloseDatabaseL");
    iDbManager->CloseDatabaseL(aDrive);
    }

// ----------------------------------------------------------------------------
// Re-create all databases
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::ReCreateDatabasesL()
    {
    MPX_FUNC("CMPXPodcastDbHandler::ReCreateDatabasesL");
    iDbManager->RecreateAllDatabasesL();
    }

// ----------------------------------------------------------------------------
// Set handler refresh status
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::RefreshStartL()
    {
    MPX_FUNC("CMPXPodcastDbHandler::RefreshStartL");

    iOutOfDisk = EFalse;
    // Re-open databases
    // This is needed for the case where we were OOD before, but user
    // has cleared some space but now try to refresh
    MPX_TRAPD(err, iDbManager->InitDatabasesL(iDbDrives));

    if(err == KErrDiskFull)
        {
        iOutOfDisk = ETrue;
        }

    if(!iOutOfDisk)
    {
        MPX_TRAP(err,CheckDiskSpaceOnDrivesL());

        if(err == KErrDiskFull)
            {
            iOutOfDisk = ETrue;
            }
    }
    BeginTransactionL();
    }

// ----------------------------------------------------------------------------
// Re-set handler refresh status
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::RefreshEndL()
    {
    MPX_FUNC("CMPXPodcastDbHandler::RefreshEndL");

    EndTransactionL(KErrNone);

    if (!iOutOfDisk)
        {
        // Write last refreshed time as current time
        // This also sets corrupt = 0
        TTime curTime;
        curTime.HomeTime();
        SetLastRefreshedTimeL(curTime);
//        iDbManager->PrintDatabaseL();    // PREQ2536 the files sqlrowsetutil.h and sqlrowsetutil.cpp has been removed
        }
    }

// ----------------------------------------------------------------------------
// CMPXPodcastDbHandler::SetIsPlayingL
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::SetIsPlayingL(
    const CMPXMedia& aMedia,
    TBool aIsPlaying)
    {
    MPX_FUNC("CMPXPodcastDbHandler::SetIsPlayingL");

    TUint32 episodeId(0);
    if (aMedia.IsSupported(KMPXMediaGeneralId))
        {
        episodeId = aMedia.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
        }
    else if (aMedia.IsSupported(KMPXMediaGeneralUri))
        {
        const TDesC& location(aMedia.ValueText(KMPXMediaGeneralUri));
        episodeId = MPXDbCommonUtil::GenerateUniqueIdL(iFs, EMPXCollection, location, EFalse);
        }
    else
        {
        User::Leave(KErrArgument);
        }

    BeginTransactionL();
    MPX_TRAPD(err, iDbPodcast->SetIsPlayingL(episodeId, aIsPlaying));
    EndTransactionL(err);
    }

// ----------------------------------------------------------------------------
// Get all records count for podcasts
// ----------------------------------------------------------------------------
//
TUint CMPXPodcastDbHandler::GetTotalCountL(TInt aDrive)
    {
    MPX_FUNC("CMPXPodcastDbHandler::GetTotalCountL");
    TUint total(0);

    total = iDbPodcast->GetDrivePodcastCountL(aDrive);

    return total;
    }

// ----------------------------------------------------------------------------
// Get all records count for podcasts
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::GetPodcastUriArrayL(TInt aDrive, TInt aFromID, TInt aRecords,
                                               CDesCArray& aUriArr, TInt& aLastID)
    {
    MPX_FUNC("CMPXPodcastDbHandler::GetTotalUriArrayL");

    iDbPodcast->GetPodcastUriArrayL(aDrive,aFromID,aRecords,aUriArr,aLastID);
    }

// ----------------------------------------------------------------------------
// CMPXPodcastDbHandler::DoCleanupDeletedRecordsL
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::DoCleanupDeletedRecordsL()
    {
    MPX_FUNC("CMPXPodcastDbHandler::DoCleanupDeletedRecordsL");

    // delete all marked records from the Music table
    iDbPodcast->CleanupL();

    // reset the count in the Auxiliary table
    iDbAuxiliary->SetSaveDeletedRecordCountL(KDbManagerAllDrives,0);
    }

// ----------------------------------------------------------------------------
// FindAllL helper
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::FindAllL(
    const CMPXMedia& aCriteria,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXPodcastDbHandler::FindAllL");

    RArray<TMPXAttribute> attributes;
    CleanupClosePushL (attributes);
    MPXUser::MergeAttributeL(aAttrs, attributes);

    // TODO: b10liu: update all users to properly use podcast category?
    // Some users of FindAll still call without specifying a podcast category group(ie.. delete helper)
    // so need to check and use media category instead if not available.
    if (!aCriteria.IsSupported(KMPXMediaPodcastCategoryGroup))
        {
        if (!aCriteria.IsSupported(KMPXMediaGeneralCategory))
            {
            User::Leave(KErrArgument);
            }
        TMPXGeneralCategory category = aCriteria.ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory);

        switch (category)
            {
            case EMPXAlbum:
                {
                if (!aCriteria.IsSupported(KMPXMediaGeneralId))
                    {
                    User::Leave(KErrArgument);
                    }
                // return all episodes within this title
                TMPXItemId episodeTitleId = aCriteria.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
                GetEpisodesMatchingTitleL(episodeTitleId, aAttrs, aMediaArray);
                break;
                }
            case EMPXPodcast:
            case EMPXSong:
                {
                FindEpisodesL(aCriteria, attributes.Array(), aMediaArray);
                break;
                }
            case EMPXGenre:
            case EMPXPlaylist:
            case EMPXArtist:
            case EMPXComposer:
            default:
                {
                DbCategoryL(category)->FindAllL(aCriteria, attributes.Array(), aMediaArray);
                break;
                }
            }
        }
    else
        {
        TMPXPodcastCategory category =
             aCriteria.ValueTObjectL<TMPXPodcastCategory>(KMPXMediaPodcastCategoryGroup);

        switch (category)
            {
            case EMPXTitle:
                {
                if (!aCriteria.IsSupported(KMPXMediaGeneralId))
                    {
                    User::Leave(KErrArgument);
                    }
                // return all episodes within this title
                TMPXItemId episodeTitleId = aCriteria.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
                GetEpisodesMatchingTitleL(episodeTitleId, aAttrs, aMediaArray);
                break;
                }
            case EMPXPubDate:
                {
                if (!aCriteria.IsSupported(KMPXMediaGeneralId))
                    {
                    User::Leave(KErrArgument);
                    }

                TMPXItemId categoryId = aCriteria.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);

                // zero out the high 4 bits which indicate the general category
                categoryId.iId2 &= 0x00FFFFFF;
                GetEpisodesMatchingPublishPlaylistL(categoryId.iId2 + KPublishTodayPlaylistUID, // offset by KPublishTodayPlaylistUID
                    aAttrs, EFalse, aMediaArray);
                break;
                }
            case EMPXRecentlyAdded:
            case EMPXNotYetPlayed:
                {
                break;
                }
            case EMPXAll:
            case EMPXEpisode:
            case EMPXNew:
            case EMPXPartlyPlayed:
            case EMPXCompletelyPlayed:
                {
                FindEpisodesL(aCriteria, attributes.Array(), aMediaArray);
                break;
                }
            default:
                User::Leave(KErrNotSupported);
            }
        }

    CleanupStack::PopAndDestroy(&attributes);
    }

// ----------------------------------------------------------------------------
// Get episode(s) from the music table that match the given criteria
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::FindEpisodesL(
    const CMPXMedia& aCriteria,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXPodcastDbHandler::FindEpisodesL");

    TMPXGeneralType type(EMPXNoType);
    if (!aCriteria.IsSupported(KMPXMediaGeneralType))
        {
        User::Leave(KErrArgument);
        }
    type = aCriteria.ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType);

    TMPXPodcastType podcastType(EMPXPodcastNoType);
    if (aCriteria.IsSupported(KMPXMediaPodcastType))
        {
        podcastType = aCriteria.ValueTObjectL<TMPXPodcastType>(KMPXMediaPodcastType);
        }

    TUint32 id(0);
    if (aCriteria.IsSupported(KMPXMediaGeneralId))
        {
        id = aCriteria.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
        }

    TUint32 containerId(0);
    if (aCriteria.IsSupported(KMPXMediaGeneralContainerId))
        {
        containerId = aCriteria.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralContainerId);
        }

    MPX_TRAPD(err, iDbPodcast->FindEpisodesL(id, containerId, type, podcastType,
        aCriteria, aAttrs, aMediaArray));
    if (err == KErrNotSupported)
        {
        User::Leave(KErrNotFound);
        }

    User::LeaveIfError(err);
    }

// ----------------------------------------------------------------------------
// Add episode to collection
// ----------------------------------------------------------------------------
//
TUint32 CMPXPodcastDbHandler::DoAddEpisodeL(
    const CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXPodcastDbHandler::DoAddEpisodeL");

    if (!aMedia.IsSupported(KMPXMediaGeneralUri))
        {
        User::Leave(KErrArgument);
        }

    TDriveUnit drive(aMedia.ValueText(KMPXMediaGeneralUri));
    return iDbPodcast->AddEpisodeL(aMedia, drive);
    }

// ----------------------------------------------------------------------------
// Update a episode in the collection
// ----------------------------------------------------------------------------
//
CMPXDbActiveTask::TChangeVisibility CMPXPodcastDbHandler::DoUpdateEpisodeL(
    const CMPXMedia& aMedia,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXPodcastDbHandler::DoUpdateEpisodeL");

    CMPXDbActiveTask::TChangeVisibility visibleChange(CMPXDbActiveTask::ENotVisibile);
    TUint32 curId(0);
    TUint32 newId(0);
    TInt curDrive(KErrNotFound);
    TInt newDrive(KErrNotFound);

    // find the episode by Id and update it
    if( aMedia.IsSupported(KMPXMediaGeneralId))
        {
        curId = (aMedia.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId)).iId2;
        curDrive = iDbPodcast->GetDriveL(curId);
        }
    // find the episode by URI and update it
    if(aMedia.IsSupported(KMPXMediaGeneralUri))
        {
        const TDesC& uri = aMedia.ValueText(KMPXMediaGeneralUri);
        if (uri.Length() == 0)
            {
            User::Leave( KErrArgument );
            }

        newDrive = TDriveUnit(uri);
        newId = MPXDbCommonUtil::GenerateUniqueIdL(iFs, EMPXCollection, uri, EFalse);
        }
    if (!curId && !newId)
        {
        User::Leave(KErrNotSupported);
        }

    TInt driveId = (curDrive != KErrNotFound) ? curDrive : newDrive;

    if (driveId != KErrNotFound)
        {
        TUint32 episodeId = curId ? curId : newId;

        // Update the Podcast table
        visibleChange = iDbPodcast->UpdateEpisodeL(episodeId, aMedia, aItemChangedMessages);
        }

    return visibleChange;
    }

// ----------------------------------------------------------------------------
// CMPXPodcastDbHandler::DoRemoveEpisodeL
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::DoRemoveEpisodeL(
    TUint32 aEpisodeId,
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages,
    TBool aDeleteRecord)
    {
    MPX_FUNC("CMPXPodcastDbHandler::DoRemoveEpisodeL");

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
    // marked records at the end of each session via CleanupDeletedMediasL.
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
            TInt drive = iDbPodcast->GetDriveL(aEpisodeId);
            TUint32 savedDeletedDriveRecordCount(iDbAuxiliary->SaveDeletedRecordCountL(drive));
            iDbAuxiliary->SetSaveDeletedRecordCountL(drive,++savedDeletedDriveRecordCount);
            }
        }
#endif

    // delete the episode 
	// IsAdd is passed EFalse to ensure the episode count will be decremented
    iDbPodcast->DeleteEpisodeL(aEpisodeId, aUriArray, aItemChangedMessages, EFalse, deleteRecord);
    }

// ----------------------------------------------------------------------------
// CMPXPodcastDbHandler::DoRemoveEpisodesMatchingCategoryL
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::DoRemoveEpisodesMatchingCategoryL(
    TMPXGeneralCategory aCategory,
    TUint32 aCategoryId,
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXPodcastDbHandler::DoRemoveEpisodesMatchingCategoryL");
    iDbPodcast->DeleteCategoryL(aCategory, aCategoryId, aUriArray,
        aItemChangedMessages);
    }

// ----------------------------------------------------------------------------
// CMPXPodcastDbHandler::DoRemoveEpisodesMatchingPublishDateCategoryL
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::DoRemoveEpisodesMatchingPublishDateCategoryL(
    TUint32 aCategoryId,
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXPodcastDbHandler::DoRemoveEpisodesMatchingPublishDateCategoryL");

    switch (aCategoryId + KPublishTodayPlaylistUID)
        {
        case KPublishTodayPlaylistUID:
            MPX_DEBUG1 (_L("CMPXPodcastDbHandler::DoRemoveEpisodesMatchingPublishDateCategoryL: Published Today"));
            iDbPodcast->DeleteTodayEpisodesL(aUriArray, aItemChangedMessages);
            break;

        case KPublishYesterdayPlaylistUID:
            MPX_DEBUG1 (_L("CMPXPodcastDbHandler::DoRemoveEpisodesMatchingPublishDateCategoryL: Published Yesterday"));
            iDbPodcast->DeleteYesterdayEpisodesL(aUriArray, aItemChangedMessages);
            break;

        case KPublishThisWeekPlaylistUID:
            MPX_DEBUG1 (_L("CMPXPodcastDbHandler::DoRemoveEpisodesMatchingPublishDateCategoryL: Published This Week"));
            iDbPodcast->DeleteThisWeekEpisodesL(aUriArray, aItemChangedMessages);
            break;

        case KPublishLastWeekPlaylistUID:
            MPX_DEBUG1 (_L("CMPXPodcastDbHandler::DoRemoveEpisodesMatchingPublishDateCategoryL: Published Last Week"));
            iDbPodcast->DeleteLastWeekEpisodesL(aUriArray, aItemChangedMessages);
            break;

        case KPublish2WeeksAgoPlaylistUID:
            MPX_DEBUG1 (_L("CMPXPodcastDbHandler::DoRemoveEpisodesMatchingPublishDateCategoryL: Published 2 Weeks Ago"));
            iDbPodcast->Delete2WeeksAgoEpisodesL(aUriArray, aItemChangedMessages);
            break;

        case KPublish3WeeksAgoPlaylistUID:
            MPX_DEBUG1 (_L("CMPXPodcastDbHandler::DoRemoveEpisodesMatchingPublishDateCategoryL: Published 3 Weeks Ago"));
            iDbPodcast->Delete3WeeksAgoEpisodesL(aUriArray, aItemChangedMessages);
            break;

        case KPublishLastMonthPlaylistUID:
            MPX_DEBUG1 (_L("CMPXPodcastDbHandler::DoRemoveEpisodesMatchingPublishDateCategoryL: Published Last Month"));
            iDbPodcast->DeleteLastMonthEpisodesL(aUriArray, aItemChangedMessages);
            break;

        case KPublishEarlierPlaylistUID:
            MPX_DEBUG1 (_L("CMPXPodcastDbHandler::DoRemoveEpisodesMatchingPublishDateCategoryL: Published Earlier"));
            iDbPodcast->DeleteEarlierEpisodesL(aUriArray, aItemChangedMessages);
            break;

        case KPublishUnknownPlaylistUID:
            MPX_DEBUG1 (_L("CMPXPodcastDbHandler::DoRemoveEpisodesMatchingPublishDateCategoryL: Published Unknown"));
            iDbPodcast->DeleteUnknownEpisodesL(aUriArray, aItemChangedMessages);
            break;

        default:
            MPX_DEBUG2 (_L("CMPXPodcastDbHandler::DoRemoveEpisodesMatchingPublishDateCategoryL: Invalid publish date ID [%d]"), aCategoryId);
            // TODO: Leave with error?
            break;
        }
    }

// ----------------------------------------------------------------------------
// CMPXPodcastDbHandler::ProcessPodcastFoldersL
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::ProcessPodcastFoldersL(
    const CDesCArray& aFolders)
    {
    MPX_FUNC("CMPXPodcastDbHandler::ProcessPodcastFoldersL");

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

        // make sure the folder is created
        TVolumeInfo info;
        if (iFs.Volume(info, driveUnit) == KErrNone)
           {
            if (!BaflUtils::PathExists(iFs, folder))
                {
                // create music folder if necessary
                TInt err(iFs.MkDirAll(folder));
                MPX_DEBUG3("Try to create podcast folder %S return code %d", &folder, err);
                if (err != KErrAlreadyExists)
                    {
                    User::LeaveIfError(err);
                    }
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// CMPXPodcastDbHandler::DbCategoryL
// ----------------------------------------------------------------------------
//
CMPXDbCategory* CMPXPodcastDbHandler::DbCategoryL(
    TMPXGeneralCategory aCategory) const
    {
    MPX_FUNC("CMPXPodcastDbHandler::DbCategoryL");

    CMPXDbCategory* dbCategory(NULL);
    switch (aCategory)
        {
        case EMPXArtist:
            {
            dbCategory = iDbArtist;
            break;
            }
        case EMPXAlbum:
            {
            dbCategory = iDbAlbum;
            break;
            }
        case EMPXGenre:
            {
            dbCategory = iDbGenre;
            break;
            }
        case EMPXComposer:
            {
            dbCategory = iDbComposer;
            break;
            }
        default:
            User::Leave(KErrNotSupported);
        }

    return dbCategory;
    }

// ----------------------------------------------------------------------------
// Starts a transaction on all databases
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::BeginTransactionL()
    {
    MPX_FUNC("CMPXPodcastDbHandler::BeginTransactionL");
    iDbManager->BeginL();
    }

// ----------------------------------------------------------------------------
// Ends a transaction on all databases
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::EndTransactionL(
    TInt aError)
    {
    MPX_FUNC("CMPXPodcastDbHandler::EndTransactionL");

    if (aError)
        {
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

// ----------------------------------------------------------------------------
// Checks if the database is currently in a transaction
// ----------------------------------------------------------------------------
//
TBool CMPXPodcastDbHandler::InTransaction()
    {
    MPX_FUNC("CMPXPodcastDbHandler::InTransaction");
    return iDbManager->InTransaction();
    }

// ----------------------------------------------------------------------------
// Verifies that the volume ID of the database matches the drive
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::VerifyVolumeIdL( TInt aDrive )
    {
    MPX_DEBUG1("CMPXPodcastDbHandler::VerifyVolumeIdL <--");

    if( iDbManager->IsOpen(aDrive) )
        {
        TVolumeInfo volInfo;
        iFs.Volume(volInfo, aDrive );
        TUint curId(volInfo.iUniqueID);

        TInt volId = iDbAuxiliary->IdL( aDrive );

        // New database, no volume id set, mask out top bit because this is an uint
        //
        MPX_DEBUG3("CMPXPodcastDbHandler::VerifyVolumeIdL drive:%i db:%i", curId, volId);
        if( volId == 0 && (curId&0x7FFFFFFF) )
            {
            MPX_DEBUG1("CMPXPodcastDbHandler::VerifyVolumeIdL -- New ID");
            BeginTransactionL();
            TRAPD( err, iDbAuxiliary->SetIdL( aDrive, curId&0x7FFFFFFF ) );
            EndTransactionL( err );

            // KSqlDbCorrupted indicates DB corrupted, need to recreate.
            if ( err == KSqlDbCorrupted )
                {
                MPX_DEBUG1("CMPXPodcastDbHandler::VerifyVolumeIdL -- Corrupted DB");
                iDbManager->RecreateDatabaseL( aDrive );
                BeginTransactionL();
                TRAPD( err, iDbAuxiliary->SetDBCorruptedL( ETrue ) );
                EndTransactionL( err );
                }
            }
        // Unmatched volume id, mark db as corrupt
        //
        else if ( (curId&0x7FFFFFFF) != (volId&0x7FFFFFFFF) )
            {
            MPX_DEBUG1("CMPXPodcastDbHandler::VerifyVolumeIdL -- ID match FAILED");
            iDbManager->RecreateDatabaseL( aDrive );
            BeginTransactionL();
            TRAPD( err, iDbAuxiliary->SetDBCorruptedL( ETrue ) );
            EndTransactionL( err );
            }
        }
    MPX_DEBUG1("CMPXPodcastDbHandler::VerifyVolumeIdL -->");
    }


// ----------------------------------------------------------------------------
// Verifies that the volume ID of the database matches the drive
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::VerifyVolumeIdL()
    {
    MPX_DEBUG1("CMPXPodcastDbHandler::VerifyVolumeIdL <--");
    TInt count( iDbDrives.Count() );
    for( TInt i=0; i<count; ++i )
        {
        VerifyVolumeIdL(iDbDrives[i]);
        }
    MPX_DEBUG1("CMPXPodcastDbHandler::VerifyVolumeIdL -->");
    }

// ----------------------------------------------------------------------------
// Checks if there is a drive that has a low disk space
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::CheckDiskSpaceOnDrivesL()
    {
    MPX_DEBUG1("CMPXPodcastDbHandler::CheckDiskSpaceOnDrivesL <--");

    TInt count( iDbDrives.Count() );
    for( TInt index=0; index<count; ++index )
        {
        iDbManager->CheckDiskSpaceL(iDbDrives[index]);
        }
    MPX_DEBUG1("CMPXPodcastDbHandler::CheckDiskSpaceOnDrivesL -->");
    }
#if defined (__MTP_PROTOCOL_SUPPORT)

// ----------------------------------------------------------------------------
// CMPXPodcastDbHandler::SaveDeletedSongs
// ----------------------------------------------------------------------------
//
TBool CMPXPodcastDbHandler::SaveDeletedSongs()
    {
    MPX_FUNC("CMPXPodcastDbHandler::SaveDeletedSongs");

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

// ----------------------------------------------------------------------------
// CMPXPodcastDbHandler::AddCategoryItemL
// ----------------------------------------------------------------------------
//
TUint32 CMPXPodcastDbHandler::AddCategoryItemL(
    TMPXGeneralCategory aCategory,
    const TDesC& aName,
    TInt aDriveId,
    CMPXMessageArray* aItemChangedMessages)
    {
    MPX_FUNC("CMPXPodcastDbHandler::AddCategoryItemL");

    TBool newRecord(EFalse);
    TUint32 id(DbCategoryL(aCategory)->AddItemL(aName, aDriveId, newRecord,
        (aCategory != EMPXGenre)));
    if (newRecord && aItemChangedMessages)
        {
        MPXDbCommonUtil::AddItemChangedMessageL(*aItemChangedMessages, id,
            EMPXItemInserted, aCategory, KDBPluginUid);
        }

    return id;
    }

// ----------------------------------------------------------------------------
// CMPXPodcastDbHandler::DeleteEpisodeForCategoryL
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbHandler::DeleteEpisodeForCategoryL(
    TMPXGeneralCategory aCategory,
    TUint32 aCategoryId,
    TInt aDriveId,
    CMPXMessageArray* aItemChangedMessages)
    {
    MPX_FUNC("CMPXPodcastDbHandler::DeleteEpisodeForCategoryL");
    DbCategoryL(aCategory)->DecrementEpisodesForCategoryL(aCategoryId, aDriveId,
        aItemChangedMessages);
    }

#ifdef RD_MULTIPLE_DRIVE

// ----------------------------------------------------------------------------------------------------------
// Retrieve all visible podcast folder locations
// ----------------------------------------------------------------------------------------------------------
//
CDesCArrayFlat* CMPXPodcastDbHandler::GetPodcastFoldersL()
    {
    MPX_FUNC("CMPXPodcastDbHandler::GetPodcastFoldersL()");
    TDriveList driveList;
    TInt driveCount(0);
    User::LeaveIfError(DriveInfo::GetUserVisibleDrives(iFs, driveList, driveCount));
    MPX_DEBUG2 ("CMPXDbHandler::GetPodcastFoldersL() - driveCount = %d", driveCount);

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
                MPX_DEBUG2("CMPXDbHandler::GetPodcastFoldersL() - adding...%S", &rootPath);
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
                MPX_DEBUG2 ("CMPXDbHandler::GetPodcastFoldersL() - adding...%S", &drive);
                }
            }
        }

    CleanupStack::Pop(folders);
    return folders;
    }

#endif // RD_MULTIPLE_DRIVE

// ---------------------------------------------------------------------------
// CMPXPodcastDbHandler::IsRemoteDrive
// ---------------------------------------------------------------------------
//
TBool CMPXPodcastDbHandler::IsRemoteDrive(TDriveNumber aDrive)
    {
    return iDbManager->IsRemoteDrive(aDrive);
    }

// End of File


