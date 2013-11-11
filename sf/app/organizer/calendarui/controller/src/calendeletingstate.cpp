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
#include "calendeletingstate.h"
#include "calendarui_debug.h"           // Debug macros
#include "calencontroller.h"
#include "calenstatemachine.h"
#include "calennotifier.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calendeletingstateTraces.h"
#endif

// ----------------------------------------------------------------------------
// CCalenDeletingState::NewLC
// First stage construction
// ----------------------------------------------------------------------------
CCalenDeletingState* CCalenDeletingState::NewLC( CCalenController& aController,
                                                      RHashSet<TCalenNotification>&  aOutstandingNotifications )
    {
    OstTraceFunctionEntry0( CCALENDELETINGSTATE_NEWLC_ENTRY );
    
    CCalenDeletingState* self = new( ELeave ) CCalenDeletingState( aController,aOutstandingNotifications );

    CleanupStack::PushL( self );
    self->ConstructL();

    OstTraceFunctionExit0( CCALENDELETINGSTATE_NEWLC_EXIT );
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenDeletingState::ConstructL
// Second stage construction
// ----------------------------------------------------------------------------
void CCalenDeletingState::ConstructL()
    {
    OstTraceFunctionEntry0( CCALENDELETINGSTATE_CONSTRUCTL_ENTRY );
    
    BaseConstructL();
  
    OstTraceFunctionExit0( CCALENDELETINGSTATE_CONSTRUCTL_EXIT );
    }
    
// ----------------------------------------------------------------------------
// CCalenDeletingState::CCalenDeletingState
// C++ Constructor
// ----------------------------------------------------------------------------
CCalenDeletingState::CCalenDeletingState( CCalenController& aController,
                                                    RHashSet<TCalenNotification>&  aOutstandingNotifications )
    : CCalenState( aController, aOutstandingNotifications )
    {
    OstTraceFunctionEntry0( CCALENDELETINGSTATE_CCALENDELETINGSTATE_ENTRY );
    
    OstTraceFunctionExit0( CCALENDELETINGSTATE_CCALENDELETINGSTATE_EXIT );
    }
    
// ----------------------------------------------------------------------------
// CCalenDeletingState::CCalenDeletingState
// Destructor
// ----------------------------------------------------------------------------    
CCalenDeletingState::~CCalenDeletingState()
    {
    OstTraceFunctionEntry0( DUP1_CCALENDELETINGSTATE_CCALENDELETINGSTATE_ENTRY );

    OstTraceFunctionExit0( DUP1_CCALENDELETINGSTATE_CCALENDELETINGSTATE_EXIT );
    }

// ----------------------------------------------------------------------------
// CCalenDeletingState::HandleCommandL
// From CCCalenState
// ----------------------------------------------------------------------------    
TBool CCalenDeletingState::HandleCommandL( const TCalenCommand& aCommand,
                                         CCalenStateMachine& aStateMachine )
    {
    OstTraceFunctionEntry0( CCALENDELETINGSTATE_HANDLECOMMANDL_ENTRY );
    
    TInt cmd = aCommand.Command();
    MCalenCommandHandler* handler = iController.GetCommandHandlerL( cmd );
    
    ASSERT( handler ); // FIXME: error ui
    
    TBool cmdUsed = EFalse;
    
    if( cmd == ECalenDeleteEntryWithoutQuery )
        {
        RequestCallbackL( handler, aCommand );
        cmdUsed = ETrue;
        }
    else if( ECalenFasterAppExit == cmd )
    	{
        SetCurrentState( aStateMachine, CCalenStateMachine::ECalenIdleState );
        ActivateCurrentStateL(aStateMachine);
        RequestCallbackL( handler, aCommand );
        cmdUsed = ETrue;
    	}

    OstTraceFunctionExit0( CCALENDELETINGSTATE_HANDLECOMMANDL_EXIT );
    return cmdUsed;
    }

// ----------------------------------------------------------------------------
// CCalenDeletingState::HandleNotificationL
// From CCCalenState
// ----------------------------------------------------------------------------        
void CCalenDeletingState::HandleNotificationL(const TCalenNotification& aNotification,
                                              CCalenStateMachine& aStateMachine )
    {
    OstTraceFunctionEntry0( CCALENDELETINGSTATE_HANDLENOTIFICATIONL_ENTRY );
    
    switch( aNotification )
        {
        case ECalenNotifyEntryDeleted:  // issued when new entry dialog is cancelled
        case ECalenNotifyInstanceDeleted:
        case ECalenNotifyMultipleEntriesDeleted:
        case ECalenNotifyDeleteFailed:  // delete failed or cancelled
        case ECalenNotifyMarkedEntryDeleted:
            {
            SetCurrentState( aStateMachine, iPreviousState );
            // DONT broadcast these notifications until the previous state( idle or viewer ) is activated
            // which would broadcast in HandleStateActivationL()
            iOutstandingNotifications.InsertL(aNotification);
            ActivateCurrentStateL(aStateMachine);
            }
            break;
        case ECalenNotifyCancelDelete:
            {
            SetCurrentState( aStateMachine, iPreviousState);
            SetCurrentPreviousState( aStateMachine, GetCurrentState(aStateMachine) );

            iOutstandingNotifications.InsertL(aNotification);
            ActivateCurrentStateL(aStateMachine);
            
            CancelPreviousCmd(aStateMachine);
            }
            break;
        default:
            CCalenState::HandleNotificationL( aNotification, aStateMachine );
            break;
        }

    OstTraceFunctionExit0( CCALENDELETINGSTATE_HANDLENOTIFICATIONL_EXIT );
    }
 
 // end of file
