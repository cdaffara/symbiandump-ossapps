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
 * Description:  Implementation of applicationmanagement components
 *
*/

 
#ifndef __AMPSKEYS_H__
#define __AMPSKEYS_H__

// Application Management Server UID for Publish and Subscribe key

const TUid KUidPSApplicationManagementKeys = { 0x200267FB };

// P&S key to check whether AM UI Server is enabled or disabled

const TUint KAMServerUIEnabled = 0x00001;

// P&S key to check whether device management session is silent
// or interactive(can be information or nonspecified as well)

const TUint KAMOperationNonSilent = 0x00002;

// P&S key to Notify OMA SCOMO adapter as soon as download is
// complete. This will be applicable in case of Sync operation

const TUint KSyncNotifier = 0x000004;

// Set this P&S key so that DM UI will not be closed in case of server initiated session
// applicable only for Interactive, Informative or Non specified

const TUint KDMUINotClosed = 0x00005;


// DM UI application ID

const TUid KPSUidNSmlDMSyncApp = {0x101f6de5};

// Key defined in DM UI Fw to know application management server
// whether session started in silent mode or non-silent

const TUint32 KSilentSession = 0x0000000D;




#endif


