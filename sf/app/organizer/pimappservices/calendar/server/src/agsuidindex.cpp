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


#include "agsuidindex.h"

#include "agmsimpleentry.h"

const TInt KAgnIndexGranularity = 64;

//
// class TAgnUidIndexKey
//

TInt CAgnLocalUidIndex::TAgnUidIndexKey::Compare(TInt aLeft, TInt aRight) const
/**	Sorted array comparison function
	
@internalComponent	
*/
	{
	const CAgnSimpleEntry* KLeft = *static_cast<CAgnSimpleEntry**>( At(aLeft) );
	const CAgnSimpleEntry* KRight = *static_cast<CAgnSimpleEntry**>( At(aRight) );
	
	if ( KLeft->LocalUid() == KRight->LocalUid() )
		{
		return ( 0 );
		}

	return ( ( KLeft->LocalUid() < KRight->LocalUid() ) ? -1 : 1 );
	}


//
// class CAgnLocalUidIndex
//

CAgnLocalUidIndex* CAgnLocalUidIndex::NewL(CAgnSimpleEntryAllocator& aAllocator)
/**	Static constructor
	
@internalComponent	
*/
	{
	CAgnLocalUidIndex* self = new (ELeave) CAgnLocalUidIndex(aAllocator);
	
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	
	return ( self );
	}

void CAgnLocalUidIndex::ConstructL()
/**	Construct array with granularity of 64
	
@internalComponent	
*/
	{
	iSearchEntry = iAllocator.CreateSimpleEntryL(CCalEntry::EAppt);	
	
	iIndex = new (ELeave) CArrayPtrSeg<CAgnSimpleEntry>(KAgnIndexGranularity);
	}


void CAgnLocalUidIndex::AddL(const CAgnSimpleEntry* aEntry)
/**	Add a CAgnSimpleEntry reference

@internalComponent
*/
	{
	TInt pos = DoFindByLocalUid(aEntry->LocalUid());
	if (pos >= 0)
		{
		if ((*iIndex)[pos]->EntryId().Value() != aEntry->EntryId().Value())
			{
			User::Leave(KErrAlreadyExists);
			}
		// in this case entry is already in database
		}
	else
		{
		iIndex->InsertIsqL( const_cast<CAgnSimpleEntry*>(aEntry), iKey);
		}
	}


void CAgnLocalUidIndex::Delete(const CAgnSimpleEntry* aEntry)
/**	Delete a CAgnSimpleEntry reference

@internalComponent
*/
	{
	const TInt KIndex = DoFindByLocalUid(aEntry->LocalUid());

	if ( KIndex != KErrNotFound )
		{
		iIndex->Delete(KIndex);
		}
	}


TInt CAgnLocalUidIndex::DoFindByLocalUid(TCalLocalUid aLocalUid) const
/**	Get the index of the specified LocalUid, or KErrNotFound if not found

@internalComponent
@return Index of the specified LocalUid or KErrNotFound if not found
*/
	{
	iSearchEntry->SetLocalUid(aLocalUid);
	iSearchEntry->SetEntryId(TAgnEntryId());

	TInt pos = 0;	

	if ( iIndex->FindIsq(iSearchEntry, (TKeyArrayFix&) iKey, pos) == 0 )
		{
		return ( pos );
		}		

	return ( KErrNotFound );
	}


TAgnEntryId CAgnLocalUidIndex::FindByLocalUid(TCalLocalUid aLocalUid) const
/**	Find EntryId by LocalUid 

@internalComponent
@return Agenda entry id matching the passed unique id
*/
	{
	const TInt KIndex = DoFindByLocalUid(aLocalUid);

	if ( KIndex != KErrNotFound )
		{
		return ( (*iIndex)[KIndex]->EntryId() );
		}

	return TAgnEntryId();
	}


//
// class TGsGuidHashIndexKey
//

TInt CGsGuidHashIndex::TGsGuidHashIndexKey::Compare(TInt aLeft, TInt aRight) const
/**	Sorted array comparison function
	
@internalComponent	
*/
	{
	const CAgnSimpleEntry* KLeft = *static_cast<CAgnSimpleEntry**>( At(aLeft) );
	const CAgnSimpleEntry* KRight = *static_cast<CAgnSimpleEntry**>( At(aRight) );

	// the sort order is by GuidHash first and by LocalUid second
	// NOTE: don't change this logic, otherwise FindHash won't work correctly

	if ( KLeft->GuidHash() == KRight->GuidHash() )
		{
		if ( KLeft->LocalUid() == KRight->LocalUid() )
			{
			return ( 0 );
			}
			
		return ( ( KLeft->LocalUid() < KRight->LocalUid() ) ? -1 : 1 );
		}
		
	return ( ( KLeft->GuidHash() < KRight->GuidHash() ) ? -1 : 1 );		
	}


//
// class CGsGuidHashIndex
//

CGsGuidHashIndex* CGsGuidHashIndex::NewL(CAgnSimpleEntryAllocator& aAllocator)
/**	Static constructor
	
@internalComponent	
*/
	{
	CGsGuidHashIndex* self = new (ELeave) CGsGuidHashIndex(aAllocator);
	
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	
	return ( self );
	}


void CGsGuidHashIndex::ConstructL()
/**	Construct array with granularity of 64

@internalComponent
*/
	{
	iSearchEntry = iAllocator.CreateSimpleEntryL(CCalEntry::EAppt);	
	
	iIndex = new (ELeave) CArrayPtrSeg<CAgnSimpleEntry>(KAgnIndexGranularity);
	}

void CGsGuidHashIndex::AddL(const CAgnSimpleEntry* aEntry)
/**	Add a CAgnSimpleEntry reference

@internalComponent
*/
	{
	TInt pos = DoFind(aEntry);
	if (pos == KErrNotFound)
		{
		// in this case entry is not already in database
		iIndex->InsertIsqL( const_cast<CAgnSimpleEntry*>(aEntry), iKey);	
		}
	}


void CGsGuidHashIndex::Delete(const CAgnSimpleEntry* aEntry)
/**	Delete a CAgnSimpleEntry reference

@internalComponent
*/
	{
	const TInt KIndex = DoFind(aEntry);

	if ( KIndex != KErrNotFound )
		{
		iIndex->Delete(KIndex);
		}
	}


TInt CGsGuidHashIndex::DoFind(const CAgnSimpleEntry* aEntry) const
/**	Get the index of the specified LocalUid / GuidHash, or KErrNotFound if not found

@internalComponent
@return Index of the specified LocalUid / GuidHash, or KErrNotFound if not found
*/
	{
	TInt pos = 0;	

	if ( iIndex->FindIsq(const_cast<CAgnSimpleEntry*>(aEntry), (TKeyArrayFix&) iKey, pos) == 0 )
		{
		return ( pos );
		}		

	return ( KErrNotFound );	
	}


void CGsGuidHashIndex::FindByHashL(const TUint32& aGuidHash, RArray<TAgnEntryId>& aFoundIdList) const
/**	Get all the LocalUid matching the passed GuidHash

@internalComponent
*/
	{
	iSearchEntry->SetGuidHash(aGuidHash);
	iSearchEntry->SetLocalUid(0);

	// The array is sorted by GuidHash first and LocalUid second
	// The FindIsq in this case looks for the first occurence of GuidHash (because LocalUid is 0)
	// We ignore the return value by purpose because it will negative anyway and nothing bad will happen.

	TInt pos = 0;
	// coverity[check_return] coverity[unchecked_value]
	iIndex->FindIsq(iSearchEntry, (TKeyArrayFix&) iKey, pos);	
	
	const TInt KSize = iIndex->Count();
	for ( ; pos < KSize; ++pos )
		{
		const CAgnSimpleEntry* KEntry = (*iIndex)[pos];

		if ( aGuidHash == KEntry->GuidHash() )
			{
			// append to the passed array any LocalUid related to the GuidHash match
			
			aFoundIdList.AppendL(KEntry->EntryId());
			}
		else
			{
			// there are no more items in the array matching the GuidHash
			// break the for-loop and exit
			break;
			}
		}	
	}
