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
*     Base class for handling key events.
*
*/


#ifndef __CPHONESYSTEMEVENTHANDLER_H
#define __CPHONESYSTEMEVENTHANDLER_H

//  INCLUDES
#include <e32base.h>
#include "mphonesystemeventhandler.h"
#include "mphonepubsubobserver.h"

// FORWARD DECLARATIONS
class MPhoneStateMachine;

/**
* Transforms system events to be sent to the states
*/
class CPhoneSystemEventHandler : 
    public CBase, 
    public MPhoneSystemEventHandler,
    public MPhonePubSubObserver
    {
    public:

        /**
        * Two-phased constructor.
        */
        static CPhoneSystemEventHandler* NewL( 
            MPhoneStateMachine* aStateMachine );

        /**
        * Destructor.
        */
        virtual ~CPhoneSystemEventHandler();

    public: // From MPhoneSystemEvents

        void HandlePhoneStartupL();
        
    public: // From MPhonePubSubObserver

        /**
        * This function is called when there is property value change.
        * @param aCategory Category of the property
        * @param aKey Property key that is changed
        * @param aValue New property value
        */
        virtual void HandlePropertyChangedL( 
            const TUid& aCategory,
            const TUint aKey,
            const TInt aValue );
    private:

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();

        /**
        * C++ default constructor.
        */
        CPhoneSystemEventHandler( MPhoneStateMachine* aStateMachine );

    private:  // Data

        /**
        * State machine
        */
        MPhoneStateMachine* iStateMachine;

    };

#endif      // __CPHONESYSTEMEVENTHANDLER_H   
            
// End of File
