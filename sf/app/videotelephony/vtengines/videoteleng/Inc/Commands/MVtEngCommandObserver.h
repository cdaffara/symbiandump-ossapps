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
* Description:  Observer for asynchronous commands.
*
*/



#ifndef MVTENGCOMMANDOBSERVER_H
#define MVTENGCOMMANDOBSERVER_H

//  INCLUDES
#include <vtengcommands.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
* Observer for asynchronous commands.
*
* @since 2.6
*/
class MVtEngCommandObserver
    {
    public: // new functions

        /**
        * Callback for command completion.
        * @param aCommand completed command. This is
        * same instance as passed in MVtEngCommandHandler::ExecuteL.
        * Client may delete the command.
        * @param aError Symbian OS error code.
        */
        virtual void HandleVTCommandPerformedL( 
            TVtEngCommandId aCommand,
            const TInt aError ) = 0;
    };

#endif      // MVTENGCOMMANDOBSERVER_H
            
// End of File
