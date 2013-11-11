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

#ifndef __AGSFILEMANAGER_H__
#define __AGSFILEMANAGER_H__

#include "agmfilter.h"
#include "calasynctask.h"
#include "calchangecallback.h"

#include <s32stor.h>
#include <badesca.h>

class CAgnCategoryList;
class CAgnEntryManager;
class CAgnEntryModel;
class CAgnPermanentData;
class CAgnServer;
class CAgnServerSession;
class CAgnTlsProxy;
class CCalAsyncDelete;
class CFileStore;
class RFile;
class RFs;
class TAgnChangeFilter;
class TAgnVersion;
class CAgnServFileShutdownDelayTimer;
class CAgnTzRuleIndex;
class CAgnCalendarInfo;

const TUid KUidAgnCategoryList = {0x101FA9E1};
const TUid KUidAgnCalendarInfo = {0x101FA9E2};

const TInt KAgnPercentageComplete = 100;
//Working calendar filename length  220 chars (excluding private path)
//Restricted filename length to 200 chars = 220 - 12(attachment doc name)-2(_a added at last of every filename) -2 ( folder number) - 4 (for sperators "\\" "\\") 
const TInt KCalMaxFilePath = 200;

// for model index filenames
_LIT (KIdxFilePostFix, "Idx"); //add to filename to ensure 1 idx per calendar file
const TInt KIdxFilePostFixLength = 3;

/**
Class to store details about the current asynchronous operation.
These are held in an array in @see CAgnServFile.
*/
class TAgnMessageToComplete
	{
public:
	TAgnMessageToComplete(RMessage2& aMessage, TBool aReportProgress, CAgnServerSession& aSession);
	RMessage2 Message() const;
	TBool ReportProgress() const;
	CAgnServerSession& Session() const;

private:
	TAgnMessageToComplete(const TAgnMessageToComplete& ); // not implemented
	void operator=(const TAgnMessageToComplete& ); // not implemented

private:
	RMessage2	iMessage;
	TBool		iReportProgress;
	CAgnServerSession& iSession;		
	};

/*
Class to control access to a Calendar file.
All operations requiring file access should come through here (open/close file, commit changes, etc.)

This class is also the active object which controls asynchronous operations.
*/
class CAgnServFile : public CActive
	{
public:
	static CAgnServFile* NewL(RFs& aFs, CAgnServer& aAgnServer);
	~CAgnServFile();

	// File functions
	CFileStore* CreateAgendaFileLC(const TDesC& aFileName);
	void OpenAgendaL(const TDesC& aFilename, CalCommon::TCalFileVersionSupport& status);
	static void CloseAgenda(TAny* aFile);
	void CloseAgenda(TBool aCloseImmediately);
	
	void DoCloseAgenda();
	

	TBool IsReadOnly() const;
	const TDesC& FileName() const;
	TBool IsLocked() const;
	
	void OpenFileL(RFile& aFileHandle, const TDesC& aFileName);
	void CreateNewFileL(RFile& aFileHandle, const TDesC& aFileName);
	
	TBool FileExistsL(const TDesC& aFileName) const;
	TBool FileIsReadOnlyL(const TDesC& aFileName) const;
	void CreateDirL(const TDesC& aDirectory) const;
	void DeleteFileL(const TDesC& aFileName) const;
	void MoveFileL(const TDesC& aSource, const TDesC& aDestination) const;
	void CopyFileL(const TDesC& aSource, const TDesC& aDestination) const;

	static TBool FileExistsL(RFs& aFs, const TDesC& aFileName);
	static void CreateDirL(RFs& aFs, const TDesC& aDirectory);
	static void DeleteFileL(RFs& aFs, const TDesC& aFileName);
	
	void GetAttachmentFolderNameL(TDes& aFolderName);
	static void GetAttachmentFolderNameL(const TDesC& aFileName, TDes& aFolderName);
	
	// Compaction functions
	void CompactFileL();

	// Index building functions
	void StartBuildIndex(TAgnMessageToComplete& aMessageToComplete);
	void DoStartBuildIndex();
	TBool AreIndexesBuilt() const;

	// Tidy functions
	void TidyByDateSetup(CAgnServerSession& aSession,
						 const TAgnFilter& aFilter,
						 const TTime& aTodaysDate,
						 const TTime& aStartDate,
						 const TTime& aEndDate);

	void TidyByDateStartL(TAgnMessageToComplete& aMessageToComplete, TAgnChangeFilter& aChangeFilter);
	TBool HasServerSession() const;
	TBool IsInterestedSession(CAgnServerSession* aSession);

	// Accessor functions.
	CAgnEntryModel* Model() const;
	CFileStore* StoreL() const;
	CStreamDictionary* Dictionary() const;
	CAgnCategoryList& CategoryList() const;
	CAgnServer& Server() const;
	CAgnServerSession* ServerSession();
	CAgnTlsProxy* TimeZoneConverter() const;
	TUint32 FileNameHash() const;
	const TDesC& PrivatePath() const;
	
	// Asynchronous operation functions	
	void RequestProgressL(TAgnMessageToComplete& aMessageToComplete);
	void CategoryTaskStartL(TAgnMessageToComplete& aMessageToComplete, CCalAsyncTaskManager::TAsyncAction aTask);
	TBool DoTaskCompleteL(TInt aErr, CAgnServerSession* aSession);

	// File version functions
	void ReplaceConvertedAgendaFileL(CAgnEntryManager& aEntryManager, CAgnTzRuleIndex& aTzRuleIndex);
	void GetFileVersionSupportStatusL(TAgnVersion& aFileVersion, CalCommon::TCalFileVersionSupport& aStatus);

	// Misc. functions.
	void AddReference();
	
	TInt ReferenceCount() const;
	void ExternalizeCategoryListL();
	TStreamId WriteCategoryListL(CStreamStore& aStore) const;
	void AddStreamToDictionaryL(const TUid& aStreamUid, const TStreamId& aStreamId, CStreamDictionary& aDictionary, CPersistentStore& aStore) const;
	void CloseAgendaImmediately();
	CAgnServFileShutdownDelayTimer& FileShutdownDelayTimer() const;
	void TzRulesHaveChangedL();
	void SetCollectionId(TCalCollectionId aFileId);
	TCalCollectionId CollectionId()const;
	TInt BackupReStoreChanged(MCalChangeCallBack2::TChangeType aChangeType);
	void BackupRestoreLock(TBool aLock);
	TBool IsBackupRestoreLock() const;
	void CancelTaskL(CAgnServerSession* aSession);
	void SetRefreshTzRules(TBool aSetRefresjTzRule);
	TBool RefreshTzRules() const;

	HBufC8* GetPropertyValueLC(TStreamId aStreamId);
	TBool SetCalendarInfoL(const CAgnCalendarInfo& aCalendarInfo);
	CAgnCalendarInfo* GetCalendarInfoLC() const;
	TBool IsCalendarInfoExistL()const;
	TBool IsFileDisabled();

#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	void QueueAlarmsImmediately();
	void SetShutdownFlag(TBool aNotificationFlag);
	void DeleteAlarmsAndRequeueSessionAlarm();
#endif

private:
	CAgnServFile(RFs& aFs, CAgnServer& aAgnServer);
	void ConstructL();

	void RunL();		// from CActive
	void DoCancel();	// from CActive
	
	// category functions
	void CreateCategoryListL() const;
	void SaveCategoryListL();
	void InternalizeCategoryListL(const TStreamId& aStreamId);
	
	// asynchronous operations
	void Start();
	void SetActiveAndMakeRequest();
	void AddAsyncRequesterL(TAgnMessageToComplete& aMessageToComplete);	
	void CompleteRequests(TBool aIsCompleted, TInt aCompleteCode, CAgnServerSession* aSession);

	// AO state machine functions.
	TBool DoStepL();
	TInt DoBuildIndexStepL();
	TInt AddAsyncRequester(TAgnMessageToComplete& aMessageToComplete);

	// miscellaneous functions
	void DoCompactFileL();
	TBool DoCompactL();
	TBool DoCompactorStepL();
	void SetFileNameHashL();
	void CreateModelForFileL();

	TStreamId DictionaryLookup(TUid aUid) const;
	void AddStreamToDictionaryL(const TUid& aStreamUid, const TStreamId& aStreamId) const;

	void DoSaveIndexFile();
	void SetLock(TBool aToLock);
	void CreateFileStoreL(const TDesC& aFilename);
    void ReopenAgendaAfterRestoreL();
    

private:

	TStreamId 			iModelStreamId;	
	CAgnServer&			iAgnServer;
	HBufC* 				iFileName;
	TUint32				iFileNameHash;
	CFileStore* 		iStore;
	TInt 				iRefCount;
	CAgnEntryModel* 	iModel;	
	CStreamDictionary* 	iDictionary; 
	RFs& 				iFs;
	TBool 				iReadOnly;
	TInt				iActiveStep;		// active object step
	
	CArrayFixFlat<TAgnMessageToComplete>* iMessageVector;
	CAgnServerSession* 	iSession;
	
	// Progress of current operation.
	TInt 				iProgressTotal;

	// Build index data.
	TBool 				iIndexesBuilt;
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	TBool               iShutdownNotification;
#endif
	// Tidy by Todo list and Date data.
	TAgnFilter			iTidyByDateFilter;
	TTime				iTidyByDateUndatedTodoDate;
	TTime				iTidyByDateStartDate;
	TTime				iTidyByDateEndDate;

	// Compaction data.
	TInt				iNextCompactEffort;
	enum
		{
		ENothing,
		ECompacting,
		EReclaiming
		} iCompactionStage;
	RStoreReclaim 		iCompactor;
	CAgnCategoryList* 	iCategoryList;
	CCalAsyncDelete*	iAsyncDelete;

	CAgnServFileShutdownDelayTimer* iFileShutdownDelayTimer;
	TCalCollectionId				iCollectionId;
	TBool                           iLocked;
	TBool                           iRefreshTzRules;
	TBool                           iIsFileDisabled;
	TBool                           iBackupRestoreLock;
 	};


/**
Class to control general file access to Calendar files.
For access to a specific Calendar file, use @see CAgnServFile.
*/
class CAgnServFileMgr : public CBase
	{
public:
	static CAgnServFileMgr* NewL(RFs& aFs, CAgnServer& aAgnServer);
	~CAgnServFileMgr();
	
	CAgnServFile& OpenAgendaL(const TDesC& aFilename, CAgnServer& aAgnServer, CalCommon::TCalFileVersionSupport& status);
	TInt CloseAgenda(CAgnServFile& aStore, TBool aCloseAgendaWithDelay);
	CFileStore* CreateAgendaFileLC(const TDesC& aFileName);
	void DeleteAgendaFileL(const TDesC& aFilename);

	const TDesC& PrivatePath() const;
	CDesCArray* ListAgendaFilesL() const;
 	HBufC* ParseFilenameLC(const TDesC& aFileName) const;
	TBool AgendaFileExistsL(const TDesC& aFileName) const;

	void CreatePermanentDataL();
	
	TBool FileCloseTimersRunning() const;
	void CloseScheduledFilesImmediately();
	void CloseAgendaFile(CAgnServFile* aServFile);
	
	CAgnServFile* GetFile(const TDesC& aFilename) const; 
	CAgnServFile* GetFileL(TInt64 aFileId) const;
	CAgnServFile* GetFileL(TCalCollectionId aCollectionId) const;
	
	TInt64 GetLongFileIdL(TCalCollectionId aCollectionId) const;
	void BackupReStoreChanged(MCalChangeCallBack2::TChangeType aChangeType);
	
	// iterate through files
	TInt Count();
	CAgnServFile* File(TInt aIndex);
	
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	void QueueAlarmsImmediatelyForShutdown();
    void RequeueAlarmsForShutdownCancellation();
#endif
private:
	CAgnServFileMgr(RFs& aFs, CAgnServer& aAgnServer);
	void ConstructL();
	
private:
	CAgnServer&	iAgnServer;
	CArrayFixFlat<CAgnServFile*>* iFileList;
	RFs& iFs;
	TFileName iPrivatePath;
	CAgnPermanentData* iPermanentData;
	};


// Delay period before the calendar file is closed on the server side
const TInt KServerShutdownDelay = 5000000; 

/** 
* Class representing the calendar file close delay timer. An agenda server calendar file can be closed either immediately or after a delay on the server side
* The delay is triggered using a timer owned by the calendar file and represented by this class. 
* If a session tries to access the calendar file during the course of the delay, the delay is cancelled and the file is not closed
*/
class CAgnServFileShutdownDelayTimer : public CTimer
	{
public:
	// aServFile is not owned
	static CAgnServFileShutdownDelayTimer* NewL(CAgnServFile& aServFile);
	void Start();
	void DoCloseAgenda();
	void CloseAgenda();
private:
	CAgnServFileShutdownDelayTimer (CAgnServFile& aServFile);
	void ConstructL();
	void RunL();

private:	
	CAgnServFile& iServFile;
	};

#endif
