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
* Description:  Negotiating state
*
*/



#ifndef CVTENGSTATENEGOTIATING_H
#define CVTENGSTATENEGOTIATING_H

//  INCLUDES
#include "CVtEngStateBase.h"

// FORWARD DECLARATIONS
class CVtEngStateManager;

// CLASS DECLARATION

/**
*  Negotiatimg state class
*
*  @lib videoteleng
*  @since 2.6
*/
NONSHARABLE_CLASS( CVtEngStateNegotiating ) : public CVtEngStateBase
    {
    public:  // Constructors and destructor

        /**
        * C++ constructor.
        */
        CVtEngStateNegotiating(
            CVtEngStateManager& aStateManager,
            CVtEngEventManager& aEventManager );


        /**
        * Destructor.
        */
        virtual ~CVtEngStateNegotiating();

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
        * @see CVtEngStateBase
        */
        virtual TBool HandleL( CVtEngOperation& aOp );

    private:

        /**
        * Handles state transition to negotiating.
        */
        void DoOpenL();

    };

#endif      // CVTENGSTATENEGOTIATING_H

// End of File
