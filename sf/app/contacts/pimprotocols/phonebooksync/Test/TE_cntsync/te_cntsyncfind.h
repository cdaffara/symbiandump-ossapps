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

/**
 @file
 @internalComponent
*/


#ifndef _TE_CNTSYNCFIND_H_
#define _TE_CNTSYNCFIND_H_

#include <phbksync.h>
#include "te_cntsyncbase.h"

class CPhbkFindIntegrationTestBase : public CCntSyncTestStep
	{
public:
	virtual enum TVerdict doTestStepL() {return EFail; };

protected:	
	void FindContactsL(const TDesC& aFindString, TFieldType aFieldToSearch, TInt aExpectedMatches, const TUid aExpectedPhonebook = KUidIccPhonebookNotSpecified); // Utilities
	void FindContactAsyncSearchL(const TDesC& aContent, TFieldType aFieldToSearch, TInt aExpectedMatches); // Utilities

	};

class CPhbkFindByNameTest : public CPhbkFindIntegrationTestBase
	{
public:
	static CPhbkFindByNameTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkFindByNameTest();
	};

class CPhbkFindByNumberTest : public CPhbkFindIntegrationTestBase
	{
public:
	static CPhbkFindByNumberTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkFindByNumberTest();
	};

class CPhbkFindByNonExistingNameTest : public CPhbkFindIntegrationTestBase
	{
public:
	static CPhbkFindByNonExistingNameTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkFindByNonExistingNameTest();
	};

class CPhbkFindByNonExistingNumberTest : public CPhbkFindIntegrationTestBase
	{
public:
	static CPhbkFindByNonExistingNumberTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkFindByNonExistingNumberTest();
	};

class CPhbkFindByNumberAsyncSearchTest : public CPhbkFindIntegrationTestBase
	{
public:
	static CPhbkFindByNumberAsyncSearchTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkFindByNumberAsyncSearchTest();
	};

class CPhbkFindByNameAsyncSearchTest : public CPhbkFindIntegrationTestBase
	{
public:
	static CPhbkFindByNameAsyncSearchTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkFindByNameAsyncSearchTest();
	};

class CPhbkFindByNonExistNumAsyncSearchTest : public CPhbkFindIntegrationTestBase
	{
public:
	static CPhbkFindByNonExistNumAsyncSearchTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkFindByNonExistNumAsyncSearchTest();
	};

class CPhbkFindByNameICCLockedTest : public CPhbkFindIntegrationTestBase
	{
public:
	static CPhbkFindByNameICCLockedTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkFindByNameICCLockedTest();
	};

class CPhbkFindByNumberICCLockedTest : public CPhbkFindIntegrationTestBase
	{
public:
	static CPhbkFindByNumberICCLockedTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkFindByNumberICCLockedTest();
	};

class CPhbkFindByNumICCLockedAsyncSearchTest : public CPhbkFindIntegrationTestBase
	{
public:
	static CPhbkFindByNumICCLockedAsyncSearchTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkFindByNumICCLockedAsyncSearchTest();
	};

class CPhbkFindByInternationalPrefixTest : public CPhbkFindIntegrationTestBase
	{
public:
	static CPhbkFindByInternationalPrefixTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkFindByInternationalPrefixTest();
	};

class CPhbkFindByInterPrefixAsyncSearchTest : public CPhbkFindIntegrationTestBase
	{
public:
	static CPhbkFindByInterPrefixAsyncSearchTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkFindByInterPrefixAsyncSearchTest();
	};


/**
 * Asynchronous find observer class.
 */
class CAsyncFinder : public CBase, public MIdleFindObserver
	{
public:
	CAsyncFinder();
	void ConstructL(CContactDatabase *aContactDatabase, const TDesC& aText,const CContactItemFieldDef *aFieldDef);
	~CAsyncFinder();
	inline CContactIdArray *TakeContactIds() {return(iFinder->TakeContactIds());};
	inline TInt Error() const {return(iFinder->Error());};
protected:
// from MIdleFindObserver
	void IdleFindCallback();
	CIdleFinder *iFinder;
	};

#endif
