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

#include "cbodyqueueentry16.h"
#include "cimaputils.h"
#include "cimaplogger.h"
#include <txtrich.h>

/**
Constructor.
*/
CBodyChunk16Info::CBodyChunk16Info(TInt aChunkNumber,HBufC8* aData)
:iChunkNumber(aChunkNumber),iData(aData)
	{
	}
/**
Destructor.
*/	
CBodyChunk16Info::~CBodyChunk16Info()
	{
	delete iData;
	}
/**
Constructor.
*/
CBodyQueueEntry16::CBodyQueueEntry16(TInt aTotalChunks,CImapMailStore& aParent,CMsvServerEntry& aServerEntry,CImapSettings& aImapSettings,CFetchBodyInfo& aFetchBodyInfo,TInt aLogId,MMailStoreObserver& aObserver, TBool aBinaryCap)
	: CQueueEntryBase(aFetchBodyInfo.PartId(),EBody16,aParent,aServerEntry,aObserver),
	iTotalChunks(aTotalChunks),
	iEncoding(aFetchBodyInfo.ContentTransferEncoding()),
	iCharsetId(aFetchBodyInfo.CharsetId()),
	iBodyPartRemainingSize(aFetchBodyInfo.BodyPartRemainingSize()),
	iImapSettings(aImapSettings),
	iLogId(aLogId)
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
CBodyQueueEntry16::~CBodyQueueEntry16()
	{
	delete iBodyText;
	delete iCharFormatLayer;
	delete iParaFormatLayer;
	delete iStoreUtilities;
	iDataArray.Close();
	}
	
/**
Factory constructors.
*/
CBodyQueueEntry16* CBodyQueueEntry16::NewL(TInt aTotalChunks,CImapMailStore& aParent,CMsvServerEntry& aServerEntry,CImapSettings& aImapSettings,CFetchBodyInfo& aFetchBodyInfo,TInt aLogId,MMailStoreObserver& aObserver, TBool aBinaryCap)
	{
	CBodyQueueEntry16* self=new(ELeave)CBodyQueueEntry16(aTotalChunks,aParent,aServerEntry,aImapSettings,aFetchBodyInfo,aLogId,aObserver,aBinaryCap);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CBodyQueueEntry16::ConstructL()
	{
	BaseConstructL();
	iParaFormatLayer=CParaFormatLayer::NewL();
	iCharFormatLayer=CCharFormatLayer::NewL(); 
	iBodyText=CRichText::NewL(iParaFormatLayer, iCharFormatLayer);
	if(iCharsetId == KUidMsvCharsetNone)
		{
		// Get the default charset
		CCnvCharacterSetConverter* characterConverter = CCnvCharacterSetConverter::NewL();
		CleanupStack::PushL(characterConverter);
		CImConvertCharconv* charConv = CImConvertCharconv::NewL(*characterConverter, CImapUtils::GetRef().Fs());
	
		iCharsetId = charConv->SystemDefaultCharset();
		delete charConv;
		CleanupStack::PopAndDestroy();//characterConverter
		}
	iStoreUtilities=CStoreUtilities::NewL(iEncoding,iCharsetId, CImapUtils::GetRef().Fs());
	}

/**
Adds a chunk of body data to the data array and sorts the array
@param aData 			the body data, ownership  is taken.
@param aChunkNumber 	the order number in which the chunk makes up the whole data.
@return 
*/		
void CBodyQueueEntry16::AddChunkL(HBufC8* aData,TInt aChunkNumber, TInt aExtraFetchRequestCount)
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
	__LOG_FORMAT((iLogId, "CBodyQueueEntry16::AddChunkL chunk number = %d",aChunkNumber));
	CleanupStack::PushL(aData);
	//make sure the chunk is within range.
	__ASSERT_DEBUG(aChunkNumber<iTotalChunks,TImapServerPanic::ImapPanic(TImapServerPanic::EMailStoreDataChunkOutOfRange));
	//append the chunk
 	CBodyChunk16Info* chunkInfo = new(ELeave) CBodyChunk16Info(aChunkNumber,aData);
 	CleanupStack::PushL(chunkInfo);
 	TLinearOrder<CBodyChunk16Info>compareChunks(CompareChunks); 	
 	iDataArray.InsertInOrderL(chunkInfo,compareChunks);
    CleanupStack::Pop(2,aData);
    
    if(!IsActive())
		{
  	  	CompleteSelf();	
		}
	}
	
/**
Used to sort chunks of data by chunk number.
@param aChunkBodyInfo1 	the first chunk in the comparision
@param aChunkBodyInfo2 	the second chunk in the comparision
@return TInt			the difference between the first chunks number and the second chunks number.
*/	
TInt CBodyQueueEntry16::CompareChunks(const CBodyChunk16Info& aChunkBodyInfo1, const CBodyChunk16Info& aChunkBodyInfo2)
	{
	return aChunkBodyInfo1.iChunkNumber-aChunkBodyInfo2.iChunkNumber;	
	}

TInt CBodyQueueEntry16::RunError(TInt aError)
	{
	iDataArray.Close();
	return CQueueEntryBase::RunError(aError);
	}
	
/**
Uses CMsvEntry to synchronousy store the requests data object.
@param 
@return
*/
void CBodyQueueEntry16::RunL()
	{
	
	//while there are contiguous chunk in the data array append the data to the CRichText object
	while(iDataArray.Count()>0 && iDataArray[0]->iChunkNumber==iNextExpectedChunk)
		{
		++iNextExpectedChunk;
		//is this the last chunk?
		TBool lastChunk=(iNextExpectedChunk==iTotalChunks);
		HBufC8* decodedData = NULL;
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)		
		if(iBinaryCap)
			{
			__LOG_FORMAT((iLogId, "CBodyQueueEntry16::RunL Decoding not required since binary capability is set..."));	
			const TPtrC8& bodyData = *(iDataArray[0]->iData);
			decodedData = HBufC8::NewL(bodyData.Length());
			// Message body is downloaded using Fetch Binary and the data is decoded by the IMAP mail server
			// Nothing to do, just copy data
			decodedData->Des().Append(bodyData);			
			}
		else
#endif
			{	
			//decode the data
			__LOG_FORMAT((iLogId, "CBodyQueueEntry16::RunL Decoding..."));
			decodedData=iStoreUtilities->DecodeL(*(iDataArray[0]->iData),lastChunk);
			}	
		__ASSERT_DEBUG(decodedData!=NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EMailStoreDecodeDataNull));
		CleanupStack::PushL(decodedData); 
		//if this is the last chunk and if was a partial download then add footer
		if(iBodyPartRemainingSize && iNextExpectedChunk==iTotalChunks)
			{
			__LOG_FORMAT((iLogId, "CBodyQueueEntry16::RunL AttachFooterInfoL..."));
			iStoreUtilities->AttachFooterInfoL(iBodyPartRemainingSize,iImapSettings,decodedData);	
			}
			
		__LOG_FORMAT((iLogId, "CBodyQueueEntry16::RunL WriteToBodyL..."));
		//convert the data
		iStoreUtilities->WriteToBodyL(decodedData->Des(),iBodyText);
		
		CleanupStack::PopAndDestroy(decodedData); 
		//delete the data
		CBodyChunk16Info* chunkInfo=iDataArray[0];
		iDataArray.Remove(0);
		delete chunkInfo;
		}
		
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)				
	// if message body is downloaded using FETCH BINARY, there might be some extra fetch command
	// issued, remove these extra fetch requests from iTotalChunks.
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
		__LOG_FORMAT((iLogId, "CBodyQueueEntry16::RunL Commiting body data..."));
		iServerEntry.SetEntry(iId);
		CMsvStore* store = iServerEntry.EditStoreL();
		CleanupStack::PushL(store);		
		store->StoreBodyTextL(*iBodyText);
		store->CommitL();
		CleanupStack::PopAndDestroy(store); 
		//call back to the observer as the operation is complete
		StoreComplete(KErrNone);	
		//request can be removed from the queue
		iParent.RemoveFromQueueAndDelete(this);	
		}
	}
