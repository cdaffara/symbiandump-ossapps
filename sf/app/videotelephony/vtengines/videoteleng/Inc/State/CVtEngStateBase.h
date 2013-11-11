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
* Description:  Base class for states
*
*/



#ifndef CVTENGSTATEBASE_H
#define CVTENGSTATEBASE_H

//  INCLUDES
#include <e32base.h>
#include "vtengcommands.h"
#include "CVtEngStateManager.h"
#include "CVtEngEventManager.h"
#include "CVtEngMediaHandler.h"
#include "CVtEngSessionHandler.h"
#include "CVtEngOperation.h"
#include "CVtEngSettings.h"
#include "VtEngUtils.h"

// FORWARD DECLARATIONS
class CVtEngSessionHandler;
class CVtEngMediaHandler;
class CVtEngAudioHandler;

// CLASS DECLARATION

/**
*  State base class

*  @lib videoteleng
*  @since 2.6
*/
NONSHARABLE_CLASS( CVtEngStateBase ) : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Destructor.
        */
        virtual ~CVtEngStateBase();

    public: // New functions
        
        /**
        * Updates state. May result to state transition
        */
        virtual void UpdateL() = 0;

        /**
        * Checks if a command is valid in current state.
        * @param aCommandId command to be validated
        * @return ETrue if command is valid.
        */
        virtual TBool ValidateCommand(
            const TVtEngCommandId aCommandId );

        /**
        * Handles an operation. Base class method is empty
        * and derived classes may handle the operation if applicable.
        * @param aOp operation to be handled
        * @return ETrue if handled
        */
        virtual TBool HandleL( CVtEngOperation& aOp );

        /**
        * Returns session handler.
        * @return reference to session handler
        */
        CVtEngSessionHandler& SessionHandler();

        /**
        * Returns media handler.
        * @return reference to media handler
        */
        CVtEngMediaHandler& MediaHandler();

        /**
        * Returns audio handler.
        * @return reference to audio handler
        */
        CVtEngAudioHandler& AudioHandler();

        /**
        * Creates new state if needed.
        * @return new state or NULL
        */
        CVtEngStateBase* NewStateL( 
            const MVtEngSessionInfo::TSessionState aState );

        virtual MVtEngSessionInfo::TSessionState State() const = 0;

    public: // New functions 

        /**
        * Sets new current state. 
        * @param aOldState old state.
        */
        void OpenL( CVtEngStateBase* aOldState );

        /**
        * For derived classes to handle state change
        */
        virtual void DoOpenL( ) = 0;

        
        /**
        * State uninitialization.
        */
        virtual void Close();

    protected: // new functions

        /**
        * Saves configuration to settings for later
        * usage.
        */
        void SaveToSettingsL( CVtEngOperation& aOp );

        /**
        * Notifies state change to event manager.
        * @param aNewState new session state.
        */
        void NotifyStateChange( MVtEngSessionInfo::TSessionState aNewState );

        /**
        * Saves video state to settings. Used e.g. when video can not be 
        * started in current state.
        * @param aEnabled video is enabled
        */
        void SetVideoEnabled( TBool aEnabled );

        /**
        * Goes to disconnecting or idle state if needed.
        * @return state object if transition is required.
        */
        //CVtEngStateBase* DisconnectingOrIdleStateL();

        /**
        * Creates negotiating state if bearer state is connected
        * and pv state is ESetup, and UI has indicated state changte
        * is permitted.
        * @return negotiating state object or NULL
        */
        CVtEngStateBase* NegotiatingOrConnectedStateL( 
            TBool aAcceptOnlyNegotiating = EFalse );


        /**
        * Saves video state to settings. Used e.g. when video can not be 
        * started in current state.
        * @param aEnabled video is enabled
        */
        void TerminateSessionL( CVtEngOperation& aOp );

    protected:

        /**
        * C++ constructor.
        */
        CVtEngStateBase( 
            CVtEngStateManager& aStateManager,
            CVtEngEventManager& aEventManager );

    protected:  // Data
        // State manager
        CVtEngStateManager& iStateManager;

        // Event manager
        CVtEngEventManager& iEventManager;

    private:
    };

#endif      // CVTENGSTATEBASE_H

// End of File
