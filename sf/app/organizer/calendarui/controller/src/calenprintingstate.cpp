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
#include "calenprintingstate.h"
#include "calendarui_debug.h"           // Debug macros
#include "calencontroller.h"
#include "calenstatemachine.h"
#include "calennotifier.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calenprintingstateTraces.h"
#endif

// ----------------------------------------------------------------------------
// CCalenPrintingState::NewLC
// First stage construction
// ----------------------------------------------------------------------------
CCalenPrintingState* CCalenPrintingState::NewLC( CCalenController& aController,
                                                        RHashSet<TCalenNotification>& aOutstandingNotifications )
    {
    OstTraceFunctionEntry0( CCALENPRINTINGSTATE_NEWLC_ENTRY );
    
    CCalenPrintingState* self = new ( ELeave ) CCalenPrintingState( aController,aOutstandingNotifications );
    CleanupStack::PushL( self );
    self->ConstructL();

    OstTraceFunctionExit0( CCALENPRINTINGSTATE_NEWLC_EXIT );
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenPrintingState::ConstructL
// Second stage construction
// ----------------------------------------------------------------------------
void CCalenPrintingState::ConstructL()
    {
    OstTraceFunctionEntry0( CCALENPRINTINGSTATE_CONSTRUCTL_ENTRY );
    
    BaseConstructL();    
    
    OstTraceFunctionExit0( CCALENPRINTINGSTATE_CONSTRUCTL_EXIT );
    }
    
// ----------------------------------------------------------------------------
// CCalenPrintingState::CCalenPrintingState
// C++ Constructor
// ----------------------------------------------------------------------------
CCalenPrintingState::CCalenPrintingState( CCalenController& aController,
                                                RHashSet<TCalenNotification>& aOutstandingNotifications )
    : CCalenState( aController, aOutstandingNotifications )
    {
    OstTraceFunctionEntry0( CCALENPRINTINGSTATE_CCALENPRINTINGSTATE_ENTRY );
    
    OstTraceFunctionExit0( CCALENPRINTINGSTATE_CCALENPRINTINGSTATE_EXIT );
    }
    
// ----------------------------------------------------------------------------
// CCalenPrintingState::CCalenPrintingState
// Destructor
// ----------------------------------------------------------------------------    
CCalenPrintingState::~CCalenPrintingState()
    {
    OstTraceFunctionEntry0( DUP1_CCALENPRINTINGSTATE_CCALENPRINTINGSTATE_ENTRY );
    
    OstTraceFunctionExit0( DUP1_CCALENPRINTINGSTATE_CCALENPRINTINGSTATE_EXIT );
    }

// ----------------------------------------------------------------------------
// CCalenPrintingState::HandleCommandL
// From CCalenState
// ----------------------------------------------------------------------------    
TBool CCalenPrintingState::HandleCommandL( const TCalenCommand& aCommand,
                                          CCalenStateMachine& aStateMachine )
    {
    OstTraceFunctionEntry0( CCALENPRINTINGSTATE_HANDLECOMMANDL_ENTRY );
    
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

    OstTraceFunctionExit0( CCALENPRINTINGSTATE_HANDLECOMMANDL_EXIT );
    return cmdUsed;
    }

// ----------------------------------------------------------------------------
// CCalenPrintingState::HandleNotificationL
// From CCalenState
// ----------------------------------------------------------------------------        
void CCalenPrintingState::HandleNotificationL(const TCalenNotification& /*aNotification*/,
                                               CCalenStateMachine& /*aStateMachine*/ )
    {
    OstTraceFunctionEntry0( CCALENPRINTINGSTATE_HANDLENOTIFICATIONL_ENTRY );
    
    OstTraceFunctionExit0( CCALENPRINTINGSTATE_HANDLENOTIFICATIONL_EXIT );
    }

// end of file
