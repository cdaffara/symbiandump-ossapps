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
* Description:  State manager for session and audio routing states.
*
*/



#ifndef CVTENGSTATEMANAGER_H
#define CVTENGSTATEMANAGER_H

//  INCLUDES
#include <e32base.h>
#include "vtengcommands.h"
#include "MVtEngSessionInfo.h"
// FORWARD DECLARATIONS
class CVtEngStateBase;
class CVtEngHandlerContainer;
class CVtEngEventManager;
class CVtEngOperation;

// CLASS DECLARATION

/**
*  Owner of engine states. Delegates update request
*  to active state when requested. Provides method
*  to enquire command availability in current state.
*
*  @lib videoteleng
*  @since 2.6
*/
NONSHARABLE_CLASS( CVtEngStateManager ): public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CVtEngStateManager* NewL(
            CVtEngHandlerContainer& aHandlers,
            CVtEngEventManager& aEventManager );
        
        /**
        * Destructor.
        */
        virtual ~CVtEngStateManager();

    public: // New functions
        
        /**
        * Requests updates for current state.        
        */
        void Update( );

        /**
        * Checks if a command is possible in current state
        * @param aCommandId command to check
        * @return ETrue if command can be performed
        */
        TBool IsCommandPossible( 
            const TVtEngCommandId aCommandId ) const;

        /**
        * Sets new session state.
        * @param aNewState new session state
        */
        void SetState( CVtEngStateBase& aNewState );

        /**
        * Passes an operation to current state for
        * handling.
        * @param aOp operation to be handled
        * @return ETrue if operation handled
        */
        TBool HandleOperationL( CVtEngOperation& aOp );

        /**
        * Previous session state.
        */
        MVtEngSessionInfo::TSessionState PreviousSessionState() const;

        /**
        * Current session state.
        */
        MVtEngSessionInfo::TSessionState SessionState() const;

        /**
        * Returns handler container.
        * @return handler container
        */
        inline CVtEngHandlerContainer& Handlers();

        /**
        * Returns event manager.
        * @return event manager
        */
        inline CVtEngEventManager& EventManager();

    private:

        /**
        * C++ constructor.
        */
        CVtEngStateManager( 
            CVtEngHandlerContainer& aHandlers,
            CVtEngEventManager& aEventManager );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        // Previous session state
        MVtEngSessionInfo::TSessionState iPreviousSessionState;

        // Current session state
        CVtEngStateBase*        iSessionState;
         
        // Handler container
        CVtEngHandlerContainer& iHandlers;

        // Event manager
        CVtEngEventManager&     iEventManager;

    };

#include    "CVtEngStateManager.inl"

#endif      // CVTENGSTATEMANAGER_H   
            
// End of File
