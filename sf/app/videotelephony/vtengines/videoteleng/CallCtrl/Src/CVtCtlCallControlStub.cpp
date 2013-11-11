/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Stub version of call control.
*
*/


#pragma CTC SKIP
// INCLUDE FILES
#include    "CVtCtlCallControl.h"
#include    "MVtCtlEventObserver.h"
#include    "CVtCtlSessionContainer.h"
#include    "CVtCtlVideoCall.h"
#include    "VtCtlTypes.h"
#include    <CVtLogger.h>

// CONSTANTS
_LIT( KVtCtlStubTelNumber, "0501234567" );

// Duration
const TInt KVtCtlStubDuration = 5;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtCtlCallControl::NewL
// Two-phase constructor.
// -----------------------------------------------------------------------------
//
CVtCtlCallControl* CVtCtlCallControl::NewL(
            MVtCtlEventObserver& aEventObserver )
    {
    __VTPRINTENTER( "CVtCtlCallControlStub.NewL" )
    CVtCtlCallControl* self = new ( ELeave )
        CVtCtlCallControl( aEventObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    __VTPRINTEXIT( "CVtCtlCallControlStub.NewL" )
    return self;
    }
        
// -----------------------------------------------------------------------------
// CVtCtlCallControl::~CVtCtlCallControl
// Destructor
// -----------------------------------------------------------------------------
//
CVtCtlCallControl::~CVtCtlCallControl()
    {
    __VTPRINTENTER( "CVtCtlCallControlStub.~" )
    delete iContainer;
    // uninitialize logging
    VTLOGUNINIT
    __VTPRINTEXIT( "CVtCtlCallControlStub.~" )
    }

// -----------------------------------------------------------------------------
// CVtCtlCallControl::CVtCtlCallControl
// c++ constructor
// -----------------------------------------------------------------------------
//
CVtCtlCallControl::CVtCtlCallControl(
    MVtCtlEventObserver& aEventObserver) :
    iEventObserver( aEventObserver )
    {
    __VTPRINTENTER( "CVtCtlCallControlStub.CVtCtlCallControl" )
    __VTPRINTEXIT( "CVtCtlCallControlStub.CVtCtlCallControl" )
    }


// -----------------------------------------------------------------------------
// CVtCtlCallControl::ConstructL
// 
// -----------------------------------------------------------------------------
//
void CVtCtlCallControl::ConstructL()
    {
    __VTPRINTENTER( "CVtCtlCallControlStub.ConstructL" )
    // initialize logging
    VTLOGINIT
    iContainer = CVtCtlSessionContainer::NewL( *this );
    __VTPRINTEXIT( "CVtCtlCallControlStub.ConstructL" )
    }

// -----------------------------------------------------------------------------
// CVtCtlCallControl::GetDuration
// 
// -----------------------------------------------------------------------------
//
TInt CVtCtlCallControl::GetDuration(
    TVtCtlSessionId ,
    TVtCtlDuration& aDuration )     
    {
    __VTPRINTENTER( "CVtCtlCallControlStub.GetDuration" )
    aDuration = 5;
    __VTPRINTEXIT( "CVtCtlCallControlStub.GetDuration" )
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVtCtlCallControl::GetSessionInfoL
// 
// -----------------------------------------------------------------------------
//
void CVtCtlCallControl::GetSessionInfoL( 
    TVtCtlSessionId ,
    TDes8& aInfo )
    {
    __VTPRINTENTER( "CVtCtlCallControlStub.GetSessionInfoL" )
    
    TPckg<TVtCtlCSSessionInfoV1>& pckg = 
        reinterpret_cast<TPckg<TVtCtlCSSessionInfoV1>&>( aInfo );
    __VTPRINT( DEBUG_SESSION, "VtCtl:CallCtrl. 1" )
    TVtCtlCSSessionInfoV1& info = pckg();
    __VTPRINT( DEBUG_SESSION, "VtCtl:CallCtrl. 2" )
    info.iDirection = MVtCtlCallControl::EDirectionMobileOriginated;
    info.iDuration = KVtCtlStubDuration;
    info.iState = MVtCtlCallControl::EConnected;
    info.iPreviousState = MVtCtlCallControl::EAlerting;
    info.iTelNumber = KVtCtlStubTelNumber;
    __VTPRINTEXIT( "CVtCtlCallControlStub.GetSessionInfoL" )
    }


// -----------------------------------------------------------------------------
// CVtCtlCallControl::GetState
// 
// -----------------------------------------------------------------------------
//
MVtCtlCallControl::TVtCtlState CVtCtlCallControl::GetSessionState( 
    TVtCtlSessionId  ) const
    {
    __VTPRINTENTER( "CVtCtlCallControlStub.GetSessionState" )
    MVtCtlCallControl::TVtCtlState state = MVtCtlCallControl::EConnected;    
    __VTPRINTEXIT( "CVtCtlCallControlStub.GetSessionState" )
    return state;
    }

// -----------------------------------------------------------------------------
// CVtCtlCallControl::GetPreviousState
// 
// -----------------------------------------------------------------------------
//
MVtCtlCallControl::TVtCtlState CVtCtlCallControl::GetPreviousPreviousState(
    TVtCtlSessionId  ) const
    {
    __VTPRINTENTER( "CVtCtlCallControlStub.GetPreviousPreviousState" )
    MVtCtlCallControl::TVtCtlState state = EAlerting;
    __VTPRINTEXIT( "CVtCtlCallControlStub.GetPreviousPreviousState" )
    return state;
    }


// -----------------------------------------------------------------------------
// CVtCtlCallControl::HandleCallStatusChangeL
// Notifies
// -----------------------------------------------------------------------------
//
void CVtCtlCallControl::HandleCallStatusChangeL(
    TVtCtlSessionId ,
    MVtCtlCallControl::TVtCtlState ,
    MVtCtlCallControl::TVtCtlState )
    {
    __VTPRINTENTER( "CVtCtlCallControlStub.HandleCallStatusChangeL" )
    __VTPRINTEXIT( "CVtCtlCallControlStub.HandleCallStatusChangeL" )
    }

// -----------------------------------------------------------------------------
// CVtCtlCallControl::ValidateDataExtensionL
// Validates extension id of descriptor representation of data type.
// -----------------------------------------------------------------------------
//
void CVtCtlCallControl::ValidateDataExtensionL(
            TDesC8& ,
            TInt  )
    {
    __VTPRINTENTER( "CVtCtlCallControlStub.ValidateDataExtensionL" )
    __VTPRINTEXIT( "CVtCtlCallControlStub.ValidateDataExtensionL" )
    }
#pragma CTC ENDSKIP
//  End of File  

