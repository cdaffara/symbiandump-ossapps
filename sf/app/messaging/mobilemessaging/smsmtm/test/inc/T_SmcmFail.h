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

#ifndef T_SMCMFAIL_H_
#define T_SMCMFAIL_H_

#include <e32base.h>
#include <txtrich.h>    //CRichText
#include <e32test.h>
#include "smutset.h"	// sms settings
#include "smsclnt.h"	// sms (a)sync commands
#include <smuthdr.h>
#include "smstestbase.h"

GLDEF_D RTest				test(_L("SMCM Test Harness"));
GLDEF_D CTrapCleanup*		theCleanup;

const TMsvPartList KMsvMessagePartBody			= 0x00000001;
const TMsvPartList KMsvMessagePartRecipient		= 0x00000002;
const TMsvPartList KMsvMessagePartOriginator	= 0x00000004;
const TMsvPartList KMsvMessagePartDescription	= 0x00000008;
const TMsvPartList KMsvMessagePartDate			= 0x00000010;
const TMsvPartList KMsvMessagePartAttachments	= 0x00000020;

GLDEF_C TInt E32Main(void);
LOCAL_C void doMainL(void);

class CSmcmFailTest : public CSmsTestBase
	{
public:
	static CSmcmFailTest* NewL(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aCurrentTest);
	~CSmcmFailTest();

private:
	CSmcmFailTest(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aCurrentTest);
	void ConstructL();

	void RunL() {};
	void DoCancel() {};

	void ShowMenuL();
	void RunAutoL();

	//Test Functions
	void TestSettingsFailL();
	void TestHeaderFailL();
	void TestNumberFailL();
	void TestReplyFailL();
	void TestForwardFailL();
	void TestLoadFailL();
	void TestSaveFailL();
	void TestAddresseeFailL();
	void TestBifFailL();

	void CleanFolderL(TMsvId aFolder);

	CRichText*			iRichText;
	CParaFormatLayer*	iParaLayer;
	CCharFormatLayer*	iCharLayer;
	};


#endif
