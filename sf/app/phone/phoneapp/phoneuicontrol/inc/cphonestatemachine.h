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
*     Class for handling states and their transitions.
*
*/


#ifndef CPHONESTATEMACHINE_H
#define CPHONESTATEMACHINE_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <pevirtualengine.h>
#include "mphonestatemachine.h"
#include "mphonestate.h"
#include "mphoneviewcommandhandle.h"

// FORWARD DECLARATIONS
class MPhoneState;
class MPhoneStorage;

// CLASS DECLARATION

/**
*  Class for handling states and their transitions
*/
class CPhoneStateMachine : 
    public CBase, 
    public MPhoneStateMachine
    {
    public:  

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CPhoneStateMachine();

    public: // From MPhoneStateMachine

        /**
        * Sets the next state to be set.
        * New state will be constructed and old one destructed in next
        * State() call.
        * @param aState to be set
        */
        IMPORT_C void ChangeState( TInt aState );

        /**
        * Returns active state
        */
        IMPORT_C MPhoneState* State();

        /**
        * Sets pointer to Phone Engine
        * @param aPhoneEngine pointer to Phone Engine instance
        */
        IMPORT_C void SetPhoneEngine( MPEPhoneModel* aPhoneEngine );

        /**
        * Gets pointer to Phone Engine
        * @returns pointer to Phone Engine
        */
        IMPORT_C MPEPhoneModel* PhoneEngine();

        /**
        * Gets pointer to Phone Engine Info
        * @return pointer to Phone Engine Info
        */
        IMPORT_C MPEEngineInfo* PhoneEngineInfo();
        
        /**
        * Instantiates protocol specific Phone Engine
        * @param reference to Engine Monitor
        * @return Phone Engine instance
        */
        IMPORT_C MPEPhoneModel* CreatePhoneEngineL(
            MEngineMonitor& aEngineMonitor );
        /**
        * Send message to phoneEngine
        * @param aMessage
        */     
        IMPORT_C void SendPhoneEngineMessage( 
            TInt aMessage );

        /**
        * Set callId to phoneEngineinfo
        * @param aCallId
        */    
        IMPORT_C void SetCallId( const TInt aCallId );

       
        /**
        * Instantiates phone storage.
        * @return storage instance    
        */    
        IMPORT_C MPhoneStorage* PhoneStorage();
        
        /**
       * Getter for CEikonEnv to avoid use of static system calls
       * @return CEikonEnv handle
       */
       IMPORT_C CEikonEnv* EikonEnv() const;

    protected:

        /**
        * By default EPOC constructor is private.
        */
        IMPORT_C CPhoneStateMachine(
            MPhoneViewCommandHandle* aViewCommandHandle );

    protected: // Data   

        /**
        * Currently active state
        */
        MPhoneState* iState; 

        /**
        * Old state ID - needed in state transitions
        */
        TInt iOldStateId;

        /**
        * New state ID to be used - needed in state transitions
        */
        TInt iNewStateId;

        /**
        * View's command handle
        */
        MPhoneViewCommandHandle* iViewCommandHandle;

        /**
        * Pointer to Phone Engine - uses relation
        */
        MPEPhoneModel* iPhoneEngine;

        // Idle state which is hold in memory all the time
        MPhoneState* iIdleState;

        // Phone's storage
        MPhoneStorage* iPhoneStorage;
        
        /** Internal variable for EikonEnv to avoid use of static system calls
        * Not own.
        */
        CEikonEnv* iEnv;
    };

#endif      // CPHONESTATEMACHINE_H
            
// End of File
