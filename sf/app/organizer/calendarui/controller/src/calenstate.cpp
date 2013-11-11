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
#include "calenstate.h"                 // Calendar States
#include "calencontroller.h"
#include "calenstatemachine.h"
#include "calennotifier.h"
#include "calenasynccallback.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calenstateTraces.h"
#endif

// ----------------------------------------------------------------------------
// CCalenState::CCalenState
// C++ constructor
// ----------------------------------------------------------------------------
CCalenState::CCalenState( CCalenController& aController,RHashSet<TCalenNotification>&  aOutstandingNotifications )
    : iController( aController ),
      iOutstandingNotifications( aOutstandingNotifications ),
      iPreviousState( CCalenStateMachine::ECalenIdleState )
    {
    OstTraceFunctionEntry0( CCALENSTATE_CCALENSTATE_ENTRY );
    
    OstTraceFunctionExit0( CCALENSTATE_CCALENSTATE_EXIT );
    }

// ----------------------------------------------------------------------------
// CCalenState::ConstructL
// second phase construction
// ----------------------------------------------------------------------------
void CCalenState::BaseConstructL()
    {
    OstTraceFunctionEntry0( CCALENSTATE_BASECONSTRUCTL_ENTRY );
    
    TCalenCommand command;
    MCalenCommandHandler* handler( NULL );
    iCallBackPackage = new( ELeave ) CalenCallbackPackage( this, command, handler );
    
    OstTraceFunctionExit0( CCALENSTATE_BASECONSTRUCTL_EXIT );
    }

// ----------------------------------------------------------------------------
// CCalenState::~CCalenState
// Destructor
// ----------------------------------------------------------------------------
CCalenState::~CCalenState()
    {
    OstTraceFunctionEntry0( DUP1_CCALENSTATE_CCALENSTATE_ENTRY );
    
    delete iCallBackPackage;
    iCallBackPackage = NULL;
    delete iCmdCallback;
    
    OstTraceFunctionExit0( DUP1_CCALENSTATE_CCALENSTATE_EXIT );
    }

// ----------------------------------------------------------------------------
// CCalenState::RequestCallback
// Request a callback
// ----------------------------------------------------------------------------
void CCalenState::RequestCallbackL( MCalenCommandHandler* aCommandHandler, const TCalenCommand& aCommand )
    {
    OstTraceFunctionEntry0( CCALENSTATE_REQUESTCALLBACKL_ENTRY );
    
    iCallBackPackage->SetCommandHandler(this, aCommand, aCommandHandler);
     
    if( !iCmdCallback )
        {
        TCallBack callback( CommandCallback, iCallBackPackage );
        iCmdCallback = CalenAsyncCallBack::NewL( callback, CActive::EPriorityHigh , iController );
       }

    TCallBack callback( CommandCallback, iCallBackPackage );
    
    iCmdCallback->Cancel();
    iCmdCallback->Set( callback );
    iCmdCallback->CallBack();
    
    OstTraceFunctionExit0( CCALENSTATE_REQUESTCALLBACKL_EXIT );
    }



// ----------------------------------------------------------------------------
// CCalenState::CommandCallback
// Static callback function
// ----------------------------------------------------------------------------
TInt CCalenState::CommandCallback( TAny* aCommandStruct )
    {
    OstTraceFunctionEntry0( CCALENSTATE_COMMANDCALLBACK_ENTRY );
    
    TBool continueCommand(EFalse);
    
    CalenCallbackPackage* package = static_cast<CalenCallbackPackage*>( aCommandStruct );
    continueCommand = package->HandleCallBack();
    
    OstTraceFunctionExit0( CCALENSTATE_COMMANDCALLBACK_EXIT );
    return continueCommand;
    }

// ----------------------------------------------------------------------------
// CCalenState::HandleNotificationL
// Default behaviour for notification handling is to go back to previous state.
// Concrete states would be expected to check the notification is related
// to the command issued.
// ----------------------------------------------------------------------------
void CCalenState::HandleNotificationL(const TCalenNotification& aNotification,
                                      CCalenStateMachine& aStateMachine )
    {
    OstTraceFunctionEntry0( CCALENSTATE_HANDLENOTIFICATIONL_ENTRY );
    
    switch( aNotification )
        {
        case ECalenNotifyAppBackgrounded:
        	{
        	CCalenStateMachine::TCalenStateIndex cachedState = aStateMachine.GetCurrentState();
			aStateMachine.SetCurrentState(CCalenStateMachine::ECalenBackgroundState);
			aStateMachine.SetCurrentPreviousState(cachedState);
			iOutstandingNotifications.InsertL(aNotification);
			aStateMachine.ActivateCurrentStateL();
        	    
        	}
            break;
        default:
            iController.Notifier().BroadcastApprovedNotification( aNotification );
            break;
        }
    
    OstTraceFunctionExit0( CCALENSTATE_HANDLENOTIFICATIONL_EXIT );
    }


// ----------------------------------------------------------------------------
// CCalenState::PreviousState
// C++ constructor
// ----------------------------------------------------------------------------
CCalenStateMachine::TCalenStateIndex CCalenState::PreviousState()
    {
    OstTraceFunctionEntry0( CCALENSTATE_PREVIOUSSTATE_ENTRY );
    
    OstTraceFunctionExit0( CCALENSTATE_PREVIOUSSTATE_EXIT );
    return iPreviousState;
    }

// ----------------------------------------------------------------------------
// CCalenState::SetPreviousState
// Sets previous state.
// ----------------------------------------------------------------------------
void CCalenState::SetPreviousState(const CCalenStateMachine::TCalenStateIndex& aPreviousState )
    {
    OstTraceFunctionEntry0( CCALENSTATE_SETPREVIOUSSTATE_ENTRY );
    
    iPreviousState = aPreviousState;
    
    OstTraceFunctionExit0( CCALENSTATE_SETPREVIOUSSTATE_EXIT );
    }
    
// ----------------------------------------------------------------------------
// CCalenState::CommandExecuting
// From CCalenState
// ----------------------------------------------------------------------------    
void CCalenState::CommandExecuting()
    {
    OstTraceFunctionEntry0( CCALENSTATE_COMMANDEXECUTING_ENTRY );
    
    OstTraceFunctionExit0( CCALENSTATE_COMMANDEXECUTING_EXIT );
    }  

// ----------------------------------------------------------------------------
// CCalenState::CalenCallbackPackage::CalenCallbackPackage
// C++ constructor
// ----------------------------------------------------------------------------
CCalenState::CalenCallbackPackage::CalenCallbackPackage( CCalenState* aSelf,
                        TCalenCommand aCommand,  MCalenCommandHandler* aCommandHandler)
    : iSelf( aSelf ), iCommand( aCommand ), iCommandHandler( aCommandHandler )                   
    {
    OstTraceFunctionEntry0( CALENCALLBACKPACKAGE_CALENCALLBACKPACKAGE_ENTRY );
    
    OstTraceFunctionExit0( CALENCALLBACKPACKAGE_CALENCALLBACKPACKAGE_EXIT );
    }

// ----------------------------------------------------------------------------
// CCalenState::CalenCallbackPackage::HandleCallBack
// Handles a callback
// ----------------------------------------------------------------------------
TBool CCalenState::CalenCallbackPackage::HandleCallBack()
    {
    OstTraceFunctionEntry0( CALENCALLBACKPACKAGE_HANDLECALLBACK_ENTRY );
    
    TBool continueCommand(EFalse);

    iSelf->CommandExecuting();
    PIM_TRAPD_HANDLE ( continueCommand = iCommandHandler->HandleCommandL( iCommand ) );
    
    OstTraceFunctionExit0( CALENCALLBACKPACKAGE_HANDLECALLBACK_EXIT );
    return continueCommand;
    }

// ----------------------------------------------------------------------------
// CCalenState::CalenCallbackPackage::SetCommandHandler
// Sets the command handler and command
// ----------------------------------------------------------------------------
void CCalenState::CalenCallbackPackage::SetCommandHandler(CCalenState* aSelf,
                        TCalenCommand aCommand,  MCalenCommandHandler* aCommandHandler)
    {
    OstTraceFunctionEntry0( CALENCALLBACKPACKAGE_SETCOMMANDHANDLER_ENTRY );
    
    iSelf = aSelf;
    iCommand = aCommand;
    iCommandHandler = aCommandHandler; 

    OstTraceFunctionExit0( CALENCALLBACKPACKAGE_SETCOMMANDHANDLER_EXIT );
    }
    
// ----------------------------------------------------------------------------
// CCalenState::HandleStateActivationL
// Default behavior when state is activated. Check and issue any outstanding
// notifications
// ----------------------------------------------------------------------------
void CCalenState::HandleStateActivationL(CCalenStateMachine& /*aStateMachine*/)
	{
    OstTraceFunctionEntry0( CCALENSTATE_HANDLESTATEACTIVATIONL_ENTRY );
    
    // Can only issue one of the following Notifications
    // ECalenNotifySettingsChanged or 
    // ECalenNotifySystemLocaleChanged or
    // ECalenNotifySettingsClosed
    
    if(iOutstandingNotifications.Find(ECalenNotifySettingsChanged))
        {
        iOutstandingNotifications.Remove(ECalenNotifySystemLocaleChanged);
        iOutstandingNotifications.Remove(ECalenNotifySettingsClosed);
        }
    else if(iOutstandingNotifications.Find(ECalenNotifySystemLocaleChanged))
        {
        iOutstandingNotifications.Remove(ECalenNotifySettingsClosed);
        }
    
    THashSetIter<TCalenNotification> iterator(iOutstandingNotifications);
    const TCalenNotification* notification = iterator.Next();
    while(notification)
        {
        iController.Notifier().BroadcastApprovedNotification( *notification );
        notification = iterator.Next();
        }
    
    iOutstandingNotifications.Close();
    
	OstTraceFunctionExit0( CCALENSTATE_HANDLESTATEACTIVATIONL_EXIT );
	}

// ----------------------------------------------------------------------------
// CCalenState::SetCurrentState
// Sets the current state
// ----------------------------------------------------------------------------
void CCalenState::SetCurrentState(CCalenStateMachine& aStateMachine,
								  const CCalenStateMachine::TCalenStateIndex& aState )	
	{
    OstTraceFunctionEntry0( CCALENSTATE_SETCURRENTSTATE_ENTRY );
    
    aStateMachine.SetCurrentState(aState);
    
	OstTraceFunctionExit0( CCALENSTATE_SETCURRENTSTATE_EXIT );
	}

// ----------------------------------------------------------------------------
// CCalenState::GetCurrentState
// Gets the current state
// ----------------------------------------------------------------------------
CCalenStateMachine::TCalenStateIndex CCalenState::GetCurrentState(
											CCalenStateMachine& aStateMachine)
	{
    OstTraceFunctionEntry0( CCALENSTATE_GETCURRENTSTATE_ENTRY );
    
    OstTraceFunctionExit0( CCALENSTATE_GETCURRENTSTATE_EXIT );
	return aStateMachine.GetCurrentState();
	}

// ----------------------------------------------------------------------------
// CCalenState::SetPreviousState
// Sets the previous state through state machine
// ----------------------------------------------------------------------------
void CCalenState::SetCurrentPreviousState(CCalenStateMachine& aStateMachine, 
								const CCalenStateMachine::TCalenStateIndex& aState)
	{
    OstTraceFunctionEntry0( CCALENSTATE_SETCURRENTPREVIOUSSTATE_ENTRY );
    
	aStateMachine.SetCurrentPreviousState(aState);
	
	OstTraceFunctionExit0( CCALENSTATE_SETCURRENTPREVIOUSSTATE_EXIT );
	}

// ----------------------------------------------------------------------------
// CCalenState::ActivateCurrentStateL
// Activate current state
// ----------------------------------------------------------------------------
void CCalenState::ActivateCurrentStateL(CCalenStateMachine& aStateMachine)
	{
    OstTraceFunctionEntry0( CCALENSTATE_ACTIVATECURRENTSTATEL_ENTRY );
    
    aStateMachine.ActivateCurrentStateL();
    	
	OstTraceFunctionExit0( CCALENSTATE_ACTIVATECURRENTSTATEL_EXIT );
	}
// ----------------------------------------------------------------------------
// CCalenState::CancelPreviousCmd
// Cancel the previous command
// ----------------------------------------------------------------------------
void CCalenState::CancelPreviousCmd(CCalenStateMachine& aStateMachine)
	{
    OstTraceFunctionEntry0( CCALENSTATE_CANCELPREVIOUSCMD_ENTRY );
    
	aStateMachine.CancelPreviousCmd();
	
	OstTraceFunctionExit0( CCALENSTATE_CANCELPREVIOUSCMD_EXIT );
	}

// ----------------------------------------------------------------------------
// CCalenState::CancelExecutingCmd
// Cancels executing command
// ----------------------------------------------------------------------------
void CCalenState::CancelExecutingCmd()
	{
    OstTraceFunctionEntry0( CCALENSTATE_CANCELEXECUTINGCMD_ENTRY );
    
	if(iCmdCallback)
		{
		iCmdCallback->Cancel();
		}
    	
	OstTraceFunctionExit0( CCALENSTATE_CANCELEXECUTINGCMD_EXIT );
	}	
// End of file
