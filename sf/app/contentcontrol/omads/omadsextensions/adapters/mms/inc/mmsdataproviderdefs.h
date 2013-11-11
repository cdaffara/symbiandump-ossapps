/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Part of SyncML Data Synchronization Plug In Adapter
*
*/


#ifndef __MMSDATAPROVIDERDEFS_H__ 
#define __MMSDATAPROVIDERDEFS_H__ 

#include <e32def.h>
#include <e32cmn.h>

// ------------------------------------------------------------------------------------------------
// Databases
// ------------------------------------------------------------------------------------------------
// Database name
_LIT( KNSmlDefaultLocalDbName, "MmsSyncLocalDb" );

// Resource file for CSmlDataStoreFormat in SIS installation
_LIT(KMmsStoreFormatRsc,"mmsdatastore.rsc");

// Must be like below in flash image
_LIT(KMmsStoreFormatRscRom,"z:mmsdatastore.rsc");

// ------------------------------------------------------------------------------------------------
// Constants
// ------------------------------------------------------------------------------------------------
// max foldername
const TInt KMaxFolderNameLength = 50;

// Implementation uid
const TUint KMmsDataProviderImplUid = 0x101FB0E9;

// ------------------------------------------------------------------------------------------------
// Macros
// ------------------------------------------------------------------------------------------------
#define SAFEDELETE(x) {if(x) {delete x; x = NULL;}}
#define SAFEDELETEARRAY(x) {if(x) {delete[] x; x = NULL;}}

#endif // __MMSDATAPROVIDERDEFS_H__ 
