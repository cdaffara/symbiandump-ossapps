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

#include "smstestbase.h"
#include <mmlist.h>


class CSmsSimParamOperation;
class CAutoTest;


_LIT(KSmsScTestName, "Reading and Writing SIM parameters");

class CSmsScTest : public CSmsTestBase
	{
public:
	static CSmsScTest* NewLC(CAutoTest& aAutoTest, CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest);
	~CSmsScTest();

	void ReadInitialParamsL(TRequestStatus& aStatus);
	void WriteOriginalParamsL(TRequestStatus& aStatus);
	void RoundTripTestL(const CMobilePhoneSmspList& aList, TRequestStatus& aStatus);

	void TestReadParamsFailL();
	void TestWriteParamsFailL(const CMobilePhoneSmspList& aList);

	//inline const CMobilePhoneSmspList& OriginalList() const {return iOriginalParams->SmspParams();}
	inline const CSmsSimParamOperation& OriginalOperation() const {return *iOriginalParams;}

	static void PrintListL(RTest& aTest, const CMobilePhoneSmspList& aList);
	static void PrintEntry(RTest& aTest, const RMobileSmsMessaging::TMobileSmspEntryV1& aEntry);

private:

	CSmsScTest(CAutoTest& aAutoTest, CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest);

	void WriteSimParamsL();
	void ReadAfterWriteL();
	
	void DoRunReadInitialParamsL();
	void DoRunWriteSimParamsL();
	void DoRunReadAfterWriteL();
	void DoRunWriteOriginalParamsL();

	TBool CompareListsL(const CMobilePhoneSmspList& aWritten, const CMobilePhoneSmspList& aRead);
	TBool CompareEntries(const RMobileSmsMessaging::TMobileSmspEntryV1& aWritten, const RMobileSmsMessaging::TMobileSmspEntryV1& aRead);

	void ConstructL();

	void RunL();
	void DoCancel();

	void ShowMenuL();
	void RunAutoL();

	void Queue(TRequestStatus& aStatus);
	void Complete(TInt aError);
	TInt RunError(TInt aError);

private:

	enum TSmsSendTestState
	/**	@enum	TSmsSendTestState
	 *	Enum defining the states of this test harness
	 */
		{
		EStateWaiting,
		EStateReadInitialParams,
		EStateWriteSimParams,
		EStateReadAfterWrite,
		EStateWriteOriginalParams
		} iState;

	CSmsSimParamOperation* iOriginalParams;		//< The operation contains the original parameters
	const CMobilePhoneSmspList* iListToWrite;	//< The list contains the new parameters to write
	TRequestStatus* iReport;					//< A request status
	CAutoTest& iAutoTest;						//< An auto test
	};
