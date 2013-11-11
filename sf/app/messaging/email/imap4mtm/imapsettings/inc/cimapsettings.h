// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CImapSettings header
// 
//

#if !defined(__CIMAPSETTINGS_H__)
#define __CIMAPSETTINGS_H__

#include <msventry.h>
#include <imapset.h>
#include <cimapsyncdownloadrules.h>

class CImIAPPreferences;
class CImapSyncDownloadRules;
class CImapTransportBufferSizes;


/**
@internalComponent
@prototype
*/
class CImapSettings : public CBase
	{
public:
	IMPORT_C static CImapSettings* NewL(CMsvServerEntry& aServerEntry);
	~CImapSettings();
	
	// Indicates if account settings have been loaded
	IMPORT_C TBool SettingsLoaded() const;

	// Loads the settings from CenRep
	IMPORT_C void LoadSettingsL(const TMsvId aId);
	
	// Access to the service ID
	IMPORT_C TMsvId ServiceId() const;
	
	// Access to CImBaseEmailSettings
	IMPORT_C const TPtrC ServerAddress() const;
	IMPORT_C TUint Port() const;
	IMPORT_C TBool SecureSockets() const;
	IMPORT_C TBool SSLWrapper() const;
	IMPORT_C TBool BearerMobility() const;
		
	// Access to CImImap4Settings
	IMPORT_C const TPtrC8 LoginName() const;
	IMPORT_C const TPtrC8 Password() const;
	IMPORT_C const TPtrC8 FolderPath() const;
	IMPORT_C const TPtrC8 SearchString() const;
	IMPORT_C const TDesC& PathSeparator() const;
	IMPORT_C TBool DisconnectedUserMode() const;
	IMPORT_C TFolderSyncType Synchronise() const;
	IMPORT_C TFolderSubscribeType Subscribe() const;
	IMPORT_C TBool DeleteEmailsWhenDisconnecting() const;
	IMPORT_C TImap4GetMailOptions GetMailOptions() const;
	IMPORT_C TInt32 InboxSynchronisationLimit() const;
	IMPORT_C TInt32 MailboxSynchronisationLimit() const;
	IMPORT_C TBool UpdatingSeenFlags() const;
	IMPORT_C TUint FetchSize() const;
	IMPORT_C TBool ImapIdle() const;
	IMPORT_C TInt ImapIdleTimeout() const;
	IMPORT_C TBool UseExpunge() const;
	IMPORT_C TPtrC8 TlsSslDomain() const;
	IMPORT_C TBool UseSyncDownloadRules() const;
	IMPORT_C TBool FolderSyncDisabled() const;
	
	//Access to SASL authenticate login methods	
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	IMPORT_C TBool FallBack() const;
	IMPORT_C TBool IMAP4Auth() const;
#endif
	// Access to CImIAPPreferences
	IMPORT_C const CImIAPPreferences& IAPPreferences() const;
	
	IMPORT_C TInt GetSyncDownloadRuleL(CImapSyncDownloadRules::TRulesType aType, TImImap4GetPartialMailInfo& aMailInfo) const;
	IMPORT_C void GetTransportBufferSizesL(TUint& aFetchRequestSize, TUint& aMaxOutstandingFetchResponses) const;

	// Access to Resource File data
	IMPORT_C TInt Store8BitData() const;
	IMPORT_C const TDesC& DefaultAttachmentName() const;
	IMPORT_C const HBufC8* DefaultPartialMessageFooter() const;
	
	// Setters for settings that can be updated by the Server MTM
	IMPORT_C void SetPathSeparatorL(const TText8 aPathSeparator);
	IMPORT_C TInt StorePlainText() const;
	IMPORT_C void SetCurrentBearerType(TUint32 aBearerType);

private:
	CImapSettings(CMsvServerEntry& aServerEntry);
	void ConstructL();

	void ReadResourceFileL();
	// stores the settings to CenRep
	void SaveSettingsL();

private:

	/** The CMsvServerEntry to use for accessing the server entry */
	CMsvServerEntry& iServerEntry;
	
	/** ETrue if settings have been loaded */
	TBool iSettingsLoaded;
	
	/** The Service ID for the IMAP Service */
	TMsvId iServiceId;
	
	/** The IMAP Service's settings */
	CImImap4Settings* iServiceSettings;

	/** The IAP preferences relating to IAPs for email*/
	CImIAPPreferences* iIapPrefs;

	/** The path separator is defined by the remote IMAP server and retrieved when the
	connection is established */
	TBufC16<1> iPathSeparator;
	
	/** The buffer for Store8BitData flag. The flag value is read from a resource file */
	TInt iStore8BitData;
	
	/** The buffer for the default atachment name. The value is read from a resource file */
	HBufC* iDefaultAttachmentName;
	
	/** The default message footer that is appended when an email is only partially fetched.
	The default partial message footer is defined in IMCM.rss */
	HBufC8* iDefaultPartialMessageFooter;
	
	/** The StorePlainBodyText flag. The flag value is read from a resource file */
	TInt iStorePlainBodyText;

	/** Download rules for sync phase */	
	CImapSyncDownloadRules* iSyncDownloadRules;
	
	/** Transport buffer sizes for fetch operations */
	CImapTransportBufferSizes* iTransportBufferSizes;
	
	/** Stores the currently connected brearer type */
	TUint32 iCurrentBearerType;
	};

#endif // __CIMAPSETTINGS_H__
