/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
*     VoIP-specific incoming state implementation.
*
*/


#ifndef CPHONESTATEINCOMINGVOIP_H
#define CPHONESTATEINCOMINGVOIP_H

// INCLUDES
#include "cphoneincoming.h"

// CLASS DECLARATION
/**
*  VoIP-specific incoming state
*/
NONSHARABLE_CLASS( CPhoneStateIncomingVoIP ): public CPhoneIncoming
    {
    public:  

        /**
        * Destructor.
        */
        virtual ~CPhoneStateIncomingVoIP();
        
        /**
        * Creates the VoIP specific Incoming state class
        * @param aStateMachine: a state machine
        * @param aViewCommandHandle: handle to the PhoneUIView
        * @return an instance of class CPhoneEmergency
        */
        static CPhoneStateIncomingVoIP* NewL( 
            MPhoneStateMachine& aStateMachine, 
            MPhoneViewCommandHandle& aViewCommandHandle,
            MPhoneCustomization& aPhoneCustomization );    
                        
    protected:

        /**
        * By default EPOC constructor is private.
        */
        CPhoneStateIncomingVoIP( 
            MPhoneStateMachine& aStateMachine, 
            MPhoneViewCommandHandle& aViewCommandHandle,
            MPhoneCustomization& aPhoneCustomization );
                    
        /**
        * ConstructL()
        */
        virtual void ConstructL();
        
        /**
        * Handles audio play stoping
        */        
        virtual void HandleAudioPlayStoppedL();
    };

#endif // CPHONESTATEINCOMINGVOIP_H

// End of File
