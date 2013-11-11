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
#include "calenbackgroundstate.h"
#include "calendarui_debug.h"           // Debug macros
#include "calencontroller.h"
#include "calenstatemachine.h"
#include "calennotifier.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calenbackgroundstateTraces.h"
#endif

// ----------------------------------------------------------------------------
// CCalenBackgroundState::NewLC
// First stage construction
// ----------------------------------------------------------------------------
CCalenBackgroundState* CCalenBackgroundState::NewLC( CCalenController& aController,
                                                            RHashSet<TCalenNotification>&  aOutstandingNotifications )
    {
    OstTraceFunctionEntry0( CCALENBACKGROUNDSTATE_NEWLC_ENTRY );
    
    CCalenBackgroundState* self = new ( ELeave ) CCalenBackgroundState( aController,aOutstandingNotifications );
    CleanupStack::PushL( self );
    self->ConstructL();

    OstTraceFunctionExit0( CCALENBACKGROUNDSTATE_NEWLC_EXIT );
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenBackgroundState::ConstructL
// Second stage construction
// ----------------------------------------------------------------------------
void CCalenBackgroundState::ConstructL()
    {
    OstTraceFunctionEntry0( CCALENBACKGROUNDSTATE_CONSTRUCTL_ENTRY );
    
    BaseConstructL();
    
    OstTraceFunctionExit0( CCALENBACKGROUNDSTATE_CONSTRUCTL_EXIT );
    }
    
// ----------------------------------------------------------------------------
// CCalenBackgroundState::CCalenBackgroundState
// C++ Constructor
// ----------------------------------------------------------------------------
CCalenBackgroundState::CCalenBackgroundState( CCalenController& aController, 
                                                                RHashSet<TCalenNotification>&  aOutstandingNotifications )
    : CCalenState( aController, aOutstandingNotifications )
    {
    OstTraceFunctionEntry0( CCALENBACKGROUNDSTATE_CCALENBACKGROUNDSTATE_ENTRY );
    
    OstTraceFunctionExit0( CCALENBACKGROUNDSTATE_CCALENBACKGROUNDSTATE_EXIT );
    }
    
// ----------------------------------------------------------------------------
// CCalenBackgroundState::CCalenBackgroundState
// Destructor
// ----------------------------------------------------------------------------    
CCalenBackgroundState::~CCalenBackgroundState()
    {
    OstTraceFunctionEntry0( DUP1_CCALENBACKGROUNDSTATE_CCALENBACKGROUNDSTATE_ENTRY );
    
    OstTraceFunctionExit0( DUP1_CCALENBACKGROUNDSTATE_CCALENBACKGROUNDSTATE_EXIT );
    }

// ----------------------------------------------------------------------------
// CCalenBackgroundState::HandleCommandL
// From CCalenState
// ----------------------------------------------------------------------------    
TBool CCalenBackgroundState::HandleCommandL( const TCalenCommand& aCommand,
                                           CCalenStateMachine& aStateMachine )
    {
    OstTraceFunctionEntry0( CCALENBACKGROUNDSTATE_HANDLECOMMANDL_ENTRY );
      
    
    //When application is pushed background
    //1,when launched from active idle ,( AI screen or missed alarm softnotification/small indicator),
    //  multiple entires -> Day view
    //2,when launched from active idle ( AI screen or missed alarm softnotification/small indicator),
    //  single entry -> Event view
    
    TInt cmd = aCommand.Command();
    MCalenCommandHandler* handler = iController.GetCommandHandlerL( cmd );
    
    ASSERT( handler ); // FIXME: error ui
    
    TBool cmdUsed = EFalse;

    switch( cmd )
        {
        case ECalenAgendaView:
        case ECalenDayView:
            SetCurrentState(aStateMachine,CCalenStateMachine::ECalenIdleState);
            cmdUsed = ETrue;
            break;
        case ECalenEventView:
            SetCurrentState(aStateMachine,CCalenStateMachine::ECalenViewingState);
            cmdUsed = ETrue;
            break;
        case ECalenFasterAppExit:        	
			if(iPreviousState == CCalenStateMachine::ECalenDeletingState)
        	    {
                // Issue ECalenNotifyDeleteFailed notification to cancel the delete
                iController.BroadcastNotification(ECalenNotifyDeleteFailed);
                SetCurrentState( aStateMachine, CCalenStateMachine::ECalenIdleState );
                ActivateCurrentStateL(aStateMachine);
                cmdUsed = ETrue;                 
        	    }
        	else
				{
				if( iPreviousState == CCalenStateMachine::ECalenSettingsState
        	       || iPreviousState == CCalenStateMachine::ECalenEditingState
        	       || iPreviousState == CCalenStateMachine::ECalenSendingState )
	        	    {
	                SetCurrentState( aStateMachine, CCalenStateMachine::ECalenIdleState );
	                ActivateCurrentStateL(aStateMachine);
	                cmdUsed = ETrue;                 
	        	    }
				}
        default:
            break;
        }
    
    RequestCallbackL( handler, aCommand );
    
    OstTraceFunctionExit0( CCALENBACKGROUNDSTATE_HANDLECOMMANDL_EXIT );
    return cmdUsed;
    }

// ----------------------------------------------------------------------------
// CCalenBackgroundState::HandleNotificationL
// From CCalenState
// ----------------------------------------------------------------------------        
void CCalenBackgroundState::HandleNotificationL(const TCalenNotification& aNotification,
                                                 CCalenStateMachine& aStateMachine )
    {
    OstTraceFunctionEntry0( CCALENBACKGROUNDSTATE_HANDLENOTIFICATIONL_ENTRY );
    
    switch( aNotification )
        {
        case ECalenNotifyAppForegrounded:
        	{
        	SetCurrentState( aStateMachine, iPreviousState );
        	// We set iOutstandingNotifications for two reasons.
        	// 1. The new state i.e. the state we are moving back can have notification info.
        	// 2. When we move to newstate we broadcast all notification from iOutstandingNotifications
        	//     and inform the registered notifiers.
        	iOutstandingNotifications.InsertL(aNotification);
            ActivateCurrentStateL(aStateMachine);
        	}
            break;
        case ECalenNotifyAppBackgrounded:
            // may receive more background notifications, just ignore
            break;
        default:
            // default is defer all other notifications when app in background
            // iOutstandingNotifications.InsertL(aNotification);
            iController.Notifier().BroadcastApprovedNotification( aNotification );
            break;
        }
    
    OstTraceFunctionExit0( CCALENBACKGROUNDSTATE_HANDLENOTIFICATIONL_EXIT );
    }

// ----------------------------------------------------------------------------
// CCalenBackgroundState::HandleStateActivationL
// Behavior when state is activated.
// ----------------------------------------------------------------------------
void CCalenBackgroundState::HandleStateActivationL(CCalenStateMachine& aStateMachine)
    {
    OstTraceFunctionEntry0( CCALENBACKGROUNDSTATE_HANDLESTATEACTIVATIONL_ENTRY );
    
    if(iOutstandingNotifications.Find(ECalenNotifyAppBackgrounded))
        {
        CCalenState::HandleStateActivationL(aStateMachine);
        }
    
    OstTraceFunctionExit0( CCALENBACKGROUNDSTATE_HANDLESTATEACTIVATIONL_EXIT );
    }

 // end of file
