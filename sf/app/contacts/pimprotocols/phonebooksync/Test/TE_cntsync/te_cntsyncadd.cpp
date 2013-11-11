// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>

#include "te_cntsyncbase.h"
#include "te_cntsyncadd.h"
#include <phbksync.h>
#include <exterror.h>

_LIT(KTestEmpty,"");
_LIT(KBoundaryTestName,"B");
_LIT(KBoundaryTestNumber,"2");
_LIT(KTestCompany,"Company A");
_LIT(KTestNumberWithDTMF,"020788p98w8548");
_LIT(KTestAddNumber,"12345678");


/** 
 * Factory construction method.
 * @return Pointer to CPhbkAddICCContactTest object
 */
CPhbkAddICCContactTest* CPhbkAddICCContactTest::NewL()
	{
	CPhbkAddICCContactTest* self = new(ELeave) CPhbkAddICCContactTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkAddICCContactTest::CPhbkAddICCContactTest()
	{
	SetTestStepName(_L("AddICCContactTest"));
	}

/** Add an ICC entry to each of the supported phonebooks - normal case */
enum TVerdict CPhbkAddICCContactTest::doTestStepL()
	{
	SetSyncModeL(RPhoneBookSession::EManual);
	CheckSyncModeL(RPhoneBookSession::EManual);
	SetSimTsyTestNumberL(19);
	DoSyncL();

	AddContactL(KTestName,KTestNumber,KUidIccGlobalAdnPhonebook,KTestEMail);
	AddContactL(KTestName,KTestNumber,KUidIccGlobalSdnPhonebook);
	AddContactL(KTestName,KTestNumber,KUidIccGlobalLndPhonebook);
	AddContactL(KTestName,KTestNumber,KUidUsimAppAdnPhonebook,KTestEMail);
	AddContactL(KTestName,KTestNumber,KUidIccGlobalFdnPhonebook);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkAddEntryEmptyNameTest object
 */
CPhbkAddEntryEmptyNameTest* CPhbkAddEntryEmptyNameTest::NewL()
	{
	CPhbkAddEntryEmptyNameTest* self = new(ELeave) CPhbkAddEntryEmptyNameTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkAddEntryEmptyNameTest::CPhbkAddEntryEmptyNameTest()
	{
	SetTestStepName(_L("AddEntryEmptyNameTest"));
	}

/** Add an ICC entry to each of the supported phonebooks - name field is empty */
enum TVerdict CPhbkAddEntryEmptyNameTest::doTestStepL()
	{
	SetSimTsyTestNumberL(19);
	DoSyncL();

	TRAPD(err, AddContactL(KTestEmpty,KTestNumber,KUidIccGlobalAdnPhonebook));
	TESTCHECKL(err, KErrNone); 

	TRAP(err, AddContactL(KTestEmpty,KTestNumber,KUidIccGlobalSdnPhonebook));
	TESTCHECKL(err, KErrNone); 

	TRAP(err, AddContactL(KTestEmpty,KTestNumber,KUidIccGlobalLndPhonebook));
	TESTCHECKL(err, KErrNone); 

	TRAP(err, AddContactL(KTestEmpty,KTestNumber,KUidUsimAppAdnPhonebook,KTestEMail));
	TESTCHECKL(err, KErrNone); 

	TRAP(err, AddContactL(KTestEmpty,KTestNumber,KUidIccGlobalFdnPhonebook));
	TESTCHECKL(err, KErrNone); 

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkAddEntryEmptyNumberTest object
 */
CPhbkAddEntryEmptyNumberTest* CPhbkAddEntryEmptyNumberTest::NewL()
	{
	CPhbkAddEntryEmptyNumberTest* self = new(ELeave) CPhbkAddEntryEmptyNumberTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkAddEntryEmptyNumberTest::CPhbkAddEntryEmptyNumberTest()
	{
	SetTestStepName(_L("AddEntryEmptyNumberTest"));
	}

/** Add an ICC entry to each of the supported phonebooks - number field is empty */
enum TVerdict CPhbkAddEntryEmptyNumberTest::doTestStepL()
	{
	SetSimTsyTestNumberL(19);
	DoSyncL();

	TRAPD(err, AddContactL(KTestName,KTestEmpty,KUidIccGlobalAdnPhonebook));
	TESTCHECKL(err, KErrNone); 

	TRAP(err, AddContactL(KTestName,KTestEmpty,KUidIccGlobalSdnPhonebook));
	TESTCHECKL(err, KErrNone); 

	TRAP(err, AddContactL(KTestName,KTestEmpty,KUidIccGlobalLndPhonebook));
	TESTCHECKL(err, KErrNone); 

	TRAP(err, AddContactL(KTestName,KTestEmpty,KUidUsimAppAdnPhonebook,KTestEMail));
	TESTCHECKL(err, KErrNone); 

	TRAP(err, AddContactL(KTestName,KTestEmpty,KUidIccGlobalFdnPhonebook));
	TESTCHECKL(err, KErrNone); 

	return TestStepResult();
	}

/** 
 * Factory construction method.
 * @return Pointer to CPhbkAddEntryEmptyEmailTest object
 */
CPhbkAddEntryEmptyEmailTest* CPhbkAddEntryEmptyEmailTest::NewL()
	{
	CPhbkAddEntryEmptyEmailTest* self = new(ELeave) CPhbkAddEntryEmptyEmailTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkAddEntryEmptyEmailTest::CPhbkAddEntryEmptyEmailTest()
	{
	SetTestStepName(_L("AddEntryEmptyEmailTest"));
	}

/** Add an ICC entry to each of the supported phonebooks - first email field is empty, second one is valid */
enum TVerdict CPhbkAddEntryEmptyEmailTest::doTestStepL()
	{
	SetSyncModeL(RPhoneBookSession::EManual);
	CheckSyncModeL(RPhoneBookSession::EManual);

	TRequestStatus status;

	// Loading a bunch of entries from the SIM (one of them contains the dodgy entry)
	SetSimTsyTestNumberL(27);

	// Sync
	iSession.DoSynchronisation(status, KUidUsimAppAdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);

	// Resetting the phone 
	SetSimTsyTestNumberL(28);

	// Sync again
	iSession.DoSynchronisation(status, KUidUsimAppAdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);

	// look for contact in the synchronised database
	CContactItemFieldDef* fieldDef = new(ELeave) CContactItemFieldDef;
	CleanupStack::PushL(fieldDef);
	fieldDef->AppendL(KUidContactFieldEMail); // define subset of fields to search
	CContactIdArray* array = iDb->FindLC(KTestEMail,fieldDef);
	TESTCHECKL(array->Count(),1); 
	TContactItemId entryId = (*array)[0];

	// now read it back
	CContactItem* entryItem = iDb->ReadContactLC(entryId);
	CContactICCEntry* newEntry = CContactICCEntry::NewL(*entryItem);
	CleanupStack::PushL(newEntry);
	
	// now checking the entry
	CContactItemFieldSet& fieldset = newEntry->CardFields();
	CheckEmptyEmailFieldContentL(fieldset,KUidContactFieldEMail,KTestEMail);
	
	CleanupStack::PopAndDestroy(newEntry);
	CleanupStack::PopAndDestroy(entryItem);
	CleanupStack::PopAndDestroy(array);
	CleanupStack::PopAndDestroy(fieldDef);

	return TestStepResult();
	}

/** 
 * Check that the field of type aFieldType has the same content as 
 * aExpectedContent
 * @param aFieldset Contact item fieldset
 * @param aFieldType Field type to test
 * @param aExpectedContent Expected data
 */
void CPhbkAddIntegrationTestBase::CheckEmptyEmailFieldContentL(CContactItemFieldSet& aFieldset, TFieldType aFieldType, const TDesC& aExpectedContent)
	{
	// Look for up to two entries with that label
	TInt pos1 = aFieldset.FindNext(aFieldType,KContactFieldSetSearchAll);
	TInt pos2 = aFieldset.FindNext(aFieldType,pos1+1);
	
	TBool match1 = EFalse;
	TBool match2 = EFalse;

	// There must be exactly two email fields 
	TESTCHECKCONDITION((pos1!=KErrNotFound)&&(pos2!=KErrNotFound));

	// Checking the value of the first match
	if(pos1 != KErrNotFound)
		{
		CContactItemField& field1 = aFieldset[pos1];		
		CContactTextField* textField1 = field1.TextStorage();
		if(textField1 != NULL)
			{
			match1 = (textField1->Text() == aExpectedContent); // Flips to ETrue
			}
		}

	// Checking the value of the second match
	if(pos2 != KErrNotFound)
		{
		CContactItemField& field2 = aFieldset[pos2];		
		CContactTextField* textField2 = field2.TextStorage();
		if(textField2 != NULL)
			{
			match2 = (textField2->Text() == aExpectedContent); // Flips to ETrue
			}
		}

	// One (and only one) of the entries must match the expected value
	TESTCHECKCONDITIONL( (match1 && !match2) || (!match1 && match2) );
	}

/** 
 * Factory construction method.
 * @return Pointer to CPhbkAddPhonebookFullTest object
 */
CPhbkAddPhonebookFullTest* CPhbkAddPhonebookFullTest::NewL()
	{
	CPhbkAddPhonebookFullTest* self = new(ELeave) CPhbkAddPhonebookFullTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkAddPhonebookFullTest::CPhbkAddPhonebookFullTest()
	{
	SetTestStepName(_L("AddPhonebookFullTest"));
	}

/** Add an ICC entry to each of the supported phonebooks - fails because phonebook is full */
enum TVerdict CPhbkAddPhonebookFullTest::doTestStepL()
	{
	SetSimTsyTestNumberL(16);
	DoSyncL();

	TRAPD(error,AddContactL(KTestName,KTestNumber,KUidIccGlobalAdnPhonebook)); 
	TESTCHECKL(error, KErrNoMemory); // TSY reports "no free slots as KErrNoMemory"

	TRAP(error,AddContactL(KTestName,KTestNumber,KUidIccGlobalSdnPhonebook)); 
	TESTCHECKL(error, KErrNoMemory); // TSY reports "no free slots as KErrNoMemory"

	TRAP(error,AddContactL(KTestName,KTestNumber,KUidIccGlobalLndPhonebook)); 
	TESTCHECKL(error, KErrNoMemory); // TSY reports "no free slots as KErrNoMemory"

	TRAP(error,AddContactL(KTestName,KTestNumber,KUidUsimAppAdnPhonebook,KTestEMail)); 
	TESTCHECKL(error, KErrNoMemory); // TSY reports "no free slots as KErrNoMemory"

	TRAP(error,AddContactL(KTestName,KTestNumber,KUidIccGlobalFdnPhonebook)); 
	TESTCHECKL(error, KErrNoMemory); // TSY reports "no free slots as KErrNoMemory"

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkAddBoundaryConditionsTest object
 */
CPhbkAddBoundaryConditionsTest* CPhbkAddBoundaryConditionsTest::NewL()
	{
	CPhbkAddBoundaryConditionsTest* self = new(ELeave) CPhbkAddBoundaryConditionsTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkAddBoundaryConditionsTest::CPhbkAddBoundaryConditionsTest()
	{
	SetTestStepName(_L("AddBoundaryConditionsTest"));
	}

/* Add an ICC entry to each of the supported phonebooks - boundary conditions met */
enum TVerdict CPhbkAddBoundaryConditionsTest::doTestStepL()
	{
	SetSimTsyTestNumberL(19);
	DoSyncL();
	RPhoneBookSession::TContactFieldsV1 fields;
	RPhoneBookSession::TContactFieldsV1Pckg fieldsPckg(fields);
	iSession.GetContactFormat(fieldsPckg);

	TInt i(0);

	TBuf16<KMaxPhBkNameSize> name;
	TBuf16<RMobilePhone::KMaxMobileTelNumberSize> number;

	// Check Condition when length of fields are less than max length defined in config
	name.Format(KBoundaryTestName);
	number.Format(KBoundaryTestNumber);

	// value of i=2 because name field already contains 1 char
	for(i=2; i<fields.iNameField.iLength; i++)
		name.Append(KBoundaryTestName);

	TESTCHECKL(fields.iNameField.iCount, 1);
	TESTCHECKCONDITION(fields.iNameField.iDisplayed);
	
	// value of i=2 because number field already contains 1 char
	for(i=2; i<fields.iNumberField.iLength; i++)
		number.Append(KBoundaryTestNumber);

	TESTCHECK(fields.iNumberField.iCount, 1);
	TESTCHECKCONDITION(fields.iNumberField.iDisplayed);
	TESTCHECK(fields.iIccSlotField.iLength, -1);
	TESTCHECK(fields.iIccSlotField.iCount, 1);
	TESTCHECKCONDITION(fields.iIccSlotField.iDisplayed);

	// Now try to add these fields that test boundary conditions 
	TRAPD(err, AddContactL(name,number,KUidIccGlobalAdnPhonebook));
	TESTCHECKL(err, KErrNone);

	TRAP(err, AddContactL(name,number,KUidIccGlobalSdnPhonebook));
	TESTCHECKL(err, KErrNone);

	TRAP(err, AddContactL(name,number,KUidIccGlobalLndPhonebook));
	TESTCHECKL(err, KErrNone);

	TRAP(err, AddContactL(name,number,KUidUsimAppAdnPhonebook,KTestEMail));
	TESTCHECKL(err, KErrNone);

	TRAP(err, AddContactL(name,number,KUidIccGlobalFdnPhonebook));
	TESTCHECKL(err, KErrNone);

	// Check Condition when length of fields are equal to max length defined in config
	name.Format(KBoundaryTestName);
	number.Format(KBoundaryTestNumber);

	// value of i=2 because name field already contains 1 char
	for(i=2; i<fields.iNameField.iLength+1; i++)
		name.Append(KBoundaryTestName);

	TESTCHECKL(fields.iNameField.iCount, 1);
	TESTCHECKCONDITION(fields.iNameField.iDisplayed);
	
	// value of i=2 because number field already contains 1 char
	for(i=2; i<fields.iNumberField.iLength+1; i++)
		number.Append(KBoundaryTestNumber);

	TESTCHECK(fields.iNumberField.iCount, 1);
	TESTCHECKCONDITION(fields.iNumberField.iDisplayed);
	TESTCHECK(fields.iIccSlotField.iLength, -1);
	TESTCHECK(fields.iIccSlotField.iCount, 1);
	TESTCHECKCONDITION(fields.iIccSlotField.iDisplayed);

	// Now try to add these fields that test boundary conditions 
	TRAP(err, AddContactL(name,number,KUidIccGlobalAdnPhonebook));
	TESTCHECKL(err, KErrNone);

	TRAP(err, AddContactL(name,number,KUidIccGlobalSdnPhonebook));
	TESTCHECKL(err, KErrNone);

	TRAP(err, AddContactL(name,number,KUidIccGlobalLndPhonebook));
	TESTCHECKL(err, KErrNone);

	TRAP(err, AddContactL(name,number,KUidUsimAppAdnPhonebook,KTestEMail));
	TESTCHECKL(err, KErrNone);

	TRAP(err, AddContactL(name,number,KUidIccGlobalFdnPhonebook));
	TESTCHECKL(err, KErrNone);

	// Check Condition when length of name field is greater than max length defined in config
	name.Format(KBoundaryTestName);
	number.Format(KBoundaryTestNumber);

	// value of i=2 because name field already contains 1 char
	for(i=2; i<fields.iNameField.iLength+2; i++)
		name.Append(KBoundaryTestName);

	TESTCHECKL(fields.iNameField.iCount, 1);
	TESTCHECKCONDITION(fields.iNameField.iDisplayed);

	// value of i=2 because number field already contains 1 char
	for(i=2; i<fields.iNumberField.iLength+1; i++)
		number.Append(KBoundaryTestNumber);

	TESTCHECK(fields.iNumberField.iCount, 1);
	TESTCHECKCONDITION(fields.iNumberField.iDisplayed);
	TESTCHECK(fields.iIccSlotField.iLength, -1);
	TESTCHECK(fields.iIccSlotField.iCount, 1);
	TESTCHECKCONDITION(fields.iIccSlotField.iDisplayed);

	// Now try to add these fields that exceed boundary conditions 
	TRAP(err, AddContactL(name,number,KUidIccGlobalAdnPhonebook));
	TESTCHECKL(err, KErrPhonebookTextOverflow);

	TRAP(err, AddContactL(name,number,KUidIccGlobalSdnPhonebook));
	TESTCHECKL(err, KErrPhonebookTextOverflow);

	TRAP(err, AddContactL(name,number,KUidIccGlobalLndPhonebook));
	TESTCHECKL(err, KErrPhonebookTextOverflow);

	TRAP(err, AddContactL(name,number,KUidUsimAppAdnPhonebook,KTestEMail));
	TESTCHECKL(err, KErrPhonebookTextOverflow);

	TRAP(err, AddContactL(name,number,KUidIccGlobalFdnPhonebook));
	TESTCHECKL(err, KErrPhonebookTextOverflow);

	// Check Condition when length of tel number field is greater than max length defined in config
	name.Format(KBoundaryTestName);
	number.Format(KBoundaryTestNumber);

	// value of i=2 because name field already contains 1 char
	for(i=2; i<fields.iNameField.iLength+1; i++)
		name.Append(KBoundaryTestName);

	TESTCHECKL(fields.iNameField.iCount, 1);
	TESTCHECKCONDITION(fields.iNameField.iDisplayed);

	// value of i=2 because number field already contains 1 char
	for(i=2; i<fields.iNumberField.iLength+2; i++)
		number.Append(KBoundaryTestNumber);

	TESTCHECK(fields.iNumberField.iCount, 1);
	TESTCHECKCONDITION(fields.iNumberField.iDisplayed);
	TESTCHECK(fields.iIccSlotField.iLength, -1);
	TESTCHECK(fields.iIccSlotField.iCount, 1);
	TESTCHECKCONDITION(fields.iIccSlotField.iDisplayed);

	// Now try to add these fields that exceed boundary conditions 
	TRAP(err, AddContactL(name,number,KUidIccGlobalAdnPhonebook));
	TESTCHECKL(err, KErrPhonebookNumberOverflow);

	TRAP(err, AddContactL(name,number,KUidIccGlobalSdnPhonebook));
	TESTCHECKL(err, KErrPhonebookNumberOverflow);

	TRAP(err, AddContactL(name,number,KUidIccGlobalLndPhonebook));
	TESTCHECKL(err, KErrPhonebookNumberOverflow);

	TRAP(err, AddContactL(name,number,KUidUsimAppAdnPhonebook,KTestEMail));
	TESTCHECKL(err, KErrPhonebookNumberOverflow);

	TRAP(err, AddContactL(name,number,KUidIccGlobalFdnPhonebook));
	TESTCHECKL(err, KErrPhonebookNumberOverflow);
	
	return TestStepResult();
	}

/** 
 * Factory construction method.
 * @return Pointer to CPhbkAddNameBoundaryExceededTest object
 */
CPhbkAddNameBoundaryExceededTest* CPhbkAddNameBoundaryExceededTest::NewL()
	{
	CPhbkAddNameBoundaryExceededTest* self = new(ELeave) CPhbkAddNameBoundaryExceededTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkAddNameBoundaryExceededTest::CPhbkAddNameBoundaryExceededTest()
	{
	SetTestStepName(_L("AddNameBoundaryExceededTest"));
	}

/* Add an ICC entry to each of the supported phonebooks - name boundary conditions exceeded */
enum TVerdict CPhbkAddNameBoundaryExceededTest::doTestStepL()
	{
	RPhoneBookSession::TContactFieldsV1 fields;
	RPhoneBookSession::TContactFieldsV1Pckg fieldsPckg(fields);
	iSession.GetContactFormat(fieldsPckg);
	
	TBuf16<KMaxPhBkNameSize> name;
	name.Format(KBoundaryTestName);
	TBuf16<RMobilePhone::KMaxMobileTelNumberSize> number;
	number.Format(KBoundaryTestNumber);

	TInt i(0);
	for(i=0; i<(fields.iNameField.iLength+10); i++)
		name.Append(KBoundaryTestName); // name boundary exceeded by 10 characters

	TESTCHECKL(fields.iNameField.iCount, 1);
	TESTCHECKCONDITIONL(fields.iNameField.iDisplayed);
	
	for(i=1; i<fields.iNumberField.iLength+3; i++)
		number.Append(KBoundaryTestNumber);

	TESTCHECKL(fields.iNumberField.iCount, 1);
	TESTCHECKCONDITIONL(fields.iNumberField.iDisplayed);
	TESTCHECKL(fields.iIccSlotField.iLength, -1);
	TESTCHECKL(fields.iIccSlotField.iCount, 1);
	TESTCHECKCONDITIONL(fields.iIccSlotField.iDisplayed);

	// move to another SIM TSY configuration where the phonebook is full

	TRAPD(err, AddContactL(name,number,KUidIccGlobalAdnPhonebook));
	TESTCHECKL(err, KErrOverflow);
	TRAP(err, AddContactL(name,number,KUidIccGlobalSdnPhonebook));
	TESTCHECKL(err, KErrOverflow);
	TRAP(err, AddContactL(name,number,KUidIccGlobalLndPhonebook));
	TESTCHECKL(err, KErrOverflow);
	TRAP(err, AddContactL(name,number,KUidUsimAppAdnPhonebook,KTestEMail));
	TESTCHECKL(err, KErrOverflow);
	TRAP(err, AddContactL(name,number,KUidIccGlobalFdnPhonebook));
	TESTCHECKL(err, KErrOverflow);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkAddNumberBoundaryExceededTest object
 */
CPhbkAddNumberBoundaryExceededTest* CPhbkAddNumberBoundaryExceededTest::NewL()
	{
	CPhbkAddNumberBoundaryExceededTest* self = new(ELeave) CPhbkAddNumberBoundaryExceededTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkAddNumberBoundaryExceededTest::CPhbkAddNumberBoundaryExceededTest()
	{
	SetTestStepName(_L("AddNumberBoundaryExceededTest"));
	}

/* Add an ICC entry to each of the supported phonebooks - number boundary conditions exceeded */
enum TVerdict CPhbkAddNumberBoundaryExceededTest::doTestStepL()
	{
	RPhoneBookSession::TContactFieldsV1 fields;
	RPhoneBookSession::TContactFieldsV1Pckg fieldsPckg(fields);
	iSession.GetContactFormat(fieldsPckg);
	TBuf16<KMaxPhBkNameSize> name;
	name.Format(KBoundaryTestName);
	TBuf16<RMobilePhone::KMaxMobileTelNumberSize> number;
	number.Format(KBoundaryTestNumber);

	TInt i(0);
	for(i=1; i<fields.iNameField.iLength+1; i++)
		name.Append(KBoundaryTestName); 

	TESTCHECKL(fields.iNameField.iCount, 1);
	TESTCHECKCONDITIONL(fields.iNameField.iDisplayed);
	
	for(i=0; i<fields.iNumberField.iLength+1; i++)
		number.Append(KBoundaryTestNumber); // number boundary exceeded by 1 character

	TESTCHECKL(fields.iNumberField.iCount, 1);
	TESTCHECKCONDITIONL(fields.iNumberField.iDisplayed);
	TESTCHECKL(fields.iIccSlotField.iLength, -1);
	TESTCHECKL(fields.iIccSlotField.iCount, 1);
	TESTCHECKCONDITIONL(fields.iIccSlotField.iDisplayed);

	// move to another SIM TSY configuration where the phonebook is full

	TRAPD(err, AddContactL(name,number,KUidIccGlobalAdnPhonebook));
	TESTCHECKL(err, KErrOverflow);
	TRAP(err, AddContactL(name,number,KUidIccGlobalSdnPhonebook));
	TESTCHECKL(err, KErrOverflow);
	TRAP(err, AddContactL(name,number,KUidIccGlobalLndPhonebook));
	TESTCHECKL(err, KErrOverflow);
	TRAP(err, AddContactL(name,number,KUidUsimAppAdnPhonebook,KTestEMail));
	TESTCHECKL(err, KErrOverflow);
	TRAP(err, AddContactL(name,number,KUidIccGlobalFdnPhonebook));
	TESTCHECKL(err, KErrOverflow);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkAddSequenceOfEntriesTest object
 */
CPhbkAddSequenceOfEntriesTest* CPhbkAddSequenceOfEntriesTest::NewL()
	{
	CPhbkAddSequenceOfEntriesTest* self = new(ELeave) CPhbkAddSequenceOfEntriesTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkAddSequenceOfEntriesTest::CPhbkAddSequenceOfEntriesTest()
	{
	SetTestStepName(_L("AddSequenceOfEntriesTest"));
	}

/* Add sequence of ICC entries - Fill ICC */
enum TVerdict CPhbkAddSequenceOfEntriesTest::doTestStepL()
	{
	SetSimTsyTestNumberL(19);
	DoSyncL();
	for(TInt i=1; i<(KMaxICCSlotsTest6+10);++i)
		{
		TRAPD(err1, AddContactL(KTestName, KTestNumber, KUidIccGlobalAdnPhonebook)); // Keep adding entries to all phonebooks
		TRAPD(err2, AddContactL(KTestName, KTestNumber, KUidIccGlobalSdnPhonebook));
		TRAPD(err3, AddContactL(KTestName, KTestNumber, KUidIccGlobalLndPhonebook));
		TRAPD(err4, AddContactL(KTestName, KTestNumber, KUidUsimAppAdnPhonebook, KTestEMail));
		TRAPD(err5, AddContactL(KTestName, KTestNumber, KUidIccGlobalFdnPhonebook));
		if(i < (KMaxICCSlotsTest6 - KNumOfUsedSlotstest6 + 1))
			{			
			TESTCHECKL(err1, KErrNone);
			TESTCHECKL(err2, KErrNone);
			TESTCHECKL(err3, KErrNone);
			TESTCHECKL(err4, KErrNone);
			TESTCHECKL(err5, KErrNone);
			}
		else
			{
			TESTCHECKL(err1, KErrNoMemory);
			TESTCHECKL(err2, KErrNoMemory);
			TESTCHECKL(err3, KErrNoMemory);
			TESTCHECKL(err4, KErrNoMemory);
			TESTCHECKL(err5, KErrNoMemory);
			}
		}

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkAddEntryICCLockedTest object
 */
CPhbkAddEntryICCLockedTest* CPhbkAddEntryICCLockedTest::NewL()
	{
	CPhbkAddEntryICCLockedTest* self = new(ELeave) CPhbkAddEntryICCLockedTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkAddEntryICCLockedTest::CPhbkAddEntryICCLockedTest()
	{
	SetTestStepName(_L("AddEntryICCLockedTest"));
	}

/* Add an ICC entry to each of the supported phonebooks - ICC locked */
enum TVerdict CPhbkAddEntryICCLockedTest::doTestStepL()
	{
	SetSimTsyTestNumberL(18);
	DoSyncL();
	TInt err1(KErrNone), err2(KErrNone), err3(KErrNone), err4(KErrNone), err5(KErrNone);
	while(err1==KErrNone && err2==KErrNone && err3==KErrNone && err4==KErrNone) // Wait for ICC to become locked
		{
		TRAP(err1,AddContactL(KTestName,KTestNumber,KUidIccGlobalAdnPhonebook));
		TRAP(err2,AddContactL(KTestName,KTestNumber,KUidIccGlobalSdnPhonebook));
		TRAP(err3,AddContactL(KTestName,KTestNumber,KUidIccGlobalLndPhonebook));
		TRAP(err4,AddContactL(KTestName,KTestNumber,KUidUsimAppAdnPhonebook,KTestEMail));
		TRAP(err5,AddContactL(KTestName,KTestNumber,KUidIccGlobalFdnPhonebook));
		User::After(2000000);	// Wait a second and try again.
		}
	TRAP(err1,AddContactL(KTestName,KTestNumber,KUidIccGlobalAdnPhonebook));
	TRAP(err2,AddContactL(KTestName,KTestNumber,KUidIccGlobalSdnPhonebook));
	TRAP(err3,AddContactL(KTestName,KTestNumber,KUidIccGlobalLndPhonebook));
	TRAP(err4,AddContactL(KTestName,KTestNumber,KUidUsimAppAdnPhonebook,KTestEMail));
	TRAP(err5,AddContactL(KTestName,KTestNumber,KUidIccGlobalFdnPhonebook));
	TESTCHECK(err1, KErrAccessDenied);
	TESTCHECK(err2, KErrAccessDenied);
	TESTCHECK(err3, KErrAccessDenied);
	TESTCHECK(err4, KErrAccessDenied);
	TESTCHECK(err5, KErrAccessDenied);
	User::After(10000000); // Wait for ICC to become unlocked

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkAddEntryICCWriteFailsTest object
 */
CPhbkAddEntryICCWriteFailsTest* CPhbkAddEntryICCWriteFailsTest::NewL()
	{
	CPhbkAddEntryICCWriteFailsTest* self = new(ELeave) CPhbkAddEntryICCWriteFailsTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkAddEntryICCWriteFailsTest::CPhbkAddEntryICCWriteFailsTest()
	{
	SetTestStepName(_L("AddEntryICCWriteFailsTest"));
	}

/* Add an ICC entry to each of the supported phonebooks - ICC write fails */
enum TVerdict CPhbkAddEntryICCWriteFailsTest::doTestStepL()
	{
	SetSimTsyTestNumberL(5);
	DoSyncL();
	TRAPD(err,AddContactL(KTestName,KTestNumber,KUidIccGlobalAdnPhonebook)); 
	TESTCHECKL(err, KErrBadHandle);
	TRAP(err,AddContactL(KTestName,KTestNumber,KUidIccGlobalSdnPhonebook)); 
	TESTCHECKL(err, KErrBadHandle);
	TRAP(err,AddContactL(KTestName,KTestNumber,KUidIccGlobalLndPhonebook)); 
	TESTCHECKL(err, KErrBadHandle);
	TRAP(err,AddContactL(KTestName,KTestNumber,KUidUsimAppAdnPhonebook,KTestEMail)); 
	TESTCHECKL(err, KErrBadHandle);
	TRAP(err,AddContactL(KTestName,KTestNumber,KUidIccGlobalFdnPhonebook)); 
	TESTCHECKL(err, KErrBadHandle);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkAddEntryInvalidFieldTest object
 */
CPhbkAddEntryInvalidFieldTest* CPhbkAddEntryInvalidFieldTest::NewL()
	{
	CPhbkAddEntryInvalidFieldTest* self = new(ELeave) CPhbkAddEntryInvalidFieldTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkAddEntryInvalidFieldTest::CPhbkAddEntryInvalidFieldTest()
	{
	SetTestStepName(_L("AddEntryInvalidFieldTest"));
	}

/* Add an ICC entry to each of the supported phonebooks - invalid field included */
enum TVerdict CPhbkAddEntryInvalidFieldTest::doTestStepL()
	{
	SetSimTsyTestNumberL(17);
	DoSyncL();

	DoAddEntryInvalidFieldL(KUidIccGlobalAdnPhonebook);
	DoAddEntryInvalidFieldL(KUidIccGlobalSdnPhonebook);
	DoAddEntryInvalidFieldL(KUidIccGlobalLndPhonebook);
	DoAddEntryInvalidFieldL(KUidUsimAppAdnPhonebook);
	DoAddEntryInvalidFieldL(KUidIccGlobalFdnPhonebook);

	return TestStepResult();
	}


/* Add an ICC entry to phonebook aPhonebook - invalid field included */
void CPhbkAddIntegrationTestBase::DoAddEntryInvalidFieldL(const TUid aPhonebook)
	{
	TContactItemId templateId = iDb->ICCTemplateIdL(aPhonebook);
	CContactItem* iccTemplate = iDb->ReadContactLC(templateId);
	CContactICCEntry* entry = CContactICCEntry::NewL(*iccTemplate);
	CleanupStack::PopAndDestroy(iccTemplate);
	CleanupStack::PushL(entry);

	AddTextFieldL(KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN, KTestName, *entry);
	AddTextFieldL(KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL, KTestNumber, *entry);
	AddTextFieldL(KUidContactFieldCompanyName,KUidContactFieldVCardMapUnusedN, KTestCompany, *entry); // invalid field 
	TContactItemId id(KNullContactId);
	TRAPD(err, id=iDb->AddNewContactL(*entry));
	CleanupStack::PopAndDestroy(entry);	
	TESTCHECKL(id, KNullContactId);
	TESTCHECKL(err, KErrNotSupported); 
	}

/** 
 * Factory construction method.
 * @return Pointer to CPhbkAddEntryNumberWithDTMFTest object
 */
CPhbkAddEntryNumberWithDTMFTest* CPhbkAddEntryNumberWithDTMFTest::NewL()
	{
	CPhbkAddEntryNumberWithDTMFTest* self = new(ELeave) CPhbkAddEntryNumberWithDTMFTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkAddEntryNumberWithDTMFTest::CPhbkAddEntryNumberWithDTMFTest()
	{
	SetTestStepName(_L("AddEntryNumberWithDTMFTest"));
	}

/* Add sequence of ICC entries - Fill ICC */
enum TVerdict CPhbkAddEntryNumberWithDTMFTest::doTestStepL()
	{
	SetSimTsyTestNumberL(19);
	DoSyncL();
	TRAPD(err,AddContactL(KTestName,KTestNumberWithDTMF,KUidIccGlobalAdnPhonebook)); 
	TESTCHECKL(err, KErrNone); 
	TRAP(err,AddContactL(KTestName,KTestNumberWithDTMF,KUidIccGlobalSdnPhonebook)); 
	TESTCHECKL(err, KErrNone); 
	TRAP(err,AddContactL(KTestName,KTestNumberWithDTMF,KUidIccGlobalLndPhonebook)); 
	TESTCHECKL(err, KErrNone); 
	TRAP(err,AddContactL(KTestName,KTestNumberWithDTMF,KUidUsimAppAdnPhonebook,KTestEMail)); 
	TESTCHECKL(err, KErrNone); 
	TRAP(err,AddContactL(KTestName,KTestNumberWithDTMF,KUidIccGlobalFdnPhonebook)); 
	TESTCHECKL(err, KErrNone); 

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkAddEntryReadOnlyAccessTest object
 */
CPhbkAddEntryReadOnlyAccessTest* CPhbkAddEntryReadOnlyAccessTest::NewL()
	{
	CPhbkAddEntryReadOnlyAccessTest* self = new(ELeave) CPhbkAddEntryReadOnlyAccessTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkAddEntryReadOnlyAccessTest::CPhbkAddEntryReadOnlyAccessTest()
	{
	SetTestStepName(_L("AddEntryReadOnlyAccessTest"));
	}

/* Add an ICC entry to each of the supported phonebooks - read-only phonebook */
enum TVerdict CPhbkAddEntryReadOnlyAccessTest::doTestStepL()
	{
	SetSimTsyTestNumberL(20);
	DoSyncL();
	TRAPD(err,AddContactL(KTestName,KTestNumber,KUidIccGlobalAdnPhonebook)); // valid fields
	TESTCHECKL(err, KErrAccessDenied); // read-only access 
	TRAP(err,AddContactL(KTestName,KTestNumber,KUidIccGlobalSdnPhonebook)); // valid fields
	TESTCHECKL(err, KErrAccessDenied); // read-only access 
	TRAP(err,AddContactL(KTestName,KTestNumber,KUidIccGlobalLndPhonebook)); // valid fields
	TESTCHECKL(err, KErrAccessDenied); // read-only access 
	TRAP(err,AddContactL(KTestName,KTestNumber,KUidUsimAppAdnPhonebook,KTestEMail)); // valid fields
	TESTCHECKL(err, KErrAccessDenied); // read-only access 
	TRAP(err,AddContactL(KTestName,KTestNumber,KUidIccGlobalFdnPhonebook)); // valid fields
	TESTCHECKL(err, KErrAccessDenied); // read-only access 

	return TestStepResult();
	}


/** 
 * Add a contact to the ICC & database.
 * @param aName Name
 * @param aNumber Phone number
 * @param aPhonebook Phonebook
 * @param aEMail Email address - only added to Usim phonebook
 */
void CPhbkAddIntegrationTestBase::AddContactL(const TDesC& aName, const TDesC& aNumber, const TUid aPhonebook, const TDesC& aEMail)
	{
	TContactItemId templateId(iDb->ICCTemplateIdL(aPhonebook));
	CContactItem* iccTemplate = iDb->ReadContactLC(templateId);
	CContactICCEntry* entry = CContactICCEntry::NewL(*iccTemplate);
	CleanupStack::PopAndDestroy(iccTemplate);
	CleanupStack::PushL(entry);
	AddTextFieldL(KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,aName,*entry);
	AddTextFieldL(KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL,aNumber,*entry);
	if ((aPhonebook == KUidUsimAppAdnPhonebook) || (aPhonebook == KUidIccGlobalAdnPhonebook))
		{
		AddTextFieldL(KUidContactFieldEMail,KUidContactFieldVCardMapUnusedN,aEMail,*entry);
		}
	TContactItemId id(iDb->AddNewContactL(*entry)); 
	CleanupStack::PopAndDestroy(entry);	
	TESTCHECKCONDITIONL(id != KNullContactId);
	}

/**
 * Add a new text field (aField) to the CContactICCEntry supplied by aIccEntry. 
 * @param aFieldType Field type
 * @param aMapping Mapping for the field's content type
 * @param aField Field data
 * @param aIccEntry CContactICCEntry item
 */ 
void CPhbkAddIntegrationTestBase::AddTextFieldL(TFieldType aFieldType, TUid aMapping, const TDesC& aField, CContactICCEntry& aIccEntry)
	{
	CContactItemFieldSet& fieldSet = aIccEntry.CardFields();
	TInt pos = fieldSet.Find(aFieldType);
	if (pos!=KErrNotFound)
		{
		CContactItemField& field=fieldSet[pos];
		field.TextStorage()->SetTextL(aField);
		}
	else
		{
		CContactItemField* field=CContactItemField::NewLC(KStorageTypeText, aFieldType);
		field->SetMapping(aMapping);
		field->AddFieldTypeL(aFieldType); 
		field->TextStorage()->SetTextL(aField);
		aIccEntry.AddFieldL(*field);
		CleanupStack::Pop(field); 
		}
	}

/** 
 * Factory construction method.
 * @return Pointer to CPhbkAddAdditionalNumWithTagTest object
 */
CPhbkAddAdditionalNumWithTagTest* CPhbkAddAdditionalNumWithTagTest::NewL()
	{
	CPhbkAddAdditionalNumWithTagTest* self = new(ELeave) CPhbkAddAdditionalNumWithTagTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkAddAdditionalNumWithTagTest::CPhbkAddAdditionalNumWithTagTest()
	{
	SetTestStepName(_L("AddAdditionalNumWithTagTest"));
	}

/** 
 * Add contacts to the ICC & database with additional number plus alpha tag.
 */
enum TVerdict CPhbkAddAdditionalNumWithTagTest::doTestStepL()
	{
	SetSimTsyTestNumberL(22);
	DoSyncL();

	// add additional entry for each type of number alpha tag
	AddAdditionalNumEntryL(KTestAddNumber, KUidContactFieldVCardMapMSG);
	AddAdditionalNumEntryL(KTestAddNumber, KUidContactFieldVCardMapVOICE);
	AddAdditionalNumEntryL(KTestAddNumber, KUidContactFieldVCardMapFAX);
	AddAdditionalNumEntryL(KTestAddNumber, KUidContactFieldVCardMapPREF);
	AddAdditionalNumEntryL(KTestAddNumber, KUidContactFieldVCardMapCELL);
	AddAdditionalNumEntryL(KTestAddNumber, KUidContactFieldVCardMapPAGER);
	AddAdditionalNumEntryL(KTestAddNumber, KUidContactFieldVCardMapBBS);
	AddAdditionalNumEntryL(KTestAddNumber, KUidContactFieldVCardMapMODEM);
	AddAdditionalNumEntryL(KTestAddNumber, KUidContactFieldVCardMapCAR);
	AddAdditionalNumEntryL(KTestAddNumber, KUidContactFieldVCardMapISDN);
	AddAdditionalNumEntryL(KTestAddNumber, KUidContactFieldVCardMapVIDEO);

	return TestStepResult();
	}


/** 
 * Add a contact with additional number plus alpha tag.
 */
void CPhbkAddIntegrationTestBase::AddAdditionalNumEntryL(const TDesC& aFieldNum, const TUid aFieldType)
	{
	TContactItemId templateId(iDb->ICCTemplateIdL(KUidUsimAppAdnPhonebook));
	CContactItem* iccTemplate = iDb->ReadContactLC(templateId);
	CContactICCEntry* entry = CContactICCEntry::NewL(*iccTemplate);
	CleanupStack::PopAndDestroy(iccTemplate);
	CleanupStack::PushL(entry);

	// create typical USIM App entry
	AddTextFieldL(KUidContactFieldFamilyName, KUidContactFieldVCardMapUnusedN, KTestName, *entry);
	AddTextFieldL(KUidContactFieldPhoneNumber, KUidContactFieldVCardMapTEL, KTestNumber, *entry);
	AddTextFieldL(KUidContactFieldEMail, KUidContactFieldVCardMapUnusedN, KTestEMail, *entry);

	// add additional number with alpha tag
	CContactItemField* field=CContactItemField::NewLC(KStorageTypeText, KUidContactFieldPhoneNumber);
	field->SetMapping(KUidContactFieldVCardMapTEL);
	field->AddFieldTypeL(KUidContactFieldPhoneNumber); 
	field->TextStorage()->SetTextL(aFieldNum);
	field->AddFieldTypeL(aFieldType); 
	entry->AddFieldL(*field);
	CleanupStack::Pop(field); 

	TContactItemId id(iDb->AddNewContactL(*entry)); 
	CleanupStack::PopAndDestroy(entry);	
	TESTCHECKCONDITIONL(id != KNullContactId);

	// check additional entry has the additional number
	CContactItem* newEntry = static_cast <CContactICCEntry*> (iDb->ReadContactLC(id));
	TESTCHECKCONDITIONL(newEntry != NULL);
	CContactItemFieldSet& fieldSet = newEntry->CardFields();
	TInt pos = fieldSet.Find(aFieldType);
	TESTCHECKCONDITIONL(pos != KErrNotFound);
	CleanupStack::PopAndDestroy(newEntry);	

	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkAddEntryWithNoTemplateTest object
 */
CPhbkAddEntryWithNoTemplateTest* CPhbkAddEntryWithNoTemplateTest::NewL()
	{
	CPhbkAddEntryWithNoTemplateTest* self = new(ELeave) CPhbkAddEntryWithNoTemplateTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises its own name.
 */
CPhbkAddEntryWithNoTemplateTest::CPhbkAddEntryWithNoTemplateTest()
	{
	SetTestStepName(_L("AddEntryWithNoTemplateTest"));
	}

/** 
 *  Create contact group with zero as template ID, and add a contact to it.
 */
enum TVerdict CPhbkAddEntryWithNoTemplateTest::doTestStepL()
	{

	//
	// delete ini file
	//
	
	INFO_PRINTF1(_L("Deleting phonefile"));
	_LIT(KPhbkSyncIniFile,"C:\\Private\\102030A1\\Phbksync.ini");
	RFs fs;
	TInt err = fs.Connect();
	TESTCHECKL(err, KErrNone);
	err=fs.Delete(KPhbkSyncIniFile);
	fs.Close();	

	//
	// add a new contact to a group with template id zero
	//
	CContactItem* group = iDb->CreateContactGroupLC();
	group->SetTemplateRefId(0);

	CContactICCEntry* contact = NULL;
	CreateContactLC(contact, KUidIccGlobalAdnPhonebook,KTestName,KTestNumber, KTestEMail);

	iDb->AddContactToGroupL(*contact,*group);

	CleanupStack::PopAndDestroy(2,group);

	//
	// force a creation of the phonebooks (including a call to CSyncEngineServer::CreateGroupIdL)
	//
	ConfigurePhbkSyncToIdleL();
	ConfigurePhbkSyncToFullL();

	return TestStepResult();
}
 
void CPhbkAddIntegrationTestBase::CreateContactLC(CContactICCEntry*& aEntry, const TUid aPhonebook, const TDesC& aName, const TDesC& aNumber, const TDesC& aEmail)
	{
	TContactItemId templateId(iDb->ICCTemplateIdL(aPhonebook));
	CContactItem* iccTemplate = iDb->ReadContactLC(templateId);
	aEntry = CContactICCEntry::NewL(*iccTemplate);
	CleanupStack::PopAndDestroy(iccTemplate);
	CleanupStack::PushL(aEntry);

	AddTextFieldL(KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,aName,*aEntry);
	AddTextFieldL(KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL,aNumber,*aEntry);
	if ((aPhonebook == KUidUsimAppAdnPhonebook) || (aPhonebook == KUidIccGlobalAdnPhonebook))
		{
		AddTextFieldL(KUidContactFieldEMail,KUidContactFieldVCardMapUnusedN,aEmail,*aEntry);
		}
	}
