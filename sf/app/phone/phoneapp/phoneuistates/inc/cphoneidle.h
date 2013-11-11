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
*     GSM-specific idle state implementation.
*
*/


#ifndef CPHONEIDLE_H
#define CPHONEIDLE_H

// INCLUDES
#include "cphonestateidle.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*  GSM-specific idle state implementation.
*/
class CPhoneIdle : public CPhoneStateIdle
    {
    public:  

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CPhoneIdle();
        
        /**
        * Creates the Idle state class
        * @param aStateMachine: a state machine
        * @param aViewCommandHandle: handle to the PhoneUIView
        * @return an instance of class CPhoneEmergency
        */
        static CPhoneIdle* NewL( 
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
        IMPORT_C CPhoneIdle( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );
                    
        /**
        * ConstructL()
        */
        IMPORT_C virtual void ConstructL();
        
        /**
        * HandleError
        * Implements error handling framework
        * @param aErrorInfo: the error info
        */
        IMPORT_C virtual void HandleErrorL( 
            const TPEErrorInfo& aErrorInfo );
        
    };

#endif // CPHONEIDLE_H

// End of File
