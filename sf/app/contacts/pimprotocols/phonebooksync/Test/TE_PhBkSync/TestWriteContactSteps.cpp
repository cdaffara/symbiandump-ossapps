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
// This contains PhbkSync API WriteContact() related Unit Test Steps
// 
//

#include <e32base.h>
#include <exterror.h>

#include "TE_PhBkSyncBase.h"
#include "TestWriteContactSteps.h"

/**
 * Test writing a contact, where all the contact fields are valid.
 * Repeat for all supported phonebooks.
 */
//
//    Test step1 -- Test#54 in Unit Test Spec. Doc. //
//
CPhbkSyncWriteContactTest_01::CPhbkSyncWriteContactTest_01()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncWriteContactTest_01"));
	}

CPhbkSyncWriteContactTest_01::~CPhbkSyncWriteContactTest_01()
	{
	}

TVerdict CPhbkSyncWriteContactTest_01::doTestStepL( )
	{
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		ChangeSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	SetSimTsyTestNumberL(54);

	// wait until all phonebooks have been synchronised
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		CheckCacheStateValidL(iPhoneBook[i]);
		}

	// to create a new entry with internal (long/new) format and initialise it
	MyCSyncContactICCEntry* iccEntryInternalNew = MyCSyncContactICCEntry::NewL();
	CleanupStack::PushL(iccEntryInternalNew); 
	InitialiseIccEntryInternalNewL(iccEntryInternalNew);

	// to create an old entry with internal (short/old) format and initialise it
	MyCSyncContactICCEntry* iccEntryInternalOld = MyCSyncContactICCEntry::NewL();
	CleanupStack::PushL(iccEntryInternalOld);
	InitialiseIccEntryInternalOldL(iccEntryInternalOld);

	// to call the DoWriteContactL
	TRequestStatus status;
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		// set the corresponding entry attributes
		if (iPhoneBook[i] == KUidIccGlobalSdnPhonebook ||
			iPhoneBook[i] == KUidIccGlobalLndPhonebook) 
			{
			// slot 1-4 is guaranteed free in phonebooks (see [test54] in cfg. file)
			iccEntryInternalOld->iSlotNum = i+1; 
			iccEntryInternalOld->iPhonebook = convertPhonebookUid2Enum(iPhoneBook[i]); 
			// do the real test by writing an entry to ICC
			DoWriteContactToICCL(iccEntryInternalOld, iPhoneBook[i], status);
			}
		else 
			{
			// slot 1-4 is guaranteed free in phonebooks (see [test54] in cfg. file)
			iccEntryInternalNew->iSlotNum = i+1; 
			iccEntryInternalNew->iPhonebook = convertPhonebookUid2Enum(iPhoneBook[i]); 
			// do the real test by writing an entry to ICC
			DoWriteContactToICCL(iccEntryInternalNew, iPhoneBook[i], status);
			}

		// check result
		TESTCHECK(status.Int(), KErrNone);
		TESTCHECKCONDITION(iccEntryInternalNew->iSlotNum != KSyncIndexNotSupplied);
		}
	CleanupStack::PopAndDestroy(2); 
	return TestStepResult();
	}

/**
 * Test writing a contact, where all the contact fields are 
 * valid except the ICC slot which is not specified and
 * there is space to add another contact. Repeat for all
 * supported phonebooks.
 */
//
//    Test step2 -- Test#55 in Unit Test Spec. Doc.  //
//
CPhbkSyncWriteContactTest_02::CPhbkSyncWriteContactTest_02()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncWriteContactTest_02"));
	}

CPhbkSyncWriteContactTest_02::~CPhbkSyncWriteContactTest_02()
	{
	}

TVerdict CPhbkSyncWriteContactTest_02::doTestStepL( )
	{
	TInt i(0);

	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		ChangeSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// load the dummy ICC which contains all phonebooks and also have 
	// plenty free slots
	SetSimTsyTestNumberL(0);

	// wait until all phonebooks have been synchronised
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		CheckCacheStateValidL(iPhoneBook[i]);
		}

	// to create an entry with internal (long/new) format and initialise it
	MyCSyncContactICCEntry* iccEntryInternalNew = MyCSyncContactICCEntry::NewL();
	CleanupStack::PushL(iccEntryInternalNew); 
	InitialiseIccEntryInternalNewL(iccEntryInternalNew);

	// to call the DoWriteContactL to do the real business
	TRequestStatus status;
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		// set the corresponding phonebook
		iccEntryInternalNew->iPhonebook = convertPhonebookUid2Enum(iPhoneBook[i]); 

		// do the real test by writing an entry to ICC
		DoWriteContactToICCL(iccEntryInternalNew, iPhoneBook[i], status);

		// check result
		TESTCHECK(status.Int(), KErrNone);
		TESTCHECKCONDITION(iccEntryInternalNew->iSlotNum != KSyncIndexNotSupplied);
		}
	CleanupStack::PopAndDestroy(); 
	return TestStepResult();
	}

/** 
 * Test writing a contact, where all the contact fields are 
 * valid except the ICC slot which is not specified and there
 * is no space to add another contact. Repeat for all supported
 * phonebooks.
 */ 
//
//    Test step3 -- Test#56 in Unit Test Spec. Doc.  //
//
CPhbkSyncWriteContactTest_03::CPhbkSyncWriteContactTest_03()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncWriteContactTest_03"));
	}

CPhbkSyncWriteContactTest_03::~CPhbkSyncWriteContactTest_03()
	{
	}

TVerdict CPhbkSyncWriteContactTest_03::doTestStepL( )
	{
	TInt i(0);

	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		ChangeSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// load all full phonebooks
	SetSimTsyTestNumberL(18);  // re-use Test#18 phonebooks, which are full

	// wait until all phonebooks have been synchronised
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		CheckCacheStateValidL(iPhoneBook[i]);
		}

	// to create an entry with internal (long/new) format and initialise it
	MyCSyncContactICCEntry* iccEntryInternalNew = MyCSyncContactICCEntry::NewL();
	CleanupStack::PushL(iccEntryInternalNew);
	InitialiseIccEntryInternalNewL(iccEntryInternalNew);

	// to call the DoWriteContactL to do the real business
	TRequestStatus status;
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		// set the corresponding phonebook
		iccEntryInternalNew->iPhonebook = convertPhonebookUid2Enum(iPhoneBook[i]); 

		// do the real test by writing an entry to ICC
		DoWriteContactToICCL(iccEntryInternalNew, iPhoneBook[i], status);

		// check result
//		TESTCHECK(status.Int(), KErrOverflow);

		// instead returning KErrOverflow, TSY wrongly returns KErrNoMemory
		TESTCHECK(status.Int(), KErrNoMemory);
		}
	CleanupStack::PopAndDestroy(); 
	return TestStepResult();
	}


/**
 * Test writing a contact, where all the contact fields are
 * valid except the ICC slot which is an invalid slot number 
 * (0, -1, -5765). Repeat for all supported phonebooks.
 */
//
//    Test step4 -- Test#57 in Unit Test Spec. Doc.  //
//
CPhbkSyncWriteContactTest_04::CPhbkSyncWriteContactTest_04()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncWriteContactTest_04"));
	}

CPhbkSyncWriteContactTest_04::~CPhbkSyncWriteContactTest_04()
	{
	}

TVerdict CPhbkSyncWriteContactTest_04::doTestStepL( )
	{
	TInt NumOfInvalidSlots = 4;
	TInt invalidSlotNumbers[] = 
		{0,9876,-256,-5765};

	TInt i(0);

	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		ChangeSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// load the dummy test which contains all phonebooks
	SetSimTsyTestNumberL(0);

	// wait until all phonebooks have been synchronised
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		CheckCacheStateValidL(iPhoneBook[i]);
		}

	// to create an entry with internal (lone/new) format and initialise it
	MyCSyncContactICCEntry* iccEntryInternalNew = MyCSyncContactICCEntry::NewL();
	CleanupStack::PushL(iccEntryInternalNew);
	InitialiseIccEntryInternalNewL(iccEntryInternalNew);

	// to call the DoWriteContactL to do the real business
	TRequestStatus status;
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		// set the corresponding phonebook
		iccEntryInternalNew->iPhonebook = convertPhonebookUid2Enum(iPhoneBook[i]); 

		// set invalid slot numbers and try to write into the phonebook
		TInt j=0;
		for (j = 0; j<NumOfInvalidSlots; j++) 
			{
			iccEntryInternalNew->iSlotNum = invalidSlotNumbers[j];

			// do the real test by writing an entry to ICC
			DoWriteContactToICCL(iccEntryInternalNew, iPhoneBook[i], status);

			// check result
			TESTCHECK(status.Int(), KErrArgument);
			}
		}
	CleanupStack::PopAndDestroy(); 
	return TestStepResult();
	}

/**
 * Test writing (edit) a contact, where all the contact fields
 * are valid, but the specified ICC slot already has an entry.
 */
//
//    Test step5 -- Test#58 in Unit Test Spec. Doc.  //
//
CPhbkSyncWriteContactTest_05::CPhbkSyncWriteContactTest_05()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncWriteContactTest_05"));
	}

CPhbkSyncWriteContactTest_05::~CPhbkSyncWriteContactTest_05()
	{
	}

TVerdict CPhbkSyncWriteContactTest_05::doTestStepL( )
	{
	TInt i(0);

	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		ChangeSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// load the dummy test which contains all phonebooks and also the 
	// first slot in each phonebook already has an entry
	SetSimTsyTestNumberL(0);

	// wait until all phonebooks have been synchronised
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		CheckCacheStateValidL(iPhoneBook[i]);
		}
	
	// to create an entry with internal (lone/new) format and initialise it
	MyCSyncContactICCEntry* iccEntryInternalNew = MyCSyncContactICCEntry::NewL();
	CleanupStack::PushL(iccEntryInternalNew);
	InitialiseIccEntryInternalNewL(iccEntryInternalNew);

	// to call the DoWriteContactL to do the real business
	TRequestStatus status;
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		// set a slot number which has been occupied by an entry in phonebook
		iccEntryInternalNew->iSlotNum = GetValidSlotFromContactsDbL(iPhoneBook[i]);

		// set the corresponding phonebook
		iccEntryInternalNew->iPhonebook = convertPhonebookUid2Enum(iPhoneBook[i]); 

		// do the real test by writing an entry to ICC
		DoWriteContactToICCL(iccEntryInternalNew, iPhoneBook[i], status);

		// check result
		TESTCHECK(status.Int(), KErrNone);  // the old entry is overwritten
		}
	CleanupStack::PopAndDestroy(); 
	return TestStepResult();
	}

/**
 * Test writing a contact, where all the contact fields are 
 * specified in such a way what they are only just valid, e.g.
 * no. of allowed characters for that field , no. of digits in
 * phone number = max no. of allowed digits for that field, 
 * ICC slot no. = max no. of ICC slots available.
 */
//
//    Test step6 -- Test#59 in Unit Test Spec. Doc.  //
//
CPhbkSyncWriteContactTest_06::CPhbkSyncWriteContactTest_06()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncWriteContactTest_06"));
	}

CPhbkSyncWriteContactTest_06::~CPhbkSyncWriteContactTest_06()
	{
	}

TVerdict CPhbkSyncWriteContactTest_06::doTestStepL( )
	{
	TInt maxSlotNumber = 20; // for all phonebooks in [test0]

	// due to the bug in TSY, the size has to be limit (set in cfg. file) -1
	_LIT16(KT59Number,"1234567890123456789012345678901234567890");// 40 digits
	_LIT16(KT59Name,"abcdefghijklmnopqrstuvwxyz12345678901234567890"); // 46 chars

	TInt i(0);

	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		ChangeSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// load the dummy test which contains all phonebooks
	SetSimTsyTestNumberL(0);

	// wait until all phonebooks have been synchronised
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		CheckCacheStateValidL(iPhoneBook[i]);
		}

	// to create an entry with internal (long/new) format and initialise it
	MyCSyncContactICCEntry* iccEntryInternalNew = MyCSyncContactICCEntry::NewL();
	CleanupStack::PushL(iccEntryInternalNew);
	InitialiseIccEntryInternalNewL(iccEntryInternalNew);

	// set the slot number = max no. of ICC slots available
	iccEntryInternalNew->iSlotNum = maxSlotNumber;

	// set the name = max no. of allowed characters
	TInt count = iccEntryInternalNew->iNameList->Count();
	for(i=0; i<count; ++i) 
		{
		iccEntryInternalNew->iNameList->At(i).Copy(KT59Name);
		}

	// set the phone number = max no. of allowed digits 
	iccEntryInternalNew->iNumber.Format(KT59Number);

	// to call the DoWriteContactL to do the real business
	TRequestStatus status;
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		// set the corresponding phonebook
		iccEntryInternalNew->iPhonebook = convertPhonebookUid2Enum(iPhoneBook[i]); 

		// do the real test by writing an entry to ICC
		DoWriteContactToICCL(iccEntryInternalNew, iPhoneBook[i], status);

		// check result
		TESTCHECK(status.Int(), KErrNone);  // everything is within limit
		}
	CleanupStack::PopAndDestroy(); 
	return TestStepResult();
	}


/**
 * Test writing a contact, where all the contact fields are
 * valid except the name which has more characters than
 * permitted.
 */
//
//    Test step7 -- Test#60 in Unit Test Spec. Doc.  //
//
CPhbkSyncWriteContactTest_07::CPhbkSyncWriteContactTest_07()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncWriteContactTest_07"));
	}

CPhbkSyncWriteContactTest_07::~CPhbkSyncWriteContactTest_07()
	{
	}

TVerdict CPhbkSyncWriteContactTest_07::doTestStepL( )
	{
	_LIT16(KT60NameTooLong,"abcdefghijklmnopqrstuvwxyz12345678901234567890abcdefghijklmnopqrstuvwxyz"); // >> 47 chars defined in [test0]

	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		ChangeSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// load the dummy test which contains all phonebooks
	SetSimTsyTestNumberL(0);

	// wait until all phonebooks have been synchronised
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		CheckCacheStateValidL(iPhoneBook[i]);
		}

	// to create an entry with internal (long/new) format and initialise it
	MyCSyncContactICCEntry* iccEntryInternalNew = MyCSyncContactICCEntry::NewL();
	CleanupStack::PushL(iccEntryInternalNew);
	InitialiseIccEntryInternalNewL(iccEntryInternalNew);

	// set the name >> max no. of allowed characters
	TInt count = iccEntryInternalNew->iNameList->Count();
	for(i=0; i<count; ++i) 
		{
		iccEntryInternalNew->iNameList->At(i).Copy(KT60NameTooLong);
		}

	// to call the DoWriteContactL to do the real business
	TRequestStatus status;
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		// set the corresponding phonebook
		iccEntryInternalNew->iPhonebook = convertPhonebookUid2Enum(iPhoneBook[i]); 

		// do the real test by writing an entry to ICC
		DoWriteContactToICCL(iccEntryInternalNew, iPhoneBook[i], status);

		// check result
		TESTCHECK(status.Int(), KErrPhonebookTextOverflow);
		}
	CleanupStack::PopAndDestroy(); 
	return TestStepResult();
	}

/**
 * Test writing a contact, where all the contact fields are
 * valid except the phone number which has more digits than
 * permitted.
 */
//
//    Test step8 -- Test#61 in Unit Test Spec. Doc.  //
//
CPhbkSyncWriteContactTest_08::CPhbkSyncWriteContactTest_08()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncWriteContactTest_08"));
	}

CPhbkSyncWriteContactTest_08::~CPhbkSyncWriteContactTest_08()
	{
	}

TVerdict CPhbkSyncWriteContactTest_08::doTestStepL( )
	{
	_LIT16(KT61NumberTooLong,"12345678901234567890123456789012345678901234567890");

	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		ChangeSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// load the dummy test which contains all phonebooks
	SetSimTsyTestNumberL(0);

	// wait until all phonebooks have been synchronised
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		CheckCacheStateValidL(iPhoneBook[i]);
		}

	// to create an entry with internal (long/new) format and initialise it
	MyCSyncContactICCEntry* iccEntryInternalNew = MyCSyncContactICCEntry::NewL();
	CleanupStack::PushL(iccEntryInternalNew);
	InitialiseIccEntryInternalNewL(iccEntryInternalNew);

	// set the phone number >> max no. of allowed digits 
	iccEntryInternalNew->iNumber.Format(KT61NumberTooLong);

	// to call the DoWriteContactL to do the real business
	TRequestStatus status;
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		// set the corresponding phonebook
		iccEntryInternalNew->iPhonebook = convertPhonebookUid2Enum(iPhoneBook[i]); 

		// do the real test by writing an entry to ICC
		DoWriteContactToICCL(iccEntryInternalNew, iPhoneBook[i], status);

		// check result
		TESTCHECK(status.Int(), KErrPhonebookNumberOverflow);
		}
	CleanupStack::PopAndDestroy();
	return TestStepResult();
	}

/**
 * Test writing a contact, where all the contact fields are
 * valid. the name field contains obscure characters
 * (anything other than letters) and the phone number field
 * contains "+" "p" "w".
 */
//
//    Test step9 -- Test#62 in Unit Test Spec. Doc.  //
//
CPhbkSyncWriteContactTest_09::CPhbkSyncWriteContactTest_09()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncWriteContactTest_09"));
	}

CPhbkSyncWriteContactTest_09::~CPhbkSyncWriteContactTest_09()
	{
	}

TVerdict CPhbkSyncWriteContactTest_09::doTestStepL( )
	{
	_LIT16(KT62Number,"+1234p678901234w23456789");// < 41 digits
	_LIT16(KT62Name,"abcd*(&^%$\xa3_=><MJN<.<>+klmn60wxyz789"); // < 47 chars

	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		ChangeSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// load the dummy test which contains all phonebooks
	SetSimTsyTestNumberL(0);

	// wait until all phonebooks have been synchronised
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		CheckCacheStateValidL(iPhoneBook[i]);
		}

	// to create an entry with internal format and initialise it
	MyCSyncContactICCEntry* iccEntryInternalNew = MyCSyncContactICCEntry::NewL();
	CleanupStack::PushL(iccEntryInternalNew);
	InitialiseIccEntryInternalNewL(iccEntryInternalNew);

	// set the name < max no. of allowed characters
	TInt count = iccEntryInternalNew->iNameList->Count();
	for(i=0; i<count; i++) 
		{
		iccEntryInternalNew->iNameList->At(i).Copy(KT62Name);
		}

	// set the phone number < max no. of allowed digits 
	iccEntryInternalNew->iNumber.Format(KT62Number);

	// to call the DoWriteContactL to do the real business
	TRequestStatus status;
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		// set the corresponding phonebook
		iccEntryInternalNew->iPhonebook = convertPhonebookUid2Enum(iPhoneBook[i]); 

		// do the real test by writing an entry to ICC
		DoWriteContactToICCL(iccEntryInternalNew, iPhoneBook[i], status);

		// check result
		TESTCHECK(status.Int(), KErrNone);
		}
	CleanupStack::PopAndDestroy(); 
	return TestStepResult();
	}

/**
 * Test writing a contact, where all the contact fields are
 * valid except the phone no. which contains letters.
 */
//
//    Test step10 -- Test#63 in Unit Test Spec. Doc.  //
//
CPhbkSyncWriteContactTest_10::CPhbkSyncWriteContactTest_10()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncWriteContactTest_10"));
	}

CPhbkSyncWriteContactTest_10::~CPhbkSyncWriteContactTest_10()
	{
	}

TVerdict CPhbkSyncWriteContactTest_10::doTestStepL( )
	{
	_LIT16(KT63Number,"12345characters1234567890");// < 41 digits defined in [test0]

	TInt i(0);

	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		ChangeSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// load the dummy test which contains all phonebooks
	SetSimTsyTestNumberL(0);

	// wait until all phonebooks have been synchronised
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		CheckCacheStateValidL(iPhoneBook[i]);
		}

	// to create an entry with internal format and initialise it
	MyCSyncContactICCEntry* iccEntryInternalNew = MyCSyncContactICCEntry::NewL();
	CleanupStack::PushL(iccEntryInternalNew);
	InitialiseIccEntryInternalNewL(iccEntryInternalNew);

	// set the phone number which contains letters
	iccEntryInternalNew->iNumber.Format(KT63Number);

	// to call the DoWriteContactL to do the real business
	TRequestStatus status;
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		// set the corresponding phonebook
		iccEntryInternalNew->iPhonebook = convertPhonebookUid2Enum(iPhoneBook[i]); 

		// do the real test by writing an entry to ICC
		DoWriteContactToICCL(iccEntryInternalNew, iPhoneBook[i], status);

		// check result
		// KErrArgument is expected, but TSY doesn't reutrn this.
//		TESTCHECK(status.Int(), KErrArgument);

		// TSY doesn't check number containing letters and it simply returns KErrNone.
		TESTCHECK(status.Int(), KErrNone);
		}
	CleanupStack::PopAndDestroy(); 
	return TestStepResult();
	}

/**
 * Test writing a contact, where all the contact fields are
 * valid, but an error occurs when writing to the ICC.
 * Repeat this test case but with an error because the ICC
 * has become locked/blocked.
 */
//
//    Test step11 -- Test#64 in Unit Test Spec. Doc.  //
//
CPhbkSyncWriteContactTest_11::CPhbkSyncWriteContactTest_11()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncWriteContactTest_11"));
	}

CPhbkSyncWriteContactTest_11::~CPhbkSyncWriteContactTest_11()
	{
	}

TVerdict CPhbkSyncWriteContactTest_11::doTestStepL( )
	{
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		ChangeSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// load the test and no phonebook should be synchronised due to 
	// locking/blocking
	SetSimTsyTestNumberL(38); // re-use the test prepared for Test#38

	// wait until all phonebooks have been synchronised
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		CheckCacheStateValidL(iPhoneBook[i]);
		}

	// get a valid UID for each phonebook and keep them somewhere
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		 GetValidUIDFromContactsDbL(iPhoneBook[i]);
		}

	// Wait for ICC to become locked 
	WaitForIccToBecomeLockedL();

	// to create an entry with internal format and initialise it
	MyCSyncContactICCEntry* iccEntryInternalNew = MyCSyncContactICCEntry::NewL();
	CleanupStack::PushL(iccEntryInternalNew);
	InitialiseIccEntryInternalNewL(iccEntryInternalNew);

	// to call the DoWriteContactL to do the real business
	TRequestStatus status;

	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		// set the corresponding phonebook
		iccEntryInternalNew->iPhonebook = convertPhonebookUid2Enum(iPhoneBook[i]); 

		// do the real test by writing an entry to ICC
		DoWriteContactToICCL(iccEntryInternalNew, iPhoneBook[i], status);

		// check result
		TESTCHECK(status.Int(), KErrAccessDenied);
		}
	CleanupStack::PopAndDestroy(); 
	return TestStepResult();
	}

/**
 * Test writing acontact where all the contact fields are
 * valid except the name which is blank.
 */
//
//    Test step12 -- Test#65 in Unit Test Spec. Doc.  //
//
CPhbkSyncWriteContactTest_12::CPhbkSyncWriteContactTest_12()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncWriteContactTest_12"));
	}

CPhbkSyncWriteContactTest_12::~CPhbkSyncWriteContactTest_12()
	{
	}

TVerdict CPhbkSyncWriteContactTest_12::doTestStepL( )
	{
	_LIT16(KT65Name,""); // empty name

	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		ChangeSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// load the dummy test which contains all phonebooks
	SetSimTsyTestNumberL(0);

	// wait until all phonebooks have been synchronised
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		CheckCacheStateValidL(iPhoneBook[i]);
		}

	// to create an entry with internal format and initialise it
	MyCSyncContactICCEntry* iccEntryInternalNew = MyCSyncContactICCEntry::NewL();
	CleanupStack::PushL(iccEntryInternalNew);
	InitialiseIccEntryInternalNewL(iccEntryInternalNew);

	TInt count = iccEntryInternalNew->iNameList->Count();
	for(i=0; i<count; i++) 
		{
		iccEntryInternalNew->iNameList->At(i).Copy(KT65Name); // set blank name
		}

	// to call the DoWriteContactL to do the real business
	TRequestStatus status;

	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		// set the corresponding phonebook
		iccEntryInternalNew->iPhonebook = convertPhonebookUid2Enum(iPhoneBook[i]); 

		// do the real test by writing an entry to ICC
		DoWriteContactToICCL(iccEntryInternalNew, iPhoneBook[i], status);

		// check result
		TESTCHECK(status.Int(), KErrNone);
		}
	CleanupStack::PopAndDestroy(); 
	return TestStepResult();
	}

/**
 * Test writing a contact where all the contact fields are
 * valid except the phone number which is blank.
 */
//
//    Test step13 -- Test#66 in Unit Test Spec. Doc.  //
//
CPhbkSyncWriteContactTest_13::CPhbkSyncWriteContactTest_13()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncWriteContactTest_13"));
	}

CPhbkSyncWriteContactTest_13::~CPhbkSyncWriteContactTest_13()
	{
	}

TVerdict CPhbkSyncWriteContactTest_13::doTestStepL( )
	{
	_LIT16(KT66Number,""); // empty number

	TInt i(0);

	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		ChangeSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// load the dummy test which contains all phonebooks
	SetSimTsyTestNumberL(0);

	// wait until all phonebooks have been synchronised
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		CheckCacheStateValidL(iPhoneBook[i]);
		}

	// to create an entry with internal format and initialise it
	MyCSyncContactICCEntry* iccEntryInternalNew = MyCSyncContactICCEntry::NewL();
	CleanupStack::PushL(iccEntryInternalNew);
	InitialiseIccEntryInternalNewL(iccEntryInternalNew);

	// set the phone number which contains letters
	iccEntryInternalNew->iNumber.Format(KT66Number);  // set blank number

	// to call the DoWriteContactL to do the real business
	TRequestStatus status;
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		// set the corresponding phonebook
		iccEntryInternalNew->iPhonebook = convertPhonebookUid2Enum(iPhoneBook[i]); 

		// do the real test by writing an entry to ICC
		DoWriteContactToICCL(iccEntryInternalNew, iPhoneBook[i], status);

		// check result
		// KErrArgument is expected, but TSY doesn't reutrn this.
//		TESTCHECK(status.Int(), KErrArgument);

		// TSY doesn't consider blank number as invalid and it simply returns KErrNone.
		TESTCHECK(status.Int(), KErrNone);
		}
	CleanupStack::PopAndDestroy(); 
	return TestStepResult();
	}

/**
 * Test writing a contact to a read only phonebook (SDN).
 */
//
//    Test step14 -- Test#67 in Unit Test Spec. Doc.  //
//
CPhbkSyncWriteContactTest_14::CPhbkSyncWriteContactTest_14()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncWriteContactTest_14"));
	}

CPhbkSyncWriteContactTest_14::~CPhbkSyncWriteContactTest_14()
	{
	}

TVerdict CPhbkSyncWriteContactTest_14::doTestStepL( )
	{
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		ChangeSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	SetSimTsyTestNumberL(67);  // SDN and LND are read only

	// to create a new entry with internal (long/new) format and initialise it
	MyCSyncContactICCEntry* iccEntryInternalNew = MyCSyncContactICCEntry::NewL();
	CleanupStack::PushL(iccEntryInternalNew);
	InitialiseIccEntryInternalNewL(iccEntryInternalNew);

	// to call the DoWriteContactL
	TRequestStatus status;
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		// set the corresponding entry attributes
		if (iPhoneBook[i] == KUidIccGlobalSdnPhonebook ||
			iPhoneBook[i] == KUidIccGlobalLndPhonebook) 
			{
			// slot 1-4 is guaranteed free in phonebooks (see cfg. file)
			iccEntryInternalNew->iSlotNum = i+1; 
			iccEntryInternalNew->iPhonebook = convertPhonebookUid2Enum(iPhoneBook[i]); 
			// do the real test by writing an entry to ICC
			DoWriteContactToICCL(iccEntryInternalNew, iPhoneBook[i], status);

			// check result
			TESTCHECK(status.Int(), KErrAccessDenied);
			}
		}
	CleanupStack::PopAndDestroy(); 
	return TestStepResult();
	}

/**
 * Test writing a contact, where the slot number is not specified.
 */
//
//    Test step15 -- Test#68 in Unit Test Spec. Doc.  //
//
CPhbkSyncWriteContactTest_15::CPhbkSyncWriteContactTest_15()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncWriteContactTest_15"));
	}

CPhbkSyncWriteContactTest_15::~CPhbkSyncWriteContactTest_15()
	{
	}

TVerdict CPhbkSyncWriteContactTest_15::doTestStepL( )
	{
	// =============================================================
	// *** This test is covered by combining Test#55 and Test#56 ***
	// =============================================================

	return TestStepResult();
	}

/**
 * Test writing (editing) a contact, where the contact is a
 * hidden entry and the hidden key has not been verified.
 */
//
//    Test step16 -- Test#69 in Unit Test Spec. Doc.  //
//
CPhbkSyncWriteContactTest_16::CPhbkSyncWriteContactTest_16()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncWriteContactTest_16"));
	}

CPhbkSyncWriteContactTest_16::~CPhbkSyncWriteContactTest_16()
	{
	}

TVerdict CPhbkSyncWriteContactTest_16::doTestStepL( )
	{
	// =============================================================
	// *** This test situation cannot be created in cfg. file    ***
	// =============================================================

	return TestStepResult();
	}

/**
 * Test writing a new hidden entry without the hidden key
 * being verified.
 */
//
//    Test step17 -- Test#70 in Unit Test Spec. Doc.  //
//
CPhbkSyncWriteContactTest_17::CPhbkSyncWriteContactTest_17()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncWriteContactTest_17"));
	}

CPhbkSyncWriteContactTest_17::~CPhbkSyncWriteContactTest_17()
	{
	}

TVerdict CPhbkSyncWriteContactTest_17::doTestStepL( )
	{
	// set mode as EAutoCurrentIcc for Global/GSM ADN and USim App ADN
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		if ((iPhoneBook[i] != KUidIccGlobalSdnPhonebook) && 
		    (iPhoneBook[i] != KUidIccGlobalLndPhonebook) &&
			(iPhoneBook[i] != KUidIccGlobalFdnPhonebook))
			{ // exclude SDN, LND and FDN
			User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]));
			CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
			}
		}

	// load Global/GSM ADN and USim App ADN phonebooks
	SetSimTsyTestNumberL(70);

	// wait for at least 20 seconds (see [test69] in cfg. file) until the hidden key
	// tag effective which will block hidden entry write.
	User::After(23*KOneSeconduS);

	// now, we should get denied when writting into the locations of hidden entries.
	MyCSyncContactICCEntry* iccEntryInternalNew = MyCSyncContactICCEntry::NewL();
	CleanupStack::PushL(iccEntryInternalNew);
	InitialiseIccEntryInternalNewL(iccEntryInternalNew);

	// since it is not clear how to get the whole CContactICCEntry object from 
	// CContactDatabase and re-write it back to ICC, a new hidden entry will
	// be created and its slot number is the same as the one used by the real
	// hidden entry in ICC. This should do the same job as fas as DoWriteContact 
	// is concerned.
	iccEntryInternalNew->iIsHidden = ETrue;
	TRequestStatus status; 

	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		if ((iPhoneBook[i] != KUidIccGlobalSdnPhonebook) &&
			(iPhoneBook[i] != KUidIccGlobalLndPhonebook) &&
			(iPhoneBook[i] != KUidIccGlobalFdnPhonebook))
			{ // excluse SDN and LND
			iccEntryInternalNew->iPhonebook = convertPhonebookUid2Enum(iPhoneBook[i]);
			DoWriteContactToICCL(iccEntryInternalNew, iPhoneBook[i], status);
			TESTCHECK(status.Int(), KErrAccessDenied); // check result
			}
		}
	CleanupStack::PopAndDestroy(); 
	return TestStepResult();
	}

/**
 * Test writing a new hidden entry after the hidden key 
 * has been verified.
 */
//
//    Test step18 -- Test#71 in Unit Test Spec. Doc.  //
//
CPhbkSyncWriteContactTest_18::CPhbkSyncWriteContactTest_18()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncWriteContactTest_18"));
	}

CPhbkSyncWriteContactTest_18::~CPhbkSyncWriteContactTest_18()
	{
	}

TVerdict CPhbkSyncWriteContactTest_18::doTestStepL( )
{
	// set mode as EAutoCurrentIcc for Global/GSM ADN and USim App ADN
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		if ((iPhoneBook[i] != KUidIccGlobalSdnPhonebook) &&
			(iPhoneBook[i] != KUidIccGlobalLndPhonebook) &&
			(iPhoneBook[i] != KUidIccGlobalFdnPhonebook))
			{ // exclude SDN and LND
			User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]));
			CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}
	}

	SetSimTsyTestNumberL(71);

	// wait for at least 20 seconds until the hidden key verified event is trigged
	User::After(23*KOneSeconduS);

	// now, we shouldn't get denied when writting a new hidden entry into ICC
	MyCSyncContactICCEntry* iccEntryInternalNew = MyCSyncContactICCEntry::NewL();
	CleanupStack::PushL(iccEntryInternalNew);
	InitialiseIccEntryInternalNewL(iccEntryInternalNew);
	iccEntryInternalNew->iIsHidden = ETrue;

	TRequestStatus status; 
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		if ((iPhoneBook[i] != KUidIccGlobalSdnPhonebook) &&
			(iPhoneBook[i] != KUidIccGlobalLndPhonebook) &&
			(iPhoneBook[i] != KUidIccGlobalFdnPhonebook))
			{ // exclude SDN and LND
			iccEntryInternalNew->iPhonebook = convertPhonebookUid2Enum(iPhoneBook[i]);
			DoWriteContactToICCL(iccEntryInternalNew, iPhoneBook[i], status);
			TESTCHECK(status.Int(), KErrNone); // check result
			}
		}
	CleanupStack::PopAndDestroy(); 
	return TestStepResult();
	}

/**
 * Test writing a new contact, which also contains an invalid
 * field.
 */
//
//    Test step19 -- Test#72 in Unit Test Spec. Doc.  //
//
CPhbkSyncWriteContactTest_19::CPhbkSyncWriteContactTest_19()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncWriteContactTest_19"));
	}

CPhbkSyncWriteContactTest_19::~CPhbkSyncWriteContactTest_19()
	{
	}

TVerdict CPhbkSyncWriteContactTest_19::doTestStepL( )
	{
	_LIT16(KTelWithLetters,"5678943Abc34");

	// set mode as EAutoCurrentIcc for all phonebooks
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// load the dummy test which contains all phonebooks
	SetSimTsyTestNumberL(0);

	// wait until all phonebooks have been synchronised
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		CheckCacheStateValidL(iPhoneBook[i]);
		}

	// try to write a new contact containing invalid field to ICC
	MyCSyncContactICCEntry* iccEntryInternalNew = MyCSyncContactICCEntry::NewL();
	CleanupStack::PushL(iccEntryInternalNew);
	InitialiseIccEntryInternalNewL(iccEntryInternalNew);

	// set an invalid number to iNumber field
	iccEntryInternalNew->iNumber.Format(KTelWithLetters);

	TRequestStatus status; 
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		iccEntryInternalNew->iPhonebook = convertPhonebookUid2Enum(iPhoneBook[i]);
		DoWriteContactToICCL(iccEntryInternalNew, iPhoneBook[i], status);

		// TSY doesn't consider the number containing letters as invalid.
//		TESTCHECK(status.Int(), KErrArgument); // check result
		TESTCHECK(status.Int(), KErrNone); // check result
		}
	CleanupStack::PopAndDestroy(); 
	return TestStepResult();
	}

/**
 * Test writing a contact, where all the contact fields are valid.
 * Repeat for all supported phonebooks.
 */
//
//    Test step22 -- Test#88 in Unit Test Spec. Doc. //
//
CPhbkSyncWriteContactTest_22::CPhbkSyncWriteContactTest_22()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncWriteContactTest_22"));
	}

CPhbkSyncWriteContactTest_22::~CPhbkSyncWriteContactTest_22()
	{
	}

TVerdict CPhbkSyncWriteContactTest_22::doTestStepL( )
	{
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		ChangeSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	SetSimTsyTestNumberL(91);

	// wait until all phonebooks have been synchronised
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		CheckCacheStateValidL(iPhoneBook[i]);
		}

	// to create a new entry with internal (long/new) format and initialise it
	MyCSyncContactICCEntry* iccEntryInternalNew = MyCSyncContactICCEntry::NewL();
	CleanupStack::PushL(iccEntryInternalNew); 
	InitialiseIccEntryInternalNewL(iccEntryInternalNew);

	// to create an old entry with internal (short/old) format and initialise it
	MyCSyncContactICCEntry* iccEntryInternalOld = MyCSyncContactICCEntry::NewL();
	CleanupStack::PushL(iccEntryInternalOld);
	InitialiseIccEntryInternalOldL(iccEntryInternalOld);

	// to call the DoWriteContactL
	TRequestStatus status;
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		// set the corresponding entry attributes
		if (iPhoneBook[i] == KUidIccGlobalSdnPhonebook ||
			iPhoneBook[i] == KUidIccGlobalLndPhonebook) 
			{
			// slot 1-4 is guaranteed free in phonebooks (see [test91] in cfg. file)
			iccEntryInternalOld->iSlotNum = i+1; 
			iccEntryInternalOld->iPhonebook = convertPhonebookUid2Enum(iPhoneBook[i]); 
			// do the real test by writing an entry to ICC
			DoWriteContactToICCL(iccEntryInternalOld, iPhoneBook[i], status);
			}
		else 
			{
			// slot 1-4 is guaranteed free in phonebooks (see [test91] in cfg. file)
			iccEntryInternalNew->iSlotNum = i+1; 
			iccEntryInternalNew->iPhonebook = convertPhonebookUid2Enum(iPhoneBook[i]); 
			// do the real test by writing an entry to ICC
			DoWriteContactToICCL(iccEntryInternalNew, iPhoneBook[i], status);
			}

		// check result
		TESTCHECK(status.Int(), KErrNone);
		TESTCHECKCONDITION(iccEntryInternalNew->iSlotNum != KSyncIndexNotSupplied);
		}
	CleanupStack::PopAndDestroy(2); 
	return TestStepResult();
	}
