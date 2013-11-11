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
#include "te_cntsyncfind.h"

_LIT(KFindName,"001");
_LIT(KFindNameInAdnOnly,"AAAA");
_LIT(KFindNameInSdnOnly,"BBBB");
_LIT(KFindNameInLndOnly,"CCCC");
_LIT(KFindNameInUsimOnly,"DDDD");
_LIT(KFindNameInFdnOnly,"EEEE");
_LIT(KFindSecondNameInUsimOnly,"Andy D");
_LIT(KFindNameAsync,"003");
_LIT(KFindNumber,"00004");
_LIT(KFindNumberInAdnOnly,"1000000");
_LIT(KFindNumberInSdnOnly,"2000000");
_LIT(KFindNumberInLndOnly,"3000000");
_LIT(KFindNumberInUsimOnly,"4000000");
_LIT(KFindNumberInFdnOnly,"5000000");
_LIT(KFindAddNumberInUsimOnly,"020812");
_LIT(KFindInternationalPrefix,"+");
_LIT(KFindInternationalPrefixInADN,"+44");
_LIT(KFindInternationalPrefixInSDN,"+55");
_LIT(KFindInternationalPrefixInLND,"+66");
_LIT(KFindInternationalPrefixInUsim,"+77");
_LIT(KFindInternationalPrefixInFDN,"+88");
_LIT(KFindNonExistingNumber,"101010101010");


/** 
 * Factory construction method.
 * @return Pointer to CPhbkFindByNameTest object
 */
CPhbkFindByNameTest* CPhbkFindByNameTest::NewL()
	{
	CPhbkFindByNameTest* self = new(ELeave) CPhbkFindByNameTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkFindByNameTest::CPhbkFindByNameTest()
	{
	SetTestStepName(_L("FindByNameTest"));
	}

/** Find an ICC entry by name - normal case */
enum TVerdict CPhbkFindByNameTest::doTestStepL()
	{
	SetSimTsyTestNumberL(1);
	DoSyncL();

	FindContactsL(KFindName, KUidContactFieldFamilyName, 5);
	FindContactsL(KFindNameInAdnOnly, KUidContactFieldFamilyName, 5, KUidIccGlobalAdnPhonebook);
	FindContactsL(KFindNameInSdnOnly, KUidContactFieldFamilyName, 5, KUidIccGlobalSdnPhonebook);
	FindContactsL(KFindNameInLndOnly, KUidContactFieldFamilyName, 5, KUidIccGlobalLndPhonebook);
	FindContactsL(KFindNameInUsimOnly, KUidContactFieldFamilyName, 5, KUidUsimAppAdnPhonebook);
	FindContactsL(KFindSecondNameInUsimOnly, KUidContactFieldSecondName, 5, KUidUsimAppAdnPhonebook);
	FindContactsL(KFindNameInFdnOnly, KUidContactFieldFamilyName, 5, KUidIccGlobalFdnPhonebook);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkFindByNumberTest object
 */
CPhbkFindByNumberTest* CPhbkFindByNumberTest::NewL()
	{
	CPhbkFindByNumberTest* self = new(ELeave) CPhbkFindByNumberTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkFindByNumberTest::CPhbkFindByNumberTest()
	{
	SetTestStepName(_L("FindByNumberTest"));
	}

/** Find an ICC entry by number - normal case */
enum TVerdict CPhbkFindByNumberTest::doTestStepL()
	{
	SetSimTsyTestNumberL(1);
	DoSyncL();

	FindContactsL(KFindNumber, KUidContactFieldPhoneNumber, 5);
	FindContactsL(KFindNumberInAdnOnly, KUidContactFieldPhoneNumber, 1, KUidIccGlobalAdnPhonebook);
	FindContactsL(KFindNumberInSdnOnly, KUidContactFieldPhoneNumber, 1, KUidIccGlobalSdnPhonebook);
	FindContactsL(KFindNumberInLndOnly, KUidContactFieldPhoneNumber, 1, KUidIccGlobalLndPhonebook);
	FindContactsL(KFindNumberInUsimOnly, KUidContactFieldPhoneNumber, 1, KUidUsimAppAdnPhonebook);
	FindContactsL(KFindAddNumberInUsimOnly, KUidContactFieldPhoneNumber, 5, KUidUsimAppAdnPhonebook);
	FindContactsL(KFindNumberInFdnOnly, KUidContactFieldPhoneNumber, 1, KUidIccGlobalFdnPhonebook);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkFindByNonExistingNameTest object
 */
CPhbkFindByNonExistingNameTest* CPhbkFindByNonExistingNameTest::NewL()
	{
	CPhbkFindByNonExistingNameTest* self = new(ELeave) CPhbkFindByNonExistingNameTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkFindByNonExistingNameTest::CPhbkFindByNonExistingNameTest()
	{
	SetTestStepName(_L("FindByNonExistingNameTest"));
	}

/** Find an ICC entry by name - name does not exist */
enum TVerdict CPhbkFindByNonExistingNameTest::doTestStepL()
	{
	CContactItemFieldDef* fieldDef=new(ELeave) CContactItemFieldDef;
	CleanupStack::PushL(fieldDef);
	fieldDef->AppendL(KUidContactFieldFamilyName); // define subset of fields to search

	CContactIdArray* array= iDb->FindLC(KFindNonExistingNumber,fieldDef);
	TESTCHECKL(array->Count(), 0); // Nothing found 
	CleanupStack::PopAndDestroy(2); // array and fieldDef

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkFindByNonExistingNumberTest object
 */
CPhbkFindByNonExistingNumberTest* CPhbkFindByNonExistingNumberTest::NewL()
	{
	CPhbkFindByNonExistingNumberTest* self = new(ELeave) CPhbkFindByNonExistingNumberTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkFindByNonExistingNumberTest::CPhbkFindByNonExistingNumberTest()
	{
	SetTestStepName(_L("FindByNonExistingNumberTest"));
	}

/** Find an ICC entry by number - number does not exist */
enum TVerdict CPhbkFindByNonExistingNumberTest::doTestStepL()
	{
	CContactItemFieldDef* fieldDef=new(ELeave) CContactItemFieldDef;
	CleanupStack::PushL(fieldDef);
	fieldDef->AppendL(KUidContactFieldPhoneNumber); // define subset of fields to search

	CContactIdArray* array= iDb->FindLC(KFindNonExistingNumber,fieldDef);
	TESTCHECKL(array->Count(), 0); // Nothing found 
	CleanupStack::PopAndDestroy(2); // array and fieldDef

	return TestStepResult();
	}


CAsyncFinder::CAsyncFinder()
	{
	}

CAsyncFinder::~CAsyncFinder()
	{
	delete iFinder;
	}

/** Asynchronous find observer constructor */
void CAsyncFinder::ConstructL(CContactDatabase *aContactDatabase, const TDesC& aText,const CContactItemFieldDef *aFieldDef)
	{
	iFinder=aContactDatabase->FindAsyncL(aText, aFieldDef, this);
	}

/** Asynchronous find observer callback */
void CAsyncFinder::IdleFindCallback()
	{
	if (iFinder->IsComplete())
		CActiveScheduler::Stop();
	}

/** 
 * Factory construction method.
 * @return Pointer to CPhbkFindByNumberAsyncSearchTest object
 */
CPhbkFindByNumberAsyncSearchTest* CPhbkFindByNumberAsyncSearchTest::NewL()
	{
	CPhbkFindByNumberAsyncSearchTest* self = new(ELeave) CPhbkFindByNumberAsyncSearchTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkFindByNumberAsyncSearchTest::CPhbkFindByNumberAsyncSearchTest()
	{
	SetTestStepName(_L("FindByNumberAsyncSearchTest"));
	}

/** Find an ICC entry by number - asynchronous search */
enum TVerdict CPhbkFindByNumberAsyncSearchTest::doTestStepL()
	{
	SetSimTsyTestNumberL(1);
	DoSyncL();

	FindContactAsyncSearchL(KFindNumber, KUidContactFieldPhoneNumber, 5);
	FindContactAsyncSearchL(KFindNumberInAdnOnly, KUidContactFieldPhoneNumber, 1);
	FindContactAsyncSearchL(KFindNumberInSdnOnly, KUidContactFieldPhoneNumber, 1);
	FindContactAsyncSearchL(KFindNumberInLndOnly, KUidContactFieldPhoneNumber, 1);
	FindContactAsyncSearchL(KFindNumberInUsimOnly, KUidContactFieldPhoneNumber, 1);
	FindContactAsyncSearchL(KFindAddNumberInUsimOnly, KUidContactFieldPhoneNumber, 5);
	FindContactAsyncSearchL(KFindNumberInFdnOnly, KUidContactFieldPhoneNumber, 1);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkFindByNameAsyncSearchTest object
 */
CPhbkFindByNameAsyncSearchTest* CPhbkFindByNameAsyncSearchTest::NewL()
	{
	CPhbkFindByNameAsyncSearchTest* self = new(ELeave) CPhbkFindByNameAsyncSearchTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkFindByNameAsyncSearchTest::CPhbkFindByNameAsyncSearchTest()
	{
	SetTestStepName(_L("FindByNameAsyncSearchTest"));
	}

/** Find an ICC entry by name - asynchronous search */
enum TVerdict CPhbkFindByNameAsyncSearchTest::doTestStepL()
	{
	SetSimTsyTestNumberL(1);
	DoSyncL();

	FindContactAsyncSearchL(KFindNameAsync, KUidContactFieldFamilyName, 5);
	FindContactAsyncSearchL(KFindNameInAdnOnly, KUidContactFieldFamilyName, 5);
	FindContactAsyncSearchL(KFindNameInSdnOnly, KUidContactFieldFamilyName, 5);
	FindContactAsyncSearchL(KFindNameInLndOnly, KUidContactFieldFamilyName, 5);
	FindContactAsyncSearchL(KFindNameInUsimOnly, KUidContactFieldFamilyName, 5);
	FindContactAsyncSearchL(KFindSecondNameInUsimOnly, KUidContactFieldSecondName, 5);
	FindContactAsyncSearchL(KFindNameInFdnOnly, KUidContactFieldFamilyName, 5);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkFindByNonExistNumAsyncSearchTest object
 */
CPhbkFindByNonExistNumAsyncSearchTest* CPhbkFindByNonExistNumAsyncSearchTest::NewL()
	{
	CPhbkFindByNonExistNumAsyncSearchTest* self = new(ELeave) CPhbkFindByNonExistNumAsyncSearchTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkFindByNonExistNumAsyncSearchTest::CPhbkFindByNonExistNumAsyncSearchTest()
	{
	SetTestStepName(_L("FindByNonExistNumAsyncSearchTest"));
	}

/** Find an ICC entry by number - asynchronous search for number that does not exist */
enum TVerdict CPhbkFindByNonExistNumAsyncSearchTest::doTestStepL()
	{
	CContactItemFieldDef* fieldDef=new(ELeave) CContactItemFieldDef;
	CleanupStack::PushL(fieldDef);
	fieldDef->AppendL(KUidContactFieldPhoneNumber); // define subset of fields to search
	CAsyncFinder* asyncFinder = new (ELeave) CAsyncFinder();
	CleanupStack::PushL(asyncFinder);
	asyncFinder->ConstructL(iDb, KFindNonExistingNumber, fieldDef); // create async finder and do async find
	
	CActiveScheduler::Start();
	CContactIdArray* array=asyncFinder->TakeContactIds(); // get array of UIDs
	TESTCHECKL(array->Count(), 0); // Nothing found 
	delete array;
	CleanupStack::PopAndDestroy(2); // fieldDef, asyncFinder

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkFindByNameICCLockedTest object
 */
CPhbkFindByNameICCLockedTest* CPhbkFindByNameICCLockedTest::NewL()
	{
	CPhbkFindByNameICCLockedTest* self = new(ELeave) CPhbkFindByNameICCLockedTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkFindByNameICCLockedTest::CPhbkFindByNameICCLockedTest()
	{
	SetTestStepName(_L("FindByNameICCLockedTest"));
	}

/** Find an ICC entry by name - ICC locked */
enum TVerdict CPhbkFindByNameICCLockedTest::doTestStepL()
	{
	SetSimTsyTestNumberL(2);
	DoSyncL();
	SignalSimTsyToReduceTimersL();

	TInt err(KErrNone);
	TInt err1(KErrNone);
	TInt err2(KErrNone);
	TInt err3(KErrNone);
	TInt err4(KErrNone);
	TInt err5(KErrNone);
	TInt err6(KErrNone);
	do // Wait for ICC to become locked
		{
		User::After(1000000);
		TRAP(err, FindContactsL(KFindName, KUidContactFieldFamilyName, 5));
		} while(err==KErrNone);

	TRAP(err1, FindContactsL(KFindNameInAdnOnly, KUidContactFieldFamilyName, 1, KUidIccGlobalAdnPhonebook));
	TRAP(err2, FindContactsL(KFindNameInSdnOnly, KUidContactFieldFamilyName, 1, KUidIccGlobalSdnPhonebook));
	TRAP(err3, FindContactsL(KFindNameInLndOnly, KUidContactFieldFamilyName, 1, KUidIccGlobalLndPhonebook));
	TRAP(err4, FindContactsL(KFindNameInUsimOnly, KUidContactFieldFamilyName, 1, KUidUsimAppAdnPhonebook));
	TRAP(err5, FindContactsL(KFindSecondNameInUsimOnly, KUidContactFieldSecondName, 5, KUidUsimAppAdnPhonebook));
	TRAP(err6, FindContactsL(KFindNameInFdnOnly, KUidContactFieldFamilyName, 1, KUidIccGlobalFdnPhonebook));
	TESTCHECKL(err, KErrAccessDenied);
	TESTCHECKL(err1, KErrAccessDenied);
	TESTCHECKL(err2, KErrAccessDenied);
	TESTCHECKL(err3, KErrAccessDenied);
	TESTCHECKL(err4, KErrAccessDenied);
	TESTCHECKL(err5, KErrAccessDenied);
	TESTCHECKL(err6, KErrAccessDenied);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkFindByNumberICCLockedTest object
 */
CPhbkFindByNumberICCLockedTest* CPhbkFindByNumberICCLockedTest::NewL()
	{
	CPhbkFindByNumberICCLockedTest* self = new(ELeave) CPhbkFindByNumberICCLockedTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkFindByNumberICCLockedTest::CPhbkFindByNumberICCLockedTest()
	{
	SetTestStepName(_L("FindByNumberICCLockedTest"));
	}

/** Find an ICC entry by number - ICC locked */
enum TVerdict CPhbkFindByNumberICCLockedTest::doTestStepL()
	{
	SetSimTsyTestNumberL(2);
	DoSyncL();
	SignalSimTsyToReduceTimersL();

	TInt err(KErrNone);
	TInt err1(KErrNone);
	TInt err2(KErrNone);
	TInt err3(KErrNone);
	TInt err4(KErrNone);
	TInt err5(KErrNone);
	TInt err6(KErrNone);
	do // Wait for ICC to become locked
		{
		User::After(1000000);
		TRAP(err, FindContactsL(KFindNumber, KUidContactFieldPhoneNumber, 5));
		} while(err==KErrNone);

	TRAP(err1, FindContactsL(KFindNumberInAdnOnly, KUidContactFieldPhoneNumber, 1, KUidIccGlobalAdnPhonebook));
	TRAP(err2, FindContactsL(KFindNumberInSdnOnly, KUidContactFieldPhoneNumber, 1, KUidIccGlobalSdnPhonebook));
	TRAP(err3, FindContactsL(KFindNumberInLndOnly, KUidContactFieldPhoneNumber, 1, KUidIccGlobalLndPhonebook));
	TRAP(err4, FindContactsL(KFindNumberInUsimOnly, KUidContactFieldPhoneNumber, 1, KUidUsimAppAdnPhonebook));
	TRAP(err5, FindContactsL(KFindAddNumberInUsimOnly, KUidContactFieldPhoneNumber, 1, KUidUsimAppAdnPhonebook));
	TRAP(err6, FindContactsL(KFindNumberInFdnOnly, KUidContactFieldPhoneNumber, 1, KUidIccGlobalFdnPhonebook));
	TESTCHECKL(err, KErrAccessDenied);
	TESTCHECKL(err1, KErrAccessDenied);
	TESTCHECKL(err2, KErrAccessDenied);
	TESTCHECKL(err3, KErrAccessDenied);
	TESTCHECKL(err4, KErrAccessDenied);
	TESTCHECKL(err5, KErrAccessDenied);
	TESTCHECKL(err6, KErrAccessDenied);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkFindByNumICCLockedAsyncSearchTest object
 */
CPhbkFindByNumICCLockedAsyncSearchTest* CPhbkFindByNumICCLockedAsyncSearchTest::NewL()
	{
	CPhbkFindByNumICCLockedAsyncSearchTest* self = new(ELeave) CPhbkFindByNumICCLockedAsyncSearchTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkFindByNumICCLockedAsyncSearchTest::CPhbkFindByNumICCLockedAsyncSearchTest()
	{
	SetTestStepName(_L("FindByNumICCLockedAsyncSearchTest"));
	}

/** Find an ICC entry by number - asynchronous search when ICC is locked */
enum TVerdict CPhbkFindByNumICCLockedAsyncSearchTest::doTestStepL()
	{
	SetSimTsyTestNumberL(2);
	DoSyncL();

	SignalSimTsyToReduceTimersL();
	TInt err(KErrNone);
	do // Wait for ICC to become locked
		{
		User::After(1000000);
		TRAP(err, FindContactsL(KFindNumber, KUidContactFieldPhoneNumber, 5));
		} while(err==KErrNone);

	TESTCHECKL(err, KErrAccessDenied);

	CContactItemFieldDef* fieldDef = new(ELeave) CContactItemFieldDef;
	CleanupStack::PushL(fieldDef);
	fieldDef->AppendL(KUidContactFieldPhoneNumber); // define subset of fields to search

	CAsyncFinder* asyncFinder = new (ELeave) CAsyncFinder();
	CleanupStack::PushL(asyncFinder);
	err = KErrNone;
	TRAP(err, asyncFinder->ConstructL(iDb, KFindNumber, fieldDef)); // create async finder and do async find
	TESTCHECKL(err, KErrNone);

	CActiveScheduler::Start();
	CContactIdArray* array = asyncFinder->TakeContactIds(); // get array of UIDs
	TESTCHECKL(array->Count(), 0); // Nothing found
	delete array;
	CleanupStack::PopAndDestroy(2); // fieldDef, asyncFinder

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkFindByInternationalPrefixTest object
 */
CPhbkFindByInternationalPrefixTest* CPhbkFindByInternationalPrefixTest::NewL()
	{
	CPhbkFindByInternationalPrefixTest* self = new(ELeave) CPhbkFindByInternationalPrefixTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkFindByInternationalPrefixTest::CPhbkFindByInternationalPrefixTest()
	{
	SetTestStepName(_L("FindByInternationalPrefixTest"));
	}

/** Find an ICC entry by number - international format */
enum TVerdict CPhbkFindByInternationalPrefixTest::doTestStepL()
	{
	SetSimTsyTestNumberL(12);
	DoSyncL();

	FindContactsL(KFindInternationalPrefix, KUidContactFieldPhoneNumber, 5);
	FindContactsL(KFindInternationalPrefixInADN, KUidContactFieldPhoneNumber, 1, KUidIccGlobalAdnPhonebook);
	FindContactsL(KFindInternationalPrefixInSDN, KUidContactFieldPhoneNumber, 1, KUidIccGlobalSdnPhonebook);
	FindContactsL(KFindInternationalPrefixInLND, KUidContactFieldPhoneNumber, 1, KUidIccGlobalLndPhonebook);
	FindContactsL(KFindInternationalPrefixInUsim, KUidContactFieldPhoneNumber, 1, KUidUsimAppAdnPhonebook);
	FindContactsL(KFindInternationalPrefixInFDN, KUidContactFieldPhoneNumber, 1, KUidIccGlobalFdnPhonebook);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkFindByInterPrefixAsyncSearchTest object
 */
CPhbkFindByInterPrefixAsyncSearchTest* CPhbkFindByInterPrefixAsyncSearchTest::NewL()
	{
	CPhbkFindByInterPrefixAsyncSearchTest* self = new(ELeave) CPhbkFindByInterPrefixAsyncSearchTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkFindByInterPrefixAsyncSearchTest::CPhbkFindByInterPrefixAsyncSearchTest()
	{
	SetTestStepName(_L("FindByInterPrefixAsyncSearchTest"));
	}

/** Find an ICC entry by number - asynchronous search of international format */
enum TVerdict CPhbkFindByInterPrefixAsyncSearchTest::doTestStepL()
	{
	SetSimTsyTestNumberL(12);
	DoSyncL();

	FindContactAsyncSearchL(KFindInternationalPrefix, KUidContactFieldPhoneNumber, 5);
	FindContactAsyncSearchL(KFindInternationalPrefixInADN, KUidContactFieldPhoneNumber, 1);
	FindContactAsyncSearchL(KFindInternationalPrefixInSDN, KUidContactFieldPhoneNumber, 1);
	FindContactAsyncSearchL(KFindInternationalPrefixInLND, KUidContactFieldPhoneNumber, 1);
	FindContactAsyncSearchL(KFindInternationalPrefixInUsim, KUidContactFieldPhoneNumber, 1);
	FindContactAsyncSearchL(KFindInternationalPrefixInFDN, KUidContactFieldPhoneNumber, 1);

	return TestStepResult();
	}


/** 
 * Find contact IDs that match aFindString in the field specified by aFieldToSearch.
 * @param aFindString Text to search for
 * @param aFieldToSearch Field to search
 * @param aExpectedMatches Expected number of matching contacts
 * @param aExpectedPhonebook Phonebook that the entries are expected to be in
 */
void CPhbkFindIntegrationTestBase::FindContactsL(const TDesC& aFindString, TFieldType aFieldToSearch, TInt aExpectedMatches, const TUid aExpectedPhonebook)
	{
	iDb->SetDbViewContactType(KUidContactICCEntry);
	CContactItemFieldDef* fieldDef=new(ELeave) CContactItemFieldDef;
	CleanupStack::PushL(fieldDef);
	fieldDef->AppendL(aFieldToSearch);
	CContactIdArray* array = iDb->FindLC(aFindString,fieldDef);

	TInt count = array->Count();
	TESTCHECKL(count, aExpectedMatches);

	if (aExpectedPhonebook != KUidIccPhonebookNotSpecified)
		{
		// check that all matched entries belong to the phonebook aPhonebook by checking the
		// value of the phonebook field
		for (TInt i=0; i<count; ++i)
			{
			CContactICCEntry* entry = static_cast<CContactICCEntry*>(iDb->ReadContactLC((*array)[i]));
			const CContactItemFieldSet& fieldset = entry->CardFields();
			const TInt n = fieldset.Find(KUidContactFieldICCPhonebook);
			if(n!=KErrNotFound)
				{
				TPtrC phonebookField = fieldset[n].TextStorage()->Text();
				TLex lex(phonebookField);
				TUid phonebookUid;
				/*TInt ignore  = */ lex.Val(phonebookUid.iUid);
				TESTCHECKL(phonebookUid, aExpectedPhonebook);
				}
			CleanupStack::PopAndDestroy(entry);
			}
		}

	CleanupStack::PopAndDestroy(2); // array & fieldDef
	}


/** Asynchronous search for field content */
void CPhbkFindIntegrationTestBase::FindContactAsyncSearchL(const TDesC& aContent, TFieldType aFieldToSearch, TInt aExpectedMatches)
	{
	CContactItemFieldDef* fieldDef=new(ELeave) CContactItemFieldDef;
	CleanupStack::PushL(fieldDef);
	fieldDef->AppendL(aFieldToSearch); // define subset of fields to search
	
	CAsyncFinder* asyncFinder = new (ELeave) CAsyncFinder();
	CleanupStack::PushL(asyncFinder);
	asyncFinder->ConstructL(iDb, aContent, fieldDef); // create async finder and do async find
	
	CActiveScheduler::Start();
	CContactIdArray* array=asyncFinder->TakeContactIds(); // get array of UIDs
	CleanupStack::PushL(array);
	TESTCHECK(array->Count(), aExpectedMatches); // compare number of UIDs
	CleanupStack::PopAndDestroy(array);
	CleanupStack::PopAndDestroy(asyncFinder);
	CleanupStack::PopAndDestroy(fieldDef);
	}

