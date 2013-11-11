/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
*     GSM-specific start up state implementation.
*
*/


#ifndef CPHONESTARTUP_H
#define CPHONESTARTUP_H

// INCLUDES
#include "cphonestatestartup.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*  GSM-specific start up state implementation.
*/
class CPhoneStartup : public CPhoneStateStartup
    {
    public:  

        /**
        * Destructor.
        */
        virtual ~CPhoneStartup();
        
        /**
        * Creates the Start up state class
        * @param aStateMachine: a state machine
        * @param aViewCommandHandle: handle to the PhoneUIView
        * @param aPEReady: true if Phone Engine already started
        * @return an instance of class CPhoneEmergency
        */
        static CPhoneStartup* NewL( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            TBool aPEReady );

        /**
        * A message handling function for Phone Engine messages
        * @param aMessage Message from Phone Engine
        * @param aCallId Call id the message concerns
        */
        void HandlePhoneEngineMessageL(
            const TInt aMessage, 
            TInt aCallId );    
                        
    protected:

        /**
        * By default EPOC constructor is private.
        */
        CPhoneStartup( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            TBool aPEReady );
                    
        /**
        * ConstructL()
        */
        virtual void ConstructL();
        
    };

#endif // CPHONESTARTUP_H

// End of File
