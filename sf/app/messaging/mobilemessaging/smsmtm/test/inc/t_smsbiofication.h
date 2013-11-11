// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <smsclnt.h>
#include <smuthdr.h>
#include "smstestbase.h"
#include <smsuaddr.h>

_LIT(KSmsBioficationTestName, "Enumerate and Write BIO messages to SIM or phone");

class CSmsBioficationTest : public CSmsTestBase
	{
public:
	static CSmsBioficationTest* NewLC(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest);
	~CSmsBioficationTest();

private:
	CSmsBioficationTest(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest);
	void ConstructL();

	void RunL();
	void DoCancel();
//  Tests
	void TestSetupL();
	void TestEnumerateL();
	void TestCopyToPhoneStoreL();
	void TestCopyFromPhoneStoreL();
	void TestMoveFromPhoneStoreL();
	void TestDeleteFromPhoneStoreL();
	void TestSendL();
	void TestDeleteEnumerationFolderL();
	void TestCopyToPhoneStoreWithClass2FolderL();
	void TestMoveToPhoneStoreWithClass2FolderL();
	
	void RunAutoL();
	void ShowMenuL();

	void RunEnumerateL();
	void RunCopyToPhoneStoreL();
	void RunCopyMoveDeleteFromPhoneStoreL();

	void DeleteEntriesFromTestFolderL();
	void ChangeMtmUidsL(CMsvEntrySelection& aSelection);
	TBool CompareSelectionsL(const CMsvEntrySelection& aSelection1,const CMsvEntrySelection& aSelection2);
	TBool CompareEntriesL(TMsvId aId1, TMsvId aId2);

	void SetClass2FolderL(TMsvId aFolder);
private:
	enum TSchSmsTestState
		{
		EStateIdle,
		EStateSettingUp,
		EStateEnumerating,
		EStateCopyingToPhoneStore,
		EStateCopyingFromPhoneStore,
		EStateMovingFromPhoneStore,
		EStateDeletingFromPhoneStore,
		EStateDeletingEnumerationFolder,
		EStateSending,
		} iState;
	TMsvId iEnumerationFolderId;
	TMsvId iTestFolderId;
	CMsvEntrySelection* iTestSelection;
	RSocketServ iSocketServ;
	RSocket iSocket;
	};
