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
* Description: Implementation of CPhoneStateMachineVoIP class.
*
*/

// INCLUDE FILES
#include "cphonestatemachinevoip.h"
#include "phonelogger.h"
#include "phonestatedefinitionsvoip.h"
#include "cphonestateidlevoip.h"
#include "cphonestateincomingvoip.h"
#include "cphonestatecallsetupvoip.h"
#include "cphonestatesinglevoip.h"
#include "cphonestatealertingvoip.h"
#include "cphonecustomizationvoip.h"
#include "cphonevoiperrormessageshandler.h"
#include "cphonestateutilsvoip.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CPhoneStateMachineVoIP::CPhoneStateMachineVoIP
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CPhoneStateMachineVoIP::CPhoneStateMachineVoIP(
    MPhoneViewCommandHandle* aViewCommandHandle ) :
    CPhoneStateMachineGSM( aViewCommandHandle )
    {
    }


// ---------------------------------------------------------------------------
// CPhoneStateMachineVoIP::~CPhoneStateMachineVoIP()
// Destructor
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CPhoneStateMachineVoIP::~CPhoneStateMachineVoIP()
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneStateMachineVoIP::~CPhoneStateMachineVoIP()" );
    
    delete iVoipCustomization;
    delete iStateUtils;
    Dll::FreeTls();
    }
    

// ---------------------------------------------------------------------------
// CPhoneStateMachineVoIP::ConstructL
// Constructor
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CPhoneStateMachineVoIP::ConstructL()
    {
    __LOGMETHODSTARTEND( 
        PhoneUIVoIPExtension, "CPhoneStateMachineVoIP::ConstructL()" );
    
    iVoipCustomization = CPhoneCustomizationVoip::NewL( *this, *iViewCommandHandle );
    iCustomization = iVoipCustomization;
    iStateUtils = CPhoneStateUtilsVoip::NewL( *this, *iViewCommandHandle );
    }


// ---------------------------------------------------------------------------
// CPhoneStateMachineVoIP::State
// ---------------------------------------------------------------------------
//
MPhoneState* CPhoneStateMachineVoIP::State()
    {
    __LOGMETHODSTARTEND( 
        PhoneUIVoIPExtension, "CPhoneStateMachineVoIP::State()" );
    
    TInt err( KErrNone );
    TBool madeStateTransition( EFalse );
    TBool deleteOldState( ETrue );

    if( iOldStateId != iNewStateId )
        {
        if( iOldStateId == EPhoneStateIdle ||
            iOldStateId == EPhoneStateEmergency )
            {
            deleteOldState = EFalse;
            }
        if ( deleteOldState )
            {
            // Possible that derived class has been deleted iState,
            // so this delete statement may be useless.
            delete iState;
            iState = NULL;                      
            }

        // State transition need to be made - construct new state and
        // destruct old
        switch( iNewStateId )
            {
            case EPhoneStateCallSetup:
                TRAP( err, iState = CPhoneStateCallSetupVoIP::NewL( 
                    *this, *iViewCommandHandle, *iVoipCustomization ) );
                __ASSERT_ALWAYS( KErrNone == err, User::Invariant() );
                madeStateTransition = ETrue;
                break;

            case EPhoneStateIdle:
                if( iIdleState == NULL )
                    {
                    TRAP( err, iIdleState = CPhoneStateIdleVoIP::NewL( 
                        *this, *iViewCommandHandle, *iVoipCustomization ) );
                    __ASSERT_ALWAYS( KErrNone == err, User::Invariant() );
                    }
                iState = iIdleState;
                madeStateTransition = ETrue;
                break;                     

            case EPhoneStateIncoming:
                TRAP( err, iState = CPhoneStateIncomingVoIP::NewL( 
                    *this, *iViewCommandHandle, *iVoipCustomization ) );
                __ASSERT_ALWAYS( KErrNone == err, User::Invariant() );
                madeStateTransition = ETrue;
                break;                     

            case EPhoneStateSingle:
                TRAP( err, iState = CPhoneStateSingleVoIP::NewL( 
                    *this, *iViewCommandHandle, *iVoipCustomization ));
                __ASSERT_ALWAYS( KErrNone == err, User::Invariant() );
                madeStateTransition = ETrue;
                break;

            case EPhoneStateAlerting:
                TRAP( err, iState = CPhoneStateAlertingVoIP::NewL(
                    *this, *iViewCommandHandle, *iVoipCustomization ));
                __ASSERT_ALWAYS( KErrNone == err, User::Invariant() );
                madeStateTransition = ETrue;
                break;

            default:
                iState = CPhoneStateMachineGSM::State();
                break;
            }
        }

    if ( madeStateTransition )
        {
        iOldStateId = iNewStateId;
        
        // Needs to be set or any attempts to access eikon env handle
        // will fail
        iState->SetEikonEnv( EikonEnv() );
        }

    return iState;
    }


// ---------------------------------------------------------------------------
// CPhoneStateMachineVoIP::SetVoipErrorMessageHandler
// ---------------------------------------------------------------------------
//
void CPhoneStateMachineVoIP::SetVoipErrorMessageHandler( 
        CPhoneVoIPErrorMessagesHandler& iErrorHandler )
    {
    iVoipErrorMessageHandler = &iErrorHandler;
    }


// ---------------------------------------------------------------------------
// CPhoneStateMachineVoIP::VoipErrorMessageHandler
// ---------------------------------------------------------------------------
//
CPhoneVoIPErrorMessagesHandler& 
        CPhoneStateMachineVoIP::VoipErrorMessageHandler() const
    {
    __ASSERT_DEBUG( NULL != iVoipErrorMessageHandler, User::Invariant() );
    return *iVoipErrorMessageHandler;
    }


// ---------------------------------------------------------------------------
// CPhoneStateMachineVoIP::StateUtils
// ---------------------------------------------------------------------------
//
CPhoneStateUtilsVoip& CPhoneStateMachineVoIP::StateUtils()
    {
    return *iStateUtils;
    }


// ---------------------------------------------------------------------------
// CPhoneStateMachineVoIP::NewL()
// Constructor
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CPhoneStateMachineVoIP* CPhoneStateMachineVoIP::NewL(
        MPhoneViewCommandHandle* aViewCommandHandle )
    {
    __LOGMETHODSTARTEND( 
        PhoneUIVoIPExtension, "CPhoneStateMachineVoIP::NewL()" );
    
    CPhoneStateMachineVoIP* self = 
        new (ELeave) CPhoneStateMachineVoIP( aViewCommandHandle );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// End of File
