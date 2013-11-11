// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CEMAILACCOUNTS_H__
#define __CEMAILACCOUNTS_H__


#include <e32std.h>
#include <e32base.h>
#include <msvapi.h>


class CImIAPPreferences;
class CImPop3Settings;
class CImImap4Settings;
class CImSmtpSettings;
class CRepository;
class CImapSyncDownloadRules;
class CImapTransportBufferSizes;
class CImSmtpMobilitySettings;

/**
Maximum size of email account name

@publishedAll
@released
*/
const TInt KEmailAccountNameSize = 256;

/**
Maximum number of entries allowed in the IAP list for a SMTP mobility account

@publishedPartner
@released
*/
const TInt KMaxSmtpMobilityAccountIAPListEntries = 32;

/**
Identifier number for a POP email account.

@publishedAll
@released
@see TPopAccount
*/
typedef TInt TPopAccountId;

/**
Identifier number for an IMAP email account.

@publishedAll
@released
@see TImapAccount
*/
typedef TInt TImapAccountId;

/**
Identifier number for an SMTP email account.

@publishedAll
@released
@see TSmtpAccount
*/
typedef TInt TSmtpAccountId;

/**
Identifier number for an SMTP moblity account.

@publishedPartner
@released
*/
typedef TInt TSmtpMobilityAccountId;

/**
Array of IAP values

@publishedPartner
@released
*/
typedef RArray<TUint32> RArrayIAP;

/**
Null or undefined SMTP account identifier

@publishedPartner
@released
*/
const TSmtpAccountId KNullSmtpAccountId = 0;

/**
POP account ID.

Objects of this type identify POP email accounts. 
They are obtained when a POP account is created 
(CEmailAccounts::CreatePopAccountL()), or by 
querying for what POP accounts exist 
(CEmailAccounts::GetPopAccountL() and CEmailAccounts::GetPopAccountsL()).

The object is then used as a handle to the account when getting
the account settings, using CEmailAccounts::LoadPopSettingsL() and 
CEmailAccounts::LoadPopIapSettingsL().

@publishedAll
@released
*/
class TPopAccount
	{
public:
	/** POP account identifier number. 
	
	Each POP account stored on the phone has a different
	identifier number to distinguish it from other accounts.
	*/
	TPopAccountId iPopAccountId;

	/** POP account name. */
	TBuf<KEmailAccountNameSize> iPopAccountName;

	/** Entry ID of the POP service for this account. */
	TMsvId iPopService;

	/** Entry ID of the related SMTP service for this account. 
		
	This is set if an SMTP service has been created, using 
	CEmailAccounts::CreateSmtpAccountL(), that specifies that 
	it is related to this account. An SMTP service (which provides
	email sending functionality) is used in conjunction with 
	a POP or IMAP service (which provide email retrieval).
	*/
	TMsvId iSmtpService;
	};

/**
IMAP account ID.

Objects of this type identify IMAP email accounts. 
They are obtained when a IMAP account is created 
(CEmailAccounts::CreateImapAccountL()), or by 
querying for what IMAP accounts exist 
(CEmailAccounts::GetImapAccountL() and CEmailAccounts::GetImapAccountsL()).

The object is then used as a handle to the account when getting
the account settings, using CEmailAccounts::LoadImapSettingsL() and 
CEmailAccounts::LoadImapIapSettingsL().

@publishedAll
@released
*/
class TImapAccount
	{
public:
	/** IMAP account identifier number.
	
	Each IMAP account stored on the phone has a different
	identifier number to distinguish it from other accounts.
	*/
	TImapAccountId iImapAccountId;

	/** IMAP account name */
	TBuf<KEmailAccountNameSize> iImapAccountName;

	/** Entry ID of the IMAP service for this account. */
	TMsvId iImapService;

	/** Entry ID of the related SMTP service for this account. 
		
	This is set if an SMTP service has been created, using 
	CEmailAccounts::CreateSmtpAccountL(), that specifies that 
	it is related to this account. An SMTP service (which provides
	email sending functionality) is used in conjunction with 
	a POP or IMAP service (which provide email retrieval).
	*/
	TMsvId iSmtpService;
	};

/**
SMTP account ID.

Objects of this type identify SMTP email accounts. 
They are obtained when a SMTP account is created 
(CEmailAccounts::CreateSmtpAccountL()), or by 
querying for what SMTP accounts exist 
(CEmailAccounts::GetSmtpAccountL() and CEmailAccounts::GetISmtpAccountsL()).

The object is then used as a handle to the account when getting
the account settings, using CEmailAccounts::LoadSmtpSettingsL() and 
CEmailAccounts::LoadSmtpIapSettingsL().

@publishedAll
@released
*/
class TSmtpAccount
	{
public:
	/** SMTP account identifier number. */
	TSmtpAccountId iSmtpAccountId;

	/** SMTP account name */
	TBuf<KEmailAccountNameSize> iSmtpAccountName;

	/** Entry ID of the SMTP service for this account. */
	TMsvId iSmtpService;

	/** Entry ID of the related POP or IMAP service for this account. 
		
	An SMTP service (which provides	email sending functionality) is 
	used in conjunction with a POP or IMAP service (which provide email retrieval).
	*/
	TMsvId iRelatedService;
	};

/**
@publishedPartner
@released
*/
class TSmtpMobilityAccount
	{
public:
	/** SMTP mobility account identifier */
	TSmtpMobilityAccountId iAccountId;

	/** SMTP mobility account name */
	TBuf<KEmailAccountNameSize> iName;

	/** Parent SMTP account identifier */
	TSmtpAccountId iParentSmtpAccountId;

	/** Linked SMTP account identifier. Identifies the SMTP account whose
	    settings will be used by this mobility account. If the mobility account
	    is using its own settings, this will be set to KNullSmtpAccountId. */
	TSmtpAccountId iLinkedSmtpAccountId;
	};

/**
Creates, loads, saves and deletes email (IMAP, POP, and SMTP) accounts. 

An account encapsulates:

- Settings for a messaging service. There is a different type of 
  settings class for each of the email protocols (CImPop3Settings, 
  CImImap4Settings, CImSmtpSettings).
- IAP (Internet account) settings for accessing the service.

The class has a family of functions for each protocol to create, get,
and delete accounts:

- Create\<protocol\>AccountL 
- Get\<protocol\>AccountL 
- Get\<protocol\>AccountsL 
- Delete\<protocol\>AccountL 

where \<protocol\> is either POP, IMAP, or SMTP. 
The Create and Get functions don't directly return settings 
objects, but only account identifiers. 

Account identifiers are passed to the following functions 
to save and load the service and IAP settings for that account:

- Load\<protocol\>SettingsL
- Save\<protocol\>SettingsL 
- Load\<protocol\>IapSettingsL 
- Save\<protocol\>IapSettingsL 

For SMTP only, there is a concept of a default account (as a client
program may want to send an email, and not care which account is used.)
SetDefaultSmtpAccountL(), DefaultSmtpAccountL() and DeleteDefaultSmtpAccountL() 
are used to handle this.

Note that an account is associated with a service entry in the message store,
but account information itself is stored in the central repository.

@publishedAll
@released
*/
class CEmailAccounts : public CBase, public MMsvSessionObserver
	{
public:

	IMPORT_C static CEmailAccounts* NewL();
	IMPORT_C static CEmailAccounts* NewLC();
	IMPORT_C virtual ~CEmailAccounts();

	IMPORT_C TPopAccount CreatePopAccountL(const TDesC& aAccountName, const CImPop3Settings& aPopSettings,
		const CImIAPPreferences& aPopIAP, TBool aReadOnly);

	IMPORT_C TImapAccount CreateImapAccountL(const TDesC& aAccountName, const CImImap4Settings& aImapSettings,
		const CImIAPPreferences& aImapIAP, TBool aReadOnly);

	IMPORT_C TSmtpAccount CreateSmtpAccountL(TPopAccount& aPopAccount, const CImSmtpSettings& aSmtpSettings,
		const CImIAPPreferences& aSmtpIAP, TBool aReadOnly);

	IMPORT_C TSmtpAccount CreateSmtpAccountL(TImapAccount& aImapAccount, const CImSmtpSettings& aSmtpSettings,
		const CImIAPPreferences& aSmtpIAP, TBool aReadOnly);

	IMPORT_C TSmtpAccount CreateSmtpAccountL(const TDesC& aAccountName, const CImSmtpSettings& aSmtpSettings, 
		const CImIAPPreferences& aSmtpIAP, TBool aReadOnly);

	IMPORT_C void GetPopAccountsL(RArray<TPopAccount>& aAccounts);
	IMPORT_C void GetPopAccountL(TMsvId aServiceId, TPopAccount& aAccount); 
	IMPORT_C void DeletePopAccountL(const TPopAccount& aAccount);

	IMPORT_C void GetImapAccountsL(RArray<TImapAccount>& aAccounts);
	IMPORT_C void GetImapAccountL(TMsvId aServiceId, TImapAccount& aAccount); 
	IMPORT_C void DeleteImapAccountL(const TImapAccount& aAccount);

	IMPORT_C void GetSmtpAccountsL(RArray<TSmtpAccount>& aAccounts);
	IMPORT_C void GetSmtpAccountL(TMsvId aServiceId, TSmtpAccount& aAccount); 
	IMPORT_C void DeleteSmtpAccountL(const TSmtpAccount& aAccount);
	IMPORT_C void DeleteDefaultSmtpAccountL();

	IMPORT_C void PopulateDefaultPopSettingsL(CImPop3Settings& aPopSettings,
	CImIAPPreferences& aPopIap);

	IMPORT_C void PopulateDefaultImapSettingsL(CImImap4Settings& aImapSettings,
	CImIAPPreferences& aImapIap);

	IMPORT_C void PopulateDefaultSmtpSettingsL(CImSmtpSettings& aSmtpSettings,
	CImIAPPreferences& aSmtpIap);

	IMPORT_C TInt DefaultSmtpAccountL(TSmtpAccount& aAccount);
	IMPORT_C void SetDefaultSmtpAccountL(const TSmtpAccount& aAccount);

	IMPORT_C void LoadPopSettingsL(const TPopAccount& aAccount, CImPop3Settings& aPopSettings);
	IMPORT_C void LoadPopIapSettingsL(const TPopAccount& aAccount, CImIAPPreferences& aIAP);

	IMPORT_C void LoadImapSettingsL(const TImapAccount& aAccount, CImImap4Settings& aImapSettings);
	IMPORT_C void LoadImapIapSettingsL(const TImapAccount& aAccount, CImIAPPreferences& aIAP);

	IMPORT_C void LoadSmtpSettingsL(const TSmtpAccount& aAccount, CImSmtpSettings& aSmtpSettings);
	IMPORT_C void LoadSmtpIapSettingsL(const TSmtpAccount& aAccount, CImIAPPreferences& aIAP);

	IMPORT_C void SavePopSettingsL(const TPopAccount& aAccount, const CImPop3Settings& aPopSettings);
	IMPORT_C void SavePopIapSettingsL(const TPopAccount& aAccount, const CImIAPPreferences& aIAP);

	IMPORT_C void SaveImapSettingsL(const TImapAccount& aAccount, const CImImap4Settings& aImapSettings);

	IMPORT_C void SaveImapIapSettingsL(const TImapAccount& aAccount, const CImIAPPreferences& aIAP);

	IMPORT_C void SaveSmtpSettingsL(const TSmtpAccount& aAccount, const CImSmtpSettings& aSmtpSettings);
	IMPORT_C void SaveSmtpIapSettingsL(const TSmtpAccount& aAccount, const CImIAPPreferences& aIAP);

	IMPORT_C TBool IsAccountReadOnlyL(const TPopAccount& aAccount);
	IMPORT_C TBool IsAccountReadOnlyL(const TImapAccount& aAccount);
	IMPORT_C TBool IsAccountReadOnlyL(const TSmtpAccount& aAccount);

	// Published partner routines
	IMPORT_C TImapAccount CreateImapAccountL(const TDesC& aAccountName,
		const CImImap4Settings& aImapSettings, const CImIAPPreferences& aImapIAP,
		const CImapSyncDownloadRules& aSyncDownloadRules, TBool aReadOnly);
	IMPORT_C void LoadImapSyncDownloadRulesL(const TImapAccount& aAccount, CImapSyncDownloadRules& aRules);
	IMPORT_C void LoadImapTransportBufferSizesL(CImapTransportBufferSizes& aBufferSizes);
	IMPORT_C void SaveImapSyncDownloadRulesL(const TImapAccount& aAccount, const CImapSyncDownloadRules& aRules);
	IMPORT_C void SaveImapTransportBufferSizesL(const CImapTransportBufferSizes& aBufferSizes);
	IMPORT_C void DeleteImapTransportBufferSizesL();

	IMPORT_C void GetSmtpAccountL(TSmtpAccountId aAccountId, TSmtpAccount& aAccount);

	IMPORT_C TSmtpMobilityAccount CreateSmtpMobilityAccountL(const TDesC& aAccountName, const RArrayIAP& aIapList, const TSmtpAccount& aSmtpAccount, const CImSmtpMobilitySettings& aSettings, TBool aReadOnly);
	IMPORT_C TSmtpMobilityAccount CreateSmtpMobilityAccountL(const TDesC& aAccountName, const RArrayIAP& aIapList, const TSmtpAccount& aSmtpAccount, const TSmtpAccount& aLinkedSmtpAccount, TBool aReadOnly);
	IMPORT_C void GetSmtpMobilityAccountsL(const TSmtpAccount& aSmtpAccount, RArray<TSmtpMobilityAccount>& aAccounts);
	IMPORT_C void DeleteSmtpMobilityAccountL(const TSmtpMobilityAccount& aAccount);

	IMPORT_C void PopulateDefaultSmtpMobilitySettingsL(CImSmtpMobilitySettings& aSettings);
	IMPORT_C void LoadSmtpMobilitySettingsL(const TSmtpMobilityAccount& aAccount, CImSmtpMobilitySettings& aSettings);
	IMPORT_C void SaveSmtpMobilitySettingsL(const TSmtpMobilityAccount& aAccount, const CImSmtpMobilitySettings& aSettings);

	IMPORT_C void LoadSmtpMobilityAccountIapListL(const TSmtpMobilityAccount& aAccount, RArrayIAP& aIapList);
	IMPORT_C void SaveSmtpMobilityAccountIapListL(const TSmtpMobilityAccount& aAccount, const RArrayIAP& aIapList);

	IMPORT_C TBool IsAccountReadOnlyL(const TSmtpMobilityAccount& aAccount);

	// Internal routines
	IMPORT_C void GetSmtpMobilityAccountsL(RArray<TSmtpMobilityAccount>& aAccounts);
	IMPORT_C TBool LoadSmtpMobilitySettingsL(TSmtpAccountId& aSmtpAccountId, TUint32 aIap, CImSmtpMobilitySettings& aSettings);

private:
	CEmailAccounts();
	void HandleSessionEventL(TMsvSessionEvent , TAny* , TAny* , TAny* );
	CMsvSession& SessionL();
	CRepository& PopRepositoryL();
	CRepository& ImapRepositoryL();
	CRepository& SmtpRepositoryL();

	TMsvId CreateEmailServiceL(CMsvEntry& aMsvEntry, const TDesC& aAccountName, TUid aMTMType, TBool aVisible);
	void SetRelatedIdL(CMsvEntry& aMsvEntry, TMsvId aId, TMsvId aRelatedId);
	void SetEntryDetailsL(TMsvId aId, const TDesC& aAccountName);
	void UnSetInPreparationFlagsL(CMsvEntry& aMsvEntry, TMsvId aId, TInt aAccountId);
	void DeleteServiceEntryL(CMsvEntry& aMsvEntry, TMsvId aId);

	TUint GetNextEmptyAccountSlotL(CRepository& aRepository);
	TUint32 FindEmailAccountL(TMsvId aService, CRepository& aRepository);

	void LoadIAPPrefSettingsL(TUint32 aAccountId, CImIAPPreferences& aIAP, CRepository& aRepository);
	void DoLoadPopSettingsL(TUint32 aAccountId, CImPop3Settings& aPopSettings);
	void DoLoadImapSettingsL(TUint32 aAccountId, CImImap4Settings& aImapSettings);
	void DoLoadSmtpSettingsL(TUint32 aAccountId, CImSmtpSettings& aSmtpSettings);

	void SaveIAPPrefSettingsL(TUint32 aAccountId, const CImIAPPreferences& aIAP, CRepository& aRepository);
	void DoSavePopSettingsL(TUint32 aAccountId, const CImPop3Settings& aPopSettings);
	void DoSaveSmtpSettingsL(TUint32 aAccountId, const CImSmtpSettings& aSmtpSettings);
	void DoSaveImapSettingsL(TUint32 aAccountId, const CImImap4Settings& aImapSettings);	
	void DoSaveImapSyncDownloadRulesL(TUint32 aAccountId, const CImapSyncDownloadRules& aRules, CRepository& aRepository);

	TImapAccount CreateImapAccountL(const TDesC& aAccountName, const CImImap4Settings& aImapSettings,
		const CImIAPPreferences& aImapIAP, const CImapSyncDownloadRules* aSyncDownloadRules, TBool aReadOnly);

	TUint32 DoCreatePopAccountL(const TDesC& aAccountName, const CImPop3Settings& aPopSettings, 
		const CImIAPPreferences& aPopIAP, TMsvId aPopService, TMsvId aRelatedService, TBool aReadOnly);
	TUint32 DoCreateImapAccountL(const TDesC& aAccountName, const CImImap4Settings& aImapSettings,
		const CImIAPPreferences& aImapIAP, const CImapSyncDownloadRules* aSyncDownloadRules, TMsvId aImapService,
		TMsvId aRelatedService, TBool aReadOnly);
	TUint32 DoCreateSmtpAccountL(TPopAccount& aPopAccount, const CImSmtpSettings& aSmtpSettings, 
		const CImIAPPreferences& aSmtpIAP, TMsvId aSmtpService, CMsvEntry& aMsvEntry, TUint32& aPopAccountId, TBool aReadOnly);
	TUint32 DoCreateSmtpAccountL(TImapAccount& aImapAccount, const CImSmtpSettings& aSmtpSettings, 
		const CImIAPPreferences& aSmtpIAP, TMsvId aSmtpService, CMsvEntry& aMsvEntry, TUint32& aImapAccountId, TBool aReadOnly);
	TUint32 DoCreateSmtpAccountL(const TDesC& aAccountName, const CImSmtpSettings& aSmtpSettings, 
		const CImIAPPreferences& aSmtpIAP, TMsvId aSmtpService, TMsvId aRelatedService, TBool aReadOnly);	

	void DeleteIAPPrefSettingsL(TUint32 aAccountId, CRepository& aRepository);
	void DeletePopSettings(TUint32 aAccountId, CRepository& aRepository);
	void DeleteSmtpSettings(TUint32 aAccountId, CRepository& aRepository);
	void DeleteImapSettings(TUint32 aAccountId, CRepository& aRepository);
	void DeleteImapSyncDownloadRulesL(TUint32 aAccountId, CRepository& aRepository);

	void GetEmailServicesL(CMsvEntrySelection& aServices, TUid aMTMType, CMsvEntry& aMsvEntry);

	TBool IsAsciiPrintable(const TDesC16& aText);
	TBool IsAsciiPrintable(const TDesC8& aText);
	TBool IsUnicodePrintable(const TDesC16& aText);

	TSmtpMobilityAccount DoCreateSmtpMobilityAccountL(const TDesC& aAccountName, const RArrayIAP& aIapList, const CImSmtpMobilitySettings& aSettings, const TSmtpAccount& aSmtpAccount, TSmtpAccountId aLinkedSmtpAccountId, TBool aReadOnly);
	void AddMobilityAccountToSmtpAccountL(const TSmtpAccount& aSmtpAccount, const RArrayIAP& aIapList, TUint32 aMobAccRepId, CRepository& aRepository);
	void AddLinkToMobilityAccountL(TSmtpAccountId aLinkedSmtpAccountId, TUint32 aMobAccRepId, CRepository& aRepository);
	TInt FindIapListEntryInSmtpMobilityAccountListL(const RArrayIAP& aIapList, CRepository& aRepository, TUint32 aSmtpAccRepId, TInt aNumEntries, TInt aIgnorePos);
	TInt FindAccountIdInSmtpMobilityAccountListL(TUint32 aMobAccRepId, CRepository& aRepository, TUint32 aSmtpAccRepId, TInt aNumEntries);
	void GetSmtpAccountInfoForMobilityAccountL(TUint32 aMobAccRepId, CRepository& aRepository, TUint32& aSmtpAccRepId, TInt& aPos);
	void DeleteAllOwnedMobilityAccountsL(TUint32 aSmtpAccRepId, CRepository& aRepository);
	void DeleteAllLinkedMobilityAccountsL(TUint32 aLinkedSmtpAccRepId, CRepository& aRepository);
	void DeleteSmtpMobilityAccountListEntryL(TUint32 aMobAccRepId, CRepository& aRepository, TUint32 aSmtpAccRepId);
	void DeleteSmtpMobilityAccountFromLinkedSmtpAccountL(TUint32 aMobAccRepId, CRepository& aRepository);
	TBool IsSmtpTypeAccountReadOnlyL(TUint32 aAccRepId);
	TBool IsSmtpMobilityAccount(CRepository& aRepository, TUint32 aAccRepId);
#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
	void CreateHeaderStoreL(TUid aMTMid);
#endif
private:
	enum TEmailSettingTypes
		{
		// Partial CenRep Id for service settings
		EServiceSettingType 			= 0x00000000,
		// Partial CenRep Id for IAP Prefs
		EIAPPrefSettingType				= 0x00010000,
		// CenRep mask for email accounts
		EAccountMask 					= 0x800FFFFF			
		};
	
	enum TPOPServiceSettingsCenRepId
		{
		EPOPServiceId					= 0x00000000,
		EPOPAccountNameId				= 0x00000001,
		EPOPRelatedSMTPServiceId		= 0x00000002,
		EPOPSettingsVersionId			= 0x00000003, 
		EPOPServerAddressId				= 0x00000004,
		EPOPPortNumberId			   	= 0x00000005,     	
		EPOPFlagsId						= 0x00000006,
		EPOPLoginNameId					= 0x00000007,
		EPOPPasswordId					= 0x00000008,
		EPOPMaxEmailSizeId				= 0x00000009,
		EPOPGetEmailOptionsId			= 0x0000000A,
		EPOPInboxSynchronisationLimitId	= 0x0000000B,
		EPOPPopulationLimitId			= 0x0000000C,
		EPOPReadOnlyAccountId			= 0x0000000D,
		EPOPTlsSslDomainId				= 0x0000000F
		};

	enum TIMAPServiceSettingsCenRepId
		{
		EIMAPServiceId					= 0x00000000,
		EIMAPAccountNameId				= 0x00000001,
		EIMAPRelatedSMTPServiceId		= 0x00000002,
		EIMAPSettingsVersionId			= 0x00000003, 
		EIMAPServerAddressId			= 0x00000004,
		EIMAPPortNumberId			   	= 0x00000005,     	
		EIMAPFlagsId					= 0x00000006,
		EIMAPLoginNameId				= 0x00000007,
		EIMAPPasswordId					= 0x00000008,
		EIMAPFolderPathId				= 0x00000009,
		EIMAPPathSeparatorId			= 0x0000000A,
		EIMAPSynchronisationStrategyId	= 0x0000000B,
		EIMAPSubscriptionStrategyId		= 0x0000000C,
		EIMAPMaxEmailSizeId				= 0x0000000D,
		EIMAPGetMailOptionsId			= 0x0000000E,
		EIMAPInboxSynchronisationLimitId= 0x0000000F,
		EIMAPMailboxSynchronisationLimitId= 0x00000010,
		EIMAPTlsSslDomainId				= 0x00000012,		
		EIMAPSyncRateId					= 0x00000020,
		EIMAPFetchSizeId				= 0x00000030,
		EIMAPIdleTimeoutId				= 0x00000040,
		EPartialMailOptionsId			= 0x00000050,
		EBodyTextSizeLimitId			= 0x00000060,		
		EAttachmentSizeLimitId			= 0x00000070,
		EIMAPSearchStringId				= 0x00000080,
		EIMAPReadOnlyAccountId			= 0x00000090,
		// 0x00001000 - 0x00002FFF reserved for sync download rules
		// 0x00010000 - 0x00014FFF reserved for IAP/SNAP preferences
		};

	enum TIMAPSyncDownloadRulesCenRepInfo
		{
		EIMAPSyncDownloadRulesVersionId = 0x00001000,
		EIMAPNumInboxSyncDownloadRulesId = 0x00001001,
		EIMAPNumFolderSyncDownloadRulesId = 0x00001002,

		EIMAPInboxSyncDownloadRulesBearerTypesId = 0x00001010,
		EIMAPInboxSyncDownloadRulesMailOptionsId = 0x00001011,
		EIMAPInboxSyncDownloadRulesTotalSizeLimitId = 0x00001012,
		EIMAPInboxSyncDownloadRulesBodyTextSizeLimitId = 0x00001013,
		EIMAPInboxSyncDownloadRulesAttachmentSizeLimitId = 0x00001014,
		EIMAPInboxSyncDownloadRulesGetMailBodyPartsId = 0x00001015,
		EIMAPInboxSyncDownloadRulesMaxEmailSizeId = 0x00001016,

		EIMAPFolderSyncDownloadRulesBearerTypesId = 0x00001030,
		EIMAPFolderSyncDownloadRulesMailOptionsId = 0x00001031,
		EIMAPFolderSyncDownloadRulesTotalSizeLimitId = 0x00001032,
		EIMAPFolderSyncDownloadRulesBodyTextSizeLimitId = 0x00001033,
		EIMAPFolderSyncDownloadRulesAttachmentSizeLimitId = 0x00001034,
		EIMAPFolderSyncDownloadRulesGetMailBodyPartsId = 0x00001035,
		EIMAPFolderSyncDownloadRulesMaxEmailSizeId = 0x00001036,

		// Offset between sets of rules
		EIMAPSyncDownloadRulesOffset = 0x40
		};

	enum TIAPPreferencesCenRepId
		{
		// single settings		
		EIAPPrefVersion 				= 0x00010000,
		EIAPChoicesCount 				= 0x00010001,
		
		// List of settings
		EIAPListIDs						= 0x00011000,
		EIAPListDialogPrefs				= 0x00012000,
		// EIAPSnapId placed here is a deviation from Design doc. Remove comment after a decision is made
		EIAPSnapId						= 0x00014000 // The same value is to be used in all account types
		};

	enum TIMAPTransportBufferSizesCenRepInfo
		{
		EIMAPTransportBufferSizesVersionId = 0x80000100,
		EIMAPNumTransportBufferSizesId = 0x80000101,

		EIMAPTransportBufferSizesBearerTypesId = 0x80000110,
		EIMAPTransportBufferSizesFetchRequestSizeId = 0x80000111,
		EIMAPTransportBufferSizesMaxOutstandingFetchResponsesId = 0x80000112,
	
		// Offset between entries
		EIMAPTransportBufferSizesOffset = 0x20
		};

	enum TSMTPServiceSettingsCenRepId
		{
		ESMTPServiceId					= 0x00000000,
		ESMTPAccountNameId				= 0x00000001,		
		ESMTPSettingsVersionId			= 0x00000002,
		ESMTPServerAddressId			= 0x00000003, 
		ESMTPPortNumberId				= 0x00000004, 
		ESMTPFlagsId				   	= 0x00000005,     	
		ESMTPEmailAliasId				= 0x00000006, 
		ESMTPEmailAddressId				= 0x00000007,
		ESMTPReplyToAddressId			= 0x00000008,
		ESMTPReceiptAddressId			= 0x00000009,
		ESMTPBodyEncodingId				= 0x0000000A,
		ESMTPDefaultMsgCharSetId		= 0x0000000B,
		ESMTPSendCopyToSelfId			= 0x0000000C,
		ESMTPSendMessageOptionId		= 0x0000000D,
		ESMTPLoginNameId				= 0x0000000E,
		ESMTPPasswordId					= 0x0000000F,
		ESMTPRelatedServiceId			= 0x00000010,
		ESMTPIncludeToCcId				= 0x00000011,
		ESMTPReadOnlyAccountId			= 0x00000012,
		ESMTPParentAccountId			= 0x00000013,
		ESMTPTlsSslDomainId				= 0x00000014,
		ESMTPLinkedAccountId			= 0x00000015
		};

	enum TSMTPLinkedMobilityAccountListCenRepInfo
		{
		ESMTPLinkedMobilityAccountListVersionId = 0x00015000,
		ESMTPLinkedMobilityAccountListNumEntriesId = 0x00015001,
		ESMTPLinkedMobilityAccountListEntryId = 0x00015010,

		// Maximum number of list entries
		ESMTPLinkedMobilityAccountListMaxEntries = 2048
		};

	enum TSMTPMobilityAccountListCenRepInfo
		{
		ESMTPMobilityAccountListVersionId = 0x00016000,
		ESMTPMobilityAccountListNumEntriesId = 0x00016001,

		ESMTPMobilityAccountListAccountId = 0x00016010,
		ESMTPMobilityAccountListNumIapsId = 0x00016011,
		ESMTPMobilityAccountListIapId = 0x00016020,

		// Offset between entries
		ESMTPMobilityAccountListOffset = 0x40,

		// Maximum number of list entries
		ESMTPMobilityAccountMaxEntries = 32
		};

private:
	CMsvSession* iMsvSession;
	CRepository* iPOPRepository;
	CRepository* iSMTPRepository;
	CRepository* iIMAPRepository;	
	};

#endif // __CEMAILACCOUNTS_H__
