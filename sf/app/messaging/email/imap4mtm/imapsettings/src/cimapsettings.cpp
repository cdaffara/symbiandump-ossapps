// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// cimapsettings.cpp
//

#include "cimapsettings.h"
#include "imappaniccodes.h"

#include <barsc.h>
#include <barsread.h>		// TResourceReader
#include <bautils.h>
#include <cemailaccounts.h>
#include <f32file.h>
#include <imcm.rsg>			// resource definition for IMCV
#include <miuthdr.h>		// For KImEngineResourceFile
#include <msvutils.h>
#include <iapprefs.h>
#include <cimaptransportbuffersizes.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "timrfc822datefield.h"
#endif
const TInt KMaxOutstandingFetchRequests = 2;

/**
Static factory constructor

@return
The constructed CImapSettings object
*/
EXPORT_C CImapSettings* CImapSettings::NewL(CMsvServerEntry& aServerEntry)
	{
	CImapSettings* self = new (ELeave) CImapSettings(aServerEntry);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CImapSettings::~CImapSettings()
	{
	delete iServiceSettings;
	delete iIapPrefs;
	delete iDefaultPartialMessageFooter;
	delete iDefaultAttachmentName;
	delete iSyncDownloadRules;
	delete iTransportBufferSizes;
	}

CImapSettings::CImapSettings(CMsvServerEntry& aServerEntry)
: iServerEntry(aServerEntry)
	{
	iServiceId = aServerEntry.Entry().Id();
	}

void CImapSettings::ConstructL()
	{
	// create a new CImImap4Settings object initialised to default values.
	iServiceSettings = new (ELeave) CImImap4Settings;

	iIapPrefs = CImIAPPreferences::NewLC();
	CleanupStack::Pop(iIapPrefs);

	iSyncDownloadRules = CImapSyncDownloadRules::NewL();

	iTransportBufferSizes = CImapTransportBufferSizes::NewL();

	// read data from resource file (KImEngineResourceFile)
	ReadResourceFileL();
	}
	
/**
Returns whether the imap settings are loaded from the central repository.

@return ETrue  account settings have been loaded
		EFalse otherwise
*/
EXPORT_C TBool CImapSettings::SettingsLoaded() const
	{
	return iSettingsLoaded;
	}

/**
Loads the settings for the specified IMAP Service from the central repository.

Note that this function may modify the context of the CMsvServerEntry passed at
construction.

@param aId
The TMsvId for the IMAP Service (or any message entry below the service).
*/
EXPORT_C void CImapSettings::LoadSettingsL(const TMsvId aId)
	{
	// delete existing service settings
	delete iServiceSettings;
	iServiceSettings=NULL;

	// Get somewhere to store service settings
	iServiceSettings=new (ELeave) CImImap4Settings;

	delete iIapPrefs;
	iIapPrefs = NULL;
	iIapPrefs = CImIAPPreferences::NewLC();
	CleanupStack::Pop(iIapPrefs);

	delete iSyncDownloadRules;
	iSyncDownloadRules = NULL;
	iSyncDownloadRules = CImapSyncDownloadRules::NewL();

	delete iTransportBufferSizes;
	iTransportBufferSizes = NULL;
	iTransportBufferSizes = CImapTransportBufferSizes::NewL();

	// find the service
	User::LeaveIfError(iServerEntry.SetEntry(aId));
	while (iServerEntry.Entry().iType != KUidMsvServiceEntry)
		{
		User::LeaveIfError(iServerEntry.SetEntry(iServerEntry.Entry().Parent()));
		}

	// has the service ID changed?
	TMsvId serviceId = iServerEntry.Entry().Id();
	if (iServiceId != serviceId && iServiceId != 0)
		{
		TImapServerPanic::ImapPanic(TImapServerPanic::EImapSettingsInvalidService);
		}
		
	iServiceId = serviceId;

	// Get settings for this service
	CEmailAccounts* account = CEmailAccounts::NewLC();
 	TImapAccount accountId;
	accountId.iImapAccountId = iServerEntry.Entry().MtmData2();  // iMtmData2 of the service entry contains TImapAccountId
	accountId.iImapAccountName = iServerEntry.Entry().iDetails;
	accountId.iImapService = iServerEntry.Entry().iServiceId;
	accountId.iSmtpService = iServerEntry.Entry().iRelatedId;
	account->LoadImapSettingsL(accountId, *iServiceSettings);
	account->LoadImapIapSettingsL(accountId, *iIapPrefs);
	account->LoadImapSyncDownloadRulesL(accountId, *iSyncDownloadRules);
	account->LoadImapTransportBufferSizesL(*iTransportBufferSizes);
	CleanupStack::PopAndDestroy(account);

	if (iServiceSettings->PathSeparator() != 0)
		{
		// Store the path separator as a 16 bit descriptor.
		TText16 sep = iServiceSettings->PathSeparator();
		iPathSeparator.Des().Copy(&sep, 1);
		}

	iSettingsLoaded = ETrue;
	}

/**
Opens the client MTM resource files and reads the values which are used by the
server MTM. When all values are read, the resource file will be closed. The default 
resource file is defined by KImEngineResourceFile in miuthdr.h

@leave KErrNotFound if the resource file does not exist
*/
void CImapSettings::ReadResourceFileL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	//	NB need to check on all drives - won't necessarily be on
	//	the same one as IMPS.DLL
	TFileName fileName(KImEngineResourceFile);
	MsvUtils::AddPathAndExtensionToResFileL(fileName);
	BaflUtils::NearestLanguageFile( fs, fileName );
	
	if (!BaflUtils::FileExists(fs,fileName))
		{
		User::Leave(KErrNotFound);
		}
	RResourceFile resourceFile;
	resourceFile.OpenL(fs,fileName);		// Qualified: no leavescan error
	CleanupClosePushL(resourceFile);

	HBufC8* buffer = NULL;
	TResourceReader reader;
	
	// Read iStore8BitData flag.
	buffer = resourceFile.AllocReadLC( STORE_8BIT_BODY_TEXT );
	reader.SetBuffer(buffer);
	iStore8BitData = reader.ReadInt8();
	CleanupStack::PopAndDestroy(buffer);
	buffer = NULL;

	//Read iStorePlainBodyText flag
	buffer = resourceFile.AllocReadLC( STORE_PLAIN_BODY_TEXT );
	reader.SetBuffer(buffer);
	iStorePlainBodyText = reader.ReadInt8();
	CleanupStack::PopAndDestroy(buffer);
	buffer = NULL;
	
	// Read iDefaultAttachmentName
	buffer = resourceFile.AllocReadLC( DEFAULT_ATTACHMENT_NAME );
	reader.SetBuffer(buffer);
	
	iDefaultAttachmentName=reader.ReadTPtrC().AllocL();	
	
	CleanupStack::PopAndDestroy(buffer);
	buffer = NULL;

	// Read the string for remaining mail size for footer	
	buffer = resourceFile.AllocReadLC( PARTIAL_DOWNLOAD_FOOTER_MESSAGE );
	reader.SetBuffer(buffer);
	
	iDefaultPartialMessageFooter = buffer;
	// No need to destroy buffer-pointer, this class takes ownership
	CleanupStack::Pop(buffer);
	
	CleanupStack::PopAndDestroy(2, &fs);	// resourcefile, fs
	}
	
/**
Saves modified settings to the central repository.
*/
void CImapSettings::SaveSettingsL()
	{
	User::LeaveIfError(iServerEntry.SetEntry(iServiceId));

	// Update service entry
  	CEmailAccounts* account = CEmailAccounts::NewLC();
 	TImapAccount accountId;
	accountId.iImapAccountId = iServerEntry.Entry().MtmData2();  // iMtmData2 of the service entry contains TImapAccountId
	accountId.iImapAccountName = KNullDesC;  // So that account name is not updated
	accountId.iImapService = iServerEntry.Entry().iServiceId;
	accountId.iSmtpService = iServerEntry.Entry().iRelatedId;
 	account->SaveImapSettingsL(accountId, *iServiceSettings);
 	account->SaveImapIapSettingsL(accountId, *iIapPrefs);
 	account->SaveImapSyncDownloadRulesL(accountId, *iSyncDownloadRules);
 	account->SaveImapTransportBufferSizesL(*iTransportBufferSizes);
 	CleanupStack::PopAndDestroy(account);
	}

/**
Retrieves the service ID for the connected IMAP service.

If settings have not been loaded, this will return zero

@return
TMsvId for the IMAP service, or zero otherwise.
*/
EXPORT_C TMsvId CImapSettings::ServiceId() const
	{
	return iServiceId;
	}
	
/**
Retrieves the configured IP address or host name of the email server.

@return
Email server IP address if it is configured, or empty if not configured.
*/
EXPORT_C const TPtrC CImapSettings::ServerAddress() const
	{
	return iServiceSettings->ServerAddress();
	}

/**
Retrieves the port number of an email server when establishing
a TCP connection.

@return
Email server TCP port number.
*/
EXPORT_C TUint CImapSettings::Port() const
	{
	return iServiceSettings->Port();
	}

/** 
Retrieves whether a secure TLS connection will be negotiated over an
unsecure TCP socket connection when a connection is being established.

Refer to RFC 2595 - "Using TLS with IMAP, POP3 and ACAP" for information
on how a secure socket is established when retrieving email from a POP3
or IMAP4 server.

@return
ETrue if secure sockets are enabled.
*/
EXPORT_C TBool CImapSettings::SecureSockets() const
	{
	return iServiceSettings->SecureSockets();
	}

/**
Whether secure sockets will be used. 

Retrieves whether a secure TLS connection will be established 
directly over a TLS/SSL socket when connecting to the email server.

The well known port number for a secure TLS IMAP4 connection is 993.

@return
ETrue if a secure TLS connection will be attempted when establishing
a connection.
*/
EXPORT_C TBool CImapSettings::SSLWrapper() const
	{
	return iServiceSettings->SSLWrapper();
	}
	
/**
Whether bearer mobility is to be used.

@return
ETrue if the email service has been configured to use bearer mobility
*/
EXPORT_C TBool CImapSettings::BearerMobility() const
	{
	return iServiceSettings->BearerMobility();
	}


/**
Retrieves the configured login user name (mailbox) of the IMAP4 service.

@return 
The user name.
*/
EXPORT_C const TPtrC8 CImapSettings::LoginName() const
	{
	return iServiceSettings->LoginName();
	}

/**
Retrieves the configured login password that will be sent during
authentication with the IMAP4 server.

@return 
The password.
*/
EXPORT_C const TPtrC8 CImapSettings::Password() const
	{
	return iServiceSettings->Password();
	}

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
EXPORT_C const TPtrC8 CImapSettings::FolderPath() const
	{
	return iServiceSettings->FolderPath();
	}

/**
Retrieves the string to insert when performing search or fetch operations when
synchronising.

Allows the client to filter the messages to be synchronised.

@return
The search string.
*/
EXPORT_C const TPtrC8 CImapSettings::SearchString() const
	{
	return iServiceSettings->SearchString();
	}

/**
Retrieves the character configured to separate hierarchical mailbox names on
the IMAP4 server. Refer to RFC 3501 section 5.1.1 for more information.

The default setting an empty null-terminated string.

@return
The path separator character.
*/
EXPORT_C const TDesC& CImapSettings::PathSeparator() const
	{
	return iPathSeparator;
	}
	

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
EXPORT_C TBool CImapSettings::DisconnectedUserMode() const
	{
	return iServiceSettings->DisconnectedUserMode();
	}


/**
Retrieves the method for synchronising folder information with the server. 

The default setting is EUseRemote.

@see
TFolderSyncType

@return
The folder synchronisation method.
*/
EXPORT_C TFolderSyncType CImapSettings::Synchronise() const
	{
	return iServiceSettings->Synchronise();
	}

/**
Retrieves the configured method for synchronising IMAP4 subscription information
with a server.

The default setting is EUpdateNeither.

@see
TFolderSubscribeType

@return
The subscription method.
*/
EXPORT_C TFolderSubscribeType CImapSettings::Subscribe() const
	{
	return iServiceSettings->Subscribe();
	}

/** 
Checks whether the caller has enabled messages marked to be deleted while offline
to be expunged from the server when disconnecting from the next online session.

Otherwise they are marked for delete and expunged during the initial
synchronisation.

@return
ETrue if emails are to be deleted during the disconnection phase.
*/
EXPORT_C TBool CImapSettings::DeleteEmailsWhenDisconnecting() const
	{
	return iServiceSettings->DeleteEmailsWhenDisconnecting();
	}

/** 
Returns whether or not the IMAP4 service will synchronise the Inbox folder
after connecting, or the maximum number of new messages that will be synchronised
if there are new messages on the server.

@return
If negative, the IMAP4 service will synchronise all emails after connecting.
If zero, the IMAP4 service will not synchronise after connecting.
If positive, the IMAP4 service will synchronise to the n most recent emails,
where n is this return value.
*/
EXPORT_C TInt32 CImapSettings::InboxSynchronisationLimit() const
	{
	return iServiceSettings->InboxSynchronisationLimit();
	}

/** 
Returns whether or not the IMAP4 service will synchronise folders
other than the Inbox after connecting, or the maximum number of messages that
will be synchronised in the folders.

@return
If negative, the IMAP4 service will synchronise all emails after connecting.
If zero, the IMAP4 service will not synchronise after connecting.
If positive, the IMAP4 service will synchronise to the n most recent emails,
where n is this return value.
*/
EXPORT_C TInt32 CImapSettings::MailboxSynchronisationLimit() const
	{
	return iServiceSettings->MailboxSynchronisationLimit();
	}

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
EXPORT_C TBool CImapSettings::UpdatingSeenFlags() const
	{
	return iServiceSettings->UpdatingSeenFlags();
	}

/**
Retrieves the IMAP fetch size.

When downloading large attachments, data is requested (fetched) in increments. 
The larger the fetch size, fewer fetch commands are sent to the IMAP4 server.
This speeds up the attachment download rate, but only to a limit.  The limit is
network specific.  A fetch size that is too large will degrade performance shortly
after downloading an attachment is cancelled, because the fetched data will still
be incomming on the socket after cancellation. 

@return
The IMAP fetch size in bytes.
*/
EXPORT_C TUint CImapSettings::FetchSize() const
	{
	return iServiceSettings->FetchSize();
	}

/**
Retrieves whether the IMAP IDLE command should be used as defined in RFC2177.

The default setting is ETrue which means that the IMAP IDLE command will be used
if the server supports it.

@return
Returns ETrue if the IMAP IDLE command should be used provided the server supports it.
Returns EFalse if the IMAP IDLE command should not be used. 
*/	
EXPORT_C TBool CImapSettings::ImapIdle() const
	{
	return iServiceSettings->ImapIdle();
	}

/**
Retrieves the IMAP IDLE timeout in seconds.

@return
The IMAP IDLE timeout in seconds. A value of zero specifies that the 
IDLE command should not timeout.
*/	
EXPORT_C TInt CImapSettings::ImapIdleTimeout() const
	{
	return iServiceSettings->ImapIdleTimeout();
	}

/**
Retrieves the SSL domain name

@return SSL domain name
*/	
EXPORT_C TPtrC8 CImapSettings::TlsSslDomain() const
	{
	return iServiceSettings->TlsSslDomain();
	}

/**
Retrieves the IAP preferences.

@return
The IAP preferences.
*/	
EXPORT_C const CImIAPPreferences& CImapSettings::IAPPreferences() const
	{
	return *iIapPrefs;
	}

/**
Retrieves the value of the Store8BitData flag. The value of the flag is read
from a resource file during construction of this class.

@return
0 indicates that the message must be converted to 16 bits before storing in the
message store, every other value indicates that the message is not converted.
*/
EXPORT_C TInt CImapSettings::Store8BitData() const
	{
	return iStore8BitData;
	}

/**
Retrieves the default attachment name. The descriptor is read from a
resource file during construction of this class.

@return
The default attachment name descriptor as stored in the resource file.
*/
EXPORT_C const TDesC& CImapSettings::DefaultAttachmentName() const
	{
	__ASSERT_DEBUG( iDefaultAttachmentName, 
					TImapServerPanic::ImapPanic(TImapServerPanic::EImapSettingsNoDefaultAttachmentFile) );
	return *iDefaultAttachmentName;
	}

/**
Retrieves the default partial message footer. The descriptor is read from a
resource file during construction of this class.

@return
The the default partial message footer descriptor as stored in the resource file
*/
EXPORT_C const HBufC8* CImapSettings::DefaultPartialMessageFooter() const
	{
	__ASSERT_DEBUG( iDefaultPartialMessageFooter, 
					TImapServerPanic::ImapPanic(TImapServerPanic::EImapSettingsNoMessageFooter) );
	return iDefaultPartialMessageFooter;
	}


/**
Allows the path separator used by the server to be set. This parameter is
returned in the response to a LIST command.

The settings are written to the central repository.

@param  aPathSeparator
The path separator used by the remote server.
*/	
EXPORT_C void CImapSettings::SetPathSeparatorL(const TText8 aPathSeparator)
	{
	iServiceSettings->SetPathSeparator(aPathSeparator);
	SaveSettingsL();

	// Store the path separator as a 16 bit descriptor.
	TText16 sep = aPathSeparator;
	iPathSeparator.Des().Copy(&sep, 1);
	}

/**
Check whether the KImap4EmailExpungeFlag is set in 
CImImap4Settings

@return
Returns ETrue if the KImap4EmailExpungeFlag was set in
CImImap4Settings::SetUseExpunge.
*/
EXPORT_C TBool CImapSettings::UseExpunge() const
	{	
	return iServiceSettings->UseExpunge();
	}

/**
Indicates whether to use download rules during account synchronisation
operations.

@return ETrue if download rules should be used, EFalse if not
*/
EXPORT_C TBool CImapSettings::UseSyncDownloadRules() const
	{	
	return iServiceSettings->UseSyncDownloadRules();
	}

/**
Allows the current bearer type to be set.

@param aBearerType: the current bearer type
*/
EXPORT_C void CImapSettings::SetCurrentBearerType(TUint32 aBearerType)
	{
	iCurrentBearerType = aBearerType;
	}

/**
Gets the synchronisation download rule for the currently connected bearer type.

A rule is returned only if the bearer types associated with the rule contain
all the bearer types specified in the passed in value.

Invalid arguments passed to the routine will cause a panic on debug builds
but a leave with KErrArgument on a release build.

@param aType Type of folder the rule is for (inbox or other folder)
@param aMailInfo On successful completion, this holds the mail info for the rule

@return The position of the rule in the list, or KErrNotFound if no matching
        bearer type was found.
*/
EXPORT_C TInt CImapSettings::GetSyncDownloadRuleL(CImapSyncDownloadRules::TRulesType aType, TImImap4GetPartialMailInfo& aMailInfo) const
	{
	if (iCurrentBearerType != 0)
		{
		return iSyncDownloadRules->GetMailInfoL(aType, iCurrentBearerType, aMailInfo);
		}
	else
		{
		return KErrNotFound;	
		}
	
	}

/**
Gets the transport buffer sizes for the currently connected bearer type.

The buffer sizes specify the amount of data fetched for each fetch command,
and also the number of outstanding requests that may be pipelined at any one
time. These parameters are configurable on a per-bearer-type basis, via the
client mtms API CImapTransportBufferSizes.

If a settings entry is not included for the current bearer-type, the maximum 
fetch size is taken from the CImap4Settings class for the account, and the
number of outstanding requests is set to the default value.

@param aType Type of folder the rule is for (inbox or other folder)
@param aMailInfo On successful completion, this holds the mail info for the rule

@return The position of the rule in the list, or KErrNotFound if no matching
        bearer type was found.
*/
EXPORT_C void CImapSettings::GetTransportBufferSizesL(TUint& aFetchRequestSize, TUint& aMaxOutstandingFetchResponses) const
	{
	TInt err = KErrNotFound;

	if (iCurrentBearerType != 0)
		{
		err = iTransportBufferSizes->GetSizesL(iCurrentBearerType, (TInt&) aFetchRequestSize, (TInt&) aMaxOutstandingFetchResponses);
		}

	if (err < KErrNone)
		{
		aFetchRequestSize = FetchSize();
		aMaxOutstandingFetchResponses = KMaxOutstandingFetchRequests;
		}
	}

/**
Retrieves the value of the StorePlainBodyText flag. The value of the 
flag is read from a resource file during construction of this class.

@return
1 indicates that the message must be stored in chunks in the message store, 
0 indicates that the message must be stored entirely in a single Store operation.
*/
EXPORT_C TInt CImapSettings::StorePlainText() const
	{
	return iStorePlainBodyText;
	}

/**
Check whether the KImap4EmailSettingsFolderSyncDisabled is set in 
CImImap4Settings, ie returns true if folder sync is disabled.

@return
Returns ETrue if the KImap4EmailSettingsFolderSyncDisabled
was set in CImImap4Settings::SetFolderSyncDisabled.
*/
EXPORT_C TBool CImapSettings::FolderSyncDisabled() const
	{
	return iServiceSettings->FolderSyncDisabled();
	}
	
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
/**
Check whether the KImap4EmailSettingsFallBackIsOnFlagEnabled is set in 
CImImap4Settings, ie returns true if Fallback   is enabled.

@return
Returns ETrue if the KImap4EmailSettingsFallBackIsOnFlagEnabled
was set in CImapSettings::SetFallBack() Enabled.
*/
EXPORT_C TBool CImapSettings::FallBack() const
	{
	return iServiceSettings->FallBack();
	}

/**
Check whether the KImap4EmailSettingsAuthenticationFlagEnabled is set in 
CImImap4Settings, ie returns true if Authentication   is enabled.

@return
Returns ETrue if the KImap4EmailSettingsAuthenticationFlagEnabled
was set in CImapSettings::SetIMAP4Auth() Enabled.
*/
EXPORT_C TBool CImapSettings::IMAP4Auth() const
	{
	return iServiceSettings->IMAP4Auth();
	}
#endif
