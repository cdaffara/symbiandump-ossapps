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

#ifndef CALENSERVICES_H
#define CALENSERVICES_H



#include <w32std.h> // needed for enum TEventCode
#include "calennotificationhandler.h"
#include "hb_calencommands.hrh"

// Forward declarations
class CCalEntryView;        // View for accessing calendar entries
class CCalInstanceView;     // View for accessing instances of calendar entries
class CCalSession;          // Handle to the Calendar file
class CalenServicesImpl;   // Services API implementation
class AgendaUtil;
class TVwsViewId;
class HbMainWindow;
class MCalenContext;        // Current calendar focus information
class HbWidget;
class QString;
class HbMenu;

/**
 * Calendar Services External API. This class is not intended for user
 * derivation - an object of this type is passed to customisations upon
 * construction.
 */
class MCalenServices 
    {
    public:
    	/**
         * Get MainWindow
         * @return reference to HbMainWindow
         */
        virtual HbMainWindow& MainWindow() = 0;
               
				/**
         * Register for notifications of Calendar events
         * @param aHandler the MCalenNotificationHandler to notify
         * @param aNotification single notification to be notified about
         */
        virtual void RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
                                                        TCalenNotification aNotification )=0;
        
        /**
         * Register for notifications of Calendar events
         * @param aHandler the MCalenNotificationHandler to notify
         * @param aNotifications array of notifications to be notified about
         */
        virtual void RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
                                                    RArray<TCalenNotification>& aNotifications )=0;

        /**
         * Cancel notifications of Calendar events
         * @param aHandler the MCalenNotificationHandler to stop notifying
         */
        virtual void CancelNotifications( MCalenNotificationHandler* aHandler )=0;
        

        /**
         * Issue a notification to Calendar, which will be broadcast
         * synchronously to all registered notification handlers.
         * Only one notification may be issued at once.
         * @param aNotification the notification to broadcast
         */
        virtual void IssueNotificationL( TCalenNotification aNotification )=0;
       
        /**
         * Request activation of a specific view
         * @param aViewId The view to be activated
         */
        virtual void RequestActivationL( const TVwsViewId& aViewId )=0;

        /**
         * Notify Calendar that a specific view has been activated.
         * Custom views must call this on activation
         * @param aViewId The view that was activated
         */
        virtual void ActivationNotificationL( const TVwsViewId& aViewId )=0;

        
        /**
         * Cleans all resources assocaited with this object and frees its memory.
         */
        virtual void Release()=0;
   
    	/**
         * Issue a command to be handled by Calendar or a customization
         * All commands will be handled asynchronously.  Calendar may reject
         * @param aCommand the command to be handled
         * @return ETrue, if Calendar will attempt to handle the command
         * @return EFalse, if Calendar will not attempt to handle the command
         */
        virtual TBool IssueCommandL( TInt aCommand  )=0;
        
        /**
         * Returns the context. This includes information such
         * as the currently focused date/instance.
         */
        virtual MCalenContext& Context()=0;

		 /**         
          * a control to be displayed in the infobar
          * @return info bar control
          */
        virtual HbWidget* Infobar( )=0;
        
        /**
          * Returns info bar text
          */
        virtual QString* InfobarTextL() = 0;
        
        /**
          * Offers the menu  for population, interested parties can
          * add or remove menu items
          * @param aHbMenu meu  to be populated          
          */
        virtual void OfferMenu(HbMenu* aHbMenu) = 0 ;
		
		/**
         * Gets the command range that has been allocated to this MCalenServices
         * instance
         *
         * @param aCommandRangeStart start of command range
         * @param aCommandRangeEnd end of command range
         */
        virtual void GetCommandRange( TInt& aCommandRangeStart, TInt& aCommandRangeEnd ) const=0;
        
        /**
        * returns the interface to the agenda database
        * @return reference to agendainterface
        */
        virtual AgendaUtil* agendaInterface() = 0;
        
        /**
		 * returns the first view with which calendar has been launched
		 * @return view Id
		 */
		virtual int getFirstView() = 0;

    protected:
        
        /**
         * Protected destructor. Use Release() instead
         */
        ~MCalenServices(){ };
    };

#endif // CALENSERVICES_H

// End of file
