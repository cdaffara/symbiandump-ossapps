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


#ifndef CALENCONTROLLER_H
#define CALENCONTROLLER_H

// System includes
#include <e32base.h>// CAsyncCallBack
#include <vwsdef.h>// TVwsViewId
#include <apadef.h>// TApaCommand
#include <qglobal.h>// Q_DECL_EXPORT macro
#include <agendautil.h>
#include <qobject>

// User includes
#include "hb_calencommands.hrh"// Calendar commands
#include "calennotificationhandler.h"// TCalenNotification
#include "calenservices.h"
#include "calenservicesfactory.h"       // MCalenServicesFactory


#ifdef  CALENCONTROLLER_DLL
#define CALENCONTROLLER_EXPORT Q_DECL_EXPORT
#else
#define CALENCONTROLLER_EXPORT Q_DECL_IMPORT
#endif

// FORWARD DECLARATIONS
class CalenViewManager;
class CalenView;
class CalenServicesImpl;
class HbMainWindow;
class CalenNotifier;
class CCalenStateMachine;
class MCalenCommandHandler;
class CCalenActionUi;
class HbWidget;
class CCalenCustomisationManager;
class CalenContextImpl;                // context implementation
class QString;
class HbMenu;

// CLASS DEFINITIONS

/**
 * The controller handles events from the rest of Calendar and delegates
 * them to the appropriate place (i.e. the action ui classes).
 */
class CALENCONTROLLER_EXPORT CCalenController : public QObject, public MCalenServicesFactory
    {
	Q_OBJECT
	
    public:  // Construction and destruction
		/**
		 * C++ constructor
		 */
		CCalenController();
		
		/**
		 * Second phase contruction for controller
		 */
		void constructController();

		/**
		 * Constructs CCalenController with existing CAknViewAppUi. If the
		 * controller has been previously initialized with the same CAknViewAppUi,
		 * the existing instance will be returned.
		 * @param aAppUi Reference to CAknViewAppUi
		 * @return CCalenController pointer
		 */
		static CCalenController* InstanceL();

		 
		 /**
		 * Releases all plugins, should only be called when 
		 * the application is exiting.
		 */
		void ReleaseCustomisations();

		/**
		 * CCalenController is a reference counting singleton. Call Release()
		 * when you are done with it, it will clean itself when it needs to
		 */
		void Release();
		
		/**
		 * destructor
		 */
		~CCalenController(); 

	signals:
		void appReady();
		
    public:  // New functions
        /**
         * Adds the passed command to the command queue. Command are handled
         * asynchronously in HandleCommandL
         * @param aCommand Command that is passed in.
         * @return EFalse if the passed command is not in the issuers command range
         * ETrue otherwise
         */
        TBool IssueCommandL( TInt aCommand );
         /**
         * Passes the notification to the calendar notifier. The notification will be
         * broadcast to all observers
         * @param aNotification Notification to be broadcast
         */
        void BroadcastNotification( TCalenNotification aNotification );
        
        /**
         * Register the passed notification handler with the calendar notifier
         * @param aHandler Notification handler
         * @param aNotification Notification that handler wants to be notified
         */ 
        void RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
                                                 TCalenNotification aNotification );
        
        
        /**
         * Register the passed notification handler with the calendar notifier
         * @param aHandler Notification handler
         * @param aNotifications Array of Notifications that handler wants to be notified
         */ 
        void RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
                                                 RArray<TCalenNotification>& aNotifications );
        
        /**
         * Unregister the passed handler with the notifier
         * @param aHandler Notification handler
         */
        void CancelNotifications( MCalenNotificationHandler* aHandler );
        
         /**
         * From MCalenServicesFactory
         */
        MCalenServices* NewServicesL();
        
        /**
         * Get infobar based on the available area for infobar
         * @param aRect The available area for Infobar
         * @return pointer to a Infobar control
         */
        HbWidget* Infobar( );
        
        /**        
         * Returns info bar text
         */
        QString* InfobarTextL();
        
        
        /**
         * Offer the menu to Plugins to customisation
         * @param aMenu Where the customisation apply to
         */
        void OfferMenu(HbMenu* aHbMenu);
        /**
         * returns the interface to the agenda database
         * @return reference to agendainterface
         */
        AgendaUtil* agendaInterface();
        /**
         * returns the calendar context
         * @return reference to the calendar context
         */
        MCalenContext& context();
        /**
         * Launches the requested view 
         */
        void handleServiceManagerSlot(int view, const QDateTime& dateTime);
        /**
		 * returns the first view with which calendar has been launched
		 * @return view Id
		 */
		int getFirstView();

    public:  // public but not exported
    
         /**
          * Get reference to the CCalenNotiffier
          */
        CalenNotifier& Notifier();
        
        /**
         * Get a command handler for a specific command
         * @param aCommand Command to find a command handler for
         */
        MCalenCommandHandler* GetCommandHandlerL( TInt aCommand );       
       
        /**
         * Get viewmanager
         * @return reference to CalenViewManager
         */
        CalenViewManager& ViewManager();
        
        /**
         * Get MainWindow
         * @return reference to HbMainWindow
         */
        HbMainWindow& MainWindow();
        
        /**
         * Get services
         * @return reference to controller's services object
         */
        MCalenServices& Services();
        /**
         * Get customisationmanager
         * @return reference to CCalenCustomisationManager
         */
        CCalenCustomisationManager& CustomisationManager();
        
	    /**
         * Filters the events eg. Locale/language change events
         * @param event
         */
        bool eventFilter(QObject *object, QEvent *event);
        
        /**
         * Emits appReady signal.
         */
        void emitAppReady();
        
    private:  // Construction and destruction
        
        /**
         * Sets the default context for today
         */        
        void SetDefaultContext();
        /**
		* Stores the controller object in TLS
		*/
        void checkMultipleCreation();
        
private slots:
    void raiseWindow();
   
    private:  // Data
        CalenViewManager*  iViewManager;// Activate / deactivate views
        CalenContextImpl* mContext; 
        AgendaUtil *mAgendaUtil; // Agenda interface provider 
        CalenServicesImpl* iServices;   // Services. 
        CalenNotifier*     iNotifier;   // Broadcasts calendar events
        CCalenStateMachine* iStateMachine;
        CCalenActionUi*     iActionUi;   // Default internal command handling
        // Manages any customisation plugins
        CCalenCustomisationManager* iCustomisationManager;
        
        TInt iRefCount;  // Reference counting singleton  
        // Start point of the next command range
        // to be assigned to a services instance.
        TInt iNextServicesCommandBase;
        bool iIsFromServiceFrmWrk;
		
    };

#endif // CALENCONTROLLER_H

// End of file
