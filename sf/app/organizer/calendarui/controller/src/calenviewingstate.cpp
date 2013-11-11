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
* Description:  Calendar state machine
*
*/


// includes
#include "calenviewingstate.h"
#include "calendarui_debug.h"           // Debug macros
#include "calencontroller.h"
#include "calenstatemachine.h"
#include "calennotifier.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calenviewingstateTraces.h"
#endif

// ----------------------------------------------------------------------------
// CCalenViewingState::NewLC
// First stage construction
// ----------------------------------------------------------------------------
CCalenViewingState* CCalenViewingState::NewLC( CCalenController& aController,
                                                   RHashSet<TCalenNotification>& aOutstandingNotifications )
    {
    OstTraceFunctionEntry0( CCALENVIEWINGSTATE_NEWLC_ENTRY );
    
    CCalenViewingState* self = new ( ELeave ) CCalenViewingState( aController,aOutstandingNotifications );
    CleanupStack::PushL( self );
    self->ConstructL();

    OstTraceFunctionExit0( CCALENVIEWINGSTATE_NEWLC_EXIT );
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenViewingState::ConstructL
// Second stage construction
// ----------------------------------------------------------------------------
void CCalenViewingState::ConstructL()
    {
    OstTraceFunctionEntry0( CCALENVIEWINGSTATE_CONSTRUCTL_ENTRY );
    
    BaseConstructL();
    
    OstTraceFunctionExit0( CCALENVIEWINGSTATE_CONSTRUCTL_EXIT );
    }
    
// ----------------------------------------------------------------------------
// CCalenIdleState::CCalenIdleState
// C++ Constructor
// ----------------------------------------------------------------------------
CCalenViewingState::CCalenViewingState( CCalenController& aController,
                                                    RHashSet<TCalenNotification>& aOutstandingNotifications )
    : CCalenState( aController, aOutstandingNotifications )
    {
    OstTraceFunctionEntry0( CCALENVIEWINGSTATE_CCALENVIEWINGSTATE_ENTRY );
    
    OstTraceFunctionExit0( CCALENVIEWINGSTATE_CCALENVIEWINGSTATE_EXIT );
    }
    
// ----------------------------------------------------------------------------
// CCalenViewingState::CCalenViewingState
// Destructor
// ----------------------------------------------------------------------------    
CCalenViewingState::~CCalenViewingState()
    {
    OstTraceFunctionEntry0( DUP1_CCALENVIEWINGSTATE_CCALENVIEWINGSTATE_ENTRY );
    
    OstTraceFunctionExit0( DUP1_CCALENVIEWINGSTATE_CCALENVIEWINGSTATE_EXIT );
    }

// ----------------------------------------------------------------------------
// CCalenViewingState::HandleCommandL
// From CCalenState
// ----------------------------------------------------------------------------    
TBool CCalenViewingState::HandleCommandL( const TCalenCommand& aCommand,
                                        CCalenStateMachine& aStateMachine )
    {
    OstTraceFunctionEntry0( CCALENVIEWINGSTATE_HANDLECOMMANDL_ENTRY );
    
    TInt cmd = aCommand.Command();
    MCalenCommandHandler* handler = iController.GetCommandHandlerL( cmd );
    
    ASSERT( handler ); // FIXME: error ui
    
    TBool cmdUsed = EFalse;

    switch( cmd )
        {
        case ECalenEditCurrentEntry:
	    case ECalenEditOccurrence:
	    case ECalenEditSeries:
        case ECalenRestoreTodo:
        case ECalenCompleteTodo:
        case ECalenEditEntryFromViewer:
      		{
    		// set the previous state as viewing state
    		CCalenStateMachine::TCalenStateIndex cachedState = GetCurrentState(aStateMachine);
            SetCurrentState( aStateMachine, CCalenStateMachine::ECalenEditingState);
            SetCurrentPreviousState( aStateMachine, cachedState );
            ActivateCurrentStateL(aStateMachine);
            cmdUsed = ETrue;
    		}
    		break;	    
		case ECalenSend:
			{
			// set the previous state as viewing state
    		CCalenStateMachine::TCalenStateIndex cachedState = GetCurrentState(aStateMachine);
            SetCurrentState( aStateMachine, CCalenStateMachine::ECalenSendingState);
            SetCurrentPreviousState( aStateMachine, cachedState );
            ActivateCurrentStateL(aStateMachine);
            cmdUsed = ETrue;
			}
			break;	    		
        case ECalenDeleteCurrentEntry:
        case ECalenDeleteSeries:
        case ECalenDeleteCurrentOccurrence:
        case ECalenDeleteEntryFromViewer:
    		{    
			// set the previous state as viewing state
            CCalenStateMachine::TCalenStateIndex cachedState = GetCurrentState(aStateMachine);
            SetCurrentState( aStateMachine, CCalenStateMachine::ECalenDeletingState);
            SetCurrentPreviousState( aStateMachine, cachedState );
            ActivateCurrentStateL(aStateMachine);
            cmdUsed = ETrue;
        	}
            break;
        case ECalenFasterAppExit:
        	{
	        SetCurrentState( aStateMachine, CCalenStateMachine::ECalenIdleState );
	        ActivateCurrentStateL(aStateMachine);
        	cmdUsed = ETrue;
        	}
			break;
		case ECalenGetLocation:
    	case ECalenShowLocation:
        case ECalenGetLocationAndSave:
    		{    		
    		}
        }
    RequestCallbackL( handler, aCommand );

    OstTraceFunctionExit0( CCALENVIEWINGSTATE_HANDLECOMMANDL_EXIT );
    return cmdUsed;
    }

// ----------------------------------------------------------------------------
// CCalenViewingState::HandleNotificationL
// From CCalenState
// ----------------------------------------------------------------------------        
void CCalenViewingState::HandleNotificationL(const TCalenNotification& aNotification,
                                              CCalenStateMachine& aStateMachine )
    {
    OstTraceFunctionEntry0( CCALENVIEWINGSTATE_HANDLENOTIFICATIONL_ENTRY );
    
    switch( aNotification )
        {
        case ECalenNotifyEntryClosed:
        	{
        	SetCurrentState( aStateMachine, iPreviousState );
            iOutstandingNotifications.InsertL(aNotification);
            ActivateCurrentStateL(aStateMachine);
        	}
	        break;
        
        case ECalenNotifyEntryDeleted:
        	{
        	SetCurrentState( aStateMachine, iPreviousState );
        	iOutstandingNotifications.InsertL(aNotification);
            ActivateCurrentStateL(aStateMachine);
       		}
       		break;
        default:
            CCalenState::HandleNotificationL( aNotification, aStateMachine );
            break;
        }
    
    OstTraceFunctionExit0( CCALENVIEWINGSTATE_HANDLENOTIFICATIONL_EXIT );
    } 
    
// ----------------------------------------------------------------------------
// CCalenViewingState::HandleStateActivationL
// Behavior when state is activated.
// ----------------------------------------------------------------------------
void CCalenViewingState::HandleStateActivationL(CCalenStateMachine& aStateMachine)
    {
    OstTraceFunctionEntry0( CCALENVIEWINGSTATE_HANDLESTATEACTIVATIONL_ENTRY );
    
    if( iOutstandingNotifications.Find(ECalenNotifyEntryDeleted) )
    	{
    	SetCurrentState( aStateMachine, iPreviousState );
        ActivateCurrentStateL(aStateMachine);
    	}
    else 
    	{
    	CCalenState::HandleStateActivationL(aStateMachine);
    	}
    
    OstTraceFunctionExit0( CCALENVIEWINGSTATE_HANDLESTATEACTIVATIONL_EXIT );
    }    
    
 // end of file
