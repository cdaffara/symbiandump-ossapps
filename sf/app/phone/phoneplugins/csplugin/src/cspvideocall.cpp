/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implements class CSPVideoCall which provides video call 
*                functionality
*
*/


#include <etelmm.h>
#include <etel.h>
#include <mccpcallobserver.h>
#include <rmmcustomapi.h>

#include "cspvideocall.h"
#include "cspetelcallrequester.h"
#include "cspetelcalleventmonitor.h"
#include "cspetelcallstatusmonitor.h"
#include "cspetelvideocallcapsmonitor.h"
#include "csplogger.h"
#include "csppanic.pan"
#include "cspconsts.h"
#include "mcspcommoninfo.h"


// ---------------------------------------------------------------------------
// CSPVideoCall::NewL Two phased construction.
// ---------------------------------------------------------------------------
//
CSPVideoCall* CSPVideoCall::NewL( const TDesC& aName, 
                          RMobileLine& aLine, TBool aMobileOriginated, 
                          const CCCECallParameters& aParams,
                          MCSPCommonInfo& aCommonInfo )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPVideoCall::NewL <");
    
    CSPVideoCall* self = new (ELeave) CSPVideoCall( aLine, aMobileOriginated,
                                            aName, aCommonInfo );
    CleanupStack::PushL( self );    
    self->ConstructL(  aParams );
    CleanupStack::Pop( self );
    CSPLOGSTRING(CSPOBJECT, "CSPVideoCall::NewL >");
    return self;
    }
    
// ---------------------------------------------------------------------------
// CSPVideoCall::~CSPVideoCall
// ---------------------------------------------------------------------------
//
CSPVideoCall::~CSPVideoCall()
    {
    CSPLOGSTRING(CSPOBJECT, "CSPVideoCall::~CSPVideoCall <");
    // Stop monitoring events    
    delete iConnectionHandler;
    delete iVideoCallCapsMonitor;

    CSPLOGSTRING(CSPOBJECT, "CSPVideoCall::~CSPVideoCall >");
    }

// ---------------------------------------------------------------------------
// CSPVideoCall::NotifyCallStateChanged
// Notifies observers about state changes
// ---------------------------------------------------------------------------
//
void CSPVideoCall::NotifyCallStateChanged(
            MCCPCallObserver::TCCPCallState aState ) 
    {
    CSPLOGSTRING2(CSPINT, "CSPVideoCall::NotifyCallStateChanged %d", aState);

    TInt errorCode( KErrNone );
    iNextCallState = aState;

    if ( aState == MCCPCallObserver::ECCPStateConnected &&
            iParams->CallType() == CCPCall::ECallTypeVideo  &&  
             !iConnectionHandler->ConnectionInitialized() )
        {
        // When connected message is received, call object must first open
        // the socket connection and loan data port
        CSPLOGSTRING(CSPINT, 
            "CSPVideoCall::NotifyCallStateChanged initializing connection");
        errorCode = iConnectionHandler->InitializeConnection();
        
        __ASSERT_ALWAYS( errorCode == KErrNone, 
                         Panic( ECSPPanicDataConnectionInitError ) );
        }
    else if ( aState == MCCPCallObserver::ECCPStateIdle && 
            iParams->CallType() == CCPCall::ECallTypeVideo )
        {
        // When idle message is received one can close the 
        // socket connections.
        CSPLOGSTRING(CSPINT,
            "CSPVideoCall::NotifyCallStateChanged releasing connection");
        iConnectionHandler->ReleaseConnection();
        }
    
    // Connected notify is not sent for video call before connection has    
    // been initialized.  
    if ( aState == MCCPCallObserver::ECCPStateConnected &&
         iParams->CallType() == CCPCall::ECallTypeVideo &&    
         !iConnectionHandler->ConnectionInitialized() ) 
        {
        CSPLOGSTRING(CSPINT, 
            "CSPVideoCall::NotifyCallStateChanged connection not initialized");
        }
    else 
        {
        CSPLOGSTRING(CSPINT, 
            "CSPVideoCall::NotifyCallStateChanged notifying state change");
        CSPCall::NotifyCallStateChanged( aState );
        }

    }
    
// ---------------------------------------------------------------------------
// CSPVideoCall::NotifyCallStateChangedWithInband
// Notifies observers about state changes
// ---------------------------------------------------------------------------
//
void CSPVideoCall::NotifyCallStateChangedWithInband(
            MCCPCallObserver::TCCPCallState aState ) 
    {
    CSPLOGSTRING2(CSPINT, 
        "CSPVideoCall::NotifyCallStateChangedWithInband %d", aState);
    iNextCallState = aState;

    CSPCall::NotifyCallStateChangedWithInband( aState );        
    }

// ---------------------------------------------------------------------------
// CSPVideoCall::ConnectionReady
// ---------------------------------------------------------------------------
//
void CSPVideoCall::ConnectionReady( TName& aDataPortName )
    {
    CSPLOGSTRING3(CSPINT, "CSPVideoCall::ConnectionReady \
        state = %d, next state: %d", iCallState, iNextCallState);
        
    // If still connecting 
    if ( iNextCallState == MCCPCallObserver::ECCPStateConnected )
        {
        NotifyCallEventOccurred(  MCCPCallObserver::ECCPCSDataPortLoaned );        
        NotifyCallStateChanged( iNextCallState );
        iCommonInfo.NotifyDataPortName( aDataPortName );
        }    
    }

// ---------------------------------------------------------------------------
// CSPVideoCall::ConnectionEstablishingFailed
// ---------------------------------------------------------------------------
//    
void CSPVideoCall::ConnectionEstablishingFailed(TInt /*aErr*/)
    {
    CSPLOGSTRING(CSPERROR, "CSPVideoCall::ConnectionEstablishingFailed");
    CSPCall::NotifyErrorOccurred( ECCPErrorVideoCallSetupFailed );
    }

// ---------------------------------------------------------------------------
// CSPVideoCall::VideoCallCapsChanged
// Receives data call caps changed notification from Etel data call caps monitor 
// ---------------------------------------------------------------------------
//       
void CSPVideoCall::DataCallCapsChanged( RMobileCall::TMobileCallDataCapsV1& aCaps )
    {
    CSPLOGSTRING(CSPINT, "CSPVideoCall::DataCallCapsChanged");
    if ( aCaps.iProtocolCaps == RMobileCall::KCapsIsdnMultimedia )
        {
        CSPLOGSTRING(CSPINT, "CSPVideoCall::DataCallCapsChanged type=video");
        iParams->SetCallType( CCPCall::ECallTypeVideo );
        }
    else
        { 
        CSPLOGSTRING(CSPINT, "CSPVideoCall::DataCallCapsChanged type=voice");
        iParams->SetCallType( CCPCall::ECallTypeCSVoice  );
        }
    }
 
// ---------------------------------------------------------------------------
// Branches dial requests according to the flag.
// ---------------------------------------------------------------------------
//
TInt CSPVideoCall::DialFdnCond( TBool aFdnCheck )
    {
    CSPLOGSTRING(CSPREQIN, "CSPVideoCall::DialFdnCond <");
    TInt err(KErrNone);

    SetDefaultCallParameters();
    
    // Update internal call params
    iCommonInfo.InitializeDataCallParameters( iDataCallParams );

    // Set the call state to dialling    
    if ( iCall.SubSessionHandle() )
        {        
        CSPLOGSTRING(CSPINT, "CSPVideoCall::DialFdnCond make request");
        if ( aFdnCheck )
            {
            err = iRequester->MakeDialRequest( 
                                iDataCallParamsPckg, iRemotePartyNumber );
            }
        else
            {
            err = iRequester->MakeDialNoFdnCheckRequest( 
                                iDataCallParamsPckg, iRemotePartyNumber );
            }

        if ( err != KErrNone )
            {
            CSPLOGSTRING2(CSPERROR, 
                "CSPVideoCall::DialFdnCond ERROR with Dial request: %d", err );
            }
        } 
    else
        {
        CSPLOGSTRING2(CSPERROR, "CSPVideoCall::DialFdnCond() ERROR with iCall session: %d", &iCall );
        err = KErrGeneral;
        }
    
    CSPLOGSTRING(CSPINT, "CSPVideoCall::DialFdnCond() >");
    return err;
    }

// ---------------------------------------------------------------------------
// From class MCCPCSCall
// CSPVideoCall::SwitchAlternatingCall
// ---------------------------------------------------------------------------
//
TInt CSPVideoCall::SwitchAlternatingCall()
    {
    CSPLOGSTRING(CSPREQIN, "CSPVideoCall::GetMobileCallInfo");
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// From class MCCPCSCall
// CSPVideoCall::GetMobileDataCallCaps
// ---------------------------------------------------------------------------
//
TInt CSPVideoCall::GetMobileDataCallCaps( TDes8& aCaps ) const
    {
    CSPLOGSTRING(CSPREQIN, "CSPVideoCall::GetMobileDataCallCaps");
    return iVideoCallCapsMonitor->GetMobileDataCallCaps( aCaps );
    }

// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPVideoCall::IsSecured
// ---------------------------------------------------------------------------
//
TBool CSPVideoCall::IsSecured() const
    {
    CSPLOGSTRING(CSPREQIN, "CSPVideoCall::IsSecured");
    return CSPCall::IsSecured();
    }

// ---------------------------------------------------------------------------
// CSPVideoCall::CSPVideoCall
// ---------------------------------------------------------------------------
//
CSPVideoCall::CSPVideoCall( RMobileLine& aLine, 
                  TBool aMobileOriginated,
                  const TDesC& aName,
                  MCSPCommonInfo& aCommonInfo ) : 
                     CSPCall( aLine, aMobileOriginated, aName,
                              aCommonInfo, EFalse ),
                     iDataCallParams(),
                     iDataCallParamsPckg (iDataCallParams)
                     
    {
    CSPLOGSTRING(CSPOBJECT, "CSPVideoCall::CSPVideoCall");
    }

// ---------------------------------------------------------------------------
// CSPVideoCall::ConstructL
// Constructing CSPVideoCall for MT call.
// ---------------------------------------------------------------------------
//    
void CSPVideoCall::ConstructL( const CCCECallParameters& aParams )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPVideoCall::ConstructL <");

    CSPCall::ConstructL( aParams );
    
    iConnectionHandler = CSPVideoCallConnectionHandler::NewL( *this, iCall );    
    iVideoCallCapsMonitor = CSPEtelVideoCallCapsMonitor::NewL( *this, iCall );
    iVideoCallCapsMonitor->StartMonitoring();
    
    // Update call type info only for MT call. 
    if ( !iMobileOriginated ) 
        {
        CSPLOGSTRING(CSPOBJECT, "CSPVideoCall::ConstructL() selecting call type for MT call");
        CCPCall::TCallType callType = SelectCallTypeFromProtocolCaps(); 
        iParams->SetCallType( callType );
        }    
        
    // Filter out illegal requests    
     if ( !(CCPCall::ECallTypeVideo == iParams->CallType()) )
        {
        CSPLOGSTRING(CSPERROR, "CSPVideoCall::ConstructL() error invalid call type");
        User::Leave( KErrArgument );
        }

    CSPLOGSTRING(CSPOBJECT, "CSPVideoCall::ConstructL >");
    }

// -----------------------------------------------------------------------------
// Set default call parameters.
// -----------------------------------------------------------------------------
//
void CSPVideoCall::SetDefaultCallParameters() 
    {
    // Set common call default parameters
    iDataCallParams.iSpeakerControl = RMobileCall::EMonitorSpeakerControlAlwaysOn;
    iDataCallParams.iSpeakerVolume = RMobileCall::EMonitorSpeakerVolumeMedium;
    iDataCallParams.iInterval = KCSPDefaultInterval;
    iDataCallParams.iWaitForDialTone = RMobileCall::EDialToneNoWait;
    iDataCallParams.iIdRestrict = RMobileCall::EIdRestrictDefault;
    iDataCallParams.iAutoRedial = KCSPDefaultAutoRedial;  
    // Set video call parameters
    iDataCallParams.iService                = RMobileCall::EServiceDataCircuitSync;
    iDataCallParams.iSpeed                  = RMobileCall::ESpeed64000;
    iDataCallParams.iProtocol               = RMobileCall::EProtocolIsdnMultimedia;
    iDataCallParams.iQoS                    = RMobileCall::EQoSTransparent;
    iDataCallParams.iRLPVersion             = RMobileCall::ERLPNotRequested; 
    iDataCallParams.iModemToMSWindowSize    = 0x00; 
    iDataCallParams.iMSToModemWindowSize    = 0x00; 
    iDataCallParams.iAckTimer               = 0x00; 
    iDataCallParams.iRetransmissionAttempts = 0x00; 
    iDataCallParams.iResequencingPeriod     = 0x00; 
    iDataCallParams.iV42bisReq              = RMobileCall::EV42bisNeitherDirection; 
    iDataCallParams.iV42bisCodewordsNum     = 0x00; 
    iDataCallParams.iV42bisMaxStringLength  = 0x00; 
    iDataCallParams.iUseEdge                = EFalse;
    }

// ---------------------------------------------------------------------------
// CSPVideoCall::SelectCallTypeFromProtocolCaps
// ---------------------------------------------------------------------------
//
CCPCall::TCallType CSPVideoCall::SelectCallTypeFromProtocolCaps() 
    {
    CSPLOGSTRING(CSPREQIN, "CSPVideoCall::SelectCallTypeFromProtocolCaps <");

    iVideoCallCapsMonitor->UpdateProtocolCaps();
    TUint protocolCaps = iVideoCallCapsMonitor->GetProtocolCaps();    
    CCPCall::TCallType callType;
    
    if ( protocolCaps == RMobileCall::KCapsIsdnMultimedia )
        {
        callType = CCPCall::ECallTypeVideo;
        }
    else 
        {
        callType = CCPCall::ECallTypeCSVoice;
        }
    CSPLOGSTRING2(CSPINT, "CSPVideoCall::SelectCallTypeFromProtocolCaps >\
         call type: %d", callType );

    return callType; 
    }


// End of File
