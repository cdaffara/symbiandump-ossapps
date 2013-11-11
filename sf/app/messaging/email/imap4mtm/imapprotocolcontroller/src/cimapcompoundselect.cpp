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

#include "cimapcompoundselect.h"
#include "cimapfolder.h"
#include "cimapsession.h"
#include "cimapsessionconsts.h"
#include "cimaplogger.h"
#include "imappaniccodes.h"

CImapCompoundSelect::CImapCompoundSelect( CImapSyncManager& aSyncManager,
						 				  CMsvServerEntry& aServerEntry,
						 				  CImapSettings& aImapSettings, 
						 				  TMsvId aFolderId  )
 : CImapCompoundBase( aSyncManager, aServerEntry, aImapSettings ),
 	iFolderId(aFolderId)
	{
	}

CImapCompoundSelect::~CImapCompoundSelect( )
	{
	}
	
CImapCompoundSelect* CImapCompoundSelect::NewL( CImapSyncManager& aSyncManager,
						 				  		CMsvServerEntry& aServerEntry,
						 				  		CImapSettings& aImapSettings,
						 				  		TMsvId aFolderId )
	{
	CImapCompoundSelect* self = new ( ELeave ) CImapCompoundSelect( aSyncManager,
																	aServerEntry,
																	aImapSettings,
																	aFolderId );
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Secondary construction
*/	
void CImapCompoundSelect::ConstructL()
	{
	CActiveScheduler::Add(this);
	}	
	
void CImapCompoundSelect::StartOperation( TRequestStatus& aStatus, CImapSession& aSession)
	{
	iSession = &aSession;
	__LOG_TEXT(iSession->LogId(), "CImapCompoundSelect::StartOperation()");
	iNextStep = ESelectSourceMailboxRW;
	Queue(aStatus);
	CompleteSelf();
	}

/**
Handles the compound operation state machine

@return ETrue if compound operation is completed, 
		EFalse otherwise (will be called again, unless active)
*/
TBool CImapCompoundSelect::DoRunLoopL()
	{
	SetCurrentStep();
	switch (iCurrentStep)
		{
		case ESelectSourceMailboxRW: // asynchronous
			{
			SetEntryL(iFolderId);
			TMsvEmailEntry entry = iServerEntry.Entry();
			iImapFolder = iSyncManager.GetFolderL(iFolderId); 
			iNextStep = EFinished;
			if (iImapFolder==NULL || !entry.Mailbox())
				{
				// folder does not exist or is marked /noselect
				iProgressErrorCode = KErrNotSupported;
				return EFalse;
				}
			else
				{
				iImapFolder->SelectL(iStatus, *iSession);
				SetActive();
				}
			break;
			}
			
		case EFinished: // finished
			{
			__LOG_TEXT(iSession->LogId(), "CImapCompoundSelect::Completing OK");
			iProgressState = TImap4GenericProgress::EIdle;
			Complete(KErrNone);
			return ETrue;
			}
			
		default:
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ESelectCompoundUnexpectedState));
			// unexpected state - complete the request
			iProgressState = TImap4GenericProgress::EIdle;
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
void CImapCompoundSelect::DoCancel()
	{
	switch (iCurrentStep)
		{
		case ESelectSourceMailboxRW :
			{
			// outstanding request on the session.
			// Note that although select operations are performed on a folder they
			// actually pass our iStatus to the session, so that is why we cancel
			// the session for them.
			iSession->Cancel();
			break;
			}
		case EFinished :
			{
			// self-completed or no outstanding request.
			break;
			}
		case ESuspendedForMigrate:
		default :
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ESelectCompoundCancelUnexpectedState));
			break;
			}
		} // end switch (iCurrentStep)

	if (!iCancelForMigrate)
		{
		// genuine cancel - update progress
		iProgressErrorCode = KErrCancel;
		}
	CMsgActive::DoCancel();
	}

	
void CImapCompoundSelect::Progress(TImap4CompoundProgress& aCompoundProgress)
	{
	aCompoundProgress.iGenericProgress.iOperation = TImap4GenericProgress::ESelect;
	
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
TInt CImapCompoundSelect::ProcessNegativeServerResponse()
	{
	TInt err = iStatus.Int();
	switch (iCurrentStep)
		{
		case ESelectSourceMailboxRW:
			{
			if (err == KErrImapNo)
				{
				err = KErrNotFound;
				}
			break;
			}
		case ESuspendedForMigrate:
		case EFinished:
		default:
			{
			// positive error code not expected,
			// self-completed states or no outstanding request.
			TImapServerPanic::ImapPanic(TImapServerPanic::ESelectCompoundUnexpectedState);
			break;
			}
		} // end of switch (iCurrentStep)
	iProgressErrorCode = err;
	return KErrNone;
	}

/**
Resumes the operation following a migration.
*/
void CImapCompoundSelect::ResumeOperationL(TRequestStatus& aStatus, CImapSession& aSession)
	{
	iSession = &aSession;
	__LOG_TEXT(iSession->LogId(), "CImapCompoundSelect::Resuming");
	__ASSERT_DEBUG(iCurrentStep==ESuspendedForMigrate, TImapServerPanic::ImapPanic(TImapServerPanic::ESelectCompoundUnexpectedState));
	iStopForMigrate = EFalse;
	
	iNextStep = ESelectSourceMailboxRW;
	CompleteSelf();
	Queue(aStatus);
	}

