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

#include "IMAPSET.H"
#include <nifvar.h>				// For KLinkLayerOpen


GLREF_C TPtrC8 LimitStringSize(const TPtrC8& aString, TInt aMaxSize);


EXPORT_C CImImap4Settings::CImImap4Settings()
/** 
Allocates and creates a new CImImap4Settings object initialised to
default values.
*/
	{
	Reset();
	} 


EXPORT_C CImImap4Settings::~CImImap4Settings()
/**
Destructor.
*/
	{
	Reset();
	
	delete iExtension;
	iExtension = NULL;
	} 


EXPORT_C void CImImap4Settings::Reset()
/**
Resets the settings to their default values.
*/
	{
	CImBaseEmailSettings::Reset();
	
	SetPort(KIMAPDefaultPortNumber);
	SetImapIdle(ETrue);

	iInboxSyncLimit=KImImapSynchroniseAll;
	iMailboxSyncLimit=KImImapSynchroniseAll;
	iMaxEmailSize = KMaxTInt;
	
	iPathSeparator='\0';

	iSynchroniseStrategy=EUseRemote;
	iSubscriptionStrategy=EUpdateNeither;

	if (Extension())
		{
		delete Extension()->iLoginName;
		Extension()->iLoginName=NULL;

		delete Extension()->iPassword;
		Extension()->iPassword = NULL;

		delete Extension()->iFolderPath;
		Extension()->iFolderPath = NULL;

		delete Extension()->iTlsSslDomain;
		Extension()->iTlsSslDomain = NULL;
		
		delete Extension()->iSearchString;
		Extension()->iSearchString = NULL;
		Extension()->iSyncRate = KImapDefaultInboxSyncRate;
		Extension()->iImapIdleTimeout = KImapDefaultIdleTimeout;
		Extension()->iFetchSizeBytes = KImapDefaultFetchSizeBytes;
		Extension()->iPartialMailOptions = ENoSizeLimits;
		Extension()->iBodyTextSizeLimit = KMaxTInt;
		Extension()->iAttachmentSizeLimit = KMaxTInt;
		}
	}


EXPORT_C const TPtrC8 CImImap4Settings::LoginName() const
/**
Retrieves the configured login user name (mailbox) of the IMAP4 service.

@see
SetLoginNameL()

@return 
The user name.
*/
	{
	if (Extension() && Extension()->iLoginName)
		return TPtrC8(*(Extension()->iLoginName));
	return TPtrC8();
	}


EXPORT_C const TPtrC8 CImImap4Settings::Password() const
/**
Retrieves the configured login password that will be sent during
authentication with the IMAP4 server.

@see
SetPasswordL()

@return 
The password.
*/
	{
	if (Extension() && Extension()->iPassword)
		return TPtrC8(*(Extension()->iPassword));
	return TPtrC8();
	}


EXPORT_C const TPtrC8 CImImap4Settings::FolderPath() const
/**
Retrieves the configured path of the IMAP4 mailbox on the server if the user's
primary mailbox is not in the Inbox folder.

Most of the time this setting will not have to be configured, however this may need
to be altered by the user for use with some IMAP4 servers. Refer to RFC 3501
Section 5.1 Mailbox Naming for more information.

The default setting is empty. 

@return
The folder path.
*/
	{
	if (Extension() && Extension()->iFolderPath)
		return TPtrC8(*(Extension()->iFolderPath));
	return TPtrC8();
	}

/**
Gets the search string used to filter which messages are synchronised.

For details, see SetSearchStringL().

@return Synchronisation filter string
*/
EXPORT_C const TPtrC8 CImImap4Settings::SearchString() const
	{
	if (Extension() && Extension()->iSearchString)
		return TPtrC8(*(Extension()->iSearchString));
	return TPtrC8();
	}

/**
Sets a search string for the service.

This setting allows a filter to be specified that prevents certain e-mail
messages from being synchronised onto the device when a client requests a
synchronisation. An IMAP Search command is issued to the server to provide a
list of messages to be synchronised.

Note that when a synchronisation command is given with a search string set:

- Mailbox synchronisation limits are ignored.
- Existing messages that do not match the search criteria, and that have not had their body fetched,
  are deleted locally.
- If the remote server does not support the filter, or the filter is malformed,
  the synchronisation command will return an error.

To remove a filter, set the search string to KNullDesC.

A complete overview of the syntax of search strings is given in section 6.4.4
(SEARCH Command) of RFC 3501. Some valid example search strings are:
@code 
FLAGGED SINCE 1-Feb-1994 NOT FROM "Smith"

BODY "some text in the body"
@endcode

@param aSearchString Synchronisation filter string
*/
EXPORT_C void CImImap4Settings::SetSearchStringL(const TDesC8& aSearchString)
	{
	CheckExtensionExistsL();
	HBufC8* newSearchString = aSearchString.AllocL();
	delete Extension()->iSearchString;
	Extension()->iSearchString = newSearchString;
	}

EXPORT_C TText8 CImImap4Settings::PathSeparator() const
/**
Retrieves the character configured to separate hierarchical mailbox names on
the IMAP4 server. Refer to RFC 3501 section 5.1.1 for more information.

The default setting an empty null-terminated string.

@return
The path separator character.
*/
	{
	return iPathSeparator;
	}


EXPORT_C TFolderSyncType CImImap4Settings::Synchronise() const
/**
Retrieves the method for synchronising folder information with the server. 

The default setting is EUseRemote.

@see
TFolderSyncType

@return
The folder synchronisation method.
*/
	{
	return iSynchroniseStrategy;
	}


EXPORT_C TFolderSubscribeType CImImap4Settings::Subscribe() const
/**
Retrieves the configured method for synchronising IMAP4 subscription information
with a server.

The default setting is EUpdateNeither.

@see
TFolderSubscribeType

@return
The subscription method.
*/
	{
	return iSubscriptionStrategy;
	}


EXPORT_C void CImImap4Settings::SetLoginNameL(const TDesC8& aLoginName)
/**
Configures the login user name (mailbox) for the IMAP4 service.

@param aLoginName
The login user name. A copy of aLoginName is made so if you
want to change the user name you must call this method again for the
changes to take effect.
*/	{
	CheckExtensionExistsL();
	HBufC8* newLoginName = aLoginName.AllocL();
	delete Extension()->iLoginName;
	Extension()->iLoginName = newLoginName;
	}


EXPORT_C void CImImap4Settings::SetPasswordL(const TDesC8& aPassword)
/**
Configures the login password for the IMAP4 service.

@param aPassword
The login password. A copy of aPassword is made so if you
want to change the user name you must call this method again for the
changes to take effect.
*/
	{
	CheckExtensionExistsL();
	HBufC8* newPassword = aPassword.AllocL();
	delete Extension()->iPassword;
	Extension()->iPassword = newPassword;
	}


EXPORT_C void CImImap4Settings::SetFolderPathL(const TDesC8& aFolderPath)
/**
Specifies the path to the IMAP4 mailbox on the server if the user's primary
mailbox is not in the Inbox folder.

Most of the time this setting will not have to be configured, however this may need
to be altered by the user for use with some IMAP4 servers. Refer to RFC 3501
Section 5.1 Mailbox Naming for more information.

The default setting is empty. 

@param aFolderPath
The folder path.
*/
	{
	CheckExtensionExistsL();
	HBufC8* newFolderPath = aFolderPath.AllocL();
	delete Extension()->iFolderPath;
	Extension()->iFolderPath = newFolderPath;
	}


EXPORT_C void CImImap4Settings::SetPathSeparator(const TText8 aPathSeparator)
/**
Specifies the character used to separate hierarchical mailbox names on
the IMAP4 server. Refer to RFC 3501 section 5.1.1 for more information.

The default setting an empty null-terminated string.

@param aPathSeparator
The path separator character.
*/
	{
	iPathSeparator = aPathSeparator;
	}


EXPORT_C void CImImap4Settings::SetSynchronise(const TFolderSyncType aType)
/** 
Sets the method for synchronising folder information with the server.

The default setting is EUseRemote.

@see
TFolderSyncType

@param aType
The folder synchronisation method.
*/
	{
	iSynchroniseStrategy = aType;
	}


EXPORT_C void CImImap4Settings::SetSubscribe(const TFolderSubscribeType aType)
/**
Sets the method for synchronising IMAP4 subscription information with a server.

The default setting is EUpdateNeither.

@see
TFolderSubscribeType

@param aType
The subscription method.
*/
	{
	iSubscriptionStrategy = aType;
	}


EXPORT_C TBool CImImap4Settings::DisconnectedUserMode() const
/**
Checks if disconnected user mode is configured.

If disconnected user mode is enabled by calling SetDisconnectedUserMode(), 
then the IMAP4 client MTM (CImap4ClientMtm) will accept message operations, 
such as deleting messages from a server, while the user is offline.
The operations are stored and executed when the user connects again.

@see
CImap4ClientMtm

@return 
ETrue if disconnected user mode is enabled.
*/
	{
	return iFlags & KImap4EmailDisconnectedModeFlag;
	}


EXPORT_C void CImImap4Settings::SetDisconnectedUserMode(TBool aFlag)
/** 
Enables the IMAP4 client MTM (CImap4ClientMtm) to accept message operations
while disconnected.

If disconnected user mode is enabled the IMAP4 client MTM will accept message
operations such as deleting messages from a server, while the user is offline.
The operations are stored and executed when the user connects again.

@see
CImap4ClientMtm

@param aFlag
Specify ETrue to enable disconnected user mode.
Specify EFalse to disable disconnected user mode.
*/
	{
	iFlags = (iFlags & ~KImap4EmailDisconnectedModeFlag) | (aFlag ? KImap4EmailDisconnectedModeFlag : KImap4EmailSettingsClearFlag);
	}


EXPORT_C TBool CImImap4Settings::AutoSendOnConnect() const
/**

 
@return 
ETrue if configured to send emails after the connection is established, but before
authentication with the IMAP4 server.
*/
	{
	return iFlags & KImap4EmailAutoSendFlag;
	}
	

EXPORT_C void CImImap4Settings::SetAutoSendOnConnect(TBool aFlag)
/** 
Configures the IMAP4 service to send queued emails associated with the same internet
access point to be sent first before logging onto the IMAP4 server.

The internet access point for each email service (POP3, IMAP4, SMTP) is configured
using CImIAPPreferences.
 
@see
CImIAPPreferences

@param aFlag 
Specify ETrue to send emails after the connection is established, but before
authentication with the IMAP4 service.
Specify EFalse to disable this functionality.
*/
	{
	iFlags = (iFlags & ~KImap4EmailAutoSendFlag) | (aFlag ? KImap4EmailAutoSendFlag : KImap4EmailSettingsClearFlag);
	}


EXPORT_C TUint CImImap4Settings::MaxEmailSize() const
/**
Returns the maximum message size in bytes to be downloaded.

This value is set by a calling SetMaxEmailSize(). The default setting is
KMaxInt.

Note that the value of this setting is not used by the IMAP MTM. It can be used
by client applications to store a user preference, and used when issuing get
commands with CImap4ClientMtm::InvokeAsyncFunctionL(). Such commands take a 
TImImap4GetMailInfo parameter, which has a iMaxEmailSize setting.

@see TImImap4GetMailInfo
@see CImImap4GetMail
@see CImap4ClientMtm

@return The maximum message size in bytes.
*/
	{
	return iMaxEmailSize;
	}


EXPORT_C void CImImap4Settings::SetMaxEmailSize(const TUint aMaxEmailSize)
/** 
Sets the maximum message size in bytes to be downloaded.

The default setting is KMaxInt.

Note that the value of this setting is not used by the IMAP MTM. It can be used
by client applications to store a user preference, and used when issuing get
commands with CImap4ClientMtm::InvokeAsyncFunctionL(). Such commands take a 
TImImap4GetMailInfo parameter, which has a iMaxEmailSize setting.

@see TImImap4GetMailInfo
@see CImImap4GetMail
@see CImap4ClientMtm

@return
The maximum message size in bytes.
*/
	{
	iMaxEmailSize = aMaxEmailSize;
	}


EXPORT_C TBool CImImap4Settings::DeleteEmailsWhenDisconnecting() const
/** 
Checks whether the caller has enabled messages marked to be deleted while offline
to be expunged from the server when disconnecting from the next online session.

Otherwise they are marked for delete and expunged during the initial
synchronisation.

To enable this functionality, call SetDeleteEmailsWhenDisconnecting().

@see
CImap4ClientMtm

@return
ETrue if emails are to be deleted during the disconnection phase.
*/
	{
	return iFlags & KImap4EmailDeleteEmailsWhenDisconnecting;
	}


EXPORT_C void CImImap4Settings::SetDeleteEmailsWhenDisconnecting(TBool aFlag)
/** 
If enabled, messages marked as deleted while offline will be deleted when
logging off from the next IMAP4 session. If disabled, messages are deleted
when the client synchronises with the server after a connection.

Enabling this functionality has the advantage of speeding up synchronisation
at the expense of disconnection speed.

@see
CImap4ClientMtm

@param aFlag
Specify ETrue to enable.
*/
	{
	iFlags = (iFlags & ~KImap4EmailDeleteEmailsWhenDisconnecting) | (aFlag ? KImap4EmailDeleteEmailsWhenDisconnecting : KImap4EmailSettingsClearFlag);
	}


EXPORT_C TBool CImImap4Settings::AcknowledgeReceipts() const
/** 
Checks whether the caller has enabled this setting by calling 
SetAcknowledgeReceipts().

This flag is a convenience setting for Messaging client applications that may 
wish to store whether or not to send a receipt email message to the sender when the
email has been received. The Messaging subsystem does not use this setting, 
and has to be explicitly commanded by the Messaging client application to
send a receipt notification. Messaging applications can use CImHeader to check
if a message has a return receipt address. Receipt notification messages can 
be created using CImEmailOperation, and sent using the SMTP client MTM.

@see CImHeader
@see CImEmailOperation
@see CSmtpClientMtm

@return
ETrue if this flag is set.
*/
	{
	return iFlags & KImap4EmailAcknowledgeReceipts;
	}


EXPORT_C void CImImap4Settings::SetAcknowledgeReceipts(TBool aFlag)
/** 
Sets or resets a flag.

This flag is a convenience setting for Messaging client applications that may 
wish to store whether or not to send a receipt email message to the sender when the
email has been received. The Messaging subsystem does not use this setting, 
and has to be explicitly commanded by the Messaging client application to
send a receipt notification. Messaging applications can use CImHeader to check
if a message has a return receipt address. Receipt notification messages can be
created using CImEmailOperation, and sent using the SMTP client MTM.

@see CImHeader
@see CImEmailOperation
@see CSmtpClientMtm

@param aFlag
Specify ETrue to set this flag.
*/
	{
	iFlags = (iFlags & ~KImap4EmailAcknowledgeReceipts) | (aFlag ? KImap4EmailAcknowledgeReceipts : KImap4EmailSettingsClearFlag);
	}

/**
Sets the domain name to use during TLS/SSL certificate validation.

@param aDomainName Domain name
*/
EXPORT_C void CImImap4Settings::SetTlsSslDomainL(const TDesC8& aDomainName)
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
EXPORT_C TPtrC8 CImImap4Settings::TlsSslDomain() const
	{
	if (Extension() && Extension()->iTlsSslDomain)
		return TPtrC8(*(Extension()->iTlsSslDomain));
	return TPtrC8();
	}
	
EXPORT_C TImap4GetMailOptions CImImap4Settings::GetMailOptions() const
/**
Gets a preference setting for which components of a message (headers, body,
attachments) to fetch when getting messages from a remote server.

Note that the value of this setting is not used by the IMAP MTM. It can be used
by client applications to store a user preference, and used when issuing get
commands with CImap4ClientMtm::InvokeAsyncFunctionL(). Such commands take a 
TImImap4GetMailInfo parameter, which has a TImap4GetMailOptions setting.

The setting is specified using SetGetMailOptions().

@see TImImap4GetMailInfo

@return Get mail message component preferences
*/
	{
	return iGetMailOptions;
	}


EXPORT_C void CImImap4Settings::SetGetMailOptions(TImap4GetMailOptions aGetMailOptions)
/**
Sets a preference setting for which components of a message (headers, body,
attachments) to fetch when getting messages from a remote server.

Note that the value of this setting is not used by the IMAP MTM. It can be used
by client applications to store a user preference, and used when issuing get
commands with CImap4ClientMtm::InvokeAsyncFunctionL(). Such commands take a 
TImImap4GetMailInfo parameter, which has a TImap4GetMailOptions setting.

@param aGetMailOptions Get mail message component preferences
*/
	{
	iGetMailOptions = aGetMailOptions;
	}


EXPORT_C TInt32 CImImap4Settings::InboxSynchronisationLimit() const
/**
Gets the maximum number of new messages in the Inbox that are synchronised when
a synchronisation takes place.

This setting is configured by calling SetInboxSynchronisationLimit(). To
configure limits for other folders, call SetMailboxSynchronisationLimit().

The default setting is to synchronise all messages in the Inbox folder.

@return If -1, all emails will be synchronised; if zero, no emails will
be synchronised; if positive, the return value is the maximum number of emails
that will be synchronised.
*/
	{
	return iInboxSyncLimit;
	}


EXPORT_C void CImImap4Settings::SetInboxSynchronisationLimit(const TInt32 aInboxSyncLimit)
/** 
Sets the maximum number of new messages in the Inbox that are synchronised when
a synchronisation takes place.

The default setting is to synchronise all messages in the Inbox folder.

@param aInboxSyncLimit
@return If -1, all emails will be synchronised; if zero, no emails will
be synchronised; if positive, the parameter specifies the maximum number of emails
that will be synchronised.
*/
	{
	iInboxSyncLimit = aInboxSyncLimit;
	}


EXPORT_C TInt32 CImImap4Settings::MailboxSynchronisationLimit() const
/**
Gets the maximum number of new messages, for folders other than the Inbox, that
are synchronised when a synchronisation takes place.

This setting is configured by calling SetMailboxSynchronisationLimit(). To
specify synchronisation limits for just the Inbox folder, call
SetInboxSynchronisationLimit().

The default setting is to synchronise all messages.

@return If -1, all emails will be synchronised; if zero, no emails will
be synchronised; if positive, the return value is the maximum number of emails
that will be synchronised.
*/
	{
	return iMailboxSyncLimit;
	}


EXPORT_C void CImImap4Settings::SetMailboxSynchronisationLimit(const TInt32 aMailboxSyncLimit)
/**
Sets the maximum number of new messages, for folders other than the Inbox, that
are synchronised when a synchronisation takes place.

The default setting is to synchronise all messages.

@param aMailboxSyncLimit
@return If -1, all emails will be synchronised; if zero, no emails will
be synchronised; if positive, the parameter specifies the maximum number of emails
that will be synchronised.
*/
	{
	iMailboxSyncLimit = aMailboxSyncLimit;
	}


EXPORT_C CImImap4Settings& CImImap4Settings::CopyL(const CImImap4Settings& aCImImap4Settings)
/**
Copies the configuration settings from another IMAP4 settings object into this object.

@param aCImImap4Settings
Specifies the object to copy.

@return
This object after copying.
*/
	{
	Reset();
	CheckExtensionExistsL();
	CImBaseEmailSettings::CopyL(aCImImap4Settings);
	Extension()->iLoginName       	= aCImImap4Settings.LoginName().AllocL();
	Extension()->iPassword        	= aCImImap4Settings.Password().AllocL();
	Extension()->iFolderPath      	= aCImImap4Settings.FolderPath().AllocL();
	Extension()->iTlsSslDomain 		= aCImImap4Settings.TlsSslDomain().AllocL();
	Extension()->iSearchString      = aCImImap4Settings.SearchString().AllocL();
	Extension()->iSyncRate	      = aCImImap4Settings.SyncRate();
	Extension()->iImapIdleTimeout = aCImImap4Settings.ImapIdleTimeout();
	Extension()->iFetchSizeBytes  = aCImImap4Settings.FetchSize();
	Extension()->iPartialMailOptions=aCImImap4Settings.PartialMailOptions();	
	Extension()->iBodyTextSizeLimit =aCImImap4Settings.BodyTextSizeLimit();
	Extension()->iAttachmentSizeLimit =aCImImap4Settings.AttachmentSizeLimit();
	iPathSeparator			=	aCImImap4Settings.PathSeparator();
	iSynchroniseStrategy	=	aCImImap4Settings.Synchronise();
	iSubscriptionStrategy	=	aCImImap4Settings.Subscribe();
	iMaxEmailSize			=	aCImImap4Settings.iMaxEmailSize;
	iGetMailOptions			=	aCImImap4Settings.iGetMailOptions;
	iInboxSyncLimit			=	aCImImap4Settings.InboxSynchronisationLimit();
	iMailboxSyncLimit		=	aCImImap4Settings.MailboxSynchronisationLimit();
	return (*this);
	}


EXPORT_C TBool CImImap4Settings::operator==(const CImImap4Settings& aCImImap4Settings) const
/**
Equality operator.

@param aCImImap4Settings
Specifies the IMAP4 settings object to compare with.

@return
ETrue if the settings are the same.
*/
	{
	return ((CImBaseEmailSettings::operator==(aCImImap4Settings)) &&
			(MaxEmailSize() == aCImImap4Settings.iMaxEmailSize) &&
			(GetMailOptions() == aCImImap4Settings.GetMailOptions()) &&
			!LoginName().Compare(aCImImap4Settings.LoginName()) &&
			!Password().Compare(aCImImap4Settings.Password()) &&
			!FolderPath().Compare(aCImImap4Settings.FolderPath()) &&
			!SearchString().Compare(aCImImap4Settings.SearchString()) &&
			(PathSeparator()	== aCImImap4Settings.PathSeparator()) &&
			(Synchronise()		== aCImImap4Settings.Synchronise()) &&
			(Subscribe()		== aCImImap4Settings.Subscribe()) &&
			(InboxSynchronisationLimit()==aCImImap4Settings.InboxSynchronisationLimit()) &&
			(MailboxSynchronisationLimit()==aCImImap4Settings.MailboxSynchronisationLimit()) &&
			(SyncRate()==aCImImap4Settings.SyncRate()) &&
			(ImapIdleTimeout()==aCImImap4Settings.ImapIdleTimeout()) &&
			(FetchSize()==aCImImap4Settings.FetchSize()) &&
			(PartialMailOptions()==aCImImap4Settings.PartialMailOptions()) &&
			(BodyTextSizeLimit()==aCImImap4Settings.BodyTextSizeLimit()) && 
			(AttachmentSizeLimit()==aCImImap4Settings.AttachmentSizeLimit()));
	} 


EXPORT_C TBool CImImap4Settings::UpdatingSeenFlags() const
/**
Retrieves whether the IMAP4 seen flag of new messages will be set on the server only
when the message has been marked as read, or when the message has been synchronised.

The default setting is EFalse which means that the seen flag will be set on the
server when the new message has been synchronised with the client. The messaging
application can mark the message as being read (seen) by calling TMsvEntry::SetUnread().

@see TMsvEntry

@return
Returns ETrue if the seen flag is set on the IMAP4 server only when
the message has been marked as read.
Returns EFalse if the seen flag is set on the IMAP4 server when the new message
is retrieved from the server. 
*/
	{
	return iFlags & KImap4EmailUpdatingSeenFlags;
	}


EXPORT_C void CImImap4Settings::SetUpdatingSeenFlags(TBool aFlag)
/**
Specifies whether the IMAP4 seen flag of messages will be set on the server only
when the message has been marked as read, or when the message has been synchronised.

The default setting is EFalse which means that the seen flag will be set on the
server when the new message has been synchronised with the client. The messaging
application can mark the message as being read (seen) by calling TMsvEntry::SetUnread().

@see TMsvEntry

@param aFlag
Specify ETrue if the seen flag is to be set on the IMAP4 server during synchronisation
only when the message has been marked as read.
Specify EFalse if the seen flag is to be set on the IMAP4 server when new messages
are retrieved from the server. 
*/
	{
	iFlags = (iFlags & ~KImap4EmailUpdatingSeenFlags) | (aFlag ? KImap4EmailUpdatingSeenFlags : KImap4EmailSettingsClearFlag);
	}


/**
Retrieves whether the IMAP IDLE command should be used as defined in RFC2177.

The default setting is ETrue which means that the IMAP IDLE command will be used
if the server supports it.

@return
Returns ETrue if the IMAP IDLE command should be used provided the server supports it.
Returns EFalse if the IMAP IDLE command should not be used. 
*/	
EXPORT_C TBool CImImap4Settings::ImapIdle() const
	{
	return iFlags & KImap4EmailIdleFlag;
	}


/**
Specifies whether the IMAP IDLE command should be used as defined in RFC2177.

The default setting is ETrue which means that the IMAP IDLE command will be used
if the server supports it.

@param aFlag
Specify ETrue if the IMAP IDLE command should be used provided the server supports it.
Specify EFalse if the IMAP IDLE command should not be used. 
*/
EXPORT_C void CImImap4Settings::SetImapIdle(TBool aFlag)
	{
	iFlags = (iFlags & ~KImap4EmailIdleFlag) | (aFlag ? KImap4EmailIdleFlag : KImap4EmailSettingsClearFlag);
	}


EXPORT_C TInt CImImap4Settings::SyncRate() const
/**
Retrieves the rate in seconds at which the Inbox is refreshed, sync rate is a non negative value.
The default setting is 300 seconds (5 minutes).
This setting applies only when the #KIMAP4MTMConnectAndSyncCompleteAfterDisconnect 
command is used to periodically resynchronise the Inbox.

@return
The Inbox refresh rate in seconds. A value of zero indicates that the
Inbox is not periodically refreshed (synchronised) when connected.
*/
	{
	if( Extension() )
		{
		return Extension()->iSyncRate;
		}

	// Return the default value
	return KImapDefaultInboxSyncRate;
	}


EXPORT_C void CImImap4Settings::SetSyncRateL(TInt aSyncRate)
/**
Specifies the rate in seconds at which the Inbox will be refreshed when connected.
The default setting is 300 seconds (5 minutes).
This setting applies only when the #KIMAP4MTMConnectAndSyncCompleteAfterDisconnect 
command is used to periodically resynchronise the Inbox.

@param 	aSyncRate  	The Inbox refresh rate in seconds. A value of zero specifies that 
					the Inbox will not periodically refresh (synchronise) when connected
					If the value is -ve, it leaves with KErrArgument
@leave 	KErrArgument Argument passed is invalid
		Other System wide error codes.	
*/
	{
	if(aSyncRate < 0)
		{
		User::Leave(KErrArgument);	
		}
	CheckExtensionExistsL();
	Extension()->iSyncRate = aSyncRate;
	}

EXPORT_C TInt32 CImImap4Settings::BodyTextSizeLimit() const
/**
Retrieves the maximum body text size limit for the partial fetch of message body.
If text/plain part + text/html part (alternative part) < body text size limit then 
fetch text/html part else fetch only text/plain part.

The default body text size limit is KMaxTInt.
The body text size limit is set by SetBodyTextSizeLimitL().

@return
The body text size limit for partial fetch of a message body text.
*/
	{
	if( Extension() )
		{
		return Extension()->iBodyTextSizeLimit;
		}

	// Return the default value
	return KMaxTInt;
	}

EXPORT_C void CImImap4Settings::SetBodyTextSizeLimitL(TInt32 aBodyTextSizeLimit)
/**
Specifies the maximum body text size limit for the partial fetch of message body.

The default body text size limit is KMaxTInt.

Note that the value of this setting is not used by the IMAP MTM. It can be used
by client applications to store a user preference, and used when issuing populate
commands with CImap4ClientMtm::InvokeAsyncFunctionL(). Such commands take a 
TImImap4GetPartialMailInfo parameter, which has a iBodyTextSizeLimit setting.

@param aBodyTextSizeLimit
The body text size limit for partial fetch of a message body text.
*/
	{
	CheckExtensionExistsL();
	Extension()->iBodyTextSizeLimit = aBodyTextSizeLimit;
	}

EXPORT_C TInt32 CImImap4Settings::AttachmentSizeLimit() const
/**
Retrieves the maximum attachment size limit for the partial fetch of attachments in
a message.

The default attachment size limit is KMaxTInt.
The attachment size limit is set by SetAttachmentSizeLimitL().

@return
The maximum attachment size limit for the partial fetch of attachments in a message.
*/
	{
	if( Extension() )
		{
		return Extension()->iAttachmentSizeLimit;
		}

	// Return the default value
	return KMaxTInt;
	}


EXPORT_C void CImImap4Settings::SetAttachmentSizeLimitL(TInt32 aAttachmentSizeLimit)
/**
Specifies the maximum attachment size limit for the partial fetch of attachments in
a message.

The default attachment size limit is KMaxTInt.

Note that the value of this setting is not used by the IMAP MTM. It can be used
by client applications to store a user preference, and used when issuing populate
commands with CImap4ClientMtm::InvokeAsyncFunctionL(). Such commands take a 
TImImap4GetPartialMailInfo parameter, which has a iAttachmentSizeLimit setting.

@param aAttachmentSizeLimit
The maximum attachment size limit for the partial fetch of attachments in a message.
*/
	{
	CheckExtensionExistsL();
	Extension()->iAttachmentSizeLimit = aAttachmentSizeLimit;
	}

EXPORT_C TImImap4PartialMailOptions CImImap4Settings::PartialMailOptions() const
/** Gets the option for how  

The specification can be as follows:

No limits on the body part, 
only body text, 
only attachments,
the entire message (both body text and attachment) with size limits on body text and attachment 
the entire message with total size limit (ECumulative - body text + attachment \<= total size limit) 
when synchronising with the IMAP4 server. 

The specification for the body parts to be downloaded is set by SetPartialMailOptionsL().

@see
TImImap4PartialMailOptions

@return
An enumeration specifying how messages will be synchronised.
*/
	{
	if(Extension())
		{
		return Extension()->iPartialMailOptions;
		}
	// Return the default value
	return ENoSizeLimits;
	}

EXPORT_C void CImImap4Settings::SetPartialMailOptionsL(TImImap4PartialMailOptions aPartialMailOptions)
/**
Specifies whether no limits on the body parts, only body text, only attachments,
the entire message (both body text and attachment) with size limits on body text
and attachment or the entire message with total size limit 
(ECumulative - body text + attachment \<= Total size limit)
will be downloaded when synchronising with the IMAP4 server.

Note that the value of this setting is not used by the IMAP MTM. It can be used
by client applications to store a user preference, and used when issuing populate
commands with CImap4ClientMtm::InvokeAsyncFunctionL(). Such commands take a 
TImImap4GetPartialMailInfo parameter, which has a iPartialMailOptions setting.

@see
TImImap4PartialMailOptions
@param aPartialMailOptions
Specifies whether to download whole message without limits, only body text,
only attachments, entire message with body text limit and attachment size limit
or the entire message with total size limit when synchronising with the IMAP4 server.
*/
	{
	CheckExtensionExistsL();
	Extension()->iPartialMailOptions = aPartialMailOptions;
	}


/**
Retrieves the IMAP IDLE timeout seconds.

The default setting is 1740 seconds (29 minutes).

@return
The IMAP IDLE timeout in seconds. A value of zero specifies that the 
IDLE command will not timeout.
*/
EXPORT_C TInt CImImap4Settings::ImapIdleTimeout() const
	{
	if( Extension() )
		{
		return Extension()->iImapIdleTimeout;
		}

	// Return the default value
	return KImapDefaultIdleTimeout;
	}


/**
Specifies the IMAP IDLE timeout in seconds.

The default setting is 1740 seconds (29 minutes).

@param  aIdleTimeout
The IMAP IDLE timeout in seconds. A value of zero specifies that the 
IDLE command will not timeout.
*/	
EXPORT_C void CImImap4Settings::SetImapIdleTimeoutL(TInt aIdleTimeout)
	{
	CheckExtensionExistsL();
	Extension()->iImapIdleTimeout = aIdleTimeout;
	}


/**
Retrieves the IMAP fetch size.

When downloading large attachments, data is requested (fetched) in increments. 
The larger the fetch size, fewer fetch commands are sent to the IMAP4 server.
This speeds up the attachment download rate, but only to a limit.  The limit is
network specific.  A fetch size that is too large will degrade performance shortly
after downloading an attachment is cancelled, because the fetched data will still
be incomming on the socket after cancellation. 

The default fetch size is 20480 bytes (20 KB).  The
default value is optimum for actual connection rates up to 2 Mbps.

@return
The IMAP fetch size in bytes.
*/
EXPORT_C TUint CImImap4Settings::FetchSize() const
	{
	TUint fetchSizeBytes = KImapDefaultFetchSizeBytes;
	if(Extension())
		fetchSizeBytes = Extension()->iFetchSizeBytes;
	
	return fetchSizeBytes;
	}


/**
Specifies the IMAP fetch size in bytes.

When downloading large attachments, data is requested (fetched) in increments. 
The larger the fetch size, fewer fetch commands are sent to the IMAP4 server.
This speeds up the attachment download rate, but only to a limit.  The limit is
network specific.  A fetch size that is too large will degrade performance shortly
after downloading an attachment is cancelled, because the fetched data will still
be incomming on the socket after cancellation. 

The default fetch size is 20480 bytes (20 KB).  The
default value is optimum for actual connection rates up to 2 Mbps.

@param  aFetchSizeBytes
The IMAP fetch size in bytes.
*/	
EXPORT_C void CImImap4Settings::SetFetchSizeL(TUint aFetchSizeBytes)
	{
	CheckExtensionExistsL();
	Extension()->iFetchSizeBytes = aFetchSizeBytes;
	}

/**
Sets a flag to indicate whether to use download rules during account synchronisation.

@param aFlag 
@return None
*/	
EXPORT_C void CImImap4Settings::SetUseSyncDownloadRules(TBool aFlag)
	{
	iFlags = (iFlags & ~KImap4EmailUseSyncDownloadRules) | (aFlag?KImap4EmailUseSyncDownloadRules : EBaseEmailSettingsClearFlag);
	}

/**
Indicates whether to use download rules during account synchronisation.

@param None
@return Boolean
ETrue if download rules should be used during account synchronisation
*/
EXPORT_C TBool CImImap4Settings::UseSyncDownloadRules()
	{
	if(iFlags & KImap4EmailUseSyncDownloadRules)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}		
	}

/**
Sets a flag to indicate whether folder tree synchronisation shall be disabled,
during account synchronisation operations.

@param aFlag 
@return None
*/	
EXPORT_C void CImImap4Settings::SetFolderSyncDisabled(TBool aFlag)
	{
	iFlags = (iFlags & ~KImap4EmailSettingsFolderSyncDisabled) | (aFlag?KImap4EmailSettingsFolderSyncDisabled : EBaseEmailSettingsClearFlag);
	}

/**
Indicates whether Folder Synchronisation is disabled during account synchronisation
operations.

@param None
@return Boolean
ETrue if folder tree synchronisation is disabled during account synchronisation.
*/
EXPORT_C TBool CImImap4Settings::FolderSyncDisabled()
	{
	if(iFlags & KImap4EmailSettingsFolderSyncDisabled)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}		
	}

EXPORT_C TInt TImap4GenericProgress::ConnectionState() const
/**
Retrieves the stage of the connection process as defined in nifvar.h and csdprog.h
while the service is connecting to the IMAP4 server.

@return
KLinkLayerOpen if the IMAP4 service has successfully connected, or
KErrNotFound if the IMAP4 service is disconnected, or 
the current connection stage (declared in nifvar.h and csdprog.h) while
establishing a connection.
*/
	{
	
	switch(iState)
		{
	case EConnecting:
		// Return the connection state temporarily stored in iMsgsDone
		//  while connection is in progress
		return iMsgsDone;
	case EDisconnected:
		// If we're definately in a disconnected state, return KErrNotFound
		return KErrNotFound;
	default:
		// If we're in one of the connected states, return KLinkLayerOpen
		return KLinkLayerOpen;
		}
	}


EXPORT_C TInt TImap4GenericProgress::ConnectionIAP() const
/**
Retrieves the internet access point that is used when connecting to the
IMAP4 service.

@return
KErrNotFound if the IMAP4 service is not connecting, otherwise the
internet access point number.
*/
	{
	if (iState == EConnecting)
		{
		// Return the connection IAP temporarily stored in iMsgsToDo
		//  while connection is in progress
		return iMsgsToDo;
		}
	else
		{
		return KErrNotFound;
		}
	}

/**
Check whether the KImap4EmailExpungeFlag is set in 
CImImap4Settings

@return
Returns ETrue if the KImap4EmailExpungeFlag was set in
CImImap4Settings::SetUseExpunge.
*/	
EXPORT_C TBool CImImap4Settings::UseExpunge() const
	{
	return iFlags & KImap4EmailExpungeFlag;
	}

/**
Allows KImap4EmailExpungeFlag to be set. It is used by the state machine in
CImapCompoundCopyToLocal,CImapCompoundCopyWithinService, CImapCompoundDelete and CImapFolder.
The state machines use the flag to decide whether to use EXPUNGE command to perform
the deletion.

The settings are written to the central repository.

@param  aFlag
Specify ETrue to enable EXPUNGE mode.
Specify EFalse to disable EXPUNGE mode.
*/
EXPORT_C void CImImap4Settings::SetUseExpunge(TBool aFlag)
	{
	iFlags = (iFlags & ~KImap4EmailExpungeFlag) | (aFlag ? KImap4EmailExpungeFlag : KImap4EmailSettingsClearFlag);
	}

#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)

/**
Retrieves the value of IMAP4 authentication flag.

@return
ETrue if IMAP4 authentication is enabled.
*/
EXPORT_C TBool CImImap4Settings::IMAP4Auth() const
	{
	return iFlags & KImap4EmailSettingsAuthenticationFlag;
	}
/**
Enables IMAP4 authentication using CRAM-MD5 when connecting to email server.
Read RFC 2195 "IMAP/POP Authorize Extension for Simple Challenge/Response" for more details.

@param aFlag
ETrue to enable IMAP4 authentication.

*/
EXPORT_C  void CImImap4Settings::SetIMAP4Auth(TBool aFlag)
	{
	iFlags = (iFlags & ~KImap4EmailSettingsAuthenticationFlag) | (aFlag ? KImap4EmailSettingsAuthenticationFlag : KImap4EmailSettingsClearFlag);
	}

/**
Retrieves the value of Fallback flag.

@return
ETrue if Fallback flag is enabled.

*/
EXPORT_C TBool CImImap4Settings::FallBack() const
	{
	return iFlags & KImap4EmailSettingsFallBackFlag;
	}
/**
Enables less secure authentication mechanism (PLAIN or LOGIN), when email server doesn't support CRAM-MD5 authentication. 
Fallback flag will be used in conjunction with Authenticate flag and will be used when Authenticate flag is enabled.
By default Fallback flag will be EFalse.

@param aFlag
ETrue to enable Fallback option for less secure authentication.

*/
EXPORT_C  void CImImap4Settings::SetFallBack(TBool aFlag)
	{
	iFlags = (iFlags & ~KImap4EmailSettingsFallBackFlag) | (aFlag ? KImap4EmailSettingsFallBackFlag : KImap4EmailSettingsClearFlag);
	}

#endif
