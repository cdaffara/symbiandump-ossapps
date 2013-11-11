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
* Description:  Initializing state
*
*/



#ifndef CVTENGSTATEINITIALIZING_H
#define CVTENGSTATEINITIALIZING_H

//  INCLUDES
#include "CVtEngStateBase.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CVtEngStateManager;

// CLASS DECLARATION

/**
*  Initializing state class
*
*  @lib videoteleng
*  @since 2.6
*/
NONSHARABLE_CLASS( CVtEngStateInitializing ): public CVtEngStateBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ constructor.
        */
        CVtEngStateInitializing( 
            CVtEngStateManager& aStateManager,
            CVtEngEventManager& aEventManager );


        /**
        * Destructor.
        */
        virtual ~CVtEngStateInitializing();

    public: // From CVtEngStateBase
        
        /**
        * Updates state. May result to state transition
        */
        virtual void UpdateL();

        /**
        * Validates a command.
        */
        TBool ValidateCommand(
            const TVtEngCommandId aCommandId );

        /**
        * @see CVtEngStateBase
        */
        virtual MVtEngSessionInfo::TSessionState State() const;
        
        /**
        * @see CVtEngStateBase::HandleL
        */
        TBool HandleL( CVtEngOperation& aOp );

    private:

        /**
        * Handles state transition to initializing.
        */
        void DoOpenL();
    };

#endif      // CVTENGSTATEINITIALIZING_H
            
// End of File
