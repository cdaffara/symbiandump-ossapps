// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __T_SMSDELIVERLASTSEGMENTREPORT_H__
#define __T_SMSDELIVERLASTSEGMENTREPORT_H__
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

LOCAL_C void doMainL(void);
GLDEF_C TInt E32Main(void);

class CSmsDeliverLastSegmentReport : public CSmsTestBase
	{
public:
	static CSmsDeliverLastSegmentReport* NewLC(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest);
	~CSmsDeliverLastSegmentReport();

private:
	CSmsDeliverLastSegmentReport(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest);
	void ConstructL();

	void RunL() {};
	void DoCancel() {};

	void ShowMenuL();
	void RunAutoL();

	//Test Functions
	void TestSmsSettingsL();
	void TestSmsHeaderL();
	void TestSettingLastSegmentDeliveryReport(CSmsMessageSettings& aSettings);
	TBool CompareSettingsFiles(
		RFs& aFs, const TDesC& aCompare1, const TDesC& aCompare2
		);
private:
	CSmsHeader*			iSmsHeader;
	CRichText*			iRichText;
	CParaFormatLayer*	iParaLayer;
	CCharFormatLayer*	iCharLayer;
};

#endif