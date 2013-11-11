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

#include "cimapmailstore.h"
#include "cimheaderqueueentry.h"
#include "cimmimeheaderqueueentry.h"
#include "cbodyqueueentry8.h"
#include "cbodyqueueentry16.h"
#include "cattachmentqueueentry.h"
#include "cqueueentrybase.h"
#include "cimaplogger.h"
#include "imappaniccodes.h"
#include "cmsvplainbodytextqueueentry.h"

/**
Constructor.
*/
CImapMailStore::CImapMailStore(CMsvServerEntry& aServerEntry)
	: iServerEntry(aServerEntry)
	{	
	}
	
/**
Destructor.
*/
CImapMailStore::~CImapMailStore()
	{
	iRequestArray.ResetAndDestroy();
	}
	
/**
Factory constructors.
*/
EXPORT_C CImapMailStore* CImapMailStore::NewL(CMsvServerEntry& aServerEntry)
	{
	CImapMailStore* self=new(ELeave) CImapMailStore(aServerEntry);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CImapMailStore::ConstructL()
	{
	}
	

EXPORT_C TInt CImapMailStore::RequestCount() const
	{
	return iRequestArray.Count();	
	}

/**
Tests two CQueueEntryBase for a match based on entry id and entry type. 
@param aFirstEntry 	the first object to be compared.
@param aSecondEntry	the second object to be compared.
@return ETrue if a match was identified, EFalse otherwise
*/

TBool CImapMailStore::IdsAndTypeMatch(const CQueueEntryBase& aFirstEntry,const CQueueEntryBase& aSecondEntry)
    {
    return ((aFirstEntry.GetId() == aSecondEntry.GetId()) && (aFirstEntry.GetEntryType() == aSecondEntry.GetEntryType()));
    }

/**
Removes a CQueueEntryBase derived object that has completed its request from the array and deletes it.
@param aQueueEntry	the CQueueEntryBase derived object to be removed.
@return
*/	
	
void CImapMailStore::RemoveFromQueueAndDelete(CQueueEntryBase* aQueueEntry)
	{
	TInt index=iRequestArray.Find(aQueueEntry);
	__ASSERT_DEBUG(index!=KErrNotFound,TImapServerPanic::ImapPanic(TImapServerPanic::EMailStoreQueueEntryObjectNotFound));
	CQueueEntryBase* qEntry=iRequestArray[index];
	iRequestArray.Remove(index);
	delete qEntry;		
	}
	
/**
Adds a request to store a CIMHeader to the queue.
@param aHeader		CImHeader* object to be stored, ownership is taken.
@param aId			The id of the entry under which the object will be stored.
@param aObserver	A reference to a MMailStoreObserver object that will be called back when the request is complete.
@return
*/	

EXPORT_C void CImapMailStore::StorePartL(CImHeader* aHeader,TMsvId aId,MMailStoreObserver& aObserver)
	{	
	CImHeaderQueueEntry* qEntry=CImHeaderQueueEntry::NewL(aHeader,aId,*this,iServerEntry,aObserver);
	CleanupStack::PushL(qEntry);
	iRequestArray.AppendL(qEntry);
	CleanupStack::Pop(qEntry);
	}

/**
Adds a request to store a CIMMimeHeader to the queue.
@param aHeader		CImMimeHeader* object to be stored, ownership is taken.
@param aId			The id of the entry under which the object will be stored.
@param aObserver	A reference to a MMailStoreObserver object that will be called back when the request is complete.
@return
*/

EXPORT_C void CImapMailStore::StorePartL(CImMimeHeader* aHeader,TMsvId aId, MMailStoreObserver& aObserver)
	{
	CImMimeHeaderQueueEntry* qEntry=CImMimeHeaderQueueEntry::NewL(aHeader,aId,*this,iServerEntry,aObserver);
	CleanupStack::PushL(qEntry);
	iRequestArray.AppendL(qEntry);
	CleanupStack::Pop(qEntry);
	}
	
/**
Creates a CAttachmentQueueEntry used to store attachments and adds the object to the queue.
@param aTotalChunks	The number of chunks of body data that will be stored.
@param aId			The id of the entry under which the object will be stored.
@param aObserver	A reference to a MMailStoreObserver object that will be called back when the request is complete.
@return
*/

EXPORT_C void CImapMailStore::InitialiseStoreAttachmentL(TInt aTotalChunks,CImapSettings& aImapSettings,CFetchBodyInfo& aFetchBodyInfo,TInt aLogId,MMailStoreObserver& aObserver)
	{
	__ASSERT_DEBUG(aTotalChunks>0,TImapServerPanic::ImapPanic(TImapServerPanic::EMailStoreNoDataChunks));
	__LOG_FORMAT((aLogId, "CImapMailStore::InitialiseStoreAttachmentL total chunks = %d",aTotalChunks));
	CAttachmentQueueEntry* qEntry=CAttachmentQueueEntry::NewL(aTotalChunks,*this,iServerEntry,aImapSettings,aFetchBodyInfo,aLogId,aObserver);
	CleanupStack::PushL(qEntry);
	//make sure that we do not place a CAttachmentQueueEntry on the queue if one aleady exists for this entry.
#if (defined _DEBUG)	
	TIdentityRelation<CQueueEntryBase> relationIdsAndTypeMatch(IdsAndTypeMatch);
	TInt index=iRequestArray.Find(qEntry,relationIdsAndTypeMatch);
	__ASSERT_DEBUG(index==KErrNotFound, TImapServerPanic::ImapPanic(TImapServerPanic::EMailStoreQueueEntryObjectAlreadyExists));		
#endif	
	iRequestArray.AppendL(qEntry);
	CleanupStack::Pop(qEntry);
	}
	
/**
Creates a CreateAttachmentInfoL used to synchronously create information only about an attachment that is not currently being downloaded.
@param aId			The id of the entry under which the data will be stored.
@return
*/

EXPORT_C void CImapMailStore::CreateAttachmentInfoL(TMsvId aId)
	{
	RFile file;
	iServerEntry.SetEntry(aId);
	CAttachmentQueueEntry::CreateAttachmentInfoL(iServerEntry,file);
	file.Close();
	}
	
/**
Creates a CMultiChunkBodyEntry used to store body text and adds the object to the queue.
@param aTotalChunks	The number of chunks of body data that will be stored.
@param aId			The id of the entry under which the object will be stored.
@param aObserver	A reference to a MMailStoreObserver object that will be called back when the request is complete.
@param aBinaryCap   if true, message body is downloaded using FETCH BINARY
					else FETCH BODY is used to download body message
@return
*/

EXPORT_C void CImapMailStore::InitialiseStoreBody16L(TInt aTotalChunks,CImapSettings& aImapSettings,CFetchBodyInfo& aFetchBodyInfo,TInt aLogId,MMailStoreObserver& aObserver, TBool aBinaryCap)
	{
	 __ASSERT_DEBUG(aTotalChunks>0, TImapServerPanic::ImapPanic(TImapServerPanic::EMailStoreNoDataChunks));
	__LOG_FORMAT((aLogId, "CImapMailStore::InitialiseStoreBody16L total chunks = %d",aTotalChunks));
	CBodyQueueEntry16* qEntry=CBodyQueueEntry16::NewL(aTotalChunks,*this,iServerEntry,aImapSettings,aFetchBodyInfo,aLogId,aObserver,aBinaryCap);
	CleanupStack::PushL(qEntry);
	//make sure that we do not place a CBodyQueueEntry16 on the queue if one aleady exists for this entry.
#if (defined _DEBUG)	
	TIdentityRelation<CQueueEntryBase> relationIdsAndTypeMatch(IdsAndTypeMatch);
	TInt index=iRequestArray.Find(qEntry,relationIdsAndTypeMatch);
	__ASSERT_DEBUG(index==KErrNotFound, TImapServerPanic::ImapPanic(TImapServerPanic::EMailStoreQueueEntryObjectAlreadyExists));		
#endif
	iRequestArray.AppendL(qEntry);
	CleanupStack::Pop(qEntry);
	}
	
/**
Creates a CBodyQueueEntry8 used to store body text and adds the object to the queue.
@param aTotalChunks				The number of chunks of body data that will be stored.
@param aId						The id of the entry under which the object will be stored.
@param aCharacterSetIdentifier	The CharacterSet that was used to encode the body text.
@param aObserver				A reference to a MMailStoreObserver object that will be called back when 
								the request is complete.
@param aBinaryCap   if true, message body is downloaded using FETCH BINARY
					else FETCH BODY is used to download body message
@return
*/
	                                            
EXPORT_C void CImapMailStore::InitialiseStoreBody8L(TInt aTotalChunks,CImapSettings& aImapSettings,CFetchBodyInfo& aFetchBodyInfo,TInt aLogId,MMailStoreObserver& aObserver, TBool aBinaryCap)
	{
	__ASSERT_DEBUG(aTotalChunks>0, TImapServerPanic::ImapPanic(TImapServerPanic::EMailStoreNoDataChunks));
	__LOG_FORMAT((aLogId, "CImapMailStore::InitialiseStoreBody8L total chunks = %d",aTotalChunks));
	CBodyQueueEntry8* qEntry=CBodyQueueEntry8::NewL(aTotalChunks,*this,iServerEntry,aImapSettings,aFetchBodyInfo,aLogId,aObserver,aBinaryCap);
	CleanupStack::PushL(qEntry);
	//make sure that we do not place a CBodyQueueEntry8 on the queue if one aleady exists for this entry.
#if (defined _DEBUG)	
	TIdentityRelation<CQueueEntryBase> relationIdsAndTypeMatch(IdsAndTypeMatch);
	TInt index=iRequestArray.Find(qEntry,relationIdsAndTypeMatch);
	__ASSERT_DEBUG(index==KErrNotFound, TImapServerPanic::ImapPanic(TImapServerPanic::EMailStoreQueueEntryObjectAlreadyExists));		
#endif	
	iRequestArray.AppendL(qEntry);
	CleanupStack::Pop(qEntry);
	}

/**
Loops through the queue looking for a CMultiChunkBodyEntry that has been created for the entry. 
If found ownership of the data is transferred to the object. Otherwise asserts.
@param aData		The text to be stored. CImapMailStore takes ownership. 		
@param aId			The id of the entry under which the object will be stored.
@param aChunkNumber	The chunk number, used store the chunk in the correct order.
@return	  			KErrNone if the request was found on the queue, KErrNotFound otherwise.
*/

EXPORT_C void CImapMailStore::StoreBodyChunk16L(HBufC8* aData,TMsvId aId,TInt aChunkNumber, TInt aExtraFetchRequestCount)
	{
	//chunk number starts at 1 for body chunks
	__ASSERT_DEBUG(aChunkNumber>=0, TImapServerPanic::ImapPanic(TImapServerPanic::EMailStoreDataChunkOutOfRange));
	//get the entry on the queue
	TInt count=iRequestArray.Count();
	//loop through the requests on the queue
	TInt i;
	for(i=0;i<count;++i)
		{
		CQueueEntryBase* qEntry=iRequestArray[i];
		//if a CMultiChunkBodyEntry request exists to store body data into this entry 
		if(qEntry->GetEntryType()==CQueueEntryBase::EBody16 && qEntry->GetId()==aId)
			{
			CBodyQueueEntry16* multiChunkBodyEntry=static_cast<CBodyQueueEntry16*>(qEntry);
			//add the chunk to the CMultiChunkBodyEntry object
			multiChunkBodyEntry->AddChunkL(aData,aChunkNumber,aExtraFetchRequestCount);	
			return;
			}
		}
	//if the request was not on the queue then assert
	if(i==count)
		{
		__ASSERT_DEBUG(EFalse,TImapServerPanic::ImapPanic(TImapServerPanic::EMailStoreQueueEntryObjectNotFound));	
		}
	}


/**
Loops through the queue looking for a CBodyQueueEntry8 that has been created for the entry. 
If found ownership of the data is transferred to the object. Otherwise asserts.
@param aData		The text to be stored. CImapMailStore takes ownership. 		
@param aId			The id of the entry under which the object will be stored.
@param aChunkNumber	The chunk number, used store the chunk in the correct order.
@return	  			KErrNone if the request was found on the queue, KErrNotFound otherwise.
*/

EXPORT_C void CImapMailStore::StoreBodyChunk8L(HBufC8* aData,TMsvId aId,TInt aChunkNumber,TInt aExtraFetchRequestCount)
	{
	//chunk number starts at 1 for body chunks
	__ASSERT_DEBUG(aChunkNumber>=0, TImapServerPanic::ImapPanic(TImapServerPanic::EMailStoreDataChunkOutOfRange));
	//get the entry on the queue
	TInt count=iRequestArray.Count();
	//loop through the requests on the queue
	TInt i;
	for(i=0;i<count;++i)
		{
		CQueueEntryBase* qEntry=iRequestArray[i];
		//if a CMultiChunkBodyEntry request exists to store body data into this entry 
		if(qEntry->GetEntryType()==CQueueEntryBase::EBody8 && qEntry->GetId()==aId)
			{
			CBodyQueueEntry8* multiChunkEightBitBodyEntry=static_cast<CBodyQueueEntry8*>(qEntry);
			//add the chunk to the CBodyQueueEntry8 object
			multiChunkEightBitBodyEntry->AddChunkL(aData,aChunkNumber,aExtraFetchRequestCount);	
			return;
			}
		}
	//if the request was not on the queue then assert
	if(i==count)
		{
		__ASSERT_DEBUG(EFalse,TImapServerPanic::ImapPanic(TImapServerPanic::EMailStoreQueueEntryObjectNotFound));
		}
	}
	
	
/**
Loops through the queue looking for a CAttachmentQueueEntry that has been created for the entry. 
If found ownership of the data is transferred to the object. Otherwise asserts.
@param aData		The text to be stored. CImapMailStore takes ownership. 		
@param aId			The id of the entry under which the object will be stored.
@param aChunkNumber	The chunk number, used store the chunk in the correct order.
@return	  			KErrNone if the request was found on the queue, KErrNotFound otherwise.
*/

EXPORT_C void CImapMailStore::StoreAttachmentChunkL(HBufC8* aData,TMsvId aId,TInt aChunkNumber)
	{
	__ASSERT_DEBUG(aChunkNumber>=0,TImapServerPanic::ImapPanic(TImapServerPanic::EMailStoreDataChunkOutOfRange));
	//get the entry on the queue
	TInt count=iRequestArray.Count();
	//loop through the requests on the queue
	TInt i;
	for(i=0;i<count;++i)
		{
		CQueueEntryBase* qEntry=iRequestArray[i];
		//if a CMultiChunkBodyEntry request exists to store body data into this entry 
		if(qEntry->GetEntryType()==CQueueEntryBase::EAttachment && qEntry->GetId()==aId)
			{
			CAttachmentQueueEntry* multiChunkAttachmentEntry=static_cast<CAttachmentQueueEntry*>(qEntry);
			//add the chunk to the CMultiChunkBodyEntry object
			multiChunkAttachmentEntry->AddChunkL(aData,aChunkNumber);	
			return;
			}
		}
	//if the request was not on the queue then assert
	if(i==count)
		{
		__ASSERT_DEBUG(EFalse,TImapServerPanic::ImapPanic(TImapServerPanic::EMailStoreQueueEntryObjectNotFound));
		}
	}

/**
@param aObserver	The observer for the entry for which all outstanding requests will be cancelled .
@return	  			KErrNone if one or more matching request were found on the queue, KErrNotFound otherwise.
*/

EXPORT_C TInt CImapMailStore::CancelRequest(MMailStoreObserver& aObserver)
	{
	TInt err=KErrNotFound;	
	//loop through the requests on the queue 
	for(TInt i=iRequestArray.Count()-1;i>=0;--i)
		{
		CQueueEntryBase* qEntry=iRequestArray[i];
		//if a request matches the observer specified then remove it from the queue.
		if(&(qEntry->GetObserver())==&aObserver)
			{
			qEntry->Cancel();
			}
		}
		
	for(TInt i=iRequestArray.Count()-1;i>=0;--i)
		{
		CQueueEntryBase* qEntry=iRequestArray[i];
		//if a request matches the observer specified then remove it from the queue.
		if(&(qEntry->GetObserver())==&aObserver)
			{
			qEntry->CancelRequest();
			//remove the request from the queue and delete it
			iRequestArray.Remove(i);
			delete qEntry;	
			err=KErrNone;
			}
		}
		
		return err;
	}
	
/**
@param aId	The TMsvId of the entry for which all outstanding requests will be cancelled .
@return	  	KErrNone if one or more matching request were found on the queue, KErrNotFound otherwise.
*/

EXPORT_C TInt CImapMailStore::CancelRequest(TMsvId aId)
	{
	TInt err=KErrNotFound;
	
	//loop through the requests on the queue 
	for(TInt i=iRequestArray.Count()-1;i>=0;--i)
		{
		CQueueEntryBase* qEntry=iRequestArray[i];
		//if a request matches the TMsvId specified then remove it from the queue.
		if(qEntry->GetId()==aId)
			{
			qEntry->Cancel();
			}
		}
	//loop through the requests on the queue 
	for(TInt i=iRequestArray.Count()-1;i>=0;--i)
		{
		CQueueEntryBase* qEntry=iRequestArray[i];
		//if a request matches the TMsvId specified then remove it from the queue.
		if(qEntry->GetId()==aId)
			{
			qEntry->CancelRequest();
			//remove the request from the queue and delete it
			iRequestArray.Remove(i);
			delete qEntry;	
			err=KErrNone;
			}
		}
		
		return err;
	}

/**
Creates a CMsvPlainBodyTextQueueEntry used to store body text and adds the object to the queue.
@param aTotalChunks		The number of chunks of body data that will be stored.
@param aImapSettings 	The CImapSettings object.
@param aFetchBodyInfo 	The CFetchBodyInfo object.
@param aId				The id of the entry under which the object will be stored.
@param aObserver		A reference to a MMailStoreObserver object that will be called back when the request is complete.
@param aChunkObserver 	A reference to a MChunkOutOfOrderObserver that will be called back when a proper chunk is 
					    recieved from server if a out of order chunk was recieved in previous FETCH response.
@param aBinaryCap   if true, message body is downloaded using FETCH BINARY
					else FETCH BODY is used to download body message
@return
*/

EXPORT_C void CImapMailStore::InitialiseStorePlainBodyTextL(TInt aTotalChunks, CImapSettings& aImapSettings, CFetchBodyInfo& aFetchBodyInfo, TInt aLogId, MMailStoreObserver& aObserver, MChunkOutOfOrderObserver& aChunkObserver, TBool aBinaryCap)
	{
	 __ASSERT_DEBUG(aTotalChunks>0, TImapServerPanic::ImapPanic(TImapServerPanic::EMailStoreNoDataChunks));

	__LOG_FORMAT((aLogId, "CImapMailStore::InitialiseStorePlainBodyTextL total chunks = %d",aTotalChunks));

	CMsvPlainBodyTextQueueEntry* qEntry=CMsvPlainBodyTextQueueEntry::NewL(aTotalChunks,*this,iServerEntry,aImapSettings,aFetchBodyInfo,aLogId,aObserver,aChunkObserver,aBinaryCap);
	CleanupStack::PushL(qEntry);
	
	//make sure that we do not place a CMsvPlainBodyTextQueueEntry on the queue if one aleady exists for this entry.
#if (defined _DEBUG)
	TIdentityRelation<CQueueEntryBase> relationIdsAndTypeMatch(IdsAndTypeMatch);
	TInt index=iRequestArray.Find(qEntry,relationIdsAndTypeMatch);
	__ASSERT_DEBUG(index==KErrNotFound, TImapServerPanic::ImapPanic(TImapServerPanic::EMailStoreQueueEntryObjectAlreadyExists));		
#endif
	iRequestArray.AppendL(qEntry);
	CleanupStack::Pop(qEntry);
	}

/**
Loops through the queue looking for a CMultiChunkBodyEntry that has been created for the entry. 
If found ownership of the data is transferred to the object. Otherwise asserts.
@param aChunkNumber	The chunk number, used store the chunk in the correct order.
@param aId			The id of the entry under which the object will be stored.
@param aData		The text to be stored. CImapMailStore takes ownership. 
@return	  			ETrue if the chunk was recieved in order.
					EFalse if the chunks were recieved out of order and the chunk storing array exceeded limit(3 chunks default).
*/	
EXPORT_C TBool CImapMailStore::StorePlainBodyTextL(HBufC8* aData, TMsvId aId, TInt aChunkNumber, TInt aExtraFetchRequestCount)
	{
	 //chunk number starts at 1 for body chunks
	__ASSERT_DEBUG(aChunkNumber>=0, TImapServerPanic::ImapPanic(TImapServerPanic::EMailStoreDataChunkOutOfRange));
	//get the entry on the queue
	TInt count=iRequestArray.Count();
	//loop through the requests on the queue
	TBool sendCommand = ETrue;
	TInt i;
	for(i=0;i<count;++i)
		{
		CQueueEntryBase* qEntry=iRequestArray[i];
		//if a CMultiChunkBodyEntry request exists to store body data into this entry 
		if(qEntry->GetEntryType()==CQueueEntryBase::EPlainTextBody && qEntry->GetId()==aId)
			{
			CMsvPlainBodyTextQueueEntry* multiChunkBodyEntry=static_cast<CMsvPlainBodyTextQueueEntry*>(qEntry);
			//add the chunk to the CMultiChunkBodyEntry object
		
			sendCommand = multiChunkBodyEntry->AddChunkL(aData,aChunkNumber,aExtraFetchRequestCount);	
			return sendCommand;
			}
		}
	//if the request was not on the queue then assert
	if(i==count)
		{
		__ASSERT_DEBUG(EFalse,TImapServerPanic::ImapPanic(TImapServerPanic::EMailStoreQueueEntryObjectNotFound));	
		}
	return sendCommand;
	}
