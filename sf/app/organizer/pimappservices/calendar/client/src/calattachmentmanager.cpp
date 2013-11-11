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

#include "calattachmentmanagerimpl.h"
	
/** Create a new attachment manager.
@param aSession An open calendar session to be used.
@param aProgressCallBack The class to be called back when the attachment manager is created. 
Note that if an entry view or instance view has been created already, then this callback will happen immediately.
@pre A calendar file has been opened on a calendar session.
@post Once the callback is complete, the attachment manager will be constructed and ready for use.
@return A pointer to a new attachment manager.
@publishedPartner
@released
@capability None
*/
EXPORT_C CCalAttachmentManager* CCalAttachmentManager::NewL(CCalSession& aSession, MCalProgressCallBack& aProgressCallBack)
	{
	CCalAttachmentManager* self = new (ELeave) CCalAttachmentManager();
	CleanupStack::PushL(self);
	self->ConstructL(aSession, aProgressCallBack);
	CleanupStack::Pop(self);
	return self;
	}

CCalAttachmentManager::CCalAttachmentManager()
	{
	}

/** Destructor for the attachment manager.
@publishedPartner
@released
@pre An attachment manager has been constructed
@post The attachment manager is destroyed
@capability None
*/
EXPORT_C CCalAttachmentManager::~CCalAttachmentManager()
	{
	delete iAttachmentManagerImpl;
	}

void CCalAttachmentManager::ConstructL(CCalSession& aSession, MCalProgressCallBack& aProgressCallBack)
	{
	iAttachmentManagerImpl = CCalAttachmentManagerImpl::NewL(aSession, aProgressCallBack);
	}

/** Fetches the details of all unique file attachments referenced from entries within the calendar store, sorted as specified.
This does not include URI attachments.
Note that the full attachment data is not loaded in this operation, only the metadata for each of the file attachments.
If an attachment is referenced by more than one entry, it will appear only once in the iterator.
@param aSortOrder The order in which to sort the attachments.
@return A pointer to an iterator of attachments. The caller takes ownership.
@pre None
@post A new attachment iterator is returned containing the sorted file attachments.
@publishedPartner
@released
@capability ReadUserData
*/
EXPORT_C CCalAttachmentIterator* CCalAttachmentManager::FetchAttachmentsL(TSortOrder aSortOrder) const
	{
	CCalAttachmentIteratorImpl* impl = iAttachmentManagerImpl->FetchAttachmentsL(aSortOrder);
	CleanupStack::PushL(impl);
	CCalAttachmentIterator* iterator = new (ELeave) CCalAttachmentIterator(*impl);
	CleanupStack::Pop(impl);
	return iterator;
	}

/** Fetches the local UIDs of all entries in the database that reference this file attachment.
This does nothing if a URI attachment is passed in, or if the file attachment has not been stored in the calendar database yet.
@param aUids On return, an array of local UIDs of all the entries that contain this file attachment. 
@param aAttachment The file attachment object whose referencing entries will be returned.
@leave KErrArgument The attachment is not a file attachment - i.e. of type CCalAttachment::EFile.
@pre None
@post None
@publishedPartner
@released
@capability ReadUserData
*/
EXPORT_C void CCalAttachmentManager::EntriesReferencingFileAttachmentL(RArray<TCalLocalUid>& aUids, const CCalAttachment& aAttachment) const
	{
	__ASSERT_ALWAYS(aAttachment.Type() == CCalAttachment::EFile, User::Leave(KErrArgument));
	iAttachmentManagerImpl->EntriesReferencingFileAttachmentL(aUids, aAttachment.Impl());
	}

//
// CCalAttachmentIterator //
//

// This takes ownership of aIterator
CCalAttachmentIterator::CCalAttachmentIterator(CCalAttachmentIteratorImpl& aIterator) :
	iAttachmentIteratorImpl(&aIterator)
	{
	}

/** Destructor.
Clears resources of the attachment iterator.
@pre An attachment iterator has been constructed from a CCalAttachmentManager::FetchAttachmentsL call.
@post The iterator is destroyed.
@publishedPartner
@released
@capability None
*/
EXPORT_C CCalAttachmentIterator::~CCalAttachmentIterator()
	{
	delete iAttachmentIteratorImpl;
	}

/** Checks to see if there are more attachments in the iterator.
If there are, the next one can be fetched by calling NextL.
@return ETrue if there are more attachments, EFalse if not.
@pre None
@post None
@publishedPartner
@released
@capability None
*/
EXPORT_C TBool CCalAttachmentIterator::HasMore() const
	{
	return iAttachmentIteratorImpl->HasMore();
	}

/** Returns the next attachment in the iterator, or NULL if there are no more attachments.
@return A pointer to a new attachment object. Ownership is returned. NULL if the iterator has finished.
@leave KErrNotFound if an attachment has been deleted since the original search that returned the iterator of attachments.
Otherwise one of the system-wide error codes.
@pre None
@post The iterator moves to the next attachment in its results list.
@publishedPartner
@released
@capability None
*/
EXPORT_C CCalAttachment* CCalAttachmentIterator::NextL()
	{
	return iAttachmentIteratorImpl->NextL();
	}

/** Returns the total number of attachments in the iterator.
@return The number of attachments.
@pre None
@post None
@publishedPartner
@released
@capability None
*/
EXPORT_C TInt CCalAttachmentIterator::Count() const
	{
	return iAttachmentIteratorImpl->Count();
	}
