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

#ifndef _TE_CNTSYNCDELETE_H_
#define _TE_CNTSYNCDELETE_

#include <phbksync.h>
#include "te_cntsyncbase.h"

class CPhbkDeleteIntegrationTestBase : public CCntSyncTestStep
	{
public:
	virtual enum TVerdict doTestStepL() {return EFail; };

protected:	
	void DeleteContactL(const TUid aPhonebook);	// Utilities
	void DeleteArrayWithInvalidIdL(const TDesC& aNumber);	// Utilities
	void DeleteArrayWithFailureL(const TDesC& aName, const TInt aExpectedError); // Utilities
	void DeleteArrayWithInvalidUIDL(const TDesC& aName);	// Utilities
	void DeleteArrayWithOneEntryL(const TDesC& aNumber, const TDesC& aName, const TUid aPhonebook = KUidIccGlobalAdnPhonebook, const TDesC& aEmail = KNullDesC); // Utilities
	void DeleteArrayWithMultEntryL(const TDesC& aNumber, const TDesC& aName, const TUid aPhonebook = KUidIccGlobalAdnPhonebook, const TDesC& aEmail = KNullDesC); // Utilities
	void DeleteWholePhonebookL(const TDesC& aName); // Utilities
	void DeleteArrayWithICCLockedL(const TDesC& aName, const TInt aExpectedError);
	void DeleteSlotTwiceL(const TDesC& aNumber, const TDesC& aName, const TUid aPhonebook = KUidIccGlobalAdnPhonebook, const TDesC& aEmail = KNullDesC);
	};

class CPhbkDeleteICCContactTest : public CPhbkDeleteIntegrationTestBase
	{
public:
	static CPhbkDeleteICCContactTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkDeleteICCContactTest();
	};

class CPhbkDeleteEntryInvalidUIDTest : public CPhbkDeleteIntegrationTestBase
	{
public:
	static CPhbkDeleteEntryInvalidUIDTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkDeleteEntryInvalidUIDTest();
	};

class CPhbkDeleteAllEntriesTest : public CPhbkDeleteIntegrationTestBase
	{
public:
	static CPhbkDeleteAllEntriesTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkDeleteAllEntriesTest();
	};

class CPhbkDeleteEntryICCLockedTest : public CPhbkDeleteIntegrationTestBase
	{
public:
	static CPhbkDeleteEntryICCLockedTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkDeleteEntryICCLockedTest();
	};

class CPhbkDeleteEntryICCDeleteFailsTest : public CPhbkDeleteIntegrationTestBase
	{
public:
	static CPhbkDeleteEntryICCDeleteFailsTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkDeleteEntryICCDeleteFailsTest();
	};

class CPhbkDeleteEntryReadOnlyAccessTest : public CPhbkDeleteIntegrationTestBase
	{
public:
	static CPhbkDeleteEntryReadOnlyAccessTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkDeleteEntryReadOnlyAccessTest();
	};

class CPhbkDeleteArrayOneEntryTest : public CPhbkDeleteIntegrationTestBase
	{
public:
	static CPhbkDeleteArrayOneEntryTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkDeleteArrayOneEntryTest();
	};

class CPhbkDeleteArrayInvalidIdTest : public CPhbkDeleteIntegrationTestBase
	{
public:
	static CPhbkDeleteArrayInvalidIdTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkDeleteArrayInvalidIdTest();
	};

class CPhbkDeleteArrayMultipleEntriesTest : public CPhbkDeleteIntegrationTestBase
	{
public:
	static CPhbkDeleteArrayMultipleEntriesTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkDeleteArrayMultipleEntriesTest();
	};

class CPhbkDeleteArrayICCLockedTest : public CPhbkDeleteIntegrationTestBase
	{
public:
	static CPhbkDeleteArrayICCLockedTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkDeleteArrayICCLockedTest();
	};

class CPhbkDeleteArrayReadOnlyAccessTest : public CPhbkDeleteIntegrationTestBase
	{
public:
	static CPhbkDeleteArrayReadOnlyAccessTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkDeleteArrayReadOnlyAccessTest();
	};

class CPhbkDeleteArrayICCDeleteFailsTest : public CPhbkDeleteIntegrationTestBase
	{
public:
	static CPhbkDeleteArrayICCDeleteFailsTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkDeleteArrayICCDeleteFailsTest();
	};

class CPhbkDeleteArrayInvalidUIDTest : public CPhbkDeleteIntegrationTestBase
	{
public:
	static CPhbkDeleteArrayInvalidUIDTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkDeleteArrayInvalidUIDTest();
	};

class CPhbkDeleteArrayFullPhonebookTest : public CPhbkDeleteIntegrationTestBase
	{
public:
	static CPhbkDeleteArrayFullPhonebookTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkDeleteArrayFullPhonebookTest();
	};

class CPhbkDeleteSlotEntryTwiceTest : public CPhbkDeleteIntegrationTestBase
	{
public:
	static CPhbkDeleteSlotEntryTwiceTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkDeleteSlotEntryTwiceTest();
	};

#endif
