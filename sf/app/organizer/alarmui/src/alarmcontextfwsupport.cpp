/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This class interfaces with Context Framework Client API.
*
*/



// SYSTEM INCLUDE
#include <coemain.h>
#include <f32file.h>
#include <pathinfo.h>

#include <cfclient.h>
#include <cfcontextobject.h>
#include <cfactionsubscription.h>
#include <cfcontextsubscription.h>
#include <cfcontextindication.h>
#include <cfactionindication.h>

// USER INCLUDE
#include "alarmcontextfwsupport.h"
#include "AlmAlarmControl.h"

#include "pim_trace.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Symbian constructor.
// ---------------------------------------------------------------------------
//
CAlarmContextFwSupport* CAlarmContextFwSupport::NewL(CAlmAlarmControl* aAlarmControl)
    {
    TRACE_ENTRY_POINT;
    CAlarmContextFwSupport* self = new( ELeave )CAlarmContextFwSupport( aAlarmControl );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TRACE_EXIT_POINT;
    return self;
    }

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
//
CAlarmContextFwSupport::~CAlarmContextFwSupport()
    {
    TRACE_ENTRY_POINT;
    CloseCFClient();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// Indicates a change in context for subscribed clients.
// ---------------------------------------------------------------------------
//
void CAlarmContextFwSupport::ContextIndicationL(const CCFContextIndication& aChangedContext)
    {
    TRACE_ENTRY_POINT;

	
    const CCFContextObject& contextObject = aChangedContext.Context();

    if( contextObject.Source() == KAlarmUISource() && contextObject.Type() == KAlarmUISourceCommand() )
        {
        
        if( contextObject.Value() == TPtrC( KAlarmUISourceCommandValues[ECommandAlarmStop] ) )
            {
            // stop alarm
            iAlarmControl->SetStopFromContext(ETrue);
            iAlarmControl->ExternalStopAlarm();
            }
        else if( contextObject.Value() == TPtrC( KAlarmUISourceCommandValues[ECommandAlarmSnooze] ) )
            {
            // snooze alarm
            if(iAlarmControl->CanSnooze())
	            {
	            iAlarmControl->ExternalSnoozeAlarm();
	            }
            }
        else
            {
            // value didn't match
            }
        }
    else
        {
        //source didn't match!
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// Indicates that specified action is needed to be executed.
// ---------------------------------------------------------------------------
//
void CAlarmContextFwSupport::ActionIndicationL(const CCFActionIndication& aActionToExecute)
    {
    TRACE_ENTRY_POINT;

    if( aActionToExecute.Identifier() == TPtrC( KAlarmUIActions[EActionSnooze] ) )
        {
        // snooze the alarm
		if(iAlarmControl->CanSnooze())
        	iAlarmControl->ExternalSnoozeAlarm();
        }
    else if( aActionToExecute.Identifier() == TPtrC( KAlarmUIActions[EActionStop] ) )
        {
        //Do not stop the alarm internally but force the user to stop the alarm manually.
        //iAlarmControl->ExternalStopAlarm();
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// Indicates that error has occured.
// ---------------------------------------------------------------------------
//
void CAlarmContextFwSupport::HandleContextFrameworkError( TCFError aError,
                                                          const TDesC& /*aSource*/,
                                                          const TDesC& /*aType*/ )
    {
    TRACE_ENTRY_POINT;
    // no implementation
    ASSERT( !aError );
    TRACE_EXIT_POINT;
    }

// Changes made for Context Framework API changes (SCB Id: MKAN-765D93)
// ---------------------------------------------------------------------------
// Returns an extension interface.
// The extension interface is mapped with the extension UID.
// ---------------------------------------------------------------------------
//
TAny* CAlarmContextFwSupport::Extension( const TUid& /*aExtensionUid */ ) const
{
	TRACE_ENTRY_POINT;
  // Just return a NULL as currently there are no extensions available
  // and also since we do not support any of these APIs defined within
  // the alarm UID.
  TRACE_EXIT_POINT;
  return NULL;
}

// ---------------------------------------------------------------------------
// c++ constructor
// ---------------------------------------------------------------------------
//
CAlarmContextFwSupport::CAlarmContextFwSupport(CAlmAlarmControl* aAlarmControl)
    : iAlarmControl( aAlarmControl )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CAlarmContextFwSupport::ConstructL()
    {
    TRACE_ENTRY_POINT;
    // Context Framework client side API
    OpenCFClientL();

    // define context(s)
    DefineContextL( KAlarmUISource, KAlarmUISourceState, KAlarmUISourceStateSec );
    DefineContextL( KAlarmUISource, KAlarmUISourceCommand, KAlarmUISourceCommandSec );
    DefineContextL( KAlarmUISource, KAlarmUISourceResult, KAlarmUISourceResultSec  );

    // define actions(s)
    DefineActionL( EActionSnooze );

    //Stop action definition should not be defined as the alarm component should not automatically stop the alarm on observing context events
    // but continue to expire the alarm and force the user to manually stop the alarm
    //DefineActionL( EActionStop );  

    // no need to keep the session open
    CloseCFClient();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// Defines a new context
// ---------------------------------------------------------------------------
//
void CAlarmContextFwSupport::DefineContextL( const TDesC& aContextSource,
                                             const TDesC& aContextType,
                                             const TSecurityPolicy& aSecurityPolicy )
    {
    TRACE_ENTRY_POINT;
    const TInt err = iCFClient->DefineContext( aContextSource,
                                               aContextType,
                                               aSecurityPolicy );
    // don't care if it already existed
    User::LeaveIfError( err == KErrAlreadyExists ? KErrNone : err );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// Defines an action
// ---------------------------------------------------------------------------
//
void CAlarmContextFwSupport::DefineActionL(const TAlarmUIActionValues aAction)

    {
    TRACE_ENTRY_POINT;
    const TInt err = iCFClient->DefineAction( TPtrC( KAlarmUIActions[ aAction ] ),
                                              KAlarmUIActionSec );
    // don't care if it already existed
    User::LeaveIfError( err == KErrAlreadyExists ? KErrNone : err );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// Subscribe an action
// ---------------------------------------------------------------------------
//
void CAlarmContextFwSupport::SubscribeActionL(const TAlarmUIActionValues aAction)
    {
    TRACE_ENTRY_POINT;
    CCFActionSubscription* as = CCFActionSubscription::NewLC();
    as->SetActionIdentifierL( TPtrC( KAlarmUIActions[ aAction ] ) );
    TInt err = iCFClient->SubscribeAction( *as );

    if( !err )
        {
        // add to array if the subscription was successfull
        err = iSubscribedActions.Append( as );
        CleanupStack::Pop( as );
        }
    else
        {
        // delete the object if it didn't go to the array
        CleanupStack::PopAndDestroy( as );
        }
    User::LeaveIfError( err );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// Unsubscribe all actions
// ---------------------------------------------------------------------------
//
void CAlarmContextFwSupport::UnsubscribeActions()
    {
    TRACE_ENTRY_POINT;
    // unsubscribe all actions
    while( iSubscribedActions.Count() )
        {
        CCFActionSubscription* as = iSubscribedActions[0];
        iSubscribedActions.Remove( 0 );
        PIM_ASSERT( iCFClient->UnsubscribeAction( *as ); )
        delete as;
        }
    iSubscribedActions.Close();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// Publish a context
// ---------------------------------------------------------------------------
//
void CAlarmContextFwSupport::PublishContextL( const TDesC& aSource,
                                              const TDesC& aType,
                                              const TDesC& aValue )
    {
    TRACE_ENTRY_POINT;
    CCFContextObject* co = CCFContextObject::NewLC( aSource, aType, aValue );

    const TInt err = iCFClient->PublishContext( *co );
    CleanupStack::PopAndDestroy( co );
    User::LeaveIfError( err );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// Subscribe to actions/ context and publish the new alarm status.
// ---------------------------------------------------------------------------
//
void CAlarmContextFwSupport::StartL(const TAlarmSourceStateValues aEvent)
    {
    TRACE_ENTRY_POINT;
    // open client session(closed in StopL)
    OpenCFClientL();  // closed in StopL

    // subscribe for action(s)
    SubscribeActionL( EActionSnooze );

    //Subscription to stop action definition from the context events should not be defined as the alarm component should not automatically stop the alarm on observing context events
    // but continue to expire the alarm and force the user to manually stop the alarm
    //SubscribeActionL( EActionStop );

    // subscribe for context with Source:  'AlarmUI' & Type:    'Command'
    SubscribeContextL(KAlarmUISource, KAlarmUISourceCommand );
        
    // publish new alarm event/status
    PublishContextL( KAlarmUISource,
                     KAlarmUISourceState,
                     TPtrC( KAlarmUISourceStateValues[ aEvent ] ) );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// publish 'alaram inactive' state and close the session
// ---------------------------------------------------------------------------
//
void CAlarmContextFwSupport::StopL()
    {
    TRACE_ENTRY_POINT;
    if( iCFClient ) // created in StartL
        {
        // publish the 'alarm inactive' status
        PublishContextL( KAlarmUISource,
                         KAlarmUISourceState,
                         TPtrC( KAlarmUISourceStateValues[ EStateAlarmInactive ] ) );

        // no need to keep the session open anymore
        CloseCFClient();
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// Create a connection to Context Framework Server 
// ---------------------------------------------------------------------------
//
void CAlarmContextFwSupport::OpenCFClientL()
    {
    TRACE_ENTRY_POINT;
    if( !iCFClient )
        {
        iCFClient = CCFClient::NewL( *this );
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// Unsusbscribe all actions and close the session.
// ---------------------------------------------------------------------------
//
void CAlarmContextFwSupport::CloseCFClient()
    {
    TRACE_ENTRY_POINT;
    UnsubscribeActions();
    
	// unsubscribe contexts
    UnsubscribeContexts();
    delete iCFClient;
    iCFClient = NULL;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// publish context alarm result Source:  'AlarmUI' Type:    'Result'
// ---------------------------------------------------------------------------
//
void CAlarmContextFwSupport::PublishAlarmResultL(const TAlarmSourceResultValues aResult)
    {
    TRACE_ENTRY_POINT;
    TBool closeCFClient( EFalse );

    // open a new CF client session if needed
    if( !iCFClient )
        {
        OpenCFClientL();
        closeCFClient = ETrue;
        }

    // publish the new alarm result
    PublishContextL( KAlarmUISource,
                     KAlarmUISourceResult,
                     TPtrC( KAlarmUISourceResultValues[ aResult ] ) );

    // close the CF client session if it was explicitly opened for this operation
    if( closeCFClient )
        {
        CloseCFClient();
        }
    TRACE_EXIT_POINT;
    }



// ---------------------------------------------------------------------------
// Subscribe a context
// ---------------------------------------------------------------------------
//
void CAlarmContextFwSupport::SubscribeContextL( const TDesC& aSource, 
                          const TDesC& aType)

	{
	TRACE_ENTRY_POINT;
	
	// define a context
	CCFContextSubscription* subscription = CCFContextSubscription::NewLC();
	subscription->SetContextSourceL( aSource  );
    subscription->SetContextTypeL( aType );
    
    //subscribe to the context
    TInt err = iCFClient->SubscribeContext( *subscription );

    if( !err )
        {
        // add to array if the subscription was successfull
        err = iSubscribedContexts.Append( subscription );
        CleanupStack::Pop( subscription );
        }
    else
        {
        // delete the object if subscription fails
        CleanupStack::PopAndDestroy( subscription );
        }
    User::LeaveIfError( err );
        
    TRACE_EXIT_POINT;	
	}
                          
// ---------------------------------------------------------------------------
// Unsubscribe all contexts
// ---------------------------------------------------------------------------
//
void CAlarmContextFwSupport :: UnsubscribeContexts()
	
	{
	TRACE_ENTRY_POINT;
	
    // unsubscribe all subscriptions
    while( iSubscribedContexts.Count() )
        {
        CCFContextSubscription* subscription = iSubscribedContexts[0];
        iSubscribedContexts.Remove( 0 );
        PIM_ASSERT( iCFClient->UnsubscribeContext( *subscription ) );
        delete subscription;
        }
    iSubscribedContexts.Close();
    
    TRACE_EXIT_POINT;	
	}


// End of File
