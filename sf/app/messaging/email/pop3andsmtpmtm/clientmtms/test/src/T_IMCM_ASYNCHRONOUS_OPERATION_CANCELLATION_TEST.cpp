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
// Comming Soon.
// Requirements :	TestUtils and the file plain_text.txt
// No network connection required
// Configuration: 
// KDelayIncrement is set as default to 2ms. If the test platform
// is slow the test will take longer, since:
// Number of cancellation = (total milliseconds/2)
// under WINS on 1.8GHz P4 it does 4 - 14 cancellations
// depending on the processes running...
// 
//


#include "T_IMCM_ASYNCHRONOUS_OPERATION_CANCELLATION_TEST.h"

#include <miutlog.h>
#include <msvapi.h>
#include <msvuids.h>
#include <miutmsg.h>

#include <mmsvattachmentmanager.h>

// Define these here so we can pop them using the checked version
LOCAL_D CEmailTestUtils*  testUtils = NULL;
LOCAL_D CActiveScheduler* scheduler = NULL;
LOCAL_D CImcmAsynchronousOperationCancellationTest* testHarness = NULL;

GLREF_D RTest test; 
RTest test(_L("T_IMCM_ASYNCHRONOUS_OPERATION_CANCELLATION_TEST"));

_LIT( KSmallAttachment, "C:\\mailtest\\rfc822\\plain_text.txt");
_LIT(     KSessionPath, "C:\\");

const  TInt KLogBufferLength			= 256;
const  TInt KDelayIncrement				= 2000; // microseconds
const  TInt KMaximumAcceptableErrors	= 10;
const  TInt KMaximumOperations			= 2;

void WriteToLog(TRefByValue<const TDesC> aFmt,...)
	{
	VA_LIST list;
	VA_START(list, aFmt);

	// Print to log file
	TBuf<KLogBufferLength> buf;
	buf.FormatList(aFmt, list);

	test.Printf(buf);	           // Print to the console
	testUtils->WriteComment(buf);  // Print to the log file
	}

//--- COperationTimer ---------------------------------------------

COperationTimer* COperationTimer::NewL(MTimerObserver& aObserver)
	{
	COperationTimer* self = new (ELeave) COperationTimer(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return (self);
	}

COperationTimer::COperationTimer(MTimerObserver& aObserver)
:	CTimer(CActive::EPriorityHigh), 
	iObserver(aObserver)
{ }

void COperationTimer::ConstructL() 
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

void COperationTimer::RunL()
	{
	iObserver.TimerEventHandler();
	}

//--- CImcmAsyncOperationBase ---------------------------------------------

CImcmAsyncOperationBase::CImcmAsyncOperationBase(CImcmAsynchronousOperationCancellationTest& aObserver, CMsvEntry& aMessageEntry)
:	CActive(CActive::EPriorityStandard),
	iObserver(aObserver),
	iCancellationInterval(KDelayIncrement),
	iMessageEntry(aMessageEntry),
	iMessageId(aMessageEntry.EntryId())
{ }


void CImcmAsyncOperationBase::DoCancel()
	{
	++iOperationsCancelled;
	iTimer->Cancel();
	iEmail->Cancel();
	}

CImcmAsyncOperationBase::~CImcmAsyncOperationBase()
	{
	delete iTimer;
	delete iEmail;
	}

void CImcmAsyncOperationBase::ConstructL()
	{
	iEmail = CImEmailMessage::NewL(iMessageEntry);
	iTimer = COperationTimer::NewL(*this);

	CActiveScheduler::Add(this);
	}

void CImcmAsyncOperationBase::InitiateAsynchronousOperation()
	{
	// This will trigger the first RunL in its subclasses and by checking 
	// the iOperationDidNotComplete being ETrue the subclass can start its 
	// own requests...
	// And since the asynchronous requests will be initiate within RunL
	// if any of them leave they can be checked at a single point RunError

	iOperationsPerformed = 0;
	iOperationDidNotComplete = ETrue;
	iState = 0; // Set the state to the first possible state described by ENUM (0)
	Cancel();

	TRequestStatus* status=&iStatus;
	User::RequestComplete(status,KErrNone);
	SetActive();
	}


void CImcmAsyncOperationBase::PrintStatistics()
	{
	WriteToLog(_L("\nStatistics:\n"));
	WriteToLog(_L("\tOperations Performed: %d\n"), iOperationsPerformed);
	WriteToLog(_L("\tOperations Cancelled: %d\n"), iOperationsCancelled);
	}

void CImcmAsyncOperationBase::TimerEventHandler()
	{
	iCancellationInterval += KDelayIncrement; // Increase by 0.5ms
	WriteToLog(_L("CANCELLED - %d\n"), iCancellationInterval);
	Cancel();

	iOperationDidNotComplete = ETrue;

	// Get back into RunL in order to start another asynch operation
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}


//--- CAddAttachmentOperation ---------------------------------------------
CAddAttachmentOperation* CAddAttachmentOperation::NewL(CImcmAsynchronousOperationCancellationTest& aObserver, CMsvEntry& aMessageEntry)
	{
	CAddAttachmentOperation* self = new (ELeave) CAddAttachmentOperation(aObserver, aMessageEntry);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return (self);
	}

CAddAttachmentOperation::CAddAttachmentOperation(CImcmAsynchronousOperationCancellationTest& aObserver, CMsvEntry& aMessageEntry)
:	CImcmAsyncOperationBase(aObserver, aMessageEntry)
	{ }

void CAddAttachmentOperation::ConstructL() 
	{ 
	CImcmAsyncOperationBase::ConstructL();
	}

TInt CAddAttachmentOperation::RunError(TInt aError) 
	{
	HandleOperationResult(aError);
	// Get back into RunL in order to start another asynch operation
	iOperationDidNotComplete = ETrue;
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();

	return(KErrNone); 
	}

void CAddAttachmentOperation::HandleOperationResult(TInt aOperationStatus)
	{
	switch(aOperationStatus)
		{
		case KErrNone:
			WriteToLog(_L("OK\n"));
			break;

		// If the error is KErrNotFound that would mean that the file
		// to be attached is not present therefore we need to terminate
		// the test...
		case KErrNotFound: // FATAL
			WriteToLog(_L("Required file 'C:\\mailtest\\rfc822\\plain_text.txt' is not present\n"));
			User::Panic(_L("Attachment file is not present."), KErrNotFound);
			break;
			
		default: // Any other error
			++iEncounteredErrors;
			iErrorStatus |= aOperationStatus;
			WriteToLog(_L("ERROR(%d)\n"), aOperationStatus);
			break;
		}

	if (iEncounteredErrors >= KMaximumAcceptableErrors)
		{
		User::Panic(_L("Too many errors"), KErrGeneral);
		}

	}

void CAddAttachmentOperation::RunL() 
	{
	if (!iOperationDidNotComplete)
		{
		iTimer->Cancel();
		HandleOperationResult(iStatus.Int());

		// If the operation was successfully completed then 
		// stop the timer from triggering a cancel.
		if (iStatus == KErrNone) 
			{
			// This will allow the next DoPerformAdditionL  
			// to start the Cancellation Timer
			++iOperationsPerformed; 		
			}
		}
	else // Make sure that the next time RunL runs it runs normally
		{			
		iOperationDidNotComplete = EFalse;
		}

	// The total number of attachment that will end up in the
	// message is two. One that gets completed without interruption
	// and the other after a series of cancellations.
	if (iOperationsPerformed >= KMaximumOperations)
		{
		PrintStatistics();
		iObserver.OperationAddCompletedL();
		}
	else
		{
		DoAddAttachmentL(); // Do next operation
		}
	}

void CAddAttachmentOperation::DoAddAttachmentL()
	{
	WriteToLog(_L("\tAdding attachments #%d..."), iOperationsPerformed);

	testUtils->iMsvEntry->SetEntryL(MessageId());
	CMsvAttachment* attachmentInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
	CleanupStack::PushL(attachmentInfo);
	iEmail->AttachmentManager().AddAttachmentL(KSmallAttachment, attachmentInfo, iStatus);
	CleanupStack::Pop(attachmentInfo);
	// If AddAttachmentL leaves it will never go beyong this point and
	// since all requests now come only from RunL we are safe to say that
	// RunError will catch anything that might go wrong...
	iStatus = KRequestPending;
	SetActive();

	// Since we do allow the first operation to go uninterrupted
	// we only start the cancellation timer if we already completed 
	// at least one.
	if (iOperationsPerformed > 0)
		{	
		iTimer->After(iCancellationInterval); 
		}
	}


//--- CDeleteAttachmentOperation ---------------------------------------------

CDeleteAttachmentOperation* CDeleteAttachmentOperation::NewL(CImcmAsynchronousOperationCancellationTest& aObserver, CMsvEntry& aMessageEntry)
	{
	CDeleteAttachmentOperation* self = new (ELeave) CDeleteAttachmentOperation(aObserver, aMessageEntry);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return (self);
	}

CDeleteAttachmentOperation::CDeleteAttachmentOperation(CImcmAsynchronousOperationCancellationTest& aObserver, CMsvEntry& aMessageEntry)
:	CImcmAsyncOperationBase(aObserver, aMessageEntry)
	{ 
	}

void CDeleteAttachmentOperation::ConstructL() 
	{ 
	CImcmAsyncOperationBase::ConstructL();
	iEntriesToDelete = new (ELeave) CMsvEntrySelection;
	iState = ERetrievingAttachments;
	}

CDeleteAttachmentOperation::~CDeleteAttachmentOperation() 
	{ 
	delete iEntriesToDelete;	
	}

TInt CDeleteAttachmentOperation::RunError(TInt aError) 
	{
	iCancellationInterval = 0;
	HandleOperationResult(aError);

	// Get back into RunL in order to start another asynch operation
	iOperationDidNotComplete = ETrue;
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();

	return(KErrNone);
	}

void CDeleteAttachmentOperation::HandleOperationResult(TInt aOperationStatus)
	{
	switch(aOperationStatus)
		{
		case KErrNone:
			WriteToLog(_L("OK\n"));
			break;

		case KErrNotFound:
			WriteToLog(_L("NOT FOUND\n"));
			iEntriesToDelete->Delete(0);// Pop top most attachment since it already been deleted
			break;

		default: // Any other error
			++iEncounteredErrors;
			iErrorStatus |= aOperationStatus;
			WriteToLog(_L("ERROR(%d)\n"), aOperationStatus);
			break;
		}

	if (iEncounteredErrors >= KMaximumAcceptableErrors)
		{
		User::Panic(_L("Too many errors"), KErrGeneral);
		}


	}

void CDeleteAttachmentOperation::RunL() 
	{
	if (!iOperationDidNotComplete)
		{
		iTimer->Cancel();
		if (iState == EDeletingAttachments) 
			{
			HandleOperationResult(iStatus.Int());
			++iOperationsPerformed;
			
			// If the operation just completed was successfully to delete the attachment
			// remove it from the array
			if ((iEntriesToDelete->Count() > 0) && (iStatus == KErrNone))
				{
				iCancellationInterval = 0;
				iEntriesToDelete->Delete(0);
				}
			}
		}
	
	// Make sure that the next time RunL runs it runs normally
	iOperationDidNotComplete = EFalse;

	switch(iState)
		{
		case ERequestAttachmentList:
			iState = ERetrievingAttachments;
			DoRetrieveAttachmentListL();
			break;

		case ERetrievingAttachments:
			iState = EDeletingAttachments;
			DoDeleteAttachmentL();
			break;

		case EDeletingAttachments:
			DoDeleteAttachmentL();
			break;

		} // switch
	} // RunL


void CDeleteAttachmentOperation::DoRetrieveAttachmentListL()
	{
	iEmail->GetAttachmentsListL(iStatus, MessageId(), 
								CImEmailMessage::EAllAttachments,
								CImEmailMessage::EThisMessageOnly);
	iStatus = KRequestPending;
	SetActive();
	}
	

void CDeleteAttachmentOperation::DoDeleteAttachmentL()
	{
	if (!iEntriesToDelete->Count()) // If no attachments left in array
		{
		PrintStatistics();
		iObserver.OperationDeleteCompleted();
		}
	else // if there are more
		{
		TMsvId attachmentId = iEntriesToDelete->At(0); // Retrieve top most attachment
		WriteToLog(_L("\tDeleting attachment #%d..."), attachmentId);
		
		// If the attachment we are trying to delete has already been deleted
		// this call leaves with -1 (KErrNorFound) and is considered OK since 
		// after a cancel we cannot tell if the delete succeded or not...and
		// by the fact the test checks the cancellation process...
		TMsvAttachmentId removeAttachmentId = attachmentId;
		iEmail->AttachmentManager().RemoveAttachmentL(removeAttachmentId, iStatus);
		iStatus = KRequestPending;
		SetActive();

		// Schedule to cancel operation after few milliseconds
		// but only if it isn't the last operation
		if (iOperationsPerformed > 0)
			{	
			iTimer->After(iCancellationInterval); 
			}
		}
	}

//---- CImcmAsynchronousOperationCancellationTest ------------------------------------------------------

CImcmAsynchronousOperationCancellationTest* CImcmAsynchronousOperationCancellationTest::NewL()
	{
	CImcmAsynchronousOperationCancellationTest* self = new (ELeave) CImcmAsynchronousOperationCancellationTest();
	return (self);
	}

CImcmAsynchronousOperationCancellationTest::~CImcmAsynchronousOperationCancellationTest() 
	{ 
	delete iAddOperation;
	delete iDeleteOperation;
	}
	
void CImcmAsynchronousOperationCancellationTest::OperationAddCompletedL()
	{
	if (iAddOperation->ErrorStatus() == KErrNone)
		WriteToLog(_L("PASSED Test 1"));
	else
		WriteToLog(_L("FAILED Test 1"));

	testUtils->TestFinish(1);
	iHarnessErrorStatus |= iAddOperation->ErrorStatus();
	PerformDeletionTestL();
	}

void CImcmAsynchronousOperationCancellationTest::OperationDeleteCompleted()
	{
	if (iDeleteOperation->ErrorStatus() == KErrNone)
		WriteToLog(_L("PASSED Test 2"));
	else
		WriteToLog(_L("FAILED Test 2"));
	testUtils->TestFinish(2);

	iHarnessErrorStatus |= iDeleteOperation->ErrorStatus();
	TerminateTestHarness();
	}

void CImcmAsynchronousOperationCancellationTest::TerminateTestHarness()
	{
	CActiveScheduler::Stop();
	if (iHarnessErrorStatus == KErrNone)
		{
		WriteToLog(_L("[PASSED]\n"));
		testUtils->TestHarnessCompleted();
		}
	else
		{
		WriteToLog(_L("[FAILED] - Error(%d)\n"), iHarnessErrorStatus);
		testUtils->TestHarnessFailed(iHarnessErrorStatus);
		}
	test.End();
	}

void CImcmAsynchronousOperationCancellationTest::CreateMessageL()
	{
	CTestActive* testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);

	TMsvEmailTypeList msvEmailTypeList = 0;
	TMsvPartList partList = (KMsvMessagePartBody | KMsvMessagePartAttachments);
	CImEmailOperation* emailOperation = CImEmailOperation::CreateNewL(testActive->iStatus, *(testUtils->iMsvSession), KMsvGlobalOutBoxIndexEntryId, partList, msvEmailTypeList, KUidMsgTypeSMTP);
	CleanupStack::PushL(emailOperation);

	TestMsvOperationTimer* testMsvOperationTimer = TestMsvOperationTimer::NewLC(test.Console(), emailOperation, test);
	testActive->StartL();
	testMsvOperationTimer->IssueRequest();
	CActiveScheduler::Start();

	//check progress
	TMsvId temp;
	TPckgC<TMsvId> paramPack(temp);
	const TDesC8& progBuf = emailOperation->ProgressL();
	paramPack.Set(progBuf);
	iMessageId = paramPack();
	testUtils->SetEntryL(iMessageId);
	iMessageEntry = testUtils->iMsvEntry;

	CleanupStack::PopAndDestroy(testMsvOperationTimer); 
	CleanupStack::PopAndDestroy(emailOperation); 
	CleanupStack::PopAndDestroy(testActive); 
	}

void CImcmAsynchronousOperationCancellationTest::PerformAdditionTestL()
	{
	testUtils->TestStart(1);
	test.Next(_L("Adding attachments to message"));
	iAddOperation = CAddAttachmentOperation::NewL(*this, *iMessageEntry);
	iAddOperation->InitiateAsynchronousOperation();
	}

void CImcmAsynchronousOperationCancellationTest::PerformDeletionTestL()
	{
	testUtils->TestStart(2);
	test.Next(_L("Deleting attachments from message"));
	iDeleteOperation = CDeleteAttachmentOperation::NewL(*this, *iMessageEntry);
	iDeleteOperation->InitiateAsynchronousOperation();
	}

//-------------------------------------------------------------------------

LOCAL_C void InitL()
	{
	scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);

	testUtils = CEmailTestUtils::NewLC(test);
	testUtils->CreateAllTestDirectories();
	testUtils->FileSession().SetSessionPath(KSessionPath);
	TRAPD(operationStatus, testUtils->CleanMessageFolderL());
	if (operationStatus != KErrNone)
		{
		WriteToLog(_L("Failed to clean message folder.\n"));
		testUtils->TestHarnessFailed(operationStatus);
		CleanupStack::PopAndDestroy(testUtils); 
		CleanupStack::PopAndDestroy(scheduler);  
		User::Leave(operationStatus);
		}
	testUtils->GoServerSideL();
	
	test.Next(_L("Create Data Component FileStores"));
	testUtils->CreateSmtpServerMtmRegL();
	testUtils->CreatePopServerMtmRegL();

	//	Loading the DLLs
	testUtils->CreateRegistryObjectAndControlL();
	testUtils->InstallSmtpMtmGroupL();
	testUtils->InstallPopMtmGroupL();

	testUtils->GoClientSideL();
	testUtils->WriteComment(_L("T_IMCM_ASYNCHRONOUS_OPERATION_CANCELLATION_TEST Tests"));

	}


LOCAL_C void Closedown()
	{
	CleanupStack::PopAndDestroy(testHarness);
	CleanupStack::PopAndDestroy(testUtils);
	CleanupStack::PopAndDestroy(scheduler);
	}

void doMainL()
	{
	InitL();
	
	testHarness = CImcmAsynchronousOperationCancellationTest::NewL();
	CleanupStack::PushL(testHarness);
	
	test.Next(_L("Create new Message"));
	testHarness->CreateMessageL();

	testHarness->PerformAdditionTestL();
	CActiveScheduler::Start();
	
	Closedown();
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	test.Start(_L("T_IMCM_ASYNCHRONOUS_OPERATION_CANCELLATION_TEST - Test Harness"));
	CTrapCleanup* theCleanup = CTrapCleanup::New();
	TRAPD(ret, doMainL());	
	if (ret != KErrNone)
		User::Panic(_L("Error(%d): Leave occured while executing DoRunL()"), ret);

	delete theCleanup;	
	test.Close();
	__UHEAP_MARKEND; 
	return(KErrNone);
	}
