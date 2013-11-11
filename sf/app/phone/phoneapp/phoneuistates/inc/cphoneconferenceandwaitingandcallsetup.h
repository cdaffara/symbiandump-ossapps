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
*     Conference and waiting and call setup state implementation.
*
*/


#ifndef CPHONECONFERENCEANDWAITINGANDCALLSETUP_H
#define CPHONECONFERENCEANDWAITINGANDCALLSETUP_H

// INCLUDES
#include "cphoneconference.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*  Conference and waiting and call setup state implementation.
*/
class CPhoneConferenceAndWaitingAndCallSetup : public CPhoneConference
    {
    public:  

        /**
        * Destructor.
        */
        virtual ~CPhoneConferenceAndWaitingAndCallSetup();

        /**
        * Creates the Conference And Waiting And Call Setup state class
        * @param aStateMachine: a container of state objects
        * @param aViewCommandHandle: handle to the PhoneUIView
        * @return an instance of class CPhoneStateIncoming
        */
        static CPhoneConferenceAndWaitingAndCallSetup* NewL( 
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
        CPhoneConferenceAndWaitingAndCallSetup( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );
        
        /**
        * ConstructL()
        */
        virtual void ConstructL();   
        
        virtual void HandleConferenceIdleL();
        
        virtual void HandleIdleL( TInt aCallId );
        
        
     private:

        void HandleConnectingL( TInt aCallId );
        
        void HandleConnectedL( TInt aCallId );
        
        void UpdateConnectingCbaL();
        
     private:
         
         /**
         * Status of ConferenceAndWaitingAndCallSetup alerting
         */
         TBool iAlerting;
        
    };

#endif // CPHONECONFERENCEANDWAITINGANDCALLSETUP_H

// End of File
