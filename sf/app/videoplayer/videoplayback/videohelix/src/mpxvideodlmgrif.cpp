/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of CMPXVideoDownloadMgrInterface class.
*
*/


// Version : %version: 25 %


#include <MMFROPCustomCommandConstants.h>
#include <mmf/common/mmfcontrollerframework.h>
#include <mpxplaybackpluginobserver.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcommandgeneraldefs.h>
#include <pathinfo.h>
#include <f32file.h>

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif  //RD_MULTIPLE_DRIVE

#include "mpxvideoplaybackcontroller.h"
#include "mpxvideodlmgrif.h"
#include "mpxvideodrmhelper.h"
#include <mpxvideoplaybackdefs.h>
#include "mpxvideo_debug.h"


CMPXVideoDlMgrIf*
CMPXVideoDlMgrIf::NewL( CMPXVideoPlaybackController* aController )
{
    MPX_ENTER_EXIT(_L("CMPXVideoDlMgrIf::NewL()"));

    CMPXVideoDlMgrIf* self = new (ELeave) CMPXVideoDlMgrIf();
    CleanupStack::PushL( self );
    self->ConstructL( aController );
    CleanupStack::Pop();
    return self;
}


void CMPXVideoDlMgrIf::ConstructL( CMPXVideoPlaybackController* aController )
{
    iController = aController;
    iUpdateDownload = CIdle::NewL( CActive::EPriorityLow );
}

CMPXVideoDlMgrIf::CMPXVideoDlMgrIf()
    : iDlId(KErrNotFound)
{
}

CMPXVideoDlMgrIf::~CMPXVideoDlMgrIf()
{
    MPX_ENTER_EXIT(_L("CMPXVideoDlMgrIf::~CMPXVideoDlMgrIf()"));

    //
    //  Check if we need to move the clip or remove PDL flag from download
    //
    HandleClose();

    //
    //  Close the connection to the Download Manager
    //  if one has been established
    //
    if ( iDlMgrConnected )
    {
        iDlMgr.Close();
        iDlMgrConnected = EFalse;
    }

    if ( iUpdateDownload->IsActive() )
    {
        iUpdateDownload->Cancel();
    }

    delete iUpdateDownload;

    if ( iClipName )
    {
        delete iClipName;
        iClipName = NULL;
    }
}

void CMPXVideoDlMgrIf::ConnectToDownloadL( CMPXCommand& aCmd )
{
    MPX_ENTER_EXIT(_L("CMPXVideoDlMgrIf::ConnectToDownloadL()"));

    //
    //  Retrieve the download id from the message
    //
    TInt downloadId = *aCmd.Value<TInt>( KMPXCommandPlaybackPDTransactionID );
    TPtrC fileName( aCmd.ValueText( KMPXMediaVideoPlaybackFileName ) );
    iMovePdlFile = *aCmd.Value<TInt>( KMPXMediaVideoMovePdlFile );

    MPX_DEBUG(_L("CMPXVideoDlMgrIf::ConnectToDownloadL(%S, %d)"), &fileName, downloadId );

    if ( iClipName )
    {
        delete iClipName;
        iClipName = NULL;
    }

    iClipName = fileName.AllocL();

    //
    //  A new download id has been sent.  Reset member variables
    //
    iMoveNeeded       = EFalse;
    iCurrentDl        = NULL;
    iDownloadState    = EPbDlStateDownloadCompleted;
    iDlId             = downloadId;
    iPlayerOpened     = EFalse;
    iDlTotalSize      = 0;
    iDownloadProgress = 0;
    iCodDownload      = EFalse;

    if ( iDlMgrConnected )
    {
        iDlMgr.Close();
        iDlMgrConnected = EFalse;
    }

    if ( iDlId != KErrNotFound )
    {
        const TUid KUidLWPlayer = { 0x102824CD };

        //
        //  Connect to the Download Manager
        //
        iDlMgr.ConnectL( KUidLWPlayer, *this, EFalse );
        iDlMgrConnected = ETrue;

        //
        //  Attach to the download id
        //
        RHttpDownload& newDownload = iDlMgr.AttachToDownloadL( iDlId );

        //
        //  Update the members with the current download info
        //
        iCurrentDl = &newDownload;

        //
        //  Set the progressive download attribute on the download
        //
        iCurrentDl->SetBoolAttribute( EDlAttrProgressive, ETrue );

        iCurrentDl->SetBoolAttribute( EDlAttrSilent, EFalse );

        TInt32 activeDownload( 0 );
        iCurrentDl->GetIntAttribute( EDlAttrActiveDownload, activeDownload );
        iCurrentDl->SetIntAttribute( EDlAttrActivePlayedDownload, activeDownload );

        //
        //  Set the Local File attribute
        //
        iCurrentDl->SetStringAttribute( EDlAttrLocalFileName, iClipName->Des() );

        TInt32 dlState;

        //
        //  Get the state, downloaded and total size of the download and save it
        //
        iCurrentDl->GetIntAttribute( EDlAttrLength, iDlTotalSize );
        iCurrentDl->GetIntAttribute( EDlAttrDownloadedSize, iDownloadProgress );
        iCurrentDl->GetIntAttribute( EDlAttrState, dlState );

        MPX_DEBUG(_L("   dlSize = %d, currentSize = %d, state = %d"),
            iDlTotalSize, iDownloadProgress, dlState );

        ConvertDMgrStatetoMpxState( dlState );

        if ( iDownloadState == EPbDlStateDownloadCompleted )
        {
            HandleDownloadComplete();
        }

        MPX_DEBUG(_L("CMPXVideoDlMgrIf::ConnectToDownloadL() Download State = %d Total Size=%d"),
            iDownloadState, iDlTotalSize);
    }
}

void
CMPXVideoDlMgrIf::HandleDMgrEventL( RHttpDownload& aDownload, THttpDownloadEvent aEvent )
{
    MPX_ENTER_EXIT(_L("CMPXVideoDlMgrIf::HandleDMgrEventL()"),
                   _L("iDownloadState = %d iProgressState = %d"),
                    aEvent.iDownloadState, aEvent.iProgressState );

    TMPXPlaybackPdDownloadState prevDownloadState = iDownloadState;

    if ( &aDownload == iCurrentDl )
    {
        switch ( aEvent.iDownloadState )
        {
            case EHttpDlInprogress:
            {
                iDownloadState = EPbDlStateDownloading;

                switch ( aEvent.iProgressState )
                {
                    case EHttpProgResponseBodyReceived:
                    {
                        if ( ! iUpdateDownload->IsActive() )
                        {
                            iUpdateDownload->Start(
                                    TCallBack( CMPXVideoDlMgrIf::UpdateDownloadProgress, this ) );
                        }
                        break;
                    }
                    case EHttpProgCodLoadEnd:
                    {
                        //
                        //  Set the member variable identifying this
                        //  as a COD download and handle the completion
                        //  of the download
                        //
                        iCodDownload = ETrue;

                        HandleDownloadComplete();

                        break;
                    }
                    case EHttpProgMovingContentFile:
                    {
                        //
                        //   Moving Started
                        //
                        MPX_DEBUG( _L("    Moving of file started"));
                        break;
                    }
                    case EHttpProgContentFileMoved:
                    case EHttpProgContentFileMovedAndDestFNChanged:
                    {
                        //
                        //  Move has completed, check error status
                        //
                        CheckForMoveError();

                        break;
                    }
                }

                break;
            }
            case EHttpDlPaused:
            {
                iDownloadState = EPbDlStateDownloadPaused;
                break;
            }
            case EHttpDlCompleted:
            {
                iDownloadState = EPbDlStateDownloadCompleted;

                switch ( aEvent.iProgressState )
                {
                    case EHttpProgNone:
                    {
                        //
                        //  Ignore this event since COD downloads send this
                        //  event after we have already moved the file.
                        //
                        if ( ! iCodDownload )
                        {
                            HandleDownloadComplete();
                        }

                        break;
                    }
                    case EHttpProgMovingContentFile:
                    {
                        //
                        //   Moving Started
                        //
                        MPX_DEBUG( _L("    Moving of file started"));
                        break;
                    }
                    case EHttpProgContentFileMoved:
                    case EHttpProgContentFileMovedAndDestFNChanged:
                    {
                        //
                        //  Move has completed, check error status
                        //
                        CheckForMoveError();

                        break;
                    }
                }

                break;
            }
            case EHttpDlFailed:
            case EHttpDlMediaRemoved:
            case EHttpDlDeleted:
            case EHttpDlDeleting:
            {
                iDownloadState = EPbDlStateDownloadCanceled;

                //
                //  Disconnect from the current download and Download Manager
                //
                iCurrentDl = NULL;
                iDlId = 0;

                break;
            }
            case EHttpDlMoved:
            case EHttpDlCreated:
            case EHttpContentTypeReceived:
            case EHttpDlMediaInserted:
            case EHttpDlPausable:
            case EHttpDlNonPausable:
            case EHttpDlAlreadyRunning:
            default:
                break;
        };

        if ( prevDownloadState != iDownloadState )
        {
            //
            //  Signal the view that the download state has changed
            //
            iController->iMPXPluginObs->HandlePluginEvent(
                MMPXPlaybackPluginObserver::EPDownloadStateChanged,
                iDownloadState,
                KErrNone );
        }
    }
    else
    {
        MPX_DEBUG(_L("CMPXVideoDlMgrIf::HandleDMgrEventL() Message for different Download"));
    }
}

void
CMPXVideoDlMgrIf::DoUpdateDownloadProgress()
{
    MPX_ENTER_EXIT(_L("CMPXVideoDlMgrIf::DoUpdateDownloadProgress()"));

    //
    //  Check if the current download is valid before updating
    //
    if ( iCurrentDl )
    {
        iCurrentDl->GetIntAttribute( EDlAttrDownloadedSize, iDownloadProgress );

        MPX_DEBUG(_L("   dlSize = %d, currentSize = %d"), iDlTotalSize, iDownloadProgress );

        //
        //  Update the size once the controller is initialized
        //
        if ( iController->iState->IsInitialized() )
        {
            iController->iMPXPluginObs->HandlePluginEvent(
                MMPXPlaybackPluginObserver::EPDownloadPositionChanged,
                iDownloadProgress,
                KErrNone );

            //
            //  Update the playback engine with the current downloaded size
            //  Helix needs this to handle reading past current file size
            //
            HandleCustomCommand( EMMFROPControllerSetDownloadSize, iDownloadProgress );
        }
    }
}

void
CMPXVideoDlMgrIf::DoHandleOpenComplete()
{
    MPX_ENTER_EXIT(_L("CMPXVideoDlMgrIf::DoHandleOpenComplete()"));

    if ( iCurrentDl )
    {
        //
        //  Set the player opened flag so messages can be sent to the
        //  video playback engine
        //
        iPlayerOpened = ETrue;

        //
        //  Do not enable progressive download on the playback engine
        //  if the download isn't active
        //
        if ( iDownloadState == EPbDlStateDownloading || iDownloadState == EPbDlStateDownloadPaused )
        {
            //
            //  Send the download id to Helix to trigger progressive
            //  download mode for the Helix engine
            //
            TInt error = HandleCustomCommand( EMMFROPControllerSetDownloadID, iDlId );

            if ( error == KErrNone )
            {
                iCurrentDl->GetIntAttribute( EDlAttrDownloadedSize, iDownloadProgress );

                //
                //  Update Helix with the current downloaded size
                //  Helix needs this to handle reading past current file size
                //
                error = HandleCustomCommand( EMMFROPControllerSetDownloadSize, iDownloadProgress );
            }
        }
    }
}

void
CMPXVideoDlMgrIf::CheckForMoveError()
{
    MPX_ENTER_EXIT(_L("CMPXVideoDlMgrIf::CheckForMoveError()"));

    TInt32 errorId = KErrNone;
    TInt32 globalErrorId = KErrNone;

    iCurrentDl->GetIntAttribute( EDlAttrErrorId, errorId );
    iCurrentDl->GetIntAttribute( EDlAttrGlobalErrorId, globalErrorId );

    MPX_DEBUG(_L("    Moving complete - error = %d global error = %d"), errorId, globalErrorId );

    //
    //  Move Complete
    //
    iMoveNeeded = EFalse;
    iDownloadState = EPbDlStateNotDownloading;
}

TInt CMPXVideoDlMgrIf::HandleCustomCommand( TUint32 aCommand, TUint32 aCmdValue )
{
    MPX_ENTER_EXIT(_L("CMPXVideoDlMgrIf::HandleCustomCommand()"),
                   _L(" aCommand = %d, aCmdValue = %d)"), aCommand, aCmdValue );

    TUint result = KErrNone;

    //
    //  Block custom commands for progressive download if the player has not been opened
    //
    if ( iPlayerOpened )
    {
        TPckgBuf<TUint> cmdValue( aCmdValue );

        const TMMFMessageDestinationPckg dest( KUidInterfaceMMFROPController );
        TPckgBuf<TMMFMessageDestination> message( dest );

        result = iController->iPlayer->CustomCommandSync( dest, aCommand, cmdValue, KNullDesC8 );

        MPX_DEBUG(_L("    Engine CustomCommand retCode = %d"), result);
    }

    return result;
}

void CMPXVideoDlMgrIf::HandleDownloadComplete()
{
    MPX_ENTER_EXIT(_L("CMPXVideoDlMgrIf::HandleDownloadComplete()"));

    iDownloadState = EPbDlStateDownloadCompleted;

    //
    //  Since the download is complete, download should be moved
    //
    if ( iMovePdlFile )
    {
        iMoveNeeded = ETrue;
    }
    else
    {
        //
        //  Disconnect from the current download since move is not required
        //
        iCurrentDl = NULL;
        iDlId = 0;
    }

    //
    //  If the update CIdle object is active, cancel it since the download
    //  is complete and we are updating the values.
    //
    if ( iUpdateDownload->IsActive() )
    {
        iUpdateDownload->Cancel();
    }

    //
    //  Notify the player and the UI view that the download is complete
    //
    HandleCustomCommand( EMMFROPControllerSetDownloadSize, iDlTotalSize );
    HandleCustomCommand( EMMFROPControllerSetDownloadComplete, ETrue );
}

// -------------------------------------------------------------------------------------------------
//  CMPXVideoDlMgrIf::UpdateDownloadProgress
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoDlMgrIf::UpdateDownloadProgress( TAny* aPtr )
{
    MPX_DEBUG(_L("CMPXVideoDlMgrIf::UpdateDownloadProgress()"));

    static_cast<CMPXVideoDlMgrIf*>(aPtr)->DoUpdateDownloadProgress();

    return KErrNone;
}

// -------------------------------------------------------------------------------------------------
//  CMPXVideoDlMgrIf::GetPdlStatusL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoDlMgrIf::GetPdlStatusL( CMPXCommand& aCmd )
{
    MPX_DEBUG(_L("CMPXVideoDlMgrIf::GetPdlStatusL()"));

    if ( iCurrentDl )
    {
        iCurrentDl->GetIntAttribute( EDlAttrDownloadedSize, iDownloadProgress );
    }

    aCmd.SetTObjectValueL<TMPXPlaybackPdDownloadState>( KMPXCommandPlaybackPDState,
                                                        iDownloadState );

    //
    //  If download is complete, ensure that the size is 100%
    //
    if ( iDownloadState == EPbDlStateDownloadCompleted )
    {
        aCmd.SetTObjectValueL<TInt>( KMPXCommandPlaybackPDDownloadedBytes, iDlTotalSize );
    }
    else
    {
        aCmd.SetTObjectValueL<TInt>( KMPXCommandPlaybackPDDownloadedBytes, iDownloadProgress );
    }

    aCmd.SetTObjectValueL<TInt>( KMPXCommandPlaybackPDTotalBytes, iDlTotalSize );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoDlMgrIf::CalculateDestinationFileNameL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoDlMgrIf::CalculateDestinationFileNameL()
{
    MPX_DEBUG(_L("CMPXVideoDlMgrIf::CalculateDestinationFileNameL()"));

    //
    //  We will try to move the file to the video path of the same
    //  drive where the download manager has saved it
    //
    TParsePtrC origFilePath( iClipName->Des() );

#ifdef RD_MULTIPLE_DRIVE
    DriveInfo::TDriveArray driveArray;
    TInt drive = 0;

    RFs::CharToDrive( origFilePath.Drive()[0], drive );

    User::LeaveIfError( PathInfo::GetFullPath( iDestinationFileName, drive, PathInfo::EVideosPath ) );
#else  //RD_MULTIPLE_DRIVE
    TParsePtrC phoneRoot( PathInfo::PhoneMemoryRootPath() );
    TParsePtrC mmcRoot( PathInfo::MemoryCardRootPath() );

    if ( origFilePath.Drive() == phoneRoot.Drive() )
    {
        iDestinationFileName = phoneRoot.DriveAndPath();
    }
    else
    {
        iDestinationFileName = mmcRoot.DriveAndPath();
    }

    iDestinationFileName += PathInfo::VideosPath();
#endif  //RD_MULTIPLE_DRIVE

    iDestinationFileName += origFilePath.NameAndExt();
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoDlMgrIf::HandleClose
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoDlMgrIf::HandleClose()
{
    MPX_ENTER_EXIT(_L("CMPXVideoDlMgrIf::HandleClose()"));

    //
    //  Before we move any files, the controller in uninitialized.
    //  Set the player opened to false
    //
    iPlayerOpened  = EFalse;

    TInt errorCode = KErrNone;

    //
    //  Download may have already been moved.  Check for a current download.
    //
    if ( iCurrentDl )
    {
        //
        //  Check if the plugin should move the file and a move is needed
        //
        if ( iMoveNeeded )
        {
            MPX_TRAPD(err, CalculateDestinationFileNameL());

            if (err == KErrNone)
            {
                errorCode =
                    iCurrentDl->SetStringAttribute( EDlAttrDestFilename, iDestinationFileName );

                MPX_DEBUG(_L(
                    "CMPXVideoDlMgrIf::MoveDownloadedFile() SetStringAtt err = %d"), errorCode );

                if ( errorCode == KErrNone )
                {
                    errorCode = iCurrentDl->Move();
                }

                MPX_DEBUG(_L("CMPXVideoDlMgrIf::MoveDownloadedFile() Move err = %d"), errorCode );
            }

            MPX_DEBUG(_L("CMPXVideoDlMgrIf::MoveDownloadedFile() Calc Dest Filename err = %d"), err );
        }
        else
        {
            //
            //  Move has been called but we have not completed downloading
            //  the file.  We are going to clear the progressive download
            //  flag and the current download variable
            //
            errorCode = iCurrentDl->SetBoolAttribute( EDlAttrProgressive, EFalse );
            iCurrentDl = NULL;

            MPX_DEBUG(_L(
                "CMPXVideoDlMgrIf::MoveDownloadedFile() Removing PD attr retCode=%d"), errorCode);
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoDlMgrIf::GetDownloadRatio
// -------------------------------------------------------------------------------------------------
//
TReal CMPXVideoDlMgrIf::GetDownloadRatio()
{
    TReal downloadRatio = 0.0;
    
    if ( iDownloadState == EPbDlStateDownloadCompleted )
    {
        downloadRatio = 100.0;
    }
    else
    {
        downloadRatio = (TReal)iDownloadProgress / (TReal)iDlTotalSize;
    }
    
    MPX_DEBUG(_L("CMPXVideoDlMgrIf::GetDownloadRatio(%f)"), downloadRatio);
    
    return downloadRatio;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoDlMgrIf::IsDownloadPaused
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoDlMgrIf::IsDownloadPaused()
{
    TBool paused = ( iDownloadState == EPbDlStateDownloadPaused );
    MPX_DEBUG(_L("CMPXVideoDlMgrIf::IsDownloadPaused(%d)"), paused);
    return paused;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoDlMgrIf::ConvertDMgrStatetoMpxState
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoDlMgrIf::ConvertDMgrStatetoMpxState( TInt32 aDMgrState )
{
    MPX_DEBUG(_L("CMPXVideoDlMgrIf::ConvertDMgrStatetoMpxState(%d)"), aDMgrState);

    switch ( aDMgrState )
    {
        case EHttpDlInprogress:
        case EHttpDlCreated:
        {
            iDownloadState = EPbDlStateDownloading;
            break;
        }
        case EHttpDlPaused:
        case EHttpDlNonPausableNetworkLoss:
        {
            iDownloadState = EPbDlStateDownloadPaused;
            break;
        }
        case EHttpDlCompleted:
        {
            iDownloadState = EPbDlStateDownloadCompleted;
            break;
        }
        case EHttpDlFailed:
        {
            iDownloadState = EPbDlStateDownloadError;
            break;
        }
        case EHttpDlDeleted:
        case EHttpDlDeleting:
        {
            iDownloadState = EPbDlStateDownloadCanceled;
            break;
        }
        case EHttpDlMoved:
        case EHttpDlMediaRemoved:
        case EHttpDlMediaInserted:
        case EHttpDlPausable:
        case EHttpDlNonPausable:
        case EHttpDlAlreadyRunning:
        case EHttpDlCancelTransaction:
        default:
        {
            iDownloadState = EPbDlStateNotDownloading;
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoDlMgrIf::UpdateDownloadSizeL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoDlMgrIf::UpdateDownloadSizeL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoDlMgrIf::UpdateDownloadSizeL()"));

    if ( iController->iDrmHelper->IsProtectedL( iController->iFileHandle ) )
    {
        MPX_DEBUG(_L("CMPXVideoDlMgrIf::UpdateDownloadSizeL() download is DRM protected"));

        ContentAccess::CContent* content =
            ContentAccess::CContent::NewLC( iController->iFileHandle );

        ContentAccess::CData* data = content->OpenContentL( ContentAccess::EPeek );

        CleanupStack::PushL( data );

        TInt drmSize;
        data->DataSizeL( drmSize );

        MPX_DEBUG(_L("CMPXVideoDlMgrIf::UpdateDownloadSizeL()    DlMgr Size = %d, DRM Size = %d"),
            iDlTotalSize, drmSize );

        iDlTotalSize = drmSize;

        CleanupStack::PopAndDestroy( 2 );  // data & content
    }
}

//  EOF
