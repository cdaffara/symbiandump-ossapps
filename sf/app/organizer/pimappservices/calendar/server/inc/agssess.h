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

#ifndef __AGSSESS_H__
#define __AGSSESS_H__

#include <calchangecallback.h>
#include <calentry.h>
#include <calfilechangenotification.h>
#include "agmcalendartime.h"
#include "agsindexiterator.h"

#include <s32mem.h>
#include <badesca.h>

class CAgnCategoryList;
class CAgnEntry;
class CAgnEntryModel;
class CAgnRptDef;
class CAgnServer;
class CAgnServFile;
class CAgnServerSession;
class CAgnInstanceInfo;
struct TAgnChange;
class TAgnEntryDateIterator;
class TAgnEntryId;
class TAgnEntryIter;
class TAgnFilter;
class TAgnInstanceExtractor;
class CAgnSessionFileManager;
class CAgnServFileMgr;
class TAgnMessageToComplete;
class CAgsInstanceIteratorMgr;
/**
Class which controls all notifications made from a calendar session.
This stores the parameters for change notifications enabled through the @see CCalSession::StartChangeNotification APIs.
It stores whether or not pub-sub notifications are enabled, from the  @see CCalSession::EnablePubSubNotificationsL API.
If notification is disabled by another session, it keeps track of whether a change has been made so that the 
appropriate notification can be made when it is re-enabled.
@internalAll
@released
*/
class TAgnChangeFilter
	{
public:
 
 enum TPubSubChange
	{
	ENoChange		= 0x00,
	ETodoChanged	= 0x01,
	EEventChanged	= 0x02,
	};
	
	TAgnChangeFilter(CAgnServerSession& aSession);
	void SetChangeParameter(const TTime& aStartTime, const TTime& aEndTime, MCalChangeCallBack2::TChangeEntryType aChangeType);
	TBool ChangeBroadcastEnabled () const;
	void SetEnableChangeBroadcast(TBool aBool);
	TBool ChangeMadeWhileDisabled()const;
	void SetChangeMadeWhileDisabled(TBool aHasChaned);
	
	TBool PubSubEnabled() const;
	void SetEnablePubSubNotification(TBool aEnablePubSubNotification);

	void SetPubSubChange(TPubSubChange aChange);
	TBool TodoChanged() const;

	const CAgnServerSession& Session() const;
	TBool IsValidChangeL(const TAgnChange& aChange) const;
private:
	TBool CheckChangeWithinRangeL(const CAgnRptDef* aRptDef, const TTime& aStartTime, const TTime& aEndTime) const;
private:
	TBool									iChangeBroadcastEnabled;
	TBool									iChangeMadeWhileDisabled;
	CAgnServerSession&						iSession;
	TTime									iStartTimeUtc;
	TTime 									iEndTimeUtc;
	MCalChangeCallBack2::TChangeEntryType	iEntryType;
	TInt									iPubSubChangeMadeDuringDisable;
	TBool									iPubSubEnabled;
	};

//Indicates that the client has already been panicked using RMessage2 object.
//so that RMessage2 object is not used twice.
const TInt KClientHasBeenPanicked = 1;

enum TChangeNotificationState
	{
	/*StartNotification has not been called, or has been cancelled*/
	ENotificationNotRequested,
	/* Request pending*/ 
	ENotificationRequested,
	/* Request completed, awaiting further request*/
	ENotificationAwaitingRequest
	};

/** 
The main Calendar server session class, which receives all IPC calls on the server side.
These calls are handled by DispatchMessageL, which calls the relevant server function.
The session gets a pointer to a CAgnServFile and a CAgnEntryModel when a Calendar file is opened.
Functions are called on these objects in most server calls.
CAgnServerSession also holds information about notifications for the client of this session. 
@internalAll
*/
class CAgnServerSession : public CSession2
	{
public:
    static CAgnServerSession* NewL(CAgnServer& aServer);
	~CAgnServerSession();	

	void BulkChangeCompletedL(TInt64 aFileId, TInt aErr);
	void AddChangeL(const TAgnChange& aChange);
	TInt FileIdCount();
	TInt64 FileId(TInt aIndex);
	void CancelAllTask();
	void BackupReStoreChangedL(TInt64 aFileId, const CAgnServFile& aServFile, MCalChangeCallBack2::TChangeType aChangeType);
	void BackupRestoreCancelTask();
	void LockClient(TBool aLock);
	
private:
	CAgnServerSession(CAgnServer& aServer);
	void ConstrucL();

	CAgnServFile* GetFileL(TInt aSlot);
	TBool DispatchMessageL();
	virtual void ServiceL(const RMessage2 &aMessage);
	void PanicClientL(TInt aPanic) const;	// Panic the client

	HBufC* ReadClientDesLC(TInt aDesNum);
	HBufC8* ReadClientDes8LC(TInt aDesNum);
//Server interface
	void CommitL();
	void RollbackL();
	void OpenAgendaL();
	void CloseAgendaL();
	void FetchEntryL();
	void FetchEntryByUIDL();
	void FetchSimpleEntryL();
	void FetchSimpleEntryByUIDL();
	void FetchSimpleEntriesL();
	void UpdateEntryL();
	void AddEntryL();
	void DeleteEntryL();

	void TransmitBufferL();
	
	void PreviousInstancesL();
	void NextInstancesL();
	void CreateEntryIteratorL();
	void EntryIteratorNextL();
	void EntryIteratorPositionL();

	void GetEntryUidsSinceDateL();
	void GetFileIdL();
	void RestoreTextL();
	void RestoreAlarmActionL();
	void RequestChangeNotification();
	void RequestChangeNotificationParametersL();
	void CancelChangeNotification();
	void DisableChangeBroadcastL();
	void EnableChangeBroadcastL();
	void StartBuildIndex();
	void CreateAgendaFileL();
	void DeleteAgendaFileL();
	void ListAgendaFilesL();
	void TidyByDateReadParamsL();
	void AgendaFileExistsL();

	void RequestProgressL();
	void GetChangesSinceLastNotificationL();
	void GetFileChangesSinceLastNotificationL();
	void TidyByDateStartL();
	void AsynchCategoryTaskStartL();
	void SetUpdateAlarmL();
	void FetchEntryByGuidL();
	void FetchSimpleEntriesByGuidL();
	void DeleteEntriesByLocalUidL();
	void DeleteEntryByGuidL();
	void SetEnablePubSubNotificationL();
	void FindInstancesL();
	void GetCategoryListCountL();
	void GetCategoryListItemL();
	void AddCategoryToListL();
	void CategoryTaskStartL();
	void FilterCategoryL();
	void CancelTaskL();
	void TransferAttachmentFileToServerL();
	void TransferAttachmentFileToClientL();
	void TransferFileToClientToWriteL();
	void FetchSortedAttachmentsL();
	void EntriesWithAttachmentL();
	void FetchAttachmentByIdL();
	void MoveFileToServerL();
	void TzRulesLastModifiedDateTimeL();

	void CreateInstanceIteratorL();
	void DestroyInstanceIteratorL();
	void InstanceIteratorNextL();
	void InstanceIteratorPreviousL();
	void InstanceIteratorCountL();
	void InstanceIteratorLocateIndexL();
	
	void GetPropertyValueL();
	void SetCalendarInfoL();
	void GetCalendarInfoL();
	
private:
	void GetCategoryInfoL(const CBufFlat& aBufFlat,TInt& aCategoryIndex, TCalCollectionId aCollectionId);
	void DeleteEntryL(CAgnEntry& aEntry, TCalCollectionId aCollectionId);
	void ExternalizeEntryToClientL(const CAgnEntry& aEntry, TInt aSlot);
	void DeleteEntryByIdL(TAgnEntryId aEntryId, TCalCollectionId aCollectionId);
	void DeleteEntryByUIdL(TCalLocalUid aEntryId, TCalCollectionId aCollectionId);
	void EnsureStringsLoadedL(CAgnEntry& aEntry, TCalCollectionId aCollectionId);
	void WriteIntArrayToBufferL(const RArray<TUint32>& aArray);
	void CloseAgendas();
	TBool CheckBackupRestore();
	void CalendarFileChangedL(CAgnFileChangeInfo& aFileChangeInfo);
    void AddFileChangeL(CAgnFileChangeInfo& aFileChangeInfo, CAgnServerSession* aSession);
    TBool FileCanBeDeletedL(TInt aSessionId, const CAgnServFile& file);
    TBool IsOwnFileL(TInt aSessionId, const CAgnServFile& file);
    TBool FileHasBeenOpenedL(const CAgnServFile& file);
    void CloseAgendaL(TInt aSessionId);
 
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	TBool IsReadOnlyOperation();
#endif
	TInt GetFileIndexL(const TDesC& aFileToOpen);
private:
	CAgnServer& 				iAgnServer;
	CBufFlat*					iBuffer;	// used to transmiting streams to client
	RMessage2					iMessage;
	TBool						iSessionReady;
	CAgnSessionFileManager*	    iAgnSessionFileManager;
	TBool                       iBackupRestoreLock;
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	CAgsInstanceIteratorMgr*	iInstanceIteratorMgr;
#endif
	};

class CAgnSessionFile : public CBase
	{
public:
	static CAgnSessionFile* NewL(TInt aSessionId, TInt64 aFileId, CAgnEntryModel& aModel, CAgnServerSession& aAgnServerSessionr);
	static CAgnSessionFile* NewL(TInt aSessionId, CAgnServerSession& aAgnServerSessionr);

	~CAgnSessionFile();

	TInt64 FileId() const;
	void TidyByDateStartL(TAgnMessageToComplete& aMessageToComplete, CAgnServFile& aServerFile);
	void SetEnablePubSubNotificationL(TBool aEnablePubSub);
	void EnableChangeBroadcastL(const CAgnServerSession& aAgnServerSession);
	void DisableChangeBroadcast();
	void RequestChangeNotification(const RMessage2 aMessage);
	void CancelChangeNotification(TUint8 aNotificationType);
	void SetChangeNotificationParametersL(const TTime aStart, const TTime aEnd,MCalChangeCallBack2::TChangeEntryType aChangeType);
	void BulkChangeCompletedL(const CAgnServerSession& aSeesion, TInt aErr);
	TInt GetChangesSinceLastNotificationL(CBufFlat*& aDataBuffer);
    TInt GetFileChangesSinceLastNotificationL(CBufFlat*& aDataBuffer);
	void AddChangeToBufferL(const TAgnChange& aChange, CAgnServerSession* aAgnServerSession);
	void AddFileChangeToBufferL(CAgnFileChangeInfo& aFileChangeInfo);

	void UpdateEntryL(CAgnEntry& aEntry, TBool aDeleteChildren);
	TAgnEntryId StoreEntryL(CAgnEntry& aEntry);
	void DeleteEntryL(CAgnEntry& aEntry);
	void DeleteEntryByGuidL(const TDesC8& aGuid, TBool aCommit);
	
	TBool CreateEntryIterL();
	TBool IsEntryAvailableAtIteratorL();
	CAgnEntry* FetchEntryAtIteratorL();
	void ResetModel(const CAgnServFile& aServFile);
	TInt SessionId();
	TBool FileNotificationIsReqested();
	void SetFileId(TCalFileId aFileId);
	 
private:
	CAgnSessionFile(TInt aSessionId,TInt64 aFileId, CAgnEntryModel* aModel, CAgnServerSession& aAgnServerSession);
	TBool ToNotifyL(const TAgnChange& aChange, CAgnServerSession* aAgnServerSession);
	void RequestEntryChangeNotification();
	void RequestFileChangeNotification();

private:
    TInt                        iSessionId;
	TInt64						iFileId;
	CAgnEntryModel*				iModel;//Not owned
 	RMessage2					iChangeNotificationMessage;
 	TChangeNotificationState	iChangeNotificationPending;
 	TBool						iBulkChangeInProgress;
 	TAgnChangeFilter			iChangeFilter;
 	TInt						iBufferedNotificationsCount;
	TBool						iNotificationBufferFull;
	CBufFlat*					iBufferedNotification;
	RBufWriteStream				iBufferedNotificationStream;
    TInt                        iFileBufferedNotificationsCount;
    CBufFlat*                   iFileBufferedNotification;
    RBufWriteStream             iFileBufferedNotificationStream;
	TAgnEntryIter*				iEntryIter;	
    TUint8                      iChangeIterested;//The client could interested entry change, file change or both of them.
	};

class CAgnSessionFileManager : public CBase
	{
public:
	static CAgnSessionFileManager* NewL(CAgnServerSession& aAgnServerSession, CAgnServFileMgr& aAgnServer);
	~CAgnSessionFileManager();
	
	CAgnSessionFile& GetSessionFileL(TInt64 aFileId);
	CAgnSessionFile& GetSessionFileL(TCalCollectionId aCollectionId);
	TInt GetSessionFile(TInt aSessionId);
	CAgnSessionFile& GetSessionFileByIndex(TInt aIndex);
	void DeleteSessionFile(TInt aSessionId);
	void DeleteSessionFileIfFileNotificationNotRequired(TInt aSessionId);
	TBool AddClientL(TInt64 aFileId, TInt aSessionId);
	void AddChangeL(const TAgnChange& aChange);
	void AddFileChangeL(CAgnFileChangeInfo& aFileChangeInfo);
	TInt FileIdCount();
	TInt64 FileId(TInt aIndex);
	
	static TInt CompareFileId(const CAgnSessionFile& aLeft, const CAgnSessionFile& aRight);
	static TInt CompareFileId(const TInt64* aKey, const CAgnSessionFile& aAgnSessionFile);
	TInt FindL(TInt64 aFileId) const;
	void ReSetModel(TInt64 aFileId, const CAgnServFile& aServFile);
	void RequestChangeNotification(const RMessage2 aMessage);
	
private:
	 
	CAgnSessionFileManager(CAgnServerSession& aAgnServerSession, CAgnServFileMgr& aAgnServer);
private:
	CAgnServerSession& 					iAgnServerSession;
	CAgnServFileMgr&					iAgnServFileMgr;
	RPointerArray<CAgnSessionFile> 	    iSessionFiles;
	};

class CAgnInstanceInfo : public CBase
	{
public:
	static CAgnInstanceInfo* NewL(const CAgnSimpleEntry& aSimpleEntry);
	static CAgnInstanceInfo* NewLC(const CAgnSimpleEntry& aSimpleEntry);
	~CAgnInstanceInfo();

	CCalEntry::TType Type() const;
	const CAgnRptDef* RptDef() const;
	const TTime& StartTimeUtc() const;
	const TTime& EndTimeUtc() const;

private:
	CAgnInstanceInfo();
	void ConstructL(const CAgnSimpleEntry& aSimpleEntry);
private:
	CCalEntry::TType	iType;
	TTime				iStartTimeUtc;
	TTime				iEndTimeUtc;
	CAgnRptDef*			iRptDef;
	};

#endif
