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
*     Describes the interface for setting and getting active state.
*
*/


#ifndef MPHONESTATEMACHINE_H
#define MPHONESTATEMACHINE_H

// INCLUDES
#include "mphonestate.h"
#include "mphoneenginemessagesender.h"


// CLASS DECLARATION
class MPhoneState;
class MPEPhoneModel;
class MPEEngineInfo;
class CSpdiaControl;
class MPhoneStorage;

/**
*  Describes the interface for setting and getting active state
*/

class MPhoneStateMachine : public MPhoneEngineMessageSender
    {
    public:

        /**
        * Destructor.
        */
        virtual ~MPhoneStateMachine() {};
    
        /**
        * Sets active state
        */
        virtual void ChangeState( TInt aState ) = 0;

        /**
        * Returns active state
        */
        virtual MPhoneState* State() = 0;
        
        /**
        * Sets pointer to Phone Engine
        * @param aPhoneEngine pointer to Phone Engine instance
        */
        virtual void SetPhoneEngine( MPEPhoneModel* aPhoneEngine ) = 0;

        /**
        * Gets pointer to Phone Engine
        * @returns pointer to Phone Engine
        */
        virtual MPEPhoneModel* PhoneEngine() = 0;

        /**
        * Gets pointer to Phone Engine Info
        * @return pointer to Phone Engine Info
        */
        virtual MPEEngineInfo* PhoneEngineInfo() = 0;
        
        /**
        * Instantiates protocol specific Phone Engine
        * @param reference to Engine Monitor
        * @return Phone Engine instance
        */
        virtual MPEPhoneModel* CreatePhoneEngineL( 
            MEngineMonitor& aEngineMonitor ) = 0;
        
        /**
        * Sets callId to Phone Engine Info
        * @param aCallId
        */    
        virtual void SetCallId( TInt aCallId ) = 0;
        
        /**
        * Phone's Storage
        */
        virtual MPhoneStorage* PhoneStorage() = 0;        

    };

#endif // MPHONESTATEMACHINE_H

// End of File
