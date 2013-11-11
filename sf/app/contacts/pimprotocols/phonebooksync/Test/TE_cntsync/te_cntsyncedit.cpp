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

#include <e32test.h>
#include <exterror.h>

#include "te_cntsyncbase.h"
#include "te_cntsyncedit.h"

_LIT(KModifiedNameInADN,  "ADN Modified Name");
_LIT(KModifiedNameInSDN,  "SDN Modified Name");
_LIT(KModifiedNameInLND,  "LND Modified Name");
_LIT(KModifiedNameInUsim, "Usim Modified Name");
_LIT(KModifiedNameInFDN,  "FDN Modified Name");

_LIT(KModifiedNumberInADN, "05050505050505");
_LIT(KModifiedNumberInSDN, "15151515151515");
_LIT(KModifiedNumberInLND, "25252525252525");
_LIT(KModifiedNumberInUsim,"35353535353535");
_LIT(KModifiedNumberInFDN, "45454545454545");

_LIT(KModifiedEmailInUsim,"modEmail@moddomain");

_LIT(KModifiedNameExceeded,"ABChfjdhfjhdfjhjfhjdhfjhfjhd");
_LIT(KModifiedNumberExceeded,"02020300000222222222222222222222220000032232222222222222323");
 
_LIT(KTestEmpty,"");

_LIT(KModifiedEmptyNameInADN, "ADN Modified Empty");
_LIT(KModifiedEmptyNameInSDN, "SDN Modified Empty");
_LIT(KModifiedEmptyNameInLND, "LND Modified Empty");
_LIT(KModifiedEmptyNameInUsim,"Usim Modified Empty");
_LIT(KModifiedEmptyNameInFDN, "FDN Modified Empty");

_LIT(KModifiedEmptyNumberInADN, "00000007878");
_LIT(KModifiedEmptyNumberInSDN, "11111117878");
_LIT(KModifiedEmptyNumberInLND, "22222227878");
_LIT(KModifiedEmptyNumberInUsim,"33333337878");
_LIT(KModifiedEmptyNumberInFDN, "44444447878");

_LIT(KModifiedEmptyEmailInUsim,"modEmptyEmail@modEmptydomain");


/** 
 * Factory construction method.
 * @return Pointer to CPhbkOpenICCContactTest object
 */
CPhbkOpenICCContactTest* CPhbkOpenICCContactTest::NewL()
	{
	CPhbkOpenICCContactTest* self = new(ELeave) CPhbkOpenICCContactTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkOpenICCContactTest::CPhbkOpenICCContactTest()
	{
	SetTestStepName(_L("OpenICCContactTest"));
	}

/** Open an ICC entry for edit - normal case */
enum TVerdict CPhbkOpenICCContactTest::doTestStepL()
	{
	SetSimTsyTestNumberL(0);
	DoSyncL();
	// Edited fields are NOT committed to database, hence EFalse
	OpenContactEditFieldsL(KUidIccGlobalAdnPhonebook, 0, EFalse, KModifiedNameInADN, KModifiedNumberInADN);
	OpenContactEditFieldsL(KUidIccGlobalSdnPhonebook, 0, EFalse, KModifiedNameInSDN, KModifiedNumberInSDN);
	OpenContactEditFieldsL(KUidIccGlobalLndPhonebook, 0, EFalse, KModifiedNameInLND, KModifiedNumberInLND);
	OpenContactEditFieldsL(KUidUsimAppAdnPhonebook, 0, EFalse, KModifiedNameInUsim, KModifiedNumberInUsim, KModifiedEmailInUsim);
	OpenContactEditFieldsL(KUidIccGlobalFdnPhonebook, 0, EFalse, KModifiedNameInFDN, KModifiedNumberInFDN);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkOpenContactInvalidUIDTest object
 */
CPhbkOpenContactInvalidUIDTest* CPhbkOpenContactInvalidUIDTest::NewL()
	{
	CPhbkOpenContactInvalidUIDTest* self = new(ELeave) CPhbkOpenContactInvalidUIDTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkOpenContactInvalidUIDTest::CPhbkOpenContactInvalidUIDTest()
	{
	SetTestStepName(_L("OpenContactInvalidUIDTest"));
	}

/** Open an ICC entry for edit - invalid UID */
enum TVerdict CPhbkOpenContactInvalidUIDTest::doTestStepL()
	{
	TContactItemId id(-7); // Negative UID
	TRAPD(err, iDb->OpenContactLX(id));
	TESTCHECKL(err, KErrNotFound);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkEditICCContactTest object
 */
CPhbkEditICCContactTest* CPhbkEditICCContactTest::NewL()
	{
	CPhbkEditICCContactTest* self = new(ELeave) CPhbkEditICCContactTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkEditICCContactTest::CPhbkEditICCContactTest()
	{
	SetTestStepName(_L("EditICCContactTest"));
	}

/** Edit an ICC entry - normal case */
enum TVerdict CPhbkEditICCContactTest::doTestStepL()
	{
	SetSimTsyTestNumberL(0);
	DoSyncL();
	// Edited fields are committed to database, hence ETrue
	OpenContactEditFieldsL(KUidIccGlobalAdnPhonebook, 0, ETrue, KModifiedNameInADN, KModifiedNumberInADN);
	OpenContactEditFieldsL(KUidIccGlobalSdnPhonebook, 0, ETrue, KModifiedNameInSDN, KModifiedNumberInSDN);
	OpenContactEditFieldsL(KUidIccGlobalLndPhonebook, 0, ETrue, KModifiedNameInLND, KModifiedNumberInLND);
	OpenContactEditFieldsL(KUidUsimAppAdnPhonebook, 0, ETrue, KModifiedNameInUsim, KModifiedNumberInUsim, KModifiedEmailInUsim);
	OpenContactEditFieldsL(KUidIccGlobalFdnPhonebook, 0, ETrue, KModifiedNameInFDN, KModifiedNumberInFDN);

	return TestStepResult();
	}

/** 
 * Factory construction method.
 * @return Pointer to CPhbkEditICCContactTwiceTest object
 */
CPhbkEditICCContactTwiceTest* CPhbkEditICCContactTwiceTest::NewL()
	{
	CPhbkEditICCContactTwiceTest* self = new(ELeave) CPhbkEditICCContactTwiceTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkEditICCContactTwiceTest::CPhbkEditICCContactTwiceTest()
	{
	SetTestStepName(_L("EditICCContactTwiceTest"));
	}

/** Edit an ICC entry twice */
enum TVerdict CPhbkEditICCContactTwiceTest::doTestStepL()
	{
	// close existing phbksync session and contact database
	delete iDb;
	iDb=NULL;
	iSession.Close();
	ConfigurePhbkSyncToIdleL();
	User::After(3000000); //3 second wait for contact to shut down

	// Mark the heap before re-starting and performing two edits
	__UHEAP_MARK;

	ConfigurePhbkSyncToFullL();
	User::LeaveIfError(iSession.Connect());
	User::After(3000000); //3 second wait for server to start
	iDb = OpenOrCreateContactDatabaseL();
	User::After(3000000); //3 second wait for contact to start the phbk server
	DoSyncL(); // synchronise

	// Edit entry for each phonebook - note these are committed to database, hence ETrue
	OpenContactEditFieldsL(KUidIccGlobalAdnPhonebook, 0, ETrue, KModifiedNameInADN, KModifiedNumberInADN);
	OpenContactEditFieldsL(KUidIccGlobalSdnPhonebook, 0, ETrue, KModifiedNameInSDN, KModifiedNumberInSDN);
	OpenContactEditFieldsL(KUidIccGlobalLndPhonebook, 0, ETrue, KModifiedNameInLND, KModifiedNumberInLND);
	OpenContactEditFieldsL(KUidUsimAppAdnPhonebook, 0, ETrue, KModifiedNameInUsim, KModifiedNumberInUsim, KModifiedEmailInUsim);
	OpenContactEditFieldsL(KUidIccGlobalFdnPhonebook, 0, ETrue, KModifiedNameInFDN, KModifiedNumberInFDN);

	// Edit same entries again (using different name to ensure entry is changed)
	OpenContactEditFieldsL(KUidIccGlobalAdnPhonebook, 0, ETrue, KModifiedEmptyNameInADN, KModifiedNumberInADN);
	OpenContactEditFieldsL(KUidIccGlobalSdnPhonebook, 0, ETrue, KModifiedEmptyNameInSDN, KModifiedNumberInSDN);
	OpenContactEditFieldsL(KUidIccGlobalLndPhonebook, 0, ETrue, KModifiedEmptyNameInLND, KModifiedNumberInLND);
	OpenContactEditFieldsL(KUidUsimAppAdnPhonebook, 0, ETrue, KModifiedEmptyNameInUsim, KModifiedNumberInUsim, KModifiedEmailInUsim);
	OpenContactEditFieldsL(KUidIccGlobalFdnPhonebook, 0, ETrue, KModifiedEmptyNameInFDN, KModifiedNumberInFDN);

	// close existing phbksync session and contact database
	delete iDb;
	iDb=NULL;
	iSession.Close();
	ConfigurePhbkSyncToIdleL();
	User::After(3000000); //3 second wait for contact to shut down

	// Test the heap is okay
	__UHEAP_MARKEND;
	ConfigurePhbkSyncToFullL();
	User::LeaveIfError(iSession.Connect());
	User::After(3000000); //3 second wait for server to start

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkEditContactEmptyNameTest object
 */
CPhbkEditContactEmptyNameTest* CPhbkEditContactEmptyNameTest::NewL()
	{
	CPhbkEditContactEmptyNameTest* self = new(ELeave) CPhbkEditContactEmptyNameTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkEditContactEmptyNameTest::CPhbkEditContactEmptyNameTest()
	{
	SetTestStepName(_L("EditContactEmptyNameTest"));
	}

/** Edit an ICC entry - empty name field */
enum TVerdict CPhbkEditContactEmptyNameTest::doTestStepL()
	{
	SetSimTsyTestNumberL(1);
	DoSyncL();

	// Edited fields are committed to database, hence ETrue
	OpenContactEditFieldsL(KUidIccGlobalAdnPhonebook, 4, ETrue, KTestEmpty, KModifiedNumberInADN);
	OpenContactEditFieldsL(KUidIccGlobalSdnPhonebook, 4, ETrue, KTestEmpty, KModifiedNumberInSDN);
	OpenContactEditFieldsL(KUidIccGlobalLndPhonebook, 4, ETrue, KTestEmpty, KModifiedNumberInLND);
	OpenContactEditFieldsL(KUidUsimAppAdnPhonebook, 4, ETrue, KTestEmpty, KModifiedNumberInUsim, KModifiedEmailInUsim);
	OpenContactEditFieldsL(KUidIccGlobalFdnPhonebook, 4, ETrue, KTestEmpty, KModifiedNumberInFDN);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkEditContactEmptyNumberTest object
 */
CPhbkEditContactEmptyNumberTest* CPhbkEditContactEmptyNumberTest::NewL()
	{
	CPhbkEditContactEmptyNumberTest* self = new(ELeave) CPhbkEditContactEmptyNumberTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkEditContactEmptyNumberTest::CPhbkEditContactEmptyNumberTest()
	{
	SetTestStepName(_L("EditContactEmptyNumberTest"));
	}

/** Edit an ICC entry - empty number field */
enum TVerdict CPhbkEditContactEmptyNumberTest::doTestStepL()
	{
	SetSimTsyTestNumberL(1);
	DoSyncL();

	// Edited fields are committed to database, hence ETrue
	OpenContactEditFieldsL(KUidIccGlobalAdnPhonebook, 3, ETrue, KModifiedNameInADN, KTestEmpty);
	OpenContactEditFieldsL(KUidIccGlobalSdnPhonebook, 3, ETrue, KModifiedNameInSDN, KTestEmpty);
	OpenContactEditFieldsL(KUidIccGlobalLndPhonebook, 3, ETrue, KModifiedNameInLND, KTestEmpty);
	OpenContactEditFieldsL(KUidUsimAppAdnPhonebook, 3, ETrue, KModifiedNameInUsim, KTestEmpty, KModifiedEmailInUsim);
	OpenContactEditFieldsL(KUidIccGlobalFdnPhonebook, 3, ETrue, KModifiedNameInFDN, KTestEmpty);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkEditContactNameExceededTest object
 */
CPhbkEditContactNameExceededTest* CPhbkEditContactNameExceededTest::NewL()
	{
	CPhbkEditContactNameExceededTest* self = new(ELeave) CPhbkEditContactNameExceededTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkEditContactNameExceededTest::CPhbkEditContactNameExceededTest()
	{
	SetTestStepName(_L("EditContactNameExceededTest"));
	}

/** Edit an ICC entry - name boundary condition exceeded */
enum TVerdict CPhbkEditContactNameExceededTest::doTestStepL()
	{
	SetSimTsyTestNumberL(0);
	DoSyncL();
	
	// Edited fields are committed to database, hence ETrue
	TRAPD(err, OpenContactEditFieldsL(KUidIccGlobalAdnPhonebook, 0, ETrue, KModifiedNameExceeded, KModifiedNumberInADN));
	TESTCHECKL(err, KErrPhonebookTextOverflow);

	TRAP(err, OpenContactEditFieldsL(KUidIccGlobalSdnPhonebook, 0, ETrue, KModifiedNameExceeded, KModifiedNumberInSDN));
	TESTCHECKL(err, KErrPhonebookTextOverflow);

	TRAP(err, OpenContactEditFieldsL(KUidIccGlobalLndPhonebook, 0, ETrue, KModifiedNameExceeded, KModifiedNumberInLND));
	TESTCHECKL(err, KErrPhonebookTextOverflow);

	TRAP(err, OpenContactEditFieldsL(KUidUsimAppAdnPhonebook, 0, ETrue, KModifiedNameExceeded, KModifiedNumberInUsim, KModifiedEmailInUsim));
	TESTCHECKL(err, KErrPhonebookTextOverflow);

	TRAP(err, OpenContactEditFieldsL(KUidIccGlobalFdnPhonebook, 0, ETrue, KModifiedNameExceeded, KModifiedNumberInFDN));
	TESTCHECKL(err, KErrPhonebookTextOverflow);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkEditContactNumberExceededTest object
 */
CPhbkEditContactNumberExceededTest* CPhbkEditContactNumberExceededTest::NewL()
	{
	CPhbkEditContactNumberExceededTest* self = new(ELeave) CPhbkEditContactNumberExceededTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkEditContactNumberExceededTest::CPhbkEditContactNumberExceededTest()
	{
	SetTestStepName(_L("EditContactNumberExceededTest"));
	}

/** Edit an ICC entry - number boundary condition exceeded */
enum TVerdict CPhbkEditContactNumberExceededTest::doTestStepL()
	{
	SetSimTsyTestNumberL(0);
	DoSyncL();
	// Edited fields are committed to database, hence ETrue
	TRAPD(err, OpenContactEditFieldsL(KUidIccGlobalAdnPhonebook, 0, ETrue, KModifiedNameInADN, KModifiedNumberExceeded));
	TESTCHECKL(err, KErrPhonebookNumberOverflow);

	TRAP(err, OpenContactEditFieldsL(KUidIccGlobalSdnPhonebook, 0, ETrue, KModifiedNameInSDN, KModifiedNumberExceeded));
	TESTCHECKL(err, KErrPhonebookNumberOverflow);

	TRAP(err, OpenContactEditFieldsL(KUidIccGlobalLndPhonebook, 0, ETrue, KModifiedNameInLND, KModifiedNumberExceeded));
	TESTCHECKL(err, KErrPhonebookNumberOverflow);

	TRAP(err, OpenContactEditFieldsL(KUidUsimAppAdnPhonebook, 0, ETrue, KModifiedNameInUsim, KModifiedNumberExceeded, KModifiedEmailInUsim));
	TESTCHECKL(err, KErrPhonebookNumberOverflow);

	TRAP(err, OpenContactEditFieldsL(KUidIccGlobalFdnPhonebook, 0, ETrue, KModifiedNameInFDN, KModifiedNumberExceeded));
	TESTCHECKL(err, KErrPhonebookNumberOverflow);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkEditContactICCLockedTest object
 */
CPhbkEditContactICCLockedTest* CPhbkEditContactICCLockedTest::NewL()
	{
	CPhbkEditContactICCLockedTest* self = new(ELeave) CPhbkEditContactICCLockedTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkEditContactICCLockedTest::CPhbkEditContactICCLockedTest()
	{
	SetTestStepName(_L("EditContactICCLockedTest"));
	}

/** Edit an ICC entry - ICC locked */
enum TVerdict CPhbkEditContactICCLockedTest::doTestStepL()
	{
	SetSimTsyTestNumberL(2);
	DoSyncL();

	OpenContactEditLockedL(KUidIccGlobalAdnPhonebook, 4, KModifiedNumberInADN);
	OpenContactEditLockedL(KUidIccGlobalSdnPhonebook, 4, KModifiedNumberInSDN);
	OpenContactEditLockedL(KUidIccGlobalLndPhonebook, 4, KModifiedNumberInLND);
	OpenContactEditLockedL(KUidUsimAppAdnPhonebook, 4, KModifiedNumberInUsim);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkEditContactICCWriteFailsTest object
 */
CPhbkEditContactICCWriteFailsTest* CPhbkEditContactICCWriteFailsTest::NewL()
	{
	CPhbkEditContactICCWriteFailsTest* self = new(ELeave) CPhbkEditContactICCWriteFailsTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkEditContactICCWriteFailsTest::CPhbkEditContactICCWriteFailsTest()
	{
	SetTestStepName(_L("EditContactICCWriteFailsTest"));
	}

/** Edit an ICC entry - ICC write fails */
enum TVerdict CPhbkEditContactICCWriteFailsTest::doTestStepL()
	{
	SetSimTsyTestNumberL(4);
	DoSyncL();

	// Edited fields are committed to database, hence ETrue
	TRAPD(err, OpenContactEditFieldsL(KUidIccGlobalAdnPhonebook, 0, ETrue, KModifiedNameInADN, KModifiedNumberInADN));
	TESTCHECKL(err, KErrBadHandle);

	TRAP(err, OpenContactEditFieldsL(KUidIccGlobalSdnPhonebook, 0, ETrue, KModifiedNameInSDN, KModifiedNumberInSDN));
	TESTCHECKL(err, KErrBadHandle);

	TRAP(err, OpenContactEditFieldsL(KUidIccGlobalLndPhonebook, 0, ETrue, KModifiedNameInLND, KModifiedNumberInLND));
	TESTCHECKL(err, KErrBadHandle);

	TRAP(err, OpenContactEditFieldsL(KUidUsimAppAdnPhonebook, 0, ETrue, KModifiedNameInUsim, KModifiedNumberInUsim, KModifiedEmailInUsim));
	TESTCHECKL(err, KErrBadHandle);

	TRAP(err, OpenContactEditFieldsL(KUidIccGlobalFdnPhonebook, 0, ETrue, KModifiedNameInFDN, KModifiedNumberInFDN));
	TESTCHECKL(err, KErrBadHandle);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkEditContactEmptyFieldsTest object
 */
CPhbkEditContactEmptyFieldsTest* CPhbkEditContactEmptyFieldsTest::NewL()
	{
	CPhbkEditContactEmptyFieldsTest* self = new(ELeave) CPhbkEditContactEmptyFieldsTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkEditContactEmptyFieldsTest::CPhbkEditContactEmptyFieldsTest()
	{
	SetTestStepName(_L("EditContactEmptyFieldsTest"));
	}

/** Open and Edit an ICC entry - empty name and number fields */
enum TVerdict CPhbkEditContactEmptyFieldsTest::doTestStepL()
	{
	SetSimTsyTestNumberL(6);
	DoSyncL();

	// *
	// * MJH  - 12 Sept 2002
	// * Assumptions for test data:
	// * 
	// * The fifth entry for each phonebook has no name
	// * The sixth entry for each phonebook has no number
	// * The sixth entry for Usim App ADN phonebook has no email
	// * 

	// Edited fields are NOT committed to database, hence EFalse
	OpenContactEditFieldsL(KUidIccGlobalAdnPhonebook, 4, EFalse, KModifiedEmptyNameInADN, KModifiedEmptyNumberInADN);
	OpenContactEditFieldsL(KUidIccGlobalSdnPhonebook, 4, EFalse, KModifiedEmptyNameInSDN, KModifiedEmptyNumberInSDN);
	OpenContactEditFieldsL(KUidIccGlobalLndPhonebook, 4, EFalse, KModifiedEmptyNameInLND, KModifiedEmptyNumberInLND);
	OpenContactEditFieldsL(KUidUsimAppAdnPhonebook, 4, EFalse, KModifiedEmptyNameInUsim, KModifiedEmptyNumberInUsim, KModifiedEmptyEmailInUsim);
	OpenContactEditFieldsL(KUidIccGlobalFdnPhonebook, 4, EFalse, KModifiedEmptyNameInFDN, KModifiedEmptyNumberInFDN);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkEditSlotWriteTest object
 */
CPhbkEditSlotWriteTest* CPhbkEditSlotWriteTest::NewL()
	{
	CPhbkEditSlotWriteTest* self = new(ELeave) CPhbkEditSlotWriteTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkEditSlotWriteTest::CPhbkEditSlotWriteTest()
	{
	SetTestStepName(_L("EditSlotWriteTest"));
	}

/** Edit entry based on slot number */
enum TVerdict CPhbkEditSlotWriteTest::doTestStepL()
	{
	SetSimTsyTestNumberL(21);
	DoSyncL();

	CheckEditSlotWriteL(KUidIccGlobalAdnPhonebook);
	CheckEditSlotWriteL(KUidIccGlobalSdnPhonebook);
	CheckEditSlotWriteL(KUidIccGlobalLndPhonebook);
	CheckEditSlotWriteL(KUidUsimAppAdnPhonebook);
	CheckEditSlotWriteL(KUidIccGlobalFdnPhonebook);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkOpenContactICCLockedTest object
 */
CPhbkOpenContactICCLockedTest* CPhbkOpenContactICCLockedTest::NewL()
	{
	CPhbkOpenContactICCLockedTest* self = new(ELeave) CPhbkOpenContactICCLockedTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkOpenContactICCLockedTest::CPhbkOpenContactICCLockedTest()
	{
	SetTestStepName(_L("OpenContactICCLockedTest"));
	}

/** Open an ICC entry - ICC locked */
enum TVerdict CPhbkOpenContactICCLockedTest::doTestStepL()
	{
	SetSimTsyTestNumberL(2);
	DoSyncL();

	// Access ICC and wait for it to become locked
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

	// *
	// * MJH  - 12 Sept 2002
	// * Assumptions for test data:
	// * 
	// * Each phonebook has at least five entries
	// * 

	// Edited fields are committed to database, hence ETrue
	TRAP(err, OpenContactEditFieldsL(KUidIccGlobalAdnPhonebook, 4, ETrue, KModifiedNameInADN, KModifiedNumberInADN));
	TESTCHECKL(err, KErrAccessDenied);

	TRAP(err, OpenContactEditFieldsL(KUidIccGlobalSdnPhonebook, 4, ETrue, KModifiedNameInSDN, KModifiedNumberInSDN));
	TESTCHECKL(err, KErrAccessDenied);

	TRAP(err, OpenContactEditFieldsL(KUidIccGlobalLndPhonebook, 4, ETrue, KModifiedNameInLND, KModifiedNumberInLND));
	TESTCHECKL(err, KErrAccessDenied);

	TRAP(err, OpenContactEditFieldsL(KUidUsimAppAdnPhonebook, 4, ETrue, KModifiedNameInUsim, KModifiedNumberInUsim));
	TESTCHECKL(err, KErrAccessDenied);

	TRAP(err, OpenContactEditFieldsL(KUidIccGlobalFdnPhonebook, 4, ETrue, KModifiedNameInFDN, KModifiedNumberInFDN));
	TESTCHECKL(err, KErrAccessDenied);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkOpenEntryReadOnlyAccessTest object
 */
CPhbkOpenEntryReadOnlyAccessTest* CPhbkOpenEntryReadOnlyAccessTest::NewL()
	{
	CPhbkOpenEntryReadOnlyAccessTest* self = new(ELeave) CPhbkOpenEntryReadOnlyAccessTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkOpenEntryReadOnlyAccessTest::CPhbkOpenEntryReadOnlyAccessTest()
	{
	SetTestStepName(_L("OpenEntryReadOnlyAccessTest"));
	}

/* Open an ICC entry - read-only phonebook */
enum TVerdict CPhbkOpenEntryReadOnlyAccessTest::doTestStepL()
	{
	SetSimTsyTestNumberL(12);
	DoSyncL();

	// *
	// * MJH  - 12 Sept 2002
	// * Assumptions for test data:
	// * 
	// * Each phonebook has at least two entries
	// * 

	// Edited fields are committed to database, hence ETrue
	TRAPD(err, OpenContactEditFieldsL(KUidIccGlobalAdnPhonebook, 0, ETrue, KModifiedNameInADN, KModifiedNumberInADN));
	TESTCHECKL(err, KErrAccessDenied);

	TRAP(err, OpenContactEditFieldsL(KUidIccGlobalSdnPhonebook, 0, ETrue, KModifiedNameInSDN, KModifiedNumberInSDN));
	TESTCHECKL(err, KErrAccessDenied);

	TRAP(err, OpenContactEditFieldsL(KUidIccGlobalLndPhonebook, 0, ETrue, KModifiedNameInLND, KModifiedNumberInLND));
	TESTCHECKL(err, KErrAccessDenied);

	TRAP(err, OpenContactEditFieldsL(KUidUsimAppAdnPhonebook, 0, ETrue, KModifiedNameInUsim, KModifiedNumberInUsim));
	TESTCHECKL(err, KErrAccessDenied);

	TRAP(err, OpenContactEditFieldsL(KUidIccGlobalFdnPhonebook, 0, ETrue, KModifiedNameInFDN, KModifiedNumberInFDN));
	TESTCHECKL(err, KErrAccessDenied);

	return TestStepResult();
	}


/** 
 * Replace the current name field with another name
 * @param aItem Reference to CContactItem
 * @param aModifiedName New name to use
 */
void CPhbkEditIntegrationTestBase::ChangeNameFieldL(CContactItem& aItem, const TDesC& aModifiedName)
	{
	CContactItemFieldSet& fieldset = aItem.CardFields();
	const TInt pos(fieldset.Find(KUidContactFieldFamilyName));	
	CContactItemField& field = fieldset[pos];
	CContactTextField* textfield = field.TextStorage();
	textfield->SetTextL(aModifiedName);
	}

/** 
 * Replace the current number field with another number
 * @param aItem Reference to CContactItem
 * @param aModifiedNumber New number to use
 */
void CPhbkEditIntegrationTestBase::ChangeNumberFieldL(CContactItem& aItem, const TDesC& aModifiedNumber)
	{
	CContactItemFieldSet& fieldset = aItem.CardFields();
	const TInt pos(fieldset.Find(KUidContactFieldPhoneNumber));
	CContactItemField& field = fieldset[pos];
	CContactTextField* textfield = field.TextStorage();
	textfield->SetTextL(aModifiedNumber);
	}

/** 
 * Replace the current email field with another email
 * @param aItem Reference to CContactItem
 * @param aModifiedEmail New email to use
 */
void CPhbkEditIntegrationTestBase::ChangeEmailFieldL(CContactItem& aItem, const TDesC& aModifiedEmail)
	{
	CContactItemFieldSet& fieldset = aItem.CardFields();
	const TInt pos(fieldset.Find(KUidContactFieldEMail));
	CContactItemField& field = fieldset[pos];
	CContactTextField* textfield = field.TextStorage();
	textfield->SetTextL(aModifiedEmail);
	}

/** Open an ICC contact for edit */
void CPhbkEditIntegrationTestBase::OpenContactEditFieldsL(TUid aPhonebookUid, TInt aIndex, TBool aCommit, const TDesC& aModifiedName, const TDesC& aModifiedNumber, const TDesC& aModifiedEmail)
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

	// Obtain the UID for the indexed entry
	TContactItemId id = (*array)[aIndex];

	// Open the entry and attempt to edit
	CContactICCEntry* entry = static_cast<CContactICCEntry*>(iDb->OpenContactLX(id));
	CleanupStack::PushL(entry);
	ChangeNumberFieldL(*entry,aModifiedNumber);
	ChangeNameFieldL(*entry,aModifiedName);
	if (aPhonebookUid == KUidUsimAppAdnPhonebook)
		ChangeEmailFieldL(*entry,aModifiedEmail);

	// Commit to database if required
	if (aCommit)
		iDb->CommitContactL(*entry);
	else
		iDb->CloseContactL(id);

	CleanupStack::PopAndDestroy(3); // group, entry & lockrecord
	}

/** Open an ICC contact - edit is locked */
void CPhbkEditIntegrationTestBase::OpenContactEditLockedL(TUid aPhonebookUid, TInt aIndex, const TDesC& aModifiedNumber)
	{
	iDb->SetDbViewContactType(KUidContactICCEntry);
	// get the unique groupId for the given phonebook
	TContactItemId groupId(KNullContactId);
	User::LeaveIfError(iSession.GetPhoneBookId(groupId, 
						RPhoneBookSession::ESyncGroupId,
						aPhonebookUid));
	TESTCHECKCONDITIONL(groupId != KNullContactId);
  
	const CContactIdArray* sortedItems = iDb->SortedItemsL();
	TContactItemId id = (*sortedItems)[0];
	TInt error(KErrAccessDenied);
	while (error==KErrAccessDenied  ||  error==KErrNotReady  ||  error==KErrNotFound) // Wait for ICC to become unlocked again
		{
		User::After(1000000);	// Wait a second and try again.
		CContactICCEntry* failEntry=NULL;
		TRAP(error, failEntry = (CContactICCEntry*)iDb->ReadContactL(id, *iDb->AllFieldsView())); // Entry unavailable due to ICC being locked
		delete failEntry;
		}
	TESTCHECKL(error, KErrNone);

	WaitForSyncToCompleteL();

	// based on the groupId, get items belonging to the phonebook
	CContactGroup* group = NULL;
	TRAPD(err, group = static_cast<CContactGroup*>(iDb->ReadContactL(groupId)));
	TESTCHECKL(err, KErrNone);
	CleanupStack::PushL(group);
	const CContactIdArray* array = group->ItemsContained();

	// Obtain the UID for the indexed entry
	TContactItemId id2 = (*array)[aIndex];

	// Open the entry
	CContactICCEntry* entry = static_cast<CContactICCEntry*>(iDb->OpenContactLX(id2));
	CleanupStack::PushL(entry);
	
	while(error==KErrNone) // Wait for ICC to become locked again
		{
		User::After(1000000);	// Wait a second and try again.
		CContactICCEntry* failEntry=NULL;
		TRAP(error, failEntry = (CContactICCEntry*)iDb->ReadContactL(id, *iDb->AllFieldsView())); // Entry unavailable due to ICC being locked
		delete failEntry;
		}
	TESTCHECKCONDITIONL(error==KErrAccessDenied  ||  error==KErrNotReady);

	ChangeNumberFieldL(*entry, aModifiedNumber);
	TRAP(err, iDb->CommitContactL(*entry));
	TESTCHECKL(err, KErrAccessDenied);

	CleanupStack::PopAndDestroy(3); // group, entry & lockrecord
	}

/** Check edit based on slot number */
void CPhbkEditIntegrationTestBase::CheckEditSlotWriteL(const TUid aPhonebook)
	{
	TInt aSlot;
	TContactItemId aId=KNullContactId;
	CPhbkIntegrationTestUtility* add = CPhbkIntegrationTestUtility::NewL();
	_LIT(Kslot17,"17");
	_LIT(Kslot18,"18");

	//test add new contact at specified slot
	TInt matchingId;
	aSlot=17;
	TInt err = iSession.GetSlotId(aSlot, aId, aPhonebook);
	TESTCHECKL(err, KErrNotFound);

	add->AddContactL(KTestName,KTestNumber,Kslot17, aPhonebook, KTestEMail);
	err = iSession.GetSlotId(aSlot, aId, aPhonebook);
	TESTCHECKL(err, KErrNone);
	matchingId=aId;

	//edit slot number
	iDb->SetDbViewContactType(KUidContactICCEntry);
	CContactICCEntry* entry = static_cast<CContactICCEntry*>(iDb->OpenContactLX(aId));
	CleanupStack::PushL(entry);

	CContactItemFieldSet& fieldset = entry->CardFields();
	const TInt pos(fieldset.Find(KUidContactFieldICCSlot));
	CContactItemField& field = fieldset[pos];
	CContactTextField* textfield = field.TextStorage();
	textfield->SetTextL(Kslot18);
	iDb->CommitContactL(*entry);
	CleanupStack::PopAndDestroy(2);

	//check slot 17 is empty
	err = iSession.GetSlotId(aSlot, aId, aPhonebook);
	TESTCHECKL(err, KErrNotFound);
	
	//check slot 18 is not empty
	aSlot=18;
	err = iSession.GetSlotId(aSlot, aId, aPhonebook);
	TESTCHECKL(err, KErrNone);
	TESTCHECKL(aId, matchingId);

	delete add;
	}

/********************* STRESS TESTS ************************************/

/** 
 * Factory construction method.
 * @return Pointer to CPhbkOpenFullICCTest object
 */
CPhbkOpenFullICCTest* CPhbkOpenFullICCTest::NewL()
	{
	CPhbkOpenFullICCTest* self = new(ELeave) CPhbkOpenFullICCTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkOpenFullICCTest::CPhbkOpenFullICCTest()
	{
	SetTestStepName(_L("OpenFullICCTest"));
	}

/** Open and close each ICC entry in a full phonebook */
enum TVerdict CPhbkOpenFullICCTest::doTestStepL()
	{
	// Global ADN Phonebook
	SetSimTsyTestNumberL(131);

	TRequestStatus status; 
	iSession.DoSynchronisation(status);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);

	iDb->SetDbViewContactType(KUidContactICCEntry);
	// get the unique groupId for the given phonebook
	TContactItemId groupId(KNullContactId);
	User::LeaveIfError(iSession.GetPhoneBookId(groupId, 
						RPhoneBookSession::ESyncGroupId,
						KUidIccGlobalAdnPhonebook));
	TESTCHECKCONDITIONL(groupId != KNullContactId);
  
	// based on the groupId, get items belonging to the phonebook
	CContactGroup* group = NULL;
	TRAPD(err, group = static_cast<CContactGroup*>(iDb->ReadContactL(groupId)));
	TESTCHECKL(err, KErrNone);
	CleanupStack::PushL(group);
	const CContactIdArray* array = group->ItemsContained();

	TInt count(array->Count());
	TESTCHECKL(count, KTest13ICCSlotsADN);
	TInt i;

	for(i=0; i<count; ++i)
		{
		TContactItemId id = (*array)[i];
		CContactICCEntry* entry = static_cast<CContactICCEntry*>(iDb->OpenContactLX(id));
		CleanupStack::PushL(entry);
		ChangeNameFieldL(*entry,KModifiedName);
		iDb->CloseContactL(id);
		CleanupStack::PopAndDestroy(2); //entry+lockrecord
		}
	CleanupStack::PopAndDestroy(); // group

	// Global SDN Phonebook
	SetSimTsyTestNumberL(132);

	iSession.DoSynchronisation(status, KUidIccGlobalSdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);

	iDb->SetDbViewContactType(KUidContactICCEntry);
	// get the unique groupId for the given phonebook
	User::LeaveIfError(iSession.GetPhoneBookId(groupId, 
						RPhoneBookSession::ESyncGroupId,
						KUidIccGlobalSdnPhonebook));
	TESTCHECKCONDITIONL(groupId != KNullContactId);
  
	// based on the groupId, get items belonging to the phonebook
	TRAP(err, group = static_cast<CContactGroup*>(iDb->ReadContactL(groupId)));
	TESTCHECKL(err, KErrNone);
	CleanupStack::PushL(group);
	array = group->ItemsContained();

	count = array->Count();
	TESTCHECKL(count, KTest13ICCSlotsSDN);

	for(i=0; i<count; ++i)
		{
		TContactItemId id = (*array)[i];
		CContactICCEntry* entry = static_cast<CContactICCEntry*>(iDb->OpenContactLX(id));
		CleanupStack::PushL(entry);
		ChangeNameFieldL(*entry,KModifiedName);
		iDb->CloseContactL(id);
		CleanupStack::PopAndDestroy(2); //entry+lockrecord
		}
	CleanupStack::PopAndDestroy(); // group

	// Global LND Phonebook
	SetSimTsyTestNumberL(133);

	iSession.DoSynchronisation(status, KUidIccGlobalLndPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);

	iDb->SetDbViewContactType(KUidContactICCEntry);
	// get the unique groupId for the given phonebook
	User::LeaveIfError(iSession.GetPhoneBookId(groupId, 
						RPhoneBookSession::ESyncGroupId,
						KUidIccGlobalLndPhonebook));
	TESTCHECKCONDITIONL(groupId != KNullContactId);
  
	// based on the groupId, get items belonging to the phonebook
	TRAP(err, group = static_cast<CContactGroup*>(iDb->ReadContactL(groupId)));
	TESTCHECKL(err, KErrNone);
	CleanupStack::PushL(group);
	array = group->ItemsContained();

	count = array->Count();
	TESTCHECKL(count, KTest13ICCSlotsLND);

	for(i=0; i<count; ++i)
		{
		TContactItemId id = (*array)[i];
		CContactICCEntry* entry = static_cast<CContactICCEntry*>(iDb->OpenContactLX(id));
		CleanupStack::PushL(entry);
		ChangeNameFieldL(*entry,KModifiedName);
		iDb->CloseContactL(id);
		CleanupStack::PopAndDestroy(2); //entry+lockrecord
		}
	CleanupStack::PopAndDestroy(); // group

	// Usim App Phonebook
	SetSimTsyTestNumberL(134);

	iSession.DoSynchronisation(status, KUidUsimAppAdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);

	iDb->SetDbViewContactType(KUidContactICCEntry);
	// get the unique groupId for the given phonebook
	User::LeaveIfError(iSession.GetPhoneBookId(groupId, 
						RPhoneBookSession::ESyncGroupId,
						KUidUsimAppAdnPhonebook));
	TESTCHECKCONDITIONL(groupId != KNullContactId);
  
	// based on the groupId, get items belonging to the phonebook
	TRAP(err, group = static_cast<CContactGroup*>(iDb->ReadContactL(groupId)));
	TESTCHECKL(err, KErrNone);
	CleanupStack::PushL(group);
	array = group->ItemsContained();

	count = array->Count();
	TESTCHECKL(count, KTest13ICCSlotsUsim);

	for(i=0; i<count; ++i)
		{
		TContactItemId id = (*array)[i];
		CContactICCEntry* entry = static_cast<CContactICCEntry*>(iDb->OpenContactLX(id));
		CleanupStack::PushL(entry);
		ChangeNameFieldL(*entry,KModifiedName);
		iDb->CloseContactL(id);
		CleanupStack::PopAndDestroy(2); //entry+lockrecord
		}
	CleanupStack::PopAndDestroy(); // group

	return TestStepResult();
	}

