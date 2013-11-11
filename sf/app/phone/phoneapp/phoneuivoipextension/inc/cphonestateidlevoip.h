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
*     VoIP-specific idle state implementation.
*
*/


#ifndef CPHONESTATEIDLEVOIP_H
#define CPHONESTATEIDLEVOIP_H

// INCLUDES
#include <settingsinternalcrkeys.h>
#include "cphoneidle.h"

// FORWARD DECLARATIONS
class MPhoneStateMachine;
class MPhoneViewCommandHandle;
class MPhoneCustomization;
class CPhoneStateUtilsVoip;

// CLASS DECLARATION
/**
*  VoIP-specific idle state
*/
NONSHARABLE_CLASS( CPhoneStateIdleVoIP ): public CPhoneIdle
    {
    public:  

        /**
        * Destructor.
        */
        virtual ~CPhoneStateIdleVoIP();

        /**
        * Creates the VoIP-specific Idle state class
        * @param    aStateMachine       A container of state objects.
        * @param    aViewCommandHandle  Handle to the PhoneUIView.
        * @param    aCustomization      Handle to the phone customization.
        * @return   An instance of class CPhoneStateIdleVoIP.
        */
        static CPhoneStateIdleVoIP* NewL( 
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
            
    protected:

        /**
        * By default EPOC constructor is private.
        */
        CPhoneStateIdleVoIP( 
            MPhoneStateMachine& aStateMachine, 
            MPhoneViewCommandHandle& aViewCommandHandle, 
            MPhoneCustomization& aCustomization );
        
        /**
        * ConstructL()
        */
        virtual void ConstructL();

        /**
         * Handles send command. 
         */
        void HandleSendCommandL();
        
        /**
        * Dial the specified number. 
        */
        void DialL( const TDesC& aNumber, 
            TPhoneNumberType aNumberType,
            TDialInitiationMethod aDialMethod );

        /**
        * A message handling function for EPEMessageDialling
        * @param aCallId: the call id of the call
        */
        void HandleDialingL( TInt aCallId );
        
        TBool IsEmergencyNumber( const TDesC& sString );

    private:

        /**
         * Returns VoIP state utils.
         * @return VoIP state utils.
         */
        CPhoneStateUtilsVoip& StateUtils();
    };

#endif // CPHONESTATEIDLEVOIP_H

// End of File
