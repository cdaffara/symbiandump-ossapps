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
* Description:  Sync Client-Server header
*
*/


#ifndef SCONSYNCCLIENTSERVER_H
#define SCONSYNCCLIENTSERVER_H

#include <e32base.h>

_LIT(KSconSyncServerName, "sconsyncserver");
_LIT(KSconSyncServerExe, "sconsyncserver.exe");


enum TService
{
    ESendChunkHandle,
    ECancelRequest,
    EListImplementations,
    
    EOpenStore,
    EOpenItem,
    ECreateItem,
    EReplaceItem,
    EMoveItem,
    EDeleteItem,
    ESoftDeleteItem,
    EDeleteAllItems,
    EReadParent,
    
    EHasHistory,
    EAddedItems,
    EDeletedItems,
    ESoftDeletedItems,
    EModifiedItems,
    EMovedItems,
    
    //EListChanges,
    EResetChangeInfo,
    ECommitChangeInfo,
    
    ECloseStore,
    ESetSyncTimeStamp,
    EGetSyncTimeStamp,
    EExportStoreFormat,
    ESetRemoteStoreFormat,
    
    EMaxService     // Not an actual service
};

#endif // SCONSYNCCLIENTSERVER_H
