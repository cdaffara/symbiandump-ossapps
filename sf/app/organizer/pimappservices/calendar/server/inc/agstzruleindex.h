// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __AGSTZRULEINDEX_H_
#define __AGSTZRULEINDEX_H_

#include <s32stor.h>
#include <s32file.h>
#include "agmtlsproxy.h"

class CAgnEntry;
class CTzRules;
class CAgnTzRules;
class CAgnServFile;

NONSHARABLE_CLASS(CAgnTzRuleIndexItem) : public CBase
	{
public:
	static CAgnTzRuleIndexItem* NewL(TStreamId aStreamId, CTzRules* aTzRule);
	static CAgnTzRuleIndexItem* NewL(RReadStream& aStream);
	virtual ~CAgnTzRuleIndexItem();
	
	virtual void ExternalizeL(RWriteStream& aStream) const;
	virtual void InternalizeL(RReadStream& aStream);
	
	//Check if the tz rule refers to a system time zone.
	virtual TBool IsSystemTzRule() const;
	
	//Increase the reference count.
	void IncrementRef();
	
	//deduce the reference count.
	TInt DecrementRef();

	void SetRule(CTzRules* aTzRule);
	CTzRules* TzRule() const;

	const TStreamId& StreamId() const;

	inline TInt ReferenceCount() const;
	inline void SetReferenceCount(TInt aReferenceCount);
	
protected:
	CAgnTzRuleIndexItem();
	CAgnTzRuleIndexItem(TStreamId aStreamId, CTzRules* aTzRule);

private:
	//Reference count to record how many entries are using this tz rule.
	TInt 					iRefCount;
	
	//The stream id that the tz rule is stored in
	TStreamId				iStreamId;
	
	//The real tz rule object.
	CTzRules*				iTzRules;
	};
	
NONSHARABLE_CLASS(CAgnSystemTzRuleIndexItem) : public CAgnTzRuleIndexItem
	{
public:
	static CAgnSystemTzRuleIndexItem* NewL(TStreamId aStreamId, const TDesC8& aTzId, CTzRules* aTzRule);
	static CAgnSystemTzRuleIndexItem* NewL(RReadStream& aStream);
	virtual ~CAgnSystemTzRuleIndexItem();

	virtual void ExternalizeL(RWriteStream& aStream) const;
	virtual void InternalizeL(RReadStream& aStream);
	
	//Check if the tz rule refers to a system time zone.
	virtual TBool IsSystemTzRule() const;
	
	//The string name id of the tz rule
	const TDesC8& TzId() const;

private:
	CAgnSystemTzRuleIndexItem();
	CAgnSystemTzRuleIndexItem(TStreamId aStreamId, CTzRules* aTzRule);
	void ConstructL(const TDesC8& aTzId);
	
private:
	HBufC8*		iTzId;
	};
	
/*
This class is instantiated after the object CAgnEntryModel is created in agenda server.
It will hold an array of items for indexing purpose. Each indexing item holds a tz id
and a stream id where a tz rule is stored in the file.

When an entry is added\deleted\updated, the model will ask the index to
add\delete the tz rule associated with the entry. The CAgnTzRuleIndex then will
possibly add\delete a rule in the memory and the store as appropriately or 
updating the reference count for an existing rule in the array. In either case above,
the index array will be updated and persisted to the file.

*/	
NONSHARABLE_CLASS(CAgnTzRuleIndex) : public CBase, public MAgnTzChangeObserver
	{
public:
	static CAgnTzRuleIndex* NewL(CStreamDictionary& aDictionary, CFileStore& aStore);
	~CAgnTzRuleIndex();
	
	//from MAgnSystemTimeChangeObserver		
	virtual void HandleCurrentTimeZoneChangeL(const CTzId& aSystemTzId); //implementaion of current time zone observer
	virtual void HandleTzRulesChangeL(const TTime& aTime);

	void AddTzRuleL(CAgnEntry& aEntry); //Add a new tz rule into the index, called when an new repeating entry is added 
	void RemoveTzRuleL(CAgnSimpleEntry& aEntry); //Remove a tz rule from the index, called when a repeating entry is deleted
	void FetchTzRuleL(CAgnSimpleEntry& aEntry); //Retrieve a tz rule by the stream id stored in aTzZone.
	void UpdateTzRuleL(CAgnEntry& aOldEntry, CAgnEntry& aNewEntry); //Update the reference of both new and old tz rules when the rule is updated inside a repeating entry. 
	
	void RollBackL(); //Rollback function.
	
	void ReloadStore(CStreamDictionary& aDictionary, CFileStore& aStore);
	CTzRules* FindTzRuleByStreamIdL(TStreamId aStreamId, TBool aIsSystemRule);	
	
	void CheckTzDbModificationL(CAgnServFile& aAgnServFile);
	TTime TzRulesLastModifiedDateL();
private:
	CAgnTzRuleIndex(CStreamDictionary& aDictionary, CFileStore& aStore);
	void ConstructL();
	
	//Read index array to the file
	TStreamId ReadItemsFromStoreL(const TUid& aUid, TBool aRollback=EFalse);
	void ReadFromStoreL(const TUid& aUid, TBool aRollback);
	TStreamId PersistToStoreL(const TUid& aUid);
	
	void InternalizeRulesL(RReadStream& aStream, TBool aIsSystemRule, TBool aRollback);
	void ExternalizeRulesL(RWriteStream& aStream, const RPointerArray<CAgnTzRuleIndexItem>& aArray) const;

	//Update rules in the file
	void UpdateItemsInStoreL(RPointerArray<CAgnTzRuleIndexItem>& aArray, TStreamId aStreamId);
	TStreamId PersistTzRuleL(const CTzRules& aTzRule);
	void DeleteARuleL(TInt aPos, RPointerArray<CAgnTzRuleIndexItem>& aArray, TStreamId aStreamId);

	//Allocate an item from the array
	TInt FindTzRuleByStreamIdL(TStreamId aStreamId, RPointerArray<CAgnTzRuleIndexItem>& aArray, TBool aTryPosLastTime);
	TInt FindRule(RPointerArray<CAgnTzRuleIndexItem>& aRuleArray, const CTzRules& aRule);
	TInt FindRule(const CTzRules& aTzRule, TBool& aIsSystemRule);

	void AddCurrentSystemTzRuleL(CAgnTzRules& aTzZone);
	
	static CAgnTzRules* GetTzRulesFromEntry(CAgnSimpleEntry& aEntry);
	void RollbackItemL(RPointerArray<CAgnTzRuleIndexItem>& aRuleArray, CAgnTzRuleIndexItem* aItem);

	//Operation related to time zone database change
	TBool RefreshTzRulesL();
	void WriteTimeStampL();
	void ExternalizeTimesL(RStoreWriteStream& aWriteStream)const;
	void InternalizeTimesL(RReadStream& aReadStream);
	void ReadTimeStampL();
	
private:
	RPointerArray<CAgnTzRuleIndexItem> iSystemTzRules;
	RPointerArray<CAgnTzRuleIndexItem> iUserDefinedTzRules;
	
	CStreamDictionary* 		iDictionary; //Not owned
	CFileStore* 			iStore; //Not owned

	TStreamId 				iSystemRuleStreamId;//stream id where system rules stored
	TStreamId				iUserDefinedRuleStreamId;//stream id where user defined rules stored
	
	CAgnTlsProxy*			iTimeZoneAccessor;
	
	TInt					iPosCurSystemTzRule;
	TInt					iPosFetchedLastTime;
	TTime 					iTzDbChangeTimeStamp;		//The time stamp when the tz dababase modification has been checked
	TTime 					iTzRulesLastModifiedTime;	//The time stamp when cached tz rules have been modified.
	CAgnServFile*			iAgnFile; 					// Not owned
	};

inline TInt CAgnTzRuleIndexItem::ReferenceCount() const
	{return iRefCount;}

inline void CAgnTzRuleIndexItem::SetReferenceCount(TInt aReferenceCount)
	{iRefCount = aReferenceCount;}

#endif /*__AGSTZRULEINDEX_H_*/
