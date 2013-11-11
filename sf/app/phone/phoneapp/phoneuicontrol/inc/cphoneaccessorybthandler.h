/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Accessory and Bluetooth handler.
*
*/


#ifndef CPHONEACCESSORYBTHANDLER_H_
#define CPHONEACCESSORYBTHANDLER_H_

// INCLUDES
#include <e32base.h>
#include <pevirtualengine.h> 
#include <mphoneneclearedhandler.h>
#include <mphonestatemachine.h>
#include <mphoneviewcommandhandle.h>

// CLASS DECLARATION
 
/**
*  It waits for bluetooth audio disconnection and
*  shows query when it happens.
*
*/
NONSHARABLE_CLASS( CPhoneAccessoryBTHandler ) : public CBase
    {
    public:
        
        /**
        * Two-phased constructor
        * @param aViewCommandHandle reference to View Command Handler
        * @param aStateMachine: reference to Phone UI state machine
        * @return an instance of class CPhoneAccessoryBTHandler
        */
        static CPhoneAccessoryBTHandler* NewLC( MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneStateMachine* aStateMachine );
        
        /**
        * Destructor.
        */
        ~CPhoneAccessoryBTHandler();


        /**
        * Set the BT handsfree mode
        * @param aHandsfreeMode ETrue to activate BT, EFalse to deactivate
        * @return ETrue if allowed
        */
        TBool SetBTHandsfreeModeL( TBool aHandsfreeMode );

        /**
        * Set the handsfree mode
        * @param aHandsfreeMode ETrue to activate IHF, EFalse to deactivate
        * @return ETrue if allowed
        */
        TBool SetHandsfreeModeL( TBool aHandsfreeMode );

        /**
        * Shows BT address
        */
        void ShowBTAddressL();
          
        /**
        * Shows BT loopback status note
        */
        void ShowBTLoopbackL();
        
        /**
        * Shows BT activated note
        */
        void ShowBTActivatedL();
          
    private:

        /**
        * C++ constructor.
        */
        CPhoneAccessoryBTHandler( MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneStateMachine* aStateMachine );

        /**
        * Set the handsfree mode
        * @param aHandsfreeMode ETrue to activate, EFalse to deactivate
        * @param aAudioOutput audio output
        * @return ETrue if allowed
        */
        TBool DoSetHandsfreeModeL( TBool aHandsfreeMode, TPEAudioOutput aAudioOutput );

    private:    // Data  
       
        /**
        * Reference to the Phone UI View command handle
        */
        MPhoneViewCommandHandle* iViewCommandHandle;

        /**
        * Reference to State machine
        */
        MPhoneStateMachine* iStateMachine;

    };


#endif /*CPHONEACCESSORYBTHANDLER_H_*/
