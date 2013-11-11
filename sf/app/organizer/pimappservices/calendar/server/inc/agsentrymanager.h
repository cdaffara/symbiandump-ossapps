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

#ifndef __AGSENTRYMANAGER_H__
#define __AGSENTRYMANAGER_H__

#include <s32std.h>

// ---------------------- Referenced classes -----------------------------------
class CAgnEntry;
class TAgnEntryId;
class TAgnEntryIter;

class CAgnEntryManager : public CBase
/**
Manages adding,deleting and updating entries in the store.
@internalAll
@released
*/
	{
public:
	friend class TAgnEntryIter;
	enum TBufferType {ETodo,ERpt,EGeneral,EIterator};
	
	static CAgnEntryManager* NewL();
	~CAgnEntryManager();
	TStreamId AddEntryL(CAgnEntry& aEntry);
	TStreamId DeleteEntryL(const TAgnEntryId& aEntryId);
	CAgnEntry* FetchEntryL(const TAgnEntryId& aEntryId);
	TStreamId UpdateEntryL(CAgnEntry& aEntry,TStreamId& aNewStreamId);
	void StoreBuffersL();
	void FlushBuffersL();
	void FreeBuffers();
	inline TBool BufferedDeleting() const;
	void SetBufferedDeleting(TBool aSetting);
	TAgnEntryId NextAvailableIdL(TBufferType aBufferType);
	inline void SetStore(CStreamStore& aStore);
	inline CStreamStore& StreamStore() const;
	inline TBool BufferHasBeenStored() const;
	void Reset();
	TInt RestoreBufferL(const TStreamId& aStreamId);
	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;
	void CopyStreamIds(const CAgnEntryManager& aOriginalManager);
private:
	class TBufItem
	/**
	@internalAll
	@released
	*/
		{
	public:
		inline TBufItem() {};
		inline TBufItem(CAgnEntry* aEntry, TBool aToBeDeleted=EFalse); 
		inline TBool ToBeDeleted() const;
		inline void SetToBeDeleted(TBool aSetting);
		inline void SetEntry(CAgnEntry* aEntry);
		inline CAgnEntry* Entry() const;
	private:		
		CAgnEntry*	iEntry;
		TBool		iToBeDeleted;
		};
	typedef CArrayFixFlat<TBufItem> CBuffer;

	CAgnEntryManager();
	const CAgnEntry* operator[](TInt aIndex) const;
	void StoreBufferL(const TStreamId& aStreamId, const CBuffer* aBuffer, TBufferType aType);
	TUint FindUniqueIdPortion(const CBuffer* aBuffer);
	CAgnEntry* SearchBuffers(const TAgnEntryId& aEntryId, TInt& aPos);
	void SetBufferHasBeenStored(TBool aHasBeenStored, TBufferType aType);
	void FreeBuffer(TBufferType aBufferType);
	void DeleteEntriesInBufferFromStoreL(CBuffer* aBuffer, TInt& aNumEntriesToDelete);	
	TStreamId DoAddEntryL(CAgnEntry& aEntry, TStreamId& aCurrentStreamId, TStreamId& aNextAvailableStreamId, CBuffer* aBuffer, TBufferType aBufferType);
	TStreamId DoDeleteEntryL(CAgnEntry* aEntry, CBuffer* aBuffer, TInt& aNumEntriesToDelete, TStreamId& aNextAvailableStreamId,
							 const TStreamId& aCurrentStreamId, TBufferType aBufferType, TInt aPos);
	void StoreAndMaybeFreeBuffersL(TBool aFreeBuffers);

	CBuffer*				iGeneralBuffer;
	CBuffer*				iTodoBuffer;
	CBuffer*				iRptBuffer;	
	CBuffer*				iIteratorBuffer;
	TStreamId				iNextAvailableGeneralStreamId;
	TStreamId				iNextAvailableTodoStreamId;
	TStreamId				iNextAvailableRptStreamId;
	TStreamId				iCurrentGeneralStreamId;
	TStreamId				iCurrentTodoStreamId;
	TStreamId				iCurrentRptStreamId;
	TInt					iNumGeneralEntriesToDelete;
	TInt					iNumTodoEntriesToDelete;
	TInt					iNumRptEntriesToDelete;
	TBool					iGeneralBufferHasBeenStored;
	TBool					iTodoBufferHasBeenStored;
	TBool					iRptBufferHasBeenStored;
	TBool					iBufferHasBeenStored;
	CStreamStore*			iStore;	
	TBool					iBufferedDeleting;
	TBufferType				iLastRestored;
	mutable TBool			iUseIteratorBuffer;
	};

#include "agsentrymanager.inl"

#endif
