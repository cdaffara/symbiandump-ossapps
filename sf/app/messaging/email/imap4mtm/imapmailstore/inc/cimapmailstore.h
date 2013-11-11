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

#ifndef __CIMAPMAILSTORE_H__
#define __CIMAPMAILSTORE_H__

#define _MSVAPI_DONT_INCLUDE_FLOGGER_

#include <msvapi.h>
#include <msventry.h>
#include <miuthdr.h>
#include "cimapsettings.h"
#include "cfetchbodyinfo.h"

//Forward declarations
class CQueueEntryBase;
class CImHeader;
class CImMimeHeader;
class CImapCharconvTls;

/**
	Created to enable a session to be created with the messaging framework. Observer not used.
	@internalComponent
	@prototype
*/
class CSessionObserver :  public CBase, public MMsvSessionObserver
	{
public:
	void HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
		{			
		}
	};

/**
Observer class used by CQueueEntryBase to call back to into the caller with the result of a 
storage operation.
@internalComponent
@prototype
*/	
class MMailStoreObserver
	{
public:
	//pure virtual member
	virtual void StoreOperationComplete(TMsvId aId,TInt aErrorCode)=0;
	};

/**
Observer class used by CQueueEntryBase to call back to into the caller when proper chunk is recieved fron server
and the chunks are written to the store in order.
@internalComponent
@prototype
*/
class MChunkOutOfOrderObserver
	{
public:
	/**
	Notify CImapFetchBody to send FETCH command to server, if it was disabled because the client recieved
	out of order chunks and it reached it maximum buffer capacity(default 3 chunks).
	*/
	virtual void EnableSendFetch()=0;
	};


/**
This class provides a public API which implements a wrapper over CMsvStosre and allows clients to make requests to stream data to the message store. 
The class maintains an array of CQueueEntryBase derived objects that service the requests. 
@internalComponent
@prototype
*/
class CImapMailStore :  public CBase
	{
public:
	virtual ~CImapMailStore();
	IMPORT_C static CImapMailStore* NewL(CMsvServerEntry& aServerEntry);
	IMPORT_C void StorePartL(CImHeader* aHeader,TMsvId aId,MMailStoreObserver& aObserver);
	IMPORT_C void StorePartL(CImMimeHeader* aHeader,TMsvId aId, MMailStoreObserver& aObserver);
	IMPORT_C void InitialiseStoreBody16L(TInt aTotalChunks, CImapSettings& aImapSettings,CFetchBodyInfo& aFetchBodyInfo,TInt aLogId,MMailStoreObserver& aObserver, TBool aBinaryCap=EFalse);
	IMPORT_C void InitialiseStoreBody8L(TInt aTotalChunks, CImapSettings& aImapSettings,CFetchBodyInfo& aFetchBodyInfo,TInt aLogId,MMailStoreObserver& aObserver, TBool aBinaryCap=EFalse);	
	IMPORT_C void StoreBodyChunk16L(HBufC8* aData,TMsvId aId,TInt aChunkNumber, TInt aExtraFetchRequestCount=0);
	IMPORT_C void StoreBodyChunk8L(HBufC8* aData,TMsvId aId,TInt aChunkNumber,  TInt aExtraFetchRequestCount=0);
	IMPORT_C void InitialiseStoreAttachmentL(TInt aTotalChunks,CImapSettings& aImapSettings,CFetchBodyInfo& aFetchBodyInfo,TInt aLogId,MMailStoreObserver& aObserver);
	IMPORT_C void StoreAttachmentChunkL(HBufC8* aData,TMsvId aId,TInt aChunkNumber);
	IMPORT_C TInt CancelRequest(MMailStoreObserver& aObserver);
	IMPORT_C TInt CancelRequest(TMsvId aId);
	IMPORT_C TInt RequestCount()const;
	IMPORT_C void CreateAttachmentInfoL(TMsvId aId);
	IMPORT_C void InitialiseStorePlainBodyTextL(TInt aTotalChunks, CImapSettings& aImapSettings, CFetchBodyInfo& aFetchBodyInfo, TInt aLogId, MMailStoreObserver& aObserver, MChunkOutOfOrderObserver& aChunkObserver, TBool aBinaryCap=EFalse);
	IMPORT_C TBool StorePlainBodyTextL(HBufC8* aData, TMsvId aId, TInt aChunkNumber, TInt aExtraFetchRequestCount=0);	
	void RemoveFromQueueAndDelete(CQueueEntryBase* aQueueEntry); 
private:
	void ConstructL();
	CImapMailStore(CMsvServerEntry& aServerEntry);
	static TBool IdsAndTypeMatch(const CQueueEntryBase& aLeft,const CQueueEntryBase& aRight);
private:
	RPointerArray<CQueueEntryBase> iRequestArray;
	CMsvServerEntry& iServerEntry;
	};
	
#endif//__CIMAPMAILSTORE_H__