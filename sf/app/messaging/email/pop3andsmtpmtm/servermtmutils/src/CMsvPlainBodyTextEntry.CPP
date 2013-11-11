// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "CMsvPlainBodyTextEntry.H"
#include <msventry.h>
#include <cmsvplainbodytext.h>

#include "IMUTDLL.H"

/**
The NewL Factory function.
*/
CMsvPlainBodyTextEntry* CMsvPlainBodyTextEntry::NewL(TInt aIs8Bit, CMsvServerEntry& aServerEntry, TUint aCharset, TUint aDefaultCharset)
	{
	CMsvPlainBodyTextEntry* self = new (ELeave) CMsvPlainBodyTextEntry();
	CleanupStack::PushL(self);
	self->ConstructL(aIs8Bit, aServerEntry, aCharset, aDefaultCharset);
	CleanupStack::Pop(self);
	return self;
	}
	
/**
Constructor.
*/
CMsvPlainBodyTextEntry::CMsvPlainBodyTextEntry()
	{	
	}

/**
Creates an instance of CMsvPlainBodytext, that is used to store the bodytext .
*/	
void CMsvPlainBodyTextEntry::ConstructL(TInt aIs8Bit, CMsvServerEntry& aServerEntry, TUint aCharset, TUint aDefaultCharset)
	{
	iPlainBodyTextCache = HBufC8::NewL(KPlainBodyTextCacheSizeBytes);
	
	iStore = aServerEntry.EditStoreL();	
	iPlainBodyText = iStore->InitialisePlainBodyTextForWriteL(aIs8Bit, aCharset, aDefaultCharset);
	}


/**
Destructor.
*/	
CMsvPlainBodyTextEntry::~CMsvPlainBodyTextEntry()
	{
	delete iPlainBodyText;	
	delete iStore;	
	delete iPlainBodyTextCache;	
	}
		
/**
Adds a chunk of body data to the intermediate buffer.If aLineOfData contains last chunk of 
data then it is directly written to store.
@param aLineOfData 	the body data, ownership  is taken.
*/	
void CMsvPlainBodyTextEntry::AddChunkL(const TDesC8& aLineOfData)
	{
	__ASSERT_DEBUG(iCommittedData != ECommittedData, gPanic(EPanicDataCommitted));
	// Leave if the data has already been committed.
	if(iCommittedData == ECommittedData)
		{
		User::Leave(KErrAlreadyExists);
		}

	TInt spaceLeft = KPlainBodyTextCacheSizeBytes - iPlainBodyTextCache->Length();
	
	// If iPlainBodyTextCache is full, then write the cache to the message store.
	if(spaceLeft < aLineOfData.Length())
		{
		iPlainBodyText->StoreChunkL(*iPlainBodyTextCache);
		iPlainBodyTextCache->Des().SetLength(0);
		}

	// Now we need to store aLineOfData
	// Recalculate how much space is available in the cache
	spaceLeft = KPlainBodyTextCacheSizeBytes - iPlainBodyTextCache->Length();
	if(spaceLeft < aLineOfData.Length())
		{
		// We still don't have enough room.  Write this line directly to the store
		iPlainBodyText->StoreChunkL(aLineOfData);
		}
	else
		{
		// Cache the line of data - it will get written later.
		iPlainBodyTextCache->Des().Append(aLineOfData);
		}
	iCommittedData = EUnCommittedData;	
	}
	
/**
Commit the plainbody text file to message store.
*/	
void CMsvPlainBodyTextEntry::TryCommitL()
	{
	switch(iCommittedData)
		{
		// Commit the data, since entire data is recieved at this point.
		case EUnCommittedData:
			{
			// Write any remaining bodytext data to the store.
			if(iPlainBodyTextCache->Size() > 0)
				{
				iPlainBodyText->StoreChunkL(*iPlainBodyTextCache);
				iPlainBodyTextCache->Des().SetLength(0);
				}
				
			// Now commit the data.
			iPlainBodyText->CommitL();
		
			iCommittedData = ECommittedData;
			// Clean up.
			delete iPlainBodyText;
			iPlainBodyText = NULL;
			
			delete iStore;
			iStore = NULL;
			break;
			}
		case ECommittedData:
		case ENoData:
		default:
			{
			// Nothing to do in this state since data has already been committed.
			break;
			}
		}

	}
