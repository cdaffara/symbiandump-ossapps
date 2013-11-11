// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __HARNESS_H
#define __HARNESS_H

#include <e32test.h>
#include <badesca.h>
#include "MsvTestUtils.h"
#include "testframeutils.h"

//
//
// MBaseTestState
//

class MBaseTestState
	{
public:
	IMPORT_C virtual void StartL(TRequestStatus& aStatus) = 0;
	IMPORT_C virtual ~MBaseTestState();

	IMPORT_C void SetDebugInfo(const TTestDebugInfo& aDebugInfo);
	IMPORT_C const TTestDebugInfo& GetDebugInfo() const;

protected:
	TTestDebugInfo iDebugInfo;
	};

class TTestDebugInfo;

class CActiveTestState : public CActive, public MBaseTestState
/**
 * This is a base class for all active object test states. It provides
 * a RunError() function which notifies the test harness appropriately
 * when the test state's RunL() function leaves. This avoids a panic.
 */
	{
public:
	IMPORT_C CActiveTestState(TInt aPriority);
	IMPORT_C virtual TInt RunError(TInt aError);

protected:
	TRequestStatus* iReportStatus;
	};

//
//
// CBaseTestHarness
//

class CBaseTestHarness : public CActive, public MBaseTestState
	{
public:
	IMPORT_C virtual ~CBaseTestHarness();

	IMPORT_C virtual void StartL(TRequestStatus& aStatus);

	IMPORT_C virtual void AddStateL(MBaseTestState* aTestState, TTestDebugInfo aDebugInfo);

	IMPORT_C virtual void LogCommentL(const TDesC& aError);
	
	IMPORT_C virtual void SetHeapFailureTesting(TBool aTest);

	// From CActive
	IMPORT_C void RunL();
	IMPORT_C void DoCancel();
	IMPORT_C TInt RunError(TInt aError);
	IMPORT_C RTest& TestConsole();

protected:
	IMPORT_C virtual void ConstructL();
	IMPORT_C CBaseTestHarness(RTest& aTest);

	TInt iCurrentState;
	TRequestStatus* iReportStatus;

	RTest& iTest;
	CArrayPtrFlat<MBaseTestState>* iStateList;

	TBool iTestHeapFailure;

private:
//	CTestScript& iScript;
	TInt iHeapFailureCount;
	};


//
//
// CMainTestHarness
//

class CMainTestHarness : public CBaseTestHarness
	{
public:
	IMPORT_C static CMainTestHarness* NewL(RTest& aTest);
	IMPORT_C virtual void StartL(TRequestStatus& aStatus);

protected:
	IMPORT_C virtual void ConstructL();
	IMPORT_C CMainTestHarness(RTest& aTest);
	};


//
//
// CMsvClientTest
//

class CMsvClientTest : public CBaseTestHarness
	{
public:
	IMPORT_C static CMsvClientTest* NewL(CMsvTestUtils& aTestUtils, RTest& aTest);
	IMPORT_C static CMsvClientTest* NewL(CMsvTestUtils* aTestUtils, RTest& aTest);
	IMPORT_C virtual ~CMsvClientTest();
	IMPORT_C CMsvTestUtils& MsvTestUtils();
	IMPORT_C void DoCancel();
	IMPORT_C void SetCurrentOperation(CMsvOperation* aOperation);
	IMPORT_C CMsvOperation& CurrentOperation();
	IMPORT_C virtual void Reset();

protected:
	IMPORT_C virtual void ConstructL();
	IMPORT_C CMsvClientTest(CMsvTestUtils& aMsvTestUtils, RTest& aTest);
	IMPORT_C CMsvClientTest(CMsvTestUtils* aMsvTestUtils, RTest& aTest);

public:
	CMsvEntrySelection* iCurrentSelection;
	TMsvId iSelectedFolder;
	TMsvId iDestinationFolder;
	TMsvId iCurrentServiceId;

protected:
	CMsvTestUtils& iMsvTestUtils;

private:
	CMsvTestUtils* iOwnedMsvTestUtils;
	CMsvOperation* iOperation;
	};


#endif
