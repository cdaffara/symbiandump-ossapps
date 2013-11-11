/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Device manager private CenRep key definitions
*
*/


#ifndef NSMLDMSYNCAPP_PRIVATE_CR_KEYS_H
#define NSMLDMSYNCAPP_PRIVATE_CR_KEYS_H

// CONSTANTS

// Device manager key UID
const TUid KCRUidNSmlDMSyncApp = {0x101f6de5};
//browser uid 
const TUid browseruid = {0x10008d39}; 
//java uid 
const TUid  javauid =   {0x2001FD68};
//flash uid 
const TUid  flashuid = {0x101FD693}; 

// CenRep keys for default dm profile used in the
// firmware update over the air.
const TUint32 KNSmlDMDefaultFotaProfileKey = 0x00000001;

// CenRep key that can be used to hide a DM profile from the
// profile list on the UI, but still retaining the UI functionality
// during server alerted session. The server identifier used
// to authenticate the server should be stored here.
const TUint32 KNSmlDMFotaHiddenProfileIdKey = 0x00000002;

// CenRep key that can be used to allow to show runtime versions
// or not.Default value is 0 which makes it off 
const TUint32 KNsmlDmRuntimeVerSupport = 0x00000003;


#endif // NSMLDMSYNCAPP_PRIVATE_CR_KEYS_H
            
// End of File
