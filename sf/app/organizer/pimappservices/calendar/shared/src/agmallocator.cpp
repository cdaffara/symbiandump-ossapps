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

#include "agmallocator.h"
#include "agmsimpleentry.h"
#include "agmpanic.h"

// ----------------------------------- CAgnSimpleEntryAllocator -------------------

EXPORT_C CAgnSimpleEntry* CAgnSimpleEntryAllocator::CreateSimpleEntryL(CCalEntry::TType aType, const CAgnSimpleEntry* aEntry)
/** Create CAgnSimpleEntry objects

@internalComponent
*/
	{
	CAgnSimpleEntry* entry = new (ELeave, this) CAgnSimpleEntry(aType);
	
	if( aEntry )
		{
		TRAPD(err, entry->CopySimpleEntryL(*aEntry, CCalEntry::ECopyAll));
		if (err != KErrNone)
			{
			Delete(entry);
			User::Leave(err);
			}
		}
	
	return ( entry );
	}

EXPORT_C CAgnSimpleEntryAllocator::CAgnSimpleEntryAllocator()
	{
	}

EXPORT_C CAgnSimpleEntryAllocator::~CAgnSimpleEntryAllocator()
/** Destructor

@internalComponent
*/
	{
	// Cleanup up any hanging around entries
	
	while ( iHeadPage )
		{
		CAgnMemPage* next = iHeadPage;
		
		iHeadPage = iHeadPage->Next();
		
		delete next;
		}
	}


CAgnSimpleEntry* CAgnSimpleEntryAllocator::NewSimpleEntryL(TUint aSize)
/** return pointer to a CAgnSimpleEntry

@internalComponent
*/
	{
	CAgnMemPage* availPage = iHeadPage;
	
	while( availPage && (availPage->IsFull() || availPage->CellSize() != aSize) )
		{
		availPage = availPage->Next();			
		}

	if ( ! availPage )
		{
		availPage = CAgnMemPage::NewL(aSize);
		availPage->SetNext(iHeadPage);
		iHeadPage = availPage;
		}
		
	++iCount;
	
	return ( static_cast<CAgnSimpleEntry*>(availPage->Alloc()) );
	}


EXPORT_C void CAgnSimpleEntryAllocator::Delete(CAgnSimpleEntry* aPtr)
/** Put freed CAgnSimpleEntry on free list

@internalComponent
*/
	{
	if ( aPtr )
		{
		CAgnMemPage* next = iHeadPage;
		
		while ( next && ! next->Contains(aPtr) )
			{
			next = next->Next();				
			}
			
		if ( next )
			{
			// clean up any memory allocated by the object itself
			// aPtr is only put in the free pool when Free is called
			
			aPtr->~CAgnSimpleEntry();
			
			next->Free();
			
			if ( next->IsEmpty() )
				{
				DeleteMemPage(next);					
				}
			}

		--iCount;
		
		if ( iCount == 0 )		// no objects so delete everything
			{
			while ( iHeadPage )
				{
				CAgnMemPage* next = iHeadPage;
				
				iHeadPage = iHeadPage->Next();
				
				delete next;
				}
			}
		}		
	}


void CAgnSimpleEntryAllocator::DeleteMemPage(CAgnMemPage* aPage)
/** Delete a page of entries

@internalComponent
*/
	{
	__ASSERT_DEBUG(iHeadPage, Panic(EAgmNoAllocatorMemoryPages));

	CAgnMemPage* prev    = NULL;
	CAgnMemPage* current = iHeadPage;
	CAgnMemPage* next    = iHeadPage->Next();

	while ( current != aPage && next )
		{
		prev = current;
		current = next;
		next = next->Next();
		}
		
	__ASSERT_DEBUG(current, Panic(EAgmAllocatorMemoryPageNotFound));

	if ( current == iHeadPage )
		{
		iHeadPage = next;			
		}
	else
		{
		prev->SetNext(next);
		}
	
	delete aPage;
	}


// ----------------------------------- CAgnMemPage -----------------------------
//
// Manages allocation and freeing of memory cells
//

CAgnMemPage* CAgnMemPage::NewL(TUint aCellSize)
//
// 
//
	{
	CAgnMemPage* self = new (ELeave) CAgnMemPage(aCellSize);
	
    CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	
	return ( self );
	}


void CAgnMemPage::ConstructL()
/** Create array of KMemPageSize memory cells

@internalComponent
*/
	{
	const TInt KSize = iCellSize * KMemPageSize;
	
	iEntries = (TInt8*) User::AllocL(KSize);
	
	Mem::FillZ(iEntries, KSize);
	
	iNextAvail = iEntries;
	}


CAgnMemPage::~CAgnMemPage()
/** Destructor

@internalComponent
*/
	{	
	User::Free(iEntries);
	}


TAny* CAgnMemPage::Alloc()
/** Return pointer to available memory cell or Null if none available

@internalComponent
*/
	{
	if ( IsFull() )
		{
		return NULL;			
		}

	TAny* alloc = iNextAvail;
	iNextAvail += iCellSize;
		
	++iCount;
	
	return ( alloc );
	}


void CAgnMemPage::Free()
/** Free memory cell

@internalComponent
*/
	{
	--iCount;
	}

