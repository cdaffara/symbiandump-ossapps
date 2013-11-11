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
* Description:  Provides services for native Calendar extensions
*
*/

#ifndef CALENSERVICESIMPL_H
#define CALENSERVICESIMPL_H

#include <e32base.h>

#include "calennotificationhandler.h"   // TCalenNotification
#include "calenservices.h"

// Forward declarations
class CCalenController;     // Calendar controller
class HbMainWindow;
class HbWidget;
class QString;
class HbMenu;

/**
 * Calendar Services External API implementation.
 */
class CalenServicesImpl  : public MCalenServices
    {
    	
    public:  // Construction and destruction.
        /**
         * Two phased constructor. Use this in preference to the C++
         * constructor where possible.
         * @param aCommandRangeStart    start of the command range
         * @param aCommandRangeEnd      end of command range
         */
        static CalenServicesImpl* NewL( TInt aCommandRangeStart,
                                                      TInt aCommandRangeEnd );
        /**
         * Two phased constructor. Use this in preference to the C++
         * constructor where possible.
         */
        static CalenServicesImpl* NewL();
        
        /**
         * Completes construction of this object.
         */
        void ConstructL();

        /**
         * Performs cleanup of this object.
         */
        void Release();

    public:  // Functions from base classes.
    
    		/**
         * Get MainWindow
         * @return reference to HbMainWindow
         */
        HbMainWindow& MainWindow();
        
        /**
         * Retrieve the calendar session currently in use by Calendar
         * @return CCalSession& currently used by Calendar
         */
        CCalSession& SessionL();

        /**
         * Retrieve the calendar entry view currently in use by Calendar
         * @return CCalEntryView* currently used by Calendar
         */
        CCalEntryView* EntryViewL();

        /**
         * Retrieve the calendar instance view currently in use by Calendar
         * @return CCalInstanceView* currently used by Calendar
         */
        CCalInstanceView* InstanceViewL();


        
        /**
         * Register for notifications of Calendar events
         * @param aHandler the MCalenNotificationHandler to notify
         * @param aNotification single notification to be notified about
         */
        virtual void RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
                                                        TCalenNotification aNotification );
        
        /**
         * Register for notifications of Calendar events
         * @param aHandler the MCalenNotificationHandler to notify
         * @param aNotifications array of notifications to be notified about
         */
        virtual void RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
                                                        RArray<TCalenNotification>& aNotifications );

        /**
         * Cancel notifications of Calendar events
         * @param aHandler the MCalenNotificationHandler to stop notifying
         */
        void CancelNotifications( MCalenNotificationHandler* aHandler );

        
        /**
         * Issue a notification to Calendar, which will be broadcast
         * synchronously to all registered notification handlers.
         * Only one notification may be issued at once.
         * @param aNotification the notification to broadcast
         */
        void IssueNotificationL( TCalenNotification aNotification );
        
        /**
         * Request activation of a specific view
         * @param aViewId The view to be activated
         */
        void RequestActivationL( const TVwsViewId& aViewId );

        /**
         * Notify Calendar that a specific view has been activated.
         * Custom views must call this on activation
         * @param aViewId The view that was activated
         */
        void ActivationNotificationL( const TVwsViewId& aViewId );
  
    
        /**
         * Issue a command to be handled by Calendar or a customization
         * All commands will be handled asynchronously.  Calendar may reject
         * @param aCommand the command to be handled
         * @return ETrue, if Calendar will attempt to handle the command
         * @return EFalse, if Calendar will not attempt to handle the command
         */
        TBool IssueCommandL( TInt aCommand);
        
        /**
         * Returns the context. This includes information such
         * as the currently focused date/instance.
         */
        MCalenContext& Context();
        /**        
         * a control to be displayed in the infobar
         * @return info bar control
         */
        HbWidget* Infobar();
        /**        
         * Returns info bar text
         */
        QString* InfobarTextL();
		
		/**
         * Gets the command range that has been allocated to this MCalenServices 
         * instance
         * 
         * @param aCommandRangeStart start of command range
         * @param aCommandRangeEnd end of command range
         */
        void GetCommandRange( TInt& aCommandRangeStart, 
                                          TInt& aCommandRangeEnd ) const;
        
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
		 * returns the first view with which calendar has been launched
		 * @return view Id
		 */
		int getFirstView();
	    
    private:
        /**
         * C++ constructor.
         */
        CalenServicesImpl( TInt aCommandRangeStart, 
                                    TInt aCommandRangeEnd );

    private:  // Data
        CCalenController* iController;
        
        TInt iCommandRangeStart; // start of the valid command range
        TInt iCommandRangeEnd;   // end of the valid command range
    };

#endif // CALENSERVICESIMPL_H

// End of file
