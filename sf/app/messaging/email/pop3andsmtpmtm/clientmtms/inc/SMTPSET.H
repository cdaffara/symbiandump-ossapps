// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#if !defined (__SMTPSET_H__)
#define __SMTPSET_H__

 
#include <msvuids.h>
#include <miutset.h>

/** Default SMTP server port number.*/
const TUint32 KSMTPDefaultPortNumber = 25;

/** Default/maximum addresses to include in body header of reply/forward messages */
const TInt KSmtpToCcIncludeLimitDefault = 0;
const TInt KSmtpToCcIncludeLimitMax = KMaxTInt;


/**
Defines options that allows the user to automatically email themselves a copy 
of all emails that are sent from the phone.

@see CImSmtpSettings::SendCopyToSelf() 

@publishedAll
@released
*/
enum TImSMTPSendCopyToSelf
	{
	/** Do not send a copy. */
	ESendNoCopy,
	/** Send a copy, with the user's address added to the To address list. */
	ESendCopyAsToRecipient,
	/** Send a copy, with the user's address added to the Cc: address list. */
	ESendCopyAsCcRecipient,
	/** Send a copy, with the user's address added to the Bcc: address list. */
	ESendCopyAsBccRecipient
	};
/**
Defines sending options for new email messages.

The option has no direct effect on the SMTP MTM, although messages in the 
outbox which are marked as ESendMessageOnNextConnection or ESendMessageImmediately 
will be appended automatically to the list of messages supplied by the client 
when either CBaseServerMtm::CopyFromLocal() or CBaseServerMtm::MoveFromLocal() 
are called. 

@publishedAll
@released
*/
enum TImSMTPSendMessageOption
	{
	/** Send message immediately. */
	ESendMessageImmediately,
	/** Send message when the next connection occurs. */
	ESendMessageOnNextConnection,
	/** Send message only when the user requests this. */
	ESendMessageOnRequest
	};

/**
Defines status values for the SMTP MTM. 

@publishedAll
@released
*/
enum TMsgImOutboxSendState
	{
	/** Idle. */
	EMsgOutboxProgressWaiting,
	/** Connecting to server. */
	EMsgOutboxProgressConnecting,
	/** Sending messages. */
	EMsgOutboxProgressSending,
	/** Sending complete. */
	EMsgOutboxProgressDone
	};

/** Sending state for SMTP. 

@publishedAll
@released
*/
enum TSmtpSessionState
	{
	/** Dialling, looking up DNS, or connecting to SMTP server. */
	EConnectingToSmtp,
	/** Waiting for the SMTP server to send welcome message. */
	EWaitingForReply,
	/** Sending HELO/EHLO command to the server. */
	EAuthorisingSmtp,
	/** Sending the STARTTLS command, if a secure connection was requested. */
	ESendingStarttls,
	/** Exchanging certificates with server, if a secure connection was requested. */
	ESettingSecurity,
	/** Sending a message to the server. */
	ESendingImail,
	/** Logging result. */
	ELogDataEvent,
	/** Disconnecting from the SMTP server. */
	EClosingSmtp,
	/** AUTH in progress. */
	EAuthInProgress,
	/** RSET in progress. */
	EResetSmtp
	};

class TImImailFileProgress				
/** 
Status information about the SMTP message which is being sent. 

@publishedAll
@released
*/
	{
public:
	/**
	Amount of data from the message already sent to the server in bytes.
	
	Note that the SMTP MTM encodes the text and binary data inside an email message 
	while it is sending the data. As a result, iBytesToSend is likely to increase 
	(by as much as one third) while a message is being sent. The size increase 
	represents all additional bytes which have been added to the RFC 822 message 
	to encode the data.
	*/
	TInt				iBytesSent;
	/** Amount of data from the message still to be sent to the server in bytes. */
	TInt				iBytesToSend;
	/** Sending state. */
	TSmtpSessionState	iSessionState;
	};

class TImSmtpProgress
/**
Progress information for SMTP operations on the Internet Mail MTM.

The information is obtained through calling CMsvOperation::Progress() 
on the operation. It is packaged into an 8-bit descriptor. 

Messaging clients use the getter functions to get various types
of information about an operation's progress. The setter functions
are for use by the MTM.

@publishedAll
@released
*/
	{

public:
	IMPORT_C TMsgImOutboxSendState	Status() const; // Progress: current progress status of mail session
	IMPORT_C TInt MsgNo() const;		            // Progress: No. of message currently being sent; zero=1st message
	IMPORT_C TInt Error() const;			        // Summary: completion code from end of last session
	IMPORT_C TInt Sent() const;			            // Summary: No. messages sent successfully
	IMPORT_C TInt NotSent() const;		            // Summary: No. messages I didn't attempt to send
	IMPORT_C TInt FailedToSend() const;	            // Summary: No. messages attempted to send but failed
	IMPORT_C TInt SendTotal() const;		        // Sent() + NotSent() + FailedToSend() = SendTotal()
	IMPORT_C void SetError(TInt anError);
	IMPORT_C void SetStatus(TMsgImOutboxSendState aStatus);
	IMPORT_C void SetMsgNo(TInt aMsgNo);
	IMPORT_C void InitialiseTotal(const TInt& aTotal);
	IMPORT_C void UpdateSent();
	IMPORT_C void UpdateFailedToSend();
	IMPORT_C void DecrementSendTotal();
	IMPORT_C TMsvId	ServiceId() const;
	IMPORT_C void SetServiceId(TMsvId aServiceId);
	IMPORT_C TInt ConnectionState() const;
	IMPORT_C TInt ConnectionIAP() const;
	IMPORT_C void SetConnectionIAP(TInt aConnectionIAP);

public:
	/** Status information about the message that is currently being sent (if 
	sending is in progress). */
	TImImailFileProgress			iSendFileProgress;

private:
	TMsgImOutboxSendState	iStatus;
	TInt					iError;
	TInt					iMsgNo;
	TInt					iSent;
	TInt					iNotSent;
	TInt					iFailedToSend;
	TInt					iSendTotal;
	TMsvId					iServiceId;
	};

class CImSmtpSettings : public CImBaseEmailSettings
/**
Run-time configuration settings for an SMTP account.

Messaging clients should use an instance of this class to specify
and retrieve configuration settings that are used by the SMTP service when
executing email operations.  

Service settings such as the email body encoding, reply address, character set,
and whether to attach a signature or vCard can be specified using this class.
Storing and restoring from the message store is also supported.

To use this class to change a setting:

1) Set the current context to the SMTP service entry using CMsvStore.

2) Create an instance of CImSmtpSettings and put it on the cleanup stack.

3) Retrieve the existing settings by calling CImSmtpSettings::RestoreL().

4) Specify whether to add a vCard to outbound email by calling CImSmtpSettings::SetAddVCardToEmail().

5) Save the new settings by calling CImSmtpSettings::StoreL().

6) Pop and destroy the CImSmtpSettings instance.

@see CMsvStore

@publishedAll
@released
*/
	{
public:
	IMPORT_C CImSmtpSettings();
	IMPORT_C virtual ~CImSmtpSettings();
	IMPORT_C void Reset();
	IMPORT_C const TPtrC EmailAlias() const;
	IMPORT_C void SetEmailAliasL(const TDesC& aEmailAlias);
	IMPORT_C const TPtrC EmailAddress() const;
	IMPORT_C void SetEmailAddressL(const TDesC& aEmailAddress);
	IMPORT_C const TPtrC ReplyToAddress() const;
	IMPORT_C void SetReplyToAddressL(const TDesC& aReplyToAddress);
	IMPORT_C const TPtrC ReceiptAddress() const;
	IMPORT_C void SetReceiptAddressL(const TDesC& aReceiptAddress);
	IMPORT_C TMsgOutboxBodyEncoding BodyEncoding() const;
	IMPORT_C void SetBodyEncoding(TMsgOutboxBodyEncoding aBodyEncoding);
	IMPORT_C const TUid DefaultMsgCharSet() const;
	IMPORT_C void SetDefaultMsgCharSet(TUid aDefaultMsgCharSet);
	IMPORT_C TBool AddVCardToEmail() const;
	IMPORT_C void SetAddVCardToEmail(TBool aFlag);
	IMPORT_C TBool AddSignatureToEmail() const;
	IMPORT_C void SetAddSignatureToEmail(TBool aFlag);
	IMPORT_C TBool RequestReceipts() const;
	IMPORT_C void SetRequestReceipts(TBool aFlag);
	IMPORT_C TImSMTPSendCopyToSelf SendCopyToSelf() const;
	IMPORT_C void SetSendCopyToSelf(TImSMTPSendCopyToSelf aSendCopyToSelf);
	IMPORT_C TImSMTPSendMessageOption SendMessageOption() const;
	IMPORT_C void SetSendMessageOption(TImSMTPSendMessageOption aSendMessageOption);
	IMPORT_C CImSmtpSettings& CopyL(const CImSmtpSettings& aCImSmtpSettings);
	IMPORT_C TBool operator==(const CImSmtpSettings& aCImSmtpSettings) const;
	IMPORT_C const TPtrC8 LoginName() const;
	IMPORT_C void SetLoginNameL(const TDesC8&);
	IMPORT_C const TPtrC8 Password() const;
	IMPORT_C void SetPasswordL(const TDesC8&);
	IMPORT_C TBool SMTPAuth() const;
	IMPORT_C void SetSMTPAuth(TBool aFlag);
	IMPORT_C TBool InboxLoginDetails() const;
	IMPORT_C void SetInboxLoginDetails(TBool aFlag);
	IMPORT_C TInt ToCcIncludeLimit() const;
	IMPORT_C void SetToCcIncludeLimitL(TInt aLimit);
	IMPORT_C void SetTlsSslDomainL(const TDesC8& aDomainName);
	IMPORT_C TPtrC8 TlsSslDomain() const;

private:
	class TImSmtpSettingsExtension
		{
	public:
		inline TImSmtpSettingsExtension();
	public:
		HBufC*	iReceiptAddress;
		HBufC8* iLoginName;
		HBufC8* iPassword;
		HBufC8*	iTlsSslDomain;
		TInt	iToCcIncludeLimit;
		};

	inline TImSmtpSettingsExtension* Extension() const;
	inline void CheckExtensionExistsL();

private:
	enum TImSMTPEmailSettings
		{
		KSmtpSettingsClearFlag				= 0x00000000,
		KSmtpBaseEmailSettingsLastUsedFlag	= CImBaseEmailSettings::EBaseEmailSettingsLastUsedFlag,	//0x00000002
		KSmtpAddVCardToEmailFlag			= KSmtpBaseEmailSettingsLastUsedFlag << 1,	//0x00000004
		KSmtpAddSignatureToEmailFlag		= KSmtpAddVCardToEmailFlag << 1,			//0x00000008
		KSmtpRequestReceipts				= KSmtpAddSignatureToEmailFlag << 1,		//0x00000010
		KSmtpSmtpAuthFlag					= KSmtpRequestReceipts << 1,				//0x00000020
		KSmtpInboxLoginDetails				= KSmtpSmtpAuthFlag << 1,					//0x00000040
		KSmtpEmailSettingsLastUsedFlag		= KSmtpInboxLoginDetails					//0x00000040
		};

	HBufC*						iEmailAlias;
	HBufC*						iEmailAddress;
	HBufC*						iReplyToAddress;
	TImSmtpSettingsExtension*	iExtension; // renamed iReceiptAddress
	TMsgOutboxBodyEncoding		iBodyEncoding;
	TUid						iDefaultMsgCharSet;
	TImSMTPSendCopyToSelf		iSendCopyToSelf;
	TImSMTPSendMessageOption	iSendMessageOption;
	};
	
inline CImSmtpSettings::TImSmtpSettingsExtension* CImSmtpSettings::Extension() const
	{
	return iExtension;
	}

inline void CImSmtpSettings::CheckExtensionExistsL()
	{
	if (!iExtension)
		iExtension=new (ELeave) CImSmtpSettings::TImSmtpSettingsExtension;
	}

inline CImSmtpSettings::TImSmtpSettingsExtension::TImSmtpSettingsExtension() : iReceiptAddress(NULL),iLoginName(NULL),iPassword(NULL),iTlsSslDomain(NULL),iToCcIncludeLimit(KSmtpToCcIncludeLimitDefault)
	{
	}

#endif    // #define __SMTPSET_H__
