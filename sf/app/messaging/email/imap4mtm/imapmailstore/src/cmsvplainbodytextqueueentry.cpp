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
// CMSVPLAINBODYTEXTQUEENTRY.CPP
// 
//

#include "cmsvplainbodytextqueueentry.h"
#include "cimaputils.h"
#include "cimaplogger.h"

#include <cmsvplainbodytext.h>

const TInt KOutOfOrderChunks = 3;



CPlainBodyChunkInfo* CPlainBodyChunkInfo::NewL(TInt aChunkNumber, HBufC8* aData)
	{
	CPlainBodyChunkInfo* self =	new(ELeave) CPlainBodyChunkInfo(aChunkNumber,aData);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

/**
Constructor for CPlainBodyChunkInfo.
*/
CPlainBodyChunkInfo::CPlainBodyChunkInfo(TInt aChunkNumber, HBufC8* aData)
:iChunkNumber(aChunkNumber), iData(aData)
	{
	}

void CPlainBodyChunkInfo::ConstructL()
	{
	
	}

/**
Destructor for CPlainBodyChunkInfo.
*/	
CPlainBodyChunkInfo::~CPlainBodyChunkInfo()
	{
	delete iData;
	}

/**
Constructor.
*/
CMsvPlainBodyTextQueueEntry::CMsvPlainBodyTextQueueEntry(TInt aTotalChunks, CImapMailStore& aParent, CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings, CFetchBodyInfo& aFetchBodyInfo, TInt aLogId, MMailStoreObserver& aObserver, MChunkOutOfOrderObserver& aChunkObserver, TBool aBinaryCap)
	:CQueueEntryBase(aFetchBodyInfo.PartId(), EPlainTextBody, aParent, aServerEntry, aObserver),
	iTotalChunks(aTotalChunks),
	iEncoding(aFetchBodyInfo.ContentTransferEncoding()),
	iCharsetId(aFetchBodyInfo.CharsetId()),
	iBodyPartRemainingSize(aFetchBodyInfo.BodyPartRemainingSize()),
	iImapSettings(aImapSettings),
	iLogId(aLogId),
	iChunkObserver(aChunkObserver)
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)		
	, iBinaryCap(aBinaryCap)
#endif
	{
	//to ignore compilation warnings
	aBinaryCap = aBinaryCap;
	}


/**
Destructor.
*/		
CMsvPlainBodyTextQueueEntry::~CMsvPlainBodyTextQueueEntry()
	{
	delete iStoreUtilities;
	iDataArray.ResetAndDestroy();
	iDataArray.Close();
	delete iPlainBodyText;
	delete iStore;
	delete iDecodedData;
	}
	
/**
NewL Factory functions.
*/
CMsvPlainBodyTextQueueEntry* CMsvPlainBodyTextQueueEntry::NewL(TInt aTotalChunks, CImapMailStore& aParent, CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings, CFetchBodyInfo& aFetchBodyInfo, TInt aLogId, MMailStoreObserver& aObserver, MChunkOutOfOrderObserver& aChunkObserver, TBool aBinaryCap)
	{
	CMsvPlainBodyTextQueueEntry* self=new(ELeave)CMsvPlainBodyTextQueueEntry(aTotalChunks, aParent, aServerEntry, aImapSettings, aFetchBodyInfo, aLogId, aObserver, aChunkObserver, aBinaryCap);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

/**
The Two Phase constructor
*/
void CMsvPlainBodyTextQueueEntry::ConstructL()
	{
	BaseConstructL();
	
	// Get the default charset
	CCnvCharacterSetConverter* characterConverter = CCnvCharacterSetConverter::NewL();
	CleanupStack::PushL(characterConverter);
	CImConvertCharconv* charConv = CImConvertCharconv::NewL(*characterConverter, CImapUtils::GetRef().Fs());
	CleanupStack::PushL(charConv);

	TUint defaultCharset =	charConv->SystemDefaultCharset();
	CleanupStack::PopAndDestroy(2, characterConverter);
	if(iCharsetId == KUidMsvCharsetNone)
		{
		iCharsetId = defaultCharset;	
		}
		
	iStoreUtilities=CStoreUtilities::NewL(iEncoding,iCharsetId,CImapUtils::GetRef().Fs());
	iServerEntry.SetEntry(iId);
	iStore = iServerEntry.EditStoreL();
	TInt is8Bit = iImapSettings.Store8BitData();
	iPlainBodyText = iStore->InitialisePlainBodyTextForWriteL(is8Bit,iCharsetId,defaultCharset);
	}
	
	

/**
Adds a chunk of body data to the data array and sorts the array
@param aData 			the body data, ownership  is taken.
@param aChunkNumber 	the order number in which the chunk makes up the whole data.
@return TBool			indicates whether the FETCH command need to be sent to server
						ETrue : Send the FETCH command.
						EFalse: Do not send the FETCH command.
*/	
	
TBool CMsvPlainBodyTextQueueEntry::AddChunkL(HBufC8* aData, TInt aChunkNumber, TInt aExtraFetchRequestCount)
	{
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)			
	if(iBinaryCap)
		{	
		iExtraFetchRequestCount=aExtraFetchRequestCount;
		}
#else
	//to ignore compilation warnings
	aExtraFetchRequestCount = aExtraFetchRequestCount;
#endif
	TBool sendCommand = ETrue;
	__LOG_FORMAT((iLogId, "CMsvPlainBodyTextQueueEntry::AddChunkL chunk number = %d",aChunkNumber));
	CleanupStack::PushL(aData);
	//make sure the chunk is within range.
	__ASSERT_DEBUG(aChunkNumber<iTotalChunks,TImapServerPanic::ImapPanic(TImapServerPanic::EMailStoreDataChunkOutOfRange));
	//append the chunk
 	CPlainBodyChunkInfo* chunkInfo = CPlainBodyChunkInfo::NewL(aChunkNumber, aData);
  	CleanupStack::Pop(aData);
 	CleanupStack::PushL(chunkInfo);
 
 	TLinearOrder<CPlainBodyChunkInfo>compareChunks(CompareChunks); 	
 	iDataArray.InsertInOrderL(chunkInfo,compareChunks);
 
 	if(iDataArray.Count() >= KOutOfOrderChunks)
 		{
 		sendCommand = EFalse;
 		}
 		
    CleanupStack::Pop(chunkInfo);
    
    if(!IsActive())
		{
  	  	CompleteSelf();	
		}
	return sendCommand;
	}

	
/** 
Used to sort chunks of data by chunk number.
@param aChunkBodyInfo1 	the first chunk in the comparision
@param aChunkBodyInfo2 	the second chunk in the comparision
@return TInt			the difference between the first chunks number and the second chunks number.
*/	
TInt CMsvPlainBodyTextQueueEntry::CompareChunks(const CPlainBodyChunkInfo& aChunkBodyInfo1, const CPlainBodyChunkInfo& aChunkBodyInfo2)
	{
	return aChunkBodyInfo1.iChunkNumber-aChunkBodyInfo2.iChunkNumber;	
	}

TInt CMsvPlainBodyTextQueueEntry::RunError(TInt aError)
	{
	return CQueueEntryBase::RunError(aError);
	}
	
	
	
/**
Uses CMsvPlainBodyText to asynchronousy store the requests data object.
@param 
@return
*/
void CMsvPlainBodyTextQueueEntry::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	if(iReadyToRemoveChunk)
		{
		delete iDecodedData;
		iDecodedData=NULL;
		iChunkObserver.EnableSendFetch();
		iReadyToRemoveChunk=EFalse;	
		}

#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)		
	if(iBinaryCap && iExtraFetchRequestCount > 0)
		{
		iTotalChunks-=iExtraFetchRequestCount;
		//reset iExtraFetchRequestCount to zero
		iExtraFetchRequestCount=0;
		}
#endif

	//if we have appended all the chunks then store the data.
	if(iNextExpectedChunk==iTotalChunks)
		{
		__LOG_FORMAT((iLogId, "CMsvPlainBodyTextQueueEntry::RunL Commiting body data..."));
		iPlainBodyText->CommitL();
		//call back to the observer as the operation is complete
		StoreComplete(KErrNone);	
		//request can be removed from the queue
		iParent.RemoveFromQueueAndDelete(this);	
		}
	else if(iDataArray.Count()>0 && iDataArray[0]->iChunkNumber==iNextExpectedChunk )
		{
		++iNextExpectedChunk;
		//is this the last chunk?
		TBool lastChunk=(iNextExpectedChunk==iTotalChunks);
		HBufC8* decodedData = NULL;
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)		
		if(iBinaryCap)
			{
			const TPtrC8& bodyData = *(iDataArray[0]->iData);
			decodedData = HBufC8::NewL(bodyData.Length());
			// Message body is downloaded using Fetch Binary and the data is decoded by the IMAP mail server
			// Nothing to do, just copy data
			decodedData->Des().Append(bodyData);						
			}
		else
#endif
			{
			// create a new buffer, decodedData and push it to CleannupStack because AttachFooterInfoL will 
			// pop decodedData first and pushes it  when it returns.
			__LOG_FORMAT((iLogId, "CMsvPlainBodyTextQueueEntry::RunL Decoding..."));
			decodedData = iStoreUtilities->DecodeL(*(iDataArray[0]->iData), lastChunk);				
			}
			
		__ASSERT_DEBUG(decodedData!=NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EMailStoreDecodeDataNull));					
		CleanupStack::PushL(decodedData); 
		
		//if this is the last chunk and if was a partial download then add footer
		if(iBodyPartRemainingSize && iNextExpectedChunk==iTotalChunks)
			{
			__LOG_FORMAT((iLogId, "CMsvPlainBodyTextQueueEntry::RunL AttachFooterInfoL..."));
			iStoreUtilities->AttachFooterInfoL(iBodyPartRemainingSize, iImapSettings, decodedData);
			}
			
		// pop decodedData since the ownership will be transferred to iDecodedData.
		CleanupStack::Pop(decodedData);			
		__ASSERT_DEBUG(iDecodedData == NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EMailStoreDecodeDataNotNull));
		
		// transferring ownership to iDecodedData.
		iDecodedData = decodedData;
		
		__LOG_FORMAT((iLogId, "CMsvPlainBodyTextQueueEntry::RunL StoreChunk..."));
		iPlainBodyText->StoreChunkL(*iDecodedData, iStatus);
		SetActive();
		
		//delete the origninal encoded chunk
		CPlainBodyChunkInfo* chunkInfo=iDataArray[0];
		iDataArray.Remove(0);
		delete chunkInfo;	
		iReadyToRemoveChunk=ETrue;
		}
	}
