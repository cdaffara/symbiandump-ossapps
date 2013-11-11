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

#ifndef __AGCLIENT_H__
#define __AGCLIENT_H__

#include "calasynctask.h"
#include "agmattachment.h"
#include "calattachmentmanager.h"
#include "calchangecallback.h"
#include "calinstanceviewimpl.h"
#include "agmids.h"
class CAgnAttachment;
class CAgnContent;
class CAgnCategory;
class CAgnEntry;
class CAgnSimpleEntry;
class CAgnSimpleEntryAllocator;
class CAgnTlsProxy;
class CCalAttachmentIteratorImpl;
class TAgnFilter;
class TAgnInstanceId;
class RFile;
class CCalCalendarInfoImpl;
class CAgnFileChangeInfo;

/**
@internalAll
@released
*/
enum TAgnClientPanic
	{
	ELockError =	-1,
	EBufferError =	-2,
	EStreamIdError =-3,
	EUnknownType =	-4,
	ENullEntry   =  -5,
	EBadModel	 =  -6,
	EServerDeath =  -7
	};

class RAgendaServ : public RSessionBase
/** The client interface to the agenda server. 

This class accesses agenda data by sending messages synchronously to the server.
This allows shared access to an agenda file by more than one client. 

Most functions in this class should not be used directly. If there is a 
corresponding version of the function in the model class, that should be used 
instead.

Note that a separate instance of the RAgendaServ class is required for each 
agenda file that is open. Only one file can be open at any one time in 
the same server session.

The API is used as follows:

@code
RAgendaServ* agnServer = RAgendaServ::NewL(); // allocate and construct server session
CleanupStack::PushL(agnServer); // ensure object is deleted
agnServer->Connect(); // connect to the server
CleanupClosePushL(*agnServer); // ensure that the session's Close() method gets called
CAgnEntryModel* model = CAgnEntryModel::NewL(); // allocate and construct model
CleanupStack::PushL(model);
model->SetServer(agnServer); // set model's server session pointer
model->OpenL(fileName) // open file using server
// ...Use agenda model API as normal - invokes corresponding server functions
CleanupStack::PopAndDestroy(3); // model, close session with server, agnServer
@endcode 
@internalAll
@released
*/
	{
public:
	// Server Connection functions
	void IncreaseRef();
	TInt DecreaseRef();
	void AddSessionL(const CCalSessionImpl& aSession, TInt& aSessionId);
	TBool RemoveSession(const CCalSessionImpl& aSession);
	static RAgendaServ* NewL();
	~RAgendaServ();
	TInt Connect();

	void OpenAgendaL(const CCalSessionImpl& aSession, const TDesC& aFileName, CalCommon::TCalFileVersionSupport& status, TInt64& aFileId, TCalCollectionId& aCollectionId);
	void WaitUntilLoadedL(TCalCollectionId aCalCollectionId);

	void CloseAgenda(TInt aSessionId);

	// Server Utility functions
	void CommitL(TCalCollectionId aCalCollectionId);
	void Rollback(TCalCollectionId aCalCollectionId);
	// Entry / Date Iterator functions
	TBool CreateEntryIteratorL(TInt64 aFileId);
	TBool EntryIteratorNextL(TInt64 aFileId);
	CAgnEntry* FetchEntryByIteratorL(TInt64 aFileId);
	void FindInstancesL(RArray<TInt64>& aCalCollectionIds, CArrayFix<TAgnInstance>& aInstances, const TFindInstanceParams& aParams);
	void FetchFileHandleL(RFile& aFileHandle, TUint32 aAttachmentUid, TInt64 aFileId);
	void FetchAttachmentsL(CCalAttachmentIteratorImpl& aIteratorImpl, CCalAttachmentManager::TSortOrder aSortOrder, TInt64 aFileId);
	void EntriesReferencingFileAttachmentL(RArray<TCalLocalUid>& aUids, const CAgnAttachment& aAttachment, TInt64 aFileId);
	CAgnAttachment* FetchAttachmentByIdL(TUint32 aAttachUid, TInt64 aFileId);

	// new connectivity sync functionality
	void GetEntryUidsSinceDateL(RArray<TCalLocalUid>& aUids, const TTime& aFilter, TInt64 aFileId);
	TInt _DebugRequestAllocatedCellsL();		
	void _DebugSetHeapFailL(TInt aHeapAllocationFailureFlag, TInt aCount);
	TInt _DebugRequestAllocatedHeapSizeL();		
	
public:
	TVersion Version() const;
	// Entry Fetch / Update functions
	CAgnEntry* FetchEntryL(TAgnEntryId& aId, TInt64 aFileId) const;
	CAgnEntry* FetchEntryL(TCalLocalUid aId, TInt64 aFileId) const;
	CAgnSimpleEntry* GetSimpleEntryL(const TAgnEntryId& aId, TCalCollectionId aCalCollectionId) const;
	CAgnSimpleEntry* GetSimpleEntryL(TCalCollectionId aShortFileId, TUint32 aId) const;
	void GetSimpleEntryL(const RArray<TAgnInstance>& aEntryIds, RPointerArray<CAgnSimpleEntry>& aLiteEntries) const;
	void UpdateEntryL(CAgnEntry& aEntry, TBool aDeleteChildren, TCalCollectionId aCalCollectionId);
	TAgnEntryId AddEntryL(CAgnEntry& aEntry, TInt64 aFileId);

	// Model Utility functions
	void NextInstancesL(const RArray<TInt64> aFileIds, CArrayFix<TAgnInstance>& aInstances,const TFindInstanceParams& aParams) const;
	void PreviousInstancesL(const RArray<TInt64> aFileIds, CArrayFix<TAgnInstance>& aInstances, TFindInstanceParams& aParams) const;
	HBufC* RestoreTextL(TStreamId aId, TInt64 aFileId);
	TStreamId StoreAlarmActionL(const CAgnContent& aAlarmAction);
	CAgnContent* RestoreAlarmActionL(TStreamId aId, TInt64 aFileId);
	void StartBuildIndex(TBool aNeedProgressReport,TRequestStatus& aStatus, TCalCollectionId aCalCollectionId);
	void StartTidyByDateL(TBool aReportProgress, TRequestStatus& aStatus,
						  const TAgnFilter& aFilter,
						  const TTime& aTodaysDate,
						  const CalCommon::TCalTimeRange& aCalTimeRange,
						  TCalCollectionId aCalCollectionId);	
	void CreateAgendaFileL(const TDesC& aFileName);
	void DeleteAgendaFileL(const TDesC& aFileName, TInt aSessionId);
	CDesCArray* ListAgendaFilesL();
	TBool AgendaFileExistsL(const TDesC& aFileName);
	void RequestProgress(TRequestStatus& aStatus, TCalCollectionId aCalCollectionId);

public:		// category access
	TInt CategoryCountL(TInt64 aFileId);
	CAgnCategory* CategoryL(TInt aIndex, TInt64 aFileId);
	void AddCategoryToListL(const TDesC& aName, TInt64 aFileId);
	void FilterCategoryL(CArrayFixSeg<TAgnEntryId>& aEntries, TInt64 aFileId);
	void CancelTask(TInt64 aFileId);
	void StartCategoryTaskL(TBool aReportProgress, TRequestStatus& aStatus,const CAgnCategory& aCategory, CCalAsyncTaskManager::TAsyncAction aTask, TCalCollectionId aCalCollectionId);
	void FilterCategorySynchronouslyL(const CAgnCategory& aCategory, TCalCollectionId aCalCollectionId);

	// Request notification of any change made to the agenda model.
		   
	void SetChangeNotificationParametersL(MCalChangeCallBack2::TChangeEntryType aEntryType,
								   TBool aIncludeUndatedTodos,
								   TTime aFilterStartTimeUtc,
								   TTime aFilterEndTimeUtc,
								   TInt64 aFileId);
	void RequestChangeNotification(TRequestStatus& aStatus, TInt aSessionId, TUint8 aNotificaitonType);
	void CancelChangeNotification(TInt aSessionId, TUint8 aNotificaitonType);
	void DisableChangeBroadcast(TInt64 aFileId);
	void EnableChangeBroadcast(TInt64 aFileId);
	void GetChangesSinceLastNotificationL(RArray<TCalChangeEntry>& aChangeItems,TCalCollectionId aCalCollectionId, TCalFileId& aFileId);
	void GetFileChangesSinceLastNotificationL(RPointerArray<CAgnFileChangeInfo>& aAgnFileChangeInfo, TInt aSessionId);
	void SetEnablePubSubNotificationsL(TBool aEnablePubSubNotifacation, TInt64 aFileId);

	TInt SetUpdateAlarm(TBool aUpdateAlarm, TInt64 aFileId);
	void DeleteEntryL(TAgnEntryId aEntryId, TCalCollectionId aCalCollectionId);
	void FetchEntryByGuidL(const TDesC8& aGuid, RPointerArray<CAgnEntry>& aList, TInt64 aFileId) const;
	void FetchSimpleEntriesByGuidL(const TDesC8& aUid, RPointerArray<CAgnSimpleEntry>& aLiteEntries, RArray<TInt> aCalCollectionIds) const;
	void DeleteEntryL(const RArray<TCalLocalUid>& aIds, TCalCollectionId aCalCollectionId);
	void DeleteEntryByGuidL(const TDesC8& aGuid, TBool aCommitAndNotify,TInt64 aFileId);
	TTime TzRulesLastModifiedDateL(TInt64 aFileId);
	void DeleteSimpleEntry(CAgnSimpleEntry* aSimpleEntry);
	CAgnSimpleEntry* CreateDumySimpleEntryL();
	CAgnSimpleEntryAllocator& SimpleEntryAllocator() const;
public:
	void __dbgClearTzClientCacheL(TBool aRestartCaching);

public: //Methods for CCalSortedInstanceIterator
#ifdef SYMBIAN_CALENDAR_ENHANCEDSEARCHANDSORT
	TInt CreateInstanceIteratorL(const TFindInstanceParams& aFindParams, const CCalSortCriteria& aSortCriteria,RArray<TInt64>& aCalCollectionIds);
#endif
	void DestroyInstanceIterator(TInt aIteratorId);
	void InstanceIteratorNextL(TInt aIteratorId, CArrayFix<TAgnInstance>& aInstances, TInt aIndex) const;
	void InstanceIteratorPreviousL(TInt aIteratorId, CArrayFix<TAgnInstance>& aInstances, TInt aIndex) const;
	TInt InstanceIteratorCountL(TInt aIteratorId) const; 
	TInt InstanceIteratorLocateIndexL(TInt aIteratorId, TAgnInstance& aInstanceId);
	
public: // Methods for calendar metadata
    HBufC8* GetPropertyValueL(const TDesC& aFilename, TStreamId aStreamId) const;
    void SetCalendarInfoL(const TDesC& aFilename, const CCalCalendarInfoImpl& aCalendarInfo) const;
    CCalCalendarInfoImpl* GetCalendarInfoLC(const TDesC& aFilename, CCalSessionImpl& aSessionImpl) const;
 private:
	RAgendaServ();	// private constructor
	void ConstructL();
	static TInt StartServer();

	void Panic(TAgnClientPanic aPanic) const;
	TInt DoConnect();
	TInt ServerRequestL(TInt aCommand, TAny* aArg1 = NULL, TAny* aArg2 = NULL, TAny* aArg3 = NULL, TAny* aArg4 = NULL) const;
	CBufFlat* CreateTransmitBufferL(TInt aBufferSize) const;
	
	void LoadingInstancesFromBufferL(CArrayFix<TAgnInstance>& aInstances, TInt aBufSize) const;
	void ExternalizeFilterL(const TAgnFilter& aFilter, RWriteStream& aStream) const;
	CAgnSimpleEntry* InternalizeSimpleEntryL(RReadStream& aStream) const;
	CAgnEntry* ReadEntryFromServerSessionL(TInt aBufferSize) const;
	CAgnEntry* CreateEntryFromBufferStreamL(RReadStream& readStream) const;
	void SendFileAttachmentHandlesL(CAgnEntry& aEntry, TInt64 aFileId);
	void TransferFileHandleToServerL(TCalLocalUid aLocalUid, TInt aAttachmentIndex, CAgnAttachmentFile& aFileAttachment, TInt64 aFileId);
	void WriteBinaryDataToNewAttachmentFileL(CAgnEntry& aEntry, TInt aAttachmentIndex, TInt64 aFileId);
	TBool FindSession(const CCalSessionImpl& aSession);

private:
	CBufFlat*					     iBuffer;
	CAgnSimpleEntryAllocator*	     iSimpleEntryAllocator;
	TInt						     iReferenceCount;
	RPointerArray<CCalSessionImpl>   iCalSessions; // Not owned.
	TInt                             iLastSessionId;
	};

/**
This Class is used for cleaning up sortentry when a leave happens,
SortEntry should be deleted using SortEntryAllocator.
Cleanup function DestroyLiteEntry uses an object of this class.
*/
class TCleanSimpleEntry
{
public:
	TCleanSimpleEntry(CAgnSimpleEntry& aSortEntry, CAgnSimpleEntryAllocator& aAlloc);
	CAgnSimpleEntry* GetSimpleEntry();
	CAgnSimpleEntryAllocator& GetSimpleEntryAllocator();
	
	static void DestroySimpleEntry(TAny* aPtr);	//Cleanup function used to delete sortentry.
	
private:
	CAgnSimpleEntry& iSimpleEntry;
	CAgnSimpleEntryAllocator& iAlloc;
};
#endif
