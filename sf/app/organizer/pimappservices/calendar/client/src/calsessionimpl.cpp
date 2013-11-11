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

#include "calsessionimpl.h"

#include "calclient.h"
#include "agmallocator.h"
#include "agmutil.h"
#include "calcommonimpl.h"
#include "calentryimpl.h"
#include "calinstance.h"
#include "agmentry.h"
#include "calcalendarinfo.h"
#include "calcalendarinfoimpl.h"
#include "agmcalendarinfo.h"
#include "calfilechangenotification.h"
#include "agmfilechangenotification.h"

CCalSessionImpl::CCalSessionImpl()
	{
	iFileId = 0;
	}

void CCalSessionImpl::ConstructL()
	{
	iAgnServer = RAgendaServ::NewL();	
	User::LeaveIfError(iAgnServer->Connect());
	iAgnChangeReporter = CAgnChangeReporter::NewL(*this);
	iCalAsyncTaskManager = CCalAsyncTaskManager::NewL(*this);
	iAgnServer->AddSessionL(*this, iSessionId);
	}

void CCalSessionImpl::ConstructL(CCalSessionImpl& aCalSessionImpl)
	{
	iAgnServer = &aCalSessionImpl.Server();
	iAgnServer->AddSessionL(*this, iSessionId);
	iAgnChangeReporter = CAgnChangeReporter::NewL(*this);
	iCalAsyncTaskManager = CCalAsyncTaskManager::NewL(*this);
	}

CCalSessionImpl* CCalSessionImpl::NewL()
	{
	CCalSessionImpl* self = new (ELeave) CCalSessionImpl();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CCalSessionImpl*  CCalSessionImpl::NewL(CCalSessionImpl& aCalSessionImpl)
	{
	CCalSessionImpl* self = new (ELeave) CCalSessionImpl();
	CleanupStack::PushL(self);
	self->ConstructL(aCalSessionImpl);
	CleanupStack::Pop(self);
	return self;
	}

TInt64 CCalSessionImpl::FileId()
	{
	return iFileId;
	}
RAgendaServ& CCalSessionImpl::Server() const
	{
	return *iAgnServer;
	}

CCalSessionImpl::~CCalSessionImpl()
	{
	__ASSERT_DEBUG(iReferenceCount==0, CalUtils::Panic(ESessionHasOpenReferences));
	iCallBack = NULL;

	delete iCalAsyncTaskManager;//Have to be deleted first because it will cancel any asyn tesk
	if(iAgnChangeReporter && iAgnChangeReporter->IsAdded() && iAgnChangeReporter->IsActive())
		{
		iAgnChangeReporter->StopReporter();
		}
	delete iAgnChangeReporter;
	if(iSessionId > 0)
	    {
	    iAgnServer->CloseAgenda(iSessionId);
	    }	
	if (iAgnServer && !iAgnServer->RemoveSession(*this) )
		{
		iAgnServer->Close();
		delete iAgnServer;
		}	
	delete iFileName;
	}
	
/**
@capability ReadUserData
*/
CAgnSimpleEntry* CCalSessionImpl::GetSimpleEntryFromServerL(const TAgnInstance& aInstance)
	{
	return iAgnServer->GetSimpleEntryL(aInstance.iId, aInstance.iCollectionId);
	}

CAgnSimpleEntry* CCalSessionImpl::GetSimpleEntryFromServerL(TUint32 aInstance)
	{
	return iAgnServer->GetSimpleEntryL(iCollectionId, aInstance);
	}


void CCalSessionImpl::StartChangeNotificationL(MCalChangeCallBack* aCallBack,
												MCalChangeCallBack::TChangeEntryType aChangeEntryType,
												TBool aIncludeUndatedTodos,
												TTime aFilterStartTime,
												TTime aFilterEndTime)
	{
	iChangeCallBack = aCallBack;
	iAgnChangeReporter->StartReporterL((MCalChangeCallBack2::TChangeEntryType)aChangeEntryType, aIncludeUndatedTodos, aFilterStartTime, aFilterEndTime);
	}
	
void CCalSessionImpl::StartChangeNotificationL(MCalChangeCallBack2& aChangeObserver, const CCalChangeNotificationFilter& aFilter)
	{
	iChangeObserver = &aChangeObserver;
	iAgnChangeReporter->StartReporterL(aFilter.ChangeEntryType(), aFilter.IncludeUndatedTodos(), aFilter.StartTime().TimeUtcL(), aFilter.EndTime().TimeUtcL());
	}

void CCalSessionImpl::StartFileChangeNotificationL(MCalFileChangeObserver& aCallBack)
    {
    iFileChangeObserver = &aCallBack;
    iAgnChangeReporter->StartFileChangeReporterL();
    }

void CCalSessionImpl::StopChangeNotification()
	{
	iAgnChangeReporter->StopEntryChangeNotification();
	}

void CCalSessionImpl::StopFileChangeNotification()
    {
    iAgnChangeReporter->StopFileChangeNotification();
    }

void CCalSessionImpl::DisableChangeBroadcast()
	{
	iAgnServer->DisableChangeBroadcast(iFileId);
	}

void CCalSessionImpl::EnableChangeBroadcast()
	{
	iAgnServer->EnableChangeBroadcast(iFileId);
	}

const TDesC& CCalSessionImpl::FileName()
	{
	if(iFileName)
		{
		return *iFileName;
		}
	else
		{
		return KNullDesC();
		}
	}
	
void CCalSessionImpl::OpenL(const TDesC& aFileName, CalCommon::TCalFileVersionSupport& aStatus) const
	{
	if(iReferenceCount>0)
		{
		User::Leave(KErrInUse);
		}
	iAgnServer->OpenAgendaL(*this, aFileName, aStatus, iFileId, iCollectionId);
	HBufC* newFileName = aFileName.AllocL();
	delete iFileName;
	iFileName = newFileName;
	}

void CCalSessionImpl::IncrementReferenceCount()
	{
	++iReferenceCount;
	}

void CCalSessionImpl::DecrementReferenceCount()
	{
	__ASSERT_DEBUG(iReferenceCount>0, User::Invariant());
	--iReferenceCount;
	}

void CCalSessionImpl::DeleteCalFileL(const TDesC& aFileName) const
	{
	if (aFileName.Length()==0)
		{
		User::Leave(KErrArgument);
		}
	
	if(iCalAsyncTaskManager)
		{
		iCalAsyncTaskManager->CancelAsynTask();
		}
	iAgnChangeReporter->StopEntryChangeNotification();
	iAgnServer->DeleteAgendaFileL(aFileName, iSessionId);
	
	if (iFileName && iFileName->CompareF(aFileName) == 0)
	    {
	    // we have deleted the file that we had open
	    delete iFileName;
	    iFileName = NULL;
	    }
	}

void CCalSessionImpl::CreateCalFileL(const TDesC&  aFileName) const
	{
	if (iAgnServer->AgendaFileExistsL(aFileName))
		{
		User::Leave(KErrAlreadyExists);
		}
	else
		{
		TRAPD(err, iAgnServer->CreateAgendaFileL(aFileName));
		if (err != KErrNone)
			{
			TRAP_IGNORE(iAgnServer->DeleteAgendaFileL(aFileName, iSessionId));
			User::Leave(err);
			}
		}
	}

void CCalSessionImpl::CreateCalFileL(const TDesC& aFileName, const CCalCalendarInfo& aCalendarInfo) const
    {
    CreateCalFileL(aFileName);
    TRAPD(error, SetCalendarInfoL(aFileName, aCalendarInfo));
    
    if (error != KErrNone)
        {
        // There was a problem setting the meta data
        // This should be transactional so try to delete the file
        
        TRAP_IGNORE(DeleteCalFileL(aFileName));
        
        // Now leave with the reason the we could not set the meta data
        User::Leave(error);
        }
    }

void CCalSessionImpl::SetCalendarInfoL(const CCalCalendarInfo& aCalendarInfo) const
    {
    // Make sure a file has been opened
    if (!iFileName)
        {
        User::Leave(KErrNotReady);
        }
    
    SetCalendarInfoL(*iFileName, aCalendarInfo);
    }

void CCalSessionImpl::SetCalendarInfoL(const TDesC& aFileName, const CCalCalendarInfo& aCalendarInfo) const
    {
    iAgnServer->SetCalendarInfoL(aFileName, aCalendarInfo.Impl());
    }

CCalCalendarInfo* CCalSessionImpl::CalendarInfoL() const
    {
    // Make sure a file has been opened
    if (!iFileName)
        {
        User::Leave(KErrNotReady);
        }
    
    CCalCalendarInfoImpl* calInfoImpl(iAgnServer->GetCalendarInfoLC(*iFileName, const_cast<CCalSessionImpl&>(*this)));
    CCalCalendarInfo* info(CCalCalendarInfo::NewL(calInfoImpl));
    CleanupStack::Pop(calInfoImpl);
    return info;
    }

CDesCArray* CCalSessionImpl::ListCalFilesL() const
	{
	return iAgnServer->ListAgendaFilesL();
	}

const TDesC& CCalSessionImpl::DefaultFileNameL() const
	{
	return KDefaultSecureAgendaFileName();
   	}
	
void CCalSessionImpl::FileIdL(TCalFileId& aCalFileId) const
	{
	aCalFileId = iFileId;
	}
	
void CCalSessionImpl::_DebugSetHeapFailL(RAllocator::TAllocFail aFail, TInt aRate)
	{
	iAgnServer->_DebugSetHeapFailL(aFail, aRate);
	}

TInt CCalSessionImpl::_DebugRequestAllocatedCellsL()
	{
	return iAgnServer->_DebugRequestAllocatedCellsL();
	}

TInt CCalSessionImpl::_DebugRequestAllocatedHeapSizeL()
	{
	return iAgnServer->_DebugRequestAllocatedHeapSizeL();
	}

void CCalSessionImpl::EnablePubSubNotificationsL()
	{
	iAgnServer->SetEnablePubSubNotificationsL(ETrue, FileId());
	}
	
void CCalSessionImpl::DisablePubSubNotificationsL()
	{
	iAgnServer->SetEnablePubSubNotificationsL(EFalse, FileId());
	}

void CCalSessionImpl::GetFileNameL(TCalPubSubData aPubSubData, TDes& aFileName)
	{
	CDesCArray* fileArray = ListCalFilesL();
	CleanupStack::PushL(fileArray);
	
	if (fileArray)
		{
		const TInt KFileArrayCount = fileArray->Count();
		for (TInt i(0); i < KFileArrayCount; ++i)
			{
			TUint32 hashFileName = FoldAndGenerateHashL((*fileArray)[i]);
			if (hashFileName == aPubSubData.iFileNameHash)
				{
				aFileName = (*fileArray)[i];
				CleanupStack::PopAndDestroy(fileArray);
				return;
				}
			}
		}
		
	CleanupStack::PopAndDestroy(fileArray);
	}
	
TBool CCalSessionImpl::IsFileNameL(TCalPubSubData aPubSubData, const TDesC& aFileName)
	{
	return FoldAndGenerateHashL(aFileName) == aPubSubData.iFileNameHash;
	}
	
TBool CCalSessionImpl::IsOpenedFileL(TCalPubSubData aPubSubData)
	{
	if(iFileName)
		{
		return (FoldAndGenerateHashL(*iFileName) == aPubSubData.iFileNameHash);
		}
	else
		{
		User::Leave(KErrArgument);
		}
	return EFalse;
	}
	
CCalAsyncTaskManager& CCalSessionImpl::AsyncTaskManager()
	{
	return *iCalAsyncTaskManager;
	}

void CCalSessionImpl::ReportChangeL(TInt aChange)
    {
    if(aChange == KErrCorrupt || aChange == EEntryChangeInSameFile)
        {
        RArray<TCalChangeEntry> changedEntries;
        CleanupClosePushL(changedEntries);
        if (aChange == KErrCorrupt)
            {            
            TCalChangeEntry change;
            change.iChangeType = MCalChangeCallBack2::ERestoredFileCanNotBeOpened;
            changedEntries.AppendL(change);
            }
        else
            {
            //If it is a restore file notification, the file ID can be different to the current one. Therefore, iFileId should be updated in the situation.
            iAgnServer->GetChangesSinceLastNotificationL(changedEntries, iCollectionId, iFileId);
            }
        EntryHasChangedL(changedEntries);
        CleanupStack::PopAndDestroy(&changedEntries); // changedEntries.Close()    
        }
    else if(aChange == EFileChange)
        {       
        RPointerArray<CCalFileChangeInfo> calFileChangeInfos;
        CleanupResetAndDestroyPushL(calFileChangeInfos);
        RPointerArray<CAgnFileChangeInfo> agnFileChangeInfos;
        CleanupResetAndDestroyPushL(agnFileChangeInfos);
        iAgnServer->GetFileChangesSinceLastNotificationL(agnFileChangeInfos, iSessionId);
        const TInt count = agnFileChangeInfos.Count();
        calFileChangeInfos.ReserveL(count);
        for(TInt ii=0; ii<count; ++ii)
            {
            CCalFileChangeInfo* calFileChangeInfo = CCalFileChangeInfo::NewL(agnFileChangeInfos[ii]);
            calFileChangeInfos.Append(calFileChangeInfo);
            agnFileChangeInfos[ii] = NULL;//owership is transfered to calFileChangeInfos
            }
        if(calFileChangeInfos.Count() > 0)
            {
            iFileChangeObserver->CalendarInfoChangeNotificationL(calFileChangeInfos);
            }
        CleanupStack::PopAndDestroy(&agnFileChangeInfos);
        CleanupStack::PopAndDestroy(&calFileChangeInfos);      
        }
    else
        {
        User::Invariant();
        }      
    }
    
void CCalSessionImpl::EntryHasChangedL(RArray<TCalChangeEntry>& aChangedEntries)
    {
    if (iChangeObserver)
        {
        iChangeObserver->CalChangeNotification(aChangedEntries);
        }
        
    if (iChangeCallBack)
        {
        TBool includeTodo = EFalse;
        TBool includeEvent = EFalse;
        TInt count=aChangedEntries.Count();
        TInt ii=0;
        while(ii<count && (includeTodo == EFalse || includeEvent == EFalse))
            {
            TCalChangeEntry change = aChangedEntries[ii];
            if (change.iChangeType == MCalChangeCallBack2::EChangeUndefined)
                {
                includeTodo = ETrue;
                includeEvent = ETrue;
                }
            else
                {
                if(aChangedEntries[ii].iEntryType == MCalChangeCallBack2::EChangeEntryTodo)
                    {
                    includeTodo = ETrue;
                    }
                else
                    {
                    includeEvent = ETrue;
                    }
                }
            ++ii;
            }
        MCalChangeCallBack::TChangeEntryType entryType = MCalChangeCallBack::EChangeEntryAll;
        if(!includeTodo)
            {
            entryType = MCalChangeCallBack::EChangeEntryEvent;
            }
        else if (!includeEvent)
            {
            entryType = MCalChangeCallBack::EChangeEntryTodo;
            }
        iChangeCallBack->CalChangeNotification(entryType);
        
        if (aChangedEntries.Count() == 0)
            {
            iChangeCallBack = NULL;
            }
        }
    }

TCalCollectionId CCalSessionImpl::CollectionId()
	{
	return iCollectionId;
	}

TInt CCalSessionImpl::SessionId() const
    {
    return iSessionId;
    }

TTime CCalSessionImpl::TzRulesLastModifiedDateL()
	{
	return iAgnServer->TzRulesLastModifiedDateL(iFileId);
	}

void CCalSessionImpl::__dbgClearTzClientCacheL(TBool aRestartCaching)
	{
	iAgnServer->__dbgClearTzClientCacheL(aRestartCaching);
	}

//

/* CAgnChangeReporter*/

CAgnChangeReporter* CAgnChangeReporter::NewL(CCalSessionImpl& aSession)
	{
	return new (ELeave) CAgnChangeReporter(aSession);
	}	

void CAgnChangeReporter::DoCancel()
	{
	//Unset both notification flag
	iChangeIterested =iChangeIterested& ~EEntryChangeInSameFile;
	iChangeIterested = iChangeIterested& ~EFileChange;
	}
	
CAgnChangeReporter::CAgnChangeReporter(CCalSessionImpl& aSession) 
:CActive(EPriorityStandard),iSession(aSession),iChangeIterested(0)
	{
	}

CAgnChangeReporter::~CAgnChangeReporter()
	{
	StopReporter();
	}


void CAgnChangeReporter::StartReporterL(MCalChangeCallBack2::TChangeEntryType aEntryType,	TBool aIncludeUndatedTodos,	TTime aFilterStartTimeUtc, TTime aFilterEndTimeUtc)
	{
	iChangeIterested = iChangeIterested|EEntryChangeInSameFile;

	if (!IsAdded())
		{
		CActiveScheduler::Add(this);
		iStatus = KRequestPending;
		}
	iSession.Server().SetChangeNotificationParametersL(aEntryType, aIncludeUndatedTodos, aFilterStartTimeUtc, aFilterEndTimeUtc, iSession.FileId());
	if (!IsActive())
		{
		SetActive();
		}
	iSession.Server().RequestChangeNotification(iStatus, iSession.SessionId(), iChangeIterested);
	}

void CAgnChangeReporter::StartFileChangeReporterL()
    {
    iChangeIterested = iChangeIterested|EFileChange;
    if (!IsAdded())
        {
        CActiveScheduler::Add(this);
        iStatus = KRequestPending;
        }
    if (!IsActive())
        {
        SetActive();       
        }
    iSession.Server().RequestChangeNotification(iStatus, iSession.SessionId(), iChangeIterested);
    }

void CAgnChangeReporter::StopReporter()
	{
	TCalFileId fileid = iSession.FileId();
	if(fileid > KNullFileId || iChangeIterested &EFileChange)
		{
		iSession.Server().CancelChangeNotification(iSession.SessionId(), iChangeIterested);	
		}
	Cancel();
	}

void CAgnChangeReporter::StopEntryChangeNotification()
    {
    if(!(iChangeIterested &EFileChange))
        {
        StopReporter();       
        }
    else
        {       
        iSession.Server().CancelChangeNotification(iSession.SessionId(),EEntryChangeInSameFile); 
        iChangeIterested =iChangeIterested& ~EEntryChangeInSameFile;
        }
    }

void CAgnChangeReporter::StopFileChangeNotification()
    {
     if(!(iChangeIterested &EEntryChangeInSameFile))
        {
        StopReporter();       
        }
    else
        {       
        iSession.Server().CancelChangeNotification(iSession.SessionId(),EFileChange); 
        iChangeIterested = iChangeIterested& ~EFileChange;
        }
    }

void CAgnChangeReporter::RunL()
	{
	if (IsAdded() && (iStatus != KErrCancel))
		{
		iSession.ReportChangeL(iStatus.Int());

		// Request another change notification.
		//
		if (!IsActive() && iStatus.Int()!=KErrCorrupt)
			{
			SetActive();
			iSession.Server().RequestChangeNotification(iStatus, iSession.SessionId(), iChangeIterested);
			}
		}
	}

