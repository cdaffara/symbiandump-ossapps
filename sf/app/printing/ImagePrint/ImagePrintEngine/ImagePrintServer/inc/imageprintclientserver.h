/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#ifndef IMAGEPRINTCLIENTSERVER_H
#define IMAGEPRINTCLIENTSERVER_H

#include <e32std.h>

#include "imageprintserveruid.h"

_LIT( KImagePrintServerName, "imageprintserver" );
_LIT( KImagePrintServerImg, "imageprintserver" );		// EXE name

const TInt KMaxImagePrintMessage = 512;
const TInt KImagePrintServerMajor = 0;
const TInt KImagePrintServerMinor = 0;
const TInt KImagePrintServerBuild = 0;

enum TImagePrintMessages
	{
	ECountConnections = 0,
	ESetForeground,
	EReserveEngine,
	EReleaseEngine,
	ESupportedProtocols,
	EGetNumPrintPages,
	EGetJobStatus,
	EGetPrinterStatus,
	ECancelDiscovery,
	ESubmitPrintJob,
	ECancelPrintJob, // 10
	EContinuePrintJob,
	ERemoveCachedPrinter,
	EGetJobTemplateIcon,
	EGetNumPreviewPages,
	ESetJobSetting,
	EGetJobSetting,
	EGetPrinterCapability,
	EGetPrinterCapId,
	EGetPrinterCapType,
	EGetPrinterCapDef, // 20
	EGetPrinterCapLow,
	EGetPrinterCapHigh,
	EGetPrinterCapEnumCount,
	EGetPrinterCapEnum,
	EGetPrinterCapabilityIDsCount,
	EGetPrinterCapabilityID,
	ERegisterIdleObserver,
	ECancelRegisterIdleObserver,
	EStartDiscovery,
	EContinueDiscovery, // 30
	EPrepareJob,
	ECreateJob,
	EContinueCreateJob,
	ECancelStartDiscovery,
	ECancelCreateJob,
	EIsPictBridgeMode,
	ESetNumberOfCopiesCount,
	ESetNumberOfCopies // 38
	};

#endif// IMAGEPRINTCLIENTSERVER_H

//  End of File
