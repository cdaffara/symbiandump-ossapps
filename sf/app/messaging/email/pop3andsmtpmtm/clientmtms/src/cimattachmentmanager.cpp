// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cimattachmentmanager.h"

CImAttachmentManager::CImAttachmentManager(CImEmailMessage& aEmailMessage,CMsvEntry& aEntry) :CActive(EPriorityStandard),iClientEntry(aEntry), iEmailMessage(aEmailMessage) 
	{
	CActiveScheduler::Add(this);
	}

void CImAttachmentManager::ConstructL()
	{
	}
	
CImAttachmentManager::~CImAttachmentManager()
	{
	delete iStore;
	iAttachmentInfoList.ResetAndDestroy();
	delete iMsvOperation;
	delete iEntry;
	}
	
CImAttachmentManager* CImAttachmentManager::NewL(CImEmailMessage& aEmailMessage,CMsvEntry& aEntry)
	{
	CImAttachmentManager* self = new(ELeave) CImAttachmentManager(aEmailMessage,aEntry);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CImAttachmentManager::RunL()
	{
	switch(iAttachmentManagerState)
		{
	case EModifyAttachmentInfo:
		{
		if(!iStore == NULL)
			{
			ModifyAttachmentInfoL();
			}
		ChangeEntryDetailsL();
		iAttachmentManagerState = EChangeEntryDetails;
		} 
		break;
	case EChangeEntryDetails:
		{
		delete iEntry;
		iEntry = NULL;

		User::RequestComplete(iReportStatus, iStatus.Int());
		}
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		};
	}
	
void CImAttachmentManager::DoCancel()
	{
	if (iMsvOperation)
		iMsvOperation->Cancel();

	delete iStore;
	iStore = NULL;

	delete iEntry;
	iEntry = NULL;
	}

TInt CImAttachmentManager::RunError(TInt aError)
	{
	if(iStore != NULL)
		{
		delete iStore;
		iStore = NULL;	
		}
	
	delete iEntry;
	iEntry = NULL;

	User::RequestComplete(iReportStatus, aError);
	return KErrNone;	
	}
	
void CImAttachmentManager::AddAttachmentL(const TDesC& aFilePath, CMsvAttachment* aAttachmentInfo, TRequestStatus& aStatus)
	{
	iEmailMessage.AddAttachmentL(aFilePath,aAttachmentInfo,aStatus);
	}

void CImAttachmentManager::AddAttachmentL(RFile& aFileHandle, CMsvAttachment* aAttachmentInfo, TRequestStatus& aStatus)
	{
	iEmailMessage.AddAttachmentL(aFileHandle,aAttachmentInfo,aStatus);
	}

void CImAttachmentManager::AddLinkedAttachmentL(const TDesC& aFilePath, CMsvAttachment* aAttachmentInfo, TRequestStatus& aStatus)
	{
	iEmailMessage.AddLinkedAttachmentL(aFilePath,aAttachmentInfo,aStatus);
	}

void CImAttachmentManager::AddEntryAsAttachmentL(TMsvId aEntryId, CMsvAttachment* aAttachmentInfo, TRequestStatus& aStatus)
	{
	iEmailMessage.AddEntryAsAttachmentL(aEntryId,aAttachmentInfo,aStatus);
	}

void CImAttachmentManager::CreateAttachmentL(const TDesC& aFileName, RFile& aAttachmentFile, CMsvAttachment* aAttachmentInfo, TRequestStatus& aStatus)
	{
	iEmailMessage.CreateAttachmentL(aFileName,aAttachmentFile,aAttachmentInfo,aStatus);
	}

/**
This API not supported.
Leaves with KErrNotSupported.
*/
void CImAttachmentManager::RenameAttachmentL(TInt /*aIndex*/, const TDesC& /*aNewName*/, TRequestStatus& /*aStatus*/)
	{
	User::Leave(KErrNotSupported);
	}

TInt CImAttachmentManager::AttachmentCount() const
	{
	return iAttachmentInfoList.Count();
	}

CMsvAttachment* CImAttachmentManager::GetAttachmentInfoL(TInt aIndex)
	{
	__ASSERT_ALWAYS(iAttachmentInfoList.Count() || (aIndex >= 0 && aIndex < iAttachmentInfoList.Count()),User::Invariant());
	
	CMsvAttachment* attachmentInfo = iAttachmentInfoList[aIndex];
	return CMsvAttachment::NewL(*attachmentInfo);
	}

CMsvAttachment* CImAttachmentManager::GetAttachmentInfoL(TMsvAttachmentId aId)
	{
	TInt index = AttachmentIndexInMessage(aId);
	User::LeaveIfError(index);
	
	CMsvAttachment* attachmentInfo = iAttachmentInfoList[index];
	return CMsvAttachment::NewL(*attachmentInfo);	
	}

RFile CImAttachmentManager::GetAttachmentFileL(TInt aIndex)
	{
	__ASSERT_ALWAYS(iAttachmentInfoList.Count() || (aIndex >= 0 && aIndex < iAttachmentInfoList.Count()),User::Invariant());
	// Need to reset the client entry to the owning attachment entry's TMsvId
	TInt entryId;
	// Stored in the integer attributes for the CMsvAttachment class
	if(iAttachmentInfoList[aIndex]->GetIntAttribute(KUidMsvEmailAttachmentEntryId,entryId) != KErrNone)
		{
	// We're moving from the per-message iAttachmentInfoList to the per attachment entry list
		entryId = iAttachmentInfoList[aIndex]->Id();
		}
	
	CMsvEntry* entry=CMsvEntry::NewL(iClientEntry.Session(), entryId, TMsvSelectionOrdering());
	CleanupStack::PushL(entry);

	CMsvStore* store = entry->ReadStoreL();
	CleanupStack::PushL(store);
	MMsvAttachmentManager& attachmentMgr = store->AttachmentManagerL();
	// We're on the attachment entry but we need the index into that array
	// This is stored in another attribute 
	RFile file;

	TInt entryIndex;
	if(iAttachmentInfoList[aIndex]->GetIntAttribute(KUidMsvEmailAttachmentEntryIndex,entryIndex) == KErrNone)
		{
	// Home in on the original CMsvAttachment
		file = attachmentMgr.GetAttachmentFileL(entryIndex);
		}
	else
		{
		file = attachmentMgr.GetAttachmentFileL(0);
		}
	CleanupStack::PopAndDestroy(2,entry);//store, entry

	return file;
	}

RFile CImAttachmentManager::GetAttachmentFileL(TMsvAttachmentId aId)
	{
	// Lookup in the per-message array and extract the index
	TInt index = AttachmentIndexInMessage(aId);
	User::LeaveIfError(index);
	
	// Need to reset the client entry to the owning attachment entry's TMsvId
	TInt entryId;
	// Stored in the integer attributes for the CMsvAttachment class
	if(iAttachmentInfoList[index]->GetIntAttribute(KUidMsvEmailAttachmentEntryId,entryId) != KErrNone)
	// We're moving from the per-message iAttachmentInfoList to the per attachment entry list
		{
		entryId = iAttachmentInfoList[index]->Id();
		}

	CMsvEntry* entry=CMsvEntry::NewL(iClientEntry.Session(), entryId, TMsvSelectionOrdering());
	CleanupStack::PushL(entry);
		
	CMsvStore* store = entry->ReadStoreL();
	CleanupStack::PushL(store);
	MMsvAttachmentManager& attachmentMgr = store->AttachmentManagerL();
	// We're on the attachment entry but we need the index into that array
	// This is stored in another attribute 
	RFile file;

	TInt entryIndex;
	if(iAttachmentInfoList[index]->GetIntAttribute(KUidMsvEmailAttachmentEntryIndex,entryIndex) == KErrNone)
		{
	// Home in on the original CMsvAttachment
		file = attachmentMgr.GetAttachmentFileL(entryIndex);
		}
	else
		{
		file = attachmentMgr.GetAttachmentFileL(0);
		}
	CleanupStack::PopAndDestroy(2,entry);//store, entry

	return file;
	}
	
RFile CImAttachmentManager::GetAttachmentFileForWriteL(TInt aIndex)
	{
	__ASSERT_ALWAYS(iAttachmentInfoList.Count() || (aIndex >= 0 && aIndex < iAttachmentInfoList.Count()),User::Invariant());
	// Need to reset the client entry to the owning attachment entry's TMsvId
	TInt entryId;
	// Stored in the integer attributes for the CMsvAttachment class
	if(iAttachmentInfoList[aIndex]->GetIntAttribute(KUidMsvEmailAttachmentEntryId,entryId) != KErrNone)
		{
	// We're moving from the per-message iAttachmentInfoList to the per attachment entry list
		entryId = iAttachmentInfoList[aIndex]->Id();
		}

	CMsvEntry* entry=CMsvEntry::NewL(iClientEntry.Session(), entryId, TMsvSelectionOrdering());
	CleanupStack::PushL(entry);
	
	CMsvStore* store = entry->EditStoreL();
	CleanupStack::PushL(store);
	MMsvAttachmentManager& attachmentMgr = store->AttachmentManagerL();
	// We're on the attachment entry but we need the index into that array
	// This is stored in another attribute 
	RFile file;

	TInt entryIndex;
	if(iAttachmentInfoList[aIndex]->GetIntAttribute(KUidMsvEmailAttachmentEntryIndex,entryIndex) == KErrNone)
		{
	// Home in on the original CMsvAttachment
		file = attachmentMgr.GetAttachmentFileForWriteL(entryIndex);
		}
	else
		{
		file = attachmentMgr.GetAttachmentFileForWriteL(0);
		}
	CleanupStack::PopAndDestroy(2, entry);//store, entry
	
	return file;
	}

RFile CImAttachmentManager::GetAttachmentFileForWriteL(TMsvAttachmentId aId)
	{
	// Lookup in the per-message array and extract the index
	TInt index = AttachmentIndexInMessage(aId);
	User::LeaveIfError(index);


	// Need to reset the client entry to the owning attachment entry's TMsvId
	TInt entryId;
	// Stored in the integer attributes for the CMsvAttachment class
	if(iAttachmentInfoList[index]->GetIntAttribute(KUidMsvEmailAttachmentEntryId,entryId) != KErrNone)
		{
		// We're moving from the per-message iAttachmentInfoList to the per attachment entry list
		entryId = iAttachmentInfoList[index]->Id();
		}

	CMsvEntry* entry=CMsvEntry::NewL(iClientEntry.Session(), entryId, TMsvSelectionOrdering());
	CleanupStack::PushL(entry);
		
	CMsvStore* store = entry->EditStoreL();
	CleanupStack::PushL(store);
	MMsvAttachmentManager& attachmentMgr = store->AttachmentManagerL();
	// We're on the attachment entry but we need the index into that array
	// This is stored in another attribute 
	RFile file;

	TInt entryIndex;
	if(iAttachmentInfoList[index]->GetIntAttribute(KUidMsvEmailAttachmentEntryIndex,entryIndex) == KErrNone)
		{
	// Home in on the original CMsvAttachment
		file = attachmentMgr.GetAttachmentFileForWriteL(entryIndex);
		}
	else
		{
		file = attachmentMgr.GetAttachmentFileForWriteL(0);
		}
	CleanupStack::PopAndDestroy(2,entry);//entry, store
	
	return file;
	}

void CImAttachmentManager::RemoveAttachmentL(TInt aIndex, TRequestStatus& aStatus)
	{	
	__ASSERT_ALWAYS(aIndex >= 0 && aIndex < iAttachmentInfoList.Count(),User::Invariant());
	
	RemoveAttachmentL(iAttachmentInfoList[aIndex]->Id(),aStatus);
	}

void CImAttachmentManager::RemoveAttachmentL(TMsvAttachmentId aId, TRequestStatus& aStatus)
	{
	iEmailMessage.RemoveAttachmentL(aId,aStatus);
	}

void CImAttachmentManager::CancelRequest()
	{
	iEmailMessage.Cancel();
	}
	
void CImAttachmentManager::ModifyAttachmentInfoL(CMsvAttachment* aAttachmentInfo, TRequestStatus& aStatus)
	{
	TInt index = AttachmentIndexInMessage(aAttachmentInfo->Id());
	User::LeaveIfError(index);
	
	iAttachmentManagerState = EModifyAttachmentInfo;

	delete iEntry;
	iEntry = NULL;
	
	iEntry=CMsvEntry::NewL(iClientEntry.Session(), iAttachmentInfoList[index]->Id(), TMsvSelectionOrdering());
	iStore = iEntry->EditStoreL();

	MMsvAttachmentManager& attachmentMgr = iStore->AttachmentManagerL();

	aStatus = KRequestPending;
	iReportStatus=&aStatus;
	
	// The id stored in the Attachment info list in CImemailmessage is the 
	// attachment entry id, this is not the actual id of the attachment
	// Reset the id as 1 , as emails have one attachment per attachment entry.
	aAttachmentInfo->SetId(1);
	attachmentMgr.ModifyAttachmentInfoL(aAttachmentInfo,iStatus);
	SetActive();
	}	

void CImAttachmentManager::LoadL() 
	{
	// Flush the array before loading
	iAttachmentInfoList.ResetAndDestroy();
	
	// Load the attachments only if it was a message entry
	const TMsvEmailEntry& entry=iClientEntry.Entry();
	if(entry.iType==KUidMsvMessageEntry)
		{
		// CImEmailMessage fills  the attachment list for CImAttachmentManager::iAttachmentInfoList
		// get the attachment list from CImEmailMessage.
		iEmailMessage.GetAttachmentsListL(iClientEntry.EntryId(),CImEmailMessage::EAllAttachments, CImEmailMessage::EThisMessageAndEmbeddedMessages);
		}
	}

RPointerArray<CMsvAttachment>& CImAttachmentManager::AttachmentInfoSelection()
	{
	return iAttachmentInfoList;
	}

TInt CImAttachmentManager::AttachmentIndexInMessage(TMsvAttachmentId aId)
	{
	if(!iAttachmentInfoList.Count())	
		return KErrNotFound;	
	// Traverse the list and get the index of the required attachment
	for(TInt i = 0;i < iAttachmentInfoList.Count(); i++)
		{
		// Id contains unique per-message id.
		if(aId == iAttachmentInfoList[i]->Id())
			{
			// found the entry
			return i;
			}
		}
	return KErrNotFound;	
	}	
	
void CImAttachmentManager::AppendAttachmentArrayL(TMsvId aAttachmentId)
	{
	CMsvEntry* entry=CMsvEntry::NewL(iClientEntry.Session(), aAttachmentId, TMsvSelectionOrdering());
	CleanupStack::PushL(entry);

	CMsvStore* store = entry->ReadStoreL();
	CleanupStack::PushL(store);
	
	MMsvAttachmentManager& attachmentMgr = store->AttachmentManagerL();
	if(attachmentMgr.AttachmentCount())
		{
		CMsvAttachment* attachment = attachmentMgr.GetAttachmentInfoL(0);
		attachment->SetId(aAttachmentId);	
		CleanupStack::PushL(attachment);
		iAttachmentInfoList.AppendL(attachment);		
		CleanupStack::Pop(attachment);
		}
	CleanupStack::PopAndDestroy(2,entry);//store, entry	
	}

void CImAttachmentManager::DeleteAttachmentInArrayL(TMsvId aDeletedAttachmentId)
	{
	TInt deleteAttachmentIdIndex = AttachmentIndexInMessage(aDeletedAttachmentId);
	// leaves if error
	User::LeaveIfError(deleteAttachmentIdIndex );
	
	delete iAttachmentInfoList[deleteAttachmentIdIndex];
	iAttachmentInfoList.Remove(deleteAttachmentIdIndex);
	}
	
void CImAttachmentManager::UpdateModifiedAttachmentInListL()
	{
	CMsvStore* store = iEntry->ReadStoreL();
	CleanupStack::PushL(store);
	MMsvAttachmentManager& attachmentMgr = store->AttachmentManagerL();
	CMsvAttachment* attachmentInfo = attachmentMgr.GetAttachmentInfoL(0);
	CleanupStack::PopAndDestroy(store);
	
	CleanupStack::PushL(attachmentInfo);
	attachmentInfo->SetId(iEntry->EntryId());
	TInt index = AttachmentIndexInMessage(iEntry->EntryId());
	
	// Insert the modified attachment at the index which was modified
	iAttachmentInfoList.InsertL(attachmentInfo, index);
	CleanupStack::Pop(attachmentInfo);
	
	// remove the previous attachment info from the list
	delete iAttachmentInfoList[index+1];
	iAttachmentInfoList.Remove(index+1);
	}
	
void CImAttachmentManager::ChangeEntryDetailsL()
	{
	TInt index = AttachmentIndexInMessage(iEntry->EntryId());
	TMsvEntry entry(iEntry->Entry());		
	entry.iSize = iAttachmentInfoList[index]->Size();
	entry.SetComplete(iAttachmentInfoList[index]->Complete());
	
	delete iMsvOperation;
	iMsvOperation = NULL;
	iMsvOperation = iEntry->ChangeL(entry,iStatus);
	SetActive();
	}

void CImAttachmentManager::ModifyAttachmentInfoL()	
	{
	iStore->CommitL();
	delete iStore;
	iStore = NULL;
	UpdateModifiedAttachmentInListL();
	}
	
TInt CImAttachmentManager::UniqueAttachmentId()
	{
	// Initialisation of iUniqueId is left to CBase (ie 0) id's start at 1 so pre-increment
	return ++iUniqueId;
	}

