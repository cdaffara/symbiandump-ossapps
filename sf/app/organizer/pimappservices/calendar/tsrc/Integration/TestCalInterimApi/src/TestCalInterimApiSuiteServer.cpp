// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TestCalInterimApiSuiteServer.h"
#include "TestCalInterimApiFileAccess.h"
#include "TestCalInterimApiIterator.h"
#include "TestCalInterimApiFilter.h"
#include "TestCalInterimApiReadData.h"
#include "TestCalInterimApiExceptionStep.h"
#include "TestCalInterimApiStartupStep.h"
#include "TestCalInterimApiSetSysTime.h"
#include "TestCalInterimApiDeleteAlarms.h"
#include "TestCalInterimApiSetTz.h"
#include "TestCalInterimApiRichAlarmStep.h"
#include "TestCalInterimApiRichAlarmFormatStep.h"
#include "TestCalInterimApiDeleteData.h"
#include "TestCalInterimApiUpdateEntry.h"
#include "TestCalInterimApiDeleteCategory.h"
#include "TestCalInterimApiCopyEntry.h"
#include "TestCalInterimApiDeleteInstances.h"
#include "TestCalInterimApiDeleteEntryByTimeRange.h"
#include "TestCalInterimApiGetCategoryList.h"
#include "TestCalInterimApiDeleteEntryByLocalId.h"
#include "TestCalInterimApiCheckAlarm.h"
#include "TestCalInterimApiAlarmProperties.h"
#include "TestCalInterimApiImportExportAsync.h"
#include "TestCalInterimApiImportExport.h"
#include "TestCalInterimApiImportExportSoakStress.h"
#include "TestCalInterimApiTZChanges.h"
#include "TestCalInterimApiModifiedLocalIds.h"
#include "TestCalInterimApiTZRules.h"
#include "TestCalInterimApiLastModified.h"
#include "TestCalInterimApiModifierThread.h"
#include "TestCalInterimApiFetchEntryAndCheckData.h"
#include "TestCalInterimApiMainThread.h"
#include "TestCalInterimApiConcurrentFileAccess.h"
#include "TestCalInterimApiCompare.h"
#include "TestCalInterimApiDeleteEntryByTimeRangeAsync.h"
#include "TestCalInterimApiCheckAttendee.h"
#include "TestCalInterimApiDeleteAttendee.h"
#include "TestCalInterimApiFetchEntryByLocalId.h"
#include "TestCalInterimApiCheckAttachment.h"
#include "TestCalInterimApiDeleteAttachment.h"
#include "TestCalInterimApiAttachmentManager.h"
#include "TestCalInterimApiCalPerformance.h"
#include "TestCalInterimApiRemoveMedia.h"
#include "TestCalInterimApiFindInstancesIterator.h"
#include "CalendarTestUtility.h"
#include "CalendarIndexingTestBase.h"
#include "CalendarIndexModificationTests.h"
#include "CalendarIndexingPerformanceTests.h"
#include "CalendarIndexStressTests.h"
#include "testcalinterimapibackuprestore.h"
#include "TestCalInterimApiDeleteAgendaServers.h"

// Constants used by the ConsoleAlarmAlertServer
#ifdef __WINS__
_LIT(KConsoleAlarmAlertServerImg,"ConsoleAlarmAlertServer");
const TUid KServerUid2={0x1000008D};
const TUid KServerUid3={0x101F502A};
const TUidType serverUid(KNullUid,KServerUid2,KServerUid3);
#endif

const static TInt KInvalidTestStepPanic = 6789;

CTestCalInterimApiSuite* CTestCalInterimApiSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTestCalInterimApiSuite *	server = new (ELeave) CTestCalInterimApiSuite();
	CleanupStack::PushL(server);
	// CServer base class call
	RProcess	handle = RProcess();
	TParsePtrC	serverName(handle.FileName());
	server->ConstructL(serverName.Name());
	CleanupStack::Pop(server);
	return server;
	}

LOCAL_C void MainL()
	{
	// Leave the hooks in for platform security
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
	CActiveScheduler*	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTestCalInterimApiSuite*	server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTestCalInterimApiSuite::NewL());
	if(err == KErrNone)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);

		#ifdef __WINS__

		// Start the Alarm Server
		RProcess alarmServer;
		TInt err = alarmServer.Create(KConsoleAlarmAlertServerImg, KNullDesC, serverUid);
		TRequestStatus stat;
		alarmServer.Rendezvous(stat);
		if (stat != KRequestPending)
			{
			alarmServer.Kill(0);
			}
		else
			{
			alarmServer.Resume();
			}
			
		User::WaitForRequest(stat);
		TInt result = stat.Int();
		if(result != KErrNone && result != KErrAlreadyExists)
			{
			User::LeaveIfError(result);	
			}
		#endif

		sched->Start();

		#ifdef __WINS__
		// If this is not done, the testserver process object is not being destroyed
		// immediately in the emulator, and hence problem arises when we try
		// to do another LOAD_SUITE immediately.
		alarmServer.Terminate(KErrNone);
		alarmServer.Close();
		#endif
		}
	delete server;
	delete sched;
	}

/**
 * @return - Standard Epoc error code on process exit
 * Process entry point. Called by client using RProcess API
 */
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD( error, MainL() );
	delete cleanup;
	__UHEAP_MARKEND;
	return error;
    }

/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
CTestStep* CTestCalInterimApiSuite::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	// They are created "just in time" when the worker thread is created
	// More test steps can be added below
	if(aStepName == KTestCalInterimApiFileAccess)
		{
		testStep = new CTestCalInterimApiFileAccess();
		}
	else if(aStepName == KTestCalInterimApiIterator)
		{
		testStep = new CTestCalInterimApiIterator();
		}
	else if(aStepName == KTestCalInterimApiFilter)
		{
		testStep = new CTestCalInterimApiFilter();
		}
	else if (aStepName == KTestCalInterimApiRichAlarmStep)
		{
		testStep = new CTestCalInterimApiRichAlarmStep();
		}
	else if (aStepName == KTestCalInterimApiRichAlarmFormatStep)
		{
		testStep = new CTestCalInterimApiRichAlarmFormatStep();
		}
	else if(aStepName == KTestCalInterimApiReadData)
		{
		testStep = new CTestCalInterimApiReadData();
		}
	else if (aStepName == KTestCalInterimApiModifierThread)
		{
		testStep = new CTestCalInterimApiModifierThread();
		}
	else if (aStepName == KTestCalInterimApiCopyEntry)
		{
		testStep = new CTestCalInterimApiCopyEntry();
		}
	else if (aStepName == KTestCalInterimApiDEF064928Step)
		{
		testStep = new CTestCalInterimApiDEF064928Step();
		}
	else if (aStepName == KTestCalInterimApiDeleteData )
		{
		testStep = new CTestCalInterimApiDeleteData();
		}
	else if (aStepName == KTestCalInterimApiUpdateEntry)
		{
		testStep = new CTestCalInterimApiUpdateEntry();
		}
  	else if (aStepName == KTestCalInterimApiDeleteCategory)
		{
		testStep = new CTestCalInterimApiDeleteCategory();
		}
	else if (aStepName == KTestCalInterimApiDeleteInstances)
		{
		testStep = new CTestCalInterimApiDeleteInstances();
		}
	else if (aStepName == KTestCalInterimApiGetCategoryList)
		{
		testStep = new CTestCalInterimApiGetCategoryList();
		}
	else if (aStepName == KTestCalInterimApiDeleteEntryByLocalId)
		{
		testStep = new CTestCalInterimApiDeleteEntryByLocalId();
		}
	else if (aStepName == KTestCalInterimApiDeleteEntryByTimeRange)
		{
		testStep = new CTestCalInterimApiDeleteEntryByTimeRange();
		}
	else if (aStepName == KTestStartUpExeStep)
        {
        testStep = new CTestStartUpExeStep(*this);
        }
	else if (aStepName == KTestCalInterimApiSetSysTime)
		{
		testStep = new CTestCalInterimApiSetSysTime();
		}
	else if (aStepName == KTestCalInterimApiDeleteAlarms)
		{
		testStep = new CTestCalInterimApiDeleteAlarms();
		}
	else if (aStepName == KTestCalInterimApiSetTz)
		{
		testStep = new CTestCalInterimApiSetTz();
		}
	else if (aStepName == KTestCalInterimApiCheckAlarm)
		{
		testStep = new CTestCalInterimApiCheckAlarm();
		}
	else if (aStepName == KTestCalInterimApiAlarmProperties)
		{
		testStep = new CTestCalInterimApiAlarmProperties();
		}
  	else if (aStepName == KTestCalInterimApiImportvCal)
		{
		testStep = new CTestCalInterimApiImportvCal();
		}
	else if (aStepName == KTestCalInterimApiExport)
		{
		testStep = new CTestCalInterimApiExport();
		}
	else if (aStepName == KTestCalInterimApiImportvCalSameUid)
		{
		testStep = new CTestCalInterimApiImportvCalSameUid();
		}
	else if (aStepName == KTestCalInterimApiImportvCalAlarm)
		{
		testStep = new CTestCalInterimApiImportvCalAlarm();
		}
	else if (aStepName == KTestCalInterimApiTZChange)
		{
		testStep = new CTestCalInterimApiTZChange();
		}
	else if (aStepName == KTestCalInterimApiDSTChange)
		{
		testStep = new CTestCalInterimApiDSTChange();
		}
	else if (aStepName == KTestCalInterimApiImportExportCreateStressData)
		{
		testStep = new CTestCalInterimApiImportExportCreateStressData();
		}
	else if (aStepName == KTestCalInterimApiImportExportCountEntry)
		{
		testStep = new CTestCalInterimApiImportExportCountEntry();
		}
	else if (aStepName == KTestCalInterimApiExportSoak)
		{
		testStep = new CTestCalInterimApiExportSoak();
		}
	else if (aStepName == KTestCalInterimApiVerifyChildParent)
		{
		testStep = new CTestCalInterimApiVerifyChildParent();
		}
	else if (aStepName == KTestCalInterimApiModifiedLocalIds)
		{
		testStep = new CTestCalInterimApiModifiedLocalIds();
		}
	else if (aStepName == KTestCalInterimApiModifiedLocalIds)
		{
		testStep = new CTestCalInterimApiModifiedLocalIds();
		}
	else if (aStepName == KTestCalInterimApiCreateRptTZRule)
		{
		testStep = new CTestCalInterimApiCreateRptTZRule();
		}
	else if (aStepName == KTestCalInterimApiVerifyTZRules)
		{
		testStep = new CTestCalInterimApiVerifyTZRules();
		}
	else if (aStepName == KTestCalInterimApiSetTZRuleEntry)
		{
		testStep = new CTestCalInterimApiSetTZRuleEntry();
		}
	else if (aStepName == KTestCalInterimApiLastModified)
		{
		testStep = new CTestCalInterimApiLastModified();
		}
	else if (aStepName == KTestCalInterimApiImportExportAsyncTest)
		{
		testStep = new CTestCalInterimApiImportExportAsyncTest();
		}
	else if (aStepName == KTestCalInterimApiFetchEntryAndCheckData)
		{
		testStep = new CTestCalInterimApiFetchEntryAndCheckData();
		}
	else if (aStepName == KTestCalInterimApiRDateExDate)
		{
		testStep = new CTestCalInterimApiRDateExDate();
		}
	else if (aStepName == KTestCalInterimApiBadRDateEx)
  		{
  		testStep = new CTestCalInterimApiBadRDateEx();
  		}

  	else if (aStepName == KTestCalInterimApiBadRptUntil)
  		{
  		testStep = new CTestCalInterimApiBadRptUntil();
  		}
  	else if (aStepName == KTestCalInterimApiBadReccurence)
  		{
  		testStep = new CTestCalInterimApiBadReccurence();
   		}
	else if (aStepName == KTestCalInterimApiConcurrentFileAccess)
		{
		testStep = new CTestCalInterimApiConcurrentFileAccess();
		}
	else if (aStepName== KTestCalInterimApiCompare)
		{
		testStep = new CTestCalInterimApiCompare();
		}
	else if (aStepName == KTestCalInterimApiMainThread)
		{
		testStep = new CTestCalInterimApiMainThread();
		}
	else if (aStepName == KTestCalInterimApiDeleteEntryByTimeRangeAsync)
		{
		testStep = new CTestCalInterimApiDeleteEntryByTimeRangeAsync();
		}
	else if (aStepName == KTestCalInterimApiCheckAttendee)
		{
		testStep = new CTestCalInterimApiCheckAttendee();
		}
	else if (aStepName == KTestCalInterimApiDeleteAttendee)
		{
		testStep = new CTestCalInterimApiDeleteAttendee();
		}
	else if (aStepName == KTestCalInterimApiFetchEntryByLocalId)
		{
		testStep = new CTestCalInterimApiFetchEntryByLocalId();
		}
	else if (aStepName == KTestCalInterimApiCheckAttachment)
		{
		testStep = new CTestCalInterimApiCheckAttachment();
		}
	else if (aStepName == KTestCalInterimApiDeleteAttachment)
		{
		testStep = new CTestCalInterimApiDeleteAttachment();
		}
	else if (aStepName == KTestCalInterimApiImportvCalAttachment)
		{
		testStep = new CTestCalInterimApiImportvCalAttachment();
		}
	else if (aStepName == KTestCalInterimApiAttachmentManager)
		{
		testStep = new CTestCalInterimApiAttachmentManager();
		}
	else if (aStepName == KTestCalInterimApiRemoveMedia)
		{
		testStep = new CTestCalInterimApiRemoveMedia();
		}
	else if (aStepName == KTestCalInterimApiCalPerformance)
		{
		testStep = new CTestCalInterimApiCalPerformance();
		}
	else if (aStepName == KTestCalInterimApiFindInstancesIterator)
		{
		testStep = new CTestCalInterimApiFindInstancesIterator();
		}
	else if (aStepName == KTestCalInterimApiUpdateAllInstances)
		{
		testStep = new CTestCalInterimApiUpdateAllInstances();
		}
	else if (aStepName == KTestCalInterimApiBackupRestore)
		{
		testStep = new CBackupRestoreTestStep;
		}
	else if (aStepName == KCalendarIndexingStartupPerformanceTest)
 		{
 		testStep = new CCalendarIndexingStartupPerformanceTest();
 		}
 	else if (aStepName == KCalendarIndexingPerformanceDegradationTest)
 		{
 		testStep = new CCalendarIndexingPerformanceDegradationTest();
 		}
 	else if (aStepName == KCalendarIndexingRamPerformanceTest)
 		{
 		testStep = new CCalendarIndexingRamPerformanceTest();
 		}
 	else if (aStepName == KCalendarIndexDCTest)
 		{
 		testStep = new CCalendarIndexDCTest();
 		}
 	else if (aStepName == KCalendarUniqueIndexTest)
 		{
 		testStep = new CCalendarUniqueIndexTest();
 		}
 	else if (aStepName == KCalendarIndexCategoryTest)
 		{
 		testStep = new CCalendarIndexCategoryTest();
 		}
 	else if (aStepName == KCalendarIndexModificationTest)
 		{
 		testStep = new CCalendarIndexModificationTest();
 		}
 	else if (aStepName == KCalendarIndexAlarmEntriesTest)
 		{
 		testStep = new CCalendarIndexAlarmEntriesTest();
 		}
 	else if (aStepName == KCalendarIndexTZBoundaryTest)
 		{
 		testStep = new CCalendarIndexTZBoundaryTest();
 		}
 	else if (aStepName == KCalendarIndexDSTBoundaryTest)
 		{
 		testStep = new CCalendarIndexDSTBoundaryTest();
 		}
 	else if (aStepName == KCalendarIndexFSErrorTest)
 		{
 		testStep = new CCalendarIndexFSErrorTest();
 		}
 	else if (aStepName == KClearDisk)
 		{
 		testStep = new CClearDisk();
 		}
 	else if (aStepName == KCalendarIndexCrashRecoveryTest)
 		{
 		testStep = new CCalendarIndexCrashRecoveryTest();
 		}
 	else if (aStepName == KCalendarIndexOOMTest)
   		{
 		testStep = new CCalendarIndexOOMTest();
   		}
 	else if (aStepName == KTestCalInterimApiDeleteAgendaServers)
 		{	
 		testStep = new CTestCalInterimApiDeleteAgendaServers();
 		}
 	else
 	    {
     		_LIT(KInvalid,"Invalid Test Step");
     	User::Panic(KInvalid, KInvalidTestStepPanic);
 	    }

	return testStep;
	}

/**
 * @return - err
 * Implements a method to launch the agenda server.
 * The method is called from a test step but the handle to
 * the new process is stores in the suiteserver so that
 * other test steps can either terminate or retieve information
 * regarding the startup process.
 * @param aExeName
 */
TInt CTestCalInterimApiSuite::LaunchAgnServer(TPtrC aExeName, TPtrC aServerMode)
    {
	TInt err=iStartUpProcess.Create(aExeName,aServerMode);

	if (err!=KErrNone)
		{
		return err;
		}

	TRequestStatus stat;
	iStartUpProcess.Rendezvous(stat);
	if (stat!=KRequestPending)
		{
		iStartUpProcess.Kill(0);		// abort startup
		}
	else
		{
		iStartUpProcess.Resume();	// logon OK - start the server
		}
	User::WaitForRequest(stat);		// wait for start or death
	return (err);
    }


/**
 * Implements a method to terminate the agenda server exe.
 * The method is called from a test step.
 */
void CTestCalInterimApiSuite::TerminateAgnServer()
    {
	iStartUpProcess.Terminate(0);
    }


/**
 * @return  TBool
 * @param aExitTypeFromIni
 * @param aExitReasonFromIni
 * Implements a method to check the agenda server exe.
 * The method is called from a test step. The exit type and
 * exit reason are retieved for the startup exe and compared
 * against expected values read from an ini file.
 */
TBool CTestCalInterimApiSuite::CheckAgnServer(TExitType aExitTypeFromIni, TInt aExitReasonFromIni)
    {
	TExitType exitType = iStartUpProcess.ExitType();
	TInt exitReason = iStartUpProcess.ExitReason();

	_LIT(KExitTypeFromIni,"Expected Exit Type From ini, ExitTypeValue = %d");
	INFO_PRINTF2(KExitTypeFromIni,aExitTypeFromIni);

	_LIT(KExitReasonFromIni,"Expected ExitReason from ini, ExitReasonValue = %d");
	INFO_PRINTF2(KExitReasonFromIni,aExitReasonFromIni);

	if (exitType == aExitTypeFromIni && exitReason == aExitReasonFromIni)
	    {
	    _LIT(KReasonOK,"Exit Type and reason for process as expected");
	    INFO_PRINTF1(KReasonOK);
	    _LIT(KExitTypeOk,"Exit Type Returned From Server, ExitType = %d");
	    _LIT(KExitReasonOk,"Exit Reason Returned Frrom Server, ExitReason = %d");
	    INFO_PRINTF2(KExitTypeOk,exitType);
	    INFO_PRINTF2(KExitReasonOk,exitReason);
		return (ETrue);
	    }
	else
	    {
	    _LIT(KReasonNotOK,"Exit Type and reason for process not as expected");
	    INFO_PRINTF1(KReasonNotOK);
	    _LIT(KExitTypeNotOk,"Exit Type Returned From Server, ExitType = %d");
	    _LIT(KExitReasonNotOk,"Exit Reason Returned From Server, ExitReason = %d");
	    ERR_PRINTF2(KExitTypeNotOk,exitType);
	    ERR_PRINTF2(KExitReasonNotOk,exitReason);
		return (EFalse);
	    }
    }
