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
*     Class for handling messages from Engine and from all inputs to the
*     states.
*
*/


#ifndef __CPHONEENGINEHANDLER_H
#define __CPHONEENGINEHANDLER_H

// INCLUDES
#include <e32base.h>
#include <pevirtualengine.h>

#include "mphonestatemachine.h"

// FORWARD DECLARATIONS
class MPhoneStateMachine;

// CLASS DECLARATION

/**
* Class for handling messages from Engine and from all inputs to the states
*/
class CPhoneEngineHandler : public CBase
    {
    public: 

        /**
        * Two-phased constructor
        * @param aStateMachine: reference to Phone UI state machine
        * @return an instance of class CPhoneEngineHandler
        */
        static CPhoneEngineHandler* NewL(
            MPhoneStateMachine* aStateMachine );

        /**
        * Destructor.
        */
        virtual ~CPhoneEngineHandler(){};
        
        /**
        * Handles message from Phone Engine.
        * @param aMessage Message from Phone Engine
        * @param aCallId Call id the message concerns
        */
        void DoHandleMessageL( 
            const TInt aMessage, 
            TInt aCallId );       

    private:

        /**
        * By default EPOC constructor is private.
        */
        CPhoneEngineHandler( MPhoneStateMachine* aStateMachine );

    private: // Data   

        /**
        * Handle to State machine
        */
        MPhoneStateMachine* iStateMachine;

    };

#endif      // __CPHONEENGINEHANDLER_H
            
// End of File
