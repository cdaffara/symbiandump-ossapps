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
*     Miscellaneous constants needed by MmsEngine and its clients
*     (e.g. MCE and DeviceManagement)
*
*/



#ifndef MMSCONST_H
#define MMSCONST_H

//  INCLUDES
#include <msvuids.h>
#include <msvstd.hrh>
#include <e32std.h>
#include <cmsvrecipientlist.h>

// CONSTANTS

// NOTE: January = 0! 1st day of month = 0!
_LIT( KMmsYear1970String, "19700000:000000.000000" );

// <DEPRECATED>
// This is the resource file that contains factory values
// for error actions (for scheduled sending and receiving)
_LIT(KMmsResourceDriveAndFile, "mmsserver.rsc");
// </DEPRECATED>

_LIT(KMMSNotificationFolder, "MMSNotifications");
_LIT(KMmsMmboxFolder, "MmsMmbox");

// constant for conversion between seconds and microseconds
const TInt KMmsMillion = 1000000;

const TInt KMmsDateBufferLength = 30;

//
// Image size definitions - Not used by MMS engine
// anyone who needs image size constants, should define their own
//
//
// <DEPRECATED>
// 160*120
const TInt KMmsImageSmallWidth = 160;
const TInt KMmsImageSmallHeight = 120;
// 640*480
const TInt KMmsImageLargeWidth = 640;
const TInt KMmsImageLargeHeight = 480;
// Original
const TInt KMmsImageOriginalWidth = KMaxTInt;
const TInt KMmsImageOriginalHeight = KMaxTInt;
// </DEPRECATED>

// This is the extra amount of disk space that will be taken
// up by the index entry when creating new message entries.
// This is a crude estimate. It is defined here as a constant
// to allow it to be made more accurate by changing the value
// in only one place.
const TInt KMmsIndexEntryExtra = 80;

// This is the safety margin for fetching MMS messages.
// There should be at least this much extra disk space
// before a message can be fetched. A new message takes
// more disk space than the amount indicated in the
// notification.
const TInt KMmsDiskSafetyMargin = 25000;

//
// DLL UIDs that might be needed
//
const TUid KUidMmsServerMtm={0x100058DB};
const TUid KUidMmsClientMtm={0x100058DC};
const TUid KUidMmsClientUIMtm={0x100058DD};
const TUid KUidMmsClientUIDataMtm={0x100058DD};
// editor and viewer
const TInt KUidMsgMmsEditor={0x100058DE};
const TInt KUidMsgMmsViewer={0x100058DF};

// message types
const TUid KUidMsgTypeMultimedia={0x100058E1};
const TUid KUidMsvTechnologyGroupMMS={0x100058E2};
const TUid KUidMsgMMSNotification={0x100059C8};

// message subtypes
const TUid KUidMsgSubTypeMmsUpload={0x102072CD};
const TUid KUidMsgSubTypeMmsPostcard={0x102072CE};
const TUid KUidMsgSubTypeMmsAudioMsg={0x1020744F};

//
// Stream UIDs
//
// Globally unique values have been allocated to all streams
// MMS headers
const TUid KUidMmsHeaderStream={0x101FB0ED};
// recipient scheduling info
const TUid KUidMmsRecipientStream={0x101FB0EE};
// Notification or delivery report in binary format
const TUid KUidBinaryNotificationStream={0x101FB0EF};
// element descriptor info, only in notification
const TUid KUidMMsElementDescriptorStream = {0x101FB0F0};
// MMBox headers that can be present in several message PDUs
const TUid KUidMMsMMBoxMessageHeaderStream = {0x101FB0F1};
// MMBox headers that are present in MMBox view request and confirmation only
const TUid KUidMMsMMBoxViewHeadersStream = {0x101FB0F2};
// Extra info (message text) present in extended notification.
const TUid KUidMMsExtendedNotificationStream = {0x101FB0F3};
// Attributes for UI use
const TUid KUidMmsAttributeStream = {0x101FB0F4};
// Application id and return-to application id
const TUid KUidMmsApplicationInfoStream = {0x101FB0F5};
// Reserved for internal use
// This stream contains all kinds of unusual information that is rarely present
// These are not part of an actual message, but control the transactions.
// - recommended retrieval mode
// - recommended retrieval mode text
// - replace or cancel id
// - cancel status
const TUid KUidMmsReserved = {0x101FB0F6};

//
// Log entry event type uid (used unless Symbian allocates an id instead)
//
const TUid KLogMmsEventTypeUid = {0x1000595e};

//-----------------------------------------------
// Flag bits in iMtmData1
// The default value for all bits is 0
//-----------------------------------------------
// Message types stored in iMtmData1
// The message types defined are those that could actually be found
// in message store. Values for transient types are not defined.
const TUint32 KMmsMessageTypeMask =         0x0000000F;
const TUint32 KMmsMessageMSendReq =         0x00000001;
// reserved for future use                  0x00000002;
const TUint32 KMmsMessageMNotificationInd = 0x00000003;
// reserved for future use                  0x00000004;
const TUint32 KMmsMessageMRetrieveConf =    0x00000005;
// reserved for future use                  0x00000006;
const TUint32 KMmsMessageDeliveryInd =      0x00000007;
const TUint32 KMmsMessageReadRecInd =       0x00000008;
const TUint32 KMmsMessageReadOrigInd =      0x00000009;
const TUint32 KMmsMessageForwardReq =       0x0000000A;
// reserved for future use                  0x0000000B;
// reserved for future use                  0x0000000C;
// reserved for future use                  0x0000000D;
// reserved for future use                  0x0000000E;
// This must be the last one to allow some expansion for known types
const TUint32 KMmsMessageUnrecognized =     0x0000000F;

// Message state stored in iMtmData1
const TUint32 KMmsMessageEncrypted =        0x00000010;
const TUint32 KMmsMessageSigned =           0x00000020;
const TUint32 KMmsMessageForwarded =        0x00000040;
const TUint32 KMmsMessageMobileTerminated = 0x00000080;
const TUint32 KMmsMessageEditorOriented =   0x00000100;

// bit to indicate that attribute stream is present in entry
const TUint32 KMmsAttributeStreamPresent =  0x00000200;

// reserved for future use
// 0x00000400
// 0x00000800

// Message type flags
// 1 byte reserved (0x0000F000)
// If no flag is set, message is personal (default)
const TUint32 KMmsMessageClassMask     =    0x0000F000;
const TUint32 KMmsMessageAdvertisement =    0x00001000;
const TUint32 KMmsMessageInformational =    0x00002000;
// two message types can be added (0x00004000 and 0x00008000)

// Some free bits here...
const TUint32 KMmsReservedMtmData1 = 0x00FF0000;

// Bit that indicates if the message contains non-conformant content.
// Default value is 0.
// 0 = Message is conformant, or the content is unknown (created by legacy software)
// 1 = Message is known to contain non-conformant content.
const TUint32 KMmsNonConformantMessage =   0x01000000;

// DRM Status in iMtmData1
// A DRM attachment is corrupted and removed
const TUint32 EMmsDrmCorruptedAttachment =		0x02000000;

// Here is still a gap here: bits 0x0C000000 are free.
// (flags 0x04000000 and 0x08000000)
// Should be reserved for future DRM needs.

// <DEPRECATED>
// Range of bits that indicates that the message has some special content
// that needs some non-standard handling
const TUint32 KMmsSpecialContentMask =  0xF0000000;
// undefined special content reserved for future use:  0x10000000
// Special content is "postcard"
const TUint32 KMmsPostcard =            0x20000000;
// Special content is "upload"
const TUint32 KMmsUpload =              0x40000000;
// undefined special content reserved for future use:  0x80000000
// </DEPRECATED>


//-----------------------------------------------
// Flag bits in iMtmData2
// The default value for all bits is 0
//-----------------------------------------------

// Message notification stuff stored in iMtmData2
// Response already sent to MMSC
// or MMBox item for which notify response will not be sent.
// (fetch acknowledged with M-Acknowledge.ind PDU)
const TUint32 KMmsNotifyResponseSent = 0x00000001;
// undecoded (binary) notification
const TUint32 KMmsNotificationBinary = 0x00000002;

// The following flags are related to notifications that are stored in inbox
// to allow the messages to be fetched manually (not automatically)
// has the message corresponding to this notification expired

// Message corresponding to this notification has expired
// 0 = Message has not expired, 1 = Message has expired
const TUint32 KMmsMessageExpired =     0x00000004;

// <DEPRECATED>
const TUint32 KMmsDeleteAtExpiration = 0x00000008;
// </DEPRECATED>

// Operation mask. The next bits tell which operation is currently in progress.
// When a message is being forwarded or fetched, another simulataneous operation
// is not allowed.
// 0 = no operation
const TUint32 KMmsOperationIdentifier = 0x00000070;

// fetch operation in progress
const TUint32 KMmsOperationFetch =      0x00000010;
// forward operation in progress or finished
const TUint32 KMmsOperationForward =    0x00000020;
// delete operation in progress or finished
const TUint32 KMmsOperationDelete =     0x00000030;
// 0x00000040 - 0x00000070 reserved for future operations

// Operation progress bits (4 bits)

// Is new operation allowed for this notification
// 0 = new operation allowed
// 1 = new operation forbidden
const TUint32 KMmsNewOperationForbidden = 0x00000080;
// Is operation currently ongoing
// 0 = no operation
// 1 = operation ongoing
const TUint32 KMmsOperationOngoing =      0x00000100;
// Has operation finished (and is result available)
// 0 = operation has not finished
// 1 = operation has finished
const TUint32 KMmsOperationFinished =     0x00000200;
// Operation result (if operation has finished)
// 0 = OK
// 1 = error
const TUint32 KMmsOperationResult =       0x00000400;

// Has message been stored to MMBox as result of some operation
// or was it in MMBox originally
// 0 = Message not in MMBox
// 1 = Message stored in MMBox
const TUint32 KMmsStoredInMMBox =         0x00000800;

// 0 = Not applicable
// 1 = Notification received in deferred mode, but response
//     sending was not allowed, and therefore notify-response
//     has not been sent to MMSC
const TUint32 KMmsDeferredButResponseNotSent = 0x00001000;

// This is some special notification that should never be
// moved to inbox in manual mode even if fetching fails.
// 0 = May be moved if needed
// 1 = Do not move to inbox
const TUint32 KMmsDoNotMoveToInbox = 0x00002000;

// The message was addressed to an application
// The message has been successfully fetched and routed to the application
// Even if notification remains in inbox, no operations are allowed except
// delete from phone.
const TUint32 KMmsMessageRoutedToApplication = 0x00004000;

// The following flags are for all message types

// bits reserved for future use
const TUint32 KMmsReservedMtmData2 =      0x000F8000;

// Delivery status bytes, valid only for sent messages
// that have been moved to "Sent" folder.
// If the message has been copied or moved to another folder,
// there is no guarantee that these bits are valid anymore.

const TUint32 KMmsDeliveryStatusMask = 0x00F00000;
// Delivery report was not requested
// If the user specifies that no delivery reports are wanted,
// the message stays in this state forever.
// This status means that no delivery data is available.
const TUint32 KMmsDeliveryStatusNotRequested = 0x00000000;

// If Delivery reports have been requested, the number of
// recipients and received delivery reports is stored in
// iMtmData3

// Delivery status is "pending" when the delivery status of all
// recipients is "pending".
const TUint32 KMmsDeliveryStatusPending = 0x00100000;
// delivery status is "partial" when delivery raport about
// successful delivery has been received from at least one recipient.
const TUint32 KMmsDeliveryStatusPartial = 0x00200000;
// delivery status is "failed" if sending has failed for at least
// one recipient
const TUint32 KMmsDeliveryStatysFailed = 0x00400000;
// delivery status is "delivered" if the status of all
// recipients is delivered
const TUint32 KMmsDeliveryStatysDelivered = 0x00800000;


// bits reserved for UI usage
// MMS engine will not change these bits.
// The default value for these bits will be 0.
const TUint32 KMmsReservedForUI =         0xFF000000;

//-----------------------------------------------
// Flag bits in iMtmData3
// The default value for all bits is 0
//-----------------------------------------------

// These are mask values for ranges of bits to be used as counters
// The shift constants indicate how much the value must be shifted
// right to be used as a number, and how much the number must be
// shifted to left before being stored.

// To examine the value use:
// number = ( iMtmData3 & mask ) >> shift;
// To set the value use:
// iMtmData3 &= ~mask;
// iMtmData3 |= number << shift;

// Retry count mask ( for automatic retries )
// max 15 retries counted
const TUint32 KMmsRetryCountMask =          0x0000000F;
// no shift needed for automatic retry count

// Retry count mask ( for manual retries )
// This is reserved for UI usage.
// max 15 retries counted
const TUint32 KMmsManualRetryCountMask =    0x000000F0;
const TInt KMmsManualRetryCountShift = 4;

// The following three areas are used to keep track of delivery status
// of several recipients. These fields are valid only for messages
// that have been sent and have been moved to sent folder.
// The values are not valid in any other context as it is impossible
// to prevent applications from copying messages behind MMS engine's back.
// MMS engine tries to clear these bits in other places, but there is no
// guarantee of that.

// Total number of recipients
// Max 255 recipients
const TUint32 KMmsSentItemTotalRecipientsMask = 0x0000FF00;
const TInt KMmsSentItemTotalRecipientsShift = 8;

// Number of delivery reports with status "delivered"
// Max 255 delivery reports
const TUint32 KMmsSentItemSuccessfullyDeliveredMask = 0x00FF0000;
const TInt KMmsSentItemSuccessfullyDeliveredShift = 16;

// Number of delivery reports with status "failed"
// Max 255 delivery reports
const TUint32 KMmsSentItemFailedDeliveryMask = 0xFF000000;
const TInt KMmsSentItemFailedDeliveryShift = 24;


// End of MtmData bit definitions
// ------------------------------------------------------

// Unicode BOM
const TUint16 KMmsByteOrderMark = 0xFEFF;
const TUint16 KMmsReversedByteOrderMark = 0xFFFE;

// Maximum iDescription read from text/plain file in terms of characters.
const TInt KMmsMaxDescription = 128;

// WSP assigned numbers
const TInt8 KUidMmsWSPMultiPartMixed = 0x23;
const TInt8 KUidMmsWSPMultiPartRelated = 0x33;

// MMS specific field assignments
// same values used for enum values in internal storage
const TUint8 KMmsMessageClassPersonal = 128;
const TUint8 KMmsMessageClassAdvertisement = 129;
const TUint8 KMmsMessageClassInformational = 130;
const TUint8 KMmsMessageClassAuto = 131;

// X-Mms-Message-Type field
// message types since encaps 1.0
const TUint8 KMmsMessageTypeMSendReq = 128;
const TUint8 KMmsMessageTypeMSendConf = 129;
const TUint8 KMmsMessageTypeMNotificationInd = 130;
const TUint8 KMmsMessageTypeMNotifyRespInd = 131;
const TUint8 KMmsMessageTypeMRetrieveConf = 132;
const TUint8 KMmsMessageTypeAcknowledgeInd = 133;
const TUint8 KMmsMessageTypeDeliveryInd = 134;
// message types since encaps 1.1
const TUint8 KMmsMessageTypeReadRecInd = 135;
const TUint8 KMmsMessageTypeReadOrigInd = 136;
const TUint8 KMmsMessageTypeForwardReq = 137;
const TUint8 KMmsMessageTypeForwardConf = 138;
// message types since encaps 1.2
const TUint8 KMmsMessageTypeMboxStoreReq = 139;
const TUint8 KMmsMessageTypeMboxStoreConf = 140;
const TUint8 KMmsMessageTypeMboxViewReq = 141;
const TUint8 KMmsMessageTypeMboxViewConf = 142;
const TUint8 KMmsMessageTypeMBoxUploadReq = 143;
const TUint8 KMmsMessageTypeMBoxUploadConf = 144;
const TUint8 KMmsMessageTypeMBoxDeleteReq = 145;
const TUint8 KMmsMessageTypeMBoxDeleteConf = 146;
const TUint8 KMmsMessageTypeMBoxDescr = 147;
// message types since encaps 1.3
const TUint8 KMmsMessageTypeDeleteReq = 148;
const TUint8 KMmsMessageTypeDeleteConf = 149;
const TUint8 KMmsMessageTypeCancelReq = 150;
const TUint8 KMmsMessageTypeCancelConf = 151;


// X-Mms-Priority field
const TUint8 KMmsPriorityLow = 128;
const TUint8 KMmsPriorityNormal = 129;
const TUint8 KMmsPriorityHigh = 130;

// All fields with Yes/No value
const TUint8 KMmsYes = 128;
const TUint8 KMmsNo = 129;

// X-Mms-Sender-Visibility field
const TUint8 KMmsSenderHide = 128;
const TUint8 KMmsSenderShow = 129;

// X-Mms-Status field
const TUint8 KMmsMessageStatusExpired = 128;
const TUint8 KMmsMessageStatusRetrieved = 129;
const TUint8 KMmsMessageStatusRejected = 130;
const TUint8 KMmsMessageStatusDeferred = 131;
const TUint8 KMmsMessageStatusUnrecognized = 132;
const TUint8 KMmsMessageStatusIndeterminate = 133;
const TUint8 KMmsMessageStatusForwarded = 134;
const TUint8 KMmsMessageStatusUnreachable = 135;

// X-Mms-Read-Status field
const TUint8 KMmsReadStatusRead = 128;
const TUint8 KMmsReadStatusDeletedWithoutBeingRead = 129;

// X-Mms-Reply-Charging field
const TUint8 KMmsReplyChargingRequested = 128;
const TUint8 KMmsReplyChargingRequestedTextOnly = 129;
const TUint8 KMmsReplyChargingAccepted = 130;
const TUint8 KMmsReplyChargingAcceptedTextOnly = 131;

// field assignments since MMS encapsulation 1.2

// X-Mms-Mbox-Quotas field & // X-Mms-MBox-Totals field
const TUint8 KMmsMessageCountToken = 128;
const TUint8 KMmsMessageSizeToken = 129;

// X-Mms-MM-Flags field
const TUint8 KMmsAddToken = 128;
const TUint8 KMmsRemoveToken = 129;
const TUint8 KMmsFilterToken = 130;

// X-Mms-MM-State field
const TUint8 KMmsDraft = 128;
const TUint8 KMmsSent = 129;
const TUint8 KMmsNew = 130;
const TUint8 KMmsRetrieved = 131;
const TUint8 KMmsForwarded = 132;

// Response status or Retrieve status OK
const TInt KMmsResponseStatusOK = 128;

// field assignments since MMS encapsulation 1.3
// These will be needed when 1.3 version is supported (not yet)

// X-Mms-Cancel-Status field
const TUint8 KMmsCancelRequestSuccessfullyReceived = 128;
const TUint8 KMmsCancelRequestCorrupted = 129;

// X-Mms-Content-Class field
const TUint8 KMmsContentClassText = 128;
const TUint8 KMmsContentClassImageBasic = 129;
const TUint8 KMmsContentClassImageRich = 130;
const TUint8 KMmsContentClassVideoBasic = 131;
const TUint8 KMmsContentClassVideoRich = 132;
const TUint8 KMmsContentClassMegaPixel = 133;
const TUint8 KMmsContentClassContentBasic = 134;
const TUint8 KMmsContentClassContentRich = 135;

// X-Mms-Recommended-Retrieval-mode field
const TUint8 KMmsRecommendedRetrievalModeManual = 128;

// end of field assignments

// All content type strings are in KContentTypeTable
// some are defined here to be used separately
_LIT8( KMmsAny, "*/*" );
_LIT8( KMmsTextPlain, "text/plain" );
_LIT8( KMmsApplicationSmil, "application/smil" );
_LIT8( KMmsUnknownType, "application/octet-stream" );
_LIT8( KMmsMultipartReport, "multipart/report" );

// DRM Content subtypes. CMsvMimeHeaders class saves type and subtype separately
_LIT8( KMmsDrmMessageCT,  "vnd.oma.drm.message" ); // DRM Message
_LIT8( KMmsDrmContentCT,  "vnd.oma.drm.content" ); // DRM Content
_LIT8( KMmsDrmRightsTFCT, "vnd.oma.drm.rights+xml" ); // DRM Rights in textual format
_LIT8( KMmsDrmRightsBFCT, "vnd.oma.drm.rights+wbxml" ); // DRM Rights in binary format
_LIT8( KMmsDrmDcfCT,      "vnd.oma.drm.dcf" ); // DRM Content format for discrete media

// Content type parameters used by Java WMA to specify application id
_LIT8( KMmsJavaApplicationId, "Application-ID" );
_LIT8( KMmsJavaReplyApplicationId, "Reply-To-Application-ID" );

// Assigned numbers for content types
// If new assignments are needed, check numbers from KContentTypeTable
const TUint8 KMmsAssignedAny = 0x00;
const TUint8 KMmsAssignedText = 0x01;
const TUint8 KMmsAssignedTextPlain = 0x03;
const TUint8 KMmsAssignedApplicationVndWapMultipart = 0x22;
const TUint8 KMmsAssignedApplicationVndWapMultipartMixed = 0x23;
const TUint8 KMmsAssignedApplicationVndWapMultipartAlternative = 0x26;
const TUint8 KMmsAssignedApplicationXml = 0x27;
const TUint8 KMmsAssignedApplicationVndWapWbxml = 0x29;
const TUint8 KMmsAssignedApplicationVndWapMultipartRelated = 0x33;

// IANA / WSP numbers for character sets
const TUint8 KMmsUtf8 = 0x6A;
const TUint8 KMmsUsAscii = 0x03;
const TUint KMmsIso10646Ucs2 = 0x03E8; // 1000
const TUint KMmsUTF16 = 0x03F7; // 1015
const TUint KMmsUTF16BE = 0x03F5; // 1013
const TUint KMmsUTF16LE = 0x03F6; // 1014
const TUint KMmsUtf7 = 0x03F4; // 1012

// CR/LF definitions for removal...
_LIT ( KLf16, "\x00A" ); // 16 bit line feed
_LIT ( KCr16, "\x00D" ); // 16 bit carriage return
_LIT ( KSpace16, " " ); // 16 bit space
_LIT ( KEqualsQuestion16, "=?" ); // MIME encoding starter
_LIT ( KQuote16, "\x022" ); // 16 bit quote
_LIT ( KMmsUnicodeLineSeparator, "\x2028");
_LIT ( KMmsUnicodeParagraphSeparator, "\x2029");
_LIT ( KMmsIdeographicSpace, "\x3000");
_LIT8 ( KLf8, "\x00A" ); // 8 bit line feed
_LIT8 ( KCr8, "\x00D" ); // 8 bit carriage return
_LIT8 ( KSpace8, " " ); // 8 bit space
_LIT8 ( KSemicolon8, ";" ); // 8 bit semicolon
_LIT8 ( KEquals8, "=" ); // 8 bit equal
_LIT8 ( KMmsSlash8, "/"); // 8 bit slash
_LIT8 ( KMmsLeftAngle, "<"); // 8 bit angle bracket
_LIT8 ( KMmsRightAngle, ">"); // 8 bit angle bracket

// MACROS

// DATA TYPES

// Possible address types
enum TMmsAddressType
    {
    EMmsAddressTypeUnknown = 0,
    EMmsAddressTypeMobile,
    EMmsAddressTypeEmail,
    EMmsAddressTypeIpv4,
    EMmsAddressTypeIpv6
    };

// These panics should only be used in ASSERT_DEBUGs
// (The user should not see these)
enum TMmsPanic // MMS error codes
    {
    EMmsBadMtmTypeUid = KErrNone+1,
    EMmsNoCMsvEntrySet,
    EMmsEntryTypeNotSupported,
    EMmsHeadersNotFound,
    EMmsAlreadyBusy,
    EMmsUnknownState,
    EMmsActiveInFinalProgress,
    EMmsFinalProgressFailed,
    EMmsNotAMessageEntry
    };


// Values for recipient types
// <DEPRECATED>
// old TMmsRecipients type values should not be used any more!
// instead use TMsvRecipientType values by Symbian!
typedef TMsvRecipientType TMmsRecipients;
#define EMmsTo EMsvRecipientTo
#define EMmsCc EMsvRecipientCc
#define EMmsBcc EMsvRecipientBcc
// </DEPRECATED>

enum TMmsYesNo
    {
    EMmsYes = KMmsYes,
    EMmsNo = KMmsNo
    };

// Values for message Delivery-Report
typedef TMmsYesNo TMmsMessageDeliveryReport;
// Values for Delivery-Report allowed
typedef TMmsYesNo TMmsSettingsDeliveryReportAllowed;
// Values for message Read-Reply
typedef TMmsYesNo TMmsMessageReadReply;
// Values for Read-Reply sending allowed
typedef TMmsYesNo TMmsSettingsReadReplyAllowed;

// <DEPRECATED>
// use EMmsYes and EMmsNo instead
#define EMmsDeliveryReportYes EMmsYes
#define EMmsDeliveryReportNo EMmsNo

#define EMmsDeliveryReportAllowedYes EMmsYes
#define EMmsDeliveryReportAllowedNo EMmsNo

#define EMmsReadReplyYes EMmsYes
#define EMmsReadreplyNo EMmsNo

#define EMmsReadReplyAllowedYes EMmsYes
#define EMmsReadReplyAllowedNo EMmsNo
// </DEPRECATED>

// Values for message class
enum TMmsMessageClass
    {
    EMmsMimimumMessageClass = KMmsMessageClassPersonal,
    EMmsClassPersonal = KMmsMessageClassPersonal,
    EMmsClassAdvertisement = KMmsMessageClassAdvertisement,
    EMmsClassInformational = KMmsMessageClassInformational,
    EMmsClassAuto = KMmsMessageClassAuto,
    // change following if new message classes are added
    EMmsMaximumMessageClass = KMmsMessageClassAuto
    };

// Values for message priority
enum TMmsMessagePriority
    {
    EMmsMinimumPriority = KMmsPriorityLow,
    EMmsPriorityLow = KMmsPriorityLow,
    EMmsPriorityNormal = KMmsPriorityNormal,
    EMmsPriorityHigh = KMmsPriorityHigh,
    // change following if new priorities added
    EMmsMaximumPriority = KMmsPriorityHigh
    };

// Values for message Sender-Visibility
enum TMmsMessageSenderVisibility
    {
    EMmsSenderVisibilityDefault = 0,
    EMmsMinimumSenderVisibility = KMmsSenderHide, // always minimum
    EMmsSenderVisibilityHide = KMmsSenderHide,
    EMmsSenderVisibilityShow = KMmsSenderShow,
    EMmsMaximumSenderVisibility = KMmsSenderShow // change if new values added
    };


// Values for MMS Creation mode
enum TMmsCreationMode
    {
    EMmsCreationModeRestricted = 0,
    EMmsCreationModeWarning = 1,
    EMmsCreationModeFree = 2
    };

// <DEPRECATED>
// Values for Message-Type field
// correspond to binary encoding!
// Message type cannot be set using Client MTM
// Use KMms constants instead.
enum TMmsMessageType
    {
    EMmsMSendReq = KMmsMessageTypeMSendReq,
    EMmsMSendConf = KMmsMessageTypeMSendConf,
    EMmsNotificationInd = KMmsMessageTypeMNotificationInd,
    EMmsNotifyRespInd = KMmsMessageTypeMNotifyRespInd,
    EMmsRetrieveConf = KMmsMessageTypeMRetrieveConf,
    EMmsAcknowledgeInd = KMmsMessageTypeAcknowledgeInd,
    EMmsDeliveryInd = KMmsMessageTypeDeliveryInd,
    EMmsReadRecInd = KMmsMessageTypeReadRecInd,
    EMmsReadOrigInd = KMmsMessageTypeReadOrigInd,
    EMmsForwardReq = KMmsMessageTypeForwardReq,
    EMmsForwardConf = KMmsMessageTypeForwardConf
    };
// </DEPRECATED>


// <DEPRECATED>
// Values for Status field
// correspond to binary encoding!
// Use KMms constants instead.
enum TMmsMessageStatus
    {
    EMmsStatusExpired = KMmsMessageStatusExpired,
    EMmsStatusRetrieved = KMmsMessageStatusRetrieved,
    EMmsStatusRejected = KMmsMessageStatusRejected,
    EMmsStatusDeferred = KMmsMessageStatusDeferred,
    EMmsStatusUnrecognized = KMmsMessageStatusUnrecognized,
    EMmsStatusIndeterminate = KMmsMessageStatusIndeterminate,
    EMmsStatusForwarded = KMmsMessageStatusForwarded,
    EMmsStatusUnreachable = KMmsMessageStatusUnreachable
    };
// </DEPRECATED>

// Values for read status
// correspond to binary encoding!
enum TMmsReadStatus
    {
    EMmsReadStatusRead = KMmsReadStatusRead,
    EMmsReadStatusDeletedWithoutBeingRead = KMmsReadStatusDeletedWithoutBeingRead
    };

enum TMmsReplyCharging
    {
    EMmsReplyChargingRequested = KMmsReplyChargingRequested,
    EMmsReplyChargingRequestedTextOnly = KMmsReplyChargingRequestedTextOnly,
    EMmsReplyChargingAccepted = KMmsReplyChargingAccepted,
    EMmsReplyChargingAcceptedTextOnly = KMmsReplyChargingAcceptedTextOnly
    };

// mms receiving mode
enum TMmsReceivingMode
    {
    EMmsReceivingMin = 1,
    EMmsReceivingAutomatic = EMmsReceivingMin,
    EMmsReceivingManual = EMmsReceivingMin + 1,
    EMmsReceivingPostpone = EMmsReceivingManual + 1,
    EMmsReceivingReject = EMmsReceivingPostpone + 1,
    EMmsReceivingMax = EMmsReceivingReject
    };

// Factory settings
enum TMmsFactorySettingsLevel
    {
    EMmsFactorySettingsLevelNormal = 0x0,
    EMmsFactorySettingsLevelDeep
    };

// Type of the Delete operation
enum TMmsDeleteOperationType
    {
    EMmsDeleteNotificationOnly,
    EMmsDeleteMMBoxOnly,
    EMmsDeleteBoth
    };

#endif      // MMSCONST_H

// End of File
