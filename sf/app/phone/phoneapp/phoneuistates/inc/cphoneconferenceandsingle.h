/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Conference and single call state implementation.
*
*/


#ifndef CPHONECONFERENCEANDSINGLE_H
#define CPHONECONFERENCEANDSINGLE_H

// INCLUDES
#include "cphoneconference.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*  Conference and single call state implementation.
*/
class CPhoneConferenceAndSingle : public CPhoneConference
    {
    public:  

        /**
        * Destructor.
        */
        virtual ~CPhoneConferenceAndSingle();

        /**
        * Creates the Conference And Single state class
        * @param aStateMachine: a container of state objects
        * @param aViewCommandHandle: handle to the PhoneUIView
        * @return an instance of class CPhoneStateIncoming
        */
        static CPhoneConferenceAndSingle* NewL( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );

        /**
        * A message handling function for Phone Engine messages
        * @param aMessage Message from Phone Engine
        * @param aCallId Call id the message concerns
        */
        virtual void HandlePhoneEngineMessageL(
            const TInt aMessage, 
            TInt aCallId ); 
            
        virtual TBool HandleCommandL( TInt aCommand );        

        virtual void HandleKeyMessageL(
            TPhoneKeyEventMessages aMessage,
            TKeyCode aCode );


    protected:

        /**
        * By default EPOC constructor is private.
        */
        CPhoneConferenceAndSingle( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );
        
        /**
        * ConstructL()
        */
        virtual void ConstructL();
        
        virtual void HandleIncomingL( TInt aCallId );
        
        virtual void HandleIdleL( TInt aCallId );
        
        virtual void HandleConferenceIdleL();
        
     private:
         
         void JoinToConferenceL();
         
         void HandleAddedConferenceMemberL( TInt aCallId );
         
         void HandleSendL();
         
    };

#endif // CPHONECONFERENCEANDSINGLE_H

// End of File
