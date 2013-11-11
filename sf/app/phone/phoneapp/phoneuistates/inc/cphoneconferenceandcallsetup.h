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
*     Call setup at conference state implementation.
*
*/


#ifndef CPHONECONFERENCEANDCALLSETUP_H
#define CPHONECONFERENCEANDCALLSETUP_H

// INCLUDES
#include "cphoneconference.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*  Call setup at conference state implementation.
*/
class CPhoneConferenceAndCallSetup : public CPhoneConference
    {
    public:  

        /**
        * Destructor.
        */
        virtual ~CPhoneConferenceAndCallSetup();

    public:
        
        /**
        * Creates the Conference And Call Setup state class
        * @param aStateMachine: a container of state objects
        * @param aViewCommandHandle: handle to the PhoneUIView
        * @return an instance of class CPhoneStateIncoming
        */
        static CPhoneConferenceAndCallSetup* NewL( 
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
        CPhoneConferenceAndCallSetup( 
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );
        
        /**
        * ConstructL()
        */
        virtual void ConstructL();
        
        
     private:
     
        /**
        * Handle EPEMessageConnecting
        */
        void HandleConnectingL( TInt aCallId );
        
        /**
        * Handle EPEMessageConnected
        */
        void HandleConnectedL( TInt aCallId );
        
        /**
        * A message handling function for EPEMessageIdle
        * @param aCallId: the call id of the call
        */
        void HandleIdleL( TInt aCallId );
        
        /**
        * Handle EPEMessageConferenceIdle
        */
        void HandleConferenceIdleL();
       
     private:
         
         /**
         * Status of ConferenceAndCallSetup alerting
         */
         TBool iAlerting;
        
    };

#endif // CPHONECONFERENCEANDCALLSETUP_H

// End of File
