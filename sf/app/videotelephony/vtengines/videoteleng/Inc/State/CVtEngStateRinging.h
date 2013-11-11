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
* Description:  Ringing state
*
*/



#ifndef CVTENGSTATERINGING_H
#define CVTENGSTATERINGING_H

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
*  Ringing state class
*
*  @lib videoteleng
*  @since 2.6
*/
NONSHARABLE_CLASS( CVtEngStateRinging ) : public CVtEngStateBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ constructor.
        */
        CVtEngStateRinging( 
            CVtEngStateManager& aStateManager,
            CVtEngEventManager& aEventManager );


        /**
        * Destructor.
        */
        virtual ~CVtEngStateRinging();

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

    private:

        /**
        * Handles state transition to ringing.
        */
        void DoOpenL();

    };

#endif      // CVTENGSTATERINGING_H
            
// End of File
