/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Common constants and other definitions needed by several classes
*
*/



#ifndef MMSSERVERCOMMON_H
#define MMSSERVERCOMMON_H

#ifdef _DEBUG
#undef _NO_MMSS_LOGGING_
#endif

//  INCLUDES
#include <e32base.h>
#include <msvstd.h>
#include "mmsversion.h"  // current MMS encapsulation version

#ifndef _NO_MMSS_LOGGING_
// c:\logs\mmss
_LIT( KMmsDefaultLogDirectory, "\x43:\\logs\\mmss\\" ); 
class TMmsLogger
    {
    public:
        static void Log(TRefByValue<const TDesC> aFmt,...);
    };
#endif

#define KMMSMAXTIDLENGTH 16  //This cannot be made a variable, must be define

// CONSTANTS
const TUint8 KMmsMajorVersionMask = 0x70; // binary 0111 0000
const TUint8 KMmsMinorVersionMask = 0x0F; // binary 0000 1111

// Symbian's CMsgActive objects are run with slightly higher priority than
// normal (i.e. '1'). Mms follows this principle in the most important AOs.
const TInt KMmsActiveObjectPriority = CActive::EPriorityStandard + 1;

// buffer size for chunked encoding - exact size will be adjusted later
// small size used for tests
const TInt KMmsChunkedBufferSize = 10*1024;
//use value 1024 to test tiny chunks

// Assigned numbers and assigned values for MMS headers
// assigned numbers, enoced as short integers
const TUint8 KMmsAssignedBcc = 0x01 | 0x80;
const TUint8 KMmsAssignedCc  = 0x02 | 0x80;
const TUint8 KMmsAssignedContentLocation = 0x03 | 0x80;
const TUint8 KMmsAssignedContentType = 0x04 | 0x80;
const TUint8 KMmsAssignedDate = 0x05 | 0x80;
const TUint8 KMmsAssignedDeliveryReport = 0x06 | 0x80;
const TUint8 KMmsAssignedDeliveryTime = 0x07 | 0x80;
const TUint8 KMmsAssignedExpiry = 0x08 | 0x80;
const TUint8 KMmsAssignedFrom = 0x09 | 0x80;
const TUint8 KMmsAssignedMessageClass = 0x0A | 0x80;
const TUint8 KMmsAssignedMessageId = 0x0B | 0x80;
const TUint8 KMmsAssignedMessageType = 0x0C | 0x80;
const TUint8 KMmsAssignedMmsVersion = 0x0D | 0x80;
const TUint8 KMmsAssignedMessageSize = 0x0E | 0x80;
const TUint8 KMmsAssignedPriority = 0x0F | 0x80;
const TUint8 KMmsAssignedReadReply = 0x10 | 0x80;
const TUint8 KMmsAssignedReportAllowed = 0x11 | 0x80;
const TUint8 KMmsAssignedResponseStatus = 0x12 | 0x80;
const TUint8 KMmsAssignedResponseText = 0x13 | 0x80;
const TUint8 KMmsAssignedSenderVisibility = 0x14 | 0x80;
const TUint8 KMmsAssignedStatus = 0x15 | 0x80;
const TUint8 KMmsAssignedSubject = 0x16 | 0x80;
const TUint8 KMmsAssignedTo = 0x17 | 0x80;
const TUint8 KMmsAssignedTID = 0x18 | 0x80;
// new for MMS encapsulation version 1.1
const TUint8 KMmsAssignedRetrieveStatus = 0x19 | 0x80;
const TUint8 KMmsAssignedRetrieveText = 0x1A | 0x80;
const TUint8 KMmsAssignedReadStatus = 0x1B | 0x80;
const TUint8 KMmsAssignedReplyCharging = 0x1C | 0x80;
const TUint8 KMmsAssignedReplyChargingDeadline = 0x1D | 0x80;
const TUint8 KMmsAssignedReplyChargingID = 0x1E | 0x80;
const TUint8 KMmsAssignedReplyChargingSize = 0x1F | 0x80;
const TUint8 KMmsAssignedPreviouslySentBy = 0x20 | 0x80;
const TUint8 KMmsAssignedPreviouslySentDate = 0x21 | 0x80;
// new for MMS encapsulation version 1.2
const TUint8 KMmsAssignedMmsStore = 0x22 | 0x80; // store to Mmbox
const TUint8 KMmsAssignedMMState = 0x23 | 0x80;
const TUint8 KMmsAssignedMMFlags = 0x24 | 0x80;
const TUint8 KMmsAssignedStoreStatus = 0x25 | 0x80;
const TUint8 KMmsAssignedStoreStatusText = 0x26 | 0x80;
const TUint8 KMmsAssignedStored = 0x27 | 0x80;
const TUint8 KMmsAssignedAttributes = 0x28 | 0x80;
const TUint8 KMmsAssignedTotals = 0x29 | 0x80;
const TUint8 KMmsAssignedMboxTotals = 0x2A | 0x80;
const TUint8 KMmsAssignedQuotas = 0x2B | 0x80;
const TUint8 KMmsAssignedMboxQuotas = 0x2C | 0x80;
const TUint8 KMmsAssignedMessageCount = 0x2D | 0x80;
const TUint8 KMmsAssignedContent = 0x2E | 0x80;
const TUint8 KMmsAssignedStart = 0x2F | 0x80;
const TUint8 KMmsAssignedAdditionalHeaders = 0x30 | 0x80;
const TUint8 KMmsAssignedDistributionIndicator = 0x31 | 0x80;
const TUint8 KMmsAssignedElementDescriptor = 0x32 | 0x80;
const TUint8 KMmsAssignedLimit = 0x33 | 0x80;
// new for MMS encapsulation version 1.3
const TUint8 KMmsAssignedRecommendedRetrievalMode = 0x34 | 0x80;
const TUint8 KMmsAssignedRecommendedRetrievalModeText = 0x35 | 0x80;
const TUint8 KMmsAssignedStatusText = 0x36 | 0x80;
const TUint8 KMmsAssignedApplicId = 0x37 | 0x80;
const TUint8 KMmsAssignedReplyApplicId = 0x38 | 0x80;
const TUint8 KMmsAssignedAuxApplicInfo = 0x39 | 0x80;
const TUint8 KMmsAssignedContentClass = 0x3A | 0x80;
const TUint8 KMmsAssignedDrmContent = 0x3B | 0x80;
const TUint8 KMmsAssignedAdaptationAllowed = 0x3C | 0x80;
const TUint8 KMmsAssignedReplaceId = 0x3D | 0x80;
const TUint8 KMmsAssignedCancelId = 0x3E | 0x80;
const TUint8 KMmsAssignedCancelStatus = 0x3F | 0x80;


// Assigned numbers for application specific headers
// these are not defined in the standard, but we define them
// internally to avoid using text strings all around
// We start allocation from top down to avoid overlap as
// long as possible
// We leave out the code that produces -1, as that is the mark
// of an unrecognized header in our code.

const TUint8 KMmsAssignedExtendedNotificationText = 0x7E | 0x80;
const TUint8 KMmsAssignedExtendedNotificationEOL = 0x7D | 0x80;

// Parameter name assignment
// since MMS encapsulation version 1.2
const TUint8 KMmsAssignedTopLevelContentType = 0x02 | 0x80;

// Header encoding

const TUint8 KMmsAbsoluteToken = 128;
const TUint8 KMmsRelativeToken = 129;

const TUint8 KMmsAddressPresentToken = 128;
const TUint8 KMmsInsertAddressToken = 129;

// The field assignements that are used in internal storage are in mmsconst.h
// The codes in internal storage are the same as those used in actual decoding

// status codes for X-Mms-Response-Status
// X-Mms-Retrieve-Status has different mapping.
const TUint8 KMmsStatusOk = 128;
const TUint8 KMmsErrorUnspecified = 129;
const TUint8 KMmsErrorServiceDenied = 130;
const TUint8 KMmsErrorMessageFormatCorrupt = 131;
const TUint8 KMmsErrorSendingAddressUnresolved = 132;
const TUint8 KMmsErrorMessageNotFound = 133;
const TUint8 KMmsErrorNetworkProblem = 134;
const TUint8 KMmsErrorNoContentAccepted = 135;
const TUint8 KMmsErrorUnsupportedMessage = 136;
// new status codes for encapsulation version 1.1
// transient errors
const TUint8 KMmsErrorTransientFailure = 192; // 0xC0
const TUint8 KMmsErrorTransientSendingAddressUnresolved = 193;
const TUint8 KMmsErrorTransientMessageNotFound = 194;
const TUint8 KMmsErrorTransientNetworkProblem = 195;
const TUint8 KMmsErrorTransientPartialSuccess = 196; // since enc 1.2
// permanent errors
const TUint8 KMmsErrorPermanentFailure = 224; // 0xE0
const TUint8 KMmsErrorPermanentServiceDenied = 225;
const TUint8 KMmsErrorPermanentMessageFormatCorrupt = 226;
const TUint8 KMmsErrorPermanentSendingAddressUnresolved = 227;
const TUint8 KMmsErrorPermanentMessageNotFound = 228;
const TUint8 KMmsErrorPermanentContentNotAccepted = 229;
const TUint8 KMmsErrorPermanentReplyChargingLimitationsNotMet = 230;
const TUint8 KMmsErrorPermanentReplyChargingRequestNotAccepted = 231;
const TUint8 KMmsErrorPermanentReplyChargingForwardingDenied = 232;
const TUint8 KMmsErrorPermanentReplyChargingNotSupported = 233;
const TUint8 KMmsErrorPermanentAddressHidingNotSupported = 234; // since enc 1.2
const TUint8 KMmsErrorPermanentLackOfPrepaid = 235; // since enc. 1.3

// status codes for X-Mms-Response-Status
// KMmsStatusOk: 128; // same as for X-Mms-Response-Status
// KMmsErrorTransientFailure:  // 0xC0 // same as for X-Mms-Response-Status
const TUint8 KMmsErrorReceiveTransientMessageNotFound = 193;
const TUint8 KMmsErrorReceiveTransientNetworkProblem = 194;
// KMmsErrorPermanentFailure: 224; // 0xE0 // same as for X-Mms-Response-Status
// KMmsErrorPermanentServiceDenied: 225; // same as for X-Mms-Response-Status
const TUint8 KMmsErrorReceivePermanentMessageNotFound = 226;
const TUint8 KMmsErrorReceivePermanentContentUnsupported = 227;

// status codes for X-Mms-Store-Status
// KMmsStatusOk: 128; // same as for X-Mms-Response-Status
// KMmsErrorTransientFailure: 192; // 0xC0 // same as for X-Mms-Response-Status
const TUint8 KMmsErrorStoreStatusTransientNetworkProblem = 193;
// KMmsErrorPermanentFailure: 224; // 0xE0 // same as for X-Mms-Response-Status
// KMmsErrorPermanentServiceDenied: 225; // same as for X-Mms-Response-Status
// KMmsErrorPermanentMessageFormatCorrupt: 226; // same as for X-Mms-Response-Status
const TUint8 KMmsErrorStoreStatusPermanentMessageNotFound = 227;
const TUint8 KMmsErrorStoreStatusPermanentMmboxFull = 228;

// masks for separating transient and permanent ranges
const TUint8 KMmsErrorRangeMask = 0x60; // 0110 0000
const TUint8 KMmsErrorTransient = 0x40; // 0010 0000 // all transient errors have this bit set
const TUint8 KMmsErrorPermanent = 0x60; // 0110 0000 // all permanent errors have these bits set

// misc bytes needed in encoding/decoding
const TUint8 KMmsQuote = 127;
const TUint8 KMmsNull = 0;

_LIT( KMiuMau, "@" );
_LIT8( KMmsPlmn, "/TYPE=PLMN" );
_LIT8( KMmsIpv4, "/TYPE=IPv4" );
_LIT8( KMmsIpv6, "/TYPE=IPv6" );

_LIT16( KMmsPlmnu, "/TYPE=PLMN" );
_LIT16( KMmsIpv4u, "/TYPE=IPv4" );
_LIT16( KMmsIpv6u, "/TYPE=IPv6" );

const TInt KMmsPlmnLength = 10;
const TInt KMmsIpv4Length = 10;
const TInt KMmsIPv6Length = 10;

const TUint8 KMmsLengthQuote = 31;
const TUint8 KMmsStringQuote = 34;

// Mime content types and their assignments are defined in mmsconst.h

// WSP well-known header field name assingments
const TUint8 KWspContentLocation = 0x0E;
const TUint8 KWspContentDisposition = 0x2E;
// WSP well-known parameter assignments
const TUint8 KWspQValue = 0x00; // value encoded as Uintvar.
const TUint8 KWspCharset = 0x01;
const TUint8 KWspName = 0x05;
const TUint8 KWspFileName = 0x06;
// parameters for multipart/related content type
const TUint8 KWspRelatedType = 0x09;
const TUint8 KWspStart = 0x0A;
const TUint8 KWspStartInfo = 0x0B;
const TUint8 KWspContentId = 0x40;
const TUint8 KWspAttachment = 0x01;
const TUint8 KWspInline = 0x02; // WSP 1.4 Only decode, never encode
_LIT8( KWspContentIdString, "Content-ID" );
_LIT8( KWspNameString, "Name" );
_LIT8( KWspFilenameString, "Filename" );
_LIT8( KWspProfileString, "Profile" );
_LIT8( KWspAttachmentString, "Attachment" );
_LIT8( KWspInlineString, "Inline" );
_LIT8( KHTTPUtf8, "utf-8" );
_LIT8( KHTTPEnglish, "en" );
_LIT8( KCharset, "charset" );
_LIT8( KMmsSeparateDeliveryOmaXHeader, "X-Oma-Drm-Separate-Delivery" );

// WSP headers for transport
// Accept: */*, Accept: application/vnd.wap.mms-message, Accept: application/vnd.wap.sic, Accept-Language: en, Accept-Charset: utf-8
_LIT8( KMmsClientHeaders, "\x080\x080\x080\x061pplication/vnd.wap.mms-message\x000\x080\x061pplication/vnd.wap.sic\x000\x083\x099\x081\x0EA" );
// static UAProf string not used: if UAProf not found in .ini file, the header is omitted
_LIT8( KMmsUAProf, "\x0B5");

// Null must be separate to support HTTP transpot, too
_LIT8( KMmsMessageContentType, "application/vnd.wap.mms-message" );
// Some gateway wants this before it is willing to send mms notifications
_LIT8( KMmsWapSicContentType, "application/vnd.wap.sic" );
_LIT8( KMmsWspHeaderUserAgent,"\x0A9" ); // assigned number only
// Useragent is read from .ini file. If not found, old static string used
_LIT8( KMmsDefaultUserAgent,"Nokia6630/1.0 Series60/2.6 Profile/MIDP-2.0 Configuration/CLDC-1.1" );

// DATA TYPES

// This is the state that will be used to indicate that
// network operations are not allowed ( sending and receiving are suspended )
// This must be one of the states defined in TMsvSendState enum list
const TUint KMmsOffLineState = KMsvSendStateSuspended;

const TUint16 KMmsPushId = 4; // this is our application id

const TInt KMMSBufferExtra = 1024; // reserve a bit extra to allow inaccuracy in message size
const TInt KMMSAttachmentExtra = 50; // extra for attachment headers.

// The scheduling works without delays
#ifdef __WINS__
const TInt KMmsDelayInSeconds = 0;
#else
const TInt KMmsDelayInSeconds = 0;
#endif //  __WINS__

// This is more than what we need, but we are cautious
const TInt KExtraSpaceForDirectionalityMarkers = 6;

// Garbage collection reason flags
// phone booted (or mmswatcher started for some other reason)
const TUint32 KMmsReasonBoot = 0x00000001;
// message store location has changed
const TUint32 KMmsReasonMessageStoreChanged = 0x00000002;
// network operations have become allowed again (after being forbidden)
const TUint32 KMmsReasonNetworkAllowed = 0x00000004;
// backup/restore operation has ended
const TUint32 KMmsReasonBackupEnded = 0x00000008;
// if media available/media unavailable events have been encountered
// without any other apparent reason, the reason code will be hotswap
const TUint32 KMmsReasonHotswap = 0x00000010;
// User has changed system time
// This event is needed in case the user moves the time too much towards future
// If time is changed too much, task scheduler does not reschedule the task
const TUint32 KMmsReasonEnvironmentTimeChanged = 0x00000020;

const TInt KMmsExtendedMessageComplete = 0x31;

const TInt KMmsMaxLogStringLength = 200;
const TInt KMmsIntegerSize = 32;

const TInt KMmsLowestAscii = 32; // 0x20
const TInt KMmsHighestAscii = 127; // 0x7F

// This is the maximum length for Mms Content location header in conformance specs.
// We use it as maximum filename length, too, so that we always have room for path.
// If we allow filenames up to maximum Symbian buffer length, there will be no room
// for attachment path, and attachment file creation will fail.
const TInt KMmsMaxFileName = 100; 

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

#endif      // MMSSERVERCOMMON_H
            
// End of File
