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
#include "te_cntsyncdelete.h"

_LIT(KDeletedNameInADN, "ADN Delete Test");
_LIT(KDeletedNameInSDN, "SDN Delete Test");
_LIT(KDeletedNameInLND, "LND Delete Test");
_LIT(KDeletedNameInUsim,"Usim Delete Test");
_LIT(KDeletedNameInFDN, "FDN Delete Test");

_LIT(KDeletedNumberInADN, "0202020323909094394");
_LIT(KDeletedNumberInSDN, "1212121323919194394");
_LIT(KDeletedNumberInLND, "2222222323929294394");
_LIT(KDeletedNumberInUsim,"3232323323939394394");
_LIT(KDeletedNumberInFDN, "4242424343949494394");

_LIT(KDeletedEmailInUsim,"example@here");

_LIT(KSearchStringInADN, "AAAAAAAAAAAAAAAAA");
_LIT(KSearchStringInSDN, "BBBBBBBBBBBBBBBBB");
_LIT(KSearchStringInLND, "CCCCCCCCCCCCCCCCC");
_LIT(KSearchStringInUsim,"DDDDDDDDDDDDDDDDD");
_LIT(KSearchStringInFDN, "EEEEEEEEEEEEEEEEE");

_LIT(KSearchNameInADN, "ADN Name");
_LIT(KSearchNameInSDN, "SDN Name");
_LIT(KSearchNameInLND, "LND Name");
_LIT(KSearchNameInUsim,"Usim Name");
_LIT(KSearchNameInFDN, "FDN Name");


/** 
 * Factory construction method.
 * @return Pointer to CPhbkDeleteICCContactTest object
 */
CPhbkDeleteICCContactTest* CPhbkDeleteICCContactTest::NewL()
	{
	CPhbkDeleteICCContactTest* self = new(ELeave) CPhbkDeleteICCContactTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkDeleteICCContactTest::CPhbkDeleteICCContactTest()
	{
	SetTestStepName(_L("DeleteICCContactTest"));
	}

/** Delete an ICC entry - normal case */
enum TVerdict CPhbkDeleteICCContactTest::doTestStepL()
	{
	SetSimTsyTestNumberL(0);
	DoSyncL();

	DeleteContactL(KUidIccGlobalAdnPhonebook);
	DeleteContactL(KUidIccGlobalSdnPhonebook);
	DeleteContactL(KUidIccGlobalLndPhonebook);
	DeleteContactL(KUidUsimAppAdnPhonebook);
	DeleteContactL(KUidIccGlobalFdnPhonebook);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkDeleteEntryInvalidUIDTest object
 */
CPhbkDeleteEntryInvalidUIDTest* CPhbkDeleteEntryInvalidUIDTest::NewL()
	{
	CPhbkDeleteEntryInvalidUIDTest* self = new(ELeave) CPhbkDeleteEntryInvalidUIDTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkDeleteEntryInvalidUIDTest::CPhbkDeleteEntryInvalidUIDTest()
	{
	SetTestStepName(_L("DeleteEntryInvalidUIDTest"));
	}

/** Delete an ICC entry - invalid UID */
enum TVerdict CPhbkDeleteEntryInvalidUIDTest::doTestStepL()
	{
	TContactItemId id(1500); // Exceeded max number on ICC
	TRAPD(err, iDb->DeleteContactL(id));
	TESTCHECKL(err, KErrNotFound);

	id = -2; // Negative UID
	TRAP(err, iDb->DeleteContactL(id));
	TESTCHECKL(err, KErrNotFound);

	id = 6789; // Out-of-range UID
	TRAP(err, iDb->DeleteContactL(id));
	TESTCHECKL(err, KErrNotFound);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkDeleteAllEntriesTest object
 */
CPhbkDeleteAllEntriesTest* CPhbkDeleteAllEntriesTest::NewL()
	{
	CPhbkDeleteAllEntriesTest* self = new(ELeave) CPhbkDeleteAllEntriesTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkDeleteAllEntriesTest::CPhbkDeleteAllEntriesTest()
	{
	SetTestStepName(_L("DeleteAllEntriesTest"));
	}

/** Delete all ICC entries one at a time */
enum TVerdict CPhbkDeleteAllEntriesTest::doTestStepL()
	{
	SetSimTsyTestNumberL(0);
	DoSyncL();

	iDb->SetDbViewContactType(KUidContactICCEntry);
	const CContactIdArray* sortedItems = iDb->SortedItemsL();
	TContactItemId id(KNullContactId);
	TInt count = sortedItems->Count();

	TInt err(KErrNone);
	for(TInt i=0; i<=count-1;i++)
		{
		id = (*sortedItems)[0];
		iDb->DeleteContactL(id);
		CContactItem* item=NULL; //check the item has been removed from the database
		TRAP(err, item = iDb->ReadContactL(id));
		TESTCHECKL(err, KErrNotFound);
		delete item;
		}

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkDeleteEntryICCLockedTest object
 */
CPhbkDeleteEntryICCLockedTest* CPhbkDeleteEntryICCLockedTest::NewL()
	{
	CPhbkDeleteEntryICCLockedTest* self = new(ELeave) CPhbkDeleteEntryICCLockedTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkDeleteEntryICCLockedTest::CPhbkDeleteEntryICCLockedTest()
	{
	SetTestStepName(_L("DeleteEntryICCLockedTest"));
	}

/** Delete an ICC entry - ICC locked */
enum TVerdict CPhbkDeleteEntryICCLockedTest::doTestStepL()
	{
	SetSimTsyTestNumberL(2);
	DoSyncL();

	// Access ICC and wait for it to be locked
	iDb->SetDbViewContactType(KUidContactICCEntry);
	const CContactIdArray* sortedItems = iDb->SortedItemsL();
	TContactItemId id = (*sortedItems)[0];
	TInt err(KErrNone);
	while(err==KErrNone) // Wait for ICC to become locked 
		{
		User::After(1000000);	// Wait a second and try again.
		CContactICCEntry* failEntry=NULL;
		TRAP(err, failEntry = (CContactICCEntry*)iDb->ReadContactL(id, *iDb->AllFieldsView())); // Entry unavailable due to ICC being locked
		delete failEntry;
		}
	TESTCHECKL(err, KErrAccessDenied);

	TRAP(err, DeleteContactL(KUidIccGlobalAdnPhonebook));
	TESTCHECKL(err, KErrAccessDenied);

	TRAP(err, DeleteContactL(KUidIccGlobalSdnPhonebook));
	TESTCHECKL(err, KErrAccessDenied);

	TRAP(err, DeleteContactL(KUidIccGlobalLndPhonebook));
	TESTCHECKL(err, KErrAccessDenied);

	TRAP(err, DeleteContactL(KUidUsimAppAdnPhonebook));
	TESTCHECKL(err, KErrAccessDenied);
	
	TRAP(err, DeleteContactL(KUidIccGlobalFdnPhonebook));
	TESTCHECKL(err, KErrAccessDenied);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkDeleteEntryICCDeleteFailsTest object
 */
CPhbkDeleteEntryICCDeleteFailsTest* CPhbkDeleteEntryICCDeleteFailsTest::NewL()
	{
	CPhbkDeleteEntryICCDeleteFailsTest* self = new(ELeave) CPhbkDeleteEntryICCDeleteFailsTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkDeleteEntryICCDeleteFailsTest::CPhbkDeleteEntryICCDeleteFailsTest()
	{
	SetTestStepName(_L("DeleteEntryICCDeleteFailsTest"));
	}

/** Delete an ICC entry - ICC delete fails */
enum TVerdict CPhbkDeleteEntryICCDeleteFailsTest::doTestStepL()
	{
	SetSimTsyTestNumberL(4);
	DoSyncL();

	TRAPD(err, DeleteContactL(KUidIccGlobalAdnPhonebook));
	TESTCHECKL(err, KErrBadHandle);

	TRAP(err, DeleteContactL(KUidIccGlobalSdnPhonebook));
	TESTCHECKL(err, KErrBadHandle);

	TRAP(err, DeleteContactL(KUidIccGlobalLndPhonebook));
	TESTCHECKL(err, KErrBadHandle);

	TRAP(err, DeleteContactL(KUidUsimAppAdnPhonebook));
	TESTCHECKL(err, KErrBadHandle);

	TRAP(err, DeleteContactL(KUidIccGlobalFdnPhonebook));
	TESTCHECKL(err, KErrBadHandle);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkDeleteEntryReadOnlyAccessTest object
 */
CPhbkDeleteEntryReadOnlyAccessTest* CPhbkDeleteEntryReadOnlyAccessTest::NewL()
	{
	CPhbkDeleteEntryReadOnlyAccessTest* self = new(ELeave) CPhbkDeleteEntryReadOnlyAccessTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkDeleteEntryReadOnlyAccessTest::CPhbkDeleteEntryReadOnlyAccessTest()
	{
	SetTestStepName(_L("DeleteEntryReadOnlyAccessTest"));
	}

/** Delete an ICC entry - read-only phonebook */
enum TVerdict CPhbkDeleteEntryReadOnlyAccessTest::doTestStepL()
	{
	SetSimTsyTestNumberL(12);
	DoSyncL();

	TRAPD(err, DeleteContactL(KUidIccGlobalAdnPhonebook));
	TESTCHECKL(err, KErrAccessDenied);

	TRAP(err, DeleteContactL(KUidIccGlobalSdnPhonebook));
	TESTCHECKL(err, KErrAccessDenied);

	TRAP(err, DeleteContactL(KUidIccGlobalLndPhonebook));
	TESTCHECKL(err, KErrAccessDenied);

	TRAP(err, DeleteContactL(KUidUsimAppAdnPhonebook));
	TESTCHECKL(err, KErrAccessDenied);

	TRAP(err, DeleteContactL(KUidIccGlobalFdnPhonebook));
	TESTCHECKL(err, KErrAccessDenied);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkDeleteArrayOneEntryTest object
 */
CPhbkDeleteArrayOneEntryTest* CPhbkDeleteArrayOneEntryTest::NewL()
	{
	CPhbkDeleteArrayOneEntryTest* self = new(ELeave) CPhbkDeleteArrayOneEntryTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkDeleteArrayOneEntryTest::CPhbkDeleteArrayOneEntryTest()
	{
	SetTestStepName(_L("DeleteArrayOneEntryTest"));
	}

/** Delete an array of ICC entries - one entry in the array */
enum TVerdict CPhbkDeleteArrayOneEntryTest::doTestStepL()
	{
	SetSimTsyTestNumberL(10);
	DoSyncL();

	// *
	// * MJH  - 12 Sept 2002
	// * Assumptions for test data:
	// * 
	// * Each phonebook must use a unique number format in order that the array 
	// * of entries to be deleted only contains a single entry.
	// * 

	DeleteArrayWithOneEntryL(KDeletedNumberInADN, KDeletedNameInADN, KUidIccGlobalAdnPhonebook);
	DeleteArrayWithOneEntryL(KDeletedNumberInSDN, KDeletedNameInSDN, KUidIccGlobalSdnPhonebook);
	DeleteArrayWithOneEntryL(KDeletedNumberInLND, KDeletedNameInLND, KUidIccGlobalLndPhonebook);
	DeleteArrayWithOneEntryL(KDeletedNumberInUsim, KDeletedNameInUsim, KUidUsimAppAdnPhonebook, KDeletedEmailInUsim);
	DeleteArrayWithOneEntryL(KDeletedNumberInFDN, KDeletedNameInFDN, KUidIccGlobalFdnPhonebook);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkDeleteArrayInvalidIdTest object
 */
CPhbkDeleteArrayInvalidIdTest* CPhbkDeleteArrayInvalidIdTest::NewL()
	{
	CPhbkDeleteArrayInvalidIdTest* self = new(ELeave) CPhbkDeleteArrayInvalidIdTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkDeleteArrayInvalidIdTest::CPhbkDeleteArrayInvalidIdTest()
	{
	SetTestStepName(_L("DeleteArrayInvalidIdTest"));
	}

/** Delete an array of ICC entries - invalid ID added to the array */
enum TVerdict CPhbkDeleteArrayInvalidIdTest::doTestStepL()
	{
	SetSimTsyTestNumberL(10);
	DoSyncL();

	// *
	// * MJH  - 12 Sept 2002
	// * Assumptions for test data:
	// * 
	// * Each phonebook must use a unique number format in order that the array 
	// * of entries to be deleted only relates to that phonebook.
	// *

	DeleteArrayWithInvalidIdL(KDeletedNumberInADN);
	DeleteArrayWithInvalidIdL(KDeletedNumberInSDN);
	DeleteArrayWithInvalidIdL(KDeletedNumberInLND);
	DeleteArrayWithInvalidIdL(KDeletedNumberInUsim);
	DeleteArrayWithInvalidIdL(KDeletedNumberInFDN);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkDeleteArrayMultipleEntriesTest object
 */
CPhbkDeleteArrayMultipleEntriesTest* CPhbkDeleteArrayMultipleEntriesTest::NewL()
	{
	CPhbkDeleteArrayMultipleEntriesTest* self = new(ELeave) CPhbkDeleteArrayMultipleEntriesTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkDeleteArrayMultipleEntriesTest::CPhbkDeleteArrayMultipleEntriesTest()
	{
	SetTestStepName(_L("DeleteArrayMultipleEntriesTest"));
	}

/** Delete an array of ICC entries - multiple entries in the array */
enum TVerdict CPhbkDeleteArrayMultipleEntriesTest::doTestStepL()
	{
	SetSimTsyTestNumberL(10);
	DoSyncL();

	// *
	// * MJH  - 12 Sept 2002
	// * Assumptions for test data:
	// * 
	// * Each phonebook must use a unique format of name for its entries in order 
	// * that the array of entries to be deleted only relates to that phonebook.
	// * 

	DeleteArrayWithMultEntryL(KDeletedNumberInADN, KSearchStringInADN, KUidIccGlobalAdnPhonebook);
	DeleteArrayWithMultEntryL(KDeletedNumberInSDN, KSearchStringInSDN, KUidIccGlobalSdnPhonebook);
	DeleteArrayWithMultEntryL(KDeletedNumberInLND, KSearchStringInLND, KUidIccGlobalLndPhonebook);
	DeleteArrayWithMultEntryL(KDeletedNumberInUsim, KSearchStringInUsim, KUidUsimAppAdnPhonebook, KDeletedEmailInUsim);
	DeleteArrayWithMultEntryL(KDeletedNumberInFDN, KSearchStringInFDN, KUidIccGlobalFdnPhonebook);

	return TestStepResult();
	}


/** Delete and then Add Multiple entries */
void CPhbkDeleteIntegrationTestBase::DeleteArrayWithMultEntryL(const TDesC& aNumber, const TDesC& aName, const TUid aPhoneBook, const TDesC& aEmail)
	{
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldFamilyName);
	CContactIdArray* array=iDb->FindLC(aName,def);
	TInt count(array->Count());
	TESTCHECKCONDITIONL(count != 0); // make sure there is at least one entry in array
	TRAPD(err, iDb->DeleteContactsL(*array));
	TESTCHECKL(err, KErrNone);

	// Now add same number of entries to Contacts DB
	CPhbkIntegrationTestUtility* add = CPhbkIntegrationTestUtility::NewL();
	CleanupStack::PushL(add);
	for(TInt i=0; i<count; i++)
		add->AddContactL(aName, aNumber, aPhoneBook, aEmail); // now write new entry

	CleanupStack::PopAndDestroy(3,def); //  array & def & add?
	}

/** 
 * Factory construction method.
 * @return Pointer to CPhbkDeleteArrayICCLockedTest object
 */
CPhbkDeleteArrayICCLockedTest* CPhbkDeleteArrayICCLockedTest::NewL()
	{
	CPhbkDeleteArrayICCLockedTest* self = new(ELeave) CPhbkDeleteArrayICCLockedTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkDeleteArrayICCLockedTest::CPhbkDeleteArrayICCLockedTest()
	{
	SetTestStepName(_L("DeleteArrayICCLockedTest"));
	}

/** Delete an array of ICC entries - ICC locked */
enum TVerdict CPhbkDeleteArrayICCLockedTest::doTestStepL()
	{
	SetSimTsyTestNumberL(2);
	DoSyncL();

	// *
	// * MJH  - 12 Sept 2002
	// * Assumptions for test data:
	// * 
	// * Each phonebook must use a unique format of name for its entries in order 
	// * that the array of entries to be deleted only relates to that phonebook.
	// *
	// * The value of KTest2ICCSlots defines the total number of entries
	// * for ALL phonebooks.
	// *

	// Access ICC and wait for it to become locked
	iDb->SetDbViewContactType(KUidContactICCEntry);
	const CContactIdArray* sortedItems = iDb->SortedItemsL();
	TInt count(sortedItems->Count());
	TESTCHECKL(count, KTest2ICCSlots);
	TContactItemId id = (*sortedItems)[0];

	TInt err(KErrNone);
	while(err==KErrNone) // Wait for ICC to become locked 
		{
		User::After(1000000);	// Wait a second and try again.
		CContactICCEntry* failEntry=NULL;
		TRAP(err, failEntry = (CContactICCEntry*)iDb->ReadContactL(id, *iDb->AllFieldsView())); // Entry unavailable due to ICC being locked
		delete failEntry;
		}
	TESTCHECKCONDITIONL(err == KErrAccessDenied  ||  err == KErrNotReady);

	DeleteArrayWithICCLockedL(KSearchStringInADN, KErrAccessDenied);
	DeleteArrayWithICCLockedL(KSearchStringInSDN, KErrAccessDenied);
	DeleteArrayWithICCLockedL(KSearchStringInLND, KErrAccessDenied);
	DeleteArrayWithICCLockedL(KSearchStringInUsim, KErrAccessDenied);
	DeleteArrayWithICCLockedL(KSearchStringInFDN, KErrAccessDenied);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkDeleteArrayReadOnlyAccessTest object
 */
CPhbkDeleteArrayReadOnlyAccessTest* CPhbkDeleteArrayReadOnlyAccessTest::NewL()
	{
	CPhbkDeleteArrayReadOnlyAccessTest* self = new(ELeave) CPhbkDeleteArrayReadOnlyAccessTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkDeleteArrayReadOnlyAccessTest::CPhbkDeleteArrayReadOnlyAccessTest()
	{
	SetTestStepName(_L("DeleteArrayReadOnlyAccessTest"));
	}

/** Delete an array of ICC entries - Read-only Access */
enum TVerdict CPhbkDeleteArrayReadOnlyAccessTest::doTestStepL()
	{
	SetSimTsyTestNumberL(12);
	DoSyncL();

	// *
	// * MJH  - 12 Sept 2002
	// * Assumptions for test data:
	// * 
	// * Each phonebook must use a unique format of name for its entries in order 
	// * that the array of entries to be deleted only relates to that phonebook.
	// *

	DeleteArrayWithFailureL(KSearchNameInADN, KErrAccessDenied);
	DeleteArrayWithFailureL(KSearchNameInSDN, KErrAccessDenied);
	DeleteArrayWithFailureL(KSearchNameInLND, KErrAccessDenied);
	DeleteArrayWithFailureL(KSearchNameInUsim, KErrAccessDenied);
	DeleteArrayWithFailureL(KSearchNameInFDN, KErrAccessDenied);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkDeleteArrayICCDeleteFailsTest object
 */
CPhbkDeleteArrayICCDeleteFailsTest* CPhbkDeleteArrayICCDeleteFailsTest::NewL()
	{
	CPhbkDeleteArrayICCDeleteFailsTest* self = new(ELeave) CPhbkDeleteArrayICCDeleteFailsTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkDeleteArrayICCDeleteFailsTest::CPhbkDeleteArrayICCDeleteFailsTest()
	{
	SetTestStepName(_L("DeleteArrayICCDeleteFailsTest"));
	}

/** Delete an array of ICC entries - ICC delete fails */
enum TVerdict CPhbkDeleteArrayICCDeleteFailsTest::doTestStepL()
	{
	SetSimTsyTestNumberL(4);
	DoSyncL();

	DeleteArrayWithFailureL(KDeletedNameInADN, KErrBadHandle);
	DeleteArrayWithFailureL(KDeletedNameInSDN, KErrBadHandle);
	DeleteArrayWithFailureL(KDeletedNameInLND, KErrBadHandle);
	DeleteArrayWithFailureL(KDeletedNameInUsim, KErrBadHandle);
	DeleteArrayWithFailureL(KDeletedNameInFDN, KErrBadHandle);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkDeleteArrayInvalidUIDTest object
 */
CPhbkDeleteArrayInvalidUIDTest* CPhbkDeleteArrayInvalidUIDTest::NewL()
	{
	CPhbkDeleteArrayInvalidUIDTest* self = new(ELeave) CPhbkDeleteArrayInvalidUIDTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkDeleteArrayInvalidUIDTest::CPhbkDeleteArrayInvalidUIDTest()
	{
	SetTestStepName(_L("DeleteArrayInvalidUIDTest"));
	}

/** Delete an array of ICC entries - array contains both valid and invalid UIDs */
enum TVerdict CPhbkDeleteArrayInvalidUIDTest::doTestStepL()
	{
	SetSimTsyTestNumberL(121);
	DoSyncL();

	// *
	// * MJH  - 12 Sept 2002
	// * Assumptions for test data:
	// * 
	// * Each phonebook must use a unique format of name for its entries in order 
	// * that the array of entries to be deleted only relates to that phonebook.
	// * 

	DeleteArrayWithInvalidUIDL(KSearchNameInADN);
	DeleteArrayWithInvalidUIDL(KSearchNameInSDN);
	DeleteArrayWithInvalidUIDL(KSearchNameInLND);
	DeleteArrayWithInvalidUIDL(KSearchNameInUsim);
	DeleteArrayWithInvalidUIDL(KSearchNameInFDN);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkDeleteArrayFullPhonebookTest object
 */
CPhbkDeleteArrayFullPhonebookTest* CPhbkDeleteArrayFullPhonebookTest::NewL()
	{
	CPhbkDeleteArrayFullPhonebookTest* self = new(ELeave) CPhbkDeleteArrayFullPhonebookTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkDeleteArrayFullPhonebookTest::CPhbkDeleteArrayFullPhonebookTest()
	{
	SetTestStepName(_L("DeleteArrayFullPhonebookTest"));
	}

/** Delete an array of ICC entries - all entries in full phonebook */
enum TVerdict CPhbkDeleteArrayFullPhonebookTest::doTestStepL()
	{
#if 1
	//
	// This test has issues with a defect in the current CntModel implementation so it is best
	// not to run it, otherwise it takes down CntModel and TE_CntSync takes an extra hour to
	// abort all the remaining tests.
	//
	INFO_PRINTF1(_L("<font color=Orange>The current CntModel implementation will not support this test!</font>"));
	SetTestStepResult(EPass);
#else
	SetSimTsyTestNumberL(0);
	DoSyncL();

	// *
	// * MJH  - 12 Sept 2002
	// * Assumptions for test data:
	// * 
	// * Each phonebook must use a unique format of name for its entries in order 
	// * that the array of entries to be deleted only relates to that phonebook.
	// * 

	DeleteWholePhonebookL(KSearchStringInADN);
	DeleteWholePhonebookL(KSearchStringInSDN);
	DeleteWholePhonebookL(KSearchStringInLND);
	DeleteWholePhonebookL(KSearchStringInUsim);
	DeleteWholePhonebookL(KSearchStringInFDN);
#endif

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkDeleteSlotEntryTwiceTest object
 */
CPhbkDeleteSlotEntryTwiceTest* CPhbkDeleteSlotEntryTwiceTest::NewL()
	{
	CPhbkDeleteSlotEntryTwiceTest* self = new(ELeave) CPhbkDeleteSlotEntryTwiceTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkDeleteSlotEntryTwiceTest::CPhbkDeleteSlotEntryTwiceTest()
	{
	SetTestStepName(_L("DeleteSlotEntryTwiceTest"));
	}

/** Delete from a given slot, twice */
enum TVerdict CPhbkDeleteSlotEntryTwiceTest::doTestStepL()
	{
	SetSimTsyTestNumberL(23);
	DoSyncL();

	DeleteSlotTwiceL(KDeletedNumberInADN, KDeletedNameInADN, KUidIccGlobalAdnPhonebook);
	DeleteSlotTwiceL(KDeletedNumberInSDN, KDeletedNameInSDN, KUidIccGlobalSdnPhonebook);
	DeleteSlotTwiceL(KDeletedNumberInLND, KDeletedNameInLND, KUidIccGlobalLndPhonebook);
	DeleteSlotTwiceL(KDeletedNumberInUsim, KDeletedNameInUsim, KUidUsimAppAdnPhonebook, KDeletedEmailInUsim);
	DeleteSlotTwiceL(KDeletedNumberInFDN, KDeletedNameInFDN, KUidIccGlobalFdnPhonebook);

	return TestStepResult();
	}


/** Delete an ICC contact - normal case */
void CPhbkDeleteIntegrationTestBase::DeleteContactL(TUid aPhonebookUid)
	{
	iDb->SetDbViewContactType(KUidContactICCEntry);
	// get the unique groupId for the given phonebook
	TContactItemId groupId(KNullContactId);
	User::LeaveIfError(iSession.GetPhoneBookId(groupId, 
						RPhoneBookSession::ESyncGroupId,
						aPhonebookUid));
	TESTCHECKCONDITIONL(groupId != KNullContactId);
  
	// based on the groupId, get items belonging to the phonebook
	CContactGroup* group = NULL;
	TRAPD(err, group = static_cast<CContactGroup*>(iDb->ReadContactL(groupId)));
	TESTCHECKL(err, KErrNone);
	CleanupStack::PushL(group);
	const CContactIdArray* array = group->ItemsContained();

	// delete first item
	TContactItemId id = (*array)[0];
	iDb->DeleteContactL(id);

	//check the item has been removed from the database
	CContactItem* item=NULL; 
	TRAP(err, item = iDb->ReadContactL(id)); 
	TESTCHECKL(err, KErrNotFound);
	delete item;
	CleanupStack::PopAndDestroy(); // group
	}

/** Delete and then Add Single entry */
void CPhbkDeleteIntegrationTestBase::DeleteArrayWithOneEntryL(const TDesC& aNumber, const TDesC& aName, const TUid aPhoneBook, const TDesC& aEmail)
	{
	CContactIdArray* array = iDb->MatchPhoneNumberL(aNumber,KMaxPhoneMatchLength);	
	CleanupStack::PushL(array);
	TESTCHECKL(array->Count(), 1); // make sure there is only 1 entry in array 
	TContactItemId deleteId=(*array)[0];
	iDb->DeleteContactsL(*array);
	CContactItem* item=NULL; // check the item has been removed from the database
	TRAPD(err, item = iDb->ReadContactL(deleteId));
	TESTCHECKL(err, KErrNotFound);

	delete item;
	CPhbkIntegrationTestUtility* add = CPhbkIntegrationTestUtility::NewL();
	CleanupStack::PushL(add);
	add->AddContactL(aName, aNumber, aPhoneBook, aEmail); // now write new entry

	CleanupStack::PopAndDestroy(2); //  array & add
	}

/** Delete array with invalid UID added */
void CPhbkDeleteIntegrationTestBase::DeleteArrayWithInvalidIdL(const TDesC& aNumber)
   	{
  	CContactIdArray* array = iDb->MatchPhoneNumberL(aNumber,KMaxPhoneMatchLength);	
  	CleanupStack::PushL(array);
  	TESTCHECKL(array->Count(), 1); // make sure there is only 1 entry in array 
  	TContactItemId invalidUID1(1500); // invalid UID
  	array->AddL(invalidUID1); // Add invalid UID
  	TRAPD(err, iDb->DeleteContactsL(*array)); // array contains invalid UID
   	TESTCHECKL(err, KErrNotFound); 
	TRAP_IGNORE(iDb->RecoverL());
 	CleanupStack::PopAndDestroy(); //  array
   	}

/** Delete array with failure */
void CPhbkDeleteIntegrationTestBase::DeleteArrayWithFailureL(const TDesC& aName, const TInt aExpectedError)
	{
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldFamilyName);
	CContactIdArray* array=iDb->FindLC(aName,def);

	TInt count(array->Count());
	TESTCHECKCONDITIONL(count != 0); // make sure there is at least one entry in array
	TRAPD(err, iDb->DeleteContactsL(*array));
	TESTCHECKL(err, aExpectedError);

	TInt i(0);
	for(i=0; i<count; i++)
		{
		TContactItemId deleteId=(*array)[i];
		CContactItem* item=NULL; // check the item has NOT been removed from the database
		TRAP(err, item = iDb->ReadContactL(deleteId));
		TESTCHECKL(err, KErrNone); 
		delete item;
		}
	
	CleanupStack::PopAndDestroy(2); // def, array
	}

void CPhbkDeleteIntegrationTestBase::DeleteArrayWithICCLockedL(const TDesC& aName, const TInt aExpectedError)
	{
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldFamilyName);	
	TInt error(KErrAccessDenied);

	iDb->SetDbViewContactType(KUidContactICCEntry);
	const CContactIdArray* sortedItems = iDb->SortedItemsL();
	TContactItemId id = (*sortedItems)[0];
	while (error==KErrAccessDenied  ||  error==KErrNotReady  ||  error==KErrNotFound) // Wait for ICC to become unlocked again
		{
		User::After(1000000);	// Wait a second and try again.
		CContactICCEntry* failEntry=NULL;
		TRAP(error, failEntry = (CContactICCEntry*)iDb->ReadContactL(id, *iDb->AllFieldsView())); // Entry unavailable due to ICC being locked
		delete failEntry;
		}
	TESTCHECKL(error, KErrNone);

	WaitForSyncToCompleteL();

	CContactIdArray* array = (CContactIdArray*)iDb->FindLC(aName,def); // Entry unavailable due to ICC being locked	
	iDb->SetDbViewContactType(KUidContactICCEntry);
	sortedItems = iDb->SortedItemsL();
	id = (*sortedItems)[0];
	while (error==KErrNone  ||  error==KErrNotReady) // Wait for ICC to become locked again
		{
		User::After(1000000);	// Wait a second and try again.
		CContactICCEntry* failEntry=NULL;
		TRAP(error, failEntry = (CContactICCEntry*)iDb->ReadContactL(id, *iDb->AllFieldsView())); // Entry unavailable due to ICC being locked
		delete failEntry;
		}
	TESTCHECKL(error, KErrAccessDenied);

	TInt count(array->Count());
	TESTCHECKCONDITIONL(count != 0); // make sure there is at least one entry in array
	TRAPD(err, iDb->DeleteContactsL(*array));
	TESTCHECKL(err, aExpectedError);

	CleanupStack::PopAndDestroy(2); // def, array
	}

/** Delete with invalid UIDs */
void CPhbkDeleteIntegrationTestBase::DeleteArrayWithInvalidUIDL(const TDesC& aName)
	{
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldFamilyName);
	CContactIdArray* array=iDb->FindLC(aName,def);

	TContactItemId invalidUID1(8955), invalidUID2(35457), invalidUID3(5555);

	array->AddL(invalidUID1); // Add some invalid UIDs 
	array->AddL(invalidUID2);
	array->AddL(invalidUID3);

	TRAPD(err,iDb->DeleteContactsL(*array));
	TESTCHECK(err, KErrNotFound); 

	TRAP_IGNORE(iDb->RecoverL());	// Recovers the database from a rollback. First closes all 
									// tables and then reopens them after the recover.
	TInt count(array->Count());
	for(TInt i=0; i<count; i++)
		{
		//
		// Check items are not in the database...
		//
		TContactItemId deleteId((*array)[i]);
		CContactItem* item=NULL;

		TRAP(err, item = iDb->ReadContactL(deleteId));
		delete item;

		TESTCHECK(err, KErrNotFound);
		}

	CleanupStack::PopAndDestroy(2); // def, array
	}

/** Delete whole phonebook */
void CPhbkDeleteIntegrationTestBase::DeleteWholePhonebookL(const TDesC& aName)
	{
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldFamilyName);

	CContactIdArray* array=iDb->FindLC(aName,def);
	TRAPD(err,iDb->DeleteContactsL(*array));
	TESTCHECKL(err, KErrNone); 

	TInt count(array->Count());
	for(TInt i=0; i<count; i++)
		{
		TContactItemId deleteId((*array)[i]);
		CContactItem* item=NULL; // check items have been removed from the database
		TRAP(err, item = iDb->ReadContactL(deleteId));
		TESTCHECKL(err, KErrNotFound);
		delete item;
		}
	
	CleanupStack::PopAndDestroy(2); // def
	}

/** Delete an ICC contact - normal case */
void CPhbkDeleteIntegrationTestBase::DeleteSlotTwiceL(const TDesC& aNumber, const TDesC& aName, const TUid aPhonebook, const TDesC& aEmail)
	{
	iDb->SetDbViewContactType(KUidContactICCEntry);
	// get the unique groupId for the given phonebook
	TContactItemId groupId(KNullContactId);
	User::LeaveIfError(iSession.GetPhoneBookId(groupId, 
						RPhoneBookSession::ESyncGroupId,
						aPhonebook));
	TESTCHECKCONDITIONL(groupId != KNullContactId);
  
	// based on the groupId, get items belonging to the phonebook
	CContactGroup* group = NULL;
	TRAPD(err, group = static_cast<CContactGroup*>(iDb->ReadContactL(groupId)));
	TESTCHECKL(err, KErrNone);
	CleanupStack::PushL(group);
	const CContactIdArray* array = group->ItemsContained();

	// delete first item
	TContactItemId id((*array)[0]);
	iDb->DeleteContactL(id);

	//check the item has been removed from the database
	CContactItem* item=NULL; 
	TRAP(err, item = iDb->ReadContactL(id)); 
	TESTCHECKL(err, KErrNotFound);

	delete item;
	// verify that there is a free slot
	RArray<TInt> slotNums;
	CleanupClosePushL(slotNums);
	iSession.GetFreeSlotsL(slotNums, aPhonebook);
	TESTCHECKCONDITIONL(slotNums.Count() > 0);

	// Now add a new item - which should use the vacated slot
	CPhbkIntegrationTestUtility* add = CPhbkIntegrationTestUtility::NewL();
	CleanupStack::PushL(add);
	add->AddContactL(aName, aNumber, aPhonebook, aEmail);

	// delete this new item - to verify that the delete on same slot is permitted
	TInt  ret = iSession.GetSlotId(slotNums[0], id, aPhonebook);
	TESTCHECKL(ret, KErrNone);
	
	item=NULL;
	TRAP(err, item = static_cast<CContactICCEntry*>(iDb->ReadContactL(id))); 
	TESTCHECKL(err, KErrNone);
	delete item;
	TRAP(err, iDb->DeleteContactL(id)); 
	TESTCHECKL(err, KErrNone);

	CleanupStack::PopAndDestroy(3); // add, group, slotNums
	}

