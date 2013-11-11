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
*     Single And Waiting state implementation.
*
*/


#ifndef CPHONESINGLEANDWAITING_H
#define CPHONESINGLEANDWAITING_H

// INCLUDES
#include "cphonegsmincall.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*  Incoming call at single state implementation.
*/
class CPhoneSingleAndWaiting : public CPhoneGsmInCall
    {
    public:  

        /**
        * Destructor.
        */
        virtual ~CPhoneSingleAndWaiting();
        
        /**
        * Creates the Single And Waiting state class
        * @param aStateMachine: a container of state objects
        * @param aViewCommandHandle: handle to the PhoneUIView
        * @return an instance of class CPhoneStateIncoming
        */
        static CPhoneSingleAndWaiting* NewL( 
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
        
        /**
        * Handles user selected UI commands.
        * @param aCommand - selected command
        */
        TBool HandleCommandL( TInt aCommand );
            
    protected:

        /**
        * By default constructor is private.
        */
        CPhoneSingleAndWaiting( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );
            
        /**
        * HandleError
        * Implements error handling framework
        * @param aErrorInfo: the error info
        */
        IMPORT_C virtual void HandleErrorL( 
            const TPEErrorInfo& aErrorInfo );    
        
        /**
        * ConstructL()
        */
        virtual void ConstructL();
        
        /**
        * Handles key messages.
        * @param aMessage - key event message
        * @param aScanCode - standard Symbian scan code
        */
        virtual void HandleKeyMessageL(
            TPhoneKeyEventMessages aMessage,
            TKeyCode aCode );

     private:
     
        /**
        * Unholds the call.
        * @param aCallId - call id to unhold
        */
        void HandleUnholdL( TInt aCallId );
     
        /**
        * Performs a state transition to Two Singles.
        * @param aCallId - call id that got connected
        */
        void MakeStateTransitionToTwoSinglesL( TInt aCallId );
        
        /**
        * Handles received idle message from Phone Engine.
        * @param aCallId - call id that received the event
        */
        void HandleIdleL( TInt aCallId );
        
        /**
        * Handles received held message from Phone Engine.
        * @param aCallId - call id that received the event
        */ 
        void HandleHeldL( TInt aCallId );
        
        /**
        * Handles received connected message from Phone Engine.
        * @param aCallId - call id that received the event
        */ 
        void HandleConnectedL( TInt aCallId );
        
        /**
        * Handles received dialing message from Phone Engine.
        * @param aCallId - call id that received the event
        */ 
        void HandleDialingL( TInt aCallId );
        
        /**
        *  Handles received EPEMessageDisconnecting message from Phone Engine.
        * @param aCallId: the call id of the call
        */
        void HandleDisconnectingL( TInt aCallId );
        
    private: //Data
            
        // Store single call id
        TInt iSingleCallId;
                           
    };

#endif // CPHONESINGLEANDWAITING_H

// End of File
