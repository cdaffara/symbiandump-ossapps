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
* Description:  Bluetooth audio disconnection handler.
*
*/


// INCLUDE FILES
#include    <featmgr.h>
#include    <StringLoader.h>
#include    <mpeengineinfo.h>

#include    "phoneui.hrh"
#include    "mphonestatemachine.h"
#include    "mphoneviewcommandhandle.h"
#include    "phoneconstants.h"
#include    "cphonebtaadisconnecthandler.h"
#include    "cphonemainresourceresolver.h"
#include    "cphonetimer.h"
#include    "phonelogger.h"
#include    "phonerssbase.h"
#include    "tphonecmdparamquery.h"
#include    "tphonecmdparamcallstatedata.h"
#include    "telinternalsingletonuids.h"
#include    "cphonestatehandle.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPhoneBtaaDisconnectHandler::InstanceL
// -----------------------------------------------------------------------------
//
CPhoneBtaaDisconnectHandler* CPhoneBtaaDisconnectHandler::InstanceL()
    {
    CPhoneBtaaDisconnectHandler* self = 
        static_cast<CPhoneBtaaDisconnectHandler*>( CCoeEnv::Static( KUidBtaaDisconnectHandlerSingleton ) );

    if ( !self )
        {
        self = new ( ELeave ) CPhoneBtaaDisconnectHandler;
        }
    
    return self;
    }

// -----------------------------------------------------------------------------
// CPhoneBtaaDisconnectHandler::CPhoneBtaaDisconnectHandler
// -----------------------------------------------------------------------------
//
CPhoneBtaaDisconnectHandler::CPhoneBtaaDisconnectHandler()
    : CCoeStatic( KUidBtaaDisconnectHandlerSingleton ), iState( EIdle )
    {
    CPhoneStateHandle* stateHandle = CPhoneStateHandle::Instance();
    
    iViewCommandHandle = stateHandle->ViewCommandHandle();
    iStateMachine = stateHandle->StateMachine();
    }

// -----------------------------------------------------------------------------
// CPhoneBtaaDisconnectHandler::~CPhoneBtaaDisconnectHandler
// -----------------------------------------------------------------------------
//
CPhoneBtaaDisconnectHandler::~CPhoneBtaaDisconnectHandler()
    {
    delete iTimer;
    }
 
// -----------------------------------------------------------------------------
// CPhoneBtaaDisconnectHandler::HandleTimeOutL
// -----------------------------------------------------------------------------
//
void CPhoneBtaaDisconnectHandler::HandleTimeOutL()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneBtaaDisconnectHandler::HandleTimeOutL()");
    Cancel();
    TerminateAllCallsL();
    }
 
// -----------------------------------------------------------------------------
// CPhoneBtaaDisconnectHandler::Cancel
// -----------------------------------------------------------------------------
//
void CPhoneBtaaDisconnectHandler::Cancel()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneBtaaDisconnectHandler::Cancel()");
    TRAP_IGNORE( iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveQuery ) );
    DeleteAll();
    if ( !iOldMuted && iStateMachine->PhoneEngineInfo()->AudioMute() )
        {
        TRAP_IGNORE ( SetMuteL( EFalse ) );
        }
    iOldMuted = EFalse;
    }

// -----------------------------------------------------------------------------
// CPhoneBtaaDisconnectHandler::LaunchQueryL
// -----------------------------------------------------------------------------
//
void CPhoneBtaaDisconnectHandler::LaunchQueryL()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneBtaaDisconnectHandler::LaunchQueryL()");
 
    if ( iState == ELaunchQuery )
        {
        // Query already available.
        __PHONELOG( EBasic,EPhoneControl,"CPhoneBtaaDisconnectHandler::LaunchQueryL.nok");
        return;
        }
   
    if ( !iTimer )
        {
        iTimer = CPhoneTimer::NewL();
        }

    // Start timer.
    iTimer->After( KPhoneBtaaConnectionLostQueryTimeout, this );
    
    // Mute the phone if needed.
    iOldMuted = iStateMachine->PhoneEngineInfo()->AudioMute() ;
    
    if ( !iOldMuted )
        {
        SetMuteL( ETrue );
        }
        
    iState = ELaunchQuery;
    
    if ( iVideoCallStatus )
        {
        ShowQueryL( EPhoneContinueInLoudspeakerConfirmationQuery );
        }
    else
        {
        ShowQueryL( EPhoneContinueInHandsetConfirmationQuery ); 
        }        
    }

// -----------------------------------------------------------------------------
// CPhoneBtaaDisconnectHandler::HandleConnectionLostL
// -----------------------------------------------------------------------------
//
void CPhoneBtaaDisconnectHandler::HandleConnectionLostL()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneBtaaDisconnectHandler::HandleConnectionLostL()"); 
    iVideoCallStatus = EFalse;
           
    if ( FeatureManager::FeatureSupported( KFeatureIdCsVideoTelephony ) )
        {
        // Fetch active call's id from view
        TPhoneCmdParamCallStateData callStateData;
        callStateData.SetCallState( EPEStateConnected );
        iViewCommandHandle->HandleCommandL( 
            EPhoneViewGetCallIdByState, &callStateData );
        TInt call = callStateData.CallId();
        if ( call != KErrNotFound &&
            iStateMachine->PhoneEngineInfo()->CallType( call ) == EPECallTypeVideo )
            {
            iVideoCallStatus = ETrue;
            }
        __PHONELOG1( EBasic, EPhoneControl, 
                "CPhoneBtaaDisconnectHandler::HandleConnectionLostL callType: %d",
                call != KErrNotFound ? 
                iStateMachine->PhoneEngineInfo()->CallType( call ) : KErrNotFound );            
        }
 
    if ( iState == EIdle )
        {
        LaunchQueryL();
        }
    }

// -----------------------------------------------------------------------------
// CPhoneBtaaDisconnectHandler::HandleQueryDismissL
// -----------------------------------------------------------------------------
//
void CPhoneBtaaDisconnectHandler::HandleQueryDismissL( 
        TInt aCommand )
    {
    __LOGMETHODSTARTEND(EPhoneControl,"CPhoneBtaaDisconnectHandler::HandleQueryDismissL()");
                                   
    if ( aCommand == EPhoneCmdYesBtDisconnectQuery )
        {
     
        // If phone was muted only for the duration of the note, unmute.
        if ( !iOldMuted )
            {
            SetMuteL( EFalse );
            }

        // Activate loudspeaker when required.
        if ( iVideoCallStatus )
            {
            iStateMachine->PhoneEngineInfo()->SetAudioOutputCommand( 
                EPELoudspeaker, ETrue);
            iStateMachine->SendPhoneEngineMessage( 
                MPEPhoneModel::EPEMessageSetAudioOutput );
            }
         }
    else
        {
        TerminateAllCallsL();
        //Unmute, for robustness      
        SetMuteL( EFalse );        
        }
    
    DeleteAll();
    iOldMuted = EFalse;
    }

// -----------------------------------------------------------------------------
// CPhoneBtaaDisconnectHandler::DeleteAll
// -----------------------------------------------------------------------------
//
void CPhoneBtaaDisconnectHandler::DeleteAll()
    {
    __LOGMETHODSTARTEND(EPhoneControl,"CPhoneBtaaDisconnectHandler::DeleteAll()");
    
    if ( iTimer ) 
        {
        if ( iTimer->IsActive() )
            {
            iTimer->Cancel();
            }
        delete iTimer;
        iTimer = NULL;
        }
 
    iState = EIdle;
    }

// -----------------------------------------------------------------------------
// CPhoneBtaaDisconnectHandler::TerminateAllCallsL
// -----------------------------------------------------------------------------
//
void CPhoneBtaaDisconnectHandler::TerminateAllCallsL()
    {
    __LOGMETHODSTARTEND(EPhoneControl,"CPhoneBtaaDisconnectHandler::TerminateAllCallsL()");
    
    if( iVideoCallStatus )
        {
        // Fetch active call's id from view
        TPhoneCmdParamCallStateData callStateData;
        callStateData.SetCallState( EPEStateConnected );
        iViewCommandHandle->HandleCommandL(
            EPhoneViewGetCallIdByState, &callStateData );
            
        if ( callStateData.CallId() != KErrNotFound )
            {
            iStateMachine->SetCallId( callStateData.CallId() );
            iStateMachine->SendPhoneEngineMessage( 
                MPEPhoneModel::EPEMessageRelease );        
            }
        else
            {
            iStateMachine->SendPhoneEngineMessage( 
                MPEPhoneModel::EPEMessageReleaseAll );        
            }        
        }
    else
        {
        iStateMachine->SendPhoneEngineMessage( 
            MPEPhoneModel::EPEMessageReleaseAll );        
        }        
    }
 
// -----------------------------------------------------------------------------
// CPhoneBtaaDisconnectHandler::ShowQueryL
// -----------------------------------------------------------------------------
//
void CPhoneBtaaDisconnectHandler::ShowQueryL( TInt aResourceId )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneBtaaDisconnectHandler::ShowQueryL( ) ");
    //__ASSERT_DEBUG( aResourceId, Panic( EPhoneCtrlParameterNotInitialized ) );
    TPhoneCmdParamQuery queryParam;
    queryParam.SetQueryType( EPhoneGlobalQueryDialog );
  
    HBufC* queryPrompt = StringLoader::LoadLC( 
            CPhoneMainResourceResolver::Instance()->ResolveResourceID( 
                aResourceId ));
   
    queryParam.SetQueryPrompt( *queryPrompt );
    queryParam.SetDefaultCba(             
        CPhoneMainResourceResolver::Instance()->
        ResolveResourceID( EPhoneDisconnectingQueryCBA ) );
  
    iViewCommandHandle->ExecuteCommandL( EPhoneViewShowQuery, &queryParam );
    
    CleanupStack::PopAndDestroy( queryPrompt );
    }

// -----------------------------------------------------------------------------
// CPhoneBtaaDisconnectHandler::SetMuteL
// -----------------------------------------------------------------------------
//
void CPhoneBtaaDisconnectHandler::SetMuteL( TBool aStatus )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneBtaaDisconnectHandler::SetMuteL( ) ");
    
    iStateMachine->PhoneEngineInfo()->SetAudioMuteCommand( aStatus );
    iStateMachine->SendPhoneEngineMessage( 
        MPEPhoneModel::EPEMessageSetAudioMute );    
    }

// -----------------------------------------------------------------------------
// CPhoneBtaaDisconnectHandler::IsQuery 
// -----------------------------------------------------------------------------
//
TBool CPhoneBtaaDisconnectHandler::IsQuery()
    {
    TBool ret = EFalse;
    if ( iState == ELaunchQuery )
        {
        ret = ETrue;
        __PHONELOG1( EBasic, EPhoneControl, 
                     "CPhoneBtaaDisconnectHandler::IsQuery: %d", ret );
        }
    return ret;
    }

//  End of File  
