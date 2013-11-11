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

#ifndef __AGMALLOCATOR_H__
#define __AGMALLOCATOR_H__

#include "calentry.h"

// -------------------------------------- Local classes ------------------------

class CAgnMemPage;
class CAgnSimpleEntry;

// ----------------------------------- CAgnSimpleEntryAllocator ------------------

NONSHARABLE_CLASS(CAgnSimpleEntryAllocator) : public CBase
/**
Class for controlling the allocation and deletion of simple entries.
These are created in a paged area of memory to prevent too much RAM defragmentation when there
are 1000s of entries in the indexes on the server. 
CAgnSimpleEntrys are pre-allocated in blocks of KMemPageSize entries

@internalComponent
@released
*/
	{
public:

	IMPORT_C CAgnSimpleEntryAllocator();
	IMPORT_C ~CAgnSimpleEntryAllocator();
	IMPORT_C CAgnSimpleEntry* CreateSimpleEntryL(CCalEntry::TType aType, const CAgnSimpleEntry* aEntry = NULL);
	IMPORT_C void Delete(CAgnSimpleEntry* aEntry);

private:
	friend class CAgnSimpleEntry;

	void DeleteMemPage(CAgnMemPage* aPage);
	CAgnSimpleEntry* NewSimpleEntryL(TUint aSize);	
	
private:	
	CAgnMemPage*	iHeadPage;
	TInt			iCount;
	};


NONSHARABLE_CLASS(CAgnMemPage) : public CBase
/**
@internalComponent
@released
*/
	{
public:

	static CAgnMemPage* NewL(TUint aCellSize);
	~CAgnMemPage();

	TAny* Alloc();
	void Free();
	
	inline TBool Contains(const TAny* aPtr) const;
	inline TUint CellSize() const;
	inline TBool IsFull() const;
	inline TBool IsEmpty() const;
	inline void SetNext(CAgnMemPage* aNext);
	inline CAgnMemPage* Next() const;	


private:

	inline CAgnMemPage(TUint aCellSize);
	void ConstructL();


private:

	CAgnMemPage*	iNext;
	TInt8*			iEntries;
	TInt8*			iNextAvail;
	TInt			iCount;
	TUint			iCellSize;
	
	};


//
// inline
//


const TInt KMemPageSize = 64;


inline CAgnMemPage::CAgnMemPage(TUint aCellSize)
	: iCellSize(aCellSize)
/** Constructor

@internalComponent
*/
	{
	}


inline TUint CAgnMemPage::CellSize() const
/** Return size of each cell(element)

@internalComponent
*/
	{
	return ( iCellSize );
	}


inline TBool CAgnMemPage::IsFull() const
/** Are there any memory cell not allocated?

@internalComponent
*/
	{
	return ( ( iNextAvail - iEntries ) / iCellSize == KMemPageSize );
	}


inline TBool CAgnMemPage::IsEmpty() const
/** Are any entries allocated?

@internalComponent
*/
	{
	return ( iCount == 0 );
	}


inline void CAgnMemPage::SetNext(CAgnMemPage* aNext)
/** Set pointer to the next page of memory cell

@internalComponent
*/
	{	
	iNext = aNext;
	}


inline CAgnMemPage* CAgnMemPage::Next() const
/** Return pointer ot next page of memory cell

@internalComponent
*/
	{
	return ( iNext );
	}


inline TBool CAgnMemPage::Contains(const TAny* aPtr) const
/** Is aPtr allocated from this page

@internalComponent
*/
	{
	return ( aPtr >= iEntries && aPtr <= iNextAvail );
	}


#endif

