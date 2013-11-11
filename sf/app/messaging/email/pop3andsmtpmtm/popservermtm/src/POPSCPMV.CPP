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

#define _MSVAPI_DONT_INCLUDE_FLOGGER_

#include <e32std.h>

#include "POPSMBX.H"
#include "POPS.H"
#include "POPSOP.H" //CImPop3Operations
#include "POPSMTM.H"

// includes for IMCV stuff
#include <txtetext.h>
#include <txtrich.h>
#include <miuthdr.h>
#include <imcvrecv.h>
// Oyster includes
#include <msventry.h>	// CMsvServerEntry

#include <logwrap.h>
#include <logwraplimits.h>

#include <s32mem.h>
#include <s32file.h>
#include <msvapi.h>
#include <imcvutil.h>

#include "PopsDele.h"
#include "PopsCpMv.h"
#include "POPS.H"
#include "POPSOP.H" //CImPop3Operations
#include "popstran.h"
#include <msvstore.h>
#include "POPS.PAN" // imrc's own panic codes
#include "POPSMBX.H"

#include "mobilitytestmtmapi.h"


// IMRC Panic function
GLREF_C void Panic(TPopsPanic aPanic);

CImPop3CopyMove::CImPop3CopyMove(CMsvServerEntry& aLocalEntry, CImPop3Session* aPop3Session, TBool aCopy, RFs& anFs, TMsvId aDestinationId, CImLogMessage* aLogMessage, TBool aDisconnectedMode)
	: CMsgActive( KMsgPop3RefreshMailboxPriority ), iDestination(aLocalEntry), iPopSession(aPop3Session),iCopy(aCopy),iFs(anFs), iDestId(aDestinationId), iPopulate(EFalse), iLogMessage(aLogMessage), iDisconnectedMode(aDisconnectedMode)
	{
	}

CImPop3CopyMove::CImPop3CopyMove(CMsvServerEntry& aLocalEntry, CImPop3Session* aPop3Session, TBool aCopy, RFs& anFs, CImLogMessage* aLogMessage, TBool aDisconnectedMode)
	: CMsgActive( KMsgPop3RefreshMailboxPriority ), iDestination(aLocalEntry), iPopSession(aPop3Session),iCopy(aCopy),iFs(anFs), iPopulate(ETrue), iLogMessage(aLogMessage), iDisconnectedMode(aDisconnectedMode)
	{
	}

CImPop3CopyMove* CImPop3CopyMove::NewL(const CMsvEntrySelection& aRemoteEntry, CMsvServerEntry& aLocalEntry, CImPop3Session* aPop3Session, TBool aCopy, RFs& anFs, TMsvId aServiceId, CImLogMessage* aLogMessage, TBool aDisconnectedMode)
	{
	CImPop3CopyMove* self = new (ELeave) CImPop3CopyMove( aLocalEntry, aPop3Session, aCopy, anFs, aServiceId, aLogMessage, aDisconnectedMode);

	CleanupStack::PushL(self);
	self->ConstructL(aRemoteEntry);
	CleanupStack::Pop();
	return self;
	}

CImPop3CopyMove* CImPop3CopyMove::NewL(const CMsvEntrySelection& aRemoteEntry, CMsvServerEntry& aLocalEntry, CImPop3Session* aPop3Session, TBool aCopy, RFs& anFs, CImLogMessage* aLogMessage, TBool aDisconnectedMode)
// This verion of NewL creates a CImPop3CopyMove for populating entries from the remote mailbox
	{
	// Moving does not make sense when populating a message entry from the remote mailbox
	__ASSERT_DEBUG(aCopy, Panic(EPopFailedDebugAssert));

	CImPop3CopyMove* self = new (ELeave) CImPop3CopyMove( aLocalEntry, aPop3Session, aCopy, anFs, aLogMessage, aDisconnectedMode);

	CleanupStack::PushL(self);
	self->ConstructL(aRemoteEntry);
	CleanupStack::Pop();
	return self;
	}

void CImPop3CopyMove::ConstructL( const CMsvEntrySelection& aRemoteSelection )
	{
	LeaveIfLowDiskL(aRemoteSelection);
	iTransfer = CImPop3TransferMessage::NewL(iDestination);

	// make our copy of aRemoteEntry
	iSource = aRemoteSelection.CopyL();
	iServiceId = iDestination.Entry().Id();
	
	// assume Entry passed in is set to service id (IMCV needs to set new entries to this)
	iRecvConverter = CImRecvConvert::NewL( iFs, &iDestination, KUidMsgTypePOP3, iServiceId);
	iRecvConverter->SetCaf(*iPopSession->GetCafL(iFs));
	iPopCopyMoveState=EPopCpMvRetrieving;
	iProcessComplete=EFalse;
	iMigratingToNewBearer 	= EFalse;
	// richtext body which we might or might not need

	// set recv conv. 
	if (iPopulate)
		{
		if (iSource->Count())
			{
			// Set the MIME parser to point at the first message to be populated.
			iRecvConverter->SetMsvId((*iSource)[0]);
			}
		}
	else
		{
		iRecvConverter->SetMsvId(iDestId);
		}
	
	CActiveScheduler::Add(this);	  // Add CImPop3CopyMove to scheduler's queue
	}


CImPop3CopyMove::~CImPop3CopyMove()
	{
	Cancel();
	// delete everything here
	delete iRetrieve;
	delete iDelete;
	delete iSource;

	delete iRecvConverter;

	delete iTransfer;
	}


void CImPop3CopyMove::LeaveIfLowDiskL(const CMsvEntrySelection& aMsgSelection)
	{
	// Get the original destination
	TMsvEntry destination = iDestination.Entry();

	// Get the size of all the messages that need to be downloaded
	TInt totalMsgsSize = 0;
	for(TInt i = 0; i < aMsgSelection.Count(); i ++)
		{
		iDestination.SetEntry(aMsgSelection.At(i));
		if(!iDestination.Entry().Complete())
			{
			totalMsgsSize += iDestination.Entry().iSize;
			}
		}

	// Set the destination back to the original
	iDestination.SetEntry(destination.Id());

	// Get the Free Disk Space
	TVolumeInfo volumeInfo;
	TInt currentDrive = MessageServer::CurrentDriveL(iFs);
	User::LeaveIfError(iFs.Volume(volumeInfo, currentDrive));

	// Leave if not enougth Disk Space
	if (volumeInfo.iFree < (totalMsgsSize + KMinimumDiskSpaceForSync))
		{
		User::Leave(KErrDiskFull);
		}
	}

//
// Cancel any current operation
//
void CImPop3CopyMove::DoCancel()
	{
	iPopSession->SetOpNotPending();

	if(iPopCopyMoveState==EPopCpMvRetrieving)
		{
		iRetrieve->Cancel();
		}
	else if (iDelete != NULL)
		{
		if (iMigratingToNewBearer)
			{
			iDelete->CancelAllowResume();
			}
		else
			{		
			iDelete->Cancel();
			}
		}

	if (iTransfer != NULL)
		{
		iTransfer->Cancel();
		}

	CMsgActive::DoCancel();
	}

//
// 
//
void CImPop3CopyMove::DoComplete(TInt& /*aCompleteStatus*/)
	{
	}
//
// Start me up
//
void CImPop3CopyMove::StartL(TRequestStatus& aStatus)
	{
	iMsgCtr=0;
	iSavedError = KErrNone;
	// set up progress obj.
	iProgress.iTotalMsgs=iSource->Count();
	iProgress.iMsgsToProcess=iProgress.iTotalMsgs;
	
	// Do a quick tally on the size of messages which are to be copied / moved.
	iProgress.iTotalSize = 0;
	for(TInt i = 0; i < iProgress.iTotalMsgs; i ++)
		{
		iDestination.SetEntry(iSource->At(i));
		if(!iDestination.Entry().Complete())
			{
			iProgress.iTotalSize += iDestination.Entry().iSize;
			}
		}
	
	delete iRetrieve;
	iRetrieve=NULL;
	iRetrieve=CImPop3Retr::NewL(iPopSession,iRecvConverter, iFs);

	if(iProgress.iTotalMsgs)
		{
		DoRetrieveL();
		Queue(aStatus);
		}
	else
		{
		aStatus = KRequestPending;
		TRequestStatus* pS=&aStatus;
		User::RequestComplete(pS,KErrNone);
		}
	}


// ****************************************************************************************** 
// Resume function called by the POP Server MTM, once it has completed Migrating to new bearer
// 
// ******************************************************************************************	
void CImPop3CopyMove::ResumeL(CImPop3Session* aPopSession, TRequestStatus& aStatus)
	{
	iMigratingToNewBearer = EFalse;
	iPopSession = aPopSession;

	delete iRecvConverter;
	iRecvConverter = NULL;
	iRecvConverter = CImRecvConvert::NewL( iFs, &iDestination, KUidMsgTypePOP3, iServiceId);
	iRecvConverter->SetCaf(*iPopSession->GetCafL(iFs));
	
	if (iPopulate)
		{
		if (iSource->Count())
			{
			// Set the MIME parser to point at the first message to be populated.
			iRecvConverter->SetMsvId((*iSource)[iMsgCtr]);
			}
		}
	else
		{
		// Set the folder
		iRecvConverter->SetMsvId(iDestId);
		}
	
	
	switch (iPopCopyMoveState)
		{
		case EPopCpMvRetrieving:
		case EPopCpMvLogging:
			{
			delete iRetrieve;
			iRetrieve=NULL;
			iRetrieve=CImPop3Retr::NewL(iPopSession,iRecvConverter, iFs);

			// Start retrieving again, from where we had left
			DoRetrieveL();
			break;
			}
		case EPopCpMvDeleting:
			{
			// set iDestination to service entry so we can delete stuff
			User::LeaveIfError(iDestination.SetEntry( iServiceId ));

			iDelete->ResumeL(iPopSession, iStatus);
			SetActive();

			break;
			}
		default:
			{
			__ASSERT_ALWAYS(EFalse, Panic(EPopUnexpectedMigrateState));
			break;
			}
		}
		Queue(aStatus);
	}	


//
// Function called from DoRunL.
//
void CImPop3CopyMove::RunLProcessingL()
// This function is called and trapped by the DoRunL function.
// This allows a logging operation to be carried if it leaves.
    {
    TInt err = KErrNone;
	
	switch (iPopCopyMoveState)
		{
		case EPopCpMvRetrieving:
			{
			
			if(iRetrieveMessage)
				{
				if ((!iAlreadyComplete) && (!iMessageMarkedForDelete))
					iRecvConverter->MessageCompleteL();

				TMsvId msgId=(*iSource)[iMsgCtr];
				err = iDestination.SetEntry(msgId);
				if(err==KErrNone)
					{
					TMsvEntry header=iDestination.Entry();
					if((header.Unread()) && (!iPopulate))
						{
						header.SetNew(EFalse);
						err = iDestination.ChangeEntry(header);
						__ASSERT_DEBUG(err == KErrNone, Panic(EPopFailedDebugAssert));
						}
					err = iDestination.SetEntry(iDestId);	// make sure not trying to delete current context
					if(err!=KErrNone)						
						{
						User::Leave(KPop3ProblemWithRemotePopServer);
						}
					}

				// If the server replies with "-ERR" command, leave with "KErrNotFound".
				if(!iRetrieve->PopCommandAccepted())
					{
					User::Leave(KErrNotFound);
					}
				}
			LogFetchedMessageL();
			}
			break;
		case EPopCpMvLogging:
			{
			// If the message has been logged then move on and download the next one.
			iProgress.iMsgsToProcess--;
			if(++iMsgCtr<iSource->Count())
				{
				if (iPopulate)
				// The destination must be changed to the new message when populating
					{
					iRecvConverter->ResetL();
					iRecvConverter->SetMsvId((*iSource)[iMsgCtr]);
					}
				// If we are migrating halt the operation here
				if (iMigratingToNewBearer)
					{			
					// If we don't SetActive, the RunL will complete the request
					iPopCopyMoveState = EPopCpMvRetrieving;
					return;
					}
				DoRetrieveL(); //add it to the collection;
				iPopCopyMoveState = EPopCpMvRetrieving;
				}
			else
				{
				// There are no more messages to download.
				// If we're moving rather than copying then delete the source messages.		
				iPopCopyMoveState = EPopCpMvDeleting;
				RetrievalCompleteL();
				}
			}
			break;
			default:
			break;
		}	
    }

//
// Result of last active call
//
void CImPop3CopyMove::DoRunL()
	{
	if (iDoingTransfer)
		{
		iDoingTransfer = EFalse;
		TMsvEntry entry = iDestination.Entry();
		entry.SetComplete(EFalse);
		iDestination.ChangeEntry(entry);
		}

	if (iSavedError != KErrNone)
		// If we have already have an error then we have just been logging the failure.
		// The failure has been logged so we can leave now.
		{
		User::Leave(iSavedError);
		}
	else
		// There was no error so handle the completion of either the logging or the retrieve operations.
		{
		TRAP(iSavedError, RunLProcessingL());

		if (iSavedError != KErrNone)
			// If an error has occured then we need to clean up the message and log the failure before bailing out.
			{
			if( iRetrieveMessage )
				{
				TInt err = KErrNone;
				if ((!iAlreadyComplete) && (!iMessageMarkedForDelete))
					{
					TRAP(err,iRecvConverter->MessageCompleteL());
					}

	 			if(err != KErrNone || iSavedError)
					{
					// something is wrong with the message delete and report back
					if (!iPopulate)
						{
						err = iDestination.SetEntry(iDestId);
						if (err == KErrNone)
							{
							err = iDestination.DeleteEntry(iRetrieve->EntryId());
							}
						}
					else
						// cleanup the message, leave the root entry only.
						{
						TMsvId entryId = iRetrieve->EntryId();
						if (entryId != KMsvNullIndexEntryId)
							{
							err = iDestination.SetEntry(iRetrieve->EntryId());
							if (err == KErrNone)
								{
								CMsvEntrySelection *childEntries = new (ELeave) CMsvEntrySelection();
								CleanupStack::PushL(childEntries);
								// ignore any errors, there is nothing that can be done about them here.
								User::LeaveIfError(iDestination.GetChildren(*childEntries));
								if (childEntries->Count() != 0)
									{
									iDestination.DeleteEntries(*childEntries);
									}
								CleanupStack::PopAndDestroy(); //childEntries
								}							
							}
						}
					}
				}

			// The message is logged as 'failed' if iSavedError is not KErrNone.
			LogFetchedMessageL();
			}
		}
	}

//
// Use Pop3Retr to Retrieve a message specified by it's Oyster message Id
//
void CImPop3CopyMove::DoRetrieveL()
	{
	iAlreadyComplete = EFalse;
	iMessageMarkedForDelete = EFalse;
	TRequestStatus* pS = &iStatus;
	TMsvId msgId=(*iSource)[iMsgCtr];
	// iDestination will later be changed back to the destination entry, if required
	TInt err = iDestination.SetEntry(msgId);
	TMsvEmailEntry entry = iDestination.Entry();

	TBool partial=entry.PartialDownloaded();
	if(err != KErrNone)
        {
		SetActive();
		User::RequestComplete(pS,err);
        }
	else if (entry.Operation() && (entry.DisconnectedOperation() == EDisconnectedDeleteOperation))
		{
		iMessageMarkedForDelete = ETrue;
		SetActive();
		User::RequestComplete(pS, KErrNone);
		}
	else if ((entry.Complete() && !partial) && (iPopulate))
		{
		iAlreadyComplete = ETrue;
		SetActive();
		User::RequestComplete(pS, KErrNone);
		}
	else
		{
		iProgress.iTotalBytes = entry.iSize;
		if(entry.Complete() && !iPopulate)
			{
			iRetrieveMessage=EFalse;
			// copy from source to target collection
			DoTransferL(entry);
			}
		else
			{
			// reset entry to destination context if the current entry is not meant to be populated
			if (!iPopulate)
				{
				err = iDestination.SetEntry(iDestId);
				User::LeaveIfError(err);
				}

			iRetrieveMessage = ETrue;

//If the message is not found complete the request.
			if (!iRetrieve->SetMessage(msgId)) 
				{
				iAlreadyComplete = ETrue;
				SetActive();
				User::RequestComplete(pS, KErrNone);
				}
			else
				{
				iRetrieve->StartL(iStatus);
				SetActive();
				MOBILITY_TEST_MTM_STATE(iServiceId, KMobilityTestMtmStatePopRetrieving);
				}
			}
		}
	}

//
// Report the refreshing news back to the UI
//
TPop3Progress CImPop3CopyMove::Progress()
	{
	if(iPopCopyMoveState==EPopCpMvRetrieving)
		{
		// Get the Copy/Move Progress
		iProgress.iBytesDone=iRetrieve->Progress();
		}
	else if (iPopCopyMoveState == EPopCpMvLogging)
		{
		// We are doing Logging so the move/copy is complete
		iProgress.iBytesDone = iProgress.iTotalBytes;
		}
	else
		{
		iProgress.iBytesDone=0;
		iProgress.iTotalBytes=0;
		iProgress.iMsgsToProcess=0;
		}
	return iProgress;
	}

//
// Transfer message from remote to local collection (when there's no TOP)
//
void CImPop3CopyMove::DoTransferL(TMsvEntry& aMsvEntry)
	{
	CImPop3TransferMessage::TImPop3TransferMethod transferMethod;

	if (iDisconnectedMode)
		{
		transferMethod = CImPop3TransferMessage::EImPop3CopyTransfer;
		}
	else
		{
		transferMethod = CImPop3TransferMessage::EImPop3MoveTransfer;
		}

	iStatus = KRequestPending;
	iTransfer->StartL(aMsvEntry.Id(), iDestId, transferMethod, iStatus);
	SetActive();
	}

void CImPop3CopyMove::RetrievalCompleteL()
	{
	iProcessComplete=ETrue;
	if(iCopy==EFalse)
		{
		// set iDestination to service entry so we can delete stuff
		User::LeaveIfError(iDestination.SetEntry( iServiceId ));

		delete iDelete;
		iDelete=NULL;
		iDelete = CImPop3Delete::NewL(iDestination,*iSource,iPopSession, iServiceId);
		iDelete->Start(iStatus);
		SetActive(); // Don't need to requeue?
		}
	}

//
// Helper function to set up the iLogEntry object which is invoked after the copy/move.
//
void CImPop3CopyMove::LogFetchedMessageL()
	{
	iPopCopyMoveState = EPopCpMvLogging;
	if (iLogMessage)
		{
// Get the header information for the message that is to be copied or moved.
		User::LeaveIfError(iDestination.SetEntry((*iSource)[iMsgCtr]));

		// Get the 'Fetch' string from the logging string table.
		TBuf<KLogMaxSharedStringLength> fetchString;
		iLogMessage->GetString(fetchString, R_LOG_DIR_FETCHED);

// Set up the log event.
		iLogMessage->LogEvent().SetEventType(KLogMailEventTypeUid);

		iLogMessage->LogEvent().SetDirection(fetchString);
		iLogMessage->LogEvent().SetRemoteParty(iDestination.Entry().iDetails);
		iLogMessage->LogEvent().SetSubject(iDestination.Entry().iDescription);
		iLogMessage->LogEvent().SetLink(iDestId);

		if (iSavedError != KErrNone)
			{
			TBuf<KLogMaxSharedStringLength> failedString;
			iLogMessage->GetString(failedString, R_LOG_DEL_FAILED);
			iLogMessage->LogEvent().SetStatus(failedString);
			}

// Run the iLogMessage operation.
		iLogMessage->Start(iDestination.Entry().iError, iStatus);
		SetActive();
		}
	else
		{
		TRequestStatus* status = &iStatus;
		SetActive();
		User::RequestComplete(status, KErrNone);
		}
	}

 
// ****************************************************************************************** 
// This is called by the POP Server MTM when it starts Migrating Bearer
// 
// ******************************************************************************************
void CImPop3CopyMove::Pause()
	{
	// Set the Migration flag, so we can migrate when the current operation (msg) is complete.
	iMigratingToNewBearer = ETrue;
	}

// ****************************************************************************************** 
// This is called by the POP Server MTM when it starts Migrating Bearer
// 
// ******************************************************************************************
void CImPop3CopyMove::CancelAllowResume()
	{
	// Cancel the copying of the current message  and decrement counters 
	// so we can restart from this message onwards when we have migrated.
	// Use the normal cancel, as we really need to cancel here.
	iMigratingToNewBearer = ETrue;
	Cancel();
	}
