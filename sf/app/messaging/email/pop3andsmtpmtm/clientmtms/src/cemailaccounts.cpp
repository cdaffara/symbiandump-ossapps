// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <msvuids.h>
#include <msvids.h>
#include <centralrepository.h>
#include <s32mem.h>

#include "POP3SET.H"
#include "IMAPSET.H"
#include "SMTPSET.H"
#include <iapprefs.h>
#include <cemailaccounts.h>
#include "MIUTSET.H"
#include "cimapsyncdownloadrules.h"
#include "cimaptransportbuffersizes.h"
#include "cimsmtpmobilitysettings.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "msvconsts.h"
#endif

const TUint32 KNullId = 0x00000000;
const TUint32 KIncrementAccount = 0x00100000;
const TUint32 KInitialiseSettings = 0x00000000; // Initial settings are in account 0
const TInt KMaxAccount = 2048;
const TUint32 KDefaultAccountId = 0x80000000; // set top bit
const TInt KNullSmtpMobilityAccountEntryPos = -1;

const TInt KMaxIAPChoices = 4096;
const TInt KShiftAccountId = 20;
const TInt KPOPSettingsVersion = 2;
const TInt KIMAPSettingsVersion = 2;
const TInt KSMTPSettingsVersion = 3;
const TInt KIAPPrefSettingsVersion = 2;
const TInt KImapSyncDownloadRulesVersion = 1;
const TInt KImapTransportBufferSizesVersion = 1;
const TInt KSmtpMobilityAccountsListVersion = 1;
const TInt KSmtpLinkedMobilityAccountsListVersion = 1;

/** 
Allocates and constructs an Email accounts object.

Initialises all member data to their default values.

@return
The newly constructed Email accounts object.
*/
EXPORT_C CEmailAccounts* CEmailAccounts::NewL()
	{
	CEmailAccounts* self = new (ELeave) CEmailAccounts();
	return self;
	}

/** 
Allocates and constructs an Email accounts object.

Initialises all member data to their default values.

@return
The newly constructed Email accounts object.
*/
EXPORT_C CEmailAccounts* CEmailAccounts::NewLC()
	{
	CEmailAccounts* self = new (ELeave) CEmailAccounts();
	CleanupStack::PushL(self);
	return self;
	}

/**
Constructor.
*/
CEmailAccounts::CEmailAccounts()
	{
	}

/** 
Destructor. 
*/
EXPORT_C CEmailAccounts::~CEmailAccounts()
	{
	delete iIMAPRepository;
	delete iSMTPRepository;
	delete iPOPRepository;
	delete iMsvSession;
	}

/**
Creates a POP Account:

1. Creates a POP service entry in the message store

2. Creates settings in CenRep with values from the supplied objects

3. Returns a POP account ID

@param	aAccountName
Account Name

@param	aPopSettings
POP Service Settings

@param	aPopIAP
POP IAP preferences

@param	aReadOnly
Specify ETrue if it is a read-only account. 
Note: Read-only accounts should not be modified, however we don't enforce it.

@return
POP account ID of the new account
@leave
KErrOverflow if the account name is longer than 256 characters.
KErrArgument if settings fields contained non-ascii printable characters.
*/
EXPORT_C TPopAccount CEmailAccounts::CreatePopAccountL(const TDesC& aAccountName, const CImPop3Settings& aPopSettings,
	const CImIAPPreferences& aPopIAP, TBool aReadOnly)
	{
	// Check the incoming account name, if its too long then leave with KErrOverflow
	if(aAccountName.Length() > KEmailAccountNameSize)
		{
		User::Leave(KErrOverflow);
		}
	
	// Check for alpha numerics for the various strings
	if(!IsUnicodePrintable(aAccountName) ||
		!IsAsciiPrintable(aPopSettings.ServerAddress()) ||
	   	!IsAsciiPrintable(aPopSettings.Password()) ||
		!IsAsciiPrintable(aPopSettings.LoginName()))
		{
		User::Leave(KErrArgument);
		}
#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
	// Create  POP header table .
	CreateHeaderStoreL(KUidMsgTypePOP3);
#endif	
	// Create POP service entry in the message store
	TMsvId popService = KMsvNullIndexEntryId;
	CMsvEntry* msvEntry = SessionL().GetEntryL(KMsvRootIndexEntryId);
    CleanupStack::PushL(msvEntry);		
	popService = CreateEmailServiceL(*msvEntry, aAccountName, KUidMsgTypePOP3, ETrue);

	// Save settings to CenRep
	TUint32 popAccountId = 0;
	TRAPD(error, popAccountId = DoCreatePopAccountL(aAccountName, aPopSettings, aPopIAP, popService, 0, aReadOnly));
	if (error != KErrNone)
		{
		// saving settings to CenRep failed, so delete pop service and leave
		DeleteServiceEntryL(*msvEntry, popService);		
		User::Leave(error);
		}

	// Create account Id
	TPopAccount account;
	account.iPopAccountId = popAccountId >> KShiftAccountId; 
	account.iPopAccountName = aAccountName; 							
	account.iPopService = popService;
	account.iSmtpService = KMsvNullIndexEntryId; // Related smtp service not created yet 
	


	// settings saved, so un-set InPreparation Flags	
	UnSetInPreparationFlagsL(*msvEntry, popService, account.iPopAccountId);
	CleanupStack::PopAndDestroy(msvEntry);	
	return account;				
	}

TUint32 CEmailAccounts::DoCreatePopAccountL(const TDesC& aAccountName, const CImPop3Settings& aPopSettings, 
		const CImIAPPreferences& aPopIAP, const TMsvId aPopService, TMsvId aRelatedService, TBool aReadOnly)
	{
	CRepository& popRepository = PopRepositoryL();
	TUint32 popAccountId = GetNextEmptyAccountSlotL(popRepository);
	
	User::LeaveIfError(popRepository.StartTransaction(CRepository::EReadWriteTransaction));					
	popRepository.CleanupRollbackTransactionPushL();
	popRepository.Set(popAccountId + EPOPServiceId, static_cast<TInt>(aPopService)); 
	popRepository.Set(popAccountId + EPOPAccountNameId, aAccountName);
	popRepository.Set(popAccountId + EPOPRelatedSMTPServiceId, static_cast<TInt>(aRelatedService));
	popRepository.Set(popAccountId + EPOPReadOnlyAccountId, static_cast<TInt>(aReadOnly));
 	DoSavePopSettingsL(popAccountId, aPopSettings);
	SaveIAPPrefSettingsL(popAccountId, aPopIAP, popRepository);
	CleanupStack::Pop();
	TUint32 errorId = 0;	
	User::LeaveIfError(popRepository.CommitTransaction(errorId));	
	
	return popAccountId;
	}

TMsvId CEmailAccounts::CreateEmailServiceL(CMsvEntry& aMsvEntry, const TDesC& aAccountName, TUid aMTMType, TBool aVisible)
    {
	TMsvEntry entry;
	entry.iMtm = aMTMType;
	entry.iType = KUidMsvServiceEntry;
    entry.iServiceId = KMsvNullIndexEntryId;
	entry.SetReadOnly(EFalse);
	entry.SetVisible(aVisible);	
    entry.iDate.UniversalTime();
	entry.iDetails.Set(aAccountName);
	entry.iDescription.Set(KNullDesC);
	entry.SetInPreparation(ETrue);
	aMsvEntry.SetEntryL(KMsvRootIndexEntryId);

	aMsvEntry.CreateL(entry);
	return (entry.Id());
	}

void CEmailAccounts::SetRelatedIdL(CMsvEntry& aMsvEntry, TMsvId aId, TMsvId aRelatedId)
	{	
	aMsvEntry.SetEntryL(aId);	
	TMsvEntry entry(aMsvEntry.Entry());
	entry.iRelatedId = aRelatedId;	
	aMsvEntry.ChangeL(entry);
	}

void CEmailAccounts::SetEntryDetailsL(TMsvId aId, const TDesC& aAccountName)
	{	
	CMsvEntry* msvEntry = SessionL().GetEntryL(aId);
    CleanupStack::PushL(msvEntry);

	TMsvEntry entry(msvEntry->Entry());
	entry.iDetails.Set(aAccountName);	
	msvEntry->ChangeL(entry);

	CleanupStack::PopAndDestroy(msvEntry);	
	}

void CEmailAccounts::UnSetInPreparationFlagsL(CMsvEntry& aMsvEntry, TMsvId aId, TInt aAccountId)
	{
	aMsvEntry.SetEntryL(aId);	
	TMsvEntry entry = aMsvEntry.Entry();
	entry.SetMtmData2(aAccountId);
	entry.SetInPreparation(EFalse);	
	aMsvEntry.ChangeL(entry);
	}

void CEmailAccounts::DeleteServiceEntryL(CMsvEntry& aMsvEntry, TMsvId aId)
	{
	aMsvEntry.SetEntryL(KMsvRootIndexEntryId);	
	TRAPD (error, aMsvEntry.DeleteL(aId));
	if (error != KErrNotFound)
		{
		User::LeaveIfError(error);		
		}
	}

/**
Creates an IMAP Account:

1. Creates an IMAP service entry in the message store

2. Creates settings in CenRep with values from the supplied objects

3. Returns an IMAP account ID

@param	aAccountName
Account Name

@param	aImapSettings
IMAP Service Settings

@param	aImapIAP
IMAP IAP preferences

@param	aReadOnly
Specify ETrue if it is a read-only account
Note: Read-only accounts should not be modified, however we don't enforce it.

@return
IMAP account ID
@leave
KErrOverflow if the account name is longer than 256 characters.
KErrArgument if settings fields contained non-ascii printable characters.
*/
EXPORT_C TImapAccount CEmailAccounts::CreateImapAccountL(const TDesC& aAccountName,
	const CImImap4Settings& aImapSettings, const CImIAPPreferences& aImapIAP, TBool aReadOnly)
	{
	return CreateImapAccountL(aAccountName, aImapSettings, aImapIAP, NULL, aReadOnly);
	}

/**
Creates an IMAP Account:

1. Creates an IMAP service entry in the message store

2. Creates settings in CenRep with values from the supplied objects

3. Returns an IMAP account ID

@param	aAccountName
Account Name

@param	aImapSettings
IMAP Service Settings

@param	aImapIAP
IMAP IAP preferences

@param	aSyncDownloadRules
IMAP synchronisation download rules

@param	aReadOnly
Specify ETrue if it is a read-only account
Note: Read-only accounts should not be modified, however we don't enforce it.

@return
IMAP account ID

@publishedPartner
*/	
EXPORT_C TImapAccount CEmailAccounts::CreateImapAccountL(const TDesC& aAccountName,
	const CImImap4Settings& aImapSettings, const CImIAPPreferences& aImapIAP,
	const CImapSyncDownloadRules& aSyncDownloadRules, TBool aReadOnly)
	{
	return CreateImapAccountL(aAccountName, aImapSettings, aImapIAP, &aSyncDownloadRules, aReadOnly);
	}

/**
Creates an IMAP Account:

1. Creates an IMAP service entry in the message store

2. Creates settings in CenRep with values from the supplied objects

3. Returns an IMAP account ID

@param	aAccountName
Account Name

@param	aImapSettings
IMAP Service Settings

@param	aImapIAP
IMAP IAP preferences

@param	aReadOnly
Specify ETrue if it is a read-only account
Note: Read-only accounts should not be modified, however we don't enforce it.

@param	aSyncDownloadRules
Synchronisation download rules (can be NULL)

@return
IMAP account ID
*/	
TImapAccount CEmailAccounts::CreateImapAccountL(const TDesC& aAccountName,
	const CImImap4Settings& aImapSettings, const CImIAPPreferences& aImapIAP,
	const CImapSyncDownloadRules* aSyncDownloadRules, TBool aReadOnly)
	{
	// Check the incoming account name, if its too long then leave with KErrOverflow
	if(aAccountName.Length() > KEmailAccountNameSize)
		{
		User::Leave(KErrOverflow);
		}

	// Check for alpha numerics for the various strings
	if(!IsUnicodePrintable(aAccountName) ||
	   !IsAsciiPrintable(aImapSettings.ServerAddress()) ||
	   !IsAsciiPrintable(aImapSettings.FolderPath()))
		{
		User::Leave(KErrArgument);
		}
#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
	// Create  IMAP header table .
	CreateHeaderStoreL(KUidMsgTypeIMAP4);
#endif
	// Create IMAP service entry in the message store
	TMsvId imapService = KMsvNullIndexEntryId;
	CMsvEntry* msvEntry = SessionL().GetEntryL(KMsvRootIndexEntryId);
	CleanupStack::PushL(msvEntry);		
	imapService = CreateEmailServiceL(*msvEntry, aAccountName, KUidMsgTypeIMAP4, ETrue);

	// Save settings to CenRep
	TUint32 imapAccountId = 0;
	TRAPD(error, imapAccountId = DoCreateImapAccountL(aAccountName, aImapSettings, aImapIAP, aSyncDownloadRules, imapService, 0, aReadOnly));
	if (error != KErrNone)
		{
		// saving settings to CenRep failed, so delete imap service and leave
		DeleteServiceEntryL(*msvEntry, imapService);	
		User::Leave(error);
		}

	TImapAccount account;
	account.iImapAccountId = imapAccountId >> KShiftAccountId;
	account.iImapAccountName = aAccountName; 				
	account.iImapService = imapService; 
	account.iSmtpService = KMsvNullIndexEntryId; 	// Related smtp service not created yet		

	// settings saved, so un-set InPreparation Flags	
	UnSetInPreparationFlagsL(*msvEntry, imapService, account.iImapAccountId);
	CleanupStack::PopAndDestroy(msvEntry);	
	return account;				
	}

TUint32 CEmailAccounts::DoCreateImapAccountL(const TDesC& aAccountName,
	const CImImap4Settings& aImapSettings, const CImIAPPreferences& aImapIAP,
	const CImapSyncDownloadRules* aSyncDownloadRules, TMsvId aImapService,
	TMsvId aRelatedService, TBool aReadOnly)
	{
	CRepository& imapRepository = ImapRepositoryL();
	TUint32 imapAccountId = GetNextEmptyAccountSlotL(imapRepository);

	User::LeaveIfError(imapRepository.StartTransaction(CRepository::EReadWriteTransaction));						
	imapRepository.CleanupRollbackTransactionPushL();
	imapRepository.Set(imapAccountId + EIMAPServiceId, static_cast<TInt>(aImapService));
	imapRepository.Set(imapAccountId + EIMAPAccountNameId, aAccountName);
	imapRepository.Set(imapAccountId + EIMAPRelatedSMTPServiceId, static_cast<TInt>(aRelatedService));
	imapRepository.Set(imapAccountId + EIMAPReadOnlyAccountId, static_cast<TInt>(aReadOnly));
	DoSaveImapSettingsL(imapAccountId, aImapSettings);
	if (aSyncDownloadRules)
		{
		DoSaveImapSyncDownloadRulesL(imapAccountId, *aSyncDownloadRules, imapRepository);
		}
	SaveIAPPrefSettingsL(imapAccountId, aImapIAP, imapRepository);
	CleanupStack::Pop();
	TUint32 errorId = 0;	
	User::LeaveIfError(imapRepository.CommitTransaction(errorId));
	
	return imapAccountId;			
	}

/**
Creates a SMTP Account:

1. Creates a SMTP service entry in the message store

2. Creates settings in CenRep with values from the supplied objects

3. Relates this account with the supplied POP account

4. Returns a SMTP account ID

@param	aPopAccount
POP Account Id

@param	aSmtpSettings
SMTP Service Settings

@param	aSmtpIAP
SMTP IAP preferences

@param	aReadOnly
Specify ETrue if it is a read-only account
Note: Read-only accounts should not be modified, however we don't enforce it.

@return
SMTP account ID
@leave
KErrArgument if settings fields contained non-ascii printable characters.
*/
EXPORT_C TSmtpAccount CEmailAccounts::CreateSmtpAccountL(TPopAccount& aPopAccount, const CImSmtpSettings& aSmtpSettings,
		const CImIAPPreferences& aSmtpIAP, TBool aReadOnly)
	{
 	// Check for alpha numerics for the various strings
 	if(!IsAsciiPrintable(aSmtpSettings.Password()) ||
 	   !IsAsciiPrintable(aSmtpSettings.LoginName())||
 	   !IsAsciiPrintable(aSmtpSettings.ServerAddress()))
 		{
 		User::Leave(KErrArgument);
 		}
#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
 	// Create  SMTP header table .
 	CreateHeaderStoreL(KUidMsgTypeSMTP);
#endif
	// Create SMTP service entries in the message store
	TMsvId smtpService = KMsvNullIndexEntryId;
	CMsvEntry* msvEntry = SessionL().GetEntryL(KMsvRootIndexEntryId);
    CleanupStack::PushL(msvEntry);		
	smtpService = CreateEmailServiceL(*msvEntry, aPopAccount.iPopAccountName, KUidMsgTypeSMTP, EFalse);		

	// Save settings to CenRep
	TUint32 smtpAccountId = 0;
	TUint32 popAccountId = aPopAccount.iPopAccountId << KShiftAccountId;
	TRAPD(error, smtpAccountId = DoCreateSmtpAccountL(aPopAccount, aSmtpSettings, 
			aSmtpIAP, smtpService, *msvEntry, popAccountId, aReadOnly));
	if (error != KErrNone)
		{
		// saving settings to CenRep failed, so delete smtp service and leave
		PopRepositoryL().Set(popAccountId + EPOPRelatedSMTPServiceId, 0);
		SetRelatedIdL(*msvEntry, aPopAccount.iPopService, 0);				
		DeleteServiceEntryL(*msvEntry, smtpService);
		User::Leave(error);
		}

	aPopAccount.iSmtpService = smtpService;

	TSmtpAccount account;
	account.iSmtpAccountId = smtpAccountId >> KShiftAccountId;
	account.iSmtpAccountName = aPopAccount.iPopAccountName; 				
	account.iSmtpService = smtpService; 
	account.iRelatedService = aPopAccount.iPopService;	

	// settings saved, so un-set InPreparation Flags
	UnSetInPreparationFlagsL(*msvEntry, smtpService, account.iSmtpAccountId);
	CleanupStack::PopAndDestroy(msvEntry);	
	return account;				
	}

TUint32 CEmailAccounts::DoCreateSmtpAccountL(TPopAccount& aPopAccount, const CImSmtpSettings& aSmtpSettings, 
		const CImIAPPreferences& aSmtpIAP, TMsvId aSmtpService, CMsvEntry& aMsvEntry, TUint32& aPopAccountId, TBool aReadOnly)
	{
	// set related smtp service
	SetRelatedIdL(aMsvEntry, aSmtpService, aPopAccount.iPopService);
		
	// update pop account
	SetRelatedIdL(aMsvEntry, aPopAccount.iPopService, aSmtpService);		
	User::LeaveIfError(PopRepositoryL().Set(aPopAccountId + EPOPRelatedSMTPServiceId, static_cast<TInt>(aSmtpService)));

	return DoCreateSmtpAccountL(aPopAccount.iPopAccountName, aSmtpSettings, aSmtpIAP, aSmtpService, aPopAccount.iPopService, aReadOnly);
	}

/**
Creates a SMTP Account:

1. Creates a SMTP service entry in the message store

2. Creates settings in CenRep with values from the supplied objects

3. Relates this account with the supplied IMAP account

4. Returns a SMTP account ID

@param	aImapAccount
IMAP account ID

@param	aSmtpSettings
SMTP Service Settings

@param	aSmtpIAP
SMTP IAP preferences

@param	aReadOnly
Specify ETrue if it is a read-only account
Note: Read-only accounts should not be modified, however we don't enforce it.

@return
SMTP account ID
@leave
KErrArgument if settings fields contained non-ascii printable characters.
*/
EXPORT_C TSmtpAccount CEmailAccounts::CreateSmtpAccountL(TImapAccount& aImapAccount, const CImSmtpSettings& aSmtpSettings,
		const CImIAPPreferences& aSmtpIAP, TBool aReadOnly)
	{
 	// Check for alpha numerics for the various strings
 	if(!IsAsciiPrintable(aSmtpSettings.Password()) ||
 	   !IsAsciiPrintable(aSmtpSettings.LoginName())||
 	   !IsAsciiPrintable(aSmtpSettings.ServerAddress()))
 		{
 		User::Leave(KErrArgument);
 		}
#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)

	// Create  SMTP header table .
  	CreateHeaderStoreL(KUidMsgTypeSMTP);
#endif 	
	// Create SMTP service entries in the message store
	TMsvId smtpService = KMsvNullIndexEntryId;
	CMsvEntry* msvEntry = SessionL().GetEntryL(KMsvRootIndexEntryId);
    CleanupStack::PushL(msvEntry);		
	smtpService = CreateEmailServiceL(*msvEntry, aImapAccount.iImapAccountName, KUidMsgTypeSMTP, EFalse);		

	// Save settings to CenRep
	TUint32 smtpAccountId = 0;
	TUint32 imapAccountId = aImapAccount.iImapAccountId << KShiftAccountId;
	TRAPD(error, smtpAccountId = DoCreateSmtpAccountL(aImapAccount, aSmtpSettings, 
			aSmtpIAP, smtpService, *msvEntry, imapAccountId, aReadOnly));
	if (error != KErrNone)
		{
		// saving settings to CenRep failed, so delete smtp service and leave
		ImapRepositoryL().Set(imapAccountId + EIMAPRelatedSMTPServiceId, 0);
		SetRelatedIdL(*msvEntry, aImapAccount.iImapService, 0);				
		DeleteServiceEntryL(*msvEntry, smtpService);		
		User::Leave(error);
		}

	aImapAccount.iSmtpService = smtpService;

	TSmtpAccount account;
	account.iSmtpAccountId = smtpAccountId >> KShiftAccountId;
	account.iSmtpAccountName = aImapAccount.iImapAccountName; 				
	account.iSmtpService = smtpService; 
	account.iRelatedService = aImapAccount.iImapService;	

	// settings saved, so un-set InPreparation Flags
	UnSetInPreparationFlagsL(*msvEntry, smtpService, account.iSmtpAccountId);
	CleanupStack::PopAndDestroy(msvEntry);	
	return account;				
	}

TUint32 CEmailAccounts::DoCreateSmtpAccountL(TImapAccount& aImapAccount, const CImSmtpSettings& aSmtpSettings, 
		const CImIAPPreferences& aSmtpIAP, TMsvId aSmtpService, CMsvEntry& aMsvEntry, TUint32& aImapAccountId, TBool aReadOnly)
	{
	// set related smtp service
	SetRelatedIdL(aMsvEntry, aSmtpService, aImapAccount.iImapService);
		
	// update imap account
	SetRelatedIdL(aMsvEntry, aImapAccount.iImapService, aSmtpService);		
	User::LeaveIfError(ImapRepositoryL().Set(aImapAccountId + EIMAPRelatedSMTPServiceId, static_cast<TInt>(aSmtpService)));

	return DoCreateSmtpAccountL(aImapAccount.iImapAccountName, aSmtpSettings, aSmtpIAP, aSmtpService, aImapAccount.iImapService, aReadOnly);
	}

/**
Creates an SMTP Account:

1. Creates an SMTP service entry in the message store

2. Creates settings in CenRep with values from the supplied objects

3. Returns an SMTP account ID

@param	aAccountName
Account Name

@param	aSmtpSettings
SMTP Service Setting

@param	aSmtpIAP
SMTP IAP preferences

@param	aReadOnly
Specify ETrue if it is a read-only account
Note: Read-only accounts should not be modified, however we don't enforce it.

@return
SMTP account ID
@leave
KErrOverflow if the account name is longer than 256 characters.
KErrArgument if settings fields contained non-ascii printable characters.
*/	
EXPORT_C TSmtpAccount CEmailAccounts::CreateSmtpAccountL(const TDesC& aAccountName, const CImSmtpSettings& aSmtpSettings, 
		const CImIAPPreferences& aSmtpIAP, TBool aReadOnly)
	{
	// Check the incoming account name, if its too long then leave with KErrOverflow
	if(aAccountName.Length() > KEmailAccountNameSize)
		{
		User::Leave(KErrOverflow);
		}
#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
	// Create  SMTP header table .
  	CreateHeaderStoreL(KUidMsgTypeSMTP);
#endif 	

 	// Check for alpha numerics for the various strings
 	if(!IsUnicodePrintable(aAccountName) ||
 	   !IsAsciiPrintable(aSmtpSettings.Password()) ||
 	   !IsAsciiPrintable(aSmtpSettings.LoginName())||
 	   !IsAsciiPrintable(aSmtpSettings.ServerAddress()))
 		{
 		User::Leave(KErrArgument);
 		}

	// Create SMTP service entry in the message store
	TMsvId smtpService = KMsvNullIndexEntryId;
	CMsvEntry* msvEntry = SessionL().GetEntryL(KMsvRootIndexEntryId);
    CleanupStack::PushL(msvEntry);		
	smtpService = CreateEmailServiceL(*msvEntry, aAccountName, KUidMsgTypeSMTP, EFalse);	

	// Save settings to CenRep
	TUint32 smtpAccountId = 0;
	TRAPD(error, smtpAccountId = DoCreateSmtpAccountL(aAccountName, aSmtpSettings, aSmtpIAP, smtpService, 0, aReadOnly));
	if (error != KErrNone)
		{
		// saving settings to CenRep failed, so delete smtp service and leave
		DeleteServiceEntryL(*msvEntry, smtpService);
		User::Leave(error);
		}

	// Create account Id
	TSmtpAccount account;
	account.iSmtpAccountId = smtpAccountId >> KShiftAccountId; 
	account.iSmtpAccountName = aAccountName; 						
	account.iSmtpService = smtpService; 
	account.iRelatedService = KMsvNullIndexEntryId;   // Related pop/imap service not created yet

	// settings saved, so un-set InPreparation Flags	
	UnSetInPreparationFlagsL(*msvEntry, smtpService, account.iSmtpAccountId);
	CleanupStack::PopAndDestroy(msvEntry);	
	return account;					
	}

TUint32 CEmailAccounts::DoCreateSmtpAccountL(const TDesC& aAccountName, const CImSmtpSettings& aSmtpSettings, 
		const CImIAPPreferences& aSmtpIAP, TMsvId aSmtpService, TMsvId aRelatedService, TBool aReadOnly)
	{
	CRepository& smtpRepository = SmtpRepositoryL();	
	TUint32 smtpAccountId = GetNextEmptyAccountSlotL(smtpRepository);
		 	
 	User::LeaveIfError(smtpRepository.StartTransaction(CRepository::EReadWriteTransaction));					 	
	smtpRepository.CleanupRollbackTransactionPushL();
	smtpRepository.Set(smtpAccountId, static_cast<TInt>(aSmtpService));
	smtpRepository.Set(smtpAccountId + ESMTPAccountNameId, aAccountName);
	smtpRepository.Set(smtpAccountId + ESMTPRelatedServiceId, static_cast<TInt>(aRelatedService));
	smtpRepository.Set(smtpAccountId + ESMTPReadOnlyAccountId, static_cast<TInt>(aReadOnly));
	smtpRepository.Set(smtpAccountId + ESMTPParentAccountId, KNullSmtpAccountId);
	DoSaveSmtpSettingsL(smtpAccountId, aSmtpSettings);
	SaveIAPPrefSettingsL(smtpAccountId, aSmtpIAP, smtpRepository);	
	CleanupStack::Pop();
	TUint32 errorId = 0;
	User::LeaveIfError(smtpRepository.CommitTransaction(errorId));
	
	return smtpAccountId;
	}

/**
Creates a SMTP mobility account.

An SMTP mobility account allows different send settings to be used by SMTP
accounts depending on what access point is being used by the SMTP connection.
When using this API to create a SMTP mobility account, a list of IAPs and a
set of settings are specified. The SMTP account which owns the SMTP mobility
account is also specified. A SMTP account can own multiple SMTP mobility
accounts.
When a connection is made for a SMTP account, its associated list of SMTP
mobility accounts is examined to see if any of them have an IAP list which
contains the IAP that the connection is using. If so, the settings for that
SMTP mobility account will be used to send emails.
If an SMTP account has no associated SMTP mobility accounts, or there is no
SMTP mobility account with a matching IAP, then the settings for the SMTP
account are used when sending emails.

@param aAccountName Account name
@param aIapList List of IAPs that will use the account settings
@param aSmtpAccount The SMTP account to add the mobility account to
@param aSettings Settings to use for the account
@param aReadOnly Flag to indicate if account is read only. Note that while read
only accounts should not be modified, we do not enforce this.

@publishedPartner
*/
EXPORT_C TSmtpMobilityAccount CEmailAccounts::CreateSmtpMobilityAccountL(const TDesC& aAccountName, const RArrayIAP& aIapList, const TSmtpAccount& aSmtpAccount, const CImSmtpMobilitySettings& aSettings, TBool aReadOnly)
	{
	return DoCreateSmtpMobilityAccountL(aAccountName, aIapList, aSettings, aSmtpAccount, KNullSmtpAccountId, aReadOnly);
	}

/**
Creates a SMTP mobility account using settings from an SMTP account.

An SMTP mobility account allows different send settings to be used by SMTP
accounts depending on what access point is being used by the SMTP connection.
When using this API to create a SMTP mobility account, a list of IAPs and a
linked SMTP account are specified. The settings for the SMTP mobility account
are then taken from that linked SMTP account. The SMTP account which owns the
SMTP mobility account is also specified. A SMTP account can own multiple SMTP
mobility accounts.
When a connection is made for a SMTP account, its associated list of SMTP
mobility accounts is examined to see if any of them have an IAP list which
contains the IAP that the connection is using. If so, the settings for that
SMTP mobility account will be used to send emails.
If an SMTP account has no associated SMTP mobility accounts, or there is no
SMTP mobility account with a matching IAP, then the settings for the SMTP
account are used when sending emails.
Note that a SMTP mobility account created in this way will always have the
same settings as the linked SMTP account. If the settings for the linked
SMTP account change then the settings for the SMTP mobility account also
change to match them. It is not possible to update the settings for a SMTP
mobility account created in this way other than through changing the settings
for the linked SMTP account.

@param aAccountName Account name
@param aIapList List of IAPs that will use the account settings
@param aSmtpAccount The SMTP account to add the mobility account to
@param aLinkedSmtpAccount SMTP account to get settings from
@param aReadOnly Flag to indicate if account is read only. Note that while read
only accounts should not be modified, we do not enforce this.

@leave KErrArgument Attempted to add to an SMTP account which is the same as
       the linked SMTP account

@publishedPartner
*/
EXPORT_C TSmtpMobilityAccount CEmailAccounts::CreateSmtpMobilityAccountL(const TDesC& aAccountName, const RArrayIAP& aIapList, const TSmtpAccount& aSmtpAccount, const TSmtpAccount& aLinkedSmtpAccount, TBool aReadOnly)
	{
	if (aSmtpAccount.iSmtpAccountId == aLinkedSmtpAccount.iSmtpAccountId)
		{
		User::Leave(KErrArgument);
		}

	CImSmtpMobilitySettings* mobilitySettings = CImSmtpMobilitySettings::NewLC();

	CImSmtpSettings* smtpSettings = new(ELeave) CImSmtpSettings;
	CleanupStack::PushL(smtpSettings);
	LoadSmtpSettingsL(aLinkedSmtpAccount, *smtpSettings);
	mobilitySettings->SetSmtpSettings(smtpSettings);
	CleanupStack::Pop(smtpSettings);

	TSmtpMobilityAccount account = DoCreateSmtpMobilityAccountL(aAccountName, aIapList, *mobilitySettings, aSmtpAccount, aLinkedSmtpAccount.iSmtpAccountId, aReadOnly);

	CleanupStack::PopAndDestroy(mobilitySettings);
	return account;
	}

/**
Gets a list of POP account IDs that exists in the current message store. 

@param	aAccounts
On return, an array of POP account IDs
*/ 
EXPORT_C void CEmailAccounts::GetPopAccountsL(RArray<TPopAccount>& aAccounts)
	{
    CleanupClosePushL( aAccounts );
	aAccounts.Reset();
	CRepository& popRepository = PopRepositoryL();
	
	// Get a list of pop accounts from CenRep
	RArray<TUint32> popAccountIds;	
	CleanupClosePushL(popAccountIds);	
	TInt error = popRepository.FindL(KNullId, static_cast<TUint32>(EAccountMask), popAccountIds);
	if (error != KErrNotFound)
		{
		User::LeaveIfError(error);
		}

	// Get a list of pop service entries from the message store
	CMsvEntry* entry = SessionL().GetEntryL(KMsvRootIndexEntryId);
	CleanupStack::PushL(entry);

	CMsvEntrySelection* popServiceEntries = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(popServiceEntries);
	GetEmailServicesL(*popServiceEntries, KUidMsgTypePOP3, *entry);
	
	// Append Ids to the array	
	TPopAccount accountId;
	TInt temp = 0;
	TInt count = popAccountIds.Count();
	for(TInt i = 1; i < count; ++i)
		{
		// create account Id
		TUint32 settingId = popAccountIds[i];
		User::LeaveIfError(popRepository.Get(settingId + EPOPAccountNameId, accountId.iPopAccountName));	
		User::LeaveIfError(popRepository.Get(settingId + EPOPServiceId, temp));	
		accountId.iPopService = static_cast<TMsvId>(temp);
		User::LeaveIfError(popRepository.Get(settingId + EPOPRelatedSMTPServiceId, temp));	
		accountId.iSmtpService = static_cast<TMsvId>(temp);
		accountId.iPopAccountId = popAccountIds[i] >> KShiftAccountId;
		
		TInt index = popServiceEntries->Find(accountId.iPopService);		
		if (index != KErrNotFound)
			{
			entry->SetEntryL(popServiceEntries->At(index));
			if (entry->Entry().MtmData2() == accountId.iPopAccountId)
				{
				aAccounts.AppendL(accountId);
				}				
			}		
		}
		
	CleanupStack::PopAndDestroy(3, &popAccountIds);
	CleanupStack::Pop(&aAccounts); // aAccounts
	}

/**
Gets a POP account ID for the specified service.

@param	aServiceId
Service ID

@param	aAccount
POP account ID
*/ 	
EXPORT_C void CEmailAccounts::GetPopAccountL(TMsvId aServiceId, TPopAccount& aAccount)
	{
	CRepository& popRepository = PopRepositoryL();
	TUint32 account = FindEmailAccountL(aServiceId, popRepository);	
	aAccount.iPopAccountId = account >> KShiftAccountId;	
	
	User::LeaveIfError(popRepository.Get(account + EPOPAccountNameId, aAccount.iPopAccountName));	
 	TInt temp = 0;
 	User::LeaveIfError(popRepository.Get(account + EPOPServiceId, temp));	
 	aAccount.iPopService = static_cast<TMsvId>(temp);
 	User::LeaveIfError(popRepository.Get(account + EPOPRelatedSMTPServiceId, temp));	
 	aAccount.iSmtpService = static_cast<TMsvId>(temp);
	}

/**
Deletes the specified POP account.

@param	aAccount
POP account ID
*/    
EXPORT_C void CEmailAccounts::DeletePopAccountL(const TPopAccount& aAccount)
	{
	CRepository& popRepository = PopRepositoryL();
	TUint32 popAccountId = aAccount.iPopAccountId << KShiftAccountId;

	// Get POP service Id
	TInt temp = 0;
	User::LeaveIfError(popRepository.Get(popAccountId + EPOPServiceId, temp));	
	TMsvId popService = static_cast<TMsvId>(temp);

	// Delete service pop service entry
	CMsvEntry* entry = CMsvEntry::NewL(SessionL(), KMsvRootIndexEntryId, TMsvSelectionOrdering(KMsvNoGrouping, EMsvSortByNone, ETrue));
	CleanupStack::PushL(entry);
	DeleteServiceEntryL(*entry, popService);		
	CleanupStack::PopAndDestroy(entry);		

	// Delete settings in CenRep
	User::LeaveIfError(popRepository.StartTransaction(CRepository::EReadWriteTransaction));						
	popRepository.CleanupRollbackTransactionPushL();
	DeletePopSettings(popAccountId, popRepository);
	DeleteIAPPrefSettingsL(popAccountId, popRepository);
	CleanupStack::Pop();
	TUint32 errorId = 0;
	User::LeaveIfError(popRepository.CommitTransaction(errorId));
	}

/**
Gets a list of IMAP account IDs that exists in the current message store.

@param	aAccounts
On return, an array of IMAP account IDs
*/    			
EXPORT_C void CEmailAccounts::GetImapAccountsL(RArray<TImapAccount>& aAccounts)
	{
    CleanupClosePushL( aAccounts );
	aAccounts.Reset();
	CRepository& imapRepository = ImapRepositoryL();

	// Get a list of imap accounts from CenRep
	RArray<TUint32> imapAccountIds;
	CleanupClosePushL(imapAccountIds);	
	TInt error = imapRepository.FindL(KNullId, static_cast<TUint32>(EAccountMask), imapAccountIds);
	if (error != KErrNotFound)
		{
		User::LeaveIfError(error);
		}

	// Get a list of imap service entries from the message store
	CMsvEntry* entry = SessionL().GetEntryL(KMsvRootIndexEntryId);
	CleanupStack::PushL(entry);

	CMsvEntrySelection* imapServiceEntries = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(imapServiceEntries);
	GetEmailServicesL(*imapServiceEntries, KUidMsgTypeIMAP4, *entry);
		
	// Append Ids to the array	
	TImapAccount accountId;	
	TInt temp = 0;
	TInt count = imapAccountIds.Count();
	for (TInt i = 1; i < count; ++i)
		{
		// create account Id
		TUint32 settingId = imapAccountIds[i];
		User::LeaveIfError(imapRepository.Get(settingId + EIMAPAccountNameId, accountId.iImapAccountName));	
 		User::LeaveIfError(imapRepository.Get(settingId + EIMAPServiceId, temp));	
 		accountId.iImapService = static_cast<TMsvId>(temp);
 		User::LeaveIfError(imapRepository.Get(settingId + EIMAPRelatedSMTPServiceId, temp));	
 		accountId.iSmtpService = static_cast<TMsvId>(temp);
		accountId.iImapAccountId = imapAccountIds[i] >> KShiftAccountId;

		TInt index = imapServiceEntries->Find(accountId.iImapService);		
		if (index != KErrNotFound)
			{
			entry->SetEntryL(imapServiceEntries->At(index));
			if (entry->Entry().MtmData2() == accountId.iImapAccountId)
				{
				aAccounts.AppendL(accountId);
				}				
			}		
		}

	CleanupStack::PopAndDestroy(3, &imapAccountIds);	
	CleanupStack::Pop(&aAccounts);  // aAccounts
	}

void CEmailAccounts::GetEmailServicesL(CMsvEntrySelection& aServices, TUid aMTMType, CMsvEntry& aMsvEntry)
	{
	aServices.Reset();
	TMsvSelectionOrdering order;
	order.SetShowInvisibleEntries(ETrue);
	aMsvEntry.SetSortTypeL(order);
	
	CMsvEntrySelection* services = aMsvEntry.ChildrenWithTypeL(KUidMsvServiceEntry);
	CleanupStack::PushL(services);

	TInt serviceCount = services->Count();
	for (TInt ii=0; ii<serviceCount; ++ii)
		{
		aMsvEntry.SetEntryL(services->At(ii));
		if (aMsvEntry.Entry().iMtm == aMTMType)
			{
			aServices.AppendL(aMsvEntry.Entry().Id());
			}
		}	

	CleanupStack::PopAndDestroy(services);			
	}

/**
Gets a IMAP account ID for the specified service.

@param	aServiceId
Service ID

@param	aAccount
IMAP account ID
*/ 		
EXPORT_C void CEmailAccounts::GetImapAccountL(TMsvId aServiceId, TImapAccount& aAccount)
	{
	CRepository& imapRepository = ImapRepositoryL();
	TUint32 account = FindEmailAccountL(aServiceId, imapRepository);	
	aAccount.iImapAccountId = account >> KShiftAccountId;	
	
	User::LeaveIfError(imapRepository.Get(account + EIMAPAccountNameId, aAccount.iImapAccountName));	
 	TInt temp = 0;
 	User::LeaveIfError(imapRepository.Get(account + EIMAPServiceId, temp));	
 	aAccount.iImapService = static_cast<TMsvId>(temp);
 	User::LeaveIfError(imapRepository.Get(account + EIMAPRelatedSMTPServiceId, temp));	
 	aAccount.iSmtpService = static_cast<TMsvId>(temp);		
	}
	
/**
Deletes the specified IMAP account.

@param	aAccount
IMAP account ID
*/
EXPORT_C void CEmailAccounts::DeleteImapAccountL(const TImapAccount& aAccount)
	{
	TUint32 imapAccountId = aAccount.iImapAccountId << KShiftAccountId;
	CRepository& imapRepository = ImapRepositoryL();

	// Get IMAP service Id
	TInt temp = 0;
	User::LeaveIfError(imapRepository.Get(imapAccountId + EIMAPServiceId, temp));	
	TMsvId imapService = static_cast<TMsvId>(temp);
				
	// Delete service entries
	CMsvEntry* entry = CMsvEntry::NewL(SessionL(), KMsvRootIndexEntryId, TMsvSelectionOrdering(KMsvNoGrouping, EMsvSortByNone, ETrue));
	CleanupStack::PushL(entry);
	DeleteServiceEntryL(*entry, imapService);		
	CleanupStack::PopAndDestroy(entry);	

	// Delete settings in CenRep
	User::LeaveIfError(imapRepository.StartTransaction(CRepository::EReadWriteTransaction));						
	imapRepository.CleanupRollbackTransactionPushL();
	DeleteImapSettings(imapAccountId, imapRepository);
	DeleteIAPPrefSettingsL(imapAccountId, imapRepository);
	DeleteImapSyncDownloadRulesL(imapAccountId, imapRepository);
	CleanupStack::Pop();
	TUint32 errorId = 0;	
	User::LeaveIfError(imapRepository.CommitTransaction(errorId));
	}

/**
Gets a list of SMTP account IDs that exists in the current message store.

@param	aAccounts
On return, an array of SMTP account IDs
*/    			
EXPORT_C void CEmailAccounts::GetSmtpAccountsL(RArray<TSmtpAccount>& aAccounts)
	{
    CleanupClosePushL( aAccounts );
	aAccounts.Reset();
	CRepository& smtpRepository = SmtpRepositoryL();
	
	// Get a list of smtp accounts from CenRep
	RArray<TUint32> smtpAccountIds;
	CleanupClosePushL(smtpAccountIds);	
	TInt error = smtpRepository.FindL(KNullId, static_cast<TUint32>(EAccountMask), smtpAccountIds);
	if (error != KErrNotFound)
		{
		User::LeaveIfError(error);
		}

	// Get a list of smtp service entries from the message store
	CMsvEntry* entry = SessionL().GetEntryL(KMsvRootIndexEntryId);
	CleanupStack::PushL(entry);

	CMsvEntrySelection* smtpServiceEntries = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(smtpServiceEntries);
	GetEmailServicesL(*smtpServiceEntries, KUidMsgTypeSMTP, *entry);
		
	// Append Ids to the array	
	TSmtpAccount accountId;	
	TInt temp = 0;
	TInt count = smtpAccountIds.Count();
	for (TInt i = 1; i < count; ++i)
		{
		TUint32 settingId = smtpAccountIds[i];

		User::LeaveIfError(smtpRepository.Get(settingId + ESMTPAccountNameId, accountId.iSmtpAccountName));	
 		User::LeaveIfError(smtpRepository.Get(settingId + ESMTPServiceId, temp));	
 		accountId.iSmtpService = static_cast<TMsvId>(temp);
 		User::LeaveIfError(smtpRepository.Get(settingId + ESMTPRelatedServiceId, temp));	
 		accountId.iRelatedService = static_cast<TMsvId>(temp);
		accountId.iSmtpAccountId = smtpAccountIds[i] >> KShiftAccountId;

		TInt index = smtpServiceEntries->Find(accountId.iSmtpService);		
		if (index != KErrNotFound)
			{
			entry->SetEntryL(smtpServiceEntries->At(index));
			if (entry->Entry().MtmData2() == accountId.iSmtpAccountId)
				{
				aAccounts.AppendL(accountId);
				}				
			}		
		}

	CleanupStack::PopAndDestroy(3, &smtpAccountIds);		
	CleanupStack::Pop(&aAccounts);  // aAccounts
	}

/**
Get a list of SMTP mobility accounts for a given SMTP account

@param aSmtpAccount SMTP account
@param aAccounts Stores the list of SMTP mobility accounts

@publishedPartner
*/
EXPORT_C void CEmailAccounts::GetSmtpMobilityAccountsL(const TSmtpAccount& aSmtpAccount, RArray<TSmtpMobilityAccount>& aAccounts)
	{
    CleanupClosePushL( aAccounts );
	aAccounts.Reset();

	CRepository& repository = SmtpRepositoryL();
	TUint32 smtpAccRepId = aSmtpAccount.iSmtpAccountId << KShiftAccountId;

	TInt numEntries = 0;
	TInt err = repository.Get(smtpAccRepId + ESMTPMobilityAccountListNumEntriesId, numEntries);	
	if (err == KErrNone)
		{
		TSmtpMobilityAccount mobAccount;
		TUint32 mobAccRepId = 0;
		TInt tempInt = 0;
		TUint32 offset = 0;

		for (TInt entryLoop = 0; entryLoop < numEntries; ++entryLoop)
			{
			offset = smtpAccRepId + (entryLoop * ESMTPMobilityAccountListOffset);

			User::LeaveIfError(repository.Get(offset + ESMTPMobilityAccountListAccountId, tempInt));
			mobAccRepId = tempInt;

			User::LeaveIfError(repository.Get(mobAccRepId + ESMTPAccountNameId, mobAccount.iName));
			User::LeaveIfError(repository.Get(mobAccRepId + ESMTPParentAccountId, tempInt));
			mobAccount.iParentSmtpAccountId = tempInt;
			User::LeaveIfError(repository.Get(mobAccRepId + ESMTPLinkedAccountId, tempInt));
			mobAccount.iLinkedSmtpAccountId = tempInt;
			mobAccount.iAccountId = mobAccRepId >> KShiftAccountId;
			aAccounts.AppendL(mobAccount);
			}
		}
	else if (err != KErrNotFound)
		{
		User::Leave(err);
		}
	
	CleanupStack::Pop(&aAccounts);  // aAccounts
	}

/**
Get a list of all SMTP mobility accounts in the repository

@param aAccounts Stores the list of SMTP mobility accounts

@internalTechnology
*/
EXPORT_C void CEmailAccounts::GetSmtpMobilityAccountsL(RArray<TSmtpMobilityAccount>& aAccounts)
	{
    CleanupClosePushL( aAccounts );
	aAccounts.Reset();

	CRepository& repository = SmtpRepositoryL();

	RArray<TUint32> smtpAccountIds;
	CleanupClosePushL(smtpAccountIds);	
	TInt error = repository.FindL(KNullId, static_cast<TUint32>(EAccountMask), smtpAccountIds);
	if (error != KErrNotFound)
		{
		User::LeaveIfError(error);
		}

	TSmtpMobilityAccount mobAccount;
	TInt tempInt;
	for (TInt count = 1; count < smtpAccountIds.Count(); ++count)
		{
		if (IsSmtpMobilityAccount(repository, smtpAccountIds[count]))
			{
			User::LeaveIfError(repository.Get(smtpAccountIds[count] + ESMTPAccountNameId, mobAccount.iName));
			User::LeaveIfError(repository.Get(smtpAccountIds[count] + ESMTPParentAccountId, tempInt));
			mobAccount.iParentSmtpAccountId = tempInt;
			User::LeaveIfError(repository.Get(smtpAccountIds[count] + ESMTPLinkedAccountId, tempInt));
			mobAccount.iLinkedSmtpAccountId = tempInt;
			mobAccount.iAccountId = smtpAccountIds[count] >> KShiftAccountId;
			aAccounts.AppendL(mobAccount);
			}
		}

	CleanupStack::PopAndDestroy(&smtpAccountIds);
	CleanupStack::Pop(&aAccounts);  // aAccounts
	}

/**
Gets a SMTP account ID for the specified service.

@param	aServiceId
Service ID

@param	aAccount
SMTP account ID
*/ 			
EXPORT_C void CEmailAccounts::GetSmtpAccountL(TMsvId aServiceId, TSmtpAccount& aAccount)
	{
	CRepository& smtpRepository = SmtpRepositoryL();	
	TUint32 account = FindEmailAccountL(aServiceId, smtpRepository);	
	aAccount.iSmtpAccountId = account >> KShiftAccountId;	
	
	User::LeaveIfError(smtpRepository.Get(account + ESMTPAccountNameId, aAccount.iSmtpAccountName));	
 	TInt temp = 0;
 	User::LeaveIfError(smtpRepository.Get(account + ESMTPServiceId, temp));	
 	aAccount.iSmtpService = static_cast<TMsvId>(temp);
 	User::LeaveIfError(smtpRepository.Get(account + ESMTPRelatedServiceId, temp));	
 	aAccount.iRelatedService = static_cast<TMsvId>(temp);			
	}


/**
Gets a SMTP account for the specified account ID.

@param aAccountId Account ID
@param aAccount Stores account details

@leave KErrNotFound Account ID specifies a SMTP mobility account

@publishedPartner
*/
EXPORT_C void CEmailAccounts::GetSmtpAccountL(TSmtpAccountId aAccountId, TSmtpAccount& aAccount)
	{
	CRepository& repository = SmtpRepositoryL();	
	TUint32 smtpAccRepId = aAccountId << KShiftAccountId;

	if (IsSmtpMobilityAccount(repository, smtpAccRepId))
		{
		User::Leave(KErrNotFound);
		}

	aAccount.iSmtpAccountId = aAccountId;

	TInt tempInt = 0;
	User::LeaveIfError(repository.Get(smtpAccRepId + ESMTPAccountNameId, aAccount.iSmtpAccountName));
 	User::LeaveIfError(repository.Get(smtpAccRepId + ESMTPServiceId, tempInt));
 	aAccount.iSmtpService = static_cast<TMsvId>(tempInt);
 	User::LeaveIfError(repository.Get(smtpAccRepId + ESMTPRelatedServiceId, tempInt));
 	aAccount.iRelatedService = static_cast<TMsvId>(tempInt);
	}

/**
Deletes the specified SMTP account.

@param	aAccount
SMTP account ID

@leave KErrNotFound Account specified is a SMTP mobility account
*/
EXPORT_C void CEmailAccounts::DeleteSmtpAccountL(const TSmtpAccount& aAccount)
	{
	TUint32 smtpAccountId = aAccount.iSmtpAccountId << KShiftAccountId;
	CRepository& smtpRepository = SmtpRepositoryL();	

	if (IsSmtpMobilityAccount(smtpRepository, smtpAccountId))
		{
		User::Leave(KErrNotFound);
		}
	
	// Get SMTP service Id
	TInt temp = 0;
	User::LeaveIfError(smtpRepository.Get(smtpAccountId + ESMTPServiceId, temp));	
	TMsvId smtpService = static_cast<TMsvId>(temp);
	
	// Check if it is the default account that is being deleted
	TBool isDefault = EFalse;
	TInt defaultAccount;
	TInt error = smtpRepository.Get(KDefaultAccountId, defaultAccount);
	if (error == KErrNone && defaultAccount == aAccount.iSmtpAccountId)
		{
		isDefault = ETrue;
		}
	else if (error != KErrNotFound)
		{
		User::LeaveIfError(error);
		}
		
	// Delete service entries
	CMsvEntry* entry = CMsvEntry::NewL(SessionL(), KMsvRootIndexEntryId, TMsvSelectionOrdering(KMsvNoGrouping, EMsvSortByNone, ETrue));
	CleanupStack::PushL(entry);	
	DeleteServiceEntryL(*entry, smtpService);	
	CleanupStack::PopAndDestroy(entry);	

	// Delete settings in CenRep		
	User::LeaveIfError(smtpRepository.StartTransaction(CRepository::EReadWriteTransaction));
	smtpRepository.CleanupRollbackTransactionPushL();
	DeleteSmtpSettings(smtpAccountId, smtpRepository);
	DeleteIAPPrefSettingsL(smtpAccountId, smtpRepository);
	DeleteAllOwnedMobilityAccountsL(smtpAccountId, smtpRepository);
	DeleteAllLinkedMobilityAccountsL(smtpAccountId, smtpRepository);
	if (isDefault)
		{		
		// Deleting default account, so remove the default account setting
		smtpRepository.Delete(KDefaultAccountId);	
		}
	CleanupStack::Pop();
	TUint32 errorId = 0;	
	User::LeaveIfError(smtpRepository.CommitTransaction(errorId));
	}

/**
Deletes the default SMTP account information from repository.
*/
EXPORT_C void CEmailAccounts::DeleteDefaultSmtpAccountL()
	{
	CRepository& smtpRepository = SmtpRepositoryL();	
	// Deleting default account info
	User::LeaveIfError(smtpRepository.Delete(KDefaultAccountId));	
	}

/**
Deletes the specified SMTP bearer mobility account

@param aAccount SMTP mobility account

@leave KErrNotFound Specified account is not a SMTP mobility account

@publishedPartner
*/
EXPORT_C void CEmailAccounts::DeleteSmtpMobilityAccountL(const TSmtpMobilityAccount& aAccount)
	{
	TUint32 mobAccRepId = aAccount.iAccountId << KShiftAccountId;
	CRepository& repository = SmtpRepositoryL();	

	if (!IsSmtpMobilityAccount(repository, mobAccRepId))
		{
		User::Leave(KErrNotFound);
		}

	TInt tempInt = 0;
	User::LeaveIfError(repository.Get(mobAccRepId + ESMTPParentAccountId, tempInt));
	TUint32 smtpAccRepId = static_cast<TUint32>(tempInt) << KShiftAccountId;

	User::LeaveIfError(repository.StartTransaction(CRepository::EReadWriteTransaction));
	repository.CleanupRollbackTransactionPushL();

	DeleteSmtpMobilityAccountListEntryL(mobAccRepId, repository, smtpAccRepId);

	DeleteSmtpMobilityAccountFromLinkedSmtpAccountL(mobAccRepId, repository);

	DeleteSmtpSettings(mobAccRepId, repository);

	CleanupStack::Pop();
	TUint32 keyInfo = 0;	
	User::LeaveIfError(repository.CommitTransaction(keyInfo));
	}

/**
Populates the supplied setting objects with default values.

@param	aPopSettings
POP Service Setting

@param	aPopIap
POP IAP preferences
*/			
EXPORT_C void CEmailAccounts::PopulateDefaultPopSettingsL(CImPop3Settings& aPopSettings,
	CImIAPPreferences& aPopIap)
	{
	DoLoadPopSettingsL(KInitialiseSettings, aPopSettings);		
	LoadIAPPrefSettingsL(KInitialiseSettings, aPopIap, PopRepositoryL());	
	}

/**
Populates the supplied setting objects with default values.

@param	aImapSettings
IMAP Service Setting

@param	aImapIap
IMAP IAP preferences
*/			
EXPORT_C void CEmailAccounts::PopulateDefaultImapSettingsL(CImImap4Settings& aImapSettings,
	CImIAPPreferences& aImapIap)
	{
	DoLoadImapSettingsL(KInitialiseSettings, aImapSettings);		
	LoadIAPPrefSettingsL(KInitialiseSettings, aImapIap, ImapRepositoryL());	
	}

/**Populates the supplied setting objects with default values.

@param	aSmtpSettings
SMTP Service Settings 

@param	aSmtpIap
SMTP IAP preferences
*/			
EXPORT_C void CEmailAccounts::PopulateDefaultSmtpSettingsL(CImSmtpSettings& aSmtpSettings,
	CImIAPPreferences& aSmtpIap)
	{
	DoLoadSmtpSettingsL(KInitialiseSettings, aSmtpSettings);			
	LoadIAPPrefSettingsL(KInitialiseSettings, aSmtpIap, SmtpRepositoryL());	
	}

/**
Populates the supplied settings object with default values

@param aSettings SMTP mobility account settings

@publishedPartner
*/			
EXPORT_C void CEmailAccounts::PopulateDefaultSmtpMobilitySettingsL(CImSmtpMobilitySettings& aSettings)
	{
	// Get default settings for a SMTP account and use these to populate the
	// settings from the SMTP mobility account.
	CImSmtpSettings* smtpSettings = new (ELeave) CImSmtpSettings();
	CleanupStack::PushL(smtpSettings);
	DoLoadSmtpSettingsL(KInitialiseSettings, *smtpSettings);
	CleanupStack::Pop(smtpSettings);
	aSettings.SetSmtpSettings(smtpSettings);
	}

/**
Returns the default SMTP account.

@param	aAccount
Default account ID

@return KErrNone if default account found, KErrNotFound if not found
*/			
EXPORT_C TInt CEmailAccounts::DefaultSmtpAccountL(TSmtpAccount& aAccount)
	{
	CRepository& smtpRepository = SmtpRepositoryL();	
	TInt error = smtpRepository.Get(KDefaultAccountId, aAccount.iSmtpAccountId);
	if (error == KErrNone)
		{
		// default account found, retrieve account details
		TUint32 cenrepAccountId = aAccount.iSmtpAccountId << KShiftAccountId;
		User::LeaveIfError(smtpRepository.Get(cenrepAccountId + ESMTPAccountNameId, aAccount.iSmtpAccountName));	
	 	TInt temp = 0;
	 	User::LeaveIfError(smtpRepository.Get(cenrepAccountId + ESMTPServiceId, temp));	
	 	aAccount.iSmtpService = static_cast<TMsvId>(temp);
	 	User::LeaveIfError(smtpRepository.Get(cenrepAccountId + ESMTPRelatedServiceId, temp));	
	 	aAccount.iRelatedService = static_cast<TMsvId>(temp);
		}
	else if (error != KErrNotFound)
		{		
		User::Leave(error);
		}				
	return error;		
	}
	
/**
Sets the default smtp account.

@param	aAccount
Account ID to set as default 
*/				
EXPORT_C void CEmailAccounts::SetDefaultSmtpAccountL(const TSmtpAccount& aAccount)
	{
	CRepository& smtpRepository = SmtpRepositoryL();	

	TUint32 smtpAccountId = aAccount.iSmtpAccountId << KShiftAccountId;

	TInt temp = 0;

	// Check the account actually exists
	User::LeaveIfError(smtpRepository.Get(smtpAccountId + ESMTPServiceId, temp));	

	if (IsSmtpMobilityAccount(smtpRepository, smtpAccountId))
		{
		User::Leave(KErrNotFound);
		}

	User::LeaveIfError(smtpRepository.Set(KDefaultAccountId, aAccount.iSmtpAccountId));
	}

/**
Loads POP service settings for the specified account. 

@param	aAccount
POP account ID

@param	aPopSettings
POP service settings
*/	
EXPORT_C void CEmailAccounts::LoadPopSettingsL(const TPopAccount& aAccount, CImPop3Settings& aPopSettings)
	{
	TUint32 accountId = aAccount.iPopAccountId << KShiftAccountId;	
	DoLoadPopSettingsL(accountId, aPopSettings);	
	}

/**
Loads POP IAP settings for the specified account. 

@param	aAccount
POP account ID

@param	aIAP
POP IAP Pref settings
*/		
EXPORT_C void CEmailAccounts::LoadPopIapSettingsL(const TPopAccount& aAccount, CImIAPPreferences& aIAP)
	{
	TUint32 accountId = aAccount.iPopAccountId << KShiftAccountId;		
	LoadIAPPrefSettingsL(accountId, aIAP, PopRepositoryL());
	}

/**
Loads IMAP service settings for the specified account. 

@param	aAccount
IMAP account ID

@param	aImapSettings
IMAP service settings
*/	
EXPORT_C void CEmailAccounts::LoadImapSettingsL(const TImapAccount& aAccount, CImImap4Settings& aImapSettings)
	{	
	TUint32 accountId = aAccount.iImapAccountId << KShiftAccountId;	
	DoLoadImapSettingsL(accountId, aImapSettings);		
	}

/**
Loads IMAP synchronisation download rules

@param aAccount IMAP account ID
@param aRules Synchronisation download rules

@publishedPartner
*/
EXPORT_C void CEmailAccounts::LoadImapSyncDownloadRulesL(const TImapAccount& aAccount, CImapSyncDownloadRules& aRules)
	{
	aRules.Reset();

	TUint32 accountId = aAccount.iImapAccountId << KShiftAccountId;

	CRepository& imapRepository = ImapRepositoryL();

	TInt version;
	
	if (imapRepository.Get(accountId + EIMAPSyncDownloadRulesVersionId, version) != KErrNone)
		{
		return;
		}

	TInt numRules;
	User::LeaveIfError(imapRepository.Get(accountId + EIMAPNumInboxSyncDownloadRulesId, numRules));

	TInt ruleOffset;
	TUint32 bearerTypes;
	TImImap4GetPartialMailInfo mailInfo;
	TInt tempInt;

	for (TInt rule = 0; rule < numRules; ++rule)
		{
		ruleOffset = accountId + (rule * EIMAPSyncDownloadRulesOffset);

		User::LeaveIfError(imapRepository.Get(ruleOffset + EIMAPInboxSyncDownloadRulesBearerTypesId, tempInt));
		bearerTypes = tempInt;

		User::LeaveIfError(imapRepository.Get(ruleOffset + EIMAPInboxSyncDownloadRulesMailOptionsId, tempInt));
		mailInfo.iPartialMailOptions = static_cast<TImImap4PartialMailOptions>(tempInt);

		User::LeaveIfError(imapRepository.Get(ruleOffset + EIMAPInboxSyncDownloadRulesTotalSizeLimitId, tempInt));
		mailInfo.iTotalSizeLimit = tempInt;

		User::LeaveIfError(imapRepository.Get(ruleOffset + EIMAPInboxSyncDownloadRulesBodyTextSizeLimitId, tempInt));
		mailInfo.iBodyTextSizeLimit = tempInt;

		User::LeaveIfError(imapRepository.Get(ruleOffset + EIMAPInboxSyncDownloadRulesAttachmentSizeLimitId, tempInt));
		mailInfo.iAttachmentSizeLimit = tempInt;

		User::LeaveIfError(imapRepository.Get(ruleOffset + EIMAPInboxSyncDownloadRulesGetMailBodyPartsId, tempInt));
		mailInfo.iGetMailBodyParts = static_cast<TImap4GetMailOptions>(tempInt);

		User::LeaveIfError(imapRepository.Get(ruleOffset + EIMAPInboxSyncDownloadRulesMaxEmailSizeId, tempInt));
		mailInfo.iMaxEmailSize = tempInt;

		aRules.AddRuleL(CImapSyncDownloadRules::EInboxRulesType, rule, bearerTypes, mailInfo);
		}

	User::LeaveIfError(imapRepository.Get(accountId + EIMAPNumFolderSyncDownloadRulesId, numRules));

	for (TInt rule = 0; rule < numRules; ++rule)
		{
		ruleOffset = accountId + (rule * EIMAPSyncDownloadRulesOffset);

		User::LeaveIfError(imapRepository.Get(ruleOffset + EIMAPFolderSyncDownloadRulesBearerTypesId, tempInt));
		bearerTypes = tempInt;

		User::LeaveIfError(imapRepository.Get(ruleOffset + EIMAPFolderSyncDownloadRulesMailOptionsId, tempInt));
		mailInfo.iPartialMailOptions = static_cast<TImImap4PartialMailOptions>(tempInt);

		User::LeaveIfError(imapRepository.Get(ruleOffset + EIMAPFolderSyncDownloadRulesTotalSizeLimitId, tempInt));
		mailInfo.iTotalSizeLimit = tempInt;

		User::LeaveIfError(imapRepository.Get(ruleOffset + EIMAPFolderSyncDownloadRulesBodyTextSizeLimitId, tempInt));
		mailInfo.iBodyTextSizeLimit = tempInt;

		User::LeaveIfError(imapRepository.Get(ruleOffset + EIMAPFolderSyncDownloadRulesAttachmentSizeLimitId, tempInt));
		mailInfo.iAttachmentSizeLimit = tempInt;

		User::LeaveIfError(imapRepository.Get(ruleOffset + EIMAPFolderSyncDownloadRulesGetMailBodyPartsId, tempInt));
		mailInfo.iGetMailBodyParts = static_cast<TImap4GetMailOptions>(tempInt);

		User::LeaveIfError(imapRepository.Get(ruleOffset + EIMAPFolderSyncDownloadRulesMaxEmailSizeId, tempInt));
		mailInfo.iMaxEmailSize = tempInt;

		aRules.AddRuleL(CImapSyncDownloadRules::EFolderRulesType, rule, bearerTypes, mailInfo);
		}
	}

/**
Loads IMAP IAP settings for the specified account. 

@param	aAccount
IMAP account ID

@param	aIAP
IMAP IAP Pref settings
*/			
EXPORT_C void CEmailAccounts::LoadImapIapSettingsL(const TImapAccount& aAccount, CImIAPPreferences& aIAP)
	{	
	TUint32 accountId = aAccount.iImapAccountId << KShiftAccountId;		
	LoadIAPPrefSettingsL(accountId, aIAP, ImapRepositoryL());	
	}

/**
Loads IMAP transport buffer sizes

@param aBufferSizes Transport buffer sizes

@publishedPartner
*/
EXPORT_C void CEmailAccounts::LoadImapTransportBufferSizesL(CImapTransportBufferSizes& aBufferSizes)
	{
	aBufferSizes.Reset();

	CRepository& imapRepository = ImapRepositoryL();

	TInt version;

	if (imapRepository.Get(EIMAPTransportBufferSizesVersionId, version) != KErrNone)
		{
		return;
		}

	TInt numEntries;
	User::LeaveIfError(imapRepository.Get(EIMAPNumTransportBufferSizesId, numEntries));

	TInt entryOffset;
	TUint32 bearerTypes;
	TInt fetchRequestSize;
	TInt maxOutstandingFetchResponses;
	TInt tempInt;

	for (TInt entry = 0; entry < numEntries; ++entry)
		{
		entryOffset = entry * EIMAPTransportBufferSizesOffset;

		User::LeaveIfError(imapRepository.Get(entryOffset + EIMAPTransportBufferSizesBearerTypesId, tempInt));
		bearerTypes = tempInt;

		User::LeaveIfError(imapRepository.Get(entryOffset + EIMAPTransportBufferSizesFetchRequestSizeId, fetchRequestSize));

		User::LeaveIfError(imapRepository.Get(entryOffset + EIMAPTransportBufferSizesMaxOutstandingFetchResponsesId, maxOutstandingFetchResponses));

		aBufferSizes.AddSizesL(entry, bearerTypes, fetchRequestSize, maxOutstandingFetchResponses);
		}
	}

/**
Loads SMTP service settings for the specified account. 

@param	aAccount
SMTP account ID

@param	aSmtpSettings
SMTP service settings

@leave KErrNotFound Account specified is a SMTP mobility account
*/	
EXPORT_C void CEmailAccounts::LoadSmtpSettingsL(const TSmtpAccount& aAccount, CImSmtpSettings& aSmtpSettings)
	{
	TUint32 accountId = aAccount.iSmtpAccountId << KShiftAccountId;

	if (IsSmtpMobilityAccount(SmtpRepositoryL(), accountId))
		{
		User::Leave(KErrNotFound);
		}

	DoLoadSmtpSettingsL(accountId, aSmtpSettings);			
	}

/**
Loads SMTP IAP settings for the specified account. 

@param	aAccount
SMTP account ID

@param	aIAP
SMTP IAP Pref settings
*/				
EXPORT_C void CEmailAccounts::LoadSmtpIapSettingsL(const TSmtpAccount& aAccount, CImIAPPreferences& aIAP)
	{
	TUint32 accountId = aAccount.iSmtpAccountId << KShiftAccountId;
	CRepository& smtpRepository = SmtpRepositoryL();

	if (IsSmtpMobilityAccount(smtpRepository, accountId))
		{
		User::Leave(KErrNotFound);
		}

	LoadIAPPrefSettingsL(accountId, aIAP, smtpRepository);			
	}

/**
Loads SMTP mobility account settings

@param aAccount SMTP mobility account
@param aSettings SMTP mobility account settings

@leave KErrNotFound Specified account is not a SMTP mobility account

@publishedPartner
*/
EXPORT_C void CEmailAccounts::LoadSmtpMobilitySettingsL(const TSmtpMobilityAccount& aAccount, CImSmtpMobilitySettings& aSettings)
	{
	TUint32 mobAccRepId = aAccount.iAccountId << KShiftAccountId;

	if (!IsSmtpMobilityAccount(SmtpRepositoryL(), mobAccRepId))
		{
		User::Leave(KErrNotFound);
		}

	CImSmtpSettings* smtpSettings = new (ELeave) CImSmtpSettings();
	CleanupStack::PushL(smtpSettings);
	DoLoadSmtpSettingsL(mobAccRepId, *smtpSettings);
	CleanupStack::Pop(smtpSettings);
	aSettings.SetSmtpSettings(smtpSettings);
	}

/**
Loads SMTP mobility account settings for a given SMTP account and IAP

@param aSmtpAccountId SMTP account ID
@param aIap IAP
@param aSettings SMTP mobility account settings

@return ETrue if account found, EFalse if not

@internalComponent
*/
EXPORT_C TBool CEmailAccounts::LoadSmtpMobilitySettingsL(TSmtpAccountId& aSmtpAccountId, TUint32 aIap, CImSmtpMobilitySettings& aSettings)
	{
	TUint32 smtpAccRepId = aSmtpAccountId << KShiftAccountId;
	CRepository& repository = SmtpRepositoryL();

	RArrayIAP iapList;
	CleanupClosePushL(iapList);
	iapList.AppendL(aIap);

	TInt numEntries = 0;
	TInt pos = KErrNotFound;

	TInt err = 	repository.Get(smtpAccRepId + ESMTPMobilityAccountListNumEntriesId, numEntries);
	if (err == KErrNone)
		{
		pos = FindIapListEntryInSmtpMobilityAccountListL(iapList, repository, smtpAccRepId, numEntries, KNullSmtpMobilityAccountEntryPos);
		}
	else if (err != KErrNotFound)
		{
		User::Leave(err);
		}

	CleanupStack::PopAndDestroy(&iapList);

	if (pos != KErrNotFound)
		{
		TInt tempInt = 0;
		User::LeaveIfError(repository.Get(smtpAccRepId + (pos * ESMTPMobilityAccountListOffset) + ESMTPMobilityAccountListAccountId, tempInt));
		TUint32 mobAccRepId = tempInt;

		CImSmtpSettings* smtpSettings = new (ELeave) CImSmtpSettings();
		CleanupStack::PushL(smtpSettings);
		DoLoadSmtpSettingsL(mobAccRepId, *smtpSettings);
		CleanupStack::Pop(smtpSettings);
		aSettings.SetSmtpSettings(smtpSettings);

		return ETrue;
		}

	return EFalse;
	}

/**
Loads the IAP list associated with a specified SMTP mobility account

@param aAccount SMTP mobility account
@param aIapList Stores the IAP list

@publishedPartner
*/
EXPORT_C void CEmailAccounts::LoadSmtpMobilityAccountIapListL(const TSmtpMobilityAccount& aAccount, RArrayIAP& aIapList)
	{
	aIapList.Reset();

	TUint32 mobAccRepId = aAccount.iAccountId << KShiftAccountId;
	CRepository& repository = SmtpRepositoryL();

	TUint32 smtpAccRepId = 0;
	TInt pos = 0;
	GetSmtpAccountInfoForMobilityAccountL(mobAccRepId, repository, smtpAccRepId, pos);

	TUint32 offset = smtpAccRepId + (pos * ESMTPMobilityAccountListOffset);

	TInt numIaps = 0;
	User::LeaveIfError(repository.Get(offset + ESMTPMobilityAccountListNumIapsId, numIaps));

	TInt iap = 0;
	for (TInt iapLoop = 0; iapLoop < numIaps; ++iapLoop)
		{
		User::LeaveIfError(repository.Get(offset + ESMTPMobilityAccountListIapId + iapLoop, iap));

		aIapList.AppendL(iap);
		}
	}

/**
Saves POP service settings for the specified account. 

@param	aAccount
POP account ID, if the account name is zero-length, the account name is not updated

@param	aPopSettings
POP service settings
*/	
EXPORT_C void CEmailAccounts::SavePopSettingsL(const TPopAccount& aAccount, const CImPop3Settings& aPopSettings)
	{
	TUint32 accountId = aAccount.iPopAccountId << KShiftAccountId;
	CRepository& popRepository = PopRepositoryL();	
	
	TInt temp = 0;
	User::LeaveIfError(popRepository.Get(accountId + EPOPServiceId, temp));
	TMsvId service = static_cast<TMsvId>(temp);		

	// Save settings to CenRep
	User::LeaveIfError(popRepository.StartTransaction(CRepository::EReadWriteTransaction));		
	popRepository.CleanupRollbackTransactionPushL();
	if (aAccount.iPopAccountName.Length() > 0)
		{
		TBuf<KEmailAccountNameSize> accountName;
		User::LeaveIfError(popRepository.Get(accountId + EPOPAccountNameId, accountName));
		if(accountName.Compare(aAccount.iPopAccountName) != 0)
			{
			// Update account name
			popRepository.Set(accountId + EPOPAccountNameId, aAccount.iPopAccountName);
			SetEntryDetailsL(service, aAccount.iPopAccountName);			
			}
		}					
	DoSavePopSettingsL(accountId, aPopSettings);
	CleanupStack::Pop();
	TUint32 errorId = 0;	
	User::LeaveIfError(popRepository.CommitTransaction(errorId));
	}

/**
Saves POP IAP settings for the specified account. 

@param	aAccount
POP account ID, if the account name is zero-length, the account name is not updated

@param	aIAP
POP IAP Pref settings
*/			
EXPORT_C void CEmailAccounts::SavePopIapSettingsL(const TPopAccount& aAccount, const CImIAPPreferences& aIAP)
	{
	TUint32 accountId = aAccount.iPopAccountId << KShiftAccountId;
	CRepository& popRepository = PopRepositoryL();	

	TInt temp = 0;
	User::LeaveIfError(popRepository.Get(accountId + EPOPServiceId, temp));
	TMsvId service = static_cast<TMsvId>(temp);		

	User::LeaveIfError(popRepository.StartTransaction(CRepository::EReadWriteTransaction));		
	popRepository.CleanupRollbackTransactionPushL();
	if (aAccount.iPopAccountName.Length() > 0)
		{
		TBuf<KEmailAccountNameSize> accountName;
		User::LeaveIfError(popRepository.Get(accountId + EPOPAccountNameId, accountName));
		if(accountName.Compare(aAccount.iPopAccountName) != 0)
			{
			// Update account name
			popRepository.Set(accountId + EPOPAccountNameId, aAccount.iPopAccountName);
			SetEntryDetailsL(service, aAccount.iPopAccountName);			
			}
		}					
	SaveIAPPrefSettingsL(accountId, aIAP, popRepository);	
	CleanupStack::Pop();
	TUint32 errorId = 0;	
	User::LeaveIfError(popRepository.CommitTransaction(errorId));
	}

/**
Saves IMAP service settings for the specified account. 

@param	aAccount
IMAP account ID, if the account name is zero-length, the account name is not updated

@param	aImapSettings
IMAP service settings
*/	
EXPORT_C void CEmailAccounts::SaveImapSettingsL(const TImapAccount& aAccount, const CImImap4Settings& aImapSettings)
	{
	TUint32 accountId = aAccount.iImapAccountId << KShiftAccountId;
	CRepository& imapRepository = ImapRepositoryL();

	TInt temp = 0;
	User::LeaveIfError(imapRepository.Get(accountId + EIMAPServiceId, temp));
	TMsvId service = static_cast<TMsvId>(temp);		

	User::LeaveIfError(imapRepository.StartTransaction(CRepository::EReadWriteTransaction));
	imapRepository.CleanupRollbackTransactionPushL();
	if (aAccount.iImapAccountName.Length() > 0)
		{
		TBuf<KEmailAccountNameSize> accountName;
		User::LeaveIfError(imapRepository.Get(accountId + EIMAPAccountNameId, accountName));
		if(accountName.Compare(aAccount.iImapAccountName) != 0)
			{
			// Update account name
			imapRepository.Set(accountId + EIMAPAccountNameId, aAccount.iImapAccountName);
			SetEntryDetailsL(service, aAccount.iImapAccountName);
			}
		}			  							
	DoSaveImapSettingsL(accountId, aImapSettings);
	CleanupStack::Pop();
	TUint32 errorId = 0;	
	User::LeaveIfError(imapRepository.CommitTransaction(errorId));
	}

/**
Saves IMAP synchronisation download rules

@param aAccount IMAP account ID
@param aRules Synchronisation download rules

@publishedPartner
*/
EXPORT_C void CEmailAccounts::SaveImapSyncDownloadRulesL(const TImapAccount& aAccount, const CImapSyncDownloadRules& aRules)
	{
	TUint32 accountId = aAccount.iImapAccountId << KShiftAccountId;
	CRepository& imapRepository = ImapRepositoryL();

	User::LeaveIfError(imapRepository.StartTransaction(CRepository::EReadWriteTransaction));
	imapRepository.CleanupRollbackTransactionPushL();

	DoSaveImapSyncDownloadRulesL(accountId, aRules, imapRepository);

	CleanupStack::Pop();
	TUint32 errorId = 0;	
	User::LeaveIfError(imapRepository.CommitTransaction(errorId));
	}

/**
Saves IMAP IAP settings for the specified account. 

@param	aAccount
IMAP account ID, if the account name is zero-length, the account name is not updated

@param	aIAP
IMAP IAP Pref settings
*/				
EXPORT_C void CEmailAccounts::SaveImapIapSettingsL(const TImapAccount& aAccount, const CImIAPPreferences& aIAP)
	{
	TUint32 accountId = aAccount.iImapAccountId << KShiftAccountId;
	CRepository& imapRepository = ImapRepositoryL();

	TInt temp = 0;
	User::LeaveIfError(imapRepository.Get(accountId + EIMAPServiceId, temp));
	TMsvId service = static_cast<TMsvId>(temp);		

	User::LeaveIfError(imapRepository.StartTransaction(CRepository::EReadWriteTransaction));
	imapRepository.CleanupRollbackTransactionPushL();
	if (aAccount.iImapAccountName.Length() > 0)
		{
		TBuf<KEmailAccountNameSize> accountName;
		User::LeaveIfError(imapRepository.Get(accountId + EIMAPAccountNameId, accountName));
		if(accountName.Compare(aAccount.iImapAccountName) != 0)
			{
			// Update account name
			imapRepository.Set(accountId + EIMAPAccountNameId, aAccount.iImapAccountName);
			SetEntryDetailsL(service, aAccount.iImapAccountName);
			}
		}										
	SaveIAPPrefSettingsL(accountId, aIAP, imapRepository);	
	CleanupStack::Pop();
	TUint32 errorId = 0;	
	User::LeaveIfError(imapRepository.CommitTransaction(errorId));		
	}

/**
Saves IMAP transport buffer sizes

@param aBufferSizes Transport buffer sizes

@publishedPartner
*/
EXPORT_C void CEmailAccounts::SaveImapTransportBufferSizesL(const CImapTransportBufferSizes& aBufferSizes)
	{
	CRepository& imapRepository = ImapRepositoryL();

	User::LeaveIfError(imapRepository.StartTransaction(CRepository::EReadWriteTransaction));
	imapRepository.CleanupRollbackTransactionPushL();

	imapRepository.Set(EIMAPTransportBufferSizesVersionId, KImapTransportBufferSizesVersion);

	TInt entryOffset;
	TUint32 bearerTypes;
	TInt fetchRequestSize;
	TInt maxOutstandingFetchResponses;
	TInt entryLoop = 0;

	while (entryLoop < KMaxImapTransportBufferSizes &&
	       aBufferSizes.GetSizesL(entryLoop, bearerTypes, fetchRequestSize, maxOutstandingFetchResponses))
		{
		entryOffset = entryLoop * EIMAPTransportBufferSizesOffset;

		imapRepository.Set(entryOffset + EIMAPTransportBufferSizesBearerTypesId, static_cast<TInt>(bearerTypes));
		imapRepository.Set(entryOffset + EIMAPTransportBufferSizesFetchRequestSizeId, fetchRequestSize);
		imapRepository.Set(entryOffset + EIMAPTransportBufferSizesMaxOutstandingFetchResponsesId, maxOutstandingFetchResponses);	

		++entryLoop;
		}

	imapRepository.Set(EIMAPNumTransportBufferSizesId, entryLoop);

	// Clear any old rules that are no longer required
	while (entryLoop < KMaxImapTransportBufferSizes)
		{
		entryOffset = entryLoop * EIMAPTransportBufferSizesOffset;

		imapRepository.Delete(entryOffset + EIMAPTransportBufferSizesBearerTypesId);
		imapRepository.Delete(entryOffset + EIMAPTransportBufferSizesFetchRequestSizeId);
		imapRepository.Delete(entryOffset + EIMAPTransportBufferSizesMaxOutstandingFetchResponsesId);		

		++entryLoop;
		}

	CleanupStack::Pop();
	TUint32 errorId = 0;	
	User::LeaveIfError(imapRepository.CommitTransaction(errorId));
	}


/**
Saves SMTP service settings for the specified account. 

@param	aAccount
SMTP account ID, if the account name is zero-length, the account name is not updated

@param	aSmtpSettings
SMTP service settings
*/	
EXPORT_C void CEmailAccounts::SaveSmtpSettingsL(const TSmtpAccount& aAccount, const CImSmtpSettings& aSmtpSettings)
	{
	TUint32 accountId = aAccount.iSmtpAccountId << KShiftAccountId;
	CRepository& smtpRepository = SmtpRepositoryL();	

	TInt temp = 0;
	User::LeaveIfError(smtpRepository.Get(accountId + ESMTPServiceId, temp));
	TMsvId service = static_cast<TMsvId>(temp);		

	if (IsSmtpMobilityAccount(smtpRepository, accountId))
		{
		User::Leave(KErrNotFound);
		}

	User::LeaveIfError(smtpRepository.StartTransaction(CRepository::EReadWriteTransaction));	
	smtpRepository.CleanupRollbackTransactionPushL();
	if (aAccount.iSmtpAccountName.Length() > 0)
		{
		TBuf<KEmailAccountNameSize> accountName;
		User::LeaveIfError(smtpRepository.Get(accountId + ESMTPAccountNameId, accountName));
		if(accountName.Compare(aAccount.iSmtpAccountName) != 0)
			{
			// Update account name
			smtpRepository.Set(accountId + ESMTPAccountNameId, aAccount.iSmtpAccountName);
			SetEntryDetailsL(service, aAccount.iSmtpAccountName);
			}
		}									
	DoSaveSmtpSettingsL(accountId, aSmtpSettings);
	CleanupStack::Pop();
	TUint32 errorId = 0;	
	User::LeaveIfError(smtpRepository.CommitTransaction(errorId));	
	}

/**
Saves SMTP IAP settings for the specified account. 

@param	aAccount
SMTP account ID, if the account name is zero-length, the account name is not updated

@param	aIAP
SMTP IAP Pref settings
*/					
EXPORT_C void CEmailAccounts::SaveSmtpIapSettingsL(const TSmtpAccount& aAccount, const CImIAPPreferences& aIAP)
	{
	TUint32 accountId = aAccount.iSmtpAccountId << KShiftAccountId;
	CRepository& smtpRepository = SmtpRepositoryL();	

	TInt temp = 0;
	User::LeaveIfError(smtpRepository.Get(accountId + ESMTPServiceId, temp));
	TMsvId service = static_cast<TMsvId>(temp);		

	if (IsSmtpMobilityAccount(smtpRepository, accountId))
		{
		User::Leave(KErrNotFound);
		}

	User::LeaveIfError(smtpRepository.StartTransaction(CRepository::EReadWriteTransaction));	
	smtpRepository.CleanupRollbackTransactionPushL();
	if (aAccount.iSmtpAccountName.Length() > 0)
		{
		TBuf<KEmailAccountNameSize> accountName;
		User::LeaveIfError(smtpRepository.Get(accountId + ESMTPAccountNameId, accountName));
		if(accountName.Compare(aAccount.iSmtpAccountName) != 0)
			{
			// Update account name
			smtpRepository.Set(accountId + ESMTPAccountNameId, aAccount.iSmtpAccountName);
			SetEntryDetailsL(service, aAccount.iSmtpAccountName);
			}
		}									
	SaveIAPPrefSettingsL(accountId, aIAP, smtpRepository);	
	CleanupStack::Pop();
	TUint32 errorId = 0;	
	User::LeaveIfError(smtpRepository.CommitTransaction(errorId));			
	}

/**
Saves the settings for a SMTP mobility account

@param aAccount SMTP mobility account
@param aSettings Settings to save

@leave KErrNotFound Specified account is not a SMTP mobility account
@leave KErrNotSupported SMTP mobility account uses settings of a linked
       SMTP account and changing the settings is not supported

@publishedPartner
*/	
EXPORT_C void CEmailAccounts::SaveSmtpMobilitySettingsL(const TSmtpMobilityAccount& aAccount, const CImSmtpMobilitySettings& aSettings)
	{
	TUint32 mobAccRepId = aAccount.iAccountId << KShiftAccountId;
	CRepository& repository = SmtpRepositoryL();	

	if (!IsSmtpMobilityAccount(repository, mobAccRepId))
		{
		User::Leave(KErrNotFound);
		}

	TInt tempInt = 0;
	User::LeaveIfError(repository.Get(mobAccRepId + ESMTPLinkedAccountId, tempInt));
	if (tempInt != KNullSmtpAccountId)
		{
		User::Leave(KErrNotSupported);
		}

	User::LeaveIfError(repository.StartTransaction(CRepository::EReadWriteTransaction));
	repository.CleanupRollbackTransactionPushL();
	if (aAccount.iName.Length() > 0)
		{
		// Update account name
		repository.Set(mobAccRepId + ESMTPAccountNameId, aAccount.iName);
		}

	DoSaveSmtpSettingsL(mobAccRepId, aSettings.SmtpSettings());

	CleanupStack::Pop();
	TUint32 keyInfo = 0;	
	User::LeaveIfError(repository.CommitTransaction(keyInfo));	
	}

/**
Sets the IAP list for a SMTP mobility account

@param aAccount SMTP mobility account
@param aIapList IAP list to save

@leave KErrArgument No entries or too many entries in the IAP list

@publishedPartner
*/
EXPORT_C void CEmailAccounts::SaveSmtpMobilityAccountIapListL(const TSmtpMobilityAccount& aAccount, const RArrayIAP& aIapList)
	{
	TInt iapListCount = aIapList.Count();

	if (iapListCount < 1 || iapListCount > KMaxSmtpMobilityAccountIAPListEntries)
		{
		User::Leave(KErrArgument);
		}

	TUint32 mobAccRepId = aAccount.iAccountId << KShiftAccountId;
	CRepository& repository = SmtpRepositoryL();

	TUint32 smtpAccRepId = 0;
	TInt pos = 0;
	GetSmtpAccountInfoForMobilityAccountL(mobAccRepId, repository, smtpAccRepId, pos);

	TInt numEntries = 0;
	User::LeaveIfError(repository.Get(smtpAccRepId + ESMTPMobilityAccountListNumEntriesId, numEntries));

	if (FindIapListEntryInSmtpMobilityAccountListL(aIapList, repository, smtpAccRepId, numEntries, pos) != KErrNotFound)
		{
		User::Leave(KErrAlreadyExists);
		}

	User::LeaveIfError(repository.StartTransaction(CRepository::EReadWriteTransaction));
	repository.CleanupRollbackTransactionPushL();

	TUint32 offset = smtpAccRepId + (pos * ESMTPMobilityAccountListOffset);

	User::LeaveIfError(repository.Set(offset + ESMTPMobilityAccountListNumIapsId, iapListCount));

	TInt iapLoop = 0;
	
	for (; iapLoop < iapListCount; ++iapLoop)
		{
		User::LeaveIfError(repository.Set(offset + ESMTPMobilityAccountListIapId + iapLoop, static_cast<TInt>(aIapList[iapLoop])));
		}

	for (; iapLoop < KMaxSmtpMobilityAccountIAPListEntries; ++iapLoop)
		{
		repository.Delete(offset + ESMTPMobilityAccountListIapId + iapLoop);
		}

	CleanupStack::Pop();
	TUint32 keyInfo = 0;
	User::LeaveIfError(repository.CommitTransaction(keyInfo));
	}

TUint CEmailAccounts::GetNextEmptyAccountSlotL(CRepository& aRepository)
	{
	TUint32 accountId = KNullId;
	TInt serviceId = 0;
	TInt error = 0;
	TBool found = EFalse;
	
	for (TInt count = 0; count < KMaxAccount; ++count)
		{		
		accountId = accountId + KIncrementAccount;
		error = aRepository.Get(accountId, serviceId);		
		if (error == KErrNotFound)
			{
			found = ETrue;				
			break;
			}
		else
			{
			User::LeaveIfError(error);
			}
		}

	if (found == EFalse)
		{
		// No empty slot available		
		User::Leave(KErrNotFound);
		}
	
	return accountId;
	}

TUint32 CEmailAccounts::FindEmailAccountL(TMsvId aService, CRepository& aRepository)
	{	
	// Get a list of accounts in CenRep that matches the service
	RArray<TUint32> accounts;
	CleanupClosePushL(accounts);	
	User::LeaveIfError(aRepository.FindEqL(KNullId, static_cast<TUint32>(EAccountMask), static_cast<TInt>(aService), accounts));

	// Get the account Id from the service entry
	CMsvEntry* msvEntry = SessionL().GetEntryL(KMsvRootIndexEntryId);
    CleanupStack::PushL(msvEntry);

	msvEntry->SetEntryL(aService);	
	TMsvEntry entry = msvEntry->Entry();
	TInt accountIdInService = entry.MtmData2();
    CleanupStack::PopAndDestroy(msvEntry);			

	// Return the account that matches with the Id in the service entry
	TUint32 cenrepId = static_cast<TUint32>(accountIdInService) << KShiftAccountId;
	TInt index = accounts.FindL(cenrepId);	
	TUint32 account = accounts[index];
	
	CleanupStack::PopAndDestroy(&accounts);
	return account;
	}
			
void CEmailAccounts::DoLoadPopSettingsL(TUint32 aAccountId, CImPop3Settings& aPopSettings)
	{
	aPopSettings.Reset();	
	CRepository& popRepository = PopRepositoryL();

	TInt temp = 0;
	TBuf<KMaxSettingStringLength> tempDesc;			
	TInt error = popRepository.Get(aAccountId + EPOPServerAddressId, tempDesc);
	if (error == KErrNone)
		{		
		aPopSettings.SetServerAddressL(tempDesc);
		}
	else if (error == KErrPermissionDenied)
		{
		aPopSettings.SetServerAddressL(KNullDesC());		
		}
	else
		{
		User::Leave(error);
		}

	error = popRepository.Get(aAccountId + EPOPPortNumberId, temp);	
	if (error == KErrNone)
		{		
		aPopSettings.SetPort(static_cast<TUint>(temp));
		}
	else if (error == KErrPermissionDenied)
		{
		aPopSettings.SetPort(0);		
		}
	else
		{
		User::Leave(error);
		}

	User::LeaveIfError(popRepository.Get(aAccountId + EPOPFlagsId, temp));	
	aPopSettings.SetSettingsFlags(static_cast<TUint32>(temp));
		
	TBuf8<KMaxSettingStringLength> tempDesc8;			
	error = popRepository.Get(aAccountId + EPOPLoginNameId, tempDesc8);	
	if (error == KErrNone)
		{		
		aPopSettings.SetLoginNameL(tempDesc8);
		}
	else if (error == KErrPermissionDenied)
		{
		aPopSettings.SetLoginNameL(KNullDesC8());		
		}
	else
		{
		User::Leave(error);
		}

	error = popRepository.Get(aAccountId + EPOPPasswordId, tempDesc8);	
	if (error == KErrNone)
		{		
		aPopSettings.SetPasswordL(tempDesc8);
		}
	else if (error == KErrPermissionDenied)
		{
		aPopSettings.SetPasswordL(KNullDesC8());		
		}
	else
		{
		User::Leave(error);
		}

	User::LeaveIfError(popRepository.Get(aAccountId + EPOPMaxEmailSizeId, temp));	
	aPopSettings.SetMaxEmailSize(temp);

	User::LeaveIfError(popRepository.Get(aAccountId + EPOPGetEmailOptionsId, temp));	
	aPopSettings.SetGetMailOptions(static_cast<TPop3GetMailOptions>(temp));
	
	User::LeaveIfError(popRepository.Get(aAccountId + EPOPInboxSynchronisationLimitId, temp));	
	aPopSettings.SetInboxSynchronisationLimit(temp);

	User::LeaveIfError(popRepository.Get(aAccountId + EPOPPopulationLimitId, temp));	
	aPopSettings.SetPopulationLimitL(temp);
	
	TInt settingsVersion;
	error = popRepository.Get(aAccountId + EPOPSettingsVersionId, settingsVersion);

	if (error == KErrNone)
		{
		if (settingsVersion > 1)
			{
			User::LeaveIfError(popRepository.Get(aAccountId + EPOPTlsSslDomainId, tempDesc8));
			aPopSettings.SetTlsSslDomainL(tempDesc8);
			}
		}
	}

void CEmailAccounts::DoLoadSmtpSettingsL(TUint32 aAccountId, CImSmtpSettings& aSmtpSettings)
	{
	aSmtpSettings.Reset();
	CRepository& smtpRepository = SmtpRepositoryL();

	TInt temp = 0;
	TBuf<KMaxSettingStringLength> tempDesc;			
	TInt error = smtpRepository.Get(aAccountId + ESMTPServerAddressId, tempDesc);	
	if (error == KErrNone)
		{		
		aSmtpSettings.SetServerAddressL(tempDesc);
		}
	else if (error == KErrPermissionDenied)
		{
		aSmtpSettings.SetServerAddressL(KNullDesC());		
		}
	else
		{
		User::Leave(error);
		}

	error = smtpRepository.Get(aAccountId + ESMTPPortNumberId, temp);	
	if (error == KErrNone)
		{		
		aSmtpSettings.SetPort(static_cast<TUint>(temp));
		}
	else if (error == KErrPermissionDenied)
		{
		aSmtpSettings.SetPort(0);		
		}
	else
		{
		User::Leave(error);
		}

	User::LeaveIfError(smtpRepository.Get(aAccountId + ESMTPFlagsId, temp));	
	aSmtpSettings.SetSettingsFlags(static_cast<TUint32>(temp));
		
	error = smtpRepository.Get(aAccountId + ESMTPEmailAliasId, tempDesc);	
	if (error == KErrNone)
		{		
		aSmtpSettings.SetEmailAliasL(tempDesc);
		}
	else if (error == KErrPermissionDenied)
		{
		aSmtpSettings.SetEmailAliasL(KNullDesC());		
		}
	else
		{
		User::Leave(error);
		}

	error = smtpRepository.Get(aAccountId + ESMTPEmailAddressId, tempDesc);	
	if (error == KErrNone)
		{		
		aSmtpSettings.SetEmailAddressL(tempDesc);
		}
	else if (error == KErrPermissionDenied)
		{
		aSmtpSettings.SetEmailAddressL(KNullDesC());		
		}
	else
		{
		User::Leave(error);
		}

	error = smtpRepository.Get(aAccountId + ESMTPReplyToAddressId, tempDesc);	
	if (error == KErrNone)
		{		
		aSmtpSettings.SetReplyToAddressL(tempDesc);
		}
	else if (error == KErrPermissionDenied)
		{
		aSmtpSettings.SetReplyToAddressL(KNullDesC());		
		}
	else
		{
		User::Leave(error);
		}

	error = smtpRepository.Get(aAccountId + ESMTPReceiptAddressId, tempDesc);	
	if (error == KErrNone)
		{		
		aSmtpSettings.SetReceiptAddressL(tempDesc);
		}
	else if (error == KErrPermissionDenied)
		{
		aSmtpSettings.SetReceiptAddressL(KNullDesC());		
		}
	else
		{
		User::Leave(error);
		}

	User::LeaveIfError(smtpRepository.Get(aAccountId + ESMTPBodyEncodingId, temp));	
	aSmtpSettings.SetBodyEncoding(static_cast<TMsgOutboxBodyEncoding>(temp));

	User::LeaveIfError(smtpRepository.Get(aAccountId + ESMTPDefaultMsgCharSetId, temp));	
	aSmtpSettings.SetDefaultMsgCharSet(TUid::Uid(temp));

	User::LeaveIfError(smtpRepository.Get(aAccountId + ESMTPSendCopyToSelfId, temp));	
	aSmtpSettings.SetSendCopyToSelf(static_cast<TImSMTPSendCopyToSelf>(temp));

	User::LeaveIfError(smtpRepository.Get(aAccountId + ESMTPSendMessageOptionId, temp));	
	aSmtpSettings.SetSendMessageOption(static_cast<TImSMTPSendMessageOption>(temp));

	TBuf8<KMaxSettingStringLength> tempDesc8;			
	error = smtpRepository.Get(aAccountId + ESMTPLoginNameId, tempDesc8);	
	if (error == KErrNone)
		{		
		aSmtpSettings.SetLoginNameL(tempDesc8);
		}
	else if (error == KErrPermissionDenied)
		{
		aSmtpSettings.SetLoginNameL(KNullDesC8());		
		}
	else
		{
		User::Leave(error);
		}

	error = smtpRepository.Get(aAccountId + ESMTPPasswordId, tempDesc8);	
	if (error == KErrNone)
		{		
		aSmtpSettings.SetPasswordL(tempDesc8);
		}
	else if (error == KErrPermissionDenied)
		{
		aSmtpSettings.SetPasswordL(KNullDesC8());		
		}
	else
		{
		User::Leave(error);
		}
	
	TInt smtpSettingsVersion;
	error = smtpRepository.Get(aAccountId + ESMTPSettingsVersionId, smtpSettingsVersion);

	if (error == KErrNone)
		{
		if (smtpSettingsVersion > 1)
			{		
			error = smtpRepository.Get(aAccountId + ESMTPIncludeToCcId, temp);	
			if (error == KErrNone)
				{		
				aSmtpSettings.SetToCcIncludeLimitL(static_cast<TUint>(temp));
				}
			else if (error == KErrPermissionDenied)
				{
				aSmtpSettings.SetToCcIncludeLimitL(0);
				}
			else
				{
				User::Leave(error);
				}
			}

		if (smtpSettingsVersion > 2)
			{
			User::LeaveIfError(smtpRepository.Get(aAccountId + ESMTPTlsSslDomainId, tempDesc8));
			aSmtpSettings.SetTlsSslDomainL(tempDesc8);
			}
		}	
	}

void CEmailAccounts::DoLoadImapSettingsL(TUint32 aAccountId, CImImap4Settings& aImapSettings)
	{
	aImapSettings.Reset();
	CRepository& imapRepository = ImapRepositoryL();

	TInt temp = 0;
	TBuf<KMaxSettingStringLength> tempDesc;			
	TInt error = imapRepository.Get(aAccountId + EIMAPServerAddressId, tempDesc);	
	if (error == KErrNone)
		{		
		aImapSettings.SetServerAddressL(tempDesc);
		}
	else if (error == KErrPermissionDenied)
		{
		aImapSettings.SetServerAddressL(KNullDesC());		
		}
	else
		{
		User::Leave(error);
		}

	error = imapRepository.Get(aAccountId + EIMAPPortNumberId, temp);	
	if (error == KErrNone)
		{		
		aImapSettings.SetPort(static_cast<TUint>(temp));
		}
	else if (error == KErrPermissionDenied)
		{
		aImapSettings.SetPort(0);		
		}
	else
		{
		User::Leave(error);
		}

	User::LeaveIfError(imapRepository.Get(aAccountId + EIMAPFlagsId, temp));	
	aImapSettings.SetSettingsFlags(static_cast<TUint32>(temp));
		
	TBuf8<KMaxSettingStringLength> tempDesc8;			
	error = imapRepository.Get(aAccountId + EIMAPLoginNameId, tempDesc8);	
	if (error == KErrNone)
		{		
		aImapSettings.SetLoginNameL(tempDesc8);
		}
	else if (error == KErrPermissionDenied)
		{
		aImapSettings.SetLoginNameL(KNullDesC8());		
		}
	else
		{
		User::Leave(error);
		}

	error = imapRepository.Get(aAccountId + EIMAPPasswordId, tempDesc8);	
	if (error == KErrNone)
		{		
		aImapSettings.SetPasswordL(tempDesc8);
		}
	else if (error == KErrPermissionDenied)
		{
		aImapSettings.SetPasswordL(KNullDesC8());		
		}
	else
		{
		User::Leave(error);
		}

	User::LeaveIfError(imapRepository.Get(aAccountId + EIMAPFolderPathId, tempDesc8));	
	aImapSettings.SetFolderPathL(tempDesc8);

	User::LeaveIfError(imapRepository.Get(aAccountId + EIMAPSearchStringId, tempDesc8));	
	aImapSettings.SetSearchStringL(tempDesc8);

	User::LeaveIfError(imapRepository.Get(aAccountId + EIMAPPathSeparatorId, temp));
	aImapSettings.SetPathSeparator(temp);	
	
	User::LeaveIfError(imapRepository.Get(aAccountId + EIMAPSynchronisationStrategyId, temp));	
	aImapSettings.SetSynchronise(static_cast<TFolderSyncType>(temp));

	User::LeaveIfError(imapRepository.Get(aAccountId + EIMAPSubscriptionStrategyId, temp));	
	aImapSettings.SetSubscribe(static_cast<TFolderSubscribeType>(temp));

	User::LeaveIfError(imapRepository.Get(aAccountId + EIMAPMaxEmailSizeId, temp));	
	aImapSettings.SetMaxEmailSize(static_cast<TUint>(temp));

	User::LeaveIfError(imapRepository.Get(aAccountId + EIMAPGetMailOptionsId, temp));	
	aImapSettings.SetGetMailOptions(static_cast<TImap4GetMailOptions>(temp));

	User::LeaveIfError(imapRepository.Get(aAccountId + EIMAPInboxSynchronisationLimitId, temp));	
	aImapSettings.SetInboxSynchronisationLimit(temp);
		
	User::LeaveIfError(imapRepository.Get(aAccountId + EIMAPMailboxSynchronisationLimitId, temp));	
	aImapSettings.SetMailboxSynchronisationLimit(temp);

	User::LeaveIfError(imapRepository.Get(aAccountId + EIMAPSyncRateId, temp));	
	aImapSettings.SetSyncRateL(temp);

	User::LeaveIfError(imapRepository.Get(aAccountId + EIMAPFetchSizeId, temp));	
	aImapSettings.SetFetchSizeL(static_cast<TUint>(temp));

	User::LeaveIfError(imapRepository.Get(aAccountId + EIMAPIdleTimeoutId, temp));	
	aImapSettings.SetImapIdleTimeoutL(temp);

	User::LeaveIfError(imapRepository.Get(aAccountId + EPartialMailOptionsId, temp));	
	aImapSettings.SetPartialMailOptionsL(static_cast<TImImap4PartialMailOptions>(temp));

	User::LeaveIfError(imapRepository.Get(aAccountId + EBodyTextSizeLimitId, temp));	
	aImapSettings.SetBodyTextSizeLimitL(temp);

	User::LeaveIfError(imapRepository.Get(aAccountId + EAttachmentSizeLimitId, temp));	
	aImapSettings.SetAttachmentSizeLimitL(temp);

	TInt settingsVersion;
	error = imapRepository.Get(aAccountId + EIMAPSettingsVersionId, settingsVersion);

	if (error == KErrNone)
		{
		if (settingsVersion > 1)
			{
			User::LeaveIfError(imapRepository.Get(aAccountId + EIMAPTlsSslDomainId, tempDesc8));
			aImapSettings.SetTlsSslDomainL(tempDesc8);
			}
		}
	}

void CEmailAccounts::LoadIAPPrefSettingsL(TUint32 aAccountId, CImIAPPreferences& aIAP, CRepository& aRepository)
	{
	// Remove existing settings.
	aIAP.Reset();

	// temp variable for reading from CenRep
	TInt temp = 0;

	// obtain the settings version
	TInt settingsVersion;
	TInt error = aRepository.Get(aAccountId + EIAPPrefVersion, settingsVersion);

	// if a SNAP has been defined, use it...
	TBool useSnap = EFalse;
	if (error == KErrNone)
		{
		if (settingsVersion > 1)
			{
			User::LeaveIfError(aRepository.Get(aAccountId + EIAPSnapId, temp));
			if (temp != 0)
				{
				aIAP.SetSNAPL(temp);
				useSnap = ETrue;
				}
			}
		}
	
	// ... otherwise read in the IAP choices.
	if (!useSnap)
		{
		TInt nChoices = 0;
		User::LeaveIfError(aRepository.Get(aAccountId + EIAPChoicesCount, nChoices));	
			
		TUint32 iapID = aAccountId + EIAPListIDs;				// 0x00011000	
		TUint32 dialogPrefs = aAccountId + EIAPListDialogPrefs;	// 0x00012000

		while (nChoices--)
			{
			// get IAP settings from CenRep
			TImIAPChoice choice;
			User::LeaveIfError(aRepository.Get(iapID, temp));	
			choice.iIAP = static_cast<TUint32>(temp);

			User::LeaveIfError(aRepository.Get(dialogPrefs, temp));	
			choice.iDialogPref = static_cast<TCommDbDialogPref>(temp);
				
			// append IAP at the end of the list
			TInt nIAPs = aIAP.NumberOfIAPs();
			aIAP.AddIAPL(choice, nIAPs);
				
			++iapID;
			++dialogPrefs;
			}
		}
	}

void CEmailAccounts::DoSavePopSettingsL(TUint32 aAccountId, const CImPop3Settings& aPopSettings)
	{
	CRepository& popRepository = PopRepositoryL();

	popRepository.Set(aAccountId + EPOPSettingsVersionId, KPOPSettingsVersion);
	popRepository.Set(aAccountId + EPOPServerAddressId, aPopSettings.ServerAddress());
	popRepository.Set(aAccountId + EPOPPortNumberId, static_cast<TInt>(aPopSettings.Port()));
	popRepository.Set(aAccountId + EPOPFlagsId, static_cast<TInt>(aPopSettings.SettingsFlags()));
	popRepository.Set(aAccountId + EPOPLoginNameId, aPopSettings.LoginName());
	popRepository.Set(aAccountId + EPOPPasswordId, aPopSettings.Password());
	popRepository.Set(aAccountId + EPOPMaxEmailSizeId, aPopSettings.MaxEmailSize());
	popRepository.Set(aAccountId + EPOPGetEmailOptionsId, static_cast<TInt>(aPopSettings.GetMailOptions()));
	popRepository.Set(aAccountId + EPOPInboxSynchronisationLimitId, static_cast<TInt>(aPopSettings.InboxSynchronisationLimit()));
	popRepository.Set(aAccountId + EPOPPopulationLimitId, static_cast<TInt>(aPopSettings.PopulationLimit()));
	popRepository.Set(aAccountId + EPOPTlsSslDomainId, aPopSettings.TlsSslDomain());
	}

void CEmailAccounts::DoSaveSmtpSettingsL(TUint32 aAccountId, const CImSmtpSettings& aSmtpSettings)
	{
	CRepository& smtpRepository = SmtpRepositoryL();

	smtpRepository.Set(aAccountId + ESMTPSettingsVersionId, KSMTPSettingsVersion);
	smtpRepository.Set(aAccountId + ESMTPServerAddressId, aSmtpSettings.ServerAddress());
	smtpRepository.Set(aAccountId + ESMTPPortNumberId, static_cast<TInt>(aSmtpSettings.Port()));
	smtpRepository.Set(aAccountId + ESMTPFlagsId, static_cast<TInt>(aSmtpSettings.SettingsFlags()));
	smtpRepository.Set(aAccountId + ESMTPEmailAliasId, aSmtpSettings.EmailAlias());
	smtpRepository.Set(aAccountId + ESMTPEmailAddressId, aSmtpSettings.EmailAddress());
	smtpRepository.Set(aAccountId + ESMTPReplyToAddressId, aSmtpSettings.ReplyToAddress());
	smtpRepository.Set(aAccountId + ESMTPReceiptAddressId, aSmtpSettings.ReceiptAddress());
	smtpRepository.Set(aAccountId + ESMTPBodyEncodingId, static_cast<TInt>(aSmtpSettings.BodyEncoding()));
	smtpRepository.Set(aAccountId + ESMTPDefaultMsgCharSetId, static_cast<TInt>(aSmtpSettings.DefaultMsgCharSet().iUid));
	smtpRepository.Set(aAccountId + ESMTPSendCopyToSelfId, static_cast<TInt>(aSmtpSettings.SendCopyToSelf()));
	smtpRepository.Set(aAccountId + ESMTPSendMessageOptionId, static_cast<TInt>(aSmtpSettings.SendMessageOption()));
	smtpRepository.Set(aAccountId + ESMTPLoginNameId, aSmtpSettings.LoginName());
	smtpRepository.Set(aAccountId + ESMTPPasswordId, aSmtpSettings.Password());
	smtpRepository.Set(aAccountId + ESMTPIncludeToCcId, aSmtpSettings.ToCcIncludeLimit());
	smtpRepository.Set(aAccountId + ESMTPTlsSslDomainId, aSmtpSettings.TlsSslDomain());
	}

void CEmailAccounts::SaveIAPPrefSettingsL(TUint32 aAccountId, const CImIAPPreferences& aIAP, CRepository& aRepository)
	{
	aRepository.Set(aAccountId + EIAPPrefVersion, KIAPPrefSettingsVersion);
	TInt nChoices = aIAP.NumberOfIAPs();	
	if (nChoices>KMaxIAPChoices)
		{
		User::Leave(KErrOverflow);
		}

	aRepository.Set(aAccountId + EIAPChoicesCount,  nChoices);
	TUint32 iapID = aAccountId + EIAPListIDs;				// 0x00011000	
	TUint32 dialogPrefs = aAccountId + EIAPListDialogPrefs;	// 0x00012000
	for (TInt i=0; i<nChoices; ++i)
		{
		aRepository.Set(iapID, static_cast<TInt>(aIAP.IAPPreference(i).iIAP));		
		aRepository.Set(dialogPrefs, static_cast<TInt>(aIAP.IAPPreference(i).iDialogPref));		
									
		++iapID;
		++dialogPrefs;
		}
	aRepository.Set(aAccountId + EIAPSnapId, static_cast<TInt>(aIAP.SNAPPreference()));
	}

void CEmailAccounts::DoSaveImapSettingsL(TUint32 aAccountId, const CImImap4Settings& aImapSettings)
	{
	CRepository& imapRepository = ImapRepositoryL();

	imapRepository.Set(aAccountId + EIMAPSettingsVersionId, KIMAPSettingsVersion);
	imapRepository.Set(aAccountId + EIMAPServerAddressId, aImapSettings.ServerAddress());
	imapRepository.Set(aAccountId + EIMAPPortNumberId, static_cast<TInt>(aImapSettings.Port()));
	imapRepository.Set(aAccountId + EIMAPFlagsId, static_cast<TInt>(aImapSettings.SettingsFlags()));
	imapRepository.Set(aAccountId + EIMAPLoginNameId, aImapSettings.LoginName());
	imapRepository.Set(aAccountId + EIMAPPasswordId, aImapSettings.Password());
	imapRepository.Set(aAccountId + EIMAPFolderPathId, aImapSettings.FolderPath());
	imapRepository.Set(aAccountId + EIMAPPathSeparatorId, static_cast<TInt>(aImapSettings.PathSeparator()));
	imapRepository.Set(aAccountId + EIMAPSynchronisationStrategyId, static_cast<TInt>(aImapSettings.Synchronise()));
	imapRepository.Set(aAccountId + EIMAPSubscriptionStrategyId, static_cast<TInt>(aImapSettings.Subscribe()));
	imapRepository.Set(aAccountId + EIMAPMaxEmailSizeId, static_cast<TInt>(aImapSettings.MaxEmailSize()));
	imapRepository.Set(aAccountId + EIMAPGetMailOptionsId, static_cast<TInt>(aImapSettings.GetMailOptions()));
	imapRepository.Set(aAccountId + EIMAPInboxSynchronisationLimitId, static_cast<TInt>(aImapSettings.InboxSynchronisationLimit()));
	imapRepository.Set(aAccountId + EIMAPMailboxSynchronisationLimitId, static_cast<TInt>(aImapSettings.MailboxSynchronisationLimit()));
	imapRepository.Set(aAccountId + EIMAPTlsSslDomainId, aImapSettings.TlsSslDomain());
	imapRepository.Set(aAccountId + EIMAPSyncRateId, aImapSettings.SyncRate());
	imapRepository.Set(aAccountId + EIMAPFetchSizeId, static_cast<TInt>(aImapSettings.FetchSize()));
	imapRepository.Set(aAccountId + EIMAPIdleTimeoutId, aImapSettings.ImapIdleTimeout());
	imapRepository.Set(aAccountId + EPartialMailOptionsId, static_cast<TInt>(aImapSettings.PartialMailOptions()));
	imapRepository.Set(aAccountId + EBodyTextSizeLimitId, static_cast<TInt>(aImapSettings.BodyTextSizeLimit()));
	imapRepository.Set(aAccountId + EAttachmentSizeLimitId, static_cast<TInt>(aImapSettings.AttachmentSizeLimit()));
	imapRepository.Set(aAccountId + EIMAPSearchStringId, aImapSettings.SearchString());
	}

/**
Saves the IMAP sync download rules

@param aAccountId IMAP account ID
@param aRules Rules to save
@param aRepository IMAP repository
*/
void CEmailAccounts::DoSaveImapSyncDownloadRulesL(TUint32 aAccountId, const CImapSyncDownloadRules& aRules, CRepository& aRepository)
	{
	aRepository.Set(aAccountId + EIMAPSyncDownloadRulesVersionId, KImapSyncDownloadRulesVersion);

	TInt ruleOffset;
	TUint32 bearerTypes;
	TImImap4GetPartialMailInfo mailInfo;
	TInt rule = 0;

	while (rule < KMaxImapSyncDownloadRules &&
	       aRules.GetRuleL(CImapSyncDownloadRules::EInboxRulesType, rule, bearerTypes, mailInfo))
		{
		ruleOffset = aAccountId + (rule * EIMAPSyncDownloadRulesOffset);

		aRepository.Set(ruleOffset + EIMAPInboxSyncDownloadRulesBearerTypesId, static_cast<TInt>(bearerTypes));
		aRepository.Set(ruleOffset + EIMAPInboxSyncDownloadRulesMailOptionsId, mailInfo.iPartialMailOptions);
		aRepository.Set(ruleOffset + EIMAPInboxSyncDownloadRulesTotalSizeLimitId, static_cast<TInt>(mailInfo.iTotalSizeLimit));
		aRepository.Set(ruleOffset + EIMAPInboxSyncDownloadRulesBodyTextSizeLimitId, static_cast<TInt>(mailInfo.iBodyTextSizeLimit));
		aRepository.Set(ruleOffset + EIMAPInboxSyncDownloadRulesAttachmentSizeLimitId, static_cast<TInt>(mailInfo.iAttachmentSizeLimit));
		aRepository.Set(ruleOffset + EIMAPInboxSyncDownloadRulesGetMailBodyPartsId, mailInfo.iGetMailBodyParts);
		aRepository.Set(ruleOffset + EIMAPInboxSyncDownloadRulesMaxEmailSizeId, static_cast<TInt>(mailInfo.iMaxEmailSize));

		++rule;
		}

	aRepository.Set(aAccountId + EIMAPNumInboxSyncDownloadRulesId, rule);

	// Delete any old rules that are no longer required
	while (rule < KMaxImapSyncDownloadRules)
		{
		ruleOffset = aAccountId + (rule * EIMAPSyncDownloadRulesOffset);

		aRepository.Delete(ruleOffset + EIMAPInboxSyncDownloadRulesBearerTypesId);
		aRepository.Delete(ruleOffset + EIMAPInboxSyncDownloadRulesMailOptionsId);
		aRepository.Delete(ruleOffset + EIMAPInboxSyncDownloadRulesTotalSizeLimitId);
		aRepository.Delete(ruleOffset + EIMAPInboxSyncDownloadRulesBodyTextSizeLimitId);
		aRepository.Delete(ruleOffset + EIMAPInboxSyncDownloadRulesAttachmentSizeLimitId);
		aRepository.Delete(ruleOffset + EIMAPInboxSyncDownloadRulesGetMailBodyPartsId);
		aRepository.Delete(ruleOffset + EIMAPInboxSyncDownloadRulesMaxEmailSizeId);

		++rule;
		}

	rule = 0;

	while (rule < KMaxImapSyncDownloadRules &&
	       aRules.GetRuleL(CImapSyncDownloadRules::EFolderRulesType, rule, bearerTypes, mailInfo))
		{
		ruleOffset = aAccountId + (rule * EIMAPSyncDownloadRulesOffset);

		aRepository.Set(ruleOffset + EIMAPFolderSyncDownloadRulesBearerTypesId, static_cast<TInt>(bearerTypes));
		aRepository.Set(ruleOffset + EIMAPFolderSyncDownloadRulesMailOptionsId, mailInfo.iPartialMailOptions);
		aRepository.Set(ruleOffset + EIMAPFolderSyncDownloadRulesTotalSizeLimitId, static_cast<TInt>(mailInfo.iTotalSizeLimit));
		aRepository.Set(ruleOffset + EIMAPFolderSyncDownloadRulesBodyTextSizeLimitId, static_cast<TInt>(mailInfo.iBodyTextSizeLimit));
		aRepository.Set(ruleOffset + EIMAPFolderSyncDownloadRulesAttachmentSizeLimitId, static_cast<TInt>(mailInfo.iAttachmentSizeLimit));
		aRepository.Set(ruleOffset + EIMAPFolderSyncDownloadRulesGetMailBodyPartsId, mailInfo.iGetMailBodyParts);
		aRepository.Set(ruleOffset + EIMAPFolderSyncDownloadRulesMaxEmailSizeId, static_cast<TInt>(mailInfo.iMaxEmailSize));

		++rule;
		}

	aRepository.Set(aAccountId + EIMAPNumFolderSyncDownloadRulesId, rule);

	// Delete any old rules that are no longer required
	while (rule < KMaxImapSyncDownloadRules)
		{
		ruleOffset = aAccountId + (rule * EIMAPSyncDownloadRulesOffset);

		aRepository.Delete(ruleOffset + EIMAPFolderSyncDownloadRulesBearerTypesId);
		aRepository.Delete(ruleOffset + EIMAPFolderSyncDownloadRulesMailOptionsId);
		aRepository.Delete(ruleOffset + EIMAPFolderSyncDownloadRulesTotalSizeLimitId);
		aRepository.Delete(ruleOffset + EIMAPFolderSyncDownloadRulesBodyTextSizeLimitId);
		aRepository.Delete(ruleOffset + EIMAPFolderSyncDownloadRulesAttachmentSizeLimitId);
		aRepository.Delete(ruleOffset + EIMAPFolderSyncDownloadRulesGetMailBodyPartsId);
		aRepository.Delete(ruleOffset + EIMAPFolderSyncDownloadRulesMaxEmailSizeId);

		++rule;
		}
	}

void CEmailAccounts::DeletePopSettings(TUint32 aAccountId, CRepository& aRepository)
	{
	aRepository.Delete(aAccountId + EPOPServiceId);	
	aRepository.Delete(aAccountId + EPOPAccountNameId);		
	aRepository.Delete(aAccountId + EPOPRelatedSMTPServiceId);	
	aRepository.Delete(aAccountId + EPOPSettingsVersionId);	
	aRepository.Delete(aAccountId + EPOPServerAddressId);	
	aRepository.Delete(aAccountId + EPOPPortNumberId);	
	aRepository.Delete(aAccountId + EPOPFlagsId);	
	aRepository.Delete(aAccountId + EPOPLoginNameId);	
	aRepository.Delete(aAccountId + EPOPPasswordId);	
	aRepository.Delete(aAccountId + EPOPMaxEmailSizeId);	
	aRepository.Delete(aAccountId + EPOPGetEmailOptionsId);	
	aRepository.Delete(aAccountId + EPOPInboxSynchronisationLimitId);	
	aRepository.Delete(aAccountId + EPOPPopulationLimitId);	
	aRepository.Delete(aAccountId + EPOPReadOnlyAccountId);
	aRepository.Delete(aAccountId + EPOPTlsSslDomainId);
	}

void CEmailAccounts::DeleteIAPPrefSettingsL(TUint32 aAccountId, CRepository& aRepository)
	{
	aRepository.Delete(aAccountId + EIAPPrefVersion);	
	
	// Read in the number of choices.
	TInt nChoices = 0;
	User::LeaveIfError(aRepository.Get(aAccountId + EIAPChoicesCount, nChoices));	
	aRepository.Delete(aAccountId + EIAPChoicesCount);	
		
	TUint32 iapID = aAccountId + EIAPListIDs;				// 0x00011000	
	TUint32 dialogPrefs = aAccountId + EIAPListDialogPrefs;	// 0x00012000
	while (nChoices--)
		{
		// Delete IAP settings in CenRep
		aRepository.Delete(iapID);	
		aRepository.Delete(dialogPrefs);	
		
		++iapID;				
		++dialogPrefs;
		}
	}

void CEmailAccounts::DeleteSmtpSettings(TUint32 aAccountId, CRepository& aRepository)
	{
	aRepository.Delete(aAccountId + ESMTPServiceId);	
	aRepository.Delete(aAccountId + ESMTPAccountNameId);	
	aRepository.Delete(aAccountId + ESMTPSettingsVersionId);	
	aRepository.Delete(aAccountId + ESMTPServerAddressId);	
	aRepository.Delete(aAccountId + ESMTPPortNumberId);	
	aRepository.Delete(aAccountId + ESMTPFlagsId);	
	aRepository.Delete(aAccountId + ESMTPEmailAliasId);	
	aRepository.Delete(aAccountId + ESMTPEmailAddressId);	
	aRepository.Delete(aAccountId + ESMTPReplyToAddressId);	
	aRepository.Delete(aAccountId + ESMTPReceiptAddressId);	
	aRepository.Delete(aAccountId + ESMTPBodyEncodingId);	
	aRepository.Delete(aAccountId + ESMTPDefaultMsgCharSetId);	
	aRepository.Delete(aAccountId + ESMTPSendCopyToSelfId);	
	aRepository.Delete(aAccountId + ESMTPSendMessageOptionId);	
	aRepository.Delete(aAccountId + ESMTPLoginNameId);	
	aRepository.Delete(aAccountId + ESMTPPasswordId);
	aRepository.Delete(aAccountId + ESMTPRelatedServiceId);	
	aRepository.Delete(aAccountId + ESMTPIncludeToCcId);
	aRepository.Delete(aAccountId + ESMTPReadOnlyAccountId);
	aRepository.Delete(aAccountId + ESMTPParentAccountId);
	aRepository.Delete(aAccountId + ESMTPTlsSslDomainId);
	aRepository.Delete(aAccountId + ESMTPLinkedAccountId);
	}

void CEmailAccounts::DeleteImapSettings(TUint32 aAccountId, CRepository& aRepository)
	{
	aRepository.Delete(aAccountId + EIMAPServiceId);	
	aRepository.Delete(aAccountId + EIMAPAccountNameId);	
	aRepository.Delete(aAccountId + EIMAPRelatedSMTPServiceId);	
	aRepository.Delete(aAccountId + EIMAPSettingsVersionId);	
	aRepository.Delete(aAccountId + EIMAPServerAddressId);	
	aRepository.Delete(aAccountId + EIMAPPortNumberId);	
	aRepository.Delete(aAccountId + EIMAPFlagsId);	
	aRepository.Delete(aAccountId + EIMAPLoginNameId);	
	aRepository.Delete(aAccountId + EIMAPPasswordId);	
	aRepository.Delete(aAccountId + EIMAPFolderPathId);	
	aRepository.Delete(aAccountId + EIMAPPathSeparatorId);	
	aRepository.Delete(aAccountId + EIMAPSynchronisationStrategyId);	
	aRepository.Delete(aAccountId + EIMAPSubscriptionStrategyId);	
	aRepository.Delete(aAccountId + EIMAPMaxEmailSizeId);	
	aRepository.Delete(aAccountId + EIMAPGetMailOptionsId);	
	aRepository.Delete(aAccountId + EIMAPInboxSynchronisationLimitId);	
	aRepository.Delete(aAccountId + EIMAPMailboxSynchronisationLimitId);	
	aRepository.Delete(aAccountId + EIMAPTlsSslDomainId);
	aRepository.Delete(aAccountId + EIMAPSyncRateId);
	aRepository.Delete(aAccountId + EIMAPFetchSizeId);	
	aRepository.Delete(aAccountId + EIMAPIdleTimeoutId);	
	aRepository.Delete(aAccountId + EPartialMailOptionsId);	
	aRepository.Delete(aAccountId + EBodyTextSizeLimitId);	
	aRepository.Delete(aAccountId + EAttachmentSizeLimitId);	
	aRepository.Delete(aAccountId + EIMAPSearchStringId);	
	aRepository.Delete(aAccountId + EIMAPReadOnlyAccountId);	
	}

/**
Deletes IMAP synchronisation download rules

@param aAccountId IMAP account ID
@param aRepository IMAP repository
*/
void CEmailAccounts::DeleteImapSyncDownloadRulesL(TUint32 aAccountId, CRepository& aRepository)
	{
	if (aRepository.Delete(aAccountId + EIMAPSyncDownloadRulesVersionId) == KErrNone)
		{
		TInt numRules;
		User::LeaveIfError(aRepository.Get(aAccountId + EIMAPNumInboxSyncDownloadRulesId, numRules));

		aRepository.Delete(aAccountId + EIMAPNumInboxSyncDownloadRulesId);

		TUint32 ruleOffset;
		for (TInt rule = 0; rule < numRules; ++rule)
			{
			ruleOffset = aAccountId + (rule * EIMAPSyncDownloadRulesOffset);
			aRepository.Delete(ruleOffset + EIMAPInboxSyncDownloadRulesBearerTypesId);
			aRepository.Delete(ruleOffset + EIMAPInboxSyncDownloadRulesMailOptionsId);
			aRepository.Delete(ruleOffset + EIMAPInboxSyncDownloadRulesTotalSizeLimitId);
			aRepository.Delete(ruleOffset + EIMAPInboxSyncDownloadRulesBodyTextSizeLimitId);
			aRepository.Delete(ruleOffset + EIMAPInboxSyncDownloadRulesAttachmentSizeLimitId);
			aRepository.Delete(ruleOffset + EIMAPInboxSyncDownloadRulesGetMailBodyPartsId);
			aRepository.Delete(ruleOffset + EIMAPInboxSyncDownloadRulesMaxEmailSizeId);
			}

		User::LeaveIfError(aRepository.Get(aAccountId + EIMAPNumFolderSyncDownloadRulesId, numRules));

		aRepository.Delete(aAccountId + EIMAPNumFolderSyncDownloadRulesId);

		for (TInt rule = 0; rule < numRules; ++rule)
			{
			ruleOffset = aAccountId + (rule * EIMAPSyncDownloadRulesOffset);
			aRepository.Delete(ruleOffset + EIMAPFolderSyncDownloadRulesBearerTypesId);
			aRepository.Delete(ruleOffset + EIMAPFolderSyncDownloadRulesMailOptionsId);
			aRepository.Delete(ruleOffset + EIMAPFolderSyncDownloadRulesTotalSizeLimitId);
			aRepository.Delete(ruleOffset + EIMAPFolderSyncDownloadRulesBodyTextSizeLimitId);
			aRepository.Delete(ruleOffset + EIMAPFolderSyncDownloadRulesAttachmentSizeLimitId);
			aRepository.Delete(ruleOffset + EIMAPFolderSyncDownloadRulesGetMailBodyPartsId);
			aRepository.Delete(ruleOffset + EIMAPFolderSyncDownloadRulesMaxEmailSizeId);
			}
		}
	}

/**
Deletes all the IMAP transport buffer sizes

@publishedPartner
*/
EXPORT_C void CEmailAccounts::DeleteImapTransportBufferSizesL()
	{
	CRepository& imapRepository = ImapRepositoryL();

	if (imapRepository.Delete(EIMAPTransportBufferSizesVersionId) == KErrNone)
		{
		TInt numEntries;
		User::LeaveIfError(imapRepository.Get(EIMAPNumTransportBufferSizesId, numEntries));

		imapRepository.Delete(EIMAPNumTransportBufferSizesId);

		TUint32 entryOffset;
		for (TInt entry = 0; entry < numEntries; ++entry)
			{
			entryOffset = entry * EIMAPTransportBufferSizesOffset;
			imapRepository.Delete(entryOffset + EIMAPTransportBufferSizesBearerTypesId);
			imapRepository.Delete(entryOffset + EIMAPTransportBufferSizesFetchRequestSizeId);
			imapRepository.Delete(entryOffset + EIMAPTransportBufferSizesMaxOutstandingFetchResponsesId);
			}
		}
	}

CMsvSession& CEmailAccounts::SessionL()
	{
	if (iMsvSession == NULL)
		{
		iMsvSession = CMsvSession::OpenSyncL(*this);		
		}
		
	return *iMsvSession;	
	}

CRepository& CEmailAccounts::PopRepositoryL()
	{
	if (iPOPRepository == NULL)
		{
		iPOPRepository = CRepository::NewL(KUidMsgTypePOP3);
		}
		
	return *iPOPRepository;	
	}

CRepository& CEmailAccounts::ImapRepositoryL()
	{
	if (iIMAPRepository == NULL)
		{
		iIMAPRepository = CRepository::NewL(KUidMsgTypeIMAP4);
		}
		
	return *iIMAPRepository;	
	}

CRepository& CEmailAccounts::SmtpRepositoryL()
	{
	if (iSMTPRepository == NULL)
		{
		iSMTPRepository = CRepository::NewL(KUidMsgTypeSMTP);
		}
		
	return *iSMTPRepository;	
	}

void CEmailAccounts::HandleSessionEventL(TMsvSessionEvent aEvent, TAny* , TAny* , TAny* ) 
	{
  	if (aEvent == EMsvCloseSession)
    	{
		delete iMsvSession;
		iMsvSession = NULL;
    	}    	
   }

/**
Checks if the account is read-only

@param	aAccount
POP account Id

@return
ETrue if it is a read-only account
Note: Read-only accounts should not be modified, however we don't enforce it.
*/	
EXPORT_C TBool CEmailAccounts::IsAccountReadOnlyL(const TPopAccount& aAccount)
	{
	TUint32 accountId = aAccount.iPopAccountId << KShiftAccountId;	

	TInt temp = 0;
	User::LeaveIfError(PopRepositoryL().Get(accountId + EPOPReadOnlyAccountId, temp));	
	TBool isReadOnly = static_cast<TBool>(temp);
	return isReadOnly;
	}

/**
Checks if the account is read-only

@param	aAccount
IMAP account Id

@return
ETrue if it is a read-only account
Note: Read-only accounts should not be modified, however we don't enforce it.
*/	
EXPORT_C TBool CEmailAccounts::IsAccountReadOnlyL(const TImapAccount& aAccount)
	{
	TUint32 accountId = aAccount.iImapAccountId << KShiftAccountId;	

	TInt temp = 0;
	User::LeaveIfError(ImapRepositoryL().Get(accountId + EIMAPReadOnlyAccountId, temp));	
	TBool isReadOnly = static_cast<TBool>(temp);
	return isReadOnly;	
	}

/**
Checks if the account is read-only

@param	aAccount
SMTP account Id

@return
ETrue if it is a read-only account
Note: Read-only accounts should not be modified, however we don't enforce it.
*/	
EXPORT_C TBool CEmailAccounts::IsAccountReadOnlyL(const TSmtpAccount& aAccount)
	{
	TUint32 smtpAccRepId = aAccount.iSmtpAccountId << KShiftAccountId;

	return IsSmtpTypeAccountReadOnlyL(smtpAccRepId);
	}

/**
Checks if SMTP mobility account is read only

@param aAccount SMTP mobility account

@publishedPartner
*/
EXPORT_C TBool CEmailAccounts::IsAccountReadOnlyL(const TSmtpMobilityAccount& aAccount)
	{
	TUint32 mobAccRepId = aAccount.iAccountId << KShiftAccountId;

	return IsSmtpTypeAccountReadOnlyL(mobAccRepId);
	}

/**
Creates a SMTP mobility account

@param aAccountName Account name
@param aIapList List of IAPs that will use the account settings
@param aSettings Settings to use for the account
@param aSmtpAccount The SMTP account to add the mobility account to
@param aLinkedSmtpAccountId Linked SMTP account ID
@param aReadOnly Flag to indicate if account is read only. Note that while read
only accounts should not be modified, we do not enforce this.

@leave KErrArgument Account name too long

@return SMTP mobility account
*/
TSmtpMobilityAccount CEmailAccounts::DoCreateSmtpMobilityAccountL(const TDesC& aAccountName, const RArrayIAP& aIapList, const CImSmtpMobilitySettings& aSettings, const TSmtpAccount& aSmtpAccount, TSmtpAccountId aLinkedSmtpAccountId, TBool aReadOnly)
	{
	if (aAccountName.Length() > KEmailAccountNameSize)
		{
		User::Leave(KErrArgument);
		}

	CRepository& repository = SmtpRepositoryL();
	TUint32 mobAccRepId = GetNextEmptyAccountSlotL(repository);

	User::LeaveIfError(repository.StartTransaction(CRepository::EReadWriteTransaction));
	repository.CleanupRollbackTransactionPushL();

	repository.Set(mobAccRepId + ESMTPServiceId, static_cast<TInt>(aSmtpAccount.iSmtpService));
	repository.Set(mobAccRepId + ESMTPAccountNameId, aAccountName);
	repository.Set(mobAccRepId + ESMTPRelatedServiceId, 0);
	repository.Set(mobAccRepId + ESMTPReadOnlyAccountId, static_cast<TInt>(aReadOnly));
	repository.Set(mobAccRepId + ESMTPParentAccountId, static_cast<TInt>(aSmtpAccount.iSmtpAccountId));
	repository.Set(mobAccRepId + ESMTPLinkedAccountId, static_cast<TInt>(aLinkedSmtpAccountId));

	DoSaveSmtpSettingsL(mobAccRepId, aSettings.SmtpSettings());

	AddMobilityAccountToSmtpAccountL(aSmtpAccount, aIapList, mobAccRepId, repository);

	if (aLinkedSmtpAccountId != KNullSmtpAccountId)
		{
		AddLinkToMobilityAccountL(aLinkedSmtpAccountId, mobAccRepId, repository);
		}

	CleanupStack::Pop();
	TUint32 keyInfo = 0;
	User::LeaveIfError(repository.CommitTransaction(keyInfo));

	TSmtpMobilityAccount account;
	account.iAccountId = mobAccRepId >> KShiftAccountId;
	account.iName = aAccountName;
	account.iParentSmtpAccountId = aSmtpAccount.iSmtpAccountId;
	account.iLinkedSmtpAccountId = aLinkedSmtpAccountId;

	return account;
	}

/**
Adds a SMTP mobility account to a SMTP account

@param aSmtpAccount SMTP account
@param aIapList IAP list associated with SMTP mobility account
@param aMobAccRepId SMTP mobility account repository ID
@param aRepository SMTP repository
*/
void CEmailAccounts::AddMobilityAccountToSmtpAccountL(const TSmtpAccount& aSmtpAccount, const RArrayIAP& aIapList, TUint32 aMobAccRepId, CRepository& aRepository)
	{
	TInt iapListCount = aIapList.Count();

	if (iapListCount < 1 || iapListCount > KMaxSmtpMobilityAccountIAPListEntries)
		{
		User::Leave(KErrArgument);
		}

	TUint32 smtpAccRepId = aSmtpAccount.iSmtpAccountId << KShiftAccountId;

	// Check the SMTP account actually exists
	TInt tempInt = 0;
	User::LeaveIfError(aRepository.Get(smtpAccRepId + ESMTPServiceId, tempInt));

	TInt numEntries = 0;
	TInt err = aRepository.Get(smtpAccRepId + ESMTPMobilityAccountListNumEntriesId, numEntries);
	if (err == KErrNone)
		{
		if (numEntries >= ESMTPMobilityAccountMaxEntries)
			{
			User::Leave(KErrOverflow);
			}

		// Check that none of the specified IAPs already exist for another entry
		if (FindIapListEntryInSmtpMobilityAccountListL(aIapList, aRepository, smtpAccRepId, numEntries, KNullSmtpMobilityAccountEntryPos) != KErrNotFound)
			{
			User::Leave(KErrAlreadyExists);
			}
		}
	else if (err != KErrNotFound)
		{
		User::Leave(err);
		}

	User::LeaveIfError(aRepository.Set(smtpAccRepId + ESMTPMobilityAccountListVersionId, KSmtpMobilityAccountsListVersion));

	User::LeaveIfError(aRepository.Set(smtpAccRepId + ESMTPMobilityAccountListNumEntriesId, numEntries + 1));

	TUint32 offset = smtpAccRepId + (numEntries * ESMTPMobilityAccountListOffset);

	User::LeaveIfError(aRepository.Set(offset + ESMTPMobilityAccountListAccountId, static_cast<TInt>(aMobAccRepId)));
	User::LeaveIfError(aRepository.Set(offset + ESMTPMobilityAccountListNumIapsId, iapListCount));

	for (TInt iapLoop = 0; iapLoop < iapListCount; ++iapLoop)
		{
		User::LeaveIfError(aRepository.Set(offset + ESMTPMobilityAccountListIapId + iapLoop, static_cast<TInt>(aIapList[iapLoop])));
		}
	}

/**
Add a link from a SMTP account to a linked SMTP mobility account

@param aLinkedSmtpAccountId

@leave KErrOverflow Maximum number of linked accounts already provisioned
*/
void CEmailAccounts::AddLinkToMobilityAccountL(TSmtpAccountId aLinkedSmtpAccountId, TUint32 aMobAccRepId, CRepository& aRepository)
	{
	TUint32 smtpAccRepId = aLinkedSmtpAccountId << KShiftAccountId;
	TInt numEntries = 0;
	TInt err = aRepository.Get(smtpAccRepId + ESMTPLinkedMobilityAccountListNumEntriesId, numEntries);
	if (err == KErrNone || err == KErrNotFound)
		{
		if (numEntries < ESMTPLinkedMobilityAccountListMaxEntries)
			{
			User::LeaveIfError(aRepository.Set(smtpAccRepId + ESMTPLinkedMobilityAccountListVersionId, KSmtpLinkedMobilityAccountsListVersion));
			User::LeaveIfError(aRepository.Set(smtpAccRepId + ESMTPLinkedMobilityAccountListNumEntriesId, numEntries + 1));
			User::LeaveIfError(aRepository.Set(smtpAccRepId + ESMTPLinkedMobilityAccountListEntryId + numEntries, static_cast<TInt>(aMobAccRepId)));
			}
		else
			{
			User::Leave(KErrOverflow);
			}
		}
	else
		{
		User::Leave(err);
		}
	}

/**
Finds an entry in a SMTP account's mobility account list which has an IAP
that matches one of the passed IAPs

@param aIapList List of IAPs to try to match
@param aRepository SMTP repository
@param aSmtpAccRepId SMTP account repository ID
@param aNumEntries Number of entries in mobility account list
@param aIgnorePos Position of entry to ignore during the search.

@return Position of entry in SMTP mobility account list with matching IAP or
        KErrNotFound
*/
TInt CEmailAccounts::FindIapListEntryInSmtpMobilityAccountListL(const RArrayIAP& aIapList, CRepository& aRepository, TUint32 aSmtpAccRepId, TInt aNumEntries, TInt aIgnorePos)
	{
	TUint32 offset = 0;
	TInt numRepositoryIaps = 0;
	TInt repositoryIap = 0;

	for (TInt entryLoop = 0; entryLoop < aNumEntries; ++entryLoop)
		{
		// If this is the entry that we have been told to ignore, skip over it
		if (entryLoop == aIgnorePos)
			{
			continue;
			}

		offset = aSmtpAccRepId + (entryLoop * ESMTPMobilityAccountListOffset);

		User::LeaveIfError(aRepository.Get(offset + ESMTPMobilityAccountListNumIapsId, numRepositoryIaps));

		for (TInt repositoryIapLoop = 0; repositoryIapLoop < numRepositoryIaps; ++repositoryIapLoop)
			{
			User::LeaveIfError(aRepository.Get(offset + ESMTPMobilityAccountListIapId + repositoryIapLoop, repositoryIap));

			for (TInt iapListLoop = 0; iapListLoop < aIapList.Count(); ++iapListLoop)
				{
				if (aIapList[iapListLoop] == repositoryIap)
					{
					return entryLoop;
					}
				}
			}
		}
	
	return KErrNotFound;
	}

/**
Finds a SMTP mobility account ID in the SMTP mobility account list for a
SMTP account

@param aMobAccRepId SMTP mobility account repository ID
@param aRepository SMTP repository
@param aSmtpAccRepId SMTP account repository ID
@param aNumEntries Number of entries in SMTP mobility account list

@return Position of entry in list or KErrNotFound
*/
TInt CEmailAccounts::FindAccountIdInSmtpMobilityAccountListL(TUint32 aMobAccRepId, CRepository& aRepository, TUint32 aSmtpAccRepId, TInt aNumEntries)
	{
	TUint32 offset = 0;
	TInt tempInt = 0;

	for (TInt entryLoop = 0; entryLoop < aNumEntries; ++entryLoop)
		{
		offset = aSmtpAccRepId + (entryLoop * ESMTPMobilityAccountListOffset);

		User::LeaveIfError(aRepository.Get(offset + ESMTPMobilityAccountListAccountId, tempInt));

		if (tempInt == aMobAccRepId)
			{
			return entryLoop;
			}
		}

	return KErrNotFound;
	}

/**
Gets SMTP Account information for a SMTP mobility account

@param aMobAccRepId SMTP mobility account repository ID
@param aRepository SMTP repository
@param aSmtpAccRepId SMTP account repository ID
@param aPos On return holds the position of the SMTP mobility account in the
       SMTP account's mobility account list.

@leave KErrNotFound SMTP mobility account not found in list
*/
void CEmailAccounts::GetSmtpAccountInfoForMobilityAccountL(TUint32 aMobAccRepId, CRepository& aRepository, TUint32& aSmtpAccRepId, TInt& aPos)
	{
	if (!IsSmtpMobilityAccount(aRepository, aMobAccRepId))
		{
		User::Leave(KErrNotFound);
		}

	TInt tempInt = 0;
	User::LeaveIfError(aRepository.Get(aMobAccRepId + ESMTPParentAccountId, tempInt));
	aSmtpAccRepId = static_cast<TUint32>(tempInt) << KShiftAccountId;

	TInt numEntries = 0;
	User::LeaveIfError(aRepository.Get(aSmtpAccRepId + ESMTPMobilityAccountListNumEntriesId, numEntries));

	aPos = FindAccountIdInSmtpMobilityAccountListL(aMobAccRepId, aRepository, aSmtpAccRepId, numEntries);
	if (aPos == KErrNotFound)
		{
		User::Leave(KErrNotFound);
		}
	}

/**
Deletes all the SMTP mobility accounts that are owned by a given SMTP account

@param aSmtpAccRepId SMTP account repository ID
@param aRepository SMTP repository
*/
void CEmailAccounts::DeleteAllOwnedMobilityAccountsL(TUint32 aSmtpAccRepId, CRepository& aRepository)
	{
	aRepository.Delete(aSmtpAccRepId + ESMTPMobilityAccountListVersionId);

	TInt numEntries = 0;
	TInt err = aRepository.Get(aSmtpAccRepId + ESMTPMobilityAccountListNumEntriesId, numEntries);
	if (err == KErrNone)
		{
		aRepository.Delete(aSmtpAccRepId + ESMTPMobilityAccountListNumEntriesId);

		TUint32 offset = 0;
		TInt tempInt = 0;
		TInt numIaps = 0;

		for (TInt entryLoop = 0; entryLoop < numEntries; ++entryLoop)
			{
			offset = aSmtpAccRepId + (entryLoop * ESMTPMobilityAccountListOffset);

			User::LeaveIfError(aRepository.Get(offset + ESMTPMobilityAccountListNumIapsId, numIaps));
			aRepository.Delete(offset + ESMTPMobilityAccountListNumIapsId);

			for (TInt iapLoop = 0; iapLoop < numIaps; ++iapLoop)
				{
				aRepository.Delete(offset + ESMTPMobilityAccountListIapId + iapLoop);
				}

			User::LeaveIfError(aRepository.Get(offset + ESMTPMobilityAccountListAccountId, tempInt));

			TUint32 mobAccRepId = tempInt;

			DeleteSmtpMobilityAccountFromLinkedSmtpAccountL(mobAccRepId, aRepository);

			DeleteSmtpSettings(mobAccRepId, aRepository);
			}
		}
	else if (err != KErrNotFound)
		{
		User::Leave(err);
		}
	}

/**
Deletes all the SMTP mobility accounts that are linked to a given SMTP account

@param aSmtpAccRepId SMTP account repository ID
@param aRepository SMTP repository
*/
void CEmailAccounts::DeleteAllLinkedMobilityAccountsL(TUint32 aLinkedSmtpAccRepId, CRepository& aRepository)
	{
	aRepository.Delete(aLinkedSmtpAccRepId + ESMTPLinkedMobilityAccountListVersionId);

	TInt numEntries = 0;
	TInt err = aRepository.Get(aLinkedSmtpAccRepId + ESMTPLinkedMobilityAccountListNumEntriesId, numEntries);
	if (err == KErrNone)
		{
		aRepository.Delete(aLinkedSmtpAccRepId + ESMTPLinkedMobilityAccountListNumEntriesId);

		TUint32 mobAccRepId = 0;
		TUint32 parentSmtpAccRepId = 0;
		TInt tempInt = 0;

		for (TInt entryLoop = 0; entryLoop < numEntries; ++entryLoop)
			{
			User::LeaveIfError(aRepository.Get(aLinkedSmtpAccRepId + ESMTPLinkedMobilityAccountListEntryId + entryLoop, tempInt));
			mobAccRepId = tempInt;

			aRepository.Delete(aLinkedSmtpAccRepId + ESMTPLinkedMobilityAccountListEntryId + entryLoop);

			User::LeaveIfError(aRepository.Get(mobAccRepId + ESMTPParentAccountId, tempInt));
			parentSmtpAccRepId = static_cast<TUint32>(tempInt) << KShiftAccountId;

			DeleteSmtpMobilityAccountListEntryL(mobAccRepId, aRepository, parentSmtpAccRepId);

			DeleteSmtpSettings(mobAccRepId, aRepository);
			}
		}
	else if (err != KErrNotFound)
		{
		User::Leave(err);
		}
	}

/**
Deletes a SMTP mobility account entry from a SMTP account

@param aMobAccRepId SMTP mobility account repository ID
@param aRepository SMTP repository
@param aSmtpAccRepId SMTP account repository ID
*/
void CEmailAccounts::DeleteSmtpMobilityAccountListEntryL(TUint32 aMobAccRepId, CRepository& aRepository, TUint32 aSmtpAccRepId)
	{
	TInt numEntries = 0;
	TInt err = aRepository.Get(aSmtpAccRepId + ESMTPMobilityAccountListNumEntriesId, numEntries);
	if (err == KErrNone)
		{
		TInt entryPos = FindAccountIdInSmtpMobilityAccountListL(aMobAccRepId, aRepository, aSmtpAccRepId, numEntries);

		if (entryPos != KErrNotFound)
			{
			TUint32 offset = 0;
			TUint32 nextOffset = 0;
			TInt tempInt = 0;
			TInt numIaps = 0;

			// The current entry is deleted by moving every entry below it one
			// place up in the list
			for (TInt entryLoop = entryPos; entryLoop < numEntries; ++entryLoop)
				{
				offset = aSmtpAccRepId + (entryLoop * ESMTPMobilityAccountListOffset);

				// Clear the current entry
				aRepository.Delete(offset + ESMTPMobilityAccountListAccountId);
				aRepository.Delete(offset + ESMTPMobilityAccountListNumIapsId);
				for (TInt iapLoop = 0; iapLoop < KMaxSmtpMobilityAccountIAPListEntries; ++iapLoop)
					{
					aRepository.Delete(offset + ESMTPMobilityAccountListIapId + iapLoop);
					}

				// Copy next entry over the top of this one
				if (entryLoop < numEntries - 1)
					{
					nextOffset = offset + ESMTPMobilityAccountListOffset;
					User::LeaveIfError(aRepository.Get(nextOffset + ESMTPMobilityAccountListAccountId, tempInt));
					User::LeaveIfError(aRepository.Set(offset + ESMTPMobilityAccountListAccountId, tempInt));

					User::LeaveIfError(aRepository.Get(nextOffset + ESMTPMobilityAccountListNumIapsId, numIaps));
					User::LeaveIfError(aRepository.Set(offset + ESMTPMobilityAccountListNumIapsId, numIaps));
					
					for (TInt iapLoop = 0; iapLoop < numIaps; ++iapLoop)
						{
						User::LeaveIfError(aRepository.Get(nextOffset + ESMTPMobilityAccountListIapId + iapLoop, tempInt));
						User::LeaveIfError(aRepository.Set(offset + ESMTPMobilityAccountListIapId + iapLoop, tempInt));
						}
					}
				}

			User::LeaveIfError(aRepository.Set(aSmtpAccRepId + ESMTPMobilityAccountListNumEntriesId, numEntries - 1));
			}
		}
	else if (err != KErrNotFound)
		{
		User::Leave(err);
		}
	}

/**
If this mobility account is linked to a SMTP account, remove it from the
list of linked accounts.

@param aMobAccRepId SMTP mobility account repository ID
@param aRepository SMTP repository
*/
void CEmailAccounts::DeleteSmtpMobilityAccountFromLinkedSmtpAccountL(TUint32 aMobAccRepId, CRepository& aRepository)
	{
	// If this mobility account is linked to a SMTP account, then we need to
	// remove it from the list of linked accounts maintained by that SMTP
	// account.
	TInt tempInt = 0;
	User::LeaveIfError(aRepository.Get(aMobAccRepId + ESMTPLinkedAccountId, tempInt));

	if (tempInt != KNullSmtpAccountId)
		{
		// This mobility account is linked to a SMTP account

		TUint32 smtpAccRepId = static_cast<TUint32>(tempInt) << KShiftAccountId;

		TInt numEntries = 0;		
		User::LeaveIfError(aRepository.Get(smtpAccRepId + ESMTPLinkedMobilityAccountListNumEntriesId, numEntries));

		for (TInt entryLoop = 0; entryLoop < numEntries; entryLoop++)
			{
			User::LeaveIfError(aRepository.Get(smtpAccRepId + ESMTPLinkedMobilityAccountListEntryId + entryLoop, tempInt));

			// Check if the entry matches the account we are looking for
			if (aMobAccRepId == tempInt)
				{
				// This entry matches so remove it. We remove it by overwriting it with
				// the last entry in the list, and then deleting the last entry in the
				// list.
				if (entryLoop < numEntries - 1)
					{
					// Get last entry from the list and overwrite this entry
					User::LeaveIfError(aRepository.Get(smtpAccRepId + ESMTPLinkedMobilityAccountListEntryId + numEntries - 1, tempInt));
					User::LeaveIfError(aRepository.Set(smtpAccRepId + ESMTPLinkedMobilityAccountListEntryId + entryLoop, tempInt));
					}

				// Delete last entry from list
				User::LeaveIfError(aRepository.Delete(smtpAccRepId + ESMTPLinkedMobilityAccountListEntryId + numEntries - 1));

				User::LeaveIfError(aRepository.Set(smtpAccRepId + ESMTPLinkedMobilityAccountListNumEntriesId, numEntries - 1));

				// Exit loop now as we have found the linked entry
				break;
				}
			}
		}
	}

/**
Checks if an SMTP account or SMTP mobility account is read only

@param aAccRepId SMTP account / SMTP mobility account ID

@return ETrue if read only, EFalse if not read only
*/
TBool CEmailAccounts::IsSmtpTypeAccountReadOnlyL(TUint32 aAccRepId)
	{
	TInt tempInt = 0;
	User::LeaveIfError(SmtpRepositoryL().Get(aAccRepId + ESMTPReadOnlyAccountId, tempInt));
	TBool isReadOnly = static_cast<TBool>(tempInt);

	return isReadOnly;
	}

/**
Checks if the account is a SMTP mobility account

@param aRepository SMTP repository
@param aAccountId Account ID

@return ETrue if it is a SMTP mobility account, EFalse if not
*/
TBool CEmailAccounts::IsSmtpMobilityAccount(CRepository& aRepository, TUint32 aAccRepId)
	{
 	TInt parent = 0;

	if (aRepository.Get(aAccRepId + ESMTPParentAccountId, parent) == KErrNone)
		{
		if (parent != KNullSmtpAccountId)
			{
			return ETrue;
			}
		}

	return EFalse;
	}


/** Tests whether all of the characters in the specified text can be represented 
in 7-bit ASCII. 

@param aText Text to be tested.
@return ETrue if all of the characters in the specified text is printable, otherwise EFalse. */
TBool CEmailAccounts::IsAsciiPrintable(const TDesC16& aText)
     {
     const TInt KMax7BitValue = 126;
     const TInt KMin7BitValue = 32;

     const TInt length = aText.Length();
     const TText16* pText = aText.Ptr();

     for(TInt i=0; i<length; i++)
		{
		if  ((pText[i] > KMax7BitValue) || (pText[i] < KMin7BitValue))
			return EFalse;
       	}

    // Is a valid printable ascii string
    return ETrue;
    }

/** Tests whether all of the characters in the specified text can be represented 
in 7-bit ASCII. 

@param aText Text to be tested.
@return ETrue if all of the characters in the specified text is printable, otherwise EFalse. */
TBool CEmailAccounts::IsAsciiPrintable(const TDesC8& aText)
     {
     const TInt KMax7BitValue = 126;
     const TInt KMin7BitValue = 32;
     
     const TInt length = aText.Length();
     const TText8* pText = aText.Ptr();

     for(TInt i=0; i<length; i++)
		{
		if  ((pText[i] > KMax7BitValue) || (pText[i] < KMin7BitValue))
			return EFalse;
       	}

    // Is a valid printable ascii string
    return ETrue;
    }


/** Tests whether all of the characters in the specified text are printable UniCode.    
    
@param aText Text to be tested.
@return ETrue if all of the characters in the specified text is printable, otherwise EFalse. */
TBool CEmailAccounts::IsUnicodePrintable(const TDesC16& aText)
     {
  	  const TText16* pText = aText.Ptr();
      for(TInt i=0; i<aText.Length(); ++i)
		{
	     TChar aChar = pText[i];
	  	 if(!(aChar.IsPrint()))  
		  	{
			return EFalse;  
		  	}
       	}

    // Is a valid printable unicode string
    return ETrue;
    }

#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
/**
* Send the header structure to message Server to create  header table.
 
* @param  aMTMid  MTM ID for which header to be created.
* @return  None.

*/
void CEmailAccounts::CreateHeaderStoreL(TUid aMTMid)
	{
	CMsvHeaderStore* aHeader = CMsvHeaderStore::NewL(aMTMid ,SessionL());
	CleanupStack::PushL(aHeader);
	
	_LIT (KEmailHeadearversion , "Version");
	aHeader->AddFieldL(KEmailHeadearversion().AllocL(),EIntegerField);

	_LIT (KReceiptAddress , "ReceiptAddress");
	aHeader->AddFieldL(KReceiptAddress().AllocL(),ETextField);
	
	_LIT (KImMsgId , "ImMsgId");
	aHeader->AddFieldL(KImMsgId().AllocL(),ETextField);

	aHeader->AddFieldL(CMsvHeaderStore::EFrom);

	_LIT (KReplyTo , "ReplyTo");
	aHeader->AddFieldL(KReplyTo().AllocL(),ETextField);

	aHeader->AddFieldL(CMsvHeaderStore::ESubject);

	_LIT (KiRemoteSize , "iRemoteSize");
	aHeader->AddFieldL(KiRemoteSize().AllocL(),EIntegerField);

	aHeader->AddFieldL(CMsvHeaderStore::ETo);

	aHeader->AddFieldL(CMsvHeaderStore::ECC);

	aHeader->AddFieldL(CMsvHeaderStore::EBCC);

	_LIT (KEncodingInfo , "EncodingInfo");
	aHeader->AddFieldL(KEncodingInfo().AllocL(),ETextField);
	
	//----------------------Used for forwarding/replying to an email ----------------------	
	
	_LIT (KResentMsgId , "ResentMsgId");
	aHeader->AddFieldL(KResentMsgId().AllocL(),ETextField);
	
	_LIT (KResentFrom , "ResentFrom");
	aHeader->AddFieldL(KResentFrom().AllocL(),ETextField);
	
	_LIT (KResentToRecipients , "ResentToRecipients");
	aHeader->AddFieldL(KResentToRecipients().AllocL(),ETextField);
	
	_LIT (KResentCcRecipients , "ResentCcRecipients");
	aHeader->AddFieldL(KResentCcRecipients().AllocL(),ETextField);
	
	_LIT (KResentBccRecipients , "ResentBccRecipients");
	aHeader->AddFieldL(KResentBccRecipients().AllocL(),ETextField);

	_LIT (KInReplyTo , "InReplyTo");
	aHeader->AddFieldL(KInReplyTo().AllocL(),ETextField);

	_LIT (KBodyEncoding , "BodyEncoding");
	aHeader->AddFieldL(KBodyEncoding().AllocL(),EIntegerField);

	_LIT (KCharset , "Charset");
	aHeader->AddFieldL(KCharset().AllocL(),EIntegerField);

	aHeader->CreateStoreL();
	
	CleanupStack::PopAndDestroy(aHeader);  //aHeader
	}

#endif
