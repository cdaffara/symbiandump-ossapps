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
* Description:  Implements class CSPClientVideoCall which provides client video call 
*               functionality
*
*/


#include <etelmm.h>
#include <etel.h>
#include <mccpcallobserver.h>
#include <rmmcustomapi.h>

#include "cspclientvideocall.h"
#include "csplogger.h"
#include "csppanic.pan"
#include "cspconsts.h"
#include "mcspcommoninfo.h"


// ---------------------------------------------------------------------------
// CSPClientVideoCall::NewL Two phased construction.
// ---------------------------------------------------------------------------
//
CSPClientVideoCall* CSPClientVideoCall::NewL( const TDesC& aName, 
                          RMobileLine& aLine,  
                          const CCCECallParameters& aParams,
                          MCSPCommonInfo& aCommonInfo )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPClientVideoCall::NewL <<");
    
    CSPClientVideoCall* self = new (ELeave) CSPClientVideoCall( aLine, 
                                            aName, aCommonInfo );
    CleanupStack::PushL( self );    
    self->ConstructL( aParams );
    CleanupStack::Pop( self );
    CSPLOGSTRING(CSPOBJECT, "CSPClientVideoCall::NewL >>");
    return self;
    }
    
// ---------------------------------------------------------------------------
// CSPClientVideoCall::~CSPClientVideoCall
// ---------------------------------------------------------------------------
//
CSPClientVideoCall::~CSPClientVideoCall()
    {
    CSPLOGSTRING(CSPOBJECT, "CSPClientVideoCall::~CSPClientVideoCall");
    }

// ---------------------------------------------------------------------------
// CSPClientVideoCall::NotifyCallStateChanged
// Notifies observers about state changes
// ---------------------------------------------------------------------------
//
void CSPClientVideoCall::NotifyCallStateChanged(
            MCCPCallObserver::TCCPCallState aState ) 
    {
    CSPLOGSTRING2(CSPINT, 
            "CSPClientVideoCall::NotifyCallStateChanged state: %d", 
            aState);
    
    // Client call type is not valid before Dialling state
    if ( aState == MCCPCallObserver::ECCPStateDialling )
        {
        CSPLOGSTRING(CSPOBJECT, "CSPClientVideoCall::NotifyCallStateChanged\
             updating call type");
        
        CCPCall::TCallType callType = SelectCallTypeFromProtocolCaps(); 
        iParams->SetCallType( callType );
        
        // Indicate client call to observer in Dialling state
        IndicateClientCall();     
        }

    CSPVideoCall::NotifyCallStateChanged( aState );  
    }
    
// ---------------------------------------------------------------------------
// CSPClientVideoCall::NotifyCallStateChangedWithInband
// Notifies observers about state changes
// ---------------------------------------------------------------------------
//
void CSPClientVideoCall::NotifyCallStateChangedWithInband(
            MCCPCallObserver::TCCPCallState aState ) 
    {
    CSPLOGSTRING2(CSPINT, 
        "CSPClientVideoCall::NotifyCallStateChangedWithInband %d", aState);
    
    // Client call type info is not available before Dialling phase
    if ( aState == MCCPCallObserver::ECCPStateDialling )
        {
        CSPLOGSTRING(CSPOBJECT, "CSPClientVideoCall::NotifyCallStateChanged\
             updating call type");
        
        CCPCall::TCallType callType = SelectCallTypeFromProtocolCaps(); 
        iParams->SetCallType( callType );

        // Indicate client call to observer in Dialling state
        IndicateClientCall(); 
        }
    
    CSPVideoCall::NotifyCallStateChangedWithInband( aState );  
    }

// ---------------------------------------------------------------------------
// CSPClientVideoCall::CSPClientVideoCall
// ---------------------------------------------------------------------------
//
CSPClientVideoCall::CSPClientVideoCall( RMobileLine& aLine,
                  const TDesC& aName,
                  MCSPCommonInfo& aCommonInfo ) : 
                     CSPVideoCall( aLine, ETrue, aName, 
                                   aCommonInfo )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPClientVideoCall::CSPClientVideoCall");
    // Set client call flag(s)
    iClientCallIndicated = EFalse; 
    }

// ---------------------------------------------------------------------------
// CSPClientVideoCall::ConstructL
// Constructing CSPClientVideoCall for MT call.
// ---------------------------------------------------------------------------
//    
void CSPClientVideoCall::ConstructL( const CCCECallParameters& aParams )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPClientVideoCall::ConstructL <");
    CSPVideoCall::ConstructL( aParams );
    CSPLOGSTRING(CSPOBJECT, "CSPClientVideoCall::ConstructL >");
    }

// ---------------------------------------------------------------------------
// CSPClientVideoCall::IndicateClientCall
// Indicate dialling if needed 
// ---------------------------------------------------------------------------
//  
void CSPClientVideoCall::IndicateClientCall()
    {
    // Can be done only once for each call and possible duplicate 
    // notifies are blocked 
    if ( !iClientCallIndicated )
        {
        iClientCallIndicated = ETrue;

        CSPLOGSTRING(CSPINT, 
            "CSPClientVideoCall::IndicateClientCall IndicateClientCall")
        
        iCommonInfo.IndicateClientCall( this );
        
        CSPLOGSTRING(CSPINT, 
            "CSPClientVideoCall::IndicateClientCall IndicateClientCall completed")                
        }    
    else 
        {
        CSPLOGSTRING(CSPERROR, 
            "CSPClientVideoCall::IndicateClientCall already indicated")
        }        
    }

// ---------------------------------------------------------------------------
// CSPClientVideoCall::OpenCallHandleL
// ---------------------------------------------------------------------------
//    
void CSPClientVideoCall::OpenCallHandleL()
    {
    CSPLOGSTRING(CSPINT, "CSPClientVideoCall::OpenCallHandleL <");

    // Client call, open existing call handle 
    OpenExistingCallL( iName ); 
    
    CSPLOGSTRING(CSPINT, "CSPClientVideoCall::OpenCallHandleL >");
    }

// ---------------------------------------------------------------------------
// CSPClientVideoCall::UpdateCallInfoImpl
// Update call info including remote party name and number data  
// ---------------------------------------------------------------------------
//    
void CSPClientVideoCall::UpdateCallInfoImpl( const RMobileCall::TMobileCallInfoV7& aCallInfo )
    {
    CSPLOGSTRING(CSPINT, "CSPClientVideoCall::UpdateCallInfoImpl <");
    
    // Call was added by ETel monitor, update info accordingly 
    UpdateCallNameNumberInfo( aCallInfo, ETrue );     
    UpdateCallOrigin( aCallInfo );
    
    CSPLOGSTRING(CSPINT, "CSPClientVideoCall::UpdateCallInfoImpl >");
    }

// End of File
