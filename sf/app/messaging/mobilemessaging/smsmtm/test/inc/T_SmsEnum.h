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

_LIT(KSmsEnumTestName, "Enumerate and Write to SIM");

class CSmsEnumTest : public CSmsTestBase
	{
public:
	static CSmsEnumTest* NewLC(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest);
	~CSmsEnumTest();

private:
	CSmsEnumTest(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest);
	void ConstructL();

	void RunL();
	void DoCancel();

	void TestEnumerateL(TMsvId aFolderId=KErrNotFound);
	void TestEnumerateAndDescLengthL(TMsvId aFolderId=KErrNotFound);
	void TestCopyFromSimL(TMsvId aFolderId=KErrNotFound);
	void TestMoveFromSimL(TMsvId aFolderId=KErrNotFound);
	void TestDeleteFromSimL();

	void TestEnumerateAndDetailsFieldL();

	void TestCopyToSimL();
	void TestMoveToSimL(TMsvId aSourceFolderId);
	void TestDeleteRemoteEntryL();

	void TestMultipleRecipientEnumerateL();

	void TestUnreadStatusL();
	void TestReadStatusL();
	void TestPhoneStoreStatusL(TMsvId aFolderId);
	void TestSimStoreStatusL(TMsvId aFolderId);

	void ShowMenuL();
	void RunAutoL();

	void DoRunEnumerateL();
	void DoRunEnumerateAndDescLengthL();
	void DoRunCopyToSim();
	void DoRunMoveToSim();
	void DoRunCopyMoveDeleteFromSim();
	void DoRunClearInbox();

	void ShowMenuAfterEnumerateL();
	void CheckParentAfterEnumerateL();
	TInt ErrorStatus();

	TInt GetInboxMsgsCountL();
	void ClearInboxL();
	void SaveClass2FolderIdL(TMsvId aFolderId);

	void TestAllEnumerateL(TMsvId aFolderId=KErrNotFound);
	TMsvId CreateEnumerateFolderLC(TMsvId aParent);

private:
	enum TSchSmsTestState
	/**	@enum	TSchSmsTestState
	 *	Enum defining the states of this test harness
	 */
		{
		EStateWaiting,
		EStateEnumerating,
		EStateEnumeratingAndDescLength,
		EStateEnumeratingAndDetailsField,
		EStateCopyFromSim,
		EStateMoveFromSim,
		EStateDeleteFromSim,
		EStateCopyToSim,
		EStateMoveToSim,
		EStateClearInbox
		} iState;

	RSocketServ iSocketServ;	//< A Socket Server
	RSocket iSocket;			//< A Socket
	TPckgBuf<TMsvId> iLastEnumerateFolder;

	CRichText* iBody;
	CSmsHeader* iHeader;
	CParaFormatLayer* iParaFormat;
	CCharFormatLayer* iCharFormat;

	};
