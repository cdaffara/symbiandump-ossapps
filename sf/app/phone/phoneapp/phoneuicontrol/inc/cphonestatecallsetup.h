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
*     Call setup state implementation.
*
*/


#ifndef CPHONESTATECALLSETUP
#define CPHONESTATECALLSETUP

// INCLUDES
#include "cphonestate.h"

// FORWARD DECLARATIONS
class CPhoneDtmfWaitCharTimer;

// CLASS DECLARATION
/**
*  Call setup state
*/
class CPhoneStateCallSetup : public CPhoneState
    {
    public:  

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CPhoneStateCallSetup();

        /**
        * Creates the Call Setup state class
        * @param aStateMachine: a state machine
        * @param aViewCommandHandle: handle to the PhoneUIView
        * @return an instance of class CPhoneStateCallSetup
        */
        static CPhoneStateCallSetup* NewL( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aCustomization );

        IMPORT_C virtual void HandleKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aEventCode );

        IMPORT_C virtual void HandleKeyMessageL(
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
        
        /**
        * Handles commands.
        * @param aCommand It is the code of the command to be handled.
        * @returns boolean value was the command handled by the state 
        *          (ETrue) or not (EFalse)
        */
        IMPORT_C virtual TBool HandleCommandL( TInt aCommand );
        
    protected:

        /** 
        * Handle state-specific behaviour when number entry is cleared
        */ 
        IMPORT_C virtual void HandleNumberEntryClearedL();
        
        /**
        * By default EPOC constructor is private.
        */
        IMPORT_C CPhoneStateCallSetup( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );
        
        /**
        * ConstructL()
        */
        IMPORT_C virtual void ConstructL();
        
    private:

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
        * Handle EPEMessageAudioOutputChanged
        */
        void HandleAudioOutputChangedL();
     
        /**
        * Handle EPEMessageSendingDTMF
        */
        void HandleSendingDTMFL();
        
        /**
        * Handle EPEMessageStoppedDTMF
        */
        void HandleStoppedDTMFL();

        /**
        * Handles EPEMessageDTMFSent
        *         EPEMessageDTMFSendingAborted
        */
        void CancelDTMFSendingL();
        
        /**
        * Sends DTMF sequence to Phone Engine
        */
        void SendDtmfL();
        
        /**
        * Sends Key specific DTMF to Phone Engine
        */
        void PlayKeySpecificDTMF( const TUint aCode );
             
    private:
        
        /**
        * Special timer for preventing recursive calls
        * which would otherwise occur with multiple
        * wait characters in a DTMF string.
        */
        CPhoneDtmfWaitCharTimer* iDtmfWaitCharTimer;
    };

#endif // CPHONESTATECALLSETUP

// End of File
