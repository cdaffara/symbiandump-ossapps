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

#ifndef _TE_CNTSYNCEDIT_H_
#define _TE_CNTSYNCEDIT_H_

#include <phbksync.h>
#include "te_cntsyncbase.h"

class CPhbkEditIntegrationTestBase : public CCntSyncTestStep
	{
public:
	virtual enum TVerdict doTestStepL() {return EFail; };

protected:	
	void OpenICCContactL();
	void OpenContactInvalidUIDL();
	void OpenContactICCLockedL();
	void OpenEntryReadOnlyAccessL();
	void OpenFullICCL();					// Stress tests
	void ChangeNameFieldL(CContactItem& aItem, const TDesC& aModifiedName); // Utilities
	void ChangeNumberFieldL(CContactItem& aItem, const TDesC& aModifiedNumber); // Utilities
	void ChangeEmailFieldL(CContactItem& aItem, const TDesC& aModifiedEmail); // Utilities
	void OpenContactEditFieldsL(TUid aPhonebookUid, TInt aIndex, TBool aCommit, const TDesC& aModifiedName = KNullDesC, const TDesC& aModifiedNumber = KNullDesC, const TDesC& aModifiedEmail = KNullDesC); // Utilities
	void OpenContactEditLockedL(TUid aPhonebookUid, TInt aIndex, const TDesC& aModifiedName); // Utilities
	void CheckEditSlotWriteL(const TUid aPhonebook);
	};

class CPhbkOpenICCContactTest : public CPhbkEditIntegrationTestBase
	{
public:
	static CPhbkOpenICCContactTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkOpenICCContactTest();
	};

class CPhbkOpenContactInvalidUIDTest : public CPhbkEditIntegrationTestBase
	{
public:
	static CPhbkOpenContactInvalidUIDTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkOpenContactInvalidUIDTest();
	};

class CPhbkEditICCContactTest : public CPhbkEditIntegrationTestBase
	{
public:
	static CPhbkEditICCContactTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkEditICCContactTest();
	};

class CPhbkEditICCContactTwiceTest : public CPhbkEditIntegrationTestBase
	{
public:
	static CPhbkEditICCContactTwiceTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkEditICCContactTwiceTest();
	};

class CPhbkEditContactEmptyNameTest : public CPhbkEditIntegrationTestBase
	{
public:
	static CPhbkEditContactEmptyNameTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkEditContactEmptyNameTest();
	};

class CPhbkEditContactEmptyNumberTest : public CPhbkEditIntegrationTestBase
	{
public:
	static CPhbkEditContactEmptyNumberTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkEditContactEmptyNumberTest();
	};

class CPhbkEditContactNameExceededTest : public CPhbkEditIntegrationTestBase
	{
public:
	static CPhbkEditContactNameExceededTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkEditContactNameExceededTest();
	};

class CPhbkEditContactNumberExceededTest : public CPhbkEditIntegrationTestBase
	{
public:
	static CPhbkEditContactNumberExceededTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkEditContactNumberExceededTest();
	};

class CPhbkEditContactICCLockedTest : public CPhbkEditIntegrationTestBase
	{
public:
	static CPhbkEditContactICCLockedTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkEditContactICCLockedTest();
	};

class CPhbkEditContactICCWriteFailsTest : public CPhbkEditIntegrationTestBase
	{
public:
	static CPhbkEditContactICCWriteFailsTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkEditContactICCWriteFailsTest();
	};

class CPhbkEditContactEmptyFieldsTest : public CPhbkEditIntegrationTestBase
	{
public:
	static CPhbkEditContactEmptyFieldsTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkEditContactEmptyFieldsTest();
	};

class CPhbkEditSlotWriteTest : public CPhbkEditIntegrationTestBase
	{
public:
	static CPhbkEditSlotWriteTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkEditSlotWriteTest();
	};

class CPhbkOpenContactICCLockedTest : public CPhbkEditIntegrationTestBase
	{
public:
	static CPhbkOpenContactICCLockedTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkOpenContactICCLockedTest();
	};

class CPhbkOpenEntryReadOnlyAccessTest : public CPhbkEditIntegrationTestBase
	{
public:
	static CPhbkOpenEntryReadOnlyAccessTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkOpenEntryReadOnlyAccessTest();
	};

class CPhbkOpenFullICCTest : public CPhbkEditIntegrationTestBase
	{
public:
	static CPhbkOpenFullICCTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkOpenFullICCTest();
	};

#endif
