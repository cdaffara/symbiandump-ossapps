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
*     GSM-specific emergency state implementation.
*
*/


#ifndef CPHONEEMERGENCY
#define CPHONEEMERGENCY

// INCLUDES
#include "cphonegsmincall.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*  GSM-specific emergency single call state
*/
class CPhoneEmergency : public CPhoneGsmInCall
    {
    public:  

        /**
        * Destructor.
        */
        virtual ~CPhoneEmergency();
        
        /**
        * Creates the Emergency state class
        * @param aStateMachine: a state machine
        * @param aViewCommandHandle: handle to the PhoneUIView
        * @return an instance of class CPhoneEmergency
        */
        static CPhoneEmergency* NewL( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );

        /**
        * A message handling function for Phone Engine messages
        * @param aMessage Message from Phone Engine
        * @param aCallId Call id the message concerns
        */
        void HandlePhoneEngineMessageL(
            const TInt aMessage, 
            TInt aCallId );    
                        
        /** 
        * Set startup status for state transition
        * @param aStartupInterrupted status
        */ 
        void SetStartupInterrupted( const TBool aStartupInterrupted );
        
    protected:

        /**
        * See CPhoneState
        */
        void HandleIdleL( TInt aCallId );

        /**
        * See CPhoneState
        */
        void HandleConnectedL( TInt aCallId );
        
        /**
        * See CPhoneState
        */
        void HandleConnectingL( TInt aCallId );
        
        /**
        * See CPhoneState
        */
        void HandleDialingL( TInt aCallId );

        /** 
        * Handle state-specific behaviour when number entry is cleared
        */ 
        virtual void HandleNumberEntryClearedL();
        
        /** 
        * Handle state-specific behaviour handleErrorL
        */ 
        void HandleErrorL( const TPEErrorInfo& aErrorInfo );
        
        TBool HandleCommandL( TInt aCommand );
        
        void DisconnectEmergencyCallL();
        
        void HandleKeyMessageL( 
            TPhoneKeyEventMessages aMessage,
            TKeyCode aCode );

        /**
        * By default EPOC constructor is private.
        */
        CPhoneEmergency( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );
            
        void HandleKeyEventL( 
            const TKeyEvent& aKeyEvent, 
            TEventCode aEventCode );
            
        /**
        * Sends key events to the phone engine
        * @param aKeyEvent a key event
        * @param aEventCode key event code
        */
        void SendKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aEventCode );
        
        /**
        * ConstructL()
        */
        virtual void ConstructL();
        
        /**
        * Handle EPEMessageAudioOutputChanged
        */
        void HandleAudioOutputChangedL();
        
        /**
        * Handle initializing cba's
        */
        void UpdateSetupCbaL();
       
        /**
        * Handles commands from the Remote Control framework.
        * @param aOperationId The operation ID of the command.
        * @param aButtonAct The button action associated with the command.
        * @return ETrue if the event was handled, EFalse otherwise.
        */
        IMPORT_C virtual TBool HandleRemConCommandL( 
                    TRemConCoreApiOperationId aOperationId, 
                    TRemConCoreApiButtonAction aButtonAct );
         
    private:
        /**
        * Is device lock on/off
        */
        TBool iDeviceLockOn;
        
        /**
        * If true update emergency call wait note
        */
        TBool iCallSetup;

        /**
        * Status of startup
        */
        TBool iStartupInterrupted;
        
        /**
        * If true show call summary ( termination ) note 
        */
        TBool iConnected;
    };

#endif // CPHONEEMERGENCY

// End of File
