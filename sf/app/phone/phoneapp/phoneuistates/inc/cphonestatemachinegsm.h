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
*       The class implements a GSM-specific state machine
*
*/


#ifndef CPHONESTATEMACHINEGSM_H
#define CPHONESTATEMACHINEGSM_H

//  INCLUDES
#include "cphonestatemachine.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
class MPhoneCustomization;

/**
*  The class implements a GSM-specific state machine
*/

class CPhoneStateMachineGSM : public CPhoneStateMachine
    {
    public:  

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CPhoneStateMachineGSM();

    public: // New functions
        
        /**
        * Creates the only instance of state machine class
        * @param aViewCommandHandle: a handle to the Phone UI view
        * @return an instance of class CPhoneStateMachine
        */
        static CPhoneStateMachineGSM* NewL(
            MPhoneViewCommandHandle* aViewCommandHandle );

    public: // From MPhoneStateMachine

        /**
        * Returns active state
        */
        IMPORT_C MPhoneState* State();
        
        /**
        * Instantiates protocol specific Phone Engine
        * @param reference to Engine Monitor
        * @return Phone Engine instance
        */
        IMPORT_C MPEPhoneModel* CreatePhoneEngineL( MEngineMonitor& aEngineMonitor );

    protected:

        /**
        * By default EPOC constructor is private.
        */
        IMPORT_C CPhoneStateMachineGSM(
            MPhoneViewCommandHandle* aViewCommandHandle );
 
        // Phone customization
        MPhoneCustomization* iCustomization;
        
    private: // Data
    
        // Is emergency object instantiated - needed to prevent multiple instantiation
        TBool iEmergencyStateConstructed;
    
         // Emergency state which is hold in memory all the time.
        MPhoneState* iEmergencyState;         
    };

#endif      // CPHONESTATEMACHINEGSM_H
            
// End of File
