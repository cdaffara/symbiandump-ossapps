// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "T_MmsSettings.h"
#include <cmmssettings.h>
#include <cmmsaccounts.h>
#include <mmmssettingsobserver.h>
#include <centralrepository.h>

class CTestNotification : public CBase, public MMmsSettingsObserver
	{
public:
	static CTestNotification* NewL( );
	CTestNotification();	
	void ConstructL();
	~CTestNotification();
  	void HandleNotify(TMmsSettingsEvent aEvent);
  	void StartSetTestL();
  	void StartCreateTestL();
	void StartDeleteTestL();
	};




RTest test(_L("MMS Settings"));
CMmsAccounts* account;
CMmsSettings* settings;
CMmsSettings* settingsVerify;
CTestNotification* notification;
TMmsAccountId accountId;
TBuf<KMmsAccountNameSize> buffer1;
TBuf<KMmsAccountNameSize> buffer2;

const TUid KProxy = {0x1001};
const TUid KNapId = {0x1002};
const TUid KMMSRepositoryUid = {0x10202D4C};

_LIT(KAccountName, "MMS Account");
_LIT(KAddress, "MMS Address");
_LIT(KAccountNameBlank, "");

_LIT(KAccount1, "AccountNo 1: %d");
_LIT(KAccount2, "AccountNo 2: %d");
_LIT(KAccount3, "AccountNo 3: %d");
_LIT(KAccount4, "AccountNo 4: %d");
	

CTestNotification* CTestNotification::NewL()
	{
	CTestNotification* self = new(ELeave) CTestNotification();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CTestNotification::ConstructL()
	{
	account->AddObserverL(*this);	
	}

CTestNotification::CTestNotification()
	{
	}

CTestNotification::~CTestNotification() 
	{
	}
	
void CTestNotification::StartCreateTestL()
	{		
	//create a new account
	settings = CMmsSettings::NewL();
	account->PopulateDefaultSettingsL(*settings);
	accountId = account->CreateMMSAccountL(KAccountName, *settings);

	// cleanup
	delete settings;	
	}

void CTestNotification::StartSetTestL()
	{		
	// change a setting
	settings = CMmsSettings::NewL();
	account->LoadSettingsL(accountId, *settings);
	settings->SetApplicationID(4);
	account->SaveSettingsL(accountId, *settings);
	
	// cleanup
	delete settings;	
	}

void CTestNotification::StartDeleteTestL()
	{		
	// delete a account
	account->DeleteMMSAccountL(accountId);
	}


void CTestNotification::HandleNotify(TMmsSettingsEvent /*aEvent*/)
	{
	_LIT(KComment, "\t\tNotification received");
	theUtils->WriteComment(KComment);
	CActiveScheduler::Stop();
	}


// Forward declarations and doTestL() are placed here to satisfy leavescan.
// If leavescan is at the bottom of the file, a leavescan error is produces.
void InitL();
void TestCreateAccountL();
void TestSaveSettingsL();
void TestDefaultAccountL();
void TestCopySettingsL();
void TestDeleteAccountL();
void TestDeletedDefaultAccountL();
void TestMultipleAccountsWithBlankNameL();
void TestMultipleAccountsL();
void TestNotificationL();
void CleanupL();
	
void doTestsL()
	{
	InitL();
	
	TestCreateAccountL();
	TestSaveSettingsL();
	TestDefaultAccountL();
	TestCopySettingsL();
	TestDeleteAccountL();
	TestDeletedDefaultAccountL();
	TestMultipleAccountsWithBlankNameL();
	TestMultipleAccountsL();
	TestNotificationL();

	CleanupL();
	}

TBool CompareBooleans(TBool aBoolean1, TBool aBoolean2, const TDesC* aTestDescription)
	{
	_LIT(KErrorReason, "\t\tSettings not set correctly for %S");
	TBuf<80> buf;
	TBuf<80> temp;
	temp.Copy(*aTestDescription);
	TBool result = ((aBoolean1 && aBoolean2) || (!aBoolean1 && !aBoolean2));
	if (!result)
		{
		buf.AppendFormat(KErrorReason, &temp);
		theUtils->WriteComment(buf);
		}

	return result;
	}

TBool CompareDescriptors(TDesC& aDesC1, TDesC16& aDesC2, const TDesC* aTestDescription)
	{
	_LIT(KErrorReason, "\t\tSettings not set correctly for %S");
	TBuf<80> buf;
	TBuf<80> temp;
	temp.Copy(*aTestDescription);
	TBool result = (aDesC1 == aDesC2);
	if (!result)
		{
		buf.AppendFormat(KErrorReason, &temp);
		theUtils->WriteComment(buf);
		}

	return result;
	}

TBool CompareIntegers(TInt aInt1, TInt aInt2, const TDesC* aTestDescription)
	{
	_LIT(KErrorReason, "\t\tSettings not set correctly for %S");
	TBuf<80> buf;
	TBuf<80> temp;
	temp.Copy(*aTestDescription);
	TBool result = (aInt1 == aInt2);
	if (!result)
		{
		buf.AppendFormat(KErrorReason, &temp);
		theUtils->WriteComment(buf);
		}

	return result;
	}
	
LOCAL_C void ResetSettings()
	{
	// Reset the repository by clearing any existing settings and restoring the 
   	// default settings
	CRepository* repository = CRepository::NewL(KMMSRepositoryUid);
	repository->Reset();
	delete repository;
	}

void InitL()
	{	
	ResetSettings();	
	account = CMmsAccounts::NewLC();
	notification = CTestNotification::NewL();
	CleanupStack::PushL(notification);	
	}

void CleanupL()
	{
	CleanupStack::PopAndDestroy(2, account);	// notification, account
	}

void FillSettings()
	{
	settings->SetApplicationID(6);
   	settings->SetAddressL(KAddress);	
   	settings->SetCreationModeL(ECreationModeWarning);
    settings->AddProxyL(KProxy);
    settings->AddProxyL(KProxy);
    settings->AddProxyL(KProxy);
    settings->AddNapIdL(KNapId);
    settings->AddNapIdL(KNapId);
    settings->AddNapIdL(KNapId);
    settings->SetDisplayNotification(ETrue);
    settings->SetAutomaticDownload(EAutomaticDownloadOnlyWhenRoaming);
    settings->SetCreationModeReadOnly(ETrue);
    settings->SetValidityPeriod(16);
    settings->SetMaxDownloadSize (20);
    settings->SetPriority (ETrue);
    settings->SetHideNumber(ETrue);
    settings->SetReadReport(ETrue);
    settings->SetDeliveryReport(ETrue);
    settings->SetReplyWithHistory (EFalse);
    settings->SetAllowDeliveryNotification(EFalse);
    settings->SetFilterAdvertisements(EFalse);
    settings->SetMaxDownloadRetries(ETrue);
    settings->SetDownloadRetryInterval(7);
    settings->SetMaxSendMsgSize (8);
    settings->SetFullScreenPreview(EFalse);
	settings->SetDeviceContentClass(4);
	settings->SetMaxImageHeight(3);
    settings->SetMaxImageWidth(6);   

	_LIT(KComment, "\t\tCreated Filled POP3 Settings");
	theUtils->WriteComment(KComment);
	}

TBool CompareSettings()
	{
	_LIT(KComment, "\t\tComparing Filled POP3 Settings");
	theUtils->WriteComment(KComment);
	TBool result = ETrue;

	result &= CompareIntegers((settings->ApplicationID()), (settingsVerify->ApplicationID()), &KNullDesC);

	buffer1 = settings->Address();
	buffer2 = settingsVerify->Address();
	result &= CompareDescriptors(buffer1, buffer2, &KNullDesC); 

	TBool result1=((settings->CreationMode())==(settingsVerify->CreationMode()));
	if (!result1)
		{
		_LIT(KComment, "\t\tCreationMode not Identical");
		theUtils->WriteComment(KComment);
		}
	result &= result1;

	result &= CompareIntegers((settings->ProxyCount()), (settingsVerify->ProxyCount()), &KNullDesC);

	TBool result2 = EFalse;
	for (TInt index = 0; index < settings->ProxyCount(); ++index)
		{		
		result2=((settings->GetProxy(index))==(settingsVerify->GetProxy(index)));
		if (!result2)
			{
			_LIT(KComment, "\t\tProxy not Identical");
			theUtils->WriteComment(KComment);
			}
		result &= result2;
		}

	result &= CompareIntegers((settings->NapIdCount()), (settingsVerify->NapIdCount()), &KNullDesC);

	TBool result3 = EFalse;
	for (TInt index = 0; index < settings->NapIdCount(); ++index)
		{		
		result3=((settings->GetNapId(index))==(settingsVerify->GetNapId(index)));
		if (!result3)
			{
			_LIT(KComment, "\t\tNap Id not Identical");
			theUtils->WriteComment(KComment);
			}
		result &= result3;
		}

	result &= CompareBooleans(settings->DisplayNotification(), settingsVerify->DisplayNotification(), &KNullDesC);

	TBool result4=((settings->AutomaticDownload())==(settingsVerify->AutomaticDownload()));
	if (!result4)
		{
		_LIT(KComment, "\t\tDisplayNotification not Identical");
		theUtils->WriteComment(KComment);
		}
	result &= result4;

	result &= CompareBooleans(settings->CreationModeReadOnly(), settingsVerify->CreationModeReadOnly(), &KNullDesC);
	result &= CompareIntegers((settings->ValidityPeriod()), (settingsVerify->ValidityPeriod()), &KNullDesC);
	result &= CompareIntegers((settings->MaxDownloadSize()), (settingsVerify->MaxDownloadSize()), &KNullDesC);
	result &= CompareBooleans(settings->Priority(), settingsVerify->Priority(), &KNullDesC);
	result &= CompareBooleans(settings->HideNumber(), settingsVerify->HideNumber(), &KNullDesC);
	result &= CompareBooleans(settings->ReadReport(), settingsVerify->ReadReport(), &KNullDesC);
	result &= CompareBooleans(settings->DeliveryReport(), settingsVerify->DeliveryReport(), &KNullDesC);
	result &= CompareBooleans(settings->ReplyWithHistory(), settingsVerify->ReplyWithHistory(), &KNullDesC);
	result &= CompareBooleans(settings->AllowDeliveryNotification(), settingsVerify->AllowDeliveryNotification(), &KNullDesC);
	result &= CompareBooleans(settings->FilterAdvertisements(), settingsVerify->FilterAdvertisements(), &KNullDesC);
	result &= CompareIntegers((settings->MaxDownloadRetries()), (settingsVerify->MaxDownloadRetries()), &KNullDesC);
	result &= CompareIntegers((settings->DownloadRetryInterval()), (settingsVerify->DownloadRetryInterval()), &KNullDesC);
	result &= CompareIntegers((settings->MaxSendMsgSize()), (settingsVerify->MaxSendMsgSize()), &KNullDesC);
	result &= CompareBooleans(settings->FullScreenPreview(), settingsVerify->FullScreenPreview(), &KNullDesC);
	result &= CompareIntegers((settings->DeviceContentClass()), (settingsVerify->DeviceContentClass()), &KNullDesC);
	result &= CompareIntegers((settings->MaxImageHeight()), (settingsVerify->MaxImageHeight()), &KNullDesC);
	result &= CompareIntegers((settings->MaxImageWidth()), (settingsVerify->MaxImageWidth()), &KNullDesC);

	return result;
	}

void TestMultipleAccountsWithBlankNameL()
	{
	theUtils->Start(_L("Multiple Account With BlankName  Test"));
	// check there are no accounts
	RArray<TMmsAccountId> mmsAccounts;
	account->GetMMSAccountsL(mmsAccounts);	
	test(mmsAccounts.Count() == 0);
	// create 4 accounts
	settings = CMmsSettings::NewL();
	account->PopulateDefaultSettingsL(*settings);

	accountId = account->CreateMMSAccountL(KAccountNameBlank, *settings);
	theUtils->Printf(KAccount1, accountId.iMmsAccountId);
	test(accountId.iMmsAccountId == 1);
	
	TMmsAccountId accountId1 = account->CreateMMSAccountL(KAccountNameBlank, *settings);
	theUtils->Printf(KAccount2, accountId1.iMmsAccountId);
    test(accountId1.iMmsAccountId == 2);
	
	TMmsAccountId accountId2 = account->CreateMMSAccountL(KAccountNameBlank, *settings);
	theUtils->Printf(KAccount3, accountId2.iMmsAccountId);
	test(accountId2.iMmsAccountId == 3);

	TMmsAccountId accountId3 = account->CreateMMSAccountL(KAccountNameBlank, *settings);
	theUtils->Printf(KAccount4, accountId3.iMmsAccountId);
    test(accountId3.iMmsAccountId == 4 );

	// check accounts
	mmsAccounts.Reset();
	account->GetMMSAccountsL(mmsAccounts);	
	test(mmsAccounts.Count() == 4);
    // delete the  accounts
	account->DeleteMMSAccountL(accountId3);
	account->DeleteMMSAccountL(accountId1);
   	account->DeleteMMSAccountL(accountId2);
	account->DeleteMMSAccountL(accountId);
	mmsAccounts.Reset();
	account->GetMMSAccountsL(mmsAccounts);	
	test(mmsAccounts.Count() == 0);
	delete settings;
	mmsAccounts.Close();
    theUtils->Complete( ); 	
	}

void TestMultipleAccountsL()
	{
	theUtils->Start(_L("Multiple Account Test"));
	
	// check there are no accounts
	RArray<TMmsAccountId> mmsAccounts;
	account->GetMMSAccountsL(mmsAccounts);	
	test(mmsAccounts.Count() == 0);

	// create 4 accounts
	settings = CMmsSettings::NewL();
	account->PopulateDefaultSettingsL(*settings);
	accountId = account->CreateMMSAccountL(KAccountName, *settings);
	TMmsAccountId accountId1 = account->CreateMMSAccountL(KAccountName, *settings);
	TMmsAccountId accountId2 = account->CreateMMSAccountL(KAccountName, *settings);
	TMmsAccountId accountId3 = account->CreateMMSAccountL(KAccountName, *settings);

	// check accounts
	mmsAccounts.Reset();
	account->GetMMSAccountsL(mmsAccounts);	
	test(mmsAccounts.Count() == 4);
	
	// delete 2 accounts
	account->DeleteMMSAccountL(accountId3);
	account->DeleteMMSAccountL(accountId1);

	// check accounts
	mmsAccounts.Reset();
	account->GetMMSAccountsL(mmsAccounts);	
	test(mmsAccounts.Count() == 2);
	
	// cleanup
	account->DeleteMMSAccountL(accountId2);
	account->DeleteMMSAccountL(accountId);
	mmsAccounts.Reset();
	account->GetMMSAccountsL(mmsAccounts);	
	test(mmsAccounts.Count() == 0);
	delete settings;
	mmsAccounts.Close();

	theUtils->Complete( );
	}

void TestDeleteAccountL()
	{
	theUtils->Start(_L("Delete Account Test"));
	
	// should be one account created previously
	RArray<TMmsAccountId> mmsAccounts;
	account->GetMMSAccountsL(mmsAccounts);	
	test(mmsAccounts.Count() == 1);

	// delete account and check
	account->DeleteMMSAccountL(accountId);
	mmsAccounts.Reset();
	account->GetMMSAccountsL(mmsAccounts);	
	test(mmsAccounts.Count() == 0);

	// cleanup
	mmsAccounts.Close();

	theUtils->Complete();
	}

void TestDeletedDefaultAccountL()
/**
Checks if the default account has been deleted from the repository
For Defect Number: ZEBRA00018625
*/
	{
	theUtils->Start(_L("Delete Default Account Test"));
	
	RArray<TMmsAccountId> mmsAccounts;
	account->GetMMSAccountsL(mmsAccounts);	
	test(mmsAccounts.Count() == 0);

	TMmsAccountId deletedDefaultaccountId;
	// if the default account exists in the repository, along with valid settings 
	// then the account Id and name will be returned, else KErrNotFound will 
	// be returned.
	TRAPD( leaveCode,deletedDefaultaccountId = account->DefaultMMSAccountL() );
	test(leaveCode == KErrNotFound);

	// cleanup
	mmsAccounts.Reset();
	mmsAccounts.Close();
	theUtils->Complete();
	}

void TestCopySettingsL()
	{
	theUtils->Start(_L("Copy Settings Test"));
	settings = CMmsSettings::NewL();
	account->PopulateDefaultSettingsL(*settings);
	
	// load settings to copy
	settingsVerify = CMmsSettings::NewL();	
	account->LoadSettingsL(accountId, *settingsVerify);	
	
	// copy setting and check
	settings->CopyL(*settingsVerify);
	test(CompareSettings() == 1);
	
	// cleanup
	delete settingsVerify;
	delete settings;

	theUtils->Complete();
	}

void TestDefaultAccountL()
	{
	theUtils->Start(_L("Default Account Test"));
	
	account->SetDefaultMMSAccountL(accountId);
	TMmsAccountId accountId2 = account->DefaultMMSAccountL();

	test(accountId.iMmsAccountId == accountId2.iMmsAccountId);	
	test(accountId.iMmsAccountName == accountId2.iMmsAccountName);	

	theUtils->Complete();
	}

void TestSaveSettingsL()
	{
	theUtils->Start(_L("Save Settings Test"));
	
	// save settings
	settings = CMmsSettings::NewL();
	settingsVerify = CMmsSettings::NewL();	
	account->PopulateDefaultSettingsL(*settings);
	FillSettings();
	account->SaveSettingsL(accountId, *settings);

	// check settings and accounts
	account->LoadSettingsL(accountId, *settingsVerify);
	test(CompareSettings() == 1);
	
	// cleanup
	delete settingsVerify;
	delete settings;

	theUtils->Complete();
	}

void TestCreateAccountL()
	{
	theUtils->Start(_L("Create Account Test"));
	
	// check there are no accounts
	RArray<TMmsAccountId> mmsAccounts;
	account->GetMMSAccountsL(mmsAccounts);	
	test(mmsAccounts.Count() == 0);

	// create accounts
	settings = CMmsSettings::NewL();
	settingsVerify = CMmsSettings::NewL();	
	account->PopulateDefaultSettingsL(*settings);
	FillSettings();
	accountId = account->CreateMMSAccountL(KAccountName, *settings);

	// check settings and accounts
	account->LoadSettingsL(accountId, *settingsVerify);
	test(CompareSettings() == 1);
	mmsAccounts.Reset();
	account->GetMMSAccountsL(mmsAccounts);	
	test(mmsAccounts.Count() == 1);
	
	// cleanup
	delete settingsVerify;
	delete settings;
	mmsAccounts.Close();

	theUtils->Complete();
	}


void TestNotificationL()
	{
	theUtils->Start(_L("Notification Test1: Create"));		
	notification->StartCreateTestL();
	CActiveScheduler::Start();	
	theUtils->Complete();	

	theUtils->Start(_L("Notification Test2: Set"));		
	notification->StartSetTestL();
	CActiveScheduler::Start();	
	theUtils->Complete();	
	
	theUtils->Start(_L("Notification Test3: Delete"));		
	notification->StartDeleteTestL();
	CActiveScheduler::Start();	
	theUtils->Complete();	
	}

