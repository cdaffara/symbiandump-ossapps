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
* Description:  Open state
*
*/



#ifndef CVTENGSTATEOPEN_H
#define CVTENGSTATEOPEN_H

//  INCLUDES
#include "CVtEngStateBase.h"

// FORWARD DECLARATIONS
class CVtEngStateManager;

// CLASS DECLARATION

/**
*  Open state class
*
*  @lib videoteleng
*  @since 2.6
*/
NONSHARABLE_CLASS( CVtEngStateOpen ) : public CVtEngStateBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ constructor.
        */
        CVtEngStateOpen( 
            CVtEngStateManager& aStateManager,
            CVtEngEventManager& aEventManager );


        /**
        * Destructor.
        */
        virtual ~CVtEngStateOpen();

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
        * Changes audio routing
        * @param aOp audio routing or null if value should
        *        be read from settings
        */
        void RouteAudioL( 
            CVtEngOperation* aOp );
        
        /**
        * Uses extension framework to complete
        * from operation.
        * @since S60 v3.2
        * @param aOp audio routing or null 
        */    
        void CallExtensionL( CVtEngOperation* aOp );    

    private:

        /**
        * Handles state transition to open.
        */
        void DoOpenL();

    };

#endif      // CVTENGSTATEOPEN_H
            
// End of File
