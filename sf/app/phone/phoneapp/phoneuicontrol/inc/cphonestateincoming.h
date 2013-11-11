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
*     Incoming state implementation.
*
*/


#ifndef CPHONESTATEINCOMING
#define CPHONESTATEINCOMING

// INCLUDES
#include "cphonestate.h"

// FORWARD DECLARATIONS
class TPhoneCmdParamCallStateData;

// CLASS DECLARATION
/**
*  Incoming state
*/
class CPhoneStateIncoming : public CPhoneState
    {
    public:  

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CPhoneStateIncoming();

        /**
        * Creates the Incoming state class
        * @param aStateMachine: a container of state objects
        * @param aViewCommandHandle: handle to the PhoneUIView
        * @return an instance of class CPhoneStateIncoming
        */
        static CPhoneStateIncoming* NewL( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );

        IMPORT_C void HandleKeyMessageL(
            TPhoneKeyEventMessages aMessage,
            TKeyCode aCode );

        IMPORT_C void HandleKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aEventCode );

        /**
        * A message handling function for Phone Engine messages
        * @param aMessage Message from Phone Engine
        * @param aCallId Call id the message concerns
        */
        IMPORT_C virtual void HandlePhoneEngineMessageL(
            const TInt aMessage, 
            TInt aCallId );       
            
        IMPORT_C TBool HandleCommandL( TInt aCommand );

        /** 
        * Handle state-specific behaviour when number entry is cleared
        */ 
        IMPORT_C void HandleNumberEntryClearedL();
        
    protected:

        /**
        * By default EPOC constructor is private.
        */
        IMPORT_C CPhoneStateIncoming( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );
        
        /**
        * ConstructL()
        */
        IMPORT_C virtual void ConstructL();
        

        /**
        * Audio play stopped handling
        */
        IMPORT_C virtual void HandleAudioPlayStoppedL();            
        
    private: // From CPhoneState

        /**
        * A message handling function for EPEMessageConnected
        * @param aCallId: the call id of the call
        */
        void HandleConnectedL( TInt aCallId );

        /**
        * A message handling function for EPEMessageIdle
        * @param aCallId: the call id of the call
        */
        void HandleIdleL( TInt aCallId );
        
        /**
        * Handles Send key or command, when pressed from the dialer 
        */
        void HandleSendL();
        
        /**
        * Open soft reject message editor
        */
        void OpenSoftRejectMessageL();

        /**
        * Answer the MT call
        */
        void AnswerCallL();
        
        /**
        * Disconnects the waiting call
        */
        void DisconnectWaitingCallL();
        
        /**
        * Gets resource to use when number entry is open
        * @return resource id
        */
        TInt GetNumberEntryVisibleMenuBar();
        
        /**
        * Gets resource to use when number entry is closed
        * @return resource id
        */
        TInt GetNumberEntryNotVisibleMenuBar();
        
        /**
        * Show disconnected note
        */        
        void ShowDisconnectingL( TInt aCallId ); 
       
    protected:
        // call id of ringing call
        TInt iRingingCallId;
        
    };

#endif // CPHONESTATEINCOMING

// End of File
