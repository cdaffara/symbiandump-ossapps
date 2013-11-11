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
* Description:  Clearing state
*
*/



#ifndef CVTENGSTATECLEARING_H
#define CVTENGSTATECLEARING_H

//  INCLUDES

#include "CVtEngStateBase.h"

/**
*  Clearing state class
*
*  @lib videoteleng
*  @since 2.6
*/
NONSHARABLE_CLASS( CVtEngStateClearing ) : public CVtEngStateBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ constructor.
        */
        CVtEngStateClearing( 
            CVtEngStateManager& aStateManager,
            CVtEngEventManager& aEventManager );


        /**
        * Destructor.
        */
        virtual ~CVtEngStateClearing();

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
        * Handles state transition to clearing.
        */
        void DoOpenL();

    };

#endif      // CVTENGSTATECLEARING_H   
            
// End of File
