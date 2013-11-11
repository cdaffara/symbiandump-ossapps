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
*     GSM-specific Call setup state implementation.
*
*/


#ifndef CPHONECALLSETUP
#define CPHONECALLSETUP

// INCLUDES
#include "cphonestatecallsetup.h"

// CLASS DECLARATION
/**
*  GSM-specific Call setup state
*/
class CPhoneCallSetup : public CPhoneStateCallSetup
    {
    public:  

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CPhoneCallSetup();

        /**
        * Creates the Call Setup state class
        * @param aStateMachine: a state machine
        * @param aViewCommandHandle: handle to the PhoneUIView
        * @return an instance of class CPhoneCallSetup
        */
        static CPhoneCallSetup* NewL( 
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
        IMPORT_C CPhoneCallSetup( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );
        
        /**
        * ConstructL()
        */
        IMPORT_C virtual void ConstructL();

        /**
        * Handle EPEMessageConnecting
        */
        IMPORT_C virtual void HandleConnectingL( TInt aCallId ); 
        
    };

#endif // CPHONECALLSETUP

// End of File
