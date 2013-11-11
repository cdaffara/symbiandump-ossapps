/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Conference and single and waiting call state implementation.
*
*/


#ifndef CPHONECONFERENCEANDSINGLEANDWAITING_H
#define CPHONECONFERENCEANDSINGLEANDWAITING_H

// INCLUDES
#include "cphoneconferenceandsingle.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*  Conference and single and waiting call state implementation.
*/
class CPhoneConferenceAndSingleAndWaiting : public CPhoneConferenceAndSingle
    {
    public:  

        /**
        * Destructor.
        */
        virtual ~CPhoneConferenceAndSingleAndWaiting();

        /**
        * Creates the Conference And Single And Waiting state class
        * @param aStateMachine: a container of state objects
        * @param aViewCommandHandle: handle to the PhoneUIView
        * @return an instance of class CPhoneStateIncoming
        */
        static CPhoneConferenceAndSingleAndWaiting* NewL( 
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

        virtual void HandleKeyMessageL(
            TPhoneKeyEventMessages aMessage,
            TKeyCode aCode );

    protected:

        /**
        * By default EPOC constructor is private.
        */
        CPhoneConferenceAndSingleAndWaiting( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );
        
        /**
        * ConstructL()
        */
        virtual void ConstructL();
        
        virtual void HandleIdleL( TInt aCallId );
        
        virtual void HandleConferenceIdleL();
        
        virtual void HandleAddedConferenceMemberL( TInt aCallId );
        
        
     private:
     
        void MakeStateTransitionToConferenceAndWaitingL( TInt aCallId );

        void MakeStateTransitionToConferenceAndSingleL( TInt aCallId );
        
        void MakeTransitionAccordingToActiveCallsL();
    };

#endif // CPHONECONFERENCEANDSINGLEANDWAITING_H

// End of File
