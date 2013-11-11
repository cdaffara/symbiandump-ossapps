/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*     MmsEngine error codes
*
*/



#ifndef MMSERRORS_H
#define MMSERRORS_H


#define MMS_ERROR_BASE -11000 //lint !e1923 this should definitely not be a variable
#define MMS_ERROR_NEW_BASE -32100 // new mms errors base

// Error codes are be defined as enum
enum
    {
    // Unspecified error, used when no defined error matches.
    KMmsGeneralError = MMS_ERROR_BASE,
    // no access points defined
    KMmsErrorNoWAPAccessPoint = MMS_ERROR_BASE - 1,
    // Both WAP access points must have the same URI
    // (both must point to the same MMSC)
    // *** obsolete ***
    KMmsErrorUrisDontMatch = MMS_ERROR_BASE - 2,
    // no IAP for WAP AP 1
    // *** obsolete ***
    KMmsErrorNoIAP1 = MMS_ERROR_BASE - 3,
    // no IAP for WAP AP 2
    // *** obsolete ***
    KMmsErrorNoIAP2 = MMS_ERROR_BASE - 4,
    // access point 1 not valid (something wrong with access point chain)
    // missing URI has own error code, see below
    KMmsErrorAP1Invalid = MMS_ERROR_BASE - 5,
    // access point 2 not valid (something wrong with access point chain)
    // missing URI has own error code, see below
    // *** obsolete ***
    KMmsErrorAP2Invalid = MMS_ERROR_BASE - 6,
    // message is too big to be sent - cannot allocate encode buffer
    KMmsErrorMessageTooBig = MMS_ERROR_BASE - 7,
    // message type invalid - cannot encode
    KMmsErrorUnknownMessageType = MMS_ERROR_BASE - 8,
    // Disk space critical level check failed
    // *** obsolete ***
    KMmsErrorDiskSpaceLow = MMS_ERROR_BASE - 9,

    // error codes that come from the X-mms-response status
    KMmsErrorStatusUnspecified = MMS_ERROR_BASE - 10,
    KMmsErrorStatusServiceDenied = MMS_ERROR_BASE - 11,
    KMmsErrorStatusMessageFormatCorrupt = MMS_ERROR_BASE - 12,
    // sending address unresolved
    KMmsErrorStatusMessageAddressUnresolved = MMS_ERROR_BASE - 13,
    KMmsErrorStatusMessageNotFound = MMS_ERROR_BASE - 14,
    KMmsErrorStatusNetworkProblem = MMS_ERROR_BASE - 15,
    KMmsErrorStatusContentNotAccepted = MMS_ERROR_BASE - 16,
    KMmsErrorStatusUnsupportedMessage = MMS_ERROR_BASE - 17,

    // Mapping of HTTP error codes. Rough categories only
    KMmsErrorHTTPConfiguration = MMS_ERROR_BASE - 18, // 1XX, 3XX
    KMmsErrorHTTPNotFound = MMS_ERROR_BASE - 19, // 4XX
    KMmsErrorHTTPServerDown = MMS_ERROR_BASE - 20, // 5XX

    // Errors from the MMS protocol to WAP mapping layer
    // *** obsolete ***
    KMmsErrorWapStack = MMS_ERROR_BASE - 21,
    KMmsErrorSessionAlreadyOpen = MMS_ERROR_BASE - 22,
    KMmsErrorSessionNotOpen = MMS_ERROR_BASE - 23,
    // *** obsolete ***
    KMmsErrorBearerSuspended = MMS_ERROR_BASE - 24,
    KMmsErrorTimeout = MMS_ERROR_BASE - 25,
    // *** obsolete ***
    KMmsErrorSuspendTimeout = MMS_ERROR_BASE - 26,
    KMmsErrorUnknownRespFromGw = MMS_ERROR_BASE - 27,
    KMmsErrorTransferCancelled = MMS_ERROR_BASE - 28,
    KMmsErrorBufferEmpty = MMS_ERROR_BASE - 29,

    // Could not connect because connection already exists
    // *** obsolete ***
    KMmsErrorConnectionAlreadyActive = MMS_ERROR_BASE - 30,
    // Access point #1 not defined
    // *** obsolete ***
    KMmsErrorNoWapAp1 = MMS_ERROR_BASE - 31,
    // URI 1 missing
    KMmsErrorNoURI1 = MMS_ERROR_BASE - 32,
    // URI 2 missing
    // *** obsolete ***
    KMmsErrorNoURI2 = MMS_ERROR_BASE - 33,

    // we cannot prove we are in home network and mode is "automatic at home"
    // *** obsolete ***
    KMmsErrorNotHomeNetwork = MMS_ERROR_BASE - 34,

    // error returned by validate service if values are
    // outside limits
    KMmsErrorInvalidSettings = MMS_ERROR_BASE - 35,

    // We must have our own definition for MRUExceeded.
    // The actual error code depends on the transport used.
    // The tranport component must map the error and
    // return it to us in this form
    KMmsErrorEMRUExceeded = MMS_ERROR_BASE - 36,

    // Error code that corresponds to Symbian OS error -34
    // (KErrCouldNotConnect)
    // This is for UI use, mmsengine does not remap Symbian
    // error codes.
    KMmsErrorCouldNotConnect = MMS_ERROR_BASE - 37,

    // Error code means that the DRM message is removed
    // from the MMS message as the handling of the DRM message has failed.
    // This error code is not shown to the end user.
    KMmsErrorRemoveDRM = MMS_ERROR_BASE - 38,

    // New error codes for mapping MMS encapsulation 1.1 errors
    // These errors correspond to X-Mms-Response_Status errors from MMSC
    // These are all permanent errors
    KMmsErrorStatusReplyChargingLimitationsNotMet = MMS_ERROR_BASE - 39,
    KMmsErrorStatusReplyChargingRequestNotAccepted = MMS_ERROR_BASE - 40,
    KMmsErrorStatusReplyChargingForwardingDenied = MMS_ERROR_BASE - 41,
    KMmsErrorStatusReplyChargingNotSupported = MMS_ERROR_BASE - 42,
    KMmsErrorStatusContentUnsupported = MMS_ERROR_BASE - 43,
    // Sending address unresolved
    KMmsErrorStatusTransientAddressUnresolved = MMS_ERROR_BASE - 44,
    KMmsErrorStatusTransientMessageNotFound = MMS_ERROR_BASE - 45,
    // Generic values for unspecified permanent and transient errors
    // These are used when error codes ouside known ranges are obtained
    KMmsErrorStatusPermanentFailure = MMS_ERROR_BASE - 46,
    KMmsErrorStatusTransientFailure = MMS_ERROR_BASE - 47,

    // This is not actually an error. It means that a message sending
    // or receiving has been suspended because of offline mode, and
    // it should be resent/refetched as soon as phone is switched to
    // online mode again.
    KMmsErrorOfflineMode = MMS_ERROR_BASE - 48,

    // Mapping of HTTP error codes.
    // (addition to previous HTTP error mappings)
    KMmsErrorHTTPClientError = MMS_ERROR_BASE - 49,

    // Sending is prohibited because the message contains some closed content
    KMmsErrorProtectedContent = MMS_ERROR_NEW_BASE - 50,
    
    // Error codes for JSR248 application message handling:
    
    // The incoming message was addressed to an application that has not been
    // registered. The message has been discarded (error visible only in the 
    // notification that is in the inbox in manual mode)
   	KMmsErrorUnregisteredApplication = MMS_ERROR_NEW_BASE - 51,
   	// The incoming message was addressed to an application.
   	// There is not enough room on the disk to store the message.
   	// However, there are enough old messages addressed to the same application
   	// so that enough room will be freed by deleting some or all of the old
   	// messages by MMS engine.
   	// In automatic mode the retrieval is automatically rescheduled.
   	// In manual mode the error code appears in the notification in inbox
	KMmsErrorApplicationDiskFull = MMS_ERROR_NEW_BASE - 52

    };

#endif      // MMSERRORS_H

// End of File
