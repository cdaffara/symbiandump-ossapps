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
*     GSM-specific alerting state implementation.
*
*/


#ifndef CPHONEALERTING_H
#define CPHONEALERTING_H

// INCLUDES
#include "cphonegsmincall.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*  GSM-specific alerting state
*/
class CPhoneAlerting : public CPhoneGsmInCall
    {
    public:  

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CPhoneAlerting();

        /**
        * Creates the GSM-specific Single call state class
        * @param aStateMachine: a state machine
        * @param aViewCommandHandle: handle to the PhoneUIView
        * @return an instance of class CPhoneAlerting
        */
        static CPhoneAlerting* NewL( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );

        /**
        * A message handling function for Phone Engine messages
        * @param aMessage Message from Phone Engine
        * @param aCallId Call id the message concerns
        */
        IMPORT_C virtual void HandlePhoneEngineMessageL(
            const TInt aMessage, 
            TInt aCallId );

                  
    protected:

        /**
        * By default EPOC constructor is private.
        */
        IMPORT_C CPhoneAlerting( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );
        
        /**
        * ConstructL()
        */
        IMPORT_C virtual void ConstructL();

        /**
        * A message handling function for EPEMessageHandleConnected
        * @param aCallId: the call id of the call
        */
        IMPORT_C void HandleConnectedL( TInt aCallId );
        
        IMPORT_C virtual void HandleKeyMessageL(
            TPhoneKeyEventMessages aMessage,
            TKeyCode aCode );
        
       /**
        * Handles user selected UI commands.
        * @param aCommand - selected command
        */
        IMPORT_C virtual TBool HandleCommandL( TInt aCommand );
 
        /**
        * Check is alerting call Video call
        */
        TBool IsVideoCallAlertingL();
                
        /**
        * From CPhoneState.
        * Checks if call termination note should be shown.
        * @return ETrue show call termination note
        */
        IMPORT_C virtual TBool CheckIfShowCallTerminationNote();
        
        /**
        * A message handling function for EPEMessageDisconnecting
        * @param aCallId: the call id of the call
        */
        IMPORT_C void HandleDisconnectingL( TInt aCallId );
        
    private:
        
        /** 
         * Call identifier for a incoming call. 
         */
        TInt iIncomingCallId;
    };

#endif // CPHONEALERTING_H

// End of File
