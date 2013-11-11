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
*       The class implements a VoIP-specific state machine
*
*/


#ifndef CPHONESTATEMACHINEVOIP_H
#define CPHONESTATEMACHINEVOIP_H

//  INCLUDES
#include "cphonestatemachinegsm.h"

// FORWARD DECLARATIONS
class CPhoneCustomizationVoip;
class CPhoneVoIPErrorMessagesHandler;
class CPhoneStateUtilsVoip;

// CLASS DECLARATION

/**
 *  The class implements a VoIP-specific state machine.
 */
NONSHARABLE_CLASS( CPhoneStateMachineVoIP ): public CPhoneStateMachineGSM
    {
    public:

        /**
         * Destructor.
         */
        virtual ~CPhoneStateMachineVoIP();

        /**
         * Creates the only instance of state machine class.
         * @param   aViewCommandHandle  A handle to the Phone UI view.
         * @return  An instance of class CPhoneStateMachine.
         */
        static CPhoneStateMachineVoIP* NewL(
            MPhoneViewCommandHandle* aViewCommandHandle );

        /**
         * Returns active state.
         */
        MPhoneState* State();

        /**
         * Sets voip error message handler.
         * @param   aErrorHandler   Voip error message handler.
         */
        void SetVoipErrorMessageHandler(
            CPhoneVoIPErrorMessagesHandler& aErrorHandler );

        /**
         * Returns error message handler instance.
         * @return  Voip error message handler.
         */
        CPhoneVoIPErrorMessagesHandler& VoipErrorMessageHandler() const;
        
        /**
         * Returns utility class for voip states.
         * @return  Voip state utils.
         */
        CPhoneStateUtilsVoip& StateUtils();

    private:

        /**
         * By default EPOC constructor is private.
         */
        CPhoneStateMachineVoIP(
            MPhoneViewCommandHandle* aViewCommandHandle );

        /**
         * ConstructL()
         */
        void ConstructL();

    private: // Data

        /**
         * Voip customization.
         * Own.
         */
        CPhoneCustomizationVoip* iVoipCustomization;

        /**
         * Voip error message handler.
         * Now own.
         */
        CPhoneVoIPErrorMessagesHandler* iVoipErrorMessageHandler;
        
        /**
         * Voip state utils.
         * Own.
         */
        CPhoneStateUtilsVoip* iStateUtils;

    };

#endif      // CPHONESTATEMACHINEVOIP_H

// End of File
