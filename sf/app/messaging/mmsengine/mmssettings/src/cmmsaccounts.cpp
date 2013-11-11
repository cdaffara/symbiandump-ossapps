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
// CMMSACCOUNT.cpp
// 
//


#include <centralrepository.h>
#include <cmmsaccounts.h>
#include <cmmssettings.h>
#include <mmmssettingsobserver.h>


const TUint32 KNullId = 0x00000000; 
const TUid KMMSRepositoryUid = {0x10202D4C};
const TInt KMaxSettingLength = 256;
const TInt KMmsSettingsVersion = 1;
const TUint32 KIncrementAccount = 0x00100000;
const TInt KShiftAccountId = 20;
const TUint32 KInitialiseSettings = 0x00000000; // Initial settings are in account 0

const TUint32 KAccountMask = 0x800FFFFF; 
const TUint32 KMaximumAccounts = 0x80000000; 
const TUint32 KDefaultAccountId = 0x80000003; 



/** 
Allocates and constructs an MMS account object.

Initialises all member data to their default values.

@return
The newly constructed MMS account object.
*/
EXPORT_C CMmsAccounts* CMmsAccounts::NewL()
	{
	CMmsAccounts* self = CMmsAccounts::NewLC();
	CleanupStack::Pop(self);	
	return self;
	}

/** 
Allocates and constructs an MMS account object.

Initialises all member data to their default values.

@return
The newly constructed MMS account object.
*/
EXPORT_C CMmsAccounts* CMmsAccounts::NewLC()
	{
	CMmsAccounts* self = new (ELeave) CMmsAccounts();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
Second phase construction.
*/
void CMmsAccounts::ConstructL()
	{
	iRepository = CRepository::NewL(KMMSRepositoryUid);
	}

/**
Constructor.
*/
CMmsAccounts::CMmsAccounts()
: CActive(CActive::EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}

/** 
Destructor. 
*/
EXPORT_C CMmsAccounts::~CMmsAccounts()
	{
	Cancel();
	iObservers.Close();
	delete iRepository;
	}

void CMmsAccounts::DoCancel()
	{
	// cancel request to the repository
	iRepository->NotifyCancel(KNullId, KNullId);
	}	

void CMmsAccounts::RunL()
	{
	// Notify all observers
	for(TInt i=iObservers.Count()-1; i>=0; --i)
      	{
     	iObservers[i]->HandleNotify(MMmsSettingsObserver::ESettingsUpdated);
      	}
      	
    // Re-issue notification request  	
	User::LeaveIfError(iRepository->NotifyRequest(KNullId, KNullId, iStatus));
	SetActive();
	}	

/** 
Registers a new MMS Settings observer. 

@param aObserver 
A reference to an observer to be registered for events 
*/	
EXPORT_C void CMmsAccounts::AddObserverL(MMmsSettingsObserver& aObserver)
	{
	iObservers.AppendL(&aObserver);
	
	if (IsActive() == EFalse)
		{
		// Issue notification request
		User::LeaveIfError(iRepository->NotifyRequest(KNullId, KNullId, iStatus));
		SetActive();
		}
	}

/** 
Deregisters a previously registered observer. 

@param aObserver
A reference to an observer to be unregistered for events 
*/	
EXPORT_C void CMmsAccounts::RemoveObserver(MMmsSettingsObserver& aObserver)
	{
	TInt i = iObservers.Find(&aObserver);
    __ASSERT_ALWAYS(i>=0, User::Invariant());
    iObservers.Remove(i);
    
    if (iObservers.Count() <= 0)
    	{
		Cancel();    	
    	}
	}

/**
Creates a MMS Account.

@param	aAccountName
MMS account Name

@param	aSettings
MMS Settings

@return
MMS Account Id
*/	
EXPORT_C TMmsAccountId CMmsAccounts::CreateMMSAccountL(const TDesC& aAccountName, const CMmsSettings& aSettings)
	{
	// Get the number of accounts exist in CenRep
	RArray<TUint32> accountIds;	
	CleanupClosePushL(accountIds);	
	TInt error = iRepository->FindL(KNullId, static_cast<TUint32>(KAccountMask), accountIds);
	if (error != KErrNotFound)
		{
		User::LeaveIfError(error);
		}
	TInt accountsCount = accountIds.Count() - 1; // ignoring the default account
	CleanupStack::PopAndDestroy(&accountIds);				

	if (accountsCount >= MaxMMSAccounts())
		{
		// Can't create any more accounts
		User::Leave(KErrOverflow);
		}
	
	TUint32 accountId = 0;
	accountId = GetNextEmptyAccountSlotL();
	DoSaveSettingsL(accountId, aSettings, aAccountName);
	
	TMmsAccountId account;
	account.iMmsAccountId = accountId >> KShiftAccountId; 
	account.iMmsAccountName = aAccountName; 			
	return account;				
	}

TUint CMmsAccounts::GetNextEmptyAccountSlotL()
	{
	TUint32 accountId = KNullId;
	TBuf<KMmsAccountNameSize> accountName;	
	TInt error = 0;
	TBool found = EFalse;
	TInt maximumAccount = MaxMMSAccounts();
	
	for (TInt count = 0; count < maximumAccount; ++count)
		{		
		accountId = accountId + KIncrementAccount;
		error = iRepository->Get(accountId + EMmsAccountName, accountName);		
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

/**
Maximum number of accounts that can be stored in the Central Repository

@return
The maximum number of accounts
*/		
EXPORT_C TInt CMmsAccounts::MaxMMSAccounts()
	{
	TInt maxAccounts = 0;
	TInt error = iRepository->Get(KMaximumAccounts, maxAccounts);
	if(error != KErrNone)
		{
		 return error;
		}
	return maxAccounts;		
	}

/**
Gets a list of MMS account IDs stored in the Central Repository.

Any existing entries in the array will be deleted.

@param	aAccountIds
A list if MMS account Ids
*/                   
EXPORT_C void CMmsAccounts::GetMMSAccountsL(RArray<TMmsAccountId>& aAccountIds) const
	{
    CleanupClosePushL( aAccountIds );
	aAccountIds.Reset();
	RArray<TUint32> accountIds;	
	CleanupClosePushL(accountIds);	
	TInt error = iRepository->FindL(KNullId, static_cast<TUint32>(KAccountMask), accountIds);
	if (error != KErrNotFound)
		{
		User::LeaveIfError(error);
		}
		
	TMmsAccountId accountId;
	TInt count = accountIds.Count();
	for(TInt i = 1; i < count; ++i)
		{
		TUint32 settingId = accountIds[i];
		User::LeaveIfError(iRepository->Get(settingId + EMmsAccountName, accountId.iMmsAccountName));	
		accountId.iMmsAccountId = accountIds[i] >> KShiftAccountId;
		aAccountIds.AppendL(accountId);		
		}
		
	CleanupStack::PopAndDestroy(&accountIds);
	CleanupStack::Pop(&aAccountIds);  // aAccountIds
	}

/**
Deletes the specified MMS account.

@param	aAccountId
MMS account Id
*/    	
EXPORT_C void CMmsAccounts::DeleteMMSAccountL(const TMmsAccountId& aAccountId)
	{
	TUint32 accountId = aAccountId.iMmsAccountId << KShiftAccountId;

	// Read in proxy and napId counts
	TInt proxyCount = 0;
	User::LeaveIfError(iRepository->Get(accountId + EMmsProxyCount, proxyCount));				

	TInt napIdCount = 0;
	User::LeaveIfError(iRepository->Get(accountId + EMmsNapIdCount, napIdCount));				

	// Delete account
	User::LeaveIfError(iRepository->StartTransaction(CRepository::EReadWriteTransaction));						
	iRepository->CleanupRollbackTransactionPushL();

	iRepository->Delete(accountId + EMmsAccountName);	
	iRepository->Delete(accountId + EMmsSettingsVersion);	
	iRepository->Delete(accountId + EMmsApplicationID);	
	iRepository->Delete(accountId + EMmsAddress);	
	iRepository->Delete(accountId + EMmsCreationMode);
	iRepository->Delete(accountId + EMmsSettingsFlags);
	iRepository->Delete(accountId + EMmsProxyCount);

	TUint32 proxyId = accountId + EMmsProxyList;
	for(TInt loop=0; loop < proxyCount; ++loop)
		{
		iRepository->Delete(proxyId);			
		++proxyId;
		}

	iRepository->Delete(accountId + EMmsNapIdCount);		

	TUint32 napId = accountId + EMmsNapIdList;
	for(TInt loop=0; loop < proxyCount; ++loop)
		{
		iRepository->Delete(napId);			
		++napId;
		}

	iRepository->Delete(accountId + EMmsAutomaticDownload);	
	iRepository->Delete(accountId + EMmsValidityPeriod);	
	iRepository->Delete(accountId + EMmsMaxDownloadSize);	
	iRepository->Delete(accountId + EMmsMaxDownloadRetries);	
	iRepository->Delete(accountId + EMmsDownloadRetryInterval);	
	iRepository->Delete(accountId + EMmsMaxSendMsgSize);	
	iRepository->Delete(accountId + EMmsDeviceContentClass);	
	iRepository->Delete(accountId + EMmsMaxImageHeight);	
	iRepository->Delete(accountId + EMmsMaxImageWidth);	

	CleanupStack::Pop();
	TUint32 errorId = 0;
	User::LeaveIfError(iRepository->CommitTransaction(errorId));
	}

/**
Loads MMS settings from the Central Repository for the specified account Id. 

@param	aAccountId
MMS account Id

@param	aSettings
MMS settings
*/	     
EXPORT_C void CMmsAccounts::LoadSettingsL(const TMmsAccountId& aAccountId, CMmsSettings& aSettings)
	{
	TUint32 accountId = aAccountId.iMmsAccountId << KShiftAccountId;		
	DoLoadSettingsL(accountId, aSettings);
	}

void CMmsAccounts::DoLoadSettingsL(TUint32 aAccountId, CMmsSettings& aSettings)
	{				
	TInt temp = 0;
	User::LeaveIfError(iRepository->Get(aAccountId + EMmsApplicationID, temp));	
	aSettings.SetApplicationID(temp);

	TBuf<KMaxSettingLength> tempBuff;
	User::LeaveIfError(iRepository->Get(aAccountId + EMmsAddress, tempBuff));	
	aSettings.SetAddressL(tempBuff);	
	
	User::LeaveIfError(iRepository->Get(aAccountId + EMmsCreationMode, temp));	
	aSettings.SetCreationModeL(static_cast<TCreationMode>(temp));

	User::LeaveIfError(iRepository->Get(aAccountId + EMmsSettingsFlags, temp));
	aSettings.SetMmsSettingsFlags(static_cast<TUint32>(temp));

	User::LeaveIfError(iRepository->Get(aAccountId + EMmsProxyCount, temp));	
	TInt count = temp;
		
	aSettings.RemoveAllProxies();
	TUint32 proxyId = aAccountId + EMmsProxyList;	
	for(TInt loop=0; loop < count; ++loop)
		{
		User::LeaveIfError(iRepository->Get(proxyId, temp));
		aSettings.AddProxyL(TUid::Uid(temp));   			
		++proxyId;
		}

	User::LeaveIfError(iRepository->Get(aAccountId + EMmsNapIdCount, count));	

	aSettings.RemoveAllNapIds();
	TUint32 napId = aAccountId + EMmsNapIdList;	
	for(TInt loop=0; loop < count; ++loop)
		{
		User::LeaveIfError(iRepository->Get(napId, temp));
		aSettings.AddNapIdL(TUid::Uid(temp));   			
		++napId;
		}
		
	User::LeaveIfError(iRepository->Get(aAccountId + EMmsAutomaticDownload, temp));	
	aSettings.SetAutomaticDownload(static_cast<TAutomaticDownloadOptions>(temp));
		
	User::LeaveIfError(iRepository->Get(aAccountId + EMmsValidityPeriod, temp));	
	aSettings.SetValidityPeriod(temp);

	User::LeaveIfError(iRepository->Get(aAccountId + EMmsMaxDownloadSize, temp));	
	aSettings.SetMaxDownloadSize(temp);

	User::LeaveIfError(iRepository->Get(aAccountId + EMmsMaxDownloadRetries, temp));	
	aSettings.SetMaxDownloadRetries(temp);

	User::LeaveIfError(iRepository->Get(aAccountId + EMmsDownloadRetryInterval, temp));	
	aSettings.SetDownloadRetryInterval(temp);

	User::LeaveIfError(iRepository->Get(aAccountId + EMmsMaxSendMsgSize, temp));	
	aSettings.SetMaxSendMsgSize(temp);

	User::LeaveIfError(iRepository->Get(aAccountId + EMmsDeviceContentClass, temp));	
	aSettings.SetDeviceContentClass(temp);

	User::LeaveIfError(iRepository->Get(aAccountId + EMmsMaxImageHeight, temp));	
	aSettings.SetMaxImageHeight(temp);

	User::LeaveIfError(iRepository->Get(aAccountId + EMmsMaxImageWidth, temp));	
	aSettings.SetMaxImageWidth(temp);	
	}

/**
Populates the supplied setting object with default values.

@param	aSettings
MMS Setting
*/	
EXPORT_C void CMmsAccounts::PopulateDefaultSettingsL(CMmsSettings& aSettings)
	{
	DoLoadSettingsL(KInitialiseSettings, aSettings);		
	}

/**
Saves MMS settings to the Central Repository for the specified account Id. 

@param	aAccountId
MMS account Id

@param	aSettings
MMS settings
*/
EXPORT_C void CMmsAccounts::SaveSettingsL(const TMmsAccountId& aAccountId, const CMmsSettings& aSettings) const
	{
	TUint32 accountId = aAccountId.iMmsAccountId << KShiftAccountId;
	DoSaveSettingsL(accountId, aSettings, aAccountId.iMmsAccountName);
	}

void CMmsAccounts::DoSaveSettingsL(TUint32 aAccountId, const CMmsSettings& aSettings, const TDesC& aAccountName) const
	{
	User::LeaveIfError(iRepository->StartTransaction(CRepository::EReadWriteTransaction));
	iRepository->CleanupRollbackTransactionPushL();
	// Update account name
	iRepository->Set(aAccountId + EMmsAccountName, aAccountName);
	iRepository->Set(aAccountId + EMmsSettingsVersion, KMmsSettingsVersion); 	
	iRepository->Set(aAccountId + EMmsApplicationID, aSettings.ApplicationID());
	iRepository->Set(aAccountId + EMmsAddress, aSettings.Address());
	iRepository->Set(aAccountId + EMmsCreationMode, static_cast<TInt>(aSettings.CreationMode()));	
	iRepository->Set(aAccountId + EMmsSettingsFlags, static_cast<TInt>(aSettings.MmsSettingsFlags()));	
	iRepository->Set(aAccountId + EMmsProxyCount, aSettings.ProxyCount());
		
	TUint32 proxyId = aAccountId + EMmsProxyList;	
	for(TInt loop=0; loop < aSettings.ProxyCount(); ++loop)
		{
		iRepository->Set(proxyId, static_cast<TInt>(aSettings.GetProxy(loop).iUid));
		++proxyId;
		}

	iRepository->Set(aAccountId + EMmsNapIdCount, aSettings.NapIdCount());

	TUint32 napId = aAccountId + EMmsNapIdList;	
	for(TInt loop=0; loop < aSettings.NapIdCount(); ++loop)
		{
		iRepository->Set(napId, static_cast<TInt>(aSettings.GetNapId(loop).iUid));
		++napId;
		}

	iRepository->Set(aAccountId + EMmsAutomaticDownload, static_cast<TInt>(aSettings.AutomaticDownload()));
	iRepository->Set(aAccountId + EMmsValidityPeriod, aSettings.ValidityPeriod());
	iRepository->Set(aAccountId + EMmsMaxDownloadSize, aSettings.MaxDownloadSize());
	iRepository->Set(aAccountId + EMmsMaxDownloadRetries, aSettings.MaxDownloadRetries());
	iRepository->Set(aAccountId + EMmsDownloadRetryInterval, aSettings.DownloadRetryInterval());
	iRepository->Set(aAccountId + EMmsMaxSendMsgSize, aSettings.MaxSendMsgSize());
	iRepository->Set(aAccountId + EMmsDeviceContentClass, aSettings.DeviceContentClass());
	iRepository->Set(aAccountId + EMmsMaxImageHeight, aSettings.MaxImageHeight());
	iRepository->Set(aAccountId + EMmsMaxImageWidth, aSettings.MaxImageWidth());
	
	CleanupStack::Pop();
	TUint32 errorId = 0;
	User::LeaveIfError(iRepository->CommitTransaction(errorId));	
	}

/**
Gets the default MMS account.

@return
The default account

@leave 
KErrNotFound If the default account not been set previously or if MMSSettings for 
             the default account does not exist in the Central Repository.
*/
EXPORT_C TMmsAccountId CMmsAccounts::DefaultMMSAccountL( ) const
	{
	TMmsAccountId account;
	User::LeaveIfError(iRepository->Get(KDefaultAccountId, account.iMmsAccountId));

	TUint32 accountId = account.iMmsAccountId << KShiftAccountId;		
	User::LeaveIfError(iRepository->Get(accountId + EMmsAccountName, account.iMmsAccountName));	
	
	return account;	
	}

/**
Sets the default MMS account.

@param	aDefaultAccount
Account to be set as default
*/		
EXPORT_C void CMmsAccounts::SetDefaultMMSAccountL(const TMmsAccountId& aDefaultAccount)
	{
	iRepository->Set(KDefaultAccountId, aDefaultAccount.iMmsAccountId);
	}
	
	

