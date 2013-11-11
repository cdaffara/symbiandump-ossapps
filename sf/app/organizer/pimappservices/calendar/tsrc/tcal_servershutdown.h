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
// tcal_servershutdown:
// This test is intended to test the delay added to the calendar file before it is closed
// 
//

#ifndef __TCAL_SERVERSHUTDOWN_H__
#define __TCAL_SERVERSHUTDOWN_H__

#include <calsession.h>
#include <calentryview.h>
#include <hal.h>
#include <ecom/ecom.h>
#include <calprogresscallback.h>
#include <coreappstest/testserver.h>
#include "caltestoom.h"
#include "caltestlib.h"


#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
#include <e32property.h>
#include "ASCliSession.h"
class CCalInstanceView;
#endif



_LIT(KTestName, "TCal_ServerShutdown");
RTest test(KTestName);

// Two sample calendar files
_LIT(KServerShutdownCalFile, "c:tcal_servershutdown_calfile");
_LIT(KServerShutdownCalFileMultifFile, "c:tcal_servershutdown_calfile_multi_file");

// v9.1 calendar file - conversion test
_LIT(KServerShutdown91CalFile, "c:tcal_servershutdown_calfile_v91");

// Calendar file - delete test
_LIT(KServerShutdownCalFileToDelete, "c:tcal_servershutdown_calfile_ToDelete");
_LIT(KServerShutdownCalFileToDeleteRAM, "c:\\private\\10003a5b\\tcal_servershutdown_calfile_ToDelete"); 

_LIT(KServerShutdownCalFileToDeleteOOM, "c:tcal_servershutdown_calfile_ToDelete");
_LIT(KServerShutdownCalFileToDeleteRAM_OOM, "c:\\private\\10003a5b\\tcal_servershutdown_calfile_ToDelete"); 

// Non-existent calendar filename
_LIT(KServerShutdownBadCalFileName, "c:tcal_servershutdown_calfile_ShouldNotExist");

// Calendar files in ROM (origin)
_LIT(KServerShutdownCalFileROM, "z:\\private\\10003a5b\\tcal_servershutdown_calfile");
_LIT(KServerShutdownCalFileMultiFileROM, "z:\\private\\10003a5b\\tcal_servershutdown_calfile_multi_file");
_LIT(KServerShutdown91CalFileROM, "z:\\private\\10003a5b\\tcal_servershutdown_calfile_v91");

// Calendar files in RAM (Destination)
_LIT(KServerShutdownCalFileRAM, "c:\\private\\10003a5b\\tcal_servershutdown_calfile");
_LIT(KServerShutdownCalFileMultiFileRAM, "c:\\private\\10003a5b\\tcal_servershutdown_calfile_multi_file");
_LIT(KServerShutdown91CalFileRAM, "c:\\private\\10003a5b\\tcal_servershutdown_calfile_v91");

// Expectd minimum delay before the calendar file is closed
const TTimeIntervalMicroSeconds32 KServerShutdownDelayMin = 5000000;
// Maximum possible delay before the calendar file is closed
const TTimeIntervalMicroSeconds32 KServerShutdownDelayMax = 6000000;

// Delay to test the cancellation of the calendar close delay timers
const TTimeIntervalMicroSeconds32 KServerCancelDelay = 3000000;

const TUid KServerUid2 = {0x1000008D};
const TUid KServerUid3 = {0x101F502A};
_LIT(KConsoleAlarmAlertServerImg, "ConsoleAlarmAlertServer");	

class CServerShutdownTestManager : public CActive, public MCalProgressCallBack, public MCalTestOomCallBack
	{
public:
	enum TTestCases
		{
		ETestServerShutdownWithIndex,
		ETestServerShutdownCancelWithIndexStart,
		ETestServerShutdownCancelWithIndexCancelled,
		ETestServerShutdownCancelWithIndexShutdown,
		ETestServerShutdownDeleteFile,
		ETestServerShutdownDeleteMultiFiles,
		ETestServerShutdownProcessExit,
		ETestServerShutdownOOMDeleteFile,
		ETestServerShutdownOOMOpenCloseFile,
		ETestServerSingleSessionOpenTwiceStart,
		ETestServerSingleSessionOpenTwiceShutdown,
		ETestServerMultiSessionSingleFileOpenStart,
		ETestServerMultiSessionSingleFileOpen,
		ETestServerMultiSessionSingleFileOpenShutdown,
		ETestServerMultiSessionMultiFileOpenStart,
		ETestServerMultiSessionMultiFileOpenShutdown,
		ETestServerShutdownOpenInvalidFileLeave,
		ETestServerShutdownv91CalendarFileConversion,
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
		ETestServerShutdownWithSystemStateNotification,
		ETestServerShutdownWithSystemStateNotificationAsync,
		ETestServerShutdownWithSystemStateNotificationInstance,
		ETestServerShutdownWithSystemStateNotificationNormal
#endif
		};
	
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	enum TSystemStateNotifications
		{
		ESystemStateNormal,
		ESystemStateShutdown,
		ESystemStateAck
		};
#endif

	static CServerShutdownTestManager* NewLC(TInt aPriority);
	~CServerShutdownTestManager();
	
	void RunAllTestsL();
	
private:
	void RunL();
	void DoCancel();
	
	void ProcessLogonL();
	void ShutdownProcessL(const TDesC& aProcessUid);
	
	// from MCalProgressCallBack
	void Progress(TInt aPercentageCompleted);
	void Completed(TInt aError);
	TBool NotifyProgress();
	
	// from MCalTestOomCallBack
	void OomTestL(TType aType, TInt aFailAt);
	
	CServerShutdownTestManager(TInt aPriority);
	void ConstructL();
	
	void TestServerShutdownWithIndexL();
	void TestServerShutdownCancelWithIndexL();
	void TestServerShutdownDeleteFileL();
	void TestServerShutdownDeleteMultiFilesL();
	void TestServerShutdownDeleteFileOOML(TInt aFailAt);
	void TestServerShutdownOpenCloseFileOOML();
	void TestServerShutdownProcessExitL();
	void TestServerShutdownOOML();
	void TestServerShutdownSingleSessionOpenTwiceL();
	void TestServerShutdownMultiSessionSingleFileOpenL();
	void TestServerShutdownMultiSessionMultiFileOpenL();
	void TestServerShutdownOpenInvalidFileLeaveL();
	void TestServerShutdownTestOldCalendarFileConversionL();
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	void TestServerShutdownWithSystemStateNotificationsL();
	void TestServerShutdownWithSystemStateNotificationsAsyncL();
	void TestServerShutdownWithStateNotificationAndInstanceViewL();
	void TestServerShutdownWithNormalSystemStateNotification();
	void CreateSystemStateNotificationProperty();
	void TestSystemAckNotification();
	void TestPostShutdownOperationsL(const TDesC8& aGuid, TInt aErr, TInt aAlarmCount = 0);
	void TestAsycnDeleteOperationL();
	void TestInstanceViewOperationsL();
	void ClearAllAlarmsL();
#endif
	
	// Record a timestamp before initiating server shutdown
	void RecordTimeStampBeforeServerShutdown();
	// Record a timestamp after initiating server shutdown, calculate the period of shutdown and ensure that is within the expected range
	void TestServerShutdownTime();
	
	void CreateSessionL();
	void CreateAdditionalSessionL();
	
	void CloseSession(TBool aWaitToClose);
	void CloseAdditionalSession(TBool aWaitToClose);
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	void CreateEntry(const TDesC8& aGuid);
	void CreateRecurringEntriesL(const TDesC8& aGuid);
	void CreateInstanceViewL(TBool aWaitToOpen);
#endif
	void CreateEntryViewL(const TDesC& aCalFile, TBool aWaitToOpen);
	
	void CreateAdditionalEntryViewL(const TDesC& aCalFile, TBool aWaitToOpen);
	
	void CloseEntryView();
	void CloseAdditionalEntryView();
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	void CloseInstanceView();
	void SendSystemStateNotification(TSystemStateNotifications aSystemStateNotification);
	void SubscribeToSystemNotification();
#endif
	
	// Async delay helper functions
	void AsyncDelay(TTimeIntervalMicroSeconds32 aMicroseconds);
	static TInt AsyncDelayCallBack(TAny* aPtr);

	RProcess iProcess;
	RPIMTestServer iPIMTestServer;
	CTestRegister*    iFileRegister; 

	CPeriodic* iPeriodic;
	
	CCalSession* iSession;
	CCalSession* iAdditionalSession;
	CCalEntryView* iEntryView;
	CCalEntryView* iAdditionalEntryView;

	TUint32 iSrvTimerStartTicks; 
	TInt iNanoTickPeriod;
	
	TInt iTestCase;
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	CCalInstanceView* iInstanceView;
	RProperty iSystemStateNotification;
	TUid iMyUid;
	TInt itcal_key;
	RASCliSession iAlarmServer;
#endif
	};

#endif
