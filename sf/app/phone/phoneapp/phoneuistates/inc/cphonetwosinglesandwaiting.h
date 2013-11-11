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
*     Two single calls and waiting call state implementation.
*
*/


#ifndef CPHONETWOSINGLESANDWAITING_H
#define CPHONETWOSINGLESANDWAITING_H

// INCLUDES
#include "cphonetwosingles.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*  Two single calls and waiting call state
*/
class CPhoneTwoSinglesAndWaiting : public CPhoneTwoSingles
    {
    public:  

        /**
        * Destructor.
        */
        virtual ~CPhoneTwoSinglesAndWaiting();

        /**
        * Creates the Two Singles And Waiting state class
        * @param aStateMachine: a state machine
        * @param aViewCommandHandle: handle to the PhoneUIView
        * @return an instance of class CPhoneSingleAndAlerting
        */
        static CPhoneTwoSinglesAndWaiting* NewL( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );

        virtual void HandleKeyMessageL(
            TPhoneKeyEventMessages aMessage,
            TKeyCode aCode );

        /**
        * A message handling function for Phone Engine messages
        * @param aMessage Message from Phone Engine
        * @param aCallId Call id the message concerns
        */
        virtual void HandlePhoneEngineMessageL(
            const TInt aMessage, 
            TInt aCallId );       
        
        /**
        * HandleError
        * Implements error handling framework
        * @param aErrorInfo: the error info
        */
        IMPORT_C virtual void HandleErrorL( 
            const TPEErrorInfo& aErrorInfo );

        /**
        * Handles user selected UI commands.
        * @param aCommand - selected command
        */    
        TBool HandleCommandL( TInt aCommand );
        
    protected:

        /**
        * By default EPOC constructor is private.
        */
        CPhoneTwoSinglesAndWaiting( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );
        
        /**
        * ConstructL()
        */
        virtual void ConstructL();
        
        virtual void HandleConnectedConferenceL( TInt aCallId );
        
     private:

        void HandleIdleL( TInt aCallId );
        
        void StateTransitionToTwoSinglesL();
        
        void StateTransitionToSingleAndWaitingL();
        
        /** 
        * Handle state-specific behaviour when number entry is cleared
        */ 
        void HandleNumberEntryClearedL();
        
        // call id of ringing call
        TInt iRingingCallId;
    };

#endif // CPHONETWOSINGLESANDWAITING_H

// End of File
