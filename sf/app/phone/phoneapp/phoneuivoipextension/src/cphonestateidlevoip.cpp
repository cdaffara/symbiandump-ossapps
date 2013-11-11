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
* Description: Implementation of CPhoneStateIdleVoIP class.
*
*/


// INCLUDES
#include <mpeengineinfo.h>       
#include "pevirtualengine.h"
#include <featmgr.h>
                                  
#include "cphonestateidlevoip.h"
#include "phoneappvoipcommands.hrh"
#include "phonerssvoip.h"
#include "tphonecmdparaminteger.h"
#include "cphonemainresourceresolver.h"
#include "mphonestatemachine.h"
#include "cphonelogger.h"
#include "phoneui.pan"
#include "cphonecustomizationvoip.h"
#include "cphonecenrepproxy.h"
#include "cphonestateutilsvoip.h"
#include "cphonestatemachinevoip.h"

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPhoneStateIdleVoIP::CPhoneStateIdleVoIP( 
    MPhoneStateMachine& aStateMachine, 
    MPhoneViewCommandHandle& aViewCommandHandle,
    MPhoneCustomization& aCustomization ) : 
    CPhoneIdle( &aStateMachine, &aViewCommandHandle, &aCustomization )
    {
    }


// -----------------------------------------------------------
// CPhoneStateIdleVoIP::~CPhoneStateIdleVoIP()
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneStateIdleVoIP::~CPhoneStateIdleVoIP()
    {
    }


// -----------------------------------------------------------
// CPhoneStateIdleVoIP::ConstructL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
void CPhoneStateIdleVoIP::ConstructL()
    {
    CPhoneIdle::ConstructL();
    }


// -----------------------------------------------------------
// CPhoneStateIdleVoIP::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneStateIdleVoIP* CPhoneStateIdleVoIP::NewL( 
    MPhoneStateMachine& aStateMachine, 
    MPhoneViewCommandHandle& aViewCommandHandle,
    MPhoneCustomization& aCustomization )
    {
    CPhoneStateIdleVoIP* self = new (ELeave) CPhoneStateIdleVoIP( 
        aStateMachine, aViewCommandHandle, aCustomization );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }


// -----------------------------------------------------------
// CPhoneStateIdleVoIP::HandlePhoneEngineMessageL
// -----------------------------------------------------------
//
void CPhoneStateIdleVoIP::HandlePhoneEngineMessageL( 
    const TInt aMessage,
    TInt aCallId )
    {
    switch ( aMessage )
        {           
        default:
            CPhoneIdle::HandlePhoneEngineMessageL( aMessage, aCallId );
            break;
        }
    }


// -----------------------------------------------------------
// CPhoneStateIdleVoIP::HandleCommandL
// -----------------------------------------------------------
//
TBool CPhoneStateIdleVoIP::HandleCommandL( TInt aCommand )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, "CPhoneStateIdleVoIP::HandleCommandL() ");

    TBool commandStatus( ETrue );

    switch( aCommand )
        {       
        case EPhoneNumberAcqCmdInternetCall:
            {
            StateUtils().SelectServiceAndDialL();
            }
            break;

        default:
            commandStatus = CPhoneIdle::HandleCommandL( aCommand );
            break;
        }

    return commandStatus;
    }


// -----------------------------------------------------------
// CPhoneStateIdleVoIP::HandleSendCommandL
// -----------------------------------------------------------
//
void CPhoneStateIdleVoIP::HandleSendCommandL()
    {
    __LOGMETHODSTARTEND( 
        PhoneUIVoIPExtension, "CPhoneStateIdleVoIP::HandleSendCommandL()" );
    
    HBufC* phoneNumber = iNumberEntryManager->PhoneNumberFromEntryLC();
     
    if ( !IsSimOk() || IsEmergencyNumber( *phoneNumber ) )
        {
        CPhoneIdle::HandleSendCommandL();         
        }  
    else
        {
        TUint serviceId( 0 );
        CPhoneStateUtilsVoip& utils = StateUtils();
        if ( utils.IsVoipPreferredCall( serviceId ) &&
             !iStateMachine->PhoneEngineInfo()->PhoneNumberIsServiceCode() )
            {
            utils.SelectServiceAndDialL( KNullDesC, serviceId );
            }
        else
            {
            if ( utils.IsVoipNumber() )
                {
                utils.SelectServiceAndDialL();
                }
            else
                {
                CPhoneIdle::HandleSendCommandL();
                }
            }        
        }
    
    CleanupStack::PopAndDestroy( phoneNumber );
    }


// -----------------------------------------------------------
// CPhoneStateIdleVoIP::HandleDialingL
// -----------------------------------------------------------
//
void CPhoneStateIdleVoIP::HandleDialingL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, "CPhoneStateIdleVoIP::HandleDialingL( ) ");

    if( iStateMachine->PhoneEngineInfo()->CallType( aCallId ) == EPECallTypeVoIP )
        {
        //TODO
        //SetCallHeaderType( EPECallTypeVoIP );       
        }

    CPhoneIdle::HandleDialingL( aCallId );
    }


// -----------------------------------------------------------
// CPhoneStateIdleVoIP::DialL
// -----------------------------------------------------------
//   
void CPhoneStateIdleVoIP::DialL( 
        const TDesC& aNumber, 
        TPhoneNumberType aNumberType,
        TDialInitiationMethod aDialMethod )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
                         "CPhoneStateIdleVoIP::DialL( ) ");
    if ( FeatureManager::FeatureSupported( KFeatureIdCommonVoip ) )
        {
        CPhoneStateUtilsVoip& utils = StateUtils();

        // When speed dialling to voice mailbox, service id comes from vmbx
        TUint serviceId = iStateMachine->PhoneEngineInfo()->ServiceIdCommand();
        
        if ( serviceId != KUnknownService && EPhoneNumberTypeNotFound != aNumberType )
            {
            if ( EPhoneNumberTypeVoip == aNumberType )
                {
                utils.DialVoIPL( aNumber, serviceId );
                }
            else
                {
                CPhoneIdle::DialL( aNumber, aNumberType, aDialMethod );
                }
            }
        else if( utils.IsVoipPreferredCall( serviceId ) )
            {
            // Because IsVoipPreferredCall assumes that Send key is pressed,
            // we only use serviceId given by it when dial method is SendCommand
            if ( EDialMethodSendCommand == aDialMethod ) 
                {
                utils.SelectServiceAndDialL( aNumber, serviceId );
                }
            else
                {
                // One key dialing is done based on number type even
                // if VoIP is preferred call.
                CPhoneIdle::DialL( aNumber, aNumberType, aDialMethod );
                }
            }
        else
            {
            if ( EPhoneNumberTypeVoip == aNumberType )
                {
                utils.SelectServiceAndDialL( aNumber );
                }
            else
                {
                CPhoneIdle::DialL( aNumber, aNumberType, aDialMethod );
                }
            }
        }
    else
        {
        CPhoneIdle::DialL( aNumber, aNumberType, aDialMethod );
        }
    }


// -----------------------------------------------------------
// CPhoneStateIdleVoip::IsEmergencyNumber()
// -----------------------------------------------------------
//
TBool CPhoneStateIdleVoIP::IsEmergencyNumber( const TDesC& aNumber )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneStateIdleVoIP::IsEmergencyNumber" );
    
    return StateUtils().IsEmergencyNumber( aNumber );
    }


// -----------------------------------------------------------
// CPhoneStateIdleVoIP::StateUtils
// -----------------------------------------------------------
//
CPhoneStateUtilsVoip& CPhoneStateIdleVoIP::StateUtils()
    {
    return static_cast<CPhoneStateMachineVoIP&>( 
        *iStateMachine ).StateUtils();
    }

// End of File
