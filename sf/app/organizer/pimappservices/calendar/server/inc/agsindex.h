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

#ifndef __AGSINDEX_H__
#define __AGSINDEX_H__

#include "agsindexbase.h"
#include <calcommon.h>
#include "agsextractor.h"

// -------------------------------- Local classes ------------------------------

class CAgnEntry;
class CAgnEntryNonRptIndex;
class CAgnEntryRptIndex;
class CAgnSimpleEntry;
class CAgnSimpleEntryAllocator;
class CAgnSimpleEntryTable;
class CAgnTodoNonRptIndex;
class CAgnTodoRptIndex;
class CAgnLocalUidIndex;
class CGsGuidHashIndex;
class TAgnFilter;
class TAgnInstanceExtractorBase;
class TAgnKeyEntryInfo;
class CAgnTzRuleIndex;

// -------------------------------- Referenced classes--------------------------

class CAgnEntryModel;

//-------------------------------- CAgnEntryNonRptIndex-----------------------------
//
class CAgnEntryNonRptIndex : public CAgnIndex
	{
public:
	static CAgnEntryNonRptIndex* NewL(MAgnCalendarTimeMode::TTimeMode aTimeMode);
	
	// from CAgnIndex
	TBool IsValid(const CAgnSimpleEntry* aEntry) const;
private:
	CAgnEntryNonRptIndex(MAgnCalendarTimeMode::TTimeMode aTimeMode);
private:
	TAgnKeyStartDate iKey;
	};
//-------------------------------- CAgnEntryRptIndex-----------------------------
//
class CAgnEntryRptIndex : public CAgnIndex
	{
public:
	static CAgnEntryRptIndex* NewL(MAgnCalendarTimeMode::TTimeMode aTimeMode);

	// from CAgnIndex
	TBool IsValid(const CAgnSimpleEntry* aEntry) const;
private:
	CAgnEntryRptIndex(MAgnCalendarTimeMode::TTimeMode aTimeMode);
private:
	TAgnKeyRptDate iKey;
	};

//-------------------------------- CAgnTodoNonRptIndex -------------------------
class CAgnTodoNonRptIndex : public CAgnIndex
	{
public:
	static CAgnTodoNonRptIndex* NewL(MAgnCalendarTimeMode::TTimeMode aTimeMode);

	// from CAgnIndex
	TBool IsValid(const CAgnSimpleEntry* aEntry) const;
	TBool FirstL(TTime& aKeyValue, TInt& aPos) const;
    TBool NextL(TTime& aKeyValue, TInt& aPos) const;
private:
	CAgnTodoNonRptIndex(MAgnCalendarTimeMode::TTimeMode aTimeMode);
private:
	TAgnKeyTodoNonRptDate iKey;
	};

//-------------------------------- CAgnTodoRptIndex-----------------------------
//
class CAgnTodoRptIndex : public CAgnIndex
	{
public:
	static CAgnTodoRptIndex* NewL(MAgnCalendarTimeMode::TTimeMode aTimeMode);

	// from CAgnIndex
	TBool IsValid(const CAgnSimpleEntry* aEntry) const;
private:
	CAgnTodoRptIndex(MAgnCalendarTimeMode::TTimeMode aTimeMode);
private:
	TAgnKeyRptDate iKey;
	};

//-------------------------------- CAgnSimpleEntryTable --------------------------

class CAgnSimpleEntryTable : public CBase
	{
	// friend classes for iterating through entries
	friend class TAgnEntryDateIterator;
	
public:

	enum TAgnSimpleEntryIndex
		{
		EAgnEntryNonRptFixedIndex,
		EAgnEntryRptFixedIndex,
		EAgnTodoNonRptFixedIndex,
		EAgnTodoRptFixedIndex,
		EAgnEntryNonRptFloatingIndex,
		EAgnEntryRptFloatingIndex,
		EAgnTodoNonRptFloatingIndex,
		EAgnTodoRptFloatingIndex,
		EAgnMaxIndex,
		};

	static CAgnSimpleEntryTable* NewL(CAgnEntryModel& aEntryModel);
	~CAgnSimpleEntryTable();
	
	void  AddEntryL(const CAgnEntry& aEntry);	
	void  DeleteEntry(const TAgnEntryId& aId);

 	void  Commit();
	void  Reset();		// clear all entries from indices

	CAgnSimpleEntry* GetEntry(const TAgnEntryId& aEntryId) const;
	CAgnSimpleEntry* GetEntry(TCalLocalUid aUniqueId) const;

	void  FindByLastModifiedDateUtcL(const TTime& aLastModifiedDateUtcFilter, 
								  	 RArray<TCalLocalUid>& aFoundIdList) const;
	void  FindByHashL(const TUint32& aGuidHash, RArray<TAgnEntryId>& aFoundIdList) const;								  	
	
	void  GetFilteredIndexListL(RArray<TInt>& aIndexList,const TAgnFilter& aFilter) const;
	CAgnInstanceExtractor* GetExtractor(TInt aEntryIndex,MAgnCalendarTimeMode::TTimeMode aTimeMode, const TAgnFilter& aFilter, const TTime& aUndatedTodoTime);

	void InternalizeL(RReadStream& aReadStream, CAgnTzRuleIndex* aTzRuleIndex);
	void ExternalizeL(RWriteStream& aWriteStream) const;

private:

	// constructor
	CAgnSimpleEntryTable(CAgnEntryModel& aAgnEntryModel);
	void ConstructL();

	void AddEntryL(CAgnSimpleEntry* aSimpleEntry);
	// Delete entry
	void DeleteEntryFromIndexes(CAgnSimpleEntry* aEntry);

	TBool Find(const TAgnEntryId& aId, TInt& aPos) const;

	static TInt CompareEntries(const CAgnSimpleEntry& aLeft, const CAgnSimpleEntry& aRight);
	
	CAgnIndex* Index(TInt aIndex) const;
	CAgnIndex* EntryNonRptIndex(const MAgnCalendarTimeMode::TTimeMode aTimeMode) const;
	CAgnIndex* EntryRptIndex(const MAgnCalendarTimeMode::TTimeMode aTimeMode) const;
	CAgnIndex* TodoNonRptIndex(const MAgnCalendarTimeMode::TTimeMode aTimeMode) const;
	CAgnIndex* TodoRptIndex(const MAgnCalendarTimeMode::TTimeMode aTimeMode) const;

	RPointerArray<CAgnIndex>			iIndexes;
	RPointerArray<CAgnSimpleEntry>		iArray;
	CAgnSimpleEntry*					iFindEntry;
	CAgnSimpleEntryAllocator*			iSimpleEntryAllocator;

	CAgnInstanceExtractor*				iNonRptExtractor;
	CAgnInstanceExtractor*				iRptExtractor;
	CAgnInstanceExtractor*				iTodoNonRptExtractor;
	CAgnInstanceExtractor*				iTodoRptExtractor;
	
	CAgnLocalUidIndex*					iLocalUidIndex;
    CGsGuidHashIndex*					iGuidHashIdIndex;	
	CAgnEntryModel* 					iOwningModel;  	// the entry model that owns this table
														// used for updating the cached index

	};

#endif
