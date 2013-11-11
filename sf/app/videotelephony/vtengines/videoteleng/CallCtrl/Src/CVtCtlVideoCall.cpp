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
* Description:  Video call abstraction implementation.
*
*/


#pragma CTC SKIP
// INCLUDE FILES
#include    "CVtCtlVideoCall.h"
#include    "CVtCtlCallStatusMonitor.h"
#include    "VtCtlPanic.h"
#include    "cvtlogger.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtCtlVideoCall::CVtCtlVideoCall
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
/*****************************************************
*   Series 60 Customer / ETel
*   Series 60  ETel API
*****************************************************/
CVtCtlVideoCall::CVtCtlVideoCall( 
    RMobileLine& aDataLine,
    TVtCtlSessionId aId,
    MVtCtlCallStatusObserver& aObserver ) 
    : iLine( aDataLine ), iObserver( aObserver )
    {
    __VTPRINTENTER( "CVtCtlVideoCall.CVtCtlVideoCall" )
    iCallInfo.iId = aId;
    __VTPRINTEXIT( "CVtCtlVideoCall.CVtCtlVideoCall" )
    }

// -----------------------------------------------------------------------------
// CVtCtlVideoCall::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
/*****************************************************
*   Series 60 Customer / ETel
*   Series 60  ETel API
*****************************************************/
CVtCtlVideoCall* CVtCtlVideoCall::NewL( 
    RMobileLine& aDataLine,
    TVtCtlSessionId aId,
    MVtCtlCallStatusObserver& aObserver )
    {
    __VTPRINTENTER( "CVtCtlVideoCall.NewL" )
    CVtCtlVideoCall* self = new( ELeave ) 
        CVtCtlVideoCall( aDataLine, aId, aObserver );
    __VTPRINTEXIT( "CVtCtlVideoCall.NewL" )
    return self;
    }

    
// Destructor
CVtCtlVideoCall::~CVtCtlVideoCall()
    {
    __VTPRINTENTER( "CVtCtlVideoCall.~" )
    __VTPRINT( DEBUG_SESSION | DEBUG_DESTRUCT, "VtCtl:Call~")
    delete iStatusMonitor;
    if ( iCall )
    	{
    	iCall->Close();
    	delete iCall;
    	}
    __VTPRINTEXIT( "CVtCtlVideoCall.~" )
    }

// -----------------------------------------------------------------------------
// CVtCtlVideoCall::CallInfo
// Returns call information containing information not associated to ETel
// -----------------------------------------------------------------------------
//
const CVtCtlVideoCall::TVtCtlCallInfo& CVtCtlVideoCall::CallInfo() const
    {
    return iCallInfo;
    }

// -----------------------------------------------------------------------------
// CVtCtlVideoCall::MobileInfo
// Returns ETel call information.
// -----------------------------------------------------------------------------
//
/*****************************************************
*   Series 60 Customer / ETel
*   Series 60  ETel API
*****************************************************/
const RMobileCall::TMobileCallInfoV1& CVtCtlVideoCall::MobileInfo() const
    {
    return iMobileCallInfo;
    }

// -----------------------------------------------------------------------------
// CVtCtlVideoCall::InitializeCallL
// Opens call subsession and starts status monitor.
// -----------------------------------------------------------------------------
//
/*****************************************************
*   Series 60 Customer / ETel
*   Series 60  ETel API
*****************************************************/
void CVtCtlVideoCall::InitializeCallL( 
    RMobileCall& aCall )
    {
    __VTPRINTENTER( "CVtCtlVideoCall.InitializeCallL" )
    __ASSERT_ALWAYS( iLine.SubSessionHandle(), 
        Panic( EVtCtlPanicLineHandleNull ) );
    iCall = &aCall;
    iStatusMonitor = new ( ELeave ) CVtCtlCallStatusMonitor( *iCall, *this );
    iStatusMonitor->Start();
    UpdateCallInfo();
    iCallInfo.iPreviousStatus = MVtCtlCallControl::EIdle;
    iCallInfo.iStatus = 
        CVtCtlCallStatusMonitor::MapState( iMobileCallInfo.iStatus );

    __VTPRINTEXIT( "CVtCtlVideoCall.InitializeCallL" )
    }

// -----------------------------------------------------------------------------
// CVtCtlVideoCall::UpdateCallInfo
// Updates call information from ETel
// -----------------------------------------------------------------------------
//
/*****************************************************
*   Series 60 Customer / ETel
*   Series 60  ETel API
*****************************************************/
TInt CVtCtlVideoCall::UpdateCallInfo()
    {
    __VTPRINTENTER( "CVtCtlVideoCall.UpdateCallInfo" )
    __ASSERT_ALWAYS( iCall->SubSessionHandle(), 
        Panic( EVtCtlPanicCallHandleNull ) );
        
    RMobileCall::TMobileCallInfoV1Pckg pckg( iMobileCallInfo );
    const TInt err( iCall->GetMobileCallInfo( pckg ) );
    __VTPRINT2( DEBUG_SESSION, "VtCtl:Call. dir %d",
        iMobileCallInfo.iRemoteParty.iDirection )
    __VTPRINT2( DEBUG_SESSION, "VtCtl:Call. status %d",
        iMobileCallInfo.iStatus )
#ifdef VTDEBUG
    RDebug::Print(_L(" VtCtl.RemoteNumber=%S"),
        &iMobileCallInfo.iRemoteParty.iRemoteNumber.iTelNumber );
#endif
    __VTPRINTEXITR( "CVtCtlVideoCall.UpdateCallInfo err=%d", err )
    return err;
    }

// -----------------------------------------------------------------------------
// CVtCtlVideoCall::GetDuration
// Returns call duration
// -----------------------------------------------------------------------------
//
TInt CVtCtlVideoCall::GetDuration( TVtCtlDuration& aDuration )
    {
    __VTPRINTENTER( "CVtCtlVideoCall.GetDuration" )
    const TInt res( UpdateCallInfo() );
    if ( res == KErrNone )
        {
        aDuration = iMobileCallInfo.iDuration;
        }
    __VTPRINTEXITR( "CVtCtlVideoCall.GetDuration err=%d", res )
    return res;
    }

// -----------------------------------------------------------------------------
// CVtCtlVideoCall::GetCallState
// Gets call state
// -----------------------------------------------------------------------------
//
TInt CVtCtlVideoCall::GetCallState( 
    MVtCtlCallControl::TVtCtlState& aState )
    {
    __VTPRINTENTER( "CVtCtlVideoCall.GetCallState" )
    RMobileCall::TMobileCallStatus status;
    aState = iCallInfo.iStatus;    
    const TInt res( iCall->GetMobileCallStatus( status ) );
    
    // update internal state only if differs from previous state
    if ( res == KErrNone &&
         CVtCtlCallStatusMonitor::IsLegalNewState( 
            status, iMobileCallInfo.iStatus ) )
        {
        // update 1) internal values, and
        iMobileCallInfo.iStatus = status;        
        iCallInfo.iPreviousStatus = iCallInfo.iStatus;
        iCallInfo.iStatus = CVtCtlCallStatusMonitor::MapState( status );

        // 2) return value
        aState = iCallInfo.iStatus;                    
        }
    __VTPRINTEXITR( "CVtCtlVideoCall.GetCallState err=%d", res )
    return res;
    }

// -----------------------------------------------------------------------------
// CVtCtlVideoCall::HandleCallStatusChangeL
// Handles status change
// -----------------------------------------------------------------------------
//
void CVtCtlVideoCall::HandleCallStatusChangeL( 
    TVtCtlSessionId ,
    MVtCtlCallControl::TVtCtlState aStatus,
    MVtCtlCallControl::TVtCtlState aPreviousStatus )
    {
    __VTPRINTENTER( "CVtCtlVideoCall.HandleCallStatusChangeL" )
    // If monitor has received outdated notification ignore the event.
    // This may happen if call status has asked from ETel and there
    // are still old completed status notifications that are not handled.
    // => contradition between internal status in this object and one 
    //    received from ststus monitor. Ignore it.
    if ( aStatus > iCallInfo.iStatus || aStatus == MVtCtlCallControl::EIdle )
        {
        iCallInfo.iPreviousStatus = iCallInfo.iStatus;
        iCallInfo.iStatus = aStatus;

        iObserver.HandleCallStatusChangeL( 
            iCallInfo.iId, 
            aStatus,
            aPreviousStatus );
        }
    __VTPRINTEXIT( "CVtCtlVideoCall.HandleCallStatusChangeL" )
    }
#pragma CTC ENDSKIP
//  End of File  

