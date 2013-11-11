/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     Active single call with call setup and waiting calls state implementation
*
*/


#ifndef CPHONESINGLEANDCALLSETUPANDWAITING_H
#define CPHONESINGLEANDCALLSETUPANDWAITING_H

// INCLUDES
#include "cphonegsmincall.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*  Active single call with call setup and waiting calls state implementation.
*/
class CPhoneSingleAndCallSetupAndWaiting : public CPhoneGsmInCall
    {
    public:  

        /**
        * Destructor.
        */
        virtual ~CPhoneSingleAndCallSetupAndWaiting();

        /**
        * Creates the Single And Call Setup And Waiting state class
        * @param aStateMachine: a container of state objects
        * @param aViewCommandHandle: handle to the PhoneUIView
        * @return an instance of class CPhoneStateIncoming
        */
        static CPhoneSingleAndCallSetupAndWaiting* NewL( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );

        /**
        * A message handling function for Phone Engine messages
        * @param aMessage Message from Phone Engine
        * @param aCallId Call id the message concerns
        */
        virtual void HandlePhoneEngineMessageL(
            const TInt aMessage, 
            TInt aCallId );     
            
        TBool HandleCommandL( TInt aCommand );  

        virtual void HandleKeyMessageL(
            TPhoneKeyEventMessages aMessage,
            TKeyCode aCode );

            
    protected:

        /**
        * By default EPOC constructor is private.
        */
        CPhoneSingleAndCallSetupAndWaiting( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );
        
        /**
        * ConstructL()
        */
        virtual void ConstructL();
            
     private:
     
        /**
        * Handles EPEMessageIdle
        */
        void HandleIdleL( TInt aCallId );

        /**
        * Handles EPEMessageConnecting
        */
        void HandleConnectingL( TInt aCallId );
                
        /**
        * Handles EPEMessageConnected
        */
        void HandleConnectedL( TInt aCallId );

        /**
        * State transition to state Call Setup and Waiting  
        */        
        void StateChangeToCallSetupAndWaitingL( TInt aCallId );

        /**
        * State transition to state Single and Waiting
        */        
        void StateChangeToSingleAndWaitingL( TInt aCallId );

        /**
        * State transition to state Single and Alerting
        */        
        void StateChangeToSingleAndAlertingL( TInt aCallId );

        /**
        * State transition to state Two Singles. 
        */        
        void StateChangeToTwoSinglesL( TInt aCallId );
        
        /**
        * State transition to state Two Singles and Waiting   
        */        
        void StateChangeToTwoSinglesAndWaitingL( TInt aCallId ); 
        
        void HandleAudioMuteChangedL();
        
      private:
      
        /**
        * Status of SingleAndCallSetupAndWaiting alerting
        */
        TBool iAlerting;
        
        /**
        * Waiting call id
        */
        TInt iWaitingCallId; 

    };

#endif // CPHONESINGLEANDCALLSETUPANDWAITING_H

// End of File
