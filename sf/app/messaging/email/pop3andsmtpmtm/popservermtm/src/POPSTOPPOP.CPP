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

#include <e32std.h>

#include "POPSMBX.H"
#include "POPS.H"
#include "POPSOP.H" //CImPop3Operations

// includes for IMCV stuff
#include <imcvrecv.h>
#include <logwrap.h>
#include <logwraplimits.h>

#include <bautils.h>
#include <msvutils.h>
#include <imcvutil.h>

#include <imcm.rsg>

#include "PopsTopPop.h"
#include "POPS.H"
#include "POPSOP.H" //CImPop3Operations
#include "POPSMBX.H"

#include "POPS.PAN" // imrc's own panic codes

#include "mobilitytestmtmapi.h"

// IMRC Panic function
GLREF_C void Panic(TPopsPanic aPanic);

CImPop3TopPopulate* CImPop3TopPopulate::NewL(const CMsvEntrySelection& aRemoteEntry, CMsvServerEntry& aLocalEntry, TInt aLimit, CImPop3Session* aPop3Session, RFs& anFs, CImLogMessage* aLogMessage, TBool aDisconnectedMode, RArray<TUint>& aRemoteSizes)
	{
	CImPop3TopPopulate* self = new (ELeave) CImPop3TopPopulate( aLocalEntry, aLimit, aPop3Session, anFs, aLogMessage, aDisconnectedMode, aRemoteSizes);

	CleanupStack::PushL(self);
	self->ConstructL(aRemoteEntry);
	CleanupStack::Pop(self);
	return self;
	}

CImPop3TopPopulate::~CImPop3TopPopulate()
	{
	iRemoteSizes.Close();
	delete iTop;
	delete iSource;
	delete iRecvConverter;
	}

void CImPop3TopPopulate::StartL(TRequestStatus& aStatus)
	{
	iMsgCtr=0;
	iSavedError = KErrNone;
	// set up progress obj.
	iProgress.iTotalMsgs=iSource->Count();
	iProgress.iMsgsToProcess=iProgress.iTotalMsgs;
	
	// Do a quick tally on the size of messages which are to be copied / moved.
	iProgress.iTotalSize = 0;
	for(TInt i = 0; i < iProgress.iTotalMsgs; ++i)
		{
		iDestination.SetEntry(iSource->At(i));
		if(!iDestination.Entry().Complete())
			{
			iProgress.iTotalSize += iRemoteSizes[i];
			}
		}
	

	if(iProgress.iTotalMsgs>0)
		{
		delete iTop;
		iTop=NULL;
		iTop=CImPop3Top::NewL(iPopSession,iRecvConverter,EFalse);
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
void CImPop3TopPopulate::ResumeL(CImPop3Session* aPopSession, TRequestStatus& aStatus)
	{
	iMigratingToNewBearer = EFalse;
	iPopSession = aPopSession;

	delete iRecvConverter;
	iRecvConverter = NULL;
	iRecvConverter = CImRecvConvert::NewL( iFs, &iDestination, KUidMsgTypePOP3, iServiceId);
	iRecvConverter->SetCaf(*iPopSession->GetCafL(iFs));
	
	delete iTop;
	iTop=NULL;
	iTop=CImPop3Top::NewL(iPopSession,iRecvConverter,EFalse);
	
	// Set the MIME parser to point at the first message to be processed.
	iRecvConverter->SetMsvId((*iSource)[iMsgCtr]);
		
	DoRetrieveL();
	Queue(aStatus);
	}	

TPop3Progress CImPop3TopPopulate::Progress()
	{
	if(iPopTopPopState==EPopTopPopRetrieving)
		{
		// Get the Copy/Move Progress
		iProgress.iBytesDone=iTop->Progress();
		}
	else if (iPopTopPopState == EPopTopPopLogging)
		{
		// We are doing Logging so the move/copy is complete
		iProgress.iBytesDone = iProgress.iTotalBytes;
		}
	else
		{
		iProgress.iBytesDone=0;
		iProgress.iTotalBytes=0;
		}
	return iProgress;
	}

CImPop3TopPopulate::CImPop3TopPopulate(CMsvServerEntry& aLocalEntry, TInt aLimit, CImPop3Session* aPop3Session, RFs& anFs, CImLogMessage* aLogMessage, TBool aDisconnectedMode, RArray<TUint>& aRemoteSizes)
	: CMsgActive( KMsgPop3RefreshMailboxPriority ), iDestination(aLocalEntry), iPopSession(aPop3Session),iFs(anFs), iLogMessage(aLogMessage), iDisconnectedMode(aDisconnectedMode), iTopLimit(aLimit), iRemoteSizes(aRemoteSizes)
	{
	}

void CImPop3TopPopulate::ConstructL( const CMsvEntrySelection& aRemoteSelection)
	{
	// make our copy of aRemoteEntry
	iSource = aRemoteSelection.CopyL();
	iServiceId = iDestination.Entry().Id();

	// assume Entry passed in is set to service id (IMCV needs to set new entries to this)
	iRecvConverter = CImRecvConvert::NewL( iFs, &iDestination, KUidMsgTypePOP3, iServiceId);
	iRecvConverter->SetCaf(*iPopSession->GetCafL(iFs));
	iPopTopPopState=EPopTopPopRetrieving;
	iProcessComplete=EFalse;

	// set recv conv. 
	if (iSource->Count())
		{
		// Set the MIME parser to point at the first message to be populated.
		iRecvConverter->SetMsvId((*iSource)[0]);
		}	
	CActiveScheduler::Add(this);	  // Add CImPop3CopyMove to scheduler's queue
	}

void CImPop3TopPopulate::DoRetrieveL()
	{
	iAlreadyComplete = EFalse;
	iMessageMarkedForDelete = EFalse;
	TRequestStatus* pS = &iStatus;
	TMsvId msgId=(*iSource)[iMsgCtr];
	// iDestination will later be changed back to the destination entry, if required
	TInt err = iDestination.SetEntry(msgId);
	TMsvEmailEntry entry = iDestination.Entry();
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
	else if ((entry.Complete()))
		{
		iAlreadyComplete = ETrue;
		SetActive();
		User::RequestComplete(pS, KErrNone);
		}
	else
		{
		iProgress.iTotalBytes = iRemoteSizes[iMsgCtr];
		// reset entry to destination context if the current entry is not meant to be populated
		User::LeaveIfError(iDestination.SetEntry(iDestId));
		iRetrieveMessage = ETrue;

		//If the message is not found complete the request.
		if (!iTop->SetMessageAndLines(msgId,iTopLimit)) 
			{
			iAlreadyComplete = ETrue;
			SetActive();
			User::RequestComplete(pS, KErrNone);
			}
		else
			{
			iCurrentMsgSize = iRemoteSizes[iMsgCtr];
			iTop->StartL(iStatus);
			SetActive();
			MOBILITY_TEST_MTM_STATE(iServiceId, KMobilityTestMtmStatePopRetrieving);
			}
		}
	}

void CImPop3TopPopulate::RetrievalCompleteL()
	{
	iProcessComplete=ETrue;
	}

void CImPop3TopPopulate::DoRunL()
	{
	// If we have already have an error then we have just been logging the failure.
	// The failure has been logged so we can leave now.
	User::LeaveIfError(iSavedError);

	// There was no error so handle the completion of either the logging or the retrieve operations.
	TRAP(iSavedError, RunLProcessingL());

	if (iSavedError != KErrNone)
		// If an error has occured then we need to clean up the message and log the failure before bailing out.
		{
		if(iRetrieveMessage)
			{
			if ((!iAlreadyComplete) && (!iMessageMarkedForDelete))
				{
				TRAPD(err,iRecvConverter->MessageCompleteL());

	 			if(err != KErrNone)
					{
					// something is wrong with the message delete and report back
					err = iDestination.SetEntry(iDestId);
					if (err == KErrNone)
						{
						err = iDestination.DeleteEntry(iTop->EntryId());
						}
					}
				}
			}

		// The message is logged as 'failed' if iSavedError is not KErrNone.
		LogFetchedMessageL();
		}
	}

void CImPop3TopPopulate::DoComplete(TInt& /*aCompleteStatus*/)
	{
	}

void CImPop3TopPopulate::DoCancel()
	{
	iPopSession->SetOpNotPending();

	if(iPopTopPopState==EPopTopPopRetrieving)
		{
		iTop->Cancel();
		}

	CMsgActive::DoCancel();
	}

void CImPop3TopPopulate::RunLProcessingL()
	{
	switch (iPopTopPopState)
		{
		case EPopTopPopRetrieving:
			{
		    TInt amountLeft = 0;
			if(iRetrieveMessage)
				{
				TBool partial=ETrue;
				if (iTop->Progress()>=TUint(iCurrentMsgSize))
					{
					partial=EFalse;
					}
				
				if ((!iAlreadyComplete) && (!iMessageMarkedForDelete))
					{
					iRecvConverter->MessageCompleteL(partial);
					}
				amountLeft=(iCurrentMsgSize)-iTop->Progress()+iRecvConverter->DeletedAttachmentSize();
				if (amountLeft<0)
					{
					amountLeft=0;
					}

				TMsvId msgId=(*iSource)[iMsgCtr];
				if(iDestination.SetEntry(msgId)==KErrNone)
					{
					TMsvEmailEntry header=iDestination.Entry();
					TBool changed=EFalse;
					if((header.Unread()))
						{
						header.SetNew(EFalse);
						changed=ETrue;
						}

					if(partial!=EFalse)
						{
						// We have used TOP so the entry will be partial, unless we have got the whole thing.
						header.SetPartialDownloaded(partial);
						changed=ETrue;
						}

					if(changed != EFalse)
						{
						User::LeaveIfError(iDestination.ChangeEntry(header));
						}						
					if(iDestination.SetEntry(iDestId)!=KErrNone)						
					{
					User::Leave(KPop3ProblemWithRemotePopServer);
					}
					}

				}

			iRecvConverter->WritePartialFooterL(amountLeft);

			iDestination.SetEntry(iDestId);	// make sure not trying to delete current context
			LogFetchedMessageL();
			}
			break;
		case EPopTopPopLogging:
			// If the message has been logged then move on and download the next one.
			{
			--iProgress.iMsgsToProcess;
			if(++iMsgCtr<iSource->Count())
				{
				// If we are migrating halt the operation here
				if (iMigratingToNewBearer)
					{			
					// If we don't SetActive, the RunL will complete the request
					return;
					}
				else
					{
 					iRecvConverter->ResetL();
					iRecvConverter->SetMsvId((*iSource)[iMsgCtr]);
					DoRetrieveL(); //add it to the collection;
					iPopTopPopState = EPopTopPopRetrieving;
					}
				}
			}
			break;
		default:
			break;
		}
	}

//
// Helper function to set up the iLogEntry object which is invoked after the copy/move.
//
void CImPop3TopPopulate::LogFetchedMessageL()
	{
	iPopTopPopState = EPopTopPopLogging;
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
void CImPop3TopPopulate::Pause()
	{
	// Set the Migration flag
	iMigratingToNewBearer = ETrue;
	}

// ****************************************************************************************** 
// This is called by the POP Server MTM when it starts Migrating Bearer
// 
// ******************************************************************************************	
void CImPop3TopPopulate::CancelAllowResume()
	{
	iMigratingToNewBearer = ETrue;
	
	// Cancel the copying of the current message  and decrement counters 
	// so we can restart from this message onwards when we have migrated.
	// Use the normal cancel, as we really need to cancel here.
	Cancel();		
	}
