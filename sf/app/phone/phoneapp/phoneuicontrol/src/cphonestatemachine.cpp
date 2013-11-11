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
* Description: Implementation of CPhoneStateMachine class.
*
*/


// INCLUDE FILES
#include <pevirtualengine.h>
#include <mpeengineinfo.h>

#include "phoneui.pan"
#include "cphonestatemachine.h"
#include "cphonestatestartup.h"
#include "cphonestateidle.h"
#include "cphonestateincoming.h"
#include "cphonestatecallsetup.h"
#include "cphonestateincall.h"
#include "phonestatedefinitions.h"
#include "phonelogger.h"
#include "cphonestorage.h"
#include "mphonestorage.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
EXPORT_C CPhoneStateMachine::CPhoneStateMachine(
    MPhoneViewCommandHandle* aViewCommandHandle ) :
    iOldStateId( EPhoneStateNull ),
    iNewStateId( EPhoneStateNull ),
    iViewCommandHandle( aViewCommandHandle ),
    iEnv( CEikonEnv::Static() ) // codescanner::eikonenvstatic
    {
    __ASSERT_ALWAYS( aViewCommandHandle,
        Panic( EPhoneCtrlParameterNotInitialized ) );
    }

// -----------------------------------------------------------
// CPhoneStateMachine::~CPhoneStateMachine()
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
EXPORT_C CPhoneStateMachine::~CPhoneStateMachine()
    {
    if( iState == iIdleState )
        {
        delete iState;
        iState = NULL;
        iIdleState = NULL;          
        }
    else
        {
        if( iState )
            {
            delete iState;
            iState = NULL;          
            }
        if( iIdleState )
            {
            delete iIdleState;
            iIdleState = NULL;          
            }           
        }
    if( iPhoneEngine )
        {
        delete iPhoneEngine;
        iPhoneEngine = NULL;            
        }
    if( iPhoneStorage )
        {
        delete iPhoneStorage;
        iPhoneStorage = NULL;
        }
    }

// ---------------------------------------------------------
// CPhoneStateMachine::SetPhoneEngine
// ---------------------------------------------------------
//
EXPORT_C void CPhoneStateMachine::SetPhoneEngine( 
    MPEPhoneModel* aPhoneEngine )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneStateMachine::SetPhoneEngine ");
    __ASSERT_ALWAYS( aPhoneEngine, 
        Panic( EPhoneCtrlParameterNotInitialized ) );
    
    iPhoneEngine = aPhoneEngine;
    }

// -----------------------------------------------------------
// CPhoneStateMachine::PhoneEngine
// -----------------------------------------------------------
//
EXPORT_C MPEPhoneModel* CPhoneStateMachine::PhoneEngine()
    {
    __ASSERT_DEBUG( iPhoneEngine, Panic( EPhoneCtrlInvariant ) );
    return iPhoneEngine;
    }
// -----------------------------------------------------------
// CPhoneStateMachine::PhoneEngineInfo
// -----------------------------------------------------------
//
EXPORT_C MPEEngineInfo* CPhoneStateMachine::PhoneEngineInfo()
    {
    if ( iPhoneEngine )
        {
        return iPhoneEngine->EngineInfo();
        }
    else
        {
        return NULL;
        }
    }

// -----------------------------------------------------------
// CPhoneStateMachine::ChangeState()
// (other items were commented in a header).
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateMachine::ChangeState( TInt aState )
    {
    __PHONELOGSTATECHANGE( iNewStateId, aState );
    iNewStateId = aState;
    }

// -----------------------------------------------------------
// CPhoneStateMachine::State()
// (other items were commented in a header).
// -----------------------------------------------------------
//
EXPORT_C MPhoneState* CPhoneStateMachine::State()
    {
    return iState;
    }

// -----------------------------------------------------------
// CPhoneStateMachine::CreatePhoneEngineL
// (other items were commented in a header).
// -----------------------------------------------------------
//
EXPORT_C MPEPhoneModel* CPhoneStateMachine::CreatePhoneEngineL(
    MEngineMonitor& /*aEngineMonitor*/ )
    {
    return NULL;        
    }
    
// -----------------------------------------------------------
// CPhoneStateMachine::SendPhoneEngineMessage
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateMachine::SendPhoneEngineMessage( 
    TInt aMessage )
    {
    __PHONELOGENGINECMD( aMessage );
    PhoneEngine()->HandleMessage( aMessage );
    }
    
// -----------------------------------------------------------
// CPhoneStateMachine::SetCallId
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateMachine::SetCallId( 
    TInt aCallId )
    {
    __PHONELOG1(
        EBasic, 
        EPhoneControl,
        "CALL ID: CPhoneStateMachine::SetCallId (%d)",
        aCallId);     
    PhoneEngineInfo()->SetCallId( aCallId );
    }

// -----------------------------------------------------------
// CPhoneStateMachine::PhoneStorage
// -----------------------------------------------------------
//
EXPORT_C MPhoneStorage* CPhoneStateMachine::PhoneStorage()
    {
    if ( iPhoneStorage == NULL )
        {
        TInt err( KErrNone );
        TRAP( err, iPhoneStorage = CPhoneStorage::NewL());
        __ASSERT_ALWAYS( KErrNone == err, User::Invariant() );
        }
    return iPhoneStorage;
    }

// -----------------------------------------------------------
// CPhoneStateMachine::EikonEnv
// -----------------------------------------------------------
//
EXPORT_C CEikonEnv* CPhoneStateMachine::EikonEnv() const
    {
    return iEnv;
    }

// End of File
