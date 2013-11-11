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
* Description:  Buffered state of DTMF sending.
*
*/



// INCLUDE FILES
#include    "CVtEngDtmfState.h"
#include    "VtEngPanic.h"
#include    <cvtlogger.h>

// CONSTANTS
const TInt KVtEngDtmfToneBufferSize = 2;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngDtmfBuffered::NewL
// 
// -----------------------------------------------------------------------------
//
CVtEngDtmfState* CVtEngDtmfBuffered::NewL( 
    MVtEngDtmfHandler& aDtmfHandler,
    MVtH324ConfigCommand& aH324Config )
    {
    CVtEngDtmfBuffered* self = new ( ELeave ) 
        CVtEngDtmfBuffered( aDtmfHandler, aH324Config );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }

// -----------------------------------------------------------------------------
// CVtEngDtmfBuffered::~CVtEngDtmfBuffered
// 
// -----------------------------------------------------------------------------
//
CVtEngDtmfBuffered::~CVtEngDtmfBuffered()
    {
    delete iTones;
    }

// -----------------------------------------------------------------------------
// CVtEngDtmfBuffered::SendDtmfL
// 
// -----------------------------------------------------------------------------
//
void CVtEngDtmfBuffered::SendDtmfL( const TVtEngDtmfTone& aTone )
    {
    // just add to buffer and wait for completion of pending send operation.
    iTones->AppendL( aTone );
    }

// -----------------------------------------------------------------------------
// CVtEngDtmfBuffered::DtmfSendCompleteL
// 
// PSEUDO CODE:
// deallocate Protocol dtmf instance
// if ( previous_sending_OK )
//     if ( tones_in_buffer > 1 )
//         next_state = buffered
//     else
//         next_state = sending
//     endif
//     send tone from buffer and change state
// endif
// if ( previous_sending_FAIL or new_sending_FAIL )
//     reset buffer 
//     next_state = idle
// endif
//
// -----------------------------------------------------------------------------
//
void CVtEngDtmfBuffered::DtmfSendCompleteL( TInt aError )
    {
    __VTPRINTENTER( "DtmfStateBuffered.DtmfSendCompleteL" )
    CVtEngDtmfState::DtmfSendCompleteL( aError );
    const TInt count( iTones->Count() );
    __VTPRINT2( DEBUG_MEDIA , "DtmfStateBuffered.Send bufSize=%d", count )

    TInt index( KErrNone );
    if ( count && aError == KErrNone )
        {
        __VTPRINT2( DEBUG_MEDIA , "DtmfStateBuffered cnt=%d", count )
        const TVtEngDtmfTone& tone = iTones->At( index );
        MVtEngDtmfHandler::TVtEngDtmfState nextState = ( count > 1 ) ?
            MVtEngDtmfHandler::EVtDtmfStateBuffered :
            MVtEngDtmfHandler::EVtDtmfStateSending;
        TRAP( aError, DoSendAndActivateStateL( tone, nextState ) );
        iTones->Delete( index );
        }
    
    if ( aError != KErrNone )
        {
        __VTPRINT2( DEBUG_MEDIA , "DtmfStateBuffered.failed=%d", aError )
        // if failed clear DTMF buffer and go to dtmf idle state
        iTones->Reset();
        iDtmfHandler.ActivateState( MVtEngDtmfHandler::EVtDtmfStateIdle );
        }
    __VTPRINTEXIT( "DtmfStateBuffered.DtmfSendCompleteL" )
    }


// -----------------------------------------------------------------------------
// CVtEngDtmfBuffered::CVtEngDtmfBuffered
// 
// -----------------------------------------------------------------------------
//
CVtEngDtmfBuffered::CVtEngDtmfBuffered( 
    MVtEngDtmfHandler& aDtmfHandler,
    MVtH324ConfigCommand& aH324Config ) 
    : CVtEngDtmfState( aDtmfHandler, aH324Config)
    {
    }

// -----------------------------------------------------------------------------
// CVtEngDtmfBuffered::ConstructL
// 
// -----------------------------------------------------------------------------
//
void CVtEngDtmfBuffered::ConstructL()
    {
    iTones = new ( ELeave ) 
        CArrayFixFlat<TVtEngDtmfTone>( KVtEngDtmfToneBufferSize );
    }

//  End of File  
