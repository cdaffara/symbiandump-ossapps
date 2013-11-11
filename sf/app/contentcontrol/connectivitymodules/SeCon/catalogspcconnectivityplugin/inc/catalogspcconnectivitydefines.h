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
* Description:  Server information that the client needs
*
*/


#include <e32base.h>

/**  Server name */
_LIT( KCatalogsPCConnectivityServerName, 
    "CatalogsPCConnectivityServer" );

/** Semaphore that is used when creating a new server */
_LIT( KCatalogsPCConnectivityServerSemaphoreName, 
    "CATALOGSPCCONNECTIVITYSemaphore" );

/** File that is passed to the new server-process for execution */
_LIT( KCatalogsPCConnectivityServerFilename, 
    "catalogsenginepcclientfrontend" );

// Default message slot amount in a session
const TUint KCatalogsPCConnectivityDefaultMessageSlots = 255;

/**  Version numbers */
const TUint KCatalogsPCConnectivityMajorVersionNumber=1;
const TUint KCatalogsPCConnectivityMinorVersionNumber=0;
const TUint KCatalogsPCConnectivityBuildVersionNumber=0;

/**  Function opcodes used in message passing between client and server */
enum TCatalogsPCConnectivityServRqst
    {
    ECatalogsPCConnectivitySupportedMimeTypes = 1,
    ECatalogsPCConnectivityPutData,
    ECatalogsPCConnectivityGetData,
    ECatalogsPCConnectivityGetSize
    };

