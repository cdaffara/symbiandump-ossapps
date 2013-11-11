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
*     VoIP -specific Alerting state implementation.
*
*/


#ifndef CPHONESTATEALERTINGVOIP_H
#define CPHONESTATEALERTINGVOIP_H

// INCLUDES
#include "cphonealerting.h"

// CLASS DECLARATION
/**
*  VoIP -specific Alerting state
*/
NONSHARABLE_CLASS( CPhoneStateAlertingVoIP ): public CPhoneAlerting
    {
    public:  

        /**
        * Destructor.
        */
        virtual ~CPhoneStateAlertingVoIP();

        /**
        * Creates the VoIP -specific Alerting state class
        * @param aStateMachine: a state machine
        * @param aViewCommandHandle: handle to the PhoneUIView
        * @return an instance of class CPhoneCallSetup
        */
        static CPhoneStateAlertingVoIP* NewL( 
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
        CPhoneStateAlertingVoIP( 
            MPhoneStateMachine& aStateMachine, 
            MPhoneViewCommandHandle& aViewCommandHandle,
            MPhoneCustomization& aPhoneCustomization );
        
        /**
        * ConstructL()
        */
        virtual void ConstructL();
   
        
    private:

        /**
        * Handle EPEMessageConnected
        */
        void HandleConnectedL( TInt aCallId );
        
    };

#endif // CPHONESTATEALERTINGVOIP_H

// End of File
