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

#include "POP3SET.H"
#include <nifvar.h>				// For KLinkLayerOpen


GLREF_C TPtrC8 LimitStringSize(const TPtrC8& aString, TInt aMaxSize);

const TInt KPOP3PopulateOptionsVersion=1;

/**
Creates and initialises a TImPop3PopulateOptions using data from a packaged
descriptor to initialise members. 

@param aDesc
descriptor containing a packaged instance of either TImPop3PopulateOptions

@param aUnpackTo
Used to return the unpacked options class

*/
EXPORT_C void TImPop3PopulateOptions::UnpackL(const TDesC8& aDesc, TImPop3PopulateOptions &aUnpackTo)

	{
	TPckgBuf<TImPop3PopulateOptions> options;
	options.Copy(aDesc);
	aUnpackTo=options();
	}

EXPORT_C TImPop3PopulateOptions::TImPop3PopulateOptions() :
		iVersion(KPOP3PopulateOptionsVersion),
		iMaxEmailSize(KMaxTInt),
		iPopulationLimit(-1)
	{

	}


EXPORT_C TInt TImPop3PopulateOptions::MaxEmailSize()
	{
	return iMaxEmailSize;
	}

EXPORT_C void TImPop3PopulateOptions::SetMaxEmailSize(TInt aMaxEmailSize)
	{
	iMaxEmailSize=aMaxEmailSize;
	}

EXPORT_C TInt TImPop3PopulateOptions::PopulationLimit()
	{
	return iPopulationLimit;
	}

EXPORT_C void TImPop3PopulateOptions::SetPopulationLimit(TInt aPopulationLimit)
	{
	iPopulationLimit=aPopulationLimit;
	}
		


EXPORT_C TInt TPop3Progress::ConnectionState() const
/**
Retrieves the stage of the connection process as defined in nifvar.h and csdprog.h
while the service is connecting to the POP3 server.

@return
KLinkLayerOpen if the POP3 service has successfully connected, or
KErrNotFound if the POP3 service is disconnected, or 
the current connection stage (declared in nifvar.h and csdprog.h) while
establishing a connection.
*/
	{
	switch(iPop3Progress )
		{
	case EPopConnecting:
		// Return the connection state temporarily stored in iTotalMsgs
		// while connection is in progress
		return iTotalMsgs;
	case EPopDisconnected:
		// If we're definately in a disconnected state, return KErrNotFound
		return KErrNotFound;
	default:
		// If we're in one of the connected states, return KLinkLayerOpen
		return KLinkLayerOpen;
		}
	}


EXPORT_C TInt TPop3Progress::ConnectionIAP() const
/**
Retrieves the internet access point that is used when connecting to the
POP3 service.

@return
KErrNotFound if the POP3 service is not connecting, otherwise the
internet access point number.
*/
	{
	if (iPop3Progress == EPopConnecting)
		{
		// Return the connection IAP temporarily stored in iTotalBytes
		// while connection is in progress
		return iTotalBytes;
		}
	else
		{
		return KErrNotFound;
		}
	}


EXPORT_C CImPop3Settings::CImPop3Settings()
/** 
Allocates and creates a new CImPop3Settings object initialised to
default values.
*/
	{
	Reset();
	} 


EXPORT_C CImPop3Settings::~CImPop3Settings()
/**
Destructor.
*/
	{
	Reset();
	
	delete iExtension;
	iExtension = NULL;
	} 


EXPORT_C void CImPop3Settings::Reset()
/**
Resets the settings to their default values.
*/
	{
	CImBaseEmailSettings::Reset();
	
	iPortNumber = KPOP3DefaultPortNumber;

	iInboxSyncLimit=KImPop3SynchroniseAll;
	iMaxEmailSize = KMaxTInt;

	if (Extension())
		{
		delete Extension()->iLoginName;
		Extension()->iLoginName=NULL;

		delete Extension()->iPassword;
		Extension()->iPassword = NULL;
		
		delete Extension()->iTlsSslDomain;
		Extension()->iTlsSslDomain=NULL;
		
		// no population limit
		Extension()->iPopulationLimit=-1;
		}

	SetApop(EFalse); 
	}


EXPORT_C const TPtrC8 CImPop3Settings::LoginName() const
/**
Retrieves the configured login user name (mailbox) of the POP3 service.

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


EXPORT_C const TPtrC8 CImPop3Settings::Password() const
/**
Retrieves the configured plain text login password that will be sent to the POP3
server if APOP is not configured.

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


EXPORT_C TBool CImPop3Settings::Apop() const
/**
Checks whether the POP3 service is configured to log on using plain text
authentication or APOP encrypted authentication.

The default setting is EFalse.

@see
SetApop()

@return
ETrue if using APOP encrypted authentication.
EFalse if using plain text authentication.
*/
	{
	return iFlags & KPop3ApopFlag;
	}


EXPORT_C TBool CImPop3Settings::AutoSendOnConnect() const
/**
Checks whether queued emails associated with the same internet access point
as the POP3 service will be sent first before logging onto the POP3 server.
The internet access point for each email service (POP3, IMAP4, SMTP) is configured
using CImIAPPreferences.
 
@see CImIAPPreferences

@return 
ETrue if configured to send emails after the connection is established, but before
authentication with the POP3 server.
*/
	{
	return iFlags & KPop3AutoSendFlag;
	}


EXPORT_C void CImPop3Settings::SetLoginNameL(const TDesC8& aLoginName)
/**
Configures the login user name (mailbox) for the POP3 service.

@param aLoginName
The plain text login user name. A copy of aLoginName is made so if you
want to change the user name you must call this method again for the
changes to take effect.
*/
	{
	CheckExtensionExistsL();
	HBufC8* newLoginName = aLoginName.AllocL();
	delete Extension()->iLoginName;
	Extension()->iLoginName = newLoginName;
	}


EXPORT_C void CImPop3Settings::SetPasswordL(const TDesC8& aPassword)
/**
Configures the login password for the POP3 service. The plain text password
will be sent during authentication with the POP3 server. It can also be
used to store the shared secret when authenticating using APOP if SetApop() is
enabled.

@param aPassword
The plain text login password. A copy of aPassword is made so if you
want to change the user name you must call this method again for the
changes to take effect.
*/
	{
	CheckExtensionExistsL();
	HBufC8* newPassword = aPassword.AllocL();
	delete Extension()->iPassword;
	Extension()->iPassword = newPassword;
	}


EXPORT_C void CImPop3Settings::SetApop(TBool aFlag)
/** 
Configures the POP3 service to authenticate using either a plain text password
or an APOP encrypted digest.

If configured to use APOP authentication, the configured password set by the call
to SetPasswordL() is used to store the shared secret between the server and the POP3
service. The timestamp sent in the servers' hello banner and the password are joined
and encrypted into a digest before being sent using the MD5 algorithm.

The default setting is EFalse.

@param aFlag
Specify ETrue to use APOP encrypted authentication.
Specify EFalse to use plain text authentication.
*/
	{
	iFlags = (iFlags & ~KPop3ApopFlag) | (aFlag ? KPop3ApopFlag : KPop3SettingsClearFlag);
	}


EXPORT_C void CImPop3Settings::SetAutoSendOnConnect(TBool aFlag)
/** 
Configures the POP3 service to send queued emails associated with the same internet
access point to be sent first before logging onto the POP3 server.

The internet access point for each email service (POP3, IMAP4, SMTP) is configured
using CImIAPPreferences.
 
@see CImIAPPreferences

@param aFlag 
Specify ETrue to send emails after the connection is established, but before
authentication with the POP3 service.
Specify EFalse to disable this functionality.
*/
	{
	iFlags = (iFlags & ~KPop3AutoSendFlag) | (aFlag ? KPop3AutoSendFlag : KPop3SettingsClearFlag);
	}


EXPORT_C TBool CImPop3Settings::DisconnectedUserMode() const
/**
Checks if disconnected user mode is configured.

If disconnected user mode is enabled by calling SetDisconnectedUserMode(), 
then the POP3 client MTM (CPop3ClientMtm) will accept message operations, 
such as deleting messages from a server, while the user is offline.
The operations are stored and executed when the user connects again.

@see
CPop3ClientMtm

@return 
ETrue if disconnected user mode is enabled.
*/
	{
	return iFlags & KPop3DisconnectedModeFlag;
	}


EXPORT_C void CImPop3Settings::SetDisconnectedUserMode(TBool aFlag)
/** 
Enables the POP3 client MTM (CPop3ClientMtm) to accept message operations
while disconnected.

If disconnected user mode is enabled the POP3 client MTM will accept message
operations such as deleting messages from a server, while the user is offline.
The operations are stored and executed when the user connects again.

@see
CPop3ClientMtm

@param aFlag
Specify ETrue to enable disconnected user mode.
Specify EFalse to disable disconnected user mode.
*/
	{
	iFlags = (iFlags & ~KPop3DisconnectedModeFlag) | (aFlag ? KPop3DisconnectedModeFlag : KPop3SettingsClearFlag);
	}


EXPORT_C TBool CImPop3Settings::DeleteEmailsWhenDisconnecting() const
/** 
Checks whether the caller has enabled this setting by calling 
SetDeleteEmailsWhenDisconnecting().

This flag is not used by the Messaging subsystem.

@see
CPop3ClientMtm

@return
ETrue if this flag is set.
*/
	{
	return iFlags & KPop3DeleteEmailsWhenDisconnectingFlag;
	}


EXPORT_C void CImPop3Settings::SetDeleteEmailsWhenDisconnecting(TBool aFlag)
/** 
Sets or resets a flag.

This flag is not used by the Messaging subsystem.

@param aFlag
Specify ETrue to set this flag.
*/
	{
	iFlags = (iFlags & ~KPop3DeleteEmailsWhenDisconnectingFlag) | (aFlag ? KPop3DeleteEmailsWhenDisconnectingFlag : KPop3SettingsClearFlag);
	}


EXPORT_C TBool CImPop3Settings::AcknowledgeReceipts() const
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

@see
CImHeader
CImEmailOperation
CSmtpClientMtm

@return
ETrue if this flag is set.
*/
	{
	return iFlags & KPop3AcknowledgeReceipts;
	}


EXPORT_C void CImPop3Settings::SetAcknowledgeReceipts(TBool aFlag)
/** 
Sets or resets a flag.

This flag is a convenience setting for Messaging client applications that may 
wish to store whether or not to send a receipt email message to the sender when the
email has been received. The Messaging subsystem does not use this setting, 
and has to be explicitly commanded by the Messaging client application to
send a receipt notification. Messaging applications can use CImHeader to check
if a message has a return receipt address. Receipt notification messages can be
created using CImEmailOperation, and sent using the SMTP client MTM.

@see
CImHeader
CImEmailOperation
CSmtpClientMtm

@param aFlag
Specify ETrue to set this flag.
*/
	{
	iFlags = (iFlags & ~KPop3AcknowledgeReceipts) | (aFlag ? KPop3AcknowledgeReceipts : KPop3SettingsClearFlag);
	}


EXPORT_C TInt CImPop3Settings::MaxEmailSize() const
/** 
Returns the maximum message size in bytes to be downloaded. This
value is set by a calling SetMaxEmailSize().

The Messaging subsystem does not use this setting. The default setting is KMaxInt.

Messaging applications can use CImPOP3GetMail or CPop3ClientMtm to specify
a maximum email size to retrieve by passing a TImPop3GetMailInfo parameter in
method calls.

@see
TImPop3GetMailInfo
CImPOP3GetMail
CPop3ClientMtm

@return
The maximum message size in bytes.
*/
	{
	return iMaxEmailSize;
	}


EXPORT_C void CImPop3Settings::SetMaxEmailSize(const TInt aMaxEmailSize)
/** 
Sets the maximum message size in bytes to be downloaded.

The Messaging subsystem does not use this setting. The default setting is KMaxInt.

Messaging applications can use CImPOP3GetMail or CPop3ClientMtm to specify
a maximum email size to retrieve by passing a TImPop3GetMailInfo parameter in
method calls.

@see
TImPop3GetMailInfo
CImPOP3GetMail
CPop3ClientMtm

@param aMaxEmailSize
Specifies the maximum message size in bytes.
*/
	{
	iMaxEmailSize = aMaxEmailSize;
	}


EXPORT_C TPop3GetMailOptions CImPop3Settings::GetMailOptions() const
/**
Retrieves an enumeration that specifies whether only the messages headers
or the entire message will be downloaded when synchronising with the POP3 server.
This is specified by calling SetGetMailOptions().

@see
TPop3GetMailOptions

@return
An enumeration specifying how messages will be synchronised.
*/
	{
	return iGetMailOptions;
	}


EXPORT_C void CImPop3Settings::SetGetMailOptions(TPop3GetMailOptions aGetMailOptions)
/** 
Specifies whether to download headers only, or entire messages when
synchronising with the POP3 server.

@see
TPop3GetMailOptions

@param aGetMailOptions
Specifies whether to download headers only, or entire messages when
synchronising with the POP3 server.
*/
	{
	iGetMailOptions = aGetMailOptions;
	}


EXPORT_C TInt32 CImPop3Settings::InboxSynchronisationLimit() const
/** 
Returns whether or not the POP3 service will synchronise with all
emails on the server after connecting, or the maximum number of new messages 
that will be synchronised if there are new messages on the server. This
setting is configured by calling SetInboxSynchronisationLimit().

The default setting is to synchronise all messages.

@return
If negative, the POP3 service will synchronise all emails after connecting.
If zero, the POP3 service will not synchronise after connecting.
If positive, the POP3 service will synchronise to the n most recent emails,
where n is this return value.
*/
	{
	return iInboxSyncLimit ;
	}


EXPORT_C void CImPop3Settings::SetInboxSynchronisationLimit(const TInt32 aInboxSyncLimit)
/** 
Specifies whether or not the POP3 service will synchronise with all
emails on the server after connecting, or the maximum number of new messages 
that will be synchronised if there are new messages on the server.

The default setting is to synchronise all messages.

@param aInboxSyncLimit
If negative, the POP3 service will synchronise all emails after connecting.
If zero, the POP3 service will not synchronise after connecting.
If positive, the POP3 service will synchronise to the n most recent emails,
where n is this return value.
*/
	{
	iInboxSyncLimit = aInboxSyncLimit;
	}

/** 
Retrieves the currently stored TOP population limit from settings.
If this is set, it does not mean that this limit is applied but rather
is just a convenient place to store the current limit
*/
EXPORT_C TInt32 CImPop3Settings::PopulationLimit() const
	{
	if(Extension())
		{
		return Extension()->iPopulationLimit;
		}
	else
		{
		// default to no poulation limit if none has been set
		return -1;
		}
	}


/** 
Sets the currently stored TOP population limit to settings.
If this is set, it does not mean that this limit is applied but rather
is just a convenient place to store the current limit, the limit must be passed in
the parameters of the command
*/
EXPORT_C void CImPop3Settings::SetPopulationLimitL(const TInt32 aPopulationLimit)
	{
	CheckExtensionExistsL();
	Extension()->iPopulationLimit = aPopulationLimit;
	}
	

EXPORT_C CImPop3Settings& CImPop3Settings::CopyL(const CImPop3Settings& aCImPop3Settings)
/**
Copies the configuration settings from another POP3 settings object into this object.

@param aCImPop3Settings
Specifies the object to copy.

@return
This object after copying.
*/
	{
	// CheckExtensionExistsL() called at the start of the function, since it's allocation will be longer lived
	CheckExtensionExistsL();
	Reset();
	CImBaseEmailSettings::CopyL(aCImPop3Settings);
	Extension()->iLoginName		=	aCImPop3Settings.LoginName().AllocL();
	Extension()->iPassword		=	aCImPop3Settings.Password().AllocL();
	Extension()->iTlsSslDomain 	= 	aCImPop3Settings.TlsSslDomain().AllocL();
	iMaxEmailSize			=	aCImPop3Settings.iMaxEmailSize;
	iGetMailOptions			=	aCImPop3Settings.GetMailOptions();
	iInboxSyncLimit			=	aCImPop3Settings.InboxSynchronisationLimit();
	SetPopulationLimitL(aCImPop3Settings.PopulationLimit());
	return (*this);
	}


EXPORT_C TBool CImPop3Settings::operator==(const CImPop3Settings& aCImPop3Settings) const
/**
Equality operator.

@param aCImPop3Settings
Specifies the POP3 settings object to compare with.

@return
ETrue if the settings are the same.
*/
	{
	return 	((MaxEmailSize() == aCImPop3Settings.iMaxEmailSize) &&
		 !(LoginName().Compare(aCImPop3Settings.LoginName())) &&
		 !(Password().Compare(aCImPop3Settings.Password())) &&
		 (CImBaseEmailSettings::operator==(aCImPop3Settings)) &&	 
		 (InboxSynchronisationLimit()==aCImPop3Settings.InboxSynchronisationLimit()) &&
		 (PopulationLimit()==aCImPop3Settings.PopulationLimit()));
	}

/**
Sets the domain name to use during TLS/SSL certificate validation.

@param aDomainName Domain name
*/
EXPORT_C void CImPop3Settings::SetTlsSslDomainL(const TDesC8& aDomainName)
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
EXPORT_C TPtrC8 CImPop3Settings::TlsSslDomain() const
	{		
	if (Extension() && Extension()->iTlsSslDomain)		
		return TPtrC8(*(Extension()->iTlsSslDomain));
	return TPtrC8();
	}


#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
/**
Enables POP3 authentication using CRAM-MD5 when connecting to email server.
Read RFC 2195 "IMAP/POP Authorize Extension for Simple Challenge/Response" for more details.

@param aFlag
ETrue to enable POP3 authentication.
*/
EXPORT_C void CImPop3Settings::SetPOP3Auth(TBool aFlag)
	{
	iFlags = (iFlags & ~KPop3AuthFlag) | (aFlag ? KPop3AuthFlag : KPop3SettingsClearFlag);
	}


/**
Retrieves the value of POP3 authentication flag.

@return
ETrue if POP3 authentication is enabled.
*/

EXPORT_C TBool CImPop3Settings::POP3Auth() const
	{
	return iFlags & KPop3AuthFlag;
	}
	

/**
Enables less secure authentication mechanism (PLAIN or LOGIN), when email server doesn't support CRAM-MD5 authentication. 
Fallback flag will be used in conjunction with Authenticate flag and will be used when Authenticate flag is enabled.
By default Fallback flag will be EFalse.

@param aFlag
ETrue to enable Fallback option for less secure authentication.
*/
EXPORT_C void CImPop3Settings::SetFallBack(TBool aFlag)
	{
	iFlags = (iFlags & ~KPop3FallbackFlag) | (aFlag ? KPop3FallbackFlag : KPop3SettingsClearFlag);
	}


/**
Retrieves the value of Fallback flag.

@return
ETrue if Fallback flag is enabled.
*/
EXPORT_C TBool CImPop3Settings::FallBack() const
	{
	return iFlags & KPop3FallbackFlag;
	}
#endif
