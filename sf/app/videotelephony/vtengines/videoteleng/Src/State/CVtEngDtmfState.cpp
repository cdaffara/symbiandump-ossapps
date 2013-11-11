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
* Description:  Base state class of DTMF sending.
*
*/



// INCLUDE FILES
#include    "CVtEngDtmfState.h"
#include    "MVtEngDtmfHandler.h"
#include    <cvtlogger.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngDtmfState::CVtEngDtmfState
// 
// -----------------------------------------------------------------------------
//
CVtEngDtmfState::CVtEngDtmfState( 
    MVtEngDtmfHandler& aDtmfHandler,
    MVtH324ConfigCommand& aH324Config ) :
    iDtmfHandler( aDtmfHandler ),
    iH324Config( aH324Config )
    {
    }

// -----------------------------------------------------------------------------
// CVtEngDtmfState::DtmfSendCompleteL
// 
// -----------------------------------------------------------------------------
//
void CVtEngDtmfState::DtmfSendCompleteL( TInt /*aError*/ )
    {
    __VTPRINTENTER( "DtmfState.DtmfSendCompleteL" )
    iDtmfHandler.ReleaseProtoDtmf();
    __VTPRINTEXIT( "DtmfState.DtmfSendCompleteL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDtmfState::DoSendAndActivateStateL
//
// Requests new Protocol dtmf tone instance from the DTMF handler, sends it and
// changes state. 
//
// -----------------------------------------------------------------------------
//
void CVtEngDtmfState::DoSendAndActivateStateL(
    const TVtEngDtmfTone& aDtmf,
    MVtEngDtmfHandler::TVtEngDtmfState aNewState )
    {
    __VTPRINTENTER( "DtmfState.DoSendAndActivateStateL" )
    MVTUserInput& protoDtmf = iDtmfHandler.ReserveProtoDtmfL( aDtmf );
    iDtmfHandler.StoreCommandId( iH324Config.SendUserInputMessageL( protoDtmf ));
    iDtmfHandler.ActivateState( aNewState );
    __VTPRINTEXIT( "DtmfState.DoSendAndActivateStateL" )
    }

//  End of File
