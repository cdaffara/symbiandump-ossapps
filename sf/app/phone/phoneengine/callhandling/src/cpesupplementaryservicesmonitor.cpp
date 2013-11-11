/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the implementation of CPESupplementaryServicesMonitor class 
*                member functions.
*
*/


//  ==================== INCLUDE FILES  ====================
#include "cpesupplementaryservicesmonitor.h"
#include "mpemessagesender.h"
#include <talogger.h>
#include <mpedatastore.h>

// EXTERNAL DATA STRUCTURES
// None.

// EXTERNAL FUNCTION PROTOTYPES  
// None.

// CONSTANTS
// None.

// MACROS
// None.

// LOCAL CONSTANTS AND MACROS
// None.

// MODULE DATA STRUCTURES
// None.

// LOCAL FUNCTION PROTOTYPES
// None.

// ==================== LOCAL FUNCTIONS ====================
// None.

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CPESupplementaryServicesMonitor::CPESupplementaryServicesMonitor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPESupplementaryServicesMonitor::CPESupplementaryServicesMonitor( 
        MPEMessageSender& aOwner,
        MPEDataStore& aDataStore
        ) : iOwner( aOwner ),
            iDataStore( aDataStore )
    {
    TEFLOGSTRING( KTAOBJECT, 
        "CALL: CPESupplementaryServcesMonitor::CPESupplementaryServicesMonitor: complete" );     
    }


// Destructor
CPESupplementaryServicesMonitor::~CPESupplementaryServicesMonitor()
    {
    TEFLOGSTRING( KTAOBJECT,
        "CALL CPESupplementaryServicesMonitor::~CPESupplementaryServicesMonitor: Complete." );
    }

// -----------------------------------------------------------------------------
// CPESupplementaryServicesMonitor::BarringEventOccurred
// -----------------------------------------------------------------------------
//
EXPORT_C void CPESupplementaryServicesMonitor::BarringEventOccurred( 
    const MCCESsObserver::TCCESsBarringEvent aBarringEvent )
    {
    TEFLOGSTRING2( 
        KTAMESIN, 
        "CALL: CPESupplementaryServcesMonitor::BarringEventOccurred aBarringEvent %d", 
        aBarringEvent );
     switch( aBarringEvent )
        {       
        case ECCESsIncomingCallBarred:  /** Incoming call is barred. (DoCoMo) */
        case ECCESsAnonymousCallBarred: /** Anonymous call barring. */
        case ECCESsOutgoingCallBarred:  /** Outgoing call barring */
            iOwner.SendMessage( MEngineMonitor::EPEMessageCallBarred, KPECallIdNotUsed );
            if ( aBarringEvent == ECCESsOutgoingCallBarred )
                {
                iOwner.SendMessage( MEngineMonitor::EPEMessageOutgoingCallBarred, 
                                    KPECallIdNotUsed );
                }
            break;
            
        default:
            break;
        }
    }
    
        
// -----------------------------------------------------------------------------
// CPESupplementaryServicesMonitor::CLIEventOccurred
// -----------------------------------------------------------------------------
//
EXPORT_C void CPESupplementaryServicesMonitor::CLIEventOccurred( 
    const MCCESsObserver::TCCESsCLIEvent aCallLineEvent )
    {
 
    switch( aCallLineEvent )
        {
        case ECCESsTempClirSuppressUnsuccessful:    /** Temporary CLIR suppression was unsuccessful */
            iOwner.SendMessage( MEngineMonitor::EPEMessageTempClirSuppressUnsuccessful, KPECallIdNotUsed );
            break;
            
        case ECCESsTempClirActivationUnsuccessful:  /** Temporary CLIR activation was unsuccessful*/
            iOwner.SendMessage( MEngineMonitor::EPEMessageTempClirActivationUnsuccessful, KPECallIdNotUsed );
            break;
            
        default:
            break;
        }

    }
                          
// -----------------------------------------------------------------------------
// CPESupplementaryServicesMonitor::CallForwardEventOccurred
// -----------------------------------------------------------------------------
//
EXPORT_C void CPESupplementaryServicesMonitor::CallForwardEventOccurred( 
                        const MCCESsObserver::TCCESsCallForwardEvent aCallForwardEvent,
                        const TDesC& /*aRemoteAddress*/ )
    {  
    TEFLOGSTRING2( 
        KTAMESIN, 
        "CALL: CPESupplementaryServcesMonitor::CallForwardEventOccurred aCallForwardEvent %d", 
        aCallForwardEvent );
    switch( aCallForwardEvent )
        {
        case ECCESsIncCallIsForw:   /** Incoming call is forwarded */
            iOwner.SendMessage( MEngineMonitor::EPEMessageIncCallIsForw, KPECallIdNotUsed );
            break;
            
        case ECCESsIncCallForwToC:  /** Incoming call was forwarded because of user own settings. */
            iOwner.SendMessage( MEngineMonitor::EPEMessageIncCallForwToC, KPECallIdNotUsed );
            break;
            
        case ECCESsOutCallForwToC:  /** outgoing call was forwarded because of user own settings. */
            iOwner.SendMessage( MEngineMonitor::EPEMessageOutCallForwToC, KPECallIdNotUsed );
            break;
        
        case ECCESsForwardUnconditionalModeActive:
            iOwner.SendMessage( MEngineMonitor::EPEMessageForwardUnconditionalModeActive, KPECallIdNotUsed );
            break;
            
        case ECCESsForwardConditionallyModeActive:
            iOwner.SendMessage( MEngineMonitor::EPEMessageForwardConditionallyModeActive, KPECallIdNotUsed );
            break;
            
        case ESsCallWaiting:
            iOwner.SendMessage( MEngineMonitor::EPEMessageCallWaiting, KPECallIdNotUsed );
            TEFLOGSTRING( KTAINT, "CPESupplementaryServicesMonitor::CallForwardEventOccurred -> ECCESsCallWaiting" );
            break;
            
        default:
            break;        
        }       
    }
                                  
// -----------------------------------------------------------------------------
// CPESupplementaryServicesMonitor::CallCugEventOccurred
// -----------------------------------------------------------------------------
//
EXPORT_C void CPESupplementaryServicesMonitor::CallCugEventOccurred( 
    const MCCESsObserver::TCCESsCugEvent aCugEvent )
    {
    // These events can be ingored for now. They don't cause any action in our
    // layer or in UI.
    
    switch( aCugEvent )
        {
        case ECCESsShowIncCallGroupIndex: // fall through
            iOwner.SendMessage( MEngineMonitor::EPEMessageShowIncCallGroupIndex, KPECallIdNotUsed );
            break;
        case ECCESsRejectedCause:
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CPESupplementaryServicesMonitor::NotifyCurrentActiveALSLine
// -----------------------------------------------------------------------------
//
EXPORT_C void CPESupplementaryServicesMonitor::NotifyCurrentActiveALSLine( 
    TInt aLine )
    {
    TEFLOGSTRING2( 
        KTAMESIN, 
        "CALL: CPESupplementaryServcesMonitor::NotifyCurrentActiveALSLine aLine %d", 
        aLine );
        
    switch( aLine )
        {
        case RMobilePhone::EAlternateLineNotAvailable:
            iDataStore.SetALSLineSupport( EFalse );
            iDataStore.SetALSLine( CCCECallParameters::ECCELineTypePrimary );
            break;
            
        case RMobilePhone::EAlternateLinePrimary:
            iDataStore.SetALSLineSupport( ETrue );
            iDataStore.SetALSLine( CCCECallParameters::ECCELineTypePrimary );
            break;
            
        case RMobilePhone::EAlternateLineAuxiliary:
            iDataStore.SetALSLineSupport( ETrue );
            iDataStore.SetALSLine( CCCECallParameters::ECCELineTypeAux );
            break;
            
        default:
            break;
        }
        
    // Notify that, ALS line changed.   
    iOwner.SendMessage( MEngineMonitor::EPEMessageALSLineChanged, KPECallIdNotUsed );
    }
    
// End of File 
