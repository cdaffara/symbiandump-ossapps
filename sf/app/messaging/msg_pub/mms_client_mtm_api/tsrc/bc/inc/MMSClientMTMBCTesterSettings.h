/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:         MMSClientMTMBCTester settings
*
*/










#ifndef MMSCLIENTMTMBCTESTERSETTINGS_H
#define MMSCLIENTMTMBCTESTERSETTINGS_H


// INCLUDES
#include <mmsconst.h>
#include <e32base.h>


// Literals for searching data from initialization files

_LIT8( KPhoneNumber,		"PhoneNumber=" );
_LIT8( KEmailAddress,		"EmailAddress=" );
_LIT8( KAlias,				"Alias=" );

_LIT8( KAttributeName,		"AttributeName=" );
_LIT8( KAttributeValue,		"AttributeValue=" );

_LIT8( KMsgSubject,			"MsgSubject=" );
_LIT8( KMsgDescription,		"MsgDescription=" );

_LIT8( KAttachmentType,		"AttachmentType=" );
_LIT8( KAttachmentFile,		"AttachmentFile=" );
_LIT8( KAttachmentName,		"AttachmentName=" );

_LIT8( KTextAttachmentName,	"TextAttachmentName=" );
_LIT8( KTextAttachmentText,	"TextAttachmentText=" );


// Constant values to be used as test values in the test cases

const TMmsMessageClass KMessageClass				= EMmsClassPersonal;
const TMmsMessagePriority KMessagePriority			= EMmsPriorityNormal;
const TMmsMessageSenderVisibility KSenderVisibility = EMmsSenderVisibilityShow;
const TMmsYesNo KRequest							= EMmsYes;

const TInt KTimeInterval							= 1234567;
const TInt KImageWidth								= 320;
const TInt KImageHeight								= 240;
const TInt KMessageSelectionSize					= 3;

_LIT( KDate, "20060519:" );


// Different methods for adding attachments
enum TMMSClientMTMBCTesterAttaType
	{
	EMMSClientMTMBCTesterAttaTypeNormal,
	EMMSClientMTMBCTesterAttaTypeCancel,
	EMMSClientMTMBCTesterAttaTypeClosedStore,
	EMMSClientMTMBCTesterAttaTypeInvalidFileHandle,
	EMMSClientMTMBCTesterAttaTypeInvalidMIMEType,
	EMMSClientMTMBCTesterAttaTypeInvalidAttaInfo
	};


// Character strings to be used with logging

_LIT( KCasePassed,				"CASE PASSED: %S" );
_LIT( KCaseFailed,				"CASE FAILED: %S" );
_LIT( KSuccess,					"Success" );

_LIT( KErrorCreateMessageEntry,		"Creating the message entry failed" );
_LIT( KErrorCreateServiceEntry,		"Service entry could not be created" );
_LIT( KErrorGetMMSSettings, 		"Getting the MMS settings failed" );
_LIT( KErrorSetMMSSettings,			"Setting the MMS settings failed" );
_LIT( KErrorStoreMMSSettings, 		"Storing the MMS settings to disk failed" );
_LIT( KErrorRestoreMMSSettings,
	"Restoring the MMS settings from disk failed" );
_LIT( KErrorRestoreFactoryMMSSettings,
	"Restoring the factory MMS settings failed" );
_LIT( KErrorValidateService,		"Validating the service failed" );
_LIT( KErrorGetDefaultService,		"Getting the default service id failed" );
_LIT( KErrorSetSender,				"Setting the sender failed" );
_LIT( KErrorGetSender,				"Getting the sender failed" );
_LIT( KErrorMessageClass,			"Handling the message class failed" );
_LIT( KErrorExpirationInterval,		"Handling the expiration interval failed" );
_LIT( KErrorExpirationDate,			"Handling the expiration date failed" );
_LIT( KErrorDeliveryTimeInterval,
	"Handling the delivery time interval failed" );
_LIT( KErrorDeliveryDate,			"Handling the delivery date failed" );
_LIT( KErrorMessagePriority,		"Handling message priority failed" );
_LIT( KErrorSenderVisibility,		"Handling sender visibility failed" );
_LIT( KErrorDeliveryReportRequest,
	"Handling the delivery repory request failed" );
_LIT( KErrorReadReplyRequest,		"Handling the read reply request failed" );
_LIT( KErrorGetSendingDate,			"Getting the sending date failed" );
_LIT( KErrorGetResponseText,		"Getting the response text failed" );
_LIT( KErrorGetResponseStatus,		"Getting the response status failed" );
_LIT( KErrorGetNumberOfPreviousSenders,
	"Getting the number of previous senders failed" );
_LIT( KErrorGetPreviousSender,		"Getting a previous sender failed" );
_LIT( KErrorGetPreviousSendingDate,	"Getting a previous sending date failed" );
_LIT( KErrorGetReceiveTime,			"Getting the message receive time failed" );
_LIT( KErrorGetTransferSize,
	"Getting the message transfer size failed" );
_LIT( KErrorGetContentLocation,		"Getting the message location failed" );
_LIT( KErrorSetMessageRootPart,		"Setting the message root part failed" );
_LIT( KErrorGetMessageRootPart,		"Getting the message root part failed" );
_LIT( KErrorMaximumImageSize,		"Handling the maximum image size failed" );
_LIT( KErrorGetMessageSize,			"Getting the message size failed" );
_LIT( KErrorSetMessageDescription,	"Setting the message description failed" );
_LIT( KErrorAddNewAttribute,		"Adding a new attribute failed" );
_LIT( KErrorGetAttribute,			"Getting an attribute failed" );
_LIT( KErrorFindAttribute,			"Checking for an attribute failed" );
_LIT( KErrorDeleteAttribute,		"Deleting an attribute failed" );
_LIT( KErrorResetAttributes,		"Resetting the attributes failed" );
_LIT( KErrorAddAttachmentSync,		"Creating the attachment failed" );
_LIT( KErrorAddTextAttachment,		"Creating the text attachment failed" );
_LIT( KErrorSendMessage,			"Sending the message failed" );
_LIT( KErrorSendMessageSelection,	"Sending the message selection failed" );
_LIT( KErrorFetchAllMessages,		"Fetching the messages failed" );
_LIT( KErrorGetType,				"Getting the MTM type failed" );
_LIT( KErrorSetContext,				"Setting the context failed");
_LIT( KErrorSwitchContext,			"Switching the context failed" );
_LIT( KErrorHasContext,				"Checking for context failed" );
_LIT( KErrorLoadMessage,			"Loading the message failed" );
_LIT( KErrorSaveMessage,			"Saving the message failed" );
_LIT( KErrorValidateMessage,		"Validating the message failed" );
_LIT( KErrorSearchForText,			"Searching for a text failed" );
_LIT( KErrorCreateReply,			"Creating a reply message failed" );
_LIT( KErrorCreateForward,			"Creating a forward message failed" );
_LIT( KErrorAddAddressee,			"Adding an addressee failed" );
_LIT( KErrorRemoveAddressee,		"Removing an addressee failed" );
_LIT( KErrorGetMessageBody,
	"Getting access to the rich text message body failed" );
_LIT( KErrorSetMessageSubject,		"Setting the message subject failed" );
_LIT( KErrorGetMessageSubject,		"Getting the message subject failed" );
_LIT( KErrorQueryCapability,		"Querying a capability failed" );
_LIT( KErrorInvokeSyncFunction,		"Invoking a synchronous function failed" );
_LIT( KErrorInvokeAsyncFunction,
	"Invoking an asynchronous function failed" );
_LIT( KErrorGetSession,				"Getting access to the session failed" );
_LIT( KErrorAddAttachment,			"Creating the attachment failed" );
_LIT( KErrorCreateMessage,			"Creating the message failed" );
_LIT( KErrorCancelAttachmentOp,
	"Canceling the attachment operation failed" );
_LIT( KErrorClearFolder,			"Cleaning up a message folder failed" );
_LIT( KErrorReturnValue,			"Unexpected return value" );
_LIT( KErrorGetEntry,
	"NOT AN MMS CLIENT ERROR: Getting the entry from the session failed." );


#endif // MMSCLIENTMTMBCTESTERSETTINGS_H
            
// End of file: MMSClientMTMBCTesterSettings.h
