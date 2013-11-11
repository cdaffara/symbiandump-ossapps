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
* Description:  Sending state of DTMF sending.
*
*/



// INCLUDE FILES
#include    "CVtEngDtmfState.h"
#include    "VtEngPanic.h"
#include    <cvtlogger.h>

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CVtEngDtmfSending::NewL
// 
// -----------------------------------------------------------------------------
//
CVtEngDtmfState* CVtEngDtmfSending::NewL( 
    MVtEngDtmfHandler& aDtmfHandler,
    MVtH324ConfigCommand& aH324Config )
    {
    CVtEngDtmfState* self = new ( ELeave ) 
        CVtEngDtmfSending( aDtmfHandler, aH324Config );
    return self;
    }

// -----------------------------------------------------------------------------
// CVtEngDtmfSending::~CVtEngDtmfSending
// 
// -----------------------------------------------------------------------------
//
CVtEngDtmfSending::~CVtEngDtmfSending()
    {
    }

// -----------------------------------------------------------------------------
// CVtEngDtmfSending::SendDtmfL
// 
// -----------------------------------------------------------------------------
//
void CVtEngDtmfSending::SendDtmfL( const TVtEngDtmfTone& aTone )
    {
    __VTPRINTENTER( "DtmfStateSending.Send" )
    CVtEngDtmfState& state = iDtmfHandler.ActivateState( 
        MVtEngDtmfHandler::EVtDtmfStateBuffered );
    // let buffered state handle this
    state.SendDtmfL( aTone );
    __VTPRINTEXIT( "DtmfStateSending.Send" )
    }

// -----------------------------------------------------------------------------
// CVtEngDtmfSending::DtmfSendCompleteL
// 
// -----------------------------------------------------------------------------
//
void CVtEngDtmfSending::DtmfSendCompleteL( TInt aError )
    {
    __VTPRINTENTER( "DtmfStateSending.SendComplete" )
    CVtEngDtmfState::DtmfSendCompleteL( aError );
    // sending complete, change state
    iDtmfHandler.ActivateState( MVtEngDtmfHandler::EVtDtmfStateIdle );
    __VTPRINTEXIT( "DtmfStateSending.SendComplete" )
    }

// -----------------------------------------------------------------------------
// CVtEngDtmfSending::CVtEngDtmfSending
// 
// -----------------------------------------------------------------------------
//
CVtEngDtmfSending::CVtEngDtmfSending( 
    MVtEngDtmfHandler& aDtmfHandler,
    MVtH324ConfigCommand& aH324Config ) 
    :  CVtEngDtmfState( aDtmfHandler, aH324Config )
    {
    }

//  End of File  
