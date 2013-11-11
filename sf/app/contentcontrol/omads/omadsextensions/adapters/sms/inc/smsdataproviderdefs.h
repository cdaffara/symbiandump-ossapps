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
* Description:  Constant definitions used by SMS DS Plug-in Adapter
*
*/


#ifndef __SMSDATAPROVIDERDEFS_H__
#define __SMSDATAPROVIDERDEFS_H__ 


// ------------------------------------------------------------------------------------------------
// Databases
// ------------------------------------------------------------------------------------------------
// Database name
_LIT( KNSmlDefaultLocalDbName, "SmsSyncLocalDb" );

// Resource file for CSmlDataStoreFormat

// The following works with DS plugin added into ROM image 
_LIT( KSmsStoreFormatRscRom,"z:smsdatastore.rsc");

// The following works with DS plugin installed from SIS
_LIT( KSmsStoreFormatRsc,"smsdatastore.rsc");

// ------------------------------------------------------------------------------------------------
// Constants
// ------------------------------------------------------------------------------------------------
// max foldername
const TInt KMaxFolderNameLength = 50;

// Implementation uid
const TUint KSmsDataProviderImplUid = 0x10206B5C;

// ------------------------------------------------------------------------------------------------
// Macros
// ------------------------------------------------------------------------------------------------
#define SAFEDELETE( x ) {delete x; x = NULL;}
#define SAFEDELETEARRAY( x ) {delete[] x; x = NULL;}

#endif // __SMSDATAPROVIDERDEFS_H__ 
