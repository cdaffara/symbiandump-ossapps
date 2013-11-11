/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 */

#ifndef MMSREADFILE_H
#define MMSREADFILE_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <f32file.h>
#include <msvstd.h>

// forward references
class CMmsAttaStructure;
class CMmsHeaders;
class CEikonEnv;
class CMmsClientMtm;

// Out-of-range value for any MMS header with assigned number
// Used in coverage tests to get coverage for "default" branches
// in "case" statements.
const TInt KMmsTestIllegalValue = 255;

//
// TEST KEY WORDS
// Extended to include all MMS headers to allow generation of arbitrary PDUs
// Content type header cannot be separately defined,
// it will always be "multipat/mixed" or "multipart/related"
_LIT8( KTestEndOfFile,          "EOF" );
_LIT8( KTestNewMessage,         "MESSAGE BEGIN" );
//------------------------------------------------
_LIT8( KTestMessageType,        "MESSAGE-TYPE" ); //X-Mms-Message-Type
_LIT8( KTestDate,               "DATE" ); // Date
_LIT8( KTestFrom,               "FROM" ); // From
_LIT8( KTestTo,                 "TO" ); // To
_LIT8( KTestCc,                 "CC" ); // Cc
_LIT8( KTestBcc,                "BCC" ); // Bcc
_LIT8( KTestSubject,            "SUBJECT" ); // Subject
_LIT8( KTestExpiryRel,          "EXPIRY-REL" ); // X-Mms-Expiry, relative
_LIT8( KTestExpiryAbs,          "EXPIRY-ABS" ); // X-Mms-Expiry, absolute
_LIT8( KTestDeliveryTimeRel,    "DELIVERY-TIME-REL" ); // X-Mms-Delivery-Time, relative
_LIT8( KTestDeliveryTimeAbs,    "DELIVERY-TIME-ABS" ); // X-Mms-Delivery-Time, absolute
_LIT8( KTestPriority,           "PRIORITY" ); // X-Mms-Priority
_LIT8( KTestSenderVisibility,   "SENDER-VISIBILITY" ); // X-Mms-Sender-Visibility
_LIT8( KTestDeliveryReport,     "DELIVERY-REPORT" ); // X-Mms-Delivery-Report
_LIT8( KTestReadReply,          "READ-REPLY" ); // X-Mms-Read-Report
// ---------------------------------------------------
_LIT8( KTestNewAttachment,      "ATTACHMENT" );
_LIT8( KTestAttachmentType,     "ATTACHMENTTYPE" );
_LIT8( KTestAttachmentName,     "ATTACHMENTNAME" );
_LIT8( KTestAttachmentContLoc,  "ATTACHMENTCONTLOC" );
_LIT8( KTestAttachmentCharset,  "ATTACHMENTCHARSET" );
_LIT8( KTestAttachmentCid,      "ATTACHMENTCID" );
_LIT8( KTestAttachmentRoot,     "ATTACHMENTROOT" );
// content-type and x-type parameters for attachments
// should appear in name-value pairs.
// Test program is not responsible for verifying illegal scripts
_LIT8( KTestAttContTypeParamName, "ContTypeParamName" );
_LIT8( KTestAttContTypeParamValue, "ContTypeParamValue" );
_LIT8( KTestAttXTypeParamName, "X-TypeParamName" );
_LIT8( KTestAttXTypeParamValue, "X-TypeParamValue" );
// -----------------------------------------------------
_LIT8( KTestMessageClass,       "MESSAGE-CLASS" ); // X-Mms-Message-Class
_LIT8( KTestDelivRepSendAllow,  "DELIV-REP-SEND-ALLOW" ); // X-Mms-Report-Allowed
_LIT8( KTestAcceptAdvertis,     "ACCEPT-ADVERTISEMENTS" );
_LIT8( KTestRetryCount,         "RETRY-COUNT" );
_LIT8( KTestRetryInterval,      "RETRY-INTERVAL" );
// -------------------------------------------------
_LIT8( KTestAlias,				"ALIAS" );
_LIT8( KTestFromAlias,			"FROMALIAS" );
_LIT8( KTestToAlias,			"TOALIAS" );
_LIT8( KTestCcAlias,			"CCALIAS" );
_LIT8( KTestBccAlias,			"BCCALIAS" );
// -------------------------------------------------
// more Message headers to allow testing all possible PDUs and
// MMS 1.1 headers
_LIT8( KTestReplyCharging,      "REPLY-CHARGING" ); // X-Mms-Reply-Charging
_LIT8( KTestReplyChargAbs,      "REPLY-CHARG-DEADLINE-ABS" ); // X-Mms-Reply-Charging, absolute
_LIT8( KTestReplyChargRel,      "REPLY-CHARG-DEADLINE-REL" ); // X-Mms-Reply-Charging, relative
_LIT8( KTestReplyChargSize,     "REPLY-CHARG-SIZE" ); // X-Mms-Reply-Charging-Size (octets)
_LIT8( KTestReplyChargID,       "REPLY-CHARGING-ID" ); // X-Mms-Reply-Charging-ID
_LIT8( KTestTID,                "TID" );
_LIT8( KTestContentLocation,    "CONTLOC" ); // X-Mms-Content-Location
// The following three must appear as triplets.
// index must be first, address and date may appear in any order
_LIT8( KTestPreviouslySentIndex, "PREVIOUSINDEX" ); // forwarded count for X-Mms-PreviouslySent headers
_LIT8( KTestPreviouslySentBy,    "PREV-SENT-BY" ); // X-Mms-Previously-Sent-By
_LIT8( KTestPreviouslySentDate,  "PREV-SENT-DATE" ); // X-Mms-Previously-Sent-Date
//---
_LIT8( KTestMessageId,           "MESSAGE-ID" ); // Message-ID
_LIT8( KTestMessageSize,         "MESSAGE-SIZE" ); // X-Mms-Message-Size (octets)
_LIT8( KTestVersion,             "MMS-VERSION" ); // X-MMS-Version (hex:, 10 = 1.0, 11 = 1.1 etc. )
_LIT8( KTestReadStatus,          "READ-STATUS" ); // X-Mms-Read-Status
_LIT8( KTestResponseStatus,      "RESPONSE-STATUS" ); // X-Mms-Response-Status
_LIT8( KTestResponseText,        "RESPONSE-TEXT" ); // X-Mms-Response-Text
_LIT8( KTestRetrieveStatus,      "RETRIEVE-STATUS" ); // X-Mms-Retrieve-Status
_LIT8( KTestRetrieveText,        "RETRIEVE-TEXT" ); // X-Mms-Retrieve-Text
_LIT8( KTestStatus,              "STATUS" ); // X-Mms-Status
// -------------------------------------------------
// MMS 1.2 headers
_LIT8( KTestAttribute,          "ATTRIBUTE" ); // X-Mms-Attribute
// The value should be one of the following:
// BCC, CC, CONTENT, CONTENT-TYPE, DATE, DELIVERY-REPORT, DELIVERY-TIME-ABS,
// EXPIRY-ABS, FROM, MESSAGE-CLASS, MESSAGE-ID, MESSAGE-SIZE, PRIORITY,
// READ-REPLY, SUBJECT, TO, REPLY-CHARGING, REPLY-CHARGING-ID,
// REPLY-CHARG-DEADLINE-ABS, REPLY-CHARG-SIZE, PREV-SENT-BY,
// PREV-SENT-DATE, ADDITIONAL-HEADERS
_LIT8( KTestDistributionIndicator,  "DISTRIBUTION-INDICATOR" ); // X-Mms-Distribution-Indicator
_LIT8( KTestLimit,              "LIMIT" ); // X-Mms-Limit
_LIT8( KTestMessageQuota,       "MESSAGE-QUOTA" ); // X-Mms-Mbox-Quota, message number
_LIT8( KTestSizeQuota,          "SIZE-QUOTA" ); // X-Mms-Mbox-Quota, size quota
_LIT8( KTestMessageTotal,       "MESSAGE-TOTAL" ); // X-Mms-Mbox-Totals, message number
_LIT8( KTestSizeTotal,          "SIZE-TOTAL" ); // X-Mms-Mbox-Totals, total size
_LIT8( KTestMessageCount,       "MESSAGE-COUNT" ); // X-Mms-Message-Count
_LIT8( KTestAddKeyword,         "ADD-KEYWORD" ); // X-Mms-MM-Flags
_LIT8( KTestRemoveKeyword,      "REMOVE-KEYWORD" ); // X-Mms-MM-Flags
_LIT8( KTestFilterKeyword,      "FILTER-KEYWORD" ); // X-Mms-MM-FLags
_LIT8( KTestMMState,            "MESSAGE-STATE" ); // X-Mms-MM-State
_LIT8( KTestQuota,              "QUOTA" ); // X-Mms-Quotas
_LIT8( KTestStart,              "START" ); // X-Mms-Start
_LIT8( KTestStore,              "STORE" ); // X-Mms-Store
_LIT8( KTestStored,             "STORED" ); // X-Mms-Stored
_LIT8( KTestStoreStatus,        "STORE-STATUS" ); // X-Mms-Store-Status
_LIT8( KTestStoreStatusText,    "STORE-STATUS-TEXT" ); // X-Mms-Store-Status-Text
_LIT8( KTestTotals,             "TOTALS" ); // X-Mms-Totals
// Delete confirmation header structure
// This consists of index, content location, response status and response status text.
// All three values are not always necessary for each index, but at least content-location
// and response status should be present.
// The index retains its value until a new index is encountered.
_LIT8( KTestDeleteInfoIndex,    "DELETESTATUSINDEX" ); // index for next delete info fields
// This header must precede content-location, response-status and response-text fields
// in a delete confirmation. Index retains its value until a new index is encountered.

// Element descriptor header not implemented

// Application id headers
// These will officially be supported in MMS encapsulation version 1.3
// Java has non-standard support even earlier

_LIT8( KTestApplicId,            "APPLIC-ID" ); // X-Mms-Applic-ID
_LIT8( KTestReplyApplicId,       "REPLY-APPLIC-ID" ); // X-Mms-Reply-Applic-ID
_LIT8( KTestApplicInfo,          "AUX-APPLIC-INFO" ); // X-Mms-Aux-Applic-Info

// Since Encapsulation 1.3
_LIT8( KTestContentClass,        "CONTENT-CLASS" ); // X-Mms-Content-Class
_LIT8( KTestDrmContent,          "DRM-CONTENT" ); // X-Mms-DRM-Content
_LIT8( KTestAdaptationAllowed,   "ADAPTATION-ALLOWED" ); // X-Mms-Adaptation-Allowed
_LIT8( KTestRecommendedRetrievalMode, "REC-RETRIEVAL-MODE" ); // X-Mms-Recommended-Retrieval-Mode
_LIT8( KTestRecRetrievalModeText, "REC-RETR-MODE-TEXT" ); // X-Mms-Recommended-Retrieval-Mode-Text
_LIT8( KTestReplaceId,           "REPLACE-ID" ); // X-Mms-Replace-ID
_LIT8( KTestStatusText,          "STATUS-TEXT" ); // X-Mms-Status-Text
_LIT8( KTestCancelId,            "CANCEL-ID" ); // X-Mms-Cancel-ID
_LIT8( KTestCancelStatus,        "CANCEL-STATUS" ); // X-Mms-Cancel-Status

// DATA TYPES
#define aSet            1
#define aReset          0
#define PartCount      100
#define MaxAttaCount   10
#define DefaultBufLen 256
#define MaxRecipients   5

// status returned by ReadRow
enum TTestReadStatus
    {
    ETestUnknown,
    ETestNewMessage,
    ETestNewAttachment,
    ETestEof,    // End of file
    ETestMessageType,
    ETestDate,
    ETestFrom,
    ETestTo,
    ETestCc,
    ETestBcc,
	ETestAlias,
	ETestFromAlias,
	ETestToAlias,
	ETestCcAlias,
	ETestBccAlias,
    ETestSubject,
    ETestExpiryRel,
    ETestExpiryAbs,
    ETestDeliveryTimeRel,
    ETestDeliveryTimeAbs,
    ETestPriority,
    ETestSenderVisibility,
    ETestDeliveryReport,
    ETestReadReply,
    ETestAttachmentType,
//    ETestAttachmentName,
    ETestAttachmentCharset,
    ETestAttachmentCid,
	ETestAttachmentRoot,
    ETestSettings,
    ETestMessageClass,
    ETestDelivReportSendAllow,
    ETestSingleMessageClass,
    ETestReplyCharging,
    ETestReplyChargAbs,
    ETestReplyChargRel,
    ETestReplyChargSize,
    ETestReplyChargID,
    ETestTID,
    ETestContentLocation,
    ETestPreviouslySentIndex,
    ETestPreviouslySentBy,
    ETestPreviouslySentDate,
    ETestMessageId,
    ETestMessageSize,
    ETestVersion,
    ETestReadStatus,
    ETestResponseStatus,
    ETestResponseText,
    ETestRetrieveStatus,
    ETestRetrieveText,
    ETestStatus,
    ETestAttribute,
    ETestDistributionIndicator,
    ETestLimit,
    ETestMessageQuota,
    ETestSizeQuota,
    ETestMessageTotal,
    ETestSizeTotal,
    ETestMessageCount,
    ETestAddKeyword,
    ETestRemoveKeyword,
    ETestFilterKeyword,
    ETestMMState,
    ETestQuota,
    ETestStart,
    ETestStore,
    ETestStored,
    ETestStoreStatus,
    ETestStoreStatusText,
    ETestTotals,
    ETestDeleteInfoIndex,
    ETestApplicId,
    ETestReplyApplicId,
    ETestApplicInfo,
    ETestContentClass,
    ETestDrmContent,
    ETestAdaptationAllowed,
    ETestRecommendedRetrievalMode,
    ETestRecRetrievalModeText,
    ETestReplaceId,
    ETestStatusText,
    ETestCancelId,
    ETestCancelStatus,
    ETestAttaRecommendedName,
    ETestAttachmentContLoc,
    ETestAttContTypeParamName,
    ETestAttContTypeParamValue,
    ETestAttXTypeParamName,
    ETestAttXTypeParamValue

    };

// VALUE KEYWORDS
_LIT( KPersonal,      "Personal" );
_LIT( KAdvertisement, "Advertisement" );
_LIT( KInformational, "Informational" );
_LIT( KAuto,          "Auto" );
_LIT( KLow,    "Low" );
_LIT( KNormal, "Normal" );
_LIT( KHigh,   "High" );
_LIT( KHide,   "Hide" );
_LIT( KShow,   "Show" );
_LIT( KYes,    "Yes" );
_LIT( KNo,     "No" );
_LIT( KOn,     "On" );
_LIT( KOff,     "Off" );
_LIT( KDeferred, "Deferred" );
_LIT( KExpired,  "Expired" );
_LIT( KRetrieved, "Retrieved" );
_LIT( KRejected, "Rejected" );
_LIT( KUnrecognized, "Unrecognized" );
_LIT( KIndeterminate, "Indeterminate" );
_LIT( KForwarded, "Forwarded" );
_LIT( KUnreachable, "Unreachable" );
_LIT( KDraft, "Draft");
_LIT( KSent, "Sent" );
_LIT( KNew, "New" );
// X-Mms-Message-Type
_LIT( KSendReq, "SendReq" );
_LIT( KSendConf, "SendConf" );
_LIT( KNotifInd, "NotifInd" );
_LIT( KNotifResp, "NotifResp" );
_LIT( KRetrConf, "RetrConf" );
_LIT( KAckInd, "AckInd" );
_LIT( KDelInd, "DelInd" );
_LIT( KReadReqInd, "ReadReqInd" );
_LIT( KReadOrigInd, "ReadOrigInd" );
_LIT( KForwardRec, "ForwardReq" );
_LIT( KForwardConf, "ForwardConf" );
_LIT( KMBoxStoreReq, "MBoxStoreReq" );
_LIT( KMBoxStoreConf, "MBoxStoreConf" );
_LIT( KMBoxViewReq, "MBoxViewReq" );
_LIT( KMBoxViewConf, "MBoxViewConf" );
_LIT( KMBoxUploadReq, "MBoxUploadReq" );
_LIT( KMBoxUploadConf, "MBoxUploadConf" );
_LIT( KMBoxDeleteReq, "MBoxDeleteReq" );
_LIT( KMBoxDeleteConf, "MBoxDeleteConf" );
_LIT( KMBoxDescr, "MBoxDescr" );
_LIT( KDeleteReq, "DeleteReq" );
_LIT( KDeleteConf, "DeleteConf" );
_LIT( KCancelReq, "CancelReq" );
_LIT( KCancelResp, "CancelConf" );
// X-Mms-Read-Status
_LIT( KRead, "Read" );
_LIT( KDelNotRead, "DeletedNotRead" );
// X-Mms-Reply-Charging
_LIT( KRequested, "Requested" );
_LIT( KReqTextOnly, "ReqTextOnly" );
_LIT( KAccepted, "Accepted" );
_LIT( KAccTextOnly, "AccTextOnly" );
// X-Mms-Cancel-Status (only receival of request, not actual cancellation)
_LIT( KCancelSuccessful, "CancelSuccess" );
_LIT( KCancelCorrupted, "CancelCorrupted" );
// X-Mms-Content-Class
_LIT( KText, "Text" );
_LIT( KImageBasic, "ImageBasic" );
_LIT( KImageRich, "ImageRich" );
_LIT( KVideoBasic, "VideoBasic" );
_LIT( KVideoRich, "VideoRich" );
_LIT( KMegaPixel, "MegaPixel" );
_LIT( KContentBasic, "ContentBasic" );
_LIT( KContentRich, "ContentRich" );
// X-Mms-Recommended-Retrieval-Mode
_LIT( KManual, "Manual" );

//X-Mms-ResponseStatus
// legacy values
_LIT( KOk, "OK" );
_LIT( KErrUnspecified, "Unspecified" );
_LIT( KErrServiceDenied, "ServiceDenied" );
_LIT( KErrMessageFormatCorrupt, "FormatCorrupt" );
_LIT( KErrAddressUnresolved, "AddressUnresolved" );
_LIT( KErrMessageNotFound, "MessageNotFound" );
_LIT( KErrNetworkProblem, "NetworkProblem" );
_LIT( KErrContentNotAccepted, "ContentNotAccepted" );
_LIT( KErrUnsupportedMessage, "UnsupportedMessage" );
// values since 1.1 (and 1.2)
_LIT( KErrTransient, "TransientFailure" );
_LIT( KErrTransientAddressUnresolved, "TransientAddressUnresolved" );
_LIT( KErrTransientNotFound, "TransientMessageNotFound" );
_LIT( KErrTransientNetworkproblem, "TransientNetworkProblem" );
_LIT( KErrTransientPartialSuccess, "PartialSuccess" );
_LIT( KErrPermanent, "PermanentFailure" );
_LIT( KErrPermanentServiceDenied, "PermanentServiceDenied" );
_LIT( KErrPermanentMessageFormatCorrupt, "PermanentFormatCorrupt" );
_LIT( KErrPermanentAddressUnresolved, "PermanentAddressUnresolved" );
_LIT( KErrPermanentNotFound, "PermanentMessageNotFound" );
_LIT( KErrPermanentContentNotAccepted, "PermanentContentNotAccepted" );
_LIT( KErrReplyChargingLimitNotMet, "ReplyChargingLimitNotMet" );
_LIT( KErrReplyChargingRequestNotAccepted, "ReplyChargingRequestNotAccepted" );
_LIT( KErrReplyChargingForwardingDenied, "ReplyChargingForwardingDenied" );
_LIT( KErrReplyChargingNotSupported, "ReplyChargingNotSupported" );
_LIT( KErrAddressHidingNotSupported, "AddressHidingNotSupported" );
_LIT( KErrPermanentLackOfPrepaid, "PermanentLackOfPrepaid" );
//X-Mms-RetrieveStatus (one extra value)
_LIT( KErrRetrieveContentUnsupported, "PermanentContentUnsupported" );
//X-Mms-Store-Status (one extra value)
_LIT( KErrMMBoxFull, "MMBoxFull" );

_LIT8( KTestContent,  "Content" ); // For attribute list only
_LIT8( KTestAdditionalHeaders,  "ADDITIONAL-HEADERS" ); // For attribute list only
_LIT8( KTestDeliveryTime,       "DELIVERY-TIME"); // for attribute list only
_LIT8( KTestExpiry,             "EXPIRY"); // for attribute list only
_LIT8( KTestReplyCharg,         "REPLY-CHARG-DEADLINE" ); // for attribute list only
_LIT8( KTestContentType,        "CONTENT-TYPE" );  // for attribute list only

/**
*  CMmsReadFile
*/
NONSHARABLE_CLASS( CMmsReadFile ):public CBase
    {
    public:  // Constructors and destructor

        static CMmsReadFile* NewL(RFs& aFs, RFileReadStream& aReadStream ); // Two-phased constructor.
        virtual ~CMmsReadFile(); // Destructor

    public: // New functions

        TInt CompleteTestL(TInt aMessageCounter, CMmsHeaders& aMmsHeaders);
        TTestReadStatus ReadRowL();
        void CreateMessageL( CMmsClientMtm* aMmsClient, CMmsHeaders* aMmsHeaders );
		TInt FindAlias(TPtrC alias);
		void SetAttaStructure( TBool aOldAttaStructure = EFalse );

    protected:  // Functions from base classes

    private:

        CMmsReadFile(); // C++ default constructor.
        // By default constructor is private.
        void ConstructL( RFs& aFs, RFileReadStream& aReadStream );
        void Reset();

    public: // data
        TInt                            iMessageType;


    private:     // Data

        RFs                             iFs;
        RFileReadStream*                iReader;
        TBuf<DefaultBufLen>             iValueBuffer;
        CDesCArray*     iAliasArray;    // aliases
        HBufC8*         iByteBuffer;
        TTime                           iDate;
        CArrayPtrFlat<CMmsAttaStructure>* iAttaStructures;
        TInt                            iAttaCount;
		TInt							iAttaRoot;
		TInt							iAliasCount;
        TInt                            iNextMessageType;
        TParse                          iParse;
        TFileName                       iFilename;

    protected:  // Data
    private:    // Data

        // CArrayFixFlat <TestMessage>* iMessages;

    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

    };


NONSHARABLE_CLASS( CMmsAttaStructure ):public CBase
    {
    public:  // Constructors and destructor
        static CMmsAttaStructure* NewL(); // Two-phased constructor.
        virtual ~CMmsAttaStructure(); // Destructor
    private:
        CMmsAttaStructure(); // C++ default constructor.
        // By default constructor is private.
        void ConstructL();

    public:     // Data
        HBufC8*         iAtta;
        HBufC8*         iAttaName; // content location
        HBufC8*         iAttaType;
        HBufC8*         iAttaCid;
        TInt            iAttaCharset;
        HBufC*          iAttaRecommendedName; // recommended filename
       	CDesC8ArrayFlat* iContentTypeParams;  // zero or more "parameter"
    	CDesC8ArrayFlat* iXTypeParams;        // zero or more X-Type "parameters"

    };


#endif      // MMSREADFILE_H

// End of File
