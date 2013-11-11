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
*     Startup state implementation.
*
*/


#ifndef CPHONESTATESTARTUP_H
#define CPHONESTATESTARTUP_H

// INCLUDES
#include "cphonestate.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*  Startup state
*/
class CPhoneStateStartup : public CPhoneState
    {
    public:  

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CPhoneStateStartup();

        /**
        * Creates the Startup state class
        * @param aStateMachine: a state machine
        * @param aViewCommandHandle: handle to the PhoneUIView
        * @return an instance of class CPhoneStateStartup
        */
        IMPORT_C static CPhoneStateStartup* NewL( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );

        /**
        * A message handling function for Phone Engine messages
        * @param aMessage Message from Phone Engine
        * @param aCallId Call id the message concerns
        */
        IMPORT_C void HandlePhoneEngineMessageL(
            const TInt aMessage, 
            TInt aCallId );       
            
        IMPORT_C void HandleKeyMessageL(
            TPhoneKeyEventMessages aMessage,
            TKeyCode aCode );
        
        IMPORT_C void HandleKeyEventL( 
                const TKeyEvent& aKeyEvent, 
                TEventCode aEventCode );

        /**
        * Handles startup of the phone application 
        */
        IMPORT_C virtual void HandlePhoneStartupL();

    protected:

        /**
        * ConstructL()
        */
        IMPORT_C virtual void ConstructL();
        
        /**
        * By default EPOC constructor is private.
        */
        IMPORT_C CPhoneStateStartup( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );        

        
    private:

        /**
         * Plays DTMF tone for key event 
         * */
        IMPORT_C void HandleDtmfKeyToneL( const TKeyEvent& aKeyEvent,
                TEventCode aEventCode );
        
        /**
        * A message handling function for message EPEMessagePEConstructionReady.
        * @param aCallId: the call id of the call
        */
        void HandlePEConstructionReadyL( TInt aCallId );
        
        /**
        * Starts to shows note.
        */
        static TInt DoShowNoteL( TAny* aAny );
        
        /**
        * Create and show appropriate note AFTER idle callback
        * if necessary.
        */
        void CreateAndShowNoteAfterIdle();


    protected: // Data

        // Provides phone engine startup status
        TBool iPEReady;

    private: // Data

        // Provides phone application startup status
        TBool iPhoneReady;
        
        // Ownded: idle object to create note.
        CIdle* iCreateNote;
    };

#endif // CPHONESTATESTARTUP_H

// End of File
