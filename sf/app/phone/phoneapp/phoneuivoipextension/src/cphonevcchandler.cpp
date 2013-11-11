/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of CPhoneVccHandler class.
*
*/

// INCLUDES
#include "phoneresourceids.h"
#include <StringLoader.h>
#include <avkon.rsg>
#include <mpeengineinfo.h>
#include <eikmenup.h>
#include <vccuipsproperty.h>
#include "cphonevcchandler.h"
#include "cphonetimer.h"
#include "cphonestate.h"
#include "cphonemainresourceresolver.h"
#include "phonelogger.h"
#include "cphonelogger.h"
#include "mphoneviewcommandhandle.h"
#include "mphonestatemachine.h"
#include "phonerssvoip.h"
#include "phoneappvoipcommands.hrh"
#include "tphonecmdparamglobalnote.h"

// For HO notification tone setting read
#include <spproperty.h>
#include <spsettings.h>
#include <spentry.h>
#include <spdefinitions.h>


// CONSTANTS
// 1,5 Second in microseconds
const TInt KPhoneShowNoteTime = 1500000;

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPhoneVccHandler::CPhoneVccHandler( 
    MPhoneStateMachine& aStateMachine,
    MPhoneViewCommandHandle& aViewCommandHandle ) : 
    iStateMachine( aStateMachine ),
    iViewCommandHandle( aViewCommandHandle ), iVccHoReady( ETrue )
    {
    }

// -----------------------------------------------------------
// CPhoneVccHandler::~CPhoneStateIdleVoIP()
// Destructor
// -----------------------------------------------------------
//
CPhoneVccHandler::~CPhoneVccHandler()
    {
    delete iPropListener;
    TRAPD( err, CancelHandoverNoteTimerL() );
    if ( err != KErrNone )
        {
        __PHONELOG( EBasic, PhoneUIVoIPExtension,
              "CPhoneVccHandler::~CPhoneVccHandler, leave" );
        }
     }

// -----------------------------------------------------------
// CPhoneVccHandler::ConstructL()
// Constructor
// -----------------------------------------------------------
//
void CPhoneVccHandler::ConstructL()
    {
    iPropListener = CVccUiPsPropertyListener::NewL( KVccPropKeyHoStatus );
    iPropListener->AddObserverL( *this );
    iPropListener->Start();
    }

// -----------------------------------------------------------
// CPhoneVccHandler::NewL()
// Constructor
// -----------------------------------------------------------
//
CPhoneVccHandler* CPhoneVccHandler::NewL( 
    MPhoneStateMachine& aStateMachine,
    MPhoneViewCommandHandle& aViewCommandHandle )
    {
    CPhoneVccHandler* self = new (ELeave) CPhoneVccHandler( 
        aStateMachine,
        aViewCommandHandle );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------
// CPhoneVccHandler::StartManualHandover
// -----------------------------------------------------------
//
void CPhoneVccHandler::StartManualHandoverL( TInt aCommand )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
         "CPhoneVccHandler::StartManualHandoverL()" )
         
    HandoverInProgressNoteL( aCommand );

    CVccUiPsProperty* prop = CVccUiPsProperty::NewL();
    CleanupStack::PushL( prop );
    
    if ( aCommand == EPhoneVoIPWaitHandoverFromGsm )
        {
        prop->NotifySubscriberL( EVccManualStartCsToPsHoRequest );    
        }
    else
        {
        prop->NotifySubscriberL( EVccManualStartPsToCsHoRequest );    
        }
    
    CleanupStack::PopAndDestroy(prop);
    iVccHoReady = EFalse;
    }

// -----------------------------------------------------------
// CPhoneVccHandler::AddHandoverMenuItem
// -----------------------------------------------------------
//
void CPhoneVccHandler::AddHandoverMenuItemL( 
        RArray<CTelMenuExtension::TCallInfo>& aArray,
        TInt /*aResourceId*/,
        CEikMenuPane& aMenuPane )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneVccHandler::AddHandoverMenuItemL()");       
    TInt activeCallId( KErrNotFound );
        
    for( TInt i = 0; i < aArray.Count(); i++ )
        {
        if( aArray[i].iCallState == CTelMenuExtension::EActive )
            {
            activeCallId = aArray[i].iCallId;
            }
        }
        
    if ( activeCallId != KErrNotFound )
        {
        // Vcc not available or Multicall not available
        if ( iVccUnavailable || aArray.Count() > 1 && iNoHoIfMultiCall )
            {
            return;    
            }

        TPECallType callType = 
            iStateMachine.PhoneEngineInfo()->CallType( activeCallId );
            
        CEikMenuPaneItem::SData data;
        data.iCascadeId=0;
        data.iFlags=0;
        HBufC* string = NULL;
        
        if ( callType == EPECallTypeVoIP && iVccHoAllowedToGsm )
            {
            string = StringLoader::LoadLC( 
              CPhoneMainResourceResolver::Instance()->
              ResolveResourceID( EPhoneVoIPHandoverToGsmOption ) );

            data.iCommandId = EPhoneCmdHandoverToGsm;
                
            }
        else if ( callType == EPECallTypeCSVoice && iVccHoAllowedToWlan )
            {  
            string = StringLoader::LoadLC( 
              CPhoneMainResourceResolver::Instance()->
              ResolveResourceID( EPhoneVoIPHandoverToWlanOption ) );

            data.iCommandId = EPhoneCmdHandoverToWlan;
            }
        else
            {
            // Not add menu item
            return;    
            }
        
        data.iText = *string;
        // Try to find index of EPhoneInCallCmdGoToIdle command
        // handover item will be placed above it if found
             
        TInt index( KErrNotFound );
      
        if ( aMenuPane.MenuItemExists( EPhoneInCallCmdGoToIdle, index) )
            {
            aMenuPane.InsertMenuItemL( data, index );  
            }
        else
            {
            aMenuPane.AddMenuItemL( data ); 
            }
       
        CleanupStack::PopAndDestroy( string );
        } 
    }

// ---------------------------------------------------------
// CPhoneVccHandler::StartHandoverNoteTimerL
// ---------------------------------------------------------
//
void CPhoneVccHandler::StartHandoverNoteTimerL()
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneVccHandler::StartHandoverNoteTimerL()");

    if( !iHandoverNoteTimer )
        {
        iHandoverNoteTimer = CPhoneTimer::NewL();          
        }
        
    iHandoverNoteTimer->After( KPhoneShowNoteTime,
        TCallBack( HandoverNoteTimerCallbackL, this ) );
    }

// ---------------------------------------------------------
// CPhoneVccHandler::CancelHandoverNoteTimerL
// ---------------------------------------------------------
//
void CPhoneVccHandler::CancelHandoverNoteTimerL()
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneVccHandler::CancelHandoverNoteTimerL()");
    if ( iHandoverNoteTimer ) 
        {
        delete iHandoverNoteTimer;
        iHandoverNoteTimer = NULL;
        }
    RemoveHandoverNoteL();
    } 

// ---------------------------------------------------------
// CPhoneVccHandler::HandoverNoteTimerCallbackL
// ---------------------------------------------------------
//
TInt CPhoneVccHandler::HandoverNoteTimerCallbackL( TAny* aAny )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneVccHandler::HandoverNoteTimerCallbackL()");
    
    reinterpret_cast<CPhoneVccHandler*>( aAny )->
        CancelHandoverNoteTimerL();

    return KErrNone;        
    }

// ---------------------------------------------------------
// CPhoneVccHandler::RemoveHandoverNoteL
// ---------------------------------------------------------
//
void CPhoneVccHandler::RemoveHandoverNoteL()
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneVccHandler::RemoveHandoverNoteL()");
        
    if ( iVccHoReady && !iHandoverNoteTimer )
        {
        iViewCommandHandle.ExecuteCommandL( EPhoneViewRemoveGlobalNote );            
        } 
    }
    
// -----------------------------------------------------------
// CPhoneVccHandler::HandoverInProgressNoteL
// -----------------------------------------------------------
//
void CPhoneVccHandler::HandoverInProgressNoteL( TInt aCommand )
    {
     TPhoneCmdParamGlobalNote globalNoteParam;  
          
// Check notification tone user setting
     TInt tone = GetHoNotifToneModeL();
    
//      if ( tone == 1 )
//           {
//           globalNoteParam.SetTone( EAvkonSIDConfirmationTone );
//           }
//       else
//           {
//           globalNoteParam.SetTone( EAvkonSIDNoSound );
//           }

     globalNoteParam.SetType( EPhoneNotificationDialog );
     globalNoteParam.SetSoftkeys( R_AVKON_SOFTKEYS_EMPTY );
     globalNoteParam.SetTextResourceId( 
                 CPhoneMainResourceResolver::Instance()->
                 ResolveResourceID( aCommand ) );
                                    
     iViewCommandHandle.ExecuteCommandL( 
                 EPhoneViewShowGlobalNote, &globalNoteParam ); 
                 
     StartHandoverNoteTimerL();   
    }    
 
// -----------------------------------------------------------------------------
// CPhoneVccHandler::VccPropertyChangedL
// Gets called when PS key is changed
// -----------------------------------------------------------------------------
//
void CPhoneVccHandler::VccPropertyChangedL( const TUint aKeyId,
                                    const TInt aValue ) 
    {            
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneVccHandler::VccPropertyChangedL() ");
   
    __PHONELOG1( EBasic, EPhoneControl,
            "CPhoneVccHandler::VccPropertyChangedL - key: %d", aValue );

    
    if( aKeyId == KVccPropKeyHoStatus )
        {
        switch( aValue )
            {               
            case EVccCsToPsHoFailure:
            case EVccPsToCsHoFailure:
                {
                __PHONELOG( EBasic, PhoneUIVoIPExtension,
                    "CPhoneVccHandler::VccPropertyChangedL VccPropertyChangedL -- HO fail" );
                // Remove handover note if handover is in progress
                if( !iVccHoReady )
                    {
                    iVccHoReady = ETrue;
                    CPhoneState* phoneState = 
                               static_cast< CPhoneState* >( iStateMachine.State() ); 
                    CancelHandoverNoteTimerL();
                    phoneState->SendGlobalInfoNoteL( EPhoneVoIPHandoverFail );
                    }
                 }
                break;
            case EVccCsToPsHoSuccessful:
            case EVccPsToCsHoSuccessful:
                {
                __PHONELOG( EBasic, PhoneUIVoIPExtension,
                    "CPhoneVccHandler::VccPropertyChangedL VccPropertyChangedL -- HO Success" );   
                iVccHoReady = ETrue;
                RemoveHandoverNoteL();
                }
                break;
            case EVccCsToPsHoStarted:
                {
                __PHONELOG( EBasic, PhoneUIVoIPExtension,
                    "CPhoneVccHandler::VccPropertyChangedL VccPropertyChangedL -- CsToPsHoStarted" );
                // Do not show if manually started since the wait note is 
                // already visible
                if ( iVccHoReady )
                    {
                    iVccHoReady = EFalse;
                    HandoverInProgressNoteL( EPhoneVoIPWaitHandoverFromGsm );
                    }
                }
                break;
            case EVccPsToCsHoStarted: 
                {
                __PHONELOG( EBasic, PhoneUIVoIPExtension,
                    "CPhoneVccHandler::VccPropertyChangedL VccPropertyChangedL -- PsToCsHoStarted" );
                // Do not show if manually started since the wait note is 
                // already visible
                if ( iVccHoReady )
                    {
                    iVccHoReady = EFalse;
                    HandoverInProgressNoteL( EPhoneVoIPWaitHandoverFromWlan);
                    }
                }
                break;
            case EVccPsToCsHoInprogress: 
            case EVccCsToPsHoInprogress:
                __PHONELOG( EBasic, PhoneUIVoIPExtension,
                    "CPhoneVccHandler::VccPropertyChangedL VccPropertyChangedL -- HoInprogress" );
                // Do nothing
                break;
            case EVccHoUnavailable:
                {
                __PHONELOG( EBasic, PhoneUIVoIPExtension,
                    "CPhoneVccHandler::VccPropertyChangedL VccPropertyChangedL -- HO Unavailable" );
                iVccUnavailable = ETrue;
                break;
                }
            case EVccHoStateIdle:
                {
                __PHONELOG( EBasic, PhoneUIVoIPExtension,
                    "CPhoneVccHandler::VccPropertyChangedL VccPropertyChangedL -- HO IDLE" );
                iVccHoAllowedToGsm = ETrue;
                iVccHoAllowedToWlan = ETrue;
                iNoHoIfMultiCall = EFalse;
                iVccUnavailable = EFalse;
                break;
                }
            case EVccCsToPsNotAllowed:
                {
                __PHONELOG( EBasic, PhoneUIVoIPExtension,
                    "CPhoneVccHandler::VccPropertyChangedL VccPropertyChangedL -- EVccCsToPsNotAllowed" );
                iVccHoAllowedToWlan = EFalse;
                iVccHoAllowedToGsm = ETrue;
                iNoHoIfMultiCall = EFalse;
                iVccUnavailable = EFalse;
                break;
                }
            
            case EVccPsToCsNotAllowed:
               {
                __PHONELOG( EBasic, PhoneUIVoIPExtension,
                    "CPhoneVccHandler::VccPropertyChangedL VccPropertyChangedL -- EVccPsToCsNotAllowed" );
                iVccHoAllowedToGsm = EFalse;
                iVccHoAllowedToWlan = ETrue;
                iNoHoIfMultiCall = EFalse;
                iVccUnavailable = EFalse;
                break;
                }
                
            case EVccHoStateIdleIfSingleCall:
                {
                __PHONELOG( EBasic, PhoneUIVoIPExtension,
                    "CPhoneVccHandler::VccPropertyChangedL VccPropertyChangedL -- EVccHoStateIdleIfSingleCall" );
                iNoHoIfMultiCall = ETrue;
                iVccHoAllowedToGsm = ETrue;
                iVccHoAllowedToWlan = ETrue;
                iVccUnavailable = EFalse;
                break;
                }
            case EVccHoAllowedToCsIfSingleCall:
                {
                __PHONELOG( EBasic, PhoneUIVoIPExtension,
                    "CPhoneVccHandler::VccPropertyChangedL VccPropertyChangedL -- EVccHoAllowedToCsIfSingleCall" );
                iNoHoIfMultiCall = ETrue;
                iVccHoAllowedToWlan = EFalse;
                iVccHoAllowedToGsm = ETrue;
                iVccUnavailable = EFalse;
                break;
                }
            case EVccHoAllowedToPsIfSingleCall:
                {
                __PHONELOG( EBasic, PhoneUIVoIPExtension,
                    "CPhoneVccHandler::VccPropertyChangedL VccPropertyChangedL -- EVccHoAllowedToPsIfSingleCall" );
                iNoHoIfMultiCall = ETrue;
                iVccHoAllowedToGsm = EFalse;
                iVccHoAllowedToWlan = ETrue;
                iVccUnavailable = EFalse;              
                break;
                }
                    
            default:

                __PHONELOG( EBasic, PhoneUIVoIPExtension,
                        "CPhoneVccHandler::VccPropertyChangedL VccPropertyChangedL -- default" );
                break;
            }       
        }
    }
// ---------------------------------------------------------------------------
// Gets HO notification tone setting value
// ---------------------------------------------------------------------------
//
TInt CPhoneVccHandler::GetHoNotifToneModeL()
    {
    // Find out VCC service id
     CSPProperty* property = CSPProperty::NewLC();
    
    CSPSettings* settings = CSPSettings::NewLC();
    
    TInt vccServiceId = GetVccServiceIdL();
    
    //get the VCC enabled VoIP service id. 
    TInt err = settings->FindPropertyL( 
            vccServiceId, ESubPropertyVccDtVoipServiceId, *property ); 
    
    TInt voipId;
    TInt error = property->GetValue( voipId );
    
    // Find and get the HO notification tone property
    TInt tone = 0;
    RBuf value;
    value.CreateL( 1 );
   
    err = settings->FindPropertyL( 
            voipId, ESubPropertyVccHoNotificationToneMode, *property ); 
    
    if ( !err )
        {
        HBufC* temp;
        property->GetValue( value );
        temp = value.AllocL();
        TLex temp2( *temp );
        temp2.Val( tone );
        delete temp;
        }
    value.Close();
    
    // Set to default (0 = off) in case of invalid value
    if ( !( tone == 0 || tone == 1 ))
        {
        tone = 0;
        }
  
    CleanupStack::PopAndDestroy( settings );
    CleanupStack::PopAndDestroy( property ); 

       
   
    return tone;
    }

// ---------------------------------------------------------------------------
// For finding out VCC service id.
// ---------------------------------------------------------------------------
//
TInt CPhoneVccHandler::GetVccServiceIdL()
    {
    TInt vccServiceId( KErrNotFound );
    //  Array of service provider IDs
    // is defined as: typedef RArray<TServiceId> RIdArray
    // in spdefinitions.h
    RIdArray serviceIdArray;
    CleanupClosePushL( serviceIdArray );

    CSPSettings* settings = CSPSettings::NewLC();
    settings->FindServiceIdsL( serviceIdArray );

    TBool foundEntry( EFalse );
     
      for ( TInt i( 0 ); i < serviceIdArray.Count() && !foundEntry; i++ )
           {
            CSPProperty* property = CSPProperty::NewLC(); 
             TInt error = settings->FindPropertyL( serviceIdArray[i], ESubPropertyVccVDI, *property ); 
           
              if( error == KErrNone )
                  {
                  vccServiceId =  serviceIdArray[i];
                  foundEntry = ETrue;
                   }
                 CleanupStack::PopAndDestroy( property );
           }
      CleanupStack::PopAndDestroy( settings );
      CleanupStack::PopAndDestroy( &serviceIdArray );  
      
      return vccServiceId;
    }
//  End of File 
