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

#include <e32base.h>
#include <mentact.h>

#include "cimapopbackgroundsync.h"
#include "cimapfolder.h"
#include "cimapprotocolcontroller.h"
#include "cimapmailstore.h" 
#include "cimapsyncmanager.h"
#include "cimapcompoundsyncservice.h"
#include "cimapsessionconsts.h"
#include "cimaplogger.h"

CImapOpBackgroundSync::CImapOpBackgroundSync( MImapOpBackgroundSyncObserver& aBackgroundSyncObserver,
											  CMsvServerEntry& aServerEntry, 
										      CImapSyncManager& aSyncMan,
											  CImapSettings& aImapSettings,
											  CImapMailStore& aImapMailStore,
											  CImapOfflineControl& aImapOfflineControl ) :
	CActive(EPriorityStandard),
	iBackgroundSyncObserver(aBackgroundSyncObserver),
	iEntry(aServerEntry),
	iSyncMan(aSyncMan),
	iImapSettings(aImapSettings),
	iImapMailStore(aImapMailStore),
	iImapOfflineControl(aImapOfflineControl),
	iMigrateState(ENotMigrating)
	{
	
	}
						   
CImapOpBackgroundSync::~CImapOpBackgroundSync()
	{
	Cancel();
	delete iCompoundSync;
	iCompoundSync = NULL;
	}

/**
Returns a CImapOpBackgroundSync. The background synchronise operation is started 
by calling StartSync().

@param aBackgroundSyncObserver Owning observer that is notified when the operation is completed.
@param aSyncMan IMAP Sync Manager used to perform the full synchronise
@return the newly created CImapOpBackgroundSync object. The caller is responsible for deleting
the object. Usually this is done after the caller is notified by the CImapOpBackgroundSync
object through the MImapOpBackgroundSyncObserver API.
*/
CImapOpBackgroundSync* CImapOpBackgroundSync::NewL( MImapOpBackgroundSyncObserver& aBackgroundSyncObserver,
													CMsvServerEntry& aServerEntry,
													CImapSyncManager& aSyncMan,
													CImapSettings& aImapSettings,
													CImapMailStore& aImapMailStore,
													CImapOfflineControl& aImapOfflineControl  )
	{
	CImapOpBackgroundSync* self = new (ELeave) CImapOpBackgroundSync( aBackgroundSyncObserver,
																	  aServerEntry,
																	  aSyncMan,
																	  aImapSettings,
																	  aImapMailStore,
																	  aImapOfflineControl );
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CImapOpBackgroundSync::ConstructL()
	{
	iCompoundSync = CImapCompoundSyncService::NewL( iSyncMan, 
											    	iEntry, 
											    	iImapSettings,
											    	iImapMailStore,
											    	iImapOfflineControl,
											    	EFalse);
	CActiveScheduler::Add(this);
	}

/**
Starts the asynchronous background synchronisation. When the operation is completed, the
observer iBackgroundSyncObserver is notified.
*/	
void CImapOpBackgroundSync::StartSync(CImapSession& aSession)
	{
	iSession = &aSession;
	iState = EStartingSync;

	__LOG_TEXT(iSession->LogId(), "CImapOpBackgroundSync::StartSync()");
	
	// Complete self
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

void CImapOpBackgroundSync::RunL()
	{
	__ASSERT_DEBUG(iSession!=NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EBackgroundSyncSessionIsNull));
	
	if (iMigrateState==EStoppingForMigrate)
		{
		// has the sync finished? if so, process normally as we do not need to resume.
		// Otherwise mark the background sync as suspended
		TImap4CompoundProgress compoundProgress;
		iCompoundSync->Progress(compoundProgress);
		if (compoundProgress.iGenericProgress.iState != TImap4GenericProgress::EIdle)
			{
			// the sync op has completed prematurely to allow the migration to occur
			iMigrateState=ESuspendedForMigrate;
			iBackgroundSyncObserver.BackgroundSyncComplete(iStatus.Int());
			iSession = NULL;
			return;
			}
		}

	// Normal behaviour follows:
	switch (iState)
		{
		case EStartingSync:
			{
			iCompoundSync->StartOperation(iStatus, *iSession);
			iState = ESynchronising;
			SetActive();
			break;
			}
		case ESynchronising:
			{
			// call backgroundsynccomplete
			iState = EFinished;
			iBackgroundSyncObserver.BackgroundSyncComplete(iStatus.Int());
			break;
			}
		case ECancelRecoveringSession:
			{
			// Call syncmanager api to clean up after cancel
			iState = ECancelRefreshingSyncManager;
			// break; fall through
			}
		case ECancelRefreshingSyncManager:
			{
			iState = EFinished;
			if(iStatus.Int() != KErrNone)
				{
				// An error has occured during the flushing of the session.
				// Pass the error code to the observer for processing.
				iBackgroundSyncObserver.BackgroundSyncComplete(iStatus.Int());	
				}
			else
				{
				iBackgroundSyncObserver.BackgroundSyncComplete(KErrCancel);	
				}
			break;
			}
		default:
			{
			__ASSERT_DEBUG(iSession!=NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EBackgroundSyncUnexpectedState));
			break;
			}
		} // end of switch (iState)
	}
	
TInt CImapOpBackgroundSync::RunError(TInt aError)
	{
	// Complete with error
	iBackgroundSyncObserver.BackgroundSyncComplete(aError);
	return KErrNone;
	}

void CImapOpBackgroundSync::DoCancel()
	{
	switch(iState)
		{
		case ESynchronising:
			{
			// Do not change state - cleanup needs it.
			// Do not delete as the status needs to be checked later.
			if (iMigrateState==ECancellingForMigrate)
				{
				iCompoundSync->CancelForMigrate();
				}
			else
				{
				iCompoundSync->Cancel();
				}
			break;
			}

		case ECancelRecoveringSession:
			{
			iSession->Cancel();
			break;
			}

		case EStartingSync:
		case ECancelRefreshingSyncManager:
			{
			// self-completed or no outstanding request.
			break;
			}

		default:
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::EBackgroundSyncCancelUnexpectedState));
			break;
			}
		}
	}

/**
Calls Cancel() to cancel any outstanding asynchronous service requests,
then cleans up the sync manager and issues the appropriate command to 
flushes the imap session if necessary.
*/
void CImapOpBackgroundSync::CancelAndCleanup()
	{
	__LOG_TEXT(iSession->LogId(), "CImapOpBackgroundSync::CancelAndCleanup()");
	if (iMigrateState==ESuspendedForMigrate)
		{
		// already suspended.
		iState = EFinished;
		iBackgroundSyncObserver.BackgroundSyncComplete(KErrCancel);
		}
	else
		{
		Cancel();
		if (iState==ESynchronising)
			{
			iSession->FlushCancelledCommand(iStatus);
			iState=ECancelRecoveringSession;
			SetActive();
			}
		else
			{
			iState = EFinished;
			iBackgroundSyncObserver.BackgroundSyncComplete(KErrCancel);
			}
		}
	}


void CImapOpBackgroundSync::Progress(TImap4CompoundProgress& aCompoundProgress)
	{
	iCompoundSync->Progress(aCompoundProgress);	
	}

/**
Returns ETrue if the background sync operation has been suspended to allow
a bearer migration to occur.
*/
TBool CImapOpBackgroundSync::IsSuspendedForMigrate()
	{
	return (iMigrateState==ESuspendedForMigrate?ETrue:EFalse);
	}

/**
Immediately cancels outstanding operation, and recovers the compound operation
object to a state in which the operation may be restarted following migration
to a new carrier.
The operation will be restarted by a call to StartOperation()
*/
void CImapOpBackgroundSync::CancelForMigrate()
	{
	iMigrateState=ECancellingForMigrate;
	Cancel();
	iMigrateState=ESuspendedForMigrate;
	iSession=NULL;
	}

/**
Indicates that the compound operation should complete as soon as it is possible
for the operation to be re-started without requiring large amounts of communication
that has already been performed to be repeated to complete the operation.
The operation will be restarted by a call to StartOperation()
*/
void CImapOpBackgroundSync::StopForMigrate()
	{
	iMigrateState=EStoppingForMigrate;
	iCompoundSync->StopForMigrate();
	}

/**
Resumes the background sync operation (following bearer migration).
@param aSession the connected IMAP session 
*/
void CImapOpBackgroundSync::ResumeOperationL(CImapSession& aSession)
	{
	iSession = &aSession;
	__ASSERT_DEBUG(iMigrateState==ESuspendedForMigrate, TImapServerPanic::ImapPanic(TImapServerPanic::EBackgroundSyncUnexpectedState));
	iMigrateState=ENotMigrating;
	
	switch (iState)
		{
		case ENotInUse:
		case EStartingSync:
			{
			// the sync operation has not yet been started.
			iCompoundSync->StartOperation(iStatus, *iSession);
			iState = ESynchronising;
			SetActive();
			break;
			}
		case ESynchronising:
			{
			iCompoundSync->ResumeOperationL(iStatus, *iSession);
			SetActive();
			break;
			}	
		case ECancelRecoveringSession:
		case ECancelRefreshingSyncManager:
		case EFinished:
		default:
			{
			// just auto-complete to force the background sync op to complete.
			iState = ESynchronising;
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrNone);
			SetActive();
			}
		}
	}

/**
Allows the protocol to specify an array of messages that should not be fetched
using the download rules during background sync. This is called in the case that
a copy to local or populate operation is requested while the background sync op
is still in progress.

@param aSelection - the selection of messages that should not be autofetched
*/
void CImapOpBackgroundSync::RemoveFromSelectionL(CMsvEntrySelection& aDeleteSel)
	{
	if (iCompoundSync)
		{
		iCompoundSync->RemoveFromSelectionL(aDeleteSel);
		}
	}

