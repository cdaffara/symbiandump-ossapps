// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "POPSMTM.H"

#include "POPSMBX.H"
#include "POPS.H"
#include "POPSOFFL.H"
#include <iapprefs.h>
// panics
#include "POPS.PAN"

#include <pop3set.h>
#include <miutset.h>	// internet mail settings
#include <miuthdr.h>
#include <offop.h>
#include <commdb.h>
#include <pop3cmds.h>
#include <imcvutil.h>
#include <cemailaccounts.h>
#include <tmsvsystemprogress.h>

#include "msventry.h"	// CMsvServerEntry
#include "POPSMTM.H"
#include "POPS.H"
#include "POPSOFFL.H"
#include "IAPPrefs.h"
#include "PopsDele.h"
#include "POPSRFSH.h"
#include "PopsCpMv.h"
#include "PopsTopPop.h"
#include "cpopsessionmanager.h"
#include "imutcon.h"
#include "cimmobilitymanager.h"
#include "mobilitytestmtmapi.h"

const TUid KUidPop3ServerMtm = {0x10003C77};

#if (defined SYMBIAN_USER_PROMPT_SERVICE) 
#include "cpopupsresponsewaiter.h"
#endif

//
// factory function
//
EXPORT_C CImppServerMtm* CImppServerMtm::NewL(CRegisteredMtmDll& aPopServerMtmDll,
											  CMsvServerEntry* aEntry) 
	{
	CImppServerMtm* popServerMtm=new CImppServerMtm(aPopServerMtmDll, aEntry);
	if (popServerMtm==NULL)
		{
		aPopServerMtmDll.ReleaseLibrary();
		User::Leave(KErrNoMemory);
		}
	CleanupStack::PushL(popServerMtm);
	popServerMtm->ConstructL();
	CleanupStack::Pop();
	return popServerMtm;
	}


	//
void CImppServerMtm::CopyFromLocalL(const CMsvEntrySelection& /*aSelection*/, TMsvId/* aDestination*/, TRequestStatus& /*aStatus*/)
	{
	User::Leave(KErrNotSupported);
	}

//
// IMPPCPMV
//
void CImppServerMtm::CopyToLocalL(const CMsvEntrySelection& aSelection,
                                  TMsvId aDestination,
                                  TRequestStatus& aStatus)
	{
	if (AcceptingOfflineOperationsL(aSelection))
		{
		AddOfflineOperationL(aSelection, aDestination, CImOffLineOperation::EOffLineOpCopyToLocal, aStatus);
		}
	else if (iMigrationState!=ENotMigrating && !iState.iRunningOfflineOperations)
		{
		// do not accept new operations if currently migrating
		User::Leave(KErrServerBusy);
		}
	else
		{
		DoCopyMoveL( aSelection, aDestination, aStatus, EImppCopy);
		}
	}

void CImppServerMtm::MoveToLocalL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus)
	{
	if (AcceptingOfflineOperationsL(aSelection))
		{
		AddOfflineOperationL(aSelection, aDestination, CImOffLineOperation::EOffLineOpMoveToLocal, aStatus);
		}
	else if (iMigrationState!=ENotMigrating && !iState.iRunningOfflineOperations)
		{
		// do not accept new operations if currently migrating
		User::Leave(KErrServerBusy);
		}
	else
		{
		DoCopyMoveL( aSelection, aDestination, aStatus, EImppMove);
		}
	}

//
// IMPPDELE
//
void CImppServerMtm::DeleteAllL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus)
	{
	if (iMigrationState!=ENotMigrating && !iState.iRunningOfflineOperations)
		{
		// do not accept new operations if currently migrating
		User::Leave(KErrServerBusy);
		}
		
	if (PruneMessages(aSelection, aStatus))
		{
		// Was this call to DeleteAllL an instruction to delete local parts of a message ?
		// If so then we don't need to continue.
		return;
		}
	
	if (iPopSettings == 0)
		{
		GetPopDetailsL(aSelection);
		}


	if ((iPopSettings->DisconnectedUserMode()) && (!iState.iQuitting))
		// If we are in disconnected mode (although not necessarily disconnected)
		// and we are not quitting then add the delete as an offline operation.
		{
		AddOfflineOperationL(aSelection, 0, CImOffLineOperation::EOffLineOpDelete, aStatus);
		}
	else
		// If we are not in disconnected mode or we are quitting then do the delete now.
		{
		__ASSERT_DEBUG(iConnectedToPopMbox, Panic(EPopNotConnectedToMbox));
		
		if(iConnectedToPopMbox==EFalse)
			{
			TRAP_IGNORE( DoShowMessagesL( EFalse ) );
			User::Leave(KErrDisconnected);
			}
		__ASSERT_DEBUG(iState.iCurrentOperation==EPopConnectedAndIdle,Panic(EImppAlreadyActive));
		
		if(iState.iCurrentOperation!=EPopConnectedAndIdle)
			{
			User::Leave(KErrInUse);
			}

		iReportStatus=&aStatus;
		Cancel();
		ResetProgress();

		__ASSERT_ALWAYS(iServerEntry, Panic(EPopNullPointer));
		// make sure iServerEntry is set to the service entry
		User::LeaveIfError(iServerEntry->SetEntry(iServiceId));

		CMsvEntrySelection* sel=StripInvalidEntriesLC(aSelection);
		// set selected entries invisible (hopefully so they wont be selected twice)
		TInt err=iServerEntry->ChangeAttributes( *sel, 0, KMsvVisibilityAttribute);
		if (err)
			{
			iServerEntry->ChangeAttributes( *sel,KMsvVisibilityAttribute,0);
			User::Leave(err);
			}
		delete iPopDelete;
		iPopDelete=NULL;
		iPopDelete=CImPop3Delete::NewL(*iServerEntry,*sel,iPopSession, iServiceId);
		CleanupStack::PopAndDestroy(); //sel
		iPopDelete->Start(iStatus);
		SetActive();
		aStatus = KRequestPending;
		iOperationActive=ETrue;
		iState.iCurrentOperation=EPopDeleting;
		}
	}

void CImppServerMtm::CopyWithinServiceL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus)
	{
	if (aDestination == iServiceId)
		// If the service id is given as the destination then the message selection is populated.
		{
		if (AcceptingOfflineOperationsL(aSelection))
			{
			AddOfflineOperationL(aSelection, aDestination, CImOffLineOperation::EOffLineOpCopyWithinService, aStatus);
			}
		else if (iMigrationState!=ENotMigrating && !iState.iRunningOfflineOperations)
			{
			// do not accept new operations if currently migrating
			User::Leave(KErrServerBusy);
			}
		else
			{
			DoCopyMoveL( aSelection, 0 /*ignored when populating*/, aStatus, EImppPopulate);	
			}
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}
	}

void CImppServerMtm::CreateL(TMsvEntry/* aNewEntry*/, TRequestStatus& /*aStatus*/)
	{
	User::Leave(KErrNotSupported);
	}

void CImppServerMtm::ChangeL(TMsvEntry aNewEntry, TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(iServerEntry, Panic(EPopNullPointer));
	User::LeaveIfError(iServerEntry->SetEntry( aNewEntry.Id() ));
	User::LeaveIfError(iServerEntry->ChangeEntry( aNewEntry ));
	
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CImppServerMtm::MoveFromLocalL(const CMsvEntrySelection& /*aSelection*/,TMsvId /*aDestination*/, TRequestStatus& /*aStatus*/)
	{
	User::Leave(KErrNotSupported);
	}

void CImppServerMtm::MoveWithinServiceL(const CMsvEntrySelection& /*aSelection*/,TMsvId /*aDestination*/, TRequestStatus& /*aStatus*/)
	{
	User::Leave(KErrNotSupported);
	}

//
// Connect and refresh mailbox or quit 
//
void CImppServerMtm::StartCommandL(CMsvEntrySelection& aSelection, TInt aCommand, const TDesC8& aParameter, TRequestStatus& aStatus)
	{

	if (iMigrationState!=ENotMigrating && aCommand!=KPOP3MTMDisconnect && !iState.iRunningOfflineOperations)
		{
		// do not accept new operations (except disconnect) if currently migrating
		User::Leave(KErrServerBusy);
		}
		
	switch(aCommand)
		{
	case KPOP3MTMConnect: // KPop3MtmConnectUID
		DoConnectL(aStatus, aSelection);
		MOBILITY_TEST_MTM_STATE(iServiceId, KMobilityTestMtmStatePopConnecting);
		break;
	case KPOP3MTMDisconnect:
		DoQuitL(aStatus);
		break;
	case KPOP3MTMCancelOfflineOperations:
		if (iPopSettings == 0)
			{
			GetPopDetailsL(aSelection);
			}
		MOBILITY_TEST_MTM_STATE(iServiceId, KMobilityTestMtmStatePopCancellingOfflineOps);
		CancelOfflineOperationsL(aSelection, aStatus);
		break;
	case KPOP3MTMPopulate:
		{
		GetPopDetailsL(aSelection);
		// unpackage the destination from aParameter
		TImPop3PopulateOptions info;
		TImPop3PopulateOptions::UnpackL(aParameter,info);			
		DoTopPopulateL(aSelection,info.PopulationLimit(),aStatus);
		}
		break;

	case KPOP3MTMIsConnected:
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	}

//
//
//
void CImppServerMtm::DoConnectL(TRequestStatus& aStatus, CMsvEntrySelection& aSelection)
	{
	__ASSERT_ALWAYS(iServerEntry, Panic(EPopNullPointer));
	if(IsActive())
		{
		User::Leave(KErrPop3ServerAlreadyConnected);
		}
	iNotConnectToPopMailBox = EFalse ;	
	// set this here so progress can be set
#if (defined SYMBIAN_USER_PROMPT_SERVICE) 	
	iState.iCurrentOperation = EPopAuthoriseAndConnect;
#else
	iState.iCurrentOperation = EPopConnecting;
#endif	
	ResetProgress();
	// POP3 settings
	// attempt to open POP3 service settings leave if we cant
	GetPopDetailsL(aSelection);

	delete iMessagesToKeep;
	iMessagesToKeep = NULL;
	iMessagesToKeep = aSelection.CopyL();
	
	iReportStatus=&aStatus;

	// Clear the new attribute on all entries if we are connecting
	User::LeaveIfError(iServerEntry->SetEntry(iServiceId));
	CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(selection);
	User::LeaveIfError(iServerEntry->GetChildren(*selection));  
	User::LeaveIfError(iServerEntry->ChangeAttributes(*selection, 0, KMsvNewAttribute));
	CleanupStack::PopAndDestroy(); // selection

	// create session manager
	if (!iSessionManager)
		{
		iSessionManager = CPopSessionManager::NewL();
		}

#if (defined SYMBIAN_USER_PROMPT_SERVICE) 	
	// Connect to UPS server and check if connection to remote server is permitted.
	iWaiter->AuthoriseAndConnectL(iPopSettings, iClientThreadId, iHasCapability, iStatus);
#else
	// obtain a connected pop session
	iSessionManager->GetSessionL(*iPopSettings, *iIAPPreferences, iPopSession, iStatus);
#endif
	iOperationActive=ETrue;
	SetActive();
	aStatus = KRequestPending;
	}


void CImppServerMtm::ResetProgress()
	{
	iPopProgress.iTotalMsgs=0;
	iPopProgress.iMsgsToProcess=0;
	iPopProgress.iBytesDone=0;
	iPopProgress.iTotalBytes=0;
	iPopProgress.iErrorCode=KErrNone;

	iPopProgressBuf.Zero();
	}

// utlity method - check state of mailbox and make sure it's raeady
void CImppServerMtm::CheckMailboxStateL()
	{
	__ASSERT_DEBUG(iConnectedToPopMbox, Panic(EPopNotConnectedToMbox));
	if(!iConnectedToPopMbox)
		{
		TRAP_IGNORE( DoShowMessagesL( EFalse ) );
		User::Leave(KErrDisconnected);
		}
	__ASSERT_DEBUG(iState.iCurrentOperation==EPopConnectedAndIdle,Panic(EImppAlreadyActive));
	if(iState.iCurrentOperation!=EPopConnectedAndIdle)
		{
		User::Leave(KErrInUse);
		}
	}


//
// Do the refresh
//
void CImppServerMtm::DoRefreshL()
	{
	__ASSERT_ALWAYS(iServerEntry, Panic(EPopNullPointer));
	ResetProgress();
	delete iMsvIdArray;
	iMsvIdArray=NULL;
	iMsvIdArray=new(ELeave)CArrayFixFlat<TMsvId>(8);
	// context of entry should be set to service entry
	User::LeaveIfError(iServerEntry->SetEntry(iServiceId));
	delete iPopRefreshMailbox;
	iPopRefreshMailbox=NULL;
	iPopRefreshMailbox = CImPop3RefreshMailBox::NewL(*iServerEntry, *iPopSession, iServerEntry->FileSession());
	iPopRefreshMailbox->SetMessagesToKeepL(iMessagesToKeep);
	iPopRefreshMailbox->Start(iStatus,iMsvIdArray);
	iOperationActive=ETrue;
	SetActive();
	iState.iCurrentOperation=EPopRefreshing;
	MOBILITY_TEST_MTM_STATE(iServiceId, KMobilityTestMtmStatePopRefreshing);
	}

void CImppServerMtm::DoCopyMoveL(const CMsvEntrySelection& aSelection, TMsvId aDestination, TRequestStatus& aStatus, TImppCopyMethod aCopyMethod)
	{
	__ASSERT_ALWAYS(iServerEntry, Panic(EPopNullPointer));
	CheckMailboxStateL();
	
	Cancel();
	iReportStatus=&aStatus;
	ResetProgress();

	User::LeaveIfError(iServerEntry->SetEntry(iServiceId));
	CMsvEntrySelection* sel=StripInvalidEntriesLC(aSelection);


	if( aCopyMethod == EImppMove)
		{
		// set selected entries invisible (hopefully so they wont be selected twice)
		TInt err=iServerEntry->ChangeAttributes(*sel, 0, KMsvVisibilityAttribute);
		if (err)
			{
			iServerEntry->ChangeAttributes(*sel,KMsvVisibilityAttribute,0);
			User::Leave(err);
			}
		}
	delete iPopCopyMove;
	iPopCopyMove=NULL;

	if (aCopyMethod == EImppPopulate)
		{
		iPopCopyMove = CImPop3CopyMove::NewL(*sel,*iServerEntry,iPopSession,ETrue,iServerEntry->FileSession(), iLogMessage, iPopSettings->DisconnectedUserMode());
		}
	else
		{
		iPopCopyMove = CImPop3CopyMove::NewL(*sel,*iServerEntry,iPopSession,(aCopyMethod == EImppCopy),iServerEntry->FileSession(),aDestination, iLogMessage, iPopSettings->DisconnectedUserMode());
		}

	CleanupStack::PopAndDestroy(); //sel
	
	switch (aCopyMethod)
		{
		case EImppCopy:
			iState.iCurrentOperation = EPopCopying;
			MOBILITY_TEST_MTM_STATE(iServiceId, KMobilityTestMtmStatePopCopying);
			break;
		case EImppMove:
			iState.iCurrentOperation = EPopMoving;
			MOBILITY_TEST_MTM_STATE(iServiceId, KMobilityTestMtmStatePopMoving);
			break;
		case EImppPopulate:
			iState.iCurrentOperation = EPopPopulating;
			MOBILITY_TEST_MTM_STATE(iServiceId, KMobilityTestMtmStatePopPopulating);
			break;
		default:
			User::Leave(KErrNotSupported);
			break;
		};

	iPopCopyMove->StartL(iStatus);

	SetActive();
	aStatus = KRequestPending;
	iOperationActive = ETrue;
	}

void CImppServerMtm::DoTopPopulateL(const CMsvEntrySelection& aSelection, TInt aLimit, TRequestStatus& aStatus)
	{
	// Deletion of service entry from entrySelection which is added at client
	CMsvEntrySelection* sel=StripInvalidEntriesLC(aSelection);
	if(sel->At(0) == iServiceId)
		{
		sel->Delete(0);
		}
		
	if ( aLimit==KErrNotFound )
		{
		// A full populate is required
		DoCopyMoveL(*sel,sel->At(0),aStatus,EImppPopulate);
		CleanupStack::PopAndDestroy(sel);
		return;
		}

	CheckMailboxStateL();

	Cancel();
	iReportStatus=&aStatus;
	ResetProgress();

	__ASSERT_ALWAYS(iServerEntry, Panic(EPopNullPointer));
	User::LeaveIfError(iServerEntry->SetEntry(iServiceId));

	RArray<TUint> sizes;
	CleanupClosePushL(sizes);

	TInt count=sel->Count();
	
	for(TInt i=0;i<count;i++)
		{
		User::LeaveIfError(sizes.Append(iPopRefreshMailbox->RemoteMessageSizeL(sel->At(i))));
		}

	delete iPopTopPop;
	iPopTopPop=NULL;

	iPopTopPop = CImPop3TopPopulate::NewL(*sel,*iServerEntry,aLimit,iPopSession,iServerEntry->FileSession(), iLogMessage, iPopSettings->DisconnectedUserMode(),sizes);
	CleanupStack::Pop(&sizes);

	CleanupStack::PopAndDestroy(sel);
	iState.iCurrentOperation = EPopTopPopulating;
	MOBILITY_TEST_MTM_STATE(iServiceId, KMobilityTestMtmStatePopTopPopulating);
	iPopTopPop->StartL(iStatus);
	SetActive();
	aStatus = KRequestPending;
	iOperationActive = ETrue;
	}

//
// quit
//
void CImppServerMtm::DoQuitL(TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(iServerEntry, Panic(EPopNullPointer));
	iReportStatus=&aStatus;
	
	// delete the mobility manager - we don't want to migrate during a disconnect
	delete iMobilityManager;
	iMobilityManager = NULL;
	
	if (!iConnectedToPopMbox)
		{
		aStatus=KRequestPending;
	 	CommandComplete(KErrNone);
		return;
		}	
	Cancel();
	// set connected to false immed. on calling QUIT
	User::LeaveIfError(iServerEntry->SetEntry(iServiceId));
	TMsvEntry entry = iServerEntry->Entry();
	entry.SetConnected( EFalse );
	User::LeaveIfError(iServerEntry->ChangeEntry(entry));	
	iState.iQuitting = ETrue;

  if (iMigrationState != ENotMigrating  &&
   		iMigrationState != EWaitingForOpToComplete &&
   		iMigrationState != EWaitingForOpToStop)
  	{
  	iMigrationState = ENotMigrating;
		iState.iCurrentOperation=EPopQuitting;
		ResetProgress();
		TRequestStatus *pS = &iStatus;
		User::RequestComplete(pS,KErrNone);
		SetActive();
		return;
  	}

	if(iState.iCurrentOperation != EPopConnectedAndIdle)	// stopping in mid operation so just die
		{
		SetActive();
		iState.iCurrentOperation=EPopDisconnected;
		ResetProgress();
		TRequestStatus *pS = &iStatus;
		User::RequestComplete(pS,KErrNone);
		}
	else	// quit gracefully
		{
		// Run any pending delete operations.
		// The quit operation will be started after this operation has completed.
		FindFirstOfflineOperationL(ETrue);
		}
	aStatus = KRequestPending;
	}

//
// Report some progess
//
const TDesC8& CImppServerMtm::Progress()
	{
	GetProgress();

	iPopProgressBuf=TPop3ProgressBuf(iPopProgress);
	return iPopProgressBuf;
	}

/** 
Obtain the progress information for POPS MTM
*/
void CImppServerMtm::GetProgress()
	{
	TBool progress = EFalse;
	TInt progressError;	
	if (iState.iRunningOfflineOperations)
		{
		const CImPop3OfflineOperationFinder::TOperationDetails operationDetails = iOfflineOpFinder->OperationDetails();

		switch (operationDetails.iOpType)
			{
			case CImOffLineOperation::EOffLineOpCopyToLocal:
			case CImOffLineOperation::EOffLineOpCopyWithinService:
			case CImOffLineOperation::EOffLineOpMoveToLocal:
				if (iPopCopyMove)
					{
					iPopProgress = iPopCopyMove->Progress();
					}
				break;
			case CImOffLineOperation::EOffLineOpDelete:
				iPopProgress.iPop3Progress = TPop3Progress::EPopDeleting;
				break;
			default:
				break;
			}

		iPopProgress.iMsgsToProcess = operationDetails.iOperationsOfType - (operationDetails.iOperationNumber + 1);
		iPopProgress.iTotalMsgs = operationDetails.iOperationsOfType;
		}
	else
		{
		TPopsMtmState state;
		// If there has been an error which has cause disconnection then we are interested
		// in the state that caused the failure rather than the Disconnected state. Therefore
		// we need to report the iState.ILastCurrentOperation in this case.
		if ((iPopProgress.iErrorCode != KErrNone) && (iState.iCurrentOperation == EPopDisconnected))
			{
			state = iState.iLastCurrentOperation;
			}
		else
			{
			state = iState.iCurrentOperation;
			}

		switch(state)
			{
		case EPopAuthoriseAndConnect:
			{
			iPopProgress.iPop3Progress = TPop3Progress::EPopConnecting;
			iPopProgress.iTotalMsgs = KErrNotFound;
			iPopProgress.iTotalBytes = KErrNotFound;
			break;
			}

		case EPopConnecting:
			{
			iPopProgress.iPop3Progress = TPop3Progress::EPopConnecting;

			// Check that session manager exists and get the progress information
			// from it if it does.
			// Under some circumstances the session manager might have been deleted
			// such as when a the networking bearer mobility framework has indicated
			// an error that means the RConnection is invalid.
			if (iSessionManager)
				{
				iSessionManager->ConnectionProgress(iPopProgress);
				}
			else
				{
				iPopProgress.iTotalMsgs = KErrNotFound;
				iPopProgress.iTotalBytes = KErrNotFound;			
				}
			break;
			}

		case EPopFindingFirstOfflineOp:
			iPopProgress.iPop3Progress = TPop3Progress::EPopConnecting;
			break;
		case EPopRefreshing:
			progressError = iPopProgress.iErrorCode;
			if(progressError != KErrNone)
				{
				progress = ETrue;
				}								
			if(iPopRefreshMailbox!=NULL)
				{
				iPopProgress=iPopRefreshMailbox->Progress();
 				}
			if(progress)
				{	
				iPopProgress.iErrorCode = progressError;
				}		
			iPopProgress.iPop3Progress = TPop3Progress::EPopRefreshing;
			break;
		case EPopTidying: 
			if(iPopRefreshMailbox!=NULL)
				{
				iPopProgress=iPopRefreshMailbox->Progress();
				}
			iPopProgress.iPop3Progress = TPop3Progress::EPopRefreshing;
			break;
		case EPopConnectedAndIdle:
			if (iState.iLastCurrentOperation==EPopAddingOfflineOp || iState.iLastCurrentOperation==EPopCancellingOfflineOps)
				{
				iOfflineOpSetter->Progress(iPopProgress);
				}
			if (iState.iLastCurrentOperation==EPopPopulating || iState.iLastCurrentOperation==EPopMoving || iState.iLastCurrentOperation==EPopCopying )
				{
				if(iPopCopyMove)
					{
					iPopProgress=iPopCopyMove->Progress();
					}
				else
					{
					iPopProgress.iMsgsToProcess = 0;
					}
				}
			break;
		case EPopCopying:
			if(iPopCopyMove)
				{
				iPopProgress=iPopCopyMove->Progress();
				}
			iPopProgress.iPop3Progress = TPop3Progress::EPopCopying;
			break;
		case EPopPopulating:
			if(iPopCopyMove)
				{
				iPopProgress=iPopCopyMove->Progress();
				}
			iPopProgress.iPop3Progress = TPop3Progress::EPopPopulating;
			break;
		case EPopTopPopulating:
			if(iPopTopPop)
				{
				iPopProgress=iPopTopPop->Progress();
				}
			iPopProgress.iPop3Progress = TPop3Progress::EPopTopPopulating;
			break;
		case EPopMoving:
			if(iPopCopyMove)
				{
				iPopProgress=iPopCopyMove->Progress();
				}
			iPopProgress.iPop3Progress = TPop3Progress::EPopMoving;
			break;
		case EPopDeleting:
			if(iPopDelete)
				{
				iPopProgress=iPopDelete->Progress();
				}
			iPopProgress.iPop3Progress = TPop3Progress::EPopDeleting;
			break;
		case EPopQuitting:
			iPopProgress.iPop3Progress = TPop3Progress::EPopDisconnecting;
			break;
		case EPopDisconnected:
			iPopProgress.iPop3Progress = TPop3Progress::EPopDisconnected;
			break;
		case EPopAddingOfflineOp:
		case EPopCancellingOfflineOps:
			iOfflineOpSetter->Progress(iPopProgress);
			break;
		default:
			break;
			}

		if ((state != EPopConnecting || state != EPopAuthoriseAndConnect) && iPopProgress.iMsgsToProcess > iPopProgress.iTotalMsgs)
			{
			iPopProgress.iMsgsToProcess = iPopProgress.iTotalMsgs;
			}

		if (iPopProgress.iMsgsToProcess<0)
			{
			iPopProgress.iMsgsToProcess = 0;
			}
		}
		iPopProgress.iServiceId = iServiceId;
}

/** 
This call leads to calling GetProgress() to populate the TPop3Progress structure.
@param aOutSysProg The TMsvSystemProgress structure to be populated
*/
void CImppServerMtm::GetSystemProgress(TMsvSystemProgress& aOutSysProg)
	{
	GetProgress();
	aOutSysProg.iErrorCode = iPopProgress.iErrorCode;
	}

/** 
The extension method provides a polymorphic behaviour to call the correct
MTM.
@param aExtensionId The Uid passed in as KUIDMsgMsvSystemProgress to obtain the
System Progress.
@return KErrNone if GetSystemProgress is called successfully.
*/
TInt CImppServerMtm::Extension_(TUint aExtensionId, TAny *&a0, TAny *a1)	
	{
	switch(aExtensionId)
		{
		case KUIDMsgMsvSystemProgress:
			{
			TMsvSystemProgress* systemProgress = reinterpret_cast<TMsvSystemProgress*>(a1);
			GetSystemProgress(*systemProgress);
			return KErrNone;
			}

#if (defined SYMBIAN_USER_PROMPT_SERVICE)
		case KUIDMsgClientThreadInfo:
			{
			iClientThreadId = *(TThreadId*) (a1);
	    	iHasCapability = (TBool)*(TInt*)(a0);
	    	return KErrNone;
			}
#endif

		case KUidMsgNonOperationMtmData:
			{
			TNonOperationMtmDataType* mtmDataType = reinterpret_cast<TNonOperationMtmDataType*>(a0);
			TPtrC8* mtmDataBuffer = reinterpret_cast<TPtrC8*>(a1);
			return GetNonOperationMtmData(*mtmDataType, *mtmDataBuffer);
			}

		default:
			{
			// Chain to base class
			return CBaseServerMtm::Extension_(aExtensionId, a0, a1);
			}
		}
	}

//
// return current state of connection?
//
 TBool CImppServerMtm::CommandExpected()
	{
	return iConnectedToPopMbox;
	}

//
// Special cancel for migration purposes. 
// Stops the current operation without completing the user/client request.
// The cancelled operation may then be restarted once migration to a new 
// bearer has completed.
//
void CImppServerMtm::CancelToMigrate()
	{
	iCancelForBMMigration = ETrue;
	Cancel();
	iCancelForBMMigration = EFalse;
	}

//
// Implements the Cancel(). Unless called via the special method for handling 
// cancelling for migration (above), calling DoCancel will cancel the user
// requested operation, delete the pop session, mark the server offline and 
// complete the user request if outstanding.
//
// If called via the special method, the server is not marked offline, and 
// user requests are not completed.
//
void CImppServerMtm::DoCancel()
	{
	// Cancel current outstanding request according to current state...
	switch (iMigrationState)
		{
		case ENotMigrating:
		case EWaitingForOpToStop:
		case EWaitingForOpToComplete:
			{
			// Cancels the current operation according to iState.iCurrentOperation
			// If cancelling for bearer migration, operations are able to be resumed.
			DoCancelCurrentOp();
			break;
			}
		case EDisconnectingForMigrate:
		case EConnectingAfterMigrate:
			{
			// Cancel the connect/disconnect on the session manager
			iSessionManager->Cancel();
			break;
			}
		case EWaitingForNewCarrier:
		case EWaitingCarrierRejected:
			{
			// self-induced dummy wait state on the mobility manager
			// Cancel it:
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrCancel);
			break;
			}
		default:
			{
			break;
			}
		}

	// Final tidying for non-migration caused cancels
	if (!iCancelForBMMigration)
		{
		if(iState.iCurrentOperation != EPopConnectedAndIdle)
			{
			iState.iCurrentOperation = EPopDisconnected;
			TRAP_IGNORE(DoShowMessagesL( EFalse ) );
			
			// delete the mobility manager	
			delete iMobilityManager;
			iMobilityManager = NULL;

			TInt err = iServerEntry->SetEntry(iServiceId);
			if (err == KErrNone)
				{
				TMsvEntry entry = iServerEntry->Entry();
				entry.SetConnected( EFalse );
				iServerEntry->ChangeEntry( entry );	// ignore any error
				}
			}
		
		if(iOperationActive)
			{
			// Complete the client request now
			CommandComplete(KErrCancel);
			}
		}
	}


/**
Cancel according to iState.iCurrentOperation

If the cancel is to allow a migration, the operation object in use
is cancelled, but not deleted. This means it is possible to restart it
later.
*/
void CImppServerMtm::DoCancelCurrentOp()
	{	
	switch(iState.iCurrentOperation)
		{
		case EPopAuthoriseAndConnect:
			{
#if (defined SYMBIAN_USER_PROMPT_SERVICE)
			iWaiter->Cancel();
#endif
			break;
			}

		case EPopDisconnected:
		case EPopTidying: 		// self completed state
			{
			break;
			}
		case EPopConnecting:
		case EPopQuitting:
			{
			__ASSERT_ALWAYS(iSessionManager, Panic(EPopNullPointer));
			iSessionManager->Cancel();
			break;
			}
		case EPopConnectedAndIdle:
			{
			__ASSERT_ALWAYS(iPopSession, Panic(EPopNullPointer));
			iPopSession->Cancel();
			break;
			}
		case EPopRefreshing:
			{
			if (iCancelForBMMigration)
				{
				iPopRefreshMailbox->CancelAllowResume();
				}
			else
				{
				iPopRefreshMailbox->Cancel();
				}
			delete iPopSession;
			iPopSession = NULL;
			break;
			}
		case EPopCopying:
		case EPopMoving:
		case EPopPopulating:
			{
			if (iCancelForBMMigration)
				{
				iPopCopyMove->CancelAllowResume();
				}
			else
				{
				delete iPopCopyMove;
				iPopCopyMove=NULL;
				}
			delete iPopSession;
			iPopSession = NULL;
			break;
			}
		case EPopTopPopulating:
			{
			if (iCancelForBMMigration)
				{
				iPopTopPop->CancelAllowResume();
				}
			else
				{
				delete iPopTopPop;
				iPopTopPop=NULL;
				}
			delete iPopSession;
			iPopSession = NULL;
			break;
			}
		case EPopDeleting:
			{
			if (iCancelForBMMigration)
				{
				iPopDelete->CancelAllowResume();
				}
			else
				{
				delete iPopDelete;
				iPopDelete=NULL;
				}
			delete iPopSession;
			iPopSession = NULL;
			break;
			}
		case EPopAddingOfflineOp:
			{
			iOfflineOpSetter->Cancel();
			break;
			}
		case EPopFindingFirstOfflineOp:
			{
			iOfflineOpFinder->Cancel();
			break;
			}
		case EPopCancellingOfflineOps:
			{
			if (iCancelForBMMigration)
				{
				// RJS: update this Cancel() to enable resume
				iOfflineOpSetter->Cancel();
				}
			else
				{
				iOfflineOpSetter->Cancel();
				}
			break;
			}
		}
	}

//
// 
//
void CImppServerMtm::DoRunL()
	{
	TInt err = KErrNone;
	TInt eCode=iStatus.Int();
	iState.iLastCurrentOperation = iState.iCurrentOperation;
	TBool quitSuccessfully = EFalse;
	TPop3Progress progress;
	
   	if (iMigrationState != ENotMigrating  &&
   		iMigrationState != EWaitingForOpToComplete &&
   		iMigrationState != EWaitingForOpToStop)
		{
		if (DoMigrationRunL()) 
			{
			return;
			}
		}

	switch(iState.iCurrentOperation)
		{
	case EPopAuthoriseAndConnect:
		{
		if (eCode == KErrNone)
			{
			iState.iCurrentOperation = EPopConnecting;
			// obtain a connected pop session
			iSessionManager->GetSessionL(*iPopSettings, *iIAPPreferences, iPopSession, iStatus);
			SetActive();
			return;
			}
		break;
		}

	case EPopConnecting:
		{
		// Register for bearer mobility,
		if (StartBearerMobilityL(eCode))
			{
			// Return now if the initial carrier has been rejected.
			return;
			}

		if (eCode == KErrNone)
			{
			__ASSERT_ALWAYS(iServerEntry, Panic(EPopNullPointer));
			// The POP session is now connected - update the connected flag.
			User::LeaveIfError( iServerEntry->SetEntry(iServiceId) );
			TMsvEntry entry;
			entry = iServerEntry->Entry();
			entry.SetConnected( ETrue );
			User::LeaveIfError( iServerEntry->ChangeEntry(entry) );

			// if we've connected OK start the refresh
			if (iPopSession->MaxHeaders()==0)
				{
				iState.iCurrentOperation=EPopQuitting;
				CommandComplete(KErrNone);
				}
			else
				{
				TRAP_IGNORE( DoRefreshL() );
				}
			}
		break;
		}
	case EPopRefreshing:
		{
		// Check if stopping for migrate
		if(iMigrationState == EWaitingForOpToStop && eCode == KErrNone)
			{
			StoreConfigurationToMigrateL();
			// We were waiting on this op to stop before we migrate.
			// so start disconnecting for migration now
			DisconnectForMigrate();
			return;
			}
		
		// tidy up any dead messages
		// should only be necessary if the refresh has failed
		if(iMsvIdArray->Count())
			{
			iArrayCtr=iMsvIdArray->Count();
			iState.iCurrentOperation=EPopTidying;
			MOBILITY_TEST_MTM_STATE(iServiceId, KMobilityTestMtmStatePopTidying);
			QueueRemoteCleanup();
			}
		else
			{
			if(eCode==KErrNone)
				{
				TRAP(eCode,DoShowMessagesL(ETrue));
				}
			iState.iQuitting = EFalse;
			if (eCode == KErrNone)
				{
				FindFirstOfflineOperationL(EFalse);
				}
			}
		break;
		}
	case EPopTidying:
		{
		if(--iArrayCtr>=0)
			{		
			__ASSERT_ALWAYS(iServerEntry, Panic(EPopNullPointer));
			//  No appropriate error handling here...
			//  Refresh has failed: need to remove all of the partially
			//  loaded headers etc..
			err = iServerEntry->DeleteEntry((*iMsvIdArray)[iArrayCtr]);
			__ASSERT_DEBUG(err == KErrNone, Panic(EPopFailedDebugAssert));
			QueueRemoteCleanup();
			}
		else
			{
			if(iMigrationState == EWaitingForOpToStop && eCode == KErrNone)
				{
				// Stopping for migrate. RestartAfterMigrateL() calls
				// FindFirstOfflineOperationL(EFalse) if restarting in
				// state EPopFindingFirstOfflineOp, so set it up and go.
				iState.iCurrentOperation = EPopFindingFirstOfflineOp;
				StoreConfigurationToMigrateL();
				DisconnectForMigrate();
				return;
				}
			else
				{
				FindFirstOfflineOperationL(EFalse);
				}
			}
		break;
		}
	case EPopFindingFirstOfflineOp:
		{
		if(iMigrationState == EWaitingForOpToStop && eCode == KErrNone)
			{
			// Stopping for migrate. RestartAfterMigrateL() calls
			// FindFirstOfflineOperationL(EFalse) if restarting in
			// state EPopFindingFirstOfflineOp. This is a little
			// inefficient, but avoids adding an extra state.
			iState.iCurrentOperation = EPopFindingFirstOfflineOp;
			StoreConfigurationToMigrateL();
			DisconnectForMigrate();
			return;
			}
				
		if (iOfflineOpFinder->OperationFound())
			{
			// A pending offline operation has been found, so run it.
			RunOfflineOperationL();
			}
		else
			// There are no more pending operations.
			{
			iState.iRunningOfflineOperations = EFalse;
			if (iState.iQuitting)
				{
				// The offline operations are being run as part of the quitting process.
				// As these operations have now been run the quitting process should be resumed.
				iState.iCurrentOperation=EPopQuitting;
				ResetProgress();
				if (iSessionManager && iPopSession)
					{
					iSessionManager->DeleteSession(*iPopSession, iStatus);
					MOBILITY_TEST_MTM_STATE(iServiceId, KMobilityTestMtmStatePopQuitting);
					iPopSession=NULL;
					}
				iOperationActive=ETrue;
				SetActive();
				}
			else
				{
				// The offline operations are being run as part of the connection process.
				// As these operations have now been run we are now connected.
				iState.iCurrentOperation=EPopConnectedAndIdle;
				iOperationActive=EFalse;
				}
			}
		break;
		}
	case EPopCopying:
	case EPopPopulating:
	case EPopMoving:
		{
		// Check if we are migrating
		if(iMigrationState == EWaitingForOpToStop && eCode == KErrNone)
			{
			// if the operation is incomplete, start migration here. Otherwise
			// allow the user request to be completed, and handle migration
			// as if the request was to complete op for migrate. 
			progress = iPopCopyMove->Progress();
			if (progress.iMsgsToProcess)
				{
				StoreConfigurationToMigrateL();
				DisconnectForMigrate();
				return;
				}
			}

		if (iState.iRunningOfflineOperations && iState.iCurrentOperation != EPopMoving)
			// The operation was a pending offline operation and should be deleted.
			{
			__ASSERT_ALWAYS(iServerEntry, Panic(EPopNullPointer));
			CImPop3OfflineUtilities::DeleteL(iOfflineOpFinder->OfflineOperation(), *iServerEntry);
			}

		__ASSERT_ALWAYS(iServerEntry, Panic(EPopNullPointer));
		delete iPopCopyMove;
		iPopCopyMove=NULL;
		iState.iCurrentOperation=EPopConnectedAndIdle;
		iOperationActive=EFalse;

		// reset server entry context
		err = iServerEntry->SetEntry( iServiceId );
		if(err != KErrNone)
			{
			CommandComplete( err );
			return;
			}	// should disconnect

		if ((iState.iRunningOfflineOperations) && (eCode == KErrNone))
			{
			iOfflineOpFinder->FindNext();
			// if we are stopping for migrate and there is another operation to perform,
			// allow the migration to happen now. Otherwise, allow the operation to finish.
			if (iMigrationState == EWaitingForOpToStop && iOfflineOpFinder->OperationFound())
				{
				StoreConfigurationToMigrateL();
				DisconnectForMigrate();
				return;			
				}
			RunOfflineOperationL();
			}			
		break;
		}
	case EPopTopPopulating:
		{
		__ASSERT_ALWAYS(iServerEntry, Panic(EPopNullPointer));
		// Check if we are migrating		
		if(iMigrationState == EWaitingForOpToStop && eCode == KErrNone)
			{
			// if the operation is incomplete, start migration here. Otherwise
			// allow the user request to be completed, and handle migration
			// as if the request was to complete op for migrate. 
			progress = iPopTopPop->Progress();
			if (progress.iMsgsToProcess)
				{
				StoreConfigurationToMigrateL();
				DisconnectForMigrate();
				return;
				}
			}

		if (iState.iRunningOfflineOperations)
			// The operation was a pending offline operation and should be deleted.
			{
			CImPop3OfflineUtilities::DeleteL(iOfflineOpFinder->OfflineOperation(), *iServerEntry);
			}

		delete iPopTopPop;
		iPopTopPop=NULL;
		iState.iCurrentOperation=EPopConnectedAndIdle;
		iOperationActive=EFalse;

		// reset server entry context
		err = iServerEntry->SetEntry( iServiceId );
		if(err != KErrNone)
			{
			CommandComplete( err );
			return;
			}	// should disconnect

		if ((iState.iRunningOfflineOperations) && (eCode == KErrNone))
			{
			iOfflineOpFinder->FindNext();
			// if we are stopping for migrate and there is another operation to perform,
			// allow the migration to happen now. Otherwise, allow the operation to finish.
			if (iMigrationState == EWaitingForOpToStop && iOfflineOpFinder->OperationFound())
				{
				StoreConfigurationToMigrateL();
				DisconnectForMigrate();
				return;			
				}
			RunOfflineOperationL();
			}
		break;
		}
	case EPopDeleting:
		{
		// Check if we are migrating
		if(iMigrationState == EWaitingForOpToStop && eCode == KErrNone)
			{
			// if the operation is incomplete, start migration here. Otherwise
			// allow the user request to be completed, and handle migration
			// as if the request was to complete op for migrate. 
			progress = iPopDelete->Progress();
			if (progress.iMsgsToProcess)
				{
				StoreConfigurationToMigrateL();
				DisconnectForMigrate();
				return;
				}
			}

		delete iPopDelete;
		iPopDelete=NULL;

		iState.iCurrentOperation=EPopConnectedAndIdle;
		iOperationActive=EFalse;
		if ((iState.iRunningOfflineOperations) && (eCode == KErrNone))
			{
			iOfflineOpFinder->FindNext();
			// if we are stopping for migrate and there is another operation to perform,
			// allow the migration to happen now. Otherwise, allow the operation to finish.
			if (iMigrationState == EWaitingForOpToStop && iOfflineOpFinder->OperationFound())
				{
				StoreConfigurationToMigrateL();
				DisconnectForMigrate();
				return;			
				}
			RunOfflineOperationL();
			}
		break;
		}
	case EPopQuitting:
		{
		quitSuccessfully = ETrue;
		break;
		}
	case EPopDisconnected:
		{
		// no change of state
		break;
		}
	case EPopAddingOfflineOp:
	case EPopCancellingOfflineOps:
		{
		iOperationActive = EFalse;
		if (!iConnectedToPopMbox)
			{
			iState.iCurrentOperation=EPopDisconnected;
			if(iNotConnectToPopMailBox)
				{
				CommandComplete(KErrDisconnected);
				iNotConnectToPopMailBox = EFalse ;
				}
			else
				{
				CommandComplete(KErrNone);
				}
			}
		else
			{
			if (eCode == KErrNotFound)
				{
				eCode = KErrNone;
				}
			iState.iCurrentOperation = EPopConnectedAndIdle;
			}
		break;
		}
	default:
		{
		iState.iCurrentOperation=(eCode==KErrNone) ? EPopConnectedAndIdle : EPopDisconnected;
		break;
		}
		} // end of switch (iState.iCurrentOperation)

	// signal completion unless we're doing the refresh or tidying
	if( quitSuccessfully || eCode!=KErrNone )
		{
		__ASSERT_ALWAYS(iServerEntry, Panic(EPopNullPointer));
		if (iPopSession)
			{			
			delete iPopSession;
			iPopSession=NULL;
			}
		iState.iQuitting = EFalse;
		
		// delete the mobility manager	
		delete iMobilityManager;
		iMobilityManager = NULL;
		
		TRAP_IGNORE( DoShowMessagesL(EFalse) );
		if( iState.iCurrentOperation != EPopQuitting)
			{
			TInt err = iServerEntry->SetEntry( iServiceId );
			if(err == KErrNone)
				{
				TMsvEntry entry = iServerEntry->Entry();
				entry.SetConnected( EFalse );
				iServerEntry->ChangeEntry( entry );	// ignore any error
				}
			}
		if(!(iState.iLastCurrentOperation == EPopConnectedAndIdle && iState.iCurrentOperation == EPopDisconnected) ) 
			{	
			CommandComplete( eCode );
			}
		else if(iState.iLastCurrentOperation == EPopConnectedAndIdle && iState.iCurrentOperation == EPopDisconnected && eCode != KErrNone)
		    {
            // Some email servers doesn’t wait for QUIT command, in such cases need to close RConnection
            delete iSessionManager; 
            iSessionManager = NULL;
			
            iPopProgress.iErrorCode = eCode;
		    }
		else
			{
			iPopProgress.iErrorCode = eCode;
			}
		iServerEntry->SetEntry(KMsvNullIndexEntryId); // when it times out the server MTM doesn't get deleted and henc the iServerEntry is locked and cannot do anythig withit.  If you set it to NULL it will solve the problem
		iState.iCurrentOperation = EPopDisconnected;
		return;
		}
		
	if(iReportStatus == NULL && IsActive())
		{
		// we are in an inconsistent state and should leave.
		// since we want to complete the request and return to the client,
		// we LEAVE with KErrUnknown. The completion of the request is done in the TRAP
		// of the ServiceL method, where DoComplete is called.
		User::Leave(KErrUnknown);
		}	
	
	
	// set pop session idle waiting
	if( iState.iCurrentOperation == EPopConnectedAndIdle && iConnectedToPopMbox
		&& iState.iCurrentOperation != iState.iLastCurrentOperation)
		{
		// Complete the waiting iStatus. Note this object stays active so that 
		// it can react to any connection failure, e.g. time-out
		CommandComplete( eCode );
		if(	(iMigrationState == EWaitingForOpToComplete) ||
			(iMigrationState == EWaitingForOpToStop) )
			{
			// We were waiting on this op to complete before we migrate.
			// so start disconnecting for migration now
			DisconnectForMigrate();
			}
		else
			{
			iPopSession->Waiting(iStatus);
			SetActive();
			MOBILITY_TEST_MTM_STATE(iServiceId, KMobilityTestMtmStatePopConnectedAndIdle);
			}
		}
	}
	
/**
Stores configuration to be re-used post Migration.
*/ 
void CImppServerMtm::StoreConfigurationToMigrateL()
	{
	iSavedValuesForMigration.iMessageArray 	= iPopSession->MessageArray();
	iSavedValuesForMigration.iNumMessages 	= iPopSession->GetNumMessages();

	TInt32* IdTab = new(ELeave) TInt32[iSavedValuesForMigration.iNumMessages];
	// The copying of Array here is very crude and in-efficient
	// Need to use an arraytype that has a copy constructor
	for (TInt count=0; count < iSavedValuesForMigration.iNumMessages; count++)
		{
		IdTab[count] = iSavedValuesForMigration.iMessageArray[count];
		}	
	iSavedValuesForMigration.iMessageArray = IdTab;
	iSavedValuesForMigration.iValuesSaved = ETrue;
	}

/**
Handles the migration state machine states.  
This is called the Pop ServerMTM doRunL() routine.

@return ETrue if the state has been handled and DoRunL can return
*/
TBool CImppServerMtm::DoMigrationRunL()
	{
	TBool migrationHandled = ETrue;

	// We are migrating so handle the migration states here
	switch (iMigrationState)
		{
		case EDisconnectingForMigrate:
			{
			// Old session disconnected, tell the MM we are ready to migrate.
			iMigrationState = EWaitingForNewCarrier;
			iMobilityManager->MigrateToNewCarrier();
			// Set dummy request pending, unless state has been changed by MigrateToNewCarrier()
			if (iMigrationState == EWaitingForNewCarrier)
				{
				iStatus = KRequestPending;
				SetActive();
				}
			break;
			}
		case EWaitingForNewCarrier:
		case EWaitingCarrierRejected:
			{
			// Migration has completed, obtain a new connected POP session
			iMigrationState = EConnectingAfterMigrate;
			iSessionManager->GetSessionL(*iPopSettings, *iIAPPreferences, iPopSession, iStatus);
			SetActive();
			MOBILITY_TEST_MTM_STATE(iServiceId, KMobilityTestMtmStatePopConnecting);
			break;
			}
		case EConnectingAfterMigrate:
			{
			if (iStatus.Int()!=KErrNone)
				{
				// An error has occurred while attempting to re-connect
				// - reject this new carrier, wait to see if a new one turns up.
				iMigrationState = EWaitingCarrierRejected;
				iMobilityManager->NewCarrierRejected();

				// Now in a waiting state, set self active
				iStatus = KRequestPending;
				SetActive();
				}
			else
				{
				// We have a new pop session, set the state and tell the mobility manager
				iMigrationState = ENotMigrating;
				iMobilityManager->NewCarrierAccepted();
				// restart the pop server 
				RestartAfterMigrateL();
				}
			break;
			}
		default:
			{
			__ASSERT_DEBUG(EFalse, Panic(EPopUnexpectedMigrateState));
			migrationHandled = EFalse;
			break;
			}
		}
	return migrationHandled;
	}

/**
Utility function to delete the Pop Session
note the Session Manager DeleteSession() request is asynchronous.
*/
void CImppServerMtm::DisconnectForMigrate()
	{
	// Call the session manager to delete the session
	iMigrationState = EDisconnectingForMigrate;
	iSessionManager->DeleteSession(*iPopSession, iStatus);
	MOBILITY_TEST_MTM_STATE(iServiceId, KMobilityTestMtmStatePopQuitting);
	iPopSession = NULL;
	SetActive();
	}
	
/**
Restarts any pre-migration outstanding operations, or set the state machine into the same
state it was pre-migration along with any necessary connections and set ups.

Note that iCurrentOperation will have returned to EPopConnectedAndIdle if the migration type
was "allow complete" or "stop for migrate" and the operation came to a natural finish.

If iCurrentOperation is not EPopConnectedAndIdle, it indicates that the operation had not
completed prior to the migration. This is caused with by an "accept immediately" migration,
a CarrierLost() migration, or a "stop operation" migration for which the operation did not
come to a natural completion. In these cases, the operation in progress needs to be resumed.
*/
void CImppServerMtm::RestartAfterMigrateL()
	{
	if (iSavedValuesForMigration.iValuesSaved)
		{
		// Ownership of MessageArray is passed to the PopSession here
		iPopSession->SetMessageArray(iSavedValuesForMigration.iMessageArray, iSavedValuesForMigration.iNumMessages);
		iSavedValuesForMigration.iValuesSaved = EFalse;
		}

	// switch on pre-migrate state
	switch(iState.iCurrentOperation)
		{
		case EPopConnecting: // in this state if initial carrier rejected...
		case EPopTidying:
			{
			// Self complete to return to the State Machine
			iStatus = KRequestPending;
			SetActive();
			TRequestStatus *status = &iStatus;
			User::RequestComplete(status,KErrNone);
			break;
			}
		case EPopConnectedAndIdle:
			{
			if (iState.iRunningOfflineOperations)
				{
				// can be in this state if the offline operations were stopped before all
				// were finished.
				RunOfflineOperationL();
				}
			else
				{
				// No operation in progress at time of migration, re-enter idle waiting state
				iPopSession->Waiting(iStatus);
				SetActive();
				MOBILITY_TEST_MTM_STATE(iServiceId, KMobilityTestMtmStatePopConnectedAndIdle);
				}
			break;
			}
		// Refresh, Copy, Move, Delete, Populate - these operations need to be 
		// resumed if they were cancelled or stopped before completing.
		// Note that if the operation had actually completed, 
		// iCurrentOperation would now be EPopConnectedAndIdle.
		case EPopRefreshing:
			{
			// Resume the refresh operation:
			iPopRefreshMailbox->ResumeL(iPopSession, iStatus);
			SetActive();
			break;
			}
		case EPopCopying:
		case EPopMoving:
		case EPopPopulating:
			{
			// Resume the copy/move operation
			iPopCopyMove->ResumeL(iPopSession, iStatus);
			iStatus = KRequestPending;
			SetActive();
			break;
			}
		case EPopDeleting:
			{
			// Resume the delete operation
			iPopDelete->ResumeL(iPopSession, iStatus);
			SetActive();
			break;
			}
		case EPopTopPopulating:
			{
			iPopTopPop->ResumeL(iPopSession, iStatus);
			SetActive();
			break;
			}
		case EPopFindingFirstOfflineOp:
			{
			// This can be the current operation either during connect or
			// disconnect. However, we will not migrate during a disconnect,
			// so we can safely assume this is during connect.
			// This is an asynchronous operation that builds an array of 
			// offline requested operations that need to be performed once
			// connection has been established. Probably best to rebuild
			// the list now.
			User::LeaveIfError(iServerEntry->SetEntry(iServiceId));
			FindFirstOfflineOperationL(EFalse); // calls SetActive()
			break;
			}
		case EPopCancellingOfflineOps:
			{
			// nothing to stop this action when connected, therefore must be able
			// to handle it having been cancelled.
			iOfflineOpSetter->ResumeCancelOfflineOperationsL(iStatus);
			break;
			}
		case EPopAuthoriseAndConnect:
		case EPopQuitting:			// we shouldn't ever get a migration when quitting
		case EPopAddingOfflineOp:	// disconnected operation
		case EPopDisconnected:		// disconnected operation
		default:
			{
			__ASSERT_ALWAYS(EFalse, Panic(EPopRestartUnexpectedOp));
			break;
			}
		}
	
	}

//
//
//
void CImppServerMtm::DoComplete(TInt aError)
	{
	CommandComplete( aError );	
	}

//
// Open up POP3 settings store
//
void CImppServerMtm::GetPopDetailsL(const CMsvEntrySelection& aSel)
	{
	__ASSERT_ALWAYS(iServerEntry, Panic(EPopNullPointer));
	delete iPopSettings;
	iPopSettings=NULL;
	iPopSettings=new(ELeave)CImPop3Settings;
	delete iIAPPreferences;
	iIAPPreferences = NULL;
	iIAPPreferences = CImIAPPreferences::NewLC();
	CleanupStack::Pop(); // doesn't need to be on the stack!
	// check our entry is the service entry else use IT'S service id
	TInt err = iServerEntry->SetEntry(aSel[0]);

	if (iServerEntry->Entry().iType != KUidMsvServiceEntry)
		{
		User::LeaveIfError(iServerEntry->SetEntry(iServerEntry->Entry().iServiceId));
		iServiceId = iServerEntry->Entry().Id();
		}

	__ASSERT_DEBUG(iServerEntry->Entry().iType==KUidMsvServiceEntry, Panic(EPopCurrentContextIsNotService));
	if(err == KErrNone && iServerEntry->Entry().iType != KUidMsvServiceEntry)
		{
		User::Leave(KErrNotSupported);
		}

	CEmailAccounts* account = CEmailAccounts::NewLC();
	TPopAccount id;
	id.iPopAccountId = iServerEntry->Entry().MtmData2();	// iMtmData2 of the service entry contains TPopAccountId
	id.iPopAccountName = iServerEntry->Entry().iDetails;
	id.iPopService = iServerEntry->Entry().iServiceId;
	id.iSmtpService = iServerEntry->Entry().iRelatedId;

	account->LoadPopSettingsL(id, *iPopSettings);
	account->LoadPopIapSettingsL(id, *iIAPPreferences);
   	CleanupStack::PopAndDestroy(account);    
	__ASSERT_DEBUG(iServiceId==iServerEntry->Entry().Id(), Panic(EPopCurrentContextIsNotService));
	iServiceId = iServerEntry->Entry().Id();
	}

//
// Async stage for cleaning up 
//
void CImppServerMtm::QueueRemoteCleanup()
	{
	TRequestStatus *pS = &iStatus;
	SetActive();
	User::RequestComplete(pS,KErrNone);
	}

//
//  DoShowMessagesL().
//
//  Gets all the chld entries for iServiceId..   
//
//
void CImppServerMtm::DoShowMessagesL(TBool aShow)
	{
	__ASSERT_ALWAYS(iServerEntry, Panic(EPopNullPointer));
	if ((!iConnectedToPopMbox) && (!aShow))
		{
		return;
		}
	
	iConnectedToPopMbox=aShow;

	// Leave messages displayed under the service if we are in disconnected mode.
	if (iPopSettings)
		{
		if ((iPopSettings->DisconnectedUserMode()))
			{
			aShow = ETrue;
			}
		}

	if (iDoCallShowMessages==(TInt)aShow)
		{
		return;
		}
	User::LeaveIfError(iServerEntry->SetEntry( iServiceId ));
	__ASSERT_DEBUG(iServerEntry->Entry().Id()!=KMsvNullIndexEntryId, Panic(EPopCurrentContextIsNullEntry));
	if (iServerEntry->Entry().iType!=KUidMsvServiceEntry || iServerEntry->Entry().Id()==KMsvNullIndexEntryId)
		{
		return;
		}
	//  Make a new CMsvEntrySelection
	CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(selection);
	TMsvSelectionOrdering order;
	order.SetShowInvisibleEntries( ETrue );
	iServerEntry->SetSort( order );
	User::LeaveIfError(iServerEntry->GetChildren(*selection));
  
    //  Change the visible flag for each TMsvEntry in the child 
    //  collection..
	if (aShow)
		{
		User::LeaveIfError(iServerEntry->ChangeAttributes(*selection, KMsvVisibilityAttribute,0));
		}
	else
		{
		User::LeaveIfError(iServerEntry->ChangeAttributes(*selection,0,KMsvVisibilityAttribute | KMsvUnreadAttribute));
		}
	CleanupStack::PopAndDestroy();  // selection
	iDoCallShowMessages=(TInt)aShow;
	}

//
// Abort an MTM command if necessary
//

void CImppServerMtm::CommandComplete( TInt aErrorCode )
	{
	// error code returned in the progress
	iPopProgress.iErrorCode = aErrorCode;

	//check iReportStatus for cases where we have already completed the caller
	if(iReportStatus)
		{
		User::RequestComplete(iReportStatus, KErrNone);
		iReportStatus = NULL;	
		}
	}

CMsvEntrySelection* CImppServerMtm::StripInvalidEntriesLC(const CMsvEntrySelection& aSelection, TBool aExcludePartial) const
	{
	__ASSERT_ALWAYS(iServerEntry, Panic(EPopNullPointer));
	CMsvEntrySelection* entries=aSelection.CopyLC();
	TMsvEntry* entry;
	TInt count=entries->Count();
	while (count--)
		{
		iServerEntry->GetEntryFromId((*entries)[count], entry);
		const TMsvEmailEntry &emailEntry=(*entry);
		if(entry->iServiceId != iServiceId || (emailEntry.PartialDownloaded() && aExcludePartial))
			{
			entries->Delete(count);
			}
		}
	return entries;
	}

void CImppServerMtm::FindFirstOfflineOperationL(TBool aQuitting)
	{
	iState.iCurrentOperation=EPopFindingFirstOfflineOp;
	iOperationActive=ETrue;
	iOfflineOpFinder->FindFirstL(iServiceId, aQuitting, iStatus);	
	iState.iRunningOfflineOperations = ETrue;
	SetActive();
	MOBILITY_TEST_MTM_STATE(iServiceId, KMobilityTestMtmStatePopFindingFirstOfflineOp);
	}

void CImppServerMtm::RunOfflineOperationL()
	{
	if (iOfflineOpFinder->OperationFound())
		{	
		const CImOffLineOperation& offlineOperation = iOfflineOpFinder->OfflineOperation();
		CMsvEntrySelection* messageSelection;
		iState.iCurrentOperation=EPopConnectedAndIdle;
		iOperationActive=EFalse;

		switch (offlineOperation.OpType())
			{
			case CImOffLineOperation::EOffLineOpNone:
			case CImOffLineOperation::EOffLineOpCopyFromLocal:
			case CImOffLineOperation::EOffLineOpMoveFromLocal:
			case CImOffLineOperation::EOffLineOpMoveWithinService:
			case CImOffLineOperation::EOffLineOpChange:
			case CImOffLineOperation::EOffLineOpCreate:
			case CImOffLineOperation::EOffLineOpMtmSpecific:
				User::Leave(KErrNotSupported);

			case CImOffLineOperation::EOffLineOpCopyToLocal:
				messageSelection = new (ELeave) CMsvEntrySelection;
				CleanupStack::PushL(messageSelection);
				messageSelection->AppendL(offlineOperation.MessageId());
				CopyToLocalL(*messageSelection, offlineOperation.TargetMessageId(),*iReportStatus);
				CleanupStack::PopAndDestroy(); // messageSelection
				break;

			case CImOffLineOperation::EOffLineOpDelete:
				messageSelection = new (ELeave) CMsvEntrySelection;
				CleanupStack::PushL(messageSelection);
				messageSelection->AppendL(offlineOperation.MessageId());
				DeleteAllL(*messageSelection, *iReportStatus);
				CleanupStack::PopAndDestroy(); // messageSelection
				break;

			case CImOffLineOperation::EOffLineOpCopyWithinService:
				messageSelection = new (ELeave) CMsvEntrySelection;
				CleanupStack::PushL(messageSelection);
				messageSelection->AppendL(offlineOperation.MessageId());
				CopyWithinServiceL(*messageSelection, offlineOperation.TargetMessageId(),*iReportStatus);
				CleanupStack::PopAndDestroy(); // messageSelection
				break;

			case CImOffLineOperation::EOffLineOpMoveToLocal:
				messageSelection = new (ELeave) CMsvEntrySelection;
				CleanupStack::PushL(messageSelection);
				messageSelection->AppendL(offlineOperation.MessageId());
				MoveToLocalL(*messageSelection, offlineOperation.TargetMessageId(),*iReportStatus);
				CleanupStack::PopAndDestroy(); // messageSelection
				break;
			default:
				User::Leave(KErrNotSupported);
				break;
			}
		}
	else
		{
		iState.iRunningOfflineOperations = EFalse;
		if (iState.iQuitting)
			{
			// Run the actual quit operation now.
			iState.iCurrentOperation=EPopQuitting;
			ResetProgress();
			//iPopSession->Quit(iStatus);
			iSessionManager->DeleteSession(*iPopSession, iStatus);
			MOBILITY_TEST_MTM_STATE(iServiceId, KMobilityTestMtmStatePopQuitting);
			iPopSession=NULL;
			iOperationActive=ETrue;
			SetActive();
			}
		else
			{
			iState.iCurrentOperation=EPopConnectedAndIdle;
			iOperationActive=EFalse;
			}
		}
	}


TBool CImppServerMtm::AcceptingOfflineOperationsL(const CMsvEntrySelection& aSelection)
// Returns true if the POPS MTM can accept an offline operation.
	{
	if (iPopSettings == 0)
		{
		GetPopDetailsL(aSelection);
		}

	return ((iState.iCurrentOperation == EPopDisconnected) && (iPopSettings->DisconnectedUserMode()));
	}


void CImppServerMtm::AddOfflineOperationL(const CMsvEntrySelection& aSelection, TMsvId aDestination, CImOffLineOperation::TOffLineOpType aOperationType, TRequestStatus& aStatus)
// Asynchronously applies the given operation type to the given selection of
// messages. The destination parameter is ignored for some operations, eg. delete.
	{
	if (iState.iCurrentOperation == EPopConnectedAndIdle)
		{
		Cancel();
		}

	iOfflineOpSetter->AddOfflineOperationL(&aSelection, aOperationType, aDestination, iStatus);
	iState.iCurrentOperation = EPopAddingOfflineOp;
	iReportStatus=&aStatus;
	ResetProgress();		
	SetActive();
	aStatus = KRequestPending;
	iOperationActive = ETrue;
	}

void CImppServerMtm::CancelOfflineOperationsL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus)
	{
	Cancel();
	iOfflineOpSetter->CancelOfflineOperationsL(aSelection, iStatus);
	iState.iCurrentOperation = EPopCancellingOfflineOps;
	iReportStatus=&aStatus;
	ResetProgress();		
	SetActive();
	aStatus = KRequestPending;
	iOperationActive = ETrue;
	}

//
// constructor
//
CImppServerMtm::CImppServerMtm(CRegisteredMtmDll& aPopServerMtmDll, CMsvServerEntry* aEntry)
:CBaseServerMtm(aPopServerMtmDll, aEntry)
	{
	}

void CImppServerMtm::ConstructL()
	{
	__ASSERT_ALWAYS(iServerEntry, Panic(EPopNullPointer));
	CActiveScheduler::Add(this);
	iConnectedToPopMbox=EFalse;
	iOperationActive=EFalse;
	iDoCallShowMessages=-1;
	ResetProgress();
	iServiceId=iServerEntry->Entry().Id();

#if (defined SYMBIAN_USER_PROMPT_SERVICE)	
	iWaiter = CPopUpsResponseWaiter::NewL();
	iHasCapability = ETrue;
#endif
	
	iOfflineOpFinder = CImPop3OfflineOperationFinder::NewL(*iServerEntry);
	iOfflineOpSetter = CImPop3SetOfflineOps::NewL(*iServerEntry);
	iState.iCurrentOperation = EPopDisconnected;
	iCancelForBMMigration = EFalse;
	iMigrationState = ENotMigrating;
	iNotConnectToPopMailBox = ETrue ;
	TRAP_IGNORE( iLogMessage=CImLogMessage::NewL( iServerEntry->FileSession() ) );
	}

CImppServerMtm::~CImppServerMtm()
	{
	Cancel();

	if (iServerEntry->Entry().Id()!=KMsvNullIndexEntryId)
		{
		TRAP_IGNORE( DoShowMessagesL(EFalse) );
		TInt err = iServerEntry->SetEntry(iServiceId);

		if (err == KErrNone)
			{
			TMsvEntry entry = iServerEntry->Entry();
			if (entry.Connected())
				{
				entry.SetConnected( EFalse );
				iServerEntry->ChangeEntry( entry );	// ignore any error
				}
			}
		}

	delete iPopSession;
	delete iPopRefreshMailbox;
	delete iPopCopyMove;
	delete iPopDelete;
	delete iPopSettings;
	delete iIAPPreferences;
	delete iMsvIdArray;
	delete iOfflineOpFinder;
	delete iOfflineOpSetter;
	delete iLogMessage;
	delete iMessagesToKeep;
	delete iPopTopPop;
#if (defined SYMBIAN_USER_PROMPT_SERVICE)	
 	delete iWaiter;
#endif

	delete iMobilityManager;
	delete iSessionManager;	
	if(iSavedValuesForMigration.iValuesSaved)
		{
		// We are leaving before ownership of Message Array was passed to PopSession
		delete iSavedValuesForMigration.iMessageArray;
		}
 	}


TBool CImppServerMtm::PruneMessages(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus)
	{
	TInt index = aSelection.Count();
	
	// See if the parent of the first entry is a message.
	// If it is then we need to prune the entries, ie. delete them locally.
	if (index == 0)
		{
		return EFalse;
		}

	TInt err = iServerEntry->SetEntry(aSelection[0]);

	if (err == KErrNone)
		{
		err = iServerEntry->SetEntry(iServerEntry->Entry().Parent());
		if (KUidMsvMessageEntry != iServerEntry->Entry().iType)
			{
			// The parent of the given entry was not a message, so we don't prune it.
			return EFalse;
			}
		}

	while ((index--) && (err==KErrNone))
		{
		// Go to the required entry
		err = iServerEntry->SetEntry(aSelection[index]);

		if (KErrNone == err)
			{
			// Go to the parent entry to see if it is a message entry
			iServerEntry->SetEntry(iServerEntry->Entry().Parent());
			TMsvEmailEntry entry = iServerEntry->Entry();

			// Clear the complete flag because we are about to delete the child parts.
			entry.SetComplete(EFalse);
			entry.SetBodyTextComplete(EFalse);
			err = iServerEntry->ChangeEntry(entry);

			if (KErrNone == err)
				{
				// Delete the body of the message.
				iServerEntry->DeleteEntry(aSelection[index]);
				}
			}
		}
	if (err == KErrNone)
		{
  		err = iServerEntry->SetEntry(KMsvNullIndexEntryId);
		}

	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);

	return ETrue;
	}

/**
Notice that a preferred carrier has become available, and migration to that bearer has been 
accepted.  The Pop Server MTM shall either pause or allow any current operation to complete
according to the action specified in parameter aAction. Once the current operation is paused 
or complete, the Pop Server MTM shall close any existing sockets and finally notify the 
mobility framework that it is ready to migrate to the new carrier.

@param aAction      - indicates the action that should be taken re: current operations
@param aIsSeamless  - indicates if the Bearer is seamless.
*/
void CImppServerMtm::PrepareForNewCarrier(TImMobilityAction aAction, TBool /*aIsSeamless*/)
	{
	// if the action is to accept immediately, CancelToMigrate can handle any current state...
	if (aAction == KAcceptImmediately)
		{
		if (iPopSession)
			{
			TRAP_IGNORE(StoreConfigurationToMigrateL());
			}

		// Accept the migration immediately, and delete any current activity
		CancelToMigrate();
		iMigrationState = EWaitingForNewCarrier;
		iMobilityManager->MigrateToNewCarrier();

		// do not set waiting if NewCarrierActive has been called synchronously.
		if (iMigrationState == EWaitingForNewCarrier)
			{
			// Now in a waiting state, set self active
			iStatus = KRequestPending;
			SetActive();
			}		
		}
	else	// ...otherwise, we need to handle being called in different migration states
		{
		switch (iMigrationState)
			{
			case ENotMigrating:
				{
				// allow any op in progress to complete, or stop when convenient,
				// according to the specified action
				if (aAction == KAcceptCompleteCurrent)
					{
					CompleteCurrentOpForMigration();
					}
				else // aAction == KAcceptStopCurrent
					{
					StopCurrentOpForMigration();
					}
				break;
				}
			case EWaitingForOpToComplete:
				{
				// override a previous complete action with a stop...
				if (aAction == KAcceptStopCurrent)
					{
					StopCurrentOpForMigration();
					}
				break;
				}
			case EWaitingForOpToStop:
			case EDisconnectingForMigrate:
				{
				// already stopping, do NOT override a stop action with a complete action
				break;
				}
			case EWaitingForNewCarrier:
			case EWaitingCarrierRejected:
				{
				// aleady ready and waiting for a new carrier. Tell the mobility manager,
				// but no further action necessary
				iMobilityManager->MigrateToNewCarrier();
				break;
				}
			case EConnectingAfterMigrate:
				{
				// Cancel the (re)connect operation, and inform the mobility manager
				CancelToMigrate();
				iMigrationState = EWaitingForNewCarrier;
				iMobilityManager->MigrateToNewCarrier();

				// do not set waiting if NewCarrierActive has been called synchronously.
				if (iMigrationState == EWaitingForNewCarrier)
					{
					// Now in a waiting state, set self active
					iStatus = KRequestPending;
					SetActive();
					}
				break;
				}
			default:
				{			
				__ASSERT_ALWAYS(EFalse, Panic(EPopNewCarrierUnknownState));
				break;	
				}			
			} // end of switch (iMigrationState)
		} // end of else...
	}

/**
Handles the completion of operations in order to migrate.  This is called by the 
PrepareForNewCarrier() function, when it is called by the mobility manager with 
the mobility action CompleteCurrentOpForMigration.
*/
void CImppServerMtm::CompleteCurrentOpForMigration()
	{
	iMigrationState = EWaitingForOpToComplete;
	switch(iState.iCurrentOperation)
		{
		case EPopRefreshing:
		case EPopTidying:
		case EPopCopying:
		case EPopMoving:
		case EPopPopulating:
		case EPopTopPopulating:
		case EPopDeleting:
		case EPopFindingFirstOfflineOp:
		case EPopCancellingOfflineOps:
			{
			// Just allow these operations to complete, handle in DoRunL
			break;
			}
		case EPopConnectedAndIdle:
			{
			// The POP Server MTM sets itself into a dummy active state when it enters
			// the EPopConnectedAndIdle state so we must cancel this active state
			CancelToMigrate();
			// Kick off the disconnect immediately. DoRunL is complex enough as it is.
			DisconnectForMigrate();
			break;
			}
		case EPopAuthoriseAndConnect:
		case EPopAddingOfflineOp:	// offline only
		case EPopDisconnected:		// offline
		case EPopConnecting: 		// haven't registered for bearer events in this state
		case EPopQuitting:			// deregistered
		default:
			{			
			__ASSERT_ALWAYS(EFalse, Panic(EPopCompleteOpUnexpectedOp));
			break;	
			}
		}
	}

/**
Handles the halting of operations in order to migrate.  This is called by the 
PrepareForNewCarrier() function, when it is called by the mobility manager with 
the mobility action StopCurrentOpForMigration.
*/
void CImppServerMtm::StopCurrentOpForMigration()
	{
	iMigrationState = EWaitingForOpToStop;
	// Signal the current operation handler to stop when convenient...
	switch (iState.iCurrentOperation)
		{
		case EPopTidying: 				// self completed state - handle in DoMigrateRunL
		case EPopFindingFirstOfflineOp: // self completed state - handle in DoMigrateRunL
		case EPopCancellingOfflineOps:	// allow to finish.
			{
			break;
			}
		case EPopCopying:
		case EPopMoving:
		case EPopPopulating:
			{
			iPopCopyMove->Pause();
			break;
			}
		case EPopTopPopulating:
			{
			iPopTopPop->Pause();
			break;
			}
		case EPopDeleting:
			{
			iPopDelete->Pause();
			break;
			}
		case EPopRefreshing:
			{	
			iPopRefreshMailbox->Pause();
			break;
			}
		case EPopConnectedAndIdle:
			{
			// The POP Server MTM sets itself into a dummy active state when it enters
			// the EPopConnectedAndIdle state so we must cancel this active state
			CancelToMigrate();
			// Kick off the disconnect immediately. DoRunL is complex enough as it is.
			DisconnectForMigrate();
			break;
			}
		case EPopAuthoriseAndConnect:  // not yet registered
		case EPopConnecting: 			// not yet registered
		case EPopQuitting:				// deregistered...
		case EPopDisconnected:			// offline
		case EPopAddingOfflineOp:		// offline
		default:
			{			
			__ASSERT_ALWAYS(EFalse, Panic(EPopStopOpUnexpectedOp));
			break;
			}
		}
	}

/**
This function may be called directly by the Mobility Manager in the case
that a downgrade notification has been received from the networking layer.
On return from this function, the server MTM must have cancelled any operations
on the existing pop session with resume enabled.

It may be called at any time, including when a migration is already in progress.
*/
void CImppServerMtm::CarrierLost()
	{
	if (iPopSession)
		{
		TRAP_IGNORE(StoreConfigurationToMigrateL());
		}

	// DoCancel() handles all migration states.
	CancelToMigrate();
	
	// Calling Cancel() doesnt *always* cause the iPopSession to be deleted,
	// for example, if in EPopConnectedAndIdle state. Just make sure:
	delete iPopSession;
	iPopSession = NULL;
	
	// Now that the current activity has been stopped, just need to wait for
	// a new carrier to come available. Set state and dummy request pending
	iMigrationState = EWaitingForNewCarrier;
	iStatus = KRequestPending;
	SetActive();
	}
	
/**
Notice that the New Carrier is now active and the server MTM can make connections on it.
Once this notice is received from the Mobility Manager, the Server MTM can create a new 
session on it and restart any awaiting operations on the New Carrier.  These could be 
operations that were stopped mid-way and need resuming from there or operations(requests)
that were cancelled and need starting from beginning.
@param aNewApp 		- acces point information.
@param aIsSeamless 	- Flag to indicate whether the migration is seamless.
*/
void CImppServerMtm::NewCarrierActive(TAccessPointInfo /*aNewAp*/, TBool /*aIsSeamless*/)
	{
	__ASSERT_DEBUG((iMigrationState == EWaitingForNewCarrier ||
	                iMigrationState == EWaitingCarrierRejected),
	               Panic(EPopUnexpectedNewCarrierActive));

	// Complete the request as the New carrier we were waiting on is now active
	// This will enable us to get back into the Server MTM state machine, 
	// initially dealing with the migration states and later with the MTM states if needed.
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}

/**
Handles Migration Errors

Simply deletes everything, marks the server offline and completes any 
outstanding user requests.

@param aError - An error code indicating the type of migration error.
*/
void CImppServerMtm::MobilityError(TUint /*aError*/)
	{
	__ASSERT_ALWAYS(iServerEntry, Panic(EPopNullPointer));
	
	// use special form of Cancel() to make sure we complete while
	// reporting the correct error code. Using this means owned operation
	// classes must be explicitly deleted here.
	CancelToMigrate();
	
	// delete the mobility manager	
	delete iMobilityManager;
	iMobilityManager = NULL;
	
	// delete operation classes
	delete iPopRefreshMailbox;
	iPopRefreshMailbox = NULL;
	delete iPopCopyMove;
	iPopCopyMove = NULL;
	delete iPopDelete;
	iPopDelete = NULL;
	delete iPopTopPop;
	iPopTopPop = NULL;

	// and the session
	delete iPopSession;
	iPopSession = NULL;

	// and the session manager - error might be a result of the RConnection
	delete iSessionManager;	
	iSessionManager = NULL;
	
	// tidyup
	iState.iQuitting = EFalse;
	TRAP_IGNORE( DoShowMessagesL(EFalse) );
	
	TInt err = iServerEntry->SetEntry( iServiceId );
	if(err == KErrNone)
		{
		TMsvEntry entry = iServerEntry->Entry();
		entry.SetConnected( EFalse );
		iServerEntry->ChangeEntry( entry );	// ignore any error
		}

	// If their is an outstanding command, then complete it with the error
	CommandComplete( KErrDisconnected );

	// final tidyup
	iServerEntry->SetEntry(KMsvNullIndexEntryId);
	iState.iCurrentOperation = EPopDisconnected;
	}

/**
Returns Progress info for Migration purposes
*/
const TDesC8& CImppServerMtm::MobilityProgress()
	{
	return Progress();
	}

/**
Gets MTM information that is not related to the current operation

@param aMtmDataType Type of data to fetch
@param aMtmDataBuffer On return this points to a descriptor holding the data

@return KErrNone if successful, or a system wide error code
*/
TInt CImppServerMtm::GetNonOperationMtmData(TNonOperationMtmDataType aMtmDataType, TPtrC8& aMtmDataBuffer)
	{
	if (aMtmDataType == EMtmDataAccessPointId)
		{
		if (iSessionManager)
			{
			TNonOperationMtmDataAccessPointId mtmDataAccessPointId;

			TInt err = iSessionManager->GetAccessPointIdForConnection(mtmDataAccessPointId.iAccessPointId);

			if (err == KErrNone)
				{
				iMtmDataAccessPointIdBuffer = TNonOperationMtmDataAccessPointIdBuffer(mtmDataAccessPointId);
				aMtmDataBuffer.Set(iMtmDataAccessPointIdBuffer);
				return KErrNone;
				}
			}

		return KErrNotFound;
		}
	return KErrNotSupported;
	}

/**
Creates the mobility manager and registers the RConnection for notification of
bearer migration events, if the necessary conditions are met.

Returns ETrue if mobility enabled and the initial carrier has been rejected.

@param  aError completion code of the session connect async operation
@return ETrue  if the initial carrier has been rejected (DoRunL should return)
		EFalse otherwise, DoRunL processing should continue
*/
TBool CImppServerMtm::StartBearerMobilityL(TInt aError)
	{
	// Connection operation complete. If the account supports bearer mobility
	// register now for bearer event notifications. 
	if (iPopSettings->BearerMobility() && iIAPPreferences->SNAPDefined())
		{
		// only create iMobilityManager once per connection request - we can be at this
		// point due an initial carrier rejected followed by successful migration...
		// also, check that the session manager has an RConnection. If not, bomb out.
		if (!iMobilityManager && iSessionManager->HasConnection())
			{
			iMobilityManager = CImMobilityManager::NewL(KUidPop3ServerMtm, iServiceId, *this);
			iMobilityManager->SetConnection(iSessionManager->GetConnection());

			// if there has been a problem connecting the session, reject the current carrier
			// via the migration manager, and wait for a new bearer available notification.
			if (aError != KErrNone)
				{
				iMigrationState = EWaitingCarrierRejected;
				iMobilityManager->NewCarrierRejected();
				
				// do not set waiting if NewCarrierActive has been called synchronously.
				if (iMigrationState == EWaitingCarrierRejected)
					{
					// Now in a waiting state, set self active
					iStatus = KRequestPending;
					SetActive();
					}
				return ETrue;
				} // end if (eCode != KErrNone)
			} // end if (!iMobilityManager && iSessionManager->HasConnection())
		} // end if (iPopSettings->BearerMobility() && iIAPPreferences->SNAPDefined())
	return EFalse;
	}
