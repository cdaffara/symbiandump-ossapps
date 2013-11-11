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

#include "agmsimpleentry.h"
#include "calclient.h"
#include "calcategoryimpl.h"
#include "calentryimpl.h"
#include <calprogresscallback.h>
#include "calsessionimpl.h"
#include "calcategorymanagerimpl.h"
#include "agmdebug.h"

CCalAsyncTaskManager::CCalAsyncTaskManager(CCalSessionImpl& aSessionImpl)
	: CActive(EPriorityStandard),
	iSessionImpl(aSessionImpl)
	{
	CActiveScheduler::Add(this);
	}
	
CCalAsyncTaskManager::~CCalAsyncTaskManager()
	{
	CancelAsynTask();
	iCallBacks.Close();
	delete iEntryIdList;
	}
	
CCalAsyncTaskManager* CCalAsyncTaskManager::NewL(CCalSessionImpl& aSessionImpl)
	{
	CCalAsyncTaskManager* self = new (ELeave) CCalAsyncTaskManager(aSessionImpl);
	return self;
	}

void CCalAsyncTaskManager::StartBuildIndexL(MCalProgressCallBack& aProgressCallBack)
	{
	/*If it is a task of building index, place the callback into the queue, 
	so that all of them can be completed when the building index. 
	*/
	_DBGLOG_ASYNC(AgmDebug::DebugLogTimeStampL("StartBuildIndexL: Commencing async build index");)
	
	if (iAction==EBuildIndex)
		{
		iCallBacks.AppendL(&aProgressCallBack);
		}
	else if (iAction!=ENoAction)
		{
		//The index must have been built if it is doing other asynchronous operations
		aProgressCallBack.Completed(KErrNone);
		}
	else
		{
		iAction = EBuildIndex;
		iCallBacks.AppendL(&aProgressCallBack);

		if (!IsActive())
			{
			SetActive();
			}
		iSessionImpl.Server().StartBuildIndex(aProgressCallBack.NotifyProgress(), iStatus, iSessionImpl.CollectionId());
		}
	}

void CCalAsyncTaskManager::StartDeleteEntryL(MCalProgressCallBack& aProgressCallBack, const CalCommon::TCalTimeRange& aCalTimeRange, CalCommon::TCalViewFilter aFilter)
	{
	/*If it is a task of building index, place the callback into the queue, 
	so that all of them can be completed when the building index. If it is a
	task of deleting entry, simply leave with "busy". 
	*/
	_DBGLOG_ASYNC(AgmDebug::DebugLogTimeStampL("StartDeleteEntryL: Commencing async delete");)
	
	if(iAction!=ENoAction)
		{
		User::Leave(KErrLocked);
		}
		
	TTime todaysDate;
	todaysDate.HomeTime();

	iAction = EDeleteEntry;
	iCallBacks.Append(&aProgressCallBack);

	TAgnFilter agnFilter(aFilter, CalCommon::EFoldedTextSearch);
	TRAPD(err, iSessionImpl.Server().StartTidyByDateL(aProgressCallBack.NotifyProgress(), iStatus, agnFilter, todaysDate, aCalTimeRange, iSessionImpl.CollectionId()));

	if (err != KErrNone)
		{
		iCallBacks.Remove(iCallBacks.Count() - 1);
		iAction = ENoAction;
		User::Leave(err);
		}
	else if (!IsActive())
		{
		SetActive();
		}
	}
	
void CCalAsyncTaskManager::StartCategoryTaskL(MCalProgressCallBack& aProgressCallBack, TAsyncAction aAction, const CCalCategory& aCategory, RPointerArray<CCalEntry>* aFilteredEntry)
	{
	/*If it is a task of building index, place the callback into the queue, 
	so that all of them can be completed when the building index. If it is a
	task of deleting entry, simply leave with "busy". 
	*/
	_DBGLOG_ASYNC(AgmDebug::DebugLogTimeStampL("StartCategoryTaskL: Commencing async category task");)
	
	if(iAction!=ENoAction)
		{
		User::Leave(KErrLocked);
		}
		

	iAction = aAction;
	iCallBacks.Append(&aProgressCallBack);

	if(iAction==EFilterCategory)
		{
		iFilteredEntry = aFilteredEntry;
		}

	TRAPD(err, iSessionImpl.Server().StartCategoryTaskL(aProgressCallBack.NotifyProgress(), iStatus, *(aCategory.Impl()->AgnCategory()), aAction, iSessionImpl.CollectionId()));
	if (err != KErrNone)
		{
		iCallBacks.Remove(iCallBacks.Count() - 1);
		iAction = ENoAction;
		User::Leave(err);
		}
	else if (!IsActive())
		{
		SetActive();
		}
	}

		
void CCalAsyncTaskManager::DoCancel()
	{
	}

void CCalAsyncTaskManager::CancelAsynTask()
	{
	if (iAction != ENoAction)
		{
		iSessionImpl.Server().CancelTask(iSessionImpl.FileId());
		CompleteRequest(KErrCancel);
		Cancel();
		}
	iAction = ENoAction;
	}

void CCalAsyncTaskManager::RunL()
	{
	//If the progress value is 100 or negative it means that the observed process
	//has finished and thereby it should have provided the progress value to report
	//Under normal conditions we use the complete status as a progress parameter (%)
	TInt progress = iStatus.Int();

	//If The ongoing process is not cancelled or finished then request again
	if (progress > 0 && progress < 100)
		{
		TInt count=iCallBacks.Count();
		for(TInt ii=0; ii<count; ++ii)
			{
			if((iCallBacks)[ii])
				{
				(iCallBacks)[ii]->Progress(progress);
				}
			}

		//requesting itself
		if (iAction == EBuildIndex)
			{//When asynchronous operation is to build index, we should call RAgendaServ::StartBuildIndex  for progress requess. 
			iSessionImpl.Server().StartBuildIndex(ETrue, iStatus, iSessionImpl.CollectionId());
			}
		else
			{
			iSessionImpl.Server().RequestProgress(iStatus, iSessionImpl.CollectionId());
			}
		
		SetActive();
		}
	else 
		{
		if (iAction==EFilterCategory && progress==KErrNone)
			{
			TRAP(progress,GetFilteredEntryL());
			}
		CompleteRequest(progress);
		}
	}

void CCalAsyncTaskManager::CompleteRequest(TInt aCompleteCode)
	{
	for(TInt ii=0; ii<iCallBacks.Count(); ++ii)
		{
		if((iCallBacks)[ii])
			{
			iCallBacks[ii]->Completed(aCompleteCode);
			}
		}
	iCallBacks.Reset();
	delete iEntryIdList;
	iEntryIdList=NULL;
	iAction = ENoAction;
	}
	
void CCalAsyncTaskManager::GetFilteredEntryL()
	{// Get the light entry by its entry id and fill in it into the array past in by the client
	iEntryIdList = new (ELeave) CArrayFixSeg<TAgnEntryId> (32);
	CCalCategoryManagerImpl::GetFilteredEntryL(*iEntryIdList, *iFilteredEntry, iSessionImpl);
	delete iEntryIdList;
	iEntryIdList=NULL;
	}

TBool CCalAsyncTaskManager::BuildingIndex() const
	{
	return (iAction == EBuildIndex);
	}
	
TBool CCalAsyncTaskManager::IsBusy() const
	{
	return iAction!=ENoAction;	
	}
