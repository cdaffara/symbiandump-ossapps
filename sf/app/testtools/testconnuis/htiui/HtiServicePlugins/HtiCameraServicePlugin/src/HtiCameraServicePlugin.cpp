/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements the ECom plugin for HTI camera
*                service.
*
*/



// INCLUDE FILES
#include <HtiDispatcherInterface.h>
#include <HtiLogging.h>


#include "HtiCameraServicePlugin.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
const static TUid KCameraServiceUid = { 0x2002EA9E };



// NOTE: Max length for error description is defined
// in HtiDispatcherInterface.h (currently 118).

_LIT8( KErrorNoCommand, "ERROR: No command given" );
_LIT8( KErrorUnknownCmd, "ERROR: Unknown Camera Service command" );
_LIT8( KErrorInitFailed, "ERROR: Failed to init");
_LIT8( KErrorUninitialized, "ERROR: Uninitialized");
_LIT8( KErrInvalidateParameters, "ERROR: Invalidate parameters");
_LIT8( KErrQualityLevel, "ERROR: Invalidate quality level");
_LIT8( KErrorPrepareVideoRecordingFailed, "ERROR: Prepare video recording failed");
_LIT8( KErrorStartVideoRecordingFailed, "ERROR: Start video recording failed");
_LIT8( KErrorPausingVideoRecordingFailed, "ERROR: Pausing video recording failed");
_LIT8( KErrorResumeVideoRecordingFailed, "ERROR: Resume video recording failed");
_LIT8( KErrorStopVideoRecordingFailed, "ERROR: Stop video recording failed");
_LIT8( KErrorSetZoomModeFailed, "ERROR: Set zoom mode failed");
_LIT8( KErrorSetZoomValueFailed, "ERROR: Set zoom value failed");

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHtiCameraServicePlugin::CHtiCameraServicePlugin
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CHtiCameraServicePlugin::CHtiCameraServicePlugin():iIsBusy(EFalse), iError(0),
        iVideoRecordingEngine(NULL), iWaiter(NULL)
    {
    }


// -----------------------------------------------------------------------------
// CHtiCameraServicePlugin::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHtiCameraServicePlugin::ConstructL()
    {
    HTI_LOG_TEXT( "CHtiCameraServicePlugin::ConstructL" );
    iWaiter = new ( ELeave ) CActiveSchedulerWait;
    }


// -----------------------------------------------------------------------------
// CHtiCameraServicePlugin::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHtiCameraServicePlugin* CHtiCameraServicePlugin::NewL()
    {
    CHtiCameraServicePlugin* self = new (ELeave) CHtiCameraServicePlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }


// Destructor
CHtiCameraServicePlugin::~CHtiCameraServicePlugin()
    {
    if(iVideoRecordingEngine)
        {
        delete iVideoRecordingEngine;
        iVideoRecordingEngine = NULL;
        }
    
    delete iWaiter;
    iWaiter = NULL;
    }


// -----------------------------------------------------------------------------
// CHtiCameraServicePlugin::ProcessMessageL
// -----------------------------------------------------------------------------
//
void CHtiCameraServicePlugin::ProcessMessageL( const TDesC8& aMessage,
                                        THtiMessagePriority /*aPriority*/ )
    {
    HTI_LOG_FUNC_IN( "CHtiCameraServicePlugin::ProcessMessageL" );
    HTI_LOG_FORMAT( "Message length = %d", aMessage.Length() );

    if ( iIsBusy )
        {
        HTI_LOG_TEXT( "Plugin is busy - leaving" );
        User::Leave( KErrInUse );
        }

    // Will be set to EFalse in the SendResponseMsg or SendErrorResponseMsg
    // methods when the response has been successfully sent and the plugin is
    // ready for next message.
    iIsBusy = ETrue;

    if ( aMessage.Length() < 1 )
        {
        SendErrorMessageL( KErrArgument, KErrorNoCommand );
        return;
        }
    
    TUint8 command = aMessage.Ptr()[0];
    TInt err = KErrNone;

    switch (command)
        {
        case ECmdInitialize:
            TRAP(err, HandleInitializeCmdL(aMessage.Right( aMessage.Length() - 1 )));
            break;
        case ECmdUninitialize:
            TRAP(err, HandleUninitializeCmdL(aMessage.Right( aMessage.Length() - 1 )));
            break;
        case ECmdPrepareVideoRecording:
            TRAP(err, HandlePrepareVideoRecordingCmdL(aMessage.Right( aMessage.Length() - 1 )));
            break;
        case ECmdStartVideoRecording:
            TRAP(err, HandleStartVideoRecordingCmdL(aMessage.Right( aMessage.Length() - 1 )));
            break;
        case ECmdPausingVideoRecording:
            TRAP(err, HandlePausingVideoRecordingCmdL(aMessage.Right( aMessage.Length() - 1 )));
            break;
        case ECmdResumeVideoRecording:
            TRAP(err, HandleResumeVideoRecordingCmdL(aMessage.Right( aMessage.Length() - 1 )));
            break;
        case ECmdStopVideoRecording:
            TRAP(err, HandleStopVideoRecordingCmdL(aMessage.Right( aMessage.Length() - 1 )));
            break;
        case ECmdCloseVideoRecording:
            TRAP(err, HandleCloseVideoRecordingCmdL(aMessage.Right( aMessage.Length() - 1 )));
            break;
        case ECmdGetZoom:
            TRAP(err, HandleGetZoomCmdL(aMessage.Right( aMessage.Length() - 1 )));
            break;
        case ECmdSetZoom:
            TRAP(err, HandleSetZoomCmdL(aMessage.Right( aMessage.Length() - 1 )));
            break;
        default:
            TRAP(err, SendErrorMessageL(KErrArgument, KErrorUnknownCmd));
            break;
        }
    
    if(err != KErrNone)
        {
        iIsBusy = EFalse;
        User::Leave( err );
        }

    HTI_LOG_FUNC_OUT( "CHtiCameraServicePlugin::ProcessMessageL" );
    }

void CHtiCameraServicePlugin::HandleInitializeCmdL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiCameraServicePlugin::HandleInitializeCmdL" );
    if(aData.Length() != 0)
        {
        SendErrorMessageL(KErrArgument, KErrInvalidateParameters);
        return;
        }
    
    HTI_LOG_TEXT("Initializes Camera Application Engine");
    if(iVideoRecordingEngine)
        {
        delete iVideoRecordingEngine;
        iVideoRecordingEngine = NULL;
        }
    
    iVideoRecordingEngine = CEngineVideoRecording::NewL(*this, 0);
    iVideoRecordingEngine->InitL();
    iWaiter->Start();
    if(iError != KErrNone)
        {
        SendErrorMessageL(iError, KErrorInitFailed);
        }
    else
        {
        HTI_LOG_TEXT("Initializes video recording");
        iVideoRecordingEngine->InitVideoRecorderL();
        
        SendOkMsgL(KNullDesC8);
        }

    HTI_LOG_FUNC_OUT( "CHtiCameraServicePlugin::HandleInitializeCmdL" );
    }

void CHtiCameraServicePlugin::HandleUninitializeCmdL(const TDesC8& aData)
    {
    HTI_LOG_FUNC_IN( "CHtiCameraServicePlugin::HandleUninitializeCmdL" );
    if(aData.Length() != 0)
        {
        SendErrorMessageL(KErrArgument, KErrInvalidateParameters);
        return;
        }
    
    if(iVideoRecordingEngine == NULL)
        {
        SendErrorMessageL(KErrNotReady, KErrorUninitialized);
        return;
        }
    
    delete iVideoRecordingEngine;
    iVideoRecordingEngine = NULL;
    
    SendOkMsgL(KNullDesC8);
    HTI_LOG_FUNC_OUT( "CHtiCameraServicePlugin::HandleUninitializeCmdL" );
    }

void CHtiCameraServicePlugin::HandlePrepareVideoRecordingCmdL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiCameraServicePlugin::HandlePrepareVideoRecordingCmdL" );
    if(aData.Length() < 2 || aData[1] != aData.Length() -2)
        {
        SendErrorMessageL(KErrArgument, KErrInvalidateParameters);
        return;
        }
    
    if(iVideoRecordingEngine == NULL)
        {
        SendErrorMessageL(KErrNotReady, KErrorUninitialized);
        return;
        }
    
    TInt qualityLevelIndex = aData[0];
    if(qualityLevelIndex < 0 || qualityLevelIndex > iVideoRecordingEngine->VideoQualityCount() -1)
        {
        SendErrorMessageL(KErrOverflow, KErrQualityLevel);
        return;
        }
    
    TBuf<255> filePath;
    TInt nextOffset = ParseString( aData, 1, filePath );
    if ( filePath.Length() < 1 || nextOffset < 0 )
        {
        SendErrorMessageL(KErrArgument, KErrInvalidateParameters);
        return;
        }
    
    HTI_LOG_FORMAT("Set video recording file name: %S", &filePath);
    iVideoRecordingEngine->SetVideoRecordingFileNameL(filePath);
    
    HTI_LOG_FORMAT("Prepare video recording with quality level index: %d", qualityLevelIndex);
    iVideoRecordingEngine->PrepareVideoRecordingL(qualityLevelIndex);
    
    iWaiter->Start();
    if(iError != KErrNone)
        {
        SendErrorMessageL(iError, KErrorPrepareVideoRecordingFailed);
        }
    else
        {
        SendOkMsgL(KNullDesC8);
        }
    
     HTI_LOG_FUNC_OUT( "CHtiCameraServicePlugin::HandlePrepareVideoRecordingCmdL" );
    }

void CHtiCameraServicePlugin::HandleStartVideoRecordingCmdL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiCameraServicePlugin::HandleStartVideoRecordingCmdL" );
    if(aData.Length() != 0)
        {
        SendErrorMessageL(KErrArgument, KErrInvalidateParameters);
        return;
        }
    
    if(iVideoRecordingEngine == NULL)
        {
        SendErrorMessageL(KErrNotReady, KErrorUninitialized);
        return;
        }
    
    HTI_LOG_TEXT("Start video recording...");
    iVideoRecordingEngine->StartVideoRecording();
    
    if(iError != KErrNone)
        {
        SendErrorMessageL(iError, KErrorStartVideoRecordingFailed);
        }
    else
        {
        SendOkMsgL(KNullDesC8);
        }

    HTI_LOG_FUNC_OUT( "CHtiCameraServicePlugin::HandleStartVideoRecordingCmdL" );
    }

void CHtiCameraServicePlugin::HandlePausingVideoRecordingCmdL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiCameraServicePlugin::HandlePausingVideoRecordingCmdL" );
    if(aData.Length() != 0)
        {
        SendErrorMessageL(KErrArgument, KErrInvalidateParameters);
        return;
        }
    
    if(iVideoRecordingEngine == NULL)
        {
        SendErrorMessageL(KErrNotReady, KErrorUninitialized);
        return;
        }
    
    HTI_LOG_TEXT("Pausing video recording");
    iVideoRecordingEngine->PauseVideoRecording();
    if(iError != KErrNone)
        {
        SendErrorMessageL(iError, KErrorPausingVideoRecordingFailed);
        }
    else
        {
        SendOkMsgL(KNullDesC8);
        }
    HTI_LOG_FUNC_OUT( "CHtiCameraServicePlugin::HandlePausingVideoRecordingCmdL" );
    }

void CHtiCameraServicePlugin::HandleResumeVideoRecordingCmdL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiCameraServicePlugin::HandleResumeVideoRecordingCmdL" );
    if(aData.Length() != 0)
        {
        SendErrorMessageL(KErrArgument, KErrInvalidateParameters);
        return;
        }
    
    if(iVideoRecordingEngine == NULL)
        {
        SendErrorMessageL(KErrNotReady, KErrorUninitialized);
        return;
        }
    
    HTI_LOG_TEXT("Resume video recording...");
    iVideoRecordingEngine->ResumeVideoRecording();
    
    if(iError != KErrNone)
        {
        SendErrorMessageL(iError, KErrorResumeVideoRecordingFailed);
        }
    else
        {
        SendOkMsgL(KNullDesC8);
        }
    HTI_LOG_FUNC_OUT( "CHtiCameraServicePlugin::HandleResumeVideoRecordingCmdL" );
    }

void CHtiCameraServicePlugin::HandleStopVideoRecordingCmdL( const TDesC8& aData )    
    {
    HTI_LOG_FUNC_IN( "CHtiCameraServicePlugin::HandleStopVideoRecordingCmdL" );
    if(aData.Length() != 0)
        {
        SendErrorMessageL(KErrArgument, KErrInvalidateParameters);
        return;
        }
    
    if(iVideoRecordingEngine == NULL)
        {
        SendErrorMessageL(KErrNotReady, KErrorUninitialized);
        return;
        }
    
    HTI_LOG_TEXT("Stop video recording");
    iVideoRecordingEngine->StopVideoRecording();
    if(iError != KErrNone)
        {
        SendErrorMessageL(iError, KErrorStopVideoRecordingFailed);
        }
    else
        {
        SendOkMsgL(KNullDesC8);
        }
    HTI_LOG_FUNC_OUT( "CHtiCameraServicePlugin::HandleStopVideoRecordingCmdL" );
    }

void CHtiCameraServicePlugin::HandleCloseVideoRecordingCmdL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiCameraServicePlugin::HandleCloseVideoRecordingCmdL" );
    if(aData.Length() != 0)
        {
        SendErrorMessageL(KErrArgument, KErrInvalidateParameters);
        return;
        }
    
    if(iVideoRecordingEngine == NULL)
        {
        SendErrorMessageL(KErrNotReady, KErrorUninitialized);
        return;
        }
    
    HTI_LOG_TEXT("Close video recording");
    iVideoRecordingEngine->CloseVideoRecording();
    SendOkMsgL(KNullDesC8);
    HTI_LOG_FUNC_OUT( "CHtiCameraServicePlugin::HandleCloseVideoRecordingCmdL" );
    }

void CHtiCameraServicePlugin::HandleGetZoomCmdL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiCameraServicePlugin::HandleGetZoomCmdL" );
    if(aData.Length() != 0)
        {
        SendErrorMessageL(KErrArgument, KErrInvalidateParameters);
        return;
        }
    
    if(iVideoRecordingEngine == NULL)
        {
        SendErrorMessageL(KErrNotReady, KErrorUninitialized);
        return;
        }
    
    TUint8 zoomMode = (TUint8)iVideoRecordingEngine->ZoomMode();
    HTI_LOG_FORMAT("Current zoom mode: %d", zoomMode);
    
    TUint32 zoomValue = iVideoRecordingEngine->ZoomValue();
    HTI_LOG_FORMAT("Current zoom value: %d", zoomValue);
    
    TUint32 zoomMinValue = iVideoRecordingEngine->MinZoomValue();
    HTI_LOG_FORMAT("Min zoom value: %d", zoomMinValue);
    
    TUint32 zoomMaxValue = iVideoRecordingEngine->MaxZoomValue();
    HTI_LOG_FORMAT("Max zoom value: %d", zoomMaxValue);
    
    TBuf8<13> buf;
    buf.Append(zoomMode);
    buf.Append((TUint8*)&zoomValue, 4);
    buf.Append((TUint8*)&zoomMinValue, 4);
    buf.Append((TUint8*)&zoomMaxValue, 4);
    SendOkMsgL( buf );

    HTI_LOG_FUNC_OUT( "CHtiCameraServicePlugin::HandleGetZoomCmdL" );
    }

void CHtiCameraServicePlugin::HandleSetZoomCmdL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiCameraServicePlugin::HandleSetZoomCmdL" );
    if(aData.Length() != 5)
        {
        SendErrorMessageL(KErrArgument, KErrInvalidateParameters);
        return;
        }
    
    if(iVideoRecordingEngine == NULL)
        {
        SendErrorMessageL(KErrNotReady, KErrorUninitialized);
        return;
        }
    
    CEngineVideoRecording::TZoomMode zoomMode = (CEngineVideoRecording::TZoomMode)aData[0];
    HTI_LOG_FORMAT("Set zoom mode: %d", zoomMode);
    TRAPD(err, iVideoRecordingEngine->SetZoomModeL(zoomMode));
    if(err != KErrNone)
        {
        SendErrorMessageL(err, KErrorSetZoomModeFailed);
        }
    else
        {
        TInt value = aData[1] + ( aData[2] << 8 )
                + ( aData[3] << 16 )
                + ( aData[4] << 24 );
        HTI_LOG_FORMAT("Set zoom value: %d", value);
        TRAPD(err, iVideoRecordingEngine->SetZoomValueL(value));
        if(err != KErrNone)
            {
            SendErrorMessageL(err, KErrorSetZoomValueFailed);
            }
        else
            {
            SendOkMsgL(KNullDesC8);
            }
        }
    
    HTI_LOG_FUNC_OUT( "CHtiCameraServicePlugin::HandleSetZoomCmdL" );
    }


// -----------------------------------------------------------------------------
// CHtiCameraServicePlugin::IsBusy
// -----------------------------------------------------------------------------
//
TBool CHtiCameraServicePlugin::IsBusy()
    {
    return iIsBusy;
    }


// ----------------------------------------------------------------------------
void CHtiCameraServicePlugin::SendOkMsgL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiCameraServicePlugin::SendOkMsgL: Starting" );

    User::LeaveIfNull( iDispatcher );

    HBufC8* temp = HBufC8::NewL( aData.Length() + 1 );
    TPtr8 response = temp->Des();
    response.Append( ( TChar ) EResultOk );
    response.Append( aData );
    User::LeaveIfError( iDispatcher->DispatchOutgoingMessage(
        temp, KCameraServiceUid ) );
    iIsBusy = EFalse;
    HTI_LOG_FUNC_OUT( "CHtiCameraServicePlugin::SendOkMsgL: Done" );
    }

// ----------------------------------------------------------------------------
void CHtiCameraServicePlugin::SendErrorMessageL( TInt aError, const TDesC8& aDescription )
    {
    HTI_LOG_FUNC_IN( "CHtiCameraServicePlugin::SendErrorMessageL: Starting" );
    User::LeaveIfNull( iDispatcher );
    User::LeaveIfError( iDispatcher->DispatchOutgoingErrorMessage(
        aError, aDescription, KCameraServiceUid ) );
    iIsBusy = EFalse;
    HTI_LOG_FUNC_OUT( "CHtiCameraServicePlugin::SendErrorMessageL: Done" );
    }

TInt CHtiCameraServicePlugin::ParseString( const TDesC8& aRequest,
                                          TInt aOffset,
                                          TDes& aResult )
    {
    HTI_LOG_FUNC_IN( "CHtiCameraServicePlugin::ParseString" );

    // If offset outside the string return empty string
    if ( aOffset >= aRequest.Size() )
        {
        return aOffset;
        }

    TInt length = aRequest[aOffset];
    HTI_LOG_FORMAT( "String length = %d", length );

    // If length is zero return empty string
    if ( length < 1 )
        {
        return aOffset + 1;
        }

    if ( length > aResult.MaxLength() )
        {
        return KErrBadDescriptor;
        }

    TInt nextOffset = length + aOffset + 1;
    HTI_LOG_FORMAT( "Next offset = %d", nextOffset );
    HTI_LOG_FORMAT( "Request size = %d", aRequest.Size() );

    if ( nextOffset > aRequest.Size() )
        {
        return KErrArgument;
        }

    aResult.Copy( aRequest.Mid( aOffset + 1, length ) );

    HTI_LOG_FUNC_OUT( "CHtiCameraServicePlugin::ParseString" );
    return nextOffset;
    }

void CHtiCameraServicePlugin::MevroInitComplete( TInt aError )
    {
    HTI_LOG_FUNC_IN( "CHtiCameraServicePlugin::MevroInitComplete" );
    HTI_LOG_FORMAT("aError = %d", aError);
    iError = aError; 
    iWaiter->AsyncStop();
    HTI_LOG_FUNC_OUT( "CHtiCameraServicePlugin::MevroInitComplete" );
    }

void CHtiCameraServicePlugin::MevroVideoPrepareComplete(TInt aError)
    {
    HTI_LOG_FUNC_IN( "CHtiCameraServicePlugin::MevroVideoPrepareComplete" );
    HTI_LOG_FORMAT("aError = %d", aError);
    iError = aError;
    iWaiter->AsyncStop();
    HTI_LOG_FUNC_OUT( "CHtiCameraServicePlugin::MevroVideoPrepareComplete" );
    }

void CHtiCameraServicePlugin::MevroVideoRecordingOn(TInt aError)
    {
    HTI_LOG_FUNC_IN( "CHtiCameraServicePlugin::MevroVideoRecordingOn" );
    HTI_LOG_FORMAT("aError = %d", aError);
    iError = aError;
    HTI_LOG_FUNC_OUT( "CHtiCameraServicePlugin::MevroVideoRecordingOn" );
    }

void CHtiCameraServicePlugin::MevroVideoRecordingPaused(TInt aError)
    {
    HTI_LOG_FUNC_IN( "CHtiCameraServicePlugin::MevroVideoRecordingPaused" );
    HTI_LOG_FORMAT("aError = %d", aError);
    iError = aError;
    HTI_LOG_FUNC_OUT( "CHtiCameraServicePlugin::MevroVideoRecordingPaused" );
    }

void CHtiCameraServicePlugin::MevroVideoRecordingComplete(TInt aError)
    {
    HTI_LOG_FUNC_IN( "CHtiCameraServicePlugin::MevroVideoRecordingComplete" );
    HTI_LOG_FORMAT("aError = %d", aError);
    iError = aError;
    HTI_LOG_FUNC_OUT( "CHtiCameraServicePlugin::MevroVideoRecordingComplete" );
    }
//  End of File
