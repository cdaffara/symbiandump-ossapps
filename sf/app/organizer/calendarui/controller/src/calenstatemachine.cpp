/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Calendar state machine
*
*/


// includes
#include "calendarui_debug.h"           // Debug macros

#include "calenstatemachine.h"
#include "calenstate.h"
#include "calenidlestate.h"
#include "calenpopulationstate.h"
#include "calendeletingstate.h"
#include "calenbackgroundstate.h"
#include "caleneditingstate.h"
#include "calenexitingstate.h"
#include "calenhelpstate.h"
#include "calenprintingstate.h"
#include "calensendingstate.h"
#include "calensettingsstate.h"
#include "calenviewingstate.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calenstatemachineTraces.h"
#endif


const TInt KHashLength = 64;
// ----------------------------------------------------------------------------
// CCalenStateMachine::NewL
// Two phased constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenStateMachine* CCalenStateMachine::NewL( CCalenController& aController )
    {
    OstTraceFunctionEntry0( CCALENSTATEMACHINE_NEWL_ENTRY );
    
    CCalenStateMachine* self = new ( ELeave ) CCalenStateMachine( aController );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( CCALENSTATEMACHINE_NEWL_EXIT );
    return self;
    }

 // ----------------------------------------------------------------------------
// CCalenStateMachine::CCalenStateMachine
// Constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//      
CCalenStateMachine::CCalenStateMachine( CCalenController& aController )
    : iController( aController ) , 
      iOutstandingNotifications(&::HashCalenNotificationFunction,&::HashCalenNotificationIdentityRelation)
    {
    OstTraceFunctionEntry0( CCALENSTATEMACHINE_CCALENSTATEMACHINE_ENTRY );
    
    OstTraceFunctionExit0( CCALENSTATEMACHINE_CCALENSTATEMACHINE_EXIT );
    }

// ----------------------------------------------------------------------------
// CCalenStateMachine::ConstructL
// Second phase of construction.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//        
void CCalenStateMachine::ConstructL()
    {
    OstTraceFunctionEntry0( CCALENSTATEMACHINE_CONSTRUCTL_ENTRY );
    
    CCalenState* state = NULL;

    iOutstandingNotifications.ReserveL(KHashLength);
    state = CCalenIdleState::NewLC( iController, iOutstandingNotifications );
    iStates.InsertL( state, ECalenIdleState );
    CleanupStack::Pop( state );
    
    state = CCalenPopulationState::NewLC( iController, iOutstandingNotifications);
    iStates.InsertL( state, ECalenPopulationState );
    CleanupStack::Pop( state );
    
    state = CCalenBackgroundState::NewLC( iController, iOutstandingNotifications );
    iStates.InsertL( state, ECalenBackgroundState );
    CleanupStack::Pop( state );
    
    state = CCalenViewingState::NewLC( iController, iOutstandingNotifications );
    iStates.InsertL( state, ECalenViewingState );
    CleanupStack::Pop( state );
    
    state = CCalenEditingState::NewLC( iController, iOutstandingNotifications );
    iStates.InsertL( state, ECalenEditingState );
    CleanupStack::Pop( state );
    
    state = CCalenDeletingState::NewLC( iController, iOutstandingNotifications );
    iStates.InsertL( state, ECalenDeletingState );
    CleanupStack::Pop( state );
    
    state = CCalenPrintingState::NewLC( iController, iOutstandingNotifications );
    iStates.InsertL( state, ECalenPrintingState );
    CleanupStack::Pop( state );
    
    state = CCalenSendingState::NewLC( iController, iOutstandingNotifications );
    iStates.InsertL( state, ECalenSendingState );
    CleanupStack::Pop( state );
    
    state = CCalenSettingsState::NewLC( iController, iOutstandingNotifications );
    iStates.InsertL( state, ECalenSettingsState );
    CleanupStack::Pop( state );
    
    state = CCalenHelpState::NewLC( iController, iOutstandingNotifications );
    iStates.InsertL( state, ECalenHelpState );
    CleanupStack::Pop( state );

    state = CCalenExitingState::NewLC( iController, iOutstandingNotifications );
    iStates.InsertL( state, ECalenExitingState );
    CleanupStack::Pop( state );

    ASSERT( iStates.Count() == KCalenLastState );

    OstTraceFunctionExit0( CCALENSTATEMACHINE_CONSTRUCTL_EXIT );
    }

// ----------------------------------------------------------------------------
// CCalenStateMachine::~CCalenStateMachine
// Destructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenStateMachine::~CCalenStateMachine()
    {
    OstTraceFunctionEntry0( DUP1_CCALENSTATEMACHINE_CCALENSTATEMACHINE_ENTRY );
    
    iOutstandingNotifications.Close();
    iStates.ResetAndDestroy();

    OstTraceFunctionExit0( DUP1_CCALENSTATEMACHINE_CCALENSTATEMACHINE_EXIT );
    }     

// ----------------------------------------------------------------------------
// CCalenStateMachine::HandleNotification
// Notification handler interface.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenStateMachine::HandleNotification(const TCalenNotification aNotification )
    {
    OstTraceFunctionEntry0( CCALENSTATEMACHINE_HANDLENOTIFICATION_ENTRY );
    
	TRAP_IGNORE(iStates[iCurrentState]->HandleNotificationL( aNotification, *this)); 
   
    OstTraceFunctionExit0( CCALENSTATEMACHINE_HANDLENOTIFICATION_EXIT );
    }

// ----------------------------------------------------------------------------
// CCalenStateMachine::CurrentState
// Returns the current state in which calendar is.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenStateMachine::TCalenStateIndex CCalenStateMachine::CurrentState()
{
    OstTraceFunctionEntry0( CCALENSTATEMACHINE_CURRENTSTATE_ENTRY );
    
	OstTraceFunctionExit0( CCALENSTATEMACHINE_CURRENTSTATE_EXIT );
	return iCurrentState;
}

// ----------------------------------------------------------------------------
// CCalenStateMachine::HandleCommandL
// Command handler interface.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenStateMachine::HandleCommandL( const TCalenCommand& aCommand )
    {
    OstTraceFunctionEntry0( CCALENSTATEMACHINE_HANDLECOMMANDL_ENTRY );
    
    TBool cmdUsed = iStates[iCurrentState]->HandleCommandL( aCommand, *this);
    
    OstTraceFunctionExit0( CCALENSTATEMACHINE_HANDLECOMMANDL_EXIT );
    return cmdUsed;
    }

// ----------------------------------------------------------------------------
// CCalenStateMachine::CancelPreviousCmd
// Interface to cancel the previous command
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenStateMachine::CancelPreviousCmd()
	{
    OstTraceFunctionEntry0( CCALENSTATEMACHINE_CANCELPREVIOUSCMD_ENTRY );
    
	TCalenStateIndex previousState = iStates[iCurrentState]->PreviousState();
	iStates[iCurrentState]->CancelExecutingCmd();
	
	if(previousState != iCurrentState)
		{
		iCurrentState = previousState;
		}
	
	OstTraceFunctionExit0( CCALENSTATEMACHINE_CANCELPREVIOUSCMD_EXIT );
	}

// ----------------------------------------------------------------------------
// CCalenStateMachine::GetCurrentState
// Interface to get the current state
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//	
CCalenStateMachine::TCalenStateIndex CCalenStateMachine::GetCurrentState()
	{
    OstTraceFunctionEntry0( CCALENSTATEMACHINE_GETCURRENTSTATE_ENTRY );
    
    OstTraceFunctionExit0( CCALENSTATEMACHINE_GETCURRENTSTATE_EXIT );
    return iCurrentState;
	}

// ----------------------------------------------------------------------------
// CCalenStateMachine::SetCurrentState
// Interface to set the current state
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//		
void CCalenStateMachine::SetCurrentState(const TCalenStateIndex& aState)
	{
    OstTraceFunctionEntry0( CCALENSTATEMACHINE_SETCURRENTSTATE_ENTRY );
    
	iCurrentState = aState;
	
	OstTraceFunctionExit0( CCALENSTATEMACHINE_SETCURRENTSTATE_EXIT );
	}

// ----------------------------------------------------------------------------
// CCalenStateMachine::SetPreviousState
// Interface to set previous state
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenStateMachine::SetCurrentPreviousState(const TCalenStateIndex& aState)
	{
    OstTraceFunctionEntry0( CCALENSTATEMACHINE_SETCURRENTPREVIOUSSTATE_ENTRY );
    
	iStates[iCurrentState]->SetPreviousState(aState);
	
	OstTraceFunctionExit0( CCALENSTATEMACHINE_SETCURRENTPREVIOUSSTATE_EXIT );
	}

// ----------------------------------------------------------------------------
// CCalenStateMachine::ActivateCurrentStateL
// Interface to activate the current state
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//	
void CCalenStateMachine::ActivateCurrentStateL()
	{
    OstTraceFunctionEntry0( CCALENSTATEMACHINE_ACTIVATECURRENTSTATEL_ENTRY );
    
	iStates[iCurrentState]->HandleStateActivationL(*this);
		
	OstTraceFunctionExit0( CCALENSTATEMACHINE_ACTIVATECURRENTSTATEL_EXIT );
	}
// End of file
