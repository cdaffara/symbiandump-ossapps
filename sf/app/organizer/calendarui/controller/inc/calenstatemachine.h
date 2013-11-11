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


#ifndef CALENSTATEMACHINE_H
#define CALENSTATEMACHINE_H

// INCLUDES
#include <e32base.h>
#include <e32hashtab.h>
#include "calennotificationhandler.h"
#include "calencommandhandler.h"
#include "calenhashfunctions.inl"       // HashFunctions 

class CCalenState;
class CCalenController;

// CLASS DEFINITIONS

/**
 * 
 */
class CCalenStateMachine : public CBase,
                           public MCalenNotificationHandler
    {
	public:  // Enumerations
	    enum TCalenStateIndex
            {
            // If you change the array order or add/remove items,
            // update CCalenStateMachine::ConstructL too.
	        ECalenIdleState,
	        ECalenPopulationState,	        
	        ECalenBackgroundState,
	        ECalenViewingState,
	        ECalenEditingState,	
	        ECalenDeletingState,
	        ECalenPrintingState,
	        ECalenSendingState,
	        ECalenSettingsState,
	        ECalenHelpState,
	        ECalenExitingState,	       
	        KCalenLastState  // Must be last
	        };

    public:  // Construction and destruction
        /**
         * Symbian 1st phase constructor
         */
        static CCalenStateMachine* NewL( CCalenController& aController );
        
        /**
         * Destructor
         */
        virtual ~CCalenStateMachine();

        TBool HandleCommandL( const TCalenCommand& aCommand );
	
	public: // from MCalenNotificationHandler
		
		/**
		 * Handles Notifications 
 		 */
        void HandleNotification(const TCalenNotification aNotification );
        
        /*
         * Returns the current state in which calendar is.
         */
        TCalenStateIndex CurrentState();

    private:  // Construction and destruction
        
        /**
         * C++ constructor
         */
        CCalenStateMachine( CCalenController& aController );
        
        /**
         * Symbian 2nd phase constructor
         */
        void ConstructL();
	
	private: 
	
		/**
         * Interface to cancel the previous command
         */
		void CancelPreviousCmd();
		
		/**
         * Interface to get the current state
         */
		TCalenStateIndex GetCurrentState();
		
		/**
         * Interface to set the current state
         */
		void SetCurrentState(const TCalenStateIndex& aState);
		
		/**
         * Interface to set the previous state
         */
		void SetCurrentPreviousState(const TCalenStateIndex& aState);
		
		/**
		 * Interface for activating the current state
		 */
		void ActivateCurrentStateL();
    
    private:
    	
    	friend class CCalenState;
        CCalenController& iController;
        RPointerArray<CCalenState> iStates;
        TCalenStateIndex iCurrentState;
        RHashSet<TCalenNotification> iOutstandingNotifications;
    };

#endif // CALENSTATEMACHINE_H

// End of file
