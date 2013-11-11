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


#ifndef __AGSUIDINDEX_H__
#define __AGSUIDINDEX_H__

#include "agmids.h"
#include "agmallocator.h"

class CAgnSimpleEntry;
class CAgnSimpleEntryAllocator;

//
// CAgnLocalUidIndex
//

class CAgnLocalUidIndex : public CBase
	{
/**
	Unique ID based index
	
@internalComponent
@released	
*/	
public:

	static CAgnLocalUidIndex* NewL(CAgnSimpleEntryAllocator& aAllocator);

	inline ~CAgnLocalUidIndex();	

	void AddL(const CAgnSimpleEntry* aEntry);
	void Delete(const CAgnSimpleEntry* aEntry);
	void Compress();
	inline TInt Count() const;	
	inline void Reset();	

	TAgnEntryId FindByLocalUid(TCalLocalUid aLocalUid) const;


private:

	inline CAgnLocalUidIndex(CAgnSimpleEntryAllocator& aAllocator); 
	void ConstructL();

	TInt DoFindByLocalUid(TCalLocalUid aLocalUid) const;


private:

	//
	// TAgnUidIndexKey
	//

	class TAgnUidIndexKey : public TKeyArrayFix
		{
	/**
		Unique ID index key
		
	@internalComponent
	@released	
	*/	
	public:

		inline TAgnUidIndexKey()
			: TKeyArrayFix(0, ECmpNormal, sizeof(CAgnSimpleEntry*)) { }
					
		virtual TInt Compare(TInt aLeft, TInt aRight) const;

		};	
	

private:

	CAgnSimpleEntry* 				iSearchEntry;
	CArrayPtrSeg<CAgnSimpleEntry>*	iIndex;	// the index (list of simple entries)
	TAgnUidIndexKey					iKey;	// defines the index ordering
	CAgnSimpleEntryAllocator& 		iAllocator;	

	};


//
// CGsGuidHashIndex
//

class CGsGuidHashIndex : public CBase
	{
/**
	Guid Hash based index
	
@internalComponent
@released	
*/	
public:

	static CGsGuidHashIndex* NewL(CAgnSimpleEntryAllocator& aAllocator);

	inline ~CGsGuidHashIndex();

	void AddL(const CAgnSimpleEntry* aEntry);
	void Delete(const CAgnSimpleEntry* aEntry);
	void Compress();	
	inline TInt Count() const;
	inline void Reset();

	void FindByHashL(const TUint32& aGuidHash, RArray<TAgnEntryId>& aFoundIdList) const;


private:

	inline CGsGuidHashIndex(CAgnSimpleEntryAllocator& aAllocator);
	void ConstructL();

	TInt DoFind(const CAgnSimpleEntry* aEntry) const;


private:

	//
	// TGsGuidHashIndexKey
	//

	class TGsGuidHashIndexKey : public TKeyArrayFix
		{
	/**
		Guid Hash index key
		
	@internalComponent
	@released	
	*/	
	public:

		inline TGsGuidHashIndexKey()
			: TKeyArrayFix(0, ECmpNormal, sizeof(CAgnSimpleEntry*)) { }
			
		virtual TInt Compare(TInt aLeft, TInt aRight) const;

		};


private:

	CAgnSimpleEntry* 				iSearchEntry;
	CArrayPtrSeg<CAgnSimpleEntry>*	iIndex;	// the index (list of simple entries)
	TGsGuidHashIndexKey				iKey; // defines the index ordering
	CAgnSimpleEntryAllocator& 		iAllocator;

	};


// Inline functions

//
// class CAgnLocalUidIndex
//


inline CAgnLocalUidIndex::CAgnLocalUidIndex(CAgnSimpleEntryAllocator& aAllocator) 
	: iAllocator(aAllocator)
	{
	}

inline CAgnLocalUidIndex::~CAgnLocalUidIndex()
	{
	delete iIndex;
	
	iAllocator.Delete(iSearchEntry);	
	}


inline void CAgnLocalUidIndex::Compress()
/**	Compress index

@internalComponent
*/
	{
	iIndex->Compress();
	}


inline TInt CAgnLocalUidIndex::Count() const
/**	Get number of index entries

@internalComponent
@return Number of index entries
*/
	{
	return ( iIndex->Count() );
	}


inline void CAgnLocalUidIndex::Reset()
/**	Reset index

@internalComponent
*/
	{
	iIndex->Reset();
	}


//
// class CGsGuidHashIndex
//


inline CGsGuidHashIndex::CGsGuidHashIndex(CAgnSimpleEntryAllocator& aAllocator) 
	: iAllocator(aAllocator)
	{
	}

inline CGsGuidHashIndex::~CGsGuidHashIndex()
	{
	delete iIndex;
	
	iAllocator.Delete(iSearchEntry);
	}


inline void CGsGuidHashIndex::Compress()
/**	Compress index

@internalComponent
*/
	{
	iIndex->Compress();
	}


inline TInt CGsGuidHashIndex::Count() const
/**	Get number of index entries

@internalComponent
@return Number of index entries
*/
	{
	return ( iIndex->Count() );
	}


inline void CGsGuidHashIndex::Reset()
/**	Reset index

@internalComponent
*/
	{
	iIndex->Reset();
	}


#endif
