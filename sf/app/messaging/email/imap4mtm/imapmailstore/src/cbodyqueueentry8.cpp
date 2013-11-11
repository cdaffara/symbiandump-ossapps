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


#include "cbodyqueueentry8.h"
#include <cmsvbodytext.h>
#include "cimaputils.h"
#include "cimaplogger.h"

const TInt KBufferGranularity = 256;

/**
Constructor.
*/
CBodyChunk8Info::CBodyChunk8Info(TInt aChunkNumber,HBufC8* aData)
:iChunkNumber(aChunkNumber),iData(aData)
	{
	}
/**
Destructor.
*/	
CBodyChunk8Info::~CBodyChunk8Info()
	{
	delete iData;
	}
/**
Constructor.
*/
CBodyQueueEntry8::CBodyQueueEntry8(TInt aTotalChunks,CImapMailStore& aParent,CMsvServerEntry& aServerEntry,CImapSettings& aImapSettings,CFetchBodyInfo& aFetchBodyInfo,TInt aLogId,MMailStoreObserver& aObserver, TBool aBinaryCap)
	: CQueueEntryBase(aFetchBodyInfo.PartId(),EBody8,aParent,aServerEntry,aObserver),
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
CBodyQueueEntry8::~CBodyQueueEntry8()
	{
	delete iBodyBuf;
	delete iBodyText;
	delete iStoreUtilities;
	delete iCharConv;
	delete iCharacterConverter;
	iDataArray.Close();
	}

/**
Factory constructors.
*/
CBodyQueueEntry8* CBodyQueueEntry8::NewL(TInt aTotalChunks,CImapMailStore& aParent,CMsvServerEntry& aServerEntry,CImapSettings& aImapSettings,CFetchBodyInfo& aFetchBodyInfo,TInt aLogId,MMailStoreObserver& aObserver, TBool aBinaryCap)
	{
	CBodyQueueEntry8* self=new(ELeave)CBodyQueueEntry8(aTotalChunks,aParent,aServerEntry,aImapSettings,aFetchBodyInfo,aLogId,aObserver, aBinaryCap);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CBodyQueueEntry8::ConstructL()
	{
	BaseConstructL();
	iBodyBuf=CBufSeg::NewL(KBufferGranularity);
	iBodyText = CMsvBodyText::NewL();	
	iStoreUtilities=CStoreUtilities::NewL(iEncoding,iCharsetId, CImapUtils::GetRef().Fs());
	
	iCharacterConverter=CCnvCharacterSetConverter::NewL();
	iCharConv=CImConvertCharconv::NewL(*iCharacterConverter, CImapUtils::GetRef().Fs());
	
	//set the character set  for the data
	iBodyText->SetDefaultCharacterSet(iCharConv->SystemDefaultCharset());
	if (iCharsetId == KUidMsvCharsetNone)
		{
		iBodyText->SetCharacterSet(0);	
		}	
	else
		{
		iBodyText->SetCharacterSet(iCharsetId);	
		}
	}

/**
Adds a chunk of body data to the data array and sorts the array
@param aData 			the body data, ownership  is taken.
@param aChunkNumber 	the order number in which the chunk makes up the whole data.
@return 
*/		
void CBodyQueueEntry8::AddChunkL(HBufC8* aData,TInt aChunkNumber, TInt aExtraFetchRequestCount)
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
	__LOG_FORMAT((iLogId, "CBodyQueueEntry8::AddChunkL chunk number = %d",aChunkNumber));
	CleanupStack::PushL(aData);
	__ASSERT_DEBUG(aChunkNumber<iTotalChunks,TImapServerPanic::ImapPanic(TImapServerPanic::EMailStoreDataChunkOutOfRange));
 	CBodyChunk8Info* chunkInfo = new(ELeave) CBodyChunk8Info(aChunkNumber,aData);
 	CleanupStack::PushL(chunkInfo);
 	TLinearOrder<CBodyChunk8Info>compareChunks(CompareChunks);    
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
TInt CBodyQueueEntry8::CompareChunks(const CBodyChunk8Info& aChunkBodyInfo1, const CBodyChunk8Info& aChunkBodyInfo2)
	{
	return aChunkBodyInfo1.iChunkNumber-aChunkBodyInfo2.iChunkNumber;	
	}


TInt CBodyQueueEntry8::RunError(TInt aError)
	{
	iDataArray.Close();
	return CQueueEntryBase::RunError(aError);
	}
	
	
/**
Uses CMsvEntry to synchronousy store the requests data object.
@param 
@return
*/
void CBodyQueueEntry8::RunL()
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
			__LOG_FORMAT((iLogId, "CBodyQueueEntry8::RunL Decoding not required since binary capability is set..."));
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
			__LOG_FORMAT((iLogId, "CBodyQueueEntry8::RunL Decoding..."));
			decodedData=iStoreUtilities->DecodeL(*(iDataArray[0]->iData),lastChunk);				
			}
		__ASSERT_DEBUG(decodedData!=NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EMailStoreDecodeDataNull));
		CleanupStack::PushL(decodedData); 
		
		//if this is the last chunk and if was a partial download then add footer
		if(iBodyPartRemainingSize && iNextExpectedChunk==iTotalChunks)
			{
			__LOG_FORMAT((iLogId, "CBodyQueueEntry8::RunL AttachFooterInfoL..."));
			iStoreUtilities->AttachFooterInfoL(iBodyPartRemainingSize,iImapSettings,decodedData);	
			}
			
		iBodyBuf->InsertL(iBodyBuf->Size(), *decodedData);
		CleanupStack::PopAndDestroy(decodedData); 
		//delete the data
		CBodyChunk8Info* chunkInfo=iDataArray[0];
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
		__LOG_FORMAT((iLogId, "CBodyQueueEntry8::RunL Commiting body data..."));
		iServerEntry.SetEntry(iId);
		CMsvStore* store = iServerEntry.EditStoreL();
		CleanupStack::PushL(store);		
		iBodyText->StoreL(*store, *iBodyBuf);
		store->CommitL();
		CleanupStack::PopAndDestroy(store); 
		//call back to the observer as the operation is complete
		StoreComplete(KErrNone);	
		//request can be removed from the queue
		iParent.RemoveFromQueueAndDelete(this);	
		}
	}
