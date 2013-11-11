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
* Description:  Calendar controller
*
*/


#include <e32std.h>
#include <hbmainwindow.h>
#include <hbwidget.h>
#include <hbinstance.h>
#include <hbapplication.h> //Activity Manager
#include <xqserviceutil.h> // service utils

// User includes
#include "calencontroller.h"            // CCalenController
#include "calenviewmanager.h"           // Responsible for all view activations        
#include "calenservicesimpl.h"          // MCalenServices implementation
#include "calenactionui.h"              // Default command handling
#include "calencustomisationmanager.h"  // Customisation Manager
#include "calennotificationhandler.h"
#include "calennotifier.h"
#include "hb_calencommands.hrh"
#include "calendarui_debug.h"           // Debug macros
#include "calenstatemachine.h"
#include "calenservicesimpl.h"          // Calendar service implementation
#include "CalenUid.h"
#include "calencontextimpl.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calencontrollerTraces.h"
#endif


// Constants
const TInt KCustomCommandRangeStart     = ECalenLastCommand; 
const TInt KNumberOfCommandsPerServices = 100;

// ----------------------------------------------------------------------------
// CCalenController::CCalenController
// Constructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenController::CCalenController()
{
    OstTraceFunctionEntry0( CCALENCONTROLLER_CCALENCONTROLLER_ENTRY );
    
	// Check the Application Startup reason, set iIsFromServiceFrmWrk if application
	// is started by service framework, false otherwise
    /*Hb::ActivationReasonService == qobject_cast<HbApplication*>(qApp)->activateReason() ? 
        iIsFromServiceFrmWrk = true:
        iIsFromServiceFrmWrk = false; */
    
    // Check if calendar is launched thru XQService framework
    iIsFromServiceFrmWrk = XQServiceUtil::isService(); // Since activateReason 
    //of hbapplication is not returning right value if the activity is started 
    //as services so using the above line temporarily untill a fix is available in 
    // hbappliacation. Need to remove this line after the fix is available for hbapplcation
    
    iNextServicesCommandBase = KCustomCommandRangeStart;
    iRefCount = 0;
    mAgendaUtil = 0;
    
    OstTraceFunctionExit0( CCALENCONTROLLER_CCALENCONTROLLER_EXIT );
}

// ----------------------------------------------------------------------------
// CCalenController::constuctController
// Construct the controller completely
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenController::constructController()
{
	OstTraceFunctionEntry0( CCALENCONTROLLER_CONSTRUCTCONTROLLER_ENTRY );
	// Store the pointer in tls, also avoid multiple creations
	checkMultipleCreation();

	// Get an instance of AgendaUtil interface class
	// This will take care of 
	mAgendaUtil = new AgendaUtil();

	iStateMachine = CCalenStateMachine::NewL( *this );

	// Create the notifier.
	iNotifier = new( ELeave )CalenNotifier( *iStateMachine );

	// Construct the context
	mContext = new CalenContextImpl(iNotifier);

	// Set the default context.Once will start use of calencmdlinelauncher,
	// Then need to remove this function
	SetDefaultContext();

	RArray<TCalenNotification> notificationArray;
	// Complete construction of the notifier and register the
	// global data for notifications
	iNotifier->ConstructL();

	// Create the services
	iServices = CalenServicesImpl::NewL();
	// Create the customisation manager, and register for 
	// notifications
	iCustomisationManager = CCalenCustomisationManager::NewL( *this,
	                                                          *iServices );
	// Create the view manager, and register for notifications
	iViewManager = new CalenViewManager(*this);

	iViewManager->SecondPhaseConstruction();

	hbInstance->allMainWindows().first()->show();

	// Create the action uis.
	iActionUi = CCalenActionUi::NewL( *this );

	notificationArray.Append(ECalenNotifySettingsChanged);
	notificationArray.Append(ECalenNotifyCheckPluginUnloading);
	notificationArray.Append(ECalenNotifyEComRegistryChanged);
	notificationArray.Append(ECalenNotifySystemLanguageChanged);

	RegisterForNotificationsL( iCustomisationManager,notificationArray);
	notificationArray.Reset();

	notificationArray.Append(ECalenNotifyViewPopulationComplete);
	notificationArray.Append(ECalenNotifyExternalDatabaseChanged);
	notificationArray.Append(ECalenNotifyMultipleEntriesDeleted);
	notificationArray.Append(ECalenNotifyDialogClosed);
	notificationArray.Append(ECalenNotifyEntrySaved);
	notificationArray.Append(ECalenNotifyEntryDeleted);
	notificationArray.Append(ECalenNotifyInstanceDeleted);
	notificationArray.Append(ECalenNotifySystemLocaleChanged);
	notificationArray.Append(ECalenNotifySystemLanguageChanged);
	notificationArray.Append(ECalenNotifySystemTimeChanged);
	notificationArray.Append(ECalenNotifyEntryClosed);
	notificationArray.Append(ECalenNotifySettingsClosed);

	RegisterForNotificationsL( iViewManager, notificationArray );
	notificationArray.Reset();  
	notificationArray.Close();
	
	OstTraceFunctionExit0( CCALENCONTROLLER_CONSTRUCTCONTROLLER_EXIT );
}

void CCalenController::checkMultipleCreation()
{
	OstTraceFunctionEntry0( CCALENCONTROLLER_CHECKMULTIPLECREATION_ENTRY );
	TAny* tlsPtr = Dll::Tls();

	// Check Thread local storage
	if( !tlsPtr )
		{
		// Store a self pointer in TLS
		User::LeaveIfError( Dll::SetTls( static_cast<TAny*>( this ) ) );
		// Increment ref count right away. If we don't do it here, and someone
		// calls Controller::InstanceL in ConstructL and then ConstructL
		// leaves, we will double delete the controller.
		++this->iRefCount;
		}
	else
		{
		// An instance of the controller exists already.
		// This function should only have been called once, by CCalenAppUi
		User::Leave( KErrAlreadyExists );
		}	
	OstTraceFunctionExit0( CCALENCONTROLLER_CHECKMULTIPLECREATION_EXIT );
}

// ----------------------------------------------------------------------------
// CCalenController::InstanceL
// Returns a pointer to the single instance of the Calendar Controller.
// Leaves with KErrNotReady if the controller has not already been created
// using NewL.  A leave here means that the Calendar application is not running
// but someone is trying to use the services API.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenController* CCalenController::InstanceL()
    {
    OstTraceFunctionEntry0( CCALENCONTROLLER_INSTANCEL_ENTRY );
    
    CCalenController* self = NULL;
    TAny* tlsPtr = Dll::Tls();

    // Check Thread local storage
    if( !tlsPtr )
        {
        // The Controller has not yet been constructed.  Someone is probably
        // trying to use the Services API outside of a customisation
        // plugin.  We don't allow that.
        User::Leave( KErrNotReady );
        }
    else
        {
        self = static_cast<CCalenController*>( tlsPtr );
        }

    ++self->iRefCount;

    
    OstTraceFunctionExit0( CCALENCONTROLLER_INSTANCEL_EXIT );
    return self;    
    }

// ----------------------------------------------------------------------------
// CCalenController::ReleaseCustomisations
// Releases any plugins by deleting the customisation manager
// should only be called on exiting by the application.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenController::ReleaseCustomisations()
    {
    OstTraceFunctionEntry0( CCALENCONTROLLER_RELEASECUSTOMISATIONS_ENTRY );
    
    delete iCustomisationManager;
    iCustomisationManager = NULL;
    
    OstTraceFunctionExit0( CCALENCONTROLLER_RELEASECUSTOMISATIONS_EXIT );
    }

// ----------------------------------------------------------------------------
// CCalenController::Release
// Decrement the reference count of this singleton.
// When the reference count is 0, the controller will self delete and free
// all resources
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenController::Release()
    {
    OstTraceFunctionEntry0( CCALENCONTROLLER_RELEASE_ENTRY );
    
    --iRefCount;
    
    // The controller owns its own instance of the services, therefore the
    // reference count will be one, immediatley before deletion.
    if (iRefCount == 1)
        {
        delete this;
        }
    
    OstTraceFunctionExit0( CCALENCONTROLLER_RELEASE_EXIT );
    }

// ----------------------------------------------------------------------------
// CCalenController::~CCalenController
// Private destructor, called from Release() when reference count is 0.
// Frees all resources.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenController::~CCalenController()
    {
    OstTraceFunctionEntry0( DUP1_CCALENCONTROLLER_CCALENCONTROLLER_ENTRY );

    if(iStateMachine) {
    	delete iStateMachine;
    	iStateMachine = NULL;
    }

    if ( iServices )
        {
        iServices->Release();
        }
    
    if( mContext )
		{
		delete mContext;
		mContext = NULL;
		}
    // iActionUi has to be deleted before iNotifier
    // as the unregistering of the notifications has to be done
    if( iActionUi )
 	   {
 	   delete iActionUi;
 	   iActionUi = NULL;
 	   }
    
    if( iNotifier )
		{
    	delete iNotifier;
    	iNotifier = NULL;
		}
    
    if( iViewManager )
		{
    	delete iViewManager;
    	iViewManager = NULL;
		}
    
    if( iCustomisationManager )
		{
    	delete iCustomisationManager;
    	iCustomisationManager = NULL;
		}
		
    if (mAgendaUtil) {
        delete mAgendaUtil;
        mAgendaUtil = 0;
    }
    
    Dll::SetTls( NULL );
    
    OstTraceFunctionExit0( DUP1_CCALENCONTROLLER_CCALENCONTROLLER_EXIT );
    }


// ----------------------------------------------------------------------------
// CCalenController::IssueCommmandL
// Adds the passed command to the comand queue. Commands are handled
// asynchronously in HandleCommandL
// Returns EFalse if the passed command is not in the issuers command range
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenController::IssueCommandL( TInt aCommand )
    {
    OstTraceFunctionEntry0( CCALENCONTROLLER_ISSUECOMMANDL_ENTRY );
    
    TCalenCommand cmd;
    cmd.SetCommandAndContextL( aCommand ,context());

    TBool ret = iStateMachine->HandleCommandL( cmd );
    
    OstTraceFunctionExit0( CCALENCONTROLLER_ISSUECOMMANDL_EXIT );
    return ret;
    }

// ----------------------------------------------------------------------------
// CCalenController::Services
// Returns the services
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
MCalenServices& CCalenController::Services()
    {
    OstTraceFunctionEntry0( CCALENCONTROLLER_SERVICES_ENTRY );
    
    OstTraceFunctionExit0( CCALENCONTROLLER_SERVICES_EXIT );
    return *iServices;
    }

// ----------------------------------------------------------------------------
// CCalenController::ViewManager
// Returns a reference to the view manager
// (other items were commented in a header).
// ----------------------------------------------------------------------------
CalenViewManager& CCalenController::ViewManager()
    {
    OstTraceFunctionEntry0( CCALENCONTROLLER_VIEWMANAGER_ENTRY );
    
	OstTraceFunctionExit0( CCALENCONTROLLER_VIEWMANAGER_EXIT );
	return *iViewManager;
    }
    
// ----------------------------------------------------------------------------
// CCalenController::MainWindow
// Returns a reference to the MainWindow
// (other items were commented in a header).
// ----------------------------------------------------------------------------
HbMainWindow& CCalenController::MainWindow()
    {
    
    OstTraceFunctionEntry0( DUP1_CCALENCONTROLLER_MAINWINDOW_ENTRY );

    OstTraceFunctionExit0( DUP1_CCALENCONTROLLER_MAINWINDOW_EXIT );
    
    return *(hbInstance->allMainWindows().first());
	
	
    }    

// ----------------------------------------------------------------------------
// CCCalenController::BroadcastNotification
// Passes the notification to the Calendar Notifier.  The notification will
// then be broadcast to all observers
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenController::BroadcastNotification( TCalenNotification aNotification )
    {
    OstTraceFunctionEntry0( CCALENCONTROLLER_BROADCASTNOTIFICATION_ENTRY );

    iNotifier->BroadcastNotification( aNotification );

    OstTraceFunctionExit0( CCALENCONTROLLER_BROADCASTNOTIFICATION_EXIT );
    }

// ----------------------------------------------------------------------------
// CCCalenController::RegisterForNotificationsL
// Registers the passed notification handler with the Calendar Notifier
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenController::RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
                                                            TCalenNotification aNotification )
    {
    OstTraceFunctionEntry0( CCALENCONTROLLER_REGISTERFORNOTIFICATIONSL_ENTRY );

    iNotifier->RegisterForNotificationsL( aHandler, aNotification );

    OstTraceFunctionExit0( CCALENCONTROLLER_REGISTERFORNOTIFICATIONSL_EXIT );
    }

// ----------------------------------------------------------------------------
// CCCalenController::RegisterForNotificationsL
// Registers the passed notification handler with the Calendar Notifier
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenController::RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
                                                            RArray<TCalenNotification>& aNotifications )
    {
    OstTraceFunctionEntry0( DUP1_CCALENCONTROLLER_REGISTERFORNOTIFICATIONSL_ENTRY );

    iNotifier->RegisterForNotificationsL( aHandler, aNotifications );
    
    OstTraceFunctionExit0( DUP1_CCALENCONTROLLER_REGISTERFORNOTIFICATIONSL_EXIT );
    }

// ----------------------------------------------------------------------------
// CCCalenController::CancelNotifications
// Removes the passed handler from the notifier.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenController::CancelNotifications( MCalenNotificationHandler* aHandler )
    {
    OstTraceFunctionEntry0( CCALENCONTROLLER_CANCELNOTIFICATIONS_ENTRY );

    iNotifier->CancelNotifications( aHandler );
    
    OstTraceFunctionExit0( CCALENCONTROLLER_CANCELNOTIFICATIONS_EXIT );
    }

// ----------------------------------------------------------------------------
// CCCalenController::GetCommandHandlerL
// Searches for a command handler for a particular command.  Customisations
// are searched first.  If no customisation wants to handle the command it is
// handled by the view manager or the action uis
// ----------------------------------------------------------------------------
MCalenCommandHandler* CCalenController::GetCommandHandlerL( TInt aCommand )
    {
    OstTraceFunctionEntry0( CCALENCONTROLLER_GETCOMMANDHANDLERL_ENTRY );
    
    MCalenCommandHandler* handler( NULL );

    // Stop non-published commands from being customised
    if ( aCommand != ECalenShowSettings
        && aCommand != ECalenSwitchView )
        {
        // See if a plugin wants the command
        handler = iCustomisationManager->GetCommandHandlerL( aCommand );
        }

    // See if the view manager wants the command
    if ( !handler )
        {
        if (   aCommand >= ECalenViewCommandBase
            && aCommand < ECalenEditCommandBase )
            {
            handler = iViewManager;
            }
     	else if( aCommand >= ECalenMissedAlarmCommandBase
      		&& aCommand < ECalenLastCommand )
	    	{
	    	//handler = iAlarmManager;
	    	} 
        else 
            {
            handler = iActionUi->GetCommandHandlerL( aCommand );
            }
        }

    // No command handler is an error  
    
    OstTraceFunctionExit0( CCALENCONTROLLER_GETCOMMANDHANDLERL_EXIT );
    
    // return the handler
    return handler;
    }

// ----------------------------------------------------------------------------
// CCCalenController::NewServicesL
// Factory function for creating new MCalenServices objects
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
MCalenServices* CCalenController::NewServicesL()
    {
    OstTraceFunctionEntry0( CCALENCONTROLLER_NEWSERVICESL_ENTRY );
    
    TInt commandRangeStart = iNextServicesCommandBase;
    TInt commandRangeEnd = commandRangeStart + KNumberOfCommandsPerServices;
    iNextServicesCommandBase = commandRangeEnd + 1;

    CalenServicesImpl* svc = CalenServicesImpl::NewL( commandRangeStart,commandRangeEnd );
    
    OstTraceFunctionExit0( CCALENCONTROLLER_NEWSERVICESL_EXIT );
    return svc;
    }

// ----------------------------------------------------------------------------
// CCalenController::Notifier
// Returns the notifier.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CalenNotifier& CCalenController::Notifier()
    {
    OstTraceFunctionEntry0( CCALENCONTROLLER_NOTIFIER_ENTRY );
    
    OstTraceFunctionExit0( CCALENCONTROLLER_NOTIFIER_EXIT );
    return *iNotifier;
    }
    
// ----------------------------------------------------------------------------
// CCalenController::Infobar
// Descriptor passed to plugins to get customised info bar text.
// Acts as a conduit between the services and the customisation manager.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
HbWidget* CCalenController::Infobar()
    {
    OstTraceFunctionEntry0( DUP1_CCALENCONTROLLER_INFOBAR_ENTRY );
    
    OstTraceFunctionExit0( DUP1_CCALENCONTROLLER_INFOBAR_EXIT );
    return iCustomisationManager->Infobar();
    }
// ----------------------------------------------------------------------------
// CCalenController::InfobarTextL
// @returns info bar text
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
QString* CCalenController::InfobarTextL()
    {
    OstTraceFunctionEntry0( CCALENCONTROLLER_INFOBARTEXTL_ENTRY );
    
    OstTraceFunctionExit0( CCALENCONTROLLER_INFOBARTEXTL_EXIT );
    return iCustomisationManager->InfobarTextL();
    }
// ----------------------------------------------------------------------------
// CCalenController::CustomisationManager
// Returns a reference to the customisation manager
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenCustomisationManager& CCalenController::CustomisationManager()
    {
    OstTraceFunctionEntry0( CCALENCONTROLLER_CUSTOMISATIONMANAGER_ENTRY );
    
    OstTraceFunctionExit0( CCALENCONTROLLER_CUSTOMISATIONMANAGER_EXIT );
    return *iCustomisationManager;
    }

// ----------------------------------------------------------------------------
// CCalenController::SetDefaultContext
// Sets the default context for today
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//       
void CCalenController::SetDefaultContext()
    {
    OstTraceFunctionEntry0( CCALENCONTROLLER_SETDEFAULTCONTEXT_ENTRY );
      
    QDateTime focusTime = mContext->defaultCalTimeForViewsL();
    mContext->setFocusDateAndTime(focusTime);
    
    OstTraceFunctionExit0( CCALENCONTROLLER_SETDEFAULTCONTEXT_EXIT );
    }

// ----------------------------------------------------------------------------
// CCalenController::OfferMenu
// Offers the menu to plugins for customisation.
// Acts as a conduit between the services and the customisation manager.
// (other items were commented in a header).
// ----------------------------------------------------------------------------

void CCalenController::OfferMenu(HbMenu* aHbMenu)
    {
    OstTraceFunctionEntry0( CCALENCONTROLLER_OFFERMENU_ENTRY );
    
    iCustomisationManager->OfferMenu(aHbMenu);
    
    OstTraceFunctionExit0( CCALENCONTROLLER_OFFERMENU_EXIT );
    }

// ----------------------------------------------------------------------------
// CCalenController::agendaInterface
// returns the interface to the agenda database
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
AgendaUtil* CCalenController::agendaInterface()
    {
    OstTraceFunctionEntry0( CCALENCONTROLLER_AGENDAINTERFACE_ENTRY );
    
    
    OstTraceFunctionExit0( CCALENCONTROLLER_AGENDAINTERFACE_EXIT );
    return mAgendaUtil;
    }

// ----------------------------------------------------------------------------
// CCalenController::context
// returns the calendar context
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
MCalenContext& CCalenController::context()
    {
	OstTraceFunctionEntry0( CCALENCONTROLLER_CONTEXT_ENTRY );
	
    OstTraceFunctionExit0( CCALENCONTROLLER_CONTEXT_EXIT );
    return *mContext;
    }

// ----------------------------------------------------------------------------
// CCalenController::handleServiceManagerSlot
// Launches the requested view 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenController::handleServiceManagerSlot(int view, const QDateTime& dateTime)
{
	OstTraceFunctionEntry0( CCALENCONTROLLER_HANDLESERVICEMANAGERSLOT_ENTRY );
	
	// Broadcast the notification ECalenNotifyCloseDialogs 
	// to close the opened dialogs if any
	BroadcastNotification(ECalenNotifyCloseDialogs);
	
	if (iIsFromServiceFrmWrk) {
		// Set the context properly
		mContext->setFocusDateAndTime(dateTime);
		// launch the appropriate view
		iViewManager->constructAndActivateView(view);
		
		iIsFromServiceFrmWrk = false;
		
	} else {
		// Calendar was in backgroung but now its being brought to foreground
		
		// Remove the previous view to avoid flicker
		iViewManager->removePreviousView();
		
		// If current state is editing state or printing state
		// or deleting state or sending state, then dont do anything as
		// user might loose the data
		
		CCalenStateMachine::TCalenStateIndex currentState = 
												iStateMachine->CurrentState();
		
		if ((currentState == CCalenStateMachine::ECalenDeletingState) ||
			(currentState == CCalenStateMachine::ECalenPrintingState) ||
			(currentState == CCalenStateMachine::ECalenSendingState)) {
			// simply return - we dont have anything to do
		} else if (currentState == CCalenStateMachine::ECalenViewingState) {
			if(iViewManager->isEventViewerActive()) {
				iViewManager->closeAgendaEventView();
			}
		} else if (currentState == CCalenStateMachine::ECalenEditingState) {
			// close the editor and save the entry if application is 
			// in background and launch the desired view
			if(iViewManager->isEventViewerActive()) {
				// First close editor and then viewer
				iViewManager->saveAndCloseEditor();
				iViewManager->closeAgendaEventView();
			} else {
				// If viewer is not active, just close the editor
				iActionUi->saveAndCloseEditor();
			}
		} else if (currentState == CCalenStateMachine::ECalenSettingsState) {
			iViewManager->removeSettingsView();
		}
 
		// Set the context properly
		mContext->setFocusDateAndTime(dateTime);
		IssueCommandL(view);
		
		// connect to raise the window to foreground once the view is ready
		connect(&MainWindow(), SIGNAL(viewReady()), this, SLOT(raiseWindow()));
		
		// Dispatches all posted events of type DeferredDelete, 
		// i.e. empties the event queue in case any deletion is pending
		qApp->sendPostedEvents(0, QEvent::DeferredDelete);
	}

	OstTraceFunctionExit0( CCALENCONTROLLER_HANDLESERVICEMANAGERSLOT_EXIT );
}

void CCalenController::raiseWindow()
    {
    MainWindow().raise();
    disconnect(&MainWindow(), SIGNAL(viewReady()), 
                        this, SLOT(raiseWindow()));
    }
// ----------------------------------------------------------------------------
// CCalenController::getFirstView
// returns the first view with which calendar has been launched
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
int CCalenController::getFirstView()
{
	OstTraceFunctionEntry0( CCALENCONTROLLER_GETFIRSTVIEW_ENTRY );
	
	OstTraceFunctionExit0( CCALENCONTROLLER_GETFIRSTVIEW_EXIT );
	
	return iViewManager->getFirstView();
	
}

// ----------------------------------------------------------------------------
// CCalenController::eventFilter
// Filters and handles the changes in events
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
bool CCalenController::eventFilter(QObject *object, QEvent *event)
{
    OstTraceFunctionEntry0( CCALENCONTROLLER_EVENTFILTER_ENTRY );
    
    switch (event->type())
        {
        case QEvent::LanguageChange:
        	//TODO: Unload the translator and install the locale specific translator
        	iNotifier->BroadcastNotification( ECalenNotifySystemLanguageChanged );
            break;
        case QEvent::LocaleChange:
        	// TODO: handle the locale changes
            break;
        default:
            break;
        }
    OstTraceFunctionExit0( CCALENCONTROLLER_EVENTFILTER_EXIT );

    return QObject::eventFilter(object, event);
}

/*
 * Emits the appReady signal.
 */
void CCalenController::emitAppReady()
{
	emit appReady();
}

// End of file	--Don't remove this.
