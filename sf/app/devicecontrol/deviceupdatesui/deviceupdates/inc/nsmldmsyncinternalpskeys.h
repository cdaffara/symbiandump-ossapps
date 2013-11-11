/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Publish and subcribe keys published by device manager
*                application.
*
*/


#ifndef NSMLDMSYNCINTERNALPSKEYS_H
#define NSMLDMSYNCINTERNALPSKEYS_H

// CONSTANTS

// Device manager key UID
const TUid KPSUidNSmlDMSyncApp = {0x101f6de5};

// Fota Server key UID
const TUid KFotaServerAppUid = {0x102072C4};

// PubSub key used to share information about the
// initiator of the device management session.
// Contains the identifier of a job that is
// started from the UI. 
const TUint32 KNSmlDMSyncUiInitiatedJobKey = 0x00000001;

// PubSub key used to share information about the
// type of the server alert for device management session.
// Contains the identifier of server alert type
// started from DM Server
const TUint32 KServerAlertType = 0x00000004;	

// Indicates that no user initiated sync jobs are in
// progress.
const TInt KNSmlDMNoUserInitiatedJob = -1;

// PubSub key used to share information about the
// launching of the device management application.
// Contains the identifier of server alert or 
// OmaDmCPPlugin
const TUint32 KNSmlDMSyncUiLaunchKey = 0x00000002;

//PubSub key used for putting Fota UI in Foreground or background
//Sets only by FOTA & DM uses this key value
const TUint32 KFotaDLStatus = 0x00000008;

//PubSub key used for main view first listitem display
//Sets by DM UI or FOTA
const TUint32 KFotaDLRefresh = 0x00000009;


//PubSub key used to mark which application is handling the process of download
//Sets by DM UI, NSC or NSCBackground
const TUint32 KNSmlCurrentFotaUpdateAppName = 0x0000000A;


//PubSub key used to define should "Install Now?" query be shown or not
//Sets by DM UI or NSC
const TUint32 KDevManShowInstallNowQuery = 0x0000000B;

//PubSub key used to store the status/error of current DM Job session
//Sets by DM UI & used by FOTA in case of OMA DM large object download
const TUint32  KDmJobCancel = 0x00000012; 

//PubSub key used to store the status of whether the DM is busy or not 
//Sets by DM UI & used by IAD or others
const TUint32 KDMIdle = 0x0000000C; 
const TUint32 KSilentSession = 0x0000000D;
#endif // NSMLDMSYNCINTERNALPSKEYS_H
            
// End of File
