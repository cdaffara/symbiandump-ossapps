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

#ifndef T_SMCM_H_
#define T_SMCM_H_
#include <e32base.h>

#include <s32std.h>
#include <txtrich.h>    //CRichText
#include <e32test.h>
#include <msvuids.h>	// ServiceEntry, FolderEntry, etc.
#include <mtmuids.h>	// query capabilities
#include "smutset.h"	// sms settings
#include "smuthdr.h"	// sms header
#include "smsclnt.h"	// sms (a)sync commands
#include "smstestbase.h"

GLDEF_D RTest				test(_L("SMCM Test Harness"));
GLDEF_D CTrapCleanup*		theCleanup;

const TUid KUidSmcmTestNonSense = {0x00011111};

const TMsvPartList KMsvMessagePartBody			= 0x00000001;
const TMsvPartList KMsvMessagePartRecipient		= 0x00000002;
const TMsvPartList KMsvMessagePartOriginator	= 0x00000004;
const TMsvPartList KMsvMessagePartDescription	= 0x00000008;
const TMsvPartList KMsvMessagePartDate			= 0x00000010;
const TMsvPartList KMsvMessagePartAttachments	= 0x00000020;

GLDEF_C TInt E32Main(void);
LOCAL_C void doMainL(void);

class CSmcmTest : public CSmsTestBase
	{
public:
	static CSmcmTest* NewLC(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest);
	~CSmcmTest();

private:
	CSmcmTest(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest);
	void ConstructL();

	void RunL() {};
	void DoCancel() {};

	void ShowMenuL();
	void RunAutoL();

	//Test Functions
	void TestSettingsL();

	//Client Mtm Test Functions
	void TestCapabilitiesL();
	void TestFindL();
	void TestFindAliasL();
	void TestReplyL();
	void TestReplyEmailL();
	void DoTestReplyEmailL(const TDesC& aEmailMessageData, const TDesC& aAddress, const TDesC& aSubject);
	TMsvId CreateEmailSmsMessageL(const TDesC& aEmailMessageData);
	void TestForwardL();
	void TestForwardEmailL();
	void TestForwardL(const TMsvEntry& aEntry, TInt aBodyLength);
	TBool TestForwardBodyL(const CRichText& aBody, const TDesC& aExpected) const;
	void DoTestForwardEmailL(const TDesC& aEmailMessageData, const TDesC& aSubject, const TDesC& aBody);
	TBool TestForwardEntry(const TMsvEntry& aOriginalEntry, const TMsvEntry& aForwardEntry) const;
	HBufC* CreateRandomLC(TInt aLen) const;
	void TestReplyWithOverloadedSaveMessageL();

	void TestSyncFunctionsL();

	CSmsHeader*			iSmsHeader;
	CRichText*			iRichText;
	CParaFormatLayer*	iParaLayer;
	CCharFormatLayer*	iCharLayer;
	};


#endif
