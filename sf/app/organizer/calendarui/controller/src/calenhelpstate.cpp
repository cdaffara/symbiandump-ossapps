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
#include "calenhelpstate.h"
#include "calendarui_debug.h"           // Debug macros
#include "calencontroller.h"
#include "calenstatemachine.h"
#include "calennotifier.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calenhelpstateTraces.h"
#endif

// ----------------------------------------------------------------------------
// CCalenHelpState::NewLC
// First stage construction
// ----------------------------------------------------------------------------
CCalenHelpState* CCalenHelpState::NewLC( CCalenController& aController,
                                                RHashSet<TCalenNotification>&  aOutstandingNotifications )
    {
    OstTraceFunctionEntry0( CCALENHELPSTATE_NEWLC_ENTRY );
    
    CCalenHelpState* self = new ( ELeave ) CCalenHelpState( aController, aOutstandingNotifications );
    CleanupStack::PushL( self );
    self->ConstructL();

    OstTraceFunctionExit0( CCALENHELPSTATE_NEWLC_EXIT );
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenHelpState::ConstructL
// Second stage construction
// ----------------------------------------------------------------------------
void CCalenHelpState::ConstructL()
    {
    OstTraceFunctionEntry0( CCALENHELPSTATE_CONSTRUCTL_ENTRY );
    
    BaseConstructL();
    
    OstTraceFunctionExit0( CCALENHELPSTATE_CONSTRUCTL_EXIT );
    }
    
// ----------------------------------------------------------------------------
// CCalenHelpState::CCalenHelpState
// C++ Constructor
// ----------------------------------------------------------------------------
CCalenHelpState::CCalenHelpState( CCalenController& aController,
                                             RHashSet<TCalenNotification>&  aOutstandingNotifications )
    : CCalenState( aController, aOutstandingNotifications )
    {
    OstTraceFunctionEntry0( CCALENHELPSTATE_CCALENHELPSTATE_ENTRY );
    
    OstTraceFunctionExit0( CCALENHELPSTATE_CCALENHELPSTATE_EXIT );
    }
    
// ----------------------------------------------------------------------------
// CCalenHelpState::CCalenHelpState
// Destructor
// ----------------------------------------------------------------------------    
CCalenHelpState::~CCalenHelpState()
    {
    OstTraceFunctionEntry0( DUP1_CCALENHELPSTATE_CCALENHELPSTATE_ENTRY );

    OstTraceFunctionExit0( DUP1_CCALENHELPSTATE_CCALENHELPSTATE_EXIT );
    }

// ----------------------------------------------------------------------------
// CCalenHelpState::HandleCommandL
// From CCalenState
// ----------------------------------------------------------------------------    
TBool CCalenHelpState::HandleCommandL( const TCalenCommand& aCommand,
                                   CCalenStateMachine& aStateMachine )
    {
    OstTraceFunctionEntry0( CCALENHELPSTATE_HANDLECOMMANDL_ENTRY );
    
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

    OstTraceFunctionExit0( CCALENHELPSTATE_HANDLECOMMANDL_EXIT );
    return cmdUsed;

    }

// ----------------------------------------------------------------------------
// CCalenHelpState::HandleNotificationL
// From CCalenState
// ----------------------------------------------------------------------------        
void CCalenHelpState::HandleNotificationL(const TCalenNotification& /*aNotification*/,
                                          CCalenStateMachine& /*aStateMachine*/ )
    {
    OstTraceFunctionEntry0( CCALENHELPSTATE_HANDLENOTIFICATIONL_ENTRY );
    
    OstTraceFunctionExit0( CCALENHELPSTATE_HANDLENOTIFICATIONL_EXIT );
    }



 // end of file
