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

#ifndef __TESTCALINTERIMAPI_BACKUPRESTORE_STEP_H__
#define __TESTCALINTERIMAPI_BACKUPRESTORE_STEP_H__

#include <test/testexecutestepbase.h>
#include <calsession.h>
#include <coreappstest/testserver.h>
#include <connect/sbeclient.h>

_LIT(KTestCalInterimApiBackupRestore,"BackupRestore");

class CBackupRestoreTestStep : public CTestStep
	{
public:
	CBackupRestoreTestStep();
	~CBackupRestoreTestStep();

private: // From CTestStep.
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();
	
	void TestBackupWhenFileIsOpenL();
	void TestBackupWhenFileCloseTimerRunningL();
	void TestRestoreWhenFileIsOpenL();
	void TestRestoreWhenFileCloseTimerRunningL();
	void TestBackupOrRestoreWhenFileIsOpenL(conn::TBURPartType aType);
	void TestBackupOrRestoreWhenFileCloseTimerRunningL(conn::TBURPartType aType);
	void SimulateClientDisconnections();
		
private:
	CActiveScheduler* iScheduler;
    CCalSession* iCalSession;
	RPIMTestServer iPIMTestServer;
	TDriveList iDriveList;
	};

#endif
