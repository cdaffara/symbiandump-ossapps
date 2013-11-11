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

#ifndef __AGSATTACHMENTINDEX_H__
#define __AGSATTACHMENTINDEX_H__

#include "agmattachment.h"
#include <calattachmentmanager.h>
#include <badesca.h>

class CAgnEntry;
class CAgnServFile;

/** 
Class for storing metadata about a file attachment, used in the attachment index.
@internalComponent
*/
class CAgnAttachmentIndexItem : public CBase
	{
public:
	static CAgnAttachmentIndexItem* NewL(const CAgnAttachmentFile& aAttachment, TCalLocalUid aLocalUid);
	static CAgnAttachmentIndexItem* NewL(RReadStream& aReadStream);
	~CAgnAttachmentIndexItem();
	

	TInt Size() const;
	const TTime& LastModifiedDateUtc() const;
	const TDesC& FileName() const;
	void SetFileName(HBufC* aFileName);//Take the ownership
	const RArray<TCalLocalUid>& Entries() const;
	TCalAttachmentUid Uid() const;

	void AddEntryL(TCalLocalUid aLocalUid);
	void DeleteEntry(TCalLocalUid aLocalUid);
	
	void InternalizeL(RReadStream& aReadStream);
	void ExternalizeL(RWriteStream& aWriteStream) const;

private:
	CAgnAttachmentIndexItem(const CAgnAttachmentFile& aAttachment);
	CAgnAttachmentIndexItem();
	void ConstructL(const CAgnAttachmentFile& aAttachment, TCalLocalUid aLocalUid);

private:
	TInt					iSize;
	TTime					iLastModifiedDateUtc;
	HBufC*					iFileName;
	RArray<TCalLocalUid>	iEntries;
	TCalAttachmentUid		iUid;
	};

/** 
The attachment index stores metadata about all file attachments in the Calendar database.
URI attachments are not stored here.
@internalComponent
*/
class CAgnAttachmentIndex : public CBase
	{
public:
	CAgnAttachmentIndex();
	~CAgnAttachmentIndex();

	// called when an entry is added, deleted or updated from the indexes
	void AddEntryL(CAgnEntry& aEntry);
	void DeleteEntryL(CAgnEntry& aEntry);

	// completely remove an attachment
	void RemoveAttachment(TCalAttachmentUid aUid);
	
	// attachment array operations
	const CAgnAttachmentIndexItem* Attachment(TCalAttachmentUid aUid) const; // doesn't return ownership
	TInt Count() const;

	void Reset();
	void CommitL(CAgnServFile& aServFile);
	void Rollback();

	// get all attachments sorted as specified	
	void GetSortedIndexL(CCalAttachmentManager::TSortOrder aSortOrder, RPointerArray<CAgnAttachmentIndexItem>& aAttachments) const;
	
	// Get the file name for given attachment uid. Return KNullDesC if the attachment with that uid is not found
	const TDesC& FileName(TCalAttachmentUid aUid);

	void InternalizeL(RReadStream& aReadStream);
	void ExternalizeL(RWriteStream& aWriteStream) const;

private:
	void RemoveEntryFromAttachmentL(TCalLocalUid aEntryUid, TCalAttachmentUid aAttachmentUid);
	TInt AttachmentIndex(TCalAttachmentUid aUid) const;
	
	static TInt SortBySizeLargestFirst(const CAgnAttachmentIndexItem& aLeft, const CAgnAttachmentIndexItem& aRight);
	static TInt SortByDateModifiedNewestFirst(const CAgnAttachmentIndexItem& aLeft, const CAgnAttachmentIndexItem& aRight);
	static TInt SortByDateModifiedOldestFirst(const CAgnAttachmentIndexItem& aLeft, const CAgnAttachmentIndexItem& aRight);
	
private:
	RPointerArray<CAgnAttachmentIndexItem>	iIndex;
	CDesCArray* 							iFileRollbackArray;
	};

#endif // __AGSATTACHMENTINDEX_H__
