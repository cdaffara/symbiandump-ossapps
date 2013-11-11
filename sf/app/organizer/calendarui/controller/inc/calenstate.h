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


#ifndef CALENSTATE_H
#define CALENSTATE_H

// INCLUDES
#include <e32base.h>
#include <e32hashtab.h>
#include "calennotificationhandler.h"       // TCalenNotification
#include "calenstatemachine.h"              // TCalenStateIndex

class CCalenController;
class CalenAsyncCallBack;
class CalenCallbackPackage;

// CLASS DEFINITIONS

/**
* Calendar state
*/
class CCalenState : public CBase
    {
    public:
        virtual TBool HandleCommandL( const TCalenCommand& aCommand,
                                      CCalenStateMachine& aStateMachine ) = 0;
        virtual void HandleNotificationL(const TCalenNotification& aNotification,
                                          CCalenStateMachine& aStateMachine );
             
        virtual void HandleStateActivationL(CCalenStateMachine& aStateMachine);
        
        void CancelExecutingCmd();
        
        CCalenStateMachine::TCalenStateIndex PreviousState();
        
        void SetPreviousState(const CCalenStateMachine::TCalenStateIndex& aPreviousState );
        
        ~CCalenState();
         
	protected:
	
		/**
         * Interface to set the current state
         */
		void SetCurrentState( CCalenStateMachine& aStateMachine,
		                     const CCalenStateMachine::TCalenStateIndex& aState );
		/**
         * Interface to get the current state
         */        
        CCalenStateMachine::TCalenStateIndex GetCurrentState(
        									CCalenStateMachine& aStateMachine);
        
        /**
         * Interface to set the previous state through state machine
         */
		void SetCurrentPreviousState(CCalenStateMachine& aStateMachine, 
							const CCalenStateMachine::TCalenStateIndex& aState);
		
		/**
         * Interface to activate the current state
         */
		void ActivateCurrentStateL(CCalenStateMachine& aStateMachine);
        
        /**
         * Interface to cancel the previous command
         */
		void CancelPreviousCmd(CCalenStateMachine& aStateMachine);						
    
    protected:
        virtual void CommandExecuting();
        void RequestCallbackL( MCalenCommandHandler* aCommandHandler, const TCalenCommand& aCommand );

    protected:
        void BaseConstructL();
        CCalenState( CCalenController& aController, RHashSet<TCalenNotification>&  aOutstandingNotifications );

    private:
        class CalenCallbackPackage : public CBase // going on heap
            {
            public: 
                CalenCallbackPackage( CCalenState* aSelf,
                                       TCalenCommand aCommand,
                                       MCalenCommandHandler* aCommandHandler);
                
                TBool HandleCallBack();
                void SetCommandHandler( CCalenState* aSelf,
                                        TCalenCommand aCommand,
                                        MCalenCommandHandler* aCommandHandler);
            private:
                CCalenState* iSelf; // not own
                TCalenCommand iCommand;
                MCalenCommandHandler* iCommandHandler; // not own
            };
        static TInt CommandCallback( TAny* aCommandStruct );
        CalenCallbackPackage* iCallBackPackage;

    protected:
        CCalenController& iController;
        RHashSet<TCalenNotification>& iOutstandingNotifications;
        CCalenStateMachine::TCalenStateIndex iPreviousState;

    private:
        CalenAsyncCallBack*     iCmdCallback;
     };


#endif // CALENSTATE_H

// End of file
