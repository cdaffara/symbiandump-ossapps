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

#include "TE_PhBkSyncBase.h"

// all test step header files
#include "TestOpenSteps.h"
#include "TestConnectSteps.h"
#include "TestDeleteContactSteps.h"
#include "TestDoSynchronisationSteps.h"
#include "TestGetContactFormatSteps.h"
#include "TestGetSyncModeSteps.h"
#include "TestOtherSteps.h"
#include "TestSetSyncModeSteps.h"
#include "TestValidateContactSteps.h"
#include "TestVersionSteps.h"
#include "TestWriteContactSteps.h"
#include "TestReadingIniFile.h"



CTestStepPhbkSync::CTestStepPhbkSync()
	{

	iUidIccGlobalAdnPhonebookCallByRef = KUidIccGlobalAdnPhonebook;
	iUidIccGlobalSdnPhonebookCallByRef = KUidIccGlobalSdnPhonebook;
	iUidIccGlobalLndPhonebookCallByRef = KUidIccGlobalLndPhonebook;
	iUidUsimAppAdnPhonebookCallByRef = KUidUsimAppAdnPhonebook;
	iUidIccGlobalFdnPhonebookCallByRef = KUidIccGlobalFdnPhonebook;

	iPhoneBook[0] = KUidIccGlobalAdnPhonebook;
	iPhoneBook[1] = KUidIccGlobalSdnPhonebook;
	iPhoneBook[2] = KUidIccGlobalLndPhonebook;
	iPhoneBook[3] = KUidUsimAppAdnPhonebook;
	iPhoneBook[4] = KUidIccGlobalFdnPhonebook;
	}


/**
 *  Deletes the PhBkSync INI file.
 */
void CTestStepPhbkSync::DestroyDefaultIniFileL()
	{
	// Delete contacts database and phonebook sync ini file 
	RFs fs;
	User::LeaveIfError(fs.Connect()); // Connect to File server
	CleanupClosePushL(fs);
	TInt err(fs.Delete(KPhbkSyncIniFile)); // Delete PhBkSync.INI file
	if(!(err == KErrNone  ||  err == KErrNotFound  ||  err == KErrPathNotFound)) // Deleted successfully OR no default INI file
		User::Leave(err);
	CleanupStack::PopAndDestroy(); // fs
	}


/**
 *  Deletes the Contacts Database and the PhBkSync INI file.
 */
void CTestStepPhbkSync::DestroyDefaultFilesL()
	{
	// Delete contacts database and phonebook sync ini file 
	RFs fs;
	User::LeaveIfError(fs.Connect()); // Connect to File server
	TInt err(fs.Delete(KPhbkSyncIniFile)); // Delete PhBkSync.INI fileNI file
	if(!(err == KErrNone  ||  err == KErrNotFound ||  err == KErrPathNotFound  )) // Deleted successfully OR no default INI file
		User::Leave(err);
	TRAP(err,CContactDatabase::DeleteDefaultFileL()); // Delete default ContactsDb 
	if(!(err == KErrNone  ||  err == KErrNotFound)) // Deleted successfully OR no default INI file
		User::Leave(err);
	}


TVerdict CTestStepPhbkSync::doTestStepPreambleL()
	{
	__UHEAP_MARK;

	iScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);

	iSecureBackupEngine = CSBEClient::NewL();

	TInt ret = iSession.Connect();
	TESTCHECKL(ret, KErrNone);

	return TestStepResult();
	}

TVerdict CTestStepPhbkSync::doTestStepPostambleL()
	{
	iSession.Close();
	
	delete iSecureBackupEngine;
	iSecureBackupEngine = NULL;
	
	delete iScheduler;
	iScheduler = NULL;
	
	__UHEAP_MARKEND;

	return TestStepResult();
	}

/** 
 * Change the synchronisation mode for the given phone book.
 */
void CTestStepPhbkSync::ChangeSyncModeL(RPhoneBookSession::TPhonebookSyncMode aMode, TUid aPhonebookUid)
	{
	User::LeaveIfError(iSession.SetSyncMode(aMode, aPhonebookUid));
	}

/** 
 * Check the synchronisation mode is the same as aMode for given phone book.
 */
void CTestStepPhbkSync::CheckSyncModeL(RPhoneBookSession::TPhonebookSyncMode aMode, TUid aPhonebookUid)
	{
	RPhoneBookSession::TPhonebookSyncMode mode;
	User::LeaveIfError(iSession.GetSyncMode(mode, aPhonebookUid));
	TESTCHECK(mode, aMode);
	}

void CTestStepPhbkSync::CheckCacheStateValidL(TUid aPhonebookUid)
	{
	RPhoneBookSession::TSyncState state;
	TInt  ret;
	
	ret = iSession.GetPhoneBookCacheState(state, aPhonebookUid);
	while (ret != KErrNone  ||  state == RPhoneBookSession::EUnsynchronised) 
		{
		User::After(KOneSeconduS);  // Wait until cache becomes usable 

		ret = iSession.GetPhoneBookCacheState(state, aPhonebookUid);
		}

	// Cache now in valid state 
	TESTCHECKL(state, RPhoneBookSession::ECacheValid);
	}

void CTestStepPhbkSync::CheckCacheStateInvalidL(TUid aPhonebookUid)
	{
	User::After(3000000); // Delay is needed to prove that synchronisation has not been carried out 
	RPhoneBookSession::TSyncState state;
	TInt  ret;
	
	ret = iSession.GetPhoneBookCacheState(state, aPhonebookUid);
	if (ret == KErrNone)
		{
		TESTCHECKL(state, RPhoneBookSession::EUnsynchronised);
		}
	}

void CTestStepPhbkSync::CheckCacheStateErrorL(TUid aPhonebookUid)
	{
	User::After(3000000); // Delay is needed to prove that synchronisation has not been carried out 
	RPhoneBookSession::TSyncState state;
	TInt  ret;
	
	ret = iSession.GetPhoneBookCacheState(state, aPhonebookUid);
	if (ret == KErrNone)
		{
		TESTCHECKL(state, RPhoneBookSession::EErrorDuringSync);
		}
	}

void CTestStepPhbkSync::WaitForIccToBecomeLockedL()
	{
	TContactItemId validContactId;
	validContactId = GetValidUIDFromContactsDbL(KUidIccGlobalAdnPhonebook);
	TESTCHECKCONDITIONL(validContactId != KNullContactId);

	// Wait for ICC to become locked 
	TInt result = KErrNone;
	result = iSession.ValidateContact(MContactSynchroniser::ERead, validContactId);
	while (result != KErrAccessDenied) 
		{
		result = iSession.ValidateContact(MContactSynchroniser::ERead, validContactId);
		User::After(1000000);
		}
	TESTCHECKL(result, KErrAccessDenied);
	}

/** 
 * Convert the given phonebookUid into its corresponding EnumId used 
 * used by phonebook internally.
 */
MyCSyncContactICCEntry::TSyncPhoneBook CTestStepPhbkSync::convertPhonebookUid2Enum(TUid aPhonebookUid)
	{
	if (aPhonebookUid == KUidIccGlobalAdnPhonebook)
		return MyCSyncContactICCEntry::EIccGlobalAdnPhonebook;
	else if (aPhonebookUid == KUidIccGlobalSdnPhonebook)
		return MyCSyncContactICCEntry::EIccGlobalSdnPhonebook;
	else if (aPhonebookUid == KUidIccGlobalLndPhonebook)
	    return MyCSyncContactICCEntry::EIccGlobalLndPhonebook;
	else if (aPhonebookUid == KUidUsimAppAdnPhonebook)
		return MyCSyncContactICCEntry::EUsimAppAdnPhonebook;
	else if (aPhonebookUid == KUidIccGlobalFdnPhonebook)
		return MyCSyncContactICCEntry::EIccGlobalFdnPhonebook;
	else
		return MyCSyncContactICCEntry::EIccGlobalAdnPhonebook;
	}

void CTestStepPhbkSync::DoWriteContactToICCL(MyCSyncContactICCEntry* aIccEntryInternal,
					     TUid&		      aPhonebook,
					     TRequestStatus&	      aStatus,
					     RPhoneBookSession* aSession) 
{
	// to get the template associated with the given phonebook
	// Use the contacts model API to get the template ID rather than
	// the RPhonebookSession as this is how it is going to be done in reality 
	CContactDatabase *iDb = NULL;
	TRAPD(err,iDb = CContactDatabase::OpenL()); // open existing database
	CleanupStack::PushL(iDb);
	CContactItem* iccTemplate = NULL;

	TESTCHECKL(err, KErrNone);

	if (err == KErrNone)
		{
		TContactItemId templateId = iDb->ICCTemplateIdL(aPhonebook); 
		iccTemplate = iDb->ReadContactL(templateId);
		}
	CleanupStack::PopAndDestroy(); // iDb
	CleanupStack::PushL(iccTemplate); 

	// Create an ICC (exteranl format) entry
	CContactICCEntry* iccEntryExternal; 

	// If an overwrite is being attempted then ensure an entry with relevant UID is used
	if (aIccEntryInternal->iContactUID > 0)
		{
		TContactItemId realId(GetValidUIDFromContactsDbL(aPhonebook, EFalse));
		iccEntryExternal = static_cast<CContactICCEntry*> (iDb->ReadContactL(realId));
		}
	else
		{
		iccEntryExternal = CContactICCEntry::NewL(*iccTemplate); // Create an ICC entry
		}
	CleanupStack::PopAndDestroy(); // iccTemplate
	CleanupStack::PushL(iccEntryExternal);
	

	// Create phone number field 
	if(aIccEntryInternal->iNumber.Length() != KErrNone) 
		{
		TBuf<RMobilePhone::KMaxMobileTelNumberSize> number;
  
		if(aIccEntryInternal->iTON==RMobilePhone::EInternationalNumber) 
			{
			// Append '+' prefix if International Number
			number.Append(KInternationalPrefix); 
			}
		number.Append(aIccEntryInternal->iNumber);
		AddTextFieldToIccContactL(KStorageTypeText, 
					  KUidContactFieldPhoneNumber, 
					  KUidContactFieldVCardMapTEL, 
					  number, 
					  iccEntryExternal,
					  0); // 0 means the first instance
		}

	// Check whether this is hidden entry and set its hidden attributes 
	if(aIccEntryInternal->iIsHidden) 
		{
		iccEntryExternal->SetHidden(ETrue);
		}

	// Additional numbers 
	TInt count(aIccEntryInternal->iNumberList->Count());
	TInt i(0);
	for(i=0; i<count; ++i) 
		{
		MyCSyncContactICCEntry::TSyncAdditionalNumber additionalNum(aIccEntryInternal->iNumberList->At(i));

/*****
		// Add additional number alpha string
		TBuf<CSyncContactICCEntry::KMaxPhBkNameSize> numberString;
		numberString.Copy(additionalNum.iNumberString);
		AddTextFieldToIccContactL(KStorageTypeText, KUidContactFieldSecondName, KUidContactFieldVCardMapUnusedN, numberString, iccEntryExternal,i);
*****/
		// Actual number
		TBuf<RMobilePhone::KMaxMobileTelNumberSize> number;
		if(additionalNum.iTON==RMobilePhone::EInternationalNumber)
			{
			number.Append(KInternationalPrefix); // Append '+' prefix if International Number
			}
		number.Append(additionalNum.iNumber);
		AddTextFieldToIccContactL(KStorageTypeText, KUidContactFieldPhoneNumber, KUidContactFieldVCardMapTEL, number, iccEntryExternal,i+1);
		}

	// Create name field(s)
	count = aIccEntryInternal->iNameList->Count();
	for(i=0; i<count; ++i)
		{
		MyCSyncContactICCEntry::TSyncEntryName nameField;
		nameField.Copy(aIccEntryInternal->iNameList->At(i));
		// First name only is stored as Family Name
		if (i == 0)
			{
			AddTextFieldToIccContactL(KStorageTypeText, KUidContactFieldFamilyName, KUidContactFieldVCardMapUnusedN, nameField, iccEntryExternal, i);
			}
		else
			{
			AddTextFieldToIccContactL(KStorageTypeText, KUidContactFieldSecondName, KUidContactFieldVCardMapUnusedN, nameField, iccEntryExternal, i-1);
			}
		}

	// Create group field(s)
	count = aIccEntryInternal->iGroupList->Count();
	for(i=0; i<count; ++i) 
		{
		MyCSyncContactICCEntry::TSyncEntryName groupField;
		groupField.Copy(aIccEntryInternal->iGroupList->At(i));
		AddTextFieldToIccContactL(KStorageTypeText, KUidContactFieldICCGroup, KUidContactFieldVCardMapUnusedN, groupField, iccEntryExternal,i);
		}

	// Create e-mail field(s)
	count = aIccEntryInternal->iEmailList->Count();
	for(i=0; i<count; ++i) 
		{
		MyCSyncContactICCEntry::TSyncEntryName emailField;
		emailField.Copy(aIccEntryInternal->iEmailList->At(i));
		AddTextFieldToIccContactL(KStorageTypeText, KUidContactFieldEMail, KUidContactFieldVCardMapUnusedN, emailField, iccEntryExternal,i);
		}

	// Create slot number field
	if(aIccEntryInternal->iSlotNum!=KSyncIndexNotSupplied) // it is not a new entry so slot number is supplied
		{
		// Create slot number field
		HBufC* buf = HBufC::NewL(5);
		TPtr indexPtr(buf->Des());
		indexPtr.AppendNum(aIccEntryInternal->iSlotNum);
		TPtrC indexPtrC = indexPtr.Mid(0);
		AddTextFieldToIccContactL(KStorageTypeText, KUidContactFieldICCSlot, KUidContactFieldVCardMapNotRequired, indexPtrC, iccEntryExternal,0);
		delete buf;
		}

	// Create phonebook type field
	TBuf<KFieldLength> buf;
	buf.FillZ();
	buf.Zero();
	buf.AppendNum(static_cast<TInt>(aIccEntryInternal->iPhonebook)); 
	TPtrC phonebookPtrC(buf.Ptr());
	AddTextFieldToIccContactL(KStorageTypeText, KUidContactFieldICCPhonebook, KUidContactFieldVCardMapNotRequired, phonebookPtrC, iccEntryExternal,0);

	// Send a Write-Contact-ICC request
	// Here you need to pass an EXTERNAL format not INTERNAL as this is what 
	// the plug-in will pass to the server
	TInt iSlotNumCallByRef = aIccEntryInternal->iSlotNum;
	
	if (aSession != NULL)
		{
		aSession->WriteContact(aStatus, *iccEntryExternal, iSlotNumCallByRef, aPhonebook); 
		}
	else
		{
		iSession.WriteContact(aStatus, *iccEntryExternal, iSlotNumCallByRef, aPhonebook); 
		}
	User::WaitForRequest(aStatus);

	// if a new slot number is allocated, write it back to EXTERNAL object
	if (aIccEntryInternal->iSlotNum == KSyncIndexNotSupplied) 
		{ 
		// a new slot should be allocated and put into "iSlotNumCallByRef".
		// In case the phonebook is full, write will faile which is why a
		// TESTCHECKCODITION(iSlotNumCallByRef != KSyncIndexNotSupplied) is not done here.
		aIccEntryInternal->iSlotNum = iSlotNumCallByRef;
		}
	
	CleanupStack::PopAndDestroy();   // iccEntryExternal
	}

/** Do a manual synchronisation */
void CTestStepPhbkSync::DoSyncL(TUid aPhonebookUid, TInt aExpectedErr, TBool aIgnoreErr)
	{
	TRequestStatus status; 
	iSession.DoSynchronisation(status, aPhonebookUid);
	User::WaitForRequest(status);
	if (!aIgnoreErr) 
		{ 
		TESTCHECK(status.Int(), aExpectedErr);
		}
	else 
		{
		// to make sure the given error code != KErrNone
		// and simply ignore the returned error message
		TESTCHECKCONDITION(status.Int()!=KErrNone);
		}
	}

/** Do a manual synchronisation with cache state change notification*/
void CTestStepPhbkSync::DoSyncAndNotificationL(TUid aPhonebookUid, TInt aExpectedErr,
											   RPhoneBookSession::TSyncState aExpectedState,
											   TRequestStatus& aNotifyStatus, TBool aIgnoreErr)
	{
	TRequestStatus status; 
	iSession.DoSynchronisation(status, aPhonebookUid);
	User::WaitForRequest(aNotifyStatus);
	User::WaitForRequest(status);

	RPhoneBookSession::TSyncState state;
	iSession.GetPhoneBookCacheState(state, aPhonebookUid);
	if (!aIgnoreErr) 
		{ 
		TESTCHECKL(status.Int(), aExpectedErr);
		TESTCHECK(state, aExpectedState);
		}
	else 
		{
		// to make sure the given error code != KErrNone
		// and simply ignore the returned error message
		TESTCHECKCONDITIONL(status.Int() != KErrNone);
		}
	}


/**
 	Instruct the PhBkSync server to configure to a 'Full' state
 	by using the Secure Backup interface. This will cause the
 	server to reconnect to ETel and SIMTSY.
 */
void CTestStepPhbkSync::ConfigurePhbkSyncToFullL()
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
	TInt ret;

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
	} // CTestStepPhbkSync::ConfigurePhbkSyncToFullL


/**
 	Instruct the PhBkSync server to unconfigure to an 'Idle' state
 	by using the Secure Backup interface. This will cause the
 	server to disconnect from ETel and SIMTSY. This is effectively
 	the same as shutting down the server, but can be performed on
 	release versions.
 */
void CTestStepPhbkSync::ConfigurePhbkSyncToIdleL()
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
	} // CTestStepPhbkSync::ConfigurePhbkSyncToIdleL
	

/** 
 * Set the SIM TSY test number to run
 *
 */
void CTestStepPhbkSync::SetSimTsyTestNumberL(TInt aTestNumber, TBool aIgnoreErr)
	{
	RProperty testNumberProperty;
	User::LeaveIfError(testNumberProperty.Attach(KUidPSSimTsyCategory, KPSSimTsyTestNumber));
	CleanupClosePushL(testNumberProperty);

	TRequestStatus status;
	testNumberProperty.Subscribe(status);
	User::LeaveIfError(testNumberProperty.Set(KUidPSSimTsyCategory,KPSSimTsyTestNumber,aTestNumber));
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
	TInt testNumberCheck;
	User::LeaveIfError(testNumberProperty.Get(testNumberCheck));
	if (aTestNumber != testNumberCheck)
	{
		User::Leave(KErrNotFound);		
	}

	CleanupStack::PopAndDestroy(&testNumberProperty);
	
	iSession.Close();

	//
	// Ensure that the server resets its configuration....
	//
	ConfigurePhbkSyncToIdleL();
	ConfigurePhbkSyncToFullL();

	TInt ret = iSession.Connect();

	if (!aIgnoreErr)
		{
		User::LeaveIfError(ret);
		}
	else
		{
		TESTCHECKCONDITION(ret != KErrNone);
		}
		
	// Ensure that the test number has been set properly
	RProperty testNumberInUseProperty;
	User::LeaveIfError(testNumberInUseProperty.Attach(KUidPSSimTsyCategory, KPSSimTsyTestNumberInUse));
	CleanupClosePushL(testNumberInUseProperty);

	TInt testNumberInUseCheck;
	User::LeaveIfError(testNumberInUseProperty.Get(KUidPSSimTsyCategory, KPSSimTsyTestNumberInUse,testNumberInUseCheck));
	TESTCHECKL(aTestNumber,testNumberInUseCheck);

	CleanupStack::PopAndDestroy(&testNumberInUseProperty);
	}


/**
 *  Publish a new value to indicate that test has finished and simtsy timer
 *  can now proceed to the next timer point.
 */
void CTestStepPhbkSync::SignalSimTsyToReduceTimersL()
	{
	RProperty reduceTimersProperty;
	User::LeaveIfError(reduceTimersProperty.Attach(KUidPSSimTsyCategory, KPSSimTsyTimersReduceTime));
	CleanupClosePushL(reduceTimersProperty);

	User::LeaveIfError(reduceTimersProperty.Set(KUidPSSimTsyCategory, KPSSimTsyTimersReduceTime,
												KReduceSimTsyTimers));

	CleanupStack::PopAndDestroy(&reduceTimersProperty);
	} // CTestStepPhbkSync::SignalSimTsyToReduceTimersL


/**
 * To return a valid contact item id.
 */
TContactItemId CTestStepPhbkSync::GetValidUIDFromContactsDbL(TUid aPhonebookUid, TBool aHiddenRequired)
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
    
	// to get the unique groupId for the given phonebook
	TContactItemId groupId(KNullContactId);
	User::LeaveIfError(iSession.GetPhoneBookId(groupId, 
						RPhoneBookSession::ESyncGroupId,
						aPhonebookUid));
	TESTCHECKCONDITIONL(groupId != KNullContactId);
  
	// based on the groupId, get all iterms belonging to the phonebook
	CContactGroup* group = 
	  static_cast<CContactGroup*>(iDb->ReadContactLC(groupId));
	const CContactIdArray* array = group->ItemsContained();
	TInt count(array->Count());
	for(TInt i=0; i<count; ++i)
		{
		firstId = (*array)[i];
		CContactICCEntry* entry = static_cast<CContactICCEntry*>(iDb->ReadContactLC(firstId));
		if(((aHiddenRequired) && (entry->IsHidden())) || 
			((!aHiddenRequired) && (!entry->IsHidden())))
			{
			CleanupStack::PopAndDestroy(entry);
			break;
			}
		CleanupStack::PopAndDestroy(entry); 
		}

	CleanupStack::PopAndDestroy(group);	
	CleanupStack::PopAndDestroy(); // iDb
	return firstId;
	}

/**
 * To return a valid slot number.
 */
TInt CTestStepPhbkSync::GetValidSlotFromContactsDbL(TUid aPhonebookUid, TBool aHiddenRequired)
	{
	TInt slot(KSyncIndexNotSupplied);
	TContactItemId id = GetValidUIDFromContactsDbL(aPhonebookUid, aHiddenRequired);

	CContactDatabase *iDb = NULL;
	TRAPD(err,iDb = CContactDatabase::OpenL()); // open existing database
	CleanupStack::PushL(iDb);
	if (err != KErrNone)
		{
		CleanupStack::PopAndDestroy(); // iDb
		return slot;
		}

	iDb->SetDbViewContactType(KUidContactICCEntry);    
	CContactICCEntry* entry = static_cast<CContactICCEntry*>(iDb->ReadContactLC(id)); // down cast

	//set of fields for this entry
	CContactItemFieldSet& fieldSet = entry->CardFields();

	//get index of the icc slot field 
	TInt index = fieldSet.Find(KUidContactFieldICCSlot); 

	if(index!=KErrNotFound) 
		{ // index field provided
		TPtrC tempPtr(NULL,0);
		tempPtr.Set(fieldSet[index].TextStorage()->Text());
		TLex lex(tempPtr);
		User::LeaveIfError(lex.Val(slot));
		}

	CleanupStack::PopAndDestroy(2); // iDb and entry 
	return slot;
	}

void CTestStepPhbkSync::AddTextFieldToIccContactL(TStorageType		aType, 
												  TFieldType		aFieldType, 
												  TUid				aMapping, 
												  TDesC&			aField, 
												  CContactICCEntry* aIccEntry,
												  TInt aCount)
/**
 * Add a new text field (aField) to the CContactICCEntry supplied by aIccEntry. 
 * @param aType Field Storage type 
 * @param aFieldType Field type
 * @param aMapping Mapping for the field's content type
 * @param aField Field data
 * @param aIccEntry CContactICCEntry item
 * @param aCount Identifies which instance of field is to be used
 */ 
	{
	CContactItemFieldSet& fieldSet = aIccEntry->CardFields();
	TInt pos(KErrNotFound);
	for (TInt i = 0; i <= aCount; i++)
		{
		pos = fieldSet.FindNext(aFieldType, pos + 1);
		if (pos == KErrNotFound)
			break;
		}
	if (pos!=KErrNotFound) // Field already present. Note, Contacts model reads all fields
						   // in template and adds them as empty fields in ICC item
		{
		CContactItemField& field=fieldSet[pos];
		field.TextStorage()->SetTextL(aField);
		}
	else
		{
		CContactItemField* field=CContactItemField::NewLC(aType, aFieldType);
		field->SetMapping(aMapping);
		field->AddFieldTypeL(aFieldType); // Appends a field type to the field's content type
		field->TextStorage()->SetTextL(aField);
		aIccEntry->AddFieldL(*field);
		CleanupStack::Pop(); // field. No need to destroy it since contact item takes ownership of field
		}
	}


/**
 * A utility used to initialise the entry with (short) internal format,
 * The values of iSlotNum, iContactUID and iTON are set as:
 * iSlotNum = KSyncIndexNotSupplied
 * iContactUID = KNullContactId
 * iTON = EUnknownNumber
 */
void CTestStepPhbkSync::InitialiseIccEntryInternalOldL(MyCSyncContactICCEntry* aIccEntryInternal)
	{
	aIccEntryInternal->iNumber.Format(KTel);

	// leave the slot number unspecified on purpose 
	aIccEntryInternal->iSlotNum = KSyncIndexNotSupplied; 

	aIccEntryInternal->iContactUID=KNullContactId;
	aIccEntryInternal->iTON=RMobilePhone::EUnknownNumber;

	// initialise the (CSyncICCEntry - TContactICCEntry) attributes
	MyCSyncContactICCEntry::TSyncEntryName tName;
	tName.Copy(KName1);
	aIccEntryInternal->iNameList->AppendL(tName);
	}


/**
 * A utility used to initialise the entry with (long) internal format.
 * The values of iSlotNum, iContactUID and iTON are set as:
 * iSlotNum = KSyncIndexNotSupplied
 * iContactUID = KNullContactId
 * iTON = EUnknownNumber
 */
void CTestStepPhbkSync::InitialiseIccEntryInternalNewL(MyCSyncContactICCEntry* aIccEntryInternal)
	{
	aIccEntryInternal->iNumber.Format(KTel);

	// leave the slot number unspecified on purpose 
	aIccEntryInternal->iSlotNum = KSyncIndexNotSupplied; 

	aIccEntryInternal->iContactUID=KNullContactId;
	aIccEntryInternal->iTON=RMobilePhone::EUnknownNumber;

	// initialise the (CSyncICCEntry - TContactICCEntry) attributes
	// 1 iNameList
	MyCSyncContactICCEntry::TSyncEntryName tName;
	tName.Copy(KName1);
	aIccEntryInternal->iNameList->AppendL(tName);
	tName.Copy(KName2);
	aIccEntryInternal->iNameList->AppendL(tName);

	// 2. iNumberList (i.e. list of additional numbers)
	MyCSyncContactICCEntry::TSyncAdditionalNumber addNum;

	addNum.iNumber.Format(KAdditionalNumber1);
//	addNum.iNumberString.Copy(KAdditionalString1);
	addNum.iTON = RMobilePhone::EUnknownNumber;	  //KAdditionalTON1

	aIccEntryInternal->iNumberList->AppendL(addNum);

	addNum.iNumber.Format(KAdditionalNumber2);
//	addNum.iNumberString.Copy(KAdditionalString2);
	addNum.iTON = RMobilePhone::EUnknownNumber;	  //KAdditionalTON2

	aIccEntryInternal->iNumberList->AppendL(addNum);

	// 3.  iGrouplist
	tName.Copy(KGroup1);
	aIccEntryInternal->iGroupList->AppendL(tName);
	tName.Copy(KGroup2);
	aIccEntryInternal->iGroupList->AppendL(tName);
/*****
	tName.Copy(KGroup3);
	iccEntryInternal->iGroupList->AppendL(tName);
	tName.Copy(KGroup4);
	iccEntryInternal->iGroupList->AppendL(tName);
	tName.Copy(KGroup5);
	iccEntryInternal->iGroupList->AppendL(tName);
*****/

	// 4. iEmailList
	tName.Copy(KEmail1);
	aIccEntryInternal->iEmailList->AppendL(tName);
	tName.Copy(KEmail2);
	aIccEntryInternal->iEmailList->AppendL(tName);
/*****
	tName.Copy(KEmail3);
	iccEntryInternal->iEmailList->AppendL(tName);
	tName.Copy(KEmail4);
	iccEntryInternal->iEmailList->AppendL(tName);
	tName.Copy(KEmail5);
	iccEntryInternal->iEmailList->AppendL(tName);
*****/

	// 5. iPhonebook
	// set each phonebook TUid during the loop (see below)

	// 6. iIsHidden
	aIccEntryInternal->iIsHidden = EFalse;
	}

void CTestStepPhbkSync::InitialiseIccEntryInternalNewL(MyCSyncContactICCEntry& aIccEntryInternal)
	{
	InitialiseIccEntryInternalNewL(&aIccEntryInternal);
	}

//
//  code copied from phbkinternal.cpp  // 
//
MyCSyncContactICCEntry* MyCSyncContactICCEntry::NewL()
	{
	MyCSyncContactICCEntry* ptr = new (ELeave) MyCSyncContactICCEntry();
	CleanupStack::PushL(ptr);
	ptr->ConstructL();
	CleanupStack::Pop(); // ptr
	return ptr;
	}

MyCSyncContactICCEntry::MyCSyncContactICCEntry() : 
  iSlotNum(0), 
  iTON(RMobilePhone::EUnknownNumber), 
  iContactUID(KNullContactId),
  iPhonebook(EIccGlobalAdnPhonebook), 
  iIsHidden(EFalse), 
  iGroupId(KNullContactId)
	{
	}

void MyCSyncContactICCEntry::ConstructL()
	{
	iNameList = new (ELeave) CArrayFixFlat<TSyncEntryName>(KSyncArrayGranularity);
	iNumberList = new (ELeave) CArrayFixFlat<TSyncAdditionalNumber>(KSyncArrayGranularity);
	iGroupList = new (ELeave) CArrayFixFlat<TSyncEntryName>(KSyncArrayGranularity);
	iEmailList = new (ELeave) CArrayFixFlat<TSyncEntryName>(KSyncArrayGranularity);
	}

MyCSyncContactICCEntry::~MyCSyncContactICCEntry()
	{
	delete iNameList;
	delete iNumberList;
	delete iGroupList;
	delete iEmailList;
	}
