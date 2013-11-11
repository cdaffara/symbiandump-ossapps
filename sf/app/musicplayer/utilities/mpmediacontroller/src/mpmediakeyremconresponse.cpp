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
* Description: Music Player media key handler.
*              Helper class for sending response back to Remote Controller Framework.
*
*/

// INCLUDE FILES
#include "mpmediakeyremconresponse.h"
#include "mptrace.h"


/*!
 \internal
 C++ default constructor can NOT contain any code, that
 might leave.
 */
MpMediaKeyRemConResponse::MpMediaKeyRemConResponse(
    CRemConCoreApiTarget& aRemConCoreApiTarget )
    : CActive( CActive::EPriorityStandard ),
      iRemConCoreApiTarget( aRemConCoreApiTarget )
{
    TX_ENTRY
    CActiveScheduler::Add( this );
    TX_EXIT
}

/*!
 \internal
 Two-phased constructor.
 */
MpMediaKeyRemConResponse* MpMediaKeyRemConResponse::NewL(
    CRemConCoreApiTarget& aRemConCoreApiTarget )
{
    MpMediaKeyRemConResponse* self =
        new (ELeave) MpMediaKeyRemConResponse( aRemConCoreApiTarget );

    return self;
}

/*!
 \internal
 Destructor
 */
MpMediaKeyRemConResponse::~MpMediaKeyRemConResponse()
{
    TX_ENTRY
    Cancel();
    iResponseArray.Close();
    TX_EXIT
}

/*!
 \internal
 Send the any key response back to Remcon server
 */
void MpMediaKeyRemConResponse::CompleteAnyKey(
    TRemConCoreApiOperationId aOperationId )
{
    TX_ENTRY_ARGS(" OperationId: " << aOperationId );
    if ( !IsActive() ) {
        switch ( aOperationId )
        {
            case ERemConCoreApiPausePlayFunction:
            {
                iRemConCoreApiTarget.PausePlayFunctionResponse( iStatus, KErrNone );
                SetActive();
                break;
            }
            case ERemConCoreApiPlay:
            {
                iRemConCoreApiTarget.PlayResponse( iStatus, KErrNone );
                SetActive();
                break;
            }
            case ERemConCoreApiStop:
            {
                iRemConCoreApiTarget.StopResponse( iStatus, KErrNone );
                SetActive();
                break;
            }
            case ERemConCoreApiPause:
            {
                iRemConCoreApiTarget.PauseResponse( iStatus, KErrNone );
                SetActive();
                break;
            }
            case ERemConCoreApiRewind:
            {
                iRemConCoreApiTarget.RewindResponse( iStatus, KErrNone );
                SetActive();
                break;
            }
            case ERemConCoreApiFastForward:
            {
                iRemConCoreApiTarget.FastForwardResponse( iStatus, KErrNone );
                SetActive();
                break;
            }
            case ERemConCoreApiBackward:
            {
                iRemConCoreApiTarget.BackwardResponse( iStatus, KErrNone );
                SetActive();
                break;
            }
            case ERemConCoreApiForward:
            {
                iRemConCoreApiTarget.ForwardResponse( iStatus, KErrNone );
                SetActive();
                break;
            }
            case ERemConCoreApiVolumeUp:
            {
                iRemConCoreApiTarget.VolumeUpResponse( iStatus, KErrNone );
                SetActive();
                break;
            }
            case ERemConCoreApiVolumeDown:
            {
                iRemConCoreApiTarget.VolumeDownResponse( iStatus, KErrNone );
                SetActive();
                break;
            }
            default:
            {
                TInt error = KErrNone;
                iRemConCoreApiTarget.SendResponse(
                    iStatus, aOperationId, error );
                SetActive();
                break;
            }
        }
    }
    // already active. Append to array and complete later.
    else
    {
        iResponseArray.Append( aOperationId );
    }
    TX_EXIT
}

/*!
 \internal
 Implements cancellation of an outstanding request.
 */
void MpMediaKeyRemConResponse::DoCancel()
{
}

/*!
 \internal
 Handles an active object's request completion event.
 */
void MpMediaKeyRemConResponse::RunL()
{
    TX_ENTRY_ARGS( "iStatus=" << iStatus.Int() );
    // if any existing -> Send response
    if ( iResponseArray.Count() ) {
        CompleteAnyKey( iResponseArray[0] );
        // Remove already completed key
        iResponseArray.Remove( 0 );
        iResponseArray.Compress();
    }
    TX_EXIT
}

// End of File
