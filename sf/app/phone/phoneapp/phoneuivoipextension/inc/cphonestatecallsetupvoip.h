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
*     VoIP -specific Call setup state implementation.
*
*/


#ifndef CPHONESTATECALLSETUPVOIP_H
#define CPHONESTATECALLSETUPVOIP_H

// INCLUDES
#include "cphonecallsetup.h"

// CLASS DECLARATION
/**
*  VoIP -specific Call setup state
*/
NONSHARABLE_CLASS( CPhoneStateCallSetupVoIP ): public CPhoneCallSetup
    {
    public:  

        /**
        * Destructor.
        */
        virtual ~CPhoneStateCallSetupVoIP();

        /**
        * Creates the VoIP -specific Call Setup state class
        * @param aStateMachine: a state machine
        * @param aViewCommandHandle: handle to the PhoneUIView
        * @return an instance of class CPhoneCallSetup
        */
        static CPhoneStateCallSetupVoIP* NewL( 
            MPhoneStateMachine& aStateMachine, 
            MPhoneViewCommandHandle& aViewCommandHandle,
            MPhoneCustomization& aPhoneCustomization );

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
        CPhoneStateCallSetupVoIP( 
            MPhoneStateMachine& aStateMachine, 
            MPhoneViewCommandHandle& aViewCommandHandle,
            MPhoneCustomization& aPhoneCustomization );
        
        /**
        * ConstructL()
        */
        virtual void ConstructL();
        
    private:

        /**
        * Handle EPEMessageConnecting
        */
        void HandleConnectingL( TInt aCallId );
        
    };

#endif // CPHONESTATECALLSETUPVOIP_H

// End of File
