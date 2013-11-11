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

#ifndef _TE_CNTSYNCSTRESS_H_
#define _TE_CNTSYNCSTRESS_H_

#include <phbksync.h>
#include "te_cntsyncbase.h"

class CPhbkIntegrationStressTestBase : public CCntSyncTestStep
	{
public:
	virtual enum TVerdict doTestStepL() {return EFail; };

protected:	
	static TInt DoSyncThread(TAny* aPtr);
	static TInt ServerLaunchThread(TAny* aPtr);
	static TInt MultipleReadThread(TAny* aPtr);
	static TInt MultipleWriteThread(TAny* aPtr);
	static TInt MultipleDeleteThread(TAny* aPtr);
	static void CheckServerThreadCountL(TInt aExpectedCount);
	static void DoSyncThreadL();
	static void MultipleReadThreadL();
	static void MultipleWriteThreadL();
	static void MultipleDeleteThreadL();
	};

class CPhbkLaunchServerSimultaneouslyTest : public CPhbkIntegrationStressTestBase
	{
public:
	static CPhbkLaunchServerSimultaneouslyTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkLaunchServerSimultaneouslyTest();
	};

class CPhbkRequestDoSyncSimultaneouslyTest : public CPhbkIntegrationStressTestBase
	{
public:
	static CPhbkRequestDoSyncSimultaneouslyTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkRequestDoSyncSimultaneouslyTest();
	};

class CPhbkRequestReadSimultaneouslyTest : public CPhbkIntegrationStressTestBase
	{
public:
	static CPhbkRequestReadSimultaneouslyTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkRequestReadSimultaneouslyTest();
	};

class CPhbkRequestWriteSimultaneouslyTest : public CPhbkIntegrationStressTestBase
	{
public:
	static CPhbkRequestWriteSimultaneouslyTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkRequestWriteSimultaneouslyTest();
	};

class CPhbkRequestDeleteSimultaneouslyTest : public CPhbkIntegrationStressTestBase
	{
public:
	static CPhbkRequestDeleteSimultaneouslyTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkRequestDeleteSimultaneouslyTest();
	};

class CPhbkRequestSyncAndReadSimultaneouslyTest : public CPhbkIntegrationStressTestBase
	{
public:
	static CPhbkRequestSyncAndReadSimultaneouslyTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkRequestSyncAndReadSimultaneouslyTest();
	};

class CPhbkCheckServerThreadPriorityTest : public CPhbkIntegrationStressTestBase
	{
public:
	static CPhbkCheckServerThreadPriorityTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkCheckServerThreadPriorityTest();
	};

class CPhbkServerLostFileRecoveryTest : public CPhbkIntegrationStressTestBase
	{
public:
	static CPhbkServerLostFileRecoveryTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkServerLostFileRecoveryTest();
	};

class CPhbkIntegrationBackupTest : public CCntSyncTestStep
	{
public:
	static CPhbkIntegrationBackupTest* NewL();
	~CPhbkIntegrationBackupTest()	;
	virtual enum TVerdict doTestStepL();
	CPhbkIntegrationBackupTest();
	};

#endif
