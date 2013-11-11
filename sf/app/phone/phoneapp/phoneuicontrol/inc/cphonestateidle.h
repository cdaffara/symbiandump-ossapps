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
*     Idle state implementation.
*
*/


#ifndef CPHONESTATEIDLE
#define CPHONESTATEIDLE

// INCLUDES
#include "cphonestate.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Idle state
*/
class CPhoneStateIdle : public CPhoneState
    {
    public:
        
        /** Dial initiation method. */
        enum TDialInitiationMethod
            {
            /** Dial initiation method unknown. */
            EDialMethodUnknown          = 0,
            /** Dial is initiated by explicit call type selection from menu.*/
            EDialMethodMenuSelection    = 1,
            /** Dial is initiated by send command. */
            EDialMethodSendCommand      = 2,
            /** Dial is initiated by one key dialing. */
            EDialMethodOneKeyDialing    = 3
            };
    
        /**
        * Phone number types
        *
        * EPhoneNumberTypeNotFound - phone number's type was not found
        * EPhoneNumberTypeCS - CS call can be made to the phone number
        * EPhoneNumberTypeVideo - video call can be made to the phone number
        * EPhoneNumberTypeVoip - VoIP call can be made to the address.
        */
        enum TPhoneNumberType
            {
            EPhoneNumberTypeNotFound = -1,
            EPhoneNumberTypeCS = 0,
            EPhoneNumberTypeVideo,
            EPhoneNumberTypeVoip
            };
    
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CPhoneStateIdle();

        /**
        * Creates the Idle state class
        * @param aStateMachine: a container of state objects
        * @param aViewCommandHandle: handle to the PhoneUIView
        * @return an instance of class CPhoneStateIdle
        */
        static CPhoneStateIdle* NewL( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );

        IMPORT_C virtual void HandleKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aEventCode );

        IMPORT_C void HandleKeyMessageL(
            TPhoneKeyEventMessages aMessage,
            TKeyCode aCode );

        /**
        * A message handling function for Phone Engine messages
        * @param aMessage Message from Phone Engine
        * @param aCallId Call id the message concerns
        */
        IMPORT_C virtual void HandlePhoneEngineMessageL(
            const TInt aMessage, 
            TInt aCallId );       
            
        IMPORT_C virtual TBool HandleCommandL( TInt aCommand );
        
        IMPORT_C virtual TBool ProcessCommandL( TInt aCommand );

        /**
        * Indicates when the Phone app has lost focus.
        */
        IMPORT_C virtual void HandlePhoneFocusLostEventL();

    protected:

        /** 
        * Handle state-specific behaviour when number entry is cleared
        */ 
        IMPORT_C virtual void HandleNumberEntryClearedL();

        IMPORT_C virtual TBool HandleRemConCommandL( 
            TRemConCoreApiOperationId aOperationId, 
            TRemConCoreApiButtonAction aButtonAct );

        /**
        * Display Incoming Call
        * @param aCallid call id
        */
        IMPORT_C void DisplayIncomingCallL( TInt aCallId );

        /**
        * By default EPOC constructor is private.
        */
        IMPORT_C CPhoneStateIdle( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );
        
        /**
        * ConstructL()
        */
        IMPORT_C virtual void ConstructL();

        /**
        * Dial the specified number. 
        * @param    aNumber            The number to call.
        * @param    aNumberType        A number type.
        * @param    aDialMethod        A dial initiation method.
        */
        IMPORT_C virtual void DialL( const TDesC& aNumber, 
            TPhoneNumberType aNumberType, TDialInitiationMethod aDialMethod );
        
        /**
        * Dial CS multimedia call
        * @param None
        */
        IMPORT_C void DialMultimediaCallL();

        /**
        * Dial CS voice call
        * @param None
        */
        IMPORT_C void DialVoiceCallL();
        
        /**
        * A message handling function for EPEMessageDialling
        * @param aCallId: the call id of the call
        */
        IMPORT_C virtual void HandleDialingL( TInt aCallId );

        /**
        * Handles send command.
        */
        IMPORT_C virtual void HandleSendCommandL();
        
    private: // New functions

        /**
        * A message handling function for EPEMessageIncoming
        * @param aCallId: the call id of the call
        */
        void HandleIncomingL( TInt aCallId );

        /**
        * A message handling function for EPEMessageHandleConnected
        * @param aCallId: the call id of the call
        */
        void HandleConnectedL( TInt aCallId );
        
        /**
        * A message handling function for EPEMessageIdle
        * @param aCallId: the call id of the call
        */
        void HandleIdleL( TInt aCallId );

        /**
        * Send Exit command
        */
        void SendExitCommandL();

        /**
        * Dials video call 
        */
        void DialVideoCallL();
        
        /**
        * Handles end key press that was catched in HandleKeyMessageL
        * @param aMessage - key event message
        */
        void HandleEndKeyPressL( TPhoneKeyEventMessages aMessage );
        
        /**
        * Checks if MC/PCSuite restore ongoing.
        * @param none.
        * @return ETrue if ongoing.
        */
        TBool RestoreOngoing();

        void HandleVoiceCallCommandL();
        
        /**
       * Changes state to aState and resets iBubbleInitialized
       * value to false.
       */
       void ChangeTo( TInt aState );
         
    private:
    
        // Owned profile engine
        MProfileEngine* iEngine;
        // indicates that incomming call is arrived.
        TBool iIncommingCall;
  
    };

#endif // CPHONESTATEIDLE

// End of File
