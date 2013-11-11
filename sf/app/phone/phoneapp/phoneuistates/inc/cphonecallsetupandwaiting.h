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
*     Call Setup And Waiting state implementation.
*
*/


#ifndef CPHONECALLSETUPANDWAITING_H
#define CPHONECALLSETUPANDWAITING_H

// INCLUDES
#include "cphonegsmincall.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*  Call Setup And Waiting state
*/
class CPhoneCallSetupAndWaiting : public CPhoneGsmInCall
    {
    public:  

        /**
        * Destructor.
        */
        virtual ~CPhoneCallSetupAndWaiting();

    public: // New functions
        
        /**
        * Creates the Call Setup And Waiting state class
        * @param aStateMachine: a state machine
        * @param aViewCommandHandle: handle to the PhoneUIView
        * @return an instance of class CPhoneSingleAndAlerting
        */
        static CPhoneCallSetupAndWaiting* NewL( 
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
        
    protected:

        /**
        * By default EPOC constructor is private.
        */
        CPhoneCallSetupAndWaiting( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );
        
        /**
        * ConstructL()
        */
        virtual void ConstructL();
        
    private:
        
        void HandleIdleL( TInt aCallId );
        
        /**
        * Handles EPEMessageConnected
        */
        void HandleConnectedL( TInt aCallId );
        
    };

#endif // CPHONECALLSETUPANDWAITING_H

// End of File
