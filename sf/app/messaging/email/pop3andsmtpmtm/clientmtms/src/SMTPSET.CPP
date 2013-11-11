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

#include "SMTPSET.H" 
#include <nifvar.h>				// For KLinkLayerOpen


GLREF_C TPtrC8 LimitStringSize(const TPtrC8& aString, TInt aMaxSize);
GLREF_C TPtrC16 LimitStringSize(const TPtrC16& aString, TInt aMaxSize);


EXPORT_C CImSmtpSettings::CImSmtpSettings()
/** 
Allocates and creates a new CImImap4Settings object initialised to
default values.
*/
	{
	Reset();
	} 


EXPORT_C CImSmtpSettings::~CImSmtpSettings()
/**
Destructor.
*/
	{
	Reset();

	delete iExtension;
	iExtension=NULL;
	}


EXPORT_C void CImSmtpSettings::Reset()
/**
Resets the settings to their default values.
*/
	{
	CImBaseEmailSettings::Reset();
	
	iPortNumber = KSMTPDefaultPortNumber;
	iBodyEncoding = EMsgOutboxMIME;

	delete iEmailAlias;
	iEmailAlias = NULL;

	delete iEmailAddress;
	iEmailAddress = NULL;

	delete iReplyToAddress;
	iReplyToAddress=NULL;
	
	if (Extension())
		{
		delete Extension()->iReceiptAddress;
		Extension()->iReceiptAddress=NULL;
		
		delete Extension()->iLoginName;
		Extension()->iLoginName=NULL;

		delete Extension()->iTlsSslDomain;
		Extension()->iTlsSslDomain=NULL;
		
		delete Extension()->iPassword;
		Extension()->iPassword=NULL;
		
		Extension()->iToCcIncludeLimit=KSmtpToCcIncludeLimitDefault;
		}
	}


EXPORT_C const TPtrC CImSmtpSettings::EmailAlias() const
/**
Retrieves the configured email alias (display name) that will be sent
with outbound email.

This is normally set to the real name of the user, e.g. "John Smith".
Set this value by calling SetEmailAliasL().

@return 
The email alias (display name) if set, empty if not.
*/
	{
	return iEmailAlias ? TPtrC(*iEmailAlias) : TPtrC();
	}


EXPORT_C const TPtrC CImSmtpSettings::EmailAddress() const
/**
Retrieves the configured SMTP email address of the user,
e.g. "john.smith@work.com".

@return
SMTP email address of the user if set, empty if not.
*/
	{
	return iEmailAddress ? TPtrC(*iEmailAddress) : TPtrC();
	}


EXPORT_C const TPtrC CImSmtpSettings::ReplyToAddress() const
/** Gets the internet email address that the user wishes replies to emails to be 
sent to. 

This may be set to be the same as for EmailAddress(), or may be set to be 
a different email address, e.g. "groupenquiries\@work.com".

@return Internet email address that the user wishes replies to emails to be 
sent to */
	{
	return iReplyToAddress ? TPtrC(*iReplyToAddress) : TPtrC();
	}


EXPORT_C const TPtrC CImSmtpSettings::ReceiptAddress() const
/** Gets the internet email address that the user wishes receipts to be returned 
to.

This would normally be set to be the same as for ReplyToAddress(), e.g. "groupenquiries\@work.com".

@return Internet email address that the user wishes receipts to be returned 
to. */
	{
	if (Extension() && Extension()->iReceiptAddress)
		return TPtrC(*(Extension()->iReceiptAddress));
	return TPtrC();
	}


EXPORT_C TMsgOutboxBodyEncoding CImSmtpSettings::BodyEncoding() const
/**
Retrieves the method of encoding email messages if the encoding is not
specified when they are sent.

The default value (EMsgOutboxMIME) is set so that text parts of the message
are sent as MIME multipart/alternative text/html parts, and are encoded using
UTF-8.
	
@see TMsgOutboxBodyEncoding

@return
Method of encoding.
*/
	{
	return iBodyEncoding;
	}


EXPORT_C const TUid CImSmtpSettings::DefaultMsgCharSet() const
/**
Retrieves which character set is used to send text in MIME email messages.

The default character set is ISO-8859-1 (Latin 1). The UIDs used to identify 
all character sets are defined in the Character Conversion API. 

@return
Character set identifier.
*/
	{
	return iDefaultMsgCharSet;
	}


EXPORT_C void CImSmtpSettings::SetEmailAliasL(const TDesC& aEmailAlias)
/**
Specifies the configured email alias (display name) that will be sent
with outbound email.

This is normally set to the real name of the user.  For example, "John Smith".

@param aEmailAlias
The email alias (display name).
*/
	{
	HBufC* newEmailAlias = aEmailAlias.AllocL();
	delete iEmailAlias;
	iEmailAlias = newEmailAlias;
	}


EXPORT_C void CImSmtpSettings::SetEmailAddressL(const TDesC& aEmailAddress)
/**
Specifies the SMTP email address of the user.

@param aEmailAddress
SMTP email address of the user.
*/
	{
	HBufC* newEmailAddress = aEmailAddress.AllocL();
	delete iEmailAddress;
	iEmailAddress = newEmailAddress;
	}


EXPORT_C void CImSmtpSettings::SetReplyToAddressL(const TDesC& aReplyToAddress)
/**
Specifies the email address that the user wishes replies to emails to be 
sent to. If this is not specified, replies will be sent to the address
set by calling SetEmailAddressL(). 

@param aReplyToAddress
SMTP email address that the user wishes replies 
to emails to be sent to.
*/
	{
	HBufC* newReplyToAddress = aReplyToAddress.AllocL();
	delete iReplyToAddress;
	iReplyToAddress = newReplyToAddress;
	}


EXPORT_C void CImSmtpSettings::SetReceiptAddressL(const TDesC& aReceiptAddress)
/**
Specifies the SMTP email address that the user wishes email receipts to be
returned to.

@param aReceiptAddress
SMTP email address that the user wishes receipts to be returned to.
*/
	{
	CheckExtensionExistsL();
	HBufC* newReceiptAddress = aReceiptAddress.AllocL();
	delete Extension()->iReceiptAddress;
	Extension()->iReceiptAddress = newReceiptAddress;
	}


EXPORT_C void CImSmtpSettings::SetBodyEncoding(TMsgOutboxBodyEncoding aBodyEncoding)
/**
Specifies the default method of encoding email messages if the encoding is
not specified when sent.

The default value (EMsgOutboxMIME) is set so that text parts of the message
are sent as MIME multipart/alternative text/html parts, and are encoded using
UTF-8.
	
@see TMsgOutboxBodyEncoding

@param aBodyEncoding
Default method of encoding
*/
	{
	iBodyEncoding = aBodyEncoding;
	}


EXPORT_C void CImSmtpSettings::SetDefaultMsgCharSet(TUid aDefaultMsgCharSet)
/**
Specifies which character set is used to send text in MIME email messages.

The default character set is ISO-8859-1 (Latin 1). The UIDs used to identify 
all character sets are defined in the Character Conversion API. 

@param aDefaultMsgCharSet
Character set identifier.
*/
	{
	iDefaultMsgCharSet = aDefaultMsgCharSet;
	}


EXPORT_C TBool CImSmtpSettings::AddVCardToEmail() const
/**
Retrieves whether email messages will have a VCard containing the user's
details from the Contacts Database when they are sent.

This is supported as an alternative method of adding contact information into 
email messages if the user does not wish to send signature text in their emails.

@return
ETrue if the user's VCard is to be attached when sending.
*/
	{
	return iFlags & KSmtpAddVCardToEmailFlag;
	}


EXPORT_C void CImSmtpSettings::SetAddVCardToEmail(TBool aFlag)
/** 
Specifies whether the email messages will have a VCard containing the user's
details from the Contacts Database when they are sent.

@param aFlag
ETrue if the user's VCards will be added when the email is sent.
*/
	{
	iFlags = (iFlags & ~KSmtpAddVCardToEmailFlag) | (aFlag ? KSmtpAddVCardToEmailFlag : KSmtpSettingsClearFlag);
	}


EXPORT_C TBool CImSmtpSettings::AddSignatureToEmail() const
/**
Retrieves whether email messages will have the user's signature text appended
to the text of the messages when they are sent.

The signature is stored in the SMTP service entry as a rich text body stream.

If there is no signature body text stored in the SMTP service entry, then the
email is sent with no signature. This method does not check whether the signature
body text exists in the SMTP entry or not.

This method is supported as an alternative method for adding contact 
information into email messages if the user does not wish to send VCards in 
emails. 

@return ETrue
If a signature text will be appended if it axists.
*/
	{
	return iFlags & KSmtpAddSignatureToEmailFlag;
	}


EXPORT_C void CImSmtpSettings::SetAddSignatureToEmail(TBool aFlag)
/**
Specifies whether email messages will have the user's signature text appended
to the text of the messages when they are sent.

The signature is stored in the SMTP service entry as a rich text body stream.

If there is no signature body text stored in the SMTP service entry, then the
email is sent with no signature. This method does not check whether the signature
body text exists in the SMTP entry or not.

This method is supported as an alternative method for adding contact 
information into email messages if the user does not wish to send VCards in 
emails. 

@param aFlag
ETrue if a signature text should be attempted to be appended to outbound email.
*/
	{
	iFlags = (iFlags & ~KSmtpAddSignatureToEmailFlag) | (aFlag ? KSmtpAddSignatureToEmailFlag : KSmtpSettingsClearFlag);
	}


EXPORT_C TBool CImSmtpSettings::RequestReceipts() const
/**
Retrieves whether outbound email messages will contain a header that requests
receipts to be returned to the address identified by ReceiptAddress().

If no receipt address has been set,then no receipt is requested. It is entirely
up to the receiving mail client whether or not it will comply with this request.

@return
ETrue if receipts are requested.
*/
	{
	return iFlags & KSmtpRequestReceipts;
	}


EXPORT_C void CImSmtpSettings::SetRequestReceipts(TBool aFlag)
/**
Specifies whether outbound email messages will have a header added that requests
a receipt from the recipient.

If no receipt address has been set,then no receipt is requested. It is entirely
up to the receiving mail client whether or not it will comply with this request.

@param aFlag
ETrue if receipts will be requested.
*/
	{
	iFlags = (iFlags & ~KSmtpRequestReceipts) | (aFlag ? KSmtpRequestReceipts : KSmtpSettingsClearFlag);
	}


EXPORT_C TImSMTPSendCopyToSelf CImSmtpSettings::SendCopyToSelf() const
/**
Retrieves the setting of the option that allows the user to automatically email 
themselves a copy of all emails that are sent. 

@see TImSMTPSendCopyToSelf

@return
Option setting.
*/
	{
	return iSendCopyToSelf;
	}


EXPORT_C void CImSmtpSettings::SetSendCopyToSelf(TImSMTPSendCopyToSelf aSendCopyToSelf)
/**
Specifies the option that allows the user to automatically email themselves a copy 
of all emails that are sent. 

@see TImSMTPSendCopyToSelf

@param aSendCopyToSelf
Option setting.
*/
	{
	iSendCopyToSelf = aSendCopyToSelf;
	}


EXPORT_C TImSMTPSendMessageOption CImSmtpSettings::SendMessageOption() const
/**
Retrieves the setting of the sending option for new email messages if it is not
specified while sending.

This option is intended to be acted on by the message client.
The default value is send the message immediately (ESendMessageImmediately).

@see TImSMTPSendMessageOption

@return
Default sending option.
*/
	{
	return iSendMessageOption;
	}


EXPORT_C void CImSmtpSettings::SetSendMessageOption(TImSMTPSendMessageOption aSendMessageOption)
/**
Specifies the sending option for outbound email messages if it is not
specified while sending.

The default value is to send the message immediately (ESendMessageImmediately).

@see TImSMTPSendMessageOption

@param aSendMessageOption
Default sending option.
*/
	{
	iSendMessageOption = aSendMessageOption;
	}


EXPORT_C CImSmtpSettings& CImSmtpSettings::CopyL(const CImSmtpSettings& aCImSmtpSettings)
/**
Copies the configuration settings from another SMTP settings object into this object.

@param aCImSmtpSettings
Specifies the object to copy.

@return
This object after copying.
*/
	{
	// CheckExtensionExistsL() called at the start of the function, since it's allocation will be longer lived
	CheckExtensionExistsL();
	Reset();
	CImBaseEmailSettings::CopyL(aCImSmtpSettings);
	iEmailAlias					=	aCImSmtpSettings.EmailAlias().AllocL();
	iEmailAddress				=	aCImSmtpSettings.EmailAddress().AllocL();
	iReplyToAddress				=	aCImSmtpSettings.ReplyToAddress().AllocL();
	Extension()->iReceiptAddress=	aCImSmtpSettings.ReceiptAddress().AllocL();
	iBodyEncoding				=	aCImSmtpSettings.BodyEncoding();
	iDefaultMsgCharSet			=	aCImSmtpSettings.DefaultMsgCharSet();
	iSendCopyToSelf				=	aCImSmtpSettings.SendCopyToSelf();
	iSendMessageOption			=	aCImSmtpSettings.SendMessageOption();
	Extension()->iLoginName		=	aCImSmtpSettings.LoginName().AllocL();
	Extension()->iPassword		=	aCImSmtpSettings.Password().AllocL();
	Extension()->iTlsSslDomain	=	aCImSmtpSettings.TlsSslDomain().AllocL();
	Extension()->iToCcIncludeLimit	=	aCImSmtpSettings.ToCcIncludeLimit();
	return (*this);
	}


EXPORT_C TBool CImSmtpSettings::operator==(const CImSmtpSettings& aCImSmtpSettings) const
/**
Equality operator.

@param aCImSmtpSettings
Specifies the SMTP settings object to compare with.

@return
ETrue if the settings are the same.
*/
	{
	return ((CImBaseEmailSettings::operator==(aCImSmtpSettings)) &&
			!((EmailAlias().Compare(aCImSmtpSettings.EmailAlias())) ||
			 (EmailAddress().Compare(aCImSmtpSettings.EmailAddress())) ||
			 (ReplyToAddress().Compare(aCImSmtpSettings.ReplyToAddress())) ||
			 (ReceiptAddress().Compare(aCImSmtpSettings.ReceiptAddress()))) &&
			(BodyEncoding()	==	aCImSmtpSettings.BodyEncoding()) &&
			(DefaultMsgCharSet() == aCImSmtpSettings.DefaultMsgCharSet()) &&
			(SendCopyToSelf() == aCImSmtpSettings.SendCopyToSelf()) &&
			(SendMessageOption() == aCImSmtpSettings.SendMessageOption()) &&
			(ToCcIncludeLimit() == aCImSmtpSettings.ToCcIncludeLimit()) &&
			!(LoginName().Compare(aCImSmtpSettings.LoginName())) &&
			!(Password().Compare(aCImSmtpSettings.Password())));
	}


EXPORT_C const TPtrC8 CImSmtpSettings::LoginName() const
/**
Retrieves the login user name used when SMTP authentication is enabled by
calling SetSMTPAuth(). 

@return
The login user name.
*/
	{
	if (Extension() && Extension()->iLoginName)
		return TPtrC8(*(Extension()->iLoginName));
	return TPtrC8();
	}


EXPORT_C void CImSmtpSettings::SetLoginNameL(const TDesC8& aLoginName)
/**
Specifies the login user name used when SMTP authentication is enabled by
calling SetSMTPAuth(). 

@param aLoginName
The login user name.
*/
	{
	CheckExtensionExistsL();
	HBufC8* newLoginName = aLoginName.AllocL();
	delete Extension()->iLoginName;
	Extension()->iLoginName = newLoginName;
	}


EXPORT_C const TPtrC8 CImSmtpSettings::Password() const
/**
Retrieves the password used when SMTP authentication is enabled by
calling SetSMTPAuth(). 

@return
The password.
*/
	{
	if (Extension() && Extension()->iPassword)
		return TPtrC8(*(Extension()->iPassword));
	return TPtrC8();
	}


EXPORT_C void CImSmtpSettings::SetPasswordL(const TDesC8& aPassword)
/**
Specifies the password used when SMTP authentication is enabled by
calling SetSMTPAuth().

@param aPassword
The password.
*/
	{
	CheckExtensionExistsL();
	HBufC8* newPassword = aPassword.AllocL();
	delete Extension()->iPassword;
	Extension()->iPassword = newPassword;
	}


EXPORT_C TBool CImSmtpSettings::SMTPAuth() const
/**
Retrieves whether SMTP authentication will be used when sending emails.
Read RFC 2554 "SMTP Service Extension for Authentication" for more details.

@return
ETrue if SMTP authentication is enabled.
*/
	{
	return iFlags & KSmtpSmtpAuthFlag;
	}


EXPORT_C void CImSmtpSettings::SetSMTPAuth(TBool aFlag)
/**
Enables or disables SMTP authentication when sending emails.
Read RFC 2554 "SMTP Service Extension for Authentication" for more details.

@param aFlag
ETrue to enable SMTP authentication.
*/
	{
	iFlags = (iFlags & ~KSmtpSmtpAuthFlag) | (aFlag ? KSmtpSmtpAuthFlag : KSmtpSettingsClearFlag);
	}


EXPORT_C TBool CImSmtpSettings::InboxLoginDetails() const
/*
This function should not be used. Use SMTPAuth() for testing if 
SMTP authentication should be used.
@return Unused
@deprecated
*/
	{
	return iFlags & KSmtpInboxLoginDetails;
	}


EXPORT_C void CImSmtpSettings::SetInboxLoginDetails(TBool aFlag)
/*
This function should not be used. Use SetSMTPAuth() for flagging that 
SMTP authentication should be used.
@param aFlag Unused
@deprecated
*/
	{
	iFlags = (iFlags & ~KSmtpInboxLoginDetails) | (aFlag ? KSmtpInboxLoginDetails : KSmtpSettingsClearFlag);
	}
	
/**
Retrieves the setting for inclusion of original To and CC email addresses 
in the body text of reply and forwarded emails.

@return
The maximum number of addresses to include in either field.
*/
EXPORT_C TInt CImSmtpSettings::ToCcIncludeLimit() const
	{
	if (Extension())
		{
		return (Extension()->iToCcIncludeLimit);
		}
	else
		{
		return KSmtpToCcIncludeLimitDefault;
		}
	}

/**
Specifies maximum number of original To and CC addresses to include in 
the body header when replying-to and forwarding emails.
The default (0) indicates that no addresses should be included.

@param aLimit 
The maximum number of addresses to include in each field.
*/
EXPORT_C void CImSmtpSettings::SetToCcIncludeLimitL(TInt aLimit)
	{
	__ASSERT_DEBUG( aLimit>=0, User::Invariant() );
	CheckExtensionExistsL();
	if (aLimit<0)
		{
		Extension()->iToCcIncludeLimit = KSmtpToCcIncludeLimitDefault;
		}
	else
		{
		Extension()->iToCcIncludeLimit = aLimit;
		}
	}

/**
Sets the domain name to use during TLS/SSL certificate validation.

@param aDomainName Domain name
*/
EXPORT_C void CImSmtpSettings::SetTlsSslDomainL(const TDesC8& aDomainName)
	{
	CheckExtensionExistsL();
	HBufC8* newTlsSslDomain = aDomainName.AllocL();
	delete Extension()->iTlsSslDomain;
	Extension()->iTlsSslDomain = newTlsSslDomain;
	}

/**
Gets the domain name used during TLS/SSL certificate validation.

@return Domain name
*/
EXPORT_C TPtrC8 CImSmtpSettings::TlsSslDomain() const
	{
	if (Extension() && Extension()->iTlsSslDomain)		
		return TPtrC8(*(Extension()->iTlsSslDomain));
	return TPtrC8();
	}


/** Gets the sending status of the SMTP operation.

@return Sending status
*/
EXPORT_C TMsgImOutboxSendState TImSmtpProgress::Status() const
	{
	return iStatus;	
	}


/** Sets the sending status of the SMTP operation

@param aStatus New sending status
*/
EXPORT_C void TImSmtpProgress::SetStatus(TMsgImOutboxSendState aStatus)
	{
	iStatus = aStatus;
	}


/** Sets an error code for the operation.

@param anError Error code
*/
EXPORT_C void TImSmtpProgress::SetError(TInt anError)
	{
	iError=anError;
	}


/** Gets an operation error code.

This is set only when a send operation has completed.

@return Operation error code
*/
EXPORT_C TInt TImSmtpProgress::Error() const
	{
	return iError;
	}


/** Gets the index of the message that is currently being sent.

The index range is from 0 to SendTotal().

@return Index of the message being sent
*/
EXPORT_C TInt TImSmtpProgress::MsgNo() const
	{
	if (Status() == EMsgOutboxProgressConnecting)
		{
		// While the status is EMsgOutboxProgressConnecting, the iMsgNo variable contains
		//  the connection progress value.  Therefore return 0 instead.
		return 0;
		}
	else
		{
		return iMsgNo;
		}
	}


/** Sets the index of the message that is currently being sent.

@param aMsgNo Index of the message being sent
*/
EXPORT_C void TImSmtpProgress::SetMsgNo(TInt aMsgNo)
	{
	iMsgNo = aMsgNo;
	}


/** Gets the number of messages sent so far in this SMTP session.

@return Number of messages
*/
EXPORT_C TInt TImSmtpProgress::Sent() const
	{
	if (Status() == EMsgOutboxProgressConnecting)
		{
		// While the status is EMsgOutboxProgressConnecting, the iSent variable contains
		//  the connection IAP value.  Therefore return 0 instead.
		return 0;
		}
	else
		{
		return iSent;	
		}
	}


/** Gets the number of messages that the MTM did not attempt to send in this session.

A possible reason is because the message store was locked by another client.

@return Number of messages
*/
EXPORT_C TInt TImSmtpProgress::NotSent() const
	{
	return iNotSent;	
	}


/** Gets the number of messages that the MTM failed to send this session.

For example, failure could be because the SMTP server refused to accept a message.

@return Number of messages
*/
EXPORT_C TInt TImSmtpProgress::FailedToSend() const
	{
	return iFailedToSend;	
	}


/** Gets the total number of messages that the MTM is attempting to send in this session. 

The following equation is always true: Sent() + NotSent() + FailedToSend() = SendTotal().

@return Number of messages
*/
EXPORT_C TInt TImSmtpProgress::SendTotal() const
	{
	return iSendTotal;	
	}


/** Set the progress variables to initial values.

The "send total" and "not sent" variables are set to aTotal. Other counter
variables are set to 0. The initial status is set to EMsgOutboxProgressWaiting.

@param aTotal Number of messages to send
*/
EXPORT_C void TImSmtpProgress::InitialiseTotal(const TInt& aTotal)
	{
	iNotSent=aTotal;
	iSendTotal=aTotal;
	iSent=0;
	iFailedToSend=0;
	iError=0;
	iMsgNo=0;
	iSendFileProgress.iBytesSent=0;
	iSendFileProgress.iBytesToSend=0;
	iSendFileProgress.iSessionState=EConnectingToSmtp;
	iStatus=EMsgOutboxProgressWaiting;
	}


/** Updates progress variables for when a message is successfully sent.

The "message sent" count is incremented; the "not sent" count is decremented.
*/
EXPORT_C void TImSmtpProgress::UpdateSent()
	{
	iNotSent--;
	iSent++;
	}


/** Updates progress variables for when a message fails to be sent.

The "failed to send" count is incremented; the "not sent" count is decremented.
*/
EXPORT_C void TImSmtpProgress::UpdateFailedToSend()
	{
	iNotSent--;
	iFailedToSend++;	// only changed if message was couldn't be sent
	}


/** Decrements the send total.

This can be used to adjust the total if a message is found to be deleted 
before it can be sent. 
*/
EXPORT_C void TImSmtpProgress::DecrementSendTotal()
	{
	// used to lower send total when I discover one message in collection 
	// which has been deleted before I had a chance to send it.
	iSendTotal--;
	iNotSent--;
	}


/** Gets the message server entry ID of the SMTP service being used.

@return Message server entry ID of the SMTP service
*/
EXPORT_C TMsvId TImSmtpProgress::ServiceId() const
	{
	return iServiceId;
	}


/** Sets the message server entry ID of the SMTP service being used.

@param aServiceId Message server entry ID of the SMTP service
*/
EXPORT_C void TImSmtpProgress::SetServiceId(TMsvId aServiceId)
	{
	iServiceId = aServiceId;
	}


EXPORT_C TInt TImSmtpProgress::ConnectionState() const
/**
Retrieves the stage of the connection process as defined in nifvar.h and csdprog.h
while the service is connecting to the SMTP server.

@return
KLinkLayerOpen if the SMTP service has successfully connected, or
KErrNotFound if the SMTP service is disconnected, or 
the current connection stage (declared in nifvar.h and csdprog.h) while
establishing a connection.
*/
	{
	switch(iStatus )
		{
	case EMsgOutboxProgressConnecting:
		// Return the connection state temporarily stored in iMsgNo
		//  while connection is in progress
		return iMsgNo;
	case EMsgOutboxProgressWaiting:
		// If we're definately in a disconnected state, return KErrNotFound
		return KErrNotFound;
	default:
		// If we're in one of the connected states, return KLinkLayerOpen
		return KLinkLayerOpen;
		}
	}

EXPORT_C TInt TImSmtpProgress::ConnectionIAP() const
/**
Retrieves the internet access point that is used when connecting to the
SMTP service.

@return
KErrNotFound if the SMTP service is not connecting, otherwise the
internet access point number.
*/
	{
	if (Status() == EMsgOutboxProgressConnecting)
		{
		// Return the connection IAP temporarily stored in iSent
		// while connection is in progress
		return iSent;
		}
	else
		{
		return KErrNotFound;
		}
	}

EXPORT_C void TImSmtpProgress::SetConnectionIAP(TInt aConnectionIAP)
/**
Sets the ConnectionIAP value.

@param aConnectionIAP
The value that the ConnectionIAP will be set to.
*/
	{
	// Only store the Connection IAP value in iSent if we're in the Connectiong state
	if (Status() == EMsgOutboxProgressConnecting)
		{
		iSent = aConnectionIAP;
		}
	}
