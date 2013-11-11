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
*     Two single call state implementation.
*
*/


#ifndef CPHONETWOSINGLES_H
#define CPHONETWOSINGLES_H

// INCLUDES
#include "cphonegsmincall.h"
#include "tphonecmdparamboolean.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*  Two single call state implementation.
*/
class CPhoneTwoSingles : public CPhoneGsmInCall
    {
    public:  

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CPhoneTwoSingles();

        /**
        * Creates the Two Singles state class
        * @param aStateMachine: a container of state objects
        * @param aViewCommandHandle: handle to the PhoneUIView
        * @return an instance of class CPhoneStateIncoming
        */
        static CPhoneTwoSingles* NewL( 
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
            
        IMPORT_C void HandleKeyMessageL(
            TPhoneKeyEventMessages aMessage,
            TKeyCode aCode );
            
    protected:

        /**
        * By default EPOC constructor is private.
        */
        IMPORT_C CPhoneTwoSingles( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );
        
        /**
        * ConstructL()
        */
        IMPORT_C virtual void ConstructL();
      
        /** 
        * Handle state-specific behaviour when number entry is cleared
        */ 
        IMPORT_C virtual void HandleNumberEntryClearedL();
        
        IMPORT_C virtual void HandleConnectedConferenceL( TInt aCallId );
        
        
    private:     
         
        void HandleIdleL( TInt aCallId );
                
        void HandleIncomingL( TInt aCallId );
    };

#endif // CPHONETWOSINGLES_H

// End of File
