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
// Test programme for Phonebook Buffers
// 
//

#include "TE_Sync.h"
#include <e32std.h>
#include <e32base.h>
#include <e32test.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <etelmm.h>
#include <c32root.h>
#include <e32property.h>

_LIT16(KTel,"1234567890");
_LIT16(KText,"TelNo 567");
_LIT16(KTelOverwrite,"07700900000");
_LIT16(KTextOverwrite,"Overwrite 1");
_LIT16(KTelNoSlot,"+442079460001");
_LIT16(KTextNoSlot,"No slot");
_LIT16(KTelWithLetters,"5678943Abc34");
_LIT16(KTelExceedMax,"12345678901234567890123456789");
_LIT16(KNameNormal,"Exceed");
_LIT16(KTextExceedMax,"MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName 1123");//244 long
_LIT16(KTelNormal,"07700900002");
_LIT16(KTelMax,"123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789");
_LIT16(KEntryMaxName,"MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName MaximName ");//230 long
_LIT16(KTextMaxNum,"Max Number");
_LIT16(KTextNonAlphabet,"(:,)=(),");
_LIT16(KTelLookUp,"561234567890");
_LIT16(KTextLookUp,"Look Up");
_LIT(KInternationalPrefix,"+");

#include <f32file.h>
#include <f32ver.h>
#include <f32dbg.h>
#include <e32cons.h>
#include <e32twin.h>

#include <cntsync.h>


CTestOpen::CTestOpen() 
	{
	SetTestStepName(_L("TestOpen"));
	}

enum TVerdict CTestOpen::doTestStepL()
/**
	Connection and disconnection test.
    @return TVerdict result of running the test.
  */
	{
	//
	// Close the session...
	//
	iSession.Close();

	//
	// Delete PhBkSync INI file and Contacts DB...
	//
	ConfigurePhbkSyncToIdleL();

	RFs fs;
	User::LeaveIfError(fs.Connect()); // Connect to File server
	CleanupClosePushL(fs);
	_LIT(KPhbkSyncIniFile,"C:\\Private\\102030A1\\Phbksync.ini");
	TInt err(fs.Delete(KPhbkSyncIniFile)); // Delete PhBkSync.INI file
	TESTCHECKCONDITION(err == KErrNone  ||  err==KErrNotFound  ||  err==KErrPathNotFound); // Deleted successfully OR no default INI file
	TRAP(err,CContactDatabase::DeleteDefaultFileL()); // Delete default ContactsDb 
	TESTCHECKCONDITION(err == KErrNone  ||  err == KErrNotFound); // Deleted successfully OR no default ContactsDb  
	CleanupStack::PopAndDestroy(&fs);

	ConfigurePhbkSyncToFullL();

	//
	// Connect to PhBkSync Server...
	//
	TInt ret=iSession.Connect();
	TESTCHECKL(ret, KErrNone);

	return TestStepResult();
	}


CTestDoSynchronisation::CTestDoSynchronisation() 
	{
	SetTestStepName(_L("TestDoSynchronisation"));
	}

enum TVerdict CTestDoSynchronisation::doTestStepL()
/**
	Do Synchronisation test.
    @return TVerdict result of running the test.
  */
	{
	TRequestStatus status;
	iSession.DoSynchronisation(status); // Send a Read-Contact-ICC request
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);

	return TestStepResult();
	}

CTestDeleteContactFromICC::CTestDeleteContactFromICC() 
	{
	SetTestStepName(_L("TestDeleteContactFromICC"));
	}

enum TVerdict CTestDeleteContactFromICC::doTestStepL()
/**
	Delete Contact from ICC test.
    @return TVerdict result of running the test.
  */
	{
	TRequestStatus status;
	TContactItemId id(GetValidUIDFromContactsDbL());	
	iSession.DeleteContact(status, id); 	// Delete the contact 
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	return TestStepResult();
	}

CTestDeleteContactInvalidIdFromICC::CTestDeleteContactInvalidIdFromICC() 
	{
	SetTestStepName(_L("TestDeleteContactInvalidIdFromICC"));
	}

enum TVerdict CTestDeleteContactInvalidIdFromICC::doTestStepL()
/**
	Delete Contact with invalid ID from ICC test.
    @return TVerdict result of running the test.
  */
	{
	TRequestStatus status;
	TContactItemId id(7777);	
	iSession.DeleteContact(status, id); 	// Delete the contact 
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrArgument);
	return TestStepResult();
	}

CTestValidateContact::CTestValidateContact() 
	{
	SetTestStepName(_L("TestValidateContact"));
	}

enum TVerdict CTestValidateContact::doTestStepL()
/**
	Validate Contact test.
    @return TVerdict result of running the test.
  */
	{
	TContactItemId id(GetValidUIDFromContactsDbL());
	TInt ret(iSession.ValidateContact(MContactSynchroniser::ERead,id));
	TESTCHECKL(ret, KErrNone);
	id = GetValidUIDFromContactsDbL();
	ret = iSession.ValidateContact(MContactSynchroniser::EEdit,id);
	TESTCHECKL(ret, KErrNone);
	id = GetValidUIDFromContactsDbL();
	ret = iSession.ValidateContact(MContactSynchroniser::ESearch,id);
	TESTCHECKL(ret, KErrNone);
	return TestStepResult();
	}

CTestValidateContactAccessLockedTest::CTestValidateContactAccessLockedTest() 
	{
	SetTestStepName(_L("TestValidateContactAccessLockedTest"));
	}

enum TVerdict CTestValidateContactAccessLockedTest::doTestStepL()
/**
	Validate Contact - ICC PIN locked test.
    @return TVerdict result of running the test.
  */
	{
	TContactItemId id(GetValidUIDFromContactsDbL());
	TInt ret(iSession.ValidateContact(MContactSynchroniser::ERead,id));
	TESTCHECKCONDITIONL(ret == KErrAccessDenied  ||  ret == KErrNone );
	return TestStepResult();
	}

CTestValidateContactInvalidUIDTest::CTestValidateContactInvalidUIDTest() 
	{
	SetTestStepName(_L("TestValidateContactInvalidUIDTest"));
	}

enum TVerdict CTestValidateContactInvalidUIDTest::doTestStepL()
/**
	Validate Contact - Invalid UID test.
    @return TVerdict result of running the test.
  */
	{
	TInt ret(iSession.ValidateContact(MContactSynchroniser::ERead,-1));
	TESTCHECKL(ret, KErrNotFound);
	ret = iSession.ValidateContact(MContactSynchroniser::EEdit,8888);
	TESTCHECKL(ret, KErrNotFound);
	ret = iSession.ValidateContact(MContactSynchroniser::ESearch,99999);
	TESTCHECKL(ret, KErrNotFound);
	return TestStepResult();
	}

CTestGetSyncMode::CTestGetSyncMode() 
	{
	SetTestStepName(_L("TestGetSyncMode"));
	}

enum TVerdict CTestGetSyncMode::doTestStepL()
/**
	Get Sync Mode test.
    @return TVerdict result of running the test.
  */
	{
	RPhoneBookSession::TPhonebookSyncMode syncMode; // Get current sync mode
	iSession.GetSyncMode(syncMode);
	TESTCHECKL(syncMode, RPhoneBookSession::EManual);
	return TestStepResult();
	}

CTestSetSyncMode::CTestSetSyncMode() 
	{
	SetTestStepName(_L("TestSetSyncMode"));
	}

enum TVerdict CTestSetSyncMode::doTestStepL()
/**
	Set Sync Mode - Auto/Same/Manual test.
    @return TVerdict result of running the test.
  */
	{
	// EAutoCurrentIcc
	RPhoneBookSession::TPhonebookSyncMode syncMode; // sync mode
	syncMode = RPhoneBookSession::EAutoCurrentIcc;
	iSession.SetSyncMode(syncMode);		// Set sync mode
	syncMode = RPhoneBookSession::EManual; // reset sync mode to something else just in case
	iSession.GetSyncMode(syncMode);
	TESTCHECKL(syncMode, RPhoneBookSession::EAutoCurrentIcc);

	// EAutoSameIcc
	syncMode = RPhoneBookSession::EAutoSameIcc;
	iSession.SetSyncMode(syncMode);		// Set sync mode
	syncMode = RPhoneBookSession::EManual; // reset sync mode to something else just in case
	iSession.GetSyncMode(syncMode);
	TESTCHECKL(syncMode, RPhoneBookSession::EAutoSameIcc);

	// EManual
	syncMode = RPhoneBookSession::EManual;
	iSession.SetSyncMode(syncMode);		// Set sync mode
	syncMode = RPhoneBookSession::EAutoSameIcc; // reset sync mode to something else just in case
	iSession.GetSyncMode(syncMode);
	TESTCHECKL(syncMode, RPhoneBookSession::EManual);
	return TestStepResult();
	}

CTestUpdateLookUpTableFailure::CTestUpdateLookUpTableFailure() 
	{
	SetTestStepName(_L("TestUpdateLookUpTableFailure"));
	}

enum TVerdict CTestUpdateLookUpTableFailure::doTestStepL()
/**
	Update look-up table - Failure test.
    @return TVerdict result of running the test.
  */
	{
	TContactItemId uid(100);
	TInt slot(200);
	TInt ret(iSession.UpdateLookuptable(uid, slot)); // Failure test cases, so make up some slot numbers
	TESTCHECKL(ret, KErrNotFound);
	return TestStepResult();
	}

CTestUpdateLookUpTableSuccess::CTestUpdateLookUpTableSuccess() 
	{
	SetTestStepName(_L("TestUpdateLookUpTableSuccess"));
	}

enum TVerdict CTestUpdateLookUpTableSuccess::doTestStepL()
/**
	Update look-up table - Success test.
    @return TVerdict result of running the test.
  */
	{
	// First populate the name and number fields - Synchroniser Internal Format
	TContactICCEntry iccEntry;
	iccEntry.iName.Format(KTextLookUp); // Supplied
	iccEntry.iNumber.Format(KTelLookUp); // Supplied
	iccEntry.iSlotNum=-1; // This is only for testing purposes, to indicate that it is a new entry
	iccEntry.iContactUID=KNullContactId; // Only for testing purposes, reset Contact UID value
	iccEntry.iTON=RMobilePhone::EUnknownNumber;

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
	
	CContactICCEntry* item = CContactICCEntry::NewL(*iccTemplate); // Create an ICC entry
	CleanupStack::PopAndDestroy(iccTemplate);
	CleanupStack::PushL(item);

	// Create phone number field 
	TBuf<RMobilePhone::KMaxMobileTelNumberSize> number;
	if(iccEntry.iTON==RMobilePhone::EInternationalNumber)
		number.Append(KInternationalPrefix); // Append '+' prefix if International Number
	number.Append(iccEntry.iNumber);
	AddTextFieldToIccContactL(KStorageTypeText, KUidContactFieldPhoneNumber, KUidContactFieldVCardMapTEL, number, item);

	// Create name field
	TBuf<TContactICCEntry::KMaxPhBkNameSize> name;
	name.Copy(iccEntry.iName);
	AddTextFieldToIccContactL(KStorageTypeText, KUidContactFieldFamilyName, KUidContactFieldVCardMapUnusedN, name, item);

	if(iccEntry.iSlotNum!=KSyncIndexNotSupplied) // it is not a new entry so slot number is supplied
		{
		// Create slot number field
		HBufC* buf = HBufC::NewL(5);
		TPtr indexPtr(buf->Des());
		indexPtr.AppendNum(iccEntry.iSlotNum);
		TPtrC indexPtrC = indexPtr.Mid(0);
		AddTextFieldToIccContactL(KStorageTypeText, KUidContactFieldICCSlot, KUidContactFieldVCardMapNotRequired, indexPtrC, item);
		delete buf;
		}

	// Send a Write-Contact-ICC request
	TRequestStatus status;

	iSession.WriteContact(status, *item, iccEntry.iSlotNum); 
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	TESTCHECKCONDITION(iccEntry.iSlotNum != KSyncIndexNotSupplied); // Make sure we received new slot number

	TContactItemId lookupUID =  78; // Make up a Contacts UID value
	TInt ret(iSession.UpdateLookuptable(lookupUID, iccEntry.iSlotNum)); // New entry successully written
																 // so try to update look-up table
	TESTCHECK(ret, KErrNone);
	
	CleanupStack::PopAndDestroy(2);	// item and ContactsDb
	return TestStepResult();
	}

CTestGetContactFormat::CTestGetContactFormat() 
	{
	SetTestStepName(_L("TestGetContactFormat"));
	}

enum TVerdict CTestGetContactFormat::doTestStepL()
/**
	Get Contact Format test.  This method gets the current contact format.
	The expected output test results may not be valid for different
	ICCs/Phones.
  */
	{
	RPhoneBookSession::TContactFieldsV1 contactFormat;
	RPhoneBookSession::TContactFieldsV1Pckg contactFormatPckg(contactFormat);
	iSession.GetContactFormat(contactFormatPckg); // Get contact format
	TESTCHECK(contactFormat.iNameField.iCount, 1);
	TESTCHECKCONDITION(contactFormat.iNameField.iDisplayed);
	TESTCHECK(contactFormat.iNumberField.iCount, 1);
	TESTCHECKCONDITION(contactFormat.iNumberField.iDisplayed);
	TESTCHECK(contactFormat.iIccSlotField.iLength, -1);
	TESTCHECK(contactFormat.iIccSlotField.iCount, 1);
	TESTCHECKCONDITION(contactFormat.iIccSlotField.iDisplayed);
	return TestStepResult();
	}

CTestGetPhoneBookCacheState::CTestGetPhoneBookCacheState() 
	{
	SetTestStepName(_L("TestGetPhoneBookCacheState"));
	}

enum TVerdict CTestGetPhoneBookCacheState::doTestStepL()
/**
	Get Cache state test.
  */
	{
	RPhoneBookSession::TSyncState cacheState;
	TInt ret(iSession.GetPhoneBookCacheState(cacheState)); // Get phone book Cache state
	TESTCHECKL(ret, KErrNone);
	TESTCHECKL(cacheState, RPhoneBookSession::ECacheValid);
	return TestStepResult();
	}

CTestGetLastSyncError::CTestGetLastSyncError()
	{
	SetTestStepName(_L("TestGetLastSyncError"));
	}

enum TVerdict CTestGetLastSyncError::doTestStepL()
/**
	GetLastSyncError test.
    @return TVerdict result of running the test.
  */
	{
	TInt lastError;
	TInt ret(iSession.GetLastSyncError(lastError)); // Get last phone book Cache error
	TESTCHECKL(ret, KErrNone);
	TESTCHECKL(lastError, KErrNone);
	return TestStepResult();
	}

CTestNotifyCacheStateChange::CTestNotifyCacheStateChange() 
	{
	SetTestStepName(_L("TestNotifyCacheStateChange"));
	}

enum TVerdict CTestNotifyCacheStateChange::doTestStepL()
/**
	tNotifyCacheStateChange Test.
    @return TVerdict result of running the test.
  */
	{
 	iSession.SetSyncMode(RPhoneBookSession::EManual);
	TRequestStatus status, status2;
	RPhoneBookSession::TSyncState cacheState;
	iSession.NotifyPhBkCacheStateChange(status);
	iSession.DoSynchronisation(status2);

	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);

	iSession.NotifyPhBkCacheStateChange(status);
	TInt ret(iSession.GetPhoneBookCacheState(cacheState)); // Get phone book Cache state
	TESTCHECKL(ret, KErrNone);
	if (cacheState==RPhoneBookSession::ECacheValid)
		{
		iSession.CancelRequest(ESyncNotifyCacheStateChangeCancelClient);
		User::WaitForRequest(status);
		TESTCHECKL(status2.Int(), KErrNone);//the sync has completed
		User::WaitForRequest(status2);
		}
	else
		{
		User::WaitForRequest(status);
		User::WaitForRequest(status2);
		iSession.GetPhoneBookCacheState(cacheState);
		TESTCHECKL(cacheState, RPhoneBookSession::ECacheValid);
		}
	return TestStepResult();
	}

CTestGetPhoneBookId::CTestGetPhoneBookId() 
	{
	SetTestStepName(_L("TestGetPhoneBookId"));
	}

enum TVerdict CTestGetPhoneBookId::doTestStepL()
/**
	Get PhoneBook Id Test.
    @return TVerdict result of running the test.
  */
	{
	TContactItemId uid(KErrNotFound);
	TInt ret(iSession.GetPhoneBookId(uid, RPhoneBookSession::ESyncGroupId)); // Group ID
	TESTCHECKL(ret, KErrNone);
	TESTCHECKCONDITION(uid != KErrNotFound); // Make sure we received valid values
	ret = iSession.GetPhoneBookId(uid, RPhoneBookSession::ESyncTemplateId); // Template ID
	TESTCHECKL(ret, KErrNone);
	TESTCHECKCONDITION(uid != KErrNotFound); // Make sure we received valid values
	return TestStepResult();
	}

CTestReset::CTestReset() 
	{
	SetTestStepName(_L("TestReset"));
	}

enum TVerdict CTestReset::doTestStepL()
/**
	Reset test.
    @return TVerdict result of running the test.
  */
	{
	TContactICCEntry i;
	i.Reset();
	return TestStepResult();
	}

CTestVersionTest::CTestVersionTest() 
	{
	SetTestStepName(_L("TestVersionTest"));
	}

enum TVerdict CTestVersionTest::doTestStepL()
/**
	Test Version.
    @return TVerdict result of running the test.
  */
	{
	TVersion version;
	version = iSession.Version();
	INFO_PRINTF4(_L("The version number of PhBkSync is %d.%d.%d"),
                 version.iMajor, version.iMinor, version.iBuild);

	TESTCHECK(version.iMajor, 1);
	TESTCHECK(version.iMinor, 2);
	TESTCHECK(version.iBuild, 1);

	return TestStepResult();
	}

CTestCancelDoSyncRequest::CTestCancelDoSyncRequest() 
	{
	SetTestStepName(_L("TestCancelDoSyncRequest"));
	}

enum TVerdict CTestCancelDoSyncRequest::doTestStepL()
/**
	Cancel DoSync request test.
    @return TVerdict result of running the test.
  */
	{
	TRequestStatus status;
	iSession.DoSynchronisation(status); // DoSync first, since EManual is set by default
	iSession.CancelRequest(EDoSynchronisationCancelClient);
	User::WaitForRequest(status);
	TESTCHECKCONDITION(status.Int() == KErrNone  ||  status.Int() == KErrCancel);

	iSession.CancelRequest(EDoSynchronisationCancelClient);	// Now try to cancel DoSynchronisation request
	return TestStepResult();
	}

CTestCancelDeleteRequest::CTestCancelDeleteRequest() 
	{
	SetTestStepName(_L("TestCancelDeleteRequest"));
	}

enum TVerdict CTestCancelDeleteRequest::doTestStepL()
/**
	Cancel Delete request test.
    @return TVerdict result of running the test.
  */
	{
	TRequestStatus status;
	TContactItemId id(GetValidUIDFromContactsDbL());	
	iSession.DeleteContact(status, id); 	// Delete a contact 
	iSession.CancelRequest(ESyncDeleteCntFromICCCancelClient); // Cancel outstanding delete request
	User::WaitForRequest(status);
	TESTCHECKCONDITION(status.Int() == KErrNone  ||  status.Int() == KErrCancel);
	
	iSession.CancelRequest(ESyncDeleteCntFromICCCancelClient);	// Now try to cancel Delete request
														// with no outstanding delete request
	return TestStepResult();
	}

CTestCancelWriteRequest::CTestCancelWriteRequest() 
	{
	SetTestStepName(_L("TestCancelWriteRequest"));
	}

enum TVerdict CTestCancelWriteRequest::doTestStepL()
/**
	Cancel Write request test.
    @return TVerdict result of running the test.
  */
	{
	// First populate the name and number fields - Synchroniser Internal Format
	TContactICCEntry iccEntry;
	iccEntry.iName.Format(KText); // Supplied
	iccEntry.iNumber.Format(KTel); // Supplied
	iccEntry.iSlotNum=-1; // This is only for testing purposes, to indicate that it is a new entry
	iccEntry.iContactUID=1; // This is usually generated by the Contacts Model, but since we are not using 
							// the contacts model to write this entry we will have to make up a number.
	iccEntry.iTON=RMobilePhone::EUnknownNumber;

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
	
	CContactICCEntry* item = CContactICCEntry::NewL(*iccTemplate); // Create an ICC entry
	CleanupStack::PopAndDestroy(iccTemplate);
	CleanupStack::PushL(item);

	// Create phone number field 
	TBuf<RMobilePhone::KMaxMobileTelNumberSize> number;
	if(iccEntry.iTON==RMobilePhone::EInternationalNumber)
		number.Append(KInternationalPrefix); // Append '+' prefix if International Number
	number.Append(iccEntry.iNumber);
	AddTextFieldToIccContactL(KStorageTypeText, KUidContactFieldPhoneNumber, KUidContactFieldVCardMapTEL, number, item);

	// Create name field
	TBuf<TContactICCEntry::KMaxPhBkNameSize> name;
	name.Copy(iccEntry.iName);
	AddTextFieldToIccContactL(KStorageTypeText, KUidContactFieldFamilyName, KUidContactFieldVCardMapUnusedN, name, item);

	if(iccEntry.iSlotNum!=KSyncIndexNotSupplied) // it is not a new entry so slot number is supplied
		{
		// Create slot number field
		HBufC* buf = HBufC::NewL(5);
		TPtr indexPtr(buf->Des());
		indexPtr.AppendNum(iccEntry.iSlotNum);
		TPtrC indexPtrC = indexPtr.Mid(0);
		AddTextFieldToIccContactL(KStorageTypeText, KUidContactFieldICCSlot, KUidContactFieldVCardMapNotRequired, indexPtrC, item);
		delete buf;
		}

	// Send a Write-Contact-ICC request
	TRequestStatus status;

	iSession.WriteContact(status, *item, iccEntry.iSlotNum); 
	iSession.CancelRequest(ESyncWriteCntToICCCancelClient);
	User::WaitForRequest(status);
	TESTCHECKCONDITION(status.Int() == KErrNone  ||  status.Int() == KErrCancel);

	iSession.CancelRequest(ESyncWriteCntToICCCancelClient);	// Now try to cancel Write request
														// with no outstanding delete request
	CleanupStack::PopAndDestroy(item);
	CleanupStack::PopAndDestroy(ContactsDb);
	return TestStepResult();
	}

CTestWriteContactToICCTest::CTestWriteContactToICCTest() 
	{
	SetTestStepName(_L("TestWriteContactToICCTest"));
	}

enum TVerdict CTestWriteContactToICCTest::doTestStepL()
/**
	Write Contact to ICC test.
    @return TVerdict result of running the test.
  */
	{
	// First populate the name and number fields - Synchroniser Internal Format
	TContactICCEntry iccEntry;
	TRequestStatus status;
	iccEntry.iName.Format(KText); // Supplied
	iccEntry.iNumber.Format(KTel); // Supplied
	iccEntry.iSlotNum=-1; // This is only for testing purposes, to indicate that it is a new entry
	iccEntry.iContactUID=1; // This is usually generated by the Contacts Model, but since we are not using 
							// the contacts model to write this entry we will have to make up a number.
	iccEntry.iTON=RMobilePhone::EUnknownNumber;

	WriteContactToICCL(iccEntry, status); // Write an entry to ICC
	TESTCHECKL(status.Int(), KErrNone);
	TESTCHECKCONDITION(iccEntry.iSlotNum != KSyncIndexNotSupplied); // Make sure we receive a value

	return TestStepResult();
	}

CTestWriteContactToICCOverwriteTest::CTestWriteContactToICCOverwriteTest() 
	{
	SetTestStepName(_L("TestWriteContactToICCOverwriteTest"));
	}

enum TVerdict CTestWriteContactToICCOverwriteTest::doTestStepL()
/**
	Write Contact to ICC test - Overwrite existing entry.
    @return TVerdict result of running the test.
  */
	{
	// First populate the name and number fields - Synchroniser Internal Format
	TContactICCEntry iccEntry;
	TRequestStatus status;
	iccEntry.iName.Format(KTextOverwrite); // Supplied
	iccEntry.iNumber.Format(KTelOverwrite); // Supplied
	iccEntry.iSlotNum=10; // Index for this entry is supplied
	iccEntry.iContactUID=1;
	iccEntry.iTON=RMobilePhone::EUnknownNumber;

	WriteContactToICCL(iccEntry, status); // Write an entry to ICC
	TESTCHECKL(status.Int(), KErrNone);
	TESTCHECK(iccEntry.iSlotNum, 10); // Make sure we receive a value

	return TestStepResult();
	}

CTestWriteContactToICCNoSlotTest::CTestWriteContactToICCNoSlotTest() 
	{
	SetTestStepName(_L("TestWriteContactToICCNoSlotTest"));
	}

enum TVerdict CTestWriteContactToICCNoSlotTest::doTestStepL()
/**
	Local function that creates a CTestData object. This method writes a
    new entry to the ICC.  The index field is not specified.
    @return TVerdict result of running the test.
  */
	{
	// First populate the name and number fields - Synchroniser Internal Format
	TRequestStatus status;
	TContactICCEntry iccEntry;
	iccEntry.iName.Format(KTextNoSlot); // Supplied
	iccEntry.iNumber.Format(KTelNoSlot); // Supplied
	iccEntry.iSlotNum=-1; // This is only for testing purposes, to indicate that it is a new entry
	iccEntry.iContactUID=2;
	iccEntry.iTON=RMobilePhone::EInternationalNumber;

	WriteContactToICCL(iccEntry, status); // Write an entry to ICC
	TESTCHECKL(status.Int(), KErrNone);
	TESTCHECKCONDITION(iccEntry.iSlotNum != KSyncIndexNotSupplied); // Make sure we receive a value

	return TestStepResult();
	}

CTestWriteContactToICCInvalidSlotTest::CTestWriteContactToICCInvalidSlotTest() 
	{
	SetTestStepName(_L("TestWriteContactToICCInvalidSlotTest"));
	}

enum TVerdict CTestWriteContactToICCInvalidSlotTest::doTestStepL()
/**
	Write Contact to ICC - Invalid Slot number test.
    @return TVerdict result of running the test.
  */
	{
	// First populate the name and number fields - Synchroniser Internal Format
	TRequestStatus status;
	TContactICCEntry iccEntry;
	iccEntry.iName.Format(KText); // Supplied
	iccEntry.iNumber.Format(KTel); // Supplied
	iccEntry.iSlotNum=230; // Invalid slot number
	iccEntry.iContactUID=3;
	iccEntry.iTON=RMobilePhone::EUnknownNumber;

	WriteContactToICCL(iccEntry, status); // Write an entry to ICC
	TESTCHECKL(status.Int(), KErrArgument);
	TESTCHECK(iccEntry.iSlotNum, 230); // Make sure slot number has not changed

	return TestStepResult();
	}

CTestWriteContactToICCBlankNumTest::CTestWriteContactToICCBlankNumTest() 
	{
	SetTestStepName(_L("TestWriteContactToICCBlankNumTest"));
	}

enum TVerdict CTestWriteContactToICCBlankNumTest::doTestStepL()
/**
	Write Contact to ICC - Blank Number test.
    @return TVerdict result of running the test.
  */
	{
	// First populate the name and number fields - Synchroniser Internal Format
	TRequestStatus status;
	TContactICCEntry iccEntry;
	iccEntry.iName.Format(KText);
	iccEntry.iSlotNum=-1; // This is only for testing purposes, to indicate that it is a new entry
	iccEntry.iContactUID=4;
	iccEntry.iTON=RMobilePhone::EUnknownNumber;

	WriteContactToICCL(iccEntry, status); // Write an entry to ICC
	TESTCHECKL(status.Int(), KErrNone);
	TESTCHECKCONDITION(iccEntry.iSlotNum != KSyncIndexNotSupplied); // Make sure we receive a value

	return TestStepResult();
	}

CTestWriteContactToICCBlankNameTest::CTestWriteContactToICCBlankNameTest() 
	{
	SetTestStepName(_L("TestWriteContactToICCBlankNameTest"));
	}

enum TVerdict CTestWriteContactToICCBlankNameTest::doTestStepL()
/**
	Write Contact to ICC - Blank Name test.
    @return TVerdict result of running the test.
  */
	{
	// First populate the name and number fields - Synchroniser Internal Format
	TRequestStatus status;
	TContactICCEntry iccEntry;
	iccEntry.iNumber.Format(KTel);
	iccEntry.iSlotNum=-1; // This is only for testing purposes, to indicate that it is a new entry
	iccEntry.iContactUID=5;
	iccEntry.iTON=RMobilePhone::EUnknownNumber;

	WriteContactToICCL(iccEntry, status); // Write an entry to ICC
//	TESTCHECKL(status.Int(), KErrNone); // Commented out as this is applicable to a Nokia phone only
//	TESTCONDITION(iccEntry.iSlotNum != KSyncIndexNotSupplied); // Make sure we receive a value - Nokia phone only

	return TestStepResult();
	}

CTestWriteContactToICCExceedMaxNameTest::CTestWriteContactToICCExceedMaxNameTest() 
	{
	SetTestStepName(_L("TestWriteContactToICCExceedMaxNameTest"));
	}

enum TVerdict CTestWriteContactToICCExceedMaxNameTest::doTestStepL()
/**
	Write Contact to ICC - Exceed Max Name test.
    @return TVerdict result of running the test.
  */
	{
	// First populate the name and number fields - Synchroniser Internal Format
	TRequestStatus status;
	TContactICCEntry iccEntry;
	iccEntry.iName.Format(KTextExceedMax); // Exceeds max num of characters
	iccEntry.iNumber.Format(KTelNormal); // Matching name which does not exceed max number of characters
	iccEntry.iSlotNum=-1; // This is only for testing purposes, to indicate that it is a new entry
	iccEntry.iContactUID=6;
	iccEntry.iTON=RMobilePhone::EUnknownNumber;

	WriteContactToICCL(iccEntry, status); // Write an entry to ICC
	TESTCHECKCONDITIONL(status.Int() == KErrGeneral  ||  status.Int() == KErrOverflow);
	TESTCHECK(iccEntry.iSlotNum, KSyncIndexNotSupplied); // No slot number assigned for this entry

	return TestStepResult();
	}

CTestWriteContactToICCExceedMaxNumTest::CTestWriteContactToICCExceedMaxNumTest() 
	{
	SetTestStepName(_L("TestWriteContactToICCExceedMaxNumTest"));
	}

enum TVerdict CTestWriteContactToICCExceedMaxNumTest::doTestStepL()
/**
	Write Contact to ICC - Exceed Max Number test.
    @return TVerdict result of running the test.
  */
	{
	// First populate the name and number fields - Synchroniser Internal Format
	TContactICCEntry iccEntry;
	TRequestStatus status;
	iccEntry.iName.Format(KNameNormal); // Matching name which does not exceed max number of characters
	iccEntry.iNumber.Format(KTelExceedMax); // Exceeds max num of characters
	iccEntry.iSlotNum=-1; // This is only for testing purposes, to indicate that it is a new entry
	iccEntry.iContactUID=7;
	iccEntry.iTON=RMobilePhone::EUnknownNumber;

	WriteContactToICCL(iccEntry, status); // Write an entry to ICC

	return TestStepResult();
	}

CTestWriteContactToICCMaxNumTest::CTestWriteContactToICCMaxNumTest() 
	{
	SetTestStepName(_L("TestWriteContactToICCMaxNumTest"));
	}

enum TVerdict CTestWriteContactToICCMaxNumTest::doTestStepL()
/**
	Write Contact to ICC - Max Number test.
    @return TVerdict result of running the test.
  */
	{
	// First populate the name and number fields - Synchroniser Internal Format
	TContactICCEntry iccEntry;
	TRequestStatus status;
	iccEntry.iName.Format(KTextMaxNum);
	iccEntry.iNumber.Format(KTelMax); // Supplied
	iccEntry.iSlotNum=-1; // This is only for testing purposes, to indicate that it is a new entry
	iccEntry.iContactUID=8;
	iccEntry.iTON=RMobilePhone::EUnknownNumber;

	WriteContactToICCL(iccEntry, status); // Write an entry to ICC
	if ((status.Int() != KErrGeneral) && (status.Int() != KErrOverflow)) // MMTSY returns KErrGeneral for this
		{
		TESTCHECKL(status.Int(), KErrNone);
		TESTCHECKCONDITION(iccEntry.iSlotNum != KSyncIndexNotSupplied); // Make sure we receive a value
		}

	return TestStepResult();
	}

CTestWriteContactToICCMaxNameTest::CTestWriteContactToICCMaxNameTest() 
	{
	SetTestStepName(_L("TestWriteContactToICCMaxNameTest"));
	}

enum TVerdict CTestWriteContactToICCMaxNameTest::doTestStepL()
/**
	Write Contact to ICC - Max Name test.
    @return TVerdict result of running the test.
  */
	{
	// First populate the name and number fields - Synchroniser Internal Format
	TContactICCEntry iccEntry;
	TRequestStatus status;
	iccEntry.iName.Format(KEntryMaxName); // Supplied
	iccEntry.iNumber.Format(KTel);
	iccEntry.iSlotNum=-1; // This is only for testing purposes, to indicate that it is a new entry
	iccEntry.iContactUID=9;
	iccEntry.iTON=RMobilePhone::EUnknownNumber;

	WriteContactToICCL(iccEntry, status); // Write an entry to ICC
	if (status.Int() != KErrOverflow)
		{
		TESTCHECKL(status.Int(), KErrNone);
		TESTCHECKCONDITION(iccEntry.iSlotNum != KSyncIndexNotSupplied); // Make sure we receive a value
		}

	return TestStepResult();
	}

CTestWriteContactToICCNumLettersTest::CTestWriteContactToICCNumLettersTest() 
	{
	SetTestStepName(_L("TestWriteContactToICCNumLettersTest"));
	}

enum TVerdict CTestWriteContactToICCNumLettersTest::doTestStepL()
/**
	Write Contact to ICC - Number with letters test.
    @return TVerdict result of running the test.
  */
	{
	// First populate the name and number fields - Synchroniser Internal Format
	TContactICCEntry iccEntry;
	TRequestStatus status;
	iccEntry.iName.Format(KText); // Supplied
	iccEntry.iNumber.Format(KTelWithLetters);
	iccEntry.iSlotNum=-1; // This is only for testing purposes, to indicate that it is a new entry
	iccEntry.iContactUID=10;
	iccEntry.iTON=RMobilePhone::EUnknownNumber;

	WriteContactToICCL(iccEntry, status); // Write an entry to ICC
	TESTCHECKCONDITIONL(status.Int() == KErrGeneral  ||  status.Int() == KErrNone);
	TESTCHECKCONDITION(iccEntry.iSlotNum == KSyncIndexNotSupplied  ||  iccEntry.iSlotNum == 18); // No slot number allocated for this entry

	return TestStepResult();
	}

CTestWriteContactToICCObsecureTest::CTestWriteContactToICCObsecureTest() 
	{
	SetTestStepName(_L("TestWriteContactToICCObsecureTest"));
	}

enum TVerdict CTestWriteContactToICCObsecureTest::doTestStepL()
/**
	Write Contact to ICC - Name with non-alphabetical characters test.
    @return TVerdict result of running the test.
  */
	{
	// First populate the name and number fields - Synchroniser Internal Format
	TContactICCEntry iccEntry;
	TRequestStatus status;
	iccEntry.iName.Format(KTextNonAlphabet);
	iccEntry.iNumber.Format(KTel);
//	iccEntry.iNumber.Format(KTelWithpw);
	iccEntry.iSlotNum=-1; // This is only for testing purposes, to indicate that it is a new entry
	iccEntry.iContactUID=11;
	iccEntry.iTON=RMobilePhone::EUnknownNumber;

	WriteContactToICCL(iccEntry, status); // Write an entry to ICC
	TESTCHECKL(status.Int(), KErrNone);
	TESTCHECKCONDITION(iccEntry.iSlotNum != KSyncIndexNotSupplied); // Make sure we receive a value

	return TestStepResult();
	}

CTestSlotHandlingTest::CTestSlotHandlingTest() 
	{
	SetTestStepName(_L("TestSlotHandlingTest"));
	}

enum TVerdict CTestSlotHandlingTest::doTestStepL()
/**
	Slot Handling Test.
    @return TVerdict result of running the test.
  */
	{
	TInt aSlot;
	TContactItemId aId=KNullContactId;
	TInt numSlots; 
	TInt err = iSession.GetNumSlots(numSlots);
	TESTCHECKL(err, KErrNone);
	TESTCHECKL(numSlots, 200);

	RArray<TInt> freeSlotArray;
	CleanupClosePushL(freeSlotArray);
	
	iSession.GetFreeSlotsL(freeSlotArray);

	TInt FirstUsedSlot;
	TBool free(EFalse);
	 

	TInt i;
	for (FirstUsedSlot=1; FirstUsedSlot<numSlots; FirstUsedSlot++)
		{
		free=EFalse;
		for (i=0;i<freeSlotArray.Count();i++)
			{
			if (FirstUsedSlot==freeSlotArray[i])
				{
				free=ETrue;
				break;
				}
			}
		if(free==EFalse)
			{
			break;
			}
		}
	
	err = iSession.GetSlotId(FirstUsedSlot, aId);
	TESTCHECKL(err, KErrNone);
	TESTCHECKCONDITIONL(aId > KNullContactId);
	
	aSlot = freeSlotArray[0];
	err = iSession.GetSlotId(aSlot, aId);
	TESTCHECKL(err, KErrNotFound);

	CleanupStack::PopAndDestroy(&freeSlotArray);

	return TestStepResult();
	}

CTestWriteToSlotTest::CTestWriteToSlotTest() 
	{
	SetTestStepName(_L("TestWriteToSlotTest"));
	}

enum TVerdict CTestWriteToSlotTest::doTestStepL()
/**
	Write To Slot Test.
    @return TVerdict result of running the test.
  */
	{
	TInt aSlot;
	TContactItemId aId=KNullContactId;
	TInt numSlots; 
	TInt err = iSession.GetNumSlots(numSlots);
	TESTCHECKL(err, KErrNone);
	TESTCHECKL(numSlots, 200);

	RArray<TInt> freeSlotArray;
	CleanupClosePushL(freeSlotArray);

	iSession.GetFreeSlotsL(freeSlotArray);

	//TInt i;
	//for (i=0; i<freeSlotArray.Count(); i++)
	//	{
	//	Log(_L("%d"),freeSlotArray[i]);
	//	}
	//Log(_L(""));
	
	TContactICCEntry iccEntry;
	TRequestStatus status;
	iccEntry.iName.Format(KText); // Supplied
	iccEntry.iNumber.Format(KTel); // Supplied
	iccEntry.iSlotNum=-1; 
	iccEntry.iContactUID=15; // This is usually generated by the Contacts Model, but since we are not using 
							// the contacts model to write this entry we will have to make up a number.
	iccEntry.iTON=RMobilePhone::EUnknownNumber;

	WriteContactToICCL(iccEntry, status); // Write an entry to ICC
	TESTCHECKL(status.Int(), KErrNone);

	iSession.GetFreeSlotsL(freeSlotArray);
	//TInt j;
	//for (j=0; j<freeSlotArray.Count(); j++)
	//	{
	//	Log(_L("%d"),freeSlotArray[j]);
	//	}
	//Log(_L(""));

	aSlot=freeSlotArray[0];
	err = iSession.GetSlotId(aSlot, aId);
	TESTCHECKL(err, KErrNotFound);

	//Add Entry at slot
	iccEntry.iContactUID=15;
	iccEntry.iSlotNum=25;//now the slot is specified
	WriteContactToICCL(iccEntry, status); // Write an entry to ICC
	TESTCHECKL(status.Int(), KErrNone);

	iSession.GetFreeSlotsL(freeSlotArray);
	//TInt jj;
	//for (jj=0; jj<freeSlotArray.Count(); jj++)
	//	{
	//	Log(_L("%d"),freeSlotArray[jj]);
	//	}
	//Log(_L(""));
	
	aSlot=25;
	err = iSession.GetSlotId(aSlot, aId);
	TESTCHECKL(err, KErrNone);

	//Edit entry same slot //
/*	iccEntry.iSlotNum=25;
	iccEntry.iName.Format(KTextOverwrite);
	WriteContactToICCL(iccEntry, status);
	for (i=0; i<freeSlotArray.Count(); i++)
		{
		Log(_L("%d"),freeSlotArray[i]);
		}
	Log(_L(""));
	TESTCHECKCONDITIONL(aId > KNullContactId);
*/
	
	CleanupStack::PopAndDestroy(&freeSlotArray);

	return TestStepResult();
	}

CTestConnectionTest::CTestConnectionTest() 
	{
	SetTestStepName(_L("TestConnectionTest"));
	} // CTestConnectionTest::CTestConnectionTest


/**
 *  Connection and disconnection test.
 *
 *  @return TVerdict result of running the test.
 */
enum TVerdict CTestConnectionTest::doTestStepL()
	{
	//
	// Connect to PhBkSync Server and wait 30 seconds...
	//
	RPhoneBookSession sessionTest;

	TInt ret(sessionTest.Connect()); // this one should start the server 
	TESTCHECK(ret, KErrNone);
	User::After(30*1000000);
	sessionTest.Close();

	return TestStepResult();
	} // CTestConnectionTest::doTestStepL


CTestFailedPhoneStartup::CTestFailedPhoneStartup() 
	{
	SetTestStepName(_L("TestFailedPhoneStartup"));
	} // CTestFailedPhoneStartup::CTestFailedPhoneStartup


/**
 *  Connection and disconnection test.
 *
 *  @return TVerdict result of running the test.
 */
enum TVerdict CTestFailedPhoneStartup::doTestStepL()
	{
	//
	// Shutdown the server if possible (only works if the server is in
	// debug mode).
	//
	iSession.ShutdownServer(EFalse);
	iSession.Close();

	//
	// Delete the SIMTSY config file so that the server cannot startup...
	//
	RFs  fs;
	TInt  deleteResult;

	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	deleteResult = fs.Delete(KConfigFilename);
	TESTCHECKCONDITION(deleteResult == KErrNone  ||  deleteResult == KErrNotFound);

	CleanupStack::PopAndDestroy(&fs);

	//
	// Connect to PhBkSync Server (restarting it). The next result will vary
	// depending on whether the server is a UDeb or URel version.
	//
	// For UDeb the shutdown and reconnect will exercise the defect scenario
	// of the TSY not opening. Initially (now that the server configures
	// asynchronously) this will succeed but this fail.
	//
	// For a URel server the shutdown will not have happened and the delete
	// of the config.txt would not be noticed. Therefore the reconnect would
	// succeed with no problems.
	//
	TInt  ret;

	ret = iSession.Connect();
#ifdef _DEBUG
	TESTCHECK(ret, KErrNone);
#else
	TESTCHECK(ret, KErrNone);
#endif

	//
	// Test if the server managed to configure successfully...
	//
	RPhoneBookSession::TPhonebookSyncMode  syncMode;

	ret = iSession.GetSyncMode(syncMode);
#ifdef _DEBUG
	TESTCHECK(ret, KErrServerTerminated);
#else
	TESTCHECK(ret, KErrNone);
#endif

	return TestStepResult();
	} // CTestFailedPhoneStartup::doTestStepL

