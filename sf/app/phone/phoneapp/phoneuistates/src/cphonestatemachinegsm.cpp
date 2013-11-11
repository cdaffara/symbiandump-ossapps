/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of CPhoneStateMachineGSM class.
*
*/


// INCLUDE FILES
#include <cpephonemodelif.h>
#include "cphonestatemachinegsm.h"
#include "phonelogger.h"
#include "phonestatedefinitions.h"
#include "phonestatedefinitionsgsm.h"
#include "cphonecallsetup.h"
#include "cphonealerting.h"
#include "cphonesinglecall.h"
#include "cphonesingleandcallsetup.h"
#include "cphonesingleandalerting.h"
#include "cphonesingleandwaiting.h"
#include "cphonetwosingles.h"
#include "cphoneconference.h"
#include "cphoneconferenceandcallsetup.h"
#include "cphoneconferenceandsingle.h"
#include "cphoneconferenceandwaiting.h"
#include "cphonesingleandcallsetupandwaiting.h"
#include "cphonecallsetupandwaiting.h"
#include "cphonetwosinglesandwaiting.h"
#include "cphoneconferenceandsingleandwaiting.h"
#include "cphoneemergency.h"
#include "cphoneconferenceandwaitingandcallsetup.h"
#include "cphoneidle.h"
#include "cphoneincoming.h"
#include "cphonestartup.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CPhoneStateMachineGSM::CPhoneStateMachineGSM
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CPhoneStateMachineGSM::CPhoneStateMachineGSM(
    MPhoneViewCommandHandle* aViewCommandHandle ) :
    CPhoneStateMachine( aViewCommandHandle )
    {
    }

// -----------------------------------------------------------
// CPhoneStateMachineGSM::~CPhoneStateMachineGSM()
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
EXPORT_C CPhoneStateMachineGSM::~CPhoneStateMachineGSM()
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, "CPhoneStateMachineGSM::~CPhoneStateMachineGSM() ");
    if( iEmergencyState )
        {
        delete iEmergencyState;
        iEmergencyState = NULL;         
        }
    Dll::FreeTls();
    }

// ---------------------------------------------------------
// CPhoneStateMachineGSM::State
// ---------------------------------------------------------
EXPORT_C MPhoneState* CPhoneStateMachineGSM::State()
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, "CPhoneStateMachineGSM::State() ");
    __PHONELOG1( EBasic, EPhoneUIStates,
            "CPhoneStateMachineGSM::State() = %d ", iNewStateId );
            
    TBool madeStateTransition = EFalse;
    TBool deleteOldState( ETrue );
    TInt err( KErrNone );
    
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
                TRAP( err, iState = CPhoneCallSetup::NewL( 
                    this, iViewCommandHandle, iCustomization ));
                __ASSERT_ALWAYS( KErrNone == err, User::Invariant() );
                madeStateTransition = ETrue;
                break;
                
            case EPhoneStateAlerting:
                TRAP( err, iState = CPhoneAlerting::NewL(
                    this, iViewCommandHandle, iCustomization ));
                __ASSERT_ALWAYS( KErrNone == err, User::Invariant() );
                madeStateTransition = ETrue;
                break;
                
            case EPhoneStateSingle:
                TRAP( err, iState = CPhoneSingleCall::NewL( 
                    this, iViewCommandHandle, iCustomization ));
                __ASSERT_ALWAYS( KErrNone == err, User::Invariant() );
                madeStateTransition = ETrue;
                break;
                
            case EPhoneStateWaitingInSingle:
                TRAP( err, iState = CPhoneSingleAndWaiting::NewL( 
                    this, iViewCommandHandle, iCustomization ));
                __ASSERT_ALWAYS( KErrNone == err, User::Invariant() );
                madeStateTransition = ETrue;
                break;
                
            case EPhoneStateTwoSingles:
                TRAP( err, iState = CPhoneTwoSingles::NewL( 
                    this, iViewCommandHandle, iCustomization ));
                __ASSERT_ALWAYS( KErrNone == err, User::Invariant() );
                madeStateTransition = ETrue;
                break;
                
            case EPhoneStateCallSetupInSingle:
                TRAP( err, iState = CPhoneSingleAndCallSetup::NewL( 
                    this, iViewCommandHandle, iCustomization ));
                __ASSERT_ALWAYS( KErrNone == err, User::Invariant() );
                madeStateTransition = ETrue;
                break;
                
            case EPhoneStateAlertingInSingle:
                TRAP( err, iState = CPhoneSingleAndAlerting::NewL( 
                    this, iViewCommandHandle, iCustomization ));
                __ASSERT_ALWAYS( KErrNone == err, User::Invariant() );
                madeStateTransition = ETrue;
                break;
                
            case EPhoneStateConference:
                TRAP( err, iState = CPhoneConference::NewL( 
                    this, iViewCommandHandle, iCustomization ));
                __ASSERT_ALWAYS( KErrNone == err, User::Invariant() );
                madeStateTransition = ETrue;
                break;
                
            case EPhoneStateConferenceAndCallSetup:
                TRAP( err, iState = CPhoneConferenceAndCallSetup::NewL( 
                    this, iViewCommandHandle, iCustomization ) );
                __ASSERT_ALWAYS( KErrNone == err, User::Invariant() );
                madeStateTransition = ETrue;
                break;
 
            case EPhoneStateConferenceAndSingle:
                TRAP( err, iState = CPhoneConferenceAndSingle::NewL( 
                    this, iViewCommandHandle, iCustomization ) );
                __ASSERT_ALWAYS( KErrNone == err, User::Invariant() );
                madeStateTransition = ETrue;
                break;
                
            case EPhoneStateConferenceAndWaiting:
                TRAP( err, iState = CPhoneConferenceAndWaiting::NewL( 
                    this, iViewCommandHandle, iCustomization ) );
                __ASSERT_ALWAYS( KErrNone == err, User::Invariant() );
                madeStateTransition = ETrue;
                break;
                
            case EPhoneStateSingleAndCallSetupAndWaiting:
                TRAP( err, iState = CPhoneSingleAndCallSetupAndWaiting::NewL( 
                    this, iViewCommandHandle, iCustomization ) );
                __ASSERT_ALWAYS( KErrNone == err, User::Invariant() );
                madeStateTransition = ETrue;
                break;
                
            case EPhoneStateCallSetupAndWaiting:
                TRAP( err, iState = CPhoneCallSetupAndWaiting::NewL( 
                    this, iViewCommandHandle, iCustomization ) );
                __ASSERT_ALWAYS( KErrNone == err, User::Invariant() );
                madeStateTransition = ETrue;
                break;

            case EPhoneStateTwoSinglesAndWaiting:
                TRAP( err, iState = CPhoneTwoSinglesAndWaiting::NewL( 
                    this, iViewCommandHandle, iCustomization ) );
                __ASSERT_ALWAYS( KErrNone == err, User::Invariant() );
                madeStateTransition = ETrue;
                break;
                
            case EPhoneStateConferenceAndSingleAndWaiting:
                TRAP( err, iState = CPhoneConferenceAndSingleAndWaiting::NewL( 
                    this, iViewCommandHandle, iCustomization ));
                __ASSERT_ALWAYS( KErrNone == err, User::Invariant() );
                madeStateTransition = ETrue;
                break;
                
            case EPhoneStateEmergency:
                if( !iEmergencyStateConstructed )
                    {
                    TRAP( err, iEmergencyState = CPhoneEmergency::NewL( 
                        this, iViewCommandHandle, iCustomization ) );
                    __ASSERT_ALWAYS( KErrNone == err, User::Invariant() );                                          
                    iEmergencyStateConstructed = ETrue;
                    }
                static_cast< CPhoneEmergency* >( iEmergencyState )->
                    SetStartupInterrupted( iOldStateId == EPhoneStateStartup );
                iState = iEmergencyState;
                madeStateTransition = ETrue;
                break;
                
            case EPhoneStateConferenceAndWaitingAndCallSetup:
                TRAP( err, iState = 
                    CPhoneConferenceAndWaitingAndCallSetup::NewL( 
                        this, iViewCommandHandle, iCustomization ) );
                __ASSERT_ALWAYS( KErrNone == err, User::Invariant() );
                madeStateTransition = ETrue;
                break;
                
            case EPhoneStateIdle:
                if ( iOldStateId != EPhoneStateEmergency )
                    {
                    if( !iEmergencyStateConstructed )
                        {
                        TRAP( err, iEmergencyState = CPhoneEmergency::NewL( 
                            this, iViewCommandHandle, iCustomization ) );
                        __ASSERT_ALWAYS( KErrNone == err, User::Invariant() );                                          
                        iEmergencyStateConstructed = ETrue;
                        }
                    }
                if( iIdleState == NULL )
                    {
                    TRAP( err, iIdleState = 
                        CPhoneIdle::NewL( this, iViewCommandHandle, iCustomization ) );
                    __ASSERT_ALWAYS( KErrNone == err, User::Invariant() );
                    }
                iState = iIdleState;
                madeStateTransition = ETrue;
                break;
                
            case EPhoneStateIncoming:
                TRAP( err, iState = 
                    CPhoneIncoming::NewL( this, iViewCommandHandle, iCustomization ) );
                __ASSERT_ALWAYS( KErrNone == err, User::Invariant() );
                madeStateTransition = ETrue;
                break;

            case EPhoneStateStartup:
                TRAP( err, iState = 
                    CPhoneStartup::NewL( this, iViewCommandHandle, 
                    iOldStateId == EPhoneStateEmergency ) );
                __ASSERT_ALWAYS( KErrNone == err, User::Invariant() );
                madeStateTransition = ETrue;
                break;

            default:
                iState = CPhoneStateMachine::State();
                break;
            }
        }

    if ( madeStateTransition )
        {
        __PHONELOGSTATECHANGE( iOldStateId, iNewStateId );
        iOldStateId = iNewStateId;
        
        // Needs to be set or any attempts to access eikon env handle
        // will fail
        iState->SetEikonEnv( EikonEnv() );
        }

    return iState;
    }

// -----------------------------------------------------------
// CPhoneStateMachineGSM::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneStateMachineGSM* CPhoneStateMachineGSM::NewL(
    MPhoneViewCommandHandle* aViewCommandHandle ) 
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, "CPhoneStateMachineGSM::NewL() ");
    CPhoneStateMachineGSM* self = 
        new (ELeave) CPhoneStateMachineGSM( aViewCommandHandle );

    return self;
    }

// -----------------------------------------------------------
// CPhoneStateMachineGSM::CreatePhoneEngineL
// -----------------------------------------------------------
//
EXPORT_C MPEPhoneModel* CPhoneStateMachineGSM::CreatePhoneEngineL( 
    MEngineMonitor& aEngineMonitor )
    {
    return CPEPhoneModelIF::CreateL( aEngineMonitor );
    }

// End of File
