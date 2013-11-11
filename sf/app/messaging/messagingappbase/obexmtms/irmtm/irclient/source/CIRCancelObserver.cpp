// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "CIRCancelObserver.h"

/**
Standard 2-phase constructor.
@param aObserverRequestStatus Request status of the observer to be completed when the operation completes.
@param aSession Handle to an opened session with the message server.
@param aEntryId The entry id for the message for the IR operation.
@return Pointer to the newly created object.
*/
CIRCancelObserver* CIRCancelObserver::NewL(TRequestStatus& aObserverRequestStatus, CMsvSession& aSession, TMsvId aEntryId)
	{
	CIRCancelObserver* me = new(ELeave) CIRCancelObserver(aObserverRequestStatus, aSession, aEntryId);
	return me;
	}

CIRCancelObserver::CIRCancelObserver(TRequestStatus& aObserverRequestStatus, CMsvSession& aSession, TMsvId aEntryId)
	:CMsvOperation(aSession, EPriorityStandard, aObserverRequestStatus), iEntryId(aEntryId)
	{
	CActiveScheduler::Add(this);
	iObserverRequestStatus = KRequestPending;
	SetActive();
	}

/**
Default destructor.
*/
CIRCancelObserver::~CIRCancelObserver()
	{
	Cancel();
	delete iMsvOperation;
	}

void CIRCancelObserver::RunL()
	{
	// The operation has completed, relay the status completion to the observer
	TRequestStatus* observerStatus = &iObserverRequestStatus;
	User::RequestComplete(observerStatus, iStatus.Int());
	}

TInt CIRCancelObserver::RunError(TInt /*aError*/)
	{
	// The RunL does not leave
	return KErrNone;
	}

void CIRCancelObserver::DoCancel()
	{
	// This is an explicit cancel from the user, cancel the operation and complete the client
	if( iMsvOperation!=NULL )
		iMsvOperation->Cancel();
	TRequestStatus* st = &iObserverRequestStatus;
	User::RequestComplete(st, KErrCancel);

	// If the operation is queued, clean up the entries
	if(!iOperationStarted)
		CleanupEntry();
	}
	
void CIRCancelObserver::CleanupEntryL() const
	{
	// Delete the entry
	CMsvEntry* entry = iMsvSession.GetEntryL(iEntryId);
	CleanupStack::PushL(entry);
	entry->DeleteL(iEntryId);
	CleanupStack::PopAndDestroy(entry);
	}

void CIRCancelObserver::CleanupEntry() const
	{
	// Call leaving version of CleanupEntry and ignore any errors
	TRAPD(err, CleanupEntryL());
	}

void CIRCancelObserver::SetOperation(CMsvOperation* aMsvOperation)
	{
	__ASSERT_DEBUG(iMsvOperation==NULL, User::Invariant());

	// Initialise the members now that we have the operation
	iMsvOperation = aMsvOperation;
	iMtm = iMsvOperation->Mtm();
	iService = iMsvOperation->Service();
	}

TRequestStatus& CIRCancelObserver::Status()
	{
	return iStatus;
	}

const TDesC8& CIRCancelObserver::ProgressL()
	{
	__ASSERT_DEBUG(iMsvOperation!=NULL, User::Invariant());

	// This call will leave if the operation is queued
	const TDesC8& progressDes = iMsvOperation->ProgressL();
	
	// Will only get to here when the operation has started so updated the flag if not already done
	if(!iOperationStarted)
		iOperationStarted = ETrue;

	return progressDes;

	}

const TDesC8& CIRCancelObserver::FinalProgress()
	{
	__ASSERT_DEBUG(iMsvOperation!=NULL, User::Invariant());

	return iMsvOperation->FinalProgress();
	}

