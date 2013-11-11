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

#include "calclient.h"
#include "agmentry.h"
#include "agmutil.h"
#include "calcommonimpl.h"
#include "calentryimpl.h"
#include <calinstance.h>
#include <calsession.h>
#include "calsessionimpl.h"
#include "agmdebug.h"
#include "agmdate.h"
#include "calinstanceiteratorall.h"
#include "calinstanceiteratoruid.h"
#include "agmallocator.h"
#ifdef SYMBIAN_CALENDAR_ENHANCEDSEARCHANDSORT
#include "calsortedinstanceiterator.h"
#endif
#include <s32strm.h>

#ifdef SYMBIAN_SKIPPED_CALENDAR_ALARMS
#include <asshdalarm.h>
#endif
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <asshdalarmcal.h>
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

CCalInstanceViewImpl* CCalInstanceViewImpl::NewL(CCalSession& aSession, MCalProgressCallBack* aProgressCallBack)
	{
	CCalInstanceViewImpl* self = new (ELeave) CCalInstanceViewImpl(aSession.Impl().Server());
	CleanupStack::PushL(self);
	self->ConstructL(aProgressCallBack, aSession);
	CleanupStack::Pop(self);
	return self;
	}

CCalInstanceViewImpl::CCalInstanceViewImpl(RAgendaServ&	aAgendaServer)
	:iAgendaServer(aAgendaServer)
	{
	}

void CCalInstanceViewImpl::ConstructL(MCalProgressCallBack* aProgressCallBack, CCalSession& aSession)
	{
	iCalSessions.AppendL(&(aSession.Impl()));
	aSession.Impl().IncrementReferenceCount();
	if (aProgressCallBack)
		{
		iCalSessions[0]->AsyncTaskManager().StartBuildIndexL(*aProgressCallBack);	
		}
	else if (! iCalSessions[0]->AsyncTaskManager().IsBusy() || iCalSessions[0]->AsyncTaskManager().BuildingIndex())
		{//If it is not busy or it is building index, we will ask the server to build index. Otherwise, e.g. the server is doing filtering category, we will do nothing because index must have been built already.
		iAgendaServer.WaitUntilLoadedL(iCalSessions[0]->CollectionId());
		}
	}

CCalInstanceViewImpl* CCalInstanceViewImpl::NewL(RPointerArray<CCalSession>& aSessions, MCalProgressCallBack* aProgressCallBack)
	{
	CCalInstanceViewImpl* self = new (ELeave) CCalInstanceViewImpl(aSessions[0]->Impl().Server());
	CleanupStack::PushL(self);
	self->ConstructL(aSessions, aProgressCallBack);
	CleanupStack::Pop(self);
	return self;
	}
void CCalInstanceViewImpl::ConstructL(RPointerArray<CCalSession>& aSessions, MCalProgressCallBack* aProgressCallBack)
	{
	TInt count = aSessions.Count();
	__ASSERT_ALWAYS(count >0, User::Leave(KErrNotFound));
	
	for (TInt i=0; i<count; ++i)
		{
		aSessions[i]->Impl().Server();
		__ASSERT_ALWAYS(&(aSessions[i]->Impl().Server())==&iAgendaServer, CalUtils::Panic(EInvalidServerHandle));
		iCalSessions.AppendL(&(aSessions[i]->Impl()));
		iCalSessions[i]->IncrementReferenceCount();
	
		if (!aProgressCallBack && (! iCalSessions[i]->AsyncTaskManager().IsBusy() || iCalSessions[i]->AsyncTaskManager().BuildingIndex()))
			{
			iAgendaServer.WaitUntilLoadedL(iCalSessions[i]->CollectionId());
			}
		}
	if (aProgressCallBack)
		{
		if(count == 1)
			{
			iCalSessions[0]->AsyncTaskManager().StartBuildIndexL(*aProgressCallBack);
			}
		else
			{
			iCurrentFileToOpen = 0;
			iCalProgressCallBack = aProgressCallBack;
			iCalSessions[iCurrentFileToOpen]->AsyncTaskManager().StartBuildIndexL(*this);
			}
		}
	}

void CCalInstanceViewImpl::Progress(TInt aPercentageCompleted)
	{
	iCalProgressCallBack->Progress((aPercentageCompleted + iCurrentFileToOpen*100)/iCalSessions.Count());
	}

void CCalInstanceViewImpl::Completed(TInt aError)
	{
	//if current file is not the last one, start to build the next file
	//otherwise complete aProgressCallBack  
	if(++iCurrentFileToOpen < iCalSessions.Count() && aError == KErrNone)
		{
		TRAPD(err, iCalSessions[iCurrentFileToOpen]->AsyncTaskManager().StartBuildIndexL(*this));
		if(err!= KErrNone)
			{
			iCalProgressCallBack->Completed(err);
			}
		}
	else
		{
		iCalProgressCallBack->Completed(aError);
		}
	}

TBool CCalInstanceViewImpl::NotifyProgress()
	{
	//get from aProgressCallBack
	return iCalProgressCallBack->NotifyProgress();
	}

CCalSessionImpl& CCalInstanceViewImpl::CalSessionImpl() const
	{//This method is not needed after multiple calendar support. However, we have to provide CCalSessionImpl for BC reason (CCalInstanceIterator::iSessionImpl)
	return *iCalSessions[0];
	}

CCalInstanceViewImpl::~CCalInstanceViewImpl()
	{
	const TInt count = iCalSessions.Count();
	for(TInt ii =0; ii< count; ++ii)
		{
		if(iCalSessions[ii]->AsyncTaskManager().BuildingIndex())
			{
			iCalSessions[ii]->AsyncTaskManager().CancelAsynTask();
			}
		iCalSessions[ii]->DecrementReferenceCount();
		}

	iCalSessions.Close();
	}
	
void CCalInstanceViewImpl::FindInstanceL(RPointerArray<CCalInstance>& aInstanceList, 
										 CalCommon::TCalViewFilter aCalViewFilter, 
										 const CalCommon::TCalTimeRange& aTimeRange) const
	{
	CCalInstanceView::TCalSearchParams params(KNullDesC(), CalCommon::EFoldedTextSearch);
	FindInstanceL(aInstanceList, aCalViewFilter, aTimeRange, params);
	}

void CCalInstanceViewImpl::DeleteL(CCalInstance* aInstance, CalCommon::TRecurrenceRange aWhichInstances)
	{
	__ASSERT_ALWAYS(aInstance, CalUtils::Panic(ENullInstanceParameter));
	
	CAgnEntry* entryToDelete = aInstance->Entry().Impl()->GetFullEntryL();
	if (entryToDelete)
		{
		TAgnCalendarTime instanceTime = CalUtils::TCalTimeToTAgnCalendarTimeL(aInstance->Time());
		DeleteInstanceL(*entryToDelete, instanceTime, aWhichInstances);
		}
	
	delete aInstance;
	}

void CCalInstanceViewImpl::DeleteInstanceL(CAgnEntry& aEntry, const TAgnCalendarTime& aInstanceTime, CalCommon::TRecurrenceRange aWhichInstances)
	{
	if (aEntry.EntryId().IsNullId() || (aEntry.RptDef() && !aEntry.RptDef()->IsAnInstanceL(aInstanceTime.LocalL())))
		{
		#if defined (__CAL_INSTANCE_LOGGING__) || (__CAL_VERBOSE_LOGGING__)
			{
			if(aEntry.EntryId().IsNullId())
				{
				_DBGLOG_INSTANCE(AgmDebug::DebugLog("Panic: EAgmErrInvalidInstance - Attempting to delete an instance where the entry Id is NULL");)	
				}
			else if (aEntry.RptDef() && !aEntry.RptDef()->IsAnInstanceL(aInstanceTime.LocalL()))
				{
				_DBGLOG_INSTANCE(AgmDebug::DebugLog("Panic: EAgmErrInvalidInstance - Attempting to delete an instance which is NOT part of the repeat definition");)
				
				TBuf<KMinTTimeStrLength> instTimeBuf;
				AgmDebug::TTimeStrL(aInstanceTime.LocalL(), instTimeBuf);
				_DBGLOG_INSTANCE(AgmDebug::DebugLog("Panic: EAgmErrInvalidInstance - Attempting to delete an instance which is NOT part of the repeat definition. Instance local time = %S", &instTimeBuf);)
				}
			
			_DBGLOG_BASIC(AgmDebug::DebugLog("Panic: EAgmErrInvalidInstance - Attempting to delete an invalid instance");)
			}
		#endif
		DBG_PANIC(EAgmErrInvalidInstance);
		return;
		}
		
	TCalLocalUid uniqId = aEntry.LocalUid();
	CAgnRptDef* rptDef = NULL;
	TBool isRepeating = EFalse;
	if (aEntry.RptDef())
		{
		isRepeating = ETrue;
		rptDef = CAgnRptDef::NewL(aEntry);
		CleanupStack::PushL(rptDef);
		rptDef->CopyL(*aEntry.RptDef());
		}
	const TAgnCalendarTime& KStart = aEntry.StartTime();
	const TAgnCalendarTime& KEnd = aEntry.EndTime();

	TRAPD(ret,DoDeleteInstanceL(aEntry, aInstanceTime, aWhichInstances));
	if (ret == KErrNone)
		{
		TRAP(ret, iAgendaServer.CommitL(aEntry.CollectionId()));
		if (ret != KErrNone)
			{
			iAgendaServer.Rollback(aEntry.CollectionId());
			}
		}
	if (ret != KErrNone)
		{
		// restore start date, end date and repeat definition
		aEntry.SetStartAndEndTimeL(KStart, KEnd);

		if (isRepeating)
			{
			aEntry.SetRptDefL(*rptDef);
			CleanupStack::PopAndDestroy();
			}
		User::Leave(ret);
		}
	if (isRepeating)
		CleanupStack::PopAndDestroy();
	}
/*
@capability WriteUserData
@capability ReadUserData
*/
void CCalInstanceViewImpl::DoDeleteInstanceL(CAgnEntry& aEntry, const TAgnCalendarTime& aInstanceTime, CalCommon::TRecurrenceRange aWhichInstances)
	{
	if (aEntry.RptDef())
		{
		switch(aWhichInstances)
			{
		case CalCommon::EThisOnly:
			{
			// if adding exception to the entry would result in all its instances
			// being excepted then delete from the store, otherwise add the exception and 
			// update it in the store
			if ( ! aEntry.HasChildren() && aEntry.RptDef()->Exceptions() && 
				 aEntry.RptDef()->InstanceCountL() == aEntry.RptDef()->Exceptions()->Count()+1)
				{
				iAgendaServer.DeleteEntryL(aEntry.EntryId(), aEntry.CollectionId());
				}
			else
				{
				aEntry.RptDef()->AddExceptionL(aInstanceTime);
				iAgendaServer.UpdateEntryL(aEntry, EFalse, aEntry.CollectionId());
				}
			break;
			}

		case CalCommon::EThisAndAll:
			{
			iAgendaServer.DeleteEntryL(aEntry.EntryId(), aEntry.CollectionId());
			break;
			}

		case CalCommon::EThisAndFuture:
			{
			TTime previousInstance;
			if ( ! aEntry.RptDef()->NudgePreviousUnexceptedInstanceL(aInstanceTime.LocalL(),previousInstance))
				{
				iAgendaServer.DeleteEntryL(aEntry.EntryId(), aEntry.CollectionId());
				}
			else
				{
				TAgnCalendarTime prevInst;
				if (aEntry.TimeMode() == MAgnCalendarTimeMode::EFloating)
					{
					prevInst.SetFloatingL(previousInstance);
					}
				else
					{
					prevInst.SetLocalL(previousInstance);
					}
				if (aEntry.RptDef()->RRule())
					{
					aEntry.RptDef()->SetUntilTime(prevInst);
					}
				aEntry.RptDef()->PruneExceptionsL();
				if (aEntry.RptDef()->InstanceCountL()==1)	 // see if there's just one instance left
					{	
					// if that one instance is an exception then delete the entry 		
					if (aEntry.RptDef()->Exceptions())
						{
						iAgendaServer.DeleteEntryL(aEntry.EntryId(), aEntry.CollectionId());
						}
					// otherwise make the entry non-repeating
					else
						{
						aEntry.ClearRepeat();
						iAgendaServer.UpdateEntryL(aEntry, EFalse, aEntry.CollectionId());
						}
					}
				else
					iAgendaServer.UpdateEntryL(aEntry, EFalse, aEntry.CollectionId());
				}
			break;
			}
		case CalCommon::EThisAndPrior: 
			{
			TTime nextInstance;
			if (!aEntry.RptDef()->NudgeNextInstanceL(aInstanceTime.LocalL(),nextInstance, ETrue))
				iAgendaServer.DeleteEntryL(aEntry.EntryId(), aEntry.CollectionId());
			else
				{
				aEntry.MoveStartTimeLocalL(nextInstance);
				aEntry.RptDef()->PruneExceptionsL();
				if (aEntry.RptDef()->InstanceCountL()==1)	 // see if there's just one instance left
					{	
					// if that one instance is an exception then delete the entry 		
					if (aEntry.RptDef()->Exceptions())
						{
						iAgendaServer.DeleteEntryL(aEntry.EntryId(), aEntry.CollectionId());
						}
					// otherwise make the entry non-repeating
					else
						{
						aEntry.ClearRepeat();
						iAgendaServer.UpdateEntryL(aEntry, EFalse, aEntry.CollectionId());
						}
					}
				else
					iAgendaServer.UpdateEntryL(aEntry, EFalse, aEntry.CollectionId());
				}
			break;
			}
		default: 
			{
			_DBGLOG_BASIC(AgmDebug::DebugLog("Panic: EAgmErrInvalidRecurrenceRange - Invalid Recurrence Range");)
			DBG_PANIC(EAgmErrInvalidRecurrenceRange);
			}
			}
		}
	else // the entry isn't repeating
		iAgendaServer.DeleteEntryL(aEntry.EntryId(), aEntry.CollectionId());

	}

// This function creates CCalLiteEntries from a list of entry ids
void CCalInstanceViewImpl::FetchLiteEntriesFromIdsL(RArray<TAgnInstance>& aEntryIds, RPointerArray<CCalLiteEntry>& aCalLiteEntries, TLinearOrder<CCalLiteEntry>* aOrder) const
	{
	RPointerArray<CAgnSimpleEntry> simpleEntries;
	CleanupClosePushL(simpleEntries);
	//for cleanup of the sortentries in the array if a leave happens, CleanupClosePushL is not enough.
	//Cleanup function needs access to non-static member iCalSession so that it can use
	//AgnEntryModel::DeleteLiteEntry to cleanup sortentries.
	TCleanSimpleEntryArray cleanupSimpleEntryArray(simpleEntries, iAgendaServer);
	CleanupStack::PushL(TCleanupItem(DestroySimpleEntryArray, &cleanupSimpleEntryArray));
	
	// first fetch the entries as CAgnSimpleEntries
	iAgendaServer.GetSimpleEntryL(aEntryIds, simpleEntries);
	__ASSERT_ALWAYS(aEntryIds.Count() == simpleEntries.Count(), User::Leave(KErrNotFound));

	// create CCalLiteEntries from the CAgnSimpleEntries
	// entries must be added to the CCalLiteEntry array in the same order they appeared in the other arrays
	while (simpleEntries.Count() > 0)
		{
		CCalLiteEntry* calLiteEntry = CCalLiteEntry::NewL(*simpleEntries[0], iAgendaServer);
		calLiteEntry->IncrementRefCount();
		simpleEntries.Remove(0); // CCalLiteEntry has taken ownership of CAgnSimpleEntry
		TInt err = 0;
		if(aOrder)
			{
			err = aCalLiteEntries.InsertInOrder(calLiteEntry, *aOrder);	
			}
		else
			{
			err = aCalLiteEntries.Append(calLiteEntry);	
			}

		if (err != KErrNone)
			{
			calLiteEntry->DecrementRefCount(); // deletes calLiteEntry
			User::Leave(err);
			}
		}
	
	CleanupStack::Pop(&cleanupSimpleEntryArray);
	CleanupStack::PopAndDestroy(&simpleEntries); // Close()
	}

CArrayFixSeg<TAgnInstance>* CCalInstanceViewImpl::FindInstanceIdL(const CCalFindInstanceSettings& aCalFindInstanceSettings) const
	{
	aCalFindInstanceSettings.Filter();
	const TInt KCalMaxSearchStringLength = 32;
	if (aCalFindInstanceSettings.Text() != KNullDesC && aCalFindInstanceSettings.Text().Length() > KCalMaxSearchStringLength)
		{
		User::Leave(KErrArgument);
		}
	
	
	TAgnFilter agnFilter(aCalFindInstanceSettings.Filter(), aCalFindInstanceSettings.Behaviour());
	
	TFindInstanceParams params;
	params.iFilter = agnFilter;
	params.iUndatedTodoTimeLocal.HomeTime();
	params.iRangeStart = CalUtils::TCalTimeToTAgnCalendarTimeL(aCalFindInstanceSettings.TimeRange().StartTime());
	params.iRangeEnd = CalUtils::TCalTimeToTAgnCalendarTimeL(aCalFindInstanceSettings.TimeRange().EndTime());
	params.iSearchString.Copy(aCalFindInstanceSettings.Text());
	
	CArrayFixSeg<TAgnInstance>* instances = new (ELeave) CArrayFixSeg<TAgnInstance>(16);
 	CleanupStack::PushL(instances);
 
	// Find the instances
 	RArray<TInt64> fileIds;
 	GetFileIdLC(fileIds);
  	iAgendaServer.FindInstancesL(fileIds,*instances, params);
 	CleanupStack::PopAndDestroy(&fileIds);
 	CleanupStack::Pop(instances);
 	return instances;
 	}

CCalSessionImpl* CCalInstanceViewImpl::FindSession(TCalCollectionId aCallectionId) const
	{
	const TInt count = iCalSessions.Count();
	for(TInt ii = 0; ii< count; ++ii)
		{
		if(iCalSessions[ii]->CollectionId() == aCallectionId)
			{
			return iCalSessions[ii];
			}
		}
	return NULL;
	}
 	
 CCalInstance* CCalInstanceViewImpl::CreateInstanceFromLiteEntryL(CCalLiteEntry& aLiteEntry, TAgnInstanceId& aInstanceId, TBool aCheckInstanceTime) const
 	{
 	CCalSessionImpl* session = FindSession(aLiteEntry.LiteEntry().CollectionId());
	__ASSERT_ALWAYS(session, User::Leave(KErrNotFound));
 	CCalEntryImpl* calEntryImpl = CCalEntryImpl::NewL(aLiteEntry, *session);
	CleanupStack::PushL(calEntryImpl);
	CCalEntry* calEntry = CCalEntry::NewL(calEntryImpl);
	CleanupStack::Pop(calEntryImpl);
	
	CleanupStack::PushL(calEntry);
	TCalTime instanceTime = CalUtils::TAgnCalendarTimeToTCalTimeL(aInstanceId.Date());

	CCalInstance* calInstance = NULL;
	TBool instanceTimeChanged = EFalse;

	if (aCheckInstanceTime)
		{// When instances are fetched using iterator, the instance ids is cached at the begining when the iterator is created. Therefore, the entry might has been changed by the time the instance is returned to the user.
		const CAgnSimpleEntry& KSimpleEntry = aLiteEntry.LiteEntry();
 		const CAgnRptDef* KRepeating = KSimpleEntry.RptDef();
 		if(KRepeating)
	 		{
	 		instanceTimeChanged = ! KRepeating->IsAnInstanceL(instanceTime.TimeLocalL());	
	 		}
	 	else //compare the instance time with the entry's start\end time depends on it is a todo or not
	 		{	
	 		instanceTimeChanged = KSimpleEntry.EntryTime() != aInstanceId.Date();
	 		}
 		}

	if (instanceTimeChanged)
		{
		User::Leave(KErrNotFound);	
		}
	else
		{
		calInstance= CCalInstance::NewL(calEntry, instanceTime);
		CleanupStack::Pop(calEntry);
		}
	return 	calInstance;	
 	}
 TInt CCalInstanceViewImpl::CompareInstanceId(const TAgnInstance& aLeft, const TAgnInstance& aRight)
 //
 // Compare two entries - used when sorting entries in the array
 //
 	{

 	if ( aLeft.iId.Value() == aRight.iId.Value() &&  aLeft.iCollectionId == aRight.iCollectionId)
 		{
 		return 0;
 		}

 	if(aLeft.iId.Value() < aRight.iId.Value())
 		{
 		return -1;
 		}
 	else if(aLeft.iId.Value() > aRight.iId.Value())
 		{
 		return 1;
 		}
 	else
 		{
 		return aLeft.iCollectionId  < aRight.iCollectionId ? -1 : 1;
 		}
  	}

void CCalInstanceViewImpl::FindInstanceL(RPointerArray<CCalInstance>& aMatchedInstanceList, 
										 CalCommon::TCalViewFilter aCalViewFilter, 
										 const CalCommon::TCalTimeRange& aTimeRange, 
										 const CCalInstanceView::TCalSearchParams& aSearchParams) const
	{
 	CCalFindInstanceSettings* findSetting = CCalFindInstanceSettings::NewL(aCalViewFilter, aTimeRange);
 	CleanupStack::PushL(findSetting);
 	findSetting->SetTextSearchL(aSearchParams);
 	CArrayFixSeg<TAgnInstance>* instances = FindInstanceIdL(*findSetting);
 	CleanupStack::PushL(instances);
 	FindInstancesFromIdsL(instances, aMatchedInstanceList); 	
	CleanupStack::PopAndDestroy(2, findSetting);//findSetting, instances
	}

void CCalInstanceViewImpl::FindInstancesFromIdsL(CArrayFixSeg<TAgnInstance>* aInstanceIds, RPointerArray<CCalInstance>& aInstanceArray) const
	{
	if (aInstanceIds->Count() > 0)
		{
		RArray<TAgnInstance> liteEntryIdsToFetch; // array of entry IDs
		CleanupClosePushL(liteEntryIdsToFetch);
	
		// Create a list of all entry IDs in the set of instances
		const TInt KInstancesCount(aInstanceIds->Count());
		TLinearOrder<TAgnInstance> order(CompareInstanceId);
		for (TInt i(0); i < KInstancesCount ; ++i)
			{
			TInt err = liteEntryIdsToFetch.InsertInOrder((*aInstanceIds)[i], order); // doesn't allow repeats
			if (err != KErrAlreadyExists)
				{
				User::LeaveIfError(err);
				}
			}
		
		RPointerArray<CCalLiteEntry> calLiteEntries; // array of light entries
		CleanupStack::PushL(TCleanupItem(CleanCalLiteEntries, &calLiteEntries));
		
		// Fetch the light entry for each entry ID
		FetchLiteEntriesFromIdsL(liteEntryIdsToFetch, calLiteEntries, NULL);
		const TInt KCalEntryCount = calLiteEntries.Count();
		
		// array of light entries must match array of entry IDs
		__ASSERT_ALWAYS(liteEntryIdsToFetch.Count() == KCalEntryCount, User::Leave(KErrNotFound));
		
		for (TInt i(0); i < KInstancesCount ; ++i)
	 		{
	 		// for each instance, find the index of the entry id
	 		TInt index = liteEntryIdsToFetch.FindInOrder((*aInstanceIds)[i], order);
	  		User::LeaveIfError(index);
			
 			// Use the index to find the CCalLiteEntry for this instance and create the CCalInstance from it
			CCalInstance* calInstance = CreateInstanceFromLiteEntryL(*calLiteEntries[index], (*aInstanceIds)[i].iId, EFalse);
			CleanupStack::PushL(calInstance);
			aInstanceArray.AppendL(calInstance);
			CleanupStack::Pop(calInstance);
			}	
	
		CleanupStack::PopAndDestroy(&calLiteEntries);
		CleanupStack::PopAndDestroy(&liteEntryIdsToFetch);
 		}
	}

/**
*/
CCalInstanceIterator* CCalInstanceViewImpl::FindInstanceL(const CCalFindInstanceSettings& aSettings) const
	{
#ifdef SYMBIAN_CALENDAR_ENHANCEDSEARCHANDSORT
	if(DefaultSortOnly(aSettings))
		{
		return CCalInstanceIteratorAll::NewL(aSettings, *this);
		}
	else
		{
		return CCalSortedInstanceIterator::NewL(aSettings, *this);
		}
#else
	return CCalInstanceIteratorAll::NewL(aSettings, *this);
#endif
	}

CCalInstanceIterator* CCalInstanceViewImpl::FindInstanceL(const CCalFindInstanceSettings& aSettings, TCalLocalUid aLocalUid, const TCalTime& aStartInstanceTime) const
	{
	__ASSERT_ALWAYS(iCalSessions.Count()==1, CalUtils::Panic(EAmbiguousInstance));
	return DoFindInstanceL(aSettings, aLocalUid, aStartInstanceTime, iCalSessions[0]->CollectionId());
	}

CCalInstanceIterator* CCalInstanceViewImpl::FindInstanceL(const CCalFindInstanceSettings& aSettings, TCalInstanceId& aInstanceId) const
	{
	return DoFindInstanceL(aSettings, aInstanceId.iEntryLocalId, aInstanceId.iInstanceTime, aInstanceId.iCollectionId);
	}

CCalInstanceIterator* CCalInstanceViewImpl::DoFindInstanceL(const CCalFindInstanceSettings& aSettings, TCalLocalUid aLocalUid, const TCalTime& aStartInstanceTime, TUint8 aShortFileId) const
	{
#ifdef SYMBIAN_CALENDAR_ENHANCEDSEARCHANDSORT
	if(DefaultSortOnly(aSettings))
		{
		return CCalInstanceIteratorAll::NewL(aSettings, *this, aLocalUid, aStartInstanceTime, aShortFileId);
		}
	else
		{
		return CCalSortedInstanceIterator::NewL(aSettings, *this, aLocalUid, aStartInstanceTime, aShortFileId);
		}
#else
	return CCalInstanceIteratorAll::NewL(aSettings, *this, aLocalUid, aStartInstanceTime, aShortFileId);	
#endif
	}


#ifdef SYMBIAN_CALENDAR_ENHANCEDSEARCHANDSORT
TBool CCalInstanceViewImpl::DefaultSortOnly(const CCalFindInstanceSettings& aSettings)
	{
	//Use the default sort if no sort criteria is found.
	return (0 == aSettings.SortCriteria().Count());
	}
#else
TBool CCalInstanceViewImpl::DefaultSortOnly(const CCalFindInstanceSettings& /*aSettings*/)
	{
	return ETrue;
	}
#endif

CCalInstanceIterator* CCalInstanceViewImpl::FindInstanceByUidL(const TDesC8& aUid, const TCalTime& aInstanceTime) const
	{
	__ASSERT_ALWAYS(iCalSessions.Count()==1, CalUtils::Panic(EAmbiguousInstance));
	return CCalInstanceIteratorUid::NewL(*this, aUid, aInstanceTime, iCalSessions[0]->CollectionId());
	}

CCalInstanceIterator* CCalInstanceViewImpl::FindInstanceByUidL(TCalCollectionId aCollectionId, const TDesC8& aUid, const TCalTime& aInstanceTime) const
	{
	return CCalInstanceIteratorUid::NewL(*this, aUid, aInstanceTime, aCollectionId);
	}

CCalInstance* CCalInstanceViewImpl::FindInstanceL(TCalLocalUid aLocalId, const TCalTime& aInstanceTime) const
	{
	__ASSERT_ALWAYS(iCalSessions.Count()==1, CalUtils::Panic(EAmbiguousInstance));
	return DoFindInstanceL(aLocalId, aInstanceTime, iCalSessions[0]->CollectionId());
	}

CCalInstance* CCalInstanceViewImpl::DoFindInstanceL(TCalLocalUid aLocalId, const TCalTime& aInstanceTime, TUint8 aShortFileId) const
	{
	CAgnSimpleEntry* simpleEntry = iAgendaServer.GetSimpleEntryL(aShortFileId, aLocalId);
 	TCleanSimpleEntry cleanSimpleEntry(*simpleEntry, iAgendaServer.SimpleEntryAllocator());
	TCleanupItem cleanupItem(TCleanSimpleEntry::DestroySimpleEntry, &cleanSimpleEntry);
	CleanupStack::PushL(cleanupItem);
	
	// Leave if the entry cannot be found
	__ASSERT_ALWAYS(simpleEntry, User::Leave(KErrNotFound));
	
	if (!IsValidInstanceL(*simpleEntry, aInstanceTime))
		{
		User::Leave(KErrNotFound);
		}
	
	TCalTime instanceTime(aInstanceTime);
	
	if ( ( !simpleEntry->EndTime().IsSet() ) && ( simpleEntry->CompletedDateUtc() == Time::NullTTime() ) )
		{
		// if the entry is an undated todo then we should use 'now' as the instance time
		TTime now;
		now.HomeTime();
		instanceTime.SetTimeLocalL(now);
		}

	CCalLiteEntry* calLiteEntry = CCalLiteEntry::NewL(*simpleEntry, iAgendaServer);
	CleanupStack::Pop(); // simpleEntry
	
	calLiteEntry->IncrementRefCount();
	
	CCalEntryImpl* calEntryImpl(NULL);
	CCalSessionImpl* session = FindSession(calLiteEntry->LiteEntry().CollectionId());
	__ASSERT_ALWAYS(session, User::Leave(KErrNotFound));
	TRAPD(err, calEntryImpl = CCalEntryImpl::NewL(*calLiteEntry, *session) );
	
	calLiteEntry->DecrementRefCount();
	
	if (err != KErrNone)
		{
		User::Leave(err);
		}
	
	CleanupStack::PushL(calEntryImpl);
	CCalEntry* calEntry = CCalEntry::NewL(calEntryImpl);
	CleanupStack::Pop(calEntryImpl);
	CleanupStack::PushL(calEntry);
	CCalInstance* instance = CCalInstance::NewL(calEntry, instanceTime);
	CleanupStack::Pop(calEntry);
	
	return instance;
	}

CCalInstance* CCalInstanceViewImpl::FindInstanceL(TCalInstanceId& aInstanceId) const
	{
	return DoFindInstanceL(aInstanceId.iEntryLocalId,aInstanceId.iInstanceTime, aInstanceId.iCollectionId);
	}

#ifdef SYMBIAN_SKIPPED_CALENDAR_ALARMS
void CCalInstanceViewImpl::FindAlarmedInstanceL(RPointerArray<CCalInstance>& aAlarmedInstanceList,    
						  						TASShdAlarmedInstanceParams aAlarmedInstanceParams) const
	{
	TAgnFilter filter(CalCommon::EIncludeAll, CalCommon::EFoldedTextSearch);
	filter.SetAlarmedInstanceSearch(ETrue);
	filter.SetFloatingInstanceOnly(aAlarmedInstanceParams.iTimeType == EFloating);
	
	TFindInstanceParams findInstanceParams;
 	findInstanceParams.iFilter = filter;
 	
 	TAgnCalendarTime searchFromTime;
 	searchFromTime.SetLocalL(aAlarmedInstanceParams.iLocalStartTime);
 	findInstanceParams.iRangeStart = searchFromTime;
 	findInstanceParams.iRangeEnd.SetLocalL(aAlarmedInstanceParams.iLocalEndTime); 	
 	
 	const TInt KGranularity(5);
 	CArrayFixSeg<TAgnInstance>* instances = new(ELeave) CArrayFixSeg<TAgnInstance>(KGranularity);
 	CleanupStack::PushL(instances);
 	RArray<TInt64> fileIds;
 	GetFileIdLC(fileIds);
	iAgendaServer.FindInstancesL(fileIds, *instances, findInstanceParams);
	CleanupStack::PopAndDestroy(&fileIds);
	FindInstancesFromIdsL(instances, aAlarmedInstanceList);
	
	CleanupStack::PopAndDestroy(instances);
	}
#endif
void CCalInstanceViewImpl::GetFileIdLC(RArray<TInt64>& aFiles)const
	{
	CleanupClosePushL(aFiles);
	const TInt count = iCalSessions.Count();
	for (TInt ii=0; ii<count; ++ii)
		{
		aFiles.AppendL(iCalSessions[ii]->FileId());
		}
	}

void CCalInstanceViewImpl::GetShortFileIdLC(RArray<TInt>& aFiles)const
	{
	CleanupClosePushL(aFiles);
	const TInt count = iCalSessions.Count();
	for (TInt ii=0; ii<count; ++ii)
		{
		User::LeaveIfError(aFiles.InsertInOrder(iCalSessions[ii]->CollectionId()));
		}
	}

TCalTime CCalInstanceViewImpl::NextInstanceL(CalCommon::TCalViewFilter aCalViewFilter, const TCalTime& aStartDate) const
 	{
 	TFindInstanceParams findInstanceParams;
 	findInstanceParams.iFilter = TAgnFilter(aCalViewFilter, CalCommon::EFoldedTextSearch);
 	
 	TAgnCalendarTime searchFromTime;
 	searchFromTime.SetLocalL(AgnDateTime::ResetToMidnight(aStartDate.TimeLocalL()) + TTimeIntervalDays(1));
 	findInstanceParams.iInstance.iId.SetDate(searchFromTime); 
 	findInstanceParams.iInstance.iCollectionId = 0;
 	findInstanceParams.iUndatedTodoTimeLocal.HomeTime();
 	findInstanceParams.iRangeStart = searchFromTime;
 	findInstanceParams.iRangeEnd.SetUtcL(AgnDateTime::MaxDate());
 	findInstanceParams.iNumInstances = 1;
 	
 	CArrayFixSeg<TAgnInstance>* instances = new(ELeave) CArrayFixSeg<TAgnInstance>(1);
 	CleanupStack::PushL(instances);
	
 	RArray<TInt64> fileIds;
 	GetFileIdLC(fileIds);
	iAgendaServer.NextInstancesL(fileIds, *instances, findInstanceParams);
	CleanupStack::PopAndDestroy(&fileIds);
	TCalTime returnTime;
	if (instances->Count() > 0)
		{
		returnTime = CalUtils::TAgnCalendarTimeToTCalTimeL((*instances)[0].iId.Date());
		}
	
	CleanupStack::PopAndDestroy(instances);
	
 	return returnTime;
	}

TCalTime CCalInstanceViewImpl::PreviousInstanceL(CalCommon::TCalViewFilter aCalViewFilter, const TCalTime& aStartDate) const
 	{
 	TFindInstanceParams findInstanceParams;
 	findInstanceParams.iFilter = TAgnFilter(aCalViewFilter, CalCommon::EFoldedTextSearch);
 	
 	TAgnCalendarTime searchFromTime;
 	// take off a microsecond because we don't want to get the instance that is at midnight on the day
 	searchFromTime.SetLocalL(AgnDateTime::ResetToMidnight(aStartDate.TimeLocalL()));
 	findInstanceParams.iInstance.iId.SetDate(searchFromTime); 
 	findInstanceParams.iInstance.iCollectionId = 0; 
 	findInstanceParams.iUndatedTodoTimeLocal.HomeTime();
 	findInstanceParams.iRangeStart.SetUtcL(AgnDateTime::MinDate());
 	findInstanceParams.iRangeEnd = searchFromTime;
 	findInstanceParams.iNumInstances = 1;
 	
 	CArrayFixSeg<TAgnInstance>* instances = new(ELeave) CArrayFixSeg<TAgnInstance>(1);
 	CleanupStack::PushL(instances);
 	RArray<TInt64> fileIds;
 	GetFileIdLC(fileIds);
 	iAgendaServer.PreviousInstancesL(fileIds, *instances, findInstanceParams);
	CleanupStack::PopAndDestroy(&fileIds);
	
	TCalTime returnTime;
	if (instances->Count() > 0)
		{		
		returnTime = CalUtils::TAgnCalendarTimeToTCalTimeL((*instances)[0].iId.Date());
		}
	
	CleanupStack::PopAndDestroy(instances);
	
 	return returnTime;
	}

/** A utility method to create an instance from an instance id

@param aInstanceId the instance id to create an instance from
@return The instance

@internalComponent
*/
CCalInstance* CCalInstanceViewImpl::CreateInstanceL(const TAgnInstance& aInstance) const
	{
	CAgnSimpleEntry* simpleEntry = iAgendaServer.GetSimpleEntryL(aInstance.iId, aInstance.iCollectionId);
	TCleanSimpleEntry simpleEntryCleanupItem(*simpleEntry, iAgendaServer.SimpleEntryAllocator());
  	CleanupStack::PushL(TCleanupItem(TCleanSimpleEntry::DestroySimpleEntry, &simpleEntryCleanupItem));
	CCalLiteEntry* liteEntry = CCalLiteEntry::NewL(*simpleEntry, iAgendaServer);
	CleanupStack::Pop(); // &simpleEntry
	CleanupStack::PushL(liteEntry);
	CCalSessionImpl* session = FindSession(liteEntry->LiteEntry().CollectionId());
	__ASSERT_ALWAYS(session, User::Leave(KErrNotFound));
	CCalEntryImpl* entryImpl = CCalEntryImpl::NewL(*liteEntry, *session);
	CleanupStack::Pop(liteEntry);
	CleanupStack::PushL(entryImpl);
	CCalEntry* entry = CCalEntry::NewL(entryImpl);
	CleanupStack::Pop(entryImpl);
	CleanupStack::PushL(entry);
	CCalInstance* instance = CCalInstance::NewL(entry, CalUtils::TAgnCalendarTimeToTCalTimeL(aInstance.iId.Date()));
	CleanupStack::Pop(entry);
	return instance;
	}

/** A utility method to create an CCalInstance from its entry and instance time

@param aEntry The entry that the instance belongs to
@param aInstanceTime The instance time of the instance
@return A CCalInstance object 

@internalComponent
*/
CCalInstance* CCalInstanceViewImpl::CreateInstanceL(CCalLiteEntry& aLiteEntry, const TCalTime& aInstanceTime) const
	{	
	CCalSessionImpl* session = FindSession(aLiteEntry.LiteEntry().CollectionId());
	__ASSERT_ALWAYS(session, User::Leave(KErrNotFound));
	CCalEntryImpl* calEntryImpl = CCalEntryImpl::NewL(aLiteEntry, *session);
	CleanupStack::PushL(calEntryImpl);
	CCalEntry* calEntry = CCalEntry::NewL(calEntryImpl);
	CleanupStack::Pop(calEntryImpl);
	CleanupStack::PushL(calEntry);
	CCalInstance* instance = CCalInstance::NewL(calEntry, aInstanceTime);
	CleanupStack::Pop(calEntry);
	return instance;
	}


/** A utility method to find if an instance time is valid

@param aEntry The entry that the instance belongs to
@param aInstanceTime The instance time of the instance
@return ETrue if the instance time is valid.  EFalse of the instance is invalid.

@internalComponent
*/
TBool CCalInstanceViewImpl::IsValidInstanceL(const CAgnSimpleEntry& aEntry, const TCalTime& aInstanceTime) const
	{
	TBool instanceExists(EFalse);
	const CAgnRptDef* rptDef(aEntry.RptDef());
	
	if (rptDef)
		{
		if (rptDef->IsAnUnexceptedInstanceL(aInstanceTime.TimeLocalL()))
			{
			instanceExists = ETrue;
			}
		}
	else
		{
		if (aEntry.Type() == CCalEntry::ETodo)
			{
			if (aEntry.CompletedDateUtc() != Time::NullTTime())
				{
				if (aEntry.CompletedDateUtc() == aInstanceTime.TimeUtcL())
					{
					// it is a completed todo and the completed date is correct
					instanceExists = ETrue;
					}
				}
			else if ( !aEntry.EndTime().IsSet() || ( aEntry.EndTime().LocalL() == aInstanceTime.TimeLocalL() ) )
				{
				// it is an undated todo or the instance time is correct
				instanceExists = ETrue;
				}
			}
		else
			{
			if (aEntry.StartTime().LocalL() == aInstanceTime.TimeLocalL())
				{
				instanceExists = ETrue;
				}
			}
		}
	
	return instanceExists;
	}


//Cleanup Callback function which deletes CAgnSimpleEntries using CCalSessionImpl. 
void CCalInstanceViewImpl::DestroySimpleEntryArray(TAny* aPtr)
    {
	TCleanSimpleEntryArray* ptrCleanup = static_cast<TCleanSimpleEntryArray*> (aPtr);
	const TInt KCount = ptrCleanup->GetCount();
	TInt loop;
	for(loop = 0;loop < KCount;++loop)
		{
		ptrCleanup->GetAgendaServ().DeleteSimpleEntry(ptrCleanup->GetSimpleEntryAt(loop));
		}
	ptrCleanup->CloseArray();
    }

void CCalInstanceViewImpl::CleanCalLiteEntries(TAny* aPtr)
	{
	if (aPtr)
		{
		RPointerArray<CCalLiteEntry>* calLiteEntries = static_cast<RPointerArray<CCalLiteEntry>*>(aPtr);
		const TInt count = calLiteEntries->Count();
		for (TInt i = count - 1; i >= 0; --i)
			{
			(*calLiteEntries)[i]->DecrementRefCount(); // removes reference and deletes if this is the last one
			calLiteEntries->Remove(i);
			}
		calLiteEntries->Reset();
		}
	}

RAgendaServ& CCalInstanceViewImpl::GetServ() const
	{
	return iAgendaServer;
	}
	
void CCalInstanceViewImpl::IncrementsessionReferenceCount()
	{
	const TInt count = iCalSessions.Count();
	for(TInt ii=0; ii< count; ++ii)
		{
		iCalSessions[ii]->IncrementReferenceCount();
		}
	}
void CCalInstanceViewImpl::DecrementSessionReferenceCount()
	{
	const TInt count = iCalSessions.Count();
	for(TInt ii =0; ii< count; ++ii)
		{
		iCalSessions[ii]->DecrementReferenceCount();
		}
	}
TCleanSimpleEntryArray::TCleanSimpleEntryArray(RPointerArray<CAgnSimpleEntry>& aSimpleEntryArray, RAgendaServ& aAgendaServ)
	: iSimpleEntryArray(aSimpleEntryArray), iAgendaServer(aAgendaServ)
	{
	}

CAgnSimpleEntry* TCleanSimpleEntryArray::GetSimpleEntryAt(TInt aPos)
	{
	return iSimpleEntryArray[aPos];
	}

RAgendaServ& TCleanSimpleEntryArray::GetAgendaServ()
	{
	return iAgendaServer;
	}

TInt TCleanSimpleEntryArray::GetCount() const
	{
	return iSimpleEntryArray.Count();
	}
	
void TCleanSimpleEntryArray::CloseArray()
	{
	iSimpleEntryArray.Close();
	}

