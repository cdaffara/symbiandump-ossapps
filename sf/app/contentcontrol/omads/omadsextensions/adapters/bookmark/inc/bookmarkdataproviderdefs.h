/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef __BOOKMARKDATAPROVIDERDEFS_H__ 
#define __BOOKMARKDATAPROVIDERDEFS_H__ 


// ------------------------------------------------------------------------------------------------
// Databases
// ------------------------------------------------------------------------------------------------

// Resource file for CSmlDataStoreFormat

// The following works with DS plugin added into ROM image 
_LIT(KBookmarkStoreFormatRscRom,"z:bookmarkdatastore.rsc");

// The following works with DS plugin installed from SIS
_LIT(KBookmarkStoreFormatRsc,"bookmarkdatastore.rsc");


// ------------------------------------------------------------------------------------------------
// Constants
// ------------------------------------------------------------------------------------------------


// ------------------------------------------------------------------------------------------------
// Macros
// ------------------------------------------------------------------------------------------------
#define SAFEDELETE(x) {if(x) {delete x; x = NULL;}}
#define SAFEDELETEARRAY(x) {if(x) {delete[] x; x = NULL;}}

#endif // __BOOKMARKDATAPROVIDERDEFS_H__ 
