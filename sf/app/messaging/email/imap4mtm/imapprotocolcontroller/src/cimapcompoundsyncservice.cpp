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

#include "cimapcompoundsyncservice.h"

#include <imapset.h>

#include "cimapfolder.h"
#include "cimapsession.h"
#include "cimapmailstore.h" 
#include "cimapofflinecontrol.h"
#include "cimapsyncmanager.h"
#include "cimapfolder.h"
#include "cimaplogger.h"

#include "cimapcompoundcopyfromlocal.h"
#include "cimapcompoundcopytolocal.h"
#include "cimapcompoundcopywithinservice.h"
#include "cimapcompoundcreate.h"
#include "cimapcompounddelete.h"
#include "cimapcompounddeletefolder.h"
#include "cimapcompounddisconnect.h"
#include "cimapcompoundrename.h"
#include "cimapcompoundselect.h"
#include "cimapsessionconsts.h"
#include "imappaniccodes.h"
#include "cimapofflinecontrol.h"
#include "imappaniccodes.h"
#include "cimapsyncdownloadrules.h"

#include "mobilitytestmtmapi.h"

CImapCompoundSyncService::CImapCompoundSyncService(CImapSyncManager& aSyncManager, CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings, CImapMailStore& aImapMailStore, CImapOfflineControl& aImapOfflineControl ,TBool aDoLateDeletes)
 : CImapCompoundBase(aSyncManager, aServerEntry, aImapSettings), iImapOfflineControl(aImapOfflineControl), iImapMailStore(aImapMailStore), iDoLateDeletes(aDoLateDeletes)
	{
	// Set default progress values here so that any progress request before
	// the operation is started returns valid information.
	if(aDoLateDeletes)
		{
		iProgressState = TImap4GenericProgress::EBusy;
		iSyncProgressState=TImap4SyncProgress::EDeleting;
		}
	else
		{
		iProgressState = TImap4GenericProgress::ESyncing;
		iSyncProgressState=TImap4SyncProgress::ESyncInbox;
		}
	}

CImapCompoundSyncService::~CImapCompoundSyncService()
	{
	delete iOutstandingOps;
	delete iOutstandingMoveTypeDeletes;
	delete iOutstandingLocalDeletes;
	delete iImapCompound;
	delete iDoNotFetchSel;
	}
	
CImapCompoundSyncService* CImapCompoundSyncService::NewL(CImapSyncManager& aSyncManager, CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings, CImapMailStore& aImapMailStore, CImapOfflineControl& aImapOfflineControl ,TBool aDoLateDeletes)
	{
	CImapCompoundSyncService* self = new (ELeave) CImapCompoundSyncService(aSyncManager, aServerEntry, aImapSettings, aImapMailStore, aImapOfflineControl,aDoLateDeletes);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CImapCompoundSyncService::ConstructL()
	{
	iOutstandingMoveTypeDeletes = new (ELeave) CArrayFixFlat<TMsvId>(4);
	iOutstandingLocalDeletes = new (ELeave) CArrayFixFlat<TMsvId>(4);
	iPerformDeletes = !(iImapSettings.DeleteEmailsWhenDisconnecting());
	
	CActiveScheduler::Add(this);
	}

void CImapCompoundSyncService::StartOperation(TRequestStatus& aStatus, CImapSession& aSession)
	{
	iSession = &aSession;
	__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncService::StartOperation()");
	if(iDoLateDeletes)
		{
		iNextStep = EInboxLateDeletes;
		}
	else
		{
		iNextStep = ESyncService;
		}
	Queue(aStatus);
	CompleteSelf();
	}
	
/**
Handles the compound operation state machine

@return ETrue if compound operation is completed, 
		EFalse otherwise (will be called again, unless active)
*/
TBool CImapCompoundSyncService::DoRunLoopL()
	{
	SetCurrentStep();
	switch (iCurrentStep)
		{
		case ESyncService:  // synchronous
			{
			// Starts the full sync process by synching the inbox
			iNextFolderSync = 0;
					
			__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncService::DoRunLoopL : Starting the full Sync steps.");
			CImapFolder* inbox = iSyncManager.Inbox();
			
			if(inbox != NULL)
				{
				iNextStep = EInboxSync;
				__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncService::DoRunLoopL : Processing offline for the inbox.");
					// Deal with operations in the array
				if (RefreshOutstandingOpsL(inbox->MailboxId()))
					{
					iNextStep = EInboxCheckPendingOps;
					}
				}
			else
				{
				iNextStep = ESyncTree;
				}
			
			CompleteSelf();
			}
			break;
		case EInboxDoneOps: // synchronous
			{
			__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncService::DoRunLoopL : Performing the current offline op for the inbox.");

			iNextStep = EInboxCheckPendingOps;
			DonePendingOpL();
			CompleteSelf();
			}
			break;
		case EInboxCheckPendingOps: // synchronous
			{
			iSyncProgressState=TImap4SyncProgress::EProcessingPendingOps;
			__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncService::DoRunLoopL : Find the next offline op for the inbox.");
			if (NextPendingOpL())
				{
				iNextStep = EInboxSync;
				DonePendingOpL();
				CompleteSelf();
				}
			else
				{
				iNextStep = EInboxDoneOps;

				// As there is a next pending op, we should be active so that the
				// operation can go ahead. However under some circumstances (such
				// as a missing shadow ID), the operation will not happen so we need
				// to ensure the state machine does not lock up.
				if (!IsActive())
					{
					CompleteSelf();
					}
				}
			}
			break;
		case EInboxEarlyDeletes: // async, SELECTing the inbox prior to EXPUNGE. 
			{
			__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncService::DoRunLoopL : Performing early deletes for the inbox.");

			iSyncProgressState=TImap4SyncProgress::EDeleting;
			// get rid of the FromLocal message sources
			CImapFolder* inbox = iSyncManager.Inbox();
			
			if(inbox)
				{
				for (TInt i = 0; i < iOutstandingLocalDeletes->Count(); ++i)
					{
					TMsvId id = (*iOutstandingLocalDeletes)[i];
					__LOG_FORMAT((iSession->LogId(), "CImapCompoundSyncService::DoRunLoopL : Removing message %d.", id));
					inbox->DeleteMessageL(id);
					}

				iOutstandingLocalDeletes->Reset();

				// then do the inbox deletes
				if (ProcessPendingDeleteOpsL(inbox->MailboxId(), !iPerformDeletes))
					{
					MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmImapSyncEInboxEarlyDeletes);
					iNextStep = EInboxEarlyExpunge;
					inbox->SelectL(iStatus, *iSession);
					SetActive();
					return EFalse;
					}
				}
			else
				{
				iOutstandingLocalDeletes->Reset();
				}

			iNextStep = ESyncTree;
			CompleteSelf();
			}
	  		break;
		case EInboxEarlyExpunge: // async - sync deletes performed by the folder object, STORE /deleted and EXPUNGE
			{
			iSyncProgressState=TImap4SyncProgress::EDeleting;

			iNextStep = ESyncTree;

			if (iStatus.Int() == KErrImapNo)
				{
				iFoldersNotFound++;
				}
			else
				{
				// expunge the folder
				iFolder = iSyncManager.Inbox();
				
				if(iFolder)
					{
					MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmImapSyncEInboxEarlyExpunge);
					__LOG_FORMAT((iSession->LogId(), "CImapCompoundSyncService::DoRunLoopL : Expunge the folder %S.", &(iFolder->FullFolderPathL())));
					iFolder->SyncDeletesL(iStatus, *iSession);
					SetActive();
					return EFalse;
					}
				}

			CompleteSelf();
			}
			break;
		case EInboxSync: // async
			{
			__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncService::DoRunLoopL : Calling the Syncmanger step for synching the inbox.");

			// suspend operation if stop for migrate has been requested
			if (iStopForMigrate)
				{
				__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncService::Stopped for migrate");
				iCurrentStep = ESuspendedForMigrate;
				iNextStep = EInboxSync;
				Complete(KErrNone);
				return ETrue;
				}

			// Call the SyncManager inbox sync
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmImapSyncEInboxSync);			
			iSyncProgressState=TImap4SyncProgress::ESyncInbox;
			iSyncManager.SynchroniseInboxL(iStatus, *iSession);
			iNextStep = EInboxEarlyDeletes;
			}
			break;
		case ESyncTree:  // async
			{
			// suspend operation if stop for migrate has been requested
			if (iStopForMigrate)
				{
				__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncService::Stopped for migrate");
				iCurrentStep = ESuspendedForMigrate;
				iNextStep = ESyncTree;
				Complete(KErrNone);
				return ETrue;
				}

			iNextStep = ESyncSubscriptions;

			// skip folder tree sync if disabled.
			if (iImapSettings.FolderSyncDisabled())
				{
				__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncService::DoRunLoopL : Folder Tree Sync Disabled....");
				return EFalse;
				}

			__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncService::DoRunLoopL : Calling the Syncmanger step to sync the folder tree.");

			// Synching the remote folders tree to local
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmImapSyncESyncTree);			
			iSyncProgressState=TImap4SyncProgress::ESyncFolderTree;
			iSyncManager.SynchroniseFolderTreeL(iStatus, *iSession);
			}
			break;
		case ESyncSubscriptions:  // async
			{
			__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncService::DoRunLoopL : Calling the Syncmanger step to sync the folders subscriptions.");
			// Synching the remote folders tree to local
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmImapSyncESyncSubscriptions);
			iSyncProgressState=TImap4SyncProgress::ECheckRemoteSubscription;
			iSyncManager.SynchroniseFolderSubscriptionsL(iStatus, *iSession);
			iNextStep = ESelectSyncFolder;
			}
			break;	
		case EFolderDoneOps:  // synchronous
			{
			if(iNextFolderSync > 0)
				{
				__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncService::DoRunLoopL : Calling DonePendingOp.");
				iNextStep = EFolderCheckPendingOPs;
				DonePendingOpL();
				}
			else
				{
				// All folders are done.
				iNextStep = ESyncInboxAutoFetch;
				}
			
			CompleteSelf();
			}
			break;
		case EFolderCheckPendingOPs: // synchronous
			{
			iSyncProgressState=TImap4SyncProgress::EProcessingPendingOps;

			if(NextPendingOpL())
				{
				__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncService::DoRunLoopL : All offline ops are done, calling folder sync.");
				iNextStep = ESyncFolder;
				DonePendingOpL();
				CompleteSelf();
				}
			else
				{
				__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncService::DoRunLoopL : Got a pending offline op, now do it.");
				iNextStep = EFolderDoneOps;

				// As there is a next pending op, we should be active so that the
				// operation can go ahead. However under some circumstances (such
				// as a missing shadow ID), the operation will not happen so we need
				// to ensure the state machine does not lock up.
				if (!IsActive())
					{
					CompleteSelf();
					}
				}
			}
			break;
		case EFolderEarlyDeletes: // asynchronous
			{
			iSyncProgressState=TImap4SyncProgress::EDeleting;

			CImapFolder* currentFolder = iSyncManager.GetFolderL(iNextFolderSync);

			if (currentFolder)
				{
				for (TInt i = 0; i < iOutstandingLocalDeletes->Count(); ++i)
					{
					TMsvId id = (*iOutstandingLocalDeletes)[i];
					__LOG_FORMAT((iSession->LogId(), "CImapCompoundSyncService::DoRunLoopL : Removing message %d.", id));
					currentFolder->DeleteMessageL(id);
					}

				iOutstandingLocalDeletes->Reset();

				// if we are doing deletes on connection then all deletes will
				// be marked Delete
				if (ProcessPendingDeleteOpsListL(!iPerformDeletes))
					{
					__LOG_FORMAT((iSession->LogId(), "CImapCompoundSyncService::DoRunLoopL : Selecting the folder %S for expunge.", &(currentFolder->FullFolderPathL())));
					MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmImapSyncEFolderEarlyDeletes);
					currentFolder->SelectL(iStatus, *iSession);
					iNextStep = EFolderEarlyExpunge;
					SetActive();
					return EFalse;
					}
				}
			else
				{
				iOutstandingLocalDeletes->Reset();
				}

			iNextStep = ESelectSyncFolder;
			CompleteSelf();
			}
			break;
		case EFolderEarlyExpunge: // asynchronous
			{
			iSyncProgressState=TImap4SyncProgress::EDeleting;

			iNextStep = ESelectSyncFolder;

			if (iStatus.Int() == KErrImapNo)
				{
				++iFoldersNotFound;
				}
			else
				{
				iFolder = iSyncManager.GetFolderL(iNextFolderSync);
				if(iFolder)
					{
					// expunge the folder
					__LOG_FORMAT((iSession->LogId(), "CImapCompoundSyncService::DoRunLoopL : Expunge the folder %S.", &(iFolder->FullFolderPathL())));
					MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmImapSyncEFolderEarlyExpunge);
					iFolder->SyncDeletesL(iStatus, *iSession);
					SetActive();
					return EFalse;
					}
				}

			CompleteSelf();
			}
			break;
		case ESelectSyncFolder: // synchronous
			{
			__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncService::ESelectSyncFolder()");
			// suspend operation if stop for migrate has been requested
			if (iStopForMigrate)
				{
				__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncService::Stopped for migrate");
				iCurrentStep = ESuspendedForMigrate;
				iNextStep = ESelectSyncFolder;
				Complete(KErrNone);
				return ETrue;
				}
				
			// Call for the first time will not be active
			iSyncManager.NextSubscribedFoldersL(iNextFolderSync);
			iSyncProgressState=TImap4SyncProgress::ESyncOther;

			if(iNextFolderSync > 0)
				{
				// Perform the pending offline operations first
				iNextStep = ESyncFolder;
				if(RefreshOutstandingOpsL(iNextFolderSync))
					{
					iNextStep = EFolderCheckPendingOPs;	
					}
				}
			else
				{
				// All folders are done.
				iNextStep = ESyncInboxAutoFetch;
				}
			CompleteSelf();
			}
			break;
		case ESyncFolder: // async
			{
			if(iNextFolderSync > 0)
				{
				// Now sync the folder
				MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmImapSyncESyncFolder);
				iSyncManager.SynchroniseSubscribedFoldersL(iStatus, *iSession, iNextFolderSync);
				iNextStep = EFolderEarlyDeletes;
				}
			else
				{
				// All folders are done.
				iNextStep = ESyncInboxAutoFetch;
				CompleteSelf();
				}
			}
			break;	
		case EInboxLateDeletes: // async
			{
			iNextFolderSync = 0;
			iSyncProgressState=TImap4SyncProgress::EDeleting;
			CImapFolder* inbox = iSyncManager.Inbox();

			if (inbox)
				{
				if (ProcessPendingDeleteOpsL( inbox->MailboxId(), EFalse ))
					{
					MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmImapEInboxLateDeletes);
					inbox->SelectL(iStatus, *iSession);
					iNextStep = EInboxLateExpunge;
					SetActive();
					return EFalse;
					}
				}

			iNextStep = EFolderLateDeletes;
			CompleteSelf();
			}
			break;	
		case EInboxLateExpunge: // async
			{
			iNextStep = EFolderLateDeletes;

			if (iStatus.Int() == KErrImapNo)
				{
				iFoldersNotFound++;
				}
			else
				{
				// expunge the folder
				iFolder = iSyncManager.Inbox();
				
				if(iFolder)
					{
					MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmImapSyncESyncFolder);
					__LOG_FORMAT((iSession->LogId(), "CImapCompoundSyncService::DoRunLoopL : Late Expunge the folder %S.", &(iFolder->FullFolderPathL())));
					iFolder->SyncDeletesL(iStatus, *iSession);
					SetActive();
					return EFalse;
					}
				}

			CompleteSelf();
			}
			break;
		case EFolderLateDeletes: // async
			{
			iSyncProgressState=TImap4SyncProgress::EDeleting;

			// Call for the first time will not be active
			iSyncManager.NextSubscribedFoldersL(iNextFolderSync);

			if(iNextFolderSync > 0)
				{
				if (ProcessPendingDeleteOpsListL( EFalse ) )
					{
					iNextStep = EFolderLateExpunge;
					}		
				else
					{
					CImapFolder* currentFolder = iSyncManager.GetFolderL(iNextFolderSync);
					if(currentFolder)
						{
						__LOG_FORMAT((iSession->LogId(), "CImapCompoundSyncService::DoRunLoopL : Selecting the folder %S for expunge.", &(currentFolder->FullFolderPathL())));
						MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmImapSyncEFolderLateDeletes);
						iNextStep = EFolderLateExpunge;
						currentFolder->SelectL(iStatus, *iSession);
						SetActive();
						return EFalse;
						}

					// back to select next folder
					iNextStep = EFolderLateDeletes;
					}
				}
			else
				{
				// No more subscribed folder was found
				iNextStep = EFinished;
				}

			CompleteSelf();
			}
			break;
		case EFolderLateExpunge: // async
			{
			iNextStep = EFolderLateDeletes;

			if (iStatus.Int() == KErrImapNo)
				{
				iFoldersNotFound++;
				}
			else
				{
				iFolder = iSyncManager.GetFolderL(iNextFolderSync);
				if(iFolder)
					{
					__LOG_FORMAT((iSession->LogId(), "CImapCompoundSyncService::DoRunLoopL : Late Expunge the folder %S.", &(iFolder->FullFolderPathL())));
					// expunge the folder
					MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmImapSyncEFolderLateExpunge);
					iFolder->SyncDeletesL(iStatus, *iSession);
					SetActive();
					return EFalse;
					}
				}

			CompleteSelf();
			}
			break;
		case ESyncInboxAutoFetch:
			{
			// suspend operation if stop for migrate has been requested
			if (iStopForMigrate)
				{
				__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncService::Stopped for migrate");
				iCurrentStep = ESuspendedForMigrate;
				iNextStep = ESyncInboxAutoFetch;
				Complete(KErrNone);
				return ETrue;
				}
				
			iSyncProgressState=TImap4SyncProgress::ESyncOther;
			if (!iImapSettings.UseSyncDownloadRules())
				{
				// not using sync rules, loop to EFinished.
				iNextStep = EFinished;
				return EFalse;
				}

			__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncService::Starting Second Phase Synchronisation");

			// set the next step to perform
			iNextStep = ESyncFolderAutoFetchCheck;
			
			if (KErrNotFound == iImapSettings.GetSyncDownloadRuleL(CImapSyncDownloadRules::EInboxRulesType, iMailInfo))
				{
				// No download rules for the inbox, check other folders
				__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncService::No download rules for inbox on this bearer");
				return EFalse;
				}
			
			CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
			CleanupStack::PushL(selection);
			
			TInt msgsToFetch = iSyncManager.Inbox()->GetFetchMessageChildrenL(*selection);
			__LOG_FORMAT((iSession->LogId(), "CImapCompoundSyncService::Fetching %d INBOX messages.", msgsToFetch));
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
				                                               iMailInfo);
				iImapCompound->StartOperation(iStatus, *iSession);
				if (iDoNotFetchSel)
					{
					((CImapCompoundCopyToLocal*)iImapCompound)->RemoveFromSelection(*iDoNotFetchSel);
					}
				SetActive();
				}
			CleanupStack::PopAndDestroy(selection);
			return EFalse; // if not active, will re-enter this function and perform next step...
			}
			
		case ESyncFolderAutoFetchCheck:
			{
			// suspend operation if stop for migrate has been requested
			if (iStopForMigrate)
				{
				__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncService::Stopped for migrate");
				iCurrentStep = ESuspendedForMigrate;
				// fetching of inbox contents may have been stopped prematurely,
				// on resume, go back to check if any inbox messages still to fetch.
				iNextStep = ESyncInboxAutoFetch;
				Complete(KErrNone);
				return ETrue;
				}

			// reset folder sync counter
			iFolderSyncCount = 0;
			// can only get here if download rules being used, so no need to check that.
			// No point in continuing if no download rules defined for non-inbox folders:
			if (KErrNotFound == iImapSettings.GetSyncDownloadRuleL(CImapSyncDownloadRules::EFolderRulesType, iMailInfo))
				{
				// No download rules for non-inbox folders.
				iNextStep = EFinished;
				return EFalse;
				}
			// else continue to next step...
			}
		case ESyncFolderAutoFetch:
			{
			// suspend operation if stop for migrate has been requested
			if (iStopForMigrate)
				{
				__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncService::Stopped for migrate");
				iCurrentStep = ESuspendedForMigrate;
				iNextStep = ESyncFolderAutoFetchCheck;
				// delete the compound fetch operation - we will create a new one on resume,
				// using the download rules for the new bearer.
				delete iImapCompound;
				iImapCompound = NULL;
				Complete(KErrNone);
				return ETrue;
				}

			TInt32 numSubscribedFolders;
			CImapFolder* folder = iSyncManager.GetSubscribedFolder(iFolderSyncCount, numSubscribedFolders);
			if (folder)
				{
				CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
				CleanupStack::PushL(selection);
				TInt msgsToFetch = folder->GetFetchMessageChildrenL(*selection);
				__LOG_FORMAT((iSession->LogId(), "CImapCompoundSyncService::Fetching %d messages in folder %d.", msgsToFetch, folder->MailboxId()));
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
					                                               iMailInfo);
					iImapCompound->StartOperation(iStatus, *iSession);
					if (iDoNotFetchSel)
						{
						((CImapCompoundCopyToLocal*)iImapCompound)->RemoveFromSelection(*iDoNotFetchSel);
						}
					SetActive();
					}
				CleanupStack::PopAndDestroy(selection);
				// when complete, check the next folder...
				++iFolderSyncCount;
				iNextStep = ESyncFolderAutoFetch;
				}
			else
				{
				__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncService::Download Rules Auto Fetch Completed.");
				iNextStep = EFinished;
				}
			return EFalse; // not finished yet...
			}
			
		case EFinished:
			{
			__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncService::Completing OK");
			iSyncProgressState=TImap4SyncProgress::EIdle;
			iProgressState = TImap4GenericProgress::EIdle;
			Complete(KErrNone);
			return ETrue;
			}
		default:
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ESyncServiceCompoundUnexpectedState));
			// unexpected state - complete the request
			iSyncProgressState=TImap4SyncProgress::EIdle;
			iProgressState = TImap4GenericProgress::EIdle;
			return ETrue;
			}
		} // end of switch (iCurrentStep)

	if (!IsActive())
		{
		SetActive();
		}
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

In most cases, it is possible to resume at the current step following a
cancel-for-migration (sometimes following a SELECT on the current folder).

Note that if the default CMsgActive::DoComplete() is overridden,
then that must also be modified to handle either case described above.

Note on cancelling folder select operations: the folder actually passes 
the iStatus to the session, hence the cancel is on the session.
*/
void CImapCompoundSyncService::DoCancel()
	{
	switch (iCurrentStep)
		{
		case ESelectSourceMailboxRW:
			{
			// only set by ResumeOperation. No update to iNextStep required
			iSession->Cancel();
			break;
			}
		case EInboxEarlyDeletes:  // re-select inbox before resuming
		case EFolderEarlyDeletes: // re-select current folder before resuming
		case EInboxLateDeletes:   // wont be resumed, as part of disconnect
		case EFolderLateDeletes:  // wont be resumed, as part of disconnect
			{
			// outstanding request on session.
			iSession->Cancel();
			break;
			}
		case EInboxEarlyExpunge:  // re-select inbox before resuming
		case EFolderEarlyExpunge: // re-select current folder before resuming
		case EInboxLateExpunge:   // should not be resumed, as part of disconnect
		case EFolderLateExpunge:  // should not resumed, as part of disconnect
			{
			// outstanding request on folder
			if (iFolder)
				{
				iFolder->Cancel();
				}
			iNextStep = iCurrentStep; 
			break;
			}
		case EInboxSync:
		case ESyncSubscriptions:
		case ESyncTree:
		case ESyncFolder:
			{
			// outstanding request on syncmanager
			if (iCancelForMigrate)
				{
				iSyncManager.CancelForMigrate();
				}
			else
				{
				iSyncManager.Cancel();
				}
			// resume from the tree synchronisation
			iNextStep = iCurrentStep;			
			break;
			}		
		case EFolderCheckPendingOPs:
		case EInboxCheckPendingOps:
			{
			// outstanding request on compound object
			if (iImapCompound)
				{
				if (iCancelForMigrate)
					{
					iImapCompound->CancelForMigrate();
					// do not update iNextStep ( == EInboxDoneOps || EFolderDoneOps )
					// This operation will be resumed...
					}
				else
					{
					iImapCompound->Cancel();
					}
				}
			break;
			}
		case ESyncInboxAutoFetch:
			{
			// outstanding request on compound object
			if (iImapCompound)
				{
				// don't cancel-for-migrate - will re-create new object on restart
				// as the download rules may well be different on the new carrier.
				iImapCompound->Cancel();
				}
			iNextStep = ESyncInboxAutoFetch;
			break;
			}
		case ESyncFolderAutoFetchCheck:
		case ESyncFolderAutoFetch:
			{
			// outstanding request on compound object
			if (iImapCompound)
				{
				// don't cancel-for-migrate - will re-create new object on restart
				// as the download rules may well be different on the new carrier.
				iImapCompound->Cancel();
				}
			// restart in the check state to update the download rules being used.
			iNextStep = ESyncFolderAutoFetchCheck;
			break;
			}
		case EInboxDoneOps:
		case ESyncService:
		case EFolderDoneOps:
		case ESelectSyncFolder:
		case EFinished:
		case ESuspendedForMigrate:
		default:
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ESyncServiceCompoundCancelUnexpectedState));
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


void CImapCompoundSyncService::Progress(TImap4CompoundProgress& aCompoundProgress)
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
TInt CImapCompoundSyncService::ProcessNegativeServerResponse()
	{
	TInt err = iStatus.Int();
	switch (iCurrentStep)
		{
	case ESyncTree:
	case ESyncSubscriptions:
	case EInboxDoneOps:
	case EFolderDoneOps:
	case EFolderCheckPendingOPs:
	case EInboxCheckPendingOps:
	case EInboxSync:
	case EInboxEarlyDeletes:
	case EFolderEarlyDeletes:
	case EInboxEarlyExpunge:
	case EFolderEarlyExpunge:
	case EInboxLateDeletes:
	case EFolderLateDeletes:
	case EInboxLateExpunge:
	case EFolderLateExpunge:
	case ESyncFolder:
	case ESyncService:
	case ESyncInboxAutoFetch:
	case ESyncFolderAutoFetchCheck:
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
		TImapServerPanic::ImapPanic(TImapServerPanic::ESyncServiceCompoundUnexpectedState);
		break;
		}
		} // end of switch (iCurrentStep)
	iProgressErrorCode = err;
	return KErrNone;
	}


/**
Given an id of a folder or the service, restores the offline
operation array. Returns the number of operations in the array.
Any non-zero return value indicates an action that needs to be addressed.
*/
TBool CImapCompoundSyncService::RefreshOutstandingOpsL(TMsvId aId)
	{
	if (iOutstandingOps)
		{
		delete iOutstandingOps;
		iOutstandingOps = NULL;
		}
	
	iOutstandingOps = iImapOfflineControl.OffLineOpArrayL(aId);
	iOutstandingOpsFolder = aId;

	// reset the count
	iMsgsToDo = iOutstandingOps->CountOperations();
	
	iMsgsDone = 0;
	
	iMovedId = KMsvNullIndexEntryId;
	iShadowId = KMsvNullIndexEntryId;

	return iMsgsToDo;
	}

/**
 This routine sets up iShadowId which will be deleted when the
operation completes successfully

*/
void CImapCompoundSyncService::DoOpL(const CImOffLineOperation& aOp)
	{
	// clean the disconnected op flags and ensure its visible and get
	// an entry copy
	SetEntryL(aOp.MessageId());
	TMsvEmailEntry entry = iServerEntry.Entry();
	entry.SetVisible(ETrue);
	entry.SetDisconnectedOperation(ENoDisconnectedOperations);
	ChangeEntryL(entry);

	// check and see if there is a shadow and whether it has been
	// removed or marked for deletion
	TBool shadowOK = ETrue;
	if ( aOp.OpType() != CImOffLineOperation::EOffLineOpMtmSpecific &&
		 aOp.OpType() != CImOffLineOperation::EOffLineOpDelete )
		{
		iShadowId = iImapOfflineControl.FindShadowIdL(aOp);

		shadowOK = iShadowId != KMsvNullIndexEntryId &&
			iServerEntry.SetEntry(iShadowId) == KErrNone &&
			((TMsvEmailEntry)iServerEntry.Entry()).DisconnectedOperation() != EDisconnectedDeleteOperation;
		}

	delete iImapCompound;
	iImapCompound = NULL;

	CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(selection);
	
	// Deal with operation
	switch(aOp.OpType())
		{
	case CImOffLineOperation::EOffLineOpCopyToLocal:
		// do the copy, if not a message
		if (entry.iType != KUidMsvMessageEntry ||
			// or the shadow exists
			shadowOK )
			{
			__LOG_FORMAT((iSession->LogId(), "CImOffLineOperation::EOffLineOpCopyToLocal : %d %d", aOp.MessageId(), aOp.TargetMessageId()));

			//Create Copy to Local Compound object
		
			selection->AppendL(aOp.MessageId());
			
			iImapCompound = CImapCompoundCopyToLocal::NewL(iSyncManager, 
														   iServerEntry, 
														   iImapSettings,
														   iImapMailStore,
														   EFalse,
		                                               	   *selection,
		                                               	   aOp.TargetMessageId());
			
			iImapCompound->StartOperation(iStatus, *iSession);
			SetActive();
			}
		break;
		
	case CImOffLineOperation::EOffLineOpCopyFromLocal:
		if (shadowOK)
			{
			__LOG_FORMAT((iSession->LogId(), "CImOffLineOperation::EOffLineOpCopyFromLocal : %d %d", aOp.MessageId(), aOp.TargetMessageId()));

			//Create Copy from Local Compound object

			selection->AppendL(aOp.MessageId());
			
			iImapCompound = CImapCompoundCopyFromLocal::NewL(iSyncManager, 
														     iServerEntry, 
														     iImapSettings,
														     EFalse,
		                                               	     *selection,
		                                               	     aOp.TargetMessageId());
			
			iImapCompound->StartOperation(iStatus, *iSession);
			SetActive();
			}
		break;

	case CImOffLineOperation::EOffLineOpCopyWithinService:
		if (shadowOK)
			{
			__LOG_FORMAT((iSession->LogId(), "CImOffLineOperation::EOffLineOpCopyWithinService : %d %d", aOp.MessageId(), aOp.TargetMessageId()));

			//Create Copy within service Compound object

			selection->AppendL(aOp.MessageId());
			
			iImapCompound = CImapCompoundCopyWithinService::NewL(iSyncManager, 
														   		 iServerEntry, 
														   		 iImapSettings,
														   		 EFalse,
														   		 *selection,
														   		 aOp.TargetMessageId());
			
			iImapCompound->StartOperation(iStatus, *iSession);
			SetActive();
			}
		break;
		
	case CImOffLineOperation::EOffLineOpMoveToLocal:
		if (shadowOK)
			{
			__LOG_FORMAT((iSession->LogId(), "CImOffLineOperation::EOffLineOpMoveToLocal : %d %d", aOp.MessageId(), aOp.TargetMessageId()));

			selection->AppendL(aOp.MessageId());
			
			iImapCompound = CImapCompoundCopyToLocal::NewL(iSyncManager, 
														   iServerEntry, 
														   iImapSettings,
														   iImapMailStore,
														   EFalse,
		                                               	   *selection,
		                                               	   aOp.TargetMessageId());
			
			iImapCompound->StartOperation(iStatus, *iSession);
			SetActive();
			}
		// even if the shadow has been removed we still want to delete
		// the original
		iMovedId = aOp.MessageId();
		break;

	case CImOffLineOperation::EOffLineOpMoveFromLocal:
		if (shadowOK)
			{
			__LOG_FORMAT((iSession->LogId(), "CImOffLineOperation::EOffLineOpMoveFromLocal : %d %d", aOp.MessageId(), aOp.TargetMessageId()));

			//Create Copy from Local Compound object

			selection->AppendL(aOp.MessageId());
			
			iImapCompound = CImapCompoundCopyFromLocal::NewL(iSyncManager, 
														     iServerEntry, 
														     iImapSettings,
														     EFalse,
		                                               	     *selection,
		                                               	     aOp.TargetMessageId());
			
			iImapCompound->StartOperation(iStatus, *iSession);
			SetActive();
			}
		// even if the shadow has been removed we still want to delete
		// the original
		iMovedId = aOp.MessageId();
		break;

	case CImOffLineOperation::EOffLineOpMoveWithinService:
		if (shadowOK)
			{
			__LOG_FORMAT((iSession->LogId(), "CImOffLineOperation::EOffLineOpMoveWithinService : %d %d", aOp.MessageId(), aOp.TargetMessageId()));

			//Create Copy within service Compound object

			selection->AppendL(aOp.MessageId());
			
			iImapCompound = CImapCompoundCopyWithinService::NewL(iSyncManager, 
														   		 iServerEntry, 
														   		 iImapSettings,
														   		 EFalse,
														   		 *selection,
														   		 aOp.TargetMessageId());
			
			iImapCompound->StartOperation(iStatus, *iSession);
			SetActive();
			}
		// even if the shadow has been removed we still want to delete
		// the original, unless the folder itself has been removed
		if (iServerEntry.SetEntry(aOp.TargetMessageId()) == KErrNone)
			{
			iMovedId = aOp.MessageId();
			}
		break;

	case CImOffLineOperation::EOffLineOpMtmSpecific:
		switch (aOp.MtmFunctionId())
			{
		case EFnOffLineOpPopulate:
			{
			__LOG_FORMAT((iSession->LogId(), "CImOffLineOperation::EFnOffLineOpPopulate : %d", aOp.MessageId()));

			//Create Copy to Local Compound object
		
			selection->AppendL(aOp.MessageId());
			
			// aOp.MtmParameters() for EFnOffLineOpPopulate is a package that contains the partial mail info.
			//
			// 1) Create an unitialised dummy partial mail object.  This will NOT receive the partial mail data, but is needed for the build.
			TImImap4GetPartialMailInfo dummyPartialMailInfo;
			// 2) Create a package object that WILL point at the package data.
			//    but for now we need to point it at our unitialised partial mail object, in order for it to compile.
			TPckgC<TImImap4GetPartialMailInfo> partialMailInfoPackage(dummyPartialMailInfo);
			// 3) Immediately point our package object at the actual package data.
			partialMailInfoPackage.Set(aOp.MtmParameters());
					
			iImapCompound = CImapCompoundCopyToLocal::NewL(iSyncManager, 
														   iServerEntry, 
														   iImapSettings,
														   iImapMailStore,
														   EFalse,
		                                               	   *selection,
		                                               	   KMsvNullIndexEntryId,
		                                               	   partialMailInfoPackage()); // pass the actual package data as an object
			
			iImapCompound->StartOperation(iStatus, *iSession);
			SetActive();
			break;
			}
		default:
			break;
			}
		break;

	case CImOffLineOperation::EOffLineOpDelete:
	default:
		break;
		}

	CleanupStack::PopAndDestroy(selection);
	}

/**
Tidies up after performing an offline operation. 
In the case of move operations, this means storing an array of messages that are to
be deleted on the server later in the sync phase (early deletes) or as the service 
is disconnected (late deletes).

*/
void CImapCompoundSyncService::DonePendingOpL()
	{
	// if we've done one then...
	if (iMsgsDone != 0)
		{
		// if this was a move then append a delete
		if (iMovedId != KMsvNullIndexEntryId)
			{
			SetEntryL(iMovedId);
			if (iServerEntry.Entry().Parent() == iOutstandingOpsFolder)
				{
				__LOG_FORMAT((iSession->LogId(), "Append MoveDelete for %x", iMovedId));
				iOutstandingMoveTypeDeletes->AppendL(iMovedId);
				}
			else
				{
				// if this id was from a MoveFrom (ie its parent is not
				// this folder) then put it in a different pending array
				__LOG_FORMAT((iSession->LogId(), "Append LocalDelete for %x", iMovedId));
				iOutstandingLocalDeletes->AppendL(iMovedId);
				}
			}
		
		// delete the shadowid if there is one, ignore errors
		if (iShadowId != KMsvNullIndexEntryId &&
			iServerEntry.SetEntry(iShadowId) == KErrNone &&
			iServerEntry.SetEntry(iServerEntry.Entry().Parent()) == KErrNone)
			{
			iServerEntry.DeleteEntry(iShadowId);
			}
		}
	}

/**
Checks current folder's pending offline operation list. If no more pending operations
exist, returns ETrue, otherwise calls DoOpL() and returns EFalse.

DoOpL() creates a compound operation object to perform the requested operation, 
and launches it, setting this class active.

Note in some circumstances it is possible that the operation cannot be performed, for
example if the folder in question has subsequently become unsubscribed. If this occurs,
this function shall return EFalse (as though an operation had been started) however,
the class will not have been set active.

@return ETrue if no more pending operations to be performed at this time.
*/
TBool CImapCompoundSyncService::NextPendingOpL()
	{
	TBool finished = EFalse;

	iMovedId = KMsvNullIndexEntryId;
	iShadowId = KMsvNullIndexEntryId;

	// Any operations in outstanding list?
	if (iOutstandingOps->CountOperations())
		{
		__LOG_FORMAT((iSession->LogId(), "Outstanding operations on this folder %d", iOutstandingOps->CountOperations()));

		// Fetch operation
		CImOffLineOperation* thisop = new(ELeave)CImOffLineOperation();
		CleanupStack::PushL(thisop);
		
		thisop->CopyL(iOutstandingOps->Operation(0));

		// when we get to one of the Delete operations then it is time
		// to stop
		if (thisop->OpType() == CImOffLineOperation::EOffLineOpDelete ||
			(thisop->OpType() == CImOffLineOperation::EOffLineOpMtmSpecific &&
			 thisop->MtmFunctionId() == EFnOffLineOpMoveDelete))
			{
			__LOG_TEXT(iSession->LogId(),"Reached delete op. Finished");
			++iMsgsDone;
			finished = ETrue;
			}
		else
			{
			// remove from list and save back
			iOutstandingOps->Delete(0);
			iImapOfflineControl.SetOffLineOpArrayL(iOutstandingOpsFolder, *iOutstandingOps);

			// and execute
			DoOpL(*thisop);
			++iMsgsDone;
			}
		
		CleanupStack::PopAndDestroy(thisop);
		}
	else
		{
		// No more operations to do, return to what we should be doing next
		finished = ETrue;
		}

	// when we are about to finish this folder then tidy up
	if (finished)
		{
		// add the list of pending deletes to the front of the
		// offline op array
		for (TInt i = 0; i < iOutstandingMoveTypeDeletes->Count(); i++)
			{
			TMsvId id = (*iOutstandingMoveTypeDeletes)[i];
			CImOffLineOperation* thisop = new(ELeave)CImOffLineOperation();
			CleanupStack::PushL(thisop);

			// if we are doing deletes on connection then store this
			// as a delete and we will do all deletes at the end of
			// the sync.

			// if we are doing deletes on disconnection then store
			// this as a special code - it will still get done at end
			// of sync, but real deletes will be done on
			// disconnection.
			if (!iImapSettings.DeleteEmailsWhenDisconnecting()) // iPerformDeletes
				{
				thisop->SetDelete(id);
				}				
			else
				{
				thisop->SetMtmSpecificCommandL(id, EFnOffLineOpMoveDelete, 0, KNullDesC8());
				}

			iOutstandingOps->InsertOperationL(*thisop, 0);
			CleanupStack::PopAndDestroy(thisop);
			}

		// if there were outstanding move type deletes then clear
		// their array and save back the main outstanding ops list
		if (iOutstandingMoveTypeDeletes->Count())
			{
			iOutstandingMoveTypeDeletes->Reset();

			iImapOfflineControl.SetOffLineOpArrayL(iOutstandingOpsFolder, *iOutstandingOps);
			}
		}

	return finished;
	}

TBool CImapCompoundSyncService::MatchDeleteOp(const CImOffLineOperation& aOp , TBool aMoveDeletesOnly )
	{
	return (aOp.OpType() == CImOffLineOperation::EOffLineOpMtmSpecific && aOp.MtmFunctionId() == EFnOffLineOpMoveDelete) ||
		(!aMoveDeletesOnly && aOp.OpType() == CImOffLineOperation::EOffLineOpDelete);
	}

TBool CImapCompoundSyncService::ProcessPendingDeleteOpsL( TMsvId aFolder, TBool aMoveDeletesOnly )
	{
	TBool hadDeletes = EFalse;

	__LOG_FORMAT((iSession->LogId(), "CImapCompoundSyncService::ProcessPendingDeleteOpsL: Folder %x aMoveDeletesOnly=%d", aFolder, aMoveDeletesOnly));
		
	// get the current offline operations of this folder
	if (RefreshOutstandingOpsL(aFolder))
		{
		// Fetch operation
		CImOffLineOperation* thisop = new(ELeave)CImOffLineOperation();
		CleanupStack::PushL(thisop);
		
		thisop->CopyL(iOutstandingOps->Operation(0));

		// check delete type
		if (MatchDeleteOp(*thisop, aMoveDeletesOnly))
			{
			do
				{
				// if can't find the entry then just skip it so it is
				// removed from array
				if (iServerEntry.SetEntry(thisop->MessageId()) == KErrNone)
					{
					// set its server deleted flag
					TMsvEmailEntry entry=iServerEntry.Entry();
					entry.SetDeletedIMAP4Flag(ETrue);
					ChangeEntryL(entry);
					}

				// remove from the array and write back immediately
				iOutstandingOps->Delete(0);
				iImapOfflineControl.SetOffLineOpArrayL(iOutstandingOpsFolder, *iOutstandingOps);

				// check for finish
				if (iOutstandingOps->CountOperations() == 0)
					break;
			
				// get next op
				thisop->CopyL(iOutstandingOps->Operation(0));
				}
			while (MatchDeleteOp(*thisop, aMoveDeletesOnly));
					
			hadDeletes = ETrue;
			}
			
		CleanupStack::PopAndDestroy(thisop);
		}

	return hadDeletes;
	}

TBool CImapCompoundSyncService::ProcessPendingDeleteOpsListL( TBool aMoveDeletesOnly )
	{
	if(iNextFolderSync > 0)
		{
		if (ProcessPendingDeleteOpsL( iNextFolderSync, aMoveDeletesOnly ))
			{
			return ETrue;
			}				
		}

	return EFalse;
	}

/**
Sets the context's index entry to the specified values on the Message server

@param  aEntry
The new details for the entry.
@leave KErrAccessDenied - the entry is read only (deleted entry, standard folder, or locked); 
KErrNotSupported - aEntry is invalid or the ID specified in aEntry is not the same as the context ID
or no context has been set for the object; 
KErrNoMemory - a memory allocation failed. 
*/
void CImapCompoundSyncService::ChangeEntryL(const TMsvEntry& aEntry)
	{
	User::LeaveIfError(iServerEntry.ChangeEntry(aEntry));
	}

/**
Called when operation has completed and we are about to complete the caller.

@param aErr Error code that we will complete with.
*/
void CImapCompoundSyncService::DoComplete(TInt& aErr)
	{
	CImapCompoundBase::DoComplete(aErr);

	if (!iCancelForMigrate)
		{
		// Reset the flags which show if the new flags should be cleared on the
		// next sync.
		iSyncManager.ResetInboxClearNewFlags();
		iSyncManager.ResetNonInboxClearNewFlags();
		}
	}


/**
Resumes the operation following a migration.
*/
void CImapCompoundSyncService::ResumeOperationL(TRequestStatus& aStatus, CImapSession& aSession)
	{
	iSession = &aSession;
	__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncService::Resuming");
	__ASSERT_DEBUG(iCurrentStep==ESuspendedForMigrate, TImapServerPanic::ImapPanic(TImapServerPanic::ESyncServiceCompoundUnexpectedState));
	iStopForMigrate = EFalse;
	
	switch (iNextStep)
		{
		case EInboxEarlyExpunge:
		case EInboxEarlyDeletes:
			{
			// re-select the inbox, and then re-enter state machine
			iSyncManager.Inbox()->SelectL(iStatus, *iSession);
			SetActive();
			iCurrentStep = ESelectSourceMailboxRW;
			break;
			}
		case EFolderEarlyDeletes:
		case EFolderEarlyExpunge:
			{
			// re-select the inbox, and then re-enter state machine
			iSyncManager.GetFolderL(iNextFolderSync)->SelectL(iStatus, *iSession);
			iCurrentStep = ESelectSourceMailboxRW;
			break;
			}
		case EInboxLateDeletes:
		case EFolderLateDeletes:
		case EInboxLateExpunge:
		case EFolderLateExpunge:
			{
			// resume not expected since these states are only entered during
			// a disconnection operation. Migrations are not accepted during
			// diconnection - a downgrade results in the operation being
			// cancelled and any sessions deleted.
			iNextStep=EFinished;
			CompleteSelf();
			break;
			}
		case EInboxSync:  			// select is called by the sync manager in this state
		case ESyncSubscriptions:	// select not required
		case ESyncTree:				// select not required
		case ESyncFolder:			// select is called by the sync manager in this state
		case ESelectSyncFolder:		// self-completed state, however can suspend in this state
		case ESyncInboxAutoFetch:   // select not required prior to resuming 2nd phase
		case ESyncFolderAutoFetchCheck:// select not required prior to resuming 2nd phase
			{
			// resume the state machine in this state.
			CompleteSelf();
			break;
			}		
		case EInboxDoneOps:
			{
			if (iImapCompound)
				{
				// restart the compound object performing the offline-requested operation.
				iCurrentStep = EInboxCheckPendingOps;
				iImapCompound->ResumeOperationL(iStatus, *iSession);
				SetActive();
				}
			else
				{
				// Resume with check for further pending inbox operations
				iNextStep = EInboxCheckPendingOps;
				CompleteSelf();
				}
			break;
			}
		case EFolderDoneOps:
			{
			if (iImapCompound)
				{
				// resume the compound object performing the offline-requested operation.
				iCurrentStep = EFolderCheckPendingOPs;
				iImapCompound->ResumeOperationL(iStatus, *iSession);
				SetActive();
				}
			else
				{
				// Resume with check for further pending folder operations
				iNextStep = EFolderCheckPendingOPs;
				CompleteSelf();
				}
			break;
			}
		case ESyncFolderAutoFetch:
		case EFolderCheckPendingOPs: // not expected...
		case EInboxCheckPendingOps:  // not expected...
		case ESyncService:           // self-completed state
		case EFinished:
		default:
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ESyncServiceCompoundCancelUnexpectedState));
			// something unexpected has happened. Complete the operation.
			iNextStep = EFinished;
			break;
			}
		} // end of switch (iNextStep)
	Queue(aStatus);
	}

/**
Provides the sync class with an array of messages that are not to be fetched according
to the download rules. This function is called by the background sync controller if a
fetch command is received by the server MTM is performing a background synchronise, and
allows the sync operation to prevent attempts to simultaneously fetch the same message
via multiple imap sessions.

If message content fetch is already underway, then the array is passed to the 
CImapCompoundCopyToLocal object for immediate checking. The array is then stored
within this class and each subsequent copy to local operation is checked for matching
message id's.

Note that it is possible for a message ID to be remove from the passed entry selection
in the case that that message is currently being fetched at the time of calling this
function.

@param aSelection - the selection of messages that should not be autofetched
*/
void CImapCompoundSyncService::RemoveFromSelectionL(CMsvEntrySelection& aDeleteSel)
	{
	if (iCurrentStep == ESyncFolderAutoFetch ||
	    iCurrentStep == ESyncInboxAutoFetch  ||
	    iCurrentStep == ESyncFolderAutoFetchCheck)
		{
		((CImapCompoundCopyToLocal*)iImapCompound)->RemoveFromSelection(aDeleteSel);
		}

	// keep a copy of this selection to prevent future fetch clashes.
	delete iDoNotFetchSel;
	iDoNotFetchSel = aDeleteSel.CopyL();
	}

/**
Indicates that the compound operation should complete as soon as it is possible
for the operation to be re-started without requiring a significant amount of
repeated communication.
The operation will be restarted by a call to ResumeOperationL()
Overrides the base-class implementation.
*/
void CImapCompoundSyncService::StopForMigrate()
	{
	__LOG_TEXT(iSession->LogId(), "CImapCompoundSyncService::StopForMigrate()");
	
	if (iCurrentStep==ESyncFolderAutoFetch || iCurrentStep==ESyncInboxAutoFetch)
		{
		// if doing a download-rules based fetch, instruct it to stop.
		if(iImapCompound)
			{
			iImapCompound->StopForMigrate();
			}
		}
	iStopForMigrate = ETrue;
	}

