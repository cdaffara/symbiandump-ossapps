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

#ifndef _TE_CNTSYNCREAD_H_
#define _TE_CNTSYNCREAD_H_

#include <phbksync.h>
#include "te_cntsyncbase.h"

class CPhbkReadIntegrationTestBase : public CCntSyncTestStep
	{
public:
	virtual enum TVerdict doTestStepL() {return EFail; };

protected:
	void CheckGroupMembershipL(CContactICCEntry& aItem, TInt aExpectedMembers); // Utilities
	void CheckFieldCountL(const TUid aPhonebook, CContactICCEntry* aEntry);
	void CheckMinimalFieldCountL(const TUid aPhonebook, CContactICCEntry* aEntry);
	void CheckFieldsL(const TUid aPhonebook, CContactICCEntry* aItem, TInt aSlotNumber);
	void CheckFieldContentL(CContactItemFieldSet& aFieldset, TFieldType aFieldType, const TDesC& aExpectedContent);
	void CheckFieldContentL(CContactItemFieldSet& aFieldset, TFieldType aFieldType, TInt aSearchFromPos, const TDesC& aExpectedContent);
	void ReadContactL(const TUid aPhonebook, TInt aExpectedCount, TBool aReadAll);
	void ReadContactViewsL(const TUid aPhonebook, TInt aExpectedCount);
	void ReadAllContactContentL(const TUid aPhonebook, TInt aExpectedCount);
	void ReadFullContactLockedL(const TUid aPhonebook, TInt aExpectedCount);
	void ReadLockedContactTextDefL(const TUid aPhonebook, TInt aExpectedCount);
	void ReadMinimalContactL(const TUid aPhonebook, TInt aExpectedCount);
	void ReadAllMinimalContactL(const TUid aPhonebook, TInt aExpectedCount);
	void ReadMinimalContactLockedL(const TUid aPhonebook, TInt aExpectedCount);
	void ReadFullContactTextDefL(const TUid aPhonebook, TInt aExpectedCount, TBool aReadAll);
	void ReadNumberContactTextDefL(const TUid aPhonebook, TInt aExpectedCount, TBool aReadAll);
	void ReadNameContactTextDefL(const TUid aPhonebook, TInt aExpectedCount, TBool aReadAll);
	void ReadInvalidContactTextDefL(const TUid aPhonebook, TInt aExpectedCount);
	void ReadAllContactFullViewL(const TUid aPhonebook, TInt aExpectedCount);
	void ReadAllContactNameViewL(const TUid aPhonebook, TInt aExpectedCount);
	void ReadAllContactNumberViewL(const TUid aPhonebook, TInt aExpectedCount);
	};

class CPhbkSyncReadICCContactTest : public CPhbkReadIntegrationTestBase
	{
public:
	static CPhbkSyncReadICCContactTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncReadICCContactTest();
	};

class CPhbkSyncReadSequenceOfICCEntriesTest : public CPhbkReadIntegrationTestBase
	{
public:
	static CPhbkSyncReadSequenceOfICCEntriesTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncReadSequenceOfICCEntriesTest();
	};

class CPhbkSyncReadContactInvalidUIDTest : public CPhbkReadIntegrationTestBase
	{
public:
	static CPhbkSyncReadContactInvalidUIDTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncReadContactInvalidUIDTest();
	};

class CPhbkSyncReadContactsEmptyFieldTest : public CPhbkReadIntegrationTestBase
	{
public:
	static CPhbkSyncReadContactsEmptyFieldTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncReadContactsEmptyFieldTest();
	};

class CPhbkSyncReadContactFullViewICCLockedTest : public CPhbkReadIntegrationTestBase
	{
public:
	static CPhbkSyncReadContactFullViewICCLockedTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncReadContactFullViewICCLockedTest();
	};

class CPhbkSyncReadContactDiffViewsTest : public CPhbkReadIntegrationTestBase
	{
public:
	static CPhbkSyncReadContactDiffViewsTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncReadContactDiffViewsTest();
	};

class CPhbkSyncReadICCMinimalContactTest : public CPhbkReadIntegrationTestBase
	{
public:
	static CPhbkSyncReadICCMinimalContactTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncReadICCMinimalContactTest();
	};

class CPhbkSyncReadMinimalContactInvalidUIDTest : public CPhbkReadIntegrationTestBase
	{
public:
	static CPhbkSyncReadMinimalContactInvalidUIDTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncReadMinimalContactInvalidUIDTest();
	};

class CPhbkSyncReadMinimalContactEmptyFieldTest : public CPhbkReadIntegrationTestBase
	{
public:
	static CPhbkSyncReadMinimalContactEmptyFieldTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncReadMinimalContactEmptyFieldTest();
	};

class CPhbkSyncReadMinimalContactICCLockedTest : public CPhbkReadIntegrationTestBase
	{
public:
	static CPhbkSyncReadMinimalContactICCLockedTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncReadMinimalContactICCLockedTest();
	};

class CPhbkSyncReadContactTextDefFullViewTest : public CPhbkReadIntegrationTestBase
	{
public:
	static CPhbkSyncReadContactTextDefFullViewTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncReadContactTextDefFullViewTest();
	};

class CPhbkSyncReadContactNumberTextDefTest : public CPhbkReadIntegrationTestBase
	{
public:
	static CPhbkSyncReadContactNumberTextDefTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncReadContactNumberTextDefTest();
	};

class CPhbkSyncReadContactNameTextDefTest : public CPhbkReadIntegrationTestBase
	{
public:
	static CPhbkSyncReadContactNameTextDefTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncReadContactNameTextDefTest();
	};

class CPhbkSyncReadContactInvalidTextDefTest : public CPhbkReadIntegrationTestBase
	{
public:
	static CPhbkSyncReadContactInvalidTextDefTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncReadContactInvalidTextDefTest();
	};

class CPhbkSyncReadContactICCLockedTest : public CPhbkReadIntegrationTestBase
	{
public:
	static CPhbkSyncReadContactICCLockedTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncReadContactICCLockedTest();
	};

class CPhbkSyncReadFullICCNameViewTest : public CPhbkReadIntegrationTestBase
	{
public:
	static CPhbkSyncReadFullICCNameViewTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncReadFullICCNameViewTest();
	};

class CPhbkSyncReadFullICCNumberViewTest : public CPhbkReadIntegrationTestBase
	{
public:
	static CPhbkSyncReadFullICCNumberViewTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncReadFullICCNumberViewTest();
	};

class CPhbkSyncReadFullICCFullViewTest : public CPhbkReadIntegrationTestBase
	{
public:
	static CPhbkSyncReadFullICCFullViewTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncReadFullICCFullViewTest();
	};

class CPhbkSyncReadFullICCNameTextDefTest : public CPhbkReadIntegrationTestBase
	{
public:
	static CPhbkSyncReadFullICCNameTextDefTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncReadFullICCNameTextDefTest();
	};

class CPhbkSyncReadFullICCNumberTextDefTest : public CPhbkReadIntegrationTestBase
	{
public:
	static CPhbkSyncReadFullICCNumberTextDefTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncReadFullICCNumberTextDefTest();
	};

class CPhbkSyncReadFullICCFullViewTextDefTest : public CPhbkReadIntegrationTestBase
	{
public:
	static CPhbkSyncReadFullICCFullViewTextDefTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncReadFullICCFullViewTextDefTest();
	};

class CPhbkSyncReadAdditionalNumTagsTest : public CPhbkReadIntegrationTestBase
	{
public:
	static CPhbkSyncReadAdditionalNumTagsTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncReadAdditionalNumTagsTest();
	};

class CPhbkSyncReadInternationalNumberTest : public CPhbkReadIntegrationTestBase
	{
public:
	static CPhbkSyncReadInternationalNumberTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkSyncReadInternationalNumberTest();
	};


class CPhbkSyncReadWithOutOpeningPhBkSyncFirstTest : public CPhbkReadIntegrationTestBase
	{
public:
	virtual TVerdict doTestStepL();
	};


#endif // _TE_CNTSYNCREAD_H_
