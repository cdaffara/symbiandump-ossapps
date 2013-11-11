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

#ifndef __AGSINDEXBASE_H__
#define __AGSINDEXBASE_H__

#include "agsindexkey.h"

// ----------------------------------- Referenced classes ----------------------
class CAgnSimpleEntry;
class CAgnEntry;
class CAgnSimpleEntryAllocator;

// ------------------------- CAgnIndex<Entry, Key, KeyValue> -------------------
class CAgnIndex : public CBase
	{
public:
	CAgnIndex(TAgnKeyBase<CAgnSimpleEntry,TTime>& aKey,MAgnCalendarTimeMode::TTimeMode aTimeMode);
	~CAgnIndex();
	TBool AddL(CAgnSimpleEntry* const aEntry);
	TBool FindL(const TTime& aDate, TInt& aPos,TBool aFirst = ETrue) const;
	virtual TBool FirstL(TTime& aDate, TInt& aPos) const;
	virtual TBool FirstLessOrEqualL(TTime& aDate, TInt& aPos) const;
	virtual TBool NextL(TTime& aDate, TInt& aPos) const;
	virtual TBool PreviousL(TTime& aDate, TInt& aPos) const;
	void GetKeyValueL(TInt Pos, TTime& aDate) const;
	virtual void GetKeyValueL(CAgnSimpleEntry* const, TTime& aDate) const;
	TBool DeleteEntry(CAgnSimpleEntry* const aEntry); 
	
	virtual TBool IsValid(const CAgnSimpleEntry* aInfo) const;
	TInt Count() const;
	TBool AppendL(CAgnSimpleEntry* const aEntry);
	CAgnSimpleEntry* operator[](TInt anIndex) const;
	CAgnSimpleEntry* Get(TInt anIndex) const;
	void Delete(TInt aPos);
	void Compress();
	void Reset();
	void SetUndatedTodoTime(TTime& aUndatedTodoTime);

	MAgnCalendarTimeMode::TTimeMode TimeMode() const;

	void InternalizeL(RReadStream& aReadStream, CAgnSimpleEntryAllocator* aAllocator);
	void ExternalizeL(RWriteStream& aWriteStream) const;

protected:
	void ConstructL();
	TBool FirstValidL(TTime& aKeyValue, TInt& aPos) const;
private:
	TInt FindWithEntry(CAgnSimpleEntry* const aEntry);
	CArrayFixSeg<CAgnSimpleEntry*>* Array() const;
	
private:
	CArrayFixSeg<CAgnSimpleEntry*>* iArray;
	TAgnKeyBase<CAgnSimpleEntry,TTime>& iKey;						// defines the index ordering
	const MAgnCalendarTimeMode::TTimeMode iTimeMode;
	};

#endif
