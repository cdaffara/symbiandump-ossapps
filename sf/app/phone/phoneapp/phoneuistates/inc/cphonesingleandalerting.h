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
*     GSM-specific Single and Alerting state implementation.
*
*/


#ifndef CPHONESINGLEANDALERTING_H
#define CPHONESINGLEANDALERTING_H

// INCLUDES
#include "cphonealerting.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*  GSM-specific Single and Alerting state
*/
class CPhoneSingleAndAlerting : public CPhoneAlerting
    {
    public:  

        /**
        * Destructor.
        */
        virtual ~CPhoneSingleAndAlerting();

        /**
        * Creates the GSM-specific Single call state class
        * @param aStateMachine: a state machine
        * @param aViewCommandHandle: handle to the PhoneUIView
        * @return an instance of class CPhoneSingleAndAlerting
        */
        static CPhoneSingleAndAlerting* NewL( 
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
        
        
    protected:

        /**
        * By default EPOC constructor is private.
        */
        CPhoneSingleAndAlerting( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );
        
        /**
        * ConstructL()
        */
        virtual void ConstructL();
            
        /**
        * A message handling function for EPEMessageHandleConnected
        * @param aCallId: the call id of the call
        */
        void HandleConnectedL( TInt aCallId );
        
        /**
        * Handle EPEMessageIdle
        */
        void HandleIdleL( TInt aCallId );
        
    };

#endif // CPhoneSingleAndAlerting_H

// End of File
