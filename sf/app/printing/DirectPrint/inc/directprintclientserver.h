/*
* Copyright (c) 2010 Kanrikogaku Kenkyusho, Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Kanrikogaku Kenkyusho, Ltd. - Initial contribution
*
* Contributors:
*
* Description:  
*
*/


// Protection against nested includes
#ifndef DIRECTPRINTCLIENTSERVER_H
#define DIRECTPRINTCLIENTSERVER_H

// System include files
#include <e32std.h>

// User include files
#include "directprintserveruid.h"

_LIT( KDirectPrintServerName, "directprintserver" );
_LIT( KDirectPrintServerImg, "directprintserver" );		// EXE name

const TInt KMaxDirectPrintMessage = 512;
const TInt KDirectPrintServerMajor = 0;
const TInt KDirectPrintServerMinor = 0;
const TInt KDirectPrintServerBuild = 0;

enum TDirectPrintMessages
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
	ESetNumberOfCopies, // 38
	EGetProtocolNamesCount,
	EGetProtocolName
	};

#endif// DIRECTPRINTCLIENTSERVER_H

//  End of File
