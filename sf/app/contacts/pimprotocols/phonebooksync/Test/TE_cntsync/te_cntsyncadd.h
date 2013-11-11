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

/**
 @file
 @internalComponent
*/

#ifndef _TE_CNTSYNCADD_H_
#define _TE_CNTSYNCADD_H_

#include <phbksync.h>

#include "te_cntsyncbase.h"

class CPhbkAddIntegrationTestBase : public CCntSyncTestStep
	{
public:
	virtual enum TVerdict doTestStepL() {return EFail; };

protected:	
	void AddContactL(const TDesC& aName, const TDesC& aNumber, const TUid aPhonebook = KUidIccGlobalAdnPhonebook, const TDesC& aEMail = KNullDesC); // Utilities
	void DoAddEntryInvalidFieldL(const TUid aPhonebook);
	void AddAdditionalNumEntryL(const TDesC& aFieldNum, const TUid aFieldType); // Utilities
	void AddTextFieldL(TFieldType aFieldType, TUid aMapping, const TDesC& aField, CContactICCEntry& aIccEntry); // Utilities
	void CheckEmptyEmailFieldContentL(CContactItemFieldSet& aFieldset, TFieldType aFieldType, const TDesC& aExpectedContent);
	void CreateContactLC(CContactICCEntry*& aEntry, const TUid aPhonebook, const TDesC& aName, const TDesC& aNumber, const TDesC& aEmail);
	};

class CPhbkAddICCContactTest : public CPhbkAddIntegrationTestBase
	{
public:
	static CPhbkAddICCContactTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkAddICCContactTest();
	};

class CPhbkAddEntryEmptyNameTest : public CPhbkAddIntegrationTestBase
	{
public:
	static CPhbkAddEntryEmptyNameTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkAddEntryEmptyNameTest();
	};

class CPhbkAddEntryEmptyNumberTest : public CPhbkAddIntegrationTestBase
	{
public:
	static CPhbkAddEntryEmptyNumberTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkAddEntryEmptyNumberTest();
	};

class CPhbkAddEntryEmptyEmailTest : public CPhbkAddIntegrationTestBase
	{
public:
	static CPhbkAddEntryEmptyEmailTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkAddEntryEmptyEmailTest();
	};


class CPhbkAddPhonebookFullTest : public CPhbkAddIntegrationTestBase
	{
public:
	static CPhbkAddPhonebookFullTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkAddPhonebookFullTest();
	};

class CPhbkAddBoundaryConditionsTest : public CPhbkAddIntegrationTestBase
	{
public:
	static CPhbkAddBoundaryConditionsTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkAddBoundaryConditionsTest();
	};

class CPhbkAddNameBoundaryExceededTest : public CPhbkAddIntegrationTestBase
	{
public:
	static CPhbkAddNameBoundaryExceededTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkAddNameBoundaryExceededTest();
	};

class CPhbkAddNumberBoundaryExceededTest : public CPhbkAddIntegrationTestBase
	{
public:
	static CPhbkAddNumberBoundaryExceededTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkAddNumberBoundaryExceededTest();
	};

class CPhbkAddSequenceOfEntriesTest : public CPhbkAddIntegrationTestBase
	{
public:
	static CPhbkAddSequenceOfEntriesTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkAddSequenceOfEntriesTest();
	};

class CPhbkAddEntryICCLockedTest : public CPhbkAddIntegrationTestBase
	{
public:
	static CPhbkAddEntryICCLockedTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkAddEntryICCLockedTest();
	};

class CPhbkAddEntryICCWriteFailsTest : public CPhbkAddIntegrationTestBase
	{
public:
	static CPhbkAddEntryICCWriteFailsTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkAddEntryICCWriteFailsTest();
	};

class CPhbkAddEntryInvalidFieldTest : public CPhbkAddIntegrationTestBase
	{
public:
	static CPhbkAddEntryInvalidFieldTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkAddEntryInvalidFieldTest();
	};

class CPhbkAddEntryNumberWithDTMFTest : public CPhbkAddIntegrationTestBase
	{
public:
	static CPhbkAddEntryNumberWithDTMFTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkAddEntryNumberWithDTMFTest();
	};

class CPhbkAddEntryReadOnlyAccessTest : public CPhbkAddIntegrationTestBase
	{
public:
	static CPhbkAddEntryReadOnlyAccessTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkAddEntryReadOnlyAccessTest();
	};

class CPhbkAddAdditionalNumWithTagTest : public CPhbkAddIntegrationTestBase
	{
public:
	static CPhbkAddAdditionalNumWithTagTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkAddAdditionalNumWithTagTest();
	};

class CPhbkAddEntryWithNoTemplateTest : public CPhbkAddIntegrationTestBase
	{
public:
	static CPhbkAddEntryWithNoTemplateTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkAddEntryWithNoTemplateTest();
	};

#endif
