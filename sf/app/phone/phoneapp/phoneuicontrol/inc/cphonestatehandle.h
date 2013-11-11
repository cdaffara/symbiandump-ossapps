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
*     Singleton class for handling states and their transitions.
*
*/


#ifndef CPHONESTATEHANDLE_H
#define CPHONESTATEHANDLE_H

// INCLUDES
#include <pevirtualengine.h>
#include "mphonestatemachine.h"
#include "cphoneresourceresolverbase.h"
#include "mphoneviewcommandhandle.h"
#include "cphoneuistatemachinefactorybase.h"
#include <coemain.h>

// FORWARD DECLARATIONS
class MPhoneState;
class CPhoneErrorMessagesHandler;

// CLASS DECLARATION

/**
*  Singleton class for handling states and their transitions
*/
class CPhoneStateHandle : public CCoeStatic
    {
    public: 

        /**
        * First call initializes the singleton object. Subsequent calls panic. 
        * Singleton must be initialized only once and then Instance() method 
        * must be used to get handle to instance.
        * @return the created instance.
        */
        static CPhoneStateHandle* CreateL( 
            MPhoneViewCommandHandle* aViewCommandHandle,
            const TDesC& aFileName, 
            const TUid aFactoryUid );
    
        /**
        * Returns an instance of this class.
        * @return A pointer to CPhoneStateHandle class 
        */
        IMPORT_C static CPhoneStateHandle* Instance();

        /**
        * Destructor.
        */
        virtual ~CPhoneStateHandle();
        
        /**
        * Returns the state machine
        * @return State machine
        */
        IMPORT_C MPhoneStateMachine* StateMachine();
        
        /**
        * Returns the view command handle
        * @return view command handle
        */
        IMPORT_C MPhoneViewCommandHandle* ViewCommandHandle();

    private:

        /**
        * Loads handle to GSM or VoIP factory
        * @return pointer to Phone Engine Info
        */
        void LoadLibraryHandleL( 
            const TDesC& aFileName,
            const TUid aFactoryUid );

        /**
        * Default constructor is private because we are using the
        * Singleton design pattern.
        */
        CPhoneStateHandle(
            MPhoneViewCommandHandle* aViewCommandHandle );

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL(
            const TDesC& aFileName, 
            const TUid aFactoryUid );

    private:

        /**
        * View's command handle
        */
        MPhoneViewCommandHandle* iViewCommandHandle;

        /**
        * Pointer to GSM or VoIP specific state machine
        */
        MPhoneStateMachine* iPhoneStateMachine;

        /**
        * Pointer to GSM or VoIP specific resource resolver
        */
        CPhoneResourceResolverBase*   iPhoneResourceResolver;
        
        /**
        * Pointer to GSM or VoIP specific error messages handler
        */
        CPhoneErrorMessagesHandler* iPhoneErrorMessagesHandler;

        /**
        * Pointer to GSM or VoIP specific factory.
        */
        CPhoneUIStateMachineFactoryBase* iStateMachineFactory;

        TLibraryFunction iEntry;

        RLibrary iFactoryLibrary;

    };

#endif // CPHONESTATEHANDLE_H
            
// End of File
