/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CSConPCD constants
*
*/


#ifndef _SCONPCDCONSTS_H
#define _SCONPCDCONSTS_H

// Timer value used for queue polling
const TInt KSConTimerValue( 20000 );
// ConML Status code: Task completed
const TInt KSConCodeTaskCompleted( 200 );
// ConML Status code: Task created
const TInt KSConCodeTaskCreated( 201 );
// ConML Status code: Task prcosessing started
const TInt KSConCodeProcessingStarted( 202 );
// ConML Status code: Task partially completed
const TInt KSConCodeTaskPartiallyCompleted( 206 );
// ConML Status code: Not found
const TInt KSConCodeNotFound( 404 );
// ConML Status code: Conflict
const TInt KSConCodeConflict( 409 );
// ConML Status code: Not supported
const TInt KSConCodeNotSupported( 501 );
// ConML Status code: Incompatible version
const TInt KSConCodeIncompatible( 507 );
// ConML Status code: Cancelled
const TInt KSConCodeCancelled( 514 );
// ConML Status code: No memory
const TInt KSConCodeNoMemory( 420 );


// ConML Status codes in installer errorcases
const TInt KSConCodeInstErrUserCancel = KSConCodeCancelled; // User cancelled the operation
const TInt KSConCodeInstErrFileCorrupted = 600; // File is corrupted
const TInt KSConCodeInstErrInsufficientMemory = KSConCodeNoMemory; // Insufficient free memory in the drive to perform the operation
const TInt KSConCodeInstErrPackageNotSupported = KSConCodeIncompatible; // Installation of the package is not supported
const TInt KSConCodeInstErrSecurityFailure = 601; // Package cannot be installed due to security error
const TInt KSConCodeInstErrMissingDependency = 602; // Package cannot be installed due to missing dependency
const TInt KSConCodeInstErrFileInUse = 603; // Mandatory file is in use and prevents the operation
const TInt KSConCodeInstErrGeneralError = 604; // Unknown error
const TInt KSConCodeInstErrNoRights = 605; // The package has no rights to perform the operation
const TInt KSConCodeInstErrNetworkFailure = 606; // Indicates that network failure aborted the operation
const TInt KSConCodeInstErrBusy = 607; // Installer is busy doing some other operation
const TInt KSConCodeInstErrAccessDenied = 608;  // Target location of package is not accessible
const TInt KSConCodeInstUpgradeError = 609;  // The package is an invalid upgrade

const TInt KSConCodeInstErrSifMissingBasePackage = 610;
const TInt KSConCodeInstErrSifOverflow = 611;
const TInt KSConCodeInstErrSifSameVersionAlreadyInstalled = 612;
const TInt KSConCodeInstErrSifNewerVersionAlreadyInstalled = 613;
const TInt KSConCodeInstErrSifAlreadyActivated = 614;
const TInt KSConCodeInstErrSifAlreadyDeactivated = 615;
const TInt KSConCodeInstErrSifBadComponentId = 616;
const TInt KSConCodeInstErrSifBadInstallerConfiguration = 617;
const TInt KSConCodeInstErrSifPackageCannotBeInstalledOnThisDevice = 618;
const TInt KSConCodeInstErrSifUnsupportedLanguage = 619;
const TInt KSConCodeInstErrScrWriteOperationInProgress = 620;
const TInt KSConCodeInstErrScrReadOperationInProgress = 621;
const TInt KSConCodeInstErrScrNoActiveTransaction = 622;
const TInt KSConCodeInstErrScrUnsupportedLocale = 623;


// ConML Status code: 1001...1050 System wide error code
const TInt KSConCodeFirstSymbianErr( 1000 );

struct SDeviceInfo
	{
	TSConMethodName method;
	TBool support;
	};

// Supported Backup/Restore and Install methods	
const SDeviceInfo KSupportedMethods[] =
	{
	{ EInstall, 			ETrue },
	{ EUninstall, 			ETrue },
	{ EListInstalledApps, 	ETrue },
	{ EListDataOwners,		ETrue },
	{ ESetBURMode, 			ETrue },
	{ ESetInstParams,		ETrue },
	{ EGetDataSize, 		ETrue },
	{ ERequestData, 		ETrue },
	{ ESupplyData, 			ETrue },
	{ EReboot,				EFalse } // reboot not supported anymore (3.2 ->)
	};
	
// Supported ConML version
_LIT8( KConMLVersion, "2.0" );
// SIS mime type for installer
_LIT8( KSisMimeType, "x-epoc/x-sisx-app" );
// Java mime type for installer
_LIT8( KMidletMimeType, "application/java-archive" );

#endif

// End of file
