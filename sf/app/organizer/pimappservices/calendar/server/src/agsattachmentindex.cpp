// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "agsattachmentindex.h"
#include "agmentry.h"
#include "agmpanic.h"
#include "agsfilemanager.h"
#include "agmdebug.h"

#include <badesca.h>

// CAgnAttachmentIndexItem //

// Create a new attachment index item for a given attachment and the entry that owns it
CAgnAttachmentIndexItem* CAgnAttachmentIndexItem::NewL(const CAgnAttachmentFile& aAttachment, TCalLocalUid aLocalUid)
	{
	CAgnAttachmentIndexItem* self = new (ELeave) CAgnAttachmentIndexItem(aAttachment);
	CleanupStack::PushL(self);
	self->ConstructL(aAttachment, aLocalUid);
	CleanupStack::Pop(self);
	return self;
	}

// Create a new attachment index from a read stream
CAgnAttachmentIndexItem* CAgnAttachmentIndexItem::NewL(RReadStream& aReadStream)
	{
	CAgnAttachmentIndexItem* self = new (ELeave) CAgnAttachmentIndexItem();
	CleanupStack::PushL(self);
	self->InternalizeL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}


// Create a new attachment index item for a given attachment
CAgnAttachmentIndexItem::CAgnAttachmentIndexItem(const CAgnAttachmentFile& aAttachment) :
	iSize(aAttachment.Size()), 
	iLastModifiedDateUtc(aAttachment.LastModifiedTimeUtc()),
	iUid(aAttachment.Uid())
	{
	}
	
CAgnAttachmentIndexItem::CAgnAttachmentIndexItem()
	{
	}

void CAgnAttachmentIndexItem::ConstructL(const CAgnAttachmentFile& aAttachment, TCalLocalUid aLocalUid)
	{
	iEntries.AppendL(aLocalUid);
	iFileName = aAttachment.FileName().AllocL();
	}

CAgnAttachmentIndexItem::~CAgnAttachmentIndexItem()
	{
	delete iFileName;
	iEntries.Reset();
	}

TInt CAgnAttachmentIndexItem::Size() const
	{
	return iSize;
	}
	
void CAgnAttachmentIndexItem::SetFileName(HBufC* aFileName)
	{
	delete iFileName;
	iFileName = aFileName;
	}

const TTime& CAgnAttachmentIndexItem::LastModifiedDateUtc() const
	{
	return iLastModifiedDateUtc;
	}
	
const TDesC& CAgnAttachmentIndexItem::FileName() const
	{
	if (iFileName)
		{
		return *iFileName;
		}
	return KNullDesC;
	}
	
void CAgnAttachmentIndexItem::InternalizeL(RReadStream& aReadStream)
	{
	iSize = aReadStream.ReadInt32L();
	
	TInt64 time64;
	aReadStream >> time64;
	TTime temptime(time64);
	iLastModifiedDateUtc = temptime;
	
	iFileName = HBufC::NewL(aReadStream, KMaxTInt);
	
	TUint32 count = aReadStream.ReadUint32L();
	for (TUint32 i = 0; i < count; ++i)
		{
		TCalLocalUid localid = static_cast<TCalLocalUid>(aReadStream.ReadUint32L());
		iEntries.Append(localid);
		}
		
	iUid = static_cast<TCalAttachmentUid>(aReadStream.ReadUint32L());
	}
	
void CAgnAttachmentIndexItem::ExternalizeL(RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteInt32L(iSize);

	TInt64 time64 = iLastModifiedDateUtc.Int64();
	aWriteStream << time64;
	
	aWriteStream << *iFileName;
	
	TUint32 count = iEntries.Count();
	aWriteStream.WriteUint32L(count);
	for (TUint32 i=0; i < count; ++i)
		{
		aWriteStream.WriteUint32L(iEntries[i]);
		}
		
	aWriteStream.WriteUint32L(iUid);
	}


// Return the array of entries that contain this attachment.
// Multiple entries owning the same attachment can only occur if an entry is copied.
const RArray<TCalLocalUid>& CAgnAttachmentIndexItem::Entries() const
	{
	return iEntries;
	}

TCalAttachmentUid CAgnAttachmentIndexItem::Uid() const
	{
	return iUid;
	}

// Called when another entry is added with the same attachment 
void CAgnAttachmentIndexItem::AddEntryL(TCalLocalUid aLocalUid)
	{
	// Don't allow duplicate local UIDs. This would only happen if an entry was associated with an attachment more than once
	// which is invalid anyway.
	if (iEntries.Find(aLocalUid) == KErrNotFound)
		{
		iEntries.AppendL(aLocalUid);
		}
	}

// Called when an entry containing this attachment has been deleted
void CAgnAttachmentIndexItem::DeleteEntry(TCalLocalUid aLocalUid)
	{
	TInt pos = iEntries.Find(aLocalUid);
	if (pos != KErrNotFound)
		{
		iEntries.Remove(pos);
		}
	}

// CAgnAttachmentIndex //
void CAgnAttachmentIndex::InternalizeL(RReadStream& aReadStream)
	{
	TUint32 count = aReadStream.ReadUint32L();
	for (TUint32 i=0; i < count; ++i)
		{
		CAgnAttachmentIndexItem* item = CAgnAttachmentIndexItem::NewL(aReadStream);
		iIndex.Append(item);
		}
	}
	
	
void CAgnAttachmentIndex::ExternalizeL(RWriteStream& aWriteStream) const
	{
	TUint32 count = iIndex.Count();
	aWriteStream.WriteUint32L(count);
	for (TUint32 i=0; i < count; ++i)
		{
		iIndex[i]->ExternalizeL(aWriteStream);
		}
		
	}

	
CAgnAttachmentIndex::CAgnAttachmentIndex()
	{
	}
	
CAgnAttachmentIndex::~CAgnAttachmentIndex()
	{
	delete iFileRollbackArray;
	Reset();
	}

void CAgnAttachmentIndex::Reset()
	{
	iIndex.ResetAndDestroy();
	}

// add an entry's attachments to the index
void CAgnAttachmentIndex::AddEntryL(CAgnEntry& aEntry)
	{
	const TInt KAttachCount = aEntry.AttachmentCount();
	for (TInt ii = 0; ii < KAttachCount; ++ii)
		{
		CAgnAttachment& attach = aEntry.Attachment(ii);
		if (attach.Uid() && attach.Type() == CCalContent::EDispositionInline)
			{
			TInt index = AttachmentIndex(attach.Uid());
			CAgnAttachmentFile* attachFile = static_cast<CAgnAttachmentFile*>(&attach);
			// Add a new attach item to the attach index if the attachment uid is not found from the index
			//Otherwise, there is existing attachment already in the index. Whether should the entry share or update the attach item depends on:
			// 1. Update the attach item if the entry has the same entry uid as the one in the index (In this case, the entry is to be updated)  
			// 2. Add an attach item if the drive name of the attachment of the entry is different from the one in the index
			// 3. Share the attachment if the drive name of the attachment of the entry is the same as the one in the index
			
			TBool addNewItem = ETrue;
			TBool shareItem = EFalse;
			
			if (index >= 0)
				{// An attacment with the same attach uid has been found from the attach index
				const TDesC& filename = iIndex[index]->FileName();
				TDriveName oldDrive = filename.Left(2);
				TDriveName newDrive = attachFile->Drive();
				TBool isSameDrive = (oldDrive.CompareF(newDrive) == 0);			
				const RArray<TCalLocalUid> entries = iIndex[index]->Entries();
				TInt entryCount = entries.Count();//Those entries have shared the same attachments.
				TInt jj=entryCount;
				for (jj=0; jj<entryCount; ++jj)
					{
					if(entries[jj] == aEntry.LocalUid())
						{
						//This entry is to be updated so update its attachment drive if it is different otherwise don't do anything since attachment is same
						addNewItem = EFalse;
						if(!isSameDrive)//drive is not the same
							{
							HBufC* newfilename = filename.AllocL();
							newfilename->Des().Replace(0,2,newDrive);
							iIndex[index]->SetFileName(newfilename);
							}
						break;	
						}
					}
					
				if (jj == entryCount)//This is newly added entry
					{//check the drive has been changed or not
					if(isSameDrive)//drive is the same
						{
						addNewItem = EFalse;//This entry will share an exiting attachment	
						shareItem = ETrue;
						}
					}
				}
				
			if(addNewItem && attachFile->FileName().Length() > 0)
				{
				CAgnAttachmentIndexItem* newItem = CAgnAttachmentIndexItem::NewL(*attachFile, aEntry.LocalUid());
				CleanupStack::PushL(newItem);
				iIndex.AppendL(newItem);
				CleanupStack::Pop(newItem);
				}
			else if (shareItem )
				{//The entry will share this attachment 
				iIndex[index]->AddEntryL(aEntry.LocalUid());	
				}
			}
		}
	
	// see if this entry replaces an existing entry and some of the attachments are no longer present
	const TInt KEntryLocalUid = aEntry.LocalUid();
	for (TInt i = iIndex.Count() - 1; i >= 0; --i)
		{
		// check every attachment
		const CAgnAttachmentIndexItem* KAttachmentItem = iIndex[i];
		const TInt entryCount = KAttachmentItem->Entries().Count();
		for (TInt j = 0; j < entryCount; ++j)
			{
			// check every entry associated with each attachment
			if (KEntryLocalUid == (KAttachmentItem->Entries())[j])
				{
				// If the entry being updated used to contain this attachment, check it still does
				TBool attachmentFound = EFalse;
				for (TInt k = 0; k < KAttachCount; ++k)
					{
					if (aEntry.Attachment(k).Uid() == KAttachmentItem->Uid())
						{
						attachmentFound = ETrue;
						break;
						}
					}
				if ( ! attachmentFound)
					{
					// this entry used to be associated with this attachment but isn't any more
					RemoveEntryFromAttachmentL(aEntry.LocalUid(), KAttachmentItem->Uid());
					}
				}
			}
		}
	}

// remove an entry's attachments from the index
void CAgnAttachmentIndex::DeleteEntryL(CAgnEntry& aEntry)
	{
	const TInt KAttachCount = aEntry.AttachmentCount();
	for (TInt j = 0; j < KAttachCount; ++j)
		{
		CAgnAttachment& attach = aEntry.Attachment(j);
		if (attach.Type() == CCalContent::EDispositionInline)
			{
			RemoveEntryFromAttachmentL(aEntry.LocalUid(), attach.Uid());
			}
		}
	}

// remove a particular entry from a particular attachment in the index
void CAgnAttachmentIndex::RemoveEntryFromAttachmentL(TCalLocalUid aEntryUid, TCalAttachmentUid aAttachmentUid)
	{
	TInt index = AttachmentIndex(aAttachmentUid);
	if (index != KErrNotFound && iIndex[index])
		{
		iIndex[index]->DeleteEntry(aEntryUid);
		_DBGLOG_ATTACH(AgmDebug::DebugLog("Remove Entry Attachment: Removing entry from attachment using Entry UID='%d', Attachment UID='%d'",aEntryUid,aAttachmentUid);)
		if (iIndex[index]->Entries().Count() == 0)
			{
			// if there are no more entries with this attachment, mark this file for deletion
			if (!iFileRollbackArray)
				{
				iFileRollbackArray = new (ELeave) CDesCArraySeg(8);
				}
			iFileRollbackArray->AppendL(iIndex[index]->FileName());
			delete iIndex[index];
			iIndex.Remove(index);
			}
		}
	}

void CAgnAttachmentIndex::RemoveAttachment(TCalAttachmentUid aUid)
	{
	TInt index = AttachmentIndex(aUid);
	if (index != KErrNotFound)
		{
		_DBGLOG_ATTACH(AgmDebug::DebugLog("Remove Attachment: Removing attachment with Attachment UID='%d'", aUid);)
		delete iIndex[index];
		iIndex.Remove(index);
		}
	}

// delete all attachment files already marked for deletion in RemoveEntryFromAttachmentL
void CAgnAttachmentIndex::CommitL(CAgnServFile& aServFile)
	{
	if (iFileRollbackArray)
		{
		for (TInt i = 0; i < iFileRollbackArray->Count(); ++i)
			{
			aServFile.DeleteFileL((*iFileRollbackArray)[i]);
			}
		
		delete iFileRollbackArray;
		iFileRollbackArray = NULL;
		}
	}

// cancel any file deletes
void CAgnAttachmentIndex::Rollback()
	{
	delete iFileRollbackArray;
	iFileRollbackArray = NULL;
	}
	
TInt CAgnAttachmentIndex::AttachmentIndex(TCalAttachmentUid aUid) const
	{
	const TInt KCount = iIndex.Count();
	for (TInt i = 0; i < KCount; ++i)
		{
		if (iIndex[i]->Uid() == aUid)
			{
			return i;
			}
		}
	return KErrNotFound;
	}
	
const CAgnAttachmentIndexItem* CAgnAttachmentIndex::Attachment(TCalAttachmentUid aUid) const
	{
	_DBGLOG_ATTACH(AgmDebug::DebugLog("Attachment: Getting attachment using Uid - %d",aUid);)
	
	TInt index = AttachmentIndex(aUid);
	if (index != KErrNotFound)
		{
		return iIndex[index];
		}
	return NULL;
	}
	
TInt CAgnAttachmentIndex::Count() const
	{
	return iIndex.Count();
	}

typedef TInt (*SortFunctionPtr)(const CAgnAttachmentIndexItem&, const CAgnAttachmentIndexItem&);

// return a new array of pointers to attachments, ordered by size of last modified date
void CAgnAttachmentIndex::GetSortedIndexL(CCalAttachmentManager::TSortOrder aSortOrder, RPointerArray<CAgnAttachmentIndexItem>& aAttachments) const
	{
	SortFunctionPtr sortFunction = NULL;
	switch (aSortOrder)
		{
		case CCalAttachmentManager::ESortBySizeLargestFirst:
			sortFunction = CAgnAttachmentIndex::SortBySizeLargestFirst;
			break;
		case CCalAttachmentManager::ESortByDateModifiedNewestFirst:
			sortFunction = CAgnAttachmentIndex::SortByDateModifiedNewestFirst;
			break;
		case CCalAttachmentManager::ESortByDateModifiedOldestFirst:
			sortFunction = CAgnAttachmentIndex::SortByDateModifiedOldestFirst;
			break;
		default:
			Panic(EAgmErrInvalidAttachmentSortOrder);
		}
	TLinearOrder<CAgnAttachmentIndexItem> order(*sortFunction);

	const TInt KCount = iIndex.Count();
	for (TInt i = 0; i < KCount; ++i)
		{
		aAttachments.InsertInOrderAllowRepeatsL(iIndex[i], order);
		}
	}

/*static*/ TInt CAgnAttachmentIndex::SortBySizeLargestFirst(const CAgnAttachmentIndexItem& aLeft, const CAgnAttachmentIndexItem& aRight)
	{
	return aRight.Size() - aLeft.Size();
	}

/*static*/ TInt CAgnAttachmentIndex::SortByDateModifiedNewestFirst(const CAgnAttachmentIndexItem& aLeft, const CAgnAttachmentIndexItem& aRight)
	{
	if (aRight.LastModifiedDateUtc() > aLeft.LastModifiedDateUtc())
		{
		return 1;
		}
	else if (aRight.LastModifiedDateUtc() == aLeft.LastModifiedDateUtc())
		{
		return 0;
		}
	return -1;
	}

/*static*/ TInt CAgnAttachmentIndex::SortByDateModifiedOldestFirst(const CAgnAttachmentIndexItem& aLeft, const CAgnAttachmentIndexItem& aRight)
	{
	// return opposite value of SortByDateModifiedNewestFirst
	return ( - SortByDateModifiedNewestFirst(aLeft, aRight));
	}

const TDesC& CAgnAttachmentIndex::FileName(TCalAttachmentUid aUid)
	{	
	// Get the file name for given attachment uid. Return KNullDesC if it the attachment with that uid is not found

	TInt index = AttachmentIndex(aUid);	
	
	if (index >= 0)
		{
		return iIndex[index]->FileName();
		}

	return KNullDesC();
	}
