// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "calentryviewimpl.h"

#include "calclient.h"
#include "agmentry.h"
#include "calcommonimpl.h"
#include "calentryimpl.h"
#include <calsession.h>
#include "calsessionimpl.h"
#include "agmdebug.h"

const TInt KAlarmQueueThreshold(1);
const TInt KNotificationThreshold(20);


CCalEntryViewImpl* CCalEntryViewImpl::NewL(CCalSession& aSession, MCalProgressCallBack* aProgressCallBack)
	{
	CCalEntryViewImpl* self = new(ELeave) CCalEntryViewImpl(aSession);
	CleanupStack::PushL(self);
	self->ConstructL(aProgressCallBack);
	CleanupStack::Pop(self);
	return self; 
	}

CCalEntryViewImpl::CCalEntryViewImpl(CCalSession& aSession):
	iCalSessionImpl(aSession.Impl()), iAgendaServer(aSession.Impl().Server())
	{
	iCalSessionImpl.IncrementReferenceCount();
	}

CCalEntryViewImpl::~CCalEntryViewImpl()
	{
	if(iCalSessionImpl.AsyncTaskManager().BuildingIndex())
		{
		iCalSessionImpl.AsyncTaskManager().CancelAsynTask();
		}
	iCalSessionImpl.DecrementReferenceCount();
	}

void CCalEntryViewImpl::ConstructL(MCalProgressCallBack* aProgressCallBack)
	{
	if (aProgressCallBack)
		{
		iCalSessionImpl.AsyncTaskManager().StartBuildIndexL(*aProgressCallBack);	
		}
	else if (! iCalSessionImpl.AsyncTaskManager().IsBusy() || iCalSessionImpl.AsyncTaskManager().BuildingIndex())
		{//If it is not busy or it is building index, we will ask the server to build index. Otherwise, e.g. the server is doing filtering category, we will do nothing because index must have been built already.
		iAgendaServer.WaitUntilLoadedL(iCalSessionImpl.CollectionId());
		}
	}
		
void CCalEntryViewImpl::FetchL(const TDesC8& aUid, RPointerArray<CCalEntry>& aCalEntryArray) const
	{
	RPointerArray<CAgnEntry> agnEntryArray;
	CleanupResetAndDestroyPushL(agnEntryArray);
	iAgendaServer.FetchEntryByGuidL(aUid, agnEntryArray, iCalSessionImpl.FileId());
	
	#if defined (__CAL_ENTRY_LOGGING__) || (__CAL_VERBOSE_LOGGING__)
		{
		TBuf<KMaxGuidBufLength> guidBuf;
		guidBuf.Copy(aUid);
		AgmDebug::DebugLog("FetchL: Client side request to fetch entries using Guid - '%S'. Found %d entries",&guidBuf, agnEntryArray.Count());
		}
	#endif
		
	if (agnEntryArray.Count() > 0)
		{
		do	{
			CCalEntryImpl* calEntryImpl = CCalEntryImpl::NewL(*agnEntryArray[0], iCalSessionImpl);
			CleanupStack::PushL(calEntryImpl);
			agnEntryArray.Remove(0);
			CCalEntry* calEntry = CCalEntry::NewL(calEntryImpl); // calEntry takes ownership of calEntryImpl
			CleanupStack::Pop(calEntryImpl);
			CleanupStack::PushL(calEntry);
			aCalEntryArray.AppendL(calEntry); // calEntryArray takes ownership of calEntry
			CleanupStack::Pop(calEntry);
			}
		while(agnEntryArray.Count()>0);
		}
	else
		{
		// use a TLex to extract a number from the descriptor aUid
		// to see if it is an old style agenda uid (i.e. the GUID is not a text string but a number equivalent to local UID)
		// if it isn't then we ignore
		TUint intUid;
		TLex8 lex(aUid);
		TInt result(lex.Val(intUid));
		
		if ( result == KErrNone)
			{
			if (lex.Get() == 0) // has the whole string been retrieved?
				{
				CAgnEntry* agnEntry = iAgendaServer.FetchEntryL(static_cast<TCalLocalUid>(intUid), iCalSessionImpl.FileId());
				if (agnEntry)
 					{
					CleanupStack::PushL(agnEntry);
					CCalEntryImpl* calEntryImpl = CCalEntryImpl::NewL(*agnEntry, iCalSessionImpl); // calEntryImpl takes ownership of agnEntry
					CleanupStack::Pop(agnEntry);
					CleanupStack::PushL(calEntryImpl);
					CCalEntry* calEntry = CCalEntry::NewL(calEntryImpl); // calEntry takes ownership of calEntryImpl
					CleanupStack::Pop(calEntryImpl);
					CleanupStack::PushL(calEntry);
					aCalEntryArray.AppendL(calEntry); // calEntryArray takes ownership of calEntry
					CleanupStack::Pop(calEntry);
					}
				}
			}
		}
	CleanupStack::PopAndDestroy(); // CleanupResetAndDestroyPushL(agnEntryArray)
	}
	
CCalEntry* CCalEntryViewImpl::FetchL(TCalLocalUid aCalLocalId) const
	{
	_DBGLOG_ENTRY(AgmDebug::DebugLog("FetchL: Client side request to fetch entries using local Id='%d'",aCalLocalId);)
	
	CAgnEntry* agnEntry = iAgendaServer.FetchEntryL(aCalLocalId, iCalSessionImpl.FileId());
	CCalEntry* calEntry = NULL;
	if (agnEntry)
		{
		CleanupStack::PushL(agnEntry);
		CCalEntryImpl* calEntryImpl = CCalEntryImpl::NewL(*agnEntry, iCalSessionImpl); // calEntryImpl takes ownership of agnEntry
		CleanupStack::Pop(agnEntry);
		CleanupStack::PushL(calEntryImpl);
		calEntry = CCalEntry::NewL(calEntryImpl); // calEntry takes ownership of calEntryImpl
		CleanupStack::Pop(calEntryImpl);
		}
		
	return calEntry;
	}
	
void CCalEntryViewImpl::GetIdsModifiedSinceDateL(const TCalTime& aTime, RArray<TCalLocalUid>& aIds) const
	{
	CleanupClosePushL(aIds); // must be pushed to maintain BC
	iAgendaServer.GetEntryUidsSinceDateL(aIds, aTime.TimeUtcL(), iCalSessionImpl.FileId());
	CleanupStack::Pop(&aIds);
	}

// Stores the array of entries
void CCalEntryViewImpl::StoreL(const RPointerArray<CCalEntry>& aCalEntryList, TInt& aNumSuccessfulEntry)
	{
	TInt count(aCalEntryList.Count());

	_DBGLOG_BASIC(AgmDebug::DebugLog("StoreL: Client side request to store %d entry(ies)",count);)
	
	if (count > 0)
		{	
		// alarm queuing disabled for bulk operations
		// we don't need alarm to be requeued after each entry
		// only at the end
		if (count > KAlarmQueueThreshold)
			{
			StopAlarmUpdatesLC();
			}

		if (count > KNotificationThreshold)
			{
			StopNotificationLC();
			}
		
		TRAPD(err, DoStoreL(aCalEntryList, aNumSuccessfulEntry));
		if (err != KErrNone)
			{
			_DBGLOG_BASIC(AgmDebug::DebugLog("StoreL: Failed - Rolling Back, Leaving with error - %d",err);)
			
			// If store fails, it roll back the last entry which wasn't stored sucessfully
			iAgendaServer.Rollback(iCalSessionImpl.CollectionId());
			User::Leave(err);
			}
		
		if (count > KNotificationThreshold)
			{
			CleanupStack::PopAndDestroy(); // enable Notification
			}
			
		if (count > KAlarmQueueThreshold)
			{
			CleanupStack::PopAndDestroy(); // enable alarm queing
			}
		
		_DBGLOG_BASIC(AgmDebug::DebugLog("StoreL: Successfully stored - %d entries",aNumSuccessfulEntry);)
		}
	}
	
void CCalEntryViewImpl::DoStoreL(const RPointerArray<CCalEntry>& aCalEntryList, TInt& aNumSuccessfulEntry)
	{
	const TInt KCount = aCalEntryList.Count();

	for(TInt i = 0; i < KCount; ++i)
		{
		CCalEntry* calEntry = aCalEntryList[i];
		CAgnEntry* agnEntry = calEntry->Impl()->GetFullEntryL();
		agnEntry->AdjustStartUntilTimeForRepEntryL();
		iAgendaServer.AddEntryL(*agnEntry, iCalSessionImpl.FileId());
		aNumSuccessfulEntry = i+1;
		iAgendaServer.CommitL(iCalSessionImpl.CollectionId());
		}
	}
	
void CCalEntryViewImpl::UpdateL(const RPointerArray<CCalEntry>& aCalEntryList, TInt& aNumSuccessfulEntry)
	{
	TInt count(aCalEntryList.Count());

	_DBGLOG_BASIC(AgmDebug::DebugLog("UpdateL: Client side request to update %d entries",count);)
	
	if (count > 0)
		{
		// alarm queuing disabled for bulk operations
		// we don't need alarm to be requeued after each entry
		// only at the end
		if (count > KAlarmQueueThreshold)
			{
			StopAlarmUpdatesLC();
			}

		if (count > KNotificationThreshold)
			{
			StopNotificationLC();
			}

		TRAPD(err, DoUpdateL(aCalEntryList, aNumSuccessfulEntry));
		if (err != KErrNone)
			{
			_DBGLOG_BASIC(AgmDebug::DebugLog("UpdateL: Failed - Rolling Back, Leaving with error - %d",err);)
			
			// If update fails, it roll back the last entry which wasn't updated sucessfully
			iAgendaServer.Rollback(iCalSessionImpl.CollectionId());
			User::Leave(err);
			}
	
		if (count > KNotificationThreshold)
			{
			CleanupStack::PopAndDestroy(); // enable Notification
			}

		if (count > KAlarmQueueThreshold)
			{
			CleanupStack::PopAndDestroy(); // enable alarm queing
			}

			_DBGLOG_BASIC(AgmDebug::DebugLog("UpdateL: Successfully updated - %d entries",aNumSuccessfulEntry);)
		}
	}
	
void CCalEntryViewImpl::DoUpdateL(const RPointerArray<CCalEntry>& aCalEntryList, TInt& aNumSuccessfulEntry)
	{
	const TInt KCount = aCalEntryList.Count();
	CAgnEntry* agnentry = NULL;
	for(TInt i = 0; i < KCount; ++i)
		{
		agnentry = aCalEntryList[i]->Impl()->GetFullEntryL();
		agnentry->AdjustStartUntilTimeForRepEntryL();
		if(agnentry->GsDataType() == CGsData::EChild)
			{
			_DBGLOG_BASIC(AgmDebug::DebugLog("DoUpdateL: Cannot update a child entry, hence leaving with KErrArgument");)
			
			// This is a child gs entry so leave
			User::Leave(KErrArgument);
			}
		
		iAgendaServer.UpdateEntryL(*agnentry, EFalse, iCalSessionImpl.CollectionId());
		aNumSuccessfulEntry = i+1;
		iAgendaServer.CommitL(iCalSessionImpl.CollectionId());
		}
	}

void CCalEntryViewImpl::DeleteL(const CDesC8Array& aUidList)
	{
	const TInt KCount(aUidList.Count());	
	
	_DBGLOG_BASIC(AgmDebug::DebugLog("DeleteL: Client side request to delete using a UID list. List count %d ",KCount);)
		
	if (KCount > 0)
		{
		// alarm queuing disabled for bulk operations
		// we don't need alarm to be requeued after each entry
		// only at the end
		if (KCount > KAlarmQueueThreshold)
			{
			StopAlarmUpdatesLC();
			}
		
		if (KCount > KNotificationThreshold)
			{
			StopNotificationLC();
			}

		TRAPD(err, DoDeleteByGuidArrayL(aUidList));
		if (err != KErrNone)
			{
			_DBGLOG_BASIC(AgmDebug::DebugLog("DeleteL: Failed - Rolling Back, Leaving with error - %d",err);)
			
			// If delete by guid fails, it could be a file error or bad data. However, there is no way to 
			// tell the user that this has partially succeeded, so just rollback and leave.
			iAgendaServer.Rollback(iCalSessionImpl.CollectionId());
			User::Leave(err);
			}
	
		if (KCount > KNotificationThreshold)
			{
			CleanupStack::PopAndDestroy(); // enable Notification
			}
		
		if (KCount > KAlarmQueueThreshold)
			{
			CleanupStack::PopAndDestroy(); // ResetAlarmUpdates is called which enables alarm queing
			}	
		}
	}

void CCalEntryViewImpl::DoDeleteByGuidArrayL(const CDesC8Array& aUidList)
	{
	const TInt KCount(aUidList.Count());	
	for(TInt i(0); i < KCount; ++i)
		{
		const TDesC8& tempUid = aUidList[i];
		if (i == KCount - 1)
			{
			// If this is the last delete, commit and notify. Must only notify about this after the last operation
			// because if it fails before the end, it should be rolled back.
			iAgendaServer.DeleteEntryByGuidL(tempUid, ETrue, iCalSessionImpl.FileId());
			}
		else	
			{
			iAgendaServer.DeleteEntryByGuidL(tempUid, EFalse, iCalSessionImpl.FileId());
			}
		}
	}

void CCalEntryViewImpl::DeleteL(const CCalEntry& aCalEntry)
	{
	TAgnEntryId entryId = aCalEntry.Impl()->GetFullEntryL()->EntryId();
	
	_DBGLOG_BASIC(AgmDebug::DebugLog("DeleteL: Client side request to delete using an entry. Entry Id is %d", entryId.Value());)
	
	TRAPD(err, DoDeleteEntryL(entryId));
	if (err != KErrNone)
		{
		_DBGLOG_BASIC(AgmDebug::DebugLog("DeleteL: Failed - Rolling Back, Leaving with error - %d",err);)
		
		// If the delete fails, rollback and leave.
		iAgendaServer.Rollback(iCalSessionImpl.CollectionId());
		User::Leave(err);
		}
	}

void CCalEntryViewImpl::DoDeleteEntryL(const TAgnEntryId& aEntryId)
	{
	iAgendaServer.DeleteEntryL(aEntryId, iCalSessionImpl.CollectionId());
	iAgendaServer.CommitL(iCalSessionImpl.CollectionId());
	}

void CCalEntryViewImpl::DeleteL(const CalCommon::TCalTimeRange& aCalTimeRange, CalCommon::TCalViewFilter aFilter, MCalProgressCallBack& aProgressCallBack)
	{
	_DBGLOG_BASIC(AgmDebug::DebugLog("DeleteL: Client side request for an asynchronous delete");)
	
	iCalSessionImpl.AsyncTaskManager().StartDeleteEntryL(aProgressCallBack, aCalTimeRange, aFilter);
	}

	
void CCalEntryViewImpl::DeleteL(const RArray<TCalLocalUid>& aIds, TInt& aNumSuccessfulEntry)
	{
	const TInt KIdsCount(aIds.Count());

	_DBGLOG_BASIC(AgmDebug::DebugLog("DeleteL: Client side request to delete using a list of local uids. 	List count %d ",KIdsCount);)
	
	if (KIdsCount > 0)
		{
		// alarm queuing disabled for bulk operations
		// we don't need alarm to be requeued after each entry
		// only at the end
		if (KIdsCount > KAlarmQueueThreshold)
			{
			StopAlarmUpdatesLC();
			}

		if (KIdsCount > KNotificationThreshold)
			{
			StopNotificationLC();
			}

		TRAPD(err, DoDeleteByLocalUidArrayL(aIds));
		if (err != KErrNone)
			{
			_DBGLOG_BASIC(AgmDebug::DebugLog("DeleteL: Failed - Rolling Back, Leaving with error - %d",err);)
			
			// If the delete fails, it must be due to a file error, so rollback and leave. 
			// Note it is not possible for this operation to partially succeed. If a local UID cannot be found, 
			// it is assumed to be deleted already and ignored.
			iAgendaServer.Rollback(iCalSessionImpl.CollectionId());
			aNumSuccessfulEntry = 0;
			User::Leave(err);
			}
		
		aNumSuccessfulEntry = aIds.Count();
		
		if (KIdsCount > KNotificationThreshold)
			{
			CleanupStack::PopAndDestroy(); // enable Notification
			}

		if (KIdsCount > KAlarmQueueThreshold)
			{
			CleanupStack::PopAndDestroy(); // ResetAlarmUpdates is called which enables alarm queing
			}
		
		_DBGLOG_BASIC(AgmDebug::DebugLog("DeleteL: Successfully deleted - %d entries",aNumSuccessfulEntry);)
		}
	}

void CCalEntryViewImpl::DoDeleteByLocalUidArrayL(const RArray<TCalLocalUid>& aIds)
	{
	_DBGLOG_BASIC(AgmDebug::DebugLog("DoDeleteByLocalUidArrayL: Client side request to delete using a Local UID list. List count %d ",aIds.Count());)
	
	iAgendaServer.DeleteEntryL(aIds, iCalSessionImpl.CollectionId());
	iAgendaServer.CommitL(iCalSessionImpl.CollectionId());
	}

void CCalEntryViewImpl::StopAlarmUpdatesLC()
	{
	CleanupStack::PushL( TCleanupItem(ResetAlarmUpdates, this) );
	User::LeaveIfError(iAgendaServer.SetUpdateAlarm(EFalse, iCalSessionImpl.FileId()));
	}

// cleanup item which re-enables alarm updates
void CCalEntryViewImpl::ResetAlarmUpdates(TAny* aPtr)
	{
	CCalEntryViewImpl* calEntryViewImpl = static_cast<CCalEntryViewImpl*>(aPtr);
	if (calEntryViewImpl)
		{
		calEntryViewImpl->iAgendaServer.SetUpdateAlarm(ETrue, calEntryViewImpl->iCalSessionImpl.FileId());
		}
	}

void CCalEntryViewImpl::StopNotificationLC()
	{
	CleanupStack::PushL( TCleanupItem(ResetNotification, this) );
	iAgendaServer.SetEnablePubSubNotificationsL(EFalse, iCalSessionImpl.FileId());
	iAgendaServer.DisableChangeBroadcast(iCalSessionImpl.FileId());
	}

// cleanup item which re-enables alarm updates
void CCalEntryViewImpl::ResetNotification(TAny* aPtr)
	{
	CCalEntryViewImpl* calEntryViewImpl = static_cast<CCalEntryViewImpl*>(aPtr);
	if (calEntryViewImpl)
		{
		TRAP_IGNORE(calEntryViewImpl->iAgendaServer.SetEnablePubSubNotificationsL(ETrue, calEntryViewImpl->iCalSessionImpl.FileId()));
		TRAP_IGNORE(calEntryViewImpl->iAgendaServer.EnableChangeBroadcast(calEntryViewImpl->iCalSessionImpl.FileId()));
		}
	}
	
