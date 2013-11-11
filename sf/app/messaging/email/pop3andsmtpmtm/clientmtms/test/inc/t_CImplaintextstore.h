// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <test/tefunit.h>
#include "emailtestutils.h"
#include <txtrich.h>
#include <cemailaccounts.h>

#include <miutlog.h>
#include <miutmsg.h>
#include <cimplainbodytext.h>

class CImPlainBodyText;
class CActiveWaiter ;

//constants shared by the tests
const TInt KTestStringLength = 10;
const TInt KMaxFilePathLength = 50;
const TInt KBufferGranularity = 256;

_LIT(KEmail40kBodyFile,"c:\\msgtest\\testdata\\T_OnlinePerformance40KBody.txt");
_LIT(KCImPlainBodyTest,"t_cimplainbodytest");


// Note that all test suites must begin with "CTest"
class CTestCImPlainTextStore : public  CActiveTestFixture
	{
public:
	CTestCImPlainTextStore();
	// SetUp and TearDown code (optional)
	virtual void SetupL();
	virtual void TearDownL();
	static CTestSuite* CreateSuiteL(const TDesC& aName);
	void TestStoreRestore16BitChunkAsyncL();	
	void TestStoreRestore16BitChunkSynchL();
	void CreateNewPlaintextMessageL();
	void TestStoreRichTextAsPlainTextL();
private:
	CImPlainBodyText* iCImPlainBodyTextStore;
	TInt iRequestCount;
	CActiveWaiter* iActiveWaiter;
	TUint iDefaultCharset;
	
	HBufC16* iFilebuf1;
	HBufC16* iFilebuf2;
	HBufC16* iFilebuf3;

	CEmailTestUtils* testUtils;
	};


