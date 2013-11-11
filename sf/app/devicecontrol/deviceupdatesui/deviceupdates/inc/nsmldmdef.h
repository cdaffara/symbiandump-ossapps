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
* Description:  Application definitions.
*
*/


#ifndef NSMLDMDEF_H
#define NSMLDMDEF_H

#include <e32base.h>
#include "nsmldmuidefines.h"
//#include "NSmlDMSyncUi.hrh"

#define KNSmlMaxURLLength 150
#define KNSmlDmServerIdMaxLength 150
#define KNSmlMaxItemLength 150	// The item max length.


const TInt KNSmlDefaultGranularity = 1;

const TInt KBufSize16   = 16;
const TInt KBufSize     = 128;
const TInt KBufSize255  = 255;
const TInt KBufSize256  = 256;
const TInt KBufSize32   = 32;
const TInt KPortMaxSize = 65535;
const TInt KPortMinSize = 1;
// View UID:s
//const TUid KNSmlDMFotaViewId        = { ENSmlFotaViewTab     };
//const TUid KNSmlDMProfilesViewId    = { ENSmlProfilesViewTab };

const TInt KDefaultHttpPort     = 80;
const TInt KDefaultHttpsPort    = 443;
 // setting dialog listbox update delay after db event
const TInt KListBoxUpdateDelay  = 250;
const TInt KHttpUsed  = 1;
const TInt KHttpNotUsed  = 0;
const TInt KNumberofCharacters  = 4;
const TInt KMultiplier  = 1000000;

//errors
const TInt KDMErr					= -8144;

// Medium type definitions
const TUid KUidNSmlMediumTypeInternet  = { 0x101F99F0 };
const TUid KUidNSmlMediumTypeBluetooth = { 0x101F99F1 };
const TUid KUidNSmlMediumTypeUSB       = { 0x101F99F2 };
const TUid KUidNSmlMediumTypeIrDA      = { 0x101F99F3 };
const TUid KUidSmlSyncApp = { 0x101F6DE5 };

_LIT( KNSmlHttp,    "http" );
_LIT( KNSmlHttps,   "https" );
_LIT( KHttpHeader,  "http://" );
_LIT( KHttpsHeader, "https://" );
_LIT( KColon,       ":" );
_LIT( KHttpSep,     "://" );
_LIT( KSlash,       "/" );
_LIT( KSmlEOL,      "\n" );
_LIT( KNSmlTab,     "\t" );
_LIT( KBackSlash,       "\\" );
_LIT( KZDrive, "z:" );

#ifdef __SCALABLE_ICONS
    _LIT( KDMAppIconFileName, "nsmldmsync.mif" );
#else // !__SCALABLE_ICONS
    _LIT( KDMAppIconFileName, "nsmldmsync.mbm" );
#endif

//
// NOTE: this enum must match to resource r_sml_bearer_type
//
enum TNSmlSyncBearerType
	{
	EAspBearerInternet = 0,
	EAspBearerBlueTooth = 1,
	EAspBearerUsb = 2,
	EAspBearerIrda = 3,
    EAspBearerLast = 4
	};

// NOTE: this enum must match with property array described in 
// SyncML_Sync_Agent_Client_API_Specification.doc.
enum TNSmlSyncTransportProperties
	{
	EPropertyIntenetAccessPoint = 0,
	EPropertyHttpUsed = 5,
	EPropertyHttpUserName = 6,
	EPropertyHttpPassword = 7
	};

enum TNSmlSyncState
	{
	ESASyncStateEnable,
	ESASyncStateDisable,
	ESASyncStateConfirm
	};

enum TNSmlSyncInitiation    
	{		
	EClientInitiated = 1,
	EServerAlerted
	};

enum TNSmlEditMode
	{ 
	ESmlNewProfile = 0,
	ESmlEditProfile,
	ESmlCopyProfile
	};

enum TNSmlProfileMediumType
	{
	EProfileInternet = 0,
	EProfileObexBlueTooth = 1
	};

enum TNSmlMandatoryProfileData
	{
	EMandatoryOk,
	EMandatoryNoProfileName,
    EMandatoryNoHostAddress,
    EMandatoryNoContent,
	EMandatoryNoLocalDatabase,
	EMandatoryNoRemoteDatabase,
	EMandatoryNoSyncType
	};

enum TNSmlStatus
	{
	ENSmlSyncComplete,
	ENSmlRefreshMainView
	};

enum TNSmlAccessPointSelection
	{	
	ENsmlDefaultConnection,
	ENsmlIap,
	ENsmlAlwaysAsk
	};	

//Constants used when update happen NSC/DM 
enum {
		EFotaUpdateNSC=1,
		EFotaUpdateNSCBg,
		EFotaUpdateDM
};


#endif // NSMLDMDEF_H

// End of file
