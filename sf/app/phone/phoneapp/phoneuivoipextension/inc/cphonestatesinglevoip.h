/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
*     VoIP-specific single state implementation.
*
*/


#ifndef CPHONESTATESINGLEVOIP_H
#define CPHONESTATESINGLEVOIP_H

// INCLUDES
#include "cphonesinglecall.h"

// CLASS DECLARATION
class CPhoneStateUtilsVoip;

/**
*  VoIP-specific single state
*/
NONSHARABLE_CLASS( CPhoneStateSingleVoIP ): public CPhoneSingleCall
    {
    public:  

        /**
        * Destructor.
        */
        virtual ~CPhoneStateSingleVoIP();
        
        /**
        * Creates the VoIP specific Single state class.
        * @param    aStateMachine       A state machine.
        * @param    aViewCommandHandle  Handle to the PhoneUIView.
        * @param    aCustomization      Handle to the phone customization.
        * @return   An instance of class CPhoneStateSingleVoIP.
        */
        static CPhoneStateSingleVoIP* NewL( 
            MPhoneStateMachine& aStateMachine, 
            MPhoneViewCommandHandle& aViewCommandHandle, 
            MPhoneCustomization& aCustomization );

        /**
        * A message handling function for Phone Engine messages
        * @param aMessage Message from Phone Engine
        * @param aCallId Call id the message concerns
        */
        void HandlePhoneEngineMessageL(
            const TInt aMessage, 
            TInt aCallId );
            
        /**
        * Handles commands.
        * @param aCommand It is the code of the command to be handled.
        * @returns boolean value was the command handled by the state 
        *          (ETrue) or not (EFalse)
        */
        TBool HandleCommandL( TInt aCommand );

        /**
        * Handle processed (short, long) key messages from the key event
        * handler
        * @param aMessage a key message
        * @param aCode key event code
        */
        void HandleKeyMessageL( TPhoneKeyEventMessages aMessage,
                TKeyCode aKeyCode );
                        
    protected:

        /**
        * By default EPOC constructor is private.
        */
        CPhoneStateSingleVoIP( 
            MPhoneStateMachine& aStateMachine, 
            MPhoneViewCommandHandle& aViewCommandHandle, 
            MPhoneCustomization& aCustomization );
                    
        /**
        * ConstructL()
        */
        virtual void ConstructL();

    private:
        
        /**
         * Start calling
         */
        void StartCallingL();
        
        /**
         * Launches unattended call transfer acceptance query dialog.
         */
        void LaunchUnattendedTransferAcceptanceQueryL();
        
        /**
         * Resolves dialog text to be shown in unattended call transfer query
         * dialog.
         * @param    aQueryText  On return contains resolved query text.
         */
        void ResolveUnattendedTransferAcceptanceQueryDialogTextL( 
            RBuf& aQueryText ) const;
        
        /**
         * Handles user response to unattended transfer request.
         * @param    aRequestAccepted    ETrue if request is accepted, 
         * EFalse otherwise.
         */
        void HandleUnattendedTransferRequestResponseL( TBool aRequestAccepted );
   
        /**
         * Returns VoIP state utils.
         * @return VoIP state utils.
         */
        CPhoneStateUtilsVoip& StateUtils();
    };

#endif // CPHONESTATESINGLEVOIP_H

// End of File
