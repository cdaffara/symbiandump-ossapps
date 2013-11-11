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

/**
 @file
 @internalComponent
*/

#ifndef __CALSESSIONIMPL_H__
#define __CALSESSIONIMPL_H__

#include <badesca.h>

#include "agmids.h"
#include "calchangecallback.h"
#include "calnotification.h"


class CAgnSimpleEntry;
class CCalAsyncTaskManager;
class CAgnChangeReporter;
class CAgnSimpleEntryAllocator;
class RAgendaServ;
class MCalChangeCallBack;
class MCalProgressCallBack;
class MCalFileChangeObserver;
class CCalInstance;
class CAgnEntry;
class TCalTime;
class CCalCalendarInfo;

enum TChangeNotificationFlag
     {
     EEntryChangeInSameFile = 0x01,
     EFileChange            = 0x10,
     };

NONSHARABLE_CLASS(CCalSessionImpl) : public CBase
	{
public:
	static CCalSessionImpl*  NewL();
	static CCalSessionImpl*  NewL(CCalSessionImpl& aCalSessionImpl);
	~CCalSessionImpl();

	RAgendaServ& Server() const;
	CCalAsyncTaskManager& AsyncTaskManager();

	void OpenL(const TDesC& aFileName, CalCommon::TCalFileVersionSupport& aStatus) const;
	void DeleteCalFileL(const TDesC& aFileName) const;
	void CreateCalFileL(const TDesC&  aFileName) const;
	void CreateCalFileL(const TDesC& aFileName, const CCalCalendarInfo& aCalendarInfo) const;
	void SetCalendarInfoL(const CCalCalendarInfo& aCalendarInfo) const;
	CCalCalendarInfo* CalendarInfoL() const;
	    
	const TDesC& DefaultFileNameL() const; 
	CDesCArray* ListCalFilesL() const;

	void StartChangeNotificationL(MCalChangeCallBack* aCallBack, MCalChangeCallBack::TChangeEntryType aChangeEntryType, TBool aIncludeUndatedTodos, TTime aFilterStartTime, TTime aFilterEndTime);
	void StartChangeNotificationL(MCalChangeCallBack2& aChangeObserver, const CCalChangeNotificationFilter& aFilter);
	void StartFileChangeNotificationL(MCalFileChangeObserver& aCallBack);
	void StopChangeNotification();
	void StopFileChangeNotification();
	void EnableChangeBroadcast();
	void DisableChangeBroadcast();
	void ReportChangeL(TInt aChange);

	
	void IncrementReferenceCount();
	void DecrementReferenceCount();
	
	void EnablePubSubNotificationsL();
	void DisablePubSubNotificationsL();
	
	void GetFileNameL(TCalPubSubData aPubSubData, TDes& aFileName);
	TBool IsFileNameL(TCalPubSubData aPubSubData, const TDesC& aFileName);
	TBool IsOpenedFileL(TCalPubSubData aPubSubData);
	
	void FileIdL(TCalFileId& aCalFileId) const;
	CAgnSimpleEntry* GetSimpleEntryFromServerL(const TAgnInstance& aInstance);
	CAgnSimpleEntry* GetSimpleEntryFromServerL(TUint32 aInstance);
	void DeleteSimpleEntry(CAgnSimpleEntry* aSimpleEntry);
	CAgnSimpleEntry* CreateDumySimpleEntryL();
	
	TTime TzRulesLastModifiedDateL();
	TInt64 FileId();
	TCalCollectionId CollectionId();
	const TDesC& FileName();
	TInt SessionId() const;

	void _DebugSetHeapFailL(RAllocator::TAllocFail aFail, TInt aRate);
	TInt _DebugRequestAllocatedCellsL();
	void __dbgClearTzClientCacheL(TBool aRestartCaching);
	TInt _DebugRequestAllocatedHeapSizeL();
	
private:

	enum TPubSubChange
		{
		ENoChange		= 0x00,
		ETodoChanged	= 0x01,
		EEventChanged	= 0x10,
		};

	CCalSessionImpl();
	void ConstructL();
	void ConstructL(CCalSessionImpl& aCalSessionImpl);
	
private:
	void SetCalendarInfoL(const TDesC& aFileName, const CCalCalendarInfo& aCalendarInfo) const;
	void EntryHasChangedL(RArray<TCalChangeEntry>& aChangedEntries);

private: 
	RAgendaServ*			iAgnServer;
	MCalProgressCallBack*	iCallBack;
	MCalChangeCallBack*		iChangeCallBack;
	MCalChangeCallBack2*	iChangeObserver;
	MCalFileChangeObserver* iFileChangeObserver;
	TInt					iReferenceCount;
	
	CAgnChangeReporter*						iAgnChangeReporter;
	CCalAsyncTaskManager*					iCalAsyncTaskManager;
	mutable TCalFileId						iFileId;
	mutable TCalCollectionId				iCollectionId;
	mutable HBufC*							iFileName;
	TInt                                    iSessionId;
	};

NONSHARABLE_CLASS(CAgnChangeReporter) : public CActive
/**
@internalComponent
*/
	{
public:

	static CAgnChangeReporter* NewL(CCalSessionImpl& aSession);
	~CAgnChangeReporter();
	void StartReporterL(MCalChangeCallBack2::TChangeEntryType aEntryType,
					   TBool aIncludeUndatedTodos,
					   TTime aFilterStartTime,
					   TTime aFilterEndTime);
	
	void StartFileChangeReporterL();

    void StopReporter();
    void StopFileChangeNotification();
    void StopEntryChangeNotification();
    
private: 
	void RunL();
	void DoCancel();
	CAgnChangeReporter(CCalSessionImpl& aSession);

private:
	CCalSessionImpl& iSession;
    TBool                   iCallbackEnabled;
    TUint8                  iChangeIterested;//The report observs either or both entry and file changes. The flag specifies which notification(s) is(are) interested.
	};

#endif

