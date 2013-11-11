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
#include "calenpopulationstate.h"
#include "calendarui_debug.h"           // Debug macros
#include "calencontroller.h"
#include "calenstatemachine.h"
#include "calennotifier.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calenpopulationstateTraces.h"
#endif

// ----------------------------------------------------------------------------
// CalenPopulationState::NewLC
// First stage construction
// ----------------------------------------------------------------------------
CCalenPopulationState* CCalenPopulationState::NewLC( CCalenController& aController,
                                                    RHashSet<TCalenNotification>& aOutstandingNotifications )
    {
    OstTraceFunctionEntry0( CCALENPOPULATIONSTATE_NEWLC_ENTRY );
    
    CCalenPopulationState* self = new( ELeave ) CCalenPopulationState( aController, aOutstandingNotifications );
    CleanupStack::PushL( self );
    self->ConstructL();

    OstTraceFunctionExit0( CCALENPOPULATIONSTATE_NEWLC_EXIT );
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenPopulationState::ConstructL
// Second stage construction
// ----------------------------------------------------------------------------
void CCalenPopulationState::ConstructL()
    {
    OstTraceFunctionEntry0( CCALENPOPULATIONSTATE_CONSTRUCTL_ENTRY );
    
    BaseConstructL();   
    
    OstTraceFunctionExit0( CCALENPOPULATIONSTATE_CONSTRUCTL_EXIT );
    }
    
// ----------------------------------------------------------------------------
// CCalenPopulationState::CCalenPopulationState
// C++ Constructor
// ----------------------------------------------------------------------------
CCalenPopulationState::CCalenPopulationState( CCalenController& aController,
                                                          RHashSet<TCalenNotification>&  aOutstandingNotifications )
    : CCalenState( aController, aOutstandingNotifications )
    {
    OstTraceFunctionEntry0( CCALENPOPULATIONSTATE_CCALENPOPULATIONSTATE_ENTRY );
    
    OstTraceFunctionExit0( CCALENPOPULATIONSTATE_CCALENPOPULATIONSTATE_EXIT );
    }
    
// ----------------------------------------------------------------------------
// CCalenPopulationState::CCalenPopulationState
// Destructor
// ----------------------------------------------------------------------------    
CCalenPopulationState::~CCalenPopulationState()
    {
    OstTraceFunctionEntry0( DUP1_CCALENPOPULATIONSTATE_CCALENPOPULATIONSTATE_ENTRY );
    
    OstTraceFunctionExit0( DUP1_CCALENPOPULATIONSTATE_CCALENPOPULATIONSTATE_EXIT );
    }

// ----------------------------------------------------------------------------
// CCalenPopulationState::HandleCommandL
// From CCalenState
// ----------------------------------------------------------------------------    
TBool CCalenPopulationState::HandleCommandL( const TCalenCommand& aCommand,
                                            CCalenStateMachine& aStateMachine )
    {
    OstTraceFunctionEntry0( CCALENPOPULATIONSTATE_HANDLECOMMANDL_ENTRY );
    
    TInt cmd = aCommand.Command();
    MCalenCommandHandler* handler = iController.GetCommandHandlerL( cmd );
    
    ASSERT( handler ); // FIXME: error ui
    
    TBool cmdUsed = EFalse;
    
    if( ECalenFasterAppExit == cmd )
        {
        SetCurrentState( aStateMachine, CCalenStateMachine::ECalenIdleState );
        ActivateCurrentStateL(aStateMachine);
        RequestCallbackL( handler, aCommand );
        cmdUsed = ETrue;
        }

    OstTraceFunctionExit0( CCALENPOPULATIONSTATE_HANDLECOMMANDL_EXIT );
    return cmdUsed;
    }

// ----------------------------------------------------------------------------
// CCalenPopulationState::HandleNotificationL
// From CCalenState
// ----------------------------------------------------------------------------        
void CCalenPopulationState::HandleNotificationL(const TCalenNotification& aNotification,
                                                  CCalenStateMachine& aStateMachine )
    {
    OstTraceFunctionEntry0( CCALENPOPULATIONSTATE_HANDLENOTIFICATIONL_ENTRY );
    
    switch ( aNotification )
        {
        case ECalenNotifyViewPopulationComplete:
            {
            SetCurrentState( aStateMachine, iPreviousState );
            // don't set us as the previous state for the state we're moving back to.
            iOutstandingNotifications.InsertL(aNotification);
            ActivateCurrentStateL(aStateMachine);
            }
            break;
        default:
            CCalenState::HandleNotificationL( aNotification, aStateMachine );
            break;
        }
    
    OstTraceFunctionExit0( CCALENPOPULATIONSTATE_HANDLENOTIFICATIONL_EXIT );
    }


 // end of file
