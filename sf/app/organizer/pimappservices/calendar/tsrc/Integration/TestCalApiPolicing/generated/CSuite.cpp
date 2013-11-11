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

// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
//

#include <e32base.h>
#include <e32cmn.h> 

#if (!defined TS_TestCalApiPolicing_SERVER_H_)
	#include "CSuite.h"
#endif

#include "EOpenAgenda1_CStep.h"
#include "ECloseAgenda1_CStep.h"
#include "ETransmitBuffer1_CStep.h"
#include "EGetInstanceExtractor1_CStep.h"
#include "EPreviousDayWithInstance1_CStep.h"
#include "ENextDayWithInstance1_CStep.h"
#include "ECreateEntryIterator1_CStep.h"
#include "EEntryIteratorNext1_CStep.h"
#include "EEntryIteratorPosition1_CStep.h"
#include "ECreateDateIterator1_CStep.h"
#include "EDateIteratorPrevious1_CStep.h"
#include "EDateIteratorNext1_CStep.h"
#include "EDateIteratorAtStart1_CStep.h"
#include "EDateIteratorAtEnd1_CStep.h"
#include "EDateIteratorCurrentKey1_CStep.h"
#include "EDateIteratorGoto1_CStep.h"
#include "EDateIteratorGotoLessOrEqual1_CStep.h"
#include "ECompactFile1_CStep.h"
#include "ECompactionStatus1_CStep.h"
#include "EGetEntryUidsSinceDate1_CStep.h"
#include "EGetFileId1_CStep.h"
#include "EGetCategoryListCount1_CStep.h"
#include "ECategoryFilter1_CStep.h"
#include "EStartBuildIndex1_CStep.h"
#include "EGetListFileNames1_CStep.h"
#include "ECancelTask1_CStep.h"
#include "EAgnResourceCount1_CStep.h"
#include "EAgnSetHeapFailure1_CStep.h"
#include "EAgendaFileExists1_CStep.h"
#include "EDisableChangeBroadcast1_CStep.h"
#include "EEnableChangeBroadcast1_CStep.h"
#include "ERequestChangeNotificationParameters21_CStep.h"
#include "ERequestChangeNotification21_CStep.h"
#include "ECancelChangeNotification21_CStep.h"
#include "ERequestProgress1_CStep.h"
#include "ESetUpdateAlarm1_CStep.h"
#include "ESetEnablePubSubNotification1_CStep.h"
#include "ERestoreAlarmAction1_CStep.h"
#include "EFetchEntry1_CStep.h"
#include "EFetchSimpleEntry1_CStep.h"
#include "EDateIteratorCurrentElement1_CStep.h"
#include "EFetchEntryByUID1_CStep.h"
#include "ERestoreText1_CStep.h"
#include "EGetCategoryListItem1_CStep.h"
#include "EGetRelatedGuid1_CStep.h"
#include "EGetChangesSinceLastNotification1_CStep.h"
#include "EFindInstances1_CStep.h"
#include "EUpdateEntry1_CStep.h"
#include "EAddEntry1_CStep.h"
#include "EDeleteEntry1_CStep.h"
#include "EAddEntrySizes1_CStep.h"
#include "EAddCategoryToList1_CStep.h"
#include "EDeleteAgendaFile1_CStep.h"
#include "ETidyByDateReadParams1_CStep.h"
#include "ETidyByDateStart1_CStep.h"
#include "ECategoryStart1_CStep.h"
#include "ECategoryStartAsyn1_CStep.h"
#include "ECreateAgendaFile1_CStep.h"
#include "EFetchEntryByGuid1_CStep.h"
#include "EDeleteEntriesByLocalUid1_CStep.h"
#include "EDeleteEntryByGuid1_CStep.h"
#include "ECommit1_CStep.h"
#include "ERollback1_CStep.h"

// __EDIT_ME__ - Substitute the name of your test server 
_LIT(KServerName,"Cap_TestCalApiPolicing_sc");
// __EDIT_ME__ - Use your own server class name

CTestTestCalApiPolicingServer* CTestTestCalApiPolicingServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
// __EDIT_ME__ - Use your own server class name
	CTestTestCalApiPolicingServer * server = new (ELeave) CTestTestCalApiPolicingServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

CTestStep* CTestTestCalApiPolicingServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	 CTestStep* testStep = NULL;
	// add test steps
		if (aStepName == _L("CEOpenAgenda1Step"))
			{
			testStep =  new(ELeave) CEOpenAgenda1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CECloseAgenda1Step"))
			{
			testStep =  new(ELeave) CECloseAgenda1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CETransmitBuffer1Step"))
			{
			testStep =  new(ELeave) CETransmitBuffer1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEGetInstanceExtractor1Step"))
			{
			testStep =  new(ELeave) CEGetInstanceExtractor1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEPreviousDayWithInstance1Step"))
			{
			testStep =  new(ELeave) CEPreviousDayWithInstance1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CENextDayWithInstance1Step"))
			{
			testStep =  new(ELeave) CENextDayWithInstance1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CECreateEntryIterator1Step"))
			{
			testStep =  new(ELeave) CECreateEntryIterator1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEEntryIteratorNext1Step"))
			{
			testStep =  new(ELeave) CEEntryIteratorNext1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEEntryIteratorPosition1Step"))
			{
			testStep =  new(ELeave) CEEntryIteratorPosition1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CECreateDateIterator1Step"))
			{
			testStep =  new(ELeave) CECreateDateIterator1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEDateIteratorPrevious1Step"))
			{
			testStep =  new(ELeave) CEDateIteratorPrevious1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEDateIteratorNext1Step"))
			{
			testStep =  new(ELeave) CEDateIteratorNext1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEDateIteratorAtStart1Step"))
			{
			testStep =  new(ELeave) CEDateIteratorAtStart1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEDateIteratorAtEnd1Step"))
			{
			testStep =  new(ELeave) CEDateIteratorAtEnd1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEDateIteratorCurrentKey1Step"))
			{
			testStep =  new(ELeave) CEDateIteratorCurrentKey1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEDateIteratorGoto1Step"))
			{
			testStep =  new(ELeave) CEDateIteratorGoto1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEDateIteratorGotoLessOrEqual1Step"))
			{
			testStep =  new(ELeave) CEDateIteratorGotoLessOrEqual1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CECompactFile1Step"))
			{
			testStep =  new(ELeave) CECompactFile1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CECompactionStatus1Step"))
			{
			testStep =  new(ELeave) CECompactionStatus1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEGetEntryUidsSinceDate1Step"))
			{
			testStep =  new(ELeave) CEGetEntryUidsSinceDate1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEGetFileId1Step"))
			{
			testStep =  new(ELeave) CEGetFileId1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEGetCategoryListCount1Step"))
			{
			testStep =  new(ELeave) CEGetCategoryListCount1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CECategoryFilter1Step"))
			{
			testStep =  new(ELeave) CECategoryFilter1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEStartBuildIndex1Step"))
			{
			testStep =  new(ELeave) CEStartBuildIndex1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEGetListFileNames1Step"))
			{
			testStep =  new(ELeave) CEGetListFileNames1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CECancelTask1Step"))
			{
			testStep =  new(ELeave) CECancelTask1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEAgnResourceCount1Step"))
			{
			testStep =  new(ELeave) CEAgnResourceCount1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEAgnSetHeapFailure1Step"))
			{
			testStep =  new(ELeave) CEAgnSetHeapFailure1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEAgendaFileExists1Step"))
			{
			testStep =  new(ELeave) CEAgendaFileExists1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEDisableChangeBroadcast1Step"))
			{
			testStep =  new(ELeave) CEDisableChangeBroadcast1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEEnableChangeBroadcast1Step"))
			{
			testStep =  new(ELeave) CEEnableChangeBroadcast1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CERequestChangeNotificationParameters21Step"))
			{
			testStep =  new(ELeave) CERequestChangeNotificationParameters21Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CERequestChangeNotification21Step"))
			{
			testStep =  new(ELeave) CERequestChangeNotification21Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CECancelChangeNotification21Step"))
			{
			testStep =  new(ELeave) CECancelChangeNotification21Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CERequestProgress1Step"))
			{
			testStep =  new(ELeave) CERequestProgress1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CESetUpdateAlarm1Step"))
			{
			testStep =  new(ELeave) CESetUpdateAlarm1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CESetEnablePubSubNotification1Step"))
			{
			testStep =  new(ELeave) CESetEnablePubSubNotification1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CERestoreAlarmAction1Step"))
			{
			testStep =  new(ELeave) CERestoreAlarmAction1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEFetchEntry1Step"))
			{
			testStep =  new(ELeave) CEFetchEntry1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEFetchSimpleEntry1Step"))
			{
			testStep =  new(ELeave) CEFetchSimpleEntry1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEDateIteratorCurrentElement1Step"))
			{
			testStep =  new(ELeave) CEDateIteratorCurrentElement1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEFetchEntryByUID1Step"))
			{
			testStep =  new(ELeave) CEFetchEntryByUID1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CERestoreText1Step"))
			{
			testStep =  new(ELeave) CERestoreText1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEGetCategoryListItem1Step"))
			{
			testStep =  new(ELeave) CEGetCategoryListItem1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEGetRelatedGuid1Step"))
			{
			testStep =  new(ELeave) CEGetRelatedGuid1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEGetChangesSinceLastNotification1Step"))
			{
			testStep =  new(ELeave) CEGetChangesSinceLastNotification1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEFindInstances1Step"))
			{
			testStep =  new(ELeave) CEFindInstances1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEUpdateEntry1Step"))
			{
			testStep =  new(ELeave) CEUpdateEntry1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEAddEntry1Step"))
			{
			testStep =  new(ELeave) CEAddEntry1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEDeleteEntry1Step"))
			{
			testStep =  new(ELeave) CEDeleteEntry1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEAddEntrySizes1Step"))
			{
			testStep =  new(ELeave) CEAddEntrySizes1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEAddCategoryToList1Step"))
			{
			testStep =  new(ELeave) CEAddCategoryToList1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEDeleteAgendaFile1Step"))
			{
			testStep =  new(ELeave) CEDeleteAgendaFile1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CETidyByDateReadParams1Step"))
			{
			testStep =  new(ELeave) CETidyByDateReadParams1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CETidyByDateStart1Step"))
			{
			testStep =  new(ELeave) CETidyByDateStart1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CECategoryStart1Step"))
			{
			testStep =  new(ELeave) CECategoryStart1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CECategoryStartAsyn1Step"))
			{
			testStep =  new(ELeave) CECategoryStartAsyn1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CECreateAgendaFile1Step"))
			{
			testStep =  new(ELeave) CECreateAgendaFile1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEFetchEntryByGuid1Step"))
			{
			testStep =  new(ELeave) CEFetchEntryByGuid1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEDeleteEntriesByLocalUid1Step"))
			{
			testStep =  new(ELeave) CEDeleteEntriesByLocalUid1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CEDeleteEntryByGuid1Step"))
			{
			testStep =  new(ELeave) CEDeleteEntryByGuid1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CECommit1Step"))
			{
			testStep =  new(ELeave) CECommit1Step  ;
			return testStep;
			 }
			 
		if (aStepName == _L("CERollback1Step"))
			{
			testStep =  new(ELeave) CERollback1Step  ;
			return testStep;
			 }
			 
		return testStep;
	 }

	

// Secure variants much simpler
// Just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
// __EDIT_ME__ - Use your own server class name
	CTestTestCalApiPolicingServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTestTestCalApiPolicingServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}


GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on process exit
 * Secure variant only
 * Process entry point. Called by client using RProcess API
 */
	{
	__UHEAP_MARK;
	TInt err = KErrNone;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP(err,MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
    }


TVerdict CCapabilityTestStep::doTestStepPreambleL( void )
	{
	//If Preamble is not required just pass a success value
	TVerdict testResult = CTestStep::doTestStepPreambleL();
	
	
	return TestStepResult();
	
	}
TVerdict CCapabilityTestStep::doTestStepPostambleL( void )
	{
	//If Postamble is not required just pass a success value
	TVerdict testResult = CTestStep::doTestStepPostambleL();

	return TestStepResult();
	
	}


// Moved from CStep.cpp

enum TVerdict CCapabilityTestStep::doTestStepL()
	{
	//DEF! INFO_PRINTF2(_L("%S - Starting ..."), &iTestStepName);

	//The MainThread()creates a separate thread that executes SendReceive
	TVerdict vResult = MainThread();
	
	SetTestStepResult(vResult);
	return TestStepResult();
	}

/*
ThreadStartFn:
Called by:	The Child thread
Function:	Calls the Exec_SendReceive
*/
static TInt ThreadStartFn( TAny * ptr )
	{
	return(((CCapabilityTestStep *)ptr)->Exec_SendReceive());
	}

/*
TVerdict GetVerdict(TInt aAPIretValue)

Called by: "MainThread" for returning verdict

Parameters(TInt aRetValue) :	0 if API call gets thru without any rejection
								1 if API call is rejected for capability error	
*/
enum TVerdict CCapabilityTestStep::GetVerdict(TInt aAPIretValue)
	{
	TVerdict vVerdict[] = {EPass, EFail};

	//please leave the following if/else block as the information printed by INFO_PRINTF1 is used bu CapTestSumm
	if(iExpect_Rejection)//[Inverse Test] EPass for 1 while EFail for 0
		{
		INFO_PRINTF1(_L("Test Expected to Fail due to lack of capabilities"));
		return vVerdict[(aAPIretValue)?0:1];
		
		}
	else //[Direct Test] EPass for 0 while EFail for 1
		{
		INFO_PRINTF1(_L("Test Expected to Pass with correct capabilities"));
		return vVerdict[(aAPIretValue)?1:0];
		}
	}


/*
TVerdict MainThread()

Called by: "doTestStepL"

Purpose:	Creates the child thread(which calls the respective function with regard
			to the server and also implements the Message Call). Then this fn.waits for the
			completion of the childthread( doesnt matter how the thread did die!)

Return Value(Verdict of the TestStep):

			A.Reporting PASS/FAIL
				Direct Test:
						When a message call gets thru. Please note that in such cases
						actually the implementation of the message has started. As we
						are passing "0" Parameters, server may panic, though our botheration
						stops once the call gets thru.
						NOTE:	The style is the same when CONNECTION capabilities
								are tested, the only diff is you dont have to expect a 
								panic from server
				Inverse Test:
						The call should be either failed or panicked with
						"KErrPermissionDenied" flag. 

				General Case:
						If a thread creation failed or if the server couldnt be connected
						apart from the above two cases, then a FAIL is reported
			
			B.Reporting INCONCLUSIVE
						Any panic say from unexpected source (eg:KERN-EXEC) will be
						reported INCONCLUSIVE														
*/
TVerdict CCapabilityTestStep::MainThread()
	{

	TBuf<100>	tExitCategory;
	TInt		tExitReason = 0;		
	TBuf<100>	TestStyle;

	iExpect_Rejection?TestStyle = _L("Inverse"):TestStyle = _L("Direct");
	TCapabilitySet theCaps =  TSecurityInfo(RProcess()).iCaps ;
	const TInt KMaxTestThreadHeapSize = 0x10000;

	//Initialize return values
	iResult_SR = iResult_Server = KErrNone;	
		

	// Create a child thread, with a new heap
	TInt nRes_Thread =	tChildThread.Create(
						ChildThread_SR,
						ThreadStartFn,
						KDefaultStackSize,
						KMinHeapSize,
						KMaxTestThreadHeapSize,
						this,			
						EOwnerProcess);


	if(nRes_Thread == KErrNone)//Thread Created
		{

		//Let me know when the thread is killed
		TRequestStatus ThreadStatus;
		tChildThread.Logon(ThreadStatus);
		tChildThread.Resume(); 
		
		User::WaitForRequest( ThreadStatus );
		
		
		tExitCategory	=	tChildThread.ExitCategory();
		tExitReason		=	tChildThread.ExitReason();
		
	=	
		if(iSessionCreated && (SR_MESSAGE_ID >=0))//Flag set by Child thread when connected to Server
		{
			//DEF INFO_PRINTF5(_L("Connected to Server(%S) for %S Test [MessageID: %d,Req.Cap: 0x%x,Present.Cap: 0x%x]"),&SR_ServerName,&TestStyle,SR_MESSAGE_ID,iStepCap,TSecurityInfo(RProcess()));			
		}
		else if(SR_MESSAGE_ID < 0)
		{
			//DEF INFO_PRINTF5(_L("Testing Connection capabilities[%S Test] for Server(%S)  [Req.Cap: 0x%x,Present.Cap: 0x%x]"),&TestStyle,
			//&SR_ServerName,TSecurityInfo(RProcess()));			
		}
		else if(!iSessionCreated)// NO Connection
			{
			INFO_PRINTF4(_L("Couldnt connect to the Server(%S) ErrorCode - ServerRet: %d C32ret: %d"),&SR_ServerName,iResult_Server,iResult_C32);
			//INFO_PRINTF3(_L("Child Thread: ExitCategory : %S ExitReason : %d"),&tExitCategory,tExitReason);
  			return EFail;
 			}



		switch(tChildThread.ExitType())
			{			
			case EExitPanic:
				//1.A Panic from the connected Server 
				//2.A CServer Panic normally for capability rejection
				//3.A kernel Panic 
				if((tExitReason == KErrPermissionDenied) ||
					//DEF ? 	it's old version (tExitReason == CServer::EClientDoesntHaveRequiredCaps))//Rejected for Insufficient Cap.
					// is it correct ?
					(tExitReason == CServer2::EClientDoesntHaveRequiredCaps))//Rejected for Insufficient Cap.
					{
					INFO_PRINTF2(_L("Rejected for insufficient capabilities [Return Value : %d] "),tExitReason);
					return(GetVerdict(API_RetValue_PermissionDenied));
					}
				else if(tExitCategory == iServer_Panic) //Panic from Server
					{
					INFO_PRINTF2(_L("Server(%S) Panic to child thread"),&tExitCategory);	
					INFO_PRINTF3(_L("Child Thread: ExitCategory : %S ExitReason : %d"),&tExitCategory,tExitReason);			
					return(GetVerdict(API_RetValue_ServerPanic));
					}
				else//A kernel Panic possibly
					{
					INFO_PRINTF3(_L("Child Thread: Panic from unexpected source (ExitCategory: %S ExitReason : %d)!"),&tExitCategory,tExitReason);
					return EInconclusive;
					}

			case EExitKill:
				if(iResult_SR != KErrPermissionDenied)
					{
					INFO_PRINTF2(_L("A Successfull call (Return Value : %d)"),((SR_MESSAGE_ID >=0)?iResult_SR:iResult_Server));
					return(GetVerdict(API_RetValue_NoCapError));
					}
				else 
					{
					INFO_PRINTF2(_L("Rejected for insufficient capabilities [Return Value : %d] "),((SR_MESSAGE_ID >=0)?iResult_SR:iResult_Server));
					return(GetVerdict(API_RetValue_PermissionDenied));			
					}

			default:					
					break;
			}
		}			
	else //thread couldnt start
		{
		INFO_PRINTF2(_L("ERROR: Failed to create Child thread,  ErrorCode:(%d)"),nRes_Thread);
		return EFail;
		}

	return EInconclusive;
	}

TInt CCapabilityTestStep::StartServer()
{
	TInt err =  KErrNone ;
	// EKA2 is simple No path required
	TBuf<32> serverFile;
	serverFile.Copy(_L("agsvexe"));
	_LIT(KExe,".exe");
	serverFile.Append(KExe);
	RProcess server;
	err = server.Create(serverFile,_L(""));
	if(err != KErrNone)
		return err;
	// Synchronise with the server
	TRequestStatus reqStatus;
	server.Rendezvous(reqStatus);
	server.Resume();
	 //Server will call the reciprocal static synchronise call
	User::WaitForRequest(reqStatus);
	//server.Close();
	if(reqStatus.Int() != KErrNone)
		return reqStatus.Int();
	return err;
}	
	
TInt CCapabilityTestStep::TestDebugHeap(TInt* iDbgIPCNo)
 	{
 	
 	//TDbgFns  {MarkHeapStart, MarkHeapEnd, CheckHeap, FailNext, ResetFailNext};
 	TInt aFnToTest= iDbgIPCNo[5];
 			
 	
 	TInt iResult_SR [6] ={0};
 	TInt i = 1;
 	TInt testedFn = 0;
 
 	TInt dbgTestSequence[5][6]	=	{	{MarkHeapStart	,2,0,1,-1,-1}, 
 										{MarkHeapEnd	,2,0,1,-1,-1},
 										{CheckHeap		,3,0,2, 1,-1},
 										{FailNext		,4,0,3, 4, 1},
 										{ResetFailNext	,4,0,3, 4, 1}
 										
 									};
 
 	
 	TInt aCount = dbgTestSequence[aFnToTest][i];
 
 	while(aCount--  )
 		{
 		testedFn =  dbgTestSequence[aFnToTest][(++i)];
 		
			iResult_SR[testedFn ]= SendReceive( iDbgIPCNo[testedFn],TIpcArgs(((iDbgIPCNo[testedFn]==3 )?4:0),0,0,0));
   		
 		
 		if( ((testedFn !=aFnToTest)?iResult_SR[testedFn]:KErrNone) == KErrPermissionDenied) 			
			
			User::Panic(_L("Failed at Initialization"),iResult_SR[testedFn]);		

		}

	return iResult_SR[aFnToTest];
	}

/**
Waits for sometime and checks if the asynchronous call has been answered by the server/kernel
If yes, we eat the signal. Else we assume that the call has been delivered to the ServiceL of the
Server, but we are being put on hold for an event to occur. After all, we only want to know if there
was a KErrPermissionDenied
*/
TInt CCapabilityTestStep::WaitForSomeTimeAndDecide(TRequestStatus& aStatus)
	{
	TRequestStatus timerStatus;
	RTimer timer;
	timer.CreateLocal();
	timer.After(timerStatus, 10000000);
	User::WaitForRequest(aStatus, timerStatus);
	
	if(aStatus == KRequestPending)
		{// timer expired and we still did not get any reply
		return KErrNone;
		}
	else
		{// Got some signal. Cancel the timer and read the signal
		timer.Cancel();
		return aStatus.Int();
		}	
	}

/**
Maintains a list of Async messages. And checks if the mesage passed as a parameter
is an async message
*/
TInt CCapabilityTestStep::GetSyncOrAsync(TInt aMessageNo)
	{
	const TInt size = 10;
	
	const TInt asyncMsgs[size] = {EStartBuildIndex, ETidyByDateStart, ERequestProgress, ECategoryStartAsyn, ERequestChangeNotification};
	
	for(TInt i = 0; i < size; ++i)
		{
		if (aMessageNo == asyncMsgs[i])
			{
			return KAsyncMessage;
			}
		}
	return KSyncMessage;
	}
