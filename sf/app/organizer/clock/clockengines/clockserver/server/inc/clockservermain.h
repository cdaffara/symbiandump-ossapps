/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Entry point for the clock server.
*
*/

#ifndef __CLOCKSERVER_MAIN_H__
#define __CLOCKSERVER_MAIN_H__

// Constants
/**
* @enum TSrvFault
* @brief The server panic codes
*/
enum TSrvFault
    {
    EMainCreateSemaphore,
    EMainCreateScheduler,
    EMainActiveError,
    EMainServerNotStarted,
    EMainClockServerThreadRename
    };

// Function declarations
/**
* @brief Called when there is a panic in the server program.
* @param aFault of type TSrvFault, one of the panic codes.
*/
GLREF_C void Fault( TSrvFault aFault );

/**
* @brief Creates the server object. Called if the object 
*			  is not already present.
* @param TAny*
* @return TInt Error code.
*/
IMPORT_C TInt TheServerThread( TAny* /*aPr*/ );

/**
* @brief Returns if ClockServer is already running or not.
* @return TBool ETrue if ClockServer already running, EFalse otherwise.
*/
LOCAL_C TBool IsServerLoaded();

/**
* @brief The Main startup entry point.
* @return TInt Error code.
*/
GLDEF_C TInt E32Main();

#endif		// __CLOCKSERVER_MAIN_H__

// End of file
