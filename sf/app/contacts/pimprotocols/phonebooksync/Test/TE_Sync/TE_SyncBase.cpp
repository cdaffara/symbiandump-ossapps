// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <c32comm.h>
#include <f32file.h>

#include <e32base.h>
#include <e32test.h>
#include <e32svr.h>
#include <e32property.h> 

#include "TE_SyncBase.h"
#include "TE_Sync.h"

const	TInt KTestStringSize=30;
typedef TBuf<KTestStringSize> TTestString;


_LIT(KInternationalPrefix,"+");


TVerdict CSyncTestStep::doTestStepPreambleL()
	{
	__UHEAP_MARK;

	iScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	
	iSecureBackupEngine = CSBEClient::NewL();

	//
	// Connect to PhBkSync...
	//
	TInt  ret = iSession.Connect();
	TESTCHECKL(ret, KErrNone);

	//
	// Perform a sync to start the tests in a clean state. We also wait for the
	// slower TSYs to get ready!
	//
	RPhoneBookSession::TSyncState  syncState(RPhoneBookSession::EUnsynchronised);
	
	ret = iSession.GetPhoneBookCacheState(syncState);

	if (ret != KErrNone  ||  syncState != RPhoneBookSession::ECacheValid)
		{
		TRequestStatus  status(KErrUnknown);

		while (status.Int() != KErrNone)
			{
			iSession.DoSynchronisation(status);
			User::WaitForRequest(status);

			if (status.Int() == KErrServerTerminated)
				{
				INFO_PRINTF1(_L("PhBkSync has terminated, restarting..."));

				iSession.Close();
				ret = iSession.Connect();
				TESTCHECKL(ret, KErrNone);

				User::After(5*1000000);
				}
			else if (status.Int() != KErrNone)
				{
				INFO_PRINTF2(_L("Preamble sync result was %d so retrying..."), status.Int());
				User::After(5*1000000);
				}
			}
		TESTCHECKL(status.Int(), KErrNone);
	
		//
		// For MMTSY testing to pass, ensure that the equivilant entries
		// from SIMTSY are on the SIM (slots 1 to 20 excluding 18 and 19)...
		//
		RArray<TInt>  freeSlotArray;
		CleanupClosePushL(freeSlotArray);
		
		iSession.GetFreeSlotsL(freeSlotArray);

		for (TInt count = 1;  count <= 20;  count++)
			{
			if (freeSlotArray.Find(count) != KErrNotFound  &&
			    count != 18  &&  count != 19)
				{
				TBuf<20>  telName;
				TBuf<20>  telNumber;

				telName.AppendFormat(_L("Friend %d"), count);
				telNumber.AppendFormat(_L("12345000%03d"), count);

				INFO_PRINTF4(_L("Preamble filling contact {\"%S\", \"%S\"} to slot %d..."),
				             &telName, &telNumber, count);
				
				TContactICCEntry  iccEntry;
				TRequestStatus  status;

				iccEntry.iName.Copy(telName);
				iccEntry.iNumber.Copy(telNumber);
				iccEntry.iSlotNum    = count;
				iccEntry.iContactUID = KNullContactId;
				iccEntry.iTON        = RMobilePhone::EUnknownNumber;

				WriteContactToICCL(iccEntry, status);
				TESTCHECKL(status.Int(), KErrNone);
				TESTCHECKCONDITION(iccEntry.iSlotNum != KSyncIndexNotSupplied);
				}
			}

		CleanupStack::PopAndDestroy(&freeSlotArray);
		}

	return TestStepResult();
	} // CSyncTestStep::doTestStepPreambleL


TVerdict CSyncTestStep::doTestStepPostambleL()
	{
	iSession.Close();
	
	delete iSecureBackupEngine;
	iSecureBackupEngine = NULL;
	
	delete iScheduler;
	iScheduler = NULL;

	__UHEAP_MARKEND;
	
	return TestStepResult();
	} // CSyncTestStep::doTestStepPostambleL


/**
 	Instruct the PhBkSync server to configure to a 'Full' state
 	by using the Secure Backup interface. This will cause the
 	server to reconnect to ETel and SIMTSY.
 */
void CSyncTestStep::ConfigurePhbkSyncToFullL()
	{
	//
	// Ensure that the Secure Backup system does not currently think
	// it is performing a backup or restore.
	//
	iSecureBackupEngine->SetBURModeL(TDriveList(_L8("C")),
									 EBURNormal, ENoBackup);

	//
	// Connect to the server to ensure that the server is running and
	// responding.
	//
	RPhoneBookSession  phoneBookSession;
	RPhoneBookSession::TPhonebookSyncMode  dummySyncMode;
	TInt  ret;
	
	ret = phoneBookSession.Connect();
	TESTCHECKL(ret, KErrNone);

	ret = KErrNotReady;
	while (ret != KErrNone)
		{
		ret = phoneBookSession.GetSyncMode(dummySyncMode);
		if (ret != KErrNone)
			{
			User::After(1);
			}
		}

	phoneBookSession.Close();
	}


/**
 	Instruct the PhBkSync server to unconfigure to an 'Idle' state
 	by using the Secure Backup interface. This will cause the
 	server to disconnect from ETel and SIMTSY. This is effectively
 	the same as shutting down the server, but can be performed on
 	release versions.
 */
void CSyncTestStep::ConfigurePhbkSyncToIdleL()
	{
	//
	// Connect to the server to ensure that the server is running.
	//
	RPhoneBookSession  phoneBookSession;
	TInt ret;
	
	ret = phoneBookSession.Connect();
	TESTCHECKL(ret, KErrNone);

	//
	// Ensure that the Secure Backup system does not currently think
	// it is performing a backup or restore.
	//
	iSecureBackupEngine->SetBURModeL(TDriveList(_L8("C")),
									 EBURBackupFull, EBackupBase);

	//
	// Check that the server is responding.
	//
	RPhoneBookSession::TPhonebookSyncMode  dummySyncMode;

	ret = KErrNone;
	while (ret == KErrNone)
		{
		ret = phoneBookSession.GetSyncMode(dummySyncMode);
		if (ret == KErrNone)
			{
			User::After(1);
			}
		}

	phoneBookSession.Close();
	}
	

void CSyncTestStep::AddTextFieldToIccContactL(TStorageType aType, TFieldType aFieldType, TUid aMapping, TDesC& aField, CContactICCEntry* aIccEntry)
/**
 * This method adds a new text field (aField) to the CContactICCEntry supplied by aIccEntry. 
 */ 
	{
	CContactItemFieldSet& fieldSet = aIccEntry->CardFields();
	TInt pos = fieldSet.Find(aFieldType); //,aMapping
	if (pos!=KErrNotFound)
		{
		CContactItemField& field=fieldSet[pos];
		field.TextStorage()->SetTextL(aField);
		}
	else
		{
		CContactItemField* field=CContactItemField::NewLC(aType, aFieldType);
		field->SetMapping(aMapping);
		field->AddFieldTypeL(aFieldType);
		field->TextStorage()->SetTextL(aField);
		aIccEntry->AddFieldL(*field);
		CleanupStack::Pop(); // field	
		}
	}

void CSyncTestStep::WriteContactToICCL(TContactICCEntry& aIccEntry, TRequestStatus& aStatus)
/**
 * This method writes the phonebook data passed in the aIccEntry parameter to the ICC.
 */
	{
	CContactDatabase *ContactsDb=NULL;
	TRAPD(err,ContactsDb=CContactDatabase::OpenL());	// First try to open existing database
	if (err != KErrNone)
		{
		TRAP(err,CContactDatabase::CreateL()); // There is no existing database, so create default one
		if (err == KErrNone)
			ContactsDb=CContactDatabase::OpenL();			// Open new database
		}
	CleanupStack::PushL(ContactsDb);
	
	TContactItemId id = ContactsDb->ICCTemplateIdL(); 
	CContactItem* iccTemplate = ContactsDb->ReadContactL(id);
	CleanupStack::PushL(iccTemplate);
	CContactICCEntry* item;
	// If an overwrite is being attempted then ensure an entry with relevant UID is used
	if (aIccEntry.iContactUID > 0)
		{
		TContactItemId realId(GetValidUIDFromContactsDbL());
		item = static_cast<CContactICCEntry*> (ContactsDb->ReadContactL(realId));
		}
	else
		{
		item = CContactICCEntry::NewL(*iccTemplate); // Create an ICC entry
		}
	CleanupStack::PopAndDestroy(iccTemplate);
	CleanupStack::PushL(item);

	// Create phone number field 
	TBuf<RMobilePhone::KMaxMobileTelNumberSize> number;
	if(aIccEntry.iTON==RMobilePhone::EInternationalNumber)
		number.Append(KInternationalPrefix); // Append '+' prefix if International Number
	number.Append(aIccEntry.iNumber);
	AddTextFieldToIccContactL(KStorageTypeText, KUidContactFieldPhoneNumber, KUidContactFieldVCardMapTEL, number, item);

	// Create name field
	TBuf<TContactICCEntry::KMaxPhBkNameSize> name;
	name.Copy(aIccEntry.iName);
	AddTextFieldToIccContactL(KStorageTypeText, KUidContactFieldFamilyName, KUidContactFieldVCardMapUnusedN, name, item);

	if(aIccEntry.iSlotNum!=KSyncIndexNotSupplied) // it is not a new entry so slot number is supplied
		{
		// Create slot number field
		HBufC* buf = HBufC::NewL(5);
		TPtr indexPtr(buf->Des());
		indexPtr.AppendNum(aIccEntry.iSlotNum);
		TPtrC indexPtrC = indexPtr.Mid(0);
		AddTextFieldToIccContactL(KStorageTypeText, KUidContactFieldICCSlot, KUidContactFieldVCardMapNotRequired, indexPtrC, item);
		delete buf;
		}

	// Send a Write-Contact-ICC request
	TRequestStatus status;
	iSession.WriteContact(status, *item, aIccEntry.iSlotNum); 
	User::WaitForRequest(status);
	aStatus = status;
	CleanupStack::PopAndDestroy(2);	// item and ContactsDb
	}

CSyncTestStep::TContactICCEntry::TContactICCEntry()
: iSlotNum(0), iTON(RMobilePhone::EUnknownNumber), iContactUID(KNullContactId)
	{
	}

void CSyncTestStep::TContactICCEntry::Reset()
/*
 * This method resets the current ICC entry values. 
 */
	{
	iSlotNum=0;
	iTON=RMobilePhone::EUnknownNumber;
	iName.FillZ();
	iName.Zero();
	iNumber.FillZ();
	iNumber.Zero();
	iContactUID = KNullContactId;
	}

/**
 * To return a valid contact item id.
 */
TContactItemId CSyncTestStep::GetValidUIDFromContactsDbL()
	{
	TContactItemId firstId(KNullContactId);

	CContactDatabase *iDb = NULL;
	TRAPD(err,iDb = CContactDatabase::OpenL()); // open existing database
	CleanupStack::PushL(iDb);
	if (err != KErrNone)
		{
		CleanupStack::PopAndDestroy(); // iDb
		return firstId;
		}

	iDb->SetDbViewContactType(KUidContactICCEntry);    
    
	// to get the unique groupId for the phonebook
	TContactItemId groupId(KNullContactId);
	User::LeaveIfError(iSession.GetPhoneBookId(groupId, RPhoneBookSession::ESyncGroupId));
	TESTCHECKCONDITIONL(groupId != KNullContactId);
  
	// based on the groupId, get an item belonging to the phonebook
	CContactGroup* group = static_cast<CContactGroup*>(iDb->ReadContactLC(groupId));
	const CContactIdArray* array = group->ItemsContained();
	TInt count(array->Count());
	if (count > 0)
		firstId = (*array)[0];

	CleanupStack::PopAndDestroy(group);	
	CleanupStack::PopAndDestroy(); // iDb
	return firstId;
	}

