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
* Description:  CVtCtlCallControl class implementation.
*
*/


#pragma CTC SKIP
// INCLUDE FILES
#include <cvtlogger.h>
#include    "CVtCtlCallControl.h"
#include    "MVtCtlEventObserver.h"
#include    "CVtCtlSessionContainer.h"
#include    "CVtCtlVideoCall.h"
#include    "VtCtlTypes.h"
#include    <cvtlogger.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtCtlCallControl::NewL
// two-phase constructor
// -----------------------------------------------------------------------------
//
CVtCtlCallControl* CVtCtlCallControl::NewL(
            MVtCtlEventObserver& aEventObserver )
    {
    __VTPRINTENTER( "CVtCtlCallControl.NewL" )
    CVtCtlCallControl* self = new ( ELeave )
        CVtCtlCallControl( aEventObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    __VTPRINTEXIT( "CVtCtlCallControl.NewL" )
    return self;
    }
        
// -----------------------------------------------------------------------------
// CVtCtlCallControl::~CVtCtlCallControl
// Destructor
// -----------------------------------------------------------------------------
//
CVtCtlCallControl::~CVtCtlCallControl()
    {
    __VTPRINTENTER( "CVtCtlCallControl.~" )
    delete iContainer;
    // uninitialize logging (if enabled)
    VTLOGUNINIT
    __VTPRINTEXIT( "CVtCtlCallControl.~" )
    }

// -----------------------------------------------------------------------------
// CVtCtlCallControl::CVtCtlCallControl
// c++ constructor
// -----------------------------------------------------------------------------
//
CVtCtlCallControl::CVtCtlCallControl(
    MVtCtlEventObserver& aEventObserver ) :
    iEventObserver( aEventObserver )
    {
    __VTPRINTENTER( "CVtCtlCallControl.CVtCtlCallControl" )
    __VTPRINTEXIT( "CVtCtlCallControl.CVtCtlCallControl" )
    }


// -----------------------------------------------------------------------------
// CVtCtlCallControl::ConstructL
// 
// -----------------------------------------------------------------------------
//
void CVtCtlCallControl::ConstructL()
    {
    __VTPRINTENTER( "CVtCtlCallControl.ConstructL" )
    // initialize logging (if enabled)
    VTLOGINIT
    iContainer = CVtCtlSessionContainer::NewL( *this );
    __VTPRINTEXIT( "CVtCtlCallControl.ConstructL" )
    }

// -----------------------------------------------------------------------------
// CVtCtlCallControl::GetDuration
// 
// -----------------------------------------------------------------------------
//
TInt CVtCtlCallControl::GetDuration(
    TVtCtlSessionId aId,
    TVtCtlDuration& aDuration )     
    {
    __VTPRINTENTER( "CVtCtlCallControl.GetDuration" )
    CVtCtlVideoCall* call = iContainer->FindCall( aId );
    TInt err( KErrNotFound );
    if ( call )
        {
        err = call->GetDuration( aDuration );
        }
    __VTPRINTEXIT( "CVtCtlCallControl.GetDuration" )
    return err;
    }

// -----------------------------------------------------------------------------
// CVtCtlCallControl::GetSessionInfoL
// 
// -----------------------------------------------------------------------------
//
/*****************************************************
*   Series 60 Customer / ETel
*   Series 60  ETel API
*****************************************************/
void CVtCtlCallControl::GetSessionInfoL( 
    TVtCtlSessionId aId,
    TDes8& aInfo )
    {
    __VTPRINTENTER( "CVtCtlCallControl.GetSessionInfoL" )
    ValidateDataExtensionL( aInfo, EVtCtlCSSessionInfoExtensionId );
    CVtCtlVideoCall* call = iContainer->FindCall( aId );
    if ( call )
        {
        TPckg<TVtCtlCSSessionInfoV1>& pckg = 
            reinterpret_cast<TPckg<TVtCtlCSSessionInfoV1>&>( aInfo );
        TVtCtlCSSessionInfoV1& info = pckg();
        TInt err( call->UpdateCallInfo() );
        if ( err == KErrNone )
            {
            const RMobileCall::TMobileCallInfoV1& mobileInfo = call->MobileInfo();
            info.iDuration = mobileInfo.iDuration;
            info.iTelNumber = mobileInfo.iRemoteParty.iRemoteNumber.iTelNumber;
            __VTPRINT2( DEBUG_SESSION, "VtCtl: mobile dir=%d", 
                mobileInfo.iRemoteParty.iDirection  )
            switch ( mobileInfo.iRemoteParty.iDirection )
                {
                case RMobileCall::EMobileOriginated:
                    info.iDirection = EDirectionMobileOriginated;
                    break;
                case RMobileCall::EMobileTerminated:
                    info.iDirection = DirectionMobileTerminated;
                    break;
                case RMobileCall::EDirectionUnknown:
                default:
                    info.iDirection = EDirectionUnknown;
                    break;
                }
            }
        }
    else
        {
        __VTPRINT( DEBUG_SESSION, "CallCtrl.GetSessionInfoL not found" )
        User::Leave( KErrNotFound );
        }
    __VTPRINTEXIT( "CVtCtlCallControl.GetSessionInfoL" )
    }


// -----------------------------------------------------------------------------
// CVtCtlCallControl::GetState
// 
// -----------------------------------------------------------------------------
//
MVtCtlCallControl::TVtCtlState CVtCtlCallControl::GetSessionState( 
    TVtCtlSessionId aId ) const
    {
    __VTPRINTENTER( "CVtCtlCallControl.GetSessionState" )
    __VTPRINT2( DEBUG_SESSION, "CallCtrl.GetSessionState for id:%d",aId )
    MVtCtlCallControl::TVtCtlState state = EUnknown;
    CVtCtlVideoCall* call = iContainer->FindCall( aId );
    if ( call )
        {
        call->GetCallState( state );
        }
    __VTPRINT3( DEBUG_SESSION, " callPtr=%d state=%d", (TInt) call, state )
    __VTPRINTEXIT( "CVtCtlCallControl.GetSessionState" )
    return state;
    }

// -----------------------------------------------------------------------------
// CVtCtlCallControl::GetPreviousState
// 
// -----------------------------------------------------------------------------
//
MVtCtlCallControl::TVtCtlState CVtCtlCallControl::GetPreviousPreviousState(
    TVtCtlSessionId aId ) const
    {
    __VTPRINTENTER( "CVtCtlCallControl.GetPreviousPreviousState" )
    MVtCtlCallControl::TVtCtlState state = EUnknown;
    const CVtCtlVideoCall* call = iContainer->FindCall( aId );
    if ( call )
        {
        state = call->CallInfo().iPreviousStatus;
        }
    __VTPRINTEXIT( "CVtCtlCallControl.GetPreviousPreviousState" )
    return state;
    }

// -----------------------------------------------------------------------------
// CVtCtlCallControl::HandleCallStatusChangeL
// Notifies observer about call status change.
// -----------------------------------------------------------------------------
//
void CVtCtlCallControl::HandleCallStatusChangeL(
    TVtCtlSessionId aId,
    MVtCtlCallControl::TVtCtlState aStatus,
    MVtCtlCallControl::TVtCtlState aPreviousStatus )
    {
    __VTPRINTENTER( "CVtCtlCallControl.HandleCallStatusChangeL" )
    __VTPRINT2( DEBUG_SESSION, " status=%d", (TInt) aStatus )
    
    TVtCtlSessionStateEvent event;
    event.iPreviousState = aPreviousStatus;
    event.iState = aStatus;
    event.iSessionId = aId;

    TAny* parameter = &event;
    iEventObserver.HandleVtSessionEventL(
        KVtCtlEventSessionStatusChanged,
        parameter );
    __VTPRINTEXIT( "CVtCtlCallControl.HandleCallStatusChangeL" )
    }

// -----------------------------------------------------------------------------
// CVtCtlCallControl::ValidateDataExtensionL
// Validates extension id of descriptor representation of data type.
// -----------------------------------------------------------------------------
//
void CVtCtlCallControl::ValidateDataExtensionL(
    TDesC8& aData,
    TInt aExtension )
    {
    __VTPRINTENTER( "CVtCtlCallControl.ValidateDataExtensionL" )
    TVtCtlType type;
    TPckg<TVtCtlType> pckg(type);
    TPtrC8 ptr( aData.Ptr(), sizeof( TVtCtlType ) );
    pckg.Copy( ptr );
    if ( type.Extension() != aExtension )
        {
        __VTPRINT( DEBUG_SESSION, "CallCtrl.Validate not supported" )
        User::Leave( KErrNotSupported );
        }
    __VTPRINTEXIT( "CVtCtlCallControl.ValidateDataExtensionL" )
    }
#pragma CTC ENDSKIP
//  End of File  

