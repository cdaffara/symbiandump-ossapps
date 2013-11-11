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

#ifndef __AGSINDEXITERATOR_H__
#define __AGSINDEXITERATOR_H__

#include "agsiterator.h"

// ------------------------------Referenced classes-----------------------------
class CAgnSimpleEntry;
class CAgnSimpleEntryTable;
class TAgnFilter;

// ----------------------------TAgnTodoNonRptIterator---------------------------
// Iterator - Non repeating todos
//
class TAgnTodoNonRptIterator : public TAgnIterator
/**
@internalAll
@released
*/
	{
public:
	TAgnTodoNonRptIterator();
	TAgnTodoNonRptIterator(CAgnIndex* aIndex,const TAgnFilter& aFilter);
	
	virtual TBool GotoL(TTime& aKeyValue);
	virtual TBool GotoLessOrEqualL(TTime& aKeyValue);
	};


// ----------------------------TAgnEntryDateIterator----------------------------
// Compound Entry Iterator 
//
class TAgnEntryDateIterator
/**
@internalAll
@released
*/
	{
public:
	TAgnEntryDateIterator(CAgnSimpleEntryTable& aEntryInfoTable, const TTime& aUndatedTodoTime, const TAgnFilter& aFilter);
	virtual ~TAgnEntryDateIterator();
	
	const CAgnSimpleEntry& CurrentElement() const;
    const TTime& CurrentKey() const;
    void StartL();
    void NextL();
    void PreviousL();
    TBool AtEnd();
    TBool AtStart();
    TBool GotoL(TTime& aKeyValue);
    TBool GotoLessOrEqualL(TTime& aKeyValue);
    TBool SetCurrent();
	TBool SetCurrentLessOrEqual();
	
private:

	TAgnIterator* iIteratorsInUse[8]; // pointers to those iterators below that are selected by aFilter
    TAgnIterator iNonRptFixedIter;
    TAgnIterator iNonRptFloatingIter;
    TAgnIterator iRptFixedIter;
    TAgnIterator iRptFloatingIter;
	TAgnTodoNonRptIterator iTodoNonRptFixedIter;
	TAgnTodoNonRptIterator iTodoNonRptFloatingIter;
	TAgnIterator iTodoRptFixedIter;
	TAgnIterator iTodoRptFloatingIter;
	
	TInt iCount; // number of iterators to use (as per filter)
	TInt iCurrent; // iterator being used
	TTime iCurrentKey; 
	TTime iPreviousKey;
	
	CAgnIndex* iArray;
	};


#endif
