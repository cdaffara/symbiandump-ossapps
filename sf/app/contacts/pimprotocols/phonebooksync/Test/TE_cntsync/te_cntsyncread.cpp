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

#include "te_cntsyncbase.h"
#include "te_cntsyncread.h"


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncReadICCContactTest object
 */
CPhbkSyncReadICCContactTest* CPhbkSyncReadICCContactTest::NewL()
	{
	CPhbkSyncReadICCContactTest* self = new(ELeave) CPhbkSyncReadICCContactTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises its own name.
 */
CPhbkSyncReadICCContactTest::CPhbkSyncReadICCContactTest()
	{
	SetTestStepName(_L("ReadICCContactTest"));
	}

/** 
 * Read an ICC entry - normal case.
 * Check group membership is correct and relevant number of fields have
 * been created.
 */
enum TVerdict CPhbkSyncReadICCContactTest::doTestStepL()
	{
	SetSyncModeL(RPhoneBookSession::EManual);
	CheckSyncModeL(RPhoneBookSession::EManual);

	SetSimTsyTestNumberL(9);
	DoSyncL();

	// We're NOT trying to read all phonebook content, only a single entry
	// from each phonebook, hence EFalse
	ReadContactL(KUidIccGlobalAdnPhonebook, KTest9ICCSlotsADN, EFalse); 
	ReadContactL(KUidIccGlobalSdnPhonebook, KTest9ICCSlotsSDN, EFalse); 
	ReadContactL(KUidIccGlobalLndPhonebook, KTest9ICCSlotsLND, EFalse); 
	ReadContactL(KUidUsimAppAdnPhonebook, KTest9ICCSlotsUsim, EFalse); 
	ReadContactL(KUidIccGlobalFdnPhonebook, KTest9ICCSlotsFDN, EFalse); 

	return TestStepResult();
	}

/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncReadSequenceOfICCEntriesTest object
 */
CPhbkSyncReadSequenceOfICCEntriesTest* CPhbkSyncReadSequenceOfICCEntriesTest::NewL()
	{
	CPhbkSyncReadSequenceOfICCEntriesTest* self = new(ELeave) CPhbkSyncReadSequenceOfICCEntriesTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncReadSequenceOfICCEntriesTest::CPhbkSyncReadSequenceOfICCEntriesTest()
	{
	SetTestStepName(_L("ReadSequenceOfICCEntriesTest"));
	}

/** 
 * Read a sequence of ICC entries - full phonebook case 
 */
enum TVerdict CPhbkSyncReadSequenceOfICCEntriesTest::doTestStepL()
	{
	// Load a data set with full slots on each phonebook
	SetSimTsyTestNumberL(1);
	DoSyncL();

	// We shall try to read the whole of each phonebook, hence ETrue
	ReadContactL(KUidIccGlobalAdnPhonebook, KTest1ICCSlotsADN, ETrue);
	ReadContactL(KUidIccGlobalSdnPhonebook, KTest1ICCSlotsSDN, ETrue);
	ReadContactL(KUidIccGlobalLndPhonebook, KTest1ICCSlotsLND, ETrue);
	ReadContactL(KUidUsimAppAdnPhonebook, KTest1ICCSlotsUsim, ETrue);
	ReadContactL(KUidIccGlobalFdnPhonebook, KTest1ICCSlotsFDN, ETrue);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncReadContactInvalidUIDTest object
 */
CPhbkSyncReadContactInvalidUIDTest* CPhbkSyncReadContactInvalidUIDTest::NewL()
	{
	CPhbkSyncReadContactInvalidUIDTest* self = new(ELeave) CPhbkSyncReadContactInvalidUIDTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncReadContactInvalidUIDTest::CPhbkSyncReadContactInvalidUIDTest()
	{
	SetTestStepName(_L("ReadContactInvalidUIDTest"));
	}

/** 
 * Read a number of ICC entries - invalid UID.
 */
enum TVerdict CPhbkSyncReadContactInvalidUIDTest::doTestStepL()
	{
	// Load a data set with full slots on each phonebook
	SetSimTsyTestNumberL(1);
	DoSyncL();
	
	iDb->SetDbViewContactType(KUidContactICCEntry);
	const CContactIdArray* sortedItems = iDb->SortedItemsL();
	TInt count(sortedItems->Count());
	TESTCHECK(count, KTest1MaxSlots);

	TContactItemId invalidUid(-2);
	CContactICCEntry* entry = NULL;
	TRAPD(err, entry = static_cast<CContactICCEntry*>(iDb->ReadContactL(invalidUid)));
	TESTCHECK(err, KErrNotFound);
	delete entry;

	invalidUid=-786;
	TRAP(err, entry = static_cast<CContactICCEntry*>(iDb->ReadContactL(invalidUid)));
	TESTCHECK(err, KErrNotFound);
	delete entry;

	invalidUid=999;
	TRAP(err, entry = static_cast<CContactICCEntry*>(iDb->ReadContactL(invalidUid)));
	TESTCHECK(err, KErrNotFound);
	delete entry;

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncReadContactsEmptyFieldTest object
 */
CPhbkSyncReadContactsEmptyFieldTest* CPhbkSyncReadContactsEmptyFieldTest::NewL()
	{
	CPhbkSyncReadContactsEmptyFieldTest* self = new(ELeave) CPhbkSyncReadContactsEmptyFieldTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncReadContactsEmptyFieldTest::CPhbkSyncReadContactsEmptyFieldTest()
	{
	SetTestStepName(_L("ReadContactsEmptyFieldTest"));
	}

/** 
 * Read a sequence of ICC entries - empty field case. 
 * Name, number or email fields are empty.
 * Check group membership is correct and relevant number of fields have
 * been created.
 */
enum TVerdict CPhbkSyncReadContactsEmptyFieldTest::doTestStepL()
	{
	// This test employs data with empty fields
	SetSimTsyTestNumberL(6);
	DoSyncL();

	ReadAllContactContentL(KUidIccGlobalAdnPhonebook, KTest6ICCSlotsADN);
	ReadAllContactContentL(KUidIccGlobalSdnPhonebook, KTest6ICCSlotsSDN);
	ReadAllContactContentL(KUidIccGlobalLndPhonebook, KTest6ICCSlotsLND);
	ReadAllContactContentL(KUidUsimAppAdnPhonebook, KTest6ICCSlotsUsim);
	ReadAllContactContentL(KUidIccGlobalFdnPhonebook, KTest6ICCSlotsFDN);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncReadContactFullViewICCLockedTest object
 */
CPhbkSyncReadContactFullViewICCLockedTest* CPhbkSyncReadContactFullViewICCLockedTest::NewL()
	{
	CPhbkSyncReadContactFullViewICCLockedTest* self = new(ELeave) CPhbkSyncReadContactFullViewICCLockedTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncReadContactFullViewICCLockedTest::CPhbkSyncReadContactFullViewICCLockedTest()
	{
	SetTestStepName(_L("ReadContactFullViewICCLockedTest"));
	}

/** 
 * Read ICC entry - entry not available as the ICC becomes locked.
 */
enum TVerdict CPhbkSyncReadContactFullViewICCLockedTest::doTestStepL()
	{
	// ADN Entry
	SetSimTsyTestNumberL(2);
	DoSyncL();
	ReadFullContactLockedL(KUidIccGlobalAdnPhonebook, KTest2ICCSlotsADN);

	// SDN Entry
	SetSimTsyTestNumberL(2);
	DoSyncL();
	ReadFullContactLockedL(KUidIccGlobalSdnPhonebook, KTest2ICCSlotsSDN);

	// LND Entry
	SetSimTsyTestNumberL(2);
	DoSyncL();
	ReadFullContactLockedL(KUidIccGlobalLndPhonebook, KTest2ICCSlotsLND);

	// Usim App Entry
	SetSimTsyTestNumberL(2);
	DoSyncL();
	ReadFullContactLockedL(KUidUsimAppAdnPhonebook, KTest2ICCSlotsUsim);

	// FDN Entry
	SetSimTsyTestNumberL(2);
	DoSyncL();
	ReadFullContactLockedL(KUidIccGlobalFdnPhonebook, KTest2ICCSlotsFDN);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncReadContactDiffViewsTest object
 */
CPhbkSyncReadContactDiffViewsTest* CPhbkSyncReadContactDiffViewsTest::NewL()
	{
	CPhbkSyncReadContactDiffViewsTest* self = new(ELeave) CPhbkSyncReadContactDiffViewsTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncReadContactDiffViewsTest::CPhbkSyncReadContactDiffViewsTest()
	{
	SetTestStepName(_L("ReadContactDiffViewsTest"));
	}

/** 
 * Read an ICC entry - specify full view, full view for non-existent entry, name, number 
 * and invalid view
 */
enum TVerdict CPhbkSyncReadContactDiffViewsTest::doTestStepL()
	{
	// ADN Entry
	SetSimTsyTestNumberL(2);
	DoSyncL();
	ReadContactViewsL(KUidIccGlobalAdnPhonebook, KTest2ICCSlotsADN);

	// SDN Entry
	SetSimTsyTestNumberL(2);
	DoSyncL();
	ReadContactViewsL(KUidIccGlobalSdnPhonebook, KTest2ICCSlotsSDN);

	// LND Entry
	SetSimTsyTestNumberL(2);
	DoSyncL();
	ReadContactViewsL(KUidIccGlobalLndPhonebook, KTest2ICCSlotsLND);

	// Usim App Entry
	SetSimTsyTestNumberL(2);
	DoSyncL();
	ReadContactViewsL(KUidUsimAppAdnPhonebook, KTest2ICCSlotsUsim);

	// FDN Entry
	SetSimTsyTestNumberL(2);
	DoSyncL();
	ReadContactViewsL(KUidIccGlobalFdnPhonebook, KTest2ICCSlotsFDN);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncReadICCMinimalContactTest object
 */
CPhbkSyncReadICCMinimalContactTest* CPhbkSyncReadICCMinimalContactTest::NewL()
	{
	CPhbkSyncReadICCMinimalContactTest* self = new(ELeave) CPhbkSyncReadICCMinimalContactTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncReadICCMinimalContactTest::CPhbkSyncReadICCMinimalContactTest()
	{
	SetTestStepName(_L("ReadICCMinimalContactTest"));
	}

/**
 * Read an ICC entry - minimal read operation.
 */
enum TVerdict CPhbkSyncReadICCMinimalContactTest::doTestStepL()
	{
	SetSimTsyTestNumberL(1);
	DoSyncL();

	ReadMinimalContactL(KUidIccGlobalAdnPhonebook, KTest1ICCSlotsADN);
	ReadMinimalContactL(KUidIccGlobalSdnPhonebook, KTest1ICCSlotsSDN);
	ReadMinimalContactL(KUidIccGlobalLndPhonebook, KTest1ICCSlotsLND);
	ReadMinimalContactL(KUidUsimAppAdnPhonebook, KTest1ICCSlotsUsim);
	ReadMinimalContactL(KUidIccGlobalFdnPhonebook, KTest1ICCSlotsFDN);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncReadMinimalContactInvalidUIDTest object
 */
CPhbkSyncReadMinimalContactInvalidUIDTest* CPhbkSyncReadMinimalContactInvalidUIDTest::NewL()
	{
	CPhbkSyncReadMinimalContactInvalidUIDTest* self = new(ELeave) CPhbkSyncReadMinimalContactInvalidUIDTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncReadMinimalContactInvalidUIDTest::CPhbkSyncReadMinimalContactInvalidUIDTest()
	{
	SetTestStepName(_L("ReadMinimalContactInvalidUIDTest"));
	}

enum TVerdict CPhbkSyncReadMinimalContactInvalidUIDTest::doTestStepL()
	{
	SetSimTsyTestNumberL(1);
	DoSyncL();

	iDb->SetDbViewContactType(KUidContactICCEntry);
	const CContactIdArray* sortedItems = iDb->SortedItemsL();
	TInt count = sortedItems->Count(); // for debugging
	TESTCHECK(count, KTest1MaxSlots);

	TContactItemId invalidUID(1500);
	TRAPD(err,iDb->ReadMinimalContactL(invalidUID));
	TESTCHECK(err, KErrNotFound);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncReadMinimalContactEmptyFieldTest object
 */
CPhbkSyncReadMinimalContactEmptyFieldTest* CPhbkSyncReadMinimalContactEmptyFieldTest::NewL()
	{
	CPhbkSyncReadMinimalContactEmptyFieldTest* self = new(ELeave) CPhbkSyncReadMinimalContactEmptyFieldTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncReadMinimalContactEmptyFieldTest::CPhbkSyncReadMinimalContactEmptyFieldTest()
	{
	SetTestStepName(_L("ReadMinimalContactEmptyFieldTest"));
	}

/** 
 * Read a sequence of ICC entries - minimal read with missing fields. 
 * Name, number or email fields are empty.
 */
enum TVerdict CPhbkSyncReadMinimalContactEmptyFieldTest::doTestStepL()
	{
	// This test employs data with empty fields
	SetSimTsyTestNumberL(6);
	DoSyncL();
	
	ReadMinimalContactL(KUidIccGlobalAdnPhonebook, KTest6ICCSlotsADN);
	ReadMinimalContactL(KUidIccGlobalSdnPhonebook, KTest6ICCSlotsSDN);
	ReadMinimalContactL(KUidIccGlobalLndPhonebook, KTest6ICCSlotsLND);
	ReadMinimalContactL(KUidUsimAppAdnPhonebook, KTest6ICCSlotsUsim);
	ReadMinimalContactL(KUidIccGlobalFdnPhonebook, KTest6ICCSlotsFDN);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncReadMinimalContactICCLockedTest object
 */
CPhbkSyncReadMinimalContactICCLockedTest* CPhbkSyncReadMinimalContactICCLockedTest::NewL()
	{
	CPhbkSyncReadMinimalContactICCLockedTest* self = new(ELeave) CPhbkSyncReadMinimalContactICCLockedTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncReadMinimalContactICCLockedTest::CPhbkSyncReadMinimalContactICCLockedTest()
	{
	SetTestStepName(_L("ReadMinimalContactICCLockedTest"));
	}

/** 
 * Read a number of ICC entries - minimal read, entry not available as the ICC becomes locked.
 */
enum TVerdict CPhbkSyncReadMinimalContactICCLockedTest::doTestStepL()
	{
	// ADN Entry
	SetSimTsyTestNumberL(2);
	DoSyncL();
	ReadMinimalContactLockedL(KUidIccGlobalAdnPhonebook, KTest2ICCSlotsADN);

	// SDN Entry
	SetSimTsyTestNumberL(2);
	DoSyncL();
	ReadMinimalContactLockedL(KUidIccGlobalSdnPhonebook, KTest2ICCSlotsSDN);

	// LND Entry
	SetSimTsyTestNumberL(2);
	DoSyncL();
	ReadMinimalContactLockedL(KUidIccGlobalLndPhonebook, KTest2ICCSlotsLND);

	// Usim App Entry
	SetSimTsyTestNumberL(2);
	DoSyncL();
	ReadMinimalContactLockedL(KUidUsimAppAdnPhonebook, KTest2ICCSlotsUsim);

	// FDN Entry
	SetSimTsyTestNumberL(2);
	DoSyncL();
	ReadMinimalContactLockedL(KUidIccGlobalFdnPhonebook, KTest2ICCSlotsFDN);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncReadContactTextDefFullViewTest object
 */
CPhbkSyncReadContactTextDefFullViewTest* CPhbkSyncReadContactTextDefFullViewTest::NewL()
	{
	CPhbkSyncReadContactTextDefFullViewTest* self = new(ELeave) CPhbkSyncReadContactTextDefFullViewTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncReadContactTextDefFullViewTest::CPhbkSyncReadContactTextDefFullViewTest()
	{
	SetTestStepName(_L("ReadContactTextDefFullViewTest"));
	}

/** 
 * Read an ICC entry - create string from contact info (full view).
 */
enum TVerdict CPhbkSyncReadContactTextDefFullViewTest::doTestStepL()
	{
	// NOT trying to read all phonebook, hence EFalse

	SetSimTsyTestNumberL(2);
	DoSyncL();

	ReadFullContactTextDefL(KUidIccGlobalAdnPhonebook, KTest2ICCSlotsADN, EFalse);
	ReadFullContactTextDefL(KUidIccGlobalSdnPhonebook, KTest2ICCSlotsSDN, EFalse);
	ReadFullContactTextDefL(KUidIccGlobalLndPhonebook, KTest2ICCSlotsLND, EFalse);
	ReadFullContactTextDefL(KUidUsimAppAdnPhonebook, KTest2ICCSlotsUsim, EFalse);
	ReadFullContactTextDefL(KUidIccGlobalFdnPhonebook, KTest2ICCSlotsFDN, EFalse);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncReadContactNumberTextDefTest object
 */
CPhbkSyncReadContactNumberTextDefTest* CPhbkSyncReadContactNumberTextDefTest::NewL()
	{
	CPhbkSyncReadContactNumberTextDefTest* self = new(ELeave) CPhbkSyncReadContactNumberTextDefTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncReadContactNumberTextDefTest::CPhbkSyncReadContactNumberTextDefTest()
	{
	SetTestStepName(_L("ReadContactNumberTextDefTest"));
	}

/** 
 * Read an ICC entry - create number string from contact info.
 */
enum TVerdict CPhbkSyncReadContactNumberTextDefTest::doTestStepL()
	{
	// NOT trying to read all phonebook, hence EFalse

	SetSimTsyTestNumberL(2);
	DoSyncL();

	ReadNumberContactTextDefL(KUidIccGlobalAdnPhonebook, KTest2ICCSlotsADN, EFalse);
	ReadNumberContactTextDefL(KUidIccGlobalSdnPhonebook, KTest2ICCSlotsSDN, EFalse);
	ReadNumberContactTextDefL(KUidIccGlobalLndPhonebook, KTest2ICCSlotsLND, EFalse);
	ReadNumberContactTextDefL(KUidUsimAppAdnPhonebook, KTest2ICCSlotsUsim, EFalse);
	ReadNumberContactTextDefL(KUidIccGlobalFdnPhonebook, KTest2ICCSlotsFDN, EFalse);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncReadContactNameTextDefTest object
 */
CPhbkSyncReadContactNameTextDefTest* CPhbkSyncReadContactNameTextDefTest::NewL()
	{
	CPhbkSyncReadContactNameTextDefTest* self = new(ELeave) CPhbkSyncReadContactNameTextDefTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncReadContactNameTextDefTest::CPhbkSyncReadContactNameTextDefTest()
	{
	SetTestStepName(_L("ReadContactNameTextDefTest"));
	}

/** 
 * Read an ICC entry - create name string from contact info.
 */
enum TVerdict CPhbkSyncReadContactNameTextDefTest::doTestStepL()
	{
	// NOT trying to read all phonebook, hence EFalse

	SetSimTsyTestNumberL(2);
	DoSyncL();

	ReadNameContactTextDefL(KUidIccGlobalAdnPhonebook, KTest2ICCSlotsADN, EFalse);
	ReadNameContactTextDefL(KUidIccGlobalSdnPhonebook, KTest2ICCSlotsSDN, EFalse);
	ReadNameContactTextDefL(KUidIccGlobalLndPhonebook, KTest2ICCSlotsLND, EFalse);
	ReadNameContactTextDefL(KUidUsimAppAdnPhonebook, KTest2ICCSlotsUsim, EFalse);
	ReadNameContactTextDefL(KUidIccGlobalFdnPhonebook, KTest2ICCSlotsFDN, EFalse);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncReadContactInvalidTextDefTest object
 */
CPhbkSyncReadContactInvalidTextDefTest* CPhbkSyncReadContactInvalidTextDefTest::NewL()
	{
	CPhbkSyncReadContactInvalidTextDefTest* self = new(ELeave) CPhbkSyncReadContactInvalidTextDefTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncReadContactInvalidTextDefTest::CPhbkSyncReadContactInvalidTextDefTest()
	{
	SetTestStepName(_L("ReadContactInvalidTextDefTest"));
	}

/** 
 * Read an ICC entry - invalid string specification.
 */
enum TVerdict CPhbkSyncReadContactInvalidTextDefTest::doTestStepL()
	{
	SetSimTsyTestNumberL(2);
	DoSyncL();

	ReadInvalidContactTextDefL(KUidIccGlobalAdnPhonebook, KTest2ICCSlotsADN);
	ReadInvalidContactTextDefL(KUidIccGlobalSdnPhonebook, KTest2ICCSlotsSDN);
	ReadInvalidContactTextDefL(KUidIccGlobalLndPhonebook, KTest2ICCSlotsLND);
	ReadInvalidContactTextDefL(KUidUsimAppAdnPhonebook, KTest2ICCSlotsUsim);
	ReadInvalidContactTextDefL(KUidIccGlobalFdnPhonebook, KTest2ICCSlotsFDN);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncReadContactICCLockedTest object
 */
CPhbkSyncReadContactICCLockedTest* CPhbkSyncReadContactICCLockedTest::NewL()
	{
	CPhbkSyncReadContactICCLockedTest* self = new(ELeave) CPhbkSyncReadContactICCLockedTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncReadContactICCLockedTest::CPhbkSyncReadContactICCLockedTest()
	{
	SetTestStepName(_L("ReadContactICCLockedTest"));
	}

/** 
 * Read an ICC entry using text definition when ICC is locked.
 */
enum TVerdict CPhbkSyncReadContactICCLockedTest::doTestStepL()
	{
	// ADN Entry
	SetSimTsyTestNumberL(2);
	DoSyncL();
	ReadLockedContactTextDefL(KUidIccGlobalAdnPhonebook, KTest2ICCSlotsADN);

	// SDN Entry
	SetSimTsyTestNumberL(2);
	DoSyncL();
	ReadLockedContactTextDefL(KUidIccGlobalSdnPhonebook, KTest2ICCSlotsSDN);

	// LND Entry
	SetSimTsyTestNumberL(2);
	DoSyncL();
	ReadLockedContactTextDefL(KUidIccGlobalLndPhonebook, KTest2ICCSlotsLND);

	// Usim App Entry
	SetSimTsyTestNumberL(2);
	DoSyncL();
	ReadLockedContactTextDefL(KUidUsimAppAdnPhonebook, KTest2ICCSlotsUsim);

	// FDN Entry
	SetSimTsyTestNumberL(2);
	DoSyncL();
	ReadLockedContactTextDefL(KUidIccGlobalFdnPhonebook, KTest2ICCSlotsFDN);

	return TestStepResult();
	}


/************************ STRESS TESTS *****************************/

/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncReadFullICCNameViewTest object
 */
CPhbkSyncReadFullICCNameViewTest* CPhbkSyncReadFullICCNameViewTest::NewL()
	{
	CPhbkSyncReadFullICCNameViewTest* self = new(ELeave) CPhbkSyncReadFullICCNameViewTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncReadFullICCNameViewTest::CPhbkSyncReadFullICCNameViewTest()
	{
	SetTestStepName(_L("ReadFullICCNameViewTest"));
	}

/** 
 * Read operation stress test.
 *
 * Read a full phonebook with the name view.
 */
enum TVerdict CPhbkSyncReadFullICCNameViewTest::doTestStepL()
	{
	// ensure contact database entries are removed because this test demands that 
	// the order of new entry IDs corresponds to the order of the test data entries
	delete iDb;
	iDb=NULL;
	User::After(1000000); //1 second wait for contact to shut down
	iSession.Close();
	ConfigurePhbkSyncToIdleL();
	TRAP_IGNORE(CContactDatabase::DeleteDefaultFileL());	
	ConfigurePhbkSyncToFullL();
	iDb = OpenOrCreateContactDatabaseL();
	User::After(1000000); //1 second wait for contact to start the phbk server
	User::LeaveIfError(iSession.Connect());
	User::After(3000000); //3 second wait for server to create phbktable

	// Test data for this scenario is divided into individual phonebooks because
	// SIMTSY cannot handle maximum entries for all phonebooks in a single set.
	// Data provided is not necessarily the maximum possible for each phonebook
	// due to inability to predict stack usage at the point of testing.
	TRequestStatus status; 

	// Global ADN
	SetSimTsyTestNumberL(131);
	iSession.DoSynchronisation(status);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	ReadAllContactNameViewL(KUidIccGlobalAdnPhonebook, KTest13ICCSlotsADN);

	// Global SDN
	SetSimTsyTestNumberL(132);
	iSession.DoSynchronisation(status, KUidIccGlobalSdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	ReadAllContactNameViewL(KUidIccGlobalSdnPhonebook, KTest13ICCSlotsSDN);

	// Global LND
	SetSimTsyTestNumberL(133);
	iSession.DoSynchronisation(status, KUidIccGlobalLndPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	ReadAllContactNameViewL(KUidIccGlobalLndPhonebook, KTest13ICCSlotsLND);

	// USIM App
	SetSimTsyTestNumberL(134);
	iSession.DoSynchronisation(status, KUidUsimAppAdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	ReadAllContactNameViewL(KUidUsimAppAdnPhonebook, KTest13ICCSlotsUsim);

	// Global FDN
	SetSimTsyTestNumberL(135);
	iSession.DoSynchronisation(status, KUidIccGlobalFdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	ReadAllContactNameViewL(KUidIccGlobalFdnPhonebook, KTest13ICCSlotsFDN);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncReadFullICCNumberViewTest object
 */
CPhbkSyncReadFullICCNumberViewTest* CPhbkSyncReadFullICCNumberViewTest::NewL()
	{
	CPhbkSyncReadFullICCNumberViewTest* self = new(ELeave) CPhbkSyncReadFullICCNumberViewTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncReadFullICCNumberViewTest::CPhbkSyncReadFullICCNumberViewTest()
	{
	SetTestStepName(_L("ReadFullICCNumberViewTest"));
	}

/** 
 * Read operation stress test.
 *
 * Read a full phonebook with the number view. It is also necessary to view the Contacts DB 
 * using the development UI to ensure that these test cases pass.
 */
enum TVerdict CPhbkSyncReadFullICCNumberViewTest::doTestStepL()
	{
	// Test data for this scenario is divided into individual phonebooks because
	// SIMTSY cannot handle maximum entries for all phonebooks in a single set.
	// Data provided is not necessarily the maximum possible for each phonebook
	// due to inability to predict stack usage at the point of testing.
	TRequestStatus status; 

	// Global ADN
	SetSimTsyTestNumberL(131);
	iSession.DoSynchronisation(status);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	ReadAllContactNumberViewL(KUidIccGlobalAdnPhonebook, KTest13ICCSlotsADN);

	// Global SDN
	SetSimTsyTestNumberL(132);
	iSession.DoSynchronisation(status, KUidIccGlobalSdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	ReadAllContactNumberViewL(KUidIccGlobalSdnPhonebook, KTest13ICCSlotsSDN);

	// Global LND
	SetSimTsyTestNumberL(133);
	iSession.DoSynchronisation(status, KUidIccGlobalLndPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	ReadAllContactNumberViewL(KUidIccGlobalLndPhonebook, KTest13ICCSlotsLND);

	// USIM App
	SetSimTsyTestNumberL(134);
	iSession.DoSynchronisation(status, KUidUsimAppAdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	ReadAllContactNumberViewL(KUidUsimAppAdnPhonebook, KTest13ICCSlotsUsim);

	// Global FDN
	SetSimTsyTestNumberL(135);
	iSession.DoSynchronisation(status, KUidIccGlobalFdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	ReadAllContactNumberViewL(KUidIccGlobalFdnPhonebook, KTest13ICCSlotsFDN);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncReadFullICCFullViewTest object
 */
CPhbkSyncReadFullICCFullViewTest* CPhbkSyncReadFullICCFullViewTest::NewL()
	{
	CPhbkSyncReadFullICCFullViewTest* self = new(ELeave) CPhbkSyncReadFullICCFullViewTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncReadFullICCFullViewTest::CPhbkSyncReadFullICCFullViewTest()
	{
	SetTestStepName(_L("ReadFullICCFullViewTest"));
	}

/** 
 * Read a full phonebook with the full view.
 */
enum TVerdict CPhbkSyncReadFullICCFullViewTest::doTestStepL()
	{
	// Test data for this scenario is divided into individual phonebooks because
	// SIMTSY cannot handle maximum entries for all phonebooks in a single set.
	// Data provided is not necessarily the maximum possible for each phonebook
	// due to inability to predict stack usage at the point of testing.
	TRequestStatus status; 

	// Global ADN
	SetSimTsyTestNumberL(131);
	iSession.DoSynchronisation(status);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	ReadAllContactFullViewL(KUidIccGlobalAdnPhonebook, KTest13ICCSlotsADN);

	// Global SDN
	SetSimTsyTestNumberL(132);
	iSession.DoSynchronisation(status, KUidIccGlobalSdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	ReadAllContactFullViewL(KUidIccGlobalSdnPhonebook, KTest13ICCSlotsSDN);

	// Global LND
	SetSimTsyTestNumberL(133);
	iSession.DoSynchronisation(status, KUidIccGlobalLndPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	ReadAllContactFullViewL(KUidIccGlobalLndPhonebook, KTest13ICCSlotsLND);

	// USIM App
	SetSimTsyTestNumberL(134);
	iSession.DoSynchronisation(status, KUidUsimAppAdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	ReadAllContactFullViewL(KUidUsimAppAdnPhonebook, KTest13ICCSlotsUsim);

	// Global FDN
	SetSimTsyTestNumberL(135);
	iSession.DoSynchronisation(status, KUidIccGlobalFdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	ReadAllContactFullViewL(KUidIccGlobalFdnPhonebook, KTest13ICCSlotsFDN);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncReadFullICCNameTextDefTest object
 */
CPhbkSyncReadFullICCNameTextDefTest* CPhbkSyncReadFullICCNameTextDefTest::NewL()
	{
	CPhbkSyncReadFullICCNameTextDefTest* self = new(ELeave) CPhbkSyncReadFullICCNameTextDefTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncReadFullICCNameTextDefTest::CPhbkSyncReadFullICCNameTextDefTest()
	{
	SetTestStepName(_L("ReadFullICCNameTextDefTest"));
	}

/** 
 * Read a full phonebook - create name string from contact info.
 */
enum TVerdict CPhbkSyncReadFullICCNameTextDefTest::doTestStepL()
	{
	// Test data for this scenario is divided into individual phonebooks because
	// SIMTSY cannot handle maximum entries for all phonebooks in a single set.
	// Data provided is not necessarily the maximum possible for each phonebook
	// due to inability to predict stack usage at the point of testing.
	TRequestStatus status; 

	// Global ADN
	SetSimTsyTestNumberL(131);
	iSession.DoSynchronisation(status);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	ReadNameContactTextDefL(KUidIccGlobalAdnPhonebook, KTest13ICCSlotsADN, ETrue);

	// Global SDN
	SetSimTsyTestNumberL(132);
	iSession.DoSynchronisation(status, KUidIccGlobalSdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	ReadNameContactTextDefL(KUidIccGlobalSdnPhonebook, KTest13ICCSlotsSDN, ETrue);

	// Global LND
	SetSimTsyTestNumberL(133);
	iSession.DoSynchronisation(status, KUidIccGlobalLndPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	ReadNameContactTextDefL(KUidIccGlobalLndPhonebook, KTest13ICCSlotsLND, ETrue);

	// USIM App
	SetSimTsyTestNumberL(134);
	iSession.DoSynchronisation(status, KUidUsimAppAdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	ReadNameContactTextDefL(KUidUsimAppAdnPhonebook, KTest13ICCSlotsUsim, ETrue);

	// Global FDN
	SetSimTsyTestNumberL(135);
	iSession.DoSynchronisation(status, KUidIccGlobalFdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	ReadNameContactTextDefL(KUidIccGlobalFdnPhonebook, KTest13ICCSlotsFDN, ETrue);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncReadFullICCNumberTextDefTest object
 */
CPhbkSyncReadFullICCNumberTextDefTest* CPhbkSyncReadFullICCNumberTextDefTest::NewL()
	{
	CPhbkSyncReadFullICCNumberTextDefTest* self = new(ELeave) CPhbkSyncReadFullICCNumberTextDefTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncReadFullICCNumberTextDefTest::CPhbkSyncReadFullICCNumberTextDefTest()
	{
	SetTestStepName(_L("ReadFullICCNumberTextDefTest"));
	}

/** 
 * Read a full phonebook - create number string from contact info.
 */
enum TVerdict CPhbkSyncReadFullICCNumberTextDefTest::doTestStepL()
	{
	// Test data for this scenario is divided into individual phonebooks because
	// SIMTSY cannot handle maximum entries for all phonebooks in a single set.
	// Data provided is not necessarily the maximum possible for each phonebook
	// due to inability to predict stack usage at the point of testing.
	TRequestStatus status; 

	// Global ADN
	SetSimTsyTestNumberL(131);
	iSession.DoSynchronisation(status);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	ReadNumberContactTextDefL(KUidIccGlobalAdnPhonebook, KTest13ICCSlotsADN, ETrue);

	// Global SDN
	SetSimTsyTestNumberL(132);
	iSession.DoSynchronisation(status, KUidIccGlobalSdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	ReadNumberContactTextDefL(KUidIccGlobalSdnPhonebook, KTest13ICCSlotsSDN, ETrue);

	// Global LND
	SetSimTsyTestNumberL(133);
	iSession.DoSynchronisation(status, KUidIccGlobalLndPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	ReadNumberContactTextDefL(KUidIccGlobalLndPhonebook, KTest13ICCSlotsLND, ETrue);

	// USIM App
	SetSimTsyTestNumberL(134);
	iSession.DoSynchronisation(status, KUidUsimAppAdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	ReadNumberContactTextDefL(KUidUsimAppAdnPhonebook, KTest13ICCSlotsUsim, ETrue);

	// Global FDN
	SetSimTsyTestNumberL(135);
	iSession.DoSynchronisation(status, KUidIccGlobalFdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	ReadNumberContactTextDefL(KUidIccGlobalFdnPhonebook, KTest13ICCSlotsFDN, ETrue);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncReadFullICCFullViewTextDefTest object
 */
CPhbkSyncReadFullICCFullViewTextDefTest* CPhbkSyncReadFullICCFullViewTextDefTest::NewL()
	{
	CPhbkSyncReadFullICCFullViewTextDefTest* self = new(ELeave) CPhbkSyncReadFullICCFullViewTextDefTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncReadFullICCFullViewTextDefTest::CPhbkSyncReadFullICCFullViewTextDefTest()
	{
	SetTestStepName(_L("ReadFullICCFullViewTextDefTest"));
	}

/** 
 * Read a full phonebook - create string from contact info (full view).
 */
enum TVerdict CPhbkSyncReadFullICCFullViewTextDefTest::doTestStepL()
	{
	// Test data for this scenario is divided into individual phonebooks because
	// SIMTSY cannot handle maximum entries for all phonebooks in a single set.
	// Data provided is not necessarily the maximum possible for each phonebook
	// due to inability to predict stack usage at the point of testing.
	TRequestStatus status; 

	// Global ADN
	SetSimTsyTestNumberL(131);
	iSession.DoSynchronisation(status);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	ReadFullContactTextDefL(KUidIccGlobalAdnPhonebook, KTest13ICCSlotsADN, ETrue);

	// Global SDN
	SetSimTsyTestNumberL(132);
	iSession.DoSynchronisation(status, KUidIccGlobalSdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	ReadFullContactTextDefL(KUidIccGlobalSdnPhonebook, KTest13ICCSlotsSDN, ETrue);

	// Global LND
	SetSimTsyTestNumberL(133);
	iSession.DoSynchronisation(status, KUidIccGlobalLndPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	ReadFullContactTextDefL(KUidIccGlobalLndPhonebook, KTest13ICCSlotsLND, ETrue);

	// USIM App
	SetSimTsyTestNumberL(134);
	iSession.DoSynchronisation(status, KUidUsimAppAdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	ReadFullContactTextDefL(KUidUsimAppAdnPhonebook, KTest13ICCSlotsUsim, ETrue);

	// Global FDN
	SetSimTsyTestNumberL(135);
	iSession.DoSynchronisation(status, KUidIccGlobalFdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	ReadFullContactTextDefL(KUidIccGlobalFdnPhonebook, KTest13ICCSlotsFDN, ETrue);

	return TestStepResult();
	}


/** 
 * Check group membership.
 * All ICC entries synchronised should belong to a single group. This group 
 * should contain all the ADN phonebook entries
 *
 * @param aItem             ICC entry 
 * @param aExpectedMembers  Expected number of members of the ADN phonebook
 */
void CPhbkReadIntegrationTestBase::CheckGroupMembershipL(CContactICCEntry& aItem, TInt aExpectedMembers)
	{
	const CContactIdArray* owned = aItem.GroupsJoined();	
	TESTCHECKCONDITIONL(owned != NULL);
	TESTCHECKL(owned->Count(), 1);
	TContactItemId groupId = (*owned)[0];

	CContactGroup* group = static_cast<CContactGroup*>(iDb->ReadContactL(groupId));
	CleanupStack::PushL(group);	
	const CContactIdArray* itemsContained = group->ItemsContained();
	TESTCHECKL(itemsContained->Count(), aExpectedMembers);
	CleanupStack::PopAndDestroy(group);	
	}

/** 
 * Check that the field of type aFieldType has the same content as 
 * aExpectedContent
 * @param aFieldset Contact item fieldset
 * @param aFieldType Field type to test
 * @param aExpectedContent Expected data
 */
void CPhbkReadIntegrationTestBase::CheckFieldContentL(CContactItemFieldSet& aFieldset, TFieldType aFieldType, const TDesC& aExpectedContent)
	{
	TInt pos = aFieldset.Find(aFieldType);
	CContactItemField& field = aFieldset[pos];
	CContactTextField* textField = field.TextStorage();
	TESTCHECKCONDITION(textField != NULL);
	TESTCHECKCONDITIONL(textField->Text() == aExpectedContent);
	}


/** 
 * Check that the field of type aFieldType has the same content as 
 * aExpectedContent.  This method is an overload of 
 * CPhbkReadIntegrationTestBase::CheckFieldContentL(CContactItemFieldSet& aFieldset, TFieldType aFieldType, const TDesC& aExpectedContent)
 * and allows the user to check the field content of a field when there
 * are multiple instances of the field in the CContactItemfieldSet array 
 * specified.  This can be done by specifying the position in the array
 * to start searching from.
 *
 * @param aFieldset Contact item fieldset
 * @param aFieldType Field type to test
 * @param aSearchFromPos The index in aFieldSet to start searching from
 * @param aExpectedContent Expected data
 */
void CPhbkReadIntegrationTestBase::CheckFieldContentL(CContactItemFieldSet& aFieldset, TFieldType aFieldType, TInt aSearchFromPos, const TDesC& aExpectedContent)
	{
	TInt pos = aFieldset.FindNext(aFieldType, aSearchFromPos);
	CContactItemField& field = aFieldset[pos];
	CContactTextField* textField = field.TextStorage();
	TESTCHECKCONDITION(textField != NULL);
	TESTCHECKCONDITIONL(textField->Text() == aExpectedContent);
	}
	
/** 
 * Check that aEntry contains correct number of fields.
 *
 * @param aPhonebook  ICC Phonebook to check.
 * @param aEntry      Contact ICC Entry
 */
void CPhbkReadIntegrationTestBase::CheckFieldCountL(const TUid aPhonebook, CContactICCEntry* aEntry)
	{
	CContactItemFieldSet& fieldset = aEntry->CardFields();
	const TInt fieldsetCount = fieldset.Count(); //for debugging
	if (aPhonebook == KUidUsimAppAdnPhonebook)
		{
		TESTCHECKCONDITION(fieldsetCount >= 6); //Name,Number,email,ICC slot, phonebook
		}
	else
		{
		TESTCHECKCONDITION(fieldsetCount >= 5); //Name,Number,ICC slot, phonebook
		}
	}

/** 
 * Check that aEntry contains correct number of fields for minimal contact item.
 *
 * @param aPhonebook  ICC Phonebook to check.
 * @param aEntry      Contact ICC Entry
 */
void CPhbkReadIntegrationTestBase::CheckMinimalFieldCountL(const TUid aPhonebook, CContactICCEntry* aEntry)
	{
	CContactItemFieldSet& fieldset = aEntry->CardFields();
	const TInt fieldsetCount = fieldset.Count(); //for debugging
	if (aPhonebook == KUidUsimAppAdnPhonebook)
		{
		TESTCHECKCONDITION(fieldsetCount >= 5); //Name,Number,email,ICC slot, phonebook
		}
	else
		{
		TESTCHECKCONDITION(fieldsetCount >= 4); //Name,Number,ICC slot, phonebook
		}
	}

/** 
 * Check that the field data stored on the SIM is identical to the
 * data stored in Contacts model.
 * @param aPhonebook type of phonebook
 * @param aItem ICC item to test
 * @param aSlotNumber Slot number 
 */
void CPhbkReadIntegrationTestBase::CheckFieldsL(const TUid aPhonebook, CContactICCEntry* aItem, TInt aSlotNumber)
	{
	TBuf<20> buf;
	CContactItemFieldSet& fieldset = aItem->CardFields();
	if(aPhonebook == KUidIccGlobalAdnPhonebook)
		{
		buf.Format(KNameFormat, 'A', aSlotNumber);
		}
	else if(aPhonebook == KUidIccGlobalSdnPhonebook)
		{
		buf.Format(KNameFormat, 'B', aSlotNumber);
		}
	else if(aPhonebook == KUidIccGlobalLndPhonebook)
		{
		buf.Format(KNameFormat, 'C', aSlotNumber);
		}
	else if(aPhonebook == KUidUsimAppAdnPhonebook)
		{
		buf.Format(KNameFormat, 'D', aSlotNumber);
		}
	else if(aPhonebook == KUidIccGlobalFdnPhonebook)
		{
		buf.Format(KNameFormat, 'E', aSlotNumber);
		}
	CheckFieldContentL(fieldset,KUidContactFieldFamilyName,buf);
	buf.Format(KNumberFormat,aSlotNumber);
	CheckFieldContentL(fieldset,KUidContactFieldPhoneNumber,buf);
	if(aPhonebook == KUidUsimAppAdnPhonebook)
		{
		buf.Format(KEmailFormat,aSlotNumber);
		CheckFieldContentL(fieldset,KUidContactFieldEMail,buf);
		}
	}

/** 
 * Read all ICC entry - normal case.
 * Check group membership is correct and relevant number of fields have
 * been created.
 */
void CPhbkReadIntegrationTestBase::ReadContactL(const TUid aPhonebook, TInt aExpectedCount, TBool aReadAll)
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

	// Verify correct number of entries created for this phonebook
	TInt count = array->Count(); // for debugging
	TESTCHECKL(count, aExpectedCount);

  	// Read all items or only first?
  	if (!aReadAll)
  		count = 1;
  	
	for (TInt i=0; i<count; ++i)
		{
		TContactItemId id = (*array)[i];
		CContactICCEntry* entry = NULL;
		TRAP(err, entry = static_cast<CContactICCEntry*>(iDb->ReadContactL(id)));
		TESTCHECKL(err, KErrNone);
		CleanupStack::PushL(entry);
		// Verify correct number of fields created for entry
		CheckFieldCountL(aPhonebook, entry);
		CleanupStack::PopAndDestroy(); // entry
		}

	CleanupStack::PopAndDestroy(); // group
	}

/** 
 * Read a sequence of ICC entries - normal case.
 * Check group membership is correct and check field content
 */
void CPhbkReadIntegrationTestBase::ReadAllContactContentL(const TUid aPhonebook, TInt aExpectedCount)
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

	// Verify correct number of entries created for this phonebook
	const TInt count = array->Count(); // for debugging
	TESTCHECKL(count, aExpectedCount);

	// Read each item
	for (TInt i=0; i<count; ++i)
		{
		TContactItemId id = (*array)[i];
		CContactICCEntry* entry = NULL;
		TRAP(err, entry = static_cast<CContactICCEntry*>(iDb->ReadContactL(id)));
		TESTCHECKL(err, KErrNone);
		delete entry;
		}

	CleanupStack::PopAndDestroy(); // group
	}

/** 
 * Read a ICC entry - entry not available as the ICC becomes locked.
 */
void CPhbkReadIntegrationTestBase::ReadFullContactLockedL(const TUid aPhonebook, TInt aExpectedCount)
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
	SignalSimTsyToReduceTimersL();

	// Access ICC and wait for it to become locked
	TContactItemId id = (*array)[0]; 
	while(err==KErrNone) 
		{
		User::After(1000000);	// Wait a second and try again.
		CContactICCEntry* failEntry=NULL;
		TRAP(err, failEntry = (CContactICCEntry*)iDb->ReadContactL(id, *iDb->AllFieldsView())); // Entry unavailable due to ICC being locked
		delete failEntry;
		}
	TESTCHECKL(err, KErrAccessDenied);
	SignalSimTsyToReduceTimersL();

	// Wait for ICC to become unlocked again
	while(err==KErrAccessDenied) 
		{
		User::After(1000000);	// Wait a second and try again.
		CContactICCEntry* failEntry=NULL;
		TRAP(err, failEntry = (CContactICCEntry*)iDb->ReadContactL(id, *iDb->AllFieldsView())); // Entry unavailable due to ICC being locked
		delete failEntry;
		}

	WaitForSyncToCompleteL();

	// Read the item again
	CContactICCEntry* entry = NULL;
	TRAP(err, entry = static_cast<CContactICCEntry*>(iDb->ReadContactL(id, *iDb->AllFieldsView())));
	TESTCHECKL(err, KErrNone);
	CleanupStack::PushL(entry);

	// Verify correct number of entries created for this phonebook
	const TInt count = array->Count(); // for debugging
	TESTCHECKL(count, aExpectedCount);

	// Verify correct number of fields created for entry
	CheckFieldCountL(aPhonebook, entry);

	CleanupStack::PopAndDestroy(2); // group and entry
	}

/** 
 * Read ICC entry - specify full view, full view for non-existent entry, name, number 
 * and invalid view
 */
void CPhbkReadIntegrationTestBase::ReadContactViewsL(const TUid aPhonebook, TInt aExpectedCount)
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

	// Verify correct number of entries created for this phonebook
	const TInt count = array->Count(); // for debugging
	TESTCHECKL(count, aExpectedCount);

	TContactItemId id = (*array)[0];

	// Full view 
	CContactICCEntry* entry = NULL;
	TRAP(err, entry = static_cast<CContactICCEntry*>(iDb->ReadContactL(id, *iDb->AllFieldsView())));
	TESTCHECKL(err, KErrNone);
	CleanupStack::PushL(entry);
	CheckGroupMembershipL(*entry,aExpectedCount);
	CheckFieldCountL(aPhonebook, entry); // Entry contains correct number of fields
	CleanupStack::PopAndDestroy();

	// Full view non-existent entry
	TContactItemId invalidUID(9877); 
	TRAP(err,entry = static_cast<CContactICCEntry*>(iDb->ReadContactL(invalidUID, *iDb->AllFieldsView())));
	TESTCHECKL(err, KErrNotFound);

	// Number view 
	CContactItemViewDef* numberView=CContactItemViewDef::NewL(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EMaskHiddenFields);
	CleanupStack::PushL(numberView);
	numberView->AddL(KUidContactFieldPhoneNumber); 
	TESTCHECKL(numberView->Count(), 1);
	TRAP(err, entry = static_cast<CContactICCEntry*>(iDb->ReadContactL(id, *numberView))); 
	TESTCHECKL(err, KErrNone);
	CleanupStack::PushL(entry);
	CheckGroupMembershipL(*entry,aExpectedCount);
	CheckFieldCountL(aPhonebook, entry); // Entry contains correct number of fields
	CleanupStack::PopAndDestroy();
	CleanupStack::PopAndDestroy();

	// Name view 
	CContactItemViewDef* nameView = CContactItemViewDef::NewL(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EMaskHiddenFields);
	CleanupStack::PushL(nameView);
	nameView->AddL(KUidContactFieldFamilyName); 
	TESTCHECKL(nameView->Count(), 1);
	TRAP(err, entry = static_cast<CContactICCEntry*>(iDb->ReadContactL(id, *nameView)));
	TESTCHECKL(err, KErrNone);
	CleanupStack::PushL(entry);
	CheckGroupMembershipL(*entry,aExpectedCount);
	CheckFieldCountL(aPhonebook, entry); // Entry contains correct number of fields
	CleanupStack::PopAndDestroy();
	CleanupStack::PopAndDestroy();

	// Invalid view 
	TUid invalidUid = {-2};
	CContactICCEntry* failEntry = NULL;
	CContactItemViewDef* invalidView=CContactItemViewDef::NewL(CContactItemViewDef::EMaskFields,CContactItemViewDef::EMaskHiddenFields);
	CleanupStack::PushL(invalidView);
	invalidView->AddL(invalidUid); 
	TESTCHECKL(invalidView->Count(), 1);
	TRAP(err, failEntry = (CContactICCEntry*)(iDb->ReadContactL(id, *invalidView)));
	TESTCHECKL(err, KErrNone); // valid id 
	CleanupStack::PushL(failEntry);
	CContactItemFieldSet& fieldset = failEntry->CardFields();
	const TInt pos(fieldset.Find(KUidContactFieldFamilyName));	
	CContactItemField& field = fieldset[pos];
	CContactTextField* textfield = field.TextStorage();
	TESTCHECKCONDITIONL(textfield->IsFull());
	CleanupStack::PopAndDestroy();
	CleanupStack::PopAndDestroy();
	SignalSimTsyToReduceTimersL();

	// Full view entry not available
	while(err==KErrNone) // Wait for ICC to become locked
		{
		CContactICCEntry* entry = NULL;
		User::After(1000000);
		TRAP(err, entry = (CContactICCEntry*)iDb->ReadContactL(id, *iDb->AllFieldsView()));
		if (!err)
			delete entry;
		}
	TESTCHECKL(err, KErrAccessDenied);

	CleanupStack::PopAndDestroy(group);	// group
	}

/**
 * Read an ICC contact - minimal read operation.
 */
void CPhbkReadIntegrationTestBase::ReadMinimalContactL(const TUid aPhonebook, TInt aExpectedCount)
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

	// Verify correct number of entries created for this phonebook
	const TInt count = array->Count(); // for debugging
	TESTCHECKL(count, aExpectedCount);

	// Read entry
	TContactItemId id = (*array)[0];
	CContactICCEntry* entry = NULL;
	TRAP(err, entry = static_cast<CContactICCEntry*>(iDb->ReadMinimalContactL(id)));
	TESTCHECKL(err, KErrNone);
	CleanupStack::PushL(entry);
	CheckMinimalFieldCountL(aPhonebook, entry); // Entry contains correct number of fields

	CleanupStack::PopAndDestroy(2); // group & entry
	}

/** 
 * Read minimal ICC entry - normal case.
 * Check group membership is correct and relevant number of fields have
 * been created.
 */
void CPhbkReadIntegrationTestBase::ReadAllMinimalContactL(const TUid aPhonebook, TInt aExpectedCount)
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

	// Verify correct number of entries created for this phonebook
	const TInt count = array->Count(); // for debugging
	TESTCHECKL(count, aExpectedCount);

	// Read each item
	for (TInt i=0; i<count; ++i)
		{
		TContactItemId id = (*array)[i];
		CContactICCEntry* entry = NULL;
		TRAP(err, entry = static_cast<CContactICCEntry*>(iDb->ReadMinimalContactL(id)));
		TESTCHECKL(err, KErrNone);
		CleanupStack::PushL(entry);
		// Verify correct number of fields created for entry
		CheckMinimalFieldCountL(aPhonebook, entry);
		CleanupStack::PopAndDestroy(); // entry
		}

	CleanupStack::PopAndDestroy(); // group
	}

/** 
 * Read minimal ICC entry - entry not available as the ICC becomes locked.
 */
void CPhbkReadIntegrationTestBase::ReadMinimalContactLockedL(const TUid aPhonebook, TInt aExpectedCount)
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
	SignalSimTsyToReduceTimersL();

	// Access ICC and wait for it to become locked
	TContactItemId id = (*array)[0]; 
	while(err==KErrNone) 
		{
		User::After(1000000);	// Wait a second and try again.
		CContactICCEntry* failEntry=NULL;
		TRAP(err, failEntry = (CContactICCEntry*)iDb->ReadMinimalContactL(id)); // Entry unavailable due to ICC being locked
		delete failEntry;
		}
	TESTCHECKL(err, KErrAccessDenied);
	SignalSimTsyToReduceTimersL();

	// Wait for ICC to become unlocked again
	while(err==KErrAccessDenied) 
		{
		User::After(1000000);	// Wait a second and try again.
		CContactICCEntry* failEntry=NULL;
		TRAP(err, failEntry = (CContactICCEntry*)iDb->ReadMinimalContactL(id)); // Entry unavailable due to ICC being locked
		delete failEntry;
		}
		
	WaitForSyncToCompleteL();

	// Read the item again
	CContactICCEntry* entry = NULL;
	TRAP(err, entry = static_cast<CContactICCEntry*>(iDb->ReadMinimalContactL(id)));
	TESTCHECKL(err, KErrNone);
	CleanupStack::PushL(entry);

	// Verify correct number of entries created for this phonebook
	const TInt count = array->Count(); // for debugging
	TESTCHECKL(count, aExpectedCount);

	// Verify correct number of fields created for entry
	CheckMinimalFieldCountL(aPhonebook, entry);

	CleanupStack::PopAndDestroy(2); // group and entry
	}

/** 
 * Read an ICC entry -  - create string from contact info (full view)
 */
void CPhbkReadIntegrationTestBase::ReadFullContactTextDefL(const TUid aPhonebook, TInt aExpectedCount, TBool aReadAll)
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

	// Verify correct number of entries created for this phonebook
	TInt count = array->Count(); // for debugging
	TESTCHECKL(count, aExpectedCount);

	// Read all items or only the first?
	if (!aReadAll)
		count = 1;
		
	for(TInt i=0; i<count; ++i)
		{
		TContactItemId id = (*array)[i];
		TBuf<RMobilePhone::KMaxMobileTelNumberSize> buf;
		TRAPD(err, iDb->ReadContactTextDefL(id,buf,NULL)); // Pass NULL as text definition - Contacts model will construct descriptor from the first fields that match
		TESTCHECKL(err, KErrNone);
		TESTCHECKCONDITIONL(buf.Length() != 0);
		}

	CleanupStack::PopAndDestroy(); // group
	}

/** 
 * Read an ICC entry -  - create number string from contact info
 */
void CPhbkReadIntegrationTestBase::ReadNumberContactTextDefL(const TUid aPhonebook, TInt aExpectedCount, TBool aReadAll)
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

	// Verify correct number of entries created for this phonebook
	TInt count = array->Count(); // for debugging
	TESTCHECKL(count, aExpectedCount);

	// Read all items or only the first?
	if (!aReadAll)
		count = 1;
		
	for(TInt i=0; i<count; ++i)
		{
		TContactItemId id = (*array)[i];
		CContactTextDef *textDef=CContactTextDef::NewL();
		CleanupStack::PushL(textDef);
		textDef->AppendL(TContactTextDefItem(KUidContactFieldPhoneNumber,_L("==")));
		TBuf<RMobilePhone::KMaxMobileTelNumberSize> buf;
		TRAPD(err, iDb->ReadContactTextDefL(id,buf,textDef));
		TESTCHECKL(err, KErrNone);
		TESTCHECKCONDITIONL(buf.Length() != 0);
		CleanupStack::PopAndDestroy(textDef);
		}

	CleanupStack::PopAndDestroy(); // group
	}

/** 
 * Read an ICC entry - create name string from contact info
 */
void CPhbkReadIntegrationTestBase::ReadNameContactTextDefL(const TUid aPhonebook, TInt aExpectedCount, TBool aReadAll)
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

	// Verify correct number of entries created for this phonebook
	TInt count = array->Count(); // for debugging
	TESTCHECKL(count, aExpectedCount);

	// Read all items or only the first?
	if (!aReadAll)
		count = 1;
		
	for(TInt i=0; i<count; ++i)
		{
		TContactItemId id = (*array)[i];
		CContactTextDef *textDef=CContactTextDef::NewL();
		CleanupStack::PushL(textDef);
		textDef->AppendL(TContactTextDefItem(KUidContactFieldFamilyName,_L("==")));
		TBuf<KMaxPhBkNameSize> buf;
		TRAPD(err, iDb->ReadContactTextDefL(id,buf,textDef));
		TESTCHECKL(err, KErrNone);
		TESTCHECKCONDITIONL(buf.Length() != 0);
		CleanupStack::PopAndDestroy(textDef);
		}

	CleanupStack::PopAndDestroy(); // group
	}

/** 
 * Read an ICC entry - invalid string specification
 */
void CPhbkReadIntegrationTestBase::ReadInvalidContactTextDefL(const TUid aPhonebook, TInt aExpectedCount)
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

	// Verify correct number of entries created for this phonebook
	const TInt count = array->Count(); // for debugging
	TESTCHECKL(count, aExpectedCount);

	// Read the first item
	TContactItemId id = (*array)[0];
	CContactTextDef *textDef=CContactTextDef::NewL();
	CleanupStack::PushL(textDef);
	textDef->SetExactMatchOnly(ETrue);
	textDef->AppendL(TContactTextDefItem(KUidContactFieldCompanyName,_L("==")));
	TBuf<KMaxPhBkNameSize> buf;
	TRAP(err, iDb->ReadContactTextDefL(id,buf,textDef));
	TESTCHECKL(err, KErrNone); // contact ID being read is valid and 
							   // the ICC contact can be read (ie. read validation was ok).
	TESTCHECKL(buf.Length(), 0); // field in the text definition does not 
								 // exist in an ICC contact so buf is empty 
	CleanupStack::PopAndDestroy(2); // group & textDef
	}

/** 
 * Read a locked ICC entry - create name string from contact info
 */
void CPhbkReadIntegrationTestBase::ReadLockedContactTextDefL(const TUid aPhonebook, TInt aExpectedCount)
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

	// Verify correct number of entries created for this phonebook
	const TInt count = array->Count(); // for debugging
	TESTCHECKL(count, aExpectedCount);

	// Read the first item
	TContactItemId id = (*array)[0];
	CContactTextDef *textDef=CContactTextDef::NewLC();
	textDef->AppendL(TContactTextDefItem(KUidContactFieldFamilyName,_L("==")));
	TBuf<KMaxPhBkNameSize> buf;
	SignalSimTsyToReduceTimersL();
	while(err==KErrNone) // Wait for ICC to become locked
		{
		User::After(5000);
		TRAP(err, iDb->ReadContactTextDefL(id,buf,textDef));
		}
	TESTCHECKL(err, KErrAccessDenied);

	CleanupStack::PopAndDestroy(2); // group & textDef
	}


/** 
 * Read all ICC entry - full view.
 * Check group membership is correct and relevant number of fields have
 * been created.
 */
void CPhbkReadIntegrationTestBase::ReadAllContactFullViewL(const TUid aPhonebook, TInt aExpectedCount)
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

	// Verify correct number of entries created for this phonebook
	const TInt count = array->Count(); // for debugging
	TESTCHECKL(count, aExpectedCount);

	for(TInt i=0; i<count; ++i)
		{
		// Full view 
		TContactItemId id = (*array)[i];
		CContactICCEntry* entry = static_cast<CContactICCEntry*>(iDb->ReadContactL(id, *iDb->AllFieldsView())); // specify full view
		CleanupStack::PushL(entry);
		CheckFieldCountL(aPhonebook, entry); // Entry contains correct number of fields
		CheckFieldsL(aPhonebook, entry, i + 1); // Entry contains correct content
		CleanupStack::PopAndDestroy(entry);
		}

	CleanupStack::PopAndDestroy(); // group
	}

/** 
 * Read all ICC entry - name view.
 * Check group membership is correct and relevant number of fields have
 * been created.
 */
void CPhbkReadIntegrationTestBase::ReadAllContactNameViewL(const TUid aPhonebook, TInt aExpectedCount)
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

	// Verify correct number of entries created for this phonebook
	const TInt count = array->Count(); // for debugging
	TESTCHECKL(count, aExpectedCount);

	for(TInt i=0; i<count; ++i)
		{
		// Name view 
		TContactItemId id = (*array)[i];
		CContactItemViewDef* view = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EMaskHiddenFields);
		view->AddL(KUidContactFieldFamilyName); 
		TESTCHECKL(view->Count(), 1);
		CContactICCEntry* entry = static_cast<CContactICCEntry*>(iDb->ReadContactL(id, *view));
		CleanupStack::PushL(entry);
		CheckFieldCountL(aPhonebook, entry); // Entry contains correct number of fields
		CleanupStack::PopAndDestroy(entry);
		CleanupStack::PopAndDestroy();	// view	
		}

	CleanupStack::PopAndDestroy(); // group
	}

/** 
 * Read all ICC entry - number view.
 * Check group membership is correct and relevant number of fields have
 * been created.
 */
void CPhbkReadIntegrationTestBase::ReadAllContactNumberViewL(const TUid aPhonebook, TInt aExpectedCount)
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

	// Verify correct number of entries created for this phonebook
	const TInt count = array->Count(); // for debugging
	TESTCHECKL(count, aExpectedCount);

	for(TInt i=0; i<count; ++i)
		{
		// Number view 
		TContactItemId id = (*array)[i];
		CContactItemViewDef* view = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EMaskHiddenFields);
		view->AddL(KUidContactFieldPhoneNumber); 
		TESTCHECKL(view->Count(), 1);
		CContactICCEntry* entry = static_cast<CContactICCEntry*>(iDb->ReadContactL(id, *view));
		CleanupStack::PushL(entry);
		CheckFieldCountL(aPhonebook, entry); // Entry contains correct number of fields
		CleanupStack::PopAndDestroy(entry);
		CleanupStack::PopAndDestroy();	// view	
		}

	CleanupStack::PopAndDestroy(); // group
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncReadAdditionalNumTagsTest object
 */
CPhbkSyncReadAdditionalNumTagsTest* CPhbkSyncReadAdditionalNumTagsTest::NewL()
	{
	CPhbkSyncReadAdditionalNumTagsTest* self = new(ELeave) CPhbkSyncReadAdditionalNumTagsTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncReadAdditionalNumTagsTest::CPhbkSyncReadAdditionalNumTagsTest()
	{
	SetTestStepName(_L("ReadAdditionalNumTagsTest"));
	}

/** 
 * Read ICC entries with additional numbers and alpha tags
 */
enum TVerdict CPhbkSyncReadAdditionalNumTagsTest::doTestStepL()
	{
	// ensure contact database entries are removed because this test demands that 
	// the order of new entry IDs corresponds to the order of the test data entries
	delete iDb;
	iDb=NULL;
	User::After(1000000); //1 second wait for contact to shut down
	
	iSession.Close();
	ConfigurePhbkSyncToIdleL();
	TRAP_IGNORE(CContactDatabase::DeleteDefaultFileL());	
	ConfigurePhbkSyncToFullL();
	iDb = OpenOrCreateContactDatabaseL();
	User::After(1000000); //1 second wait for contact to start the phbk server
	User::LeaveIfError(iSession.Connect());
	User::After(3000000); //3 second wait for server to create phbktable

	SetSimTsyTestNumberL(22);
	DoSyncL();

	iDb->SetDbViewContactType(KUidContactICCEntry);
	// get the unique groupId for the USIM App phonebook
	TContactItemId groupId(KNullContactId);
	User::LeaveIfError(iSession.GetPhoneBookId(groupId, 
						RPhoneBookSession::ESyncGroupId,
						KUidUsimAppAdnPhonebook));
	TESTCHECKCONDITIONL(groupId != KNullContactId);
  
	// based on the groupId, get items belonging to the phonebook
	CContactGroup* group = NULL;
	TRAPD(err, group = static_cast<CContactGroup*>(iDb->ReadContactL(groupId)));
	TESTCHECKL(err, KErrNone);
	CleanupStack::PushL(group);
	const CContactIdArray* array = group->ItemsContained();

	const TInt count = array->Count(); // for debugging
	for(TInt i=0; i<count; ++i)
		{
		TContactItemId id = (*array)[i];
		CContactICCEntry* entry = static_cast<CContactICCEntry*>(iDb->ReadContactL(id));
		CleanupStack::PushL(entry);
		CContactItemFieldSet& fieldset = entry->CardFields();
		TInt pos(KErrNotFound);
		switch (i)
		{
		case 0:
			pos = fieldset.Find(KUidContactFieldVCardMapMSG);
			break;
		case 1:
			pos = fieldset.Find(KUidContactFieldVCardMapVOICE);
			break;
		case 2:
			pos = fieldset.Find(KUidContactFieldVCardMapFAX);
			break;
		case 3:
			pos = fieldset.Find(KUidContactFieldVCardMapPREF);
			break;
		case 4:
			pos = fieldset.Find(KUidContactFieldVCardMapCELL);
			break;
		case 5:
			pos = fieldset.Find(KUidContactFieldVCardMapPAGER);
			break;
		case 6:
			pos = fieldset.Find(KUidContactFieldVCardMapBBS);
			break;
		case 7:
			pos = fieldset.Find(KUidContactFieldVCardMapMODEM);
			break;
		case 8:
			pos = fieldset.Find(KUidContactFieldVCardMapCAR);
			break;
		case 9:
			pos = fieldset.Find(KUidContactFieldVCardMapISDN);
			break;
		case 10:
			pos = fieldset.Find(KUidContactFieldVCardMapVIDEO);
			break;

		default:
			break;
		}
		TESTCHECKCONDITIONL(pos != KErrNotFound);
		CleanupStack::PopAndDestroy(entry);
		}

	CleanupStack::PopAndDestroy(); // group
	return TestStepResult();
	}
	
/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncReadInternationalNumberTest object
 */
CPhbkSyncReadInternationalNumberTest* CPhbkSyncReadInternationalNumberTest::NewL()
	{
	CPhbkSyncReadInternationalNumberTest* self = new(ELeave) CPhbkSyncReadInternationalNumberTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncReadInternationalNumberTest::CPhbkSyncReadInternationalNumberTest()
	{
	SetTestStepName(_L("ReadInternationalNumberTest"));
	}

/** 
 * Read ICC entries with international numbers.
 */
enum TVerdict CPhbkSyncReadInternationalNumberTest::doTestStepL()
	{
	// Clear out the Contacts Database to start from afresh for this test.
	delete iDb;
	iDb=NULL;
	User::After(1000000); //1 second wait for contact to shut down
	
	iSession.Close();
	ConfigurePhbkSyncToIdleL();
	TRAP_IGNORE(CContactDatabase::DeleteDefaultFileL());	
	ConfigurePhbkSyncToFullL();
	iDb = OpenOrCreateContactDatabaseL();
	User::After(1000000); //1 second wait for contact to start the phbk server
	User::LeaveIfError(iSession.Connect());
	User::After(3000000); //3 second wait for server to create phbktable

	SetSimTsyTestNumberL(136);
	
	// Sync the USIM App ADN Phonebook
	TRequestStatus status = KErrNone;
	iSession.DoSynchronisation(status, KUidUsimAppAdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
		
	iDb->SetDbViewContactType(KUidContactICCEntry);	
	
	// Search for a particular contact in the database
	
	// Specify which fields to search
	CContactItemFieldDef* fieldDef = new(ELeave) CContactItemFieldDef;
	CleanupStack::PushL(fieldDef);
	fieldDef->AppendL(KUidContactFieldFamilyName); 
	
	// Get the contact item ID of the contact we are looking for
	CContactIdArray* array = iDb->FindLC(KTestPersonWithInternationalNumber, fieldDef);
	TESTCHECKL(array->Count(),1);
  	
  	TContactItemId contactItemId = (*array)[0];
  	
  	CleanupStack::PopAndDestroy(2, fieldDef); // array, fieldDef
  	
  	// Get the contact item
	CContactItem* contactItem = iDb->ReadContactL(contactItemId);
	CleanupStack::PushL(contactItem);
	
	CContactItemFieldSet& fieldset = contactItem->CardFields();
	
	// Compare the international numbers with what is expected
	// index stores the field position to search from since all
	// the numbers are stored in multiple instances of the same field type.	
	TInt index = fieldset.Find(KUidContactFieldPhoneNumber);
	CheckFieldContentL(fieldset, KUidContactFieldPhoneNumber, KTestInternationalNumber1);

	index = fieldset.FindNext(KUidContactFieldPhoneNumber, index+1);
	CheckFieldContentL(fieldset, KUidContactFieldPhoneNumber, index, KTestInternationalNumber2);

	index = fieldset.FindNext(KUidContactFieldPhoneNumber, index+1);
	CheckFieldContentL(fieldset, KUidContactFieldPhoneNumber, index, KTestInternationalNumber1);

	CleanupStack::PopAndDestroy(contactItem);
	
	return TestStepResult();
	}


/** 
 *  Tests that an ICC contact can be read without starting PhBkSync first.
 * 
 *  @return EPass if the successful was successful.
 */
TVerdict CPhbkSyncReadWithOutOpeningPhBkSyncFirstTest::doTestStepL()
	{
	//
	// Setup SIMTSY so some contacts exist on the SIM and sync them...
	//
	SetSyncModeL(RPhoneBookSession::EManual);
	CheckSyncModeL(RPhoneBookSession::EManual);
	SetSimTsyTestNumberL(0);
	DoSyncL();

	//
	// Close the Contacts Database as may have a handle to Phonebook Sync...
	//
    delete iDb;
	iDb=NULL;

	//
	// Close the Phonebook Sync handle and tell the server to shutdown and
	// unload. This is required for this test to work and for security
	// reasons is only possible with a debug server.
	//
#if _DEBUG
	iSession.ShutdownServer(ETrue);
	iSession.Close();
	User::After(1); // To ensure the close / server shutdown is processed.
#else
	INFO_PRINTF1(_L("<font color=Orange>This test is only valid in UDEB!</font>"));
	iSession.Close();
#endif

	//
	// Now open Contacts Database (Phonebook Sync will not be automatically
	// opened)...
	//
	iDb = OpenOrCreateContactDatabaseL();

	//
	// Find an ICC contact ID to read...
	//
	iDb->SetDbViewContactType(KUidContactICCEntry);

	TContactIter  iter(*iDb);
    TContactItemId  contactId = iter.FirstL();
    
    TESTCHECKCONDITIONL(contactId != KNullContactId);
    
    //
    // Try and read the contact...
    //
	CContactItem*  item = NULL;

	TRAPD(err, item = iDb->ReadContactL(contactId));
#if _DEBUG
	TESTCHECK(err, KErrNotFound);
#else
	TESTCHECK(err, KErrNone);
	TESTCHECKCONDITION(item != NULL);
#endif
	delete item;
	item = NULL;
	
	//
	// Ensuring Phonebook Sync is running and synchronised...
	//
	TInt  ret = iSession.Connect();
	TESTCHECKL(ret, KErrNone);

	ConfigurePhbkSyncToFullL();
	DoSyncL();

	//
    // Now repeat the test...
    //
	TRAP(err, item = iDb->ReadContactL(contactId));
	TESTCHECK(err, KErrNone);
	TESTCHECKCONDITION(item != NULL);
	delete item;
	item = NULL;

	return TestStepResult();
	} // CPhbkSyncReadWithOutOpeningPhBkSyncFirstTest::doTestStepL

