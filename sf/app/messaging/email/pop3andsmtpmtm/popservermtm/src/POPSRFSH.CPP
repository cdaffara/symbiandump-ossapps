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
#include <txtetext.h>
#include <txtrich.h>
#include <miuthdr.h>
#include <imcvrecv.h>
#include <msvapi.h>

#include <s32mem.h>
#include <s32file.h>

#include "POPS.PAN" // imrc's own panic codes
// Oyster includes
#include <msvstd.h>
#include <msventry.h>	// CMsvServerEntry
#include <msvstore.h>
#include <mmsvattachmentmanager.h>
#include <mmsvattachmentmanagersync.h>

#include <miutset.h>
#include <imcvutil.h>

#include <popsmtm.h>
#include "POPSRFSH.h"
#include "POPS.H"
#include "POPSOP.H" //CImPop3Operations
#include "POPSMBX.H"

_LIT(KUidlFile,"UIDLS");

const TInt KMaxStringLength = 1024;	



//
// My own panic command 
//
GLREF_C void Panic(TPopsPanic aPanic);

CImPop3RefreshMailBox::CImPop3RefreshMailBox(CImPop3Session& aPopSession): CMsgActive( KMsgPop3RefreshMailboxPriority ), iPopSession( aPopSession)
	{
	__DECLARE_NAME(_S("CImPop3RefreshMailBox"));
	}

CImPop3RefreshMailBox* CImPop3RefreshMailBox::NewL(CMsvServerEntry& aRemoteServerEntry, CImPop3Session& aPopSession, RFs& anFs)
	{
	CImPop3RefreshMailBox* self = new (ELeave) CImPop3RefreshMailBox( aPopSession);
	CleanupStack::PushL(self);	
	self->ConstructL(aRemoteServerEntry, anFs);
	CleanupStack::Pop();
	return self;
	}

void CImPop3RefreshMailBox::ConstructL(CMsvServerEntry& aRemoteServerEntry, RFs& anFs)
	{
	iRefreshOperation = CImPop3RefreshOperation::NewL(aRemoteServerEntry, &iPopSession, anFs);	
	_LIT8(KCrLfStr,".\r\n");
	iFullStopTerminator = KCrLfStr();
	iTextServer=iPopSession.TextServerSession();
	CActiveScheduler::Add(this);	  // Add PopSession to scheduler's queue
	}


CImPop3RefreshMailBox::~CImPop3RefreshMailBox()
	{
	Cancel();
	delete iRefreshOperation;
	}

void CImPop3RefreshMailBox::DoCancel()
	{
	switch(iState)
		{
		case EPop3RefreshBoxDefault:
		case EPop3RefreshBoxTop:
			{
			if (iMigratingToNewBearer)
			{
			iRefreshOperation->CancelAllowResume();
			}
			else
			{
			iRefreshOperation->Cancel();
			}
			break;
			}
		case EPop3RefreshBoxPurgeInput:
			{
			iTextServer->Cancel();
			break;
			}
		default:
			{
			__ASSERT_DEBUG(EFalse, Panic(EImppUnknownRefreshState));
			}
			break;
		}
	CMsgActive::DoCancel();
	}


void CImPop3RefreshMailBox::Start(TRequestStatus& aStatus, CArrayFixFlat<TMsvId>* aMsvIdArray)
	{	
	Queue(aStatus);
	iMsvIdArray = aMsvIdArray;// Provided for transparency
	
	TRAPD(stateErr,ChangeStateL(EPop3RefreshBoxDefault));
	if(stateErr!=KErrNone)
		{
		Complete(stateErr);
		}
	}

void CImPop3RefreshMailBox::DoRunL()
	{
	if (iMigratingToNewBearer)
		{
		// The Pops MTM is currently migrating so no processing is required here
		// This call to doRunl is probably as a result of CImPop3RefreshOperation::Pause()
		// completing us.
		return;
		}
	switch (iState)
		{
	case EPop3RefreshBoxDefault:
		if (iStatus.Int() == KPop3RefreshUidlEquate) 
			{
			ChangeStateL(EPop3RefreshBoxPurgeInput);
			}
		break;
	case  EPop3RefreshBoxPurgeInput:
		iTextServer->GetCurrentTextLine(iTextServerResponse);
		if(iTextServerResponse.Compare(iFullStopTerminator)!=0)	
			{
			iTextServer->QueueReceiveNextTextLine(iStatus);
			SetActive();
			} 
		else 
			{
			ChangeStateL(EPop3RefreshBoxTop);
			}
		break;
	case EPop3RefreshBoxTop:
			break;
	default:
			break;
		};		
	}

void CImPop3RefreshMailBox::ChangeStateL(TState aState)
    {
	switch (aState)
        {
	case EPop3RefreshBoxDefault:
			// Start refresh with UIDL
			iRefreshOperation->Start(iStatus,iMsvIdArray);
			break;
	case EPop3RefreshBoxPurgeInput:
			iPopSession.SetOpNotPending();
			iTextServer->GetCurrentTextLine(iTextServerResponse);
			if(iTextServerResponse.Compare(iFullStopTerminator)!=0) 
			{
				iTextServer->QueueReceiveNextTextLine(iStatus);
				// Break here to make sure Async function is called before SetActive() is called, 
				// else we'll get a Stray Signal
				break;
			}
	case EPop3RefreshBoxTop:
			iRefreshOperation->Cancel();
			iRefreshOperation->Start(iStatus); // Restart refresh with Top
			break;
	default:
			break;
	}
	iState = aState;
	SetActive();
	}

TPop3Progress CImPop3RefreshMailBox::Progress()
	{
	 return iRefreshOperation->Progress();
	}


void CImPop3RefreshMailBox::SetMessagesToKeepL(const CMsvEntrySelection* aMessagesToKeep)
	{
	iRefreshOperation->SetMessagesToKeepL(aMessagesToKeep);
	}


TUint CImPop3RefreshMailBox::RemoteMessageSizeL(TMsvId aId)
	{
	return iRefreshOperation->RemoteMessageSizeL(aId);
	}

// ****************************************************************************************** 
// This is called by the POP Client MTM when it starts Migrating Bearer
// 
// ******************************************************************************************	
void CImPop3RefreshMailBox::Pause()
	{
	iMigratingToNewBearer = ETrue;
	// Ask the Refresh Operation to prepare itself for Migration
	switch(iState)
		{
		case EPop3RefreshBoxDefault:
		case EPop3RefreshBoxTop:
			{
			iRefreshOperation->Pause();
			break;
			}
		case EPop3RefreshBoxPurgeInput:
			{
			// Cancel the Text Server request, so we can complete to migrate.
			iTextServer->Cancel();
			break;
			}
		default:
			{
			__ASSERT_DEBUG(EFalse, Panic(EImppUnknownRefreshState));
			}
			break;
			
		}
	
	}

// ****************************************************************************************** 
// This is called by the POP Client MTM when it starts Migrating Bearer
// 
// ******************************************************************************************	
void CImPop3RefreshMailBox::CancelAllowResume()
	{
	iMigratingToNewBearer = ETrue;
	Cancel();
	}

// Called by the POP Client MTM, once it has completed Migrating to new bearer 
void CImPop3RefreshMailBox::ResumeL(CImPop3Session* aPopSession, TRequestStatus& aStatus)
	{
	Queue(aStatus);
	iMigratingToNewBearer = EFalse;

	if (iState == EPop3RefreshBoxPurgeInput)
		{
		ChangeStateL(EPop3RefreshBoxTop);
		}
	else
		{
		// Inform the Refresh Operation of Migration Completion
		iRefreshOperation->ResumeL(aPopSession, iStatus);
		SetActive();
		}
	}

//================================================================================

CImPop3RefreshOperation::CImPop3RefreshOperation(CMsvServerEntry& aRemoteServerEntry, CImPop3Session* aPopSession, RFs& anFs)
	: CMsgActive( KMsgPop3RefreshMailboxPriority ),	iUidlKey(_FOFF(TMsgUidlStore,iMsvId),ECmpTUint),
		iRemoteServerEntry(aRemoteServerEntry),iPopSession(aPopSession),iFs(anFs)
	{
	__DECLARE_NAME(_S("CImPop3RefreshOperation"));
	}


CImPop3RefreshOperation* CImPop3RefreshOperation::NewL(CMsvServerEntry& aRemoteServerEntry, CImPop3Session* aPopSession, RFs& anFs)
	{
	CImPop3RefreshOperation* self = new (ELeave) CImPop3RefreshOperation( aRemoteServerEntry, aPopSession, anFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}


void CImPop3RefreshOperation::ConstructL()
	{
	// store id of remote collection
	iRemoteId=iRemoteServerEntry.Entry().Id();
	iRecvConverter=CImRecvConvert::NewL( iFs, &iRemoteServerEntry, KUidMsgTypePOP3, iRemoteId);
	iRecvConverter->SetCaf(*iPopSession->GetCafL(iFs));
	iPopTop=CImPop3Top::NewL(iPopSession,iRecvConverter);
	iPopRetr=CImPop3Retr::NewL(iPopSession,iRecvConverter, iFs);
	
	iUidlArray = new(ELeave) CArrayFixSeg<TMsgUidlStore>(8);

	iMsvSelection = new (ELeave) CMsvEntrySelection;
	iUnwantedEntries = new (ELeave) CMsvEntrySelection;
	// store id of remote collection
	// set recv conv. 
	iRecvConverter->SetMsvId(iRemoteId);

	CActiveScheduler::Add(this);	  // Add CImPop3RefreshOperation to scheduler's queue

	iCurrentDrive = MessageServer::CurrentDriveL(iFs);
	}


CImPop3RefreshOperation::~CImPop3RefreshOperation()
	{
	Cancel();
	delete  iMsvSelection;
	
	// delete everything here
	delete iPopStat;
	delete iPopList;
	delete iPopUidl;
  	delete iPopTop;
	delete iPopRetr;
	iUniqueUidlPosArray.Close();
	iTemporaryUidlPosArray.Close();
	if (iUidlArray)
		{
		for (TInt ii=iUidlArray->Count();--ii>=0;)
			DeleteUidl(ii);
		delete iUidlArray;
		}
	delete iPop3Uidls;
	delete [] iSize;

	delete iRecvConverter;
	delete iUnwantedEntries;
	delete iMessagesToKeep;
	}

//
// Cancel any current operation
//
void CImPop3RefreshOperation::DoCancel()
	{
	iPopSession->SetOpNotPending();

	switch(iState)
		{
		case EPopRefreshStat:
			if(iPopStat)
				{
				iPopStat->Cancel();
				}
			break;
		case EPopRefreshList:
			if(iPopList)
				{
				iPopList->Cancel();
				}
			break;
		case EPopRefreshUidl:
			if(iPopUidl)
				{
				iPopUidl->Cancel();
				}
			break;
		case EPopRefreshGetHeader:
			if(iPopTop)
				{
				iPopTop->Cancel();
				}
			if(iPopRetr)
				{
				iPopRetr->Cancel();
				}
			break;
		default:	// do nothing for all other states
				break;
		}
	CMsgActive::DoCancel();
	}

//
// what to do on completion
//
void CImPop3RefreshOperation::DoComplete(TInt& aCompleteStatus)
	{
	// if we had empty headers report back to user with error code
	switch (iState)
		{
	case EPopRefreshComplete:
		break;
	case EPopRefreshGetHeader:
		if (aCompleteStatus!=KErrNone)
			{// If there is no memory to download further, complete
			 // the operation without calling MessageCompleteL.
			 if(aCompleteStatus == KErrNoMemory)
				{
				break;
				}
			// try and remember what we've done, at least
			TRAPD(ignore,iRecvConverter->MessageCompleteL());
			TRAP(ignore,RefreshFinishedL());
			}
		break;
	default:
		break;
		}
	}

//
// Start me up
//
void CImPop3RefreshOperation::Start(TRequestStatus& aStatus, CArrayFixFlat<TMsvId>* aMsgIdArray)
	{	
	Queue(aStatus);
	iCheckDiskSpaceCounter = 0;
	iNewMsg=aMsgIdArray; // take ownership of the message id array
	TRAPD(stateErr,ChangeStateL(EPopRefreshSyncCollection));
	if(stateErr!=KErrNone)
		{
		Complete(stateErr);
		}
	}

//
// Start me up and skip UIDL downloading
// (BFSW1-2016)
void CImPop3RefreshOperation::Start(TRequestStatus& aStatus)
	{
	Queue(aStatus);

	__ASSERT_ALWAYS(iNewMsg,Panic(ETopStartBeforeUidlStart));

	iCheckDiskSpaceCounter = 0;
	if (iUidlArray) 
		{ // Delete unreliable UIDL list (BFSW1-2016)
		for (TInt ii=iUidlArray->Count();--ii>=0;)
			DeleteUidl(ii);
		}
	

	TRAPD(stateErr,ChangeStateL(EPopRefreshGetHeader));
	if(stateErr!=KErrNone)
		{
		Complete(stateErr);	
		}
	}

// ****************************************************************************************** 
// Resume function called by the POP Client MTM, once it has completed Migrating to new bearer
// 
// ******************************************************************************************	
void CImPop3RefreshOperation::ResumeL(CImPop3Session* aPopSession, TRequestStatus& aStatus)
	{
	iMigratingToNewBearer = EFalse;
	iPopSession = aPopSession;

	delete iRecvConverter;
	iRecvConverter = NULL;
	iRecvConverter=CImRecvConvert::NewL( iFs, &iRemoteServerEntry, KUidMsgTypePOP3, iRemoteId);
	iRecvConverter->SetCaf(*iPopSession->GetCafL(iFs));

	delete iPopTop;
	iPopTop = NULL;
	iPopTop=CImPop3Top::NewL(iPopSession,iRecvConverter);
	delete iPopRetr;
	iPopRetr = NULL;
	iPopRetr=CImPop3Retr::NewL(iPopSession,iRecvConverter, iFs);

	iRecvConverter->SetMsvId(iRemoteId);
	
	Queue(aStatus);
	iCheckDiskSpaceCounter = 0;
	TRAPD(stateErr,ChangeStateL(iState));
	if(stateErr!=KErrNone)
		{
		Complete(stateErr);
		}
	}	

//
// Result of last active call
//
void CImPop3RefreshOperation::DoRunL()
    {
        
	switch(iState)
	{
	case EPopRefreshSyncCollection:
		if(iRemoteArrayCtr>0)
			{
			SyncCollectionL();
			QueueRemoteUpdate();
			}
		else
			{
			CompleteSyncCollection();
			if (iMigratingToNewBearer)
				{
				ChangeStateForMigrate(EPopRefreshStat);
				return;
				}
			else
				{
				ChangeStateL(EPopRefreshStat);
				}
			}
		break;

	case EPopRefreshStat:
		// initialise progress object

		iProgress.iMsgsToProcess=iNoMessages;
	
		iProgress.iTotalMsgs=iNoMessages;

		User::LeaveIfError(iRemoteServerEntry.SetEntry(iRemoteId));
		iEntry = iRemoteServerEntry.Entry();
		iEntry.SetMtmData3(iNoMessages);
		User::LeaveIfError(iRemoteServerEntry.ChangeEntry(iEntry));

		delete iPopStat;
		iPopStat=NULL;
		if(iNoMessages)
			{
			if (iMigratingToNewBearer)
				{
				ChangeStateForMigrate(EPopRefreshList);
				return;
				}
			ChangeStateL(EPopRefreshList);
			}
		else
			{
			if (iMigratingToNewBearer)
				{
				ChangeStateForMigrate(EPopRefreshDeleteDeadMessages);
				return;
				}
			ChangeStateL(EPopRefreshDeleteDeadMessages);
			}
		break;

	case EPopRefreshList:
		delete iPopList;
		iPopList=NULL;
		if (iMigratingToNewBearer)
			{
			ChangeStateForMigrate(EPopRefreshUidl);
			return;
			}
		ChangeStateL(EPopRefreshUidl);
		break;

	case EPopRefreshUidl:
		__ASSERT_ALWAYS(iPopUidl, Panic(EPopNullPointer));
			
		iUidlExists=iPopUidl->PopCommandAccepted();
		delete iPopUidl;
		iPopUidl=NULL;
		if (iUidlExists)
			{
			if (iMigratingToNewBearer)
				{
				ChangeStateForMigrate(EPopRefreshSyncUidl);
				return;
				}
			ChangeStateL(EPopRefreshSyncUidl);
			}
		else
			{
			// delete any entries with a zero length inet msg id
			//scan UIDL file for msgs with no POP id. If found, delete from UIDL file and Messaging Server
			//??? why would they have a zero length POP id ?
			User::LeaveIfError(iRemoteServerEntry.SetEntry(iRemoteId));
			CArrayFix<TMsgUidlStore>& uidlarray=*iUidlArray;
			TInt row = uidlarray.Count();
			while(--row>=0)
				{
				if(uidlarray[row].iPopId->Length()==0)
					{
#ifdef _DEBUG
					TInt error = 
#endif						
					iRemoteServerEntry.DeleteEntry( uidlarray[row].iMsvId);
					__ASSERT_DEBUG( error == KErrNone, Panic(EPopFailedDebugAssert));
					DeleteUidl(row);
					}
				}
			if (iMigratingToNewBearer)
				{
				ChangeStateForMigrate(EPopRefreshGetHeader);
				return;
				}	
			ChangeStateL(EPopRefreshGetHeader);
			}
		break;

	case EPopRefreshSyncUidl:		
		if(iRemoteArrayCtr>0)
			{
			SyncUidlL();
			QueueRemoteUpdate();
			}
		else
			{	
			if (iMigratingToNewBearer)
				{
				ChangeStateForMigrate(EPopRefreshGetHeader);
				return;
				}
			ChangeStateL(EPopRefreshGetHeader);
			}
		break;
	
	case EPopRefreshGetHeader:
		//At this point iMsgCtr contains the index of the hdr to download
	
		if(iFirstCallToTop)
			{
			__ASSERT_ALWAYS(iPopTop, Panic(EPopNullPointer));
			iTopExists=iPopTop->PopCommandAccepted();
			}

		if ( !iFirstCallToTop || iTopExists )
			{
			//This is not the 1st call to TOP, or the TOP command exists

			// Don't create a message if the header has none of the following RFC 822 fields:
			//  To:, Cc:, Bcc:, From:, ReturnPath 
			// I assume that if it is missing ALL of these fields then the Email message MUST be broken!
						
			if(iUidlExists || CompareIdAndSizeL())
				{
				// create new TMsvEntry here in folder CMsvEntry
				TMsvEntry msvEntry = iRecvConverter->MessageEntryDetailsL();
				TRAP_IGNORE(CreateNewHeaderEntryL( msvEntry ));
	
				iIdTab[iMsgCtr]=msvEntry.Id();
				__ASSERT_ALWAYS(iNewMsg, Panic(EPopNullPointer));
				iNewMsg->AppendL(msvEntry.Id());	// cannot fail, as we have SetReserveL()'d
				
				if (iTopExists)
					{
					iRecvConverter->MessageCompleteL(msvEntry);
					}
				}
			else
				{
				TMsvEntry msvEntry = iRecvConverter->MessageEntryDetailsL();
				iRecvConverter->MessageCompleteL(msvEntry);
				}
			iProgress.iMsgsToProcess--;

			iMsgCtr--;
			}

		iFirstCallToTop=EFalse;

		if (GetMessagesIfAny())
			{
			RetrieveHeadersL();
			}
		else
			{
			if (iMigratingToNewBearer)
				{
				ChangeStateForMigrate(EPopRefreshDeleteExcessMessages);
				return;
				}
			ChangeStateL(EPopRefreshDeleteExcessMessages);
			if(iUidlExists==EFalse)
				{
				//We need to remove TOPed or RETRed msgs that were already downloaded
				User::LeaveIfError(iRemoteServerEntry.SetEntry(iRemoteId));
				for( TInt loop = iUnwantedEntries->Count(); --loop>=0;)
					{
#if defined(_DEBUG)
					TInt err = iRemoteServerEntry.DeleteEntry((*iUnwantedEntries)[loop]); 
					__ASSERT_DEBUG(err == KErrNone, Panic(EPopFailedDebugAssert));
#else
					iRemoteServerEntry.DeleteEntry((*iUnwantedEntries)[loop]); 
#endif
					}
				}
			}
		break;

	case EPopRefreshDeleteExcessMessages:
	
		if (!iUidlExists)
			{
			// Fix to stop the deletion of existing headers when POP MTM should connect but not synch 

			if (iPopSession->MaxHeaders()==0)
				{
				if (iMigratingToNewBearer)
					{
					ChangeStateForMigrate(EPopRefreshDeleteDeadMessages);
					return;
					}
				ChangeStateL(EPopRefreshDeleteDeadMessages);
				break;
				}  
			else
				{
				while (--iRemoteArrayCtr>=0)
					{
					if (iRemoteArrayCtr<TInt(iNoMessages-iPopSession->MaxHeaders()))
						{
						iRemoteServerEntry.SetEntry(iIdTab[iRemoteArrayCtr]);
						const TMsvEmailEntry& entry=iRemoteServerEntry.Entry();
						// Check if the message entry is marked for offline delete operation.
						TBool notMarkedForOfflineDelete = EFalse;
						TImDisconnectedOperationType  opType;
						if((opType = entry.DisconnectedOperation())!= EDisconnectedDeleteOperation)
							{
							notMarkedForOfflineDelete = ETrue;
							}
						if (CanDeleteEntry(entry))
							{
							//  Don't handle the error
							User::LeaveIfError(iRemoteServerEntry.SetEntry(iRemoteId));
							// The message entry is not marked for offline delete, so delete the entry from
							// message INDEX(mail2) file.
							if(notMarkedForOfflineDelete)
								{
								User::LeaveIfError(iRemoteServerEntry.DeleteEntry(iIdTab[iRemoteArrayCtr]));								
								}
							DeleteUidl(iRemoteArrayCtr);
							QueueRemoteUpdate();
							break;
							}
						}
					}
				}

			}
		else
			// Fix to stop the deletion of existing headers when POP MTM should connect but not synch 

			if (iPopSession->MaxHeaders()==0)
				{
				if (iMigratingToNewBearer)
					{
					ChangeStateForMigrate(EPopRefreshDeleteDeadMessages);
					return;
					}
				ChangeStateL(EPopRefreshDeleteDeadMessages);
				break;
				}  
			else 
				while (--iRemoteArrayCtr>=0)
					{
					const TMsgUidlStore& uidl=iUidlArray->At(iRemoteArrayCtr);
					if (iPop3Uidls->MsgNo(*uidl.iPopId) < TInt(iNoMessages-iPopSession->MaxHeaders()))
						{
						iRemoteServerEntry.SetEntry(uidl.iMsvId);
						const TMsvEmailEntry& entry=iRemoteServerEntry.Entry();
						// Check if the message entry is marked for offline delete operation.
						TBool notMarkedForOfflineDelete = EFalse;
						TImDisconnectedOperationType  opType;
						if((opType = entry.DisconnectedOperation())!= EDisconnectedDeleteOperation)
							{
							notMarkedForOfflineDelete = ETrue;
							}
						if (CanDeleteEntry(entry))
							{
							//  Don't handle the error
							User::LeaveIfError(iRemoteServerEntry.SetEntry(iRemoteId));
							// The message entry is not marked for offline delete, so delete the entry from
							// message INDEX(mail2) file.
							if(notMarkedForOfflineDelete)
								{
								User::LeaveIfError(iRemoteServerEntry.DeleteEntry(uidl.iMsvId));	
								}
							DeleteUidl(iRemoteArrayCtr);
							QueueRemoteUpdate();
							break;
							}
						}
					}
			
			if (iRemoteArrayCtr<0)
				{
				if (iMigratingToNewBearer)
					{
					ChangeStateForMigrate(EPopRefreshDeleteDeadMessages);
					return;
					}
				ChangeStateL(EPopRefreshDeleteDeadMessages);
				}
			break;
				
	
	case EPopRefreshDeleteDeadMessages:	// dispose of un-present messages
		if (!iUidlExists)
			{
			//delete all messages from the Messaging Server whose MsvID does NOT appear in
			//the iIdTab[] array.
			User::LeaveIfError(iRemoteServerEntry.SetEntry(iRemoteId));
			CMsvEntrySelection* pop3Entries = new (ELeave) CMsvEntrySelection;
			CleanupStack::PushL(pop3Entries);
			User::LeaveIfError(iRemoteServerEntry.GetChildren(*pop3Entries ));
			
			if (iNoMessages == 0)
				{
				// Clear the entries from the local POP3 Mailbox, and delete the UIDs
				if (pop3Entries ->Count()!=0)
					{
					iRemoteServerEntry.DeleteEntries(*pop3Entries);
					}
				if (iUidlArray) 
					{ // Delete UIDL list 
					for (TInt ii=iUidlArray->Count();--ii>=0;)
						DeleteUidl(ii);
					}
				}		
			else
				{
				// Downloaded each header in turn to get the uid list.
				// Delete any messages in remote folder that aren't in the iUidlArray.
				// 
				CMsvEntrySelection* deadEntries = new (ELeave) CMsvEntrySelection;
				CleanupStack::PushL(deadEntries);
				TInt msgCtr = pop3Entries->Count();
				//nested loops! - not an ideal search method 
				while (--msgCtr>=0)
					{
					TBool found = EFalse;
					for (TUint index = 0; index < iNoMessages && !found; index++)
						{
						if ((*pop3Entries)[msgCtr] == iIdTab[index])
							{
							found = ETrue;
							}
						}
					if (!found)
						{
						deadEntries->AppendL((*pop3Entries)[msgCtr]);
						}
					}
				if (deadEntries->Count())
					{
					User::LeaveIfError(iRemoteServerEntry.DeleteEntries(*deadEntries));
					}
				CleanupStack::PopAndDestroy(deadEntries);
				}
			CleanupStack::PopAndDestroy(pop3Entries);
			ChangeStateL(EPopRefreshComplete);
			break;
			}
		else
			{
			while (--iRemoteArrayCtr>=0)
				{
				const TMsgUidlStore& uidl=iUidlArray->At(iRemoteArrayCtr);

				if (uidl.iSelectionPos!=TMsgUidlStore::EPresent)
					{
					//  Don't handle the error
					iRemoteServerEntry.DeleteEntry(uidl.iMsvId);
					User::LeaveIfError(iRemoteServerEntry.DeleteEntry(uidl.iMsvId));
					DeleteUidl(iRemoteArrayCtr);
					QueueRemoteUpdate();
					break;
					}
				}
			}

		if (iRemoteArrayCtr<0)
			{
			ChangeStateL(EPopRefreshComplete);
			}
		break;

	default:     // Unknown state
	Panic(EImppUnknownRefreshState);
	break;
	}
    }

	//
	// Finished refresh so tidy up
	//
	//
	void CImPop3RefreshOperation::RefreshFinishedL()
	{
	iRemoteServerEntry.CompleteBulk();
	CreateUidlFileL();
	delete iPopTop;
	iPopTop=NULL;
	delete iPopRetr;
	iPopRetr=NULL;
	__ASSERT_ALWAYS(iNewMsg, Panic(EPopNullPointer));
	iNewMsg->Reset();	// all new messages are sound
	}

	// retrieve headers(or whole messages) else complete
	//
	TBool CImPop3RefreshOperation::GetMessagesIfAny()
	{
	while (iMsgCtr>=iLastHeaderToGet && iIdTab[iMsgCtr]!=0)
	iMsgCtr--;
	if (iMsgCtr < 0 )
	{
	return EFalse;
	}
	return (iMsgCtr>=iLastHeaderToGet);
	}

void CImPop3RefreshOperation::ChangeStateForMigrate(TState aState)
	{
	iState = aState;
	
	delete iPopTop;
	iPopTop=NULL;
	delete iPopRetr;
	iPopRetr=NULL;
	}
	
void CImPop3RefreshOperation::ChangeStateL(TState aState)
	{
	//
	// State machine of the whole POP mail session.
	//
	// Identify state on entry, change to next state and then
	// start new operation associated with that new state.
	//
	switch (aState)
	{
	case EPopRefreshSyncCollection:
		GetRemoteMessagesL();
		QueueRemoteUpdate();	
	break;

	case EPopRefreshStat:
		GetNoMessagesL();
	break;

	case EPopRefreshList:
		AllocateArraysL();
		GetMsgSizesL();
	break;

	case EPopRefreshUidl:
		GetMsgUidlsL();
	break;

	case EPopRefreshSyncUidl:
		iUniqueUidlPosArray.Reset();
		iTemporaryUidlPosArray.Reset();
		iPreviousIndex = KErrGeneral;		
		iRemoteArrayCtr=iUidlArray->Count();
		QueueRemoteUpdate();	
	break;

	case EPopRefreshGetHeader:
		// start retrieving headers

		iFirstCallToTop=ETrue;
		iTopExists=ETrue;

		iMsgCtr=iNoMessages-1;
		if (!iUidlExists)
			{
			iLastHeaderToGet =0;
			}
		else
			{
			iLastHeaderToGet=iPopSession->MaxHeaders();
			if (iLastHeaderToGet >= 0)
				iLastHeaderToGet=iNoMessages-iLastHeaderToGet;
			if (iLastHeaderToGet<0)
				iLastHeaderToGet=0;
			}

		if(GetMessagesIfAny())
			{
			if(iPopSession->PipeliningSupport())
				{
				__ASSERT_ALWAYS(iPopTop, Panic(EPopNullPointer));
				iPopTop->SetStartAndEndMessageIndex(iLastHeaderToGet, iMsgCtr);
				}
			RetrieveHeadersL();
			break;
			}
		aState=EPopRefreshDeleteExcessMessages;
		// nothing to do: drop through

	case EPopRefreshDeleteExcessMessages:
		//Need to commit downloaded headers to file before doing any deletions
		iRemoteServerEntry.CompleteBulk();
		if (iLastHeaderToGet==0)	//no excess messages !
			{
			aState=EPopRefreshDeleteDeadMessages;
			}
		// nothing to do: drop through

	case EPopRefreshDeleteDeadMessages:
		//Need to commit downloaded headers to file before doing any deletions
		iRemoteServerEntry.CompleteBulk();
		// make sure we're pointing to correct context
		User::LeaveIfError(iRemoteServerEntry.SetEntry(iRemoteId));
		iRemoteArrayCtr=iUidlArray->Count();
		QueueRemoteUpdate();	
	break;

	case EPopRefreshComplete:
		RefreshFinishedL();
	break;

	default:     // Unknown state
		Panic(EImppUnknownRefreshState);
	break;
	}
	iState=aState;
	}

	//
	// Use Pop STAT command to obtain of messages in remote mailbox
	//
	void CImPop3RefreshOperation::GetNoMessagesL()
	{
	CImPop3Stat* popStat = CImPop3Stat::NewL(iPopSession);
	if ( iPopStat  )
	{
	delete iPopStat;
	iPopStat = NULL;
	}
	iPopStat = popStat;
	iPopStat->Start(iStatus,iNoMessages,iMboxSize);
	SetActive();
	}

	//

	// Use LIST command to get message sizes
	//
	void CImPop3RefreshOperation::GetMsgSizesL()
	{
	CImPop3List* popList = CImPop3List::NewL(iPopSession);
	if ( iPopList  )
		{
		delete iPopList;
		iPopList = NULL;
		}
	iPopList = popList;
	iPopList->Start(iStatus,iSize);
	SetActive();
	}

//
// Use UIDL to obtain POP3 unique message Ids
//
void CImPop3RefreshOperation::GetMsgUidlsL()
	{
	CImPop3Uidl* popUidl = CImPop3Uidl::NewL(iPopSession);
	if ( iPopUidl  )
		{
		delete iPopUidl;
		iPopUidl = NULL;
		}
	iPopUidl = popUidl;
	iPopUidl->Start(iStatus,*iPop3Uidls);
	SetActive();
	}
	
//
// Post Stat allocate the various arrays
// we know how many messages there are
void CImPop3RefreshOperation::AllocateArraysL()
	{
 	// Allocate the message id array and give it to the POP session
 	// so that any operation can access it.For example, Top requires
 	// access to this for pipelining
	iIdTab = new(ELeave) TInt32[iNoMessages];
	Mem::FillZ(iIdTab,iNoMessages*sizeof(TInt32));
	iPopSession->SetMessageArray(iIdTab, iNoMessages);
	iSize = new(ELeave) TUint[iNoMessages];
	__ASSERT_ALWAYS(iNewMsg, Panic(EPopNullPointer));
	iNewMsg->SetReserveL(iNoMessages);		// no failure during AppendL()

	iPop3Uidls = CImPop3UidlMap::NewL(iNoMessages);
	}

void CImPop3RefreshOperation::DeleteUidl(TInt anIndex)
	{
	TMsgUidlStore& uidl=(*iUidlArray)[anIndex];
	delete uidl.iPopId;
	iUidlArray->Delete(anIndex);
	}

//
// Open uidl file
//
void CImPop3RefreshOperation::GetRemoteMessagesL()
	{
	TMsvSelectionOrdering order;
	order.SetShowInvisibleEntries( ETrue );
	iRemoteServerEntry.SetSort( order );
	TInt selErr=iRemoteServerEntry.GetChildren(*iMsvSelection);
	if(selErr!=KErrNone)
		{
		User::Leave(selErr);
		}
	iRemoteArrayCtr=iMsvSelection->Count();
	TRAP_IGNORE(OpenUidlFileL());
	}

//
//  Kick off async operation
//
void CImPop3RefreshOperation::QueueRemoteUpdate()
	{
	TRequestStatus *pS = &iStatus;
	SetActive();
	User::RequestComplete(pS,KErrNone);
	}

//
// Sets the element with the INDEX in the message server
//
void CImPop3RefreshOperation::SyncCollectionL()
	{
	//iUidlArray has been read from the UIDL file
	//Look for the Messaging Server ID in iUidlArray
	//If found, set the member in the array element that points to the message INDEX (not id) in the Messaging Server
	--iRemoteArrayCtr;	// one at a time
	TMsgUidlStore aUidlStore;;
	aUidlStore.iMsvId=(*iMsvSelection)[iRemoteArrayCtr];
	TInt selectRow;
	if(iUidlArray->FindIsq(aUidlStore,iUidlKey,selectRow))
		{
        //  Don't handle error in release mode.
#ifdef _DEBUG        
		TInt err = 
#endif				
		iRemoteServerEntry.DeleteEntry(aUidlStore.iMsvId);
      	__ASSERT_DEBUG(err == KErrNone, Panic(EPopFailedDebugAssert));
		}
	else
		iUidlArray->At(selectRow).iSelectionPos=iRemoteArrayCtr;
	}

void CImPop3RefreshOperation::CompleteSyncCollection()
//
// Complete the synchronization of the collection and local UIDL store
//
	{
	for(TInt ii=iUidlArray->Count();--ii>=0;)
		{
		if(iUidlArray->At(ii).iSelectionPos==TMsgUidlStore::EInvalid)
			{
			DeleteUidl(ii);
			}
		}
	}
//
// Sync the UIDL file for the Message Server to the POP server.
//
void CImPop3RefreshOperation::SyncUidlL()
	{
	//iPop3Uidls now contains the message ids of the messages on the POP3 server
	--iRemoteArrayCtr;
	TMsgUidlStore& uidl=(*iUidlArray)[iRemoteArrayCtr];
	__ASSERT_DEBUG(uidl.iSelectionPos>=0,User::Invariant());

	TInt popUidlPos = 0;
	TInt popUidlRow = iPop3Uidls->MsgUidlNo(*uidl.iPopId,popUidlPos);
	// For duplicated case get the actual positions of the UIDLs.
	if(popUidlRow == KErrAlreadyExists)
		{
		 while ( popUidlPos > 0 )
			{
			if (iPop3Uidls->MsgUidl(popUidlPos-1) == *uidl.iPopId)
		 		{
		 		--popUidlPos;
				}
				else
				{
				break;
				}
			}
		TInt uidlExists = iUniqueUidlPosArray.FindInOrder(popUidlPos);
		if(uidlExists != KErrNotFound)
			{
			//If aUidl already exists find position of next duplicated uidl.
			popUidlPos = iTemporaryUidlPosArray[uidlExists] + 1;
	    
	    	// Remove the last duplicated Uidl position value.
			iTemporaryUidlPosArray.Remove(uidlExists);
	
			//Insert the position of the present duplicate uidl to the array.
			iTemporaryUidlPosArray.Insert(popUidlPos,uidlExists);
			}
		else
			{
			//iUniqueUidlPosArray will contain the first occurrences(pos)of all duplicated UIDLs
			iUniqueUidlPosArray.InsertInOrder(popUidlPos);
			
			// For getting next position of duplicate Uidl.
			iTemporaryUidlPosArray.InsertInOrder(popUidlPos);
			}
	
		//Maximum value of popUidlPos should not exceed count(No. of uidls in the popserver - 1).
		TInt count = iPop3Uidls->MsgCount() - 1;
		popUidlRow = -1;
		if(count >= popUidlPos)		
			{
			// Check Whether the UIDL at popUidlRow of POP server matches the UIDL in Uidl file..
			if(iPop3Uidls->MsgUidl(popUidlPos) == *uidl.iPopId)
				{
				popUidlRow = iPop3Uidls->MsgIndex(popUidlPos);	
				}
			}
		}
	
	//Check whether the UIDL index has already been passed.
	//This is to Handle the deletion of all duplicated mails from server using other email client.
	if(iPreviousIndex == popUidlRow)
		{
		popUidlRow = -1;
		}
	else
		{
		iPreviousIndex = popUidlRow;	
		}


	if(popUidlRow<0)
		{
		//A Message is in the Messaging Server but not on the POP3 server, so delete it from the
		//the Messaging Server
#ifdef _DEBUG		
     	TInt err = 
#endif     	     	
     	iRemoteServerEntry.DeleteEntry(uidl.iMsvId);//  Don't handle the error in release mode
         __ASSERT_DEBUG(err == KErrNone, Panic(EPopFailedDebugAssert));
		// & delete it from the UIDL file
		DeleteUidl(iRemoteArrayCtr);
		}
	else
		{
		//A message is in the Messaging Server *and* is on the POP3 server,
		//so mark it as present in the UIDL file, and decrement the number of headers to download
		iIdTab[popUidlRow]=uidl.iMsvId; //GetMessagesIfAny() checks this to see if the header is already present
		iProgress.iMsgsToProcess--;
		uidl.iSelectionPos=TMsgUidlStore::EPresent;
		}
	}
//
// Report the refreshing news back to the UI
//
TPop3Progress CImPop3RefreshOperation::Progress()
	{
	return iProgress;
	}
//
// Gets Headers or Messages
//
void CImPop3RefreshOperation::RetrieveHeadersL()
	{
	// Check to see we have at least the minimum free disk space available
	if (--iCheckDiskSpaceCounter <= 0)
  		{
		// If we are running low on disk space then leave

		ImCheckDiskSpace::LeaveIfLowDiskL(iFs, iCurrentDrive);
		iCheckDiskSpaceCounter = KCheckDiskSpaceEveryNMessages;
		}
  	
	// get header or msg
	iTopExists ? GetHeaderByIndexL(iMsgCtr) : GetMessageByIndexL(iMsgCtr);

	SetActive();
	}

//
// Use Top to Retrieve the message header
//
void CImPop3RefreshOperation::GetHeaderByIndexL(TInt anIndex)
	{
	__ASSERT_ALWAYS(iPopTop, Panic(EPopNullPointer));
	iPopTop->SetMessageIndexAndLines(anIndex+1,1);
	iPopTop->StartL(iStatus);
	}

//
// If no Top need to retrieve whole message
//
void CImPop3RefreshOperation::GetMessageByIndexL(TInt anIndex)
	{
	__ASSERT_ALWAYS(iPopRetr, Panic(EPopNullPointer));
	iPopRetr->SetMessageIndex(anIndex+1);
	iPopRetr->StartL(iStatus);
	}

//
//
// Get TMsvEntry from CImHeader via CMsgIMail add to msg collection?
//
void CImPop3RefreshOperation::CreateNewHeaderEntryL(TMsvEntry& aNewEntry)
	{
	TPtrC8 id;
	if(iTopExists)
		{
		__ASSERT_ALWAYS(iPopTop, Panic(EPopNullPointer));
		iPopTop->EntryId();
		id.Set(iPopTop->ImMsgId());
		}
	else
		{
		__ASSERT_ALWAYS(iPopRetr, Panic(EPopNullPointer));
		iPopRetr->EntryId();
		id.Set(iPopRetr->ImMsgId());
		}

	aNewEntry.SetComplete(EFalse);
	aNewEntry.SetAttachment(EFalse);
	aNewEntry.iSize = iSize[iMsgCtr];
	aNewEntry.iServiceId = iRemoteId;
	aNewEntry.SetVisible(ETrue);
	aNewEntry.SetInPreparation(EFalse);

	// add message to list uidl list
	TMsgUidlStore	msgUidlStore;	
	if(iUidlExists)
		{
		msgUidlStore.iPopId=(*iPop3Uidls)[iMsgCtr].AllocLC();
		}
	else
#if defined _UNICODE
		{
		msgUidlStore.iPopId = HBufC8::NewLC(id.Length());
		msgUidlStore.iPopId->Des().Copy(id);
		}
#else
		msgUidlStore.iPopId=id.AllocLC();
#endif

	msgUidlStore.iMsvId=(aNewEntry.Id());
	msgUidlStore.iRemoteSize=iSize[iMsgCtr];
	msgUidlStore.iSelectionPos=TMsgUidlStore::EPresent;
	iUidlArray->InsertIsqL(msgUidlStore,iUidlKey);
	CleanupStack::Pop();	//iPopId
	}


//
// Compare internet msg id of retrieved header to what we have stored and compare sizes
//
TBool CImPop3RefreshOperation::CompareIdAndSizeL()
	{
	TPtrC8 id;

	__ASSERT_ALWAYS(iPopTop && iPopRetr, Panic(EPopNullPointer));
	iTopExists ? id.Set(iPopTop->ImMsgId()) : id.Set(iPopRetr->ImMsgId());

	CArrayFix<TMsgUidlStore>& uidlarray=*iUidlArray;
	TInt row=uidlarray.Count();
	while (--row>=0 && *uidlarray[row].iPopId!=id)
		;

	if (row>=0)	
		{
//we've found an entry in the UIDL file which matches our msg id
//so we don't need to create a new message
		TMsgUidlStore& uidlstore=uidlarray[row];
		if (uidlstore.iRemoteSize==iSize[iMsgCtr])			
			{
			iIdTab[iMsgCtr]=uidlstore.iMsvId;
			__ASSERT_DEBUG(uidlstore.iSelectionPos>=0,User::Invariant());
			uidlstore.iSelectionPos=TMsgUidlStore::EPresent;
//as the UIDL command is not supported, we have downloaded the header or the message body
//we're throwing this away now because the message already exists.
			__ASSERT_ALWAYS(iPopTop && iPopRetr, Panic(EPopNullPointer));
			TMsvId newId;
			iTopExists ? newId = iPopTop->EntryId() : newId = iPopRetr->EntryId();
			iUnwantedEntries->AppendL(newId);
			return EFalse;
			}
		// Set the current context to the POP3 service entry else DeleteEntry panics
		User::LeaveIfError(iRemoteServerEntry.SetEntry(iRemoteId));
        //  Don't handle the error in release mode.
#ifdef _DEBUG        
		TInt err = 
#endif				
		iRemoteServerEntry.DeleteEntry(uidlstore.iMsvId); // should this happen?
        __ASSERT_DEBUG(err == KErrNone, Panic(EPopFailedDebugAssert));
		DeleteUidl(row);
		}
	return ETrue;
	}

//
// Open a file of existing POP Uidls with corresponding Babel MsgIds
//
void CImPop3RefreshOperation::OpenUidlFileL()
	{
	// get path of uidls file from id of remote server entry
    TInt error = KErrNone;
	TParse p;
	TFileName path;

	CMsvStore* store = iRemoteServerEntry.ReadStoreL(); 
	CleanupStack::PushL(store);
	MMsvAttachmentManager& attachmentMgr = store->AttachmentManagerL();
	// if there is no attachment then a UIDL file has not been
	// created, so not found.
	if(attachmentMgr.AttachmentCount() < 1)
		{
		User::Leave(KErrNotFound);
		}
	CMsvAttachment* attachment = attachmentMgr.GetAttachmentInfoL(0);
	CleanupStack::PushL(attachment);
	path = attachment->FilePath();
	p.Set(path,NULL,NULL);
	CleanupStack::PopAndDestroy(2,store);

	RFile f;
	error = f.Open(iFs,p.FullName(),EFileShareReadersOnly);
	User::LeaveIfError(error);

	CDirectFileStore* index=CDirectFileStore::FromLC(f);
	const TUidType type(KDirectFileStoreLayoutUid,KNullUid,KMsgFileRemoteIdIndex);
	if (type != index->Type())
		{
		User::Leave(KErrCorrupt);
		}
	RStoreReadStream in;
	in.OpenLC(*index,index->Root());
	TInt entries = in.ReadInt32L();
	TMsgUidlStore entry;
	while (--entries>=0)
		{
		HBufC8* popid=HBufC8::NewLC( in, KMaxStringLength );
		// internalization code... inline to handle cleanup issues
		entry.iPopId=popid;
		entry.iMsvId=in.ReadUint32L();
		entry.iRemoteSize=in.ReadUint32L();
		entry.iSelectionPos=TMsgUidlStore::EInvalid;	// not present in selection (yet)
		//
		iUidlArray->AppendL(entry);
		CleanupStack::Pop(popid);	// HBufC8's
		}
	CleanupStack::PopAndDestroy(2); // in, index
	}

//
// Create new POP Uidl file
//
void CImPop3RefreshOperation::CreateUidlFileL()
	{
	// create the new index of uidl stuff
	// put it in folder for current server entry (correct remote mailbox folder)
	TParse p;
	TFileName path;

	TInt fileErr = iRemoteServerEntry.SetEntry( iRemoteId );
	if(fileErr!=KErrNone)
		{
		return;
		}
	
	CMsvStore* store = iRemoteServerEntry.EditStoreL(); 
	CleanupStack::PushL(store);
	// UIDL file is stored as the first attachment to the server entry
	// if it is already present then overwrite, else create it,
	MMsvAttachmentManager& attachmentMgr = store->AttachmentManagerL();
	RFile file;
	if(attachmentMgr.AttachmentCount() < 1)
		{
		MMsvAttachmentManagerSync& attachmentMgrSync = store->AttachmentManagerExtensionsL();
		CMsvAttachment *attachment = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
		attachmentMgrSync.CreateAttachmentL(KUidlFile,file,attachment); // Takes ownership of attachment
		CleanupClosePushL(file);
		store->CommitL();
		CleanupStack::Pop(&file);
		}
	else
		{
		file=attachmentMgr.GetAttachmentFileForWriteL(0);
		}
	CleanupStack::PopAndDestroy(store);

	CDirectFileStore* index = CDirectFileStore::NewLC(file);
	const TUidType type(KDirectFileStoreLayoutUid,KNullUid,KMsgFileRemoteIdIndex);
	index->SetTypeL(type);
	RStoreWriteStream out;
	TStreamId id = out.CreateLC(*index);
	TInt entries = iUidlArray->Count();
	out.WriteInt32L(entries);
	for(TInt number = 0; number<entries; number++)	// must keep order, go forwards
		{
		out << (*iUidlArray)[number];
		}
	out.CommitL();                      // Commit stream : Is this needed?
	CleanupStack::PopAndDestroy();      // Destroy stream
	index->SetRootL(id);
	index->CommitL();                   // Commit store : I guess this is needed!
	CleanupStack::PopAndDestroy();      // Destroy store
	}

//
// Externalise/Internalise Uidl structure
//

void CImPop3RefreshOperation::TMsgUidlStore::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << *iPopId;
	aStream.WriteUint32L(iMsvId);
	aStream.WriteUint32L(iRemoteSize);
	}
//
// Sets the selection of entries to keep
//
void CImPop3RefreshOperation::SetMessagesToKeepL(const CMsvEntrySelection* aMessagesToKeep)
	{
	delete iMessagesToKeep;
	iMessagesToKeep = NULL;
	if (aMessagesToKeep)
		{
		iMessagesToKeep = aMessagesToKeep->CopyL();
		}
	}
//
// Check if ok to delete message
//
TBool CImPop3RefreshOperation::CanDeleteEntry(const TMsvEmailEntry& aEntry) const
	// Don't delete email if body exists or if it is an email that is being opened immediately after
	// the inbox refreshing (See defect PEN-5ESAWM)
	{
	// iMessagesToKeep may be Null
	if (iMessagesToKeep && iMessagesToKeep->Find(aEntry.Id()) != KErrNotFound)
		{
		return EFalse;
		}
	return ETrue;
	}
//
// Gets the size of the remote message
//
TUint CImPop3RefreshOperation::RemoteMessageSizeL(TMsvId aId)
	{
	TInt 	count	= iUidlArray->Count();
	TUint 	retVal 	= NULL;

	while( count-- )
		{
		const TMsgUidlStore &store=iUidlArray->At(count);
		if(store.iMsvId == aId)
			{
			// We have found a match, so return the size for this item
			retVal = store.iRemoteSize;
			break;
			}
		}

	if (!retVal)
		{
		// Leave if none of the MsvIds match
		User::Leave(KErrNotFound);
		}
	return retVal;
	}

// ****************************************************************************************** 
// This is called by the POP Client MTM when it starts Migrating Bearer
// 
// ******************************************************************************************
void CImPop3RefreshOperation::Pause()
	{
	// Set the Migration flag
	iMigratingToNewBearer = ETrue;
	}

// ****************************************************************************************** 
// This is called by the POP Client MTM when it starts Migrating Bearer
// 
// ******************************************************************************************	
void CImPop3RefreshOperation::CancelAllowResume()
	{
	// Cancel the copying of the current message  and decrement counters 
	// so we can restart from this message onwards when we have migrated.
	// Use the normal cancel, as we really need to cancel here.
	Cancel();		
	}
