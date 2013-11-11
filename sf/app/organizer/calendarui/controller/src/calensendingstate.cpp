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
#include "calensendingstate.h"
#include "calendarui_debug.h"           // Debug macros
#include "calencontroller.h"
#include "calenstatemachine.h"
#include "calennotifier.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calensendingstateTraces.h"
#endif

// ----------------------------------------------------------------------------
// CCalenSendingState::NewLC
// First stage construction
// ----------------------------------------------------------------------------
CCalenSendingState* CCalenSendingState::NewLC( CCalenController& aController,
                                                    RHashSet<TCalenNotification>&  aOutstandingNotifications )
    {
    OstTraceFunctionEntry0( CCALENSENDINGSTATE_NEWLC_ENTRY );
    
    CCalenSendingState* self = new ( ELeave ) CCalenSendingState( aController,aOutstandingNotifications );
    CleanupStack::PushL( self );
    self->ConstructL();

    OstTraceFunctionExit0( CCALENSENDINGSTATE_NEWLC_EXIT );
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenSendingState::ConstructL
// Second stage construction
// ----------------------------------------------------------------------------
void CCalenSendingState::ConstructL()
    {
    OstTraceFunctionEntry0( CCALENSENDINGSTATE_CONSTRUCTL_ENTRY );
    
    BaseConstructL();
    
    OstTraceFunctionExit0( CCALENSENDINGSTATE_CONSTRUCTL_EXIT );
    }
    
// ----------------------------------------------------------------------------
// CCalenSendingState::CCalenSendingState
// C++ Constructor
// ----------------------------------------------------------------------------
CCalenSendingState::CCalenSendingState( CCalenController& aController,
                                                    RHashSet<TCalenNotification>&  aOutstandingNotifications )
    : CCalenState( aController, aOutstandingNotifications )
    {
    OstTraceFunctionEntry0( CCALENSENDINGSTATE_CCALENSENDINGSTATE_ENTRY );
    
    OstTraceFunctionExit0( CCALENSENDINGSTATE_CCALENSENDINGSTATE_EXIT );
    }
    
// ----------------------------------------------------------------------------
// CCalenSendingState::CCalenSendingState
// Destructor
// ----------------------------------------------------------------------------    
CCalenSendingState::~CCalenSendingState()
    {
    OstTraceFunctionEntry0( DUP1_CCALENSENDINGSTATE_CCALENSENDINGSTATE_ENTRY );
    
    OstTraceFunctionExit0( DUP1_CCALENSENDINGSTATE_CCALENSENDINGSTATE_EXIT );
    }

// ----------------------------------------------------------------------------
// CCalenSendingState::HandleCommandL
// From CCalenState
// ----------------------------------------------------------------------------    
TBool CCalenSendingState::HandleCommandL( const TCalenCommand& aCommand,
                                         CCalenStateMachine& aStateMachine )
    {
    OstTraceFunctionEntry0( CCALENSENDINGSTATE_HANDLECOMMANDL_ENTRY );
    
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

    OstTraceFunctionExit0( CCALENSENDINGSTATE_HANDLECOMMANDL_EXIT );
    return cmdUsed;
    }

// ----------------------------------------------------------------------------
// CCalenSendingState::HandleNotificationL
// From CCalenState
// ----------------------------------------------------------------------------        
void CCalenSendingState::HandleNotificationL(const TCalenNotification& aNotification,
                                              CCalenStateMachine& aStateMachine )
    {
    OstTraceFunctionEntry0( CCALENSENDINGSTATE_HANDLENOTIFICATIONL_ENTRY );
    
    switch( aNotification )
        {
        case ECalenNotifyDialogClosed:  // issued when send method query dialog is cancelled
        	{
        	// FIXME Should have a ECalenNotifyEntrySent?
            SetCurrentState( aStateMachine, iPreviousState);
            iOutstandingNotifications.InsertL(aNotification);
        	ActivateCurrentStateL(aStateMachine);
        	}
            break;
        case ECalenNotifyEntrySent:
        	{
            SetCurrentState( aStateMachine, iPreviousState);
            iOutstandingNotifications.InsertL(aNotification);
        	ActivateCurrentStateL(aStateMachine);
        	}
        default:
            CCalenState::HandleNotificationL( aNotification, aStateMachine );
            break;
        }

    OstTraceFunctionExit0( CCALENSENDINGSTATE_HANDLENOTIFICATIONL_EXIT );
    }


 // end of file
