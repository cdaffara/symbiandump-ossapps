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
*       Class that does processing for Phone Engine messages that are common
*       for all GSM states. Most of the GSM states are intressed of these 
*       messages but due to the class architecture they don't have any single
*       state class to be put into. This class exists to minimize duplicate
*       code so that not every GSM class need to implement these same handlings
*       for the same messages.
*
*/


#ifndef CPHONEGENERALGSMMESSAGESHANDLER_H
#define CPHONEGENERALGSMMESSAGESHANDLER_H

// INCLUDES
#include <w32std.h>

// FORWARD DECLARATIONS
class MPhoneStateMachine;
class MPhoneViewCommandHandle;
class MPhoneState;

// CLASS DECLARATION

/**
* Class that does processing for Phone Engine messages that are common
* for all GSM states. Most of the GSM states are intressed of these 
* messages but due to the class architecture they don't have any single
* state class to be put into. This class exists to minimize duplicate
* code so that not every GSM class need to implement these same handlings
* for the same messages.
*
*/
class CPhoneGeneralGsmMessagesHandler : public CBase
    {
    public:

        /**
        * Creates the General GSM Messages Handler instance
        * @param aPhoneState: active state
        * @param aViewCommandHandle: handle to the PhoneUIView
        * @return an instance of class CPhoneGeneralGsmMessagesHandler
        */
        static CPhoneGeneralGsmMessagesHandler* NewL( 
            MPhoneStateMachine& aStateMachine, 
            MPhoneViewCommandHandle& aViewCommandHandle,
            MPhoneState& aActiveState );

        /**
        * Destructor.
        */
        virtual ~CPhoneGeneralGsmMessagesHandler();

        /**
        * A message handling function for Phone Engine messages
        * @param aMessage Message from Phone Engine
        * @param aCallId Call id the message concerns
        */
        void HandlePhoneEngineMessageL(
            const TInt aMessage, 
            TInt aCallId );  
         

    private:
        
        /**
        * By default EPOC constructor is private.
        */
        CPhoneGeneralGsmMessagesHandler( 
            MPhoneStateMachine& aStateMachine, 
            MPhoneViewCommandHandle& aViewCommandHandle,
            MPhoneState& aActiveState );


    private:
    
        /**
        * Show global InfoNote
        * @param aResourceId resource id to be resolved
        * @param aNotificationDialog ETrue if notification dialog should be used
        */
        void SendGlobalInfoNoteL( TInt aResourceId, 
                TBool aNotificationDialog = EFalse );

        /**
        * Show global ErrorNote
        * @param aResourceId resource id to be resolved
        * @param aNotificationDialog ETrue if notification dialog should be used
        */
        void SendGlobalErrorNoteL( TInt aResourceId,
                TBool aNotificationDialog = EFalse );


    private:

        /**
        * Currently active state.
        */    
        MPhoneStateMachine& iStateMachine;
        
        /**
        * Handle for sending view commands.
        */
        MPhoneViewCommandHandle& iViewCommandHandle;
        
        /**
        * Currently active state object.
        */
        MPhoneState& iActiveState; 
           
    };

#endif // CPHONEGENERALGSMMESSAGESHANDLER_H
            
// End of File
