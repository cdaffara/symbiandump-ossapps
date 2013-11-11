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
 
#include <mmsvattachmentmanager.h>
#include <mmsvattachmentmanagersync.h>
#include <imcvtext.h>
#include <miuthdr.h>
#include "cattachmentqueueentry.h"
#include "cimaputils.h"
#include "cimaplogger.h"

/**
Used to sort chunks of data by chunk number.
@param aChunkAttachmentInfo1 	the first chunk in the comparision
@param aChunkAttachmentInfo2 	the second chunk in the comparision
@return TInt					the difference between the first chunks number and the second chunks number.
*/	
TInt CAttachmentQueueEntry::CompareChunks(const CChunkAttachmentInfo& aChunkAttachmentInfo1, const CChunkAttachmentInfo& aChunkAttachmentInfo2)
	{
	return aChunkAttachmentInfo1.iChunkNumber-aChunkAttachmentInfo2.iChunkNumber;	
	}

/**
Constructor.
*/
CChunkAttachmentInfo::CChunkAttachmentInfo(TInt aChunkNumber,HBufC8* aData)
:iChunkNumber(aChunkNumber),iData(aData)
	{
	}

/**
Destructor.
*/	
CChunkAttachmentInfo::~CChunkAttachmentInfo()
	{
	delete iData;
	}
		
/**
Constructor.
*/
CAttachmentQueueEntry::CAttachmentQueueEntry(TInt aTotalChunks,CImapMailStore& aParent,CMsvServerEntry& aServerEntry,CImapSettings& aImapSettings,CFetchBodyInfo& aFetchBodyInfo,TInt aLogId,MMailStoreObserver& aObserver)
	: CQueueEntryBase(aFetchBodyInfo.PartId(),EAttachment,aParent,aServerEntry,aObserver),
	iTotalChunks(aTotalChunks),
	iEncoding(aFetchBodyInfo.ContentTransferEncoding()),
	iImapSettings(aImapSettings),
	iCaf(aFetchBodyInfo.Caf()),
	iLogId(aLogId)
	{
	}
	
/**
Destructor.
*/
CAttachmentQueueEntry::~CAttachmentQueueEntry()
	{
	iFile.Close();
	iDataArray.ResetAndDestroy();
	delete iStoreUtilities;
	delete iDecodedData;
	iDataArray.Close();
	}

/**
Factory constructors.
*/
CAttachmentQueueEntry* CAttachmentQueueEntry::NewL(TInt aTotalChunks,CImapMailStore& aParent,CMsvServerEntry& aServerEntry,CImapSettings& aImapSettings,CFetchBodyInfo& aFetchBodyInfo,TInt aLogId,MMailStoreObserver& aObserver)
	{
	CAttachmentQueueEntry* self=new(ELeave)CAttachmentQueueEntry(aTotalChunks,aParent,aServerEntry,aImapSettings,aFetchBodyInfo,aLogId,aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();	
	CleanupStack::Pop();
	return self;
	}
	
void CAttachmentQueueEntry::ConstructL()
	{
	BaseConstructL();
	iServerEntry.SetEntry(iId);	
	//create an attachment ready for downloading	
	CreateAttachmentL(iServerEntry);
	//charsetId is not used by attachments
	TUint charsetId=0;
	iStoreUtilities=CStoreUtilities::NewL(iEncoding,charsetId, CImapUtils::GetRef().Fs());
	}

/**
Adds a chunk of attachment data to the data array and sorts the array
@param aData 			the attachment data, ownership  is taken.
@param aChunkNumber 	the order number in which the chunk makes up the whole data.
@return 
*/	
void CAttachmentQueueEntry::AddChunkL(HBufC8* aData,TInt aChunkNumber)
	{
	__LOG_FORMAT((iLogId, "CAttachmentQueueEntry::AddChunkL chunk number = %d",aChunkNumber));
	CleanupStack::PushL(aData);
	__ASSERT_DEBUG(aChunkNumber<iTotalChunks,TImapServerPanic::ImapPanic(TImapServerPanic::EMailStoreDataChunkOutOfRange));
	
 	CChunkAttachmentInfo* chunkInfo = new(ELeave) CChunkAttachmentInfo(aChunkNumber,aData); 	
 	CleanupStack::PushL(chunkInfo);
	TLinearOrder<CChunkAttachmentInfo>compareChunks(CompareChunks);
   	iDataArray.InsertInOrderL(chunkInfo,compareChunks);
    CleanupStack::Pop(2,aData);
    
    if(!IsActive())
		{
  	  	CompleteSelf();	
		}
	}
	
/**
Creates and stores an CMsvAttachment object and an empty file as a place holder for an attachment
	    that is not currently being downloaded.
@param 	aServerEntry entry used to access the store.
		aFile used to create the attachment file.
@return
*/
void CAttachmentQueueEntry::CreateAttachmentInfoL(CMsvServerEntry& aServerEntry,RFile& aFile)
	{
	CMsvStore* store = aServerEntry.EditStoreL();
	CleanupStack::PushL(store);

	MMsvAttachmentManager& attachmentMgr = store->AttachmentManagerL();

	TInt attachmentCount=attachmentMgr.AttachmentCount();
	// if we are just regestering that the attachment exists we dont need to do any more. 
	// need to do anything	
	if(attachmentCount!=0)
		{
		CleanupStack::PopAndDestroy(store);
		return;
		}
	
	MMsvAttachmentManagerSync& attachmentMgrSync = store->AttachmentManagerExtensionsL();
	// Now create the attachment entry
	CMsvAttachment* attachment = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
	CleanupStack::PushL(attachment);
	
	// Need to create the MIME-type information - first get the MIME headers
	//check if their present if not leave the mime type blank
	if(store->IsPresentL(KUidMsgFileMimeHeader))
		{
		CImMimeHeader* mimeHeaders = CImMimeHeader::NewLC();
		mimeHeaders->RestoreL(*store);

		HBufC8* buf = HBufC8::NewLC(mimeHeaders->ContentSubType().Length() + mimeHeaders->ContentType().Length() + 1);
		TPtr8 ptr(buf->Des());
		ptr.Copy(mimeHeaders->ContentType());
		ptr.Append(KImcvForwardSlash);
		ptr.Append(mimeHeaders->ContentSubType());
		//set the mime time
		attachment->SetMimeTypeL(ptr);
		CleanupStack::PopAndDestroy(2, mimeHeaders);	
		}

	//set flags	
	attachment->SetAttachmentNameL(aServerEntry.Entry().iDetails);	
	attachment->SetComplete(EFalse);
	//if not downloading then set the size here
	attachment->SetSize(aServerEntry.Entry().iSize);
	//Creates an empty file representing the attachment, used by MMsvAttachmentManagerSync
	attachmentMgrSync.CreateAttachmentL(aServerEntry.Entry().iDetails,aFile,attachment);
 	// ownership passed to attachment manager
	CleanupStack::Pop(attachment);	
	//commit changes
	store->CommitL();
	CleanupStack::PopAndDestroy(store);
	}
	
/**
Creates and stores a CMsvAttachment object and file representing the attachment being downloaded.
@param 	aServerEntry entry used to access the store.			
@return
*/
void CAttachmentQueueEntry::CreateAttachmentL(CMsvServerEntry& aServerEntry)
	{
	__LOG_FORMAT((iLogId, "CAttachmentQueueEntry::CreateAttachmentL"));
	
	CMsvStore* store = aServerEntry.EditStoreL();
	CleanupStack::PushL(store);

	MMsvAttachmentManager& attachmentMgr = store->AttachmentManagerL();

	TInt attachmentCount=attachmentMgr.AttachmentCount();

	//delete existing attachments
	for(TInt i=0;i<attachmentCount;++i)
		{
		// Remove [0] as array is shuffled. Once index [n] is removed n+1 becomes n
		store->AttachmentManagerExtensionsL().RemoveAttachmentL(0);
		}
	if(attachmentCount)
		{
		store->CommitL();	
		}	

	MMsvAttachmentManagerSync& attachmentMgrSync = store->AttachmentManagerExtensionsL();
	// Now create the attachment entry
	CMsvAttachment* attachment = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
	CleanupStack::PushL(attachment);
	
	// Need to create the MIME-type information - first get the MIME headers
	//check if their present if not leave the mime type blank
	if(store->IsPresentL(KUidMsgFileMimeHeader))
		{
		CImMimeHeader* mimeHeaders = CImMimeHeader::NewLC();
		mimeHeaders->RestoreL(*store);

		HBufC8* buf = HBufC8::NewLC(mimeHeaders->ContentSubType().Length() + mimeHeaders->ContentType().Length() + 1);
		TPtr8 ptr(buf->Des());
		ptr.Copy(mimeHeaders->ContentType());
		ptr.Append(KImcvForwardSlash);
		ptr.Append(mimeHeaders->ContentSubType());
		//set the mime time
		attachment->SetMimeTypeL(ptr);
		CleanupStack::PopAndDestroy(2, mimeHeaders);	
		}

	//set flags	
	attachment->SetAttachmentNameL(aServerEntry.Entry().iDetails);	
	attachment->SetComplete(EFalse);
	
	//is caf interested in this attachment?
	
	if(iCaf!=NULL && iCaf->Registered())
		{
		iCaf->PrepareProcessingL(); // Init the CAF import file session
		RFile startFile;
		TFileName suggestedFileName;
		if(iCaf->GetSuggestedAttachmentFileName(suggestedFileName) == KErrNone) // CAF agent may provide a filename
			{
			attachmentMgrSync.CreateAttachmentL(suggestedFileName,startFile,attachment);
			}
		else
			{
			attachmentMgrSync.CreateAttachmentL(aServerEntry.Entry().iDetails,startFile,attachment);
			}
			
		iCaf->StartProcessing(iImapSettings.DefaultAttachmentName(),attachment->FilePath(),aServerEntry,startFile); // Init the CAF session
		startFile.Close();
		}
	else
		{
		// Normal behaviour
		attachmentMgrSync.CreateAttachmentL(aServerEntry.Entry().iDetails,iFile,attachment);
		}

	CleanupStack::Pop(attachment);	
	//commit changes
	store->CommitL();
	CleanupStack::PopAndDestroy(store);
		
	}
	
void CAttachmentQueueEntry::DoCancel()
	{
	//there is no way to cancel the RFile::Write() request.	
	delete iDecodedData;
	iDecodedData=NULL;
	}
	
	
void CAttachmentQueueEntry::CancelRequest()
	{
	//any error is ingnored
	TRAP_IGNORE(CancelRequestL());	
	}
	
void CAttachmentQueueEntry::CancelRequestL()
	{
	if (iCaf && iCaf->Processing())
		{
		iCaf->EndProcessingL();
		}
	else
		{
		iFile.Flush();
		iFile.Close();
		}

	CMsvStore* store = iServerEntry.EditStoreL(); 
	CleanupStack::PushL(store);
	// Could be multiple attachments in the folder.
	TInt attachmentCount = store->AttachmentManagerL().AttachmentCount();
	for(TInt i=0;i<attachmentCount;i++)
		{
		// Remove [0] as array is shuffled. Once index [n] is removed n+1 becomes n
		store->AttachmentManagerExtensionsL().RemoveAttachmentL(0);
		}
	if(attachmentCount)
		{
		store->CommitL();	
		}
	CleanupStack::PopAndDestroy(store);
	//create attachment info
	CreateAttachmentInfoL(iServerEntry,iFile);	
	}
	
TInt CAttachmentQueueEntry::RunError(TInt aError)
	{
	delete iDecodedData;
	iDecodedData=NULL;
	iDataArray.Close();
	return CQueueEntryBase::RunError(aError);
	}
	
/**
Uses an RFile object to asynchronously write the attachment data to the file system.
@param 	 
@return
*/
void CAttachmentQueueEntry::RunL()
	{	
	
	//if we have just saved a chunk of data to the file store then we can delete the chunk.
	if(iReadyToRemoveChunk)
		{
		delete iDecodedData;
		iDecodedData=NULL;
		iReadyToRemoveChunk=EFalse;	
		}
		
	//if we have finished then we close the file and call back to the observer
	if(iNextExpectedChunk==iTotalChunks)
		{
		
		if(iCaf!=NULL && iCaf->Processing())
			{
			__LOG_FORMAT((iLogId, "CAttachmentQueueEntry::RunL finished caf processing..."));
			iCaf->EndProcessingL();		
			}
		else
			{
			__LOG_FORMAT((iLogId, "CAttachmentQueueEntry::RunL storing attachment..."));
			iFile.Close();	
			}
		
		StoreComplete(KErrNone);
		//request can be removed from the queue
		iParent.RemoveFromQueueAndDelete(this);	
		}
	//if we have an item of contiguous data in the data array then write it to the file asynchronously
	else if(iDataArray.Count()>0 && iDataArray[0]->iChunkNumber==iNextExpectedChunk )
		{
		++iNextExpectedChunk;
		//is this the last chunk?
		TBool lastChunk=(iNextExpectedChunk==iTotalChunks);
		//decode the data
		iDecodedData=iStoreUtilities->DecodeL(*(iDataArray[0]->iData),lastChunk);
		
		if(iCaf!=NULL && iCaf->Processing())
			{
			iCaf->WriteData(*iDecodedData);
			CompleteSelf();
			}
		else
			{
			iFile.Write(*iDecodedData,iStatus);		
			SetActive();
			}
		//delete the origninal encoded chunk
		CChunkAttachmentInfo* chunkInfo=iDataArray[0];
		iDataArray.Remove(0);
		delete chunkInfo;	
			
		iReadyToRemoveChunk=ETrue;
		}
	}
