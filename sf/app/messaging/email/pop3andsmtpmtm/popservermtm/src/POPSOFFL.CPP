
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

#include <msventry.h>
#include <offop.h>
#include "POPSOFFL.H"
#include "popstran.h"
#include "POPS.PAN"

// API
// ===
// The CImPop3OfflineOperationFinder class is used to find and order offline
// operations. It can also be used to obtain progress information eg. the
// number of operations queued of a particular type.

// Offline operations are retrieved by first calling the asynchronous
// FindFirstL function. After completetion call the OperationFound function
// to find out whether or not an offline operation has been found, if it has
// then the OfflineOperation function should be called to retrive it.
//
// The FindNextL function should be called to find other offline operations
// (in conjunction with the OperationFound and OfflineOperations functions.)
// It should be noted that the FindNextL function is synchronous.

// The DeleteCurrentOperationL() function deletes the current entry from the
// message store.

// OperationDetails returns TOperationDetails which can be used for providing
// progress information.


// Implementaiton
// ==============
// The asynchronous FindFirstL function builds up a list of
// CImOffLineOperationArray objects, each array holds a list of operations
// of a particular type.

// The operations are stored this way to facilitate easy ordering based on
// operation type as well as being useful for generating progress
// information.

// The operations are retrieved by first visiting each message under the
// service, this potentially slow operation is handled by the asynchronous
// function.

CImPop3OfflineOperationFinder* CImPop3OfflineOperationFinder::NewL(CMsvServerEntry& aEntry)
	{
	CImPop3OfflineOperationFinder* self = NewLC(aEntry);
	CleanupStack::Pop(); // self
	return self;
	}

CImPop3OfflineOperationFinder* CImPop3OfflineOperationFinder::NewLC(CMsvServerEntry& aEntry)
	{
	CImPop3OfflineOperationFinder* self = new (ELeave) CImPop3OfflineOperationFinder(aEntry);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CImPop3OfflineOperationFinder::FindFirstL(TMsvId aServiceId, TBool aQuitting, TRequestStatus &aStatus)
// Asynchronous function for finding all the offline operations and setting
// the current operation (iOperationArrayIndex and iOperationIndex) to point
// to the first one.

// Use OperationFound(), OfflineOperation() and OperationDetails() to access
// the results of this search.
	{
	TInt index = 0;
	// Delete each of the CImOffLineOperationArrays
	while (index < iOfflineOperations->Count())
		{
		delete (*iOfflineOperations)[index];
		index++;
		}
	iOfflineOperations->Reset();

	iServiceId = aServiceId;
	iState = EPopsOffOpLookingForMessages;
	iQuitting = aQuitting;

	if (iChildMessages)
		{
		delete iChildMessages;
		iChildMessages = 0;
		}

	iChildMessages = new (ELeave) CMsvEntrySelection();

	User::LeaveIfError(iEntry.GetChildren(*iChildMessages));
	iMessageIndex = 0;
	Queue(aStatus);
	if (iChildMessages->Count())
		{
		// Check the first message for offline operations.
		CheckNextChildMessageL();
		}
	else
		{
		// There are no messages found so complete and stop searching here.
		delete iChildMessages;
		iChildMessages = 0;
		Complete(KErrNone);
		}
	}

void CImPop3OfflineOperationFinder::CheckNextChildMessageL()
// Check the current message (iMessageIndex) and append any offline operations
// to the offline operations list.
	{
	if (iMessageIndex < iChildMessages->Count())
		{
		iEntry.SetEntry((*iChildMessages)[iMessageIndex]);
		TMsvEmailEntry entry = iEntry.Entry();
		if (entry.DisconnectedOperation() != ENoDisconnectedOperations)
			// If the entry has offline operations queued then add it to the list.
			{
			AppendOfflineOpsL();
			}
		}

	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}

void CImPop3OfflineOperationFinder::AppendOfflineOpsL()
// This private help function appends the offline operations associated with the
// current message to the appropriate offline operations array.
// The array to which it is added depends on the operation type. If there
// is currently no array for this operation type then one is created.
	{
	// Get the offline operations for the current message.
	CImOffLineOperationArray* localOfflineOpArray = CImOffLineOperationArray::NewL();
	CleanupStack::PushL(localOfflineOpArray);
	CImOffLineArrayStore* offlineArrayStore = new (ELeave) CImOffLineArrayStore(*localOfflineOpArray);
	CleanupStack::PushL(offlineArrayStore);
	CMsvStore* messageStore = iEntry.ReadStoreL();
	CleanupStack::PushL(messageStore);
	offlineArrayStore->RestoreL(*messageStore);
	
	// Append the local offline operations to the correct operations list.
	TInt opCounter;
	TInt offlineOps = localOfflineOpArray->CountOperations();
	const CImOffLineOperation* newOperation;
	for (opCounter = 0; opCounter < offlineOps; opCounter++)
		{
		newOperation = &localOfflineOpArray->Operation(opCounter);
		// Append newOperation to the correct list as each list holds a different operation type.

		// Find the appropriate offline operation list.
		CImOffLineOperationArray* operationArray = 0;
		if (iOfflineOperations->Count() == 0)
			{
			// If there are no arrays then create the first one.
			operationArray = CImOffLineOperationArray::NewL();
			CleanupStack::PushL(operationArray);
			iOfflineOperations->AppendL(operationArray, sizeof(void*));
			CleanupStack::Pop(operationArray);
			}
		else
			{
			TBool found = EFalse;
			TInt index = 0;
			// Search the different arrays for one that contains the correct operation types.
			while ((!found) && (index < iOfflineOperations->Count()))
				{
				if ((*iOfflineOperations)[index]->CountOperations())
					{
					if (((*iOfflineOperations)[index]->Operation(0)).OpType() == newOperation->OpType())
						{
						found = ETrue;
						operationArray = (*iOfflineOperations)[index];
						}
					}
				index++;
				}

			if (!found)
				{
				// If there are no appropriate arrays then create a new one.
				operationArray = CImOffLineOperationArray::NewL();
				CleanupStack::PushL(operationArray);
				iOfflineOperations->AppendL(operationArray, sizeof(void*));
				CleanupStack::Pop(operationArray);
				}
			}

		operationArray->AppendOperationL(*newOperation);
		}

	CleanupStack::PopAndDestroy(3); // offlineArrayStore, messageStore, localOfflineOperationArray
	}


TBool CImPop3OfflineOperationFinder::AppropriateOperation(const CImOffLineOperation& aOperation) const
	{
// Returns true if the operation should be run at this point.
// Delete operations should be run at the end of the session, all others at the start.
	return (aOperation.OpType()==CImOffLineOperation::EOffLineOpDelete) ? iQuitting : (!iQuitting);
	}


void CImPop3OfflineOperationFinder::FindNext()
// Finds the next offline operations.
// The results of this function call are valid as soon as the function returns.
	{
	TBool go = ETrue;

	CImOffLineOperationArray* operationArray = 0;
	iOperationIndex++;
	while (go)
		{
		go = ETrue;

		if (iOfflineOperations->Count())
			{
			operationArray = (*iOfflineOperations)[iOperationArrayIndex];
			}
		else
			{
			operationArray = 0;
			}

		if (operationArray == 0)
			{
			go = EFalse;
			}
		else if (iOperationIndex == operationArray->CountOperations())
		// If there are no more operations in this array then move on to the next one.
			{
			iOperationArrayIndex++;
			iOperationIndex = 0;
			}
		else if (!AppropriateOperation(operationArray->Operation(0)))
		// If the operations in this array aren't of an appropriate type then move on to the next one.
			{
			iOperationArrayIndex++;
			iOperationIndex = 0;
			}
		else
		// An appropriate operation has been found.
			{
			go = EFalse;
			}

		if (iOperationArrayIndex == iOfflineOperations->Count())
		// If there are no more operation arrays then don't carry on looking.
			{
			go = EFalse;
			}
		}

	// Set the operaitons details for later.
	if (OperationFound())
		{
		iOperationDetails.iOpType = OfflineOperation().OpType();
		iOperationDetails.iOperationNumber = iOperationIndex;
		iOperationDetails.iOperationsOfType = ((*iOfflineOperations)[iOperationArrayIndex])->CountOperations();
		}
	else
		{
		iOperationDetails.iOpType = CImOffLineOperation::EOffLineOpNone;
		}
	}

TBool CImPop3OfflineOperationFinder::OperationFound() const
// Returns true if an operation has been found.
	{
	return (iOperationArrayIndex < (iOfflineOperations->Count()));
	}

const CImOffLineOperation& CImPop3OfflineOperationFinder::OfflineOperation() const
// Returns the current offline operations.
	{
	return ((*iOfflineOperations)[iOperationArrayIndex])->Operation(iOperationIndex);
	}

const CImPop3OfflineOperationFinder::TOperationDetails& CImPop3OfflineOperationFinder::OperationDetails() const
// Returns the details for the current operation.
	{
	return iOperationDetails;
	}

CImPop3OfflineOperationFinder::~CImPop3OfflineOperationFinder()
	{
	delete iChildMessages;
	TInt index = 0;
	if (iOfflineOperations)
		{
		// Delete each of the CImOffLineOperationArrays
		while (index < iOfflineOperations->Count())
			{
			delete (*iOfflineOperations)[index];
			index++;
			}
		delete iOfflineOperations;
		}
	}

void CImPop3OfflineOperationFinder::DoRunL()
	{
	++iMessageIndex;
	if (iMessageIndex < iChildMessages->Count())
		// If there are more messages then check them for offline operations.
		{
		CheckNextChildMessageL();
		}
	else
		// If there are no more messages then go back and find all the offline operations.
		{
		delete iChildMessages;
		iChildMessages = 0;

		// Move the offline copy opertion array to the front of the list.
		TInt index = 0;
		TBool go = ETrue;
		while ((index < iOfflineOperations->Count())
				&& go)
			{
			CImOffLineOperation::TOffLineOpType opType = ((*iOfflineOperations)[index]->Operation(0)).OpType();
			if (opType == CImOffLineOperation::EOffLineOpCopyToLocal)
				{
				// Move the offline copy operation array to the front of the list.
				go = EFalse;
				CImOffLineOperationArray* populateOperationList = (*iOfflineOperations)[index];
				iOfflineOperations->Delete(index);
				iOfflineOperations->InsertL(index, populateOperationList, sizeof(void*));
				}
			else
				{
				index++;
				}
			}

		// Move the offline populate opertion array to the front of the list.
		index = 0;
		go = ETrue;
		while ((index < iOfflineOperations->Count())
				&& go)
			{
			CImOffLineOperation::TOffLineOpType opType = ((*iOfflineOperations)[index]->Operation(0)).OpType();
			if (opType == CImOffLineOperation::EOffLineOpCopyWithinService)
				{
				// Move the offline populate operation array to the front of the list.
				go = EFalse;
				CImOffLineOperationArray* populateOperationList = (*iOfflineOperations)[index];
				iOfflineOperations->Delete(index);
				iOfflineOperations->InsertL(index, populateOperationList, sizeof(void*));
				}
			else
				{
				index++;
				}
			}

		// Set the iOperationIndex to the first appropriate operation.
		iOperationArrayIndex = 0;
		iOperationIndex = -1;
		FindNext();

		Complete(KErrNone);
		}
	}

void CImPop3OfflineOperationFinder::DoCancel()
	{
	CMsgActive::DoCancel();
	}

void CImPop3OfflineOperationFinder::DoComplete()
	{

	}

void CImPop3OfflineOperationFinder::ConstructL()
	{
	CActiveScheduler::Add(this);
	iOfflineOperations = new (ELeave) CArrayVarFlat<CImOffLineOperationArray*>(3);
	}

CImPop3OfflineOperationFinder::CImPop3OfflineOperationFinder(CMsvServerEntry& aEntry) : CMsgActive(EPriorityStandard), iEntry(aEntry)
	{

	}






// API
// ===
// The CImPop3SetOfflineOps class provides a means of asynchronously adding
// offline operations to the message stores of the appropriate messages.

// Use the AddOfflineOperationL to add the offline operations.


CImPop3SetOfflineOps* CImPop3SetOfflineOps::NewL(CMsvServerEntry& aEntry)
	{
	CImPop3SetOfflineOps* self = NewLC(aEntry);
	CleanupStack::Pop(); // self
	return self;
	}

CImPop3SetOfflineOps* CImPop3SetOfflineOps::NewLC(CMsvServerEntry& aEntry)
	{
	CImPop3SetOfflineOps* self = new (ELeave) CImPop3SetOfflineOps(aEntry);
	CleanupStack::PushL(self);
	self->ConstructL();
	CActiveScheduler::Add(self);
	//	ConstructL();
	return self;
	}

void CImPop3SetOfflineOps::AddOfflineOperationL(const CMsvEntrySelection *aMessageSelection,
							CImOffLineOperation::TOffLineOpType aOperationType,
							TMsvId aTargetFolderId,
							TRequestStatus& aStatus)
// Adds the aOperationType operation to each message in aMessageSelection.
// The aTargetFolderId is not used for some operation types, eg. deleting.
	{
	iState = EAddingOfflineOperations;

	iMessageCounter = 0;
	iOperationType = aOperationType;
	iDestinationEntryId = aTargetFolderId;

	delete iSourceMessages;
	iSourceMessages = 0;

	delete iMessagesToCopyLocally;
	iMessagesToCopyLocally = new (ELeave) CMsvEntrySelection;

	iSourceMessages = aMessageSelection->CopyL();
	
	TInt count = iSourceMessages->Count();
	iOfflineOperationArrayFlag.Reset();
	
	for(TInt i=0; i< count; ++i )
		{
		iOfflineOperationArrayFlag.AppendL(EFalse);	
		}
	
	
	Queue(aStatus);

	AddOfflineOperationL();
	}


void CImPop3SetOfflineOps::CancelOfflineOperationsL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus)
// Cancel all pending operations on each message in aSelection.
	{
	iState = ECancellingOfflineOperations;
	iMessageCounter = 0;
	
	delete iSourceMessages;
	iSourceMessages = 0;

	iSourceMessages = aSelection.CopyL();
	Queue(aStatus);

	CancelOfflineOperationL();
	}

// Resumes a cancelled "Cancel Offline Operations" operation.
void CImPop3SetOfflineOps::ResumeCancelOfflineOperationsL(TRequestStatus& aStatus)
	{
	Queue(aStatus);
	CancelOfflineOperationL();
	}

void CImPop3SetOfflineOps::AddOfflineOperationL()
	{
	// Set the current entry, ready for the operation to be attached to it.
	User::LeaveIfError(iEntry.SetEntry((*iSourceMessages)[iMessageCounter]));

	iModifiedOperationType = iOperationType;

	if ((iEntry.Entry()).Complete())
	// Any messages that have already been populated can be copied locally without
	// the need to be online.
	// This is done after any offline operations have been added but they are
	// saved for later here:
		{
		if (iOperationType == CImOffLineOperation::EOffLineOpCopyToLocal)
			{
			iMessagesToCopyLocally->AppendL(iEntry.Entry().Id());
			iModifiedOperationType = CImOffLineOperation::EOffLineOpNone;
			}

		if (iOperationType == CImOffLineOperation::EOffLineOpMoveToLocal)
			{
			iMessagesToCopyLocally->AppendL(iEntry.Entry().Id());
			iModifiedOperationType = CImOffLineOperation::EOffLineOpDelete;
			}
		}

	if (iModifiedOperationType == CImOffLineOperation::EOffLineOpNone)
		{
		iStatus = KRequestPending;
		SetActive();
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		return;
		}

	CImOffLineOperationArray* operationArray = CImOffLineOperationArray::NewL();
	CleanupStack::PushL(operationArray);
	CImOffLineArrayStore* operationStore = new (ELeave) CImOffLineArrayStore(*operationArray);
	CleanupStack::PushL(operationStore);
	CMsvStore* messageStore = iEntry.EditStoreL();
	CleanupStack::PushL(messageStore);

	// Get the array of operations currently attached to the message.
	operationStore->RestoreL(*messageStore);

	// If the operation is a move or delete then ensure that it doesn't interfere with any existing operations.
	if ((iModifiedOperationType == CImOffLineOperation::EOffLineOpMoveToLocal)
		|| (iModifiedOperationType == CImOffLineOperation::EOffLineOpDelete))
		{
		// Find out if there is a move or a delete operation already pending on the message.
		TBool moveOrDeleteAlreadyPending = EFalse;
		CImOffLineOperation::TOffLineOpType opType;
		TInt counter = operationArray->CountOperations();

		while (counter > 0)
			{
			counter--;
			opType = (operationArray->Operation(counter)).OpType();
			if ((opType == CImOffLineOperation::EOffLineOpMoveToLocal)
				|| (opType == CImOffLineOperation::EOffLineOpDelete))
				{
				moveOrDeleteAlreadyPending = ETrue;
				counter = 0;
				}
			}

		if (moveOrDeleteAlreadyPending)
			{
			if (iModifiedOperationType == CImOffLineOperation::EOffLineOpMoveToLocal)
				{
				// Change the move operation to a copy, it will be deleted by the existing move or delete.
				iModifiedOperationType = CImOffLineOperation::EOffLineOpCopyToLocal;
				}

			if (iModifiedOperationType == CImOffLineOperation::EOffLineOpDelete)
				{
				// No need to append this delete operation as it will be delete be the existing move or delete.
				iModifiedOperationType = CImOffLineOperation::EOffLineOpNone;
				}
			}
		}

	CImOffLineOperation* operation = new (ELeave) CImOffLineOperation();
	CleanupStack::PushL(operation);

	TMsvEmailEntry entry = iEntry.Entry();

	switch (iModifiedOperationType)
		{
		case CImOffLineOperation::EOffLineOpCopyFromLocal:
		case CImOffLineOperation::EOffLineOpMoveFromLocal:
		case CImOffLineOperation::EOffLineOpMoveWithinService:
		case CImOffLineOperation::EOffLineOpChange:
		case CImOffLineOperation::EOffLineOpCreate:
		case CImOffLineOperation::EOffLineOpMtmSpecific:
			User::Leave(KErrNotSupported);

		case CImOffLineOperation::EOffLineOpCopyToLocal:
			operation->SetCopyToLocal((*iSourceMessages)[iMessageCounter], iDestinationEntryId);
			break;

		case CImOffLineOperation::EOffLineOpDelete:
			operation->SetDelete((*iSourceMessages)[iMessageCounter]);
			break;

		case CImOffLineOperation::EOffLineOpCopyWithinService:
			operation->SetCopyWithinService((*iSourceMessages)[iMessageCounter], iDestinationEntryId);
			break;

		case CImOffLineOperation::EOffLineOpMoveToLocal:
			operation->SetMoveToLocal((*iSourceMessages)[iMessageCounter], iDestinationEntryId);
			break;

		case CImOffLineOperation::EOffLineOpNone:
			break;

		default:
			User::Leave(KErrNotSupported);
		}
	

	if (iModifiedOperationType != CImOffLineOperation::EOffLineOpNone)
		{
		operationArray->AppendOperationL(*operation);
		entry.SetOperation(ETrue);
		iOfflineOperationArrayFlag[iMessageCounter] = ETrue;
		}

	CImPop3OfflineUtilities::SetOfflineFlags(*operationArray, entry);

	operationStore->StoreL(*messageStore);
	iEntry.ChangeEntry(entry);
	messageStore->CommitL();
	CleanupStack::PopAndDestroy(4); // operation, messageStore, operationStore, operationArray
	iStatus = KRequestPending;
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}

void CImPop3SetOfflineOps::CancelOfflineOperationL()
	{
	SetActive();

	// Set the current entry, ready for the operation to be attached to it.
	User::LeaveIfError(iEntry.SetEntry((*iSourceMessages)[iMessageCounter]));
	TMsvEmailEntry entry = iEntry.Entry();
	CImOffLineOperationArray* operationArray = CImOffLineOperationArray::NewL();
	CleanupStack::PushL(operationArray);
	CImOffLineArrayStore* operationStore = new (ELeave) CImOffLineArrayStore(*operationArray);
	CleanupStack::PushL(operationStore);
	CMsvStore* messageStore = iEntry.EditStoreL();
	CleanupStack::PushL(messageStore);

	// Get the array of operations currently attached to the message.
	operationStore->RestoreL(*messageStore);

	TInt counter = operationArray->CountOperations();
	while (counter > 0)
		{
		counter--;
		operationArray->Delete(counter);
		}

	operationStore->StoreL(*messageStore);
	messageStore->CommitL();
	entry.SetOperation(EFalse);
	entry.SetDisconnectedOperation(ENoDisconnectedOperations);
	iEntry.ChangeEntry(entry);
	CleanupStack::PopAndDestroy(3); // messageStore, operationStore, operationArray
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}

void CImPop3SetOfflineOps::CopyLocalMessageL()
	// Copy the current local message.
	{
	User::LeaveIfError(iEntry.SetEntry((*iSourceMessages)[iMessageCounter]));
	User::LeaveIfError(iEntry.SetEntry(iEntry.Entry().Parent()));

	iStatus = KRequestPending;
	iTransfer->StartL((*iSourceMessages)[iMessageCounter], iDestinationEntryId, CImPop3TransferMessage::EImPop3CopyTransfer, iStatus);

	SetActive();
	}

CImPop3SetOfflineOps::~CImPop3SetOfflineOps()
	{
	Cancel();
	delete iSourceMessages;
	delete iMessagesToCopyLocally;
	delete iTransfer;
	iOfflineOperationArrayFlag.Close();
	}

void CImPop3SetOfflineOps::Progress(TPop3Progress& rPop3Progress) const
	{
	// It is only necessary to report the progress when it is in the
	// ECopyingLocalEntries state. All other states should be near-instantaneous
	// and a 0 filled progress will do.

	switch (iState)
		{
		case ECopyingLocalEntries:
 			if (iOperationType == CImOffLineOperation::EOffLineOpMoveToLocal)
 				{
 				rPop3Progress.iPop3Progress = TPop3Progress::EPopMoving;
 				rPop3Progress.iPop3SubStateProgress = TPop3Progress::EPopMoving;
 				}
 			else
  				{
 				rPop3Progress.iPop3Progress = TPop3Progress::EPopCopying;
 				rPop3Progress.iPop3SubStateProgress = TPop3Progress::EPopCopying;
 				}
			rPop3Progress.iTotalMsgs = iMessagesToCopyLocally->Count();
			rPop3Progress.iMsgsToProcess = rPop3Progress.iTotalMsgs - iMessageCounter;
			rPop3Progress.iBytesDone = 0;
			rPop3Progress.iTotalBytes = 0;
			rPop3Progress.iErrorCode = 0;
			break;

		case EAddingOfflineOperations:
			if (iOperationType == CImOffLineOperation::EOffLineOpDelete)
				{
				rPop3Progress.iPop3Progress = TPop3Progress::EPopDeleting;
				rPop3Progress.iTotalMsgs = iSourceMessages->Count();
				rPop3Progress.iMsgsToProcess = rPop3Progress.iTotalMsgs - iMessageCounter;
				rPop3Progress.iBytesDone = 0;
				rPop3Progress.iTotalBytes = 0;
				rPop3Progress.iErrorCode = 0;
				rPop3Progress.iPop3SubStateProgress = TPop3Progress::EPopDeleting;
				}
  			else if (iOperationType == CImOffLineOperation::EOffLineOpMoveToLocal)
 				{
 				rPop3Progress.iPop3Progress = TPop3Progress::EPopMoving;
 				rPop3Progress.iTotalMsgs = iSourceMessages->Count();
 				rPop3Progress.iMsgsToProcess = rPop3Progress.iTotalMsgs - iMessageCounter;
 				rPop3Progress.iBytesDone = 0;
				rPop3Progress.iTotalBytes = 0;
 				if(rPop3Progress.iErrorCode != KErrDisconnected)
 					{
 					rPop3Progress.iErrorCode = 0;
 					}
 				rPop3Progress.iPop3SubStateProgress = TPop3Progress::EPopMoving;
 				}
  			else if ((iOperationType == CImOffLineOperation::EOffLineOpCopyToLocal) ||
 				(iOperationType == CImOffLineOperation::EOffLineOpCopyWithinService))
 				{
 				rPop3Progress.iPop3Progress = TPop3Progress::EPopCopying;
 				rPop3Progress.iTotalMsgs = iSourceMessages->Count();
 				rPop3Progress.iMsgsToProcess = rPop3Progress.iTotalMsgs - iMessageCounter;
 				rPop3Progress.iBytesDone = 0;
 				rPop3Progress.iTotalBytes = 0;
 				if(rPop3Progress.iErrorCode  != KErrDisconnected)
 					{
 					rPop3Progress.iErrorCode = 0; 					
 					}
 				rPop3Progress.iPop3SubStateProgress = TPop3Progress::EPopCopying;
 				}
			break;

		case ECancellingOfflineOperations:
			rPop3Progress.iPop3Progress = TPop3Progress::EPopCancellingOfflineOps;
			rPop3Progress.iTotalMsgs = iSourceMessages->Count();
			rPop3Progress.iMsgsToProcess = rPop3Progress.iTotalMsgs - iMessageCounter;
			rPop3Progress.iBytesDone = 0;
			rPop3Progress.iTotalBytes = 0;
			rPop3Progress.iErrorCode = 0;
			rPop3Progress.iPop3SubStateProgress = TPop3Progress::EPopCancellingOfflineOps;
			break;

		default:
            {
            _LIT(KPop3Session,"Pop3 session");
			__ASSERT_DEBUG(EFalse, 	User::Panic(KPop3Session, EPopInvalidState));
            }
			break;
		}
	}

//private:
void CImPop3SetOfflineOps::DoRunL()
	{
	// Create the ghost destination entry if one is required.
	switch (iState)
		{
		case EAddingOfflineOperations:
			if ((iModifiedOperationType == CImOffLineOperation::EOffLineOpCopyToLocal)
				|| (iModifiedOperationType == CImOffLineOperation::EOffLineOpMoveToLocal))
				{
				User::LeaveIfError(iEntry.SetEntry((*iSourceMessages)[iMessageCounter]));
				TMsvEntry ghostEntry = iEntry.Entry();
				ghostEntry.iRelatedId = iEntry.Entry().Id();
				ghostEntry.SetComplete(EFalse);
				User::LeaveIfError(iEntry.SetEntry(iDestinationEntryId));
				iEntry.CreateEntry(ghostEntry);
				}

			iMessageCounter++;
			if (iMessageCounter < iSourceMessages->Count())
				{
				AddOfflineOperationL();
				}
			else
				{
				iMessageCounter = 0;
				if (iMessageCounter < iMessagesToCopyLocally->Count())
					{
					iState = ECopyingLocalEntries;
					CopyLocalMessageL();
					}
				}
			break;

		case ECancellingOfflineOperations:
			iMessageCounter++;
			if (iMessageCounter < iSourceMessages->Count())
				{
				CancelOfflineOperationL();
				}
			break;

		case ECopyingLocalEntries:
			iMessageCounter++;
			if (iMessageCounter < iMessagesToCopyLocally->Count())
				{
				CopyLocalMessageL();
				}
			break;
		}
	}

void CImPop3SetOfflineOps::DoCancel()
	{
	iTransfer->Cancel();
	
		switch(iState)
			{
			case ECopyingLocalEntries:
				{
				TInt messageCounter = iMessageCounter;
				
				// Loop to delete all the operations attached to the messages in iSourceMessages.
				for(TInt i = iSourceMessages->Count() - messageCounter; i > 0; -- i)
					{
					DeleteOfflineOperationL(messageCounter);
					messageCounter++;
					}
				break;
				}
			
			case EAddingOfflineOperations:
				{
				// Loop to delete all the operations attached to the messages in iSourceMessages.
				for(TInt count = 0; count <= iMessageCounter; ++count)
					{
					DeleteOfflineOperationL(count);
					}
				break;
				}
			
			case ECancellingOfflineOperations:
			default:
				{
				// Nothing to be done here.
				break;
				}
			}
		iOfflineOperationArrayFlag.Reset();
		CMsgActive::DoCancel();
	}


void CImPop3SetOfflineOps::DeleteOfflineOperationL(TInt aMessageCounter)
	{
	// Set the current entry, ready for the operation to be attached to it.
	User::LeaveIfError(iEntry.SetEntry((*iSourceMessages)[aMessageCounter]));
	TMsvEmailEntry entry = iEntry.Entry();
	
	if(iOfflineOperationArrayFlag[aMessageCounter])
		{
		CImOffLineOperationArray* operationArray = CImOffLineOperationArray::NewL();
		CleanupStack::PushL(operationArray);
		CImOffLineArrayStore* operationStore = new (ELeave) CImOffLineArrayStore(*operationArray);
		CleanupStack::PushL(operationStore);
		CMsvStore* messageStore = iEntry.EditStoreL();
		CleanupStack::PushL(messageStore);

		// Get the array of operations currently attached to the message.
		operationStore->RestoreL(*messageStore);

		TInt counter = operationArray->CountOperations();
		if (counter > 0)
			{
			counter--;
			operationArray->Delete(counter);
			}
		
		operationStore->StoreL(*messageStore);
		messageStore->CommitL();
		
		if(counter == 0)
			{
			entry.SetOperation(EFalse);
			entry.SetDisconnectedOperation(ENoDisconnectedOperations);	
			}
		else
			{
			CImPop3OfflineUtilities::SetOfflineFlags(*operationArray, entry);		
			}
		iEntry.ChangeEntry(entry);
		CleanupStack::PopAndDestroy(3); // messageStore, operationStore, operationArray	
		}
	}
	

void CImPop3SetOfflineOps::DoComplete(TInt& /*status*/)
	{

	}

CImPop3SetOfflineOps::CImPop3SetOfflineOps(CMsvServerEntry& aEntry) : CMsgActive(EPriorityStandard), iEntry(aEntry)
	{

	}

void CImPop3SetOfflineOps::ConstructL()
	{
	iTransfer = CImPop3TransferMessage::NewL(iEntry);
	}



void CImPop3OfflineUtilities::DeleteL(const CImOffLineOperation& aOperation, CMsvServerEntry& aEntry)
// Finds the specified operation in the message tree and then deletes it.
	{
	aEntry.SetEntry(aOperation.MessageId());
	CImOffLineOperationArray* localOperationArray = CImOffLineOperationArray::NewL();
	CleanupStack::PushL(localOperationArray);
	CImOffLineArrayStore* operationStore = new (ELeave) CImOffLineArrayStore(*localOperationArray);
	CleanupStack::PushL(operationStore);
	CMsvStore* messageStore = aEntry.EditStoreL();
	CleanupStack::PushL(messageStore);

	// Get the array of operations currently attached to the message.
	operationStore->RestoreL(*messageStore);

	// Look for the offline operation
	TBool found = EFalse;
	TInt index = 0;
	while ((!found) && (index < localOperationArray->CountOperations()))
		{
		if (aOperation == localOperationArray->Operation(index))
			{
			found = ETrue;
			}
		else
			{
			index++;
			}
		}

	if (found)
		{
		// Remove the entry from the list
		localOperationArray->Delete(index);

		// Write out the list to the stream
		operationStore->StoreL(*messageStore);
		messageStore->CommitL();

		// Set the TMsvEmailEntry details
		TMsvEmailEntry entry = aEntry.Entry();
		SetOfflineFlags(*localOperationArray, entry);
		aEntry.ChangeEntry(entry);
		}

	CleanupStack::PopAndDestroy(3); // messageStore, operationStore, operationArray
	}

void CImPop3OfflineUtilities::SetOfflineFlags(const CImOffLineOperationArray& aOperationArray, TMsvEmailEntry& aEntry)
	{
	// Sets the DisconnectedOperation flag on the given TMsvEmailEntry.
	// Note that it is NOT commited to the index.
	TImDisconnectedOperationType operationType = ENoDisconnectedOperations;
	TInt index = 0;
	while (index < aOperationArray.CountOperations())
		{
		if (operationType != ENoDisconnectedOperations)
			{
			operationType = EDisconnectedMultipleOperation;
			}
		else
			{
			switch (aOperationArray.Operation(index).OpType())
				{
				case CImOffLineOperation::EOffLineOpCopyToLocal:
					operationType = EDisconnectedCopyToOperation;
					break;
				case CImOffLineOperation::EOffLineOpCopyFromLocal:
					operationType = EDisconnectedCopyFromOperation;
					break;
				case CImOffLineOperation::EOffLineOpCopyWithinService:
					operationType = EDisconnectedCopyWithinServiceOperation;
					break;
				case CImOffLineOperation::EOffLineOpMoveToLocal:
					operationType = EDisconnectedMoveToOperation;
					break;
				case CImOffLineOperation::EOffLineOpMoveFromLocal:
					operationType = EDisconnectedMoveFromOperation;
					break;
				case CImOffLineOperation::EOffLineOpMoveWithinService:
					operationType = EDisconnectedMoveWithinServiceOperation;
					break;
				case CImOffLineOperation::EOffLineOpDelete:
					operationType = EDisconnectedDeleteOperation;
					break;
				case CImOffLineOperation::EOffLineOpChange:
					operationType = EDisconnectedChangeOperation;
					break;
				case CImOffLineOperation::EOffLineOpCreate:
					operationType = EDisconnectedCreateOperation;
					break;
				case CImOffLineOperation::EOffLineOpMtmSpecific:
					operationType = EDisconnectedSpecialOperation;
					break;
				default:
					operationType = EDisconnectedUnknownOperation;
					break;
				}
			}
		index++;
		}
	aEntry.SetDisconnectedOperation(operationType);
	}


