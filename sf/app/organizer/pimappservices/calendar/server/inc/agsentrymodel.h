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

#ifndef __AGSENTRYMODEL_H__
#define __AGSENTRYMODEL_H__
#include <f32file.h>
#include "agsver.h"
#include <calattachmentmanager.h>
#include <calchangecallback.h>
#include <calentry.h>
#include "calinstanceviewimpl.h"
#include <f32file.h>

class CAgnAlarm;
class CAgnAttachment;
class CAgnAttachmentFile;
class CAgnAttachmentIndex;
class CAgnCalendarConverter;
class CAgnCategoryIndex;
class CAgnContent;
class CAgnEntry;
class CAgnEntryManager;
class CAgnModelStreamIdSet;
class CAgnRptDef;
class CAgnServerSession;
class CAgnServFile;
class CAgnSimpleEntryTable;
class CAgnInstanceInfo;
class CCalAsyncDelete;
class CStreamStore;
class RFile;
class TAgnChangeFilter;
class TAgnEntryIter;
class TAgnFilter;
class TAgnInstanceExtractor;
class TAgnSortInstance;
class TFindInstanceParams;
class CAgnTzRuleIndex;
class CAgsInstanceIteratorMgr;

// function pointer used in text matching 
typedef TInt (*MatchTextFnPtr)(const TDesC& aTextField, const TDesC& aSearchText);

/** This class represents details of a database change to be notified to clients. 
Most of this information is used for filtering.
@internalComponent
*/
struct TAgnChange
	{
	// The start and end time of the entry affected
	TTime							iStartTimeOfEntryUtc;
	TTime							iEndTimeOfEntryUtc;
	
	// The repeat rule of the entry affected (NULL if there isn't one)
	const CAgnRptDef*				iRepeatRule;

	// If the operation is an update, the start and end time of the original entry
	TTime							iOriginalStartTimeUtc;
	TTime							iOriginalEndTimeUtc;
	
	// If the operation is an update, the repeat rule of the original entry (NULL if there isn't one)
	const CAgnRptDef*				iOriginalRepeatRule;

	// The type of operation
	MCalChangeCallBack2::TChangeType	iOperationType;
	
	// The type of entry
	CCalEntry::TType				iEntryType;

	// The id of the entry
	TCalLocalUid					iEntryId;

	// The session which made the operation (not owned)
	CAgnServerSession*				iSession;
	TInt64							iFileId;
	};


/** The server-side model to control access to entries.

This class contains the main functionality for adding, updating, deleting and fetching entries, and 
searching for instances.
It is responsible for change notification, queuing alarms, building the entry indexes, opening calendar files, 
and carrying out roll-back if an error occurs.

CAgnEntryModel references other server-side classes which are used for various functions:
- CAgnEntryManager takes responsibility for all file-reading and file-writing operations, e.g. adding an entry 
to the calendar file.
- CAgnServFile is used to control higher level file access operations, e.g. opening a file.
- CAgnSimpleEntryTable contains the entry indexes. CAgnCategoryIndex and CAgnAttachmentIndex contain
other indexes for controlling category and attachment data.
- TAgnInstanceExtractor is used for all instance searching operations, including finding alarmed instances when 
queuing alarms.

@see CAgnEntryModel 
@internalComponent
@released
*/
class CAgnEntryModel : public CBase
	{
public:
	friend class CAgnCalendarConverter; // to allow access to UpdateIndexL
	
	enum TUpdateIndex
		{
		EAdd,
		EDelete,
		EUpdate,
		EBuildIndex
		};
	
// construction / destruction
	static CAgnEntryModel* NewL(CAgnServFile* aAgnServerFile);
	~CAgnEntryModel();

// file operations
	void OpenL(CStreamStore& aStore, const TStreamId& aModelStreamId);
	TStreamId CreateL(CStreamStore& aStore);
	const TInt64& GetFileIdL();
	const TDesC& FileName() const;
	
	void CommitL();
	void Rollback();
	void CommitAndNotifyDeletesL(TAgnChangeFilter& aChangeFilter);

// build indexes
	void ResetIndexes();
	TInt DoIndexBuildStepL(); // return percentage complete
	void BuildIndexCompleteL();
	TBool AreIndexesBuilt() const;
	TBool StreamsAreEmpty() const;

// queueing alarms
	void NextAlarmForServerL(const TTime& aNow,CArrayFixFlat<TAgnSortInstance>& aAlarmedIds);
	void NextFewAlarmsForServerL(const TTime& aStartDateTime,const TTime& aEndDateTime,CArrayFixFlat<TAgnSortInstance>& aAlarmedIds,const TInt aMaxNumberOfAlarms);
	void FindAndQueueNextFewAlarmsL();
	void SetUpdateAlarmL(TBool aUpdateAlarm);
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	void DeleteAlarmsAndRequeueSessionAlarmL();
#endif
// SyncML requirements
	void GetEntryUidsSinceDateL(const TTime& aTime, RArray<TCalLocalUid>& aUniqueIdList); // find entries modified since a certain time

// access entry's attributes which are not stored with entries
	HBufC* RestoreTextL(const TStreamId& aStream);
	CAgnContent* RestoreAlarmActionL(const TStreamId& aStream);

// entry operations
	TAgnEntryId StoreL(CAgnEntry& aEntry, TAgnChangeFilter* aChangeFilter);
	void UpdateEntryL(CAgnEntry& aNewEntry, TAgnChangeFilter* aChangeFilter, TBool aDeleteChildren);
    void DeleteEntryL(CAgnEntry& aEntry, TBool aCascadeDelete, TAgnChangeFilter* aChangeFilter);
    
// fetching entries
	const CAgnSimpleEntry* GetSimpleEntryFromIndexes(const TAgnEntryId& aId);
	const CAgnSimpleEntry* GetSimpleEntryFromIndexes(TCalLocalUid aId);
	CAgnEntry* FetchEntryL(const TAgnEntryId& aId) const;	
	CAgnEntry* FetchEntryL(TCalLocalUid aId) const;
	CAgnEntry* FetchEntryL(const TDesC8& aGuid) const;
	void FetchEntriesL(const TDesC8& aGuid, RPointerArray<CAgnEntry>& aList) const;
	
// instance searching
	void FindInstancesL(CArrayFix<TAgnSortInstance>& aInstances, const TFindInstanceParams& aParameters);
	void NextPossibleInstancesL(CArrayFix<TAgnSortInstance>& aInstances, const TFindInstanceParams& aSearchParams) const;
	void PreviousPossibleInstancesL(CArrayFix<TAgnSortInstance>& aInstances, const TFindInstanceParams& aSearchParams) const;
	void LogInstanceSearchL(const TFindInstanceParams& aParameters, const CArrayFix<TAgnSortInstance>& aInstances) const;

// used by extractors when searching
	TBool MatchFullEntryL(const TAgnEntryId& aEntryId, const TFindInstanceParams& aSearchParams);
	TBool MatchSearchTextL(CAgnEntry& aEntry, const TDesC& aSearchText, const TAgnFilter& aFilter);
	static TBool CheckStartAndEndDateOverlap(const TAgnSortInstance& aInstance, const TFindInstanceParams& aSearchParams);

// notifications
	void NotifyChangeL(const CAgnServerSession& aSession, CAgnEntry* aEntry, MCalChangeCallBack2::TChangeType aChangeType, CAgnInstanceInfo* aOriginalEntry);
	void NotifyPublishAndSubscribeL(TAgnChangeFilter& aChangeFilter);

// Attachments
	TInt TransferAttachmentFileToServerL(RFile& aFile, TCalLocalUid aLocalUid, TInt aAttachmentIndex);
	void OpenAttachmentFileL(RFile& aFile, TInt aAttachmentIndex) const;
	HBufC* GenerateFilenameLC(const TDesC& aDrive, const TDesC& aFileName);
	HBufC* GenerateRandomFilenameLC(const TDesC& aDrive);

	void UpdateAttachmentDetailsL(TCalLocalUid aLocalUid, TInt aAttachmentIndex, const TDesC& aFileName, TInt aAttachmentSize);
	void UpdateAttachmentDetailsOnCopyL(TCalLocalUid aLocalUid, TInt aAttachIndex, const TDesC& aFileName);
	void CreateNewFileL(RFile& aFileHandle, const TDesC& aFileName);
	const RArray<TCalLocalUid>* GetEntriesWithAttachment(TUint32 aAttachmentUid) const;
	void GetSortedAttachmentsL(RArray<TUint32>& aAttachmentIds, CCalAttachmentManager::TSortOrder aSortOrder);
	CAgnAttachment* FetchAttachmentByIdL(TUint32 aAttachUid);
	TInt MoveFileToServerL(TCalLocalUid aLocalUid, TInt aAttachmentIndex);

// Accessor functions
	CAgnCategoryIndex& CategoryIndex() const;

// Miscellaneous
	const TDesC8& GetEntryGuidL(CAgnEntry& aEntry) const;
	void SetBufferedDeleting(TBool aSetting);	
	void FlushL();
	void RestoreCategoriesL();
	CCalAsyncDelete* CreateAsyncDeleteL(TAgnChangeFilter& aChangeFilter);
	TAgnEntryIter* CreateEntryIterL() const;
	TTime TzRulesLastModifiedDateL();
	void CheckTzDbModificationL();
	void HandleTzRulesChangeL(const TTime& aTime);
	TBool IsIndexFileAvailableL();

// Calendar file conversion operations
	void LoadNewStreamStoreL(CStreamStore& aStore, const TStreamId& aModelStreamId, CAgnEntryManager& aEntryManager, CAgnTzRuleIndex& aTzRuleIndex);
	
// Alarm accessor
   CAgnAlarm& Alarm();

//methods for cached index file
public:
	TBool IsIndexFileDirty() const;
	void  MarkIndexFileAsDirtyL();
	TBool DoLoadIndexFile();
	TBool LoadIndexFileL();
	void  SaveIndexFileL();
	TBool GenerateIndexFileName(TFileName& aFileName);
	TCalCollectionId CollectionId() const;
	const CAgnServFile& AgnServFile();
	void ResetDeleteRollbackArray();

private:
	void ConstructL(CAgnServFile* aAgnServerFile);

// file functions
	void Reset();	
	void DoOpenL(const TStreamId& aId);
	void ExternalizeNextUidValuesL() const;
	void ExternalizeNextUidValuesL(CStreamStore& aStore, const TStreamId& aStreamId) const;
	void InternalizeNextUidValuesL();
	void ExternalizeEntryManagerL() const;
	void InternalizeEntryManagerL();
	void ExternalizeFileIdL(CStreamStore& aStore, const TStreamId& aStreamId) const;

// entry operations
	TAgnEntryId AddEntryL(CAgnEntry& aEntry);
	TAgnEntryId DoAddEntryL(CAgnEntry& aEntry);
   	void DoDeleteEntryL(CAgnEntry& aEntry);
	void DoUpdateEntryL(CAgnEntry& aEntry, CAgnEntry* aOldEntry);
	void DoUpdateAttachmentsL(CAgnEntry& aEntry, CAgnEntry& aOldEntry);

	CAgnEntry* FetchEntryL(const TDesC8& aGuid, const TAgnCalendarTime& aRecurrenceId) const;
	void MoveAttachmentToDriveL(CAgnAttachmentFile& aFileAttachment, CAgnAttachmentFile& aNewFileAttachment);
	
	void DoCommitL();
	
// parent-child functionality
	TAgnEntryId AddChildEntryL(CAgnEntry& aChild, CAgnEntry& aParent);
	TAgnEntryId AddRepeatingChildEntryUpdateParentRuleL(CAgnEntry& aParentEntry, CAgnEntry& aRepeatingChild);
	TAgnEntryId AddNonRepeatingChildEntryUpdateParentExceptionsL(CAgnEntry& aParentEntry, CAgnEntry& aNonRepeatingChild);

	TBool EntryHasNoChildrenAndNoValidInstancesL(CAgnEntry& aEntry) const;
	TInt EntryHasRepeatingChildrenL(const CAgnEntry& aParentEntry);
	void DeleteNonRepeatingChildrenOutsideRangeL(const CAgnEntry& aParentEntry, const TAgnCalendarTime& aRecIdUtc, CalCommon::TRecurrenceRange aRange);
 	void DeleteChildrenL(CAgnEntry& aParent);
	void UpdateParentL(CAgnEntry& aChild);
    CAgnEntry* FindChildFromParentL(const CAgnEntry& aParent, const TAgnCalendarTime& aRecurrenceId) const;
	
// entry properties stored in separate streams
	TStreamId StoreTextL(const TDesC& aText);
	void UpdateTextL(const TDesC& aText, const TStreamId& aStream);
	void DeleteTextStreamL(const TStreamId& aStream);
	TStreamId StoreAlarmActionL(const CAgnContent& aAlarmAction);
	void UpdateAlarmActionL(const CAgnContent& aAlarmAction, const TStreamId& aStream);
	void DeleteAlarmActionStreamL(const TStreamId& aStream);

	TStreamId WriteDescriptorToStreamL(const TDesC& aString);
	void StoreExternalAttributesL(CAgnEntry& aEntry);
	void DeleteExternalAttributesL(CAgnEntry& aEntry);
	void UpdateExternalAttributesL(CAgnEntry& aEntry);

// used when text searching
	TBool MatchSearchTextL(MatchTextFnPtr aMatchTextFunction, CAgnEntry& aEntry, const TDesC& aSearchText, const TAgnFilter& aFilter);
	static TInt MatchExactText(const TDesC& aTextField, const TDesC& aSearchText);
	static TInt MatchFoldedText(const TDesC& aTextField, const TDesC& aSearchText);

// accessor functions	
	CStreamStore& StreamStore() const;
	
// alarm queuing
	void CreateAlarmForServerL();	
	void UpdateAlarmListL(CArrayFixFlat<TAgnSortInstance>& aAlarmedIds,CArrayFixSeg<TAgnSortInstance>& aDayInfoList, const TTime& aNow);
	void AddToAlarmListL(CArrayFixFlat<TAgnSortInstance>& aAlarmedIds,CArrayFixSeg<TAgnSortInstance>& aDayInfoList, const TTime& aStartDateTime,
					const TTime& aEndDateTime, const TInt aMaxNumberOfAlarms);
	
// notification
	void NotifyingL(MCalChangeCallBack2::TChangeType aChangeType, CAgnEntry& aEntry, CAgnInstanceInfo* aOriginalEntry);
	void NotifySessionsOfChangeL(const TAgnChange& aChange);

// index updates
	void UpdateIndexL(CAgnEntry& aEntry, CAgnEntry* aOldEntry, TUpdateIndex aUpdateIndex);
	void AddEntryToIndexesL(CAgnEntry& aEntry);

// rollback functions
	void AppendRollbackArrayL(const CAgnEntry& aEntry, TBool aAdd);
	void ResetRollback();
	void RollbackIndexesL();
// Miscellaneous

	TInt TransferFileFromClientL(RFile& aAttachfileHandle, CAgnAttachmentFile& aAttachFile, CAgnEntry& aEntry, TBool aIsSameDrive);
	TInt DoMoveFileToServerL(TCalLocalUid aLocalUid, TInt aAttachmentIndex);
	void CopyAttachmentFileToDifferentPlaceL(CAgnEntry& aEntry);
	TBool IsAttachmentFileFromSameSessionL(const TDesC& aAttachmentFile);	
private:	  
	TInt									iNumStreamsProcessed; 
	CAgnModelStreamIdSet*					iModelStreamIdSet;
	CAgnEntryManager* 						iEntryManager;
	CAgnAlarm*								iAlarm;
	TUint									iNextLocalUidValue;
	TUint									iNextAttachmentUid;
	TAgnInstanceExtractor*                  iExtractor;
	CAgnSimpleEntryTable*					iSimpleEntryTable;
	CAgnCategoryIndex*						iCategoryIndex;
	CAgnAttachmentIndex*					iAttachmentIndex;
	CAgnTzRuleIndex*						iTzRuleIndex;	
	TInt64									iFileId;
	CAgnServFile* 							iAgnServerFile;	//not owned
	TBool									iUpdateAlarm;
	TAgnChangeFilter*						iChangeFilter;//not owned
	RArray<TAgnEntryId> 					iAddRollbackArray;
	RPointerArray<CAgnEntry> 				iDeleteRollbackArray;
	CAgnCalendarConverter*                  iCalConverter;
	TInt									iOperationsCounter;
    RFile									iAttachmentFileHandle;
	RFs										iFs;
	TBool  									iIndexFileIsDirty;
	TBool  									iIndexFileIsPresent;
	CAgsInstanceIteratorMgr*				iInstanceIteratorMgr;
	
	};

#endif


