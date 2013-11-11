/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of applicationmanagement components
*
*/


#ifndef __APPLICATIONMANAGEMENTCOMMON_H__
#define __APPLICATIONMANAGEMENTCOMMON_H__

#include <e32base.h>
#include <uikon.hrh>
#include "ApplicationManagementConst.h"

namespace NApplicationManagement
	{

const TUint32  KAppMgmtServerUid         = {0x200267FB};

const TUint32 KAMServiceUid = {0x1020781B};

const TUid KProperty = {0x101F9A02};

const TUint32 KInteger = 0x00000001;

//const TUint32 KOmaDMAppUid = {0x200267FB};
const TInt KVersionLength= 7; // 3 for Major, 1 for '.' and 3 for Minor
_LIT( KLiteralPeriod, ".");

const TInt KAMDefaultValue = 0;
const TInt KAMSCMAdapter = 1;
const TInt KAMSCOMOAdapter = 2;

const int KDCIdMaxLength = 128 ;
typedef TBuf8<KDCIdMaxLength> TDeploymentComponentId;

const int KDeploymentComponentNameMaxLength = 128 ;
typedef TBuf8<KDeploymentComponentNameMaxLength> TDeploymentComponentName;

const int KDeploymentComponentVersionMaxLength = 32;
typedef TBuf8<KDeploymentComponentVersionMaxLength> TDeploymentComponentVersion;

const int KDeploymentComponentDescriptionMaxLength = 512;
typedef TBuf8<KDeploymentComponentVersionMaxLength> TDeploymentComponentDescription;

const int KDeploymentComponentPkgIdMaxLength = 256;
typedef TBuf8<KDeploymentComponentPkgIdMaxLength> TDeploymentComponentPkgId;

const int KDeploymentComponentPkgIdRefMaxLength = 256;
typedef TBuf8<KDeploymentComponentPkgIdRefMaxLength> TDeploymentComponentPkgIdRef;

const int KDeploymentComponentPkgTypeRefMaxLength = 256;
typedef TBuf8<KDeploymentComponentPkgTypeRefMaxLength> TDeploymentComponentPkgType;

typedef TBuf8<KDeploymentComponentNameMaxLength> TMidletnameName;

typedef TBuf8<KDeploymentComponentNameMaxLength> TMidletVendorName;
typedef TBuf8<KDeploymentComponentVersionMaxLength> TMidletVersion;

typedef TBuf8<KDeploymentComponentNameMaxLength> TPreInstalledAppName;
typedef TBuf8<KDeploymentComponentNameMaxLength> TPreInstalledAppVendorName;
typedef TBuf8<KDeploymentComponentVersionMaxLength> TPreInstalledVersion;

const int KDeploymentComponentURLMaxLength = 32;

const int KMaxMimeLength = 256 ;
typedef TBuf8<KMaxMimeLength> TMimeType;

const int KMaxDCUserIdLength = 256;
typedef TBuf8<KMaxDCUserIdLength> TDCUserId;

enum TDeploymentComponentState
	{
	EDCSError = -1,
	EDCSNone = 0,
	EDCSDelivered = 1,
	EDCSInactive = 2,
	EDCSActive = 3,
	EDCSDownload = 4,
	EDCSDeployed =5,
	EDCSDelete = 6
	};
	
enum TDeplCompAttrType 
	{
	EDCNone = 0,
	EDCId = 1,
	EDCName = 2,
	EDCVersion = 4,
	EDCInstallOptions = 8,
	EDCMetaData = 16,
	EDCMetaDataMime = 24,
	EDCData = 32,
	EDCDataMime = 48,
	EDCStatus = 64,
	EDCDownloadURI = 128,
	EDCStateValue = 256,
	EDCConRef = 512,
	EDCDescriptionRef = 1024,
	EDCPkgID = 2048,
	EDCPkgType = 4096
	};

enum TDownloadTarget
	{
	EDeliver,
	EInstall,
	EInstallAndActivate,
	EUpdate,
	EUpdateAndActivate
	};
enum TSCOMOStatusNodeStates
    {
    //Download Status Values
    EIdle = 10,
    EDownload_DownloadFailed = 20,
    EDownload_DownloadProgress = 30,
    EDownload_DownloadComplete = 40,
    EDownload_InstallProgress = 50,
    EDownload_InstalledFailedWithData = 60,
    EDownload_InstalledFailedWithOutData = 70,
    //Delivered Status Values
    EDelivered_RemoveFailed = 20,
    EDelivered_RemoveProgress = 30,
    EDelivered_InstallProgress = 40,
    EDelivered_InstalledFailedWithData = 50,
    EDelivered_InstallFailedwithOutData = 60,
    //Deployed Status Values
    EDeployed_RemoveFailed = 20,
    EDeployed_RemoveProgress = 30,
    EDeployed_ActivateFailed = 40,
    EDeployed_ActivateProgress = 50,
    EDeployed_DeactivateFailed = 60,
    EDeployed_DeactivateProgress = 70
    };
/**
*  CAMDeliveryDeploymentComponent class to represent delivery items
*/	

struct TDeploymentComponent 
	{
	// Data
	TDeploymentComponentId iId ;
	TDeploymentComponentName iName;
	TDeploymentComponentVersion iVersion ;
	TDeploymentComponentDescription iDescription;
	TDeploymentComponentPkgId iPkgID;
	TDeploymentComponentPkgIdRef iPkgIDRef;
	TDeploymentComponentPkgType iPkgType;
	TDeploymentComponentState iState ;
	TDeploymentComponentState iOldState ;
	TDeploymentComponent() : iId( KNullDesC8() ),
							 iName( KNullDesC8() ),
							 iVersion( KNullDesC8() ),
							 iDescription (KNullDesC8),
							 iState( EDCSNone ),
							 iOldState ( EDCSNone ) 
							 
	{
	
	}
	TDeploymentComponent( const TDesC8 &aId, 
						  const TDesC8 &aName, 
						  const TDesC8 &aVersion, 
						  TDeploymentComponentState aState )
		: iId( aId ), 
		iName( aName ), 
		iVersion ( aVersion ), 
		iState ( aState ), 
		iOldState( EDCSNone )
		{
		} 
	};

struct TDeploymentComponentIPC
	{
	TDeploymentComponent iComponent;
	TBuf8<256> iMimeType;
	TDCUserId iUserId;
	TDeploymentComponentIPC() : iComponent(),
								iMimeType( KNullDesC8() ),
								iUserId( KNullDesC8())
								
		{
		
		}
	TDeploymentComponentIPC( const TDeploymentComponent &aComponent,
		const TDesC8 &aMimeType,
		const TDCUserId &aUserId) 
			: iComponent( aComponent ), 
				iMimeType( aMimeType ),
				iUserId( aUserId )
		{
		
		}
	};
	
struct TDeplCompDataIPC
	{
	TUint32 iInternalId;
	TDeplCompAttrType iAttrType;
	TBuf8<256> iDataMime;
	};	
	
	}
#endif 

//  End of File
