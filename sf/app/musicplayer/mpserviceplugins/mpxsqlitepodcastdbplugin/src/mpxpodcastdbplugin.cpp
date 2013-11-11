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
* Description:  Implementation of podcast collection DB Plugin interface
*
*/


// INCLUDE FILES
#include <e32cmn.h>
#include <pcres.rsg>
#include <bautils.h>
#include <data_caging_path_literals.hrh>

#include <mpxcmn.h>
#include <mpxcollectionpluginobserver.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxmediaaudiodefs.h>
#include <mpxmediacollectiondetaildefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxmessagecontainerdefs.h>
#include <mpxcollectioncommanddefs.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxdrmmediautility.h>
#include <mpxmediadrmdefs.h>
#include <mpxlog.h>
#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif //RD_MULTIPLE_DRIVE

#include "mpxdbcommondef.h"
#include "mpxresource.h"
#include "mpxdbcommonstd.h"
#include "mpxdbcommonutil.h"

#include "mpxpodcastcollectiondbstd.h"
#include "mpxpodcastdbhandler.h"
#include "mpxdbutil.h"
#include "mpxpodcastcollectiondbdef.h"
#include "mpxpodcastcollectiondb.hrh"
#include "mpxpodcastdbplugin.h"

// CONSTANTS
const TInt KIncrementalDeleteCount = 400;

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXPodcastDbPlugin* CMPXPodcastDbPlugin::NewL(
    TAny* /* aInitParams */)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::NewL");

    CMPXPodcastDbPlugin* self = new (ELeave) CMPXPodcastDbPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor.
// ----------------------------------------------------------------------------
//
CMPXPodcastDbPlugin::~CMPXPodcastDbPlugin()
    {
    MPX_FUNC("CMPXPodcastDbPlugin::~CMPXPodcastDbPlugin");

    iSelections.Reset();
    iSelections.Close();
    iFs.Close();
    delete iDbHandler;
    delete iDrmMediaUtility;
    if (iResource)
        {
        iResource->Release();
        }
    iPodcastLibraryMainMenuItemIds.Close();
    delete iPodcastLibraryMainMenuItemTitles;
    delete iPodcastLibraryTitles;
    iPodcastPublishDateIds.Close();
    delete iPodcastTitlePublishDateCat;
    iPodcastEpisodeViewPublishDateIds.Close();
    delete iPodcastEpisodeViewPublishDateTitle;
    delete iTitleMyPodcast;
    delete iTitleAllEpisodes;
    delete iTitlePubDate;
    delete iTitleTitles;
    delete iTitleAdded;
    delete iTitleUnplayed;

    if (iActiveTask)
        {
        iActiveTask->Cancel();
        delete iActiveTask;
        }
    }

// ----------------------------------------------------------------------------
// Constructor.
// ----------------------------------------------------------------------------
//
CMPXPodcastDbPlugin::CMPXPodcastDbPlugin()
    {
    MPX_FUNC("CMPXPodcastDbPlugin::CMPXPodcastDbPlugin");
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::ConstructL ()
    {
    MPX_FUNC("CMPXPodcastDbPlugin::ConstructL");
    iFirstDeleteStep = ETrue;
    User::LeaveIfError(iFs.Connect());
    iDrmMediaUtility = CMPXDrmMediaUtility::NewL();

    TParse parse;
    parse.Set( KMPXCollectionDbResourceFile, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName resFile(parse.FullName());
    User::LeaveIfError(MPXUser::CompleteWithDllPath(resFile));
    BaflUtils::NearestLanguageFile(iFs, resFile);
    iResource = CMPXResource::NewL(resFile);

    iDbHandler = CMPXPodcastDbHandler::NewL(iFs, *iResource);

    iPodcastLibraryMainMenuItemTitles = iResource->ReadMenuArrayL(R_MC_MENU_ITEMS_ARRAY,
        iPodcastLibraryMainMenuItemIds);
    iPodcastTitlePublishDateCat = iResource->ReadMenuArrayL(R_MPX_QTN_NMP_PUBLISH_DATE_ARRAY,
        iPodcastPublishDateIds);
    iPodcastEpisodeViewPublishDateTitle = iResource->ReadMenuArrayL(
        R_MPX_QTN_NMP_EPISODES_TITLE_PUBLISHED_DATE_ARRAY, iPodcastEpisodeViewPublishDateIds);
    iTitleMyPodcast = iResource->ReadHBufCL(R_MPX_QTN_MP_TITLE_MY_PODCAST);
    iTitleAllEpisodes = iResource->ReadHBufCL(R_MPX_QTN_MP_TITLE_ALL_EPISODES);
    iTitlePubDate = iResource->ReadHBufCL(R_MPX_QTN_MUS_TITLE_PUBLISH_DATE);
    iTitleTitles = iResource->ReadHBufCL(R_MPX_QTN_MP_TITLE_PODCASTS);
    iTitleAdded = iResource->ReadHBufCL(R_MPX_QTN_MP_TITLE_RECENTLY_ADDED);
    iTitleUnplayed = iResource->ReadHBufCL(R_MPX_QTN_MP_TITLE_UNPLAYED);
    iPodcastLibraryTitles = iResource->ReadMenuArrayL(R_MC_MENU_TITLES_ARRAY,
        iPodcastLibraryMainMenuItemIds);

    iActiveTask = CMPXDbActiveTask::NewL(*this);
    }

// ----------------------------------------------------------------------------
// Navigates to the given path
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::OpenL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXFilter* /*aFilter*/)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::OpenL");
    MPX_DEBUG_PATH (aPath);

    RArray<TMPXAttribute> openAttrs;
    CleanupClosePushL(openAttrs);

    RArray<TInt> supportedIds;
    CleanupClosePushL(supportedIds);

    SetAttributesL(aPath, openAttrs, supportedIds);

    CMPXMedia* entries = CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PopAndDestroy(&supportedIds);
    CleanupStack::PushL(entries);

    TInt error(KErrNone);
    TBool isEpisode(EFalse);
    CMPXCollectionPath* newPath(NULL);

    TBool openingForPlayback(EFalse);

    if(aAttrs.Count() == 1 &&
       aAttrs[0].ContentId() == KMPXMediaIdPodcast)
    {
        if(aAttrs[0].AttributeId() & EMPXMediaPodcastSetIsPlayingTrue)
        {
        openingForPlayback = ETrue;
        }
    }

    // Make sure we handle the correct open mode
    //
    TMPXOpenMode openmode = aPath.OpenNextMode();
    switch (openmode)
        {
        case EMPXOpenGroupOrPlaylist:
            {
            MPX_TRAP(error, isEpisode = DoOpenL (
                aPath, openAttrs.Array(), *entries, openingForPlayback));
            break;
            }

        case EMPXOpenPlaylistOnly:
            {
            if( aPath.Count() > 0 )
                {
                // Try to open
                MPX_TRAP(error, newPath = DoOpenPlaylistL(aPath, openAttrs.Array(), openingForPlayback));
                CleanupStack::PushL(newPath);
                isEpisode = ETrue;
                }
            else // no items, so open in normal mode
                {
                MPX_TRAP(error, isEpisode = DoOpenL (
                    aPath, openAttrs.Array(), *entries, openingForPlayback));
                }
            break;
            }
        default:
            // do nothing
            break;
        }

    if (isEpisode)
        {
        if (openmode == EMPXOpenGroupOrPlaylist)
            {
            iObs->HandleOpen(const_cast<CMPXCollectionPath*>(&aPath), error);
            }
        else // openmode == EMPXOpenPlaylistOnly
            {
            iObs->HandleOpen(newPath, error);
            }
        }
    else
        {
        MPX_DEBUG_PATH (aPath);

        entries->SetCObjectValueL(KMPXMediaGeneralContainerPath,
            const_cast<CMPXCollectionPath*>(&aPath));
        iObs->HandleOpen(entries, error);
        }

    if (newPath)
        {
        CleanupStack::PopAndDestroy(newPath);
        }

    CleanupStack::PopAndDestroy(entries);
    CleanupStack::PopAndDestroy(&openAttrs);
    }

// ----------------------------------------------------------------------------
// Get the extended properties of the current file (async)
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::MediaL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
    const TArray<TCapability>& /*aCaps*/,
    CMPXAttributeSpecs* /*aSpecs*/)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::MediaL");
    MPX_DEBUG_PATH(aPath);

    RArray<TInt> supportedIds;
    CleanupClosePushL(supportedIds);
    if (aPath.Selection().Count())
        {
        // it's a container if there are multiple selection, else it's not a container
        supportedIds.AppendL(KMPXMediaIdContainer);
        }
    MPXDbCommonUtil::FillInSupportedUIDsL (aAttrs, supportedIds);
    CMPXMedia* entries = CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PopAndDestroy(&supportedIds);
    CleanupStack::PushL(entries);

    DoMediaL(aPath, aAttrs, *entries);

    // Also fetch collection details
    DoHandleOtherMediaAttributesL(aAttrs, aPath, *entries);

    iObs->HandleMedia(entries, KErrNone);
    CleanupStack::PopAndDestroy(entries);
    }

// ----------------------------------------------------------------------------
// Cancel outstanding request
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::CancelRequest()
    {
    MPX_FUNC("CMPXPodcastDbPlugin::CancelRequest");
    iActiveTask->Cancel();
    }

// ----------------------------------------------------------------------------
// Executes the given command on the collection
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::CommandL(
    TMPXCollectionCommand aCmd,
    TInt aArg /* = 0 */)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::CommandL");

    switch (aCmd)
        {
        case EMcCmdRemoveAll:
            {
            MPX_DEBUG1("CMPXPodcastDbPlugin::CommandL - EMcCmdRemoveAll");
            // Remove EVERYthing from the collection
            iDbHandler->RemoveEntireCollectionL();
            break;
            }
        case EMcCmdClose:
        case EMcCloseCollection:
            {
            MPX_DEBUG1("CMPXPodcastDbPlugin::CommandL - EMcCloseCollection");
            // Close the specified database
 #ifdef RD_MULTIPLE_DRIVE
            MPX_DEBUG1("Multiple drives closing databases");
            if ( aArg <0)
                {
                DriveInfo::TDriveArray driveArray;
                User::LeaveIfError ( DriveInfo::GetUserVisibleDrives ( iFs, driveArray));
                TInt count( driveArray.Count ());
                for (TInt i=0; i<count; ++i)
                    {
                    MPX_DEBUG2("At drive %i", driveArray[i]);
                    if ((driveArray[i] != EDriveC) && (!iDbHandler->IsRemoteDrive(static_cast<TDriveNumber>(driveArray[i])))) 
                        {
                        MPX_DEBUG2("Closing database %i", driveArray[i]);
                        TRAP_IGNORE( iDbHandler->CloseDatabaseL( driveArray[i] ) );
                        }
                    }
                }
            else
                {
                iDbHandler->CloseDatabaseL (aArg);
                }
 #else
            iDbHandler->CloseDatabaseL(aArg);
 #endif // RD_MULTIPLE_DRIVE
            break;
            }
        case EMcReOpenCollection:
            {
            MPX_DEBUG1("CMPXPodcastDbPlugin::CommandL - EMcReOpenCollection");
            // Open the specified database
#ifdef RD_MULTIPLE_DRIVE
            MPX_DEBUG1("Multiple drives opening databases");
            DriveInfo::TDriveArray driveArray;
            User::LeaveIfError( DriveInfo::GetUserVisibleDrives( iFs, driveArray ) );
            TInt count( driveArray.Count() );
            for( TInt i=0; i<count; ++i )
                {
                MPX_DEBUG2("At drive %i", driveArray[i]);
                if (( driveArray[i] != EDriveC ) && (!iDbHandler->IsRemoteDrive(static_cast<TDriveNumber>(driveArray[i])))) 
                    {
                    TUint driveStatus(0);
                    User::LeaveIfError( DriveInfo::GetDriveStatus(
                        iFs, driveArray[i], driveStatus ) );
                    if( driveStatus & DriveInfo::EDrivePresent )
                        {
                        MPX_DEBUG2("Opening database %i", driveArray[i]);
                        TRAP_IGNORE( iDbHandler->OpenDatabaseL( driveArray[i] ) );
                        }
                    }
                }
#else
            iDbHandler->OpenDatabaseL(aArg);
#endif // RD_MULTIPLE_DRIVE
            break;
            }
        case EMcRefreshStarted:
            {
            MPX_DEBUG1("CMPXPodcastDbPlugin::CommandL - EMcRefreshStarted");
            iDbHandler->RefreshStartL();
            iRefreshing = ETrue;
            break;
            }
        case EMcRefreshEnded:
            {
            MPX_DEBUG1("CMPXPodcastDbPlugin::CommandL - EMcRefreshEnded");
            // ask the handler to finalize the transaction
            iDbHandler->RefreshEndL();
            iRefreshing = EFalse;
            break;
            }
         case EMcCmdReCreateDB:
            {
            // Recreate all databases
            MPX_DEBUG1("CMPXPodcastDbPlugin::CommandL - EMcCmdReCreateDB");
            iDbHandler->ReCreateDatabasesL();
            break;
            }
         case EMcCmdDbCorrupted:
            {
            MPX_DEBUG1("CMPXPodcastDbPlugin::CommandL - EMcCmdDbCorrupted");
            iDbHandler->SetDBCorruptedL(ETrue);
            break;
            }
        case EMcCmdCollectionInit:
        case EMcCmdRefresh:
        case EMcCmdCollectionResyn:
            {
            // deprecated
            break;
            }
        case EMcCmdMtpStart:
            iMtpInUse = ETrue;
            break;
        case EMcCmdMtpEnd:
            iMtpInUse = EFalse;
            break;
        default:
            {
            User::Leave(KErrNotSupported);
            }
        }
    }

// ----------------------------------------------------------------------------
// Executes the given command on the collection
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::CommandL(
    CMPXCommand& aCmd)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::CommandL");

    if (!aCmd.IsSupported(KMPXCommandGeneralId))
        {
        User::Leave(KErrArgument);
        }

    TMPXCommandId commandId = aCmd.ValueTObjectL<TMPXCommandId>(KMPXCommandGeneralId);

    TBool syncOp(EFalse);
    if( aCmd.IsSupported(KMPXCommandGeneralDoSync) )
        {
        syncOp = aCmd.ValueTObjectL<TBool>(KMPXCommandGeneralDoSync);
        }

    // Handle this operation synchronously or asynchronously
    if( !syncOp )
        {
        iActiveTask->StartL(commandId, aCmd);
        }
    else  // Sync operation
        {
        switch (commandId)
            {
            case KMPXCommandIdCollectionRetrieveUriForDeletion:
                {
                MPX_DEBUG1("CMPXPodcastDbPlugin::CommandL - KMPXCommandIdCollectionRetrieveUriForDeletion");
                DoRetrieveUriForDeletionL(aCmd);
                break;
                }
            case KMPXCommandIdCollectionRemove:
                {
                MPX_DEBUG1("CMPXPodcastDbPlugin::CommandL - KMPXCommandIdCollectionRemove");
                if (iFirstDeleteStep )
                    {
                    iFirstDeleteStep = EFalse;
                    }
                DoRemovePathL(aCmd);
                break;
                }
            case KMPXCommandIdCollectionRemoveMedia:
                {
                MPX_DEBUG1("CMPXPodcastDbPlugin::CommandL - KMPXCommandIdCollectionRemoveMedia");
                DoRemoveMediaL(aCmd);
                break;
                }
            case KMPXCommandIdCollectionCleanupDeletedMedias:
                {
                MPX_DEBUG1("CMPXPodcastDbPlugin::CommandL - KMPXCommandIdCollectionCleanupDeletedMedias");
                CleanupDeletedRecordsL(aCmd);
                break;
                }
            case KMPXCommandIdCollectionAdd:
                {
                MPX_DEBUG1("CMPXPodcastDbPlugin::CommandL - KMPXCommandIdCollectioAdd");
                CMPXMedia* media = aCmd.Value<CMPXMedia>(KMPXCommandColAddMedia);
                User::LeaveIfNull( media );
                TInt id = DoAddL(*media);
                aCmd.SetTObjectValueL<TMPXItemId>(KMPXCommandColAddRtnId, id);
                break;
                }
            case KMPXCommandIdCollectionSet:
                {
                MPX_DEBUG1("CMPXPodcastDbPlugin::CommandL - KMPXCommandIdCollectionSet");
                CMPXMedia* media = aCmd.Value<CMPXMedia>(KMPXCommandColSetMedia);
                User::LeaveIfNull( media );
                DoSetL(*media);
                break;
                }
            case KMPXCommandIdCollectionCompleteDelete:
                {
                MPX_DEBUG1("CMPXPodcastDbPlugin::CommandL - KMPXCommandIdCollectionCompleteDelete");
                DoHandleDeleteCompleteL(aCmd);
                break;
                }
            case KMPXCommandIdUpdateRefreshTime:
                {
                MPX_DEBUG1("CMPXPodcastDbPlugin::CommandL - KMPXCommandIdUpdateRefreshTime");
                TTime curTime;
                curTime.HomeTime();
                iDbHandler->SetLastRefreshedTimeL(curTime);
                break;
                }
            case KMPXCommandCollectionGetCount:
                {
                MPX_DEBUG1("CMPXPodcastDbPlugin::CommandL - KMPXCommandCollectionGetCount");
                DoGetCollectionCountL(aCmd);
                break;
                }
            case KMPXCommandCollectionGetURIs:
                {
                MPX_DEBUG1("CMPXPodcastDbPlugin::CommandL - KMPXCommandCollectionGetURIs");
                DoGetCollectionUriL(aCmd);
                break;
                }
            default:
                {
                User::Leave(KErrNotSupported);
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// Adds a podcast to the collection
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::AddL(
    const CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::AddL");
    DoAddL(aMedia);
    }

// ----------------------------------------------------------------------------
// Remove an item from the collection database using the given path
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::RemoveL(
    const CMPXCollectionPath& aPath)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::RemoveL(by path)");
    MPX_DEBUG_PATH(aPath);

    CMPXMessageArray* msgAry = CMPXMessageArray::NewL();
    CleanupStack::PushL( msgAry );

    // Return file path for deleted item(s)
    CDesCArray* fp = DoRemoveL(aPath,*msgAry);

    iObs->HandleRemove(*fp, KErrNone);
    delete fp;

    // Send Change Messages
    iActiveTask->SetVisibleChange(CMPXDbActiveTask::EAllVisible);
    DoHandleChangeL(msgAry);
    CleanupStack::PopAndDestroy( msgAry );
    }

// ----------------------------------------------------------------------------
// Remove an item from the collection database using the given media properties
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::RemoveL(
    const CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::RemoveL(by media)");
    DoRemoveL(aMedia, EFalse);
    }

// ----------------------------------------------------------------------------
// Sets/updates the media for an item in the collection
// DEPRECATED for week 18
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::SetL(
    const CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::SetL");
    DoSetL(aMedia);
    }

// ----------------------------------------------------------------------------
// Find the items matching the media specifications
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::FindAllL(
    const CMPXMedia& aCriteria,
    const TArray<TMPXAttribute>& aAttrs)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::FindAllL");

    CMPXMedia* entries = FindAllSyncL(aCriteria, aAttrs);

    // notify client. if FindAllL leaves, framework will notify client of the error
    iObs->HandleFindAll(entries, KErrNone);
    delete entries;
    }

// ----------------------------------------------------------------------------
// Find the items matching the media specifications
// ----------------------------------------------------------------------------
//
CMPXMedia* CMPXPodcastDbPlugin::FindAllSyncL(
    const CMPXMedia& aCriteria,
    const TArray<TMPXAttribute>& aAttrs)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::FindAllSyncL");

    CMPXMedia* entries = iDbHandler->FindAllLC(aCriteria, aAttrs);
    CleanupStack::Pop(entries);

    return entries;
    }

// ----------------------------------------------------------------------------
// Get the list of supported capabilities
// ----------------------------------------------------------------------------
//
TCollectionCapability CMPXPodcastDbPlugin::GetCapabilities()
    {
    // This one supports simple search
    return EMcSearch;
    }

// ----------------------------------------------------------------------------
// Get the list of supported capabilities
// ----------------------------------------------------------------------------
//
TBool CMPXPodcastDbPlugin::HandleStepL()
    {
    MPX_FUNC("CMPXPodcastDbPlugin::HandleStepL");

    TBool done(ETrue);

    switch (iActiveTask->GetTask())
        {
        case KMPXCommandIdCollectionSet:
            {
            done = DoSetAsyncL();
            break;
            }
        case KMPXCommandIdCollectionAdd:
            {
            done = DoAddAsyncL();
            break;
            }
        case KMPXCommandIdCollectionRemove:
            {
            DoRemovePathL(iActiveTask->GetCommand());
            done = ETrue;
            break;
            }
        case KMPXCommandIdCollectionRemoveMedia:
            {
            DoRemoveMediaL(iActiveTask->GetCommand());
            done = ETrue;
            break;
            }
        case KMPXCommandIdCollectionRetrieveUriForDeletion:
            {
            DoRetrieveUriForDeletionL(iActiveTask->GetCommand());
            done = ETrue;
            break;
            }
        case KMPXCommandIdCollectionCleanupDeletedMedias:
            {
            CleanupDeletedRecordsL(iActiveTask->GetCommand());
            done = ETrue;
            break;
            }
        case KMPXCommandIdCollectionCompleteDelete:
            {
            DoHandleDeleteCompleteL( iActiveTask->GetCommand() );
            break;
            }
        case KMPXCommandIdUpdateRefreshTime:
            {
            MPX_DEBUG1("CMPXPodcastDbPlugin::CommandL - KMPXCommandIdUpdateRefreshTime");
            TTime curTime;
            curTime.HomeTime();
            iDbHandler->SetLastRefreshedTimeL(curTime);
            break;
            }
        default:
            {
            // Should never happen!
            ASSERT(0);
            break;
            }
        }
    return done;
    }

// ----------------------------------------------------------------------------
// Handler for async operations completed
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::HandleOperationCompleted(
    TInt aErr)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::HandleOperationCompleted");
    TRAP_IGNORE(DoHandleOperationCompletedL(aErr));
    }

// ----------------------------------------------------------------------------
// Process the OpenL command
// ----------------------------------------------------------------------------
//
TBool CMPXPodcastDbPlugin::DoOpenL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
	CMPXMedia& aEntries,
	TBool aFlagToSignalToBePlayed)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoOpenL");

    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);

    TInt levels(aPath.Levels());
    TBool isEpisode(EFalse);

    aEntries.SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, aPath.Id(levels - 1));

    if (1 == levels)
        {
        isEpisode = DoOpenBrowseTitleL(aPath, aAttrs, aEntries, *array);
        }
    else if (levels >= 2)
        {
        isEpisode = DoOpenBrowseTitleL(aPath, aAttrs, aEntries, *array);
        }
    else
        {
        User::Leave(KErrNotSupported);
        }

    if(isEpisode &&
       aFlagToSignalToBePlayed)
        {
        // opening an episode to be played so set the IsPlaying flag to
        // prevent this episode from being picked up as Not Yet Played
        // (need to do this because setting last playback position causes
        // visible change and the playlist to update)
        if(array->Count() == 1)
            {
            // ignore the error because if there is a problem
            // updating the media file, the error will show up
            // when opening the track for playing and be handled
            // properly by the playback engine and skipped
            TRAP_IGNORE(iDbHandler->SetIsPlayingL(*((*array)[0]), ETrue));
            }
        }

    aEntries.SetCObjectValueL(KMPXMediaArrayContents, array);
    aEntries.SetTObjectValueL(KMPXMediaArrayCount, array->Count());

    CleanupStack::PopAndDestroy(array);

    return isEpisode;
    }

// ----------------------------------------------------------------------------
// Handles OpenL called for EBrowseAll
// ----------------------------------------------------------------------------
//
TBool CMPXPodcastDbPlugin::DoOpenBrowseAllL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aEntries,
    CMPXMediaArray& aArray)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoOpenBrowseAllL");

    TInt levels(aPath.Levels());
    switch (levels)
       {
       // All Episodes
       case 2:
            {
            MPX_PERF_START(CMPXPodcastDbPlugin_DoOpenBrowseAllL_All);

            iDbHandler->GetAllEpisodesL(aAttrs, aArray);
            SetMediaGeneralAttributesL(aEntries, EMPXGroup, EMPXPodcastGroup, EMPXAll,
                *iTitleAllEpisodes, aArray.Count());

            MPX_PERF_END(CMPXPodcastDbPlugin_DoOpenBrowseAllL_All);
            break;
            }

         // An episode in all episodes
         case 3:
            {
            MPX_PERF_START(CMPXPodcastDbPlugin_DoOpenBrowseAllL_Episode);

            iDbHandler->GetEpisodeL(aPath.Id(levels - 1), aAttrs, aArray);

            MPX_PERF_END(CMPXPodcastDbPlugin_DoOpenBrowseAllL_Episode);
            break;
            }

         default:
            {
            MPX_DEBUG2("CMPXPodcastDbPlugin_DoOpenBrowseAllL: Invalid levels[%d]", levels);
            User::Leave(KErrNotSupported);
            }
        }

    return (levels == 3);
    }

// ----------------------------------------------------------------------------
// Handles OpenL called for EBrowsePubDate
// ----------------------------------------------------------------------------
//
TBool CMPXPodcastDbPlugin::DoOpenBrowsePubDateL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aEntries,
    CMPXMediaArray& aArray)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoOpenBrowsePubDateL");

    TBool isEpisode(EFalse);
    TInt levels(aPath.Levels());

    switch (levels)
        {
        // All By Publish Date Categories
        case 2:
            {
            MPX_PERF_START(CMPXPodcastDbPlugin_DoOpenBrowsePubDateL_All);

            TInt numEpisodes(iDbHandler->GetEpisodesMatchingPublishPlaylistL(KPublishAllPlaylistUID, aAttrs,
                    EFalse, aArray));
            SetMediaGeneralAttributesL(aEntries, EMPXGroup, EMPXPodcastGroup, EMPXPubDate,
                *iTitlePubDate);
            aEntries.SetTObjectValueL(KMPXMediaGeneralNonPermissibleActions, EMPXCache );
            MPX_PERF_END(CMPXPodcastDbPlugin_DoOpenBrowsePubDateL_All);
            break;
            }
        // All episodes within a specific By Publish Date category
        case 3:
            {
            MPX_PERF_START(CMPXPodcastDbPlugin_DoOpenBrowsePubDateL_Playlist);
            iNumberOfEpisodesInCurrentPublishDateCategory =
                iDbHandler->GetEpisodesMatchingPublishPlaylistL(
                (aPath.Id(levels - 1).iId2 & 0x00FFFFFF) + KPublishTodayPlaylistUID, // offset by KPublishTodayPlaylistUID
                aAttrs, EFalse, aArray);

            SetMediaGeneralAttributesL(aEntries, EMPXItem, EMPXPodcastItem, EMPXEpisode,
                iPodcastEpisodeViewPublishDateTitle->MdcaPoint(aPath.Id(levels - 1).iId2 & 0x00FFFFFF),
                iNumberOfEpisodesInCurrentPublishDateCategory);

            MPX_PERF_END(CMPXPodcastDbPlugin_DoOpenBrowsePubDateL_Playlist);
            break;
            }
        // An episode
        case 4:
            {
            MPX_PERF_START(CMPXPodcastDbPlugin_DoOpenBrowsePubDateL_Episode);

            iDbHandler->GetEpisodeL(aPath.Id(levels - 1), aAttrs, aArray);
            isEpisode = ETrue;

            MPX_PERF_END(CMPXPodcastDbPlugin_DoOpenBrowsePubDateL_Episode);
            break;
            }
        default:
            {
            MPX_DEBUG2("CMPXPodcastDbPlugin_DoOpenBrowsePubDateL: Invalid levels[%d]", levels);
            User::Leave(KErrNotSupported);
            }
        }
    return isEpisode;
    }

// ----------------------------------------------------------------------------
// Handles OpenL called for EBrowseTitle
// ----------------------------------------------------------------------------
//
TBool CMPXPodcastDbPlugin::DoOpenBrowseTitleL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aEntries,
    CMPXMediaArray& aArray)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoOpenBrowseTitleL");

    TBool isEpisode(EFalse);
    TInt levels(aPath.Levels());

    switch (levels)
         {
         // All Titles
		 case 1:
            {
            MPX_PERF_START(CMPXPodcastDbPlugin_DoOpenBrowseTitleL_All);

            iDbHandler->GetAllPodcastTitlesL(aAttrs, aArray);

            SetMediaGeneralAttributesL(aEntries, EMPXGroup, EMPXPodcastGroup, EMPXTitle,
                *iTitleTitles);

            MPX_PERF_END(CMPXPodcastDbPlugin_DoOpenBrowseTitleL_All);
            break;
            }
         // All episodes in a title
		 case 2:
            {
            MPX_PERF_START(CMPXPodcastDbPlugin_DoOpenBrowseTitleL_Title);

            iDbHandler->GetEpisodesMatchingTitleL(aPath.Id(levels - 1).iId2,
                aAttrs, aArray);

            HBufC* title = iDbHandler->GetTitleNameMatchingIdL(aPath.Id(levels - 1));
            CleanupStack::PushL(title);
            SetMediaGeneralAttributesL(aEntries, EMPXItem, EMPXPodcastItem, EMPXEpisode,
                *title, aArray.Count());
            CleanupStack::PopAndDestroy(title);

            MPX_PERF_END(CMPXPodcastDbPlugin_DoOpenBrowseTitleL_Title);
            break;
            }
         // An episode within a title
         case 3:
            {
            MPX_PERF_START(CMPXPodcastDbPlugin_DoOpenBrowseTitleL_Episode);
            iDbHandler->GetEpisodeL(aPath.Id(levels - 1), aAttrs, aArray);
            isEpisode = ETrue;

            MPX_PERF_END(CMPXPodcastDbPlugin_DoOpenBrowseTitleL_Episode);
            break;
            }

         default:
            {
            MPX_DEBUG2("CMPXPodcastDbPlugin_DoOpenBrowseTitleL: Invalid levels[%d]", levels);
            User::Leave(KErrNotSupported);
            }
       }

    return isEpisode;
    }

// ----------------------------------------------------------------------------
// Handles OpenL called for EBrowseRecentlyAdded
// ----------------------------------------------------------------------------
//
TBool CMPXPodcastDbPlugin::DoOpenBrowseRecentlyAddedL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aEntries,
    CMPXMediaArray& aArray)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoOpenBrowseRecentlyAddedL");

    TBool isEpisode(EFalse);
    TInt levels(aPath.Levels());

    switch (levels)
         {
         case 2:
            {
            // All recently added episodes
            MPX_PERF_START(CMPXPodcastDbPlugin_DoOpenBrowseRecentlyAddedL_All);

            TInt indexOfCurrentlyPlayingItem(KErrNotFound);
            iDbHandler->GetEpisodesMatchingPlaylistL(KRecentlyAddedPlaylistUID,
                    aAttrs, aArray, indexOfCurrentlyPlayingItem);
            SetMediaGeneralAttributesL(aEntries, EMPXGroup, EMPXPodcastGroup, EMPXRecentlyAdded,
                *iTitleAdded, aArray.Count());
            aEntries.SetTObjectValueL(KMPXMediaPodcastCurrentlyPlayingIndex,
                indexOfCurrentlyPlayingItem);
            // Fix for Autoplaylist, set the permission to not writable and cacheable
            aEntries.SetTObjectValueL<TMPXGeneralNonPermissibleActions>(
                KMPXMediaGeneralNonPermissibleActions, (TMPXGeneralNonPermissibleActions)(EMPXWrite | EMPXCache));

            MPX_PERF_END(CMPXPodcastDbPlugin_DoOpenBrowseRecentlyAddedL_All);
            break;
            }
         // An episode in the recently added episodes list
         case 3:
            {
            MPX_PERF_START(CMPXPodcastDbPlugin_DoOpenBrowseRecentlyAddedL_Episode);
            iDbHandler->GetEpisodeL(aPath.Id(levels - 1), aAttrs, aArray);
            isEpisode = ETrue;

            MPX_PERF_END(CMPXPodcastDbPlugin_DoOpenBrowseRecentlyAddedL_Episode);
            break;
            }
         default:
            {
            MPX_DEBUG2("CMPXPodcastDbPlugin_DoOpenBrowseRecentlyAddedL: Invalid levels[%d]", levels);
            User::Leave(KErrNotSupported);
            }
        }

    return isEpisode;
    }

// ----------------------------------------------------------------------------
// Handles OpenL called for EBrowseNotPlayed
// ----------------------------------------------------------------------------
//
TBool CMPXPodcastDbPlugin::DoOpenBrowseNotPlayedL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aEntries,
    CMPXMediaArray& aArray)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoOpenBrowseNotPlayedL");

    TBool isEpisode(EFalse);
    TInt levels(aPath.Levels());

    switch (levels)
         {
         case 2:
            {
            // All episodes that haven't been played
            MPX_PERF_START(CMPXPodcastDbPlugin_DoOpenBrowseNotPlayedL_All);

            TInt indexOfCurrentlyPlayingItem(KErrNotFound);
            iDbHandler->GetEpisodesMatchingPlaylistL(KNotPlayedPlaylistUID, aAttrs, aArray,
                indexOfCurrentlyPlayingItem);
            SetMediaGeneralAttributesL(aEntries, EMPXGroup, EMPXPodcastGroup, EMPXNotYetPlayed,
                *iTitleUnplayed, aArray.Count());
            aEntries.SetTObjectValueL(KMPXMediaPodcastCurrentlyPlayingIndex,
                indexOfCurrentlyPlayingItem);
            // Fix for Autoplaylist, set the permission to not writable and cacheable
            aEntries.SetTObjectValueL<TMPXGeneralNonPermissibleActions>(
                KMPXMediaGeneralNonPermissibleActions, (TMPXGeneralNonPermissibleActions)(EMPXWrite | EMPXCache));

            MPX_PERF_END(CMPXPodcastDbPlugin_DoOpenBrowseNotPlayedL_All);
            break;
            }
         // An episode in the recently added episodes list
         case 3:
            {
            MPX_PERF_START(CMPXPodcastDbPlugin_DoOpenBrowseNotPlayedL_Episode);
            iDbHandler->GetEpisodeL(aPath.Id(levels - 1), aAttrs, aArray);
            isEpisode = ETrue;

            MPX_PERF_END(CMPXPodcastDbPlugin_DoOpenBrowseNotPlayedL_Episode);
            break;
            }
         default:
            {
            MPX_DEBUG2("CMPXPodcastDbPlugin_DoOpenBrowseNotPlayedL: Invalid levels[%d]", levels);
            User::Leave(KErrNotSupported);
            }
       }

    return isEpisode;
    }

// ----------------------------------------------------------------------------
// Process the OpenL method with open mode EMPXOpenPlaylistOnly
// ----------------------------------------------------------------------------
//
CMPXCollectionPath* CMPXPodcastDbPlugin::DoOpenPlaylistL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
    TBool aFlagToSignalToBePlayed)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoOpenPlaylistL");

    RArray<TMPXItemId> ids;
    CleanupClosePushL(ids);

    CMPXMedia* entries = CMPXMedia::NewL();
    CleanupStack::PushL(entries);

    CMPXCollectionPath* path = CMPXCollectionPath::NewL( aPath );
    CleanupStack::PushL( path );

    // Go through the browse path
    TInt levels(aPath.Levels());
    if (levels == 2)
        {
        // Create a new collection path
        CleanupStack::PopAndDestroy(path);
        path = CMPXCollectionPath::NewL();
        CleanupStack::PushL(path);

        // Always return all episodes here
        //
        ids.Reset();
        ids.AppendL( KDBPluginUid );
        path->AppendL(ids.Array());
        path->SelectL((TMPXItemId)KDBPluginUid);

        ids.Reset();
        ids.AppendL(EBrowseAll);
        path->AppendL(ids.Array());
        path->SelectL((TMPXItemId) EBrowseAll);
        path->Set(EMPXOpenPlaylistOnly);

        // Get all item IDs
        CMPXMediaArray* array = CMPXMediaArray::NewL();
        CleanupStack::PushL(array);

        DoOpenBrowseAllL(*path, aAttrs, *entries, *array);

        entries->SetCObjectValueL(KMPXMediaArrayContents, array);
        entries->SetTObjectValueL<TInt>(KMPXMediaArrayCount, array->Count());

        CleanupStack::PopAndDestroy(array);

        DoAppendLevelL(*path, *entries);
        }
    else if (levels > 2)
        {
        TInt selectedId(aPath.Id(1));
        switch (selectedId)
            {
            case EBrowseAll:
                {
                path->Set(EMPXOpenPlaylistOnly);
                if(aFlagToSignalToBePlayed)
                    {
                    // Set the episode ID to be played
                    entries->SetTObjectValueL( KMPXMediaGeneralId, aPath.Id(2) );

                    // ignore the error because if there is a problem
                    // updating the media file, the error will show up
                    // when opening the track for playing and be handled
                    // properly by the playback engine and skipped
                    TRAP_IGNORE(iDbHandler->SetIsPlayingL(*entries, ETrue));
                    }
                break;
                }
            case EBrowseTitle:
            case EBrowsePubDate:
            case EBrowseRecentlyAdded:
            case EBrowseNotPlayed:
                {
                if (!DoOpenL(aPath, aAttrs, *entries, aFlagToSignalToBePlayed))
                    {
                    path->Set(EMPXOpenPlaylistOnly);
                    // If it is not at a episode level
                    // Append all entries to create collection path
                    //
                    DoAppendLevelL(*path, *entries);
                    }
                break;
                }
            default:
                {
                User::Leave(KErrNotSupported);
                }
            }
        }
    else  // levels < 2
        {
        User::Leave(KErrNotSupported);
        }

    // Cleanup
    CleanupStack::Pop(path);
    CleanupStack::PopAndDestroy(entries);
    CleanupStack::PopAndDestroy(&ids);

    return path;
    }

// ----------------------------------------------------------------------------
// Process the MediaL command
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::DoMediaL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aEntries)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoMediaL");

	CMPXMediaArray* array = CMPXMediaArray::NewL();
	CleanupStack::PushL(array);

	DoTitlesMediaL(aPath, aAttrs, aEntries, *array);

	if (array->Count() > 0)
		{
		aEntries.SetCObjectValueL(KMPXMediaArrayContents, array);
		aEntries.SetTObjectValueL<TInt>(KMPXMediaArrayCount, array->Count());
		}
	CleanupStack::PopAndDestroy(array);
    }

// ----------------------------------------------------------------------------
// Find the collection media for root level
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::DoRootMediaL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoRootMediaL");
#ifndef __ENABLE_PODCAST_IN_MUSIC_MENU
    aMedia.SetTObjectValueL<TMPXGeneralNonPermissibleActions> (
            KMPXMediaGeneralNonPermissibleActions, (TMPXGeneralNonPermissibleActions)(EMPXWrite | EMPXCache) );
#endif // __ENABLE_PODCAST_IN_MUSIC_MENU
    TInt count(aAttrs.Count());
    for (TInt i = 0; i < count; ++i)
        {
        if (aAttrs[i].ContentId() == KMPXMediaIdGeneral)
            {
            TUint att = aAttrs[i].AttributeId();

            if (att & EMPXMediaGeneralTitle)
                {
                // set the collection plugin name
                HBufC* title(iResource->ReadHBufCL(R_MPX_QTN_MUS_PODCASTS));
                CleanupStack::PushL(title);
                aMedia.SetTextValueL(KMPXMediaGeneralTitle, *title);
                CleanupStack::PopAndDestroy(title);
                }
            if (att & EMPXMediaGeneralSubTitle)
                {
                TInt numEpisodes(iDbHandler->NumberOfItemsL(EMPXEpisode));

                HBufC* text(iResource->ReadHBufCL((numEpisodes == 1) ?
                    R_MPX_QTN_MUS_PODCAST_ONE_EPISODE : R_MPX_QTN_MUS_PODCAST_NUM_EPISODES));
                CleanupStack::PushL(text);
                aMedia.SetTextValueL(KMPXMediaGeneralSubTitle, *text);
                aMedia.SetTObjectValueL<TInt>(KMPXMediaGeneralCount, numEpisodes);
                CleanupStack::PopAndDestroy(text);
                }
            if (att & EMPXMediaGeneralIcon)
                {
/*
                // set the collection plugin icon
                TIconInfo icon;
                icon.bmpfile = KMPlayerDbPluginMbmFile;
                icon.bitmapId = EMbmMpxpodcastdbpluginQgn_graf_mup_dlst_podcast;
                icon.maskId = EMbmMpxpodcastdbpluginQgn_graf_mup_dlst_podcast_mask;
                aMedia.SetTObjectValueL<TIconInfo>(KMPXMediaGeneralIcon, icon );
*/
                }
            } // if
        } // for
    }

// ----------------------------------------------------------------------------
// Find the collection media for all episodes category
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::DoAllEpisodesMediaL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aEntries,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoAllEpisodesMediaL");

    TInt levels(aPath.Levels());
    switch (levels)
       {
        // All episodes
        case 2:
            {
            MPX_PERF_START(CMPXPodcastDbPlugin_DoAllEpisodesMediaL_All);
            DoRootCategoryMediaL(aAttrs, aPath.Id(1), EMPXAll, aEntries);
            MPX_PERF_END(CMPXPodcastDbPlugin_DoAllEpisodesMediaL_All);
            break;
            }
        // An episode in all episodes
        case 3:
            {
            MPX_PERF_START(CMPXPodcastDbPlugin_DoAllEpisodesMediaL_Episode);
            GetEpisodeInfoL(aPath, aAttrs, aEntries, aMediaArray);
            MPX_PERF_END(CMPXPodcastDbPlugin_DoAllEpisodesMediaL_Episode);
            break;
            }
        default:
            {
            MPX_DEBUG2("CMPXPodcastDbPlugin_DoAllEpisodesMediaL: Invalid levels[%d]", levels);
            User::Leave(KErrNotSupported);
            }
        } // end switch(levels)
    }

// ----------------------------------------------------------------------------
// Find the collection media for by publish date category
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::DoByPublishDateMediaL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aEntries,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoByPublishDateMediaL");

    TInt levels(aPath.Levels());
    TInt idIndex(levels - 1);

    // All By Publish Date Categories
    if (levels == 2)
        {
        MPX_PERF_START(CMPXPodcastDbPlugin_DoByPublishDateMediaL_All);
        DoRootCategoryMediaL(aAttrs, aPath.Id(1), EMPXPubDate, aEntries);
        MPX_PERF_END(CMPXPodcastDbPlugin_DoByPublishDateMediaL_All);
        }
    else if (levels == 3) // by publish date category selected
        {
        MPX_PERF_START(CMPXPodcastDbPlugin_DoByPublishDateMediaL_Category);
        const TDesC& title = iPodcastEpisodeViewPublishDateTitle->MdcaPoint(
            aPath.Id(idIndex).iId2 & 0x00FFFFFF);
        aEntries.SetTextValueL(KMPXMediaGeneralTitle, title);
        MPX_PERF_END(CMPXPodcastDbPlugin_DoByPublishDateMediaL_Category);
        }
     else if (levels == 4) // an episode within the category
        {
        MPX_PERF_START(CMPXPodcastDbPlugin_DoByPublishDateMediaL_Episode);
        GetEpisodeInfoL(aPath, aAttrs, aEntries, aMediaArray);
        MPX_PERF_END(CMPXPodcastDbPlugin_DoByPublishDateMediaL_Episode);
        }
    else
        {
        MPX_DEBUG2("CMPXPodcastDbPlugin_DoByPublishDateMediaL: Invalid levels[%d]", levels);
        User::Leave(KErrNotSupported);
        }
    }

// ----------------------------------------------------------------------------
// Find the collection media for titles category
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::DoTitlesMediaL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aEntries,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoTitlesMediaL");

    TInt levels(aPath.Levels());
    TInt idIndex(levels - 1);

     // All Titles Categories
    if (levels == 1)
        {
        MPX_PERF_START(CMPXPodcastDbPlugin_DoTitlesMediaL_All);
        DoRootCategoryMediaL(aAttrs, aPath.Id(1), EMPXTitle, aEntries);
        MPX_PERF_END(CMPXPodcastDbPlugin_DoTitlesMediaL_All);
        }
    else if (levels == 2) // All episodes within selected title
        {
        MPX_PERF_START (CMPXPodcastDbPlugin_DoTitlesMediaL_Title);
        iDbHandler->GetAllPodcastTitlesL(aAttrs, aMediaArray);
        HBufC* title = iDbHandler->GetTitleNameMatchingIdL(aPath.Id(idIndex));
        CleanupStack::PushL(title);
        aEntries.SetTextValueL(KMPXMediaGeneralTitle, *title);
        CleanupStack::PopAndDestroy(title);
        MPX_PERF_END (CMPXPodcastDbPlugin_DoTitlesMediaL_Title);
        }
     else if (levels == 3) // an episode within a selected title
        {
        MPX_PERF_START(CMPXPodcastDbPlugin_DoTitlesMediaL_Episode);
        GetEpisodeInfoL (aPath, aAttrs, aEntries, aMediaArray);
        MPX_PERF_END(CMPXPodcastDbPlugin_DoTitlesMediaL_Episode);
        }
     else
        {
        MPX_DEBUG2("CMPXPodcastDbPlugin_DoTitlesMediaL: Invalid levels[%d]", levels);
        User::Leave(KErrNotSupported);
        }
    }

// ----------------------------------------------------------------------------
// Find the collection media for recently added
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::DoRecentlyAddedMediaL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aEntries,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoRecentlyAddedMediaL");

    switch (aPath.Levels())
       {
       // All episodes
       case 2:
            {
            MPX_PERF_START(CMPXPodcastDbPlugin_DoRecentlyAddedMediaL_All);
            DoRootCategoryMediaL(aAttrs, aPath.Id(1), EMPXRecentlyAdded, aEntries);
            MPX_PERF_END(CMPXPodcastDbPlugin_DoRecentlyAddedMediaL_All);
            break;
            }

         // An episode that was recently added
         case 3:
            {
            MPX_PERF_START(CMPXPodcastDbPlugin_DoRecentlyAddedMediaL_Episode);
            GetEpisodeInfoL(aPath, aAttrs, aEntries, aMediaArray);
            MPX_PERF_END(CMPXPodcastDbPlugin_DoRecentlyAddedMediaL_Episode);
            break;
            }

         default:
            {
            MPX_DEBUG2("CMPXPodcastDbPlugin_DoRecentlyAddedMediaL: Invalid levels[%d]", aPath.Levels());
            User::Leave(KErrNotSupported);
            }
        } // end switch(levels)
    }

// ----------------------------------------------------------------------------
// Find the collection media for recently added
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::DoNotYetPlayedMediaL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aEntries,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoNotYetPlayedMediaL");

    switch (aPath.Levels())
       {
       // All episodes
       case 2:
            {
            MPX_PERF_START(CMPXPodcastDbPlugin_DoNotYetPlayedMediaL_All);
            DoRootCategoryMediaL(aAttrs, aPath.Id(1), EMPXNotYetPlayed, aEntries);
            MPX_PERF_END(CMPXPodcastDbPlugin_DoNotYetPlayedMediaL_All);
            break;
            }

         // An episode that was never played
         case 3:
            {
            MPX_PERF_START(CMPXPodcastDbPlugin_DoNotYetPlayedMediaL_Episode);
            MPX_TRAPD(err, GetEpisodeInfoL(aPath, aAttrs, aEntries, aMediaArray));

            if (err != KErrNotFound)
                {
                // it's o.k if the episode isn't found because the episode
                // might have finished playing and is no longer
                // part of the "Not yet played" playlist
                User::LeaveIfError(err);
                }

            MPX_PERF_END(CMPXPodcastDbPlugin_DoNotYetPlayedMediaL_Episode);
            break;
            }

         default:
            {
            MPX_DEBUG2("CMPXPodcastDbPlugin_DoNotYetPlayedMediaL: Invalid levels[%d]", aPath.Levels());
            User::Leave(KErrNotSupported);
            }
        } // end switch(levels)
    }

// ----------------------------------------------------------------------------
// Find the collection media for the root menu
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::DoRootCategoryMediaL(
    const TArray<TMPXAttribute>& aAttrs,
    TMPXItemId aRootCategoryId,
    TMPXPodcastCategory aCategory,
    CMPXMedia& aEntries)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoRootCategoryMediaL");

    TInt count(aAttrs.Count());
    for (TInt i = 0; i < count; ++i)
        {
        if (aAttrs[i].ContentId() == KMPXMediaIdGeneral)
            {
            TUint att(aAttrs[i].AttributeId());

            if (att & EMPXMediaGeneralId)
                {
                aEntries.SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId,
                    aRootCategoryId);
                }
            if (att & EMPXMediaGeneralTitle)
                {
                aEntries.SetTextValueL(KMPXMediaGeneralTitle,
                    iPodcastLibraryTitles->MdcaPoint(BrowseTypeForCategory(aCategory)));
                }
            } // end if
        } // end for

    aEntries.SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXGroup);
    aEntries.SetTObjectValueL<TMPXPodcastCategory>(KMPXMediaGeneralCategory, aCategory);
    }

// ----------------------------------------------------------------------------
// Set all the attributes in CMPXMedia corresponding to KMPXMediaIdDrm
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::DoSetMediaDrmL(
    CMPXMedia& aMedia,
    TUint aDrmAttributes,
    const TDesC& aLocation)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoSetMediaDrmL");

    iDrmMediaUtility->InitL(aLocation);
    CleanupClosePushL(*iDrmMediaUtility);
    const CMPXMedia* drmMedia(iDrmMediaUtility->GetMediaL(aDrmAttributes));

    // Only get attributes if it's a DRM file
    if (drmMedia)
        {
        if ((aDrmAttributes & EMPXMediaDrmType) &&
            drmMedia->IsSupported(KMPXMediaDrmType))
            {
            aMedia.SetTObjectValueL(KMPXMediaDrmType,
                drmMedia->ValueTObjectL<TInt>(KMPXMediaDrmType));
            }
        if ((aDrmAttributes & EMPXMediaDrmRightsStatus) &&
            drmMedia->IsSupported(KMPXMediaDrmRightsStatus))
            {
            aMedia.SetTObjectValueL(KMPXMediaDrmRightsStatus,
                 drmMedia->ValueTObjectL<TInt>(KMPXMediaDrmRightsStatus));
            }
        if ((aDrmAttributes & EMPXMediaDrmRightsType) &&
            drmMedia->IsSupported(KMPXMediaDrmRightsType))
            {
            aMedia.SetTObjectValueL(KMPXMediaDrmRightsType,
                 drmMedia->ValueTObjectL<TInt>(KMPXMediaDrmRightsType));
            }
        if ((aDrmAttributes & EMPXMediaDrmCount) &&
            drmMedia->IsSupported(KMPXMediaDrmCount))
            {
            aMedia.SetTObjectValueL(KMPXMediaDrmCount,
                 drmMedia->ValueTObjectL<TInt>(KMPXMediaDrmCount));
            }
        if ((aDrmAttributes & EMPXMediaDrmProtected) &&
            drmMedia->IsSupported(KMPXMediaDrmProtected))
            {
            aMedia.SetTObjectValueL(KMPXMediaDrmProtected,
                 drmMedia->ValueTObjectL<TBool>(KMPXMediaDrmProtected));
            }
        if ((aDrmAttributes & EMPXMediaDrmSendingAllowed) &&
            drmMedia->IsSupported(KMPXMediaDrmSendingAllowed))
            {
            aMedia.SetTObjectValueL(KMPXMediaDrmSendingAllowed,
                 drmMedia->ValueTObjectL<TBool>(KMPXMediaDrmSendingAllowed));
            }
        if ((aDrmAttributes & EMPXMediaDrmCanSetAutomated) &&
            drmMedia->IsSupported(KMPXMediaDrmCanSetAutomated))
            {
            aMedia.SetTObjectValueL(KMPXMediaDrmCanSetAutomated,
                 drmMedia->ValueTObjectL<TBool>(KMPXMediaDrmCanSetAutomated));
            }
        if ((aDrmAttributes & EMPXMediaDrmHasInfoUrl) &&
            drmMedia->IsSupported(KMPXMediaDrmHasInfoUrl))
            {
            aMedia.SetTObjectValueL(KMPXMediaDrmHasInfoUrl,
                 drmMedia->ValueTObjectL<TBool>(KMPXMediaDrmHasInfoUrl));
            }
        if ((aDrmAttributes & EMPXMediaDrmHasPreviewUrl) &&
            drmMedia->IsSupported(KMPXMediaDrmHasPreviewUrl))
            {
            aMedia.SetTObjectValueL(KMPXMediaDrmHasPreviewUrl,
                 drmMedia->ValueTObjectL<TBool>(KMPXMediaDrmHasPreviewUrl));
            }
        if ((aDrmAttributes & EMPXMediaDrmAboutToExpire) &&
            drmMedia->IsSupported(KMPXMediaDrmAboutToExpire))
            {
            aMedia.SetTObjectValueL( KMPXMediaDrmAboutToExpire,
                 drmMedia->ValueTObjectL<TBool>(KMPXMediaDrmAboutToExpire));
            }
        if ((aDrmAttributes & EMPXMediaDrmStartTime) &&
            drmMedia->IsSupported(KMPXMediaDrmStartTime))
            {
            aMedia.SetTObjectValueL(KMPXMediaDrmStartTime,
                 drmMedia->ValueTObjectL<TInt64>(KMPXMediaDrmStartTime));
            }
        if ((aDrmAttributes & EMPXMediaDrmEndTime) &&
            drmMedia->IsSupported(KMPXMediaDrmEndTime))
            {
            aMedia.SetTObjectValueL( KMPXMediaDrmEndTime,
                 drmMedia->ValueTObjectL<TInt64>(KMPXMediaDrmEndTime));
            }
        if ((aDrmAttributes & EMPXMediaDrmIntervalStartTime) &&
            drmMedia->IsSupported(KMPXMediaDrmIntervalStartTime))
            {
            aMedia.SetTObjectValueL( KMPXMediaDrmIntervalStartTime,
                 drmMedia->ValueTObjectL<TInt64>(KMPXMediaDrmIntervalStartTime));
            }
        if ((aDrmAttributes & EMPXMediaDrmAccumulatedTime) &&
            drmMedia->IsSupported(KMPXMediaDrmAccumulatedTime))
            {
            aMedia.SetTObjectValueL(KMPXMediaDrmAccumulatedTime,
                 drmMedia->ValueTObjectL<TInt64>(KMPXMediaDrmAccumulatedTime));
            }
        if ((aDrmAttributes & EMPXMediaDrmInterval) &&
            drmMedia->IsSupported(KMPXMediaDrmInterval))
            {
            aMedia.SetTObjectValueL( KMPXMediaDrmInterval,
                 drmMedia->ValueTObjectL<TTimeIntervalSeconds>(KMPXMediaDrmInterval));
            }
        }

    CleanupStack::PopAndDestroy(iDrmMediaUtility);
    }

// ----------------------------------------------------------------------------
// Add media objects to the array with attributes from episode details
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::GetEpisodeInfoL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aEntry,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::GetEpisodeInfoL");

    RArray<TInt> supportedIds;
    CleanupClosePushL(supportedIds);
    MPXDbCommonUtil::FillInSupportedUIDsL(aAttrs, supportedIds);

    RArray<TMPXItemId> selections;
    CleanupClosePushL(selections);
    aPath.SelectionL(selections);

    TInt countSelection(aPath.Selection().Count());
    if (countSelection)
        {
        for (TInt selectionIndex = 0; selectionIndex < countSelection; ++selectionIndex)
            {
            CMPXMedia* newEntry = CMPXMedia::NewL(supportedIds.Array());
            CleanupStack::PushL(newEntry);

            DoGetEpisodeInfoL(aAttrs, selections[selectionIndex].iId2, *newEntry);

            aMediaArray.AppendL(*newEntry);
            CleanupStack::PopAndDestroy(newEntry);
            }
        }
    else
        {
        // No selection, get the attributes for the one song
        DoGetEpisodeInfoL(aAttrs, aPath.Id(aPath.Levels() - 1).iId2, aEntry);
        }

    CleanupStack::PopAndDestroy(&selections);
    CleanupStack::PopAndDestroy(&supportedIds);
    }

// ----------------------------------------------------------------------------
// Retrieves the attributes for a media object.
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::DoGetEpisodeInfoL(
    const TArray<TMPXAttribute>& aAttrs,
    TInt aEntryId,
    CMPXMedia& aEntry)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoGetEpisodeInfoL");

    iDbHandler->GetEpisodeL(aEntryId, aAttrs, aEntry);

    const TDesC& location(aEntry.ValueText(KMPXMediaGeneralUri));

    // Check DRM Only if we have a location
    if (location != KNullDesC)
        {
        TUint drmAttributes(0);

        // Compact the attribute set
        TInt count(aAttrs.Count());
        for (TInt i = 0; i < count; ++i)
            {
            if (aAttrs[i].ContentId() == KMPXMediaIdDrm)
                {
                drmAttributes |= aAttrs[i].AttributeId();
                }
            }

        // Set the correct attributes to media, only if requested
        if (drmAttributes)
            {
            DoSetMediaDrmL(aEntry, drmAttributes, location);
            }
        }
    }

// ----------------------------------------------------------------------------
// Retrieve the collection details
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::DoHandleOtherMediaAttributesL(
    const TArray<TMPXAttribute>& aAttrs,
    const CMPXCollectionPath& aPath,
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoHandleOtherMediaAttributesL");

    TInt count(aAttrs.Count());
    for (TInt i = 0; i < count; ++i)
        {
        if (aAttrs[i].ContentId() == KMPXMediaIdCollectionDetails)
            {
            TUint att(aAttrs[i].AttributeId());

            if (att & EMPXMediaColDetailNumberOfItems)
                {
                aMedia.SetTObjectValueL(KMPXMediaColDetailNumberOfItems,
                    iDbHandler->NumberOfItemsL(EMPXEpisode) );
                }
            if (att & EMPXMediaColDetailDuration)
                {
                aMedia.SetTObjectValueL(KMPXMediaColDetailDuration,
                    DoDurationL( aMedia, EMPXEpisode ) );
                }
            if (att & EMPXMediaColTotalSize)
                {
                TInt totalSize(0);
                // todo
                aMedia.SetTObjectValueL(KMPXMediaColDetailTotalSize, totalSize );
                }
            if (att & EMPXMediaLastRefreshed)
                {
                TTime lastRefreshed = iDbHandler->GetLastRefreshedTimeL();
                aMedia.SetTObjectValueL(KMPXMediaColDetailLastRefreshed,
                    lastRefreshed.Int64() );
                }
            if (att & EMPXMediaColDetailDBCreated)
                {
                aMedia.SetTObjectValueL(KMPXMediaColDetailDBCreated,
                    iDbHandler->DatabaseCreated());
                }
            if (att & EMPXMediaColDetailDBCorrupted)
                {
                aMedia.SetTObjectValueL(KMPXMediaColDetailDBCorrupted,
                    iDbHandler->IsDBCorruptedL());
                }
            }
        else if (aAttrs[i] == KMPXMediaGeneralPath)
            {
            aMedia.SetCObjectValueL(KMPXMediaGeneralPath,
                const_cast<CMPXCollectionPath*>(&aPath));
            }
        }
    }

// ----------------------------------------------------------------------------
// Remove an item from the collection database using the given path
// ----------------------------------------------------------------------------
//
CDesCArray* CMPXPodcastDbPlugin::DoRemoveL(
    const CMPXCollectionPath& aPath,
    CMPXMessageArray& aChangeMsgArray)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoRemoveL");

    if (aPath.Levels() <= 0)
        {
        User::Leave(KErrNotSupported);
        }

    // Return file path for deleted item(s)
    //
    CDesCArray* fp = new(ELeave) CDesCArrayFlat(1);
    CleanupStack::PushL(fp);

    // Ids of the selected items
    RArray<TMPXItemId> selections;
    CleanupClosePushL(selections);
    aPath.SelectionL(selections);

    DoRemoveFromCategoriesL(aPath, selections.Array(), EMPXAlbum, *fp, aChangeMsgArray);

    MPX_DEBUG2("CMPXPodcastDbPlugin::DoRemoveL itemId[%d]", aPath.Id (aPath.Levels() - 1).iId2);

    CleanupStack::PopAndDestroy(&selections);
    CleanupStack::Pop(fp);

    return fp;
    }

// ----------------------------------------------------------------------------
// Remove media by path through a command
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::DoRemoveL(
    const CMPXMedia& aMedia,
    TBool aDeleteRecord)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoRemoveL(by command)");

    // Return deleted file paths to caller
    CDesCArray* fp = new(ELeave) CDesCArrayFlat(1);
    CleanupStack::PushL(fp);

    // a list of change event messages a result of the item being removed
    CMPXMessageArray* itemChangedMessages = CMPXMediaArray::NewL();
    CleanupStack::PushL(itemChangedMessages);

    TUint32 podcastEpisodeId(0);

    // Removing a container of items
    //
    if (aMedia.IsSupported(KMPXMediaArrayContents))
        {
        MPX_DEBUG1("CMPXPodcastDbPlugin::RemoveL -- Removing a container of items");
        const CMPXMediaArray* media = (aMedia.Value<CMPXMediaArray>(KMPXMediaArrayContents));
        if( !media )
            {
            User::Leave( KErrNoMemory );
            }
        const TInt mediaCount(media->Count());
        for (TInt i = 0; i < mediaCount; ++i)
            {
            CMPXMedia* entry = media->AtL(i);
            if( entry->IsSupported(KMPXMediaGeneralId))
                {
                podcastEpisodeId = entry->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
                }
            else if (entry->IsSupported(KMPXMediaGeneralUri))
                {
                podcastEpisodeId = iDbHandler->GetEpisodeIdMatchingUriL(
                    entry->ValueText(KMPXMediaGeneralUri));
                }
            else
                {
                // Unable to process this item
                continue;
                }

            iDbHandler->RemoveEpisodeL(podcastEpisodeId, *fp, *itemChangedMessages,
                aDeleteRecord);
            }
        }
    // Removing an item with known item id
    //
    else if (aMedia.IsSupported(KMPXMediaGeneralId))
        {
        MPX_DEBUG1("CMPXPodcastDbPlugin::RemoveL -- Removing an item by item id");
        podcastEpisodeId = aMedia.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);

        if ((podcastEpisodeId >> 28) != EMPXPlaylist)
            {
            iDbHandler->RemoveEpisodeL(podcastEpisodeId, *fp,
                *itemChangedMessages, aDeleteRecord);
            }
        else
            {
            MPX_DEBUG1("CMPXPodcastDbPlugin::RemoveL -- Playlists not supported within podcast collection");
            User::Leave(KErrNotSupported);
            }
        }
    // Removing an item with known uri
    //
    else if (aMedia.IsSupported(KMPXMediaGeneralUri))
        {
        MPX_DEBUG1("CMPXPodcastDbPlugin::RemoveL -- Removing an item by uri");
        podcastEpisodeId = iDbHandler->GetEpisodeIdMatchingUriL(
            aMedia.ValueText(KMPXMediaGeneralUri));
        iDbHandler->RemoveEpisodeL(podcastEpisodeId, *fp, *itemChangedMessages,
            aDeleteRecord);
        }
    else
        {
        MPX_DEBUG1("CMPXPodcastDbPlugin::RemoveL -- Unknown item for removal");
        User::Leave(KErrNotSupported);
        }

    iActiveTask->SetVisibleChange(CMPXDbActiveTask::EAllVisible);
    DoHandleChangeL(itemChangedMessages);

    CleanupStack::PopAndDestroy(itemChangedMessages);
    CleanupStack::PopAndDestroy(fp);
    }

// ----------------------------------------------------------------------------
// Remove an item from the collection database using the given media properties
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::DoRemovePathL(
    CMPXCommand& aCmd)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoRemovePathL(by command)");

    TInt removeError(KErrNone);
    TBool removeCompleted(ETrue);

    if (!aCmd.IsSupported(KMPXCommandCollectionRemovePath) ||
        !aCmd.IsSupported(KMPXCommandCollectionRemoveMediaCount))
        {
        removeError = KErrArgument;
        }
    else
        {
        CMPXCollectionPath* path =
            aCmd.ValueCObjectL<CMPXCollectionPath>(KMPXCommandCollectionRemovePath);
        CleanupStack::PushL(path);

        // in order to support cancel delete for a category, we need to adjust path. If
        // the path ends in a category, retrieve all episodes under the selected category
        // and append a new level with all episodes under the selected category
        DoAppendLevelL(*path);

        CMPXCollectionPath* iterationPath = CMPXCollectionPath::NewL(*path);
        CleanupStack::PushL(iterationPath);
        iterationPath->ClearSelection();

        // indices of the selected items
        TArray<TInt> selectionIndices = path->Selection();
        TInt count(selectionIndices.Count());

        // number of medias to remove in this iteration
        TInt removeCount = (aCmd.ValueTObjectL<TInt>(KMPXCommandCollectionRemoveMediaCount));

        // remove all in one shut if removeCount is 0 or negative
        if (removeCount <= 0)
            {
            removeCount = count;
            }

        // If the given path contains multiple selections, remove the first n selected media
        // and update the path so that client can use this path to call remove iteratively
        // until all selections are processed
        //
        if (count)
            {
            for (TInt i = 0; i < removeCount; ++i)
                {
                TInt index(selectionIndices[i]);

                MPX_DEBUG4(" path: selected item [index %d] [selectioncount %d] [remove count %d]", index, count, removeCount);

                iterationPath->SelectL(index);
                path->Remove(index);
                }

            aCmd.SetCObjectValueL(KMPXCommandCollectionRemovePath, path);

            // indicate to the client that subsequent remove command is required
            if ((count - removeCount) > 0)
                {
                removeCompleted = EFalse;
                }
            }

        // Remove the media specified by the path
        CDesCArray* fp(NULL);
        TBool supressMsgs(EFalse);
        CMPXMessageArray* msgAry(NULL);
        if (aCmd.IsSupported(KMPXCommandCollectionRemoveSuppressMsgs) &&
            aCmd.ValueTObjectL<TBool>(KMPXCommandCollectionRemoveSuppressMsgs) )
            {
            // Msgs are stored in the command
            supressMsgs = ETrue;
            CMPXMessageArray* msgs( aCmd.Value<CMPXMessageArray>(KMPXCommandCollectionChangeMsgs) );
            User::LeaveIfNull( msgs );
            fp = DoRemoveL(*iterationPath, *msgs );
            }
        else
            {
            // Msgs will be sent after delete
            msgAry = CMPXMessageArray::NewL();
            CleanupStack::PushL( msgAry );
            fp = DoRemoveL(*iterationPath, *msgAry);
            }

        CleanupStack::PushL(fp);
        if (fp->MdcaCount() > removeCount)
            {
            removeError = KErrCorrupt;
            }
        CleanupStack::PopAndDestroy(fp);

        if (!supressMsgs)
            {
            // Send Change Messages
            iActiveTask->SetVisibleChange(CMPXDbActiveTask::EAllVisible);
            DoHandleChangeL(msgAry);
            CleanupStack::PopAndDestroy(msgAry);
            }

        // Cleanup
        CleanupStack::PopAndDestroy(iterationPath);
        CleanupStack::PopAndDestroy(path);
        }

    // mandatory return parameters
    aCmd.SetTObjectValueL<TInt>(KMPXCommandCollectionRemoveError, removeError);
    aCmd.SetTObjectValueL<TBool>(KMPXCommandCollectionRemoveCompleted, removeCompleted);
    }

// ----------------------------------------------------------------------------
// Remove media by CMPXMedia through a command
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::DoRemoveMediaL(
    CMPXCommand& aCmd)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoRemoveMediaL(by command)");

    TInt error(KErrArgument);

    if (aCmd.IsSupported(KMPXCommandCollectionRemoveMedia))
        {
        CMPXMedia* media = aCmd.ValueCObjectL<CMPXMedia>(KMPXCommandCollectionRemoveMedia);
        CleanupStack::PushL(media);

        MPX_TRAP(error, DoRemoveL(*media,
            aCmd.ValueTObjectL<TBool>(KMPXCommandCollectionRemoveMediaDeleteRecord)));

        CleanupStack::PopAndDestroy(media);
        }

    aCmd.SetTObjectValueL<TInt>(KMPXCommandCollectionRemoveMediaError, error);
    }

// ----------------------------------------------------------------------------
// Remove a media/media items from All Episodes view
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::DoRemoveFromAllEpisodesL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXItemId>& aSelections,
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoRemoveFromAllEpisodesL");

    switch (aPath.Levels())
        {
        case 2:
            {
            // when the collection is removed, it's intended not to delete the files
            iDbHandler->RemoveEntireCollectionL();
            }
            break;

        case 3:
            {
            TInt count(aSelections.Count());
            if (count)
                {
                for (TInt i = 0; i < count; ++i)
                    {
                    iDbHandler->RemoveEpisodeL (aSelections[i], aUriArray,
                        aItemChangedMessages);
                    } // end for
                }
            else
                {
                iDbHandler->RemoveEpisodeL(aPath.Id(aPath.Levels() - 1), aUriArray,
                    aItemChangedMessages);
                }
            }
            break;

        default:
            {
            MPX_DEBUG2("CMPXPodcastDbPlugin_DoRemoveFromAllEpisodesL: Invalid levels[%d]", aPath.Levels());
            User::Leave(KErrNotSupported);
            }
        }
    }

// ----------------------------------------------------------------------------
// Remove a media/media items from By Publish Date view
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::DoRemoveFromPublishDateL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXItemId>& aSelections,
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoRemoveFromPublishDateL");

    TInt levels(aPath.Levels());

    if (levels == 2)
        {
        // when the collection is removed, it's intended no to delete the files
        iDbHandler->RemoveEntireCollectionL();
        }
    else
        {
        TInt count(aSelections.Count());
        if (count)
            {
            for (TInt i = 0; i < count; ++i)
                {
                RemoveFromPublishDateL(aPath, aSelections[i].iId2, aUriArray, aItemChangedMessages);
                }
            }
        else
            {
            RemoveFromPublishDateL(aPath, aPath.Id(levels - 1).iId2, aUriArray, aItemChangedMessages);
            }
        }

    MPX_DEBUG2("CMPXPodcastDbPlugin__RemoveL__EBrowsePubDate: levels[%d]", aPath.Levels());
    }

// ----------------------------------------------------------------------------
// Remove a media item from By Publish Date view
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::RemoveFromPublishDateL(
    const CMPXCollectionPath& aPath,
    TInt aItemId,
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::RemoveFromPublishDateL");

    switch (aPath.Levels())
        {
        case 3:
            {
            iDbHandler->RemoveEpisodesMatchingPublishDateCategoryL(aItemId, aUriArray,
                aItemChangedMessages);
            break;
            }
        case 4:
            {
            iDbHandler->RemoveEpisodeL(aItemId, aUriArray,
                aItemChangedMessages);
            break;
            }
        default:
            {
            User::Leave(KErrArgument);
            }
        }
    }

// ----------------------------------------------------------------------------
// Remove a media/media items from Titles/Genre etc.. view
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::DoRemoveFromCategoriesL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXItemId>& aSelections,
    TMPXGeneralCategory aCategory,
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoRemoveFromCategoriesL");

    TInt levels(aPath.Levels());
    if (levels == 1)
        {
        // when the collection is removed, it's intended no to delete the files
        iDbHandler->RemoveEntireCollectionL();
        }
    else
        {
        TInt count(aSelections.Count());
        if (count)
            {
            for (TInt i = 0; i < count; ++i)
                {
                RemoveFromCategoriesL(aPath, aSelections[i], aCategory, aUriArray,
                    aItemChangedMessages);
                }
            }

        else
            {
            RemoveFromCategoriesL(aPath, aPath.Id (aPath.Levels() - 1), aCategory, aUriArray,
                aItemChangedMessages);
            }
        }

    MPX_DEBUG2("CMPXPodcastDbPlugin_DoRemoveFromCategoriesL: levels[%d]", aPath.Levels());
    }

// ----------------------------------------------------------------------------
// Remove a media item from Albums/Genres/Composers view
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::RemoveFromCategoriesL(
    const CMPXCollectionPath& aPath,
    TInt aItemId,
    TMPXGeneralCategory aCategory,
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::RemoveFromCategoriesL");

    switch (aPath.Levels())
        {
        case 2:
            {
            iDbHandler->RemoveEpisodesMatchingCategoryL(aCategory, aItemId,
                aUriArray, aItemChangedMessages);
            break;
            }
        case 3:
            {
            iDbHandler->RemoveEpisodeL(aItemId, aUriArray, aItemChangedMessages);
            break;
            }
        default:
            {
            User::Leave(KErrArgument);
            }
        }
    }

// ----------------------------------------------------------------------------
// Retrieve URIs associated with this file path for file deletion
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::DoRetrieveUriForDeletionL(
    CMPXCommand& aCmd)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoRetrieveUriForDeletionL");

    // initialize mandatory return parameters
    aCmd.SetTObjectValueL<TInt>(KMPXCommandCollectionRetrieveUriError, KErrNone);

    if (!aCmd.IsSupported(KMPXCommandCollectionRetrievePath))
        {
        aCmd.SetTObjectValueL<TInt>(KMPXCommandCollectionRetrieveUriError, KErrArgument);
        }
    else
        {
        CMPXCollectionPath* path = aCmd.ValueCObjectL<CMPXCollectionPath>(
            KMPXCommandCollectionRetrievePath);
        CleanupStack::PushL(path);

        if (iFirstDeleteStep )
            {
            iSelections.Reset( );
            // in order to support cancel delete for a category, we need to adjust path. If
            // the path ends in a category, retrieve all songs under the selected category
            // and append a new level with all songs under the selected category
            DoAppendLevelL(*path );

            // Ids of the selected items
            path->SelectionL(iSelections );

            // single selection
            if (iSelections.Count()== 0 )
                {
                iSelections.AppendL(path->Id (path->Levels()- 1 ) );
                }
            }
        CDesCArray* fp = new(ELeave) CDesCArrayFlat(4);
        CleanupStack::PushL(fp);

        TInt count = iSelections.Count();
        TInt itemCount = count > KIncrementalDeleteCount ? KIncrementalDeleteCount : count;
        for (TInt i = 0; i < itemCount; ++i)
            {
            HBufC* uri = iDbHandler->GetUriMatchingIdL(iSelections[0]);
            CleanupStack::PushL(uri);
            fp->AppendL(*uri);
            CleanupStack::PopAndDestroy(uri);
            iSelections.Remove(0);
            
            }
        aCmd.SetNoNewLCObjectL (KMPXCommandCollectionRetrieveMediaUriArray, fp);
        if (iFirstDeleteStep)
            {
            aCmd.SetCObjectValueL(KMPXCommandCollectionRetrievePath, path);
            }
        CleanupStack::PopAndDestroy(fp);
        CleanupStack::PopAndDestroy(path);
        }
    }

// ----------------------------------------------------------------------------
// Cleanup deleted medias
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::CleanupDeletedRecordsL(
    CMPXCommand& aCmd)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::CleanupDeletedRecordsL");

    MPX_TRAPD(error, iDbHandler->CleanupDeletedRecordsL());
    aCmd.SetTObjectValueL<TInt>(KMPXCommandCollectionCleanupError, error);
    }

// ----------------------------------------------------------------------------
// Retrieve the duration
// ----------------------------------------------------------------------------
//
TInt CMPXPodcastDbPlugin::DoDurationL(
    CMPXMedia& aMedia,
    TMPXPodcastCategory aCategory,
    TMPXItemId aId)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoDurationL");

    TInt duration(0);
    switch (aCategory)
        {
        case EMPXEpisode:
            {
            duration = iDbHandler->GetAllEpisodesDurationL();
            break;
            }
        case EMPXTitle:
            {
            duration = iDbHandler->GetTitleDurationL(aId.iId2);
            break;
            }
        case EMPXNotYetPlayed:
            {
            duration = iDbHandler->GetNotPlayedDurationL();
            break;
            }
       case EMPXRecentlyAdded:
            {
            duration = iDbHandler->GetRecentlyAddedDurationL();
            break;
            }
        default:
            {
            User::Leave(KErrNotSupported);
            }
        }

    aMedia.SetTObjectValueL<TInt>(KMPXMediaGeneralDuration, duration);
    return duration;
    }

// ----------------------------------------------------------------------------
// Append a level to a collection path and set selection to the first level
// ----------------------------------------------------------------------------
//
TInt CMPXPodcastDbPlugin::DoAppendLevelL(
    CMPXCollectionPath& aPath,
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoAppendLevelL");

    RArray<TMPXItemId> ids;
    CleanupClosePushL(ids);

    // Extract media array, and get all item ids
    //
    const CMPXMediaArray* mediaArray = aMedia.Value<CMPXMediaArray>(KMPXMediaArrayContents);
    if( !mediaArray )
        {
        User::Leave(KErrNoMemory);
        }
    TInt count(mediaArray->Count());

    if (count >= 0)
        {
        for (TInt i = 0; i < count; ++i)
            {
            TMPXItemId id = mediaArray->AtL(i)->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
            ids.AppendL(id);
            }

        // Put item id array into the path and select the first one
        aPath.AppendL(ids.Array());
        if (count > 0)
        	{
	        aPath.Set(0);
        	}
        }

    CleanupStack::PopAndDestroy(&ids);
    return count;
    }

// ----------------------------------------------------------------------------
// Append a level to a collection path and set selection to all episodes under the selected category/categories
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::DoAppendLevelL(
    CMPXCollectionPath& aPath)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoAppendLevelL");

    TMPXItemId contextId(aPath.Id(1));
    TInt levels(aPath.Levels());

    if (contextId == EBrowseAll)
        {
        return;
        }
    else if (levels == 2)
        {
        // retrieve episodes in the selected category
        CMPXMediaArray* episodes = CMPXMediaArray::NewL();
        CleanupStack::PushL(episodes);

        RArray<TMPXAttribute> attributes;
        CleanupClosePushL(attributes);
        attributes.AppendL(KMPXMediaGeneralId);

        // Ids of the selected items
        RArray<TMPXItemId> selections;
        CleanupClosePushL(selections);
        aPath.SelectionL(selections);

        // single selection
        if (selections.Count() == 0)
            {
            selections.AppendL(aPath.Id(aPath.Levels() - 1));
            }

        TInt count(selections.Count());

        for (TInt i = 0; i < count; ++i)
            {
            iDbHandler->GetEpisodesMatchingTitleL(selections[i], attributes.Array(), *episodes);
            }

        CleanupStack::PopAndDestroy(2, &attributes); // selections & attributes

        // transform from CMPXMediaArray to RArray
        RArray<TMPXItemId> episodeIds;
        CleanupClosePushL(episodeIds);

        TInt episodeCount(episodes->Count());
        for (TInt i = 0; i < episodeCount; ++i)
            {
            CMPXMedia* episode = (*episodes)[i];

            if (episode->IsSupported(KMPXMediaGeneralId))
                {
                episodeIds.AppendL(episode->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId));
                }
            }

        // modify the collection path. append another level with all episodes under the selected
        // category/categories selected
        episodeCount = episodeIds.Count();

        if (episodeCount)
            {
            aPath.ClearSelection();
            aPath.AppendL(episodeIds.Array());

            // select all
            for (TInt i = 0; i < episodeCount; ++i)
                {
                aPath.SelectL(episodeIds[i]);
                }
            }

        CleanupStack::PopAndDestroy(2, episodes); // episodeIds & episodes
        }
    else
        {
        // else do nothing
        }
    }

// ----------------------------------------------------------------------------
// Execute an Add task step
// ----------------------------------------------------------------------------
//
TBool CMPXPodcastDbPlugin::DoAddAsyncL()
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoAddAsyncL");

    TBool done(EFalse);
    CMPXMedia* task = iActiveTask->GetCommand().Value<CMPXMedia>(KMPXCommandColAddMedia);
    User::LeaveIfNull(task);

    CMPXMessageArray& msgArray = iActiveTask->GetChangeMessages();

    if (!task->IsSupported(KMPXMediaGeneralType))
        {
        User::Leave(KErrArgument);
        }

    // Group of items or a single item
    //
    if (task->ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType) == EMPXGroup)
        {
        if(!task->IsSupported(KMPXMediaArrayContents))
            {
            User::Leave(KErrArgument);
            }

        CMPXMediaArray* ary = task->Value<CMPXMediaArray>(KMPXMediaArrayContents);
        User::LeaveIfNull( ary );

        TInt step( iActiveTask->GetStep() );
        DoAddItemL(*ary->AtL(step), msgArray);

        if (++step == ary->Count())
            {
            done = ETrue;
            }
        }
    else // type == EMPXItem
        {
        TUint32 item = DoAddItemL( *task, msgArray );
        iActiveTask->GetCommand().SetTObjectValueL<TMPXItemId>( KMPXCommandColAddRtnId, item );
        done = ETrue;
        }

    iActiveTask->SetVisibleChange(CMPXDbActiveTask::EAllVisible);
    return done;
    }

// ----------------------------------------------------------------------------
// Add an item to the collection
// ----------------------------------------------------------------------------
//
TUint32 CMPXPodcastDbPlugin::DoAddL(
    const CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoAddL");

    TUint32 itemId(0);
    CMPXMessageArray* changeMsgAry = CMPXMessageArray::NewL();
    CleanupStack::PushL( changeMsgAry );

    if (!aMedia.IsSupported(KMPXMediaGeneralType))
        {
        User::Leave(KErrArgument);
        }

    // Group of items
    //
    if (aMedia.ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType) == EMPXGroup)
        {
        CMPXMediaArray* ary = aMedia.Value<CMPXMediaArray>(KMPXMediaArrayContents);
        User::LeaveIfNull( ary );

        TInt count(ary->Count());
        for (TInt i = 0; i < count; ++i)
            {
            DoAddItemL(*ary->AtL(i), *changeMsgAry);
            }
        }
    else // single item
        {
        itemId = DoAddItemL(aMedia, *changeMsgAry);
        }

    iActiveTask->SetVisibleChange(CMPXDbActiveTask::EAllVisible);
    DoHandleChangeL(changeMsgAry);
    CleanupStack::PopAndDestroy(changeMsgAry);

    return itemId;
    }

// ----------------------------------------------------------------------------
// Add an item to the collection
// ----------------------------------------------------------------------------
//
TUint32 CMPXPodcastDbPlugin::DoAddItemL(
    const CMPXMedia& aMedia,
    CMPXMessageArray& aMessageArray)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoAddItemL");

    TInt itemId(0);

    if (!aMedia.IsSupported(KMPXMediaGeneralCategory))
        {
        User::Leave(KErrArgument);
        }

    switch ( aMedia.ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory))
        {
        case EMPXPodcast:
        case EMPXSong:
            {
            itemId = iDbHandler->AddEpisodeL(aMedia);
            MPXDbCommonUtil::AddItemChangedMessageL(aMessageArray, itemId, EMPXItemInserted,
                EMPXPodcast, KDBPluginUid);
            break;
            }
        default:
            {
            User::Leave(KErrNotSupported);
            }
        }

    return itemId;
    }
// ----------------------------------------------------------------------------
// Sets/updates the media for an item in the collection
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::DoSetL(
    const CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoSetL");

    if (!aMedia.IsSupported(KMPXMediaGeneralType) ||
        !aMedia.IsSupported(KMPXMediaGeneralCategory))
        {
        User::Leave(KErrArgument);
        }

    CMPXDbActiveTask::TChangeVisibility visibleChange(CMPXDbActiveTask::ENotVisibile);

    switch ( aMedia.ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory))
        {
        case EMPXPodcast:
        case EMPXSong:
            {
            // a list of changed messages as a result of the episode being updated
            CMPXMessageArray* itemChangedMessages = CMPXMediaArray::NewL();
            CleanupStack::PushL(itemChangedMessages);

             if (aMedia.ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType) == EMPXGroup)
                {
                if (!aMedia.IsSupported(KMPXMediaArrayContents))
                    {
                    User::Leave(KErrArgument);
                    }

                CMPXMediaArray* array = aMedia.Value<CMPXMediaArray>(KMPXMediaArrayContents);
                User::LeaveIfNull( array );

                TInt count(array->Count());
                for (TInt i = 0; i < count; ++i)
                    {
                    visibleChange = (CMPXDbActiveTask::TChangeVisibility)(visibleChange |
                            iDbHandler->UpdateEpisodeL(*array->AtL(i), *itemChangedMessages));
                    }
                }
             else
                {
                visibleChange = iDbHandler->UpdateEpisodeL(aMedia,
                    *itemChangedMessages);
                }

             if (visibleChange)
                {
                iActiveTask->SetVisibleChange(visibleChange);
                DoHandleChangeL(itemChangedMessages);
                }

            CleanupStack::PopAndDestroy(itemChangedMessages);
            }
            break;

        default:
            {
            User::Leave(KErrNotSupported);
            }
            break;
        }
    }

// ----------------------------------------------------------------------------
// Execute a task step for async set
// ----------------------------------------------------------------------------
//
TBool CMPXPodcastDbPlugin::DoSetAsyncL()
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoSetAsyncL");

    TBool done(EFalse);
    CMPXMedia* task = (iActiveTask->GetCommand().Value<CMPXMedia>(KMPXCommandColSetMedia));
    User::LeaveIfNull( task );

    CMPXMessageArray& msgArray = iActiveTask->GetChangeMessages();
    CMPXDbActiveTask::TChangeVisibility visibleChange(iActiveTask->GetVisibleChange());

    // Multiple steps can be in a transaction for faster response
    if( iDbHandler && !iDbHandler->InTransaction() )
        {
        iDbHandler->BeginTransactionL();
        }

    if (!task->IsSupported(KMPXMediaGeneralType))
        {
        User::Leave(KErrArgument);
        }

    if (task->ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType) == EMPXGroup)
        {
        if (!task->IsSupported(KMPXMediaArrayContents))
            {
            User::Leave(KErrArgument);
            }

        // Multiple items
        CMPXMediaArray* array = task->Value<CMPXMediaArray>(KMPXMediaArrayContents);
        User::LeaveIfNull( array );

        TInt step = iActiveTask->GetStep();
        visibleChange = (CMPXDbActiveTask::TChangeVisibility)( visibleChange | iDbHandler->UpdateEpisodeL(*array->AtL(step), msgArray));

        if (++step == array->Count())
            {
            done = ETrue;
            }
        }
    else // Single item
        {
        if(iDbHandler)
            {		
            visibleChange = iDbHandler->UpdateEpisodeL(*task, msgArray);
            done = ETrue;
            }
        }
    iActiveTask->SetVisibleChange(visibleChange);
    return done;
    }

// ----------------------------------------------------------------------------
// Handle change events
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::HandleChangeL(
    const CMPXMessage& aMessage)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::HandleChange");

    // check if message is filled
    if (aMessage.IsSupported(KMPXMessageGeneralId))
        {
#ifdef _DEBUG
        PrintMessagesL(aMessage);
#endif  // _DEBUG
        if(iRefreshing)
            {
            if (aMessage.IsSupported(KMPXMessageArrayContents))
                {
                const CMPXMessageArray* messageArray = aMessage.Value<CMPXMessageArray>(KMPXMessageArrayContents);
                if(messageArray)
                    {
                    CMPXMessage& message = *((*messageArray)[0]);
                    TMPXChangeEventType changeType( message.ValueTObjectL<TMPXChangeEventType>( KMPXMessageChangeEventType ) );
                    TMPXGeneralCategory cat(message.ValueTObjectL<TMPXGeneralCategory>(KMPXMessageMediaGeneralCategory));
                    if(changeType == EMPXItemInserted && (cat == EMPXPodcast || cat == EMPXSong || cat == EMPXPlaylist))
                        {
                        iObs->HandleMessage(aMessage);
                        }
                    }
                }
            else
                {
                TMPXChangeEventType changeType( aMessage.ValueTObjectL<TMPXChangeEventType>( KMPXMessageChangeEventType ) );
                TMPXGeneralCategory cat(aMessage.ValueTObjectL<TMPXGeneralCategory>(KMPXMessageMediaGeneralCategory));
                if(changeType == EMPXItemInserted && (cat == EMPXPodcast || cat == EMPXSong || cat == EMPXPlaylist))
                    {
                    iObs->HandleMessage(aMessage);
                    }
                }
            }
        else
            {
            if(!iMtpInUse)
                {
                iObs->HandleMessage(aMessage);
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// Construct a CMPXMedia and call HandleChange
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::DoHandleChangeL(
    CMPXMessageArray* aItemChangedMessages)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoHandleChangeL");

    TMPXCommandId cmdId = iActiveTask->GetTask();
    if((iActiveTask->GetVisibleChange() & CMPXDbActiveTask::EAllVisible)
        && (cmdId == KMPXCommandIdCollectionSet ||
            cmdId == KMPXCommandIdCollectionAdd ||
            cmdId == KMPXCommandIdCollectionRemove ||
            cmdId == KMPXCommandIdCollectionRemoveMedia ||
            cmdId == KMPXCommandIdCollectionCompleteDelete ))
        {
        MPXDbCommonUtil::AddItemChangedMessageL(*aItemChangedMessages, EBrowseAll,
                                                 EMPXItemModified, EMPXCollection, KDBPluginUid);
        }

    // group change messages and send to collection client context
    CMPXMessage* message = CMPXMessage::NewL();
    CleanupStack::PushL(message);

    message->SetTObjectValueL<TMPXMessageId>(KMPXMessageGeneralId, KMPXMessageIdItemChanged);
    message->SetCObjectValueL(KMPXMessageArrayContents, aItemChangedMessages);
    message->SetTObjectValueL<TInt>(KMPXMessageArrayCount, aItemChangedMessages->Count());

    HandleChangeL(*message);

    CleanupStack::PopAndDestroy(message);
    }

// ----------------------------------------------------------------------------
// Handle out of disk events during db merging
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::HandleOutOfDiskMessageL()
    {
    MPX_FUNC("CMPXPodcastDbPlugin::HandleOutOfDiskMessageL");

    if (iObs)
        {
        // Create the msg
        CMPXMessage* msg = CMPXMessage::NewL();
        CleanupStack::PushL( msg );

        // Setup the message parameters
        msg->SetTObjectValueL<TInt>(KMPXMessageGeneralId, KMPXCustomMessageId);
        msg->SetTObjectValueL<TInt>(KMPXCustomMessageCollectionId, KDBPluginUid);
        msg->SetTObjectValueL<TInt>(KMPXCustomMessageEventType, EMcsOpen);
        msg->SetTObjectValueL<TInt>(KMPXCustomMessageErrorCode, KErrDiskFull);

        // Callback and Cleanup
        iObs->HandleMessage(*msg);
        CleanupStack::PopAndDestroy(msg);
        }
    }

// ----------------------------------------------------------------------------
// Handle completion of operation
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::DoHandleOperationCompletedL(
    TInt aErr)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoHandleOperationCompletedL");

    if( iDbHandler && iDbHandler->InTransaction() )
        {
        // Commit if cancelled
        TInt err(aErr);
        if( err == KErrCancel )
            {
            err = KErrNone;
            }
        iDbHandler->EndTransactionL( err );
        }
    
    // Broadcase change messages
    //
    if (iActiveTask->GetVisibleChange())
        {
        DoHandleChangeL(&iActiveTask->GetChangeMessages());
        }

    // Callback to engine to signal completion
    // NOTE: Collection server immediately completes the async message when
    // Cancel is called, no need to callback to observer
    if (aErr != KErrCancel)
        {
        iObs->HandleCommandComplete(NULL, aErr);
        }    
    }
     

// ----------------------------------------------------------------------------------------------------------
// Complete a delete operation
// ----------------------------------------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::DoHandleDeleteCompleteL(
    CMPXCommand& aCmd)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoHandleDeleteCompleteL");
    iFirstDeleteStep = ETrue;
    iSelections.Reset();
    // Change messages
    if (aCmd.IsSupported(KMPXCommandCollectionDeleteMsgArray))
        {
        CMPXMessageArray* msgs = aCmd.Value<CMPXMessageArray>(KMPXCommandCollectionDeleteMsgArray);
        User::LeaveIfNull( msgs );
        iActiveTask->SetVisibleChange(CMPXDbActiveTask::EAllVisible);
        DoHandleChangeL(msgs);
        }
    }

// ----------------------------------------------------------------------------
// Maps a given browse type to a category ID.
// ----------------------------------------------------------------------------
//
TMPXPodcastCategory CMPXPodcastDbPlugin::CategoryForBrowseType(
    TMCBrowseType aBrowseType)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::CategoryForBrowseType");

    TMPXPodcastCategory cat(EMPXUnknown);

    switch (aBrowseType)
        {
        case EBrowseAll:
            {
            cat = EMPXAll;
            break;
            }
        case EBrowseTitle:
            {
            cat = EMPXTitle;
            break;
            }
        case EBrowsePubDate:
            {
            cat = EMPXPubDate;
            break;
            }
        case EBrowseRecentlyAdded:
            {
            cat = EMPXRecentlyAdded;
            break;
            }
        case EBrowseNotPlayed:
            {
            cat = EMPXNotYetPlayed;
            break;
            }
        default:
            {
            // do nothing
            break;
            }
        }

    return cat;
    }

// ----------------------------------------------------------------------------
// Maps a given category ID to a browse type.
// ----------------------------------------------------------------------------
//
TMCBrowseType CMPXPodcastDbPlugin::BrowseTypeForCategory(
    TMPXPodcastCategory aCategory)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::BrowseTypeForCategory");

    TMCBrowseType browseType(EBrowseNotPlayed);

    switch (aCategory)
        {
        case EMPXAll:
            {
            browseType = EBrowseAll;
            break;
            }
        case EMPXTitle:
            {
            browseType = EBrowseTitle;
            break;
            }
        case EMPXPubDate:
            {
            browseType = EBrowsePubDate;
            break;
            }
        case EMPXRecentlyAdded:
            {
            browseType = EBrowseRecentlyAdded;
            break;
            }
        default:
            {
            // do nothing
            break;
            }
        }

    return browseType;
    }

// ----------------------------------------------------------------------------
// Sets the type, category and title attributes in the specified media instance
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::SetMediaGeneralAttributesL(
    CMPXMedia& aMedia,
    TMPXGeneralType aGeneralType,
    TMPXPodcastType aType,
    TMPXPodcastCategory aCategory,
    const TDesC& aTitle,
    TInt aCount /* = -1 */)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::SetMediaGeneralAttributesL");

    aMedia.SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, aGeneralType);
    aMedia.SetTObjectValueL<TMPXPodcastType>(KMPXMediaPodcastType, aType);
    aMedia.SetTObjectValueL<TMPXPodcastCategory>(KMPXMediaPodcastCategoryGroup, aCategory);
    aMedia.SetTextValueL(KMPXMediaGeneralTitle, aTitle);

    if (aCount >= 0)
        {
        aMedia.SetTObjectValueL(KMPXMediaGeneralCount, aCount);
        }
    }

// ----------------------------------------------------------------------------
// Sets the type, category and title attributes in the specified media instance
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::SetMediaGeneralAttributesL(
    CMPXMedia& aMedia,
    TMPXGeneralType aGeneralType,
    TMPXPodcastType aType,
    TMPXPodcastCategory aCategory,
    TInt aId,
    TInt aCount /* = -1 */)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::SetMediaGeneralAttributesL");

    HBufC* title = iDbHandler->GetTitleNameMatchingIdL(aId);
    CleanupStack::PushL(title);
    SetMediaGeneralAttributesL(aMedia, aGeneralType, aType, aCategory, *title, aCount);
    CleanupStack::PopAndDestroy(title);
    }

// ----------------------------------------------------------------------------------------------------------
// Get total podcast count for a database
// ----------------------------------------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::DoGetCollectionCountL( const CMPXCommand& aCmd )
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoGetCollectionCountL");
    //as there is only one table containing uri data, KMPXCommandCollectionCountTable is ignored
    if (!aCmd.IsSupported(KMPXCommandCollectionCountDrive))
        {
        User::Leave(KErrArgument);
        }

    TInt drive = aCmd.ValueTObjectL<TInt>(KMPXCommandCollectionCountDrive);
    TInt count = (TInt)iDbHandler->GetTotalCountL(drive);
    ((CMPXMedia&)aCmd).SetTObjectValueL<TInt>(KMPXCommandCollectionCountValue, count);
    }

// ----------------------------------------------------------------------------------------------------------
// Get URIs for all podcasts in a database
// ----------------------------------------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::DoGetCollectionUriL( const CMPXCommand& aCmd )
    {
    MPX_FUNC("CMPXPodcastDbPlugin::DoGetCollectionCountL");
    //as there is only one table containing uri data, KMPXCommandCollectionCountTable is ignored
    if (!aCmd.IsSupported(KMPXCommandCollectionURIDrive) ||
        !aCmd.IsSupported(KMPXCommandCollectionURIFromID) ||
        !aCmd.IsSupported(KMPXCommandCollectionURIRecords) )
        {
        User::Leave(KErrArgument);
        }

    TInt drive = aCmd.ValueTObjectL<TInt>(KMPXCommandCollectionURIDrive);
    TInt fromID = aCmd.ValueTObjectL<TInt>(KMPXCommandCollectionURIFromID);
    TInt recnum = aCmd.ValueTObjectL<TInt>(KMPXCommandCollectionURIRecords);

    CDesCArray* uris = new(ELeave) CDesCArrayFlat(4);
    CleanupStack::PushL(uris);
    TInt lastID = 0;

    iDbHandler->GetPodcastUriArrayL(drive, fromID, recnum, *uris, lastID);

    ((CMPXMedia&)aCmd).SetNoNewLCObjectL(KMPXCommandCollectionURIList, uris);
    ((CMPXMedia&)aCmd).SetTObjectValueL(KMPXCommandCollectionURILastID, lastID);
    CleanupStack::PopAndDestroy(uris);
    }

void CMPXPodcastDbPlugin::SetAttributesL(
    const CMPXCollectionPath& aPath,
    RArray<TMPXAttribute>& aAttrs,
    RArray<TInt>& aSupportedIds )
    {
    CleanupClosePushL(aAttrs);
    CleanupClosePushL(aSupportedIds);
    
    aAttrs.AppendL(TMPXAttribute(KMPXMediaIdGeneral, EMPXMediaGeneralTitle |
        EMPXMediaGeneralDuration | EMPXMediaGeneralSize |
        EMPXMediaGeneralType | EMPXMediaGeneralCategory | EMPXMediaGeneralId |
        EMPXMediaGeneralUri | EMPXMediaGeneralFlags | EMPXMediaGeneralCount |
        EMPXMediaGeneralPlayCount | EMPXMediaGeneralLastPlaybackPosition |
        EMPXMediaGeneralCollectionId | EMPXMediaGeneralDate));
    
    aAttrs.AppendL(TMPXAttribute(KMPXMediaIdPodcast,
        EMPXMediaPodcastType | EMPXMediaPodcastCategoryGroup | EMPXMediaPodcastIsPlaying));

    aSupportedIds.AppendL(KMPXMediaIdContainer);
    aSupportedIds.AppendL(KMPXMediaIdGeneral);
    aSupportedIds.AppendL(KMPXMediaIdPodcast);

    TInt levels(aPath.Levels());
    if ( 1 < levels )
        {
        // All episodes in a title
        aAttrs.AppendL( TMPXAttribute(KMPXMediaIdMusic, EMPXMediaMusicAlbumArtFileName ) );
        aSupportedIds.AppendL( KMPXMediaIdMusic );
        }
    CleanupStack::Pop();
    CleanupStack::Pop();
    }

#ifdef _DEBUG

// ----------------------------------------------------------------------------
// Print change events
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::PrintMessagesL(
    const CMPXMessage& aMessage)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::PrintMessages");

    if (aMessage.IsSupported(KMPXMessageArrayContents))
        {
        const CMPXMessageArray* messageArray =
            aMessage.Value<CMPXMessageArray>(KMPXMessageArrayContents);
        if( !messageArray )
            {
            User::Leave( KErrNoMemory );
            }

        TInt count(messageArray->Count());
        MPX_DEBUG2("%d messages:", count);

        for (TInt i = 0; i < count; ++i)
            {
            PrintMessage(*((*messageArray)[i]));
            }
        }
    else
        {
        PrintMessage(aMessage);
        }
    }

// ----------------------------------------------------------------------------
// Print one change event
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbPlugin::PrintMessage(
    const CMPXMessage& aMessage)
    {
    MPX_FUNC("CMPXPodcastDbPlugin::PrintMessage");

    if (aMessage.IsSupported(KMPXMessageGeneralId))
        {
        TMPXItemId id = aMessage.ValueTObjectL<TMPXItemId>(KMPXMessageGeneralId);
        MPX_DEBUG3("    message id[0x%x, 0x%x]", id.iId1, id.iId2);
        }

    if (aMessage.IsSupported(KMPXMessageCollectionId))
        {
        TUid uid = aMessage.ValueTObjectL<TUid>(KMPXMessageCollectionId);
        MPX_DEBUG2("    uid [0x%x]", uid.iUid);
        }

    if (aMessage.IsSupported(KMPXMessageChangeEventType))
        {
        MPX_DEBUG2("    change event type [%d]",
             aMessage.ValueTObjectL<TMPXChangeEventType>(KMPXMessageChangeEventType));
        }

    if (aMessage.IsSupported(KMPXMessageMediaGeneralCategory))
        {
        MPX_DEBUG2("    category [%d]",
             aMessage.ValueTObjectL<TMPXGeneralCategory>(KMPXMessageMediaGeneralCategory));
        }

    if (aMessage.IsSupported(KMPXMessageMediaGeneralId))
        {
        TMPXItemId id = aMessage.ValueTObjectL<TMPXItemId>(KMPXMessageMediaGeneralId);
        MPX_DEBUG3("    media id[0x%x, 0x%x]", id.iId1, id.iId2);
        }

    if (aMessage.IsSupported(KMPXMessageMediaDeprecatedId))
        {
        TMPXItemId id = aMessage.ValueTObjectL<TMPXItemId>(KMPXMessageMediaGeneralId);
        MPX_DEBUG3("    deprecated id [0x%x, 0x%x]", id.iId1, id.iId2);
        }
    }

#endif// _DEBUG

// End of file
