/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:    Implementation for MPX My Videos Collection Move, Copy and Delete operations.*
*/




// INCLUDE FILES
#include <e32cmn.h>
#include <s32mem.h>
#include <mpxlog.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxcollectionpluginobserver.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>
#include <mpxmessagecontainerdefs.h>
#include <vcxmyvideosuids.h>
#include <drmutility.h>
#include <bautils.h>
#include "vcxmyvideoscollectionplugin.h"
#include "vcxmyvideoscollection.hrh"
#include "vcxmyvideoscollectionutil.h"
#include "vcxmyvideosvideocache.h"
#include "vcxmyvideoscategories.h"
#include "vcxmyvideosmessagelist.h"
#include "vcxmyvideosasyncfileoperations.h"
#include "vcxmyvideosasyncfilecopy.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosAsyncFileOperations* CVcxMyVideosAsyncFileOperations::NewL(
    CVcxMyVideosCollectionPlugin& aCollection )
    {
    CVcxMyVideosAsyncFileOperations* self = new (ELeave) CVcxMyVideosAsyncFileOperations(
            aCollection );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosAsyncFileOperations::~CVcxMyVideosAsyncFileOperations()
    {
    iOperationIdArray.Close();
    iOperationResult.Close();
    delete iFileCopier;
    delete iMediaForMoveOp;
    delete iMediaForCopyOp;
    }

// ----------------------------------------------------------------------------
// Constructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosAsyncFileOperations::CVcxMyVideosAsyncFileOperations( CVcxMyVideosCollectionPlugin& aCollection )
: iCollection( aCollection )
    {
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAsyncFileOperations::ConstructL ()
    {
    iFileCopier = CVcxMyVideosAsyncFileCopy::NewL( iCollection.iFs );
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::DeleteVideoL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAsyncFileOperations::DeleteVideoL( TUint32 aMdsId, TBool aForce )
    {
    MPX_FUNC("CVcxMyVideosAsyncFileOperations::DeleteVideoL");
            
    TInt pos;
    CMPXMedia* videoInCache = iCollection.iCache->FindVideoByMdsIdL( aMdsId, pos );
                    
    if ( !videoInCache )
        {
        MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: delete failed, MPX item not found from cache.");
        User::Leave( KErrNotFound );
        }
    
    TUint32 downloadId = TVcxMyVideosCollectionUtil::DownloadIdL( *videoInCache );            
    if ( downloadId )
        {
        TVcxMyVideosDownloadState dlState =
                TVcxMyVideosCollectionUtil::DownloadStateL( *videoInCache );
                        
        if ( aForce )
            {
            MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: aForce is ETrue, the video will be deleted even if download is ongoing");
            }
        else if ( dlState != EVcxMyVideosDlStateNone )
            {
            MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: delete failed since there is download going on.");
            MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: use download cancel instead.");
            User::Leave( KErrArgument );
            }
        else
            {
            MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: download id was != 0 but download state was EVcxMyVideosDlStateNone");
            MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: -> we delete the video, it is prob gone from Download Manager already.");
            }        
        }
            
    if ( !videoInCache->IsSupported( KMPXMediaGeneralUri ) )
        {
        MPX_DEBUG1("CVcxMyVideosAsyncFileOperations::DeleteVideoL() KMPXMediaGeneralUri attribute missing -> leaving");
        User::Leave( KErrArgument );
        }
    
    MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: trying to delete: %S",
            &videoInCache->ValueText( KMPXMediaGeneralUri ));
    TInt err = iCollection.iFs.Delete( videoInCache->ValueText( KMPXMediaGeneralUri ) );
        
    if ( err != KErrNone )
        {
        MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: file delete failed: %d", err );

        if ( err == KErrNotFound || err == KErrPathNotFound )
            {
            MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: file was not found, trying to remove mds object anyways");
            TInt result = iCollection.iMyVideosMdsDb->RemoveVideo( aMdsId );
            if ( result == KErrNone )
                {
                MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: %d removed from mds", aMdsId); 
                //mds events will clean cache
                }
            else
                {
                if ( result == KErrNotFound )
                    {
                    // file is gone, and mds item is gone -> try to delete from cache
                    MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: %d not found from MDS, trying to delete from cache", aMdsId );
                    TInt result = iCollection.iCache->RemoveL( aMdsId );
                    if ( result == KErrNone )
                        {
                        MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: %d deleted from cache, generating event", aMdsId );
                        iCollection.iMessageList->AddEventL( TMPXItemId( aMdsId, 0),
                                EMPXItemDeleted, EVcxMyVideosListNoInfo );
                        iCollection.iMessageList->SendL();
                        } 
                    }
                else
                    {
                    //file is gone, mds item and cache are still present, couldnt remove them
                    MPX_DEBUG3("CVcxMyVideosAsyncFileOperations:: failed to remove %d from mds (%d)", aMdsId, result);
                    User::Leave( result );
                    }
                }
            }            
        else
            {
            // file delete failed and file still exists
            MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: leaving mds object intact");
            User::Leave( err );
            }
        }
    else
        {
        MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: file delete successful, deleting item from MDS also to speed things up");
        TInt result = iCollection.iMyVideosMdsDb->RemoveVideo( aMdsId );
        if ( result == KErrNone )
            {
            MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: %d removed from mds", aMdsId);
            //mds events will clean cache
            }
        else
            {
            if ( result == KErrNotFound )
                {
                // file is gone, and mds item is gone -> try to delete from cache
                MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: %d not found from MDS, trying to delete from cache", aMdsId );
                TInt result = iCollection.iCache->RemoveL( aMdsId );
                if ( result == KErrNone )
                    {
                    MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: %d deleted from cache, generating event", aMdsId );
                    iCollection.iMessageList->AddEventL( TMPXItemId( aMdsId, 0),
                            EMPXItemDeleted, EVcxMyVideosListNoInfo );
                    iCollection.iMessageList->SendL();
                    }
                }
            else
                {
                //mds item and cache are still present, couldnt remove them
                MPX_DEBUG3("CVcxMyVideosAsyncFileOperations:: failed to remove %d from mds (%d)", aMdsId, result);
                User::Leave( result );
                }
            }
        }   
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileOperations::HandleMoveOrCopyStepL
// ----------------------------------------------------------------------------
//
MVcxMyVideosActiveTaskObserver::TStepResult
        CVcxMyVideosAsyncFileOperations::HandleMoveOrCopyStepL()
    {
    // Reset inactivity timer. This will prevent ThumbAGDaemon start running while
    // Move/copy is ongoing and failing the operation due to locked file handle.
    User::ResetInactivityTime();
    
    CMPXMedia& cmd = iCollection.iActiveTask->GetCommand();
    
    MVcxMyVideosActiveTaskObserver::TStepResult stepResult;
        
    if ( iCurrentOperationIndex == 0 && !iFileCopier->CopyIsOngoing() )
        {
        InitMoveOrCopyOperationsL( cmd );        
        }

    TRAPD( err, MoveOrCopyVideoL( iOperationIdArray[iCurrentOperationIndex] ) );

    if ( iFileCopier->CopyIsOngoing() && err == KErrNone )
        {
        // copy didnt finish yet, lets do some more steps before jumping to next file
        return MVcxMyVideosActiveTaskObserver::EMoreToCome;
        }
        
    iOperationResult.AppendL( err );
    
    iCurrentOperationIndex++;
    
    // End operations
    if ( iCurrentOperationIndex > (iOperationIdArray.Count() - 1) )
        {
        iCurrentOperationIndex = 0;
        stepResult             = MVcxMyVideosActiveTaskObserver::EDone;
        if ( iIsMoveOperation )
            {
            SendOperationRespL( KVcxMessageMyVideosMoveResp );
            }
        else
            {
            SendOperationRespL( KVcxMessageMyVideosCopyResp );
            }
        }
    else
        {
        stepResult = MVcxMyVideosActiveTaskObserver::EMoreToCome;
        }
        
    return stepResult;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileOperations::InitMoveOrCopyOperationsL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAsyncFileOperations::InitMoveOrCopyOperationsL( CMPXMedia& aCmd )
    {
    if ( !aCmd.IsSupported( KMPXMediaArrayContents ) )
        {
        MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: error, no array defined");
        User::Leave( KErrArgument );
        }

    CMPXMediaArray* idMediaArray = aCmd.Value<CMPXMediaArray>(
            KMPXMediaArrayContents );

    if ( idMediaArray->Count() == 0 )
        {
        MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: error, no items in array ");
        User::Leave( KErrArgument );
        }
    
    TUint32 cmdId = aCmd.ValueTObjectL<TUint32>( KVcxMediaMyVideosCommandId );    
    if ( cmdId == KVcxCommandMyVideosMove )
        {
        MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: move operation");
        iIsMoveOperation = ETrue;
        }
    else
        {
        iIsMoveOperation = EFalse;
        }

    iTargetDrive = aCmd.ValueTObjectL<TInt32>( KVcxMediaMyVideosInt32Value );
    
    TMPXItemId mpxId;    
    iOperationIdArray.Reset();
    TInt count = idMediaArray->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        mpxId = idMediaArray->AtL( i )->
                        ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
        MPX_DEBUG3("CVcxMyVideosAsyncFileOperations:: MPX ID: (%d, %d) will be moved ",
                mpxId.iId1,
                mpxId.iId2);
        iOperationIdArray.AppendL( idMediaArray->AtL( i )->
                ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId ).iId1 );
        }
    
    iCollection.SendMyVideosMessageL( KVcxMessageMyVideosMoveOrCopyStarted, &aCmd );

    iOperationResult.Reset();
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileOperations::CancelOperationL
// Called when leave or cancel occurs for the operation, generates resp msg.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAsyncFileOperations::CancelOperationL( TInt aErr )
    {
    if ( iCollection.iActiveTask->IsActive() )
        {
        TInt mvCmdId = -1;
        CMPXMedia& cmd = iCollection.iActiveTask->GetCommand();
        TMPXCommandId commandId = *cmd.Value<TMPXCommandId>( KMPXCommandGeneralId );
        
        if ( commandId == KVcxCommandIdMyVideos )
            {
            mvCmdId = cmd.ValueTObjectL<TUint32>( KVcxMediaMyVideosCommandId );
            }
                    
        TInt messageId; 
        
        switch ( mvCmdId )
            {
            case KVcxCommandMyVideosMove:
                MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: generating KVcxMessageMyVideosMoveResp");
                messageId = KVcxMessageMyVideosMoveResp;
                if ( iFileCopier->CopyIsOngoing() )
                    {
                    //these have to be in this order, otherwise wrong item gets removed from mds
                    TRAP_IGNORE( HandleFileCopyCompletedL( aErr ) ); // rolls mds back
                    iFileCopier->Cancel(); // removes generated file and resets variables
                    }
                break;
                
            case KVcxCommandMyVideosCopy:
                MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: generating KVcxMessageMyVideosCopyResp");
                messageId = KVcxMessageMyVideosCopyResp;
                if ( iFileCopier->CopyIsOngoing() )
                    {
                    TRAP_IGNORE( HandleFileCopyCompletedL( aErr ) ); // rolls mds back
                    iFileCopier->Cancel(); // removes generated file and resets variables
                    }
                break;
                
            case KVcxCommandMyVideosDelete:
                MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: generating KVcxMessageMyVideosDeleteResp");
                messageId = KVcxMessageMyVideosDeleteResp;
                break;
            
            default:
                MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: no resp msg for this operation");
                return;
            }
                        
        // generates response array and resp message
        for ( TInt i = iCurrentOperationIndex; i < iOperationIdArray.Count(); i++ )
            {
            iOperationResult.Append( aErr );
            }
        iCurrentOperationIndex = 0;

        SendOperationRespL( messageId );
        }
    else
        {
        MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: no move,copy or delete operations were going on (nor anything else)");
        User::Leave( KErrNotFound );
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileOperations::SendOperationRespL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAsyncFileOperations::SendOperationRespL( TInt aCmdId )
    {
    CMPXMessage* message = TVcxMyVideosCollectionUtil::CreateEmptyMediaListL();
    CleanupStack::PushL( message ); // 1->

    CMPXMediaArray* messageArray = message->Value<CMPXMediaArray>(
            KMPXMediaArrayContents );
    message->SetTObjectValueL<TUid>(KMPXMessageCollectionId,
            TUid::Uid(KVcxUidMyVideosMpxCollection));        
            
    // Add results
    CMPXMedia* media;
    TInt count = iOperationIdArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: item added to array");
        
        media = CMPXMedia::NewL();
        CleanupStack::PushL( media ); // 2->
        media->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId,
                TMPXItemId( iOperationIdArray[i], 0) );
        media->SetTObjectValueL<TInt32>( KVcxMediaMyVideosInt32Value,
                iOperationResult[i] );
        
        messageArray->AppendL( media );
        CleanupStack::Pop( media ); // <-2                            
        }
        
    // Set message attributes
    //
    message->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId,
            TMPXItemId( KVcxCommandIdMyVideos, 0 ));

    message->SetTObjectValueL<TInt>( KVcxMediaMyVideosCommandId, aCmdId );
    
    iCollection.iMessageList->AddL( message );
    CleanupStack::Pop( message ); // <-1
    iCollection.iMessageList->SendL();
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileOperations::MoveOrCopyVideoL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAsyncFileOperations::MoveOrCopyVideoL( TUint32 aMdsId )
    {
    if ( iFileCopier->CopyIsOngoing() )
        {
        TInt err = iFileCopier->DoNextBlockCopy();
        if ( err != KErrNone || !iFileCopier->CopyIsOngoing() )
            {
            HandleFileCopyCompletedL( err );
            }
        return;
        }
        
    //New file copy starts -> do sanity checks and mds and collection preparations
    InitSingleMoveOrCopyL( aMdsId );
            
    MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: copying: %S", &iSourcePath);
    MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: to     : %S", &iTargetPath);
    
    TBool completed = ETrue; // set to avoid warning
    TRAPD( err, completed = iFileCopier->CopyL( iSourcePath, iTargetPath ) );
    
    if ( completed || err )
        {
        HandleFileCopyCompletedL( err );
        }
    }    

// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileOperations::InitSingleMoveOrCopyL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAsyncFileOperations::InitSingleMoveOrCopyL( TUint32 aMdsId )
    {
    //get media from cache or mds
    TInt pos;
    CMPXMedia* videoInCache = iCollection.iCache->FindVideoByMdsIdL( aMdsId, pos );

    CMPXMedia* video = NULL;
    if ( videoInCache )
        {
        video = CMPXMedia::NewL( *videoInCache );
        }
    else
        {
        video = iCollection.iMyVideosMdsDb->CreateVideoL( aMdsId, EFalse /* brief details */ );
        }
        
    if ( !video )
        {
        MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: mds id %d not found from mds or cache", aMdsId);
        User::Leave( KErrNotFound );
        }

    CleanupStack::PushL( video ); // 1->
    
    // sanity checks
    if ( TVcxMyVideosCollectionUtil::DownloadIdL( *video ) != 0 )
        {
        MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: file is being downloaded, fail, leaving with KErrInUse code.");
        User::Leave( KErrInUse );
        }

    iSourcePath = video->ValueText( KMPXMediaGeneralUri );
            
    if ( !DriveHasEnoughFreeSpaceL( iSourcePath, iTargetDrive ) )
        {
        MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: target drive full -> skipping");
        User::Leave( KErrDiskFull );
        }

    TUint att = 0;
    iCollection.iFs.Att( iSourcePath, att);    
    if ( iIsMoveOperation && (att & KEntryAttReadOnly) )
        {
        MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: move operation and source file is read only -> skipping");
        User::Leave( KErrAccessDenied );
        }

    TInt sourceDrive;
    User::LeaveIfError( iCollection.iFs.CharToDrive( iSourcePath[0], sourceDrive ) );

    if ( sourceDrive == iTargetDrive )
        {
        MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: source and target drives are the same, leaving with KErrAlreadyExists.");
        CleanupStack::PopAndDestroy( video ); // <-1
        User::Leave( KErrAlreadyExists );
        }
    
    GenerateTargetPathForMoveOrCopyL( iSourcePath, iTargetPath, iTargetDrive );

    MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: target path = %S", &iTargetPath );
    
    // update mds and cache
    delete iMediaForMoveOp;
    iMediaForMoveOp = NULL;
    delete iMediaForCopyOp;
    iMediaForCopyOp = NULL;

    if ( iIsMoveOperation )
        {
        // Update existing media.
        // Create new media object with only KMPXMediaGeneralId, and KMPXMediaGeneralUri
        // attributes set, that way update is lighter operation.
        iMediaForMoveOp = CMPXMedia::NewL();
        iMediaForMoveOp->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId,
               video->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
        iMediaForMoveOp->SetTextValueL( KMPXMediaGeneralUri, iTargetPath );
        
        iCollection.SetVideoL( *iMediaForMoveOp );
        }
    else
        {
        // Create new media.
        iMediaForCopyOp = CMPXMedia::CopyL( *video );
        iMediaForCopyOp->SetTextValueL( KMPXMediaGeneralUri, iTargetPath );
        iCollection.AddVideoToMdsAndCacheL( *iMediaForCopyOp );
        }
 
     CleanupStack::PopAndDestroy( video ); // <-1           
    }


// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileOperations::HandleFileCopyCompletedL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAsyncFileOperations::HandleFileCopyCompletedL( TInt aErr )
    {
    if ( aErr == KErrNone )
        {
        MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: Copy succeeded");
        if ( iIsMoveOperation )
            {
            MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: move op: deleting source file");
            aErr = BaflUtils::DeleteFile( iCollection.iFs, iSourcePath );
            if ( aErr != KErrNone )
                {
                MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: delete for source file failed: %d", aErr );
                }
            }
        }
    else
        {
        MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: CopyFile failed: %d", aErr);
        }
    
    // roll everything back if file operations failed
    if ( aErr != KErrNone )
        {
        if ( iIsMoveOperation )
            {
            MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: move operation failed %d", aErr );
            MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: setting media path back in MDS" );
            iMediaForMoveOp->SetTextValueL( KMPXMediaGeneralUri, iSourcePath );
            iCollection.SetVideoL( *iMediaForMoveOp );
            MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: deleting target file");
            TInt delErr = BaflUtils::DeleteFile( iCollection.iFs, iTargetPath );
            if ( delErr != KErrNone )
                {
                MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: delete for target file failed: %d", delErr );
                }
            }
        else
            {
            MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: copy failed %d", aErr );
            MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: deleting the added media object and leaving");
            iCollection.iMyVideosMdsDb->RemoveVideo( iMediaForCopyOp->ValueTObjectL<TMPXItemId>(
                    KMPXMediaGeneralId ).iId1 );
            }
        }
    
    delete iMediaForMoveOp;
    iMediaForMoveOp = NULL;    
    delete iMediaForCopyOp;
    iMediaForCopyOp = NULL;

    User::LeaveIfError( aErr );
    
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileOperations::DriveHasEnoughFreeSpaceL
// ----------------------------------------------------------------------------
//
TBool CVcxMyVideosAsyncFileOperations::DriveHasEnoughFreeSpaceL( const TDesC& aPath, TInt aDrive )
    {
    TEntry entry;
    User::LeaveIfError( iCollection.iFs.Entry( aPath, entry ) );
    TUint32 size = static_cast<TUint32>( entry.iSize );
    
    TVolumeInfo volInfo;
    User::LeaveIfError( iCollection.iFs.Volume( volInfo, aDrive ) );
    
    TInt64 freeSpace = volInfo.iFree;

    const TInt K4MBSlack = 0x400000;

    MPX_DEBUG3("CVcxMyVideosAsyncFileOperations:: space needed: %d, space available: %d", size + K4MBSlack,
            freeSpace );
    
    if ( freeSpace < ( size + K4MBSlack ) )
        {
        MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: target drive does not have enough free space");
        return EFalse;
        }
    else
        {
        return ETrue;
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileOperations::GenerateTargetPathForMoveOrCopyL
// ----------------------------------------------------------------------------
//    
void CVcxMyVideosAsyncFileOperations::GenerateTargetPathForMoveOrCopyL(
        const TDesC& aSourcePath, TDes& aTargetPath, TInt aTargetDrive )
    {
    MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: source path = %S", &aSourcePath );
    
    aTargetPath.Zero();
    
    TChar targetDriveChar;
    User::LeaveIfError( iCollection.iFs.DriveToChar( aTargetDrive, targetDriveChar ) );    
    aTargetPath.Append( targetDriveChar );
    aTargetPath.Append( ':' );
    
    TInt sourceDrive;
    User::LeaveIfError( iCollection.iFs.CharToDrive( aSourcePath[0], sourceDrive ) );

    TInt systemDrive = iCollection.iFs.GetSystemDrive();
    
    MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: source drive: %d", sourceDrive);
    MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: target drive: %d", aTargetDrive);
    MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: system drive: %d", systemDrive);

    _LIT(KDataDes, "data");
        
    if ( sourceDrive == systemDrive )
        {
        //remove *:\data\* from the path
        const TInt dataWordStartPos = 3;
        const TInt dataWordLength   = 4;
        
        TPtrC pathData( aSourcePath.Mid( dataWordStartPos, dataWordLength ) );
        MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: sourcePath.Mid(3,4)= %S", &pathData);

        if ( pathData == KDataDes )
            {
            MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: source drive is system drive and 'data' exists in sourcePath");
            MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: not copying 'data' to the target path");
            
            const TInt skipDataWordPos = 7;
            aTargetPath.Append( aSourcePath.Mid( skipDataWordPos ) );
            }
        else
            {
            const TInt dontSkipDataWordPos = 2;
            aTargetPath.Append( aSourcePath.Mid( dontSkipDataWordPos ) );
            }
        }    
    else if ( aTargetDrive == systemDrive )
        {
        MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: target drive is system drive -> adding 'data' to path");
        aTargetPath.Append( '\\' );
        aTargetPath.Append( KDataDes );
        aTargetPath.Append( aSourcePath.Mid( 2 ) );
        }
    else 
        {
        MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: source and target drives are not system drive, not adding or removing 'data'");        
        aTargetPath.Append( aSourcePath.Mid( 2 ) );        
        }
    const TInt KMaxPathLength = 255;
    TBuf<KMaxPathLength> uniquePath;
    TVcxMyVideosCollectionUtil::MakeUniqueFileNameL( iCollection.iFs, aTargetPath, uniquePath );
    aTargetPath = uniquePath;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileOperations::HandleDeleteStepL
// ----------------------------------------------------------------------------
//
MVcxMyVideosActiveTaskObserver::TStepResult CVcxMyVideosAsyncFileOperations::HandleDeleteStepL()
    {
    // Reset inactivity timer. This will prevent ThumbAGDaemon start running while
    // delete is ongoing and failing the operation due to locked file handle.
    User::ResetInactivityTime();
    
    CMPXMedia& cmd = iCollection.iActiveTask->GetCommand();
    
    //no sanity checks for array items, since we want to generate all events, even if there is nothing to delete
    
    MVcxMyVideosActiveTaskObserver::TStepResult stepResult;
    
    TUint32 cmdId = cmd.ValueTObjectL<TUint32>( KVcxMediaMyVideosCommandId );    
        
    // Start operations
    if ( iCurrentOperationIndex == 0 )
        {
        iOperationIdArray.Reset();

        if ( cmd.IsSupported( KMPXMediaArrayContents ) )
            {    
            CMPXMediaArray* idMediaArray = cmd.Value<CMPXMediaArray>(
                KMPXMediaArrayContents );
                
            TMPXItemId mpxId;
            TInt count = idMediaArray->Count();
            for ( TInt i = 0; i < count; i++ )
                {
                mpxId = idMediaArray->AtL( i )->
                                ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
                MPX_DEBUG3("CVcxMyVideosAsyncFileOperations:: MPX ID: (%d, %d) will be deleted ",
                        mpxId.iId1,
                        mpxId.iId2);
                iOperationIdArray.AppendL( idMediaArray->AtL( i )->
                        ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId ).iId1 );
                }

            }
                    
        iCollection.SendMyVideosMessageL( KVcxMessageMyVideosDeleteStarted, &cmd );

        iOperationResult.Reset();
        }

    if ( iOperationIdArray.Count() > 0 )
        {
        TRAPD( err, DeleteVideoL( iOperationIdArray[iCurrentOperationIndex] ) );
        iOperationResult.AppendL( err );
        }
        
    iCurrentOperationIndex++;
    
    // End operations
    if ( iCurrentOperationIndex > (iOperationIdArray.Count() - 1) )
        {
        iCurrentOperationIndex = 0;
        stepResult = MVcxMyVideosActiveTaskObserver::EDone;
        SendOperationRespL( KVcxMessageMyVideosDeleteResp );
        }
    else
        {
        stepResult = MVcxMyVideosActiveTaskObserver::EMoreToCome;
        }
        
    return stepResult;
    }


