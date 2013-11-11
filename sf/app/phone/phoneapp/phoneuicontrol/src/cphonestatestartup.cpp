/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of CPhoneStateStartup class.
*
*/

// INCLUDES
#include "cphonestatestartup.h"
#include "phonestatedefinitions.h"
#include "mphonestatemachine.h"
#include "tphonecmdparamkeycapture.h"
#include "tphonecmdparamboolean.h"
#include "phonelogger.h"
#include "phonerssbase.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
EXPORT_C CPhoneStateStartup::CPhoneStateStartup( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization
     ) : 
    CPhoneState( aStateMachine, aViewCommandHandle, aPhoneCustomization ),
    iPEReady( EFalse ),
    iPhoneReady( EFalse )
    {
    }

// -----------------------------------------------------------
// CPhoneStateStartup::~CPhoneStateStartup()
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
EXPORT_C CPhoneStateStartup::~CPhoneStateStartup()
    {
    delete iCreateNote;
    }

// -----------------------------------------------------------
// CPhoneStateStartup::ConstructL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateStartup::ConstructL()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneStateStartup::ConstructL() ");
    CPhoneState::BaseConstructL();
    }

// -----------------------------------------------------------
// CPhoneStateStartup::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
EXPORT_C CPhoneStateStartup* CPhoneStateStartup::NewL( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization )
    {
    CPhoneStateStartup* self = new (ELeave) CPhoneStateStartup( 
        aStateMachine, aViewCommandHandle, aPhoneCustomization );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------
// CPhoneStateStartup::HandlePhoneEngineMessageL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateStartup::HandlePhoneEngineMessageL( 
    const TInt aMessage, 
    TInt aCallId )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneStateStartup::HandlePhoneEngineMessageL() ");
    switch ( aMessage )
        {
        case MEngineMonitor::EPEMessagePEConstructionReady:
            HandlePEConstructionReadyL( aCallId );
            break;
            
        default:
            CPhoneState::HandlePhoneEngineMessageL( aMessage, aCallId );
            break;
        }
    }

// -----------------------------------------------------------
// CPhoneStateStartup::HandlePEConstructionReadyL
// -----------------------------------------------------------
//
void CPhoneStateStartup::HandlePEConstructionReadyL( TInt /*aCallId*/ )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneStateStartup::HandlePEConstructionReadyL() ");
    // Indicate that phone engine construction is ready
    iPEReady = ETrue;
    // Only go to the idle state if phone is also ready
    if ( iPhoneReady )
        {
        TPhoneCmdParamBoolean isSecurityMode;
        iViewCommandHandle->ExecuteCommandL( EPhoneViewGetSecurityModeStatus, &isSecurityMode );
        if ( !isSecurityMode.Boolean() && !IsSimOk() )
            {
            TPhoneCmdParamBoolean securityMode;
            securityMode.SetBoolean( ETrue );
            iViewCommandHandle->ExecuteCommandL( EPhoneViewSetSecurityMode, &securityMode );
            iCreateNote = CIdle::NewL( CActive::EPriorityHigh );
            CreateAndShowNoteAfterIdle(); 
            }
        SetDefaultFlagsL();
        iStateMachine->ChangeState( EPhoneStateIdle );
        }
    }

// -----------------------------------------------------------
// CPhoneStateStartup::HandleKeyMessageL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateStartup::HandleKeyMessageL( 
    TPhoneKeyEventMessages /*aMessage*/,
    TKeyCode /*aCode*/ )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneStateStartup::HandleKeyMessageL( ) ");
    // TBD: Panic if PE Construction fails
    // For Debug use only
    // If a key event is received in this state, either the phone engine
    // is not ready or the phone has not started up yet. Display an error note
    // if the phone engine is not ready.
    if ( iPhoneReady && !iPEReady )
        {
        SendGlobalErrorNoteL( EPhoneNoteTextPEFailedAtStartup, ETrue );
        }
    }

// -----------------------------------------------------------
// CPhoneStateStartup::HandleKeyEventL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateStartup::HandleKeyEventL(
        const TKeyEvent& /*aKeyEvent*/, TEventCode /*aEventCode*/ )
    {
    // Empty implementation
    }

// -----------------------------------------------------------
// CPhoneStateStartup::HandleDtmfKeyToneL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateStartup::HandleDtmfKeyToneL( 
        const TKeyEvent& /* aKeyEvent */, TEventCode /* aEventCode */ )
    {
    // Empty implementation
    }
    
// -----------------------------------------------------------
// CPhoneStateStartup::HandlePhoneStartupL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateStartup::HandlePhoneStartupL()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneStateStartup::HandlePhoneStartupL() ");
    // Indicate that the phone is ready
    iPhoneReady = ETrue;
    // Only go to idle state when Phone engine is also ready
    if ( iPEReady )
        {
        TPhoneCmdParamBoolean isSecurityMode;
        iViewCommandHandle->ExecuteCommandL( EPhoneViewGetSecurityModeStatus, &isSecurityMode );
        if ( !isSecurityMode.Boolean() && !IsSimOk() )
            {
            TPhoneCmdParamBoolean securityMode;
            securityMode.SetBoolean( ETrue );
            iViewCommandHandle->ExecuteCommandL( EPhoneViewSetSecurityMode, &securityMode );
            iCreateNote = CIdle::NewL( CActive::EPriorityHigh );
            CreateAndShowNoteAfterIdle(); 
            }
        SetDefaultFlagsL();
        iStateMachine->ChangeState( EPhoneStateIdle );
        }
    }

// -----------------------------------------------------------------------------
// CPhoneStateStartup::CreateAndShowNoteAfterIdle
//
// Callback function.
// -----------------------------------------------------------------------------
//
void CPhoneStateStartup::CreateAndShowNoteAfterIdle()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneStateStartup::CreateAndShowNoteAfterIdle ");
    if ( !iCreateNote->IsActive() )
        {
        iCreateNote->Start( 
            TCallBack( DoShowNoteL, this ) );
        }
    }

// -----------------------------------------------------------------------------
// CPhoneStateStartup::DoShowNoteL
//
// Callback function.
// -----------------------------------------------------------------------------
//
TInt CPhoneStateStartup::DoShowNoteL( TAny* aAny )
    {        
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneStateStartup::DoShowNoteL ");
    static_cast< CPhoneStateStartup* >( aAny )->
        StartShowSecurityNoteL();
    return KErrNone;
    }

// End of File
