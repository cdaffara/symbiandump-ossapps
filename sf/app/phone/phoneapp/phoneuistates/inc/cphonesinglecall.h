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
*     GSM-specific single call state implementation.
*
*/


#ifndef CPHONESINGLECALL_H
#define CPHONESINGLECALL_H

// INCLUDES
#include "cphonegsmincall.h"
#include "tphonecmdparamboolean.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*  GSM-specific single call state
*/
class CPhoneSingleCall : public CPhoneGsmInCall
    {
    public:  

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CPhoneSingleCall();
        
        /**
        * Creates the GSM-specific Single call state class
        * @param aStateMachine: a state machine
        * @param aViewCommandHandle: handle to the PhoneUIView
        * @return an instance of class CPhoneSingleCall
        */
        static CPhoneSingleCall* NewL( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );

        IMPORT_C virtual void HandleKeyMessageL(
            TPhoneKeyEventMessages aMessage,
            TKeyCode aCode );

        /**
        * A message handling function for Phone Engine messages
        * @param aMessage Message from Phone Engine
        * @param aCallId Call id the message concerns
        */
        IMPORT_C virtual void HandlePhoneEngineMessageL(
            const TInt aMessage, 
            TInt aCallId );       
            
        IMPORT_C virtual TBool HandleCommandL( TInt aCommand );

    protected:

        /**
        * By default EPOC constructor is private.
        */
        IMPORT_C CPhoneSingleCall( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );
        
        /**
        * ConstructL()
        */
        IMPORT_C virtual void ConstructL();

        /**
        * Returns call identifier of the call this state is associated with.
        * @return   Call identifier.
        */
        IMPORT_C TInt CallId() const;
    private:
        
        void ToggleHoldL();
        
        void HandleIncomingL( TInt aCallId );
        
        void HandleDialingL( TInt aCallId );
        
    private:
    
        // Call id information.
        TInt iCallId;
        
    };

#endif // CPHONESINGLECALL_H

// End of File
