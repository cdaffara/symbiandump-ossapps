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
* Description:
* Constant definitions for Email shutdown
*
*/

#ifndef EMAILSHUTDOWNCONST_H
#define EMAILSHUTDOWNCONST_H

#include <e32cmn.h>                 // TUid
#include "emailshutdownconst.hrh"   // KEmailShutdownUidAsTInt


// Publish & Subscribe category for Email shutdown events
const TUid KEmailShutdownPsCategory = { KEmailShutdownUidAsTInt };

// Size of one item in platform API UID list (in Publish & Subscribe
// key EEmailPsKeyPlatformApiAppsToClose)
const TInt KEmailPlatformApiUidItemSize = sizeof( TInt32 );

// Publish & Subscribe keys used in shutdown process
enum TEmailShutdownPsKeys
    {
    // Publish & Subscribe key used in communication between Shutter
    // application and installation initiators
    EEmailPsKeyInstallationStatus = 1,
    
    // Publish & Subscribe keys for Email shutdown events
    EEmailPsKeyShutdownClients,
    EEmailPsKeyShutdownPlugins,
    EEmailPsKeyShutdownMsgStore,
    
    // Publish & Subscribe keys to register 3rd party applications to be closed
    EEmailPsKeyPlatformApiAppsToCloseLength,
    EEmailPsKeyPlatformApiAppsToClose
    };

// Publish & Subscribe values to be used with key EEmailPsKeyInstallationStatus
enum TEmailShutdownPsInstallationValues
    {
    // Value to be set by installation initiators when installation is starting
    EEmailPsValueInstallationStarting = 1,
    // Value to be set by Shutter when all services are shutdown
    EEmailPsValueInstallationOkToStart,
    // Value to be set by installation initiators when installation is started
    // after EEmailPsValueInstallationOkToStart event received
    EEmailPsValueInstallationStarted,
    // Value to be set by Starter when installation has finished
    EEmailPsValueInstallationFinished
    };

// Executable name of the process that implements the shutdown functionality
_LIT( KEmailShutdownHandlerExe, "emailservermonitor.exe" );

// Command line arguments that can be given to shutdown handler process when
// starting it

// KEmailShutdownHandlerArgOnlyShutter should be used when starting the process
// just before starting installation (if it wasn't already running)
_LIT( KEmailShutdownHandlerArgOnlyShutter, "OnlyShutter" );

// KEmailShutdownHandlerArgRestart should be given when restarting
// the process after installation
_LIT( KEmailShutdownHandlerArgRestart, "RestartAfterInstallation" );


#endif // EMAILSHUTDOWNCONST_H
