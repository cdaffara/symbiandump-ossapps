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
* Description:  Implementation of collection DB Plugin interface
*
*/


// INCLUDE FILES
#include <e32cmn.h>
#include <StringLoader.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh>
#include <hbtextresolversymbian.h>

#include <mpxcmn.h>
#include <mpxuser.h>
#include <mpxcollectionpluginobserver.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxmediaaudiodefs.h>
#include <mpxmediacollectiondetaildefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxmessagecontainerdefs.h>
#include <mpxcollectioncommanddefs.h>
#include <mpxcollectionmessagedefs.h>
#include <mpxincrementalopendefs.h>
#include <mpxcollectionopenlresultdef.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxdrmmediautility.h>
#include <mpxmediadrmdefs.h>
#include <mpxlog.h>
#include <mpxcollectiondbhgres.rsg>
#include <centralrepository.h>

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif //RD_MULTIPLE_DRIVE

#include "mpxresource.h"
#include "mpxdbcommondef.h"
#include "mpxdbcommonutil.h"

#include "mpxdbhandler.h"
#include "mpxdbutil.h"
#include "mpxcollectiondbdef.h"
#include "mpxdbplugin.h"

// CONSTANTS
const TInt KFirstFetchCount = 100;

const TUid KCRUIDMusicPlayerFeatures = { 0x101FFCD0 };
const TInt KMusicPlayerFeatures = 1;
const TInt KDisablePodcasting = 0x08;

const TInt KIncrementalDeleteCount = 400;

const TInt KSQLErrGeneral = -311; // SQL General error. Don't want to include sql header here
// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbPlugin* CMPXDbPlugin::NewL(
    TAny* /*aInitParams*/)
    {
    MPX_FUNC("CMPXDbPlugin::NewL");

    CMPXDbPlugin* self = new (ELeave) CMPXDbPlugin();
    CleanupStack::PushL (self);
    self->ConstructL ();
    CleanupStack::Pop (self);
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor.
// ----------------------------------------------------------------------------
//
CMPXDbPlugin::~CMPXDbPlugin()
    {
    MPX_FUNC("CMPXDbPlugin::~CMPXDbPlugin");

    iSelections.Reset();
    iSelections.Close();
    iFs.Close();
    delete iDbHandler;
    delete iDrmMediaUtility;
    if (iResource)
        {
        iResource->Release();
        }
    iMusicLibraryMenuIds.Close();
    delete iMusicLibraryMenuTitles;
    delete iMusicLibraryTitles;
    delete iAllSongsForArtistTitle;
    delete iMusicMenuTitle;

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
CMPXDbPlugin::CMPXDbPlugin()
    {
    MPX_FUNC("CMPXDbPlugin::CMPXDbPlugin");
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::ConstructL()
    {
    MPX_FUNC("CMPXDbPlugin::ConstructL");
    iFirstDeleteStep = ETrue;
    User::LeaveIfError(iFs.Connect());
    iDrmMediaUtility = CMPXDrmMediaUtility::NewL();

    TParse parse;
    parse.Set( KMPXCollectionDbResourceFile, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName resFile(parse.FullName());
    User::LeaveIfError(MPXUser::CompleteWithDllPath(resFile));
    BaflUtils::NearestLanguageFile(iFs, resFile);
    iResource = CMPXResource::NewL(resFile);

    iDbHandler = CMPXDbHandler::NewL(iFs, *iResource);
    iMusicLibraryMenuTitles = iResource->ReadMenuArrayL(R_MC_MENU_ITEMS_ARRAY, iMusicLibraryMenuIds);
    iMusicLibraryTitles = iResource->ReadMenuArrayL(R_MC_TITLE_ITEMS_ARRAY, iMusicLibraryMenuIds );
    
    // Localization using QT
    TBool result = HbTextResolverSymbian::Init(KMPXMusicPlayerTsFile, KMPXMusicPlayerTsPath);
    if ( result )
        {
        iAllSongsForArtistTitle = HbTextResolverSymbian::LoadL( _L("txt_mus_dblist_all_songs") );
        }
    else
        {
        // error initializing HbTextResolverSymbian, use logical string.
        MPX_DEBUG1("CMPXDbPlugin::ConstructL - HbTextResolverSymbian::Init() Failed.");
        TBufC<50> buf( _L("txt_mus_dblist_all_songs") );
        iAllSongsForArtistTitle = buf.AllocL();
        }

#ifdef __ENABLE_MUSIC_TEXT_ALIGNMENT
    iMusicMenuTitle = iResource->ReadHBufCL(R_MPX_QTN_MP_TITLE_MY_MUSIC_MENU_NSERIES);
#else
    iMusicMenuTitle = iResource->ReadHBufCL(R_MPX_QTN_MUS_TITLE_MUSIC_MENU);
#endif // __ENABLE_MUSIC_TEXT_ALIGNMENT


    iActiveTask = CMPXDbActiveTask::NewL(*this);

    CRepository* cenrep(NULL);
    TRAPD( err, cenrep = CRepository::NewL( KCRUIDMusicPlayerFeatures ) );
    if( err == KErrNone )
        {
        TInt val(0);
        cenrep->Get( KMusicPlayerFeatures, val );
        iDisablePodcasting = val&KDisablePodcasting ? ETrue:EFalse;
        delete cenrep;
        }
    else
        {
        iDisablePodcasting = EFalse;
        }
    iAllSongsValid = ETrue;
    }

// ----------------------------------------------------------------------------
// Navigates to the given path
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::OpenL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& /*aAttrs*/,
    CMPXFilter* aFilter)
    {
    MPX_FUNC("CMPXDbPlugin::OpenL");
    MPX_DEBUG_PATH(aPath);
    //Create a path object to be returned.
    CMPXCollectionPath* path = CMPXCollectionPath::NewL(aPath);
    CleanupStack::PushL(path);
    RArray<TMPXAttribute> openAttrs;
    CleanupClosePushL(openAttrs);

    RArray<TInt> supportedIds;
    CleanupClosePushL(supportedIds);

    // Replace the attributes requested by the client with the ones below.
    // This will eventually have to be fixed
    SetAttributesL(aPath, openAttrs, supportedIds);

    CMPXMedia* entries = CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PopAndDestroy(&supportedIds);
    CleanupStack::PushL(entries);
    //Add returned path into media
    entries->SetTObjectValueL<TInt>(KMPXMediaGeneralValue, (TInt)path);

    TInt error(KErrNone);
    TBool isASong(EFalse);
    CMPXCollectionPath* newPath(NULL);

    // Make sure we handle open the correct open mode
    //
    TMPXOpenMode openmode = aPath.OpenNextMode();
    switch (openmode)
        {
        case EMPXOpenGroupOrPlaylist:
            {
            // Open By Path
            MPX_TRAP(error, isASong = DoOpenL(aPath, openAttrs.Array(), *entries, aFilter));
            break;
            }

        case EMPXOpenPlaylistOnly:
            {
            if (aPath.Count() > 0)
                {
                // Try to open
                MPX_TRAP(error, newPath = DoOpenPlaylistL(aPath, openAttrs.Array()));
                CleanupStack::PushL(newPath);
                isASong = ETrue;
                }
            else // no items
                {
                MPX_TRAP(error, isASong = DoOpenL(aPath, openAttrs.Array(), *entries, aFilter));
                }

            break;
            }

        default:
            // do nothing
            break;
        }

    // generate the callback
    if (isASong )
        {
        if (openmode == EMPXOpenGroupOrPlaylist)
            {
            iObs->HandleOpen(const_cast<CMPXCollectionPath*>(&aPath), error);
            }
        else  // openmode == EMPXOpenPlaylistOnly
            {
            iObs->HandleOpen(newPath, error);
            }
        }
    else
        {
        entries->SetCObjectValueL(KMPXMediaGeneralContainerPath,
            const_cast<CMPXCollectionPath*>(&aPath));
        entries->Delete(KMPXMediaGeneralValue);
        iObs->HandleOpen(entries, path, error);
        }

    if (newPath)
        {
        CleanupStack::PopAndDestroy(newPath);
        }

    CleanupStack::PopAndDestroy(entries);
    CleanupStack::PopAndDestroy(&openAttrs);
    CleanupStack::PopAndDestroy(path);
    }

// ----------------------------------------------------------------------------
// Get the extended properties of the current file (async)
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::MediaL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
    const TArray<TCapability>& /*aCaps*/,
    CMPXAttributeSpecs* /*aSpecs*/)
    {
    MPX_FUNC("CMPXDbPlugin::MediaL");
    MPX_DEBUG_PATH(aPath);

    RArray<TInt> supportedIds;
    CleanupClosePushL(supportedIds);
    if (aPath.Selection().Count())
        {
        // it's a container if there are multiple selection, else it's not a container
        supportedIds.AppendL(KMPXMediaIdContainer);
        }
    MPXDbCommonUtil::FillInSupportedUIDsL(aAttrs, supportedIds);

    CMPXMedia* entries = CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PopAndDestroy(&supportedIds);
    CleanupStack::PushL(entries);

    DoMediaL(aPath, aAttrs, *entries);

    // Also fetch collection details and set the path if required
    DoHandleOtherMediaAttributesL(aAttrs, aPath, *entries);

    iObs->HandleMedia(entries, KErrNone);
    CleanupStack::PopAndDestroy(entries);
    }

// ----------------------------------------------------------------------------
// Cancel outstanding request
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::CancelRequest()
    {
    MPX_FUNC("CMPXDbPlugin::CancelRequest");
    iActiveTask->Cancel();
    }

// ----------------------------------------------------------------------------
// Executes the given command on the collection
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::CommandL(
    TMPXCollectionCommand aCmd,
    TInt aArg /* = 0 */)
    {
    MPX_FUNC("CMPXDbPlugin::CommandL");
    MPX_DEBUG2("CMPXDbPlugin::CommandL %d", aCmd);
    iAllSongsValid = ETrue;
    switch (aCmd)
        {
        case EMcCmdRemoveAll:
            {
            MPX_DEBUG1("CMPXDbPlugin::CommandL - EMcCmdRemoveAll");
            // Remove EVERYthing from the collection
            iDbHandler->RemoveEntireCollectionL();
            break;
            }
        case EMcCmdClose:
        	  // called before destructing this plug-in: no actions required
        	  break;
        case EMcCloseCollection:
            {
            MPX_DEBUG2("CMPXDbPlugin::CommandL - EMcCloseCollection %d", aArg);
            // Close the specified database
            TRAP_IGNORE(iDbHandler->PreCloseCollectionL());
 #ifdef RD_MULTIPLE_DRIVE
            MPX_DEBUG1("Multiple drives closing databases");
            if ( aArg <0)
                {
                DriveInfo::TDriveArray driveArray;
                User::LeaveIfError ( DriveInfo::GetUserVisibleDrives( iFs, driveArray));
                TInt count( driveArray.Count ());
                for (TInt i=0; i<count; ++i)
                    {
                    MPX_DEBUG2("At drive %i", driveArray[i]);
                    if (( driveArray[i] != EDriveC) && (!iDbHandler->IsRemoteDrive(static_cast<TDriveNumber>(driveArray[i]))))
                        {
                        MPX_DEBUG2("Closing database %i", driveArray[i]);
                        TRAP_IGNORE( iDbHandler->CloseDatabaseL( driveArray[i] ) );
                        }
                    }
                }
            else
                {
                TRAP_IGNORE( iDbHandler->CloseDatabaseL(aArg) ); //Closing can fail if physical media has been removed or forced disk dismount has occurred.
                }
 #else
            iDbHandler->CloseDatabaseL(aArg);
 #endif // RD_MULTIPLE_DRIVE
            break;
            }
        case EMcReOpenCollection:
            {
            MPX_DEBUG1("CMPXDbPlugin::CommandL - EMcReOpenCollection");
            // Open the specified database
#ifdef RD_MULTIPLE_DRIVE
            MPX_DEBUG1("Multiple drives opening databases");
            DriveInfo::TDriveArray driveArray;
            User::LeaveIfError( DriveInfo::GetUserVisibleDrives( iFs, driveArray ) );
            TInt count( driveArray.Count() );
            for( TInt i=0; i<count; ++i )
                {
                MPX_DEBUG2("At drive %i", driveArray[i]);
                if( (driveArray[i] != EDriveC)  && (!iDbHandler->IsRemoteDrive(static_cast<TDriveNumber>(driveArray[i]))))
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
            MPX_DEBUG1("CMPXDbPlugin::CommandL - EMcRefreshStarted");
            iDbHandler->CheckDiskSpaceOnDrivesL();
            // ask the handler to start a transaction
            iDbHandler->RefreshStartL();
            iRefreshing=ETrue;
            break;
            }
        case EMcRefreshEnded:
            {
            MPX_DEBUG1("CMPXDbPlugin::CommandL - EMcRefreshEnded");
            // ask the handler to finalize the transaction
            iDbHandler->RefreshEndL();
            iRefreshing=EFalse;
            break;
            }
         case EMcCmdReCreateDB:
            {
            MPX_DEBUG1("CMPXDbPlugin::CommandL - EMcCmdReCreateDB");
            // Recreate all databases
            iDbHandler->ReCreateDatabasesL();
            break;
            }
         case EMcCmdDbCorrupted:
            {
            MPX_DEBUG1("CMPXDbPlugin::CommandL - EMcCmdDbCorrupted");
            iDbHandler->SetDBCorruptedL(ETrue);
            break;
            }
        case EMcCmdRefresh:
        case EMcCmdCollectionInit:
        case EMcCmdCollectionResyn:
            {
            // deprecated
            break;
            }
        case EMcCmdMtpStart:
            iDbHandler->CheckDiskSpaceOnDrivesL();
            iMtpInUse = ETrue;
            iDbHandler->MtpStartL();
            break;
        case EMcCmdMtpEnd:
            iMtpInUse = EFalse;
            iDbHandler->MtpEndL();
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
void CMPXDbPlugin::CommandL(
    CMPXCommand& aCmd)
    {
    MPX_FUNC("CMPXDbPlugin::CommandL");

    if (!aCmd.IsSupported(KMPXCommandGeneralId))
        {
        User::Leave(KErrArgument);
        }

    TMPXCommandId commandId = aCmd.ValueTObjectL<TMPXCommandId>(KMPXCommandGeneralId);

    TBool syncOp(EFalse);
    if (aCmd.IsSupported(KMPXCommandGeneralDoSync))
        {
        syncOp = aCmd.ValueTObjectL<TBool>(KMPXCommandGeneralDoSync);
        }

    // Handle this operation synchronously or asynchronously
    //
    if (!syncOp)
        {
        iActiveTask->StartL(commandId, aCmd);
        }
    else  // Sync operation
        {
        switch (commandId)
            {
            case KMPXCommandIdCollectionRetrieveUriForDeletion:
                {
                MPX_DEBUG1("CMPXDbPlugin::CommandL - KMPXCommandIdCollectionRetrieveUriForDeletion");
                DoRetrieveUriForDeletionL(aCmd);
                break;
                }
            case KMPXCommandIdCollectionRemove:
                {
                MPX_DEBUG1("CMPXDbPlugin::CommandL - KMPXCommandIdCollectionRemove");
                if ( !iDbHandler->InTransaction() )
                    {
                    iDbHandler->BeginTransactionL();
                    }

                if (iFirstDeleteStep )
                    {
                    iFirstDeleteStep = EFalse;
                    }
                DoRemovePathL(aCmd);
                break;
                }
            case KMPXCommandIdCollectionRemoveMedia:
                {
                MPX_DEBUG1("CMPXDbPlugin::CommandL - KMPXCommandIdCollectionRemoveMedia");
                DoRemoveMediaL(aCmd);
                break;
                }
            case KMPXCommandIdCollectionCleanupDeletedMedias:
                {
                MPX_DEBUG1("CMPXDbPlugin::CommandL - KMPXCommandIdCollectionCleanupDeletedMedias");
                CleanupDeletedRecordsL(aCmd);
                break;
                }
            case KMPXCommandIdCollectionAdd:
                {
                MPX_DEBUG1("CMPXDbPlugin::CommandL - KMPXCommandIdCollectioAdd");
                CMPXMedia* media = aCmd.Value<CMPXMedia>(KMPXCommandColAddMedia);
                User::LeaveIfNull( media );
                TUint32 id(DoAddL(*media));
                aCmd.SetTObjectValueL<TMPXItemId>(KMPXCommandColAddRtnId, id);
                break;
                }
            case KMPXCommandIdCollectionSet:
                {
                MPX_DEBUG1("CMPXDbPlugin::CommandL - KMPXCommandIdCollectionSet");
                CMPXMedia* media = aCmd.Value<CMPXMedia>(KMPXCommandColSetMedia);
                User::LeaveIfNull( media );
                DoSetL(*media);
                break;
                }
            case KMPXCommandIdCollectionCompleteDelete:
                {
                MPX_DEBUG1("CMPXDbPlugin::CommandL - KMPXCommandIdCollectionCompleteDelete");
                DoHandleDeleteCompleteL(aCmd);
                break;
                }
            case KMPXCommandIdReorderPlaylist:
                {
                MPX_DEBUG1("CMPXDbPlugin::CommandL - KMPXCommandIdReorderPlaylist");
                DoReorderPlaylistL(aCmd);
                break;
                }
            case KMPXCommandIdUpdateRefreshTime:
                {
                MPX_DEBUG1("CMPXDbPlugin::CommandL - KMPXCommandIdUpdateRefreshTime");
                TTime curTime;
                curTime.HomeTime();
                iDbHandler->SetLastRefreshedTimeL(curTime);
                break;
                }
            case KMPXCommandCollectionGetCount:
                {
                MPX_DEBUG1("CMPXDbPlugin::CommandL - KMPXCommandCollectionGetCount");
                DoGetCollectionCountL(aCmd);
                break;
                }
            case KMPXCommandCollectionGetURIs:
                {
                MPX_DEBUG1("CMPXDbPlugin::CommandL - KMPXCommandCollectionGetURIs");
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
// Adds an item (song or playlist) to the collection
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::AddL(
    const CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbPlugin::AddL");
    DoAddL(aMedia);
    }

// ----------------------------------------------------------------------------
// Remove an item from the collection database using the given path
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::RemoveL(
    const CMPXCollectionPath& aPath)
    {
    MPX_FUNC("CMPXDbPlugin::RemoveL(by path)");
    MPX_DEBUG_PATH(aPath);

    CMPXMessageArray* msgAry = CMPXMessageArray::NewL();
    CleanupStack::PushL(msgAry);

    // Return file path for deleted item(s)
    CDesCArray* fp = DoRemoveL(aPath, *msgAry);

    iObs->HandleRemove(*fp, KErrNone);
    delete fp;

    // Send Change Messages
    iActiveTask->SetVisibleChange(CMPXDbActiveTask::EAllVisible);
    DoHandleChangeL(msgAry);
    CleanupStack::PopAndDestroy(msgAry);
    }

// ----------------------------------------------------------------------------
// Remove an item from the collection database using the given media properties
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::RemoveL(
    const CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbPlugin::RemoveL(by media)");
    DoRemoveL(aMedia, EFalse);
    }

// ----------------------------------------------------------------------------
// Sets/updates the media for an item in the collection
// DEPRECATED for week 18
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::SetL(
    const CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbPlugin::SetL");
    DoSetL(aMedia);
    }

// ----------------------------------------------------------------------------
// Find the items matching the media specifications
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::FindAllL(
    const CMPXMedia& aCriteria,
    const TArray<TMPXAttribute>& aAttrs)
    {
    MPX_FUNC("CMPXDbPlugin::FindAllL");

    CMPXMedia* entries = FindAllSyncL(aCriteria, aAttrs);

    // notify client. if FindAllL leaves, framework will notify client of the error
    iObs->HandleFindAll(entries, KErrNone);
    delete entries;
    }

// ----------------------------------------------------------------------------
// Find the items matching the media specifications
// ----------------------------------------------------------------------------
//
CMPXMedia* CMPXDbPlugin::FindAllSyncL(
    const CMPXMedia& aCriteria,
    const TArray<TMPXAttribute>& aAttrs)
    {
    MPX_FUNC("CMPXDbPlugin::FindAllSyncL");

    CMPXMedia* entries = iDbHandler->FindAllLC(aCriteria, aAttrs);

    if (entries)
        {
        CMPXMediaArray* ary = entries->Value<CMPXMediaArray>(KMPXMediaArrayContents);
        User::LeaveIfNull( ary );
        DoSetDrmForArrayL( *ary, aAttrs );
        }

    CleanupStack::Pop(entries);
    return entries;
    }

// ----------------------------------------------------------------------------
// Get the list of supported capabilities
// ----------------------------------------------------------------------------
//
TCollectionCapability CMPXDbPlugin::GetCapabilities()
    {
    // This one supports simple search
    return EMcSearch;
    }

// ----------------------------------------------------------------------------
// Get the list of supported capabilities
// ----------------------------------------------------------------------------
//
TBool CMPXDbPlugin::HandleStepL()
    {
    MPX_FUNC("CMPXDbPlugin::HandleStepL");

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
            MPX_DEBUG1("CMPXDbPlugin::CommandL - KMPXCommandIdUpdateRefreshTime");
            TTime curTime;
            curTime.HomeTime();
            iDbHandler->SetLastRefreshedTimeL(curTime);
            break;
            }
        case KMPXCommandIdIncrementalOpenL:
            {
            DoIncrementalOpenL( iActiveTask->GetCommand() );
            done = ETrue;
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
void CMPXDbPlugin::HandleOperationCompleted(
    TInt aErr)
    {
    MPX_FUNC("CMPXDbPlugin::HandleOperationCompleted");
    TRAP_IGNORE(DoHandleOperationCompletedL(aErr));
    }

// ----------------------------------------------------------------------------
// Process the OpenL command
// ----------------------------------------------------------------------------
//
TBool CMPXDbPlugin::DoOpenL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aEntries,
    CMPXFilter* aFilter )
    {
    MPX_FUNC("CMPXDbPlugin::DoOpenL");

    TInt err( KErrNone );
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);

    TInt levels(aPath.Levels());
    TBool isASong(EFalse);


	if ( 1 == levels )
	    {

	    // Redirecting all open requests at level 1 to open albums 
	    // due to UI changes that removed the library menu collection level.
        TInt acount = array->Count();
        MPX_DEBUG2(" array count11 [%d]", acount);
        
 		CMPXCollectionPath* path = CMPXCollectionPath::NewL(aPath);
		CleanupStack::PushL( path );
		
		path->AppendL(3); // Albums
		TInt whatLevel = path->Levels();

		MPX_DEBUG_PATH(*path);
		
		aEntries.SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, path->Id(whatLevel - 1) );
		
      // Create a media which hold the pointer to the returned path
        if (aEntries.IsSupported(KMPXMediaGeneralValue))
            {
            MPX_DEBUG1(" pointer to the returned path ");
            CMPXMedia* pMedia = CMPXMedia::NewL();
            CleanupStack::PushL(pMedia);
            pMedia->SetTObjectValueL<TInt>(KMPXMediaGeneralValue,
                                          aEntries.ValueTObjectL<TInt>(KMPXMediaGeneralValue));
            array->AppendL(*pMedia);
            CleanupStack::PopAndDestroy(pMedia);
            }
		
	
        RArray<TMPXAttribute> openAttrs;
        CleanupClosePushL(openAttrs);

        RArray<TInt> supportedIds;
        CleanupClosePushL(supportedIds);

        SetAttributesL(*path, openAttrs, supportedIds);
        openAttrs.AppendL(KMPXMediaArrayContents);
        
        CleanupStack::PopAndDestroy(&supportedIds);
		
		if( iAllSongsValid )
		    {
		    isASong = DoOpenBrowseAlbumL( *path, openAttrs.Array(), aEntries, array );
		    }
		CleanupStack::PopAndDestroy(&openAttrs);
		CleanupStack::PopAndDestroy( path );

        //Remove the first media
        if ( array->Count() &&
            (*array)[0]->IsSupported(KMPXMediaGeneralValue))
            {
            array->Remove(0);
            }
	    }
    else if (levels >= 2)
        {
	    aEntries.SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, aPath.Id(levels - 1));
        // Create a media which hold the pointer to the returned path
        if (aEntries.IsSupported(KMPXMediaGeneralValue))
            {
            CMPXMedia* pMedia = CMPXMedia::NewL();
            CleanupStack::PushL(pMedia);
            pMedia->SetTObjectValueL<TInt>(KMPXMediaGeneralValue,
                                          aEntries.ValueTObjectL<TInt>(KMPXMediaGeneralValue));
            array->AppendL(*pMedia);
            CleanupStack::PopAndDestroy(pMedia);
            MPX_ASSERT(array->Count()==1);
            }

        // check the browse type
        switch (aPath.Id(1).iId2)
            {
            case EBrowseAll:
                {
                TBool doIncremental(ETrue);
                if( aFilter && aFilter->IsSupported( KMPXCollectionOpenLSupportsIncremental ) )
                    {
                    doIncremental =
                         aFilter->ValueTObjectL<TBool>(KMPXCollectionOpenLSupportsIncremental);
                    }

                if( doIncremental )
                    {
                    TRAP( err, isASong = DoOpenIncrementalL( aPath, aAttrs, aEntries, array ) );
                    }
                else
                    {
                    TRAP( err, isASong = DoOpenBrowseAllL(aPath, aAttrs, aEntries, array) );
                    }
                break;
                }

            case EBrowseArtist:
                {
                if( iAllSongsValid )
                    {
                    isASong = DoOpenBrowseArtistL( aPath, aAttrs, aEntries, array );
                    }
                break;
                }

            case EBrowseAlbum:
            case EBrowseAlbumMediaWall:
                {
                if( iAllSongsValid )
                    {
                    isASong = DoOpenBrowseAlbumL( aPath, aAttrs, aEntries, array );
                    }
                break;
                }

            case EBrowsePlaylist:
                {
                if( iAllSongsValid )
                    {
                    isASong = DoOpenBrowsePlaylistL(aPath, aAttrs, aEntries, array);
                    }

                break;
                }

            case EBrowseGenre:
                {
                if( iAllSongsValid )
                    {
                    isASong = DoOpenBrowseGenreL( aPath, aAttrs, aEntries, array );
                    }
                break;
                }

            case EBrowseComposer:
                {
                if( iAllSongsValid )
                    {
                    isASong = DoOpenBrowseComposerL( aPath, aAttrs, aEntries, array );
                    }
                break;
                }

            default:
                {
                User::Leave(KErrArgument);
                }
            }
        //Remove the first media
        if ( array->Count() &&
            (*array)[0]->IsSupported(KMPXMediaGeneralValue))
            {
            array->Remove(0);
            }
        }
    else
        {
        User::Leave(KErrNotSupported);
        }
    aEntries.SetCObjectValueL(KMPXMediaArrayContents, array);
    aEntries.SetTObjectValueL<TInt>(KMPXMediaArrayCount, array->Count());

    CleanupStack::PopAndDestroy(array);

    if( err == KSQLErrGeneral )
        {
        iAllSongsValid = EFalse;
        User::Leave( KErrDiskFull );
        }
    else if( err != KErrNone )
        {
        User::Leave( err );
        }
    return isASong;
    }


// ----------------------------------------------------------------------------
// CMPXDbPlugin::DoOpenIncrementalL
// ----------------------------------------------------------------------------
//
TBool CMPXDbPlugin::DoOpenIncrementalL( const CMPXCollectionPath& aPath,  const TArray<TMPXAttribute>& aAttrs,
                                       CMPXMedia& aEntries, CMPXMediaArray* aArray)
    {
    TBool isASong(EFalse);

    TInt levels(aPath.Levels());

    if( levels == 2 )
        {
        // Remove the pPath dummy from the array
        TInt pPath = aEntries.ValueTObjectL<TInt>(KMPXMediaGeneralValue);
        aArray->Remove(0);
        CMPXCollectionPath* p = (CMPXCollectionPath*) pPath;

        RArray<TMPXItemId> ids;
        CleanupClosePushL( ids );

        iDbHandler->GetAllSongsLimitedL( aAttrs, *aArray, KFirstFetchCount );

        TInt c( aArray->Count() );
        for( TInt i=0; i<c; ++i )
            {
            TMPXItemId id = aArray->AtL(i)->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
            ids.AppendL( id );
            }

        // Rest are all blank items
        CMPXMedia* entry = CMPXMedia::NewL();
        CleanupStack::PushL(entry);
        entry->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
        entry->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXSong);
        entry->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, KMPXInvalidItemId );

        TInt count = iDbHandler->NumberOfItemsL(EMPXSong);
        count-=c;
        for( TInt i=0; i<count; ++i )
            {
            aArray->AppendL( *entry );
            ids.AppendL( KMPXInvalidItemId );
            }
        CleanupStack::PopAndDestroy( entry );

        // Set the "Supportes incremental Command" flag
        //
        aEntries.SetTObjectValueL( KMPXCollectionOpenLSupportsIncremental, ETrue );

        TMPXOpenDataBlock block;
        block.iOffset=KErrNotFound;
        block.iSize=count;
        aEntries.SetTObjectValueL<TMPXOpenDataBlock>( KMPXCollectionOpenLResultRange, block );

        SetMediaGeneralAttributesL(aEntries, EMPXGroup, EMPXSong,
                iMusicLibraryTitles->MdcaPoint(EBrowseAll));

        p->AppendL(ids.Array());
        CleanupStack::PopAndDestroy( &ids );
        }
    else if( levels == 3 )
        {
        iDbHandler->GetSongL(aPath.Id(levels - 1).iId2, aAttrs, *aArray);
        isASong = ETrue;
        }


    return isASong;
    }

// ----------------------------------------------------------------------------
// Handles OpenL called for EBrowseAll
// ----------------------------------------------------------------------------
//
TBool CMPXDbPlugin::DoOpenBrowseAllL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aEntries,
    CMPXMediaArray* aArray)
    {
    MPX_FUNC("CMPXDbPlugin::DoOpenBrowseAllL");

    TInt levels(aPath.Levels());
    switch (levels)
       {
       // All songs
       case 2:
            {
            MPX_PERF_START(CMPXDbPlugin_DoOpenBrowseAllL_All);

            iDbHandler->GetAllSongsL(aArray, aAttrs);
            SetMediaGeneralAttributesL(aEntries, EMPXGroup, EMPXSong,
                iMusicLibraryTitles->MdcaPoint(EBrowseAll));

            MPX_PERF_END(CMPXDbPlugin_DoOpenBrowseAllL_All);
            break;
            }

         // A Song in all songs
         case 3:
            {
            MPX_PERF_START(CMPXDbPlugin_DoOpenBrowseAllL_Song);

            iDbHandler->GetSongL(aPath.Id(levels - 1).iId2, aAttrs, *aArray);

            MPX_PERF_END(CMPXDbPlugin_DoOpenBrowseAllL_Song);
            break;
            }

         default:
            {
            MPX_DEBUG2("CMPXDbPlugin_DoOpenBrowseAllL: Invalid levels[%d]", levels);
            User::Leave(KErrNotSupported);
            }
        } // end switch(levels)

    return (levels == 3);
    }

// ----------------------------------------------------------------------------
// Handles OpenL called for EBrowseArtist
// ----------------------------------------------------------------------------
//
TBool CMPXDbPlugin::DoOpenBrowseArtistL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aEntries,
    CMPXMediaArray* aArray)
    {
    MPX_FUNC("CMPXDbPlugin::DoOpenBrowseArtistL");

    TBool isASong(EFalse);
    TInt levels(aPath.Levels());
    TInt idIndex(levels - 1);

    RArray<TMPXItemId> selections;
    CleanupClosePushL(selections);
    aPath.SelectionL(selections);

    switch (levels)
        {
        // All artists
        case 2:
            {
            MPX_PERF_START(CMPXDbPlugin_DoOpenBrowseArtistL_All);

            iDbHandler->GetAllArtistsL(aAttrs, aArray);
            SetMediaGeneralAttributesL(aEntries, EMPXGroup, EMPXArtist,
                iMusicLibraryTitles->MdcaPoint(EBrowseArtist));

            MPX_PERF_END(CMPXDbPlugin_DoOpenBrowseArtistL_All);
            break;
            }

        // All albums of an artist
        case 3:
            {
            MPX_PERF_START(CMPXDbPlugin_DoOpenBrowseArtistL_AllAlbums);
            // get the albums
            TInt id(aPath.Id(idIndex).iId2);
            MPX_ASSERT(aArray->Count());
            MPX_ASSERT((*aArray)[0]->IsSupported(KMPXMediaGeneralValue));
            TInt pPath = (*aArray)[0]->ValueTObjectL<TInt>(KMPXMediaGeneralValue);
            MPX_ASSERT(pPath);
            iDbHandler->GetAlbumsMatchingArtistL(id, aAttrs, *aArray);
            SetMediaGeneralAttributesL(aEntries, EMPXItem, EMPXArtist, id);

            TInt count = aArray->Count();
            if ( count > 2 )
                {
                // Only show "all" item if there is more than 1 album
                // count for "all" item
                TInt total = 0;
                for ( TInt i = 1; i < count; i++ )
                {
                TInt temp = (*aArray)[i]->ValueTObjectL<TInt>(KMPXMediaGeneralCount);
                total += temp;
                }
            // Add "all" item under an artist
            MPXDbCommonUtil::PrependMediaL(*aArray, *iAllSongsForArtistTitle, EMPXItem, EMPXAlbum,
                aPath.Id(idIndex), 0, 0, 1);

                (*aArray)[1]->SetTObjectValueL<TInt>(KMPXMediaGeneralCount, total);

            TMPXItemId allId = ((*aArray)[1]->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId));
            if (aArray->Count() > 2)
                { // path media, all id  and at least one media
                ((CMPXCollectionPath*)pPath)->InsertL(allId, 0);
                }
            else
                { // only has all item
                ((CMPXCollectionPath*)pPath)->AppendL(allId);
                    }
                }

            MPX_PERF_END(CMPXDbPlugin_DoOpenBrowseArtistL_AllAlbums);
            break;
            }

        // All songs of an album for an artist or all songs for an artist
        case 4:
            {
            // all songs for an artist
            if (aPath.Id(3) == aPath.Id(2))
                {
                MPX_PERF_START(CMPXDbPlugin_DoOpenBrowseArtistL_AllSongs);

                TInt id(aPath.Id(idIndex - 1).iId2);
                iDbHandler->GetSongsMatchingArtistL(id, aAttrs, aArray);
                SetMediaGeneralAttributesL(aEntries, EMPXItem, EMPXSong, id);

                MPX_PERF_END(CMPXDbPlugin_DoOpenBrowseArtistL_AllSongs);
                }
            // all songs of an album for an artist
            else
                {
                MPX_PERF_START(CMPXDbPlugin_DoOpenBrowseArtistL_AllSongsForAlbum);
                TUint32 artistId(aPath.Id(idIndex - 1).iId2);

                if (selections.Count())
                    {
                    // Multiple albums
                    const TInt count(aPath.Selection().Count());
                    for (TInt i = 0; i < count; ++i)
                        {
                        iDbHandler->GetSongsMatchingArtistAndAlbumL(artistId, selections[i].iId2,
                            aAttrs, aArray);
                        }
                    }
                else
                    {
                    // One album
                    iDbHandler->GetSongsMatchingArtistAndAlbumL(artistId, aPath.Id(idIndex).iId2,
                        aAttrs, aArray);
                    }

                SetMediaGeneralAttributesL(aEntries, EMPXItem, EMPXAlbum, aPath.Id(idIndex).iId2);
                MPX_PERF_END(CMPXDbPlugin_DoOpenBrowseArtistL_AllSongsForAlbum);
                }

            MPX_DEBUG2("CMPXDbPlugin_DoOpenBrowseArtistL: retrieved %d items", aArray->Count());

            break;
            }

        // A Song in an album
        case 5:
            {
            MPX_PERF_START(CMPXDbPlugin_DoOpenBrowseArtistL_Song);

            iDbHandler->GetSongL(aPath.Id(idIndex).iId2, aAttrs, *aArray);
            isASong = ETrue;

            MPX_PERF_END(CMPXDbPlugin_DoOpenBrowseArtistL_Song);
            break;
            }

        default:
            {
            MPX_DEBUG2("CMPXDbPlugin_DoOpenBrowseArtistL: Invalid levels[%d]", levels);
            User::Leave(KErrNotSupported);
            }
        } // end switch(level)

    CleanupStack::PopAndDestroy(&selections);
    return isASong;
    }

// ----------------------------------------------------------------------------
// Handles OpenL called for EBrowseAlbum
// ----------------------------------------------------------------------------
//
TBool CMPXDbPlugin::DoOpenBrowseAlbumL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aEntries,
    CMPXMediaArray* aArray)
    {
    MPX_FUNC("CMPXDbPlugin::DoOpenBrowseAlbumL");

    TBool isASong(EFalse);
    TInt levels(aPath.Levels());
    TInt idIndex(levels - 1);

    RArray<TMPXItemId> selections;
    CleanupClosePushL(selections);
    aPath.SelectionL(selections);

    switch (levels)
        {
        // All Albums
        case 2:
            {
            MPX_PERF_START(CMPXDbPlugin_DoOpenBrowseAlbumL_All);
            TInt err = 0;
            if( aPath.Id(1).iId2 == EBrowseAlbumMediaWall ) 
                {
                TRAP(err, iDbHandler->GetAllAlbumsMediaWallL(aAttrs, aArray) );
                }
            else 
                {
                TRAP(err, iDbHandler->GetAllAlbumsL(aAttrs, aArray) );
                }
            // in error case, return empty list and append empty id to path 
            // in order to increase one level 
            if ( err != KErrNone )
                {
                TInt pPath(0);
                if (aArray->Count())
                    {
                    CMPXMedia* pMedia = (*aArray)[0];
                    if (pMedia->IsSupported(KMPXMediaGeneralValue))
                        {
                        pPath = pMedia->ValueTObjectL<TInt>(KMPXMediaGeneralValue);
                        MPX_ASSERT(pPath);
                        }                   
                    }

                RArray<TMPXItemId> ids;
                CleanupClosePushL(ids);
            
                // Append ids to the returned path
                if (pPath)
                    {
                    ((CMPXCollectionPath*)pPath)->AppendL(ids.Array());
                    }
                CleanupStack::PopAndDestroy(&ids);
                }
            
            SetMediaGeneralAttributesL(aEntries, EMPXGroup, EMPXAlbum,
                iMusicLibraryTitles->MdcaPoint(EBrowseAlbum));

            MPX_PERF_END(CMPXDbPlugin_DoOpenBrowseAlbumL_All);
            break;
            }

        // All songs in one or multiple albums
        case 3:
            {
            MPX_PERF_START(CMPXDbPlugin_DoOpenBrowseAlbumL_AllSongs);
            if (selections.Count())
                {
                // Multiple albums
                const TInt count(aPath.Selection().Count());
                for (TInt i = 0; i < count; ++i)
                    {
                    iDbHandler->GetSongsMatchingAlbumL(selections[i].iId2, aAttrs, aArray);
                    }
                }
            else
                {
                // One album
                iDbHandler->GetSongsMatchingAlbumL(aPath.Id(idIndex).iId2, aAttrs, aArray);
                }

				// added for ganes
                SetMediaGeneralAttributesL(aEntries, EMPXItem, EMPXAlbum, iMusicLibraryTitles->MdcaPoint(EBrowseAlbumSong));

            MPX_PERF_END(CMPXDbPlugin_DoOpenBrowseAlbumL_AllSongs);
            break;
            }

        // A song in an album
        case 4:
            {
            MPX_PERF_START(CMPXDbPlugin_DoOpenBrowseAlbumL_Song);

            iDbHandler->GetSongL(aPath.Id(idIndex).iId2, aAttrs, *aArray);
            isASong = ETrue;

            MPX_PERF_END(CMPXDbPlugin_DoOpenBrowseAlbumL_Song);
            break;
            }

        default:
            {
            MPX_DEBUG2("CMPXDbPlugin_DoOpenBrowseAlbumL: Invalid levels[%d]", levels);
            User::Leave(KErrNotSupported);
            }
        }

    CleanupStack::PopAndDestroy(&selections);
    return isASong;
    }

// ----------------------------------------------------------------------------
// Handles OpenL called for EBrowsePlaylist
// ----------------------------------------------------------------------------
//
TBool CMPXDbPlugin::DoOpenBrowsePlaylistL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aEntries,
    CMPXMediaArray* aArray)
    {
    MPX_FUNC("CMPXDbPlugin::DoOpenBrowsePlaylistL");

    TBool isASong(EFalse);
    TInt levels(aPath.Levels());
    TInt idIndex(levels - 1);

    RArray<TMPXItemId> selections;
    CleanupClosePushL(selections);
    aPath.SelectionL(selections);

    switch (levels)
         {
         case 2:
            {
            // All playlists
            MPX_PERF_START(CMPXDbPlugin_DoOpenBrowsePlaylistL_All);

            iDbHandler->GetAllPlaylistsL(aArray, aAttrs);
            iDbHandler->GetAllSystemPlaylistNamesL(aArray);

            SetMediaGeneralAttributesL(aEntries, EMPXGroup, EMPXPlaylist,
                iMusicLibraryTitles->MdcaPoint(EBrowsePlaylist));

            MPX_PERF_END(CMPXDbPlugin_DoOpenBrowsePlaylistL_All);
            break;
            }

         // All songs in a playlist
         case 3:
            {
            MPX_PERF_START(CMPXDbPlugin_DoOpenBrowsePlaylistL_AllSongs);

            if (selections.Count())
                {
                const TInt count(aPath.Selection().Count());
                for (TInt i = 0; i < count; ++i)
                    {
                    iDbHandler->GetSongsMatchingPlaylistL(selections[i].iId2, aAttrs, aArray);
                    }
                }
            else
                {
                iDbHandler->GetSongsMatchingPlaylistL(aPath.Id (idIndex).iId2, aAttrs, aArray);
                }


            SetMediaGeneralAttributesL(aEntries, EMPXItem, EMPXPlaylist, aPath.Id(idIndex).iId2);

            // populate EMPXMediaGeneralNonPermissibleActions
            if (iDbHandler->IsAutoPlaylistL(aPath.Id(idIndex).iId2))
                {
                // set non-permissible actions to not writable and cacheable
                aEntries.SetTObjectValueL<TMPXGeneralNonPermissibleActions>(
                    KMPXMediaGeneralNonPermissibleActions, (TMPXGeneralNonPermissibleActions)(EMPXWrite | EMPXCache));
                }

            MPX_PERF_END(CMPXDbPlugin_DoOpenBrowsePlaylistL_AllSongs);
            break;
            }

         // Specific song in a playlist
         case 4:
            {
            MPX_PERF_START(CMPXDbPlugin_DoOpenBrowsePlaylistL_Song);

            iDbHandler->GetPlaylistSongL(aPath.Id(idIndex).iId2, aPath.Id(idIndex - 1).iId2,
                aAttrs, *aArray);
            isASong = ETrue;

            MPX_PERF_END(CMPXDbPlugin_DoOpenBrowsePlaylistL_Song);
            break;
            }

         default:
            {
            MPX_DEBUG2("CMPXDbPlugin_DoOpenBrowsePlaylistL: Invalid levels[%d]", levels);
            User::Leave(KErrNotSupported);
            }
       }

    CleanupStack::PopAndDestroy(&selections);
    return isASong;
    }

// ----------------------------------------------------------------------------
// Handles OpenL called for EBrowseGenre
// ----------------------------------------------------------------------------
//
TBool CMPXDbPlugin::DoOpenBrowseGenreL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aEntries,
    CMPXMediaArray* aArray)
    {
    MPX_FUNC("CMPXDbPlugin::DoOpenBrowseGenreL");

    TBool isASong(EFalse);
    TInt levels(aPath.Levels());
    TInt idIndex(levels - 1);

    RArray<TMPXItemId> selections;
    CleanupClosePushL(selections);
    aPath.SelectionL(selections);

    switch (levels)
        {
        // All genres
         case 2:
            {
            MPX_PERF_START(CMPXDbPlugin_DoOpenBrowseGenreL_All);

            iDbHandler->GetAllGenresL(aAttrs, aArray);
            SetMediaGeneralAttributesL(aEntries, EMPXGroup, EMPXGenre,
                iMusicLibraryTitles->MdcaPoint(EBrowseGenre));

            MPX_PERF_END(CMPXDbPlugin_DoOpenBrowseGenreL_All);
            break;
            }

        // All songs of a genre
        case 3:
            {
            MPX_PERF_START(CMPXDbPlugin_DoOpenBrowseGenreL_AllSongs);

            if (selections.Count())
                {
                const TInt count(aPath.Selection().Count());
                for (TInt i = 0; i < count; ++i)
                    {
                    iDbHandler->GetSongsMatchingGenreL(selections[i].iId2, aAttrs, aArray);
                    }
                }
            else
                {
                iDbHandler->GetSongsMatchingGenreL(aPath.Id(idIndex).iId2, aAttrs, aArray);
                }
            SetMediaGeneralAttributesL(aEntries, EMPXItem, EMPXGenre, aPath.Id(idIndex).iId2);

            MPX_PERF_END(CMPXDbPlugin_DoOpenBrowseGenreL_AllSongs);
            break;
            }

        // Specific song in a genre
        case 4:
            {
            MPX_PERF_START(CMPXDbPlugin_DoOpenBrowseGenreL_Song);

            iDbHandler->GetSongL(aPath.Id(idIndex).iId2, aAttrs, *aArray);
            isASong = ETrue;

            MPX_PERF_END(CMPXDbPlugin_DoOpenBrowseGenreL_Song);
            break;
            }

        default:
            {
            MPX_DEBUG2("CMPXDbPlugin_DoOpenBrowseGenreL: Invalid levels[%d]", levels);
            User::Leave(KErrNotSupported);
            }
        }

    CleanupStack::PopAndDestroy(&selections);
    return isASong;
    }

// ----------------------------------------------------------------------------
// Handles OpenL called for EBrowseComposer
// ----------------------------------------------------------------------------
//
TBool CMPXDbPlugin::DoOpenBrowseComposerL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aEntries,
    CMPXMediaArray* aArray)
    {
    MPX_FUNC("CMPXDbPlugin::DoOpenBrowseComposerL");

    TBool isASong(EFalse);
    TInt levels(aPath.Levels());
    TInt idIndex(levels - 1);

    RArray<TMPXItemId> selections;
    CleanupClosePushL(selections);
    aPath.SelectionL(selections);

    switch (levels)
        {
        // All composers
        case 2:
            {
            MPX_PERF_START(CMPXDbPlugin_DoOpenBrowseComposerL_All);

            iDbHandler->GetAllComposersL(aAttrs, aArray);
            SetMediaGeneralAttributesL(aEntries, EMPXGroup, EMPXComposer,
                iMusicLibraryTitles->MdcaPoint(EBrowseComposer));

            MPX_PERF_END(CMPXDbPlugin_DoOpenBrowseComposerL_All);
            break;
            }

        // All songs of a composer
        case 3:
            {
            MPX_PERF_START(CMPXDbPlugin_DoOpenBrowseComposerL_AllSongs);
            if (selections.Count())
                {
                const TInt count(aPath.Selection().Count());
                for (TInt i = 0; i < count; ++i)
                    {
                    iDbHandler->GetSongsMatchingComposerL(selections[i].iId2, aAttrs, aArray);
                    }
                }
            else
                {
                iDbHandler->GetSongsMatchingComposerL(aPath.Id(idIndex).iId2, aAttrs, aArray);
                }
            SetMediaGeneralAttributesL(aEntries, EMPXItem, EMPXComposer, aPath.Id(idIndex).iId2);

            MPX_PERF_END(CMPXDbPlugin_DoOpenBrowseComposerL_AllSongs);
            break;
            }

        // Specific song of a composer
        case 4:
            {
            MPX_PERF_START(CMPXDbPlugin_DoOpenBrowseComposerL_Song);

            iDbHandler->GetSongL(aPath.Id(idIndex).iId2, aAttrs, *aArray);
            isASong = ETrue;

            MPX_PERF_END(CMPXDbPlugin_DoOpenBrowseComposerL_Song);
            break;
            }

        default:
            {
            MPX_DEBUG2("CMPXDbPlugin_DoOpenBrowseComposerL: Invalid levels[%d]", levels);
            User::Leave(KErrNotSupported);
            }
        }

    CleanupStack::PopAndDestroy(&selections);
    return isASong;
    }

// ----------------------------------------------------------------------------
// Process the OpenL method with open mode EMPXOpenPlaylistOnly
// ----------------------------------------------------------------------------
//
CMPXCollectionPath* CMPXDbPlugin::DoOpenPlaylistL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs )
    {
    MPX_FUNC("CMPXDbPlugin::DoOpenPlaylistL");

    RArray<TMPXItemId> ids;
    CleanupClosePushL(ids);

    CMPXMedia* entries = CMPXMedia::NewL();
    CleanupStack::PushL(entries);

    CMPXCollectionPath* path = CMPXCollectionPath::NewL(aPath);
    CleanupStack::PushL(path);

    // Go through the browse path
    TInt levels(aPath.Levels());
    if (levels == 2)
        {
        // Create a new collection path
        CleanupStack::PopAndDestroy( path );
        path = CMPXCollectionPath::NewL();
        CleanupStack::PushL( path );

        // Always return all songs here
        ids.Reset();
        ids.AppendL( KDBPluginUid );
        path->AppendL(ids.Array());
        path->SelectL((TMPXItemId) KDBPluginUid);

        ids.Reset();
        ids.AppendL(EBrowseAll);
        path->AppendL(ids.Array());
        path->SelectL((TMPXItemId) EBrowseAll);
        path->Set(EMPXOpenPlaylistOnly);

        // Get all item IDs
        CMPXMediaArray* array = CMPXMediaArray::NewL();
        CleanupStack::PushL(array);

        DoOpenBrowseAllL(*path, aAttrs, *entries, array);

        entries->SetCObjectValueL(KMPXMediaArrayContents, array);
        entries->SetTObjectValueL<TInt>(KMPXMediaArrayCount, array->Count());

        CleanupStack::PopAndDestroy(array);

        DoAppendLevelL(*path, *entries);
        }
    else if (levels > 2)
        {
        switch (aPath.Id(1).iId2)
            {
            case EBrowseAll:
                {
                path->Set(EMPXOpenPlaylistOnly);
                // Returns the same path that we copied
                break;
                }
            case EBrowseArtist:
                {
                if (levels == 3)
                    {
                    // return all songs of a particular artist (currently highlighted)
                    path->Set(EMPXOpenGroupOrPlaylist);
                    ids.Reset();
                    ids.AppendL(aPath.Id(2));
                    path->AppendL(ids.Array());
                    path->SelectL(aPath.Id(2));
                    path->Set(EMPXOpenPlaylistOnly);

                    // Opens all songs of an artist and create the corresponding
                    // Collection playlist by appending all songs of an artist to
                    // the collection path
                    //
                    DoOpenL(*path, aAttrs, *entries, NULL);
                    DoAppendLevelL(*path, *entries);
                    }
                else if (levels == 4)
                    {
                    // Open the album of an artist and create the corresponding
                    // Collection playlist by appending all songs of an artist to
                    // the collection path
                    //
                    path->Set(EMPXOpenPlaylistOnly);
                    DoOpenL(*path, aAttrs, *entries, NULL);
                    DoAppendLevelL(*path, *entries);
                    }
                else
                    {
                    // case is a song no need to open again!
                    }

                break;
                }
            // Playlist, album, genre and composer easier, only 2 levels deep
            // plugin | category | category contents | songs of category
            //
            case EBrowsePlaylist:
            case EBrowseAlbum:
            case EBrowseGenre:
            case EBrowseComposer:
                {
                if (!DoOpenL(aPath, aAttrs, *entries, NULL))
                    {
                    // If it is not at a song level
                    // Append all entries to create collection path
                    //
                    path->Set(EMPXOpenPlaylistOnly);
                    DoAppendLevelL( *path, *entries );
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
void CMPXDbPlugin::DoMediaL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aEntries)
    {
    MPX_FUNC("CMPXDbPlugin::DoMediaL");

    // Fetch Media for root level
    //
    if (aPath.Levels() == 1) //root
        {
        DoRootMediaL( aAttrs, aEntries );
        }
    // Ensure the database has been merged before attempting MediaL()
    //
    else
        {
        CMPXMediaArray* array = CMPXMediaArray::NewL();
        CleanupStack::PushL(array);

        switch (aPath.Id(1).iId2)
            {
            case EBrowseAll:
                {
                DoAllSongsMediaL(aPath, aAttrs, aEntries, *array);
                break;
                } // end case EBrowseAll

            case EBrowseArtist:
                {
                DoArtistMediaL(aPath, aAttrs, aEntries, *array);
                break;
                } // end case EBrowseArtist

            case EBrowseAlbum:
                {
                DoCategoryMediaL(aPath, aAttrs, EMPXAlbum, aEntries, *array);
                break;
                } // end case EBrowseAlbum

            case EBrowsePlaylist:
                {
                DoCategoryMediaL(aPath, aAttrs, EMPXPlaylist, aEntries, *array);
                break;
                } // end case EBrowsePlaylist

            case EBrowseGenre:
                {
                DoCategoryMediaL(aPath, aAttrs, EMPXGenre, aEntries, *array);
                break;
                } // end case EBrowseGenre

            case EBrowseComposer:
                {
                DoCategoryMediaL(aPath, aAttrs, EMPXComposer, aEntries, *array);
                break;
                } // end case EBrowseComposer
#ifdef __ENABLE_PODCAST_IN_MUSIC_MENU
            case EBrowsePodcasts:
                {
                break;
                }
#endif
            default:
                {
                User::Leave(KErrArgument);
                }
            } // end switch(aPath.id(1)

        if (array->Count() > 0)
            {
            aEntries.SetCObjectValueL(KMPXMediaArrayContents, array);
            aEntries.SetTObjectValueL<TInt>(KMPXMediaArrayCount, array->Count());
            }
        CleanupStack::PopAndDestroy(array);
        }
    // Else case cannot leave, because this will happen when we have no disk space to
    // perform the merging. It should NOT leave.
    //
    }

// ----------------------------------------------------------------------------
// Find the collection media for root level
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::DoRootMediaL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aMedia )
    {
    MPX_FUNC("CMPXDbPlugin::DoRootMediaL");

    TInt count(aAttrs.Count());
#ifndef __ENABLE_PODCAST_IN_MUSIC_MENU
    aMedia.SetTObjectValueL<TMPXGeneralNonPermissibleActions> (
            KMPXMediaGeneralNonPermissibleActions, (TMPXGeneralNonPermissibleActions)(EMPXWrite | EMPXCache) );
#endif // __ENABLE_PODCAST_IN_MUSIC_MENU
    for (TInt i = 0; i < count; ++i)
        {
        if (aAttrs[i].ContentId() == KMPXMediaIdGeneral)
            {
            TUint att(aAttrs[i].AttributeId());
            if (att & EMPXMediaGeneralTitle)
                {
                HBufC* title(iResource->ReadHBufCL(R_MPX_QTN_MUS_MUSIC));
                CleanupStack::PushL(title);
                aMedia.SetTextValueL(KMPXMediaGeneralTitle, *title);
                CleanupStack::PopAndDestroy(title);
                }
            if (att & EMPXMediaGeneralSubTitle)
                {
                TInt numSongs(iDbHandler->NumberOfItemsL(EMPXSong));
                aMedia.SetTObjectValueL<TInt>(KMPXMediaGeneralCount, numSongs);

                HBufC* text(iResource->ReadHBufCL((1 == numSongs) ?
                    R_MPX_QTN_MUS_MUSIC_ONE_SONG : R_MPX_QTN_MUS_MUSIC_NUM_SONGS));

                CleanupStack::PushL(text);
                aMedia.SetTextValueL(KMPXMediaGeneralSubTitle, *text);
                CleanupStack::PopAndDestroy(text);
                }
            if (att & EMPXMediaGeneralIcon)
                {
/*                TIconInfo icon;
                icon.bmpfile = KMPlayerDbPluginMbmFile;
                icon.bitmapId = EMbmMpxdbpluginQgn_graf_mup_dlst_music;
                icon.maskId = EMbmMpxdbpluginQgn_graf_mup_dlst_music_mask;
                aMedia.SetTObjectValueL<TIconInfo>(KMPXMediaGeneralIcon, icon);
*/
// commented out for hb migration
                }
            } // if
        } // for
    }

// ----------------------------------------------------------------------------
// Find the collection media for all songs category
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::DoAllSongsMediaL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aEntries,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbPlugin::DoAllSongsMediaL");

    TInt levels(aPath.Levels());

    switch (levels)
       {
       // All songs
       case 2:
            {
            MPX_PERF_START(CMPXDbPlugin_DoAllSongsMediaL_All);
            DoRootCategoryMediaL(aAttrs, EBrowseAll, EMPXSong, aEntries);
            MPX_PERF_END(CMPXDbPlugin_DoAllSongsMediaL_All);
            break;
            }

         // A Song in all songs
         case 3:
            {
            MPX_PERF_START(CMPXDbPlugin_DoAllSongsMediaL_Song);
            GetSongInfoL(aPath, aAttrs, aEntries, aMediaArray);
            MPX_PERF_END(CMPXDbPlugin_DoAllSongsMediaL_Song);
            break;
            }

         default:
            {
            MPX_DEBUG2("CMPXDbPlugin_DoAllSongsMediaL: Invalid levels[%d]", levels);
            User::Leave(KErrNotSupported);
            }
        } // end switch(levels)
    }

// ----------------------------------------------------------------------------
// Find the collection media for artists category
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::DoArtistMediaL (
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aEntries,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbPlugin::DoArtistMediaL");

    TInt levels(aPath.Levels());
    TInt count(aPath.Selection().Count());

     // All artists
    if (levels == 2)
        {
        MPX_PERF_START(CMPXDbPlugin_DoArtistMediaL_All);
        DoRootCategoryMediaL(aAttrs, EBrowseArtist, EMPXArtist, aEntries);
        MPX_PERF_END(CMPXDbPlugin_DoArtistMediaL_All);
        }
    else if ((levels == 3) && count) // multiple artists selected
        {
        RArray<TMPXItemId> selections;
        CleanupClosePushL(selections);
        aPath.SelectionL(selections);

        for (TInt i = 0; i < count; ++i)
            {
            CMPXMedia* artist = CMPXMedia::NewL();
            CleanupStack::PushL(artist);
            iDbHandler->GetCategoryL(selections[i].iId2, EMPXArtist, aAttrs, artist);
            aMediaArray.AppendL(*artist);
            CleanupStack::PopAndDestroy(artist);
            }

        CleanupStack::PopAndDestroy(&selections);
        }
    else if (levels == 3) // single artist selected
        {
        iDbHandler->GetCategoryL(aPath.Id(2).iId2, EMPXArtist, aAttrs, &aEntries);
        }
    else if (levels == 4 && (aPath.Id(3) == aPath.Id(2))) // all songs for an artist
        {
        MPX_PERF_START(CMPXDbPlugin_DoArtistMediaL_AllSongs);
        // Calculate duration directly with SQL
        if (MPXDbCommonUtil::AttributeExists(aAttrs, KMPXMediaGeneralDuration))
            {
            DoDurationL(aEntries, EMPXArtist, aPath.Id(2));
            }

        MPX_PERF_END(CMPXDbPlugin_DoArtistMediaL_AllSongs);
        }
    else if ((levels == 4) && count) // multiple albums of an artist
        {
        MPX_PERF_START(CMPXDbPlugin_DoArtistMediaL_AllAlbums);

        RArray<TMPXItemId> selections;
        CleanupClosePushL(selections);
        aPath.SelectionL(selections);

        for (TInt i = 0; i < count; ++i)
            {
            CMPXMedia* media = CMPXMedia::NewL();
            CleanupStack::PushL(media);
            iDbHandler->GetCategoryL(selections[i].iId2, EMPXAlbum, aAttrs, media);
            aMediaArray.AppendL(*media);
            CleanupStack::PopAndDestroy(media);
            }

        CleanupStack::PopAndDestroy(&selections);

        MPX_PERF_END(CMPXDbPlugin_DoArtistMediaL_AllAlbums);
        }
    else if (levels == 4) // one album of an artist
        {
        MPX_PERF_START(CMPXDbPlugin_DoArtistMediaL_OneAlbum);
        iDbHandler->GetCategoryL(aPath.Id(3).iId2, EMPXAlbum, aAttrs, &aEntries);

        // Calculate duration
        if (MPXDbCommonUtil::AttributeExists(aAttrs, KMPXMediaGeneralDuration))
            {
            DoDurationL(aEntries, EMPXArtist, aPath.Id(2), EMPXAlbum, aPath.Id(3));
            }

        MPX_PERF_END(CMPXDbPlugin_DoArtistMediaL_OneAlbum);
        }
     else if (levels == 5) // a song/songs in an album
        {
        MPX_PERF_START(CMPXDbPlugin_DoArtistMediaL_Song);
        GetSongInfoL(aPath, aAttrs, aEntries, aMediaArray);
        MPX_PERF_END(CMPXDbPlugin_DoArtistMediaL_Song);
        }
    else
        {
        MPX_DEBUG2("CMPXDbPlugin_DoArtistMediaL: Invalid levels[%d]", levels);
        User::Leave(KErrNotSupported);
        }
    }

// ----------------------------------------------------------------------------
// Find the collection media for all songs category
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::DoRootCategoryMediaL (
    const TArray<TMPXAttribute>& aAttrs,
    TMPXItemId aRootCategoryId,
    TMPXGeneralCategory aCategory,
    CMPXMedia& aEntries)
    {
    MPX_FUNC("CMPXDbPlugin::DoRootCategoryMediaL");

    TInt count(aAttrs.Count());
    for (TInt i = 0; i < count; ++i)
        {
        if (aAttrs[i].ContentId() == KMPXMediaIdGeneral)
            {
            TUint att(aAttrs[i].AttributeId());
            switch (att)
                {
                case EMPXMediaGeneralId:
                    {
                    aEntries.SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, aRootCategoryId);
                    break;
                    }
                case EMPXMediaGeneralTitle:
                    {
                    aEntries.SetTextValueL(KMPXMediaGeneralTitle,
                        iMusicLibraryMenuTitles->MdcaPoint(BrowseTypeForCategory(aCategory)));
                    break;
                    }
                case EMPXMediaGeneralCount:
                    {
                    // count number of category
                    aEntries.SetTObjectValueL<TInt>(KMPXMediaGeneralCount,
                        iDbHandler->NumberOfItemsL(aCategory));
                    break;
                    }
                case EMPXMediaGeneralDuration:
                    {
                    if (aCategory == EMPXSong)
                        {
                        DoDurationL(aEntries, EMPXSong);
                        }
                    break;
                    }
                default:
                    // not supported
                    break;
                } // end switch
            } // end if
        } // end for

    aEntries.SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXGroup);
    aEntries.SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, aCategory);
    }

// ----------------------------------------------------------------------------
// Find the collection media for albums/playlists/genres/composers category
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::DoCategoryMediaL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
    TMPXGeneralCategory aCategory,
    CMPXMedia& aEntries,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbPlugin::DoCategoryMediaL");

    TInt levels(aPath.Levels());
    TInt count(aPath.Selection().Count());

    if (levels == 2) // all albums/playlists/genres/composers
        {
        MPX_PERF_START (CMPXDbPlugin_DoCategoryMediaL_All);
        DoRootCategoryMediaL(aAttrs, aPath.Id(1).iId2, aCategory, aEntries);
        MPX_PERF_END (CMPXDbPlugin_DoCategoryMediaL_All);
        }
    else if (levels == 3 && count) // multiple albums/playlists/genres/composers selected
        {
        RArray<TMPXItemId> selections;
        CleanupClosePushL(selections);
        aPath.SelectionL(selections);

        for (TInt i = 0; i < count; ++i)
            {
            CMPXMedia* media = CMPXMedia::NewL();
            CleanupStack::PushL(media);
            iDbHandler->GetCategoryL(selections[i].iId2, aCategory, aAttrs, media);
            aMediaArray.AppendL(*media);
            CleanupStack::PopAndDestroy(media);
            }

        CleanupStack::PopAndDestroy(&selections);
        }
    else if (levels == 3) // all songs in an album/playlist/genre/composer
        {
        MPX_PERF_START(CMPXDbPlugin_DoCategoryMediaL_Category);
        TMPXItemId id = aPath.Id(2);
        iDbHandler->GetCategoryL(id.iId2, aCategory, aAttrs, &aEntries);

        // Calculate duration directly with SQL
        if (MPXDbCommonUtil::AttributeExists(aAttrs, KMPXMediaGeneralDuration))
            {
            DoDurationL(aEntries, aCategory, id);
            }

        TInt nonPermisAction( aEntries.ValueTObjectL<TInt>(KMPXMediaGeneralNonPermissibleActions));
        nonPermisAction |= EMPXCache;

        aEntries.SetTObjectValueL<TMPXGeneralNonPermissibleActions>(
                    KMPXMediaGeneralNonPermissibleActions, (TMPXGeneralNonPermissibleActions) nonPermisAction );

        MPX_PERF_END(CMPXDbPlugin_DoCategoryMediaL_Category);
        }
    else if (levels == 4) // a song/songs in an album/playlist/genre/composer
        {
        MPX_PERF_START(CMPXDbPlugin_DoCategoryMediaL_Song);
        GetSongInfoL(aPath, aAttrs, aEntries, aMediaArray);
        MPX_PERF_END(CMPXDbPlugin_DoCategoryMediaL_Song);
        }
    else
        {
        MPX_DEBUG2("CMPXDbPlugin__DoMediaL__EBrowseAlbum: Invalid levels[%d]", levels);
        User::Leave(KErrNotSupported);
        }
    }

// ----------------------------------------------------------------------------
// Set all the attributes in CMPXMedia corresponding to KMPXMediaIdDrm
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::DoSetMediaDrmL(
    CMPXMedia& aMedia,
    TUint aDrmAttributes,
    const TDesC& aLocation)
    {
    MPX_FUNC("CMPXDbPlugin::DoSetMediaDrmL");

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
// Add media objects to the array with attributes from song details
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::GetSongInfoL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aEntry,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbPlugin::GetSongInfoL");

    RArray<TInt> supportedIds;
    CleanupClosePushL(supportedIds);
    MPXDbCommonUtil::FillInSupportedUIDsL(aAttrs, supportedIds);

    RArray<TMPXItemId> selections;
    CleanupClosePushL(selections);
    aPath.SelectionL(selections);

    //
    // If we are trying to locate a song from a playlist, we should read available song
    // info from Playlist table first in case the song is located on a removable
    // drive and the drive is not currently present. This is achieved by supplying
    // playlist Id to GetSongMatchingSongIdL. When playlistId is 0, we are reading song
    // info directly from Songs table. If playlistId is specified, GetSongMatchingSongIdL
    // will populate song media from Playlist table and if Songs table for the drive
    // exists, song media will be overwritten with info from Songs table.
    //
    TMPXItemId playlistId(0);
    if (aPath.Id(1) == EBrowsePlaylist)
        {
        if (aPath.Levels() < 2)
            {
            User::Leave(KErrArgument);
            }

        playlistId = aPath.Id(aPath.Levels() - 2);
        }

    TInt countSelection(aPath.Selection().Count());
    if (countSelection)
        {
        // We have a selection, iterate it
        for (TInt selectionIndex = 0; selectionIndex < countSelection; ++selectionIndex)
            {
            CMPXMedia* newEntry = CMPXMedia::NewL(supportedIds.Array());
            CleanupStack::PushL(newEntry);

            DoGetSongInfoL(aAttrs, selections[selectionIndex].iId2, playlistId.iId2, *newEntry);

            aMediaArray.AppendL(*newEntry);
            CleanupStack::PopAndDestroy(newEntry);
            }
        }
    else
        {
        // No selection, get the attributes for the one song
        DoGetSongInfoL(aAttrs, aPath.Id(aPath.Levels() - 1).iId2, playlistId.iId2, aEntry);
        }

    CleanupStack::PopAndDestroy(&selections);
    CleanupStack::PopAndDestroy(&supportedIds);
    }

// ----------------------------------------------------------------------------
// Retrieves the attributes for a media object.
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::DoGetSongInfoL(
    const TArray<TMPXAttribute>& aAttrs,
    TInt aEntryId,
    TInt aPlaylistId,
    CMPXMedia& aEntry)
    {
    MPX_FUNC("CMPXDbPlugin::DoGetSongInfoL");

    if (aPlaylistId)
        {
        iDbHandler->GetPlaylistSongL(aEntryId, aPlaylistId, aAttrs, aEntry);
        }
    else
        {
        iDbHandler->GetSongL(aEntryId, aAttrs, aEntry);
        }

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

    // Disable caching for any MediaL() returning song info.
    TInt nonPermisAction( aEntry.ValueTObjectL<TInt>(KMPXMediaGeneralNonPermissibleActions));
    nonPermisAction |= EMPXCache;

    aEntry.SetTObjectValueL<TMPXGeneralNonPermissibleActions>(
                KMPXMediaGeneralNonPermissibleActions, (TMPXGeneralNonPermissibleActions) nonPermisAction );
    }

// ----------------------------------------------------------------------------
// Find the collection details
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::DoHandleOtherMediaAttributesL(
    const TArray<TMPXAttribute>& aAttrs,
    const CMPXCollectionPath& aPath,
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbPlugin::DoHandleOtherMediaAttributesL");

    TInt count(aAttrs.Count());
    for (TInt i = 0; i < count; ++i)
        {
        if (aAttrs[i].ContentId() == KMPXMediaIdCollectionDetails)
            {
            TUint att(aAttrs[i].AttributeId());

            if (att & EMPXMediaColDetailNumberOfItems)
                {
                aMedia.SetTObjectValueL(KMPXMediaColDetailNumberOfItems,
                    iDbHandler->NumberOfItemsL(EMPXSong));
                }
            if (att & EMPXMediaColDetailDuration)
                {
                aMedia.SetTObjectValueL(KMPXMediaColDetailDuration,
                    DoDurationL(aMedia, EMPXSong));
                }
            if (att & EMPXMediaColTotalSize)
                {
                // todo
                TInt totalSize(0);
                aMedia.SetTObjectValueL(KMPXMediaColDetailTotalSize, totalSize);
                }
            if (att & EMPXMediaLastRefreshed)
                {
                TTime lastRefreshed(iDbHandler->GetLastRefreshedTimeL());
                aMedia.SetTObjectValueL(KMPXMediaColDetailLastRefreshed,
                    lastRefreshed.Int64());
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
CDesCArray* CMPXDbPlugin::DoRemoveL(
    const CMPXCollectionPath& aPath,
    CMPXMessageArray& aChangeMsgArray)
    {
    MPX_FUNC("CMPXDbPlugin::DoRemoveL");

    if (aPath.Levels() <= 1)
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

    switch (aPath.Id(1).iId2)
        {
        case EBrowseAll:
            {
            DoRemoveFromAllSongsL(aPath, selections.Array(), *fp, aChangeMsgArray);
            }
            break;

        case EBrowseArtist:
            {
            DoRemoveFromArtistsL(aPath, selections.Array(), *fp, aChangeMsgArray);
            }
            break;

        case EBrowseAlbum: // deliberate fall through
        case EBrowseGenre: // deliberate fall through
        case EBrowseComposer: // deliberate fall through
            {
            DoRemoveFromCategoriesL(aPath, selections.Array(),
                CategoryForBrowseType(static_cast<TMCBrowseType>(aPath.Id(1).iId2)), *fp, aChangeMsgArray);
            }
            break;

        case EBrowsePlaylist:
            {
            DoRemoveFromPlaylistsL(aPath, selections.Array(), *fp, aChangeMsgArray);
            }
            break;

        default:
            {
            User::Leave(KErrArgument);
            }
        } // end switch (aPath.Id(1))

    MPX_DEBUG2("CMPXDbPlugin::RemoveL itemId[%d]", aPath.Id(aPath.Levels() - 1).iId2);

    CleanupStack::PopAndDestroy( &selections );
    CleanupStack::Pop(fp);

    return fp;
    }

// ----------------------------------------------------------------------------
// Remove an item from the collection database using the given media properties
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::DoRemoveL(
    const CMPXMedia& aMedia,
    TBool aDeleteRecord)
    {
    MPX_FUNC("CMPXDbPlugin::DoRemoveL(by media)");

    // Return deleted file paths to caller
    CDesCArray* fp = new(ELeave) CDesCArrayFlat(1);
    CleanupStack::PushL(fp);

    // a list of change event messages a result of the item being removed
    CMPXMessageArray* itemChangedMessages = CMPXMediaArray::NewL();
    CleanupStack::PushL(itemChangedMessages);

    TUint32 mediaId(0);

    // Removing a container of items
    //
    if (aMedia.IsSupported(KMPXMediaArrayContents))
        {
        MPX_DEBUG1("CMPXDbPlugin::RemoveL -- Removing a container of items");
        const CMPXMediaArray* media = aMedia.Value<CMPXMediaArray>(KMPXMediaArrayContents);
        if( !media )
            {
            User::Leave(KErrNoMemory);
            }
        const TInt mediaCount(media->Count());
        for (TInt i = 0; i < mediaCount; ++i)
            {
            CMPXMedia* entry = media->AtL(i);
            if (entry->IsSupported(KMPXMediaGeneralId))
                {
                mediaId = entry->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
                }
            else if (entry->IsSupported(KMPXMediaGeneralUri))
                {
                const TDesC& uri = entry->ValueText(KMPXMediaGeneralUri);
                mediaId = iDbHandler->GetSongIdMatchingUriL(uri);
                }
            else
                {
                // Unable to process this item
                continue;
                }

            iDbHandler->RemoveSongL(mediaId, *fp, *itemChangedMessages, aDeleteRecord);
            }
        }
    // Removing an item with known item id
    //
    else if (aMedia.IsSupported(KMPXMediaGeneralId))
        {
        MPX_DEBUG1("CMPXDbPlugin::RemoveL -- Removing an item by item id");
        mediaId =  aMedia.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);

        if (MPX_ITEM_CATEGORY(mediaId) != EMPXPlaylist)
            {
            iDbHandler->RemoveSongL(mediaId, *fp, *itemChangedMessages, aDeleteRecord);
            }
        else
            {
            iDbHandler->RemovePlaylistL(mediaId, *fp, *itemChangedMessages);
            }
        }
    // Removing an item with known uri
    //
    else if (aMedia.IsSupported(KMPXMediaGeneralUri))
        {
        MPX_DEBUG1("CMPXDbPlugin::RemoveL -- Removing an item by uri");
        TPtrC uri = aMedia.ValueText( KMPXMediaGeneralUri );

        TMPXGeneralCategory category =
            aMedia.ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory);

        if (category == EMPXSong)
            {
            mediaId = iDbHandler->GetSongIdMatchingUriL(uri);
            iDbHandler->RemoveSongL(mediaId, *fp, *itemChangedMessages, aDeleteRecord);
            }
        else if (category == EMPXPlaylist)
            {
            mediaId = iDbHandler->GetPlaylistIdMatchingUriL(uri);
            iDbHandler->RemovePlaylistL(mediaId, *fp, *itemChangedMessages);
            }
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
        else if (category == EMPXAbstractAlbum )
            {
            mediaId = iDbHandler->GetAbstractAlbumIdMatchingUriL(uri);
            iDbHandler->RemoveAbstractAlbumL(mediaId, *itemChangedMessages, EFalse);
            }
#endif // ABSTRACTAUDIOALBUM_INCLUDED
        else
            {
            // otherwise unable to process this item
            }
        }
    else
        {
        MPX_DEBUG1("CMPXDbPlugin::RemoveL -- Unknown item for removal");
        User::Leave(KErrNotSupported);
        }

    iActiveTask->SetVisibleChange(CMPXDbActiveTask::EAllVisible);

    // Removing an item always invalidates all songs.
    //
    MPXDbCommonUtil::AddItemChangedMessageL(*itemChangedMessages, EBrowseAll,
                                            EMPXItemModified, EMPXCollection, KDBPluginUid);

    DoHandleChangeL(itemChangedMessages);

    CleanupStack::PopAndDestroy(itemChangedMessages);
    CleanupStack::PopAndDestroy(fp);
    }

// ----------------------------------------------------------------------------
// Remove media by path through a command
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::DoRemovePathL(
    CMPXCommand& aCmd)
    {
    MPX_FUNC("CMPXDbPlugin::DoRemovePathL(by command)");

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
        // the path ends in a category, retrieve all songs under the selected category
        // and append a new level with all songs under the selected category
        DoAppendLevelL(*path);

        CMPXCollectionPath* iterationPath = CMPXCollectionPath::NewL(*path);
        CleanupStack::PushL(iterationPath);
        iterationPath->ClearSelection();

        // indices of the selected items
        TArray<TInt> selectionIndices = path->Selection();
        TInt count(selectionIndices.Count());

        // number of media items to remove in this iteration
        TInt removeCount = (aCmd.ValueTObjectL<TInt>(KMPXCommandCollectionRemoveMediaCount));

        // remove all in one shot if removeCount is 0 or negative
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
            aCmd.ValueTObjectL<TBool>(KMPXCommandCollectionRemoveSuppressMsgs))
            {
            // Msgs are stored in the command
            supressMsgs = ETrue;

            CMPXMessageArray* msgs( aCmd.Value<CMPXMessageArray>(KMPXCommandCollectionChangeMsgs) );
            User::LeaveIfNull( msgs );
            fp = DoRemoveL(*iterationPath, *msgs);
            }
        else
            {
            // Msgs will be sent after delete
            msgAry = CMPXMessageArray::NewL();
            CleanupStack::PushL(msgAry);
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
void CMPXDbPlugin::DoRemoveMediaL(
    CMPXCommand& aCmd)
    {
    MPX_FUNC("CMPXDbPlugin::DoRemoveMediaL(by command)");

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
// Remove a media/media items from All Songs view
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::DoRemoveFromAllSongsL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXItemId>& aSelections,
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbPlugin::DoRemoveFromAllSongsL");

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
                    iDbHandler->RemoveSongL(aSelections[i].iId2, aUriArray, aItemChangedMessages);
                    } // end for
                }
            else
                {
                iDbHandler->RemoveSongL(aPath.Id(aPath.Levels() - 1).iId2, aUriArray,
                     aItemChangedMessages);
                }
            }
            break;

        default:
            {
            MPX_DEBUG2("CMPXDbPlugin_DoRemoveFromAllSongsL: Invalid levels[%d]", aPath.Levels());
            User::Leave(KErrNotSupported);
            }
        }
    }

// ----------------------------------------------------------------------------
// Remove a media/media items from Artists view
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::DoRemoveFromArtistsL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXItemId>& aSelections,
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbPlugin::DoRemoveFromArtistsL");

    TInt levels(aPath.Levels());
    if (levels == 2)
        {
        // when the collection is removed, it's intended not to delete the files
        iDbHandler->RemoveEntireCollectionL();
        }
    else
        {
        TInt count(aSelections.Count());
        if (count)
            {
            for (TInt i = 0; i < count; ++i)
                {
                RemoveFromArtistsL(aPath, aSelections[i].iId2, aUriArray, aItemChangedMessages);
                }
            }
        else
            {
            RemoveFromArtistsL(aPath, aPath.Id(levels - 1).iId2, aUriArray, aItemChangedMessages);
            }
        }

    MPX_DEBUG2("CMPXDbPlugin__RemoveL__EBrowseArtist: levels[%d]", levels);
    }

// ----------------------------------------------------------------------------
// Remove a media item from Artists view
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::RemoveFromArtistsL(
    const CMPXCollectionPath& aPath,
    TUint32 aItemId,
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbPlugin::RemoveFromArtistsL");

    switch (aPath.Levels())
        {
        case 3:
            {
            iDbHandler->RemoveSongsMatchingCategoryL(EMPXArtist, aItemId, aUriArray, aItemChangedMessages);
            break;
            }
        case 4:
            {
            // remove the songs for the artist and album
            iDbHandler->RemoveSongsMatchingArtistAndAlbumL(aPath.Id(aPath.Levels() - 2), aItemId,
                aUriArray, aItemChangedMessages);
            break;
            }
        case 5:
            {
            iDbHandler->RemoveSongL(aItemId, aUriArray, aItemChangedMessages);
            break;
            }
        default:
            {
            User::Leave(KErrArgument);
            }
        }
    }

// ----------------------------------------------------------------------------
// Remove a media/media items from Albums/Genres/Composers view
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::DoRemoveFromCategoriesL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXItemId>& aSelections,
    TMPXGeneralCategory aCategory,
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbPlugin::DoRemoveFromCategoriesL");

    TInt levels(aPath.Levels());
    if (levels == 2)
        {
        // when the collection is removed, it's intended not to delete the files
        iDbHandler->RemoveEntireCollectionL();
        }
    else
        {
        TInt count(aSelections.Count());
        if (count)
            {
            for (TInt i = 0; i < count; ++i)
                {
                RemoveFromCategoriesL(aPath, aSelections[i].iId2, aCategory,
                    aUriArray, aItemChangedMessages);
                }
            }
        else
            {
            RemoveFromCategoriesL(aPath, aPath.Id(levels - 1).iId2, aCategory,
                aUriArray, aItemChangedMessages);
            }
        }

    MPX_DEBUG2("CMPXDbPlugin__RemoveL__EBrowseAlbum: levels[%d]", levels);
    }

// ----------------------------------------------------------------------------
// Remove a media item from Albums/Genres/Composers view
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::RemoveFromCategoriesL(
    const CMPXCollectionPath& aPath,
    TUint32 aItemId,
    TMPXGeneralCategory aCategory,
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbPlugin::RemoveFromCategoriesL");

    switch (aPath.Levels())
        {
        case 3:
            {
            iDbHandler->RemoveSongsMatchingCategoryL(aCategory, aItemId, aUriArray,
                aItemChangedMessages);
            break;
            }
        case 4:
            {
            iDbHandler->RemoveSongL(aItemId, aUriArray, aItemChangedMessages);
            break;
            }
        default:
            {
            User::Leave(KErrArgument);
            }
        }
    }

// ----------------------------------------------------------------------------
// Remove one or multiple media items from Playlists view
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::DoRemoveFromPlaylistsL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXItemId>& aSelections,
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbPlugin::DoRemoveFromPlaylistsL");

    TInt levels(aPath.Levels());

    // all playlists
    if (levels == 2)
        {
        // when the collection is removed, it's intended not to delete the files
        iDbHandler->RemoveAllPlaylistsL();
        }
    else
        {
        TArray<TInt> selectionIndices = aPath.Selection();
        TInt count(selectionIndices.Count());

        // multiple selections
        if (count)
            {
            for (TInt i = (count - 1); i >= 0; --i)
                {
                RemoveFromPlaylistsL(aPath, aSelections[i], selectionIndices[i],
                    aUriArray, aItemChangedMessages);
                }
            }
        // else no selection
        else
            {
            RemoveFromPlaylistsL(aPath, aPath.IdOfIndex(aPath.Index()), aPath.Index(),
                aUriArray, aItemChangedMessages);
            }
        }

    MPX_DEBUG2("CMPXDbPlugin__RemoveL__EBrowsePlaylist: levels[%d]", levels);
    }

// ----------------------------------------------------------------------------
// Remove a media item from Playlists view
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::RemoveFromPlaylistsL(
    const CMPXCollectionPath& aPath,
    const TMPXItemId& aItemId,
    TInt aIndex,
    CDesCArray& aUriArray,
    CMPXMessageArray& aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbPlugin::RemoveFromPlaylistsL");

    switch (aPath.Levels())
        {
        case 3:
            {
            iDbHandler->RemovePlaylistL(aItemId.iId2, aUriArray, aItemChangedMessages);
            break;
            }
        case 4:
            {
            if ( !iDbHandler->InTransaction() )
                {
                iDbHandler->BeginTransactionL();
                }
            iDbHandler->RemoveSongFromPlaylistL(aPath.Id(aPath.Levels() - 2).iId2, aItemId,
                aIndex, aItemChangedMessages);
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
void CMPXDbPlugin::DoRetrieveUriForDeletionL(
    CMPXCommand& aCmd)
    {
    MPX_FUNC("CMPXDbPlugin::DoRetrieveUriForDeletionL");

    // initialize mandatory return parameters
    aCmd.SetTObjectValueL<TInt>(KMPXCommandCollectionRetrieveUriError, KErrNone);

    if (!aCmd.IsSupported(KMPXCommandCollectionRetrievePath))
        {
        aCmd.SetTObjectValueL<TInt>(KMPXCommandCollectionRetrieveUriError, KErrArgument);
        }
    else
        {
        CMPXCollectionPath* path = aCmd.ValueCObjectL<CMPXCollectionPath>(KMPXCommandCollectionRetrievePath);
        CleanupStack::PushL(path);
        if (iFirstDeleteStep )
            {
            iSelections.Reset( );
        // in order to support cancel delete for a category, we need to adjust path. If
        // the path ends in a category, retrieve all songs under the selected category
        // and append a new level with all songs under the selected category
        DoAppendLevelL(*path);

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

        TInt count(iSelections.Count());
        TInt itemCount = count > KIncrementalDeleteCount ? KIncrementalDeleteCount : count;
        for (TInt i = 0; i < itemCount; ++i )
            {
            // do not report song URIs if collection path is for songs within a playlist, i.e.
            // EBrowsePlaylist and level 4, because client should not be deleting those song
            // files
            if (path->Id(1) != EBrowsePlaylist || path->Levels() !=4)
                {
                HBufC * uri = iDbHandler->GetUriMatchingIdL (iSelections[0].iId2 );
                CleanupStack::PushL(uri);
                fp->AppendL(*uri);
                CleanupStack::PopAndDestroy(uri);
                }
            iSelections.Remove(0);
            }

        aCmd.SetNoNewLCObjectL(KMPXCommandCollectionRetrieveMediaUriArray, fp);
        if (iFirstDeleteStep)
            {
        aCmd.SetCObjectValueL(KMPXCommandCollectionRetrievePath, path);
            }

        CleanupStack::PopAndDestroy(fp);
        CleanupStack::PopAndDestroy(path);
        }
    }

// ----------------------------------------------------------------------------
// Cleanup deleted records
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::CleanupDeletedRecordsL(
    CMPXCommand& aCmd)
    {
    MPX_FUNC("CMPXDbPlugin::CleanupDeletedRecordsL");

    MPX_TRAPD(error, iDbHandler->CleanupDeletedRecordsL());
    aCmd.SetTObjectValueL<TInt>(KMPXCommandCollectionCleanupError, error);
    }

// ----------------------------------------------------------------------------
// Find the duration
// ----------------------------------------------------------------------------
//
TInt CMPXDbPlugin::DoDurationL(
    CMPXMedia& aMedia,
    TMPXGeneralCategory aFirstCat,
    TMPXItemId aId,
    TMPXGeneralCategory aSecondCat,
    TMPXItemId aSubId)
    {
    MPX_FUNC("CMPXDbPlugin::DoDurationL");

    TInt duration(0);

    switch (aFirstCat)
        {
        case EMPXSong:
            {
            duration = iDbHandler->GetAllSongsDurationL();
            break;
            }
        case EMPXAlbum:
            {
            duration = iDbHandler->GetAlbumDurationL(aId.iId2);
            break;
            }
        case EMPXComposer:
            {
            duration = iDbHandler->GetComposerDurationL(aId.iId2);
            break;
            }
        case EMPXGenre:
            {
            duration = iDbHandler->GetGenreDurationL(aId.iId2);
            break;
            }
        case EMPXArtist:
            {
            if (aSecondCat == EMPXAlbum)
                {
                duration = iDbHandler->GetArtistAlbumDurationL(aId.iId2, aSubId.iId2);
                }
            else
                {
                duration = iDbHandler->GetArtistDurationL(aId.iId2);
                }
            break;
            }
        case EMPXPlaylist:
            {
            duration = iDbHandler->GetPlaylistDurationL(aId.iId2);
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
// Append a level to a collection path and set selection to the first item
// ----------------------------------------------------------------------------
//
TInt CMPXDbPlugin::DoAppendLevelL(
    CMPXCollectionPath& aPath,
    CMPXMedia& aMedia )
    {
    MPX_FUNC("CMPXDbPlugin::DoAppendLevelL");

    RArray<TMPXItemId> ids;
    CleanupClosePushL(ids);

    // Extract media array, and get all item ids
    //
    const CMPXMediaArray* mediaArray = aMedia.Value<CMPXMediaArray>(KMPXMediaArrayContents);
    if( !mediaArray )
        {
        User::Leave( KErrNoMemory );
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
// Append a level to a collection path and set selection to all songs under the
// selected category/categories
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::DoAppendLevelL(
    CMPXCollectionPath& aPath)
    {
    MPX_FUNC("CMPXDbPlugin::DoAppendLevelL");

    TMPXItemId contextId(aPath.Id(1));
    TInt levels(aPath.Levels());

    if ((contextId == EBrowseAll) ||
        (contextId == EBrowsePlaylist))
        {
        return;
        }
    else if (levels == 3 || (levels == 4 && contextId == EBrowseArtist))
        {
        // retrieve songs in the selected category
        //
        CMPXMediaArray* songs = CMPXMediaArray::NewL();
        CleanupStack::PushL(songs);

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

        // all songs for the selected artist
        if (contextId == EBrowseArtist && levels == 3)
            {
            for (TInt i = 0; i < count; ++i)
                {
                iDbHandler->GetSongsMatchingArtistL(selections[i].iId2, attributes.Array(), songs);
                }
            if (songs->Count())
                {
                aPath.AppendL(selections.Array());
                }
            }

        // all songs for the selected artist in the specified album
        else if ((contextId == EBrowseArtist) && (levels == 4))
            {
            for (TInt i = 0; i < count; ++i)
                {
                if (aPath.Id(2) == aPath.Id(3))
                    {
                    iDbHandler->GetSongsMatchingArtistL(aPath.Id(3).iId2,
                        attributes.Array(), songs);
                    }
                else
                    {
                    iDbHandler->GetSongsMatchingArtistAndAlbumL(aPath.Id(aPath.Levels() - 2),
                        selections[i].iId2, attributes.Array(), songs);
                    }
                }
            }

         // all songs for the selected album
        else if (contextId == EBrowseAlbum && levels == 3)
            {
            for (TInt i = 0; i < count; ++i)
                {
                iDbHandler->GetSongsMatchingAlbumL(selections[i], attributes.Array(), songs);
                }
            }

        // all songs for the selected genre
        else if (contextId == EBrowseGenre && levels == 3)
            {
            for (TInt i = 0; i < count; ++i)
                {
                iDbHandler->GetSongsMatchingGenreL(selections[i], attributes.Array(), songs);
                }
            }

        // all songs for the selected composer
        else if (contextId == EBrowseComposer && levels == 3)
            {
            for (TInt i = 0; i < count; ++i)
                {
                iDbHandler->GetSongsMatchingComposerL(selections[i], attributes.Array(), songs);
                }
            }
        else
            {
            // else do nothing
            }

        CleanupStack::PopAndDestroy(&selections);
        CleanupStack::PopAndDestroy(&attributes);

        // transform from CMPXMediaArray to RArray
        RArray<TMPXItemId> songIds;
        CleanupClosePushL(songIds);

        TInt songCount(songs->Count());
        for (TInt i = 0; i < songCount; ++i)
            {
            CMPXMedia* song = (*songs)[i];

            if (song->IsSupported(KMPXMediaGeneralId))
                {
                songIds.AppendL(song->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId));
                }
            }

        // modify the collection path. append another level with all songs under the selected
        // category/categories selected
        songCount = songIds.Count();

        if (songCount)
            {
            aPath.ClearSelection();
            aPath.AppendL(songIds.Array());

            // select all
            for (TInt i = 0; i < songCount; ++i)
                {
                aPath.SelectL(songIds[i]);
                }
            }

        CleanupStack::PopAndDestroy(&songIds);
        CleanupStack::PopAndDestroy(songs);
        }
    else
        {
        // do nothing
        }
    }

// ----------------------------------------------------------------------------
// Execute an Add task step
// ----------------------------------------------------------------------------
//
TBool CMPXDbPlugin::DoAddAsyncL()
    {
    MPX_FUNC("CMPXDbPlugin::DoAddAsyncL");

    TBool done(EFalse);
    const CMPXMedia* task = (iActiveTask->GetCommand().Value<CMPXMedia>(KMPXCommandColAddMedia));
    if( !task )
        {
        User::Leave(KErrNoMemory);
        }

    CMPXMessageArray& msgArray = iActiveTask->GetChangeMessages();

    if (!task->IsSupported(KMPXMediaGeneralType))
        {
        User::Leave(KErrArgument);
        }

    // Group of items or a single item
    //
    if (task->ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType) == EMPXGroup)
        {
        if (!task->IsSupported(KMPXMediaArrayContents))
            {
            User::Leave(KErrArgument);
            }

        CMPXMediaArray* ary = task->Value<CMPXMediaArray>(KMPXMediaArrayContents);
        User::LeaveIfNull(ary);
        TInt step( iActiveTask->GetStep() );
        DoAddItemL( *ary->AtL(step), msgArray );

        if (++step == ary->Count())
            {
            done = ETrue;
            }
        }
    else // type == EMPXItem
        {
        TUint32 item = DoAddItemL( *task, msgArray );
        iActiveTask->GetCommand().SetTObjectValueL<TMPXItemId>( KMPXCommandColAddRtnId, item);
        done = ETrue;
        }

    iActiveTask->SetVisibleChange(CMPXDbActiveTask::EAllVisible);
    return done;
    }

// ----------------------------------------------------------------------------
// Add an item to the collection
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbPlugin::DoAddL(
    const CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbPlugin::DoAddL");

    if (!aMedia.IsSupported(KMPXMediaGeneralType))
        {
        User::Leave(KErrArgument);
        }

    TUint32 itemId(0);
    CMPXMessageArray* changeMsgAry = CMPXMessageArray::NewL();
    CleanupStack::PushL(changeMsgAry);

    // start a transaction here
    if (!iDbHandler->InTransaction())
        {
        iDbHandler->BeginTransactionL();
        }

    // Group of items
    if (aMedia.ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType) == EMPXGroup)
        {
        CMPXMediaArray* array = aMedia.Value<CMPXMediaArray>(KMPXMediaArrayContents);
        User::LeaveIfNull( array );

        TInt count(array->Count());
        for (TInt i = 0; i < count; ++i)
            {
            // ETrue indicates we are batch committing the songs.
            // This parameter is only used for the use case of adding
            // thousands of songs at a time.
            DoAddItemL(*array->AtL(i), *changeMsgAry, ETrue);
            }
        }
    else // single item
        {
        itemId = DoAddItemL(aMedia, *changeMsgAry);
        }

    // end transaction here.
    iDbHandler->EndTransactionL(KErrNone);

    iActiveTask->SetVisibleChange(CMPXDbActiveTask::EAllVisible);
    DoHandleChangeL(changeMsgAry);
    CleanupStack::PopAndDestroy(changeMsgAry);

    return itemId;
    }

// ----------------------------------------------------------------------------------------------------------
// Add an item to the collection
// ----------------------------------------------------------------------------------------------------------
//
TUint32 CMPXDbPlugin::DoAddItemL(
    const CMPXMedia& aMedia,
    CMPXMessageArray& aMessageArray,
    TBool aBatchCommit)
    {
    MPX_FUNC("CMPXDbPlugin::DoAddItemL");

    TUint32 itemId(0);
    if (!aMedia.IsSupported(KMPXMediaGeneralCategory))
        {
        User::Leave(KErrArgument);
        }

    switch (aMedia.ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory))
        {
        case EMPXPlaylist:
            {
            if (!aMedia.IsSupported(KMPXMediaArrayContents))
                {
                User::Leave(KErrArgument);
                }

            if (aMedia.IsSupported(KMPXMediaGeneralId))
                {
                itemId = iDbHandler->AddSongToPlaylistL(aMedia);
                MPXDbCommonUtil::AddItemChangedMessageL(aMessageArray, itemId, EMPXItemModified,
                    EMPXPlaylist, KDBPluginUid);
                }
            else if (aMedia.IsSupported(KMPXMediaGeneralUri))
                {
                itemId = iDbHandler->AddPlaylistL(aMedia);
                MPXDbCommonUtil::AddItemChangedMessageL(aMessageArray, EBrowsePlaylist, EMPXItemInserted,
                    EMPXPlaylist, KDBPluginUid);
                }
            else
                {
                User::Leave(KErrArgument);
                }
            }
            break;
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
       case EMPXAbstractAlbum:
            {
            if (aMedia.IsSupported(KMPXMediaGeneralUri))
                {
                //add abstractalbum to AbstractAlbum table
                itemId = iDbHandler->AddAbstractAlbumL(aMedia, &aMessageArray);

                //in case aMedia as mediaArray which contains songs as arrayContents, need to update all songs associated
                if ( aMedia.IsSupported(KMPXMediaArrayContents))
                    {
                    iDbHandler->UpdateSongsAbstractAlbumInfoL(aMedia, aMessageArray);
                    }
                }
            else
                {
                User::Leave(KErrArgument);
                }
            }
            break;
#endif // ABSTRACTAUDIOALBUM_INCLUDED
        case EMPXSong:
            {
            // For the use case of adding thousands of songs at once,
            // we do not create a new database transaction for each song;
            // Instead DoAddL() will batch 100 songs under a single transaction.
            // This enhancement improves performance with MMC-based databases.
            if (aBatchCommit)
                {
                itemId = iDbHandler->AddSongWithNoTransactionL(aMedia, &aMessageArray);
                }
            else
                {
                itemId = iDbHandler->AddSongL(aMedia, &aMessageArray);
                }

            MPXDbCommonUtil::AddItemChangedMessageL(aMessageArray, itemId, EMPXItemInserted,
                EMPXSong, KDBPluginUid);
            }
            break;

        default:
            {
            User::Leave(KErrNotSupported);
            }
        }

    return itemId;
    }

// ----------------------------------------------------------------------------
// Update the collection from a media
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::DoSetL(
    const CMPXMedia& aMedia )
    {
    MPX_FUNC("CMPXDbPlugin::DoSetL");

    if (!aMedia.IsSupported(KMPXMediaGeneralType))
        {
        User::Leave(KErrArgument);
        }

    CMPXDbActiveTask::TChangeVisibility visibleChange(CMPXDbActiveTask::ENotVisibile);

    CMPXMessageArray* changeMsgArray = CMPXMessageArray::NewL();
    CleanupStack::PushL(changeMsgArray);

    if (aMedia.ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType) == EMPXGroup)
        {
        if (!aMedia.IsSupported(KMPXMediaArrayContents))
            {
            User::Leave(KErrArgument);
            }

        CMPXMediaArray* array = aMedia.Value<CMPXMediaArray>(KMPXMediaArrayContents);
        User::LeaveIfNull( array );
        TInt count( array->Count() );
        for (TInt i = 0; i < count; ++i)
            {
            visibleChange = (CMPXDbActiveTask::TChangeVisibility)(visibleChange | DoSetItemL(*array->AtL(i), *changeMsgArray));
            }
        }
    else
        {
        visibleChange = DoSetItemL(aMedia, *changeMsgArray);
        }

    // Handle Change Events
    if (visibleChange)
        {
        iActiveTask->SetVisibleChange(visibleChange);
        DoHandleChangeL(changeMsgArray);
        }
    CleanupStack::PopAndDestroy(changeMsgArray);
    }

// ----------------------------------------------------------------------------
// Execute a task step for async set
// ----------------------------------------------------------------------------
//
TBool CMPXDbPlugin::DoSetAsyncL()
    {
    MPX_FUNC("CMPXDbPlugin::DoSetAsyncL");

    TBool done(EFalse);
    CMPXMedia* task = iActiveTask->GetCommand().Value<CMPXMedia>(KMPXCommandColSetMedia);
    User::LeaveIfNull( task );

    CMPXMessageArray& msgArray = iActiveTask->GetChangeMessages();
    CMPXDbActiveTask::TChangeVisibility visibleChange(iActiveTask->GetVisibleChange());

    // Multiple steps can be in a transaction for faster response
    if( !iDbHandler->InTransaction() )
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
        visibleChange = (CMPXDbActiveTask::TChangeVisibility)(visibleChange | DoSetItemL(*array->AtL(step), msgArray));

        if (++step == array->Count())
            {
            done = ETrue;
            }
        }
    else // Single item
        {
        visibleChange = DoSetItemL(*task, msgArray);
        done = ETrue;
        }
    iActiveTask->SetVisibleChange(visibleChange);
    return done;
    }

// ----------------------------------------------------------------------------
// Update the collection from a media
// ----------------------------------------------------------------------------
//
CMPXDbActiveTask::TChangeVisibility CMPXDbPlugin::DoSetItemL(
    const CMPXMedia& aMedia,
    CMPXMessageArray& aMessageArray )
    {
    MPX_FUNC("CMPXDbPlugin::DoSetItemL");

    TMPXGeneralCategory category = aMedia.ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory);

    CMPXDbActiveTask::TChangeVisibility visibleChange(CMPXDbActiveTask::ENotVisibile);
    switch (category)
        {
        case EMPXPlaylist:
            {
            if (aMedia.IsSupported(KMPXMediaArrayContents))
                {
                CMPXMessage* message = CMPXMedia::NewL();
                CleanupStack::PushL(message);

                iDbHandler->UpdatePlaylistSongsL(aMedia, *message);

                aMessageArray.AppendL(message); // ownership xfer
                CleanupStack::Pop(message);
                }
            else
                {
                iDbHandler->UpdatePlaylistL(aMedia, aMessageArray);
                }

            visibleChange = CMPXDbActiveTask::ESingleVisible;
            }
            break;
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
        case EMPXAbstractAlbum:
            {
            //update all songes which associate with ABSTRACTALBUM
            if (aMedia.IsSupported(KMPXMediaGeneralUri))
                {
                //in case aMedia as mediaArray which contains songs as arrayContents, need to update all songs associated
                if ( aMedia.IsSupported(KMPXMediaArrayContents))
                    {
                    iDbHandler->UpdateSongsAbstractAlbumInfoL(aMedia, aMessageArray);
                    }
                //only update field values in abstractalbum table, or renaming (change uri) for abstractalbum table
                else
                    {
                    visibleChange = iDbHandler->UpdateAbstractAlbumL(aMedia, aMessageArray);
                    }
                }
            }
            break;
#endif // ABSTRACTAUDIOALBUM_INCLUDED
        case EMPXSong:
            {
            // a list of changed messages as a result of the song being updated
            visibleChange = iDbHandler->UpdateSongL(aMedia, aMessageArray);
            }
            break;

        default:
            {
            User::Leave(KErrNotSupported);
            }
            break;
        }
    return visibleChange; // ownership xfer
    }

// ----------------------------------------------------------------------------
// Sets the drm properties for a list of medias
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::DoSetDrmForArrayL(
    const CMPXMediaArray& mediaArray,
    const TArray<TMPXAttribute>& aAttrs)
    {
    MPX_FUNC("CMPXDbPlugin::DoSetDrmForArrayL");

    TUint drmAttributes(0);

    // Compact the attribute set
    //
    TInt attrCount(aAttrs.Count());
    for (TInt i = 0; i < attrCount; ++i)
        {
        if (aAttrs[i].ContentId() == KMPXMediaIdDrm)
            {
            drmAttributes |= aAttrs[i].AttributeId();
            }
        }

    // Fetch drm attributes for every item
    //
    if (drmAttributes)
        {
        TInt count(mediaArray.Count());
        for (TInt i = 0; i < count; ++i)
            {
            if (mediaArray[i]->IsSupported(KMPXMediaGeneralUri))
                {
                DoSetMediaDrmL(*mediaArray[i], drmAttributes,
                    mediaArray[i]->ValueText(KMPXMediaGeneralUri));
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// Handle change events
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::HandleChangeL(
    const CMPXMessage& aMessage)
    {
    MPX_FUNC("CMPXDbPlugin::HandleChange");

    // check if message is filled
    if (aMessage.IsSupported(KMPXMessageGeneralId))
        {
#if _DEBUG
        PrintMessagesL(aMessage);
#endif // _DEBUG
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
                    if(changeType == EMPXItemInserted && (cat == EMPXSong || cat == EMPXPlaylist || cat == EMPXPodcast))
                        {
                        iObs->HandleMessage(aMessage);
                        }
                    }
                }
            else
                {
                TMPXChangeEventType changeType( aMessage.ValueTObjectL<TMPXChangeEventType>( KMPXMessageChangeEventType ) );
                TMPXGeneralCategory cat(aMessage.ValueTObjectL<TMPXGeneralCategory>(KMPXMessageMediaGeneralCategory));
                if(changeType == EMPXItemInserted && (cat == EMPXSong || cat == EMPXPlaylist || cat == EMPXPodcast))
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
void CMPXDbPlugin::DoHandleChangeL(
    CMPXMessageArray* aItemChangedMessages,
    TMPXCommandId aCommandId )
    {
    MPX_FUNC("CMPXDbPlugin::DoHandleChangeL");

    if( (iActiveTask->GetVisibleChange() & CMPXDbActiveTask::EAllVisible)
        && (aCommandId == KMPXCommandIdCollectionSet ||
            aCommandId == KMPXCommandIdCollectionAdd ||
            aCommandId == KMPXCommandIdCollectionRemove ||
            aCommandId == KMPXCommandIdCollectionCompleteDelete ))
        {
        MPXDbCommonUtil::AddItemChangedMessageL(*aItemChangedMessages, EBrowseAll,
                                                 EMPXItemModified, EMPXCollection, KDBPluginUid);
        }
    // group change messages and send to collection client context
    //
    CMPXMessage* message = CMPXMessage::NewL();
    CleanupStack::PushL(message);

    message->SetTObjectValueL<TMPXMessageId>(KMPXMessageGeneralId, KMPXMessageIdItemChanged);
    message->SetCObjectValueL(KMPXMessageArrayContents, aItemChangedMessages);
    message->SetTObjectValueL<TInt>(KMPXMessageArrayCount, aItemChangedMessages->Count());

    HandleChangeL(*message);

    CleanupStack::PopAndDestroy(message);
    }

// ----------------------------------------------------------------------------
// Handle completion of operation
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::DoHandleOperationCompletedL(
    TInt aErr)
    {
    MPX_FUNC("CMPXDbPlugin::DoHandleOperationCompletedL");

    // Broadcase change messages
    //
    if (iActiveTask->GetVisibleChange())
        {
        DoHandleChangeL(&iActiveTask->GetChangeMessages(), iActiveTask->GetTask() );
        }

    // Callback to engine to signal completion
    // NOTE: Collection server immediately completes the async message when
    // Cancel is called, no need to callback to observer
    if (aErr != KErrCancel)
        {
        if( iActiveTask->GetTask() == KMPXCommandIdCollectionAdd )
            {
            iObs->HandleCommandComplete( &iActiveTask->GetCommand(), aErr );
            }
        else
            {
            iObs->HandleCommandComplete(NULL, aErr);
            }
        }

    if( iDbHandler->InTransaction() )
        {
        // Commit if cancelled
        TInt err(aErr);
        if( err == KErrCancel )
            {
            err = KErrNone;
            }
        iDbHandler->EndTransactionL( err );
        }
    }

// ----------------------------------------------------------------------------------------------------------
// Complete a delete operation
// ----------------------------------------------------------------------------------------------------------
//
void CMPXDbPlugin::DoHandleDeleteCompleteL(
    CMPXCommand& aCmd)
    {
    MPX_FUNC("CMPXDbPlugin::DoHandleDeleteCompleteL");
    iFirstDeleteStep = ETrue;
    iSelections.Reset();
    if ( iDbHandler->InTransaction() )
        {
        // if it can reach this point in a transaction, there's no error
        iDbHandler->EndTransactionL( KErrNone );
        }

    // Change messages
    if (aCmd.IsSupported(KMPXCommandCollectionDeleteMsgArray))
        {
        CMPXMessageArray* msgs = aCmd.Value<CMPXMessageArray>(KMPXCommandCollectionDeleteMsgArray);
        User::LeaveIfNull( msgs );
        iActiveTask->SetVisibleChange(CMPXDbActiveTask::EAllVisible);
        DoHandleChangeL(msgs, KMPXCommandIdCollectionCompleteDelete);
        }
    }

// ----------------------------------------------------------------------------------------------------------
// Reorder a song in a playlist
// ----------------------------------------------------------------------------------------------------------
//
void CMPXDbPlugin::DoReorderPlaylistL(
    const CMPXCommand& aCmd)
    {
    MPX_FUNC("CMPXDbPlugin::DoReorderPlaylistL");

    if (!aCmd.IsSupported(KMPXCommandReorderPlaylistId) ||
        !aCmd.IsSupported(KMPXCommandReorderSongId) ||
        !aCmd.IsSupported(KMPXCommandReorderOriginalOrdinal) ||
        !aCmd.IsSupported(KMPXCommandReorderNewOrdinal))
        {
        User::Leave(KErrArgument);
        }

    CMPXMessage* message = CMPXMedia::NewL();
    CleanupStack::PushL(message);

    iDbHandler->ReorderPlaylistL(
        aCmd.ValueTObjectL<TMPXItemId>(KMPXCommandReorderPlaylistId),
        aCmd.ValueTObjectL<TMPXItemId>(KMPXCommandReorderSongId),
        aCmd.ValueTObjectL<TUint>(KMPXCommandReorderOriginalOrdinal),
        aCmd.ValueTObjectL<TUint>(KMPXCommandReorderNewOrdinal),
        *message);

    HandleChangeL(*message);

    CleanupStack::PopAndDestroy(message);
    }

// ----------------------------------------------------------------------------------------------------------
// Get total songs and playlists count for a database
// ----------------------------------------------------------------------------------------------------------
//
void CMPXDbPlugin::DoGetCollectionCountL( const CMPXCommand& aCmd )
    {
    MPX_FUNC("CMPXDbPlugin::DoGetCollectionCountL");
    if (!aCmd.IsSupported(KMPXCommandCollectionCountDrive) ||
        !aCmd.IsSupported(KMPXCommandCollectionCountTable) )
        {
        User::Leave(KErrArgument);
        }

    TInt count = 0;
    TInt drive = aCmd.ValueTObjectL<TInt>(KMPXCommandCollectionCountDrive);
    TInt table = aCmd.ValueTObjectL<TInt>(KMPXCommandCollectionCountTable);
    switch(table)
        {
        case EMPXCollectionCountTrack:
            count = (TInt)iDbHandler->GetMusicCountL(drive);
            break;
        case EMPXCollectionCountPlaylist:
            count = (TInt)iDbHandler->GetPlaylistCountL(drive);
            break;
        case EMPXCollectionCountTotal:
            count = (TInt)iDbHandler->GetTotalCountL(drive);
            break;
        default:
            User::Leave(KErrArgument);
        }
    ((CMPXMedia&)aCmd).SetTObjectValueL<TInt>(KMPXCommandCollectionCountValue, count);
    }

// ----------------------------------------------------------------------------------------------------------
// Get URIs for all songs and file playlists in a database
// ----------------------------------------------------------------------------------------------------------
//
void CMPXDbPlugin::DoGetCollectionUriL( const CMPXCommand& aCmd )
    {
    MPX_FUNC("CMPXDbPlugin::DoGetCollectionCountL");
    if (!aCmd.IsSupported(KMPXCommandCollectionURIDrive) ||
        !aCmd.IsSupported(KMPXCommandCollectionURITable) ||
        !aCmd.IsSupported(KMPXCommandCollectionURIFromID) ||
        !aCmd.IsSupported(KMPXCommandCollectionURIRecords) )
        {
        User::Leave(KErrArgument);
        }

    TInt drive = aCmd.ValueTObjectL<TInt>(KMPXCommandCollectionURIDrive);
    TInt table = aCmd.ValueTObjectL<TInt>(KMPXCommandCollectionURITable);
    TInt fromID = aCmd.ValueTObjectL<TInt>(KMPXCommandCollectionURIFromID);
    TInt recnum = aCmd.ValueTObjectL<TInt>(KMPXCommandCollectionURIRecords);

    CDesCArray* uris = new(ELeave) CDesCArrayFlat(4);
    CleanupStack::PushL(uris);
    TInt lastID = 0;

    switch(table)
        {
        case EMPXCollectionURITrack:
            iDbHandler->GetMusicUriArrayL(drive, fromID, recnum, *uris, lastID);
            break;
        case EMPXCollectionURIPlaylist:
            iDbHandler->GetPlaylistUriArrayL(drive, fromID, recnum, *uris, lastID);
            break;
        default:
            User::Leave(KErrArgument);
        }

    ((CMPXMedia&)aCmd).SetNoNewLCObjectL(KMPXCommandCollectionURIList, uris);
    ((CMPXMedia&)aCmd).SetTObjectValueL(KMPXCommandCollectionURILastID, lastID);
    CleanupStack::PopAndDestroy(uris);
    }


// ----------------------------------------------------------------------------------------------------------
// Perform one step of the incremental operation
// ----------------------------------------------------------------------------------------------------------
//
void CMPXDbPlugin::DoIncrementalOpenL( const CMPXCommand& aCmd )
    {
    MPX_DEBUG1("CMPXDbPlugin::DoIncrementalOpenL <--");

    TInt offset = aCmd.ValueTObjectL<TInt>( KMPXCollectionCommandIdIncOpenLOffset );
    TInt numItems = aCmd.ValueTObjectL<TInt>( KMPXCollectionCommandIdIncOpenLNumItems );

    CMPXCollectionPath* path =  aCmd.ValueCObjectL<CMPXCollectionPath>(KMPXCollectionCommandIdIncOpenLPath);
    CleanupStack::PushL( path );
    MPX_DEBUG_PATH( *path );

    // Switch on level and item selected
    //
    TInt levels( path->Levels() );
    switch( levels )
        {
        case 3:  // levels of 3 top level is not stripped
            {
            switch( path->Id(1).iId2 )
                {
                case EBrowseAll:
                    {
                    CMPXMedia* results = CMPXMedia::NewL();
                    CleanupStack::PushL( results );

                    TMPXOpenDataBlock block;
                    block.iOffset = offset;
                    block.iSize = numItems;

                    // Todo: this should come from the UI
                    RArray<TMPXAttribute> attrs;
                    CleanupClosePushL( attrs );
                    attrs.AppendL(TMPXAttribute(KMPXMediaIdGeneral,
                                   EMPXMediaGeneralId | EMPXMediaGeneralType | EMPXMediaGeneralCategory |
                                   EMPXMediaGeneralTitle | EMPXMediaGeneralFlags | EMPXMediaGeneralUri));
                    attrs.AppendL( TMPXAttribute(KMPXMediaIdMusic,
                                        EMPXMediaMusicArtist | EMPXMediaMusicAlbumArtFileName ) );

                    // Array to read data from
                    CMPXMediaArray* array = CMPXMediaArray::NewL();
                    CleanupStack::PushL( array );
                    iDbHandler->GetSongsAtOffsetL( array, attrs.Array(), offset, numItems );
                    TInt max( path->Count() );
                    TInt count(0);
                    TInt aryCount( array->Count() );
                    // Update the collection path
                    while( count<numItems && offset<max &&
                           count<aryCount )
                        {
                        TMPXItemId id = array->AtL(count)->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
                        path->Update( offset, id );

                        // Next items
                        offset++;
                        count++;
                        }

                    // Setup the results
                    //
                    results->SetCObjectValueL(KMPXMediaArrayContents, array);
                    results->SetTObjectValueL<TInt>(KMPXMediaArrayCount, array->Count());
                    CleanupStack::PopAndDestroy( array );
                    CleanupStack::PopAndDestroy( &attrs );

                    // Callback with results
                    //
                    results->SetTObjectValueL<TMPXOpenDataBlock>( KMPXCollectionOpenLResultRange, block );
                    iObs->HandleOpen( results, path, KErrNone );
                    CleanupStack::PopAndDestroy( results );
                    break;
                    }
                default:
                    User::Leave(KErrNotSupported);
                    break;
                }
            break;
            }
        default:
            {
            User::Leave(KErrNotSupported);
            break;
            }

        }
    CleanupStack::PopAndDestroy( path );

    MPX_DEBUG1("CMPXDbPlugin::DoIncrementalOpenL -->");
    }

// ----------------------------------------------------------------------------
// Maps a given browse type to a category ID.
// ----------------------------------------------------------------------------
//
TMPXGeneralCategory CMPXDbPlugin::CategoryForBrowseType(
    TMCBrowseType aBrowseType)
    {
    MPX_FUNC("CMPXDbPlugin::CategoryForBrowseType");

    TMPXGeneralCategory cat(EMPXNoCategory);

    switch (aBrowseType)
        {
        case EBrowseAll:
            {
            cat = EMPXSong;
            break;
            }
        case EBrowseArtist:
            {
            cat = EMPXArtist;
            break;
            }
        case EBrowseAlbum:
            {
            cat = EMPXAlbum;
            break;
            }
        case EBrowsePlaylist:
            {
            cat = EMPXPlaylist;
            break;
            }
        case EBrowseGenre:
            {
            cat = EMPXGenre;
            break;
            }
        case EBrowseComposer:
            {
            cat = EMPXComposer;
            break;
            }
#ifdef __ENABLE_PODCAST_IN_MUSIC_MENU
        case EBrowsePodcasts:
            {
            cat = EMPXPodcast;
            break;
            }
#endif
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
TMCBrowseType CMPXDbPlugin::BrowseTypeForCategory(
    TMPXGeneralCategory aCategory)
    {
    MPX_FUNC("CMPXDbPlugin::BrowseTypeForCategory");

    TMCBrowseType browseType(EBrowseComposer);

    switch (aCategory)
        {
        case EMPXSong:
            {
            browseType = EBrowseAll;
            break;
            }
        case EMPXArtist:
            {
            browseType = EBrowseArtist;
            break;
            }
        case EMPXAlbum:
            {
            browseType = EBrowseAlbum;
            break;
            }
        case EMPXPlaylist:
            {
            browseType = EBrowsePlaylist;
            break;
            }
        case EMPXGenre:
            {
            browseType = EBrowseGenre;
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
void CMPXDbPlugin::SetMediaGeneralAttributesL(
    CMPXMedia& aMedia,
    TMPXGeneralType aType,
    TMPXGeneralCategory aCategory,
    const TDesC& aTitle)
    {
    aMedia.SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, aType);
    aMedia.SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, aCategory);
    aMedia.SetTextValueL(KMPXMediaGeneralTitle, aTitle);
    }

// ----------------------------------------------------------------------------
// Sets the type, category and title attributes in the specified media instance
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::SetMediaGeneralAttributesL(
    CMPXMedia& aMedia,
    TMPXGeneralType aType,
    TMPXGeneralCategory aCategory,
    TInt aId)
    {
    MPX_FUNC("CMPXDbPlugin::SetMediaGeneralAttributesL");

    HBufC* title = iDbHandler->GetNameMatchingIdL(aId);
    CleanupStack::PushL(title);
    SetMediaGeneralAttributesL(aMedia, aType, aCategory, *title);
    CleanupStack::PopAndDestroy(title);
    }

// ----------------------------------------------------------------------------
// Set the attribute list according to current path
// ----------------------------------------------------------------------------
//
void CMPXDbPlugin::SetAttributesL(
    const CMPXCollectionPath& aPath,
    RArray<TMPXAttribute>& aAttrs,
    RArray<TInt>& aSupportedIds )
    {
    CleanupClosePushL(aAttrs);
    CleanupClosePushL(aSupportedIds);
       
    aAttrs.AppendL( TMPXAttribute(KMPXMediaIdGeneral,
        EMPXMediaGeneralId | EMPXMediaGeneralType | EMPXMediaGeneralCategory |
        EMPXMediaGeneralTitle | EMPXMediaGeneralFlags | EMPXMediaGeneralUri) );

    aSupportedIds.AppendL(KMPXMediaIdContainer);
    aSupportedIds.AppendL(KMPXMediaIdGeneral);

    TInt levels(aPath.Levels());
    if ( 2 == levels )
        {
        // check the browse type
        switch ( aPath.Id(1).iId2 )
            {
            case EBrowseAll:
                {
                aAttrs.AppendL( TMPXAttribute(KMPXMediaIdMusic,
                    EMPXMediaMusicArtist | EMPXMediaMusicAlbumArtFileName ) );
                aSupportedIds.AppendL( KMPXMediaIdMusic );
                break;
                }
            case EBrowseArtist:
                {
                aAttrs.AppendL( TMPXAttribute(KMPXMediaIdGeneral, EMPXMediaGeneralCount) );
                aAttrs.AppendL( TMPXAttribute(KMPXMediaIdMusic,
                                    EMPXMediaMusicAlbumArtFileName ) );
                break;
                }
            case EBrowseAlbum:
            case EBrowseAlbumMediaWall:
                {
                aAttrs.AppendL( TMPXAttribute(KMPXMediaIdMusic,
                    EMPXMediaMusicArtist | EMPXMediaMusicAlbum | EMPXMediaMusicAlbumArtFileName ) );
                aSupportedIds.AppendL( KMPXMediaIdMusic );
                break;
                }
            case EBrowsePlaylist:
                {
                aAttrs.AppendL( TMPXAttribute(KMPXMediaIdGeneral,
                    EMPXMediaGeneralCount | EMPXMediaGeneralDuration ) );
                break;
                }
            case EBrowseGenre:
                {
                aAttrs.AppendL( TMPXAttribute(KMPXMediaIdGeneral, EMPXMediaGeneralCount) );
                break;
                }
            case EBrowseComposer:
                {
                aAttrs.AppendL( TMPXAttribute(KMPXMediaIdGeneral, EMPXMediaGeneralCount) );
                break;
                }
            default:
                {
                User::Leave(KErrArgument);
                }
            }
        }
    else if ( 3 == levels )
        {
        // check the browse type
        switch ( aPath.Id(1).iId2 )
            {
            case EBrowseArtist:
                {
                aAttrs.AppendL( TMPXAttribute(KMPXMediaIdGeneral, EMPXMediaGeneralCount) );
                aAttrs.AppendL( TMPXAttribute(KMPXMediaIdMusic, EMPXMediaMusicAlbumArtFileName ) );
                aSupportedIds.AppendL( KMPXMediaIdMusic );
                break;
                }
            case EBrowseAlbum:
                {

                aAttrs.AppendL( TMPXAttribute(KMPXMediaIdMusic,
                    EMPXMediaMusicArtist | EMPXMediaMusicAlbum | EMPXMediaMusicAlbumArtFileName ) );
                aSupportedIds.AppendL( KMPXMediaIdMusic );
                break;
                }
            case EBrowsePlaylist:
            case EBrowseGenre:
            case EBrowseComposer:
                {
                aAttrs.AppendL( TMPXAttribute(KMPXMediaIdMusic,
//                    EMPXMediaMusicArtist | EMPXMediaMusicAlbumArtFileName ) );
                //added ganes
                    EMPXMediaMusicArtist | EMPXMediaMusicAlbumArtFileName | EMPXMediaMusicAlbum) );
                aSupportedIds.AppendL( KMPXMediaIdMusic );
                break;
                }
            }
        }
    else if ( (4 == levels) && (aPath.Id(1).iId2 == EBrowseArtist) )
        {
        aAttrs.AppendL( TMPXAttribute(KMPXMediaIdMusic,
            EMPXMediaMusicArtist | EMPXMediaMusicAlbum | EMPXMediaMusicAlbumArtFileName ) );
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
void CMPXDbPlugin::PrintMessagesL(
    const CMPXMessage& aMessage)
    {
    MPX_FUNC("CMPXDbPlugin::PrintMessages");

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
void CMPXDbPlugin::PrintMessage(
    const CMPXMessage& aMessage)
    {
    MPX_FUNC("CMPXDbPlugin::PrintMessage");

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
        TMPXItemId id = aMessage.ValueTObjectL<TMPXItemId>(KMPXMessageMediaDeprecatedId);
        MPX_DEBUG3("    deprecated id [0x%x, 0x%x]", id.iId1, id.iId2);
        }
    }

#endif// _DEBUG

// End of file
