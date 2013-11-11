/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Common constants for UPnP printing
*
*/


#ifndef UPCONSTS_H
#define UPCONSTS_H

#include <e32def.h>
#include <e32std.h>

#include "printmessagecodes.h"

// Debugging
#define _DEBUG_OUT

#define KUPnPIdBase 402216
typedef TBuf8<KMaxFileName> TFileName8;

// UPnP setting names
_LIT8(KUPnPPrintQuality, 	"PrintQuality");
_LIT8(KUPnPNumberUp, 		"NumberUp");
_LIT8(KUPnPMediaSize, 		"MediaSize");

// UPnP values
_LIT8(KUPnPDeviceSetting, 	"device-setting");
_LIT8(KUPnPOne, 			"1");
#define KUPnPDeviceSettingStrLength 15

// Actions
_LIT8( KCancelJob,				"CancelJob" );
_LIT8( KCreateJob,				"CreateJob" );
_LIT8( KCreateJobV2,			"CreateJobV2" );
_LIT8( KCreateURIJob,			"CreateURIJob" );
_LIT8( KGetJobAttr, 			"GetJobAttributes" );
_LIT8( KGetMargins,		 		"GetMargins" );
_LIT8( KGetMediaList,		 	"GetMediaList" );
_LIT8( KGetPrinterAttr, 		"GetPrinterAttributes" );
_LIT8( KGetPrinterAttrV2, 		"GetPrinterAttributesV2" );

// Arguments
_LIT8( KJobId,					"JobId" );
_LIT8( KJobIdList,				"JobIdList" );
_LIT8( KJobName,				"JobName" );
_LIT8( KUserName,				"JobOriginatingUserName" );
_LIT8( KCopies,					"Copies" );
_LIT8( KSides,					"Sides" );
_LIT8( KNumberUp,				"NumberUp" );
_LIT8( KOrientationRequest,		"OrientationRequested" );
_LIT8( KSize,					"MediaSize" );
_LIT8( KType,					"MediaType" );
_LIT8( KQuality,				"PrintQuality" );
_LIT8( KDataSink,				"DataSink");
_LIT8( KCriticalAttributes,		"CriticalAttributesList" );
//_LIT8( KCriticalAttributes,		"A_ARG_TYPE_CriticalAttribList" );
_LIT8( KDocFormat,				"DocumentFormat" );
_LIT8( KSourceURI,				"SourceURI" );
_LIT8( KSheetsCompleted, 		"JobMediaSheetsCompleted" );
_LIT8( KPrinterState,			"PrinterState" );
_LIT8( KPrinterStateReasons,	"PrinterStateReasons" );
_LIT8( KInternetConnectState,	"InternetConnectState" );
_LIT8( KJobAbortState,			"JobAbortState" );
_LIT8( KJobEndState,			"JobEndState" );
_LIT8( KContentCompleteList,	"ContentCompleteList" );

_LIT8( KPrintBasic,				"PrintBasic:1" );
_LIT8( KPrintEnhanced,			"PrintEnhanced:1" );
_LIT8( KJobIdErr, 				"-1");

// Job state
_LIT8(KUPnPJobStateSuccessful, 	"successful");
_LIT8(KUPnPJobStateAborted, 	"aborted");
_LIT8(KUPnPJobStateCanceled,	"canceled");

// Printer states
_LIT8(KUPnPPrinterStateIdle, 		"idle");
_LIT8(KUPnPPrinterStateProcessing,	"processing" );
_LIT8(KUPnPPrinterStateStopped,		"stopped");

enum TUPnPState
{
	EJobStateSuccessful = 0,
	EJobStateAborted,
	EJobStateCanceled,
	EPrinterStateIdle,
	EPrinterStateProcessing,
	EPrinterStateStopped,
	EStatePrinting,
	EStateReady,
	EStatePending,
	EStateUnknown
};

//UPnP PrinterStateReasons
_LIT8(KUPnPStateNone, 					"none");
_LIT8(KUPnPStateAttentionRequired, 		"attention-required");
_LIT8(KUPnPStateMediaJam, 				"media-jam");
_LIT8(KUPnPStatePaused, 				"paused");
_LIT8(KUPnPStateDoorOpen, 				"door-open");
_LIT8(KUPnPStateMediaLow, 				"media-low");
_LIT8(KUPnPStateMediaEmpty, 			"media-empty");
_LIT8(KUPnPStateOutputAreaAlmostFull,	"output-area-almost-full");
_LIT8(KUPnPStateOutputAreaFull, 		"output-area-full");
_LIT8(KUPnPStateMarkerSupplyLow, 		"marker-supply-low");
_LIT8(KUPnPStateMarkerSupplyEmpty, 		"marker-supply-empty");
_LIT8(KUPnPStateMarkerFailure, 			"marker-failure");
_LIT8(KUPnPStateMediaChangeRequest, 	"media-change-request");
_LIT8(KUPnPStateVendorDefined, 			"vendor-defined");
_LIT8(KUPnPJobStateHwError, 			"hardware-error");
_LIT8(KUPnPJobStateUriNotFound, 		"external-access-uri-not-found");
_LIT8(KUPnPJobStateObjectFailure, 		"external-access-object-failure");
_LIT8(KUPnPJobStateFormatError, 		"external-access-doc-format-error");
_LIT8(KUPnPJobStateHttpError, 			"external-access-http-error");
_LIT8(KUPnPJobStateCopies, 				"copies");
_LIT8(KUPnPJobStateSides, 				"sides");
_LIT8(KUPnPJobStateNumberUp, 			"number-up");
_LIT8(KUPnPJobStateOrientationRequested,"orientation-requested");
_LIT8(KUPnPJobStateMediaSize, 			"media-size");
_LIT8(KUPnPJobStateMediaType, 			"media-type");
_LIT8(KUPnPJobStatePrintQuality, 		"print-quality");
_LIT8(KUPnPJobStateTextLayout, 			"text-layout");
_LIT8(KUPnPJobStateImageLayout, 		"image-layout");
_LIT8(KUPnPJobStateImageOrientation, 	"image-orientation");
_LIT8(KUPnPJobStatePdlFidelity, 		"pdl-fidelity");
_LIT8(KUPnPJobStateFontFamily, 			"font-family");
_LIT8(KUPnPJobStateFontSize, 			"font-size");


// Copies
#define KMaxCopiesLength 3

// Sides
_LIT8(KUPnPOneSided,	 		"one-sided");
_LIT8(KUPnPTwoSidedLongEdge,	"two-sided-long-edge");
_LIT8(KUPnPTwoSidedShortEdge,	"two-sided-short-edge");

// Number up
_LIT8(KUPnP1Up,	"1");
_LIT8(KUPnP2Up,	"2");
_LIT8(KUPnP4Up,	"4");

// Orientations
_LIT8(KUPnPPortrait,		"portrait");
_LIT8(KUPnPLandscape,		"landscape");
_LIT8(KUPnPReversePortrait,	"reverse-portrait");
_LIT8(KUPnPReverseLandscape,"reverse-landscape");

#define KUPnP270Deg 270;
#define KUPnPZeroDeg 0;
#define KUPnP90Deg 90;
#define KUPnP180Deg 180;

// Media sizes
_LIT8(KUPnPNaLetterMediaSize,	"na_letter_8.5x11in");
_LIT8(KUPnPNaLegalMediaSize, 	"na_legal_8.5x14in");
_LIT8(KUPnPIsoA4MediaSize, 		"iso_a4_210x297mm");
_LIT8(KUPnPIsoC5MediaSize, 		"iso_c5_162x229mm");
_LIT8(KUPnPIsoD1MediaSize, 		"iso_d1_110x220mm");
_LIT8(KUPnPJisB4MediaSize, 		"jis_b4_257x364mm");
_LIT8(KUPnPCustomXMmMediaSize, 	"custom_xxx_IIIxJJJmm");
_LIT8(KUPnPCustomXInMediaSize, 	"custom_xxx_IIIxJJJin");
_LIT8(KUPnPCustomMinMmMediaSize,"custom_min_xxx_IIIxJJJmm");
_LIT8(KUPnPCustomMaxInMediaSize,"custom_max_xxx_IIIxJJJin");

// Custom sizes
_LIT8(KUPnPCustom4x6MediaSize, 	"custom_xxx_4x6in");

// Media types
_LIT8(KUPnPStationeryMediaType,			"stationery");
_LIT8(KUPnPStationeryInkjetMediaType,	"stationery-inkjet");
_LIT8(KUPnPTransparencyMediaType,		"transparency");
_LIT8(KUPnPEnvelopeMediaType,			"envelope");
_LIT8(KUPnPLabelsMediaType,				"labels");
_LIT8(KUPnPPhotographicMediaType,		"photographic");
_LIT8(KUPnPCardstockMediaType,			"cardstock");

// Print quality
_LIT8(KUPnPNormalPrintQuality,	"normal");
_LIT8(KUPnPDraftPrintQuality,	"draft");
_LIT8(KUPnPHighPrintQuality,	"high");

// Misc
_LIT8(KUPnPDocFormat, 			"application/xhtml-print");
_LIT8(KUPnPUserName, 			"UPnP Print"); // Use instead the KUPnPAppFriendlyName name from central repository
_LIT( KUPnPDataPath, 			"c:\\system\\data\\upnp\\" );
_LIT( KUPnPXhtmlPath, 			"c:\\system\\data\\upnp\\xhtml\\" );
_LIT8(KHttpPrefix,				"http://");
_LIT8(KUPnPLineFeed8, 			"\n");
_LIT( KUPnPLineFeed, 			"\n");
_LIT8(KUPnPComma,				",");
_LIT8(KUPnPColon,				":");
_LIT8(KUPnPSlash,				"/");

// Upnp item type
_LIT8(KImageType, "object.item.imageItem");
_LIT8(KTextType, "object.item.textItem");

// Mime type
_LIT8(KJpegMime, "image/jpeg");
_LIT8(KHtmlMime, "text/html");

#define KMaxIdLength 		10

// Print service
_LIT8(KUPnPServicePrint, "urn:schemas-upnp-org:service:PrintBasic:1");

/*
* @class 		TMapError
* @description	This class contains the mapping for a single error 
				string / ImagePrint error code.
*/
class TMapError
{
	public:	
		// @var iErrorCode		ImagePrint error codes
		TPrintMessageCodes	iErrorCode;

		// @var iErrorString	UPnP specific error string
		TPtrC8	iErrorString;
};

#endif // UPCONSTS_H

//  End of File
