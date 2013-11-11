/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Class takes care of CBA's.
*
*/

#include "cphoneuicommandmanager.h"

#include "mpeengineinfo.h"
#include "mphoneviewcommandhandle.h"
#include "mphonestatemachine.h"
#include "phonerssbase.h"
#include "phonelogger.h"
#include "cphonemainresourceresolver.h"
#include "tphonecmdparaminteger.h"
#include "tphonecmdparamcallstatedata.h"
#include "tphonecmdparamboolean.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPhoneUiCommandManager::CPhoneUiCommandManager
// ---------------------------------------------------------------------------
//
CPhoneUiCommandManager::CPhoneUiCommandManager(
         CPhoneState* aState,
         MPhoneViewCommandHandle& aViewCommandHandle,
         MPhoneStateMachine& aStateMachine )
   : iState ( aState ), 
     iViewCommandHandle ( aViewCommandHandle ),
     iStateMachine ( aStateMachine ),
     iSoftRejectStatus ( ETrue )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneUiCommandManager::CPhoneUiCommandManager() ");
    }

 // ---------------------------------------------------------------------------
 // CPhoneUiCommandManager::NewL
 // ---------------------------------------------------------------------------
 //
CPhoneUiCommandManager* CPhoneUiCommandManager::NewL( 
        CPhoneState* aState,
        MPhoneViewCommandHandle& aViewCommandHandle,
        MPhoneStateMachine& aStateMachine )
     {
     __LOGMETHODSTARTEND( EPhoneControl, "CPhoneUiCommandManager::NewL() ");
     return new (ELeave) CPhoneUiCommandManager( aState, aViewCommandHandle, aStateMachine );
     }

// ---------------------------------------------------------------------------
//  CPhoneUiCommandManager::~CPhoneUiCommandManager
// ---------------------------------------------------------------------------
//
CPhoneUiCommandManager::~CPhoneUiCommandManager()
     {
     __LOGMETHODSTARTEND( EPhoneControl, "CPhoneUiCommandManager::~CPhoneUiCommandManager() ");
     }
 
// ======== LOCAL FUNCTIONS ========

// -----------------------------------------------------------
// CPhoneUiCommandManager::UpdateUiCommands
// -----------------------------------------------------------
//
void CPhoneUiCommandManager::UpdateUiCommands()
    {
    SetButtonFlags();
    
    SetTouchButtons();
    
    SetToolBarButtons();
    }

// -----------------------------------------------------------
// CPhoneUiCommandManager::SetSoftRejectStatus
// -----------------------------------------------------------
//
void CPhoneUiCommandManager::SetSoftRejectStatus( TBool aSoftRejectStatus )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneUiCommandManager::SetSoftRejectStatus ()" );
    // Set the internal TBool variable
    iSoftRejectStatus = aSoftRejectStatus;
    __PHONELOG1( 
            EBasic, 
            EPhoneControl, 
            "CPhoneUiCommandManager::SetSoftRejectStatus - iSoftRejectStatus =%d", 
            iSoftRejectStatus  );
    }

// -----------------------------------------------------------
// CPhoneUiCommandManager::SoftRejectStatus
// -----------------------------------------------------------
//
TBool CPhoneUiCommandManager::SoftRejectStatus()
    {
    return iSoftRejectStatus;
    }

// -----------------------------------------------------------
// CPhoneUiCommandManager::SetSoftRejectDimmedFlag
// -----------------------------------------------------------
//
void CPhoneUiCommandManager::SetSoftRejectDimmedFlag()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneUiCommandManager::GetIncomingCallSilenceCBA ()" );
    TPhoneCmdParamCallStateData callStateData;
    callStateData.SetCallState( EPEStateRinging );
    TRAPD( err, iViewCommandHandle.HandleCommandL(
        EPhoneViewGetCallIdByState, &callStateData ) );
    TPhoneCmdParamBoolean dimSoftRejectParam;
    
    if ( err == KErrNone && callStateData.CallId() > KErrNotFound )
        {
        TBool privateNumber = iStateMachine.PhoneEngineInfo()->
                RemotePhoneNumber( callStateData.CallId() ).Length() == 0;
        
        TBool voip = iStateMachine.PhoneEngineInfo()->
                CallType( callStateData.CallId() ) == EPECallTypeVoIP;
        
        dimSoftRejectParam.SetBoolean( privateNumber || !iSoftRejectStatus || voip );        
        }
    else
        {
        iSoftRejectStatus = ETrue;
        dimSoftRejectParam.SetBoolean( EFalse );
        }
    iViewCommandHandle.ExecuteCommand(
            EPhoneViewSetSoftRejectDimmed, &dimSoftRejectParam );
    }

// -----------------------------------------------------------
// CPhoneUiCommandManager::SetButtonFlags
// -----------------------------------------------------------
//
void CPhoneUiCommandManager::SetButtonFlags()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneUiCommandManager::SetButtonFlags() ");
    SetSoftRejectDimmedFlag();
    
    TPhoneCmdParamBoolean muteParam;
    muteParam.SetBoolean( iStateMachine.PhoneEngineInfo()->AudioMute() );
    iViewCommandHandle.ExecuteCommand(EPhoneViewSetMuteFlag,&muteParam);
    
    const TPEAudioOutput audioOutput =
        iStateMachine.PhoneEngineInfo()->AudioOutput();

    TBool btAvailable = iStateMachine.PhoneEngineInfo()->AudioOutputAvailable(
        EPEBTAudioAccessory );
    
    TPhoneCmdParamBoolean btParam;
    btParam.SetBoolean( audioOutput == EPEBTAudioAccessory );        
    iViewCommandHandle.ExecuteCommand(EPhoneViewSetBlueToothFlag,&btParam);

    TPhoneCmdParamBoolean btAvailableParam;
    btAvailableParam.SetBoolean( btAvailable );        
    iViewCommandHandle.ExecuteCommand(
            EPhoneViewSetBluetoothAvailableFlag,&btAvailableParam);
    }

// -----------------------------------------------------------
// CPhoneUiCommandManager::SetTouchButtons
// -----------------------------------------------------------
//
void CPhoneUiCommandManager::SetTouchButtons()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneUiCommandManager::SetTouchButtons() ");
    TPhoneCmdParamBoolean booleanParam;
    booleanParam.SetBoolean( IsEmergency() );

    iViewCommandHandle.ExecuteCommand(
                 EPhoneViewSetTouchPaneButtons,
                 &booleanParam );
    }

// -----------------------------------------------------------
// CPhoneUiCommandManager::SetToolBarButtons
// -----------------------------------------------------------
//
void CPhoneUiCommandManager::SetToolBarButtons()
    {
    
    TInt resource( IsEmergency() ? 
        EPhoneCallHandlingEmergencyCBA :
        ToolBarResource() );
    
    if ( resource != KErrNotFound )
        {
        TPhoneCmdParamInteger integerParam;
        integerParam.SetInteger(
            CPhoneMainResourceResolver::Instance()->ResolveResourceID(
                    resource ) );
        iViewCommandHandle.ExecuteCommand( EPhoneViewUpdateCba,
            &integerParam );
        }
    }

// -----------------------------------------------------------
// CPhoneUiCommandManager::ToolBarResource
// -----------------------------------------------------------
//
TInt CPhoneUiCommandManager::ToolBarResource()
    {
    TInt ret( KErrNotFound );
    TPhoneCmdParamInteger callIdParam;
    iViewCommandHandle.ExecuteCommand( 
            EPhoneViewGetExpandedBubbleCallId, &callIdParam );
       
    TInt callID = callIdParam.Integer();
    
    if ( callID != KErrNotFound )
        {
        TInt callState = iStateMachine.PhoneEngineInfo()->
                CallState( callID );
                                
        if ( callState == EPEStateRinging ) 
            {
            ret = RingingCallResource();
            }
        else
            {
            ret = EPhoneCallHandlingInCallCBA;
            }
        }

    return ret;
    }

// -----------------------------------------------------------
// CPhoneUiCommandManager::RingingCallResource
// -----------------------------------------------------------
//
TInt CPhoneUiCommandManager::RingingCallResource()
    {
    TInt ret( KErrNotFound );
    TPhoneCmdParamInteger activeCallCountParam;
    TRAP_IGNORE( iViewCommandHandle.ExecuteCommandL(
            EPhoneViewGetCountOfActiveCalls, &activeCallCountParam ) );
    TInt activeCallCount = activeCallCountParam.Integer();
    
    if ( 0 == activeCallCount ) 
        {
        ret = SingleRingingCallResource();
        }
    else if ( 2 == activeCallCount )
        {
        ret = EPhoneCallHandlingIncomingRejectCBA;
        }
    else
        {
        ret = EPhoneCallHandlingCallWaitingCBA;
        }
    
    return ret;
    }

// -----------------------------------------------------------
// CPhoneUiCommandManager::SingleRingingCallResource
// -----------------------------------------------------------
//
TInt CPhoneUiCommandManager::SingleRingingCallResource()
    {
    TInt ret( KErrNotFound );

    TPhoneCmdParamBoolean ringingTonePlaying;
    iViewCommandHandle.ExecuteCommand( 
            EPhoneViewIsRingingTonePlaying, &ringingTonePlaying );
    
    if ( ringingTonePlaying.Boolean() )
        {
        ret = EPhoneCallHandlingIncomingCBA;
        }
    else
        {
        ret = EPhoneCallHandlingIncomingSoftRejectCBA;
        }
    
    return ret;
    }

// -----------------------------------------------------------
// CPhoneUiCommandManager::IsEmergency
// -----------------------------------------------------------
//
TBool CPhoneUiCommandManager::IsEmergency()
    {
    TBool emergency( EPEStateIdle != 
        iStateMachine.PhoneEngineInfo()->CallState( KPEEmergencyCallId ) );
    
    return emergency;
    }

