// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <imapset.h>
#include <mentact.h>
#include <miut_err.h>

#include "cimapprotocolcontroller.h"
#include "cimapsettings.h"
#include "cimapsessionmanager.h"
#include "cimapsession.h"
#include "cimapsessionconsts.h"
#include "cimapmailstore.h" 
#include "cimapofflinecontrol.h"
#include "cimapsyncmanager.h"
#include "cimapfolder.h"

#include "cimapcompoundcopyfromlocal.h"
#include "cimapcompoundcopytolocal.h"
#include "cimapcompoundcopywithinservice.h"
#include "cimapcompoundcreate.h"
#include "cimapcompounddelete.h"
#include "cimapcompounddeletefolder.h"
#include "cimapcompounddisconnect.h"
#include "cimapcompoundrename.h"
#include "cimapcompoundselect.h"
#include "cimapcompoundsyncfolder.h"
#include "cimapcompoundsynctree.h"
#include "cimapcompoundsyncservice.h"

#include "cimapopbackgroundsync.h"
#include "cimapidlecontroller.h"
#include "cimapupdateflagoperation.h"

#include "cimaplogger.h"

#include "mobilitytestmtmapi.h"

// at the moment we do not need more than 2 sessions
const TInt KImapSessionArrayGranularity = 2;

const TUid KUidImapServerMtm = {0x10003C4E};

CImapProtocolController::CImapProtocolController(CMsvServerEntry& aEntry,CImapOfflineControl& aImapOfflineControl):
	CMsgActive(EPriorityStandard),
	iEntry(aEntry),
	iImapSessionArray(KImapSessionArrayGranularity),
	iImapOfflineControl(aImapOfflineControl)
	{
	}
	
CImapProtocolController::~CImapProtocolController()
	{
	Cancel();

	// The mail store needs to be deleted before DisconnectAll() as its destructor
	// uses the session that DisconnectAll() destroys.
	delete iImapMailStore;	

	//serviceid indicates that we are online
	if(iServiceId!=0)
		{
		DisconnectAll();
		}

	delete iBackgroundSyncOp;
	delete iImapCompound;
	delete iMigrateCompound;
	delete iImapSyncManager;
	delete iImapIdleController;
	delete iImapSettings;
	
	delete iMobilityManager;

	// Need to destroy the sessions before the session manager is deleted.
	// This is because session manager will cleanup transport handler code,
	// and it is important to ensure that the sockets are not being used
	// by the session when that happens.
	iImapSessionArray.ResetAndDestroy();
	delete iImapSessionManager;
	}

EXPORT_C CImapProtocolController* CImapProtocolController::NewL(CMsvServerEntry& aEntry,CImapOfflineControl& aImapOfflineControl)
	{
	CImapProtocolController* self = new (ELeave) CImapProtocolController(aEntry,aImapOfflineControl);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Part of two phase construction
Note that syncmanager is not constructed until a connection is made.
*/
void CImapProtocolController::ConstructL()
	{
	iImapSettings       = CImapSettings::NewL(iEntry);
	iImapMailStore 		= CImapMailStore::NewL(iEntry);
	iImapSessionManager = CImapSessionManager::NewL(*iImapSettings, *iImapMailStore);

	iFlushPrimary = EFalse;
	// We're an active object...
	CActiveScheduler::Add(this);
	}

/**
Query connection status - return true if at least one IMAP Session exists and is 
connected. Also return true if migrating.
This is not a compound operation.

@return 
*/
EXPORT_C TBool CImapProtocolController::Connected() const
	{
	if (iImapSessionArray.Count() > 0 || iMigrateState != ENotMigrating)
		{
		// Sessions are deleted if a disconnect is observed,
		// therefore the existance of any sessions indicates 
		// that the Protocol Controller is connected.
		return ETrue;
		}
	return EFalse;
	}

/**
If a connected session already exists, this API shall complete immediately with 
KErrServerBusy. Otherwise, this API uses the IMAP Session Manager to create and connect 
an IMAP Session object that can be used for subsequent commands.
On successful connection, the passed request status is completed. This API can not 
be used to instantiate further IMAP Session objects.

Sequence For Connect
	CreateSession
	SelectInboxRW
	Finished
	
@param aStatus
@param aSelection
@leave KErrServerBusy if a connected session already exists
*/
EXPORT_C void CImapProtocolController::ConnectL( TRequestStatus& aStatus,
												 CMsvEntrySelection& aSelection )
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::ConnectL()");
	
	ResetProgress();
	
	if (Connected())
		{
		// Do not call Complete() - this would result in the Protocol
		// Controller disconnecting all sessions.
		TRequestStatus* pStatus = &aStatus;
		User::RequestComplete(pStatus, KErrServerBusy);
		}
	else
		{
		// Load the Service Settings
		iServiceId = aSelection[0];
		iImapSettings->LoadSettingsL(iServiceId);

		// We're going online - create a session manager if none exists
		if (iImapSessionManager==NULL)
			{
			// session manage may have been deleted due to a mobility error
			iImapSessionManager = CImapSessionManager::NewL(*iImapSettings, *iImapMailStore);
			}
	
		// create a sync manager
		delete iImapSyncManager;
		iImapSyncManager = NULL;
		iImapSyncManager = CImapSyncManager::NewL( iEntry, *iImapSettings);
		
		// create the mobility manager if mobility supported
		delete iMobilityManager;
		iMobilityManager = NULL;

		if (iImapSettings->BearerMobility())
			{
			iMobilityManager = CImMobilityManager::NewL(KUidImapServerMtm, iServiceId, *this);
			}
	
		// Create the primary session pointer
		CImapSession* imapSession = NULL;
		iImapSessionArray.AppendL(imapSession);

		// Request the session manager to connect the session
		iImapSessionManager->GetSessionL(iStatus, iImapSessionArray[0]);

		iForegroundSession = 0;
		iRequestedOp = EConnect;
		iCurrentOp = iRequestedOp;

		Queue(aStatus);
		SetActive();
		}
	}

/**
This method allows the client to connect to an IMAP account and initialise a 
background synchronisation of the account once connection has completed. If a 
connected IMAP Session already exists, the API shall complete immdediately with 
KErrServerBusy. 
On successful connection, a background synchronisation is started and the passed 
request status is completed. The background synchronisation is processed by a 
CImapBackgroundSyncOp object, which will notify the Protocol Controller that 
the process has completed via a callback function void 
BackgroundSyncCompleted( TInt aError )

Sequence For ConnectAndSynchronise
	CreateSession
	StartBackgroundSyncOp
	Finished

@param aStatus
@param aSelection
@leave KErrServerBusy if a connected session already exists
*/
EXPORT_C void CImapProtocolController::ConnectAndSynchroniseL( TRequestStatus& aStatus, 
															   CMsvEntrySelection& aSelection )
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::ConnectAndSynchroniseL()");

	// Issue the connect request (this will cause ResetProgress() to be called)
	ConnectL(aStatus, aSelection);

	// Create the Background Sync Operation object
	if (IsActive())
		{
		__ASSERT_DEBUG(iBackgroundSyncOp==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EConnectAndSyncBgSyncOpIsNotNull));
		iBackgroundSyncOp = CImapOpBackgroundSync::NewL(*this, iEntry, *iImapSyncManager, *iImapSettings, *iImapMailStore, iImapOfflineControl);
		}
	}

/**
Logs out and disconnects all connected sessions with the remote server.

Disconnect request is handled and acted upon during migration, unlike other 
client-requested operations which are rejected (with KErrServerBusy).

The Bearer Mobility Manager is deleted prior to acting on this request,
effectively de-registering the server MTM from the mobility framework

@param aStatus
*/
EXPORT_C void CImapProtocolController::DisconnectL(TRequestStatus& aStatus)
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::DisconnectL()");

	__ASSERT_DEBUG(iImapCompound==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EDisconnectCompoundIsNotNull));
	
	// de-register for mobility notifications
	delete iMobilityManager;
	iMobilityManager = NULL;
	
	// Disconnect allowed to happen even if we are migrating...
	switch (iMigrateState)
		{
		case ENotMigrating:
		case EHandlingConnectError:
			{
			// normal behaviour...
			DoDisconnectL(aStatus);
			break;
			}
		case EConnectingAfterMigrate: // Cancel connecting of new session
		case EStartingReconnect:	  // already disconnected. Cancel dummy request
		case EWaitingForNewCarrier:   // already disconnected. Cancel dummy request
		case EWaitingInitialCarrierRejected: // already disconnected. Cancel dummy request
			{
			Cancel();
			// mark offline and complete
			DisconnectAll();
			Queue(aStatus);
			Complete(KErrNone);
			break;
			}
		case EDisconnectingForMigrate:
			{
			// already doing a graceful disconnect, switch to main state machine.
			iMigrateState = ENotMigrating;
			iRequestedOp  = EDisconnect;
			iCurrentOp    = EDisconnect;
			// however, using the iMigrateCompound pointer...
			iImapCompound = iMigrateCompound;
			iMigrateCompound = NULL;
			Queue(aStatus);
			break;
			}
		case EWaitingForOpToComplete:
		case EWaitingForOpToStop:
		default:
			{
			__ASSERT_DEBUG(iBackgroundSyncOp==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EDisconnectUnexpectedMigrateState));
			break;
			}
		}
	}

void CImapProtocolController::DoDisconnectL(TRequestStatus& aStatus)
	{
	// Unlike the other async methods, DisconnectL() will wait for any flush operation to finish
	// and then initiate the disconnect.
	// Consequently there is no need to call CompleteIfBackgroundOpInProgress()	
	ResetProgress();
	
	iImapCompound = CImapCompoundDisconnect::NewL( *iImapSyncManager, 
												   iEntry, 
												   *iImapSettings,
												   *iImapSessionManager,
												   *iImapMailStore,
												   iImapSessionArray,
												   iImapOfflineControl,
												   ETrue );
	iRequestedOp = EDisconnect;
	
	if (iCurrentOp != ECancelRecoverPrimary)
		{
		// Cancel any outstanding background sync												   
		if (iBackgroundSyncOp!=NULL)
			{
			iBackgroundSyncOp->Cancel();
			delete iBackgroundSyncOp;
			iBackgroundSyncOp=NULL;

			// Flush the session, now that it has been cancelled
			iCurrentOp = ECancelRecoverPrimary;
			CImapSession* session = iImapSessionArray[0];
			session->FlushCancelledCommand(iStatus);
			SetActive();
			}
		else
			{
			StartPrimaryOperation();	
			SetActive();	
			}
		}
	
	// NOTE: In the case of iCurrentOp == ECancelRecoverPrimary, DoRunL() will start the async 
	// iImapCompound operation after the flush has completed.
	// The iImapCompound will expect aStatus to have been queued so that it can be Completed later.
	
	Queue(aStatus);
	}

/**
Checks the status of the CImapSessions

@return ETrue if a backgroundsync is in progress and the second session is busy,
		EFalse otherwise.
*/
EXPORT_C TBool CImapProtocolController::Busy() const
	{
	// second session is busy if a compound operation exists.
	if (BackgroundSyncInProgress() && iImapCompound!=NULL)
		{
		return ETrue;
		}
	return EFalse;	
					
	}

/**
Gets the access point ID in use for the connection to the server

@param aIap On return stores the access point ID value

@return KErrNone if successful, or a system wide error code
*/
EXPORT_C TInt CImapProtocolController::GetAccessPointIdForConnection(TUint32& aAccessPointId) const
	{
	if (iImapSessionManager)
		{
		return iImapSessionManager->GetAccessPointIdForConnection(aAccessPointId);
		}

	return KErrNotFound;
	}

/**
Returns whether a Background synchronisation is in progress

@return ETrue if a Background synchronisation is in progress, EFalse otherwise
*/
EXPORT_C TBool CImapProtocolController::BackgroundSyncInProgress() const
	{
	return (iBackgroundSyncOp != NULL);
	}

/**
Cancels an outstanding background synchronise operation. Calls 
CImapOpBackgroundSync::Cancel() to propagate the cancel and kicks off
an asynchronous cleanup operation.
The cancel request is completed immediately with KErrNone if no background
sync was in operation, otherwise it is completed with KErrCancel when
the cleanup process is complete.
*/	
EXPORT_C void CImapProtocolController::CancelBackgroundSync(TRequestStatus& aStatus)
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::CancelBackgroundSync()");
	
	ResetProgress();
	
	Queue(aStatus);
	if (BackgroundSyncInProgress())
		{
		if (iMigrateState != ENotMigrating && iBackgroundSyncOp->IsSuspendedForMigrate())
			{
			delete iBackgroundSyncOp;
			iBackgroundSyncOp = NULL;
			Complete(KErrCancel);
			}
		else
			{
			iBackgroundSyncOp->CancelAndCleanup();
			iWaitForBackgroundSync = ETrue;
			}
		}
	else
		{
		Complete(KErrNone);
		}
	}

/**
Wait for background sync operation to complete. This effectively makes a background 
synchronise a foreground operation blocking subsequent Server MTM requests until the 
synchonise is completed.

@param aStatus
*/
EXPORT_C void CImapProtocolController::WaitForBackground(TRequestStatus& aStatus)
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::WaitForBackground()");
	
	ResetProgress();
	
	Queue(aStatus);
	// Wait for background operation to complete: is one running?
	if (!BackgroundSyncInProgress())
		{
		// No, complete immediately
		Complete(KErrNone);
		}
	else
		{
		// Otherwise, wait for completion
		iWaitForBackgroundSync = ETrue;
		}
	}


/**
Completes the passed request status if a background operation is in progress,
or if the primary session is being recovered following a cancel operation.
This should be called prior to starting a compound operation that requires the
primary session to be performed.

This method must be called before assigning iRequestedOp, 
for any operation that requires the primary session.

@return ETrue if the user request has been completed
		EFalse otherwise.
*/
TBool CImapProtocolController::CompleteIfBackgroundOpInProgress(TRequestStatus& aStatus)
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::CompleteIfBackgroundOpInProgress()");
	if (BackgroundSyncInProgress() || iCurrentOp==ECancelRecoverPrimary || iMigrateState!=ENotMigrating)
		{
		// Complete the user request
		// Do not call Complete() - this would result in the Protocol
		// Controller disconnecting all sessions.
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrServerBusy);
		return ETrue;
		}
	return EFalse;
	}
	
/**
Synchronise Folder Tree - calls CImapSyncManager::SynchroniseTreeL()
Sequence For SynchroniseTree
	StopIdle
	SynchroniseTree // CImapSyncManager Operation

@param aStatus
@leave
*/	
EXPORT_C void CImapProtocolController::SynchroniseTreeL(TRequestStatus& aStatus)
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::SynchroniseTreeL()");
	__ASSERT_DEBUG(iImapCompound==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ESynchroniseTreeCompoundIsNotNull));

	ResetProgress();

	if (!CompleteIfBackgroundOpInProgress(aStatus))	
		{
		iImapCompound = CImapCompoundSyncTree::NewL( *iImapSyncManager, 
													 iEntry, 
													 *iImapSettings );
		
		iRequestedOp = ESync;
		StartPrimaryOperation();	
		Queue(aStatus);
		SetActive();
		}
	}


/**
Perform a full account synchronisation - calls CImapSyncManager::SynchroniseL()
Sequence For SynchroniseAll
	StopIdle
	SynchroniseL  // CImapSyncManager Operation

@param aStatus
@leave
*/	
EXPORT_C void CImapProtocolController::SynchroniseAllL( TRequestStatus& aStatus )
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::SynchroniseAllL()");
	__ASSERT_DEBUG(iImapCompound==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ESynchroniseAllCompoundIsNotNull));

	ResetProgress();

	if (!CompleteIfBackgroundOpInProgress(aStatus))	
		{
		iImapCompound = CImapCompoundSyncService::NewL( *iImapSyncManager, 
												    	iEntry, 
												    	*iImapSettings,
												    	*iImapMailStore,
												    	iImapOfflineControl,
												    	EFalse);
	
		iRequestedOp = ESync;
		StartPrimaryOperation();	
		Queue(aStatus);
		SetActive();
		}
	}

/**
Callback function to notify Protocol Controller that a background sync 
operation has completed.
Deletes the CImapOpBackgroundComplete object that has just completed.
If WaitForBackground has been previously called, completes the user request.

@param aError
*/	
EXPORT_C void CImapProtocolController::BackgroundSyncComplete(TInt aError)
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::BackgroundSyncComplete()");
	__ASSERT_DEBUG(iBackgroundSyncOp!=NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ENoBackgroundSyncInProgress));

	// get final sync progress... 
	// unless a foreground operation is being performed
	if (iImapCompound == NULL)
		{
		iBackgroundSyncOp->Progress(iProgress);
		}
		
	if (iMigrateState != ENotMigrating)
		{
		// If the protocol controller is not active, then there is no
		// operation currently in progress. Therefore we are now ready to
		// migrate. Complete the request here, and then Disconnect current sockets in DoMigrateRunL(). 
		// We cannot just test existance of a compound operation object, as there may be a "paused" operation.
		if (!IsActive())
			{
			// Complete the request so that, we migrate from DoMigrateRunL()
			SetActive();
			TRequestStatus* pStatus = &iStatus;
			User::RequestComplete(pStatus, KErrNone);
			}
		
		// the background sync may have been completed early to allow
		// a migration to occur. If that is the case, then we do not want
		// to delete the background sync operation object - it will need
		// to be restarted once the migrate has occurred. Otherwise, just
		// process the completion of the background sync normally.
		if (iBackgroundSyncOp->IsSuspendedForMigrate())
			{
			return;
			}
		}

	// Deleting the background sync op.
	// CImapOpBackgroundComplete must finish immediately after calling this API
	delete iBackgroundSyncOp;
	iBackgroundSyncOp = NULL;
	
	if (aError>0)
		{
		// translate positive error codes, weed out non-fatal errors
		ThranslateSessionError(aError);
		}

	TBool foregroundOpCancelled = EFalse;
	if (aError==KErrNone || aError==KErrCancel)
		{
		if (iImapCompound==NULL && iMigrateState == ENotMigrating)
			{
			// only start IDLE if foreground operation not in progress
			StartIdle();
			MOBILITY_TEST_MTM_STATE(iServiceId, KMobilityTestMtmStateImapIdle);
			}
		}
	else
		{
		// Cancel any foreground operation - the primary session has
		// failed, so all sessions are to be torn down.
		if (iImapCompound != NULL)
			{
			iImapCompound->Cancel();
			foregroundOpCancelled = ETrue;
			delete iImapCompound;
			iImapCompound=NULL;
			}

		// DisconnectAll will be process by DoComplete if a client request is outstanding.
		if (!iWaitForBackgroundSync && !foregroundOpCancelled)
			{
			// Drop all session connections and mark the service offline.
			DisconnectAll();
			}
		}

	// Store the error in the progress object
	iProgress.iGenericProgress.iErrorCode=aError;
	iProgress.iGenericProgress.iState=TImap4GenericProgress::EIdle;
		
	// Complete the client request if it was waiting for notification of 
	// bg sync complete, or if we have cancelled an foreground operation
	if (iWaitForBackgroundSync || foregroundOpCancelled)
		{
		iCancelInProgress = ETrue;
		Complete(aError);
		iCancelInProgress = EFalse;
		}

	iWaitForBackgroundSync = EFalse;
	}

/**
CopyToLocalL() is used to fetch a selection of messages from the remote 
server as a single operation, without return to idle state between handling each
message. The email is fetched to the local mirror of the containing folder. Optionally 
this function can also copy the message to a local folder, specified in aDestination. 
A fetch to mirror only is performed by calling with aDestination set to 
KMsvNullIndexEntryId

Sequence For CopyToLocal
	StopIdle
	SelectSourceMailboxRO
	FetchMessage(s)
	InboxDuplicateCopy

@param aStatus
@param aSourceSel
@param aDestination
*/
EXPORT_C void CImapProtocolController::CopyToLocalL(TRequestStatus& aStatus, 
													const CMsvEntrySelection& aSourceSel, 
													const TMsvId aDestination)
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::CopyToLocalL()");
	__ASSERT_DEBUG(iImapCompound==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECopyToLocalCompoundIsNotNull));

	ResetProgress();

	// Instead of calling CompleteIfBackgroundOpInProgress(), just check whether a flush is occurring 
	// on the primary session (always index 0).
	// If a background op is in progress, then the copy will be performed on a secondary session.
	if (iCurrentOp == ECancelRecoverPrimary || iMigrateState != ENotMigrating)
		{
		// Do not call Complete() - this would result in the Protocol
		// Controller disconnecting all sessions.
		TRequestStatus* pStatus = &aStatus;
		User::RequestComplete(pStatus, KErrServerBusy);

		return;
		}
	
	// must be set before calling SelectSession
	iRequestedOp = ECopyToLocal;

	// request the session to use.
	TBool startNow = SelectSessionL(iForegroundSession);

	if (iBackgroundSyncOp && iImapSettings->UseSyncDownloadRules())
		{
		// we need a non-const copy of the message selection to operate on.
		CMsvEntrySelection* selection = aSourceSel.CopyLC();
		
		// show the list to the background sync controller
		iBackgroundSyncOp->RemoveFromSelectionL(*selection);
		// Create the copy to local operation object using the (possibly) updated selection
		iImapCompound = CImapCompoundCopyToLocal::NewL(*iImapSyncManager, 
		                    			               iEntry, 
		                                			   *iImapSettings,
		                                			   *iImapMailStore,
		                                               EFalse,
		                                               *selection,
		                                               aDestination);
		CleanupStack::PopAndDestroy(selection);
		}
	else
		{
		// Create the compound operation object
		iImapCompound = CImapCompoundCopyToLocal::NewL(*iImapSyncManager, 
		                    			               iEntry, 
		                                			   *iImapSettings,
		                                			   *iImapMailStore,
		                                               EFalse,
		                                               aSourceSel,
		                                               aDestination);
		}

	if (startNow)
		{
		// A session is ready - start the command
		// otherwise a request to connect a secondary session has been issued
		if (iForegroundSession==0)
			{
			// Primary session
			StartPrimaryOperation();
			}
		else
			{
			// Secondary session
			iCurrentOp = iRequestedOp;
			iImapCompound->StartOperation(iStatus, *iImapSessionArray[iForegroundSession]);
			}
		}	
	
	Queue(aStatus);
	SetActive();
	}


/**
PopulateL() performs the same action as CopyToLocalL(), however uses the passed 
TImImap4GetPartialMailInfo object instead of using the default parameters. This 
allows the client application to specify the parts of the message that are to be 
fetched, and to impose limits to on the amount of data that can be transferred.

Sequence For Populate
	StopIdle
	SelectSourceMailboxRO
	FetchMessage(s)
	InboxDuplicateCopy

@param aStatus
@param aSourceSel
@param aGetPartialMailInfo
*/
EXPORT_C void CImapProtocolController::PopulateL(TRequestStatus& aStatus, 
                                                 const CMsvEntrySelection& aSourceSel, 
                                                 TImImap4GetPartialMailInfo aGetPartialMailInfo)
	{
	__LOG_FORMAT((KDefaultLog, "CImapProtocolController::PopulateL() - START - iCurrentOp: %d, iRequestedOp: %d", iCurrentOp, iRequestedOp ));
	__ASSERT_DEBUG(iImapCompound==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EPopulateCompoundIsNotNull));

	// Instead of calling CompleteIfBackgroundOpInProgress(), just check whether a flush is occurring 
	// on the primary session (always index 0).
	// If a background op is in prgoress, then the copy will be performed on a secondary session.
	ResetProgress();

	// If a flush is occurring on the primary session (always index 0) then we are busy
	if (iCurrentOp == ECancelRecoverPrimary || iMigrateState != ENotMigrating)
		{
		// Do not call Complete() - this would result in the Protocol
		// Controller disconnecting all sessions.
		TRequestStatus* pStatus = &aStatus;
		User::RequestComplete(pStatus, KErrServerBusy);
	    __LOG_TEXT(KDefaultLog, "CImapProtocolController::PopulateL() - END - KErrServerBusy...");
		return;
		}

	// must be set before calling SelectSessionL()
	iRequestedOp = EPopulate;		

	// request the session to use.
	TBool startNow = SelectSessionL(iForegroundSession);
	__LOG_FORMAT((KDefaultLog, "CImapProtocolController::PopulateL() - iCurrentOp: %d, iRequestedOp: %d, startNow: %d", iCurrentOp, iRequestedOp, startNow ));
	
	if (iBackgroundSyncOp && iImapSettings->UseSyncDownloadRules())
		{
		// we need a non-const copy of the message selection to operate on.
		CMsvEntrySelection* selection = aSourceSel.CopyLC();
		
		// show the list to the background sync controller
		iBackgroundSyncOp->RemoveFromSelectionL(*selection);
		// Create the copy to local operation object using the (possibly) updated selection
		iImapCompound = CImapCompoundCopyToLocal::NewL(*iImapSyncManager, 
			                			               iEntry, 
		    	                        			   *iImapSettings,
		        	                    			   *iImapMailStore, 
		            	                               EFalse,
		                	                           *selection,
		                    	                       KMsvNullIndexEntryId,
		                        	                   aGetPartialMailInfo);
		CleanupStack::PopAndDestroy(selection);
		}
	else
		{
		// Create the compound operation object
		iImapCompound = CImapCompoundCopyToLocal::NewL(*iImapSyncManager, 
	    	                			               iEntry, 
	        	                        			   *iImapSettings,
	            	                    			   *iImapMailStore, 
	                	                               EFalse,
	                    	                           aSourceSel,
	                        	                       KMsvNullIndexEntryId,
	                            	                   aGetPartialMailInfo);
		}

	__LOG_TEXT(KDefaultLog, "CImapProtocolController::PopulateL() - done creation of CompoundCopyToLocal...");
	if (startNow)
		{
		// A session is ready - start the command
		// otherwise a request to connect a secondary session has been issued
		if (iForegroundSession==0)
			{
			// Primary session
	        __LOG_TEXT(KDefaultLog, "CImapProtocolController::PopulateL() - starting primary op...");
			StartPrimaryOperation();
			}
		else
			{
			// Secondary session
	        __LOG_TEXT(KDefaultLog, "CImapProtocolController::PopulateL() - starting compound op...");
			iCurrentOp = iRequestedOp;
			iImapCompound->StartOperation(iStatus, *iImapSessionArray[iForegroundSession]);
			}
		}	
	
	Queue(aStatus);
	SetActive();
	__LOG_FORMAT((KDefaultLog, "CImapProtocolController::PopulateL() - END - iCurrentOp: %d, iRequestedOp: %d, startNow: %d", iCurrentOp, iRequestedOp, startNow ));
	}

/**
MoveToCopyL() shall enable a selection of messages to be moved as a single operation, 
without return to idle state between handling each message. If a message has not been 
previously fetched (ie is not complete on the local mirror of the IMAP folder) the 
message is fetched to the local mirror and a copy is then made in the local service 
folder. The message is then deleted from the IMAP folder on the remote server and the 
local mirror.

Sequence For MoveToLocal
	StopIdle
	SelectSourceMailbox 
	FetchMessage
	InboxDuplicateCopy
	DeleteMessage

@param aStatus
@param aSourceSel
@param aDestination
*/	
EXPORT_C void CImapProtocolController::MoveToLocalL(TRequestStatus& aStatus, 
													const CMsvEntrySelection& aSourceSel, 
													const TMsvId aDestination)
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::MoveToLocalL()");
	__ASSERT_DEBUG(iImapCompound==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EMoveToLocalCompoundIsNotNull));

	ResetProgress();

	if (!CompleteIfBackgroundOpInProgress(aStatus))	
		{
		// We can't do a 'movetolocal' with the destination being the same as the
		// source, as then the mirror will be out of sync. We must be moving to somewhere
		// outside this service. Check this.
		if (!IdIsLocalL(aDestination))
			{
			// Complete the user request
			// Do not call Complete() - this would result in the Protocol
			// Controller disconnecting all sessions.
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, KErrNotSupported);
			}
		else
			{
			iImapCompound = CImapCompoundCopyToLocal::NewL(*iImapSyncManager, 
														   iEntry, 
														   *iImapSettings,
														   *iImapMailStore,
														   ETrue,
		                                               	   aSourceSel,
		                                               	   aDestination);
			
			iRequestedOp = EMoveToLocal;
			StartPrimaryOperation();
			Queue(aStatus);
			SetActive();
			}
		}
	}

/**
Copy within service is the operation of moving a message or selection of messages from 
one mailbox  to another on the remote IMAP server. This is done by issuing a COPY IMAP 
command to the remote server, followed by a sync operation on the destination folder.

Sequence For CopyWithinService
	StopIdle
	SelectSourceMailboxRO
	CopyMessage
	SelectDestinationMailboxRO
	NewSyncFolder

@param aStatus
@param aSourceSel
@param aDestination
*/
EXPORT_C void CImapProtocolController::CopyWithinServiceL( TRequestStatus& aStatus, 
							 					  		   const CMsvEntrySelection& aSourceSel, 
							 					  		   const TMsvId aDestination )
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::CopyWithinServiceL()");
	__ASSERT_DEBUG(iImapCompound==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECopyWithinServiceCompoundIsNotNull));
	
	ResetProgress();

	if (!CompleteIfBackgroundOpInProgress(aStatus))	
		{	
		iImapCompound = CImapCompoundCopyWithinService::NewL( *iImapSyncManager, 
															  iEntry, 
															  *iImapSettings,
															  EFalse,
															  aSourceSel,
															  aDestination);
		
		iRequestedOp = ECopyWithinService;
		StartPrimaryOperation();
		Queue(aStatus);
		SetActive();
		}
	}

/**
Move within service is the same as Copy Within Service, however the message is deleted 
from the original location following the copy operation.

Sequence For MoveWithinService
	StopIdle
	SelectSourceMailboxRW
	CopyMessage
	DeleteMessage
	SelectDestinationMailboxRO
	NewSyncFolder
	
@param aStatus
@param aSourceSel
@param aDestination
*/
EXPORT_C void CImapProtocolController::MoveWithinServiceL( TRequestStatus& aStatus, 
							 					  		   const CMsvEntrySelection& aSourceSel, 
							 					  		   const TMsvId aDestination )
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::MoveWithinServiceL()");
	__ASSERT_DEBUG(iImapCompound==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EMoveWithinServiceCompoundIsNotNull));
	
	ResetProgress();
	
	if (!CompleteIfBackgroundOpInProgress(aStatus))	
		{	
		iImapCompound = CImapCompoundCopyWithinService::NewL( *iImapSyncManager, 
															  iEntry, 
															  *iImapSettings,
															  ETrue,
															  aSourceSel,
															  aDestination );

		iRequestedOp = EMoveWithinService;
		StartPrimaryOperation();
		Queue(aStatus);
		SetActive();
		}
	}

/**
Copy from local is the action of copying an email from a local service folder (for 
example the local inbox) to a folder on the remote IMAP service. This is done using 
the IMAP APPEND command.

Sequence For CopyFromLocal
	AppendMessage
	SelectDestinationMailboxRO
	NewSyncFolder

@param aStatus
@param aSourceSel
@param aDestination
*/	
EXPORT_C void CImapProtocolController::CopyFromLocalL( TRequestStatus& aStatus, 
						 					  		   const CMsvEntrySelection& aSourceSel, 
						 					  		   const TMsvId aDestination )
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::CopyFromLocalL()");
	__ASSERT_DEBUG(iImapCompound==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECopyFromLocalCompoundIsNotNull));
	
	ResetProgress();
	
	if (!CompleteIfBackgroundOpInProgress(aStatus))	
		{	
		iImapCompound = CImapCompoundCopyFromLocal::NewL( *iImapSyncManager, 
														  iEntry, 
														  *iImapSettings,
														  EFalse,
														  aSourceSel,
														  aDestination );
		
		iRequestedOp = ECopyFromLocal;
		StartPrimaryOperation();
		Queue(aStatus);
		SetActive();
		}
	}

/**
Move from local is the action of moving an email from a local service folder (for 
example the local inbox) to a folder on the remote IMAP service. It is performed 
by following the same steps as for a Copy From Local operation, using the IMAP APPEND 
command, however the local copy of the message is deleted after the APPEND has 
successfully completed.

Sequence For MoveFromLocal
	StopIdle
	AppendMessage
	DeleteLocalMessage
	SelectDestinationMailboxRO
	NewSyncFolder

@param aStatus
@param aSourceSel
@param aDestination
*/
EXPORT_C void CImapProtocolController::MoveFromLocalL( TRequestStatus& aStatus, 
						 					  		   const CMsvEntrySelection& aSourceSel, 
						 					  		   const TMsvId aDestination )
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::MoveFromLocalL()");
	__ASSERT_DEBUG(iImapCompound==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECopyFromLocalCompoundIsNotNull));
	
	ResetProgress();
	
	if (!CompleteIfBackgroundOpInProgress(aStatus))	
		{
		iImapCompound = CImapCompoundCopyFromLocal::NewL( *iImapSyncManager, 
														  iEntry, 
														  *iImapSettings,
														  ETrue,
														  aSourceSel,
														  aDestination );
	
		iRequestedOp = EMoveFromLocal;
		StartPrimaryOperation();
		Queue(aStatus);
		SetActive();
		}
	}

/**
Deletes the specified messages from the remote server.

Sequence For Delete
	StopIdle
	SelectSourceMailboxRW
	Store /delete flags for each message
	Expunge remote messages
	Delete local messages

@param aStatus
@param aSourceSel
*/
EXPORT_C void CImapProtocolController::DeleteL(TRequestStatus& aStatus, 
									   		   const CMsvEntrySelection& aSourceSel)
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::DeleteL()");
	__ASSERT_DEBUG(iImapCompound==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EDeleteCompoundIsNotNull));
	
	ResetProgress();
	
	if (!CompleteIfBackgroundOpInProgress(aStatus))	
		{
		// Create the compound object for message delete
		iImapCompound = CImapCompoundDelete::NewL(*iImapSyncManager,
												  iEntry,
												  *iImapSettings,
												  aSourceSel);

		iRequestedOp = EDelete;
		StartPrimaryOperation();
		Queue(aStatus);
		SetActive();
		}
	}

/**
Deletes the specified folder(s) on the remote IMAP Server.

Sequence For DeleteFolder
	StopIdle
	SelectSourceMailboxRW
	DeleteAllMessages
	CloseFolder
	DeleteLocalFolder

@param aStatus
@param aSelection
*/
EXPORT_C void CImapProtocolController::DeleteFolderL( TRequestStatus& aStatus, 
											 		  const CMsvEntrySelection& aSelection )
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::DeleteFolderL()");
	__ASSERT_DEBUG(iImapCompound == NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EDeleteFolderCompoundIsNotNull));

	ResetProgress();
	
	if (!CompleteIfBackgroundOpInProgress(aStatus))	
		{
		iImapCompound = CImapCompoundDeleteFolder::NewL( *iImapSyncManager,
													     iEntry,
													     *iImapSettings,
													     aSelection );
													     
		iRequestedOp = EDelete;
		StartPrimaryOperation();
		Queue(aStatus);
		SetActive();
		}	
	}

/**
NewOnlySyncL() is used to synchronise recent messages in the specified folder, 
i.e. any messages that have arrived in the remote IMAP mailbox since the last 
synchronisation. This is done by requesting (FETCHing) the header details for 
messages with UID's that are greater than the highest UID of the messages present 
in the local mirror of the folder. The message's header summary information, 
flags etc are stored as new entries in the Message Server entry array under the 
folder entry, and the header information is streamed to the Mailstore using the 
Mailstore API. 

Sequence For NewSyncFolder
	StopIdle
	SelectSourceMailboxRW
	NewSyncFolder

@param aStatus
*/
EXPORT_C void CImapProtocolController::NewOnlySyncFolderL( TRequestStatus& aStatus )
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::NewOnlySyncFolderL()");
	__ASSERT_DEBUG(iImapCompound == NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ENewOnlySyncFolderCompoundIsNotNull));

	ResetProgress();
	
	if (iMigrateState!=ENotMigrating)
		{
		// Complete the user request if migrating.
		// Do not call Complete() - this would result in the Protocol
		// Controller disconnecting all sessions.
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrServerBusy);
		return;
		}
	
	// If we support idle, and the server we are talking to supports idle,
	// then we don't need to do the new only sync as IMAP idle will take
	// care of it for us.
	// Just complete the user request with KErrNone.
	if (iImapSettings->ImapIdle() && iImapSessionArray[0]->ImapIdleSupported())
		{
		__LOG_TEXT(KDefaultLog, "CImapProtocolController::NewOnlySyncFolderL() - Immediate complete as idle is in use");
		Queue(aStatus);
		Complete(KErrNone);
		return;
		}
	
	if (!CompleteIfBackgroundOpInProgress(aStatus))	
		{	
		CImapFolder* folder = iImapSyncManager->Inbox();
		TMsvId inboxId = folder->MailboxId();
		
		iImapCompound = CImapCompoundSyncFolder::NewL( *iImapSyncManager,
													   iEntry,
													   *iImapSettings, 
													   *iImapMailStore, 
													   ETrue,
													   inboxId);

		iRequestedOp = ESync;
		StartPrimaryOperation();
		Queue(aStatus);
		SetActive();
		}
	}

/**
The full synchronisation of a folder involves the synchronisation of messages newly 
received at the IMAP server, as described for NewOnlySyncL, above, and the 
synchronisation of "old messages", ie messages that have previously been synchronised 
to the local mirror of the folder.
When synchronising old messages, messages that have been marked for delete locally 
are marked such on the remote folder (the actual delete occurs either at the end of 
the synchronisation process, or is deferred until the connection is cancelled), 
messages that have been removed from the remote server (by another client) are 
deleted locally and any outstanding offline operations are performed, for example 
move operations may be outstanding, etc.

Sequence For FullSyncFolder
	StopIdle
	SelectSourceMailboxRW
	SyncFolder

@param aStatus
@param aFolder
*/
EXPORT_C void CImapProtocolController::FullSyncFolderL( TRequestStatus& aStatus, 
											   			const TMsvId aFolder )
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::FullSyncFolderL()");
	__ASSERT_DEBUG(iImapCompound == NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EFullSyncFolderCompoundIsNotNull));
	
	ResetProgress();

	if (!CompleteIfBackgroundOpInProgress(aStatus))	
		{
		iImapCompound = CImapCompoundSyncFolder::NewL( *iImapSyncManager,
													   iEntry,
													   *iImapSettings,
													   *iImapMailStore,
													   EFalse,
													   aFolder );

		iRequestedOp = ESync;
		StartPrimaryOperation();
		Queue(aStatus);
		SetActive();
		}
	}

/**
SELECT is made available to the client application directly via an IMAP MTM command 
which is handled by this function. It allows the client to specifically SELECT a 
mailbox and then, using the Synchronise MTM command, to specifically request the 
selected mailbox is synchronised.

Sequence For Select
	StopIdle
	SelectSourceMailboxRW

@param aStatus
@param aFolder
*/	
EXPORT_C void CImapProtocolController::SelectL(TRequestStatus& aStatus, const TMsvId aFolder)
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::SelectL()");
	__ASSERT_DEBUG(iImapCompound == NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ESelectCompoundIsNotNull));
	
	ResetProgress();
	
	if (!CompleteIfBackgroundOpInProgress(aStatus))	
		{
		iImapCompound = CImapCompoundSelect::NewL( *iImapSyncManager, 
												   iEntry, 
												   *iImapSettings,
												   aFolder );

		iRequestedOp = ESelect;
		StartPrimaryOperation();
		Queue(aStatus);
		SetActive();
		}
	}

/**
Performs a full sync on the currently selected mailbox.

Sequence For FullSyncSelectedFolder
	StopIdle
	SyncFolder

@param aStatus
*/	
EXPORT_C void CImapProtocolController::FullSyncSelectedFolderL( TRequestStatus& aStatus )
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::FullSyncSelectedFolderL()");
	__ASSERT_DEBUG( iImapCompound == NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EFullSyncSelectedFolderCompoundIsNotNull));
	
	ResetProgress();
	
	if (!CompleteIfBackgroundOpInProgress(aStatus))	
		{
		iImapCompound = CImapCompoundSyncFolder::NewL( *iImapSyncManager,
													   iEntry,
													   *iImapSettings,
													   *iImapMailStore, 
													   EFalse );
		
		iRequestedOp = ESync;
		StartPrimaryOperation();
		Queue(aStatus);
		SetActive();
		}
	}
	
/**
Creates a new folder on the remote IMAP service with the given name.

Sequence For Create
	StopIdle
	Create

@param aStatus
@param aParent
@param aLeafName
@param aFolder
*/
EXPORT_C void CImapProtocolController::CreateL( TRequestStatus& aStatus, 
				  					   			const TMsvId aParent, 
				  					   			const TDesC& aLeafName, 
				  					   			const TBool aFolder )
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::CreateL()");
	__ASSERT_DEBUG(iImapCompound==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECreateCompoundIsNotNull));
	
	ResetProgress();
	
	if (!CompleteIfBackgroundOpInProgress(aStatus))	
		{
		iImapCompound = CImapCompoundCreate::NewL( *iImapSyncManager,
												   iEntry,
												   *iImapSettings,
												   aParent, 
												   aLeafName, 
												   aFolder );

		iRequestedOp = ECreate;
		StartPrimaryOperation();
		Queue(aStatus);
		SetActive();
		}
	}

/**
Renames the specified folder. Note that although this does cause a write to the 
remote folder, it does not change the contents of the folder and hence does not 
force a re-synchronisation of the folder.

Sequence For Rename
	StopIdle
	RenameRemote
	RenameLocal

@param aStatus
@param aTarget
@param aNewName
*/	
EXPORT_C void CImapProtocolController::RenameL( TRequestStatus& aStatus, 
				  					   			const TMsvId aTarget, 
				  					   			const TDesC& aNewName )
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::RenameL()");
	__ASSERT_DEBUG(iImapCompound == NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ERenameCompoundIsNotNull));
	
	ResetProgress();
	
	if (!CompleteIfBackgroundOpInProgress(aStatus))	
		{
		iImapCompound = CImapCompoundRename::NewL( *iImapSyncManager, 
												   iEntry, 
												   *iImapSettings,
												   aTarget,
												   aNewName );
												   
		iRequestedOp = ERename;
		StartPrimaryOperation();
		Queue(aStatus);
		SetActive();
		}
	}


EXPORT_C void CImapProtocolController::DoRunL()
	{
	// Handle all migration operations in a separate state machnine.
	if (iMigrateState != ENotMigrating)
		{
		DoMigrateRunL();
		return;
		}

	// ProcessError completes and returns ETrue if an error occured
	if (ProcessError(iStatus.Int()))
		{
		return;
		}

	switch (iCurrentOp)
		{
	case EConnect:
		{
		// Collect the final connect progress information		
		iImapSessionManager->Progress(iProgress.iGenericProgress);

		// Set last socket activity timeout to iMtmData1. This is used by Imcm.
		User::LeaveIfError( iEntry.SetEntry( iServiceId ) );
		TMsvEntry entry=iEntry.Entry();
		entry.SetMtmData1(iImapSessionManager->LastSocketActivityTimeout());
		User::LeaveIfError( iEntry.ChangeEntry( entry ) );

		TRAP_IGNORE(MarkOnOrOfflineL(ETrue));
				
		// Create an IMAP IDLE controller
		delete iImapIdleController;
		iImapIdleController=NULL;
		iImapIdleController = CImapIdleController::NewL(*this, iImapSessionArray[0], *iImapSyncManager, iEntry, *iImapSettings, *iImapMailStore);
		
		// Register the connection with the mobility manager, 
		// if we are a mobile service
		if (iMobilityManager)
			{
			iMobilityManager->SetConnection(iImapSessionManager->GetConnectionL());
			}

		// kick off a background sync if it was requested.
		if (iBackgroundSyncOp != NULL)
			{
			iBackgroundSyncOp->StartSync(*iImapSessionArray[0]);
			iCurrentOp = EIdle;
			}
		else
			{
			StartIdle();
			}

		// complete the connect request
		Complete(iStatus.Int());
		break;
		}

	case EConnectSecondary:
		{
		// perform the user-requested operation using the
		// newly created imap session
		iCurrentOp = iRequestedOp;
		if(iImapCompound->Suspended())
			{
			// if the operation was previously suspended for migration, resume it...
			iImapCompound->ResumeOperationL(iStatus, *iImapSessionArray[iForegroundSession]);
			}
		else
			{
			// otherwise, just start the operation.
			iImapCompound->StartOperation(iStatus, *iImapSessionArray[iForegroundSession]);
			}
		SetActive();
		break;
		}
	
	case EStopIdle:
		{
		// Idle has been cancelled, the primary session is available
		// Kick off the requested operation
		iCurrentOp = iRequestedOp;
		iImapCompound->StartOperation(iStatus, *iImapSessionArray[0]);
		SetActive();
		break;
		}

	case EDisconnect:
		{
		TRAP_IGNORE( MarkOnOrOfflineL( EFalse ) );

		iImapSessionArray.ResetAndDestroy();

		// update last operation progress state
		iImapCompound->Progress(iProgress);

		delete iImapCompound;
		iImapCompound = NULL;
		iServiceId=0;
		
		Complete(iStatus.Int());
		break;
		}
	
	case ECancelRecoverPrimary:
		{
		// Primary session is successfully recovered
		// Start the next requested operation
		
		if (iRequestedOp == EIdle)
			{
			// We should only go into Idle if no async operations have been requested.
			// Consequently, CMsgActive::iReport should be NULL.
			// We can't ASSERT this here as it is a private member of CMsgActive, but it is worth checking while debugging.
			StartIdle();
			}
		else if (iRequestedOp == EDisconnect)
			{
			// We should only start a primary operation if an async operation has been requested, and an aStatus Queue()ed.
			// Consequently, CMsgActive::iReport should NOT be NULL.
			// We can't ASSERT this here as it is a private member of CMsgActive, but it is worth checking while debugging.
			StartPrimaryOperation();
			SetActive();
			}
		else
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::EProtocolControllerUnexpectedRequestedOp));
			}
		
		break;
		}

	case ESync:
	case ESelect:
	case ECopyToLocal:
	case ECopyWithinService:
	case ECopyFromLocal:
	case EMoveToLocal:
	case EMoveWithinService:
	case EMoveFromLocal:
	case EPopulate:
	case EDelete:
	case EDeleteFolder:
	case ECreate:
	case ERename:
	case EUpdateFlag:
		{
		// update last operation progress state
		iImapCompound->Progress(iProgress);

		delete iImapCompound;
		iImapCompound = NULL;
		
		if ((iRequestedOp==ESelect) && (iProgress.iGenericProgress.iErrorCode==KErrNone))
			{
			// Do not start IDLE following a select request
			Complete(iStatus.Int());
			}
		else
			{
			// Start idle and complete the server mtm request.
			StartIdle();
			Complete(iStatus.Int());
			}
		break;
		}

	case EIdle:	// shouldn't happen
	default:
		{
		Complete(iStatus.Int());
		break;
		}
		
		} // end of switch (iCurrentOp)
	}

/**
This variation on DoRunL() presents a super-state machine, that replaces 
the default state machine during a migration process.
*/
void CImapProtocolController::DoMigrateRunL()
	{
	// waiting for existing operation to stop to allow migration to occur.
	__LOG_FORMAT((KDefaultLog, "CImapProtocolController::DoMigrateRunL(iMigrateState = %d) CurrentOp = %d", iMigrateState, iCurrentOp));
	switch (iMigrateState)
		{
 		case EWaitingForOpToStop: 
		case EWaitingForOpToComplete:
			{
			// operation has completed (is either finished, or has "paused"
			// to allow migration to complete. Do any tidying up necessary.
			ProcessOpCompleteForMigrate();
			
			// is there also a background sync op in progress? if not (or if the
			// background sync op is suspended, we are now ready to migrate, 
			// otherwise wait for the background op to complete.
			 // 
			if (!iBackgroundSyncOp || iBackgroundSyncOp->IsSuspendedForMigrate())
				{
				// Asynch disconnect current sockets.
				DisconnectForMigrateL();
				}
			break;
			}
		case EDisconnectingForMigrate:
			{
			// The disconnect operation has completed. Delete IMAP Sessions.
			iImapSessionArray.ResetAndDestroy();

			// delete the migration compound object
			delete iMigrateCompound;
			iMigrateCompound = NULL;
			
			// Notify the mobility framework that we are ready to migrate.
			iMigrateState = EWaitingForNewCarrier;
			iMobilityManager->MigrateToNewCarrier();
			
			// do not set waiting if NewCarrierActive has been called synchronously.
			if (iMigrateState == EWaitingForNewCarrier)
				{
				// Now in a waiting state, set self active
				iStatus = KRequestPending;
				SetActive();
				}
			break;
			}
		case EHandlingConnectError:
			{
			// register with the mobility framework
			iMobilityManager->SetConnection(iImapSessionManager->GetConnectionL());
						
			// empty the session array
			TInt numSessions = iImapSessionArray.Count();
			for (TInt i=0; i<numSessions; ++i)
				{
				if (iImapSessionArray[i])
					{
					iImapSessionManager->Disconnect(*(iImapSessionArray[i]));
					}
				}
			iImapSessionArray.ResetAndDestroy();

			// reject the initial carrier
			iMigrateState = EWaitingInitialCarrierRejected;
			iMobilityManager->NewCarrierRejected();
			
			if (iMigrateState == EWaitingInitialCarrierRejected)
				{
				iStatus = KRequestPending;
				SetActive();
				}
			break;
			}
		case EStartingReconnect:
			{
			NewPrimarySessionL();
			break;
			}
		case EConnectingAfterMigrate:
			{
			if (iStatus.Int()!=KErrNone)
				{
				// An error has occurred while attempting to re-connect
				// - reject this new carrier, wait to see if a new one turns up.
				iMigrateState = EWaitingForNewCarrier;
				iMobilityManager->NewCarrierRejected();

				// Now in a waiting state, set self active
				if (iMigrateState == EWaitingForNewCarrier)
					{
					iStatus = KRequestPending;
					SetActive();
					}
				}
			else
				{
				iMobilityManager->NewCarrierAccepted();
				RestartAfterMigrateL();
				}
			break;
			}
		case ENotMigrating:
		case EWaitingForNewCarrier:
		case EWaitingInitialCarrierRejected:
			// DoRunMigrateL() Should never be called in this state.
		default:
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::EProtocolControllerUnexpectedMigrateState));
			break;
			}
		}
	}


/** 
Called when completing an outstanding client request.
Handles negative (system-wide) error codes returned
on completion of asynchronous service requests.
*/
void CImapProtocolController::DoComplete(TInt& aErr)
	{
	__LOG_FORMAT((KDefaultLog, "CImapProtocolController::DoComplete() - START - aErr = %d, CurrentOp = %d, iCancelInProgress: %d", aErr, iCurrentOp, iCancelInProgress));

	// Requested operation is completed.
	iRequestedOp = EIdle;

	// return if everything is OK
	if (aErr==KErrNone)
		{
	    __LOG_TEXT( KDefaultLog, "CImapProtocolController::DoComplete() - END - KErrNone" );
		return;
		}
		
	// First retrieve the progress information
	this->Progress();
	
	// log that a cancel has occurred and return
	if (aErr==KErrCancel)
		{
	    __LOG_TEXT( KDefaultLog, "CImapProtocolController::DoComplete() - dealing with cancel case..." );
		iProgress.iGenericProgress.iErrorCode=aErr;
		if(!iCancelInProgress)
			{
	        __LOG_TEXT( KDefaultLog, "CImapProtocolController::DoComplete() - no cancel in progress, so disconnecting all..." );
			DisconnectAll();
			}
			
	    __LOG_TEXT( KDefaultLog, "CImapProtocolController::DoComplete() - END - KErrCancel" );
		return;
		}

	// Non-fatal errors should have been handled prior to this.
	__LOG_TEXT( KDefaultLog, "CImapProtocolController::DoComplete() - fatal error - disconnect all..." );
	// iServiceId = 0, indicates we are already disconnected.
	if(iServiceId!=0)
		{
		DisconnectAll();
		}
	
	// Save error code in progress and flag the disconnect
	iProgress.iGenericProgress.iState=TImap4GenericProgress::EDisconnected;
	iProgress.iGenericProgress.iErrorCode=aErr;

    __LOG_TEXT( KDefaultLog, "CImapProtocolController::DoComplete() - END - fatal error" );
	}
	

/**
Cancels any outstanding client requested asynchronous operations and recovers
the primary session if required.

This function is exclusively for the use of the CImapServerMtm, and only calls
Cancel() if the PC is currently active performing the requested operation. 
If the PC is active for other reasons (for example, waiting for a bearer
migration to complete), the appropriate action is taken.
*/
EXPORT_C void CImapProtocolController::CancelAndCleanup()
	{
	__LOG_FORMAT( (KDefaultLog, "CImapProtocolController::CancelAndCleanup() - START - iCurrentOp: %d, iRequestedOp: %d, iFlushPrimary: %d", iCurrentOp, iRequestedOp, iFlushPrimary ) );
	if ( iMigrateState == ENotMigrating ||
	     iMigrateState == EWaitingForOpToComplete ||
	     iMigrateState == EWaitingForOpToStop )
		{
		// In these states the protocol controller is currently doing something
		// at the clients request.
		// iCancelInProgress
		iCancelInProgress = ETrue;
		Cancel();

	    __LOG_TEXT(KDefaultLog, "CImapProtocolController::CancelAndCleanup() - done DoCancel");

		// DoCancel() will set iFlushPrimary to ETrue for operations that require 
		// a flush after being cancelled. This will cause DoRunL() to be called when the
		// session has been flushed. In the case of bearer migration, the next step will
		// be started in DoRunL.
		if (iFlushPrimary) 
			{
			iRequestedOp = EIdle; // We want to return to idle when the flush has completed.
			iCurrentOp = ECancelRecoverPrimary;
			CImapSession* session = iImapSessionArray[0];
			session->FlushCancelledCommand(iStatus);
			SetActive();
			iFlushPrimary = EFalse;
			}
		// clear the cancelling flag
		iCancelInProgress = EFalse;
		}
	else			
		{
		// in other migration states, we don't want to cancel because we 
		// are active for migration purposes. However, we do need to clean
		// compound operation objects and complete the user.
		iRequestedOp = iCurrentOp = EIdle;
		
		// Update the progress object
		iProgress.iGenericProgress.iState = TImap4GenericProgress::EIdle;
		iProgress.iGenericProgress.iErrorCode = KErrCancel;

		delete iImapCompound;
		iImapCompound = NULL;
		CMsgActive::DoCancel(); // completes the user request.
		}
    __LOG_TEXT(KDefaultLog, "CImapProtocolController::CancelAndCleanup() - END");
	}


/**
DoCancel - called by CMsgActive::Cancel() to cancel asychronous services:

Cancel() must not be called on this class to cancel a requested async
service - CancelAndCleanup() should be used instead. CancelAndCleanup()
provides support for cancelling requested operations while this class
has become active for migration purposes.

However, Cancel() must always be called eventually as the iStatus needs 
to be cleared. Hence this function must support all cancel operations.

This class is allows for Cancel() to be called internally to cancel
async operations that have been launched internally for migration 
purposes, without completing the CImapServerMtm.
*/
void CImapProtocolController::DoCancel()
	{
	__LOG_FORMAT( (KDefaultLog, "CImapProtocolController::DoCancel() - START - iCurrentOp: %d, iRequestedOp: %d, iFlushPrimary: %d", iCurrentOp, iRequestedOp, iFlushPrimary ) );

	if (iMigrateState==ENotMigrating)
		{
		DoCancelClientOp();
		}
	else
		{
		switch (iMigrateState)
			{
			case ESuspendingForMigrate:
				{
				// Special handling for cancelling current operation to allow
				// migration to occur.
				DoCancelForMigrate();
				break;
				}
			case EWaitingForOpToComplete:
			case EWaitingForOpToStop:
				{
				// we have an outstanding request on the compound operation
				// call the default DoCancelClientOp(). This will also
				// complete the client's iStatus with KErrCancel
				DoCancelClientOp();
				break;
				}
			case EDisconnectingForMigrate:
				{
				// outstanding op is on the iMigrateCompound
				iMigrateCompound->Cancel();
				delete iMigrateCompound;
				iMigrateCompound = NULL;
				break;
				}
			case EConnectingAfterMigrate:
				{
				// This state represents re-connection of the primary session,
				// it is safe to simply clear the imap session array.
				iImapSessionManager->Cancel();
				iImapSessionArray.ResetAndDestroy();
				break;
				}
			case EWaitingForNewCarrier:
			case EWaitingInitialCarrierRejected:
				{
				// in these states, we are in a self-induced active state
				// Cancel it:
				TRequestStatus* status = &iStatus;
				User::RequestComplete(status, KErrCancel);
				break;
				}

			case EStartingReconnect: // nothing to cancel
			case EHandlingConnectError: // nothing to cancel
			case ENotMigrating:		 // somethings gone wrong
			default:
				{
				__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::EProtocolControllerCancelBadMigrateState));
				}
			} // switch (iMigrateState)
		}

	__LOG_TEXT(KDefaultLog, "CImapProtocolController::DoCancel() - END");
	}

void CImapProtocolController::DoCancelClientOp()
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::DoCancelClientOp() - START");

	// Update the requested and current operations
	// the implication is that a return to EIdle is required
	// current op may be updated later if an async op is issued.
	TImapProtocolOp cancelledOp = iCurrentOp;
	iRequestedOp = iCurrentOp = EIdle;

	switch (cancelledOp)
		{
		case EConnect:
			{
			__LOG_TEXT(KDefaultLog, "CImapProtocolController::DoCancelClientOp() - EConnect");
			iImapSessionManager->Cancel();
			
			// As this state represents connection of the primary session,
			// it is safe to simply clear the imap session array.
			iImapSessionArray.ResetAndDestroy();

			// delete the background sync operation if it exists.
			if (iBackgroundSyncOp != NULL)
				{
				iBackgroundSyncOp->Cancel();
				delete iBackgroundSyncOp;
				iBackgroundSyncOp=NULL;
				}

			// delete the sync manager		
			delete iImapSyncManager;
			iImapSyncManager = NULL;

			break;
			}
		
		case EConnectSecondary:
			{
	        __LOG_TEXT(KDefaultLog, "CImapProtocolController::DoCancelClientOp() - EConnectSecondary");
			// Cancel the connect request
			iImapSessionManager->Cancel();
			iForegroundSession=0;
			
			// Remove the session pointer from the array
			// The session has already been deleted - see note above.
			TInt numSessions = iImapSessionArray.Count();
			iImapSessionArray.Remove(numSessions-1);
			
			if (iImapCompound!=NULL)
				{
				// The requested op hasn't actually started yet
				// so don't cancel on the compound operation..
				// update last operation progress state
				iImapCompound->Progress(iProgress);
				
				delete iImapCompound;
				iImapCompound = NULL;
				}
			
			// The progress will not report cancel at this stage - force it here.
			iProgress.iGenericProgress.iState = TImap4GenericProgress::EIdle;
			iProgress.iGenericProgress.iErrorCode = KErrCancel;

			break;
			}

		case EDisconnect:
			{
	        __LOG_TEXT(KDefaultLog, "CImapProtocolController::DoCancelClientOp() - EDisconnect");
			// Cancel the disconnect compound operation
			iImapCompound->Cancel();

			delete iImapCompound;
			iImapCompound = NULL;
			
			// A cancelled disconnect will leave the sessions disconnected
			// but not deleted. Reset and destroy the session array.
			iImapSessionArray.ResetAndDestroy();
			
			break;
			}
		
		case EIdle:
			{
			// This is unexpected - in this state there should be no
			// outstanding asynconous requests on the Protocol Controller.
			// Protocol Controller has no outstanding requests in this state
			// however a sync may be running in the background. This is
			// cancelled when the background sync object's destructor is called.
			// So.. Nothing to do.
			break;
			}
		
		case EStopIdle:
			{
			if (iImapCompound!=NULL)
				{
				// The requested op hasn't actually started yet
				// so don't cancel on the compound operation..
				// update last operation progress state
				iImapCompound->Progress(iProgress);
				
				delete iImapCompound;
				iImapCompound = NULL;
				}
			
			// The progress will not report cancel at this stage - force it here.
			iProgress.iGenericProgress.iState = TImap4GenericProgress::EIdle;
			iProgress.iGenericProgress.iErrorCode = KErrCancel;
			
			// Cancel the idle controller.
			iImapIdleController->Cancel();
			
			// Cancelling the idle controller is likely to leave the IMAP Session
			// with data left on the input stream to be dealt with. We need to 
			// flush the session if we wish to re-use it.
			iFlushPrimary = ETrue;

			break;
			}

		case ESync:
		case ESelect:
		case ECopyToLocal:
		case ECopyWithinService:
		case ECopyFromLocal:
		case EMoveToLocal:
		case EMoveWithinService:
		case EMoveFromLocal:
		case EPopulate:
		case EDelete:
		case EDeleteFolder:
		case ECreate:
		case ERename:
	case EUpdateFlag:
			{
			__LOG_TEXT(KDefaultLog, "CImapProtocolController::DoCancelClientOp() - ESync/ESelect/ECopyToLocal/ECopyWithinService/ECopyFromLocal/EMoveToLocal/EMoveWithinService/EMoveFromLocal/EPopulate/EDelete/EDeleteFolder/ECreate/ERename/");
			// This is the normal case, a requested operation is
			// being performed by a compound operation object.
			// First of all, check one exists.
			if (iImapCompound!=NULL)
				{
				// Cancel the compound operation
				__LOG_TEXT(KDefaultLog, "CImapProtocolController::DoCancelClientOp() - cancelling iImapCompound...");
				iImapCompound->Cancel();

				// update last operation progress state
				__LOG_TEXT(KDefaultLog, "CImapProtocolController::DoCancelClientOp() - getting progres...");
				iImapCompound->Progress(iProgress);

				// Delete the compound object
				__LOG_TEXT(KDefaultLog, "CImapProtocolController::DoCancelClientOp() - deleting compound...");
				delete iImapCompound;
				iImapCompound = NULL;
				
				// Cancelling the compound operation is likely to leave 
				// the IMAP Session in an incomplete state. We need to 
				// flush the session if we wish to re-use it.
				if (iForegroundSession==0)
					{
					__LOG_TEXT(KDefaultLog, "CImapProtocolController::DoCancelClientOp() - flush primary = ETrue...");
					iFlushPrimary = ETrue;
					}
				else
					{
					__LOG_TEXT(KDefaultLog, "CImapProtocolController::DoCancelClientOp() - drop session...");
					DropSession(iForegroundSession);
					iForegroundSession=0;
					}

				__LOG_TEXT(KDefaultLog, "CImapProtocolController::DoCancelClientOp() - main iImapCompound handling complete...");
				}
			break;
			}
	
		case ECancelRecoverPrimary:
			{
			// The recover must be taking too long.
			// cancel the recover request..
			(iImapSessionArray[0])->Cancel();
			// Delete all sessions, go offline etc.
			DisconnectAll();
			break;
			}
		default:
			{
			// nothing to do..
			break;
			}
		} // end of switch (cancelledOp)

	__LOG_TEXT(KDefaultLog, "CImapProtocolController::DoCancelClientOp() - about to call CMsgActive::DoCancel()...");
	CMsgActive::DoCancel();

	__LOG_TEXT(KDefaultLog, "CImapProtocolController::DoCancelClientOp() - END");
	}


/**
Performs a fast disconnect on all sessions

Ensures that the compound, background sync and sync manager objects are
deleted following the disconnect.
@param aMarkOffline - indicates if the service is to be marked offline.
*/
void CImapProtocolController::DisconnectAll()
	{
    __LOG_TEXT(KDefaultLog, "CImapProtocolController::DisconnectAll() - START");

	// Delete the background sync op
    __LOG_TEXT(KDefaultLog, "CImapProtocolController::DisconnectAll() - deleting iBackgroundSyncOp");
	delete iBackgroundSyncOp;
	iBackgroundSyncOp=NULL;

	// Delete the imap compound operation
    __LOG_TEXT(KDefaultLog, "CImapProtocolController::DisconnectAll() - deleting iImapCompound");
	delete iImapCompound;
	iImapCompound=NULL;
	
	// Delete the migrate compound operation
	delete iMigrateCompound;
	iMigrateCompound = NULL;

	// delete the sync manager
    __LOG_TEXT(KDefaultLog, "CImapProtocolController::DisconnectAll() - deleting iImapSyncManager");
	delete iImapSyncManager;
	iImapSyncManager = NULL;
	
	// delete the mobility manager (de-register for mobility notifications)
	delete iMobilityManager;
	iMobilityManager = NULL;

	// delete the imap idle controller
	delete iImapIdleController;
	iImapIdleController = NULL;

	// Drop connection on each of the imap sessions
	TInt numSessions = iImapSessionArray.Count();
	for (TInt i=0; i<numSessions; ++i)
		{
		if (iImapSessionArray[i])
			{
	        __LOG_FORMAT( (KDefaultLog, "CImapProtocolController::DisconnectAll() - disconnecting session: %d/%d", i, numSessions ) );
			iImapSessionManager->Disconnect(*(iImapSessionArray[i]));
			}
		}
		
	// The current network connection will be closed
 	iImapSessionManager->CloseNetworkConnection();


	// empty the session array
    __LOG_TEXT(KDefaultLog, "CImapProtocolController::DisconnectAll() - resetting and destroying sessions...");
	iImapSessionArray.ResetAndDestroy();
	
	// force the state to IDLE
	iRequestedOp = EIdle;
	iCurrentOp = EIdle;
	
	// Clear serviceid and mark the service as offline
    __LOG_TEXT(KDefaultLog, "CImapProtocolController::DisconnectAll() - about to MarkOnOrOffline...");
	TRAP_IGNORE(MarkOnOrOfflineL(EFalse));
	iServiceId=0;

    __LOG_TEXT(KDefaultLog, "CImapProtocolController::DisconnectAll() - END");
	}

/**
Performs an immediate disconnect of specifed session and removes it from
the session array.

@param sessionId - the index in the session array of the session to drop.
*/
void CImapProtocolController::DropSession(TInt sessionId)
	{
	if (sessionId < iImapSessionArray.Count())
		{
		CImapSession* tempSession = iImapSessionArray[sessionId];
		if (tempSession)
			{
			iImapSessionManager->Disconnect(*tempSession);
			delete tempSession;
			}
		iImapSessionArray.Remove(sessionId);
		}
	}

/**
Private method to select session from the session array for use for operations
that are supported while background operations are in progress.
Issues a request to connect a secondary session if the primary session is busy
and a secondary session is not already available. In this case the requested op
is updated to show a secondary connect request, and the original requested op
is saved.

@param  aIndexToUse updated to indicate the index of the session to be used
@return ETrue if the session is connected and ready to use immediately.
		EFalse if a request to connect a secondary session has been issued.
*/
TBool CImapProtocolController::SelectSessionL(TInt& aIndexToUse)
	{
	__ASSERT_DEBUG(iCurrentOp != ECancelRecoverPrimary, TImapServerPanic::ImapPanic(TImapServerPanic::EProtocolControllerUnexpectedCurrentOp));
	
	aIndexToUse = 0; // default to primary session.
	TBool sessionReady = ETrue;
	if (iBackgroundSyncOp != NULL)
		{
		TInt sessionCount = iImapSessionArray.Count();
		if (sessionCount>=2)
			{
			// already a secondary session available - use this
			aIndexToUse = sessionCount-1;
			}
		else
			{
			// Create a new session and get it connected.
			iCurrentOp = EConnectSecondary;
			
			CImapSession* imapSession = NULL;
			iImapSessionArray.AppendL(imapSession);
			iImapSessionManager->GetSessionL(iStatus, iImapSessionArray[sessionCount]);
			aIndexToUse = sessionCount;
			sessionReady = EFalse;
			}
		}
	return sessionReady;
	}

/**
Called to kick off a foreground operation, ie a user requested
operation that is being performed on the primary IMAP session.

If the session is in IMAP IDLE, a request is issued to stop this
and the requested operation is started when the stop-idle operation
has completed. Otherwise the operation is started immediately.
*/
void CImapProtocolController::StartPrimaryOperation()
	{
	// Make sure the foreground session id is correct
	iForegroundSession = 0;
	// imap idle controller is active in any state after
	// idle has been requested.
	if (iImapIdleController->IsActive())
		{
		iCurrentOp = EStopIdle;
		iImapIdleController->StopIdle(iStatus);
		}
	else
		{
		iCurrentOp = iRequestedOp;
		iImapCompound->StartOperation(iStatus, *iImapSessionArray[iForegroundSession]);
		}
	}

/**
Starts the background IDLE process.
If IDLE is not supported by the server, or if IDLE is disabled via the
account settings, a "dummy read" is issued, to manage any unsolicited
server messages.

This is called when user-requested compound operations have completed
to return the primary session to IDLE state. Note that some operations
require that IDLE is not re-issued, eg "select". It is also called
when a background sync operation has completed, if there is no foreground
operation currently in progress

Idle is only issued on the primary session.

Idle is not started if there is a background operation in progress 
*/
void CImapProtocolController::StartIdle()
	{
	iCurrentOp = EIdle;
	if (iBackgroundSyncOp==NULL)
		{
		iImapIdleController->StartIdle();
		}
	}

/**
Called by the CImapIdleController in the case of an error being observed
while in IDLE state.

@param aError - the observed error
*/
void CImapProtocolController::OnIdleError(TInt aError)
	{
	__LOG_FORMAT((KDefaultLog, "CImapProtocolController::OnIdleError(aError = %d)", aError));
	if (aError>0)
		{
		aError = KErrImapServerFail;
		}
	iProgress.iGenericProgress.iErrorCode = aError;
	iProgress.iGenericProgress.iState=TImap4GenericProgress::EDisconnected;
	DisconnectAll();
	// Nothing to complete.
	}

/**
Returns the progress information for the outstanding command.

@return progress information
*/
EXPORT_C TImap4CompoundProgress CImapProtocolController::Progress()
	{
	// Progress of foreground operation is returned	over that of a
	// background sync operation.
	// Do not refresh the progress while migrating.
	if (iCurrentOp!=EIdle && iMigrateState == ENotMigrating)
		{
		// do not update if IDLEing - return the stored progress object
		// this should have been populated correctly on completion of
		// requested operation, whether successful or otherwise.
		switch (iCurrentOp)
			{
		case EConnect:
			{
			iImapSessionManager->Progress(iProgress.iGenericProgress);
			break;
			}
		case EStopIdle:
		case ESync:
		case ESelect:
		case ECopyToLocal:
		case ECopyWithinService:
		case ECopyFromLocal:
		case EMoveToLocal:
		case EMoveWithinService:
		case EMoveFromLocal:
		case EPopulate:
		case EDelete:
		case EDeleteFolder:
		case ECreate:
		case ERename:
		case EDisconnect:
		case EUpdateFlag: 
			{
			// Obtain progress from compound object if it exists
			// Otherwise the last progress obtained on completion
			// of the compound object is returned.
			if (iImapCompound!=NULL)
				{
				iImapCompound->Progress(iProgress);
				}
			break;
			}
		case EIdle:
		default:
			break;
			}
		}
		else if (iBackgroundSyncOp!=NULL)
		{
		// update sync progress if background sync is being performed.
		iBackgroundSyncOp->Progress(iProgress);
		}

	return iProgress;
	}

/**
changes local subscription flag on a folder immediately.
If unsubscribing, the folder is marked as invisible, and the invisible
flag is propagated to any parent folders that are not themselves subscribed
or contain subscribed folders.

@param aFolder
@param aSubscribed
@return Error code
*/
EXPORT_C TInt CImapProtocolController::SetLocalSubscription(const TMsvId aFolder, 
															TBool aSubscribed)
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::SetLocalSubscription()");

	TInt err;

	// Move to the entry
	if ((err=iEntry.SetEntry(aFolder))!=KErrNone)
		return(err);

	// Check it's a folder
	if (iEntry.Entry().iType!=KUidMsvFolderEntry)
		return(KErrNotSupported);

	// update subscription flag
	TMsvEmailEntry entry=iEntry.Entry();
	entry.SetLocalSubscription(aSubscribed);
	entry.SetVisible(aSubscribed);
	
	err = iEntry.ChangeEntry(entry);
		
	// Return if error or if setting subscribed
	if (err!=KErrNone || !aSubscribed)
		{
		TRAP(err, PropagateUnsubscribeL(aFolder));
		return err;
		}
	else
		{
		return KErrNone;
		}
	}

/**
Marks aFolder as invisible and propagates the invisible flag to parent folder.

@param aFolder the unsubscribed folder.
*/
void CImapProtocolController::PropagateUnsubscribeL(const TMsvId aFolder)
	{
	// Settings may not yet be loaded.
	if (iImapSettings->SettingsLoaded()==EFalse)
		{
		iImapSettings->LoadSettingsL(aFolder);
		}

	// if synchronisation setting is not remote only then
	// update the invisibility flags
	if (iImapSettings->Synchronise() != EUseRemote)
		{
		PropagateInvisibleFlagL(aFolder);
		ChangeVisibilityL(aFolder,ETrue,EFalse,KUidMsvMessageEntry);
		}
	}


// Mark service as on or offline
void CImapProtocolController::MarkOnOrOfflineL( const TBool aOnline )
	{
	// Mark service entry as on/offline	
	User::LeaveIfError( iEntry.SetEntry( iServiceId ) );

	TMsvEntry entry=iEntry.Entry();
	entry.SetConnected( aOnline );
	User::LeaveIfError( iEntry.ChangeEntry( entry ) );

	// Release the service entry
	User::LeaveIfError( iEntry.SetEntry( KMsvNullIndexEntryId ) );

	// Going offline?
	if ( !aOnline && iImapSettings->DisconnectedUserMode() )
		{
		// We're an expert user going offline: don't touch anything
		return;
		}

	// Mark all immediate children of the service as invisible
	if ( !aOnline )
		ChangeVisibilityL( iServiceId, !aOnline );
	}
	
void CImapProtocolController::ChangeVisibilityL(TMsvId aParent, TBool aInvisible)
	{
	ChangeVisibilityL(aParent, aInvisible, ETrue, KUidMsvFolderEntry);
	}


void CImapProtocolController::ChangeVisibilityL(TMsvId aParent, TBool aInvisible, TBool aRecurse, TUid aType)
	{
	// Get children at this level
	CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL( selection );

	CMsvEntrySelection* folders = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL( folders );

	User::LeaveIfError( iEntry.SetEntry( aParent ) );
	User::LeaveIfError( iEntry.GetChildren( *selection ) );

	if ( selection->Count() )
		{
		for( TInt child=0; child < selection->Count(); child++ )
			{
			// Move to this child
			User::LeaveIfError( iEntry.SetEntry( (*selection)[child] ) );
			
			TMsvEntry message=iEntry.Entry();
			
			// Is this the type we want to change?
			if (message.iType==aType)
				{
				// Add to selection to do bulk change on, if necessary
				if ((message.Visible() && aInvisible) ||
					(!message.Visible() && !aInvisible))
					{
					folders->AppendL(message.Id());
					}
				}
			
			// Recurse downwards
			if (aRecurse && message.iType==KUidMsvFolderEntry)
				ChangeVisibilityL(message.Id(),aInvisible,aRecurse,aType);
			}

		// Change its visibility off all children if necessary
		if (folders->Count())
			{
			// Do the change to the invisible flag (actual constant for the
			// flag we want is private:()
			
			User::LeaveIfError( iEntry.SetEntry( aParent ) );
			User::LeaveIfError( iEntry.ChangeAttributes(*folders,
								aInvisible?0:KMsvVisibilityAttribute,
								aInvisible?KMsvVisibilityAttribute:0));
			}
		}

	// Release the service entry
	User::LeaveIfError( iEntry.SetEntry( KMsvNullIndexEntryId ) );

	// Get rid of selection
	CleanupStack::PopAndDestroy(2);
	}

/**
Propagates invisible flag for unsubscribed folders.
aId has been unsubscribed. If it has no visible child folders then
it is made invisible and its parent checked with the same test

@param aId the folder that has been unsubscribed
*/
void CImapProtocolController::PropagateInvisibleFlagL(TMsvId aId)
	{
	__LOG_FORMAT((KDefaultLog, "PropagateInvisibleFlagL: 0x%x", aId));

	// finish if we've reached the top
	if (aId == KMsvRootIndexEntryId)
		{
		return;
		}
	
	User::LeaveIfError(iEntry.SetEntry(aId));

	// finish if we've reached a service
	if (iEntry.Entry().iType == KUidMsvServiceEntry)
		{
		return;
		}

	// return if we've found a subscribed folder since we can't make
	// it invisible
	if (((TMsvEmailEntry)iEntry.Entry()).LocalSubscription())
		{
		return;
		}
	
	// check the children of this unsubscribed folder
	CMsvEntrySelection* selection=new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(selection);

	User::LeaveIfError(iEntry.GetChildren(*selection));

	TBool visible=EFalse;
	TInt count = selection->Count();
	for (TInt i=0; i < count; ++i)
		{
		User::LeaveIfError(iEntry.SetEntry((*selection)[i]));

		// look for a visible folder
		TMsvEmailEntry entry = (TMsvEmailEntry)iEntry.Entry();
		if (entry.iType == KUidMsvFolderEntry && entry.Visible())
			{
			visible=ETrue;
			break;
			}
		}
	
	CleanupStack::PopAndDestroy(selection);

	// if no child folders were visible then make this folder
	// invisible and continue up
	if (!visible)
		{
		User::LeaveIfError(iEntry.SetEntry(aId));
		
		// make this invisible
		TMsvEntry entry = iEntry.Entry();
		entry.SetVisible(EFalse);
		User::LeaveIfError(iEntry.ChangeEntry(entry));

		// go up
		PropagateInvisibleFlagL(entry.Parent());
		}
	}


/**
Is this id in the local service?
@param  aId   the id of the entry to check
@return true, if the message id belongs to the local service.
*/
TBool CImapProtocolController::IdIsLocalL(TMsvId aId)
	{
	return ServiceOfL(aId) == KMsvLocalServiceIndexEntryIdValue;
	}

/**
Returns the id of the service containing this id
@param  aId   the id of the entry to check
@return       the id of the containing service
*/
TMsvId CImapProtocolController::ServiceOfL(TMsvId aId)
	{
	TMsvId current=aId;
	while(current!=KMsvRootIndexEntryIdValue)
		{
		// Visit this entry
		User::LeaveIfError(iEntry.SetEntry(current));

		TMsvEmailEntry entry = iEntry.Entry();
		
		// if service then searched far enough
		if (entry.iType==KUidMsvServiceEntry)
			break;

		// Go upwards
		current=entry.Parent();
		}

	return current;
 	}
 	
/**
Processes positive error codes returned by asynchronous service requests.
Specifically, these will be error codes returned by the CImapSession,
indicating error conditions in communication with the remote server.

See cimapsessionconsts.h for full definition of error codes, summarised here:

KErrImapNo 		 	 IMAP server returned a tagged NO response
KErrImapBad 	 	 IMAP server returned a tagged BAD response
KErrImapClosed		 indicates one of the streams has closed
KErrImapFlushTimeout an attempt to flush a cancelled command has timed out
KErrImapCorrupt		 corrupt data was enountered during parsing of IMAP server data

@param  aError - the positive error code received on completion 
                 of an asynchronous service

@return ETrue  - if this function has called Complete() due to a fatal error.
		EFalse - otherwise
*/
TBool CImapProtocolController::ProcessError(TInt aError)
	{
	__LOG_FORMAT((KDefaultLog, "CImapProtocolController::ProcessError(iCurrentOp = %d, aError = %d)", iCurrentOp, aError));
	
	// Bearer mobility support for initially connect failure 
	if (iMobilityManager && aError == KErrImapConnectError && iImapSessionManager->HasConnection())
		{
		// Change migration state, so the connect error can be dealt with, 
		// using the migration state machine.
		iMigrateState = EHandlingConnectError;

		SetActive();
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);

		return ETrue;
		}
	

	// weed out non-fatal server errors
	ThranslateSessionError(aError);
	if (aError == KErrNone)
		{
		return EFalse;
		}
	
	if (iForegroundSession>0)
		{
		// error has occurred on a secondary session.
		// This is not fatal to the whole protocol controller's
		// existance, but it is fatal to the requested operation
		
		// get final progress state...
		if (iCurrentOp==EConnectSecondary)
			{
			iImapSessionManager->Progress(iProgress.iGenericProgress);
			}
		else
			{
			iImapCompound->Progress(iProgress);
			}
		
		//... and update reported error code
		iProgress.iGenericProgress.iErrorCode=aError;
		
		// delete the compound operation object
		delete iImapCompound;
		iImapCompound = NULL;
		
		// drop the secondary session
		DropSession(iForegroundSession);
		
		// complete the user request with KErrNone..
		Complete(KErrNone);
		}
	else
		{
		// DoComplete normally handles the session tidyup, but
		// is not called if the error has occurred during a background
		// operation, for example ECancelRecoverPrimary
		if (iCurrentOp==ECancelRecoverPrimary)
			{
			// Non-fatal errors should have been handled prior to this.
			DisconnectAll();
	
			// Save error code in progress and flag the disconnect
			// this is defensive as the client should not be watching progress
			iProgress.iGenericProgress.iState=TImap4GenericProgress::EDisconnected;
			iProgress.iGenericProgress.iErrorCode=aError;
			}
		Complete(aError);
		}
	return ETrue;
	}

/**
Translates session error codes for reporting to the Server MTM.
KErrImapNo and KErrImapBad are non-fatal to the session, and 
should have been fielded by the compound operation, sync manager
or session manager (during session connect), however they are
translated to KErrNone here.

All other session errors are fatal to the connection. If an
error occurs on the primary session, all connected sessions 
should be disconnected and the client completed.
*/
void CImapProtocolController::ThranslateSessionError(TInt& errCode)
	{
	if ( errCode == KErrNone 
	  || errCode == KErrImapNo 
	  || errCode == KErrImapBad )
		{
		// No and bad should have been handled by the compound 
		// operation and are non-fatal to the connection.
		errCode = KErrNone;
		}
	else
		{
		errCode = KErrImapServerFail;
		}
	}

/**
Resets the progress object
*/
void CImapProtocolController::ResetProgress()
	{
	//initialise the TImap4SyncProgress part
	iProgress.iSyncProgress.iFoldersToDo = 0;
	iProgress.iSyncProgress.iFoldersDone = 0;
	iProgress.iSyncProgress.iMsgsToDo = 0;
	iProgress.iSyncProgress.iMsgsDone = 0;
	iProgress.iSyncProgress.iHeadersFetched = 0;
	iProgress.iSyncProgress.iOrphanedFolders = 0;
	iProgress.iSyncProgress.iNewFolders = 0;
	iProgress.iSyncProgress.iOrphanedMessages = 0;
	iProgress.iSyncProgress.iRemoteMessagesDeleteTagged = 0;
	iProgress.iSyncProgress.iMessagesFetchedOK = 0;
	iProgress.iSyncProgress.iMessagePartsFetchedOK = 0;
	iProgress.iSyncProgress.iMessagePartsNotFound = 0;
	iProgress.iSyncProgress.iFoldersNotFound = 0;
	iProgress.iSyncProgress.iErrorCode = KErrNone;	
	iProgress.iSyncProgress.iType = EImap4SyncProgressType;
	
	//initialise the TImap4GenericProgress part
	iProgress.iGenericProgress.iMsgsToDo = 0;
	iProgress.iGenericProgress.iMsgsDone = 0;
	iProgress.iGenericProgress.iPartsToDo = 0;
	iProgress.iGenericProgress.iPartsDone = 0;
	iProgress.iGenericProgress.iBytesToDo = 0;
	iProgress.iGenericProgress.iBytesDone = 0;
	iProgress.iGenericProgress.iReturnedMsvId = 0;
	iProgress.iGenericProgress.iTotalSize = 0;
	iProgress.iGenericProgress.iErrorCode = KErrNone;
	iProgress.iGenericProgress.iType = EImap4GenericProgressType;

	}

/**
Notice that a preferred carrier has become available, and migration to that bearer has been accepted.
The protocol controller shall either pause or allow any current operation to complete
according to the action specified in parameter aAction. Once the current operation 
is paused or complete, the protocol controller shall close any existing sockets
and finally notify the mobility framework that it is ready to migrate to the new
carrier.

@param aAction      - indicates the action that should be taken re: current operations
@param aIsSeamless  - indicates if the 
*/
void CImapProtocolController::PrepareForNewCarrier(TImMobilityAction aAction, TBool /*aIsSeamless*/)
	{
	__LOG_FORMAT((KDefaultLog, "CImapProtocolController::PrepareForNewCarrier(iMigrateState=%d, aAction = %d)", iMigrateState, aAction));
	// Handle being called in an already-migrating state before handling usual case
	if (iMigrateState!=ENotMigrating)
		{
		switch (iMigrateState)
			{
			case EDisconnectingForMigrate:
			case EWaitingForOpToStop:
				{
				// Do not allow change from EWaitingForOpToStop to EWaitingForOpToComplete
				// but do allow an operation to be stopped suddenly if necessary.
				if (aAction==KAcceptImmediately)
					{
					CarrierLost();
					iMigrateState = EWaitingForNewCarrier;
					iMobilityManager->MigrateToNewCarrier();
		
					// do not set waiting if NewCarrierActive has been called synchronously.
					if (iMigrateState == EWaitingForNewCarrier)
						{
						// Now in a waiting state, set self active
						iStatus = KRequestPending;
						SetActive();
						}
					}
				// else already preparing for new carrier, nothing to do.
				return;
				}

			case EWaitingForOpToComplete:	// async wait on compound op/bground op/idle controller
				{
				// Allow change from EWaitingForOpToComplete to EWaitingForOpToStop
				// also allow an operation to be stopped suddenly if necessary.
				if (aAction==KAcceptImmediately)
					{
					CarrierLost();
					iMigrateState = EWaitingForNewCarrier;
					iMobilityManager->MigrateToNewCarrier();
		
					// do not set waiting if NewCarrierActive has been called synchronously.
					if (iMigrateState == EWaitingForNewCarrier)
						{
						// Now in a waiting state, set self active
						iStatus = KRequestPending;
						SetActive();
						}
					}
				else if (aAction==KAcceptStopCurrent)
					{
					// change from EWaitingForOpToComplete to EWaitingForOpToStop
					StopCurrentForMigrate();
					}
				// else already preparing for new carrier, nothing to do.
				return;
				}
			case EHandlingConnectError:
				{
				// We have a Connect Error, from previous connect attempt
				// We are ready to (migrate) reconnect
				iMigrateState = EWaitingForNewCarrier;
				iMobilityManager->MigrateToNewCarrier();
				
				// do not set waiting if NewCarrierActive has been called synchronously.
				if (iMigrateState == EWaitingForNewCarrier)
					{
					// Now in a waiting state, set self active
					iStatus = KRequestPending;
					SetActive();
					}
				break;
				}
			case EConnectingAfterMigrate:	// async wait on session manager
				{
				// cancelling in this state tidies up the session array, so
				// nothing extra to do here - now ready for migration.
				Cancel();
				iMigrateState = EWaitingForNewCarrier;
				iMobilityManager->MigrateToNewCarrier();
	
				// do not set waiting if NewCarrierActive has been called synchronously.
				if (iMigrateState == EWaitingForNewCarrier)
					{
					// Now in a waiting state, set self active
					iStatus = KRequestPending;
					SetActive();
					}
				return;				
				}

			case EWaitingInitialCarrierRejected:	// async wait state on mobility engine
			case EWaitingForNewCarrier:		// async wait state on mobility engine
			case EStartingReconnect:		// intermediate state
				{
				// already ready and waiting for a new carrier - Cancel dummy request
				// and tell the Mobility framework we are ready
				Cancel();
				iMigrateState = EWaitingForNewCarrier;
				iMobilityManager->MigrateToNewCarrier();
	
				// do not set waiting if NewCarrierActive has been called synchronously.
				if (iMigrateState == EWaitingForNewCarrier)
					{
					// Now in a waiting state, set self active
					iStatus = KRequestPending;
					SetActive();
					}
				return;
				}
			case ESuspendingForMigrate:     // intermediate state
			case ENotMigrating:				// already checked this isn't true
			default:
				{
				__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::EProConPreForNewCarBadMigrateState));
				}
			}
		}

	// Do the requested action...
	switch (aAction)
		{
		case KAcceptCompleteCurrent:
			{
			// Any current operation shall be allowed to complete prior
			// to indicating that the server MTM is ready to migrate to
			// the new bearer.
			CompleteCurrentForMigrate();
			break;
			}
		case KAcceptStopCurrent:
			{
			// Any current operation shall be allowed to continue until
			// such a point that it may be resumed without re-sending or
			// re-receiveing a significant amount to data prior to 
			// indicating that the server MTM is ready to migrate to the 
			// new bearer.
			StopCurrentForMigrate();
			break;
			}
		case KAcceptImmediately:
		default:
			{
			// accept immediately is an instruction to immediately suspend any
			// current operation (cancel any outstanding server communication)
			// and close any open sockets. This is the same behaviour as required
			// for a downgrade situation.
			CarrierLost();
			iMigrateState = EWaitingForNewCarrier;
			iMobilityManager->MigrateToNewCarrier();

			// do not set waiting if NewCarrierActive has been called synchronously.
			if (iMigrateState == EWaitingForNewCarrier)
				{
				// Now in a waiting state, set self active
				iStatus = KRequestPending;
				SetActive();
				}
			break;
			}
		} // end switch (aAction)
	}

/**
Configures the protocol controller (PC) to allow any current operation in progress
to complete prior to indicating to the mobility framework that it is ready to
migrate to the new carrier.

Note the protocol controller may be in any of the following states
 - IDLE (only one session exists)
 - background sync in progress only (only one session exists)
 - foreground op in progress only (only one session exists)
 - background sync and foreground operation in progress (2 sessions exist)
 - background sync in progress and connecting second session for foreground op

*/ 	
void CImapProtocolController::CompleteCurrentForMigrate()	
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::CompleteCurrentForMigrate()");
	// set the migrating flag
	iMigrateState = EWaitingForOpToComplete;
	
	if (BackgroundSyncInProgress())
		{
		// 2-phase background sync is always delayed till after migration
		iBackgroundSyncOp->StopForMigrate();
		}

	// If no operation in progress, stop the IDLE
	if (iCurrentOp == EIdle && iImapIdleController->IsActive())
		{
		iCurrentOp = EStopIdle;
		iImapIdleController->StopIdle(iStatus);
		SetActive();
		return;
		}

	// cancel a secondary session connect
	if (iCurrentOp == EConnectSecondary)
		{
		CancelForMigrate();
		iMigrateState = EWaitingForOpToComplete;
		
		// defensive: it is possible that the background sync has completed
		// while the secondary connect is still in progress. In this case
		// neither DoRunL() nor BackgroundSyncComplete() will not be called.
		if (!BackgroundSyncInProgress())
			{
			// fake a stop-idle op
			iCurrentOp = EStopIdle;
			SetActive();
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrNone);
			}
		}
	}

/**
CancelForMigrate() - cancels any outstanding service without deleting the
compound operation or completing the user request.

This does not cancel a background sync.

iMigrateState must be set to a new state immediately after calling this.
*/
void CImapProtocolController::CancelForMigrate()
	{
	iMigrateState = ESuspendingForMigrate;
	Cancel();
	}

/**
*/
void CImapProtocolController::DoCancelForMigrate()
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::DoCancelForMigrate()");
	switch (iCurrentOp)
		{
		case EConnectSecondary:
			{
			// Cancel the connect request
			iImapSessionManager->Cancel();
			// Remove the session pointer from the array
			// The session has already been deleted - see note above.
			TInt numSessions = iImapSessionArray.Count();
			iImapSessionArray.Remove(numSessions-1);
			break;
			}

		case EIdle:
		case EStopIdle:
			{
			// Cancel the IDLE controller.
			iImapIdleController->Cancel();
			break;
			}

		case ESync:
		case ESelect:
		case ECopyToLocal:
		case ECopyWithinService:
		case ECopyFromLocal:
		case EMoveToLocal:
		case EMoveWithinService:
		case EMoveFromLocal:
		case EPopulate:
		case EDelete:
		case EDeleteFolder:
		case ECreate:
		case ERename:
			{
			// Cancel the compound operation
			iImapCompound->CancelForMigrate();
			break;
			}

		case ECancelRecoverPrimary:
			{
			// Cancel the session recovery.
			(iImapSessionArray[0])->Cancel();
			}

		case EConnect:
//		case EConnectAndSync:
		case EDisconnect:
		default:
			{
			// nothing to do..
			break;
			}
			
		} // end of switch (iCurrentOp)

	// Do NOT call CMsgActive::DoCancel()
	// - the client requested operation is not being cancelled
	}


/**
Configures the protocol controller (PC) to stop any current operation in 
progress at the next convenient point - the meaning of this depends on the 
specific compound operations. For example, for simple operations such as 
creating a folder, this will be when the operation has completed but for
operations for which multiple messages are processed in sequence (eg 
fetching several messages), it shall be once the current message has been
processed (eg fetched). At this point the operation shall complete the
protocol controller's iStatus

Note the protocol controller may be in any of the following states
 - IDLE (only one session exists)
 - background sync in progress only (only one session exists)
 - foreground op in progress only (only one session exists)
 - background sync and foreground operation in progress (2 sessions exist)
 - background sync in progress and connecting second session for foreground op

*/ 
void CImapProtocolController::StopCurrentForMigrate()	
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::StopCurrentForMigrate()");
	// set the migrating flag
	iMigrateState = EWaitingForOpToStop;
	
	if (BackgroundSyncInProgress())
		{
		iBackgroundSyncOp->StopForMigrate();
		}
	
	if (iCurrentOp == EIdle && iImapIdleController->IsActive())
		{
		// If no operation in progress, stop the IDLE
		iCurrentOp = EStopIdle;
		iImapIdleController->StopIdle(iStatus);
		SetActive();
		return;
		}
	else if (iCurrentOp == EConnectSecondary)
		{
		// cancel a secondary session connect
		Cancel();
		
		// defensive: it is possible that the background sync has completed
		// while the secondary connect is still in progress. In this case
		// neither DoRunL() nor BackgroundSyncComplete() will not be called.
		if (!BackgroundSyncInProgress())
			{
			// fake a stop-idle op
			iCurrentOp = EStopIdle;
			SetActive();
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrNone);
			}
		}
	else if (iCurrentOp != ECancelRecoverPrimary)
		{
		// nothing to do for ECancelRecoverPrimary - just wait for the
		// session to flush.
		// all other operations are told to stop:
		iImapCompound->StopForMigrate();		
		}
	}		

/**
This API is called by the Bearer Mobility Manager. It is typically called in the 
case that a downgrade is occuring, and the original sockets are no longer valid
for use. It may also be called if an immediate migration to an preferred bearer is
required, without graceful closing of the original sockets.

Any operations are cancelled immediately using the CancelForMigrate() API. This
cancels the operations in such a way that they may be restarted following migration
to a new carrier.
*/
void CImapProtocolController::CarrierLost()
	{
	__LOG_FORMAT((KDefaultLog, "CImapProtocolController::CarrierLost(iMigrateState=%d)", iMigrateState));

	// Handle being called in an already-migrating state before handling usual case
	if (iMigrateState!=ENotMigrating)
		{
		switch (iMigrateState)
			{
			case EWaitingForOpToStop:
			case EWaitingForOpToComplete:
				{
				// break to default behaviour
				break;
				}
			
			case EStartingReconnect:		// intermediate state
			case EDisconnectingForMigrate:
			case EConnectingAfterMigrate:
				{
				// cancel the disconnect/connect, then break to default behaviour
				// (calling CancelForMigrate() shouldn't do anything as any compound
				// operations will not themselves be active)
				Cancel();
				break;
				}
				
			case EHandlingConnectError:
				{
				// empty the session array
				TInt numSessions = iImapSessionArray.Count();
				for (TInt i=0; i<numSessions; ++i)
					{
					if (iImapSessionArray[i])
						{
						iImapSessionManager->Disconnect(*(iImapSessionArray[i]));
						}
					}
				iImapSessionArray.ResetAndDestroy();

				// Set the migration state:
				iMigrateState = EWaitingForNewCarrier;
				
				// Now in an waiting state, set self active
				iStatus = KRequestPending;
				SetActive();
				return;
				}
					
			case EWaitingInitialCarrierRejected:
			case EWaitingForNewCarrier:
				{
				// already ready and waiting for a new carrier - nothing to do.
				return;
				}
			
			case ESuspendingForMigrate:     // intermediate state
			case ENotMigrating:				// normal state
			default:
				{
				__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::EProConKillCurrentBadMigrateState));
				break;
				}
			}
		}


	// stop any background operation
	if (iBackgroundSyncOp)
		{
		iBackgroundSyncOp->CancelForMigrate();
		}

	// Cancel foreground operations
	CancelForMigrate();
		
	// delete the imap idle controller - this will be re-created on reconnect.
	delete iImapIdleController;
	iImapIdleController=NULL;
		
	// Drop connection on each of the imap sessions
	TInt numSessions = iImapSessionArray.Count();
	for (TInt i=0; i<numSessions; ++i)
		{
		if (iImapSessionArray[i])
			{
			iImapSessionManager->Disconnect(*(iImapSessionArray[i]));
			}
		}

	// empty the session array
	iImapSessionArray.ResetAndDestroy();
	
	// Set the migration state:
	iMigrateState = EWaitingForNewCarrier;
	
	// Now in an waiting state, set self active
	iStatus = KRequestPending;
	SetActive();
	}
	
/**
Called to indicate the migration has completed and the RConnection is ready to
provide new sockets via the new bearer.

Initiates the creation of a new IMAP session, following which any suspended
operation shall be restarted.
*/
void CImapProtocolController::NewCarrierActive(TAccessPointInfo /*aNewAp*/, TBool /*aIsSeamless*/)
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::NewCarrierActive()");
	__ASSERT_DEBUG((iMigrateState == EWaitingForNewCarrier || iMigrateState == EWaitingInitialCarrierRejected),
	               TImapServerPanic::ImapPanic(TImapServerPanic::ENewCarrierActiveUnexpectedMigrateState));

	// Cancel the dummy active state
	Cancel();

	// set the new migration state
	iMigrateState = EStartingReconnect;

	// complete self - this requires some memory allocation, do it within the RunL.
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();	
	}

	
void CImapProtocolController::NewPrimarySessionL()
	{
	// Create the primary session pointer
	CImapSession* imapSession = NULL;
	iImapSessionArray.AppendL(imapSession);

	// Request the session manager to connect the session
	iImapSessionManager->GetSessionL(iStatus, iImapSessionArray[0]);
	iMigrateState = EConnectingAfterMigrate;

	SetActive();
	}
	
/**
Indicates that the RConnection is no longer valid.
Therefore disconnect and mark the service as offline.
*/
#ifdef __IMAP_LOGGING
void CImapProtocolController::MobilityError(TUint aError)
#else //__IMAP_LOGGING
void CImapProtocolController::MobilityError(TUint /*aError*/)
#endif //__IMAP_LOGGING
	{
	__LOG_FORMAT((KDefaultLog, "CImapProtocolController::MobilityError(iMigrateState=%d, aError=%d)", iMigrateState, aError));
	
	// Cancel any async request
	Cancel();
	
	if (iRequestedOp != EIdle)
		{
		// Complete the user request - calls DisconnectAll()
		Complete(KErrDisconnected);
		}
	else
		{
		// Disconnect all sessions and mark the service as offline
		DisconnectAll();
		}
	
	// the RConnection is no longer valid - delete the session manager
	// (will be recreated on ConnectL())
	delete iImapSessionManager;
	iImapSessionManager = NULL;
	
	// no longer migrating...
	iMigrateState = ENotMigrating;

	// Save error code in progress and flag the disconnect
	// this is defensive as the client should not be watching progress
	iProgress.iGenericProgress.iState=TImap4GenericProgress::EDisconnected;
	iProgress.iGenericProgress.iErrorCode=KErrDisconnected;
	}

void CImapProtocolController::ProcessOpCompleteForMigrate()
	{
	// Any tidying up to do for the current user-requested operation
	switch (iCurrentOp)
		{
		case ECancelRecoverPrimary:
		case EIdle:
		case EStopIdle:
			{
			// engine was IDLE or recovering from a cancel when the 
			// migrate notice arrived... nothing to do before disconnecting.
			break;
			}

		case ESync:
		case ESelect:
		case ECopyToLocal:
		case ECopyWithinService:
		case ECopyFromLocal:
		case EMoveToLocal:
		case EMoveWithinService:
		case EMoveFromLocal:
		case EPopulate:
		case EDelete:
		case EDeleteFolder:
		case ECreate:
		case ERename:
			{
			// update last operation progress state
			iImapCompound->Progress(iProgress);
			
			// if the user requested operation has finished, 
			// tidyup and inform the client 
			if (iProgress.iGenericProgress.iState == TImap4GenericProgress::EIdle)
				{
				delete iImapCompound;
				iImapCompound = NULL;
		
				// Complete the server mtm request.
				Complete(iStatus.Int());
				}
			break;
			}

		case EConnect:
		case EDisconnect:
		case EConnectSecondary:
		default:
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::EProcessOpCompleteForMigrateUnexpectedState));
			}
		} // end of switch (iCurrentOp)
	}

/**
Logs out and disconnects all connected sessions with the remote server.
*/
void CImapProtocolController::DisconnectForMigrateL()
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::DisconnectForMigrateL()");
	__ASSERT_DEBUG(iMigrateCompound==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EMigrateCompoundIsNotNull));
	
	// Background operations will have been taken care of already - just disconnect.
	// Do not perform the late-delete offline operations.
	delete iMigrateCompound;
	iMigrateCompound = NULL;
	
	iMigrateCompound = CImapCompoundDisconnect::NewL( *iImapSyncManager, 
												      iEntry, 
												      *iImapSettings,
												      *iImapSessionManager,
												      *iImapMailStore,
												      iImapSessionArray,
												      iImapOfflineControl,
												      EFalse );

	iMigrateState = EDisconnectingForMigrate;
	iMigrateCompound->StartOperation(iStatus, *iImapSessionArray[0]);
	SetActive();	
	}


/**
Resumes operations that were stopped to allow migration to occur,
or operations that were requested while migration was in progress.
*/
void CImapProtocolController::RestartAfterMigrateL()
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::RestartAfterMigrateL()");
	
	// The migration has completed. Restart anything that was going on
	// before the migration started, or has been requested in the meantime.
	iMigrateState = ENotMigrating;
	
	// was the initial carrier rejected during the connect operation?
	// if so, mark as online and get the final connection progress info.
	if (iCurrentOp == EConnect)
		{
		TRAP_IGNORE(MarkOnOrOfflineL(ETrue));

		// Collect the final connect progress information		
		iImapSessionManager->Progress(iProgress.iGenericProgress);

		// Set last socket activity timeout to iMtmData1. This is used by Imcm.
		User::LeaveIfError( iEntry.SetEntry( iServiceId ) );
		TMsvEntry entry=iEntry.Entry();
		entry.SetMtmData1(iImapSessionManager->LastSocketActivityTimeout());
		User::LeaveIfError( iEntry.ChangeEntry( entry ) );
				
		// complete the connect request
		Complete(iStatus.Int());		
		}
	
	// Create a new IDLE controller - the old one was deleted.
	delete iImapIdleController;
	iImapIdleController=NULL;
	iImapIdleController = CImapIdleController::NewL(*this, iImapSessionArray[0], *iImapSyncManager, iEntry, *iImapSettings, *iImapMailStore);

	//	was there a background sync in progress prior to migration?
	if (iBackgroundSyncOp)
		{
		// restart any background operation, using the new primary session.
		iBackgroundSyncOp->ResumeOperationL(*iImapSessionArray[0]);
		
		// was there a forground operation paused as well?
		if (iImapCompound)
			{
			// Create a second connected session.
			iCurrentOp = EConnectSecondary;
			// iRequestedOp should not have been changed...
			CImapSession* imapSession = NULL;
			iImapSessionArray.AppendL(imapSession);
			TInt sessionCount = iImapSessionArray.Count();
			iImapSessionManager->GetSessionL(iStatus, iImapSessionArray[sessionCount-1]);
			SetActive();
			}
		else
			{
			iCurrentOp = EIdle;
			}
		return;
		}

	if (iImapCompound)
		{
		// Idle has not been restarted after the migration, so the requested 
		// operation can be re-started without calling StopIdle()
		// resume the suspended operation
		iCurrentOp = iRequestedOp;
		iImapCompound->ResumeOperationL(iStatus, *iImapSessionArray[0]);
		SetActive();		
		}
	else
		{
		// otherwise just start Idle
		iCurrentOp = EIdle;
		iRequestedOp = EIdle;
		StartIdle();
		}
	}

/**
Returns a packaged copy of the current progress for passing to the
mobility policy plugin when a preferred carrier available notice
is received.
*/
const TDesC8& CImapProtocolController::MobilityProgress()
	{
	Progress();
	iProgressBuffer = TImap4ProgressBuf(iProgress);
	return iProgressBuffer;
	}
/**
Updates  the specified messages read/unread status from the remote server.
@param aStatus
*/	
EXPORT_C void CImapProtocolController::UpdateFlagL( TRequestStatus& aStatus)
 
	{
	__LOG_TEXT(KDefaultLog, "CImapProtocolController::UpdateFlagL()");
	__ASSERT_DEBUG(iImapCompound==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECreateCompoundIsNotNull));
	ResetProgress();
	
    if (!CompleteIfBackgroundOpInProgress(aStatus)) 
            {
        
            iImapCompound = CImapUpdateFlagOperation::NewL(*iImapSyncManager,
                                                       iEntry,
                                                       *iImapSettings
                                                       );
            
            iRequestedOp = EUpdateFlag;
            StartPrimaryOperation();
            Queue(aStatus);
            SetActive();
            }

	    }	



/**
Updates  the selection of  messages read/unread status from the remote server.
@param aStatus
*/  
EXPORT_C void CImapProtocolController::UpdateEntriesFlagL( TRequestStatus& aStatus,const CMsvEntrySelection& aSourceSel ,TBool aFlagChanged)
 
    {
    __LOG_TEXT(KDefaultLog, "CImapProtocolController::UpdateEntriesFlagL()");
    __ASSERT_DEBUG(iImapCompound==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECreateCompoundIsNotNull));
    ResetProgress();
    
   if (!CompleteIfBackgroundOpInProgress(aStatus)) 
        {
    
        iImapCompound = CImapUpdateFlagOperation::NewL(*iImapSyncManager,
                                                   iEntry,
                                                   *iImapSettings,
                                                   aSourceSel,
                                                   aFlagChanged);
        iRequestedOp = EUpdateFlag;
        StartPrimaryOperation();
        Queue(aStatus);
        SetActive();
        }
        
    }   

