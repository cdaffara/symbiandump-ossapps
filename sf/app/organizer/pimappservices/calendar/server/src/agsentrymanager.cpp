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

#include "agsentrymanager.h"
#include "agmutil.h"
#include "agmentry.h"
#include "agmdebug.h"

#include <s32stor.h>

CAgnEntryManager::CAgnEntryManager()
//
// Constructor
//
	:iNextAvailableGeneralStreamId(KNullStreamId),
	iNextAvailableTodoStreamId(KNullStreamId),
	iNextAvailableRptStreamId(KNullStreamId),
	iCurrentGeneralStreamId(KNullStreamId),
	iCurrentTodoStreamId(KNullStreamId),
	iCurrentRptStreamId(KNullStreamId),
	iGeneralBufferHasBeenStored(EFalse),
	iTodoBufferHasBeenStored(EFalse),
	iRptBufferHasBeenStored(EFalse),
	iBufferHasBeenStored(EFalse),
	iBufferedDeleting(EFalse)
	{ 
	}


CAgnEntryManager* CAgnEntryManager::NewL()
//
// Create a CAgnEntryManager object
//
	{

	CAgnEntryManager* Self=new(ELeave)CAgnEntryManager();
	CleanupStack::PushL(Self);
	Self->iGeneralBuffer=new(ELeave) CBuffer(TAgnEntryId::EMaxNumEntriesPerStream);
	Self->iTodoBuffer=new(ELeave) CBuffer(TAgnEntryId::EMaxNumEntriesPerStream);
	Self->iRptBuffer=new(ELeave) CBuffer(TAgnEntryId::EMaxNumEntriesPerStream);
	Self->iIteratorBuffer=new(ELeave) CBuffer(TAgnEntryId::EMaxNumEntriesPerStream);
	CleanupStack::Pop();
	return (Self);
	}


CAgnEntryManager::~CAgnEntryManager()
//
// Destructor 
//
	{

	if (iGeneralBuffer)
		{
		FreeBuffer(EGeneral);
		delete iGeneralBuffer;
		}
	if (iTodoBuffer)
		{
		FreeBuffer(ETodo);
		delete iTodoBuffer;
		}
	if (iRptBuffer)
		{
		FreeBuffer(ERpt);
		delete iRptBuffer;
		}
	if (iIteratorBuffer)
		{
		FreeBuffer(EIterator);
		delete iIteratorBuffer;
		}
	}

TStreamId CAgnEntryManager::DoAddEntryL(CAgnEntry& aEntry, TStreamId& aCurrentStreamId,
										TStreamId& aNextAvailableStreamId, CBuffer* aBuffer, TBufferType aBufferType)
//
// Add an entry by either adding it to an existing stream or creating a new stream if necessary
//
	{
	if (aNextAvailableStreamId==KNullStreamId)  // create a new stream
		{
		if (aBuffer->Count() > 0) // clear out any previous entries loaded in memory
			FreeBuffer(aBufferType); 

		if (aEntry.EntryId().IsNullId()) // the entry might have already been assigned an id (via NextAvailableIdL)	
			aEntry.SetEntryId(NextAvailableIdL(aBufferType)); // this will create a new stream

		aNextAvailableStreamId = aEntry.EntryId().StreamId();
   		
		CAgnEntry* entry = aEntry.CloneL();
		CleanupStack::PushL(entry);
 		aBuffer->AppendL(TBufItem(entry));
 		CleanupStack::Pop(entry);
 
		__ASSERT_DEBUG(aBuffer->Count()==1,Panic(EAgmErrOnlyOneEntryAllowedInBuffer));
		TRAPD(ret,StoreBufferL(aNextAvailableStreamId,aBuffer,aBufferType));		
		if (ret!=KErrNone)
			{
			FreeBuffer(aBufferType); 
			User::Leave(ret);
			}

		aCurrentStreamId = aNextAvailableStreamId;
		
		return (aNextAvailableStreamId);
		}

// else - the entry is to be added to an existing stream:

	TAgnEntryId entryId = NextAvailableIdL(aBufferType);	
	aEntry.SetEntryId(entryId);

	CAgnEntry* entry = aEntry.CloneL();
	CleanupStack::PushL(entry);
	aBuffer->AppendL(TBufItem(entry));
	CleanupStack::Pop(entry);

	TInt ret;
	TRAP(ret,StoreBufferL(entryId.StreamId(),aBuffer,aBufferType));
	if (ret!=KErrNone)
		{
		if (ret!=KErrNoMemory) 
			FreeBuffer(aBufferType);
		else
			{
			aBuffer->Delete(aBuffer->Count()-1); //!!! any reason why not resetanddestroy?
			delete entry;
			}
		User::Leave(ret);
		}

	if (aBuffer->Count()==TAgnEntryId::EMaxNumEntriesPerStream)
		aNextAvailableStreamId = KNullStreamId;

	return (KNullStreamId);	// indicate a new stream wasn't created during the addition 
	}


TStreamId CAgnEntryManager::AddEntryL(CAgnEntry& aEntry)
//
// Stores aEntry into a stream in iStore. If iNextAvailableStreamId is null then this indicates that the entry
// is to be inserted into a brand new stream, otherwise iNextAvailableStreamId should identify an existing stream
// which may contain other entries - if so then they are loaded into memory so that a unique id can be
// assigned to the new entry, the existing entries are then re-written back to the stream along with
// the new entry. The entries' details are kept loaded in memory. 
//
// An entry's id is a TStreamId which contains 28 bits for the stream id and 4 spare bits, it is necessary
// to look at any other entries' 4 bits so that a unique number can be assigned to the new entry.
//
	{

	__ASSERT_DEBUG(iStore,Panic(EAgmErrStreamStoreIsNull));

	if (aEntry.Type()==CCalEntry::ETodo)
		{
		return (DoAddEntryL(aEntry,iCurrentTodoStreamId,iNextAvailableTodoStreamId,iTodoBuffer,ETodo));
		}		
	else
		{
		if (aEntry.RptDef())
			{
			return (DoAddEntryL(aEntry,iCurrentRptStreamId,iNextAvailableRptStreamId,iRptBuffer,ERpt));
			}			
		}
		
	return (DoAddEntryL(aEntry,iCurrentGeneralStreamId,iNextAvailableGeneralStreamId,iGeneralBuffer,EGeneral));
	}


/**
 Update the details of the existing entry aEntry in the store

 Call SetHowRichTextIsStored() IF its loaded in memory (rich text object will panic otherwise)
 Notes:
 1) Was inline and still is -> simply rewrite to stream
 2) Was outline and still is
		a) its not loaded - can't have changed -> do nothing
		b) its loaded and has changed ->
			i ) delete the store in the child stream
			ii) store components as if it were a new entry
 3) Was inline and now is outline	 -> call StoreComponents
 4) Was outline and now is inline
		a) delete the store
		b) delete the child stream
		c) save as normal

If an entry moves from one stream to another and it was the last entry in the stream then
the id of that stream is returned otherwise KNullStreamId is returned. If when an entry moves
to a new stream the stream was not previoulsy existing then its id is returned via aNewStreamId
which is otherwise KNullStreamId;

@capability ReadUserData
*/
TStreamId CAgnEntryManager::UpdateEntryL(CAgnEntry& aEntry, TStreamId& aNewStreamId)
	{

	__ASSERT_DEBUG(iStore,Panic(EAgmErrStreamStoreIsNull));
	aNewStreamId = KNullStreamId;

	TAgnEntryId entryId = aEntry.EntryId();

	// If the entry isn't already loaded in memory then do so
	TInt pos;
	CAgnEntry* originalEntry=SearchBuffers(entryId,pos);
	if (!originalEntry)
		{	
		RestoreBufferL(entryId.StreamId());
		originalEntry=SearchBuffers(entryId,pos);
		}
		
	__ASSERT_ALWAYS(originalEntry,User::Leave(KErrNotFound));
	TBufferType type = EGeneral;
	if (originalEntry->Type() == CCalEntry::ETodo)
		type = ETodo;
	else
		{
		if (originalEntry->RptDef())
			type = ERpt;
		}		


	//delete original entry from buffer and add new one then save buffer
 	CAgnEntry* entry = NULL;
 
	switch (type)
		{
		case EGeneral:
			{
			delete (((*iGeneralBuffer)[pos]).Entry());
			iGeneralBuffer->Delete(pos);
			if (aEntry.RptDef())
				{	// move the entry to the repeat buffer (thus generating a new id)
				TAgnEntryId nullId;
				aEntry.SetEntryId(nullId);
				aNewStreamId = AddEntryL(aEntry);  
				}
			else
				{
				entry = aEntry.CloneL();
				CleanupStack::PushL(entry);
				iGeneralBuffer->AppendL(TBufItem(entry));
  				CleanupStack::Pop();
				}
			if (iGeneralBuffer->Count())
				StoreBufferL(iCurrentGeneralStreamId,iGeneralBuffer,EGeneral);
			else
				{
				iNextAvailableGeneralStreamId = KNullStreamId;
				return (iCurrentGeneralStreamId);
				}
			break;
			}
		case ETodo:
			{
			entry = aEntry.CloneL();
			delete (((*iTodoBuffer)[pos]).Entry());
			(*iTodoBuffer)[pos] = TBufItem(entry);
			StoreBufferL(iCurrentTodoStreamId,iTodoBuffer,ETodo);
			break;
			}
		case ERpt:
			{
			delete (((*iRptBuffer)[pos]).Entry());
			iRptBuffer->Delete(pos);
			if (aEntry.RptDef())
				{
  				entry = aEntry.CloneL();
  				CleanupStack::PushL(entry);
  				iRptBuffer->AppendL(TBufItem(entry));
  				CleanupStack::Pop();
				}
			else
				{ // move the entry to the general buffer (thus generating a new id)
				TAgnEntryId nullId2;
				aEntry.SetEntryId(nullId2);
				aNewStreamId = AddEntryL(aEntry);
				}
			if (iRptBuffer->Count())
				StoreBufferL(iCurrentRptStreamId,iRptBuffer,ERpt);
			else
				{
				iNextAvailableRptStreamId = KNullStreamId;
				return (iCurrentRptStreamId);
				}
			break;
			}
		}
	return (KNullStreamId);
	}


TStreamId CAgnEntryManager::DeleteEntryL(const TAgnEntryId& aEntryId)
//
// Delete the entry identified by aEntryId from the store. If the entry was the last entry in the
// stream then the stream's id is returned otherwise KNullStreamId is returned
//
	{

	__ASSERT_DEBUG(iStore,Panic(EAgmErrStreamStoreIsNull));

	// If the entry isn't already loaded in memory then do so
	TInt pos;
	CAgnEntry* pEntry=SearchBuffers(aEntryId,pos);
	if (!pEntry)
		{
		if (BufferedDeleting())
			{
			if (!iGeneralBufferHasBeenStored && iGeneralBuffer->Count())
				{
				DeleteEntriesInBufferFromStoreL(iGeneralBuffer,iNumGeneralEntriesToDelete);
				__ASSERT_DEBUG(iCurrentGeneralStreamId!=KNullStreamId,Panic(EAgmErrStreamIdIsNull));
				__ASSERT_DEBUG(iGeneralBuffer->Count(),Panic(EAgmErrExpectedEntriesInStore)); // There should have been at least 1 entry left over from the previous deletion
				StoreBufferL(iCurrentGeneralStreamId,iGeneralBuffer,EGeneral);
				SetBufferHasBeenStored(ETrue,EGeneral);
				}
			if (!iTodoBufferHasBeenStored && iTodoBuffer->Count())
				{
				DeleteEntriesInBufferFromStoreL(iTodoBuffer,iNumTodoEntriesToDelete);
				__ASSERT_DEBUG(iCurrentTodoStreamId!=KNullStreamId,Panic(EAgmErrStreamIdIsNull));
				__ASSERT_DEBUG(iTodoBuffer->Count(),Panic(EAgmErrExpectedEntriesInStore)); // There should have been at least 1 entry left over from the previous deletion
				StoreBufferL(iCurrentTodoStreamId,iTodoBuffer,ETodo);
				SetBufferHasBeenStored(ETrue,ETodo);
				}
			if (!iRptBufferHasBeenStored && iRptBuffer->Count())
				{
				DeleteEntriesInBufferFromStoreL(iRptBuffer,iNumRptEntriesToDelete);
				__ASSERT_DEBUG(iCurrentRptStreamId!=KNullStreamId,Panic(EAgmErrStreamIdIsNull));
				__ASSERT_DEBUG(iRptBuffer->Count(),Panic(EAgmErrExpectedEntriesInStore)); // There should have been at least 1 entry left over from the previous deletion
				StoreBufferL(iCurrentRptStreamId,iRptBuffer,ERpt);
				SetBufferHasBeenStored(ETrue,ERpt);
				}
			}
		RestoreBufferL(aEntryId.StreamId());
		pEntry=SearchBuffers(aEntryId,pos);
		}
	__ASSERT_ALWAYS(pEntry,User::Leave(KErrNotFound));

	if (pEntry->Type()==CCalEntry::ETodo)
		return (DoDeleteEntryL(pEntry,iTodoBuffer,iNumTodoEntriesToDelete,iNextAvailableTodoStreamId,iCurrentTodoStreamId,ETodo,pos));
	else
		{
		if (pEntry->RptDef())
			return (DoDeleteEntryL(pEntry,iRptBuffer,iNumRptEntriesToDelete,iNextAvailableRptStreamId,iCurrentRptStreamId,ERpt,pos));
		}
	return (DoDeleteEntryL(pEntry,iGeneralBuffer,iNumGeneralEntriesToDelete,iNextAvailableGeneralStreamId,iCurrentGeneralStreamId,EGeneral,pos));
	}

TStreamId CAgnEntryManager::DoDeleteEntryL(CAgnEntry* aEntry, CBuffer* aBuffer, TInt& aNumEntriesToDelete,
											TStreamId& aNextAvailableStreamId, const TStreamId& aCurrentStreamId,
											TBufferType aBufferType, TInt aPos)
//
// Delete the entry. 
// If the entry was the last entry in a stream then the stream's id is returned otherwise KNullStreamId
// is returned.
//
	{
	TStreamId streamId = aEntry->EntryId().StreamId();
	 
	if (BufferedDeleting())
		{
		((*aBuffer)[aPos]).SetToBeDeleted(ETrue);
		if (++aNumEntriesToDelete == aBuffer->Count())
			{
			__ASSERT_DEBUG(aCurrentStreamId!=KNullStreamId,Panic(EAgmErrStreamIdIsNull));
			DeleteEntriesInBufferFromStoreL(aBuffer,aNumEntriesToDelete);
			__ASSERT_DEBUG(aBuffer->Count()==0,Panic(EAgmErrExpectedEntriesInStore)); // All the entries should have been marked for deletion
			SetBufferHasBeenStored(ETrue,aBufferType); // the removal of a stream is semantically the same as rewriting it
										// with a lesser number of entries - hence the call to SetBufferHasBeenStored
			aNextAvailableStreamId = KNullStreamId;
			return (aCurrentStreamId); // indicate the stream is empty
			}
		else
			SetBufferHasBeenStored(EFalse,aBufferType);
		}
	else
		{
		__ASSERT_DEBUG(aCurrentStreamId!=KNullStreamId,Panic(EAgmErrStreamIdIsNull));
		delete aEntry;
		aBuffer->Delete(aPos);
		if (aBuffer->Count()) // only write back to the stream if it contains entries
			StoreBufferL(aCurrentStreamId,aBuffer,aBufferType);
		else
			{
			SetBufferHasBeenStored(ETrue,aBufferType);
			aNextAvailableStreamId = KNullStreamId;
			return (streamId); // indicate the stream is empty
			}
		}
	return (KNullStreamId);
	}


CAgnEntry* CAgnEntryManager::FetchEntryL(const TAgnEntryId& aEntryId)
//
// Create, restore and return the entry identified by aEntryId. If the entry cannot be found or a problem
// is encountered reading the stream then the function leaves (with KErrNotFound in the former case).
//
	{
	__ASSERT_DEBUG(iStore, Panic(EAgmErrStreamStoreIsNull));

	TInt pos;
	TBool createdEntryFromStream(EFalse);

	CAgnEntry* pEntry = SearchBuffers(aEntryId, pos);
	
	if ( ! pEntry )
		{
		if ( BufferedDeleting() ) 
		// don't want to restore into the buffer during i.e. merging as this will slow things down dramatically 
			{			
			RStoreReadStream stream;
			stream.OpenLC(*iStore, aEntryId.StreamId());

			TBufferType type = TBufferType(stream.ReadInt8L());
			__ASSERT_ALWAYS(type == EGeneral || type == ETodo || type == ERpt, User::Leave(KErrCorrupt));

			TUint8 entryCount = stream.ReadUint8L();
			__ASSERT_ALWAYS(entryCount <= TAgnEntryId::EMaxNumEntriesPerStream, User::Leave(KErrCorrupt));

			for ( TInt ii = 0; ii < entryCount; ++ii )
				{
				pEntry = CAgnEntry::NewL(stream);

				if ( pEntry->EntryId() == aEntryId )
					{
					createdEntryFromStream = ETrue;
					break;
					}

				delete pEntry;
				pEntry = NULL;
				}

			CleanupStack::PopAndDestroy();
			
			if (createdEntryFromStream)
				{
				CleanupStack::PushL(pEntry);
				}
			}
		else
			{
		   	RestoreBufferL(aEntryId.StreamId());
			pEntry = SearchBuffers(aEntryId, pos);
			}
		}

	// sanity check, if no entry found leave

	__ASSERT_ALWAYS(pEntry, User::Leave(KErrNotFound));

	// allocate a new entry, copy from the found one and return
	
	CAgnEntry* entry = pEntry->CloneL();
	if (createdEntryFromStream)
		{	
		CleanupStack::PopAndDestroy(pEntry);
		}
	
	return (entry);
	}


TInt CAgnEntryManager::RestoreBufferL(const TStreamId& aStreamId)
//
// Continuously reads entries from aStream until there are none left, and stores the
// details to an internal data structure.
//
	{

	__ASSERT_DEBUG(iStore,Panic(EAgmErrStreamStoreIsNull));
	RStoreReadStream stream;
	stream.OpenLC(*iStore,aStreamId);

	TBufferType type;
	if(iUseIteratorBuffer)
		{
		//need to read the type from the stream, but ignore it
		stream.ReadInt8L();
		type = EIterator;
		}
	else
		{
		type = TBufferType(stream.ReadInt8L());
		__ASSERT_ALWAYS(type == EGeneral || type == ETodo || type == ERpt,
						User::Leave(KErrCorrupt));
		}
	
	CBuffer* buffer = NULL;
	switch (type)
		{
		case EGeneral:	iCurrentGeneralStreamId = KNullStreamId;
						buffer = iGeneralBuffer;
						break;
		case ETodo:		iCurrentTodoStreamId = KNullStreamId; 
						buffer = iTodoBuffer;
						break;
		case ERpt:		iCurrentRptStreamId = KNullStreamId; 
						buffer = iRptBuffer;
						break;
		case EIterator:	buffer = iIteratorBuffer;
						break;
		default: 
			{
			_DBGLOG_ENTRY(AgmDebug::DebugLog("Entry Manager: CAgnEntryManager::RestoreBufferL Panic: Invalid Buffer Type - Unknown type");)
			DBG_PANIC(EAgmErrInvalidBufferType);
			}
		}

	TUint8 count = stream.ReadUint8L();
	__ASSERT_ALWAYS(count <= TAgnEntryId::EMaxNumEntriesPerStream,User::Leave(KErrCorrupt));

	FreeBuffer(type);

	for (TInt ii=0; ii < count; ++ii)
		{
		CAgnEntry* entry = NULL;
		TRAPD(ret, entry = CAgnEntry::NewL(stream));
		if (ret!=KErrNone)
			{
			if (ret == KErrCorrupt || ret == KErrEof) // skip any corrupted entries 
				{ 
				_DBGLOG_ENTRY(AgmDebug::DebugLog("Entry Manager: RestoreBufferL Panic - Failed to restore buffer");)
				DBG_PANIC(EAgmErrFailedToRestoreBuffer);
				continue; 
				}
			else  //i.e. run out of memory or file read error
				{
				FreeBuffer(type);
				User::Leave(ret);			
				}
			}
	
		TRAP(ret,buffer->AppendL(TBufItem(entry)));
		if (ret!=KErrNone)
			{
			FreeBuffer(type);
			delete entry;
			User::Leave(ret);
			}
		}
		
	__ASSERT_DEBUG(count == buffer->Count(), Panic(EAgmErrFailedToRestoreBuffer));
	switch (type)
		{
		case EGeneral:	iCurrentGeneralStreamId = aStreamId; 
						iNumGeneralEntriesToDelete = 0;
						break;
		case ETodo:		iCurrentTodoStreamId = aStreamId; 
						iNumTodoEntriesToDelete = 0;
						break;
		case ERpt:		iCurrentRptStreamId = aStreamId; 
						iNumRptEntriesToDelete = 0;
						break;
		case EIterator:	break;
		default: 
			{
			_DBGLOG_ENTRY(AgmDebug::DebugLog("Entry Manager: RestoreBufferL Panic - Invalid Buffer Type - Type unknown");)
			DBG_PANIC(EAgmErrInvalidBufferType);
			}
		}
	CleanupStack::PopAndDestroy();
	iLastRestored = type;
	return (buffer->Count());
	}

CAgnEntry* CAgnEntryManager::SearchBuffers(const TAgnEntryId& aEntryId, TInt& aPos)
//
// Search the entries currently loaded in memory looking for one with an id equal to aEntryId 
// and return a pointer to it if found also set aPos to its position within the array, 
// return NULL if the entry can't be found
//	
	{

	if (iGeneralBuffer->Count() !=0 && ((*iGeneralBuffer)[0]).Entry()->EntryId().StreamId() == aEntryId.StreamId())
		{
		for (TInt ii=iGeneralBuffer->Count(); ii > 0; --ii)
			{
			if (((*iGeneralBuffer)[ii-1]).Entry()->EntryId()==aEntryId)
				{
				aPos=ii-1;
				return (((*iGeneralBuffer)[ii-1]).Entry());			
				}
			}
		}

	if (iTodoBuffer->Count() !=0 && ((*iTodoBuffer)[0]).Entry()->EntryId().StreamId() == aEntryId.StreamId())
		{
		for (TInt ii=iTodoBuffer->Count(); ii > 0; --ii)
			{
			if (((*iTodoBuffer)[ii-1]).Entry()->EntryId()==aEntryId)
				{
				aPos=ii-1;
				return (((*iTodoBuffer)[ii-1]).Entry());			
				}
			}
		}

	if (iRptBuffer->Count() !=0 && ((*iRptBuffer)[0]).Entry()->EntryId().StreamId() == aEntryId.StreamId())
		{
		for (TInt ii=iRptBuffer->Count(); ii > 0; --ii)
			{
			if (((*iRptBuffer)[ii-1]).Entry()->EntryId()==aEntryId)
				{
				aPos=ii-1;
				return (((*iRptBuffer)[ii-1]).Entry());			
				}
			}
		}

	aPos=-1;
	return (NULL);
	}

const CAgnEntry* CAgnEntryManager::operator[](TInt aIndex) const
//
// Return a pointer to the aIndex'th entry currently loaded in memory
//
	{ 
	if(iUseIteratorBuffer)
		{
		__ASSERT_DEBUG(aIndex >= 0 && aIndex < iIteratorBuffer->Count(),Panic(EAgmErrInvalidEntryManagerIndex));
		return(((*iIteratorBuffer)[aIndex]).Entry());
		}
	else
		{
		switch (iLastRestored)
			{
			case EGeneral:	__ASSERT_DEBUG(aIndex >= 0 && aIndex < iGeneralBuffer->Count(),Panic(EAgmErrInvalidEntryManagerIndex));
							return(((*iGeneralBuffer)[aIndex]).Entry()); 
			case ETodo:		__ASSERT_DEBUG(aIndex >= 0 && aIndex < iTodoBuffer->Count(),Panic(EAgmErrInvalidEntryManagerIndex));
							return(((*iTodoBuffer)[aIndex]).Entry()); 
			case ERpt:		__ASSERT_DEBUG(aIndex >= 0 && aIndex < iRptBuffer->Count(),Panic(EAgmErrInvalidEntryManagerIndex));
							return(((*iRptBuffer)[aIndex]).Entry());
		default: 
				{
				_DBGLOG_ENTRY(AgmDebug::DebugLog("Entry Manager: CAgnEntryManager::operator[] Panic - Invalid Buffer Type");)
				DBG_PANIC(EAgmErrInvalidBufferType);
				}
			}
		}
	return (NULL);
	}



void CAgnEntryManager::StoreBufferL(const TStreamId& aStreamId,
									const CBuffer* aBuffer, TBufferType aType)
//
// Write the entries in the buffer to the stream identified by aStreamId
//
	{

	__ASSERT_DEBUG(aStreamId!=KNullStreamId,Panic(EAgmErrStreamIdIsNull));
	__ASSERT_DEBUG(iStore,Panic(EAgmErrStreamStoreIsNull));
	__ASSERT_DEBUG(aBuffer->Count() <= TAgnEntryId::EMaxNumEntriesPerStream,Panic(EAgmErrEntryBufferExceedsMaxStreamSize));

	if (aBuffer->Count()==0)
		return;

	RStoreWriteStream out;
	out.ReplaceLC(*iStore,aStreamId);

	TInt count = aBuffer->Count();
	__ASSERT_ALWAYS(count <= TAgnEntryId::EMaxNumEntriesPerStream,Panic(EAgnErrEntryManagerInternalError));
	out.WriteInt8L(aType);
	out.WriteUint8L(count);
	// Externalize the BC version of the entry (this is the bit only old Agenda's will see)
	TInt jj;
	for (jj = 0; jj < count; ++jj)
		{
		CAgnEntry* entry = ((*aBuffer)[jj]).Entry();
		__ASSERT_ALWAYS(entry->Type()==CCalEntry::EAppt || entry->Type()==CCalEntry::EEvent ||
					entry->Type()==CCalEntry::EAnniv || entry->Type()==CCalEntry::ETodo || 
					entry->Type()==CCalEntry::EReminder ||
					entry->Type() == CCalEntry::ENote, Panic(EAgmErrBadTypeEntry));

		out.WriteUint8L(entry->Type());
		entry->ExternalizeL(out);
		}

	out.CommitL();
	CleanupStack::PopAndDestroy();	
	SetBufferHasBeenStored(ETrue,aType);
	}


void CAgnEntryManager::SetBufferHasBeenStored(TBool aSetting,TBufferType aType)
	{
	switch (aType)
		{
		case EGeneral:	iGeneralBufferHasBeenStored = aSetting; break;
		case ETodo:		iTodoBufferHasBeenStored = aSetting; break;
		case ERpt:		iRptBufferHasBeenStored = aSetting; break;
		default: 
			{
			_DBGLOG_ENTRY(AgmDebug::DebugLog("Entry Manager: CAgnEntryManager::SetBufferHasBeenStored Panic - Invalid Buffer Type");)
			DBG_PANIC(EAgmErrInvalidBufferType);
			}
		}
	iBufferHasBeenStored = aSetting;
	}

void CAgnEntryManager::StoreBuffersL()
//
// Store all unstored buffers but don't free them afterwards
//
	{
	
	StoreAndMaybeFreeBuffersL(EFalse);
	}


void CAgnEntryManager::FlushBuffersL()
//
// During a prolonged activity such as merging,tidying then entries are not updated in the store
// until the buffer is full, if one of these activities finishes before the buffer has had a change
// to automaticaly update its contents in the store then this function must be called to make it do so.
//
	{

	StoreAndMaybeFreeBuffersL(ETrue);
	}


void CAgnEntryManager::StoreAndMaybeFreeBuffersL(TBool aFreeBuffers)
	{

	__ASSERT_DEBUG(iStore,Panic(EAgmErrStreamStoreIsNull));
	if (!iGeneralBufferHasBeenStored) 
		{
		if (BufferedDeleting())
			DeleteEntriesInBufferFromStoreL(iGeneralBuffer,iNumGeneralEntriesToDelete);
		if (iCurrentGeneralStreamId != KNullStreamId)
			StoreBufferL(iCurrentGeneralStreamId,iGeneralBuffer,EGeneral);
		SetBufferHasBeenStored(ETrue,EGeneral);
		}
	if (aFreeBuffers)
		FreeBuffer(EGeneral);

	if (!iTodoBufferHasBeenStored) 
		{
		if (BufferedDeleting())
			DeleteEntriesInBufferFromStoreL(iTodoBuffer,iNumTodoEntriesToDelete);
		if (iCurrentTodoStreamId != KNullStreamId)
			StoreBufferL(iCurrentTodoStreamId,iTodoBuffer,ETodo);
		SetBufferHasBeenStored(ETrue,ETodo);
		}
	if (aFreeBuffers)
		FreeBuffer(ETodo);

	if (!iRptBufferHasBeenStored) 
		{
		if (BufferedDeleting())
			DeleteEntriesInBufferFromStoreL(iRptBuffer,iNumRptEntriesToDelete);
		if (iCurrentRptStreamId != KNullStreamId)
			StoreBufferL(iCurrentRptStreamId,iRptBuffer,ERpt);
		SetBufferHasBeenStored(ETrue,ERpt);
		}
	if (aFreeBuffers)
		FreeBuffer(ERpt);
	}


TAgnEntryId CAgnEntryManager::NextAvailableIdL(TBufferType aBufferType)
//
// Find the next available id for an entry. If iNextAvailableStreamId is null then it indicates that
// the next entry to be added will go into a new stream in which case the stream must be created
// to determine its stream id. If iNextAvailableStreamId is not null then the entries which currently exist
// in the stream identifed by aStreamId must be loaded into memory to find the next partial id.
//
// An entry's id consists of 28 bits corresponsing to the stream id of the stream in which it
// is stored and 4 bits giving it a unique number within that stream.
//
	{

	__ASSERT_DEBUG(iStore,Panic(EAgmErrStreamStoreIsNull));

	CBuffer* buffer = NULL;
	TStreamId nextAvailableStreamId=KNullStreamId;
	switch (aBufferType)
		{
		case EGeneral:	buffer = iGeneralBuffer; 
						nextAvailableStreamId = iNextAvailableGeneralStreamId;
						break; 
		case ETodo:		buffer = iTodoBuffer; 
						nextAvailableStreamId = iNextAvailableTodoStreamId;
						break; 
		case ERpt:		buffer = iRptBuffer;
						nextAvailableStreamId = iNextAvailableRptStreamId;	 
						break; 
		default: 
			{
			_DBGLOG_ENTRY(AgmDebug::DebugLog("Entry Manager: CAgnEntryManager::NextAvailableIdL Panic - Invalid Buffer Type");)
			DBG_PANIC(EAgmErrInvalidBufferType);
			}
		}

	if (nextAvailableStreamId==KNullStreamId)
		{
		TStreamId newStreamId = iStore->ExtendL();
		return (TAgnEntryId(newStreamId,0)); // entries partial ids start numbering from 0
		}
	else
		{
		// If the entries in the stream to which the new entry will be added are not already
		// loaded in memory then do so
		if (buffer->Count()==0 || (buffer->Count() > 0 && (((*buffer)[0]).Entry()->EntryId().StreamId() != nextAvailableStreamId)))
			RestoreBufferL(nextAvailableStreamId);

		TUint uniqueIdPortion = FindUniqueIdPortion(buffer);

#if defined(_DEBUG)
TInt pos;
if (SearchBuffers(TAgnEntryId(nextAvailableStreamId,uniqueIdPortion),pos) != NULL)
	{
	Panic(EAgmErrEntryIdNotUnique);
	}
#endif
		
		return (TAgnEntryId(nextAvailableStreamId,uniqueIdPortion));
		}
	}


TUint CAgnEntryManager::FindUniqueIdPortion(const CBuffer* aBuffer)
//
// Finds and returns an entry id portion that is not being used by any other entry.
// Where an id portion is a number in the range 0 to 15.
//
	{
	__ASSERT_DEBUG(aBuffer->Count() <=16, Panic(EAgmErrEntryBufferExceedsMaxStreamSize));
	static const TUint mapToBits[16] = 
		{
		EBit1,EBit2,EBit3,EBit4,EBit5,EBit6,EBit7,EBit8,EBit9,
		EBit10,EBit11,EBit12,EBit13,EBit14,EBit15,EBit16
		};

	TUint usedPortions=0;
	for (TInt ii=aBuffer->Count(); ii > 0; --ii)
		{
		TUint id = ((*aBuffer)[ii-1]).Entry()->EntryId().PartialId();
		__ASSERT_DEBUG(id <= 15,Panic(EAgmErrInvalidPartialId));
		usedPortions |= mapToBits[id];
		}

	__ASSERT_DEBUG(usedPortions <= 0xFFFF,Panic(EAgmErrNoMorePartialIdsAvailable));

	TUint id=0;
	TUint num=1;
	while (usedPortions & num)
		{
		num <<= 1;
		++id;
		}

	__ASSERT_DEBUG(id<=15,Panic(EAgmErrPartialIdOverFlow));
	return (id);
	}


void CAgnEntryManager::DeleteEntriesInBufferFromStoreL(CBuffer* aBuffer,TInt& aNumEntriesToDelete)
//
// For each entry in the buffer marked as being deleted, delete any child streams within the
// store owned by the entry.
//
	{

	__ASSERT_DEBUG(iStore,Panic(EAgmErrStreamStoreIsNull));
	TBool found = EFalse;
	FOREVER
		{
		found = EFalse;
		for (TInt ii=0,n=aBuffer->Count();ii<n;++ii)
			{
			if (((*aBuffer)[ii]).ToBeDeleted())
				{
				delete ((*aBuffer)[ii]).Entry();
				aBuffer->Delete(ii);		
				found = ETrue;
				break;
				}
			}
		if (!found)
			break;
		}		
	aNumEntriesToDelete = 0;
	}


void CAgnEntryManager::SetBufferedDeleting(TBool aSetting)
//
// When this is set to true, whenever an entry is deleted it is deleted from the buffer but
// but not from the store until	the buffer is empty (in which case the entire stream in which
// the entry/s reside is deleted) or another action is performed such as fetching an entry.
//
	{ 
	iBufferedDeleting = aSetting; 
	SetBufferHasBeenStored(ETrue,EGeneral);
	SetBufferHasBeenStored(ETrue,ETodo);
	SetBufferHasBeenStored(ETrue,ERpt);
	}


void CAgnEntryManager::FreeBuffers()
	{
	FreeBuffer(EGeneral);
	FreeBuffer(ETodo);
	FreeBuffer(ERpt);
	}


void CAgnEntryManager::FreeBuffer(TBufferType aBufferType)
	{ 

	CBuffer* buffer=NULL;
	switch (aBufferType)
		{
		case EGeneral:	buffer = iGeneralBuffer; break;
		case ETodo:		buffer = iTodoBuffer; break;	
		case ERpt:		buffer = iRptBuffer; break;
		case EIterator:	buffer = iIteratorBuffer; break;
		default: 
			{
			_DBGLOG_ENTRY(AgmDebug::DebugLog("Entry Manager: CAgnEntryManager::FreeBuffer Panic - Invalid Buffer Type");)
			DBG_PANIC(EAgmErrInvalidBufferType);
			}
		}

	TInt n=buffer->Count();
	for (TInt ii=0;ii<n;++ii)
		delete ((*buffer)[ii]).Entry();
	buffer->Delete(0,n); // no point in compressing 
	}


void CAgnEntryManager::Reset()
	{
	FreeBuffers(); 
	iNumGeneralEntriesToDelete = 0;
	iNumTodoEntriesToDelete = 0;
	iNumRptEntriesToDelete = 0;
	iBufferedDeleting = EFalse;
	iGeneralBufferHasBeenStored = EFalse;
	iTodoBufferHasBeenStored = EFalse;
	iRptBufferHasBeenStored = EFalse;
	iNextAvailableGeneralStreamId = KNullStreamId;
	iNextAvailableTodoStreamId = KNullStreamId;
	iNextAvailableRptStreamId = KNullStreamId;
	iCurrentGeneralStreamId = KNullStreamId;
	iCurrentTodoStreamId = KNullStreamId;
	iCurrentRptStreamId = KNullStreamId;
	}


void CAgnEntryManager::InternalizeL(RReadStream& aStream)
	{

	aStream >> iNextAvailableGeneralStreamId;
	aStream >> iNextAvailableTodoStreamId;
	aStream >> iNextAvailableRptStreamId;
	}


void CAgnEntryManager::ExternalizeL(RWriteStream& aStream) const
	{

	aStream << iNextAvailableGeneralStreamId;
	aStream << iNextAvailableTodoStreamId;
	aStream << iNextAvailableRptStreamId;
	}

void CAgnEntryManager::CopyStreamIds(const CAgnEntryManager& aOriginalManager)
	{
	iNextAvailableGeneralStreamId = aOriginalManager.iNextAvailableGeneralStreamId;
	iNextAvailableTodoStreamId = aOriginalManager.iNextAvailableTodoStreamId;
	iNextAvailableRptStreamId = aOriginalManager.iNextAvailableRptStreamId;
	}


												 
