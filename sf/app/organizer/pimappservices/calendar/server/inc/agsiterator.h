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

#ifndef __AGSITERATOR_H__
#define __AGSITERATOR_H__

#include "agmfilter.h"
#include "agmids.h"

// ----------------------------------- Reference classes -----------------------
class CAgnEntryManager;
class CAgnIndex;
class CAgnSimpleEntry;
class CAgnStreamIdSet;

// ----------------------------------- TAgnIteratorBase-------------------------
class TAgnIteratorBase
/**
Base class for instance iterators.
@internalAll
@released
*/
	{
public:
	TAgnIteratorBase() {};
	virtual void StartL() = 0;
	virtual void NextL() = 0;
	virtual void PreviousL() = 0;
	virtual TBool AtEnd() = 0;
	virtual TBool AtStart() = 0;
	virtual TBool GotoL(TTime& aKeyValue) = 0;
	virtual TBool GotoLessOrEqualL(TTime& aKeyValue) = 0;
	virtual const TTime& CurrentKey() const = 0;
	virtual const CAgnSimpleEntry& CurrentElement() const = 0;
	};
	
class TAgnIterator : public TAgnIteratorBase
/**
@internalAll
@released
*/
	{
public:	

    TAgnIterator();
	TAgnIterator(CAgnIndex* aIndex, const TAgnFilter& aFilter);	
	void StartL();
	virtual void NextL();
	virtual void PreviousL();
	virtual TBool GotoL(TTime& aKeyValue);
	virtual TBool GotoLessOrEqualL(TTime& aKeyValue);
	const CAgnSimpleEntry& CurrentElement() const;
	virtual const TTime& CurrentKey() const;
	virtual TBool AtEnd();
	virtual TBool AtStart();

	inline void SetIndex(CAgnIndex& aSimpleEntryIndex);
	inline void SetFilter(const TAgnFilter& aFilter);
	inline const TAgnFilter* Filter();
	inline const void SetUndatedTodoTime(const TTime& aUndatedTodoTime);
	
protected:
	TBool IsValidElement();			// Current element valid w.r.t. filter
	void SetCursor(TInt aCursor);
	TInt Cursor() const;
	const CAgnIndex* Array() const;
	void SetCurrentKey(const TTime& aKeyValue);
	TBool FirstL(TTime& aTargetKey);
	TBool FirstLessOrEqualL(TTime& aTargetKey);
	virtual TBool MovedForwardL(TTime /*aTargetValue*/) {return ETrue;}
	virtual TBool MovedToL(TTime /*aTargetValue*/) {return ETrue;}
	virtual TBool MovedBackL(TTime /*aTargetValue*/) {return ETrue;}
	TTime iCurrentKey;
	TTime iPreviousKey;
	TTime iUndatedTodoTime;
private:
	TInt iCursor;
    TAgnFilter iFilter;
	CAgnIndex* iArray;
	};

class TAgnEntryIter
/** Iterates over the entries in an agenda file. 

@internalAll
@released
*/
	{
public:		 
	TAgnEntryIter(CAgnStreamIdSet& aStreamIdSet, CAgnEntryManager& aEntryManager);
	TBool SetToFirstL();	// position to the first entry
	TBool NextL();		// position to the next entry
	TAgnEntryId At();	// return the entry id of the current entry
	
private:	
	CAgnStreamIdSet&	iStreamIdSet;
	CAgnEntryManager&	iEntryManager;
	TInt				iEntryIter;
	TInt				iEntryCount;
	};
	
#include "agsiterator.inl"

#endif
