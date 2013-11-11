/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of CPhoneUIController class.
*
*/


// INCLUDE FILES
#include <featmgr.h>        // for FeatureManager
#include <settingsinternalcrkeys.h>

#include "cphoneuicontroller.h"
#include "pevirtualengine.h"
#include "cphonestatehandle.h"
#include "cphoneenginehandler.h"
#include "cphonesystemeventhandler.h"
#include "cphoneremotecontrolhandler.h"
#include "mphoneviewcommandhandle.h"
#include "phonestatedefinitions.h"
#include "phonelogger.h"
#include "phoneconstants.h"
#include "phoneui.pan"
#include "cphonecenrepproxy.h"
#include "cphonemediatorfactory.h"
#include "tphonecmdparamnumberentryobserver.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------
// CPhoneUIController::CPhoneUIController
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------
//
CPhoneUIController::CPhoneUIController()
    {
    }

// -----------------------------------------------------------
// CPhoneUIController::ConstructL()
// (other items were commented in a header).
// -----------------------------------------------------------
//
void CPhoneUIController::ConstructL( 
    MPhoneViewCommandHandle* aViewCommandHandle )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneUIController::ConstructL()");
    // Creates correct protocol DLL
    CreateProtocolDllL( aViewCommandHandle );

    // Reference the phone state machine
    iStateMachine = CPhoneStateHandle::Instance()->StateMachine();

    // Create the phone engine handler
    iEngineHandler = CPhoneEngineHandler::NewL( iStateMachine );
    // Create the system event handler
    iSystemEventHandler = CPhoneSystemEventHandler::NewL( iStateMachine );
     // Create the remote control handler
    iRemoteControlHandler = CPhoneRemoteControlHandler::NewL( iStateMachine );

    TInt leaveCode( 0 );
    TInt retry( 0 );
    RTimer timer;
    TRequestStatus timerReady;
    timer.CreateLocal();
    // Phone Engine is tried to start for KPeRetryCount times
    do
        {
        // Handle the message and trap leaves from message handling functions
        TRAP( leaveCode, iPhoneEngine = iStateMachine->CreatePhoneEngineL( 
            *this ) );
            
        if ( leaveCode )
            {
            // This thread is suspended for KPeRetryDelay
            timer.After( timerReady, KPeRetryDelay );
            User::WaitForRequest( timerReady );
            retry++;
            }
        }while ( leaveCode && retry < KPeRetryCount );
    timer.Close();
    
    if ( leaveCode )
        {
        // If Phone Engine didn't start
        User::Leave( leaveCode );
        }
    
    // Get the engine info
    iEngineInfo = iPhoneEngine->EngineInfo();

    // Store the phone engine information in the logger
    CPhoneLogger* phoneLogger = static_cast<CPhoneLogger*>
        ( CCoeEnv::Static( KUidPhoneUILoggerSingleton ) );
    phoneLogger->SetPhoneEngine( iPhoneEngine );

    // Store the phone engine information in the state machine
    iStateMachine->SetPhoneEngine( iPhoneEngine );
    
    // Go to the startup state
    iStateMachine->ChangeState( EPhoneStateStartup );

      CPhoneMediatorFactory::Instance()->CommandListener( this, iStateMachine,
            iStateMachine->PhoneEngineInfo() );
    }

// -----------------------------------------------------------------------------
// CPhoneUIController::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPhoneUIController* CPhoneUIController::NewL( 
    MPhoneViewCommandHandle* aViewCommandHandle )
    {
    __ASSERT_DEBUG( aViewCommandHandle,
        Panic( EPhoneCtrlParameterNotInitialized ) );
    
    CPhoneUIController* self = new( ELeave ) CPhoneUIController;
    
    CleanupStack::PushL( self );
    self->ConstructL( aViewCommandHandle );
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------
// Destructor
// 
// -----------------------------------------------------------

EXPORT_C CPhoneUIController::~CPhoneUIController()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneUIController::~CPhoneUIController()");
    delete iRemoteControlHandler;
    delete iSystemEventHandler;
    delete iEngineHandler;
    delete iStateHandle;
    }

// ---------------------------------------------------------
// CPhoneUIController::HandleMessage( TInt aMessage, TInt aCallId ) 
// ---------------------------------------------------------
//
EXPORT_C void CPhoneUIController::HandleMessage( 
    const TInt aMessage, 
    const TInt aCallId ) 
    {
    __PHONELOGENGINEMSG( aMessage, aCallId );
    TRAPD( err, iEngineHandler->DoHandleMessageL( aMessage, aCallId ) );

    if ( err != KErrNone )
        {
        __PHONELOG2(
            EOnlyFatal, 
            EPhoneControl,
            "PHONEUI_ERROR: CPhoneUIController::HandleMessage - Message received in unexpected state (aMessage=%d, err=%d)",
            aMessage,
            err);
        }

    __PHONELOGENGINEMSGEND( aMessage );
    }
    
// ---------------------------------------------------------
// CPhoneUIController::HandleError( const TPEErrorInfo& aErrorInfo )
// ---------------------------------------------------------
//
EXPORT_C void CPhoneUIController::HandleError( 
    const TPEErrorInfo& aErrorInfo ) 
    {
    TInt err( KErrNone );
    
       TRAP( err, iStateMachine->State()->HandleErrorL( aErrorInfo ) );            
    
    if( err != KErrNone )
        {
        __PHONELOG1(
            EOnlyFatal, 
            EPhoneControl,
            "PHONEUI_ERROR: CPhoneUIController::HandleError -  leave (err=%d)",
            err);
           __ASSERT_DEBUG( EFalse, Panic( EPhoneCtrlUnknownPanic ) );
        }
    }    

// ---------------------------------------------------------
// CPhoneUIController::HandleKeyEventL
// ---------------------------------------------------------
//
EXPORT_C TKeyResponse CPhoneUIController::HandleKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aEventCode )
    {
    if ( aEventCode == EEventKey )
        {
        iStateMachine->State()->HandleKeyMessageL( 
                        MPhoneKeyEvents::EPhoneKeyShortPress, 
                        TKeyCode( aKeyEvent.iCode ) );
        
        iStateMachine->State()->HandleDtmfKeyToneL( aKeyEvent, aEventCode );
        }
    else if ( aEventCode == EEventLongPress )
        {
        iStateMachine->State()->HandleKeyMessageL( 
                        MPhoneKeyEvents::EPhoneKeyLongPress, 
                        TKeyCode( aKeyEvent.iCode ) );    
        }
    else 
        {
        iStateMachine->State()->HandleDtmfKeyToneL( aKeyEvent, aEventCode );
        }
        
    return EKeyWasNotConsumed;
    }

// ---------------------------------------------------------
// CPhoneUIController::HandlePhoneStartupL
// ---------------------------------------------------------
//
EXPORT_C void CPhoneUIController::HandlePhoneStartupL()
    {
    __ASSERT_DEBUG( iSystemEventHandler, Panic( EPhoneCtrlInvariant ) );
    iSystemEventHandler->HandlePhoneStartupL();
    }

// ---------------------------------------------------------
// CPhoneUIController::HandleCommandL
// ---------------------------------------------------------
//
EXPORT_C TBool CPhoneUIController::HandleCommandL( TInt aCommand )
    {
    __ASSERT_DEBUG( iStateMachine->State(), Panic( EPhoneCtrlInvariant ) );

    return iStateMachine->State()->HandleCommandL( aCommand );
    }

// ---------------------------------------------------------
// CPhoneUIController::ProcessCommandL
// ---------------------------------------------------------
//
EXPORT_C TBool CPhoneUIController::ProcessCommandL( TInt aCommand )
    {
    __ASSERT_DEBUG( iStateMachine->State(), Panic( EPhoneCtrlInvariant ) );
    return iStateMachine->State()->ProcessCommandL( aCommand );
    }

// ---------------------------------------------------------
// CPhoneUIController::EngineInfo
// ---------------------------------------------------------
//
EXPORT_C MPEEngineInfo* CPhoneUIController::EngineInfo()
    {
    return iEngineInfo;
    }

// ---------------------------------------------------------
// CPhoneUIController::CreateProtocolDllL
// ---------------------------------------------------------
//
void CPhoneUIController::CreateProtocolDllL( 
    MPhoneViewCommandHandle* aViewCommandHandle )
    {
    TBool voipSupported( EFalse );

    if( FeatureManager::FeatureSupported( KFeatureIdCommonVoip ) )
        {
        TInt dynamicVoIP( KDynamicVoIPOff );
        CPhoneCenRepProxy::Instance()->GetInt( 
            KCRUidTelephonySettings, KDynamicVoIP, dynamicVoIP );
            
        if( dynamicVoIP == KDynamicVoIPOn )
            {
            voipSupported = ETrue;                
            }
        }

    if( voipSupported )
        {
        iStateHandle = CPhoneStateHandle::CreateL(
            aViewCommandHandle,
            KVoIPExtension,
            KUidAppVoIPExtensionStates );                        
        }
    else
        {
        iStateHandle = CPhoneStateHandle::CreateL(
            aViewCommandHandle,
            KGSMProtocol,
            KUidAppGSMStates );            
        }        
    }

// ---------------------------------------------------------
// CPhoneUIController::HandlePhoneNumberEditorCallBack
// ---------------------------------------------------------
//
TInt CPhoneUIController::HandlePhoneNumberEditorCallBack( TAny* aAny )
    {
    CPhoneUIController* aPhoneUiController =
        static_cast< CPhoneUIController* >( aAny );
        
    aPhoneUiController->DoHandlePhoneNumberEditorCallBack();
    
    return 0;
    }

// ---------------------------------------------------------
// CPhoneUIController::DoHandlePhoneNumberEditorCallBack
// ---------------------------------------------------------
//
void CPhoneUIController::DoHandlePhoneNumberEditorCallBack()
    {
    }

//  End of File  
