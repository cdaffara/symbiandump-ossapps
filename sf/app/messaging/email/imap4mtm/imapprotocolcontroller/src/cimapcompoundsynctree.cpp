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

#include "cimapcompoundsynctree.h"

#include "cimapfolder.h"
#include "cimapsession.h"
#include "cimaplogger.h"
#include "imappaniccodes.h"

#include "mobilitytestmtmapi.h"

CImapCompoundSyncTree::CImapCompoundSyncTree(CImapSyncManager& aSyncManager, CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings)
 : CImapCompoundBase(aSyncManager, aServerEntry, aImapSettings)
	{
	// Set default progress values here so that any progress request before
	// the operation is started returns valid information.
	iProgressState = TImap4GenericProgress::ESyncing;	
	iSyncProgressState = TImap4SyncProgress::ESyncFolderTree;
	}

CImapCompoundSyncTree::~CImapCompoundSyncTree()
	{
	}
	
CImapCompoundSyncTree* CImapCompoundSyncTree::NewL(CImapSyncManager& aSyncManager, CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings)
	{
	CImapCompoundSyncTree* self = new (ELeave) CImapCompoundSyncTree(aSyncManager, aServerEntry, aImapSettings);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CImapCompoundSyncTree::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

void CImapCompoundSyncTree::StartOperation(TRequestStatus& aStatus, CImapSession& aSession)
	{
	iSession = &aSession;
	__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncTree::StartOperation()");
	iNextStep = ESyncTree;
	Queue(aStatus);
	CompleteSelf();
	}
	
/**
Handles the compound operation state machine

@return ETrue if compound operation is completed, 
		EFalse otherwise (will be called again, unless active)
*/
TBool CImapCompoundSyncTree::DoRunLoopL()
	{
	SetCurrentStep();
	switch (iCurrentStep)
		{
		case ESyncTree:
			{
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapSyncFolderTree);
			
			// SyncManager does the tree synchronisation.
			iSyncManager.SynchroniseFolderTreeL(iStatus, *iSession);
			iNextStep = EFinished;
			SetActive();
			break;
			}
			
		case EFinished:
			{
			__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncTree::Completing OK");
			iProgressState = TImap4GenericProgress::EIdle;
			iSyncProgressState = TImap4SyncProgress::EIdle;
			Complete(KErrNone);
			return ETrue;
			}
			
		default:
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ESyncTreeCompoundUnexpectedState));
			// unexpected state - complete the request
			iProgressState = TImap4GenericProgress::EIdle;
			iSyncProgressState = TImap4SyncProgress::EIdle;
			return ETrue;
			}
		} // end of switch (iCurrentStep)

	return EFalse;
	}
	

/**
May be called in case of a genuine cancel or a cancel for migrate.
Following a genuine cancel, the compound operation will be deleted.
Following a cancel for migrate, the compound operation will be resumed,
so the iNextState is updated here to ensure the operation is
correctly restarted.

In either case, CMsgActive::DoCancel() is called to complete the
user request with KErrCancel.

Note that if the default CMsgActive::DoComplete() is overridden,
then that must also be modified to handle either case described above.
*/
void CImapCompoundSyncTree::DoCancel()
	{
	switch (iCurrentStep)
		{
		case ESyncTree:
			{
			// outstanding request on syncmanager
			iSyncManager.Cancel();
			iNextStep=ESyncTree;
			break;
			}
		case EFinished:
			{
			// self-completed or no outstanding request.
			break;
			}
		case ESuspendedForMigrate:
		default:
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ESyncTreeCompoundCancelUnexpectedState));
			break;		
			}
		} // end of switch (iCurrentStep)

	// always re-start after migration		
	iNextStep=ESelectSourceMailboxRW;

	if (!iCancelForMigrate)
		{
		// genuine cancel - update progress
		iProgressErrorCode = KErrCancel;
		}
	CMsgActive::DoCancel();	
	}
	
void CImapCompoundSyncTree::Progress(TImap4CompoundProgress& aCompoundProgress)
	{
	aCompoundProgress.iGenericProgress.iOperation = TImap4GenericProgress::ESync;
	aCompoundProgress.iGenericProgress.iState = iProgressState;

	iSyncManager.Progress(aCompoundProgress.iSyncProgress);
	aCompoundProgress.iSyncProgress.iState = iSyncProgressState;

	// Put error into progress buffer
	if( aCompoundProgress.iGenericProgress.iErrorCode == KErrNone )
		{
		aCompoundProgress.iGenericProgress.iErrorCode = iProgressErrorCode;
		}
	}

/**
Handles NO/BAD responses according to current step.
Negative server responses are not fatal - the error is saved in
the message currently being operated on and the state machine pushed
on to process the next message in the requested selection.

@return KErrNone if the error has been handled
		Completion error code otherwise.
*/
TInt CImapCompoundSyncTree::ProcessNegativeServerResponse()
	{
	TInt err = iStatus.Int();
	switch (iCurrentStep)
		{
		case ESyncTree:
			{
			// Just store the error code in the progress response.
			break;
			}
		case ESuspendedForMigrate:
		case EFinished:
		default:
			{
			// positive error code not expected,
			// self-completed states or no outstanding request.
			TImapServerPanic::ImapPanic(TImapServerPanic::ESyncTreeCompoundUnexpectedState);
			break;
			}
		} // end of switch (iCurrentStep)
	iProgressErrorCode = err;
	return KErrNone;
	}


/**
Resume the operation following a bearer migration.
*/
void CImapCompoundSyncTree::ResumeOperationL(TRequestStatus& aStatus, CImapSession& aSession)
	{
	iSession = &aSession;
	__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncTree::Resuming");
	__ASSERT_DEBUG(iCurrentStep==ESuspendedForMigrate, TImapServerPanic::ImapPanic(TImapServerPanic::ESyncTreeCompoundUnexpectedState));
	iStopForMigrate = EFalse;
	iNextStep = ESyncTree;
	Queue(aStatus);
	CompleteSelf();
	}
