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
* Description:  Idle state of DTMF sending.
*
*/



// INCLUDE FILES
#include    "CVtEngDtmfState.h"
#include    "VtEngPanic.h"
#include    "MVtEngDtmfHandler.h"
#include    <cvtlogger.h>
#include    <mvtprotocolhandler.h>

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CVtEngDtmfIdle::NewL
// 
// -----------------------------------------------------------------------------
//
CVtEngDtmfState* CVtEngDtmfIdle::NewL( 
    MVtEngDtmfHandler& aDtmfHandler,
    MVtH324ConfigCommand& aH324Config )
    {
    CVtEngDtmfIdle* self = new ( ELeave ) 
        CVtEngDtmfIdle( aDtmfHandler, aH324Config );
    return self;
    }

// -----------------------------------------------------------------------------
// CVtEngDtmfIdle::~CVtEngDtmfIdle
// 
// -----------------------------------------------------------------------------
//
CVtEngDtmfIdle::~CVtEngDtmfIdle()
    {
    }

// -----------------------------------------------------------------------------
// CVtEngDtmfIdle::SendDtmfL
// 
// -----------------------------------------------------------------------------
//
void CVtEngDtmfIdle::SendDtmfL( const TVtEngDtmfTone& aTone )
    {
    __VTPRINTENTER( "DtmfStateIdle.Send" )
    DoSendAndActivateStateL( aTone, MVtEngDtmfHandler::EVtDtmfStateSending );    
    __VTPRINTEXIT( "DtmfStateIdle.Send" )
    }

// -----------------------------------------------------------------------------
// CVtEngDtmfIdle::DtmfSendCompleteL
// 
// -----------------------------------------------------------------------------
//
void CVtEngDtmfIdle::DtmfSendCompleteL( TInt /*aError*/ )
    {
    __VTPRINT( DEBUG_MEDIA , "DtmfStateIdle.SendComplete" )
    // Do nothing (should be never called since state is always
    // "sending" or "buffered" when sending is completed).
    }

// -----------------------------------------------------------------------------
// CVtEngDtmfIdle::CVtEngDtmfIdle
// 
// -----------------------------------------------------------------------------
//
CVtEngDtmfIdle::CVtEngDtmfIdle( 
    MVtEngDtmfHandler& aDtmfHandler,
    MVtH324ConfigCommand& aH324Config ) 
    : CVtEngDtmfState( aDtmfHandler, aH324Config )
    {
    }

//  End of File  
