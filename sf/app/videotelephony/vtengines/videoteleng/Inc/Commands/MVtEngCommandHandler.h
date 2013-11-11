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
* Description:  Command handler interface of the engine.
*
*/



#ifndef MVTENGCOMMANDHANDLER_H
#define MVTENGCOMMANDHANDLER_H

//  INCLUDES
#include <e32base.h>
#include "vtengcommands.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class MVtEngCommandObserver;

// CLASS DECLARATION

/**
*  Handler for commands from client.
*
*  @lib videoteleng
*  @since 2.6
*/
class MVtEngCommandHandler
    {
    public:        

        /**
        * EAttribAsync
        *  command is asynchronous. 
        * EAttribEnabled
        *  Command can be executed at the moment.
        * EAttribSupported
        *  Command is supported by the engine.
        */
        enum TCommandCaps 
            { 
            EAttribAsync      = 0x00000001,
            EAttribEnabled    = 0x00000002,
            EAttribSupported  = 0x00000004
            };

        /**
        * Excecutes a command. This function leaves KErrNotReady
        * if engine is in such a state that command cannot be
        * executed.
        * 
        * @param aCommandId command to be executed.
        * @param aParams command parameters, NULL if command
        * does not require any.
        */
        virtual void ExecuteL( 
            const TVtEngCommandId aCommandId,
            TDesC8* aParams ) = 0;

        /**
        * Cancels a command.
        * @param aCommandId command to cancel. 
        * @return KErrNotFound if command is not pending.
        */
        virtual TInt CancelCommand( const TVtEngCommandId aCommandId ) = 0;

        /**
        * Returns capabilities of a command.
        * @return command caps
        */
        virtual TInt GetCommandCaps( 
            const TVtEngCommandId aCommand ) = 0;

        /**
        * Returns pending command.
        * @return pending command
        */
        virtual TVtEngCommandId PendingCommand() = 0;

        /**
        * Returns invalid command.
        * @return invalid command
        */
        virtual TVtEngCommandId InvalidCommand() = 0;
        
        /**
        * Sets command observer. First call on this is guaranteed
        * to succeed.
        * @param aObserver command observer.
        */
        virtual void AddObserverL( MVtEngCommandObserver& aObserver ) = 0;

        /**
        * Removes a command observer or if non-op if specified observer
        * is not found.
        * @param aObserver command observer to be removed.
        */
        virtual void RemoveObserver( MVtEngCommandObserver& aObserver ) = 0;
    };

#endif //MVTENGCOMMANDHANDLER_H

// End of File
