/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DTMF handler interface for DTMF states and media 
*                handler to use.
*
*/



#ifndef MVTENGDTMFHANDLER_H
#define MVTENGDTMFHANDLER_H

//  INCLUDES
#include <e32std.h>

// DATA TYPES

// FORWARD DECLARATIONS
class CVtEngOperation;
class TVtEngDtmfTone;
class MVTUserInput;
class CVtEngDtmfState;

// CLASS DECLARATION

/**
*  Base class for data types
*
*  @lib videoteleng
*  @since 2.6
*/
class MVtEngDtmfHandler
    {
    public: // 
        /**
        * Dtmf states
        */
        enum TVtEngDtmfState 
            {
            // Not sending DTMF
            EVtDtmfStateIdle = 0,
            // Sending DTMF
            EVtDtmfStateSending,
            // Sending DTMF, another waiting in buffer
            EVtDtmfStateBuffered
            };

    public:

        /**
        * Handles a DTMF command.
        * @param 
        */
        virtual void HandleL( CVtEngOperation& aOp ) = 0;

        /**
        * Activates a DTMF state.
        * @param aNewState state index
        * @return activated state
        */
        virtual CVtEngDtmfState& ActivateState( 
            MVtEngDtmfHandler::TVtEngDtmfState aNewState ) = 0;

        /**
        * Handles completion of DTMF sending, non-leavable method.
        * @param aError Symbian OS error code
        */
        virtual void SendComplete( TInt aError ) = 0;

        /**
        * Returns new instance used for sending DTMF tone to protocol 
        * @param aTone tone to send
        * @return protocol DTMF tone.
        */
        virtual MVTUserInput& ReserveProtoDtmfL(
            const TVtEngDtmfTone& aTone  ) = 0;

        /**
        * Releses protocol DTMF instance.
        */
        virtual void ReleaseProtoDtmf() = 0;

		/**
        * Compares last issued DTMF command ID to given command ID.
        * @param aCheckCommandId command ID to be compared against
        * @return ETrue if IDs are same, otherwise EFalse
        */
		virtual TBool CheckCommandId( TInt aCheckCommandId ) const = 0 ;

		/**
        * Stores command ID, used in CheckCommandID.
        * @param aStoreCommandId command ID to be stored.        
        */
		virtual void StoreCommandId( TInt aStoreCommandId ) = 0;
    };

#endif      MVTENGDTMFHANDLER_H

// End of File
