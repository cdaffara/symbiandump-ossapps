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

#ifndef __CALATTACHMENTMANAGER_H__
#define __CALATTACHMENTMANAGER_H__

#include <calcommon.h>
#include <calprogresscallback.h>

class CCalAttachment;
class CCalAttachmentIterator;
class CCalAttachmentIteratorImpl;
class CCalAttachmentManagerImpl;
class CCalSession;

/**
Class for managing file attachments in the calendar store.

This class allows the user to find out how many file attachments are in the calendar store, and to fetch them 
sorted by size or by last modified date.
Once an attachment is fetched, the attachment manager can find the entries that reference that attachment.
@publishedPartner
@released
*/
NONSHARABLE_CLASS(CCalAttachmentManager) : public CBase
	{
public:
	/** Defines the sort order for calendar file attachments.
	@publishedPartner
	@released
	*/
	enum TSortOrder 
		{
		/** Sorts by file size, largest first. */
		ESortBySizeLargestFirst,
		/** Sorts by last date modified, newest first. */
		ESortByDateModifiedNewestFirst,
		/** Sorts by last date modified, oldest first. */
		ESortByDateModifiedOldestFirst,
		};
	
	IMPORT_C static CCalAttachmentManager* NewL(CCalSession& aSession, MCalProgressCallBack& aProgressCallBack); 
	IMPORT_C ~CCalAttachmentManager(); 

	IMPORT_C CCalAttachmentIterator* FetchAttachmentsL(TSortOrder aSortType) const;
	
	IMPORT_C void EntriesReferencingFileAttachmentL(RArray<TCalLocalUid>& aCalLocalUids, const CCalAttachment& aAttachment) const;
private:
	CCalAttachmentManager();
	void ConstructL(CCalSession& aSession, MCalProgressCallBack& aProgressCallBack);

private:
	CCalAttachmentManagerImpl* iAttachmentManagerImpl;
	};

/**
Iterator class to access file attachments fetched using the CCalAttachmentManager.

When CCalAttachmentManager::FetchAttachmentsL is called, this iterator is returned. It provides a snapshot of the 
attachments in the database at that time. They can then be fetched incrementally.
@publishedPartner
@released
*/
NONSHARABLE_CLASS(CCalAttachmentIterator) : public CBase
	{
public:
	IMPORT_C ~CCalAttachmentIterator();

	IMPORT_C TBool HasMore() const;
	IMPORT_C CCalAttachment* NextL();
	IMPORT_C TInt Count() const;

	CCalAttachmentIterator(CCalAttachmentIteratorImpl& aAttachmentIteratorImpl);
private:
	CCalAttachmentIteratorImpl* iAttachmentIteratorImpl; // owned
	};

#endif // __CALATTACHMENTMANAGER_H__
