/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header file for CNSmlDMDbNotifier
*
*/


#ifndef NSMLDMDBNOTIFIER_H
#define NSMLDMDBNOTIFIER_H

#include <e32std.h>
#include <SyncMLObservers.h>
#include <SyncMLClient.h>
#include "nsmldmsyncutil.h"

// CLASS DECLARATION

/**
* DB events
* 
*/
NONSHARABLE_CLASS (TNSmlDMDbEvent)
	{
	public:
		TInt iType;	
		TInt iProfileId;
		TInt iError;
	};

/**
* Interface class to handle database events
* 
*/
class MNSmlDMDbEventHandler
	{

	public:
		/**
        * CNSmlDMDbNotifier calls this to inform that database has changed.
        * @param aStatus EClose, EUnlock, ECommit, ERollback, or ERecover (d32dbms.h).
		* @param aMoreEvents ETrue issue new request, EFalse no requests.
        * @return None. 
        */
		virtual void HandleDbEventL( TNSmlDMDbEvent aEvent ) = 0;
	};
	
/**
* DB notifier
* 
*/
NONSHARABLE_CLASS (CNSmlDMDbNotifier) : public CBase,
                                        public MNSmlDMActiveCallerObserver,
                                        public MSyncMLEventObserver
	{	
    public:
	    enum TEventType
			{
			EUpdate,
			EUpdateAll,
			EDelete,
	        EClose
			};

	public:
		/**
        * Two-phased constructor
        * @param aDatabase Reference to RDbDatabase instance.
		* @param aHandler Pointer to MNSmlDMDbEventHandler instance.
        * @return None. 
        */
		static CNSmlDMDbNotifier* NewL( RSyncMLSession* aSyncSession,
		                                MNSmlDMDbEventHandler* aHandler );

        /**
        * Destructor.
        */
		virtual ~CNSmlDMDbNotifier();

		// Functions from base classes

	private:

		/**
        * Constructor
        * @param aDatabase Reference to RDbDatabase instance.
		* @param aHandler Pointer to event handler.
        * @return None. 
        */
		CNSmlDMDbNotifier( RSyncMLSession* aSyncSession,
		                   MNSmlDMDbEventHandler* aHandler);

        /**
        * By default Symbian OS constructor is private.
        */
		void ConstructL();

	public:
		/**
		* Start requesting events from db.
        * @param None. 
        * @return None. 
        */
		void RequestL();

   
	private:   // from MNSmlDMActiveCallerObserver
	
		/**
        * From MNSmlDMActiveCallerObserver
		* Called when CNSmlDMActiveCaller completes.
        * @param None
        * @return None.
        */
		void HandleActiveCallL();

    private:    // from MSyncMLEventObserver
        
		/**
        * From MSyncMLEventObserver
		* Called when SyncML session events oocur.
        * @param aEvent.
        * @param aIdentifier.
        * @param aError.
        * @param aAdditionalData.
        * @return None.
        */
        void OnSyncMLSessionEvent( TEvent aEvent,
                                   TInt aIdentifier,
                                   TInt aError,
                                   TInt aAdditionalData);

    public:
		/**
		* Call observer with delay
		* @param None
		* @return None
        */
		void CallObserverWithDelay();
		
		/**
		* Call observer
		* @param None
		* @return None
        */		
		void CallObserver();
		
		/**
		* Set as disabled
		* @param aDisable
		* @return None
        */		
		void SetDisabled( TBool aDisable );
		
		/**
		* Change profile count
		* @param None
		* @return TInt
        */		
		TInt ChangedProfileCount();
		
		/**
		* DB event
		* @param aIndex
		* @return db event
        */        
        TNSmlDMDbEvent Event(TInt aIndex);
		
		/**
		* Event count
		* @param None
		* @return TInt
        */		
		TInt EventCount();
		
		/**
		* Reset
		* @param None
		* @return None
        */		
		void Reset();
		
		/**
		* Find event
		* @param None
		* @return TInt
        */
		TInt FindCloseEvent();
		
		/**
		* Find event
		* @param None
		* @return TInt
        */
		TInt FindSingleProfileEvent();
		
		/**
		* Find event
		* @param None
		* @return TInt
        */
		TInt FindProfileEvent();
		
		/**
		* Check if profile id is unique
		* @param aId Profile id
		* @return ETrue if unique
        */
		TBool IsUniqueProfileId(TInt aId);
		
		/**
		* Utility function.
		* @return Sync session.
        */
		RSyncMLSession& Session();

	private:  
		// sync session
		RSyncMLSession* iSyncSession;
		// Pointer to event handler
		MNSmlDMDbEventHandler* iHandler;
		// for making function call via active scheduler
		CNSmlDMActiveCaller* iActiveCaller;
		// for storing ids of changed profiles
		RArray<TNSmlDMDbEvent> iList;
		// can observer be called
		TBool iDisabled;
		// is synchronizing going on
		TBool iSyncRunning;
	};

#endif // NSMLDMDBNOTIFIER_H

// End of File
