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
* Description:  Implements class CSPClientVoiceCall which provides call functionality
*
*/


#include <etelmm.h>
#include <etel.h>
#include <mccpcallobserver.h>
#include <cccecallparameters.h>

#include "cspclientvoicecall.h"
#include "csplogger.h"
#include "csppanic.pan"
#include "cspconsts.h"
#include "mcspcommoninfo.h"


// ---------------------------------------------------------------------------
// CSPClientVoiceCall::NewL Two phased construction.
// ---------------------------------------------------------------------------
//
CSPClientVoiceCall* CSPClientVoiceCall::NewL( const TDesC& aName, 
                          RMobileLine& aLine,  
                          const CCCECallParameters& aParams,
                          MCSPCommonInfo& aCommonInfo )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPClientVoiceCall::NewL <");
    
    CSPClientVoiceCall* self = new ( ELeave ) CSPClientVoiceCall( aLine,
                                            aName,
                                            aCommonInfo );
    CleanupStack::PushL( self );
    self->ConstructL( aParams );
    CleanupStack::Pop( self );
    CSPLOGSTRING(CSPOBJECT, "CSPClientVoiceCall::NewL >");
    return self;
    }
    
// ---------------------------------------------------------------------------
// CSPClientVoiceCall::~CSPClientVoiceCall
// ---------------------------------------------------------------------------
//
CSPClientVoiceCall::~CSPClientVoiceCall()
    {
    CSPLOGSTRING(CSPOBJECT, "CSPClientVoiceCall::~CSPClientVoiceCall");
    }
  
// ---------------------------------------------------------------------------
// CSPClientVoiceCall::CSPClientVoiceCall
// ---------------------------------------------------------------------------
//
CSPClientVoiceCall::CSPClientVoiceCall( RMobileLine& aLine,
                  const TDesC& aName,
                  MCSPCommonInfo& aCommonInfo ):
                  CSPVoiceCall( aLine,
                     ETrue, aName, 
                     aCommonInfo, EFalse )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPClientVoiceCall::CSPClientVoiceCall");
    
    // Set client call flag(s)
    iClientCallIndicated = EFalse; 
    }

// ---------------------------------------------------------------------------
// CSPClientVoiceCall::ConstructL
// Constructing CSPClientVoiceCall for MT call.
// ---------------------------------------------------------------------------
//    
void CSPClientVoiceCall::ConstructL( const CCCECallParameters& aParams )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPClientVoiceCall::ConstructL <");
    CSPVoiceCall::ConstructL( aParams ); 
    CSPLOGSTRING(CSPOBJECT, "CSPClientVoiceCall::ConstructL >");
    }

// ---------------------------------------------------------------------------
// CSPClientVoiceCall::NotifyCallStateChanged
// Notifies observers about state changes
// ---------------------------------------------------------------------------
//
void CSPClientVoiceCall::NotifyCallStateChanged(
            MCCPCallObserver::TCCPCallState aState ) 
    {
    CSPLOGSTRING2(CSPINT, 
            "CSPClientVoiceCall::NotifyCallStateChanged state: %d", 
            aState);
    
    // Client call is indicated during dialling state 
    if ( aState == MCCPCallObserver::ECCPStateDialling )
        {
        IndicateClientCall(); 
        }
    
    CSPVoiceCall::NotifyCallStateChanged( aState );  
    }
    
// ---------------------------------------------------------------------------
// CSPClientVoiceCall::NotifyCallStateChangedWithInband
// Notifies observers about state changes
// ---------------------------------------------------------------------------
//
void CSPClientVoiceCall::NotifyCallStateChangedWithInband(
            MCCPCallObserver::TCCPCallState aState ) 
    {
    CSPLOGSTRING2(CSPINT, 
            "CSPClientVoiceCall::NotifyCallStateChangedWithInband state: %d", 
            aState);

    // Client call is indicated during dialling state
    if ( aState == MCCPCallObserver::ECCPStateDialling )
        {
        IndicateClientCall(); 
        }

    CSPVoiceCall::NotifyCallStateChangedWithInband( aState );  
    }

// ---------------------------------------------------------------------------
// CSPClientVoiceCall::IndicateClientCall
// Indicate dialling if needed 
// ---------------------------------------------------------------------------
//  
void CSPClientVoiceCall::IndicateClientCall()
    {
    // Can be done only once for each call and possible duplicate 
    // notifies are blocked 
    if ( !iClientCallIndicated )
        {
        iClientCallIndicated = ETrue;

        CSPLOGSTRING(CSPINT, 
            "CSPClientVoiceCall::IndicateClientCall indicate client call")
        
        iCommonInfo.IndicateClientCall( this );
        
        CSPLOGSTRING(CSPINT, 
            "CSPClientVoiceCall::IndicateClientCall IndicateClientCall completed")                
        }
    else 
        {
        CSPLOGSTRING(CSPERROR, 
            "CSPClientVoiceCall::IndicateClientCall already indicated")
        }        
    }

// ---------------------------------------------------------------------------
// CSPClientVoiceCall::OpenCallHandleL
// ---------------------------------------------------------------------------
//    
void CSPClientVoiceCall::OpenCallHandleL()
    {
    CSPLOGSTRING(CSPINT, "CSPClientVoiceCall::OpenCallHandleL <");

    // Client call, open existing call handle 
    OpenExistingCallL( iName ); 
    
    CSPLOGSTRING(CSPINT, "CSPClientVoiceCall::OpenCallHandleL >");
    }

// ---------------------------------------------------------------------------
// CSPClientVoiceCall::UpdateCallInfoImpl
// Update call info including remote party name and number data  
// ---------------------------------------------------------------------------
//    
void CSPClientVoiceCall::UpdateCallInfoImpl( const RMobileCall::TMobileCallInfoV7& aCallInfo )
    {
    CSPLOGSTRING(CSPINT, "CSPClientVoiceCall::UpdateCallInfoImpl <");    

    // Call was added by ETel monitor, update info accordingly 
    UpdateCallNameNumberInfo( aCallInfo, ETrue );     
    UpdateCallOrigin( aCallInfo );
    
    CSPLOGSTRING(CSPINT, "CSPClientVoiceCall::UpdateCallInfoImpl >");
    }

// End of File
