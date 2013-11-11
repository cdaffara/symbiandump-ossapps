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

#include "cimapcompoundsyncfolder.h"
#include "cimapfolder.h"
#include "cimapfolderinfo.h"
#include "cimapsession.h"
#include "cimapsessionconsts.h"
#include "cimaplogger.h"
#include "imappaniccodes.h"
#include "cimapcompoundcopytolocal.h"

#include "mobilitytestmtmapi.h"

CImapCompoundSyncFolder::CImapCompoundSyncFolder(CImapSyncManager& aSyncManager, CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings, CImapMailStore& aImapMailStore, TBool aNewOnly, TMsvId aFolderId)
 : CImapCompoundBase(aSyncManager, aServerEntry, aImapSettings), 
 	iImapMailStore(aImapMailStore), iNewOnly(aNewOnly), iFolderId(aFolderId)
	{
	// Set default progress values here so that any progress request before
	// the operation is started returns valid information.
	iSyncProgressState=TImap4SyncProgress::ESyncOther;
	}

CImapCompoundSyncFolder::~CImapCompoundSyncFolder()
	{
	delete iImapCompound;
	}
	
/**
Static constructor for Sync Folder operations

@param aSession      The IMAP Session to use
@param aSyncManager  IMAP Sync Manager
@param aServerEntry  CMsvServerEntry for messaging server operations
@param aImapSettings The IMAP account service settings
@param aNewOnly		 ETrue if only newly-received mail should be synchronised
@param aFolderId     Folder to synchronise (default KMsvNullIndexEntryId 
					   indicates the currently selected folder should be synchronised)
*/
CImapCompoundSyncFolder* CImapCompoundSyncFolder::NewL(CImapSyncManager& aSyncManager, CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings, CImapMailStore& aImapMailStore, TBool aNewOnly, TMsvId aFolderId/* = KMsvNullIndexEntryId */)
	{
	CImapCompoundSyncFolder* self = new (ELeave) CImapCompoundSyncFolder(aSyncManager, aServerEntry, aImapSettings, aImapMailStore, aNewOnly, aFolderId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CImapCompoundSyncFolder::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

void CImapCompoundSyncFolder::StartOperation(TRequestStatus& aStatus, CImapSession& aSession)
	{
	iSession = &aSession;
	__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncFolder::StartOperation()");
	iNextStep = ESelectSourceMailboxRW;
	Queue(aStatus);
	CompleteSelf();
	}

/**
Handles the compound operation state machine

@return ETrue if compound operation is completed, 
		EFalse otherwise (will be called again, unless active)
*/
TBool CImapCompoundSyncFolder::DoRunLoopL()
	{
	SetCurrentStep();
	switch (iCurrentStep)
		{
		case ESelectSourceMailboxRW:
			{
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapSyncFolder1);
						
			if(iFolderId==KMsvNullIndexEntryId)
				{
				// Get the selected folder
				if(iSession->SelectedFolderInfo())
					{
					iFolderId = iSession->SelectedFolderInfo()->MsvId();	
					}
				else
					{
					Complete(KErrArgument);
					return ETrue;
					}
				}
			
			iSyncFolder=iSyncManager.GetFolderL(iFolderId);
			if (iSyncFolder==NULL)
				{
				Complete(KErrNotSupported);
				return ETrue;
				}
			
			iSyncFolder->SelectL(iStatus, *iSession);
			iProgressState = TImap4GenericProgress::ESelecting;
			iNextStep=ESyncFolder;
			SetActive();
			break;
			}
		
		case ESyncFolder:
			{
			// suspend operation if stop for migrate has been requested
			if (iStopForMigrate)
				{
				__LOG_TEXT(iSession->LogId(), "CImapCompoundDelete::Stopped for migrate");
				iCurrentStep = ESuspendedForMigrate;
				iNextStep = ESelectSourceMailboxRW;
				Complete(KErrNone);
				return ETrue;
				}
			
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapSyncFolder2);

			// Synchronise the folder
			iSyncFolder->SynchroniseL(iStatus, *iSession, iNewOnly, ETrue);
			iProgressState = TImap4GenericProgress::ESyncing;
			
			// update next step according to download rule usage.
			iNextStep = EFinished;
			if (iImapSettings.UseSyncDownloadRules())
				{
				iNextStep = ESyncFolderAutoFetch;
				}
			SetActive();
			break;
			}
		
		case ESyncFolderAutoFetch:
			{
			// suspend operation if stop for migrate has been requested
			if (iStopForMigrate)
				{
				__LOG_TEXT(iSession->LogId(), "CImapCompoundDelete::Stopped for migrate");
				iCurrentStep = ESuspendedForMigrate;
				iNextStep = ESyncFolderAutoFetch;
				// delete the compound fetch operation - we will create a new one on resume,
				// using the download rules for the new bearer.
				delete iImapCompound;
				iImapCompound = NULL;
				Complete(KErrNone);
				return ETrue;
				}
			
			// Set the next step, which is always EFinished
			iNextStep = EFinished;

			// Get download rules defined for this folder type/bearer type
			TInt ruleId=0;
			TImImap4GetPartialMailInfo mailInfo;

			if (iFolderId == (iSyncManager.Inbox())->MailboxId())
				{
				ruleId = iImapSettings.GetSyncDownloadRuleL(CImapSyncDownloadRules::EInboxRulesType, mailInfo);
				}
			else
				{
				ruleId = iImapSettings.GetSyncDownloadRuleL(CImapSyncDownloadRules::EFolderRulesType, mailInfo);
				}

			// finish now if no download rules configured for this folder type/bearer type
			if (ruleId == KErrNotFound)
				{
				return EFalse;
				}
			
			// build a list of messages to be fetched...
			CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
			CleanupStack::PushL(selection);
			TInt msgsToFetch = iSyncFolder->GetFetchMessageChildrenL(*selection);
			__LOG_FORMAT((iSession->LogId(), "CImapCompoundSyncFolder::Fetching %d messages in folder %d.", msgsToFetch, iSyncFolder->MailboxId()));
			if (msgsToFetch>0)
				{
				delete iImapCompound;
				iImapCompound = NULL;
				// Create the compound operation object
				iImapCompound = CImapCompoundCopyToLocal::NewL(iSyncManager, 
				                    			               iServerEntry, 
				                                			   iImapSettings,
				                                			   iImapMailStore, 
				                                               EFalse,
				                                               *selection,
				                                               KMsvNullIndexEntryId,
				                                               mailInfo);
				iImapCompound->StartOperation(iStatus, *iSession);
				SetActive();
				}
			CleanupStack::PopAndDestroy(selection);
			return EFalse; // If not active, will re-loop.
			}
			
		case EFinished:
			{
			__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncFolder::Completing OK");
			iProgressState = TImap4GenericProgress::EIdle;
			iSyncProgressState = TImap4SyncProgress::EIdle;
			iSyncFolder = NULL;
			Complete(KErrNone);
			return ETrue;
			}
			
		default:
			{
			
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ESyncFolderCompoundUnexpectedState));
			// unexpected state - complete the request
			iProgressState = TImap4GenericProgress::EIdle;
			iSyncProgressState = TImap4SyncProgress::EIdle;
			return ETrue;
			}
		} // end of switch (iState)

	return EFalse;
	}
	
/**
May be called in case of a genuine cancel or a cancel for migrate.
Following a genuine cancel, the compound operation will be deleted.
If a second stage sync/auto fetch is in progress, this is cancelled
and the iImapCompound deleted. This will be re-created on resume,
using the download rules appropriate to the new bearer. 

iNextState is updated here to ensure the operation is
correctly restarted.

In either case, CMsgActive::DoCancel() is called to complete the
user request with KErrCancel.

Note that if the default CMsgActive::DoComplete() is overridden,
then that must also be modified to handle either case described above.
*/
void CImapCompoundSyncFolder::DoCancel()
	{
	switch (iCurrentStep)
		{
		case ESelectSourceMailboxRW:
			{
			// Outstanding request on session.
			// Note that although select operations are performed on a folder they
			// actually pass our iStatus to the session, so that is why we cancel
			// the session for them.
			iNextStep = ESelectSourceMailboxRW;
			iSession->Cancel();
			break;
			}
		case ESyncFolder:
			{
			// outstanding request on folder
			if (iSyncFolder != NULL)
				{
				iSyncFolder->Cancel();
				}
			iNextStep = ESelectSourceMailboxRW;
			break;
			}
		case ESyncFolderAutoFetch:
			{
			if (iImapCompound)
				{
				iImapCompound->Cancel();
				delete iImapCompound;
				iImapCompound = NULL;
				}
			// resume the autofetch with new bearer's download rules on reconnect.
			iNextStep = ESyncFolderAutoFetch;
			break;
			}
		case EFinished:
			{
			// self-completed or no outstanding request.
			iNextStep = EFinished;
			break;
			}
		case ESuspendedForMigrate:
		default:
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ESyncFolderCompoundCancelUnexpectedState));
			iNextStep = EFinished;
			break;	
			}
		} // end of switch (iCurrentStep)

	if (!iCancelForMigrate)
		{
		// genuine cancel - update progress
		iProgressErrorCode = KErrCancel;
		}
	CMsgActive::DoCancel();		
	}

void CImapCompoundSyncFolder::Progress(TImap4CompoundProgress& aCompoundProgress)
	{
	aCompoundProgress.iGenericProgress.iOperation = TImap4GenericProgress::ESync;
	aCompoundProgress.iGenericProgress.iState = iProgressState;

	//if currently syncing then get the latest progress
	if((iSyncProgressState!=TImap4SyncProgress::EIdle) && (iSyncFolder != NULL))
		{
		iSyncFolder->Progress(aCompoundProgress.iSyncProgress);
		}
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
TInt CImapCompoundSyncFolder::ProcessNegativeServerResponse()
	{
	TInt err = iStatus.Int();
	switch (iCurrentStep)
		{
		case ESelectSourceMailboxRW:
			{
			if (err == KErrImapNo)
				{
				err = KErrNotFound;
				iNextStep = EFinished;
				}
			break;
			}
		case ESyncFolder:
		case ESyncFolderAutoFetch:
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
			TImapServerPanic::ImapPanic(TImapServerPanic::ESyncFolderCompoundUnexpectedState);
			break;
			}
		} // end of switch (iCurrentStep)
	iProgressErrorCode = err;
	return KErrNone;
	}

/**
Called when operation has completed and we are about to complete the caller.

@param aErr Error code that we will complete with.
*/
void CImapCompoundSyncFolder::DoComplete(TInt& aErr)
	{
	// call base class for logging
	CImapCompoundBase::DoComplete(aErr);

	// Reset the flags which show if the new flags should be cleared on the
	// next sync.
	if (!iCancelForMigrate)
		{
		iSyncManager.ResetInboxClearNewFlags();
		iSyncManager.ResetNonInboxClearNewFlags();
		}
	}


/**
Resumes the operation following a migration.
*/
void CImapCompoundSyncFolder::ResumeOperationL(TRequestStatus& aStatus, CImapSession& aSession)
	{
	iSession = &aSession;
	__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncFolder::Resuming");
	__ASSERT_DEBUG(iCurrentStep==ESuspendedForMigrate, TImapServerPanic::ImapPanic(TImapServerPanic::ESyncFolderCompoundUnexpectedState));
	iStopForMigrate = EFalse;
	Queue(aStatus);
	CompleteSelf();
	}

/**
Indicates that the compound operation should complete as soon as it is possible
for the operation to be re-started without requiring a significant amount of
repeated communication.
The operation will be restarted by a call to ResumeOperationL()
Overrides the base-class implementation.
*/
void CImapCompoundSyncFolder::StopForMigrate()
	{
	__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncFolder::StopForMigrate()");
	if (iCurrentStep==ESyncFolderAutoFetch && iImapCompound)
		{
		// if doing a download-rules based fetch, instruct it to stop.
		iImapCompound->StopForMigrate();
		}
	iStopForMigrate = ETrue;
	}
