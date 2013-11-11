// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _TE_CNTSYNCSYNC_H_
#define _TE_CNTSYNCSYNC_H_

#include <phbksync.h>

class CPhbkSyncIntegrationTestBase : public CCntSyncTestStep
	{
public:
	virtual enum TVerdict doTestStepL() {return EFail; };

protected:	
	void CheckNameInDBL(const TDesC& aName);
	void CheckDefaultsL();
	void ReadEntryInDBL(const TDesC& aName);
	void DeleteEntryInDBL(const TDesC& aName);
	void AddEntryInDBL(const TDesC& aNumber, const TDesC& aName, const TDesC& aEmail);
	void EditEntryInDBL(const TDesC& aNumber, const TDesC& aEmail);
	void CheckSlotManipulationL(const TUid aPhonebook);
	void CheckSlotManipulationAndWriteL(const TUid aPhonebook);
	};

class CPhbkSyncAutomaticSameICCTest : public CPhbkSyncIntegrationTestBase
	{
public:
	static CPhbkSyncAutomaticSameICCTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncAutomaticSameICCTest();
	};

class CPhbkSyncAutomaticCurrentICCTest : public CPhbkSyncIntegrationTestBase
	{
public:
	static CPhbkSyncAutomaticCurrentICCTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncAutomaticCurrentICCTest();
	};

class CPhbkSyncManualTest : public CPhbkSyncIntegrationTestBase
	{
public:
	static CPhbkSyncManualTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncManualTest();
	};

class CPhbkSyncManualNotReadyTest : public CPhbkSyncIntegrationTestBase
	{
public:
	static CPhbkSyncManualNotReadyTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncManualNotReadyTest();
	};

class CPhbkSyncManualDiffSizeSIMTest : public CPhbkSyncIntegrationTestBase
	{
public:
	static CPhbkSyncManualDiffSizeSIMTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncManualDiffSizeSIMTest();
	};

class CPhbkSyncSATUpdatesTestBase : public CPhbkSyncIntegrationTestBase
	{
public:
	static CPhbkSyncSATUpdatesTestBase* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncSATUpdatesTestBase();
	};

class CPhbkSyncICCLockedTestBase : public CPhbkSyncIntegrationTestBase
	{
public:
	static CPhbkSyncICCLockedTestBase* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncICCLockedTestBase();
	};

class CPhbkSyncConsecutiveTestBase : public CPhbkSyncIntegrationTestBase
	{
public:
	static CPhbkSyncConsecutiveTestBase* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncConsecutiveTestBase();
	};

class CPhbkSyncFailureTestBase : public CPhbkSyncIntegrationTestBase
	{
public:
	static CPhbkSyncFailureTestBase* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncFailureTestBase();
	};

class CPhbkSyncFailureAndCheckErrorTestBase : public CPhbkSyncIntegrationTestBase
	{
public:
	static CPhbkSyncFailureAndCheckErrorTestBase* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncFailureAndCheckErrorTestBase();
	};

class CPhbkSyncNotificationTestBase : public CPhbkSyncIntegrationTestBase
	{
public:
	static CPhbkSyncNotificationTestBase* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncNotificationTestBase();
	};

class CPhbkSyncDeleteDBTestBase : public CPhbkSyncIntegrationTestBase
	{
public:
	static CPhbkSyncDeleteDBTestBase* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncDeleteDBTestBase();
	};

class CPhbkSyncEmptyICCTestBase : public CPhbkSyncIntegrationTestBase
	{
public:
	static CPhbkSyncEmptyICCTestBase* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncEmptyICCTestBase();
	};

class CPhbkSyncICCSlotsManipulationTestBase : public CPhbkSyncIntegrationTestBase
	{
public:
	static CPhbkSyncICCSlotsManipulationTestBase* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncICCSlotsManipulationTestBase();
	};

class CPhbkSyncICCSlotsManipulationAndWriteTestBase : public CPhbkSyncIntegrationTestBase
	{
public:
	static CPhbkSyncICCSlotsManipulationAndWriteTestBase* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncICCSlotsManipulationAndWriteTestBase();
	};

class CPhbkSyncMultipleRequestsTestBase : public CPhbkSyncIntegrationTestBase
	{
public:
	static CPhbkSyncMultipleRequestsTestBase* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncMultipleRequestsTestBase();
	};

class CPhbkSyncGeneralResponseTestBase : public CPhbkSyncIntegrationTestBase
	{
public:
	static CPhbkSyncGeneralResponseTestBase* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncGeneralResponseTestBase();
	void DoSyncAndTestServerResponceL(TUid aPhonebookUid, TBool aTestResponse,
									  const TDesC& aPhonebookName);
	};

class CPhbkSyncGetAndContactFormatTestBase : public CPhbkSyncIntegrationTestBase
	{
public:
	static CPhbkSyncGetAndContactFormatTestBase* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncGetAndContactFormatTestBase();
	};

class CPhbkSyncSetSyncModeAndAutoSyncTest : public CPhbkSyncIntegrationTestBase
	{
public:
	static CPhbkSyncSetSyncModeAndAutoSyncTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncSetSyncModeAndAutoSyncTest();
	};

class CPhbkSyncSyncUnsupportedPhonebook : public CPhbkSyncIntegrationTestBase
	{
public:
	static CPhbkSyncSyncUnsupportedPhonebook* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncSyncUnsupportedPhonebook();
	};


class CPhbkSyncSyncDiffNumberTypesRepeatedly : public CPhbkSyncIntegrationTestBase
	{
public:
	virtual enum TVerdict doTestStepL();
	};


class CPhbkSyncSyncInternationalNumbers : public CPhbkSyncIntegrationTestBase
	{
public:
	virtual enum TVerdict doTestStepL();
	};

class CPhbkSyncSyncAutomaticWithUnsupportedPhonebook : public CPhbkSyncIntegrationTestBase
	{
public:
	virtual enum TVerdict doTestStepL();
	};


#endif
