// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __T_IMCM_ASYNCHRONOUS_OPERATION_CANCELLATION_TEST_H__
#define __T_IMCM_ASYNCHRONOUS_OPERATION_CANCELLATION_TEST_H__

#include <e32base.h>
#include "emailtestutils.h"
#include "msvtestutilsbase.h"

#include <cmsvattachment.h>
class CImcmAsynchronousOperationCancellationTest;

class MTimerObserver
{
public:
	virtual void TimerEventHandler() = 0;
};

// CLASS : COperationTimer
// 
// NOTES : This class is a TIMER!!! and when the time period (interval) expires 
//         the observer's method TimerEventHandlerL() gets called.
//         It is up to the user of this class to restart the Timer.

class COperationTimer : public CTimer
{
public:
	static COperationTimer* NewL(MTimerObserver& aObserver);

protected:
	void RunL();

private:
	COperationTimer(MTimerObserver& aObserver);
	void ConstructL();

private:
	MTimerObserver&	iObserver;
};


// CLASS : CImcmAsyncOperationBase
// BASE  : CActive
// NOTES : Base class for all IMCM asynchronous operations cancellation tests
//       
//
class CImcmAsyncOperationBase : public CActive, MTimerObserver
{
public:
	void InitiateAsynchronousOperation();
	virtual void TimerEventHandler();
	~CImcmAsyncOperationBase();
	TInt ErrorStatus() { return(iErrorStatus); }

protected:
	TMsvId MessageId() { return(iMessageId); } // implicit inline method

	virtual void HandleOperationResult(TInt aOperationStatus) = 0;
	CImcmAsyncOperationBase(CImcmAsynchronousOperationCancellationTest& aObserver, CMsvEntry& aMessageEntry);
	void ConstructL();
	void DoCancel();
	void PrintStatistics();

protected:
	CImcmAsynchronousOperationCancellationTest&	iObserver;
	// iState: Defined as a signed integer and not as a enum in order 
	// to allow derived classes that have states to defined their enums 
	// with their own names and assigned them to this iState member variable.
	//
	// Requirement: If the derived class has states please make sure that  
	//              the first state to run must be zero (0) in the ENUM.
	TInt				iState; 

	TInt				iOperationsCancelled;
	TInt				iCancellationInterval; 
	TInt				iOperationsPerformed;
	TInt				iEncounteredErrors;
	TInt				iErrorStatus;
	TBool				iOperationDidNotComplete;
	COperationTimer*	iTimer;			// To be deleted
	CImEmailMessage*	iEmail;			// To be deleted

private:
	CMsvEntry&			iMessageEntry;
	TMsvId				iMessageId;
};


// CLASS : CAddAttachmentOperation
// BASE  : CImcmAsyncOperationBase
// NOTES : The class performs the test of adding attachments and then cancelleing them 
//         during different stages of the operation. This is controlled by a timer in 
//         the base class.
class CAddAttachmentOperation : public CImcmAsyncOperationBase
{
public:
	static CAddAttachmentOperation* NewL(CImcmAsynchronousOperationCancellationTest& aObserver, CMsvEntry& aMessageEntry);
	
private:
	virtual void HandleOperationResult(TInt aOperationStatus);

	virtual void RunL();
	virtual TInt RunError(TInt aError);
	void ConstructL();
	CAddAttachmentOperation(CImcmAsynchronousOperationCancellationTest& aObserver, CMsvEntry& aMessageEntry);

	void DoAddAttachmentL();
};

// CLASS : CDeleteAttachmentOperation
// BASE  : CImcmAsyncOperationBase
// NOTES : The class performs the test of deleting attachments and then cancelleing them 
//         during different stages of the operation. This is controlled by a timer in 
//         the base class.
class CDeleteAttachmentOperation : public CImcmAsyncOperationBase
{
public:
	static CDeleteAttachmentOperation* NewL(CImcmAsynchronousOperationCancellationTest& aObserver, CMsvEntry& aMessageEntry);
	~CDeleteAttachmentOperation();

private:
	virtual void HandleOperationResult(TInt aOperationStatus);

	virtual void RunL();
	virtual TInt RunError(TInt aError);

	void DoRetrieveAttachmentListL();	// State:0
	void DoDeleteAttachmentL();			// State:2

	void ConstructL();
	CDeleteAttachmentOperation(CImcmAsynchronousOperationCancellationTest& aObserver, CMsvEntry& aMessageEntry);

private:
	enum TOperationState
		{
		ERequestAttachmentList = 0,
		ERetrievingAttachments,
		EDeletingAttachments
		};

	CMsvEntrySelection* iEntriesToDelete; // To be deleted
};


// CLASS : CImcmAsynchronousOperationCancellationTest
// 
// NOTES : This class is the main driver of the test
//
class CImcmAsynchronousOperationCancellationTest : public CBase
{
public:
	static CImcmAsynchronousOperationCancellationTest* NewL();
	~CImcmAsynchronousOperationCancellationTest();	

	void PerformAdditionTestL();
	void CreateMessageL();
	
	void OperationAddCompletedL();
	void OperationDeleteCompleted();
private:
	void PerformDeletionTestL();
	void TerminateTestHarness();

private:
	CAddAttachmentOperation*	iAddOperation;
	CDeleteAttachmentOperation*	iDeleteOperation;
	CMsvEntry*					iMessageEntry;
	TMsvId						iMessageId;
	TInt						iHarnessErrorStatus;
};





#endif
