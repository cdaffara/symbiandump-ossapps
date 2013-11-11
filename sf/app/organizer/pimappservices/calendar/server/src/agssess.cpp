// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "agssess.h"

#include "agmattachment.h"
#include "agscategoryindex.h"
#include "agscategorylist.h"
#include "agmentry.h"
#include "agsiterator.h"
#include "agsentrymodel.h"
#include "agmpanic.h"
#include "agmcategory.h"
#include "agmcontent.h"
#include "agmserv.h"
#include "agsfilemanager.h"
#include "agsmain.h"
#include "calcommonimpl.h"
#include "agmdebug.h"
#include "agmtzrules.h"
#include "agssortinstance.h"
#include "agsinstanceiterator.h"
#include "agmsortcriteria.h"
#include "agmdate.h"
#include "agssort.h"
#include "agmcalendarinfo.h"
#include "calsessionimpl.h"
#include "agmfilechangenotification.h"
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
#include "agssystemstateobserver.h"
#endif

#include <apparc.h>
#include <s32file.h>

//
// CAgnServerSession
//

const TInt KGranFilteredEntries = 20;
const TInt KMaxNumberOfBufferedNotifications = 50;
const TInt KGranNotificationBuffer = 64;

const TInt KSlot0 = 0;
const TInt KSlot1 = 1;
const TInt KSlot2 = 2;
const TInt KSlot3 = 3;

void CleanupCloseAgendaImmediately(TAny* aRef)
    {
    if (aRef)
        {
        static_cast<CAgnServFile*>(aRef)->CloseAgenda(ETrue);
        }
    }

TAgnChangeFilter::TAgnChangeFilter(CAgnServerSession& aSession) :
	iChangeBroadcastEnabled(ETrue),
	iSession(aSession),
	iStartTimeUtc(Time::NullTTime()),
	iEndTimeUtc(Time::NullTTime()),
	iPubSubChangeMadeDuringDisable(ENoChange),
	iPubSubEnabled(ETrue)
	{
	}

TBool TAgnChangeFilter::ChangeBroadcastEnabled() const
	{
	return iChangeBroadcastEnabled;
	}
	
const CAgnServerSession& TAgnChangeFilter::Session() const
	{
	return iSession;
	}

void TAgnChangeFilter::SetEnableChangeBroadcast(TBool aBool)
	{
	iChangeBroadcastEnabled = aBool;
	}

TBool TAgnChangeFilter::ChangeMadeWhileDisabled() const
	{
	return iChangeMadeWhileDisabled;
	}
	
void TAgnChangeFilter::SetChangeMadeWhileDisabled(TBool aBool)
	{
	iChangeMadeWhileDisabled = aBool;	
	}

void TAgnChangeFilter::SetChangeParameter(const TTime& aStartTime, const TTime& aEndTime, MCalChangeCallBack2::TChangeEntryType aChangeType)
	{
	iStartTimeUtc = aStartTime;
	iEndTimeUtc = aEndTime;
	iEntryType = aChangeType;
	}

TBool TAgnChangeFilter::CheckChangeWithinRangeL(const CAgnRptDef* aRptDef, const TTime& aStartTime, const TTime& aEndTime) const
	{
	TBool entryInTimeRange = ETrue;
	if (aRptDef)
		{
		TTime nudgedNextTimeUtc;
		TBool nextDateExists = aRptDef->NudgeNextInstanceUtcL(iStartTimeUtc, nudgedNextTimeUtc);
		if ( ! nextDateExists || nudgedNextTimeUtc > iEndTimeUtc)
			{
			entryInTimeRange = EFalse;
			}
		}
	else
		{
		if (iStartTimeUtc > aEndTime || iEndTimeUtc < aStartTime)
			{
			entryInTimeRange = EFalse;
			}
		}
	return entryInTimeRange;
	}
			
TBool TAgnChangeFilter::IsValidChangeL(const TAgnChange& aChange) const
	{
	if (aChange.iOperationType != MCalChangeCallBack2::EChangeUndefined)
		{
		if (iEntryType == MCalChangeCallBack2::EChangeEntryTodo && aChange.iEntryType != CCalEntry::ETodo)
			{
			return EFalse;
			}
		if (iEntryType == MCalChangeCallBack2::EChangeEntryEvent && aChange.iEntryType == CCalEntry::ETodo)
			{
			return EFalse;
			}

		// Check for the requested entry type and the change type.
		// Don't notify clients which are listening for event and
		// to-do notifictaions when note event is modified.
		if ( ((iEntryType == MCalChangeCallBack2::EChangeEntryEvent) ||
			(iEntryType == MCalChangeCallBack2::EChangeEntryTodo)) &&
			(aChange.iEntryType == CCalEntry::ENote))
			{
			return EFalse;
			}

		// Notify clients which are listening for event type note modifications.
		if (iEntryType == MCalChangeCallBack2::EChangeEntryNote &&
			aChange.iEntryType == CCalEntry::ENote)
			{
			return ETrue;
			}

		// Check the entry is within the time range specified by the filter
		// aChange.iRepeatRule gives the repeat data for the newly stored entry. If this operation is an update,
		// then aChange.iOriginalRepeatRule gives the repeat data for the old entry.
		TBool entryInTimeRange = CheckChangeWithinRangeL(aChange.iRepeatRule, aChange.iStartTimeOfEntryUtc, aChange.iEndTimeOfEntryUtc);
		TBool originalEntryInTimeRange = CheckChangeWithinRangeL(aChange.iOriginalRepeatRule, aChange.iOriginalStartTimeUtc, aChange.iOriginalEndTimeUtc);
			
		if (iEntryType != MCalChangeCallBack2::EChangeEntryEvent)
			{
			if (aChange.iEntryType == CCalEntry::ETodo && aChange.iStartTimeOfEntryUtc == Time::NullTTime() &&  aChange.iEndTimeOfEntryUtc == Time::NullTTime())
				{
				return ETrue;
				}
			}
		
		if ( ! entryInTimeRange && ! originalEntryInTimeRange)
			{
			return EFalse;
			}
		}
	return ETrue;
	}
	
TBool TAgnChangeFilter::PubSubEnabled() const
	{
	return iPubSubEnabled;
	}

void TAgnChangeFilter::SetPubSubChange(TPubSubChange aChange)
	{
	if(aChange==ENoChange)
		{//clear all bits
		iPubSubChangeMadeDuringDisable = ENoChange; // = 0
		}
	else
		{
		iPubSubChangeMadeDuringDisable = iPubSubChangeMadeDuringDisable | aChange;
		}
	}
	
TBool TAgnChangeFilter::TodoChanged() const
	{
	return iPubSubChangeMadeDuringDisable & ETodoChanged;
	}

void TAgnChangeFilter::SetEnablePubSubNotification(TBool aEnablePubSubNotification)
	{
	iPubSubEnabled = aEnablePubSubNotification;
	}
	
CAgnServerSession::CAgnServerSession(CAgnServer& aServer)
    :iAgnServer(aServer), iSessionReady(ETrue)
	{
	iAgnServer.SessionAdded();
	}

CAgnServerSession* CAgnServerSession::NewL(CAgnServer& aServer)
    {
    CAgnServerSession* self = new (ELeave) CAgnServerSession(aServer);
    CleanupStack::PushL(self);
    self->ConstrucL();
    CleanupStack::Pop(self);
    return self;
    }

void CAgnServerSession::ConstrucL()
    {
    iAgnSessionFileManager = CAgnSessionFileManager::NewL(*this, *(iAgnServer.FileMgr()));
    }

void CAgnServerSession::ServiceL(const RMessage2 &aMessage)
	{
	TBool complete = ETrue;
	TInt err = KErrNone;
	iMessage = aMessage;
	
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	if (iAgnServer.SystemStateObserver().IsShutdownInProgress())
		{
		//Shutdown in progress
		err = IsReadOnlyOperation();
		complete = ETrue;
		}
#endif
	if (err == KErrNone)
		{
	    TRAP(err,complete = DispatchMessageL());
	    _DBGLOG_IPC(AgmDebug::DebugLogIPCL(aMessage.Function(), TInt(this), err);)

	    if ( err != KErrNone )
	    	{
	    	if (err == KErrNotReady && 
	    			aMessage.Function() != ETransferAttachmentFileToServer &&
	    			aMessage.Function() != ETransferFileToClientToWrite &&
	    			aMessage.Function() != ETransferAttachmentFileToClient&&
	    			aMessage.Function() != EMoveFileToServer)
	    		{
	    		// If a function has failed with Not ready, then a drive must not be ready
	    		// If it is not an attachment failure, then it must have happened on the drive containing
	    		// the Calendar file. In this case, ensure the session always leaves with KErrNotReady
	    		// until the session is closed and re-opened.
	    		iSessionReady = EFalse;
	    		}
	    	delete iBuffer;
	    	iBuffer = NULL;
	    	}
		}
		
	if (complete && err != KClientHasBeenPanicked)
		{
		aMessage.Complete(err);
		}
	}

CAgnServerSession::~CAgnServerSession()
	{
	//This is required for robustness. Clients which are incorrectly
	//written can make requests with MCalProgressCallBack callbacks could
	//subsequently panic and cause the session to be destructed. 
	//The task which the client had initiated will be cancelled so it does not
	//attempt to use the session again with further progress information.
	
	CancelAllTask();
 
	// Cleanup any resources that have been allocated
	delete iBuffer;
	// Close the calendar file after a delay
	CloseAgendas();
	iAgnServer.SessionClosed();
	delete iAgnSessionFileManager;
	delete iInstanceIteratorMgr;
	}

/** 
* Check to see if an operation can be performed due to backup\restore process
* @return ETrue, if the operation can not be performed, EFalse otherwise. 
*/
TBool CAgnServerSession::CheckBackupRestore()
    {
    if(iBackupRestoreLock  
    //All opeerations apart from the following ones are not allowed when Backup\Restore is in progress. 
    && iMessage.Function() != ECloseAgenda
    && iMessage.Function() != ECancelChangeNotification
    && iMessage.Function() != EDisableChangeBroadcast
    && iMessage.Function() != ETransmitBuffer
    && iMessage.Function() != ERequestChangeNotificationParameters
    && iMessage.Function() != ERequestChangeNotification
    && iMessage.Function() != EEnableChangeBroadcast
    && iMessage.Function() != EGetChangesSinceLastNotification
    && iMessage.Function() != ERequestProgress)
        {
        return ETrue;
        }
        
  // Return false, if iBackupRestoreLock is false or operations are those above, CAgnServFile::IsLocked()
  // will determine whether or not an operation can be performed.
    return EFalse;
    }

TBool CAgnServerSession::DispatchMessageL()
	{
    TBool operationRequiresFileLock = EFalse;
    TBool callIsAsynchronous = ETrue;
 	if(CheckBackupRestore())
	    {
	    User::Leave(KErrLocked);
	    }

	switch (iMessage.Function())
		{
		case EOpenAgenda:
			{
			OpenAgendaL();
			iSessionReady = ETrue;
			}
			break;
			
		case ECloseAgenda:
			{
			// Close the calendar file after a delay
			CloseAgendaL();
			iSessionReady = ETrue;
			}
			break;
	
		case ERequestChangeNotificationParameters:
			{
			RequestChangeNotificationParametersL();
			}			
			break;
			
		case ERequestChangeNotification:
			{
			RequestChangeNotification();
			callIsAsynchronous = EFalse;
			}
			break;

		case ECancelChangeNotification:
			{
			CancelChangeNotification();
			}
			break;

		case EDisableChangeBroadcast:
			{
			DisableChangeBroadcastL();
			}
			break;

		case EEnableChangeBroadcast:
			{
			EnableChangeBroadcastL();
			}
			break;

		case EAgnHeapSizeCount:
			{
			TInt allocSize;
			User::Heap().AllocSize(allocSize);
			TPckg<TInt> HeapSize(allocSize);
			iMessage.WriteL(KSlot0, HeapSize);			
			}
			break;

		case EAgnResourceCount:
			{
			TPckg<TInt> HeapCells(User::Heap().Count());
			iMessage.WriteL(KSlot0, HeapCells);			
			}
			break;
			
		case EAgnSetHeapFailure:
			{
			User::__DbgSetAllocFail(RHeap::EUser,(RHeap::TAllocFail)iMessage.Int0(),iMessage.Int1());		
			}
			break;

		case ECreateAgendaFile:
			{
			CreateAgendaFileL();			
			}
  			break;
 
		case EGetListFileNames:
			{
			ListAgendaFilesL();
			}
			break;

		case EDeleteAgendaFile:
			{
	  		DeleteAgendaFileL();
			}
	  		break;

		case ECancelTask:
			{
			CancelTaskL();
			}
			break;

		case EAgendaFileExists:
			{
			AgendaFileExistsL();
			}
			break;

		case ETransmitBuffer:
			{
			TransmitBufferL();
			}
			break;

		case ESetUpdateAlarm:
			{
			SetUpdateAlarmL();
			}
			break;

		case ESetEnablePubSubNotification:
			{
			SetEnablePubSubNotificationL();
			}
			break;

		case ERequestProgress:
			{
			RequestProgressL();
			callIsAsynchronous = EFalse; 
			}
			break;
			
		case EGetChangesSinceLastNotification:
			{
			GetChangesSinceLastNotificationL();
			}
			break;
			
		case EStartBuildIndex:
			{
			//Asynchronous call
			StartBuildIndex();
			callIsAsynchronous = EFalse; 
			}
			break;
		case ETzDbChangedTime:
			{
			TzRulesLastModifiedDateTimeL();
			}
			break;
		case ERollback:
			{
			RollbackL();
			}
			break;
		case EGetFileChangesSinceLastNotification:
		    {
		    GetFileChangesSinceLastNotificationL();
		    }
		    break;
		default:
			{
			operationRequiresFileLock = ETrue;
			}
			break;
		}
		
	// Most commands are not permitted while the file is locked
    
    if ( operationRequiresFileLock )
	    {
    	if ( ! iSessionReady)
			{
			User::Leave(KErrNotReady);
			}
		else
			{
			switch ( iMessage.Function() )
				{
				case EFetchEntry:
					{
					FetchEntryL();
					}
					break;
				case EFetchEntryByUID:
					{
					FetchEntryByUIDL();
					}
					break;
				case EFetchSimpleEntry:
					{
					FetchSimpleEntryL();
					}
					break;
				case EFetchSimpleEntries:
					{
					FetchSimpleEntriesL();
					}
					break;
				case EUpdateEntry:
					{
					UpdateEntryL();
					}
					break;
				case EAddEntry:
					{
					AddEntryL();
					}
					break;
				case EDeleteEntry:
					{
					DeleteEntryL();
					}
					break;
				case EPreviousInstances:
					{
					PreviousInstancesL();
					}
					break;
				case ENextInstances:
					{
					NextInstancesL();
					}
					break;
				case ECreateEntryIterator:
					{
					CreateEntryIteratorL();
					}
					break;
				case EEntryIteratorNext:
					{
					EntryIteratorNextL();
					}
					break;
				case EEntryIteratorPosition:
					{
					EntryIteratorPositionL();
					}
					break;
				case ECommit:
					{
					CommitL();
					}
					break;
				case EGetEntryUidsSinceDate:
					{
					GetEntryUidsSinceDateL();
					}
					break;
				case ERestoreText:
					{
					RestoreTextL();
					}
					break;
				case ERestoreAlarmAction:
					{
					RestoreAlarmActionL();
					}
					break;
				case EGetCategoryListCount:
					{
					GetCategoryListCountL();
					}
					break;
				case EGetCategoryListItem:
					{
					GetCategoryListItemL();
					}
					break;
				case EAddCategoryToList:
					{
					AddCategoryToListL();
					}
					break;
				case ECategoryFilter:
					{
					FilterCategoryL();
					}
					break;
				case ETidyByDateReadParams:
					{
					TidyByDateReadParamsL();
					}
					break;
				case ETidyByDateStart:
					{
					TidyByDateStartL();
					callIsAsynchronous = EFalse; 
					}
					break;
				case ECategoryStart:
					{
					CategoryTaskStartL();
					}
					break;
				case ECategoryStartAsyn:
					{
					AsynchCategoryTaskStartL();
					callIsAsynchronous = EFalse;
					}
					break;
				case EInstanceIteratorCreate:
					{
					CreateInstanceIteratorL();
					}
					break;
				case EInstanceIteratorDestroy:
					{
					DestroyInstanceIteratorL();
					}
					break;
				case EInstanceIteratorNext:
					{
					InstanceIteratorNextL();
					}
					break;
				case EInstanceIteratorPrevious:
					{
					InstanceIteratorPreviousL();
					}
					break;
				case EInstanceIteratorCount:
					{
					InstanceIteratorCountL();
					}
					break;
				case EInstanceIteratorLocateIndex:
					{
					InstanceIteratorLocateIndexL();
					}
					break;
				
			// Group Scheduling
				case EGetChangesSinceLastNotification:
					{
					GetChangesSinceLastNotificationL();
					}
					break;
				case EFetchEntryByGuid:
					{
					FetchEntryByGuidL();
					}
					break;
				case EFetchSimpleEntriesByGuid:
					{
					FetchSimpleEntriesByGuidL();
					}
					break;
				case EDeleteEntriesByLocalUid:
					{
					DeleteEntriesByLocalUidL();
					}
					break;
				case EDeleteEntryByGuid:
					{
					DeleteEntryByGuidL();
					}
					break;
				case EFindInstances:
					{
					FindInstancesL();
					}
					break;
				case ETransferAttachmentFileToServer:
					{
					TransferAttachmentFileToServerL();
					}
					break;
				case ETransferAttachmentFileToClient:
					{
					TransferAttachmentFileToClientL();
					callIsAsynchronous = EFalse;
					}
					break;
				case ETransferFileToClientToWrite:
					{
					TransferFileToClientToWriteL();
					callIsAsynchronous = EFalse;
					}
					break;
				case EFetchSortedAttachments:
					{
					FetchSortedAttachmentsL();
					}
					break;
				case EEntriesWithAttachment:
					{
					EntriesWithAttachmentL();
					}
					break;
				case EFetchAttachmentById:
					{
					FetchAttachmentByIdL();
					}
					break;
				case EMoveFileToServer:
					{
					MoveFileToServerL();
					}
					break;
				case EFetchSimpleEntryByUID:
					{
					FetchSimpleEntryByUIDL();
					}
					break;
               case ESetCalendarInfo:
                    {
                    SetCalendarInfoL();
                    }
                    break;
               case EGetCalendarInfo:
                    {
                    GetCalendarInfoL();
                    }
                    break;
               case EGetPropertyValue:
                    {
                    GetPropertyValueL();
                    }
                    break;
				default:
					PanicClientL(EBadRequest);
					return ETrue;
				}
			}
		}
	
	// If the buffer size is greater than KInitialBufferSize, then the buffer cannot be 
	// transmitted now. A large enough buffer must first be created on the client side to 
	// hold this.
	if ( iBuffer && iBuffer->Size() <= KInitialBufferSize && 
		 iBuffer->Size() > 0 )
		{
		TransmitBufferL();
		}

	return callIsAsynchronous;	// complete normally
	}

void CAgnServerSession::SetUpdateAlarmL()
	{
	TBool updateAlarm = iMessage.Int0();
	CAgnServFile* file = GetFileL(KSlot1);
	file->Model()->SetUpdateAlarmL(updateAlarm);
	}

void CAgnServerSession::SetEnablePubSubNotificationL()
	{
	TBool enablePubsub = iMessage.Int0();
	TPckgBuf<TInt64> fileId;
	iMessage.ReadL(KSlot1, fileId);
	
	if(iAgnSessionFileManager)
		{
		CAgnSessionFile* client = NULL;
		TRAPD(err, client = &(iAgnSessionFileManager->GetSessionFileL(fileId())));
		if(err == KErrNone && client)
		    {
		    client->SetEnablePubSubNotificationL(enablePubsub);
		    }
		else if (err != KErrNotFound)
		    {
		    User::Leave(err);
		    }		    
		}
	}

void CAgnServerSession::PanicClientL(TInt aPanic) const
	{
	_LIT(KAgendaServerPanic,"AgnServer");
  	iMessage.Panic(KAgendaServerPanic,aPanic);
	User::Leave(KClientHasBeenPanicked);
	}

//
// Utility Functions:

HBufC* CAgnServerSession::ReadClientDesLC(TInt aDesNum)
	{
	const TInt KDesLength = iMessage.GetDesLength(aDesNum);
	if (KDesLength == KErrArgument)
		{
        PanicClientL(EIndexError);
		}
	else if (KDesLength == KErrBadDescriptor)
		{
        PanicClientL(EBadDescriptor);
		}
	User::LeaveIfError(KDesLength);

	HBufC* readBuffer = HBufC::NewLC(KDesLength);
	TPtr readPtr(readBuffer->Des());

	TRAPD(ret, iMessage.ReadL(aDesNum, readPtr));
	if (ret != KErrNone)
		{
		PanicClientL(EBadDescriptor);
		}

	return readBuffer;
	}


HBufC8* CAgnServerSession::ReadClientDes8LC(TInt aDesNum)
	{
	const TInt KDesLength = iMessage.GetDesLength(aDesNum);
	if (KDesLength == KErrArgument)
		{
        PanicClientL(EIndexError);
		}
	else if (KDesLength == KErrBadDescriptor)
		{
        PanicClientL(EBadDescriptor);
		}
	User::LeaveIfError(KDesLength);
	
	HBufC8* readBuffer = HBufC8::NewLC(KDesLength);
	TPtr8 readPtr(readBuffer->Des());

	TRAPD(ret, iMessage.ReadL(aDesNum, readPtr));
	if (ret != KErrNone)
		{
		PanicClientL(EBadDescriptor);
		}

	return readBuffer;
	}	

//
//Server Functions:
void CAgnServerSession::OpenAgendaL()
	{
	// Open a new file for this session
	// Arguments:   0: Input - buffer contains name of file and session ID
	//              1: Output - status
	//              2: Output - file ID
	//              3: Output - collection ID
	//

	// Get descriptor for filename to open
	const TInt KBufferSize = iMessage.GetDesLengthL(KSlot0);
	CBufFlat* buffer = CBufFlat::NewL(KBufferSize);
	CleanupStack::PushL(buffer);
	buffer->ExpandL(0, KBufferSize);
	TPtr8 des(buffer->Ptr(0));
	iMessage.ReadL(KSlot0, des);
	RDesReadStream readStream(des);
	HBufC* fileName = HBufC::NewLC(readStream, KMaxTInt);
	TInt sessionId = readStream.ReadInt32L();	
	CalCommon::TCalFileVersionSupport status;

	HBufC* nameWithPath = iAgnServer.FileMgr()->ParseFilenameLC(*fileName);
	CAgnServFile* file = iAgnServer.FileMgr()->GetFile(*nameWithPath);
	TInt err = KErrNone;
	if(file)
		{
		if(IsOwnFileL(sessionId, *file))
			{//If this file has been opened by same CCalSession object
			User::Leave(KErrAlreadyExists);
			}
		else if (FileHasBeenOpenedL(*file))
			{//If this file has been opened by a different CCalSession object
			User::Leave(KErrArgument);
			} 
		}
	CleanupStack::PopAndDestroy(nameWithPath);
	CloseAgendaL(sessionId);//Check if this CCalSession object has had opened a different file, if so, close it.      
	TRAP(err, file = &iAgnServer.FileMgr()->OpenAgendaL(*fileName, iAgnServer, status));
	CleanupStack::PopAndDestroy(fileName);
	CleanupStack::PopAndDestroy(buffer);
	User::LeaveIfError(err);
	CleanupStack::PushL(TCleanupItem(CleanupCloseAgendaImmediately, file));
	TPckg<TInt> theStatus(status);
	iMessage.WriteL(KSlot1, theStatus);

	__ASSERT_ALWAYS(file, User::Leave(KErrCorrupt));
	TInt64 fileId = file->Model()->GetFileIdL();
	TCalCollectionId shortFileId = file->CollectionId();
	TPckg<TInt64> fileIdPackage(fileId);
	iMessage.WriteL(KSlot2, fileIdPackage);
	TPckg<TUint8> shortFileIdPackage(shortFileId);
	iMessage.WriteL(KSlot3, shortFileIdPackage);
	file->Model()->CategoryIndex().SetCategoryList(&(file->CategoryList()));
	if(iAgnSessionFileManager->AddClientL(fileId, sessionId))
		{
		file->Model()->CheckTzDbModificationL();
		}

	// This is the cleanup item to close agenda if anything fails
	CleanupStack::Pop(); 
	}

void CAgnServerSession::BackupReStoreChangedL(TInt64 aOldFileId, const CAgnServFile& aServFile, MCalChangeCallBack2::TChangeType aChangeType)
	{
	if(iAgnSessionFileManager)
		{
		TAgnChange change;
		change.iOperationType = aChangeType;
		
		//reset model and file id if restore ends
		if(aChangeType == MCalChangeCallBack2::ERestoreEnd)
			{
			iAgnSessionFileManager->ReSetModel(aOldFileId, aServFile);
			change.iFileId = aServFile.Model()->GetFileIdL();
			}
		else
			{
			change.iFileId = aOldFileId;        
			}
		
		iAgnSessionFileManager->AddChangeL(change);
		}

	if(aChangeType == MCalChangeCallBack2::EBackupStart || aChangeType == MCalChangeCallBack2::ERestoreStart)
	   {
	   LockClient(ETrue);
	   }
	else
	   {
	   LockClient(EFalse);
	   }
	}

void CAgnServerSession::LockClient(TBool aLock)
	{
	iBackupRestoreLock = aLock;
	}

/** 
* Closes the calendar file in the current agenda server session immediately or after a delay.
* The delay is triggered using a timer owned by the calendar file. If a session then tries to access the same calendar file, the delay is cancelled and the file is not closed
* @param aCloseImmediately EFalse if a delay is required before the closure of the calendar file, ETrue if the calendar file is to be closed immediately
*/
void CAgnServerSession::CloseAgendaL()
	{
	TInt sessionId = iMessage.Int0();
	CloseAgendaL(sessionId);
	}

void CAgnServerSession::CloseAgendaL(TInt aSessionId)
	{
	if(iAgnSessionFileManager)
		{
		TInt index = iAgnSessionFileManager->GetSessionFile(aSessionId);
		CAgnServFile* file = NULL;
		if(index >= 0)
			{
			TRAPD(err, file = iAgnServer.FileMgr()->GetFileL(iAgnSessionFileManager->GetSessionFileByIndex(index).FileId()));           
			if(file && err == KErrNone)
				{
				User::LeaveIfError(iAgnServer.FileMgr()->CloseAgenda(*file, EFalse));
				}       
			iAgnSessionFileManager->DeleteSessionFile(aSessionId);           
			}
		}
	}
void CAgnServerSession::FetchEntryL()
	{
	// Fetch an entry from the agenda file
	// Arguments: 0 : Output - Data Buffer
	//			  1 : Input  - Entry Id to fetch
	//			  2 : Output - Size of buffer
	//			  3 : Input  - File Id
	TPckgBuf<TAgnEntryId> entryId;
	iMessage.ReadL(KSlot1, entryId);
	
	CAgnServFile* file = GetFileL(KSlot3);
	if(file->IsLocked())
		{
		User::Leave(KErrLocked);
		}
	CAgnEntry* entry = file->Model()->FetchEntryL(entryId());
	
	if ( entry )
		{
		CleanupStack::PushL(entry);
	
		ExternalizeEntryToClientL(*entry, KSlot2);
	
		CleanupStack::PopAndDestroy(entry);
		}
	else
		{
		TPckg<TInt> size(0);
		iMessage.WriteL(KSlot2, size);
		}	
	}


void CAgnServerSession::FetchEntryByUIDL()
	{
	// Fetch an entry from the agenda file
	// Arguments: 0 : Output - Data Buffer
	//			  1 : Input  - Unique Id to fetch
	//			  2 : Output - Size of buffer
	//			  3 : Input  - File ID

	TPckgBuf<TCalLocalUid> uniqueId;
	iMessage.ReadL(KSlot1, uniqueId);
	
	CAgnServFile* file = GetFileL(KSlot3);
	if(file->IsLocked())
		{
		User::Leave(KErrLocked);
		}
	CAgnEntry* entry = file->Model()->FetchEntryL(uniqueId());
	
	if ( entry )
		{
		CleanupStack::PushL(entry);
	
		ExternalizeEntryToClientL(*entry, KSlot2);
	
		CleanupStack::PopAndDestroy(entry);
		}
	else
		{
		TPckg<TInt> size(0);
		iMessage.WriteL(KSlot2, size);
		}	
	}


void CAgnServerSession::ExternalizeEntryToClientL(const CAgnEntry& aEntry, TInt aSlot)
	{
	delete iBuffer;
	iBuffer = NULL;
		
	iBuffer = CBufFlat::NewL(KInitialBufferSize);

	// Create a write stream for this buffer
	RBufWriteStream bufStream;
	bufStream.Open(*iBuffer);
	CleanupClosePushL(bufStream);	

	bufStream.WriteUint32L( aEntry.Type() );
	aEntry.ExternalizeToBufferL(bufStream);

	bufStream.CommitL();

	CleanupStack::PopAndDestroy(&bufStream); // bufStream.Close();

	// Send it back to the client
	TPckg<TInt> size(iBuffer->Size());
	iMessage.WriteL(aSlot, size);	
	}


void CAgnServerSession::TransmitBufferL()
	{
	// Transmit the contents of the buffer stream to the client,
	// then delete it
	// Arguments: 0 : Output - Data Buffer

	if (!iBuffer)
		{
		PanicClientL(EBadRequest);	
		}
	iMessage.WriteL(KSlot0, iBuffer->Ptr(0));

	delete iBuffer;
	iBuffer = NULL;
	}


void CAgnServerSession::FetchSimpleEntryL()
	{
	// Fetch a lite-entry from the server
	// (A lite entry consists of time information without any text)
	// Arguments: 0 : Output - Data buffer
	//			  1 : Input  - TAgnInstace
	//			  2 : Output - Size of buffer
	const TInt KBufferSize = iMessage.GetDesLengthL(KSlot1);
	CBufFlat* buffer = CBufFlat::NewL(KBufferSize);
	CleanupStack::PushL(buffer);
	buffer->ExpandL(0, KBufferSize);
	TPtr8 des(buffer->Ptr(0));
	iMessage.ReadL(KSlot1, des);
	RDesReadStream readStream(des);
	TAgnEntryId entryId;
	readStream >> entryId;
	CleanupStack::PopAndDestroy(buffer);

	// Make sure this entry hasn't been deleted or updated		TAgnInstanceId entryId = (*entryIds)[ii].iId;
	TCalCollectionId collectionId = iMessage.Int3();
	CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(collectionId);
	if(file->IsLocked())
		{
		User::Leave(KErrLocked);
		}

	const CAgnSimpleEntry* KEntry = file->Model()->GetSimpleEntryFromIndexes(entryId);

	if ( KEntry )
		{
		delete iBuffer;
		iBuffer = NULL;

		iBuffer = CBufFlat::NewL(KInitialBufferSize);

		// Create a write stream for this buffer
		RBufWriteStream bufStream;
		bufStream.Open(*iBuffer);
		CleanupClosePushL(bufStream);	

		// Write a bool to indicate if entry is available
		bufStream.WriteUint8L(KEntry != NULL);

		bufStream.WriteUint32L(KEntry->Type());
		KEntry->ExternalizeL(bufStream, ETrue);

		bufStream.CommitL();

		CleanupStack::PopAndDestroy(&bufStream); // bufStream.Close();

		// Send it back to the client
		TPckg<TInt> size(iBuffer->Size());
		iMessage.WriteL(KSlot2, size);
		}
	else
		{
		TPckg<TInt> emptyBuffer(0);
		iMessage.WriteL(KSlot2, emptyBuffer);
		}
	}

void CAgnServerSession::FetchSimpleEntryByUIDL()
	{
	// Fetch a lite-entry from the server
	// (A lite entry is consists of time information without any text)
	// Arguments: 0 : Output - Data buffer
	//			  1 : Input  - TAgnInstace
	//			  2 : Output - Size of buffer
	const TInt KBufferSize = iMessage.GetDesLengthL(KSlot1);
	CBufFlat* buffer = CBufFlat::NewL(KBufferSize);
	CleanupStack::PushL(buffer);
	buffer->ExpandL(0, KBufferSize);
	TPtr8 des(buffer->Ptr(0));
	iMessage.ReadL(KSlot1, des);
	RDesReadStream readStream(des);	
	TUint32 entryId = readStream.ReadUint32L();
	CleanupStack::PopAndDestroy(buffer);

	TCalCollectionId collectionId = iMessage.Int3();
	CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(collectionId );
	if(file->IsLocked())
		{
		User::Leave(KErrLocked);
		}
	// Make sure this entry hasn't been deleted or updated
	const CAgnSimpleEntry* KEntry = file->Model()->GetSimpleEntryFromIndexes(entryId);

	if ( KEntry )
		{
		delete iBuffer;
		iBuffer = NULL;

		iBuffer = CBufFlat::NewL(KInitialBufferSize);

		// Create a write stream for this buffer
		RBufWriteStream bufStream;
		bufStream.Open(*iBuffer);
		CleanupClosePushL(bufStream);	

		// Write a bool to indicate if entry is available
		bufStream.WriteUint8L(KEntry != NULL);

		bufStream.WriteUint32L(KEntry->Type());
		KEntry->ExternalizeL(bufStream, ETrue);
		bufStream.CommitL();

		CleanupStack::PopAndDestroy(&bufStream); // bufStream.Close();

		// Send it back to the client
		TPckg<TInt> size(iBuffer->Size());
		iMessage.WriteL(KSlot2, size);
		}
	else
		{
		TPckg<TInt> emptyBuffer(0);
		iMessage.WriteL(KSlot2, emptyBuffer);
		}
	}

void CAgnServerSession::EnsureStringsLoadedL(CAgnEntry& aEntry, TUint8 aShortFileId)
	{
	CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(aShortFileId);
	if (!aEntry.SummaryIsLoaded())
		{
		HBufC* summary = file->Model()->RestoreTextL(aEntry.SummaryStreamId());
		aEntry.SetSummary(summary);
		}

	if (!aEntry.DescriptionIsLoaded())
		{
		HBufC* description = file->Model()->RestoreTextL(aEntry.DescriptionStreamId());
		aEntry.SetDescription(description);
		}
	}
		
void CAgnServerSession::FetchSimpleEntriesL()
	{
	// Fetch an array of lite-entries from the server
	// (A lite entry consists of time information without any text)
	// Arguments: 0 : Output - Data buffer
	//			  1 : Input  - TArray<TAgnEntryId> to fetch
	//			  2 : Output - Size of buffer

	CArrayFixSeg<TAgnInstance>* instanceIds = new (ELeave) CArrayFixSeg<TAgnInstance>(8);
	CleanupStack::PushL(instanceIds);
	
	const TInt KDesLength = iMessage.GetDesLength(KSlot1);
	if(KDesLength<0)
		{
		User::Leave(KErrArgument);
		}

	CBufFlat* buffer = CBufFlat::NewL(KDesLength);
	CleanupStack::PushL(buffer);
	
	buffer->ExpandL(0, KDesLength);
	TPtr8 des(buffer->Ptr(0));
	iMessage.ReadL(KSlot1, des);

	RBufReadStream readStream;
	readStream.Open(*buffer);
	CleanupClosePushL(readStream);
	
	const TInt KIdCount1 = readStream.ReadUint32L();
	
	for ( TInt i = 0; i < KIdCount1; ++i )
		{
		TAgnInstance instance;
		instance.InternalizeL(readStream);
		instanceIds->AppendL(instance);
		}
		
	CleanupStack::PopAndDestroy(&readStream);	// readStream.Close();

	CleanupStack::PopAndDestroy(buffer);		// buffer
	
	delete iBuffer;
	iBuffer = NULL;

	iBuffer = CBufFlat::NewL(KInitialBufferSize);
	
	// Create a write stream for this buffer
	RBufWriteStream bufStream;
	bufStream.Open(*iBuffer);
	CleanupClosePushL(bufStream);	
	
	const TInt KIdCount2 = instanceIds->Count();
	
	bufStream.WriteUint8L(KIdCount2);
	
	for ( TInt ii = 0; ii < KIdCount2 ; ++ii )
		{
#if defined (__CAL_VERBOSE_LOGGING__) || defined (__CAL_ENTRY_LOGGING__)
		CAgnServFile* agnFile = iAgnServer.FileMgr()->GetFile((*instanceIds)[ii].iCollectionId);
		CAgnEntry* fullEntry = agnFile->Model()->FetchEntryL((*instanceIds)[ii]);
		CleanupStack::PushL(fullEntry);
		AgmDebug::DebugLog("FetchSimpleEntriesL - Fetch entry with Id %d", (*instanceIds)[ii].Value());
		EnsureStringsLoadedL(*fullEntry, *entryIds[ii].iCollectionId);
		AgmDebug::DebugLogEntryL(*fullEntry, EDumpEntryAll);
		CleanupStack::PopAndDestroy(fullEntry);
#endif
		TAgnInstanceId entryId = (*instanceIds)[ii].iId;
		CAgnServFile* file = iAgnServer.FileMgr()->GetFileL((*instanceIds)[ii].iCollectionId);
		if(file->IsLocked())
			{
			User::Leave(KErrLocked);
			}
		const CAgnSimpleEntry* KEntry = file->Model()->GetSimpleEntryFromIndexes(entryId);
		
		// Write a bool to indicate if entry is available
		bufStream.WriteUint8L(KEntry != NULL);
		
		if ( KEntry )
			{
			bufStream.WriteUint32L(KEntry->Type());
			KEntry->ExternalizeL(bufStream, ETrue);
			bufStream.WriteUint8L((*instanceIds)[ii].iCollectionId);
			}
		}

	bufStream.CommitL();
	
	CleanupStack::PopAndDestroy(&bufStream); // bufStream.Close();
	
	CleanupStack::PopAndDestroy(instanceIds);

	// Send it back to the client
	TPckg<TInt> size(iBuffer->Size());
	iMessage.WriteL(KSlot2, size);
	}

void CAgnServerSession::UpdateEntryL()
	{
	// Updates an entry on the server
	// Arguments: 0 : Input  - Size of buffer
	//            1 : Input  - Data Buffer (containing entry data)
	//		      2 : Output - Entry Id of updated entry
	//			  3 : File ID	
	
	// Restore length
	const TInt KBufferSize = iMessage.Int0();
	
	// Restore buffer
	CBufFlat* buffer = CBufFlat::NewL(KBufferSize);
	CleanupStack::PushL(buffer);

	buffer->ExpandL(0, KBufferSize);
	TPtr8 des(buffer->Ptr(0));
	iMessage.ReadL(KSlot1, des);

	// Internalize the data from the stream
	RBufReadStream readStream;
	readStream.Open(*buffer);
	CleanupClosePushL(readStream);

	const CCalEntry::TType KEntryType = static_cast<CCalEntry::TType>(readStream.ReadUint32L());

	TBool deleteChildren = readStream.ReadUint8L();

	CAgnEntry* entry = CAgnEntry::NewL(KEntryType);
	CleanupStack::PushL(entry);

	entry->InternalizeFromBufferL(readStream);

	_DBGLOG_ENTRY(AgmDebug::DebugLog("Updating calendar file with entry");)
	_DBGLOG_ENTRY(AgmDebug::DebugLogEntryL(*entry, EDumpEntryAll);)

	TCalCollectionId collectionId = iMessage.Int3();
	CAgnSessionFile& client = iAgnSessionFileManager->GetSessionFileL(collectionId);
	client.UpdateEntryL(*entry, deleteChildren);
	_DBGLOG_ENTRY(AgmDebug::DebugLog("Entry: Updated entry on file. Entry ID: %d",entry->EntryId().Value());)

	// Return entry id
	TPckg<TAgnEntryId> entryPckg(entry->EntryId());
	iMessage.WriteL(KSlot2, entryPckg);

	CleanupStack::PopAndDestroy(entry);
	CleanupStack::PopAndDestroy(&readStream); // readStream.Close();	
	CleanupStack::PopAndDestroy(buffer);
	}

void CAgnServerSession::AddChangeL(const TAgnChange& aChange)
    {
    if(iAgnSessionFileManager)
        {
        iAgnSessionFileManager->AddChangeL(aChange);
        }
 	}

void CAgnServerSession::AddEntryL()
	{
	// Adds an entry to the server
	// Arguments: 0 : Input  - Size of buffer
	//            1 : Input  - Data Buffer (containing entry data)
	//		      2 : Output - Ids of updated entry
	//            3 : Input  - File Id
		

	// Restore length
	const TInt KBufferSize = iMessage.Int0();
	
	// Restore buffer
	CBufFlat* buffer = CBufFlat::NewL(KBufferSize);
	CleanupStack::PushL(buffer);

	buffer->ExpandL(0, KBufferSize);
	TPtr8 des(buffer->Ptr(0));
	iMessage.ReadL(KSlot1, des);

	// Internalize the data from the stream
	RBufReadStream readStream;
	readStream.Open(*buffer);
	CleanupClosePushL(readStream);

	const CCalEntry::TType KEntryType = (CCalEntry::TType) readStream.ReadUint32L();
	CAgnEntry* entry = CAgnEntry::NewL(KEntryType);
	CleanupStack::PushL(entry);

	entry->InternalizeFromBufferL(readStream);

	TAgnEntryParameters params;
	TPckgBuf<TInt64> fileId;
	iMessage.ReadL(KSlot3, fileId);
	CAgnSessionFile& client = iAgnSessionFileManager->GetSessionFileL(fileId());
	params.iEntryId = client.StoreEntryL(*entry);
	params.iLocalId = entry->LocalUid();
	params.iRecurrenceId = entry->RecurrenceId().LocalL();
	params.iLastModifiedDateUtc = entry->LastModifiedDateUtc();
	params.iRecurrenceRange = entry->RecurrenceRange();

	if (entry->TimeMode() != MAgnCalendarTimeMode::EFloating &&	entry->RptDef() != NULL )
		{
		CAgnTzRules* tzRules = entry->RptDef()->AgnTzRules();
		if(tzRules != NULL)
			{
			params.iTzStreamId = (tzRules->TzZoneStreamId());
			params.iSystemTzRule = (tzRules->SystemTzRule());
			}
		}
	
	// Set entry Id and unique Id
	TPckg<TAgnEntryParameters> paramsPckg(params);
 	iMessage.WriteL(KSlot2, paramsPckg);

	CleanupStack::PopAndDestroy(entry);			// entry
	CleanupStack::PopAndDestroy(&readStream);	// readStream.Close();
	CleanupStack::PopAndDestroy(buffer);		// buffer
	}

/**
Called when a new attachment is stored with a file handle.
The file handle is transferred to the server side and the file is moved to the Calendar store.
*/
void CAgnServerSession::TransferAttachmentFileToServerL()
	{
	// Arguments: 0 - Output	: Attachment Data - local UID of entry and index of attachment on that entry
	//			  1 - Input		: Attachment UID of new attachment
	//			  2,3 - FS		: Used by file server to transfer file from client to agenda server
	
	RFile file;
	CleanupClosePushL(file);
	
	User::LeaveIfError(file.AdoptFromClient(iMessage, KTransferAttachmentToSrvFsArgIndex, KTransferAttachmentToSrvFileArgIndex));
	
	const TInt KBufferSize = iMessage.GetDesLengthL(KSlot0);
	CBufFlat* buffer = CBufFlat::NewL(KBufferSize);
	CleanupStack::PushL(buffer);
	buffer->ExpandL(0, KBufferSize);

	TPtr8 des(buffer->Ptr(0));
	iMessage.ReadL(KSlot0, des);
	
	RDesReadStream readStream(des);
	TCalLocalUid localUid = readStream.ReadUint32L();
	TInt attachmentIndex = readStream.ReadInt16L();
	TInt64 fileId;
	readStream >> fileId;	
	
	CAgnServFile* agnFile = iAgnServer.FileMgr()->GetFileL(fileId);
	TPckg<TCalAttachmentUid> attachUid = agnFile->Model()->TransferAttachmentFileToServerL(file, localUid, attachmentIndex);
	iMessage.WriteL(1, attachUid);
	CleanupStack::PopAndDestroy();
	CleanupStack::PopAndDestroy(&file); // file.Close()
	}
	
void CAgnServerSession::MoveFileToServerL()
	{
	const TInt KBufferSize = iMessage.GetDesLengthL(KSlot0);
	CBufFlat* buffer = CBufFlat::NewL(KBufferSize);
	CleanupStack::PushL(buffer);
	buffer->ExpandL(0, KBufferSize);

	TPtr8 des(buffer->Ptr(0));
	iMessage.ReadL(KSlot0, des);
	
	RDesReadStream readStream(des);
	TCalLocalUid localUid = readStream.ReadUint32L();
	TInt attachmentIndex = readStream.ReadInt16L();
	TInt64 fileId;
	readStream >> fileId;	
	
	CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(fileId);
	TPckg<TCalAttachmentUid> attachUid = file->Model()->MoveFileToServerL(localUid, attachmentIndex);
	iMessage.WriteL(1, attachUid);
	CleanupStack::PopAndDestroy();
	}
/**
Called when a file attachment is fetched from the client.
The file handle is opened on the server side then transferred to the client.
*/
void CAgnServerSession::TransferAttachmentFileToClientL()
	{
	// Arguments: 0 - Output	: Attachment UID of file attachment to send to client
	//			  1 - FS		: Used by file server to transfer file from agenda server to client
	//			  2 - File Id   : Long file id of the calendar file
	TCalAttachmentUid attachmentUid = iMessage.Int0();

	RFile file;
	CleanupClosePushL(file);
	
	TPckgBuf<TInt64> fileId;
	iMessage.ReadL(KSlot2, fileId);
	CAgnServFile* agnServfile = iAgnServer.FileMgr()->GetFileL(fileId());
	agnServfile->Model()->OpenAttachmentFileL(file, attachmentUid);
	
	TPckg<TInt> pckgSubSessionHandle(file.SubSessionHandle()); 
	iMessage.WriteL(KTransferAttachmentToCliFileArgIndex, pckgSubSessionHandle);
	
	User::LeaveIfError(file.TransferToClient(iMessage, KTransferAttachmentToCliFileArgIndex));
	
	CleanupStack::PopAndDestroy(&file); // file.Close()
	}

/**
Called when a new attachment is stored with binary data.
The new file handle is opened on the server side, then transferred to the client side where the data is written directly onto the file.
This means that the binary data is not passed directly over IPC.
*/
void CAgnServerSession::TransferFileToClientToWriteL()
	{
	// Arguments: 0 - Output	: Attachment data of new file attachment to be created
	//			  1 - FS		: Used by file server to transfer new file from agenda server to client
	//			  2 - Input     : File Id
	// 			  3 - Output	: Size of attachment data that will be written to file handle on client side
	TAttachmentData data;
	TPckg<TAttachmentData> pckgData(data);
	iMessage.ReadL(0, pckgData);
	TInt dataSize = iMessage.Int3();
	
	RFile file;
	CleanupClosePushL(file);
	
	TPckgBuf<TInt64> fileId;
	iMessage.ReadL(KSlot2, fileId);
	CAgnServFile* agnServFile = iAgnServer.FileMgr()->GetFileL(fileId());
	HBufC* fileName = agnServFile->Model()->GenerateRandomFilenameLC(data.iDrive);
	agnServFile->Model()->CreateNewFileL(file, *fileName);
	agnServFile->Model()->UpdateAttachmentDetailsL(data.iLocalUid, data.iAttachmentIndex, *fileName, dataSize);
	CleanupStack::PopAndDestroy(fileName);
	
	TPckg<TInt> pckgSubSessionHandle(file.SubSessionHandle()); 
	iMessage.WriteL(KTransferAttachmentToCliFileArgIndex, pckgSubSessionHandle);
	User::LeaveIfError(file.TransferToClient(iMessage, KTransferAttachmentToCliFileArgIndex));
	
	CleanupStack::PopAndDestroy(&file); // file.Close()
	}

/**
Fetches attachment UIDs of all file attachments in the order specified in CCalAttachmentManager::FetchAttachmentsL.
*/
void CAgnServerSession::FetchSortedAttachmentsL()
	{
	// Arguments: 0 - Output	: Data buffer (containing list of TCalLocalUid's of entries)
	//			  1 - Output	: Size of data buffer
	//			  2 - Input		: Sort order
	//			  3 - Input     : File Id
	
	TUint32 sortOrder = iMessage.Int2();

	RArray<TCalAttachmentUid> attachmentIds;
	CleanupClosePushL(attachmentIds);
	
	TPckgBuf<TInt64> fileId;
	iMessage.ReadL(KSlot3, fileId);
	CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(fileId());
	if(file->IsLocked())
		{
		User::Leave(KErrLocked);
		}

	file->Model()->GetSortedAttachmentsL(attachmentIds, static_cast<CCalAttachmentManager::TSortOrder>(sortOrder));
	
	WriteIntArrayToBufferL(attachmentIds);

	CleanupStack::PopAndDestroy(&attachmentIds); // attachmentIds->Close()

	// Send it back to the client
	TPckg<TInt> listsize(iBuffer->Size());
	iMessage.WriteL(KSlot1, listsize);
	}

void CAgnServerSession::WriteIntArrayToBufferL(const RArray<TUint32>& aArray)
	{
	// Create a write stream for this buffer
	const TInt KCount = aArray.Count();
	
	// Buffer contains enough space for KCount integers and a count
	const TInt KSizeOfBuffer = (KCount + 1) * sizeof(TUint32);
	
	delete iBuffer;
	iBuffer = NULL;
	
	iBuffer = CBufFlat::NewL(KSizeOfBuffer);
	RBufWriteStream bufStream;
	bufStream.Open(*iBuffer);
	CleanupClosePushL(bufStream);	

	// Externalize the list 
	bufStream.WriteUint32L(KCount);

	for ( TInt ii = 0; ii < KCount; ++ii )
		{
		bufStream << aArray[ii];
		}

	bufStream.CommitL();
	
	CleanupStack::PopAndDestroy(&bufStream); // bufStream.Close();
	}

/**
Fetches all file attachments with the attachment specified by the UID. 
This is called from CCalAttachmentManager::EntriesReferencingFileAttachmentL.
*/
void CAgnServerSession::EntriesWithAttachmentL()
	{
	// Arguments: 0 - Output	: Data buffer (containing list of TCalLocalUid's of entries)
	//			  1 - Output	: Size of data buffer
	//			  2 - Input		: attachment Uid
	//			  3 - Input     : File Id
	TCalAttachmentUid attachUid = iMessage.Int2();
	TPckgBuf<TInt64> fileId;
	iMessage.ReadL(KSlot3, fileId);
	CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(fileId());
	if(file->IsLocked())
		{
		User::Leave(KErrLocked);
		}

	const RArray<TCalLocalUid>* KUniqueIdList = file->Model()->GetEntriesWithAttachment(attachUid);

	WriteIntArrayToBufferL(*KUniqueIdList);
	
	// Send it back to the client
	TPckg<TInt> listsize(iBuffer->Size());
	iMessage.WriteL(KSlot1, listsize);
	}

/**
Fetches a file attachment by its UID.
Attachment UIDs are not exposed to the user. This function is used in the CCalAttachmentIterator.
*/
void CAgnServerSession::FetchAttachmentByIdL()
	{
	// Get a list of file names held by the server 
	// currently setup extractor
	// Arguments: 0 : Output - Data buffer 
	//			   1 : Output - Size of buffer
	//			   2 : Input - attachment uid
	//			   3 : Input - File Id

	TCalAttachmentUid attachUid = iMessage.Int2();
	TPckgBuf<TInt64> fileId;
	iMessage.ReadL(KSlot3, fileId);
	CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(fileId());
	if(file->IsLocked())
		{
		User::Leave(KErrLocked);
		}

	CAgnAttachment* attachmentToWrite = file->Model()->FetchAttachmentByIdL(attachUid);

	if ( attachmentToWrite )
		{
		CleanupStack::PushL(attachmentToWrite);

		delete iBuffer;
		iBuffer = NULL;

		iBuffer = CBufFlat::NewL(KInitialBufferSize);

		// Create a write stream for this buffer
		RBufWriteStream bufStream;
		bufStream.Open(*iBuffer);
		CleanupClosePushL(bufStream);
	
		attachmentToWrite->ExternalizeL(bufStream);
			
		bufStream.CommitL();

		CleanupStack::PopAndDestroy(&bufStream); // bufStream.Close();
		CleanupStack::PopAndDestroy(attachmentToWrite);

		// Send it back to the client
		TPckg<TInt> size(iBuffer->Size());
		iMessage.WriteL(KSlot1, size);
		}
	else
		{
		TPckg<TInt> emptyBuffer(0);
		iMessage.WriteL(KSlot1, emptyBuffer);
		}
	}

void CAgnServerSession::PreviousInstancesL()
	{
	// Gets the time of the previous instance to today
	// Arguments: 0 : output data buffer
	//			  1 : size of output data buffer
	//			  2 : input data buffer
	//			  3 : size of input data buffer

	// Restore length
	const TInt KBufferSize = iMessage.Int3();
	
	const TInt KInstanceArrayGranularity = 16;
	CArrayFixSeg<TAgnSortInstance>* instances = new (ELeave) CArrayFixSeg<TAgnSortInstance>(KInstanceArrayGranularity);
	CleanupStack::PushL(instances);

	// Restore buffer
	CBufFlat* buffer = CBufFlat::NewL(KBufferSize);
	CleanupStack::PushL(buffer);
	buffer->ExpandL(0, KBufferSize);

	TPtr8 des(buffer->Ptr(0));
	iMessage.ReadL(KSlot2, des);
	
	TFindInstanceParams parameters;
	
	RDesReadStream readStream(des);
	
	parameters.InternalizeL(readStream);

	TInt filecount = readStream.ReadInt16L();
	for(TInt ii=0;ii<filecount;++ii)
		{
		TInt64 fileId;
		readStream >> fileId;
		CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(fileId);
		if(file->IsLocked())
			{
			User::Leave(KErrLocked);
			}
		CAgnEntryModel* model = file->Model();
		model->PreviousPossibleInstancesL(*instances, parameters);
		}

	CleanupStack::PopAndDestroy(buffer);

	const CAgnSimpleEntry* searchFromEntry = NULL;
	if(parameters.iInstance.iCollectionId > 0)
		{//If the iInstance.iFileId ==0, instances should be found from the start date of the searching range so there is no searchFromEntry
		CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(parameters.iInstance.iCollectionId);
		if(file->IsLocked())
			{
			User::Leave(KErrLocked);
			}
		searchFromEntry = file->Model()->GetSimpleEntryFromIndexes(parameters.iInstance.iId);
		}
	
	TAgnSortInstance searchFromInstance(*searchFromEntry);
	
	if (searchFromEntry)
		{
		TAgnDaySortKey sortKey(AgnDateTime::MaxDate(), parameters.iUndatedTodoTimeLocal);
		searchFromInstance.SetL(parameters.iInstance.iId.Date().LocalL(), parameters.iUndatedTodoTimeLocal);
		const_cast<CAgnSimpleEntry&>(searchFromInstance.SimpleEntry()).SetCollectionId(parameters.iInstance.iCollectionId);
		//the search from instance must match the filter settings
		__ASSERT_ALWAYS(searchFromInstance.CheckStartAndEndDateOverlap(parameters), User::Leave(KErrNotFound));
		
		// remove all instances before and including the searched from instance
		TInt thisPosition(0);
		User::LeaveIfError(instances->FindIsq(searchFromInstance, sortKey, thisPosition));
		instances->Delete(thisPosition, instances->Count() - thisPosition);
		}
	else if (!parameters.iInstance.iId.IsNullId())
		{
		// The entry searched has been deleted or the entry id is wrong
		User::Leave(KErrNotFound);
		}
	else
		{
		// we did not search from a specified instance
		// so remove all instances that are after and including the start time
		for (TInt i(instances->Count() - 1) ; i >= 0 && instances->At(i).InstanceDate() >= parameters.iInstance.iId.Date().LocalL() ; --i)
			{
			instances->Delete(i);
			}
		}
	
	// Truncate the list to the length of aSearchParams.iNumInstances by removeing instances from the end
	if (instances->Count() > parameters.iNumInstances)
		{
		instances->Delete(0, instances->Count() - parameters.iNumInstances);
		}
	delete iBuffer;
	iBuffer = NULL;
	iBuffer = CBufFlat::NewL(KInitialBufferSize);

	// Create a write stream for this buffer
	RBufWriteStream bufStream;
	bufStream.Open(*iBuffer);
	CleanupClosePushL(bufStream);

	const TInt KInstanceCount(instances->Count());

	bufStream.WriteUint32L(KInstanceCount);

	for (TInt i(0) ; i < KInstanceCount ; ++i)
		{
		TAgnInstance instance;
		instance.iId = instances->At(i).InstanceIdL();
		instance.iCollectionId = (*instances)[i].SimpleEntry().CollectionId();
		bufStream << instance;
		}

	bufStream.CommitL();

	CleanupStack::PopAndDestroy(&bufStream); // bufStream.Close();
	CleanupStack::PopAndDestroy(instances);

	// Send it back to the client
	TPckg<TInt> size(iBuffer->Size());
	iMessage.WriteL(KSlot1, size);
	}

void CAgnServerSession::NextInstancesL()
	{
	// Gets the time of the next instance to today
	// Arguments: 0 : output data buffer
	//			  1 : size of the output data buffer
	//			  2 : input data buffer

	// Restore length
	const TInt KBufferSize = iMessage.GetDesLengthL(KSlot2);
	
	// Restore buffer
	CBufFlat* buffer = CBufFlat::NewL(KBufferSize);
	CleanupStack::PushL(buffer);
	buffer->ExpandL(0, KBufferSize);

	TPtr8 des(buffer->Ptr(0));
	iMessage.ReadL(KSlot2, des);
	
	TFindInstanceParams parameters;
	
	RDesReadStream readStream(des);
	
	parameters.InternalizeL(readStream);
	TInt filecount = readStream.ReadInt16L();
	const TInt KInstanceArrayGranularity = 16;
	CArrayFixSeg<TAgnSortInstance>* instances = new (ELeave) CArrayFixSeg<TAgnSortInstance>(KInstanceArrayGranularity);
	CleanupStack::PushL(instances);

	for(TInt ii=0;ii<filecount;++ii)
		{
		TInt64 fileId;
		readStream >> fileId;
		CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(fileId);
		if(file->IsLocked())
			{
			User::Leave(KErrLocked);
			}
		CAgnEntryModel* model = file->Model();
		model->NextPossibleInstancesL(*instances, parameters);
		}
	
	const CAgnSimpleEntry* searchFromEntry = NULL;
	if(parameters.iInstance.iCollectionId > 0)
		{
		CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(parameters.iInstance.iCollectionId);
		if(file->IsLocked())
			{
			User::Leave(KErrLocked);
			}
		searchFromEntry = file->Model()->GetSimpleEntryFromIndexes(parameters.iInstance.iId);
		}

	TAgnSortInstance searchFromInstance(*searchFromEntry);
	if (searchFromEntry)
		{
		TAgnDaySortKey sortKey(AgnDateTime::MaxDate(), parameters.iUndatedTodoTimeLocal);
		searchFromInstance.SetL(parameters.iInstance.iId.Date().LocalL(), parameters.iUndatedTodoTimeLocal);
		const_cast<CAgnSimpleEntry&>(searchFromInstance.SimpleEntry()).SetCollectionId(parameters.iInstance.iCollectionId);
		//the search from instance must match the filter settings
		__ASSERT_ALWAYS(searchFromInstance.CheckStartAndEndDateOverlap(parameters), User::Leave(KErrNotFound));
		TInt thisPosition(0);
		User::LeaveIfError(instances->FindIsq(searchFromInstance, sortKey, thisPosition));
		instances->Delete(0, thisPosition + 1);
		}
	else if (!parameters.iInstance.iId.IsNullId())
		{
		// The entry searched has been deleted or the entry id is wrong
		User::Leave(KErrNotFound);
		}
	
	// Truncate the list to the length of aSearchParams.iNumInstances by removeing instances from the end
	if (instances->Count() > parameters.iNumInstances)
		{
		instances->Delete(parameters.iNumInstances, instances->Count() - parameters.iNumInstances);
		}
	delete iBuffer;
	iBuffer = NULL;
	iBuffer = CBufFlat::NewL(KInitialBufferSize);

	// Create a write stream for this buffer
	RBufWriteStream bufStream;
	bufStream.Open(*iBuffer);
	CleanupClosePushL(bufStream);

	const TInt KInstanceCount(instances->Count());

	bufStream.WriteUint32L(KInstanceCount);

	for (TInt i(0) ; i < KInstanceCount ; ++i)
		{
		TAgnInstance instance;
		instance.iId = instances->At(i).InstanceIdL();
		instance.iCollectionId = (*instances)[i].SimpleEntry().CollectionId();
		bufStream << instance;
		}

	bufStream.CommitL();

	CleanupStack::PopAndDestroy(&bufStream); // bufStream.Close();
	CleanupStack::PopAndDestroy(instances);
	CleanupStack::PopAndDestroy(buffer);

	// Send it back to the client
	TPckg<TInt> size(iBuffer->Size());
	iMessage.WriteL(KSlot1, size);
	}

void CAgnServerSession::CreateEntryIteratorL()
	{
	// Creates a server side entry iterator
	// Used to iterate through all entries in the model e.g. for saving
	// Arguments: 1 : Output - True if an entry is available
	//			  0 : Input  - File Id

	TPckgBuf<TInt64> fileId;
	iMessage.ReadL(KSlot0, fileId);

	TBool isEntry = EFalse;
	if(iAgnSessionFileManager)
		{
		CAgnSessionFile& client = iAgnSessionFileManager->GetSessionFileL(fileId());
		isEntry = client.CreateEntryIterL();
		}
	TPckg<TBool> entryBuffer(isEntry);
	iMessage.WriteL(KSlot1, entryBuffer);
	}

void CAgnServerSession::EntryIteratorNextL()
	{
	TPckgBuf<TInt64> fileId;
	iMessage.ReadL(KSlot0, fileId);

	// Iterates to the next entry
	// Arguments: 0 : Output - True if an entry is available
	TBool isEntry = EFalse;
	if(iAgnSessionFileManager)
		{
		CAgnSessionFile& client = iAgnSessionFileManager->GetSessionFileL(fileId());
		isEntry = client.IsEntryAvailableAtIteratorL();
		}

	TPckg<TBool> boolBuffer(isEntry);
	iMessage.WriteL(KSlot1, boolBuffer);
	}

void CAgnServerSession::EntryIteratorPositionL()
	{
	TPckgBuf<TInt64> fileId;
	iMessage.ReadL(KSlot2, fileId);

	// Returns the entry ID of the current entry
	// Arguments: 0 : Output - Entry ID
	CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(fileId());
	if(file->IsLocked())
		{
		User::Leave(KErrLocked);
		}
	CAgnEntry* entry = NULL;
	if(iAgnSessionFileManager)
		{
		CAgnSessionFile& client = iAgnSessionFileManager->GetSessionFileL(fileId());
		entry = client.FetchEntryAtIteratorL();
		}
	
	if ( entry )
		{
		CleanupStack::PushL(entry);
	
		ExternalizeEntryToClientL(*entry, KSlot1);
	
		CleanupStack::PopAndDestroy(entry);
		}
	else
		{
		TPckg<TInt> size(0);
		iMessage.WriteL(KSlot1, size);
		}	
	}

void CAgnServerSession::GetEntryUidsSinceDateL()
	{
	// Selects entries with a last changed date greater than specified and that match filter
	// Arguments: 0 - Output	: Data buffer (containing list of TCalLocalUid's of entries)
	//			  1 - Output	: Size of data buffer
	//			  2 - Input		: Time

	TPckgBuf<TTime> time;
	iMessage.ReadL(KSlot2,time);

 	RArray<TCalLocalUid> uniqueIdList;
 	CleanupClosePushL(uniqueIdList);

 	CAgnServFile* file = GetFileL(KSlot3);
 	if(file->IsLocked())
 		{
 		User::Leave(KErrLocked);
 		}
	file->Model()->GetEntryUidsSinceDateL(time(), uniqueIdList);

	WriteIntArrayToBufferL(uniqueIdList);

	CleanupStack::PopAndDestroy(&uniqueIdList); //uniqueIdList.Close();

	// Send it back to the client
	TPckg<TInt> listsize(iBuffer->Size());
	iMessage.WriteL(KSlot1, listsize);
	}
	
void CAgnServerSession::RequestChangeNotification()
	{	
	if(iAgnSessionFileManager)
		{
		iAgnSessionFileManager->RequestChangeNotification(iMessage);
		}
	}

void CAgnSessionFileManager::RequestChangeNotification(const RMessage2 aMessage)
    {
    TInt sessionId = aMessage.Int0();
    TUint8 whichNotification = aMessage.Int1();

    TInt index = GetSessionFile(sessionId);
    if (index == KErrNotFound && whichNotification &EFileChange)
        {
        CAgnSessionFile* sessionFile = CAgnSessionFile::NewL(sessionId, iAgnServerSession);
        CleanupStack::PushL(sessionFile);
        iSessionFiles.AppendL(sessionFile);
        CleanupStack::Pop(sessionFile);
        sessionFile->RequestChangeNotification(aMessage);
        }
    else if(index >= 0)
        {
        iSessionFiles[index]->RequestChangeNotification(aMessage);
        }
    }

void CAgnServerSession::CancelChangeNotification()
	{
	//get fileid
	TInt sessionId = iMessage.Int0();
	TUint8 aNotificationType = iMessage.Int1();
	if(iAgnSessionFileManager)
		{
		TInt index = iAgnSessionFileManager->GetSessionFile(sessionId);	
		if (index >= 0)
			{
			iAgnSessionFileManager->GetSessionFileByIndex(index).CancelChangeNotification(aNotificationType);
			}
		}
	}

void CAgnServerSession::RequestChangeNotificationParametersL()
	{
	TPckgBuf<TTime> start;
	iMessage.ReadL(KSlot0, start);

	TPckgBuf<TTime> end;
	iMessage.ReadL(KSlot1, end);
	
	// Restore length
	const TInt KBufferSize = iMessage.GetDesLength(KSlot2);
	
	// Restore buffer
	CBufFlat* buffer = CBufFlat::NewL(KBufferSize);
	CleanupStack::PushL(buffer);
	buffer->ExpandL(0, KBufferSize);

	TPtr8 des(buffer->Ptr(0));
	iMessage.ReadL(KSlot2, des);
	RDesReadStream readStream(des);
	TInt16 type = readStream.ReadInt16L();
	TBool includeUndatedTodos = readStream.ReadInt16L(); 
	TInt64 fileid;
	readStream >> fileid;
	CleanupStack::PopAndDestroy(buffer);
	if(iAgnSessionFileManager)
		{
		CAgnSessionFile& client = iAgnSessionFileManager->GetSessionFileL(fileid);
		client.SetChangeNotificationParametersL(start(), end(), MCalChangeCallBack2::TChangeEntryType(type));
		}
	}

/**
 * Gets the Notes Text writes it to a stream buffer and returns the size.
 * Arguments:
 *		0.  Output  - Buffer
 *		1.  Input  - Stream Id
 *		2.	Output - Size of Notes Field
 */
void CAgnServerSession::RestoreTextL()
	{
	delete iBuffer;
	iBuffer = NULL;
	
	iBuffer = CBufFlat::NewL(KInitialBufferSize);	
	
	// Create a write stream for this buffer
	RBufWriteStream bufStream;
	bufStream.Open(*iBuffer);
	CleanupClosePushL(bufStream);	

	TPckgBuf<TStreamId> textId;
	iMessage.ReadL(KSlot1,textId);
	
	CAgnServFile* file = GetFileL(KSlot3);
	if(file->IsLocked())
		{
		User::Leave(KErrLocked);
		}
	HBufC* text = file->Model()->RestoreTextL(textId());
	CleanupStack::PushL(text);
	
	bufStream.WriteUint32L(text->Length());
	bufStream << *text;
	CleanupStack::PopAndDestroy(text);
	
	bufStream.CommitL();
	CleanupStack::PopAndDestroy(&bufStream); // bufStream.Close();

	// Send the size back to the client
	TPckg<TInt> sizeBuf(iBuffer->Size());
	iMessage.WriteL(KSlot2, sizeBuf); 
	}

/**
Gets the Alarm Action writes it to a stream buffer and returns the size.
Arguments:
	0.  Output - Buffer
	1.  Input  - Stream Id
	2.	Output - Size of alarm action
	3.  Input  - File Id  
*/
void CAgnServerSession::RestoreAlarmActionL()
	{
	delete iBuffer;
	iBuffer = NULL;
	
	iBuffer = CBufFlat::NewL(KInitialBufferSize);	
	
	// Create a write stream for this buffer
	RBufWriteStream bufStream;
	bufStream.Open(*iBuffer);
	CleanupClosePushL(bufStream);
	
	TPckgBuf<TStreamId> id;

	iMessage.ReadL(KSlot1, id);
	TPckgBuf<TInt64> fileId;
	iMessage.ReadL(KSlot3, fileId);
	CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(fileId());
	if(file->IsLocked())
		{
		User::Leave(KErrLocked);
		}
	CAgnContent* alarmAction = file->Model()->RestoreAlarmActionL(id());
	CleanupStack::PushL(alarmAction);
	alarmAction->ExternalizeL(bufStream);
	CleanupStack::PopAndDestroy(alarmAction);	
	
	bufStream.CommitL();

	CleanupStack::PopAndDestroy(&bufStream); // bufStream.Close(); 

	// Send the size back to the client
	TPckg<TInt> sizeBuf(iBuffer->Size());
	iMessage.WriteL(KSlot2, sizeBuf);
	}

void CAgnServerSession::StartBuildIndex()
	{
	// if the calendar file is not opened, leave
	TBool reportProgress = iMessage.Int0();
	TAgnMessageToComplete message(iMessage, reportProgress, *this);
	
	TUint8 fileId = iMessage.Int1();
	CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(fileId);
	file->StartBuildIndex(message);
	}

void CAgnServerSession::TidyByDateStartL()
	{
	TBool reportProgress = iMessage.Int0();
	TAgnMessageToComplete message(iMessage, reportProgress, *this);
	TUint8 fileId = iMessage.Int1();
	CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(fileId);
	if(file->IsLocked())
		{
		User::Leave(KErrLocked);
		}
	CAgnSessionFile& client = iAgnSessionFileManager->GetSessionFileL(fileId);
	client.TidyByDateStartL(message, *file);
	}

void CAgnServerSession::RequestProgressL()
	{
	TAgnMessageToComplete message(iMessage, ETrue, *this);
	TUint8 fileId = iMessage.Int0();
	CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(fileId);
	file->RequestProgressL(message);
	}

void CAgnServerSession::AsynchCategoryTaskStartL()
	{
	TBool reportProgress = iMessage.Int0();
	TAgnMessageToComplete message(iMessage, reportProgress, *this);
	CCalAsyncTaskManager::TAsyncAction task = (CCalAsyncTaskManager::TAsyncAction)iMessage.Int1();
	TUint8 fileId = iMessage.Int2();
	CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(fileId);
	if(file->IsLocked())
		{
		User::Leave(KErrLocked);
		}
	file->CategoryTaskStartL(message, task);
	}

void CAgnServerSession::GetCategoryListCountL()
// gets the number of categories in the list
// arguments:
//	0 - output, count
//  1 - input, file id
	{
	TPckgBuf<TInt64> fileId;
	iMessage.ReadL(KSlot1, fileId);
	CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(fileId());
	if(file->IsLocked())
		{
		User::Leave(KErrLocked);
		}
	TInt count = file->CategoryList().CategoryCount();
	TPckg<TInt> buf(count);
	iMessage.WriteL(KSlot0, buf);
	}

void CAgnServerSession::GetCategoryListItemL()
// gets the category in the list at index passed in from the client
// arguments:
//	0 - output,	data buffer
//	1 - input, index
//  2 - output, size of buffer
//  3 - input, file id
	{
	TInt index = iMessage.Int1();
	
	TPckgBuf<TInt64> fileId;
	iMessage.ReadL(KSlot3, fileId);
	CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(fileId());
	if(file->IsLocked())
		{
		User::Leave(KErrLocked);
		}
	if( index >= file->CategoryList().CategoryCount() )
		{
		User::Leave(KErrNotFound);
		}
	
	delete iBuffer;
	iBuffer = NULL;

	iBuffer = CBufFlat::NewL(KInitialBufferSize);
	
	// Create a write stream for this buffer
	RBufWriteStream bufStream;
	bufStream.Open(*iBuffer);
	CleanupClosePushL(bufStream);	

	// Buffer contains one byte for category type and the string for the extended category name
	file->CategoryList().Category(index).ExternalizeL(bufStream);

	bufStream.CommitL();

	TPckg<TInt> size(iBuffer->Size());
	iMessage.WriteL(KSlot2, size);
	
	CleanupStack::PopAndDestroy(&bufStream); // bufStream.Close()
	}

void CAgnServerSession::AddCategoryToListL()
// adds a new category to the list; 
// arguments:
//	0 - input,	category name
//  1 - input,  file id
	{

	HBufC* bufptr = ReadClientDesLC(KSlot0);
	TPtr categoryName = bufptr->Des();
	
	TPckgBuf<TInt64> fileId;
	iMessage.ReadL(KSlot1, fileId);
	CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(fileId());
	if(file->IsLocked())
		{
		User::Leave(KErrLocked);
		}
	file->CategoryList().AddCategoryL(categoryName);
	
	CleanupStack::PopAndDestroy(bufptr);
	}

void CAgnServerSession::CategoryTaskStartL()
	{
	// Arguments: 0 - Input	: Data buffer (category and step size)
	//			  1 - Input	: Size of data buffer
	//			  2 - Input : step of the task
	//			  3 - Input	: File Id
	const TInt KBufferSize = iMessage.Int1();
	// Restore buffer
	CBufFlat* buffer = CBufFlat::NewL(KBufferSize);
	CleanupStack::PushL(buffer);

	buffer->ExpandL(0, KBufferSize);

	TPtr8 buf(buffer->Ptr(0));
	iMessage.ReadL(KSlot0,buf);
	// Internalize the data from the stream
	TInt categoryIndex;
	TUint8 shortFileId = iMessage.Int3();
	GetCategoryInfoL(*buffer,categoryIndex, shortFileId);
	CleanupStack::PopAndDestroy(buffer);
	
	CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(shortFileId);
	if(file->IsLocked())
		{
		User::Leave(KErrLocked);
		}
	TInt task = iMessage.Int2();
	file->Model()->CategoryIndex().PrepareStepTaskL(task,categoryIndex);
	}

void CAgnServerSession::FilterCategoryL()
	{
	//	Arguments:0 - Input		: Data Buffer (category and step size)
	//			  1 - Input		: Size of data buffer
	CArrayFixSeg<TAgnEntryId>* entryList = new (ELeave) CArrayFixSeg<TAgnEntryId>(KGranFilteredEntries);
	CleanupStack::PushL(entryList);
	
	TPckgBuf<TInt64> fileId;
	iMessage.ReadL(KSlot2, fileId);
	CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(fileId());
	if(file->IsLocked())
		{
		User::Leave(KErrLocked);
		}
	file->Model()->CategoryIndex().GetArrayFilterCategoryL(*entryList);
	
	delete iBuffer;
	iBuffer = NULL;
	
	iBuffer = CBufFlat::NewL(KInitialBufferSize);

	// Create a write stream for this buffer
	RBufWriteStream bufStream;
	bufStream.Open(*iBuffer);
	CleanupClosePushL(bufStream);	

	const TInt KCount = entryList->Count();

	bufStream.WriteInt16L(KCount);
	
	for(  TInt ii = 0; ii < KCount; ++ii )
		{
		entryList->At(ii).ExternalizeL(bufStream);
		}
		
	bufStream.CommitL();

	CleanupStack::PopAndDestroy(&bufStream); // bufStream.Close();

	CleanupStack::PopAndDestroy(entryList);

	// Send it back to the client
	TPckg<TInt> size(iBuffer->Size());
	iMessage.WriteL(KSlot1, size);
	}

void CAgnServerSession::CancelTaskL()
	{
	TPckgBuf<TInt64> fileId;
	iMessage.ReadL(KSlot0, fileId);
	CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(fileId());
	if(file)
		{
		file->DoTaskCompleteL(KErrCancel, this);
		}
	}

void CAgnServerSession::CancelAllTask()
	{
	const TInt count = FileIdCount();
	for(TInt ii=0; ii<count; ++ii)
		{
		CAgnServFile* file = NULL;
		TRAPD(err, file = iAgnServer.FileMgr()->GetFileL(FileId(ii)));
		if(err == KErrNone)
			{
			TRAP_IGNORE(file->DoTaskCompleteL(KErrCancel, this));
			}
		}
	}

void CAgnServerSession::BackupRestoreCancelTask()
    {
    const TInt count = FileIdCount();
    for(TInt ii=0; ii<count; ++ii)
        {
        CAgnServFile* file = NULL;
        TRAPD(err, file = iAgnServer.FileMgr()->GetFileL(FileId(ii)));
        if(err == KErrNone)
            {
            TRAP_IGNORE(file->CancelTaskL(this));
            }
        }
    }

void CAgnServerSession::GetCategoryInfoL(const CBufFlat& aBufFlat,TInt& aCategoryIndex, TCalCollectionId aCollectionId)
	{
	RBufReadStream readStream;
	readStream.Open(aBufFlat);
	CleanupClosePushL(readStream);
		
	CAgnCategory* category = CAgnCategory::NewL(readStream);

	CleanupStack::PopAndDestroy(&readStream); // readStream.Close();

	CleanupStack::PushL(category);
	
	CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(aCollectionId);
	if(file->IsLocked())
		{
		User::Leave(KErrLocked);
		}
	aCategoryIndex = file->CategoryList().FindCategoryIndex(*category);
	User::LeaveIfError(aCategoryIndex);

	CleanupStack::PopAndDestroy(category);
	}

/** Create a store with Agenda file */
void CAgnServerSession::CreateAgendaFileL()
	{
	// Arguments: 0 : Input - size fo buffer
	//			  1 : Input - Data Buffer
	//			  2 : Input - file name
	//			  3 : Input - todo list name

	//Create a store
	HBufC* filenameBuf = ReadClientDesLC(KSlot0);
	CFileStore* store=iAgnServer.FileMgr()->CreateAgendaFileLC(*filenameBuf);//PUSH: store
	store->SetTypeL(TUidType(KPermanentFileStoreLayoutUid, KUidAppDllDoc, KUidAgnApp));

	CAgnEntryModel* model = CAgnEntryModel::NewL(NULL);
	CleanupStack::PushL(model); //PUSH: model
	TStreamId headstreamId = model->CreateL(*store); 

	//Write dummy app id to the store
	//Create the streamdictionary (effectively the root stream)
	CStreamDictionary* streamDic=CStreamDictionary::NewLC();//PUSH: streamDic
	if (streamDic->At(KUidAgnModel)!=headstreamId)
		{
		streamDic->AssignL(KUidAgnModel, headstreamId);
		}

	// Stream dictionary (root)
	TApaAppIdentifier dummyApp(KUidAgnModel, KNullDesC());
	CApaProcess::WriteRootStreamL(*store,*streamDic, dummyApp);
	store->CommitL();
	CleanupStack::PopAndDestroy(3, store);
	
	//Convert the file name into standard format "c:filename
	HBufC* parsedFilename = iAgnServer.FileMgr()->ParseFilenameLC(*filenameBuf);//full name include the parth	
    parsedFilename->Des().Fold();   
    TParsePtrC parse(*parsedFilename);
    HBufC* shortFileName = HBufC::NewLC(parsedFilename->Length());
    shortFileName->Des().Append(parse.Drive());
    shortFileName->Des().Append(parse.NameAndExt());   
    CAgnFileChangeInfo* changeInfo = NULL;
    changeInfo = CAgnFileChangeInfo::NewL(shortFileName, MCalFileChangeObserver::ECalendarFileCreated);
    CleanupStack::Pop(shortFileName);
    CleanupStack::PushL(changeInfo);
    CalendarFileChangedL(*changeInfo);//add the change to the buffer  
    CleanupStack::PopAndDestroy(3, filenameBuf);//changeInfo, parsedFilename, filenameBuf
	}

TBool CAgnServerSession::IsOwnFileL(TInt aSessionId, const CAgnServFile& file)
    {
    TBool isOwnFile = EFalse;    
    //Check whether it is its own file
    if(iAgnSessionFileManager)
       {
       TInt index = iAgnSessionFileManager->GetSessionFile(aSessionId);
       if(index >= 0)
           {
           TCalFileId fileId = iAgnSessionFileManager->GetSessionFileByIndex(index).FileId();
           if(fileId == file.Model()->GetFileIdL())
               {
               isOwnFile = ETrue;
               }
           }
       }
    return isOwnFile;
    }

TBool CAgnServerSession::FileCanBeDeletedL(TInt aSessionId, const CAgnServFile& file)
    {
    TBool fileCanBeDeleted = EFalse;
    TBool isOwnFile = IsOwnFileL(aSessionId, file);    
    if(file.ReferenceCount() == 0 || (isOwnFile && file.ReferenceCount() == 1))
        {
        fileCanBeDeleted = ETrue;
        }
    return fileCanBeDeleted;       
    }

TBool CAgnServerSession::FileHasBeenOpenedL(const CAgnServFile& file)
    {
    TBool fileHasBeenOpened = EFalse;
    if(iAgnSessionFileManager)
        {
        if(iAgnSessionFileManager->FindL(file.Model()->GetFileIdL()) != KErrNotFound)
            {
            fileHasBeenOpened = ETrue;
            }      
        }
    return fileHasBeenOpened;
    }

void CAgnServerSession::DeleteAgendaFileL()
    {
    // Delete a file held by the server
    //              1: Input - Agenda file name
    
    // Get descriptor for filename to delete
    HBufC* bufptr = ReadClientDesLC(KSlot0);
    TPtr filename = bufptr->Des();
    
    HBufC* fullFilename = iAgnServer.FileMgr()->ParseFilenameLC(filename);
    TInt sessionId = iMessage.Int1();
  
    CAgnServFile* file = NULL;
    file = iAgnServer.FileMgr()->GetFile(*fullFilename);
    if(file && !FileCanBeDeletedL(sessionId, *file))
        {
        User::Leave(KErrInUse);
        }
    
    TBool closeFile(EFalse);
    TInt err = KErrNone;
    if (!file)
        {
        // the file is not open so we must open it now
        CalCommon::TCalFileVersionSupport status;
        TRAP(err, file = &iAgnServer.FileMgr()->OpenAgendaL(filename, iAgnServer, status));       
        if (err == KErrNone)
            {
            // Everything was fine when opening the file
            closeFile = ETrue;
            CleanupStack::PushL(TCleanupItem(CleanupCloseAgendaImmediately, file));        
            }
        }
    if(!file)
        {
        iAgnServer.FileMgr()->DeleteAgendaFileL(filename);
        }
    else
        {
        TBool calInfoExist = file->IsCalendarInfoExistL();
        TBool deleteSessionFile = EFalse;     
        if (closeFile)
            {
            // This will call CloseAgenda(ETrue)
            CleanupStack::PopAndDestroy();
            }
        else if( IsOwnFileL(sessionId, *file) )
            {
            // The client has requested to delete it's own calendar file
            // so try to close it immediately
            User::LeaveIfError(iAgnServer.FileMgr()->CloseAgenda(*file, ETrue));
            deleteSessionFile = ETrue;
            iSessionReady = ETrue;
            }
        iAgnServer.FileMgr()->DeleteAgendaFileL(filename);
        
        TParsePtrC parse(*fullFilename);
        HBufC* shortFileName = HBufC::NewLC(fullFilename->Length());
        shortFileName->Des().Append(parse.Drive());
        shortFileName->Des().Append(parse.NameAndExt());
		iAgnServer.AlarmServer().AlarmDeleteByCalendarFile(*shortFileName, EAllAlarms);
        CAgnFileChangeInfo* changeInfo = NULL;
    
        if(calInfoExist)
            {
            HBufC* shortFileNameCopy = shortFileName->AllocLC();
            changeInfo = CAgnFileChangeInfo::NewL(shortFileNameCopy, MCalFileChangeObserver::ECalendarInfoDeleted);
            CleanupStack::Pop(shortFileNameCopy);         
            CleanupStack::PushL(changeInfo);
            CalendarFileChangedL(*changeInfo);//add the change to the buffer
            CleanupStack::PopAndDestroy(changeInfo);
            }       
        changeInfo = CAgnFileChangeInfo::NewL(shortFileName, MCalFileChangeObserver::ECalendarFileDeleted);
        CleanupStack::Pop(shortFileName);
        CleanupStack::PushL(changeInfo);
        CalendarFileChangedL(*changeInfo);
        CleanupStack::PopAndDestroy(changeInfo);
        if(deleteSessionFile)
            {
            iAgnSessionFileManager->DeleteSessionFileIfFileNotificationNotRequired(sessionId); 
            }
        }  
    
    CleanupStack::PopAndDestroy(fullFilename);
    CleanupStack::PopAndDestroy(bufptr);    // buffer
    }

void CAgnServerSession::CalendarFileChangedL(CAgnFileChangeInfo& aFileChangeInfo)
    {
    RPointerArray<CAgnServerSession> sessions;
    CleanupClosePushL(sessions);
    iAgnServer.FetchSessionsL(sessions);
    const TInt count = sessions.Count();
    for ( TInt i = 0; i < count; ++i )
        {       
        sessions[i]->AddFileChangeL(aFileChangeInfo, this);
        }
    CleanupStack::PopAndDestroy(&sessions); // sessions.Close()   
    }

void CAgnServerSession::AddFileChangeL(CAgnFileChangeInfo& aFileChangeInfo, CAgnServerSession* aSession)
    {
    if(iAgnSessionFileManager && this != aSession)
        {
        iAgnSessionFileManager->AddFileChangeL(aFileChangeInfo);
        }
    }

void CAgnSessionFileManager::AddFileChangeL(CAgnFileChangeInfo& aFileChangeInfo)
    {
    const TInt count = iSessionFiles.Count();
    for(TInt ii = 0; ii < count; ++ii)
        {
        iSessionFiles[ii]->AddFileChangeToBufferL(aFileChangeInfo);
        }
    }

void CAgnServerSession::ListAgendaFilesL()
	{
	// Get a list of file names held by the server 
	// currently setup extractor
	// Arguments: 0 : Output - Data buffer 
	//			   1 : Output - Size of buffer

	//create the buffer

	delete iBuffer;
	iBuffer = NULL;

	iBuffer = CBufFlat::NewL(KInitialBufferSize);
	
	// Create a write stream for this buffer
	RBufWriteStream bufStream;
	bufStream.Open(*iBuffer);
	CleanupClosePushL(bufStream);	

	CDesCArray* fileNames = iAgnServer.FileMgr()->ListAgendaFilesL();
	if( fileNames )
		{
		CleanupStack::PushL(fileNames);

		const TInt KCount = fileNames->Count();

		bufStream.WriteUint8L(KCount);

		for ( TInt ii = 0; ii < KCount; ++ii )
			{
			const TInt KLength = (fileNames->MdcaPoint(ii)).Length();
			bufStream.WriteUint8L(KLength);
			bufStream.WriteL(fileNames->MdcaPoint(ii), KLength);
			}

		CleanupStack::PopAndDestroy(fileNames);			
		}
	else
		{
		bufStream.WriteUint8L(0);
		}
		
	bufStream.CommitL();

	CleanupStack::PopAndDestroy(&bufStream); // bufStream.Close();

	// Send it back to the client
	TPckg<TInt> size(iBuffer->Size());
	iMessage.WriteL(KSlot1, size);
	}

void CAgnServerSession::TidyByDateReadParamsL()
	{
	// Set up a tidy by date operation 
	// Arguments: 0 : Input -  Buffer containing packaged tidy by date operation data
	//			  1 : Input - File Id	 
	TUint8 fileId = iMessage.Int1();
	CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(fileId);
	__ASSERT_ALWAYS( file, Panic(EAgmErrTidyByDateNoFileOpen));
	// If the file already has a session associated with it, then it's busy
	// for this delete operation.
	if (file->IsLocked())
	    {
	    return;
	    }
	
	const TInt KDesLength = iMessage.GetDesLength(KSlot0);
	if(KDesLength<0)
		{
		User::Leave(KErrArgument);
		}
	CBufFlat* buffer = CBufFlat::NewL(KDesLength);
	CleanupStack::PushL(buffer);

	buffer->ExpandL(0, KDesLength);
	TPtr8 des(buffer->Ptr(0));
	iMessage.ReadL(KSlot0, des);

	RBufReadStream readStream;
	readStream.Open(*buffer);
	CleanupClosePushL(readStream);

	TAgnFilter filter;
	filter.InternalizeL(readStream);
	TPckgBuf<TTime> undatedTodosDatePckgBuf;
	readStream.ReadL(undatedTodosDatePckgBuf);
	TPckgBuf<TTime> startDatePckgBuf;
	readStream.ReadL(startDatePckgBuf);
	TPckgBuf<TTime> endDatePckgBuf;
	readStream.ReadL(endDatePckgBuf);
	
	CleanupStack::PopAndDestroy(&readStream); // readStream.Close();

	file->TidyByDateSetup(*this,
							  filter,
							  undatedTodosDatePckgBuf(),
							  startDatePckgBuf(),
							  endDatePckgBuf());

	CleanupStack::PopAndDestroy(buffer);
	}

void CAgnServerSession::AgendaFileExistsL()
	{
	// Check if a Calendar file exists 
	// Arguments: 0 : Output -  True if the file exists, otherwise False
	//			  1 : Input  -  Buffer containing calendar file name
	HBufC* bufptr = ReadClientDesLC(KSlot1);

	TPckg<TBool> isExists(iAgnServer.FileMgr()->AgendaFileExistsL(*bufptr));
	
	CleanupStack::PopAndDestroy(bufptr);	// bufptr

	iMessage.WriteL(KSlot0, isExists);
	}

void CAgnServerSession::BulkChangeCompletedL(TInt64 aFileId, TInt aErr)
	{
	CAgnSessionFile& client = iAgnSessionFileManager->GetSessionFileL(aFileId);
	client.BulkChangeCompletedL(*this, aErr);
	}

void CAgnServerSession::EnableChangeBroadcastL()
	{
	//get fileid
	TPckgBuf<TInt64> fileId;
	iMessage.ReadL(KSlot0, fileId);
	if(iAgnSessionFileManager)
		{
		CAgnSessionFile* client = NULL;
		TRAPD(err, client = &(iAgnSessionFileManager->GetSessionFileL(fileId())));
		if(err == KErrNone && client)
			{
			client->EnableChangeBroadcastL(*this);
			}
		else if (err != KErrNotFound)
			{
			User::Leave(err);
			}           
		}    
	}

void CAgnServerSession::DisableChangeBroadcastL()
	{
	//get fileid
	TPckgBuf<TInt64> fileId;
	iMessage.ReadL(KSlot0, fileId);
	if(iAgnSessionFileManager)
		{
		CAgnSessionFile* client = NULL;
		TRAPD(err, client = &(iAgnSessionFileManager->GetSessionFileL(fileId())));
		if(err == KErrNone && client)
			{
			client->DisableChangeBroadcast();
			}
		else if (err != KErrNotFound)
			{
			User::Leave(err);
			}           
		}    
	}

void CAgnServerSession::GetFileChangesSinceLastNotificationL()
	{
	// Get calendar change info
	// Arguments: 0 : Output - Data buffer 
	//            1 : Output - Size of buffer
	//            2 : Input - Session Id

	TInt sessionId = iMessage.Int2();;
	delete iBuffer;
	iBuffer = NULL;
	TInt buffersize = 0;
	if(iAgnSessionFileManager)
		{
		TInt index = iAgnSessionFileManager->GetSessionFile(sessionId);
		if(index >= 0)
			{
			buffersize = iAgnSessionFileManager->GetSessionFileByIndex(index).GetFileChangesSinceLastNotificationL(iBuffer);       
			}
		}

	TPckg<TInt> size(buffersize);
	iMessage.WriteL(KSlot1, size);
	}

void CAgnServerSession::GetChangesSinceLastNotificationL()
	{
	// Get changed entries from the log kept in the model
	// Arguments: 0 : Output - Data buffer 
	//			   1 : Input  - Size of buffer
	
	TCalCollectionId collectionId = iMessage.Int2();;
	delete iBuffer;
	iBuffer = NULL;
	TInt buffersize = 0;
	TInt64 fileId = 0;
	if(iAgnSessionFileManager)
		{
		CAgnSessionFile& client = iAgnSessionFileManager->GetSessionFileL(collectionId);
		buffersize = client.GetChangesSinceLastNotificationL(iBuffer);
		fileId = client.FileId();
		}
	
	TPckg<TInt> size(buffersize);
	iMessage.WriteL(KSlot1, size);
	TPckg<TInt64> fileIdPackage(fileId);
	iMessage.WriteL(KSlot3, fileIdPackage);
	}

void CAgnServerSession::FindInstancesL()
	{
	// Restore length
	const TInt KBufferSize = iMessage.GetDesLengthL(KSlot2);
	
	// Restore buffer
	CBufFlat* buffer = CBufFlat::NewL(KBufferSize);
	CleanupStack::PushL(buffer);
	buffer->ExpandL(0, KBufferSize);

	TPtr8 des(buffer->Ptr(0));
	iMessage.ReadL(KSlot2, des);
	
	TFindInstanceParams parameters;
	
	RDesReadStream readStream(des);
	
	parameters.InternalizeL(readStream);
	TInt filecount = readStream.ReadInt16L();
	const TInt KInstanceArrayGranularity = 16;
	CArrayFixSeg<TAgnSortInstance>* instances = new (ELeave) CArrayFixSeg<TAgnSortInstance>(KInstanceArrayGranularity);
	CleanupStack::PushL(instances);
	for(TInt ii=0;ii<filecount;++ii)
		{
		TInt64 fileId;
		readStream >> fileId;
		CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(fileId);
		if(file->IsLocked())
			{
			User::Leave(KErrLocked);
			}
		CAgnEntryModel* model = file->Model();
		model->FindInstancesL(*instances, parameters);
		}
	
	TAgnDaySortKey sortKey(AgnDateTime::MaxDate(), parameters.iUndatedTodoTimeLocal);
	instances->Sort(sortKey);
	
	// Now put the relevant info into a buffer
	delete iBuffer;
	iBuffer = NULL;

	iBuffer = CBufFlat::NewL(KInitialBufferSize);
	
	// Create a write stream for this buffer
	RBufWriteStream bufStream;
	bufStream.Open(*iBuffer);
	CleanupClosePushL(bufStream);	

	const TInt KCount = instances->Count();

	bufStream.WriteUint32L(KCount);
	
	for ( TInt ii = 0; ii < KCount; ++ii )
		{
		TAgnInstance instance;
		instance.iId = (*instances)[ii].InstanceIdL();
		instance.iCollectionId = (*instances)[ii].SimpleEntry().CollectionId();
		bufStream << instance;
		}

	bufStream.CommitL();
	
	CleanupStack::PopAndDestroy(&bufStream); // bufStream.Close();

	CleanupStack::PopAndDestroy(instances);
	CleanupStack::PopAndDestroy(buffer);

	// Send it back to the client
	TPckg<TInt> size(iBuffer->Size());
	iMessage.WriteL(KSlot1, size);
	}

// CAgnInstanceInfo //	
	
CAgnInstanceInfo* CAgnInstanceInfo::NewL(const CAgnSimpleEntry& aSimpleEntry)
	{
	CAgnInstanceInfo* self = CAgnInstanceInfo::NewLC(aSimpleEntry);
	CleanupStack::Pop(self);
	return self;
	}


CAgnInstanceInfo* CAgnInstanceInfo::NewLC(const CAgnSimpleEntry& aSimpleEntry)
	{
	CAgnInstanceInfo* self = new (ELeave) CAgnInstanceInfo();
	CleanupStack::PushL(self);
	self->ConstructL(aSimpleEntry);
	return self;
	}

void CAgnInstanceInfo::ConstructL(const CAgnSimpleEntry& aSimpleEntry)
	{
	iType = aSimpleEntry.Type();
	if ( aSimpleEntry.RptDef() )
		{
		iRptDef = CAgnRptDef::NewL(aSimpleEntry);
		iRptDef->CopyL(*aSimpleEntry.RptDef());
		}
	else
		{
		iStartTimeUtc = aSimpleEntry.StartTime().UtcL();
		iEndTimeUtc = aSimpleEntry.EndTime().UtcL();
		}
	}

CAgnInstanceInfo::CAgnInstanceInfo()
	{
	}

CCalEntry::TType CAgnInstanceInfo::Type() const
	{
	return iType;
	}
	
CAgnInstanceInfo::~CAgnInstanceInfo()
	{
	delete iRptDef;
	}

const CAgnRptDef* CAgnInstanceInfo::RptDef() const
	{
	return iRptDef;
	}

const TTime& CAgnInstanceInfo::StartTimeUtc() const
	{
	return iStartTimeUtc;
	}

const TTime& CAgnInstanceInfo::EndTimeUtc() const
	{
	return iEndTimeUtc;
	}
	
// TAgnMessageToComplete //
	
TAgnMessageToComplete::TAgnMessageToComplete(RMessage2& aMessage, TBool aReportProgress, CAgnServerSession& aSession)
	:iMessage(aMessage), iReportProgress(aReportProgress), iSession(aSession)		
	{
	}

RMessage2 TAgnMessageToComplete::Message() const
	{
	return iMessage;
	}

TBool TAgnMessageToComplete::ReportProgress() const
	{
	return iReportProgress;
	}

CAgnServerSession& TAgnMessageToComplete::Session() const
	{
	return iSession;
	}
       
/*
@capability WriteUserData
@capability ReadUserData
*/
void CAgnServerSession::DeleteEntryByUIdL(TCalLocalUid aUniqueId, TCalCollectionId aCollectionId)
	{
	CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(aCollectionId); 
	if(file->IsLocked())
		{
		User::Leave(KErrLocked);
		}

	CAgnEntry* entry = file->Model()->FetchEntryL(aUniqueId);
	if (entry)
		{
		CleanupStack::PushL(entry);
		DeleteEntryL(*entry, aCollectionId);
		CleanupStack::PopAndDestroy(entry);
		}
	}
	
void CAgnServerSession::DeleteEntryByIdL(TAgnEntryId aEntryId, TCalCollectionId aCollectionId)
	{
	CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(aCollectionId);
	__ASSERT_ALWAYS(file, User::Leave(KErrCorrupt));
	if(file->IsLocked())
		{
		User::Leave(KErrLocked);
		}
	CAgnEntry* entry = file->Model()->FetchEntryL(aEntryId);
	if (entry)
		{
		CleanupStack::PushL(entry);
		DeleteEntryL(*entry, aCollectionId);
		CleanupStack::PopAndDestroy(entry);
		}
	}
	
void CAgnServerSession::DeleteEntryL()
	{
	TPckgBuf<TAgnEntryId> entryId;
	iMessage.ReadL(KSlot0,entryId);
	TUint8 fileId = iMessage.Int1();
	DeleteEntryByIdL(entryId(), fileId);
	}
	
void CAgnServerSession::DeleteEntryL(CAgnEntry& aEntry, TCalCollectionId aCollectionId)
	{
	CAgnSessionFile& client = iAgnSessionFileManager->GetSessionFileL(aCollectionId);
	client.DeleteEntryL(aEntry);
	}

void CAgnServerSession::FetchEntryByGuidL()
	{
	// Get a list of entries from the server with this GUID
	// currently setup extractor
	// Arguments:  0 : Output - Data buffer 
	//			   1 : Output - Size of buffer
	//			   2 : Input - Guid
	//create the buffer

	HBufC8* bufptr = ReadClientDes8LC(KSlot2);
	TPtr8 guid = bufptr->Des();

	RPointerArray<CAgnEntry> list;
	CleanupResetAndDestroyPushL(list);
	CAgnServFile* file = GetFileL(KSlot3);
	if(file->IsLocked())
		{
		User::Leave(KErrLocked);
		}

	file->Model()->FetchEntriesL(guid, list);

	delete iBuffer;
	iBuffer = NULL;

	iBuffer = CBufFlat::NewL(KInitialBufferSize);
	
	// Create a write stream for this buffer
	RBufWriteStream bufStream;
	bufStream.Open(*iBuffer);
	CleanupClosePushL(bufStream);	

	const TInt KCount = list.Count();

	bufStream.WriteUint32L(KCount);

	for ( TInt ii = 0; ii < KCount; ++ii )
		{
		CAgnEntry* entry = list[ii];

 		bufStream.WriteUint32L( entry->Type() );
 		entry->ExternalizeToBufferL(bufStream);
		}
		
	bufStream.CommitL();
	
	CleanupStack::PopAndDestroy(&bufStream); // bufStream.Close();
	CleanupStack::PopAndDestroy(&list); // list.Close()
	CleanupStack::PopAndDestroy(bufptr);


	// Send it back to the client
	TPckg<TInt> size(iBuffer->Size());
	iMessage.WriteL(KSlot1, size);
	}

void CAgnServerSession::FetchSimpleEntriesByGuidL()
	{
	// Get a list of entries from the server with this GUID
	// currently setup extractor
	// Arguments:  0 : Output - Data buffer 
	//			   1 : Output - Size of buffer
	//			   2 : Input - Guid
	//create the buffer

	RPointerArray<CAgnEntry> list;
	CleanupResetAndDestroyPushL(list);

	const TInt KBufferSize = iMessage.GetDesLengthL(KSlot2);
	CBufFlat* buffer = CBufFlat::NewL(KBufferSize);
	CleanupStack::PushL(buffer);
	buffer->ExpandL(0, KBufferSize);

	TPtr8 des(buffer->Ptr(0));
	iMessage.ReadL(KSlot2, des);

	RDesReadStream readStream(des);
	TInt uidLen = readStream.ReadInt16L();
	HBufC8* uid = HBufC8::NewLC(uidLen);
	TPtr8 guid = uid->Des();
	readStream.ReadL(guid, uidLen);
	TInt filecount = readStream.ReadInt16L();
	TInt preEntryCount = 0;;
	TInt entryCount = 0;
	for(TInt ii=0;ii<filecount;++ii)
		{
		TUint8 fileId = readStream.ReadUint8L();
		CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(fileId);
		if(file->IsLocked())
			{
			User::Leave(KErrLocked);
			}
		file->Model()->FetchEntriesL(*uid, list);
		entryCount = list.Count();
		for(TInt jj = preEntryCount; jj<entryCount; ++jj)
			{
			list[jj]->SetCollectionId(fileId);
			}
		preEntryCount = entryCount;
		}
	CleanupStack::PopAndDestroy(uid);
	CleanupStack::PopAndDestroy(buffer);

	delete iBuffer;
	iBuffer = NULL;

	iBuffer = CBufFlat::NewL(KInitialBufferSize);
	
	// Create a write stream for this buffer
	RBufWriteStream bufStream;
	bufStream.Open(*iBuffer);
	CleanupClosePushL(bufStream);	

	const TInt KCount = list.Count();

	bufStream.WriteUint32L(KCount);

	for ( TInt ii = 0; ii < KCount; ++ii )
		{
		CAgnEntry* entry = list[ii];
		
		bufStream.WriteUint32L(entry->Type());
		// explictly use the simple entriy's ExternalizeL
 		entry->CAgnSimpleEntry::ExternalizeL(bufStream, ETrue);
 		bufStream.WriteUint8L(entry->CollectionId());
		}
		
	bufStream.CommitL();
	
	CleanupStack::PopAndDestroy(&bufStream); // bufStream.Close();
	CleanupStack::PopAndDestroy(&list); // list.Close()

	// Send it back to the client
	TPckg<TInt> size(iBuffer->Size());
	iMessage.WriteL(KSlot1, size);
	}

void CAgnServerSession::DeleteEntriesByLocalUidL()
	{
	
	// Delete entries by their local uids
	// Arguments: 0 : Input  - Size of data buffer
	//			  1 : Input  - Data Buffer (containing filter)
	const TInt KBufferSize = iMessage.Int0();

	// Restore buffer
	CBufFlat* buffer = CBufFlat::NewL(KBufferSize);
	CleanupStack::PushL(buffer);

	buffer->ExpandL(0, KBufferSize);
	TPtr8 des(buffer->Ptr(0));
	iMessage.ReadL(KSlot1, des);

	// Internalize the data from the stream
	RBufReadStream readStream;
	readStream.Open(*buffer);
	CleanupClosePushL(readStream);
	
	const TInt KCount = readStream.ReadUint32L();
	TUint8 fileId = iMessage.Int2();
	for ( TInt ii = 0; ii < KCount; ++ii )
		{
		const TCalLocalUid KUniqueId = readStream.ReadUint32L();
		DeleteEntryByUIdL(KUniqueId, fileId);
		}

	CleanupStack::PopAndDestroy(&readStream);	// readStream.Close();

	CleanupStack::PopAndDestroy(buffer);		// buffer
	}

void CAgnServerSession::DeleteEntryByGuidL()
	{

	HBufC8* guid = ReadClientDes8LC(KSlot0);
	TBool commitAndNotify = iMessage.Int1();
	TPckgBuf<TInt64> fileId;
	iMessage.ReadL(KSlot2, fileId);
	CAgnSessionFile& client = iAgnSessionFileManager->GetSessionFileL(fileId());
	client.DeleteEntryByGuidL(*guid, commitAndNotify);
	CleanupStack::PopAndDestroy(guid);
	}
	
void CAgnServerSession::CommitL()
	{
	TUint8 fileId = iMessage.Int0();
	CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(fileId);
	if(file->IsLocked())
		{
		User::Leave(KErrLocked);
		}
	file->Model()->CommitL();
	}	

void CAgnServerSession::RollbackL()
	{
	TUint8 fileId = iMessage.Int0();
	CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(fileId);

	//We cannot allow Rollback to happen if a file is locked.
	//A change requested by one client if not committed, can be undone when another client requests a RollBack.
	//This cannot be shifted to "RequiresFileLock" section as this operation can be allowed if session is not ready. 
	if(file->IsLocked())
		{
		User::Leave(KErrLocked);
		}
	file->Model()->Rollback();
	}

CAgnServFile* CAgnServerSession::GetFileL(TInt aSlot)
	{
	TPckgBuf<TInt64> fileId;
	iMessage.ReadL(aSlot, fileId);
	return iAgnServer.FileMgr()->GetFileL(fileId());
	}

void CAgnServerSession::TzRulesLastModifiedDateTimeL()
	{
	TPckgBuf<TInt64> fileId;
	iMessage.ReadL(KSlot1, fileId);
	CAgnServFile* file = iAgnServer.FileMgr()->GetFileL(fileId());
	TPckgBuf<TTime> time(file->Model()->TzRulesLastModifiedDateL());
	iMessage.WriteL(KSlot0, time);
	}

TInt CAgnServerSession::FileIdCount()
	{
	TInt count = 0;
	if(iAgnSessionFileManager)
		{
		count = iAgnSessionFileManager->FileIdCount();
		}
	return count;
	}

TInt64 CAgnServerSession::FileId(TInt aIndex)
	{
	TInt64 fileId = KNullFileId;
	if(iAgnSessionFileManager)
		{
		fileId = iAgnSessionFileManager->FileId(aIndex);
		}
	return fileId;
	}

void CAgnServerSession::SetCalendarInfoL()
	{
	// Restore length
	const TInt KBufferSize = iMessage.GetDesLength(KSlot1);

	// Restore buffer
	CBufFlat* buffer = CBufFlat::NewL(KBufferSize);
	CleanupStack::PushL(buffer);
	buffer->ExpandL(0, KBufferSize);

	TPtr8 des(buffer->Ptr(0));
	iMessage.ReadL(KSlot1, des);
	RDesReadStream readStream(des);

	CAgnCalendarInfo* info = CAgnCalendarInfo::NewL();
	CleanupStack::PushL(info);
	info->IpcInternalizeL(readStream);

	// try to get the file if it is already open
	HBufC* filename = ReadClientDesLC(KSlot0);
	HBufC* parsedFilename = iAgnServer.FileMgr()->ParseFilenameLC(*filename);
	parsedFilename->Des().Fold();
	CAgnServFile* file = iAgnServer.FileMgr()->GetFile(*parsedFilename);

	TBool closeFile(EFalse);

	if (!file)
		{
		// the file is not open so we must open it now
		CalCommon::TCalFileVersionSupport status;
		file = &iAgnServer.FileMgr()->OpenAgendaL(*filename, iAgnServer, status);
		closeFile = ETrue;
		CleanupStack::PushL(TCleanupItem(CleanupCloseAgendaImmediately, file));
		}

	// we have the file open, now set the file name on the info
	// before saving the info in a stream in the file.
	TParsePtrC parse(*parsedFilename);
	HBufC* shortFileName = HBufC::NewLC(parsedFilename->Length());
	shortFileName->Des().Append(parse.Drive());
	shortFileName->Des().Append(parse.NameAndExt());
	info->SetFileNameL(*shortFileName);
	TBool fileInfoExist = file->SetCalendarInfoL(*info);

	// set the file name to the correct format
	CAgnFileChangeInfo* changeInfo = NULL;
	if(fileInfoExist)
		{
		changeInfo = CAgnFileChangeInfo::NewL(shortFileName, MCalFileChangeObserver::ECalendarInfoUpdated);
		}
	else
		{
		changeInfo = CAgnFileChangeInfo::NewL(shortFileName, MCalFileChangeObserver::ECalendarInfoCreated);
		}
	CleanupStack::Pop(shortFileName);
	CleanupStack::PushL(changeInfo);
	CalendarFileChangedL(*changeInfo);//add the change to the buffer
	CleanupStack::PopAndDestroy(changeInfo);
	if (closeFile)
		{
		// This will call CloseAgenda(ETrue)
		CleanupStack::PopAndDestroy();
		}

	CleanupStack::PopAndDestroy(parsedFilename);
	CleanupStack::PopAndDestroy(filename);
	CleanupStack::PopAndDestroy(info);
	CleanupStack::PopAndDestroy(buffer);
	}

	void CAgnServerSession::GetPropertyValueL()
	{
	// try to get the file if it is already open
	HBufC* filename = ReadClientDesLC(KSlot2);
	HBufC* parsedFilename = iAgnServer.FileMgr()->ParseFilenameLC(*filename);
	parsedFilename->Des().Fold();
	CAgnServFile* file = iAgnServer.FileMgr()->GetFile(*parsedFilename);

	TBool closeFile(EFalse);

	if (!file)
		{
		// the file is not open so we must open it now
		CalCommon::TCalFileVersionSupport status;
		TRAPD(err, file = &iAgnServer.FileMgr()->OpenAgendaL(*filename, iAgnServer, status));
		
		if (err == KErrNone)
			{
			// Everything was fine when opening the file
			closeFile = ETrue;
			CleanupStack::PushL(TCleanupItem(CleanupCloseAgendaImmediately, file));
			}
		else if (err == KErrNoMemory || err == KErrNotFound)
			{
			// Leave if we ran out of memory or the file does not exist
			// for other leave types we assume that the file is corrupt
			// and we will return some invalid metadata later
			User::Leave(err);
			}
		}

	HBufC8* value(NULL);

	if (file)
		{
		TPckgBuf<TStreamId> streamId;
		iMessage.ReadL(KSlot3, streamId);
		value = file->GetPropertyValueLC(streamId());
		}
	else
		{
		// We failed to open the file
		value = KNullDesC8().AllocLC();
		}

	delete iBuffer;
	iBuffer = NULL;
	iBuffer = CBufFlat::NewL(1000);
	RBufWriteStream bufStream;
	bufStream.Open(*iBuffer);
	CleanupClosePushL(bufStream);   

	// Externalize the calendar info
	bufStream << *value;

	bufStream.CommitL();    
	CleanupStack::PopAndDestroy(&bufStream); // bufStream.Close();

	CleanupStack::PopAndDestroy(value);

	if (closeFile)
		{
		// This will call CloseAgenda(ETrue)
		CleanupStack::PopAndDestroy();
		}

	CleanupStack::PopAndDestroy(parsedFilename);
	CleanupStack::PopAndDestroy(filename);
	}

	void CAgnServerSession::GetCalendarInfoL()
	{
	// try to get the file if it is already open
	HBufC* filename = ReadClientDesLC(KSlot2);
	HBufC* parsedFilename = iAgnServer.FileMgr()->ParseFilenameLC(*filename);
	parsedFilename->Des().Fold();
	CAgnServFile* file = iAgnServer.FileMgr()->GetFile(*parsedFilename);

	TBool closeFile(EFalse);

	if (!file)
		{
		// the file is not open so we must open it now
		CalCommon::TCalFileVersionSupport status;
		TRAPD(err, file = &iAgnServer.FileMgr()->OpenAgendaL(*filename, iAgnServer, status));
		
		if (err == KErrNone)
			{
			// Everything was fine when opening the file
			closeFile = ETrue;
			CleanupStack::PushL(TCleanupItem(CleanupCloseAgendaImmediately, file));
			}
		else if (err == KErrNoMemory || err == KErrNotFound)
			{
			// Leave if we ran out of memory or the file does not exist
			// for other leave types we assume that the file is corrupt
			// and we will return some invalid metadata later
			User::Leave(err);
			}
		}

	CAgnCalendarInfo* info(NULL);

	if (file)
		{
		info = file->GetCalendarInfoLC();
		}
	else
		{
		// We failed to open the file so send back some
		// invalid calendar info
		info = CAgnCalendarInfo::NewL();
		CleanupStack::PushL(info);
		info->SetIsValid(EFalse);
		
		// set the file name to the correct format
		TParsePtrC parse(*parsedFilename);
		TFileName shortFileName(parse.Drive());
		shortFileName.Append(parse.NameAndExt());
		info->SetFileNameL(shortFileName);
		}

	delete iBuffer;
	iBuffer = NULL;
	iBuffer = CBufFlat::NewL(1000);
	RBufWriteStream bufStream;
	bufStream.Open(*iBuffer);
	CleanupClosePushL(bufStream);   

	// Externalize the calendar info
	info->IpcExternalizeL(bufStream);

	bufStream.CommitL();    
	CleanupStack::PopAndDestroy(&bufStream); // bufStream.Close();

	CleanupStack::PopAndDestroy(info);

	if (closeFile)
		{
		// This will call CloseAgenda(ETrue)
		CleanupStack::PopAndDestroy();
		}

	CleanupStack::PopAndDestroy(parsedFilename);
	CleanupStack::PopAndDestroy(filename);
	}

void CAgnServerSession::CloseAgendas()
	{
	const TInt count = FileIdCount();
	for (TInt ii =0; ii<count; ii++)
		{
		CAgnServFile* servFile = NULL;
		TRAPD(err,  servFile = iAgnServer.FileMgr()->GetFileL(FileId(ii)));
		if(servFile && err==KErrNone)
			{
			servFile->CloseAgenda(EFalse);
			}
		}
	}

//CAgnSessionFileManager
CAgnSessionFileManager::CAgnSessionFileManager(CAgnServerSession& aAgnServerSession, CAgnServFileMgr& aAgnServFileMgr)
	:iAgnServerSession(aAgnServerSession),iAgnServFileMgr(aAgnServFileMgr)
	{
	}

CAgnSessionFileManager* CAgnSessionFileManager::NewL(CAgnServerSession& aAgnServerSession, CAgnServFileMgr& aAgnServFileMgr)
	{
	return new(ELeave) CAgnSessionFileManager(aAgnServerSession, aAgnServFileMgr);
	}

CAgnSessionFileManager::~CAgnSessionFileManager()
	{
	iSessionFiles.ResetAndDestroy();
	}

TBool CAgnSessionFileManager::AddClientL(TInt64 aFileId, TInt aSessionId)
	{//return ETrue if an object of CAgnSessionFile has been added in the array iSessionFiles
	TBool objectAdded = EFalse;
	TInt index = GetSessionFile(aSessionId);
	if(index < 0)
		{
		CAgnServFile* file = iAgnServFileMgr.GetFileL(aFileId);
		CAgnSessionFile* SessionFile = CAgnSessionFile::NewL(aSessionId, aFileId, *(file->Model()), iAgnServerSession);
		CleanupStack::PushL(SessionFile);
		TLinearOrder<CAgnSessionFile> order(CompareFileId);
		User::LeaveIfError(iSessionFiles.InsertInOrder(SessionFile, order));
		CleanupStack::Pop(SessionFile);
		objectAdded = ETrue;
		}
	else if (iSessionFiles[index]->FileId() == 0)
	    {//CAgnSessionFile was added when the client start to observ the file change but file may not opened yet.
	    CAgnServFile* file = iAgnServFileMgr.GetFileL(aFileId);
	    iSessionFiles[index]->ResetModel(*file);//set the model and file Id.
	    }
	return objectAdded;
	}
	
TInt CAgnSessionFileManager::FileIdCount()
	{
	return iSessionFiles.Count();
	}

TInt64 CAgnSessionFileManager::FileId(TInt aIndex)
	{
	return iSessionFiles[aIndex]->FileId();
	}

TInt CAgnSessionFileManager::CompareFileId(const CAgnSessionFile& aLeft, const CAgnSessionFile& aRight)
	 {
	 TInt64 key =aLeft.FileId();
	 return CompareFileId(&key, aRight);
	 }

TInt CAgnSessionFileManager::CompareFileId(const TInt64* aKey, const CAgnSessionFile& aAgnSessionFile)
	 {
	 if (*aKey > aAgnSessionFile.FileId())
	     {
	     return 1;
	     }
	 else if (*aKey < aAgnSessionFile.FileId())
	     {
	     return -1;
	     }
	 
	 return 0;
	 }

TInt CAgnSessionFileManager::FindL(TInt64 aFileId) const
	{
	return iSessionFiles.FindInOrder(aFileId, CompareFileId);
	}

void CAgnSessionFileManager::AddChangeL(const TAgnChange& aChange)
	{
	TInt index = FindL(aChange.iFileId);
	if(index >= 0)
		{
		(*iSessionFiles[index]).AddChangeToBufferL(aChange, &iAgnServerSession);
		}
	}

void CAgnSessionFileManager::DeleteSessionFile(TInt aSessionId)
	{
	TInt index = GetSessionFile(aSessionId);
	if(index >= 0)
		{
		delete iSessionFiles[index];
		iSessionFiles.Remove(index);
		}
	}

void CAgnSessionFileManager::DeleteSessionFileIfFileNotificationNotRequired(TInt aSessionId)
    {//Delete the object ony if it doesn't request for file notification otherwise set its fileid = 0 which indicate the file is closed by this CCalSession object.
    TInt index = GetSessionFile(aSessionId);
    if(index >= 0)
        {
        if(!iSessionFiles[index]->FileNotificationIsReqested())
            {
            delete iSessionFiles[index];
            iSessionFiles.Remove(index);
            }
        else
            {
            iSessionFiles[index]->SetFileId(KNullFileId);
            }
        }
    }

CAgnSessionFile& CAgnSessionFileManager::GetSessionFileL(TInt64 aFileId)
	{
	TInt index = FindL(aFileId);
	if(index < 0)
		{
		User::Leave(KErrNotFound);
		}
	return *iSessionFiles[index];
	}

TInt CAgnSessionFileManager::GetSessionFile(TInt aSessionId)
    {
    const TInt count = iSessionFiles.Count();
    for (TInt ii = 0; ii < count; ++ii)
        {
        if(iSessionFiles[ii]->SessionId() == aSessionId)
            {
            return ii;
            }
        }
    
    return KErrNotFound;
    }

CAgnSessionFile& CAgnSessionFileManager::GetSessionFileByIndex(TInt aIndex)
    {
    __ASSERT_ALWAYS(aIndex >= 0, User::Invariant());
    return *iSessionFiles[aIndex];
    }

CAgnSessionFile& CAgnSessionFileManager::GetSessionFileL(TCalCollectionId aCollectionId)
	{
	TInt64 fileId = iAgnServFileMgr.GetLongFileIdL(aCollectionId);
	return GetSessionFileL(fileId);
	}

void CAgnSessionFileManager::ReSetModel(TInt64 aFileId, const CAgnServFile& aServFile)
    {
    CAgnSessionFile& sessionFile = GetSessionFileL(aFileId);
    sessionFile.ResetModel(aServFile);
    }

CAgnSessionFile::CAgnSessionFile(TInt aSessionId, TInt64 aFileId, CAgnEntryModel* aModel, CAgnServerSession& aAgnServerSession)
	:iSessionId(aSessionId),
    iFileId(aFileId),
	iModel(aModel),
	iChangeNotificationPending(ENotificationNotRequested),
	iBulkChangeInProgress(EFalse),
	iChangeFilter(aAgnServerSession),
	iBufferedNotificationsCount(0),
	iNotificationBufferFull(EFalse)
	{
	}

TInt64 CAgnSessionFile::FileId() const 
	{
	return iFileId;
	}

CAgnSessionFile* CAgnSessionFile::NewL(TInt aSessionId, TInt64 aFileId, CAgnEntryModel& aModel, CAgnServerSession& aAgnServerSession)
	{
	return new(ELeave) CAgnSessionFile(aSessionId, aFileId, &aModel, aAgnServerSession);
	}

CAgnSessionFile* CAgnSessionFile::NewL(TInt aSessionId, CAgnServerSession& aAgnServerSession)
    {
    return new(ELeave) CAgnSessionFile(aSessionId, 0, NULL, aAgnServerSession);
    }

CAgnSessionFile::~CAgnSessionFile()
	{
	iBufferedNotificationStream.Close();
	delete iBufferedNotification;
	delete iEntryIter;
	}

void CAgnSessionFile::TidyByDateStartL(TAgnMessageToComplete& aMessageToComplete, CAgnServFile& aServerFile)
	{
	aServerFile.TidyByDateStartL(aMessageToComplete, iChangeFilter);
	iBulkChangeInProgress = ETrue;
	}

void CAgnSessionFile::BulkChangeCompletedL(const CAgnServerSession& aSession, TInt aErr)
	{
	if ( iBulkChangeInProgress )
		{
		if ( aErr == KErrNone )
			{
			if ( iChangeFilter.ChangeBroadcastEnabled() )
				{
				iModel->NotifyChangeL(aSession, NULL, MCalChangeCallBack2::EChangeUndefined, NULL);
				iChangeFilter.SetChangeMadeWhileDisabled(EFalse);
				}
			else
				{
				iChangeFilter.SetChangeMadeWhileDisabled(ETrue);	
				}

			iModel->NotifyPublishAndSubscribeL(iChangeFilter);
			}

		iBulkChangeInProgress = EFalse;
		}

	iModel->SetUpdateAlarmL(ETrue);
	}

void CAgnSessionFile::SetEnablePubSubNotificationL(TBool aEnablePubSub)
	{
	iChangeFilter.SetEnablePubSubNotification(aEnablePubSub);
	iModel->NotifyPublishAndSubscribeL(iChangeFilter);
	}

void CAgnSessionFile::EnableChangeBroadcastL(const CAgnServerSession& aAgnServerSession)
	{
	TBool changeMadeWhileDisabled = iChangeFilter.ChangeMadeWhileDisabled();
	iChangeFilter.SetEnableChangeBroadcast(ETrue);
	if ( changeMadeWhileDisabled )
		{
		iModel->NotifyChangeL(aAgnServerSession, NULL, MCalChangeCallBack2::EChangeUndefined, NULL);
		iChangeFilter.SetChangeMadeWhileDisabled(EFalse);
		}
	}

void CAgnSessionFile::DisableChangeBroadcast()
	{
	iChangeFilter.SetEnableChangeBroadcast(EFalse);
	}

void CAgnSessionFile::UpdateEntryL(CAgnEntry& aEntry, TBool aDeleteChildren)
	{
	if(iModel->AgnServFile().IsLocked())
		{
		User::Leave(KErrLocked);
		}
	
	iModel->UpdateEntryL(aEntry, &iChangeFilter, aDeleteChildren);
	}

TAgnEntryId CAgnSessionFile::StoreEntryL(CAgnEntry& aEntry)
	{
	if(iModel->AgnServFile().IsLocked())
		{
		User::Leave(KErrLocked);
		}

	return iModel->StoreL(aEntry, &iChangeFilter);
	}

void CAgnSessionFile::DeleteEntryL(CAgnEntry& aEntry)
	{
	if(iModel->AgnServFile().IsLocked())
		{
		User::Leave(KErrLocked);
		}

	iModel->DeleteEntryL(aEntry, ETrue, &iChangeFilter);
	}

void CAgnSessionFile::DeleteEntryByGuidL(const TDesC8& aGuid, TBool aCommit)
	{
	if(iModel->AgnServFile().IsLocked())
		{
		User::Leave(KErrLocked);
		}

	CAgnEntry* entry = iModel->FetchEntryL(aGuid);

	if( entry )
		{
		CleanupStack::PushL(entry);
		iModel->DeleteEntryL(*entry, ETrue, NULL);
		if (aCommit)
			{
			iModel->CommitAndNotifyDeletesL(iChangeFilter);
			}
		CleanupStack::PopAndDestroy(entry);
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	}

void CAgnSessionFile::RequestChangeNotification(const RMessage2 aMessage)
	{
	iChangeNotificationMessage = aMessage;
	iChangeNotificationPending = ENotificationRequested;
    TUint8 whichNotification = aMessage.Int1();   
    if(whichNotification&EEntryChangeInSameFile )
        {
        RequestEntryChangeNotification();
        }
    
    if(whichNotification&EFileChange  )
        {
        RequestFileChangeNotification();
        }
	}

void CAgnSessionFile::RequestEntryChangeNotification()
    {
    iChangeIterested = iChangeIterested|EEntryChangeInSameFile;
	if (iChangeNotificationPending == ENotificationRequested && iBufferedNotification && iBufferedNotification->Size() > 0 )
		{
		iChangeNotificationMessage.Complete(EEntryChangeInSameFile);
		iChangeNotificationPending = ENotificationAwaitingRequest;
		}
	}

void CAgnSessionFile::RequestFileChangeNotification()
    {
    iChangeIterested = iChangeIterested|EFileChange;
    if ( iChangeNotificationPending == ENotificationRequested && iFileBufferedNotification && iFileBufferedNotification->Size() > 0 )
        {
        iChangeNotificationMessage.Complete(EFileChange);
        iChangeNotificationPending = ENotificationAwaitingRequest;
        }
      }

void CAgnSessionFile::CancelChangeNotification(TUint8 aNotificationType)
	{
	if (iChangeNotificationPending != ENotificationNotRequested)
	    {
        if(aNotificationType&EEntryChangeInSameFile)
            {
            iChangeIterested =iChangeIterested&~EEntryChangeInSameFile;
            }    
        if(aNotificationType&EFileChange)
            {
            iChangeIterested = iChangeIterested&~EFileChange;
            }
        if(iChangeIterested == 0)
            {
            iChangeNotificationPending = ENotificationNotRequested;
            if ( ! iChangeNotificationMessage.IsNull())
                {
                iChangeNotificationMessage.Complete(KErrCancel);
                }
            }
	    }
	}

void CAgnSessionFile::SetChangeNotificationParametersL(const TTime aStart, const TTime aEnd,MCalChangeCallBack2::TChangeEntryType aChangeType)
	{
	iChangeFilter.SetChangeParameter(aStart, aEnd, aChangeType);
	}

TInt CAgnSessionFile::GetChangesSinceLastNotificationL( CBufFlat*& aDataBuffer)
	{
	iBufferedNotificationStream.Close();
		
	// Send it back to the client
	TInt bufferSize = 0;
	if (iBufferedNotification)
		{
		bufferSize = iBufferedNotification->Size();
		}
	
	aDataBuffer = iBufferedNotification ;
	iBufferedNotification = NULL;
	iBufferedNotificationsCount = 0;
	iNotificationBufferFull = EFalse;
	
	return bufferSize;
	}

TInt CAgnSessionFile::GetFileChangesSinceLastNotificationL( CBufFlat*& aDataBuffer)
    {
    iFileBufferedNotificationStream.Close();
        
    // Send it back to the client
    TInt bufferSize = 0;
    if (iFileBufferedNotification)
        {
        bufferSize = iFileBufferedNotification->Size();
        }
    
    aDataBuffer = iFileBufferedNotification ;
    iFileBufferedNotification = NULL;
    iFileBufferedNotificationsCount = 0;
    return bufferSize;
    }

TBool CAgnSessionFile::ToNotifyL(const TAgnChange& aChange, CAgnServerSession* aAgnServerSession)
    {
    if(iChangeNotificationPending == ENotificationNotRequested || !(iChangeIterested &EEntryChangeInSameFile))
        {
        //Notification is not required.
        return EFalse;
        }
    
    if(aChange.iOperationType == MCalChangeCallBack2::EChangeTzRules ||
            aChange.iOperationType == MCalChangeCallBack2::EBackupStart ||
            aChange.iOperationType == MCalChangeCallBack2::ERestoreStart ||
            aChange.iOperationType == MCalChangeCallBack2::EBackupEnd ||
            aChange.iOperationType == MCalChangeCallBack2::ERestoreEnd) 
        {
        //Tz rule has been changed or backup-restore state has been changed.
        return ETrue;
        }
    
    if(aAgnServerSession == aChange.iSession)
        {
        //Calendar entry has been changed but the change came from the same session
        return EFalse;
        }
    
    // It is Calendar entry change come from another session
    return iChangeFilter.IsValidChangeL(aChange);
    }

void CAgnSessionFile::AddChangeToBufferL(const TAgnChange& aChange, CAgnServerSession* aAgnServerSession)
	{
	if(iChangeNotificationPending == ENotificationRequested && aChange.iOperationType == MCalChangeCallBack2::ERestoredFileCanNotBeOpened)
	    {
        iChangeNotificationMessage.Complete(KErrCorrupt);
        iChangeNotificationPending = ENotificationAwaitingRequest;
	    }
	else if (ToNotifyL(aChange, aAgnServerSession))         
		{
		// store the change in the notification buffer
		if (!iBufferedNotification)
			{
			iBufferedNotification = CBufFlat::NewL(KGranNotificationBuffer);
			iBufferedNotificationsCount = 0;
			iNotificationBufferFull = EFalse;
			iBufferedNotificationStream.Open(*iBufferedNotification);
			}

		if (iBufferedNotificationsCount < KMaxNumberOfBufferedNotifications)
			{
			if(!iNotificationBufferFull)
				{
				iBufferedNotificationStream.WriteUint32L(aChange.iEntryId);
				iBufferedNotificationStream.WriteInt8L(aChange.iOperationType);
				iBufferedNotificationStream.WriteInt8L(aChange.iEntryType);
				iBufferedNotificationStream.CommitL();
			
				++iBufferedNotificationsCount;
		
				// notify client that a change has occured
				if (iChangeNotificationPending == ENotificationRequested)
					{
					iChangeNotificationMessage.Complete(EEntryChangeInSameFile);
					iChangeNotificationPending = ENotificationAwaitingRequest;
					}
				}
			}
		else
			{
			iNotificationBufferFull = ETrue;

			// Clear all the notifications in the buffer
			iBufferedNotification->Reset();
			iBufferedNotificationStream.Open(*iBufferedNotification);		
		
			// Add a single 'Undefined Change' notification to the notification buffer
			iBufferedNotificationStream.WriteUint32L(0);
			iBufferedNotificationStream.WriteInt8L(MCalChangeCallBack2::EChangeUndefined);
			iBufferedNotificationStream.WriteInt8L(MCalChangeCallBack2::EChangeEntryAll);
			iBufferedNotificationStream.CommitL();
	
			// Reset the count to 1 - Undefined Change
			iBufferedNotificationsCount = 1;
			}
		}
	}

void CAgnSessionFile::AddFileChangeToBufferL(CAgnFileChangeInfo& aFileChangeInfo)
    {
    if(iChangeNotificationPending != ENotificationNotRequested && iChangeIterested &EFileChange)
       {
        // store the change in the notification buffer
        if (!iFileBufferedNotification)
            {
            iFileBufferedNotification = CBufFlat::NewL(KGranNotificationBuffer);
            iFileBufferedNotificationsCount = 0;
            iFileBufferedNotificationStream.Open(*iFileBufferedNotification);
            iFileBufferedNotificationStream.WriteInt32L(0);
            }
        MStreamBuf * streamBuf = iFileBufferedNotificationStream.Sink();
        TStreamPos posStart(0);
 
        if (iFileBufferedNotificationsCount < KMaxNumberOfBufferedNotifications)
            {
            aFileChangeInfo.ExternalizeL(iFileBufferedNotificationStream);
            //bring the stream write position to the start
            streamBuf->SeekL(MStreamBuf::EWrite, posStart); 
            iFileBufferedNotificationStream.WriteInt32L(++iFileBufferedNotificationsCount);
            iFileBufferedNotificationStream.CommitL();           
            }
        else
            {
             // Clear all the notifications in the buffer
            iFileBufferedNotification->Reset();
            iFileBufferedNotificationStream.Open(*iBufferedNotification);       
        
            // Add a single 'Undefined Change' notification to the notification buffer
            CAgnFileChangeInfo* fileChang = CAgnFileChangeInfo::NewL(NULL, MCalFileChangeObserver::ECalendarInfoUpdated);
            CleanupStack::PushL(fileChang);
            aFileChangeInfo.ExternalizeL(iFileBufferedNotificationStream);
            //bring the stream write position to the start
            streamBuf->SeekL(MStreamBuf::EWrite, posStart); 
            iFileBufferedNotificationStream.WriteInt32L(++iFileBufferedNotificationsCount);
            iFileBufferedNotificationStream.CommitL();
            CleanupStack::PopAndDestroy(fileChang);
            // Reset the count to 1 - Undefined Change
            iFileBufferedNotificationsCount = 1;
            }
        // notify client that a change has occured
        if (iChangeNotificationPending == ENotificationRequested)
            {
            iChangeNotificationMessage.Complete(EFileChange);
            iChangeNotificationPending = ENotificationAwaitingRequest;
            }
        //bring the stream write position to the end
       streamBuf->SeekL(MStreamBuf::EWrite, posStart + iFileBufferedNotification->Size());
       }
    }

TBool CAgnSessionFile::CreateEntryIterL()
	{
	if(iModel->AgnServFile().IsLocked())
		{
		User::Leave(KErrLocked);
		}

	if(!iEntryIter)
		{
		iEntryIter = iModel->CreateEntryIterL();
		}
	return  iEntryIter->SetToFirstL();
	}

TBool CAgnSessionFile::IsEntryAvailableAtIteratorL()
	{
	if(iModel->AgnServFile().IsLocked())
		{
		User::Leave(KErrLocked);
		}

	__ASSERT_ALWAYS(iEntryIter, User::Leave(KErrCorrupt));
	return iEntryIter->NextL();
	}

CAgnEntry* CAgnSessionFile::FetchEntryAtIteratorL()
	{
	__ASSERT_ALWAYS(iEntryIter, User::Leave(KErrCorrupt));
	return iModel->FetchEntryL(iEntryIter->At());
	}

void CAgnSessionFile::ResetModel(const CAgnServFile& aServFile)
    {
    delete iEntryIter;
    iEntryIter = NULL;
    iModel = aServFile.Model();
    iFileId = iModel->GetFileIdL();
    }

TInt CAgnSessionFile::SessionId()
    {
    return iSessionId;
    }

TBool CAgnSessionFile::FileNotificationIsReqested()
    {
    return iChangeIterested& EFileChange;
    }

void CAgnSessionFile::SetFileId(TCalFileId aFileId)
    {
    iFileId = aFileId;
    }

#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
TBool CAgnServerSession::IsReadOnlyOperation()
	{
	//Only operations that have read user data permissions are allowed once a shutdown notification
	//is received. The Rollback operation is still allowed.
	TInt operation = iMessage.Function();
	if (operation == ERollback || operation < KCapabilityWriteUserData)
		{
		return KErrNone;
		}
	else
		{
		return KErrLocked; 
		}
	}
#endif


void CAgnServerSession::CreateInstanceIteratorL()
	{

	//TFindInstanceParams buffer, CCalSortCriteria buffer, iteratorId
	//Do this first, so it can be pushed off the cleanup stack last
	CAgnSortCriteria* sortCriteria = CAgnSortCriteria::NewL();
	CleanupStack::PushL(sortCriteria);
	
	TFindInstanceParams* parameters = new (ELeave) TFindInstanceParams();
	CleanupStack::PushL(parameters);
	
	RArray<TInt64> fileIds;
	CleanupClosePushL(fileIds);
	
	// Restore find instance parameter
	const TInt KBufferSize = iMessage.GetDesLengthL(KSlot0);
	CBufFlat* buffer = CBufFlat::NewL(KBufferSize);
	CleanupStack::PushL(buffer);
	buffer->ExpandL(0, KBufferSize);

	TPtr8 des(buffer->Ptr(0));
	iMessage.ReadL(KSlot0, des);

	RDesReadStream readStream(des);
	parameters->InternalizeL(readStream);
	
	// Restore sort criteria
	const TInt KSortCriteriaSize = iMessage.GetDesLengthL(KSlot1);
	CBufFlat* sortCriteriaBuffer = CBufFlat::NewL(KSortCriteriaSize);
	CleanupStack::PushL(sortCriteriaBuffer);
	sortCriteriaBuffer->ExpandL(0, KSortCriteriaSize);
	TPtr8 desSortCriteria(sortCriteriaBuffer->Ptr(0));
	iMessage.ReadL(KSlot1, desSortCriteria);
	RBufReadStream sortCriteriaStream;
	sortCriteriaStream.Open(*sortCriteriaBuffer);
	sortCriteria->InternalizeL(sortCriteriaStream);

	//Create iterator with specified finding and sorting settings.
	
	if(iInstanceIteratorMgr == NULL)
		{
		//Do this to protect server from fuzzy attacks.
		iInstanceIteratorMgr = CAgsInstanceIteratorMgr::NewL();
		}
	
	TInt filecount = sortCriteriaStream.ReadInt16L();
	for(TInt ii=0;ii<filecount;++ii)
		{
		TInt64 fileId;
		sortCriteriaStream >> fileId;
		fileIds.AppendL(fileId);
		}

	CleanupStack::PopAndDestroy(sortCriteriaBuffer);
	CleanupStack::PopAndDestroy(buffer);

	TInt iteratorId = iInstanceIteratorMgr->CreateIteratorL(parameters, sortCriteria, fileIds, *iAgnServer.FileMgr());
	//We are satisfied that the above line has taken ownership, so we can now pop these two
	CleanupStack::PopAndDestroy(&fileIds);//doesn't destroy the content of the array
	CleanupStack::Pop(parameters);
	CleanupStack::Pop(sortCriteria);
	
	// Send the assigned iterator id back to the client
	TPckg<TInt> id(iteratorId);
	iMessage.WriteL(KSlot2, id);
	}

void CAgnServerSession::DestroyInstanceIteratorL()
	{
	TInt iteratorId = iMessage.Int0();
	iInstanceIteratorMgr->DestroyIteratorL(iteratorId);
	}
	
void CAgnServerSession::InstanceIteratorNextL()
	{
	TInt iteratorId = iMessage.Int2();
	TInt startIndex = iMessage.Int3();
	
	RArray<TAgnInstance> instances;
	CleanupClosePushL(instances);
	iInstanceIteratorMgr->NextInstancesL(iteratorId, instances, startIndex);

	delete iBuffer;
	iBuffer = NULL;
	iBuffer = CBufFlat::NewL(KInitialBufferSize);
	
	RBufWriteStream bufStream;
	bufStream.Open(*iBuffer);
	CleanupClosePushL(bufStream);	

	const TInt KCount = instances.Count();

	bufStream.WriteUint32L(KCount);
	
	for ( TInt ii = 0; ii < KCount; ++ii )
		{
		bufStream << instances[ii];
		}

	bufStream.CommitL();
	
	CleanupStack::PopAndDestroy(&bufStream); // bufStream.Close();
	CleanupStack::PopAndDestroy(&instances);

	// Send it back to the client
	TPckg<TInt> size(iBuffer->Size());
	iMessage.WriteL(KSlot1, size);
	}

void CAgnServerSession::InstanceIteratorPreviousL()
	{
	TInt iteratorId = iMessage.Int2();
	TInt lastIndex = iMessage.Int3();
	
	RArray<TAgnInstance> instances;
	CleanupClosePushL(instances);
	iInstanceIteratorMgr->PreviousInstancesL(iteratorId, instances, lastIndex);

	delete iBuffer;
	iBuffer = NULL;
	iBuffer = CBufFlat::NewL(KInitialBufferSize);
	
	RBufWriteStream bufStream;
	bufStream.Open(*iBuffer);
	CleanupClosePushL(bufStream);	

	const TInt KCount = instances.Count();

	bufStream.WriteUint32L(KCount);
	
	for ( TInt ii = 0; ii < KCount; ++ii )
		{
		bufStream << instances[ii];
		}

	bufStream.CommitL();
	
	CleanupStack::PopAndDestroy(&bufStream); // bufStream.Close();
	CleanupStack::PopAndDestroy(&instances);

	// Send it back to the client
	TPckg<TInt> size(iBuffer->Size());
	iMessage.WriteL(KSlot1, size);
	}

void CAgnServerSession::InstanceIteratorCountL()
	{
	TInt iteratorId = iMessage.Int0();
	TInt count = iInstanceIteratorMgr->CountL(iteratorId);
	TPckg<TInt> size(count);
	iMessage.WriteL(KSlot1, size);
	}

void CAgnServerSession::InstanceIteratorLocateIndexL()
	{
	TInt iteratorId = iMessage.Int0();
	TPckgBuf<TAgnInstance> instanceId;	
	iMessage.ReadL(KSlot1,instanceId);
	TInt index = iInstanceIteratorMgr->LocateIndexL(iteratorId, instanceId());
	TPckg<TInt> size(index);
	iMessage.WriteL(KSlot2, size);
	}



