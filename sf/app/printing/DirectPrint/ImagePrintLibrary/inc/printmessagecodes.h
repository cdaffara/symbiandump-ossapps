/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Contains the TPrintErrorMessageCodes and TPrintMessageCodes enumerations.
*
*/


#ifndef PRINTMESSAGECODES_H
#define PRINTMESSAGECODES_H

/// Global printing Error Message Codes.
// All supported by UI
enum TPrintErrorMessageCodes
{
	EObexConnectError = -3000,
	EObexSendError = -3001,
	EObexDisconnectError = -3002,
	EObexGeneralError = -3003,
};

/// Global printing Message Codes.
enum TPrintMessageCodes
{
	// No message
	ENoMessage = 0,

	// Print Progress Codes
	EJobPending = 3100,
	EPrinting = 3101,
	ECopying = 3102,
	ECancelling = 3103,
	ECancellingNoMessage = 3104,

	// Pict Bridge
	EPbOutOfPaper = 3300,	///<@deprecated Use TPrintMessageCodes::EPbStatusErrorReasonPaperEmpty
	EPbCheckPrinter = 3301,
	EPbPrinterIsDisconnected = 3302,
	EPbPrinterIsConnected = 3303,
	EPbFileNotSupported = 3304,
	EPbFileNotFound = 3305,

	EPbStatusErrorReasonNoReason,	/*3306*/ ///<@brief No reason
	EPbStatusErrorReasonPaper,	///<@brief Paper-related error
	EPbStatusErrorReasonInk,	///<@brief Ink-related error
	EPbStatusErrorReasonHardware,	///<@brief Hardware-related error
	EPbStatusErrorReasonFile,	///<@brief File-related error
	EPbStatusErrorReasonPaperEmpty,	/*3311*/ ///<@brief Paper is empty
	EPbStatusErrorReasonPaperLoad,	///<@brief Load error - paper not loaded correctly
	EPbStatusErrorReasonPaperEject,	///<@brief Eject error - paper not ejected correctly
	EPbStatusErrorReasonPaperMedia,	///<@brief Media error - paper is not detected, paper skewed, roll paper connot be cut or torn-off
	EPbStatusErrorReasonPaperJam,	///<@brief Paper jam
	EPbStatusErrorReasonPaperNearlyEmpty,	///<@brief Paper nearly empty
	EPbStatusErrorReasonPaperCombination,	///<@brief Paper type / paper size combination not supported, or paper found in printer does not match request
	EPbStatusErrorReasonInkEmpty,	/*18*/ ///<@brief Ink empty
	EPbStatusErrorReasonInkLow,	///<@brief Ink low
	EPbStatusErrorReasonInkWaste,	///<@brief Waste ink error (waste-ink reservior full).
	EPbStatusErrorReasonHardwareFatal,	/*21*/ ///<@brief Fatal error
	EPbStatusErrorReasonWarning,        // warning
	EPbStatusErrorReasonHardwareServiceCall,	///<@brief Service call
	EPbStatusErrorReasonHardwarePrinterUnavailable,	///<@brief Printer unavailable (performing maintenance or otherwise occupied)
	EPbStatusErrorReasonHardwarePrinterBusy,	///<@brief Printer busy (processing a job from another source, e.g. a PC)
	EPbStatusErrorReasonHardwareLever,	///<@brief Lever error (lever position not correct)
	EPbStatusErrorReasonHardwareCoverOpen,	///<@brief Cover open
	EPbStatusErrorReasonHardwareNoMarkingAgent,	///<@brief No marking agent (e.g. missing printer head)
	EPbStatusErrorReasonHardwareInkCoverOpen,	///<@brief Ink cover open
	EPbStatusErrorReasonHardwareNoInkCartridge,	///<@brief No ink cartridge
	EPbStatusErrorReasonFilePrintInfo,	/*31*/ ///<@brief printInfo error (specified &lt;printInfo&gt; is too large).
	EPbStatusErrorReasonFileFileDecode,	///<@brief File decode error (picture format of specified image cannot be decoded).

	// Printer status reasons
	EPrintReasonAttentionRequired, /*33*/ ///<@brief Indicates reason other than those listed
	EPrintReasonPaused, ///<@brief Someone has paused the printer and the PrinterState is "stopped". In this state, a Printer may not produce printed output.
	EPrintReasonOutputAreaAlmostFull, ///<@brief One or more output area is almost full: e.g., tray, stacker, collator.
	EPrintReasonOutputAreaFull, ///<@brief One or more output area is full: e.g., tray, stacker, collator.
	EPrintReasonMarkerSupplyLow, ///<@brief The device is low on at least one marker supply: e.g., toner, ink, ribbon.
	EPrintReasonMarkerSupplyEmpty, ///<@brief The device is out of at least one marker supply: e.g., toner, ink, ribbon.
	EPrintReasonMarkerFailure, ///<@brief The device has at least one marking device which has failed and requires service or replacement: e.g., pen.

	// OBEX response codes
	EObexReserved, ///<@brief Reserved
	EObexContinue, ///<@brief Continue
	EObexOKSuccess, ///<@brief OK, Success
	EObexCreated, ///<@brief Created
	EObexAccepted, ///<@brief Accepted
	EObexNonAuthoritativeInformation, ///<@brief Non-Authoritative Information
	EObexNoContent, ///<@brief No Content
	EObexResetContent, ///<@brief Reset Content
	EObexPartialContent, ///<@brief Partial Content
	EObexMultipleChoices, ///<@brief Multiple Choices
	EObexMovedPermanently, ///<@brief Moved Permanently
	EObexMovedTemporarily, ///<@brief Moved Temporarily
	EObexSeeOther, ///<@brief See Other
	EObexNotModified, ///<@brief Not Modified
	EObexUseProxy, ///<@brief Use Proxy
	EObexBadRequest, ///<@brief Bad Request - server could not understand the request
	EObexUnauthorized, ///<@brief Unauthorized
	EObexPaymentRequired, ///<@brief Payment required
	EObexForbidden, ///<@brief Forbidden: operation is understood but refused
	EObexNotFound, ///<@brief Not found DPS: Object or File does not exist.
	EObexMethodNotAllowed, ///<@brief Method not allowed
	EObexNotAcceptable, ///<@brief Not acceptable
	EObexProxyAuthRequired, ///<@brief Proxy authentication is required
	EObexRequestTimedOut, ///<@brief Request timed out
	EObexConflict, ///<@brief Conflict
	EObexGone, ///<@brief Gone
	EObexLengthRequired, ///<@brief Length Required
	EObexPreconditionFailed, ///<@brief Precondition failed
	EObexRequestedEntityTooLarge, ///<@brief Requested entity is too large
	EObexRequestedURLTooLarge, ///<@brief Requested URL is too large
	EObexUnsupportedMediaType, ///<@brief Unsupported media type. Note: Refers to MIME media-type
	EObexInternalReceiverError, ///<@brief Internal receiver error
	EObexNotImplemented, ///<@brief Not implemented
	EObexBadGateway, ///<@brief Bad Gateway
	EObexServiceUnavailable, ///<@brief Service Unavailable
	EObexGatewayTimeout, ///<@brief Gateway Timeout
	EObexHTTPVersionNotSupported, ///<@brief HTTP protocol version is not supported
	EObexDatabaseFull, ///<@brief Database Full
	EObexDatabaseLocked, ///<@brief Database Locked
	
	EKErrIrObexRespTimedOut = -5572,	// OBEX command timed out
	
};

enum TOperationStatusCodes
{
	EOperationStatusOk = 0x0000,		///<@brief successful-ok
	EOperationStatusOkIgnoredOrSubstitutedAttributes = 0x0001,		///<@brief successful-ok-ignored-or-substituted-attributes
	EOperationStatusOkConflictingAttributes = 0x0002,		///<@brief successful-ok-conflicting-attributes
	EOperationStatusClientBadRequest = 0x0400,		///<@brief client-error-bad-request
	EOperationStatusClientForbidden = 0x0401,		///<@brief client-error-forbidden
	EOperationStatusClientNotAuthenticated = 0x0402,		///<@brief client-error-not-authenticated
	EOperationStatusClientNotAuthorized = 0x0403,		///<@brief client-error-not-authorized
	EOperationStatusClientNotPossible = 0x0404,		///<@brief client-error-not-possible
	EOperationStatusClientTimeout = 0x0405,		///<@brief client-error-timeout
	EOperationStatusClientNotFound = 0x0406,		///<@brief client-error-not-found
	EOperationStatusClientGone = 0x0407,		///<@brief client-error-gone
	EOperationStatusClientRequestEntityTooLarge = 0x0408,		///<@brief client-error-request-entity-too-large
	EOperationStatusClientRequestValueTooLong = 0x0409,		///<@brief client-error-request-value-too-long
	EOperationStatusClientDocumentFormatNotSupported = 0x040a,		///<@brief client-error-document-format-not-supported
	EOperationStatusClientAttributesOrValuesNotSupported = 0x040b,		///<@brief client-error-attributes-or-values-not-supported
	EOperationStatusClientUriSchemeNotSupported = 0x040c,		///<@brief client-error-uri-scheme-not-supported
	EOperationStatusClientCharsetNotSupported = 0x040d,		///<@brief client-error-charset-not-supported
	EOperationStatusClientConflictingAttributes = 0x040e,		///<@brief client-error-conflicting-attributes
	EOperationStatusClientCompressionNotSupported = 0x040f,		///<@brief client-error-compression-not-supported
	EOperationStatusClientCompression = 0x0410,		///<@brief client-error-compression-error
	EOperationStatusClientDocumentFormat = 0x0411,		///<@brief client-error-document-format-error
	EOperationStatusClientDocumentAccess = 0x0412,		///<@brief client-error-document-access-error
	EOperationStatusClientMediaNotLoaded = 0x0418,		///<@brief client-error-media-not-loaded
	EOperationStatusServerInternalError = 0x0500,		///<@brief server-error-internal-error
	EOperationStatusServerOperation = 0x0501,		///<@brief server-error-operation-not-supported
	EOperationStatusServerServiceUnavailable = 0x0502,		///<@brief server-error-service-unavailable
	EOperationStatusServerVersionNotSupported = 0x0503,		///<@brief server-error-version-not-supported
	EOperationStatusServerDeviceError = 0x0504,		///<@brief server-error-device-error
	EOperationStatusServerTemporaryError = 0x0505,		///<@brief server-error-temporary-error
	EOperationStatusServerNotAcceptingJobs = 0x0506,		///<@brief server-error-not-accepting-jobs
	EOperationStatusServerBusy = 0x0507,		///<@brief server-error-busy
	EOperationStatusServerJobCanceled = 0x0508,		///<@brief server-error-job-canceled
	EOperationStatusServerMultipleDocument = 0x0509		///<@brief server-error-multiple-document-jobs-not-supported
};

#endif  //  PRINTMESSAGECODES_H

//  End of File
