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

#ifndef __AMPRIVATECRKEYS_H__
#define __AMPRIVATECRKEYS_H__

/**
* Application Management Internal Keys UID
*/
const TUid KCRUidPrivateApplicationManagementKeys = {	0x200267FB };

/**
* CenRep key that holds smallest supported Internal Component Id 
*
* Possible integer values:
* positive integer values 
*
* Default value: -1 (not defined)
*
* Note: Only for Application Management private use
*
*/
 
const TInt KMinIdValue = 0x20;

/**
* CenRep key for Next free ID:
* Holds the next free if for deployment component
*
* Possible integer values:
* positive integer values 
*
* Default value: -1 (not defined)
*
* Note: Only for Application Management private use
*
*/
const TInt KNextIdKey = 0x01;

/**
* Holds the lenght of deployment component id list
*
* Possible integer values:
* positive integer values 
*
* Default value: 0 
*
* Note: Only for Application Management private use
*
*/
const TInt KIdListLengthKey = 0x02;

/**
* Holds the deployment component id list
*
* Possible string values: Application Management defined
*
* Default value: (empty)
*
* Note: Only for Application Management private use
*
*/
const TInt KIdListKey = 0x03 ;

/**
* Holds the value which determines whether SYnc or Async operation
* is enabled
*
* Possible string values: Application Management defined
*
* Default value: 1 ( ASync is enabled)
*
* Note: Only for Application Management private use
*
*/
const TInt KAsyncEnabled = 0x04;

/**
* This value is static node name and cenrep configurable
*
* Possible string values: Application Management defined
*
* Note: Only for Application Management private use
*
*/

const TInt KAMDownloadPredefinedNode= 0x05 ;

/**
* This value is retry count and determines the maximum
* allowable retry count
*
* Possible string values: Application Management defined
*
* Note: Only for Application Management private use
*
*/
const TInt KMaxRetryCount = 0x06;

/**
* This value stores the status of Sync reporting 
*
* Possible string values: Application Management defined
*
* Note: Only for Application Management private use
*
*/
const TInt KAMSyncStatus = 0x07;

/**
* This value tells which adapter(SCM/SCOMO) did DM operations (ADD/DELETE/GET/UPDATE/EXEC) in previous DM session.
* Based on this we will call CheckStateChangesL() in DDFStructurel() API 
*
* Possible string values: default      - 0
*                         SCM adapter  - 1
*                         SCOMO adapter -2
*
* Note: Only for Application Management private use
*
*/
const TInt KAMAdapterValue = 0x08;

/**
* This value checks if Static node feature is enabled  
* if value is 1 then CMCCSCM static node is visible under 
* download node otherwise it is not visible
* 
* default value from platform side this feature is disabled.
* 
* Note: Only for Application Management private use
*/
const TInt KAMStaticNodeEnabled = 0x09;

/**
* This value indicate SCOMO MetaType used in generic alert and this 
* is configurable
* 
* Note: Only for Application Management private use
* 
* 
*/
const TInt KSCOMOMetaTypeKey = 0x0A;

/**
* This value indicate SCOMO MetaFormat used in generic alert and this 
* is configurable
* 
* Note: Only for Application Management private use
* 
* 
*/
const TInt KSCOMOMetaFormatKey = 0x0B;

/**
* This value indicate SCOMO Mark used in generic alert and this 
* is configurable
* 
* Note: Only for Application Management private use
* 
* 
*/
const TInt KSCOMOMark = 0x0C;

/**
* This default value is used to get drive from cenrep and this is
* applicable in case of silent mode of SCOMO operation
* 
* Note: Only for Application Management private use
* 
* 
*/
const TInt KDefaultDriveForInstall = 0x0D;

#endif // __AMPRIVATECRKEYS_H__

// End of file
