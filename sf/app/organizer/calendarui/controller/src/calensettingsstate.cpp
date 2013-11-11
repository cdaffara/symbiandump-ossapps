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
#include "calensettingsstate.h"
#include "calendarui_debug.h"           // Debug macros
#include "calencontroller.h"
#include "calenstatemachine.h"
#include "calennotifier.h"
#include "calenviewmanager.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calensettingsstateTraces.h"
#endif

// ----------------------------------------------------------------------------
// CCalenSettingsState::NewLC
// First stage construction
// ----------------------------------------------------------------------------
CCalenSettingsState* CCalenSettingsState::NewLC( CCalenController& aController,
                    RHashSet<TCalenNotification>& aOutstandingNotifications )
    {
    OstTraceFunctionEntry0( CCALENSETTINGSSTATE_NEWLC_ENTRY );
    
    CCalenSettingsState* self = new ( ELeave ) CCalenSettingsState( aController,
                                                    aOutstandingNotifications );
    CleanupStack::PushL( self );
    self->ConstructL();

    OstTraceFunctionExit0( CCALENSETTINGSSTATE_NEWLC_EXIT );
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsState::ConstructL
// Second stage construction
// ----------------------------------------------------------------------------
void CCalenSettingsState::ConstructL()
    {
    OstTraceFunctionEntry0( CCALENSETTINGSSTATE_CONSTRUCTL_ENTRY );
    
    BaseConstructL();
    
    OstTraceFunctionExit0( CCALENSETTINGSSTATE_CONSTRUCTL_EXIT );
    }
    
// ----------------------------------------------------------------------------
// CCalenSettingsState::CCalenSettingsState
// C++ Constructor
// ----------------------------------------------------------------------------
CCalenSettingsState::CCalenSettingsState( CCalenController& aController,
                    RHashSet<TCalenNotification>& aOutstandingNotifications )
    : CCalenState( aController, aOutstandingNotifications )
    {
    OstTraceFunctionEntry0( CCALENSETTINGSSTATE_CCALENSETTINGSSTATE_ENTRY );
    
    OstTraceFunctionExit0( CCALENSETTINGSSTATE_CCALENSETTINGSSTATE_EXIT );
    }
    
// ----------------------------------------------------------------------------
// CCalenSettingsState::CCalenSettingsState
// Destructor
// ----------------------------------------------------------------------------    
CCalenSettingsState::~CCalenSettingsState()
    {
    OstTraceFunctionEntry0( DUP1_CCALENSETTINGSSTATE_CCALENSETTINGSSTATE_ENTRY );
    
    OstTraceFunctionExit0( DUP1_CCALENSETTINGSSTATE_CCALENSETTINGSSTATE_EXIT );
    }

// ----------------------------------------------------------------------------
// CCalenSettingsState::HandleCommandL
// From CCalenState
// ----------------------------------------------------------------------------    
TBool CCalenSettingsState::HandleCommandL( const TCalenCommand& aCommand,
                                        CCalenStateMachine& aStateMachine  )
    {
    OstTraceFunctionEntry0( CCALENSETTINGSSTATE_HANDLECOMMANDL_ENTRY );
    
    TInt cmd = aCommand.Command();
    MCalenCommandHandler* handler = iController.GetCommandHandlerL( cmd );
	    
    ASSERT( handler ); 
    TBool cmdUsed = EFalse;
	    
    switch( cmd )
        {
        case ECalenFasterAppExit:
            {
            SetCurrentState( aStateMachine, CCalenStateMachine::ECalenIdleState );
            ActivateCurrentStateL(aStateMachine);
            cmdUsed = ETrue;
            }
            break;
        case ECalenMonthView:
        case ECalenAgendaView:
        case ECalenDayView:
        case ECalenStartActiveStep:
            {
            // set previous state to idle
            CCalenStateMachine::TCalenStateIndex cachedState = CCalenStateMachine::ECalenIdleState;
            SetCurrentState( aStateMachine, CCalenStateMachine::ECalenPopulationState );
            SetCurrentPreviousState( aStateMachine, cachedState );
            ActivateCurrentStateL(aStateMachine);               
            cmdUsed = ETrue;
            }
            break;
        default:
            break;
        }
    if(cmdUsed)
        {
        RequestCallbackL( handler, aCommand );
        }

    OstTraceFunctionExit0( CCALENSETTINGSSTATE_HANDLECOMMANDL_EXIT );
    return cmdUsed;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsState::HandleNotificationL
// From CCalenState
// ----------------------------------------------------------------------------        
void CCalenSettingsState::HandleNotificationL(const TCalenNotification& aNotification,
                                               CCalenStateMachine& aStateMachine )
    {
    OstTraceFunctionEntry0( CCALENSETTINGSSTATE_HANDLENOTIFICATIONL_ENTRY );
    
    switch( aNotification )
        {
        case ECalenNotifyPluginEnabledDisabled:
            {
			 CCalenState::HandleNotificationL( aNotification, aStateMachine );
            }
            break;
        case ECalenNotifySettingsClosed:
            {
            // Settings closed, go back to previous state.
            SetCurrentState( aStateMachine, iPreviousState );
            iOutstandingNotifications.InsertL(aNotification);
            ActivateCurrentStateL(aStateMachine);
            }
            break;
        case ECalenNotifySystemLocaleChanged:
            {
            CCalenState::HandleNotificationL( aNotification, aStateMachine );
            }
            break;
        default:
            // default is defer all other notifications when we are in setting state
            iOutstandingNotifications.InsertL(aNotification);
            break;
        }
    
    OstTraceFunctionExit0( CCALENSETTINGSSTATE_HANDLENOTIFICATIONL_EXIT );
    }

// ----------------------------------------------------------------------------
// CCalenSettingsState::HandleStateActivationL
// Behavior when state is activated.
// ----------------------------------------------------------------------------
void CCalenSettingsState::HandleStateActivationL()
    {
    OstTraceFunctionEntry0( CCALENSETTINGSSTATE_HANDLESTATEACTIVATIONL_ENTRY );
    
    if( iOutstandingNotifications.FindL(ECalenNotifySettingsCRepKeyChanged) )
        {
        iController.Notifier().BroadcastApprovedNotification( ECalenNotifySettingsCRepKeyChanged );
        }
    
    OstTraceFunctionExit0( CCALENSETTINGSSTATE_HANDLESTATEACTIVATIONL_EXIT );
    }

 // end of file
