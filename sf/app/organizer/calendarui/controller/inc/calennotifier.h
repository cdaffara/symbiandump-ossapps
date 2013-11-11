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
* Description:  Calendar notifier
*
*/


#ifndef CALENNOTIFIER_H
#define CALENNOTIFIER_H

// INCLUDES
#include <e32hashtab.h>                 //RHashSet
#include "calennotificationhandler.h"   // MCalenNotificationHandler
#include "calencontextchangeobserver.h" // MCalenContextChangeObserver

// FORWARD DECLARATIONS
class CEnvironmentChangeNotifier;       // Receive system event notifications
class CCalenStateMachine;

/**
 * CalenNotifier observes various system events relevent to Calendar
 * MCalenNotificationHandlers can register for notifications of specific
 * events. This minimises the number of event observers required
 * throughout the Calendar application.
 */
class CalenNotifier :  public CBase,   
                       public MCalenContextChangeObserver
    {
    public:
        /**
         * C++ default Constructor
         */
        CalenNotifier( CCalenStateMachine& aStateMachine );

        /**
         * 2nd phase of construction.
         */
        void ConstructL();

        /**
         * Destructor
         */
        virtual ~CalenNotifier();

    public:
        /**
         * Allows MCalenNotificationHandlers to register for system event
         * notifications
         * @param aHandler Handler to register
         * @param aFlag Notification of events to notify aHandler about
         */
        void RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
                                                TCalenNotification aNotification );
        
        /**
         * Allows MCalenNotificationHandlers to register for system event
         * notifications
         * @param aHandler Handler to register
         * @param aFlags Array of notifications to  notify aHandler about
         */
        void RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
                                                 RArray<TCalenNotification>& aNotifications );

        /**
         * Allows MCalenNotificationHandlers to cancel system event
         * notifications
         * @param aHandler Handler to deregister
         */
        void CancelNotifications( MCalenNotificationHandler* aHandler );

        /**
         * Notify all interested handlers about an event
         * @param aNotification Notification to broadcast
         */
        void BroadcastNotification( TCalenNotification aNotification );
        
        /**
         * Notify all interested handlers about an event
         * @param aNotification Notification to broadcast
         */
        void BroadcastApprovedNotification( TCalenNotification aNotification );
        
    public:       

        /**
         * From MCalenContextChangeObserver.
         * Called when the context changes.
         */
        void ContextChanged();

    private:    // Callback functions from observers
        
        /**
         * Called from CEnvironmentChangeNotifier when the
         * system environment changes
         * @param aThisPtr self pointer
         * @return EFalse
         */
        static TInt EnvChangeCallbackL( TAny* aThisPtr );
        
        /**
         * Broadcasts the given notification.
         * @param aNotification Notification id to be broadcasted
         */
        void DoBroadcast( TCalenNotification aNotification );  
        
        TInt DoEnvChange();
       

    private:  // Data
        // Array of handlers to notify
        class TNotificationHandler
            {  
            public:
                TNotificationHandler();
                
                RHashSet<TCalenNotification>  iHashSet;
                MCalenNotificationHandler* iHandler;
            };

        RArray<TNotificationHandler> iHandlers;

        RArray<TCalenNotification> iBroadcastQueue;
        TBool iBroadcastActive;
        
        CCalenStateMachine& iStateMachine; 
        
        // Notifications about locale and time changes
        CEnvironmentChangeNotifier* iEnvChangeNotifier;
        
        bool iIgnoreFirstLocaleChange;

    };

#endif // CALENNOTIFIER_H

// End of file
