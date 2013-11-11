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
#include "calenexitingstate.h"
#include "calendarui_debug.h"           // Debug macros
#include "calencontroller.h"
#include "calenstatemachine.h"
#include "calennotifier.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calenexitingstateTraces.h"
#endif

// ----------------------------------------------------------------------------
// CCalenExitingState::NewLC
// First stage construction
// ----------------------------------------------------------------------------
CCalenExitingState* CCalenExitingState::NewLC( CCalenController& aController,
                                                       RHashSet<TCalenNotification>&  aOutstandingNotifications )
    {
    OstTraceFunctionEntry0( CCALENEXITINGSTATE_NEWLC_ENTRY );

    CCalenExitingState* self = new ( ELeave ) CCalenExitingState( aController,aOutstandingNotifications);
    CleanupStack::PushL( self );
    self->ConstructL();

    OstTraceFunctionExit0( CCALENEXITINGSTATE_NEWLC_EXIT );
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenExitingState::ConstructL
// Second stage construction
// ----------------------------------------------------------------------------
void CCalenExitingState::ConstructL()
    {
    OstTraceFunctionEntry0( CCALENEXITINGSTATE_CONSTRUCTL_ENTRY );
    
    BaseConstructL();
    
    OstTraceFunctionExit0( CCALENEXITINGSTATE_CONSTRUCTL_EXIT );
    }
    
// ----------------------------------------------------------------------------
// CCalenExitingState::CCalenExitingState
// C++ Constructor
// ----------------------------------------------------------------------------
CCalenExitingState::CCalenExitingState( CCalenController& aController,
                                                    RHashSet<TCalenNotification>&  aOutstandingNotifications )
    : CCalenState( aController, aOutstandingNotifications )
    {
    OstTraceFunctionEntry0( CCALENEXITINGSTATE_CCALENEXITINGSTATE_ENTRY );
    
    OstTraceFunctionExit0( CCALENEXITINGSTATE_CCALENEXITINGSTATE_EXIT );
    }
    
// ----------------------------------------------------------------------------
// CCalenExitingState::CCalenExitingState
// Destructor
// ----------------------------------------------------------------------------    
CCalenExitingState::~CCalenExitingState()
    {
    OstTraceFunctionEntry0( DUP1_CCALENEXITINGSTATE_CCALENEXITINGSTATE_ENTRY );
    
    OstTraceFunctionExit0( DUP1_CCALENEXITINGSTATE_CCALENEXITINGSTATE_EXIT );
    }

// ----------------------------------------------------------------------------
// CCalenExitingState::HandleCommandL
// From CCalenState
// ----------------------------------------------------------------------------    
TBool CCalenExitingState::HandleCommandL( const TCalenCommand& /*aCommand*/,
                                        CCalenStateMachine& /*aStateMachine*/  )
    {
    OstTraceFunctionEntry0( CCALENEXITINGSTATE_HANDLECOMMANDL_ENTRY );
    
    OstTraceFunctionExit0( CCALENEXITINGSTATE_HANDLECOMMANDL_EXIT );
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CCalenExitingState::HandleNotificationL
// From CCalenState
// ----------------------------------------------------------------------------        
void CCalenExitingState::HandleNotificationL(const TCalenNotification& /*aNotification*/,
                                              CCalenStateMachine& /*aStateMachine*/ )
    {
    OstTraceFunctionEntry0( CCALENEXITINGSTATE_HANDLENOTIFICATIONL_ENTRY );
    
    OstTraceFunctionExit0( CCALENEXITINGSTATE_HANDLENOTIFICATIONL_EXIT );
    }  


 // end of file
