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
* Description:  P/S Category and Keys.
*
*/
#ifndef _EMAILSTOREPSKEYS_H_
#define _EMAILSTOREPSKEYS_H_

#include "emailstoreuids.hrh"

/**
  * EmailStore Upgrade P/S Category UID
  */
static const TUid KEmailStoreUpgradePSCategory = { KUidEmailStorePreInstallExe };

// Properties -> Value range is arbitrary chosen to be 115-215 (0x73-0xD7).
/**
  * EmailStore P/S Upgrade Key
  */
const TInt KProperty_EmailStore_Upgrade = 115;
  

// Copied from emailshutter.h 
// Value to be set in shutdown P&S key.
const TInt KEmailShutterPsValue = 1;

#endif //_EMAILSTOREPSKEYS_H_
