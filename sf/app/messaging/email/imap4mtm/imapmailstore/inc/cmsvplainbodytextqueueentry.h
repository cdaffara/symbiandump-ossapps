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
//

#ifndef __CMSVPLAINBODYTEXTQUEUEENTRY_H__
#define __CMSVPLAINBODYTEXTQUEUEENTRY_H__

#include "cqueueentrybase.h"
#include "cstoreutilities.h"

class CMsvPlainBodyText;

/**
Used to hold chunk info when sorting chunks into the correct order.
@internalComponent
@prototype
*/
class CPlainBodyChunkInfo : public CBase
	{
public:
	static CPlainBodyChunkInfo* NewL(TInt aChunkNumber,HBufC8* aData);
	~CPlainBodyChunkInfo();
private:
	CPlainBodyChunkInfo(TInt aChunkNumber,HBufC8* aData);
	void ConstructL();
public:
	TInt iChunkNumber;
	HBufC8* iData;	
	};

/**
Used for requests to store body text.
@internalComponent
@prototype
*/
class CMsvPlainBodyTextQueueEntry : public CQueueEntryBase
	{
public:                            
	static CMsvPlainBodyTextQueueEntry* NewL(TInt aTotalChunks,CImapMailStore& aParent,CMsvServerEntry& aServerEntry,CImapSettings& aImapSettings,CFetchBodyInfo& aFetchBodyInfo,TInt aLogId,MMailStoreObserver& aObserver,MChunkOutOfOrderObserver& aChunkObserver, TBool aBinaryCap);
	~CMsvPlainBodyTextQueueEntry();
	TBool AddChunkL(HBufC8* aData,TInt aChunkNumber, TInt aExtraFetchRequestCount);
protected:
	virtual void RunL();
	virtual TInt RunError(TInt aError);
private:
	void ConstructL();
	CMsvPlainBodyTextQueueEntry(TInt aTotalChunks,CImapMailStore& aParent,CMsvServerEntry& aServerEntry,CImapSettings& aImapSettings,CFetchBodyInfo& aFetchBodyInfo,TInt aLogId,MMailStoreObserver& aObserver,MChunkOutOfOrderObserver& aChunkObserver, TBool aBinaryCap);
	static TInt CompareChunks(const CPlainBodyChunkInfo& aChunkBodyInfo1, const CPlainBodyChunkInfo& aChunkBodyInfo2);
private:
	/** Total no. of chunks to be fetched.*/
	TInt 								iTotalChunks;
	TInt 								iNextExpectedChunk;
	TImEncodingType 					iEncoding;
	/** The charsetid for the message being fetched.*/
	TUint 								iCharsetId;
	/** For a partial fetch this is the amount of data not being fetched */
	TInt32  							iBodyPartRemainingSize;
	/** Array to store the incoming chunk before it is written to message store.*/
	RPointerArray<CPlainBodyChunkInfo> 	iDataArray;
	/** Utility class for decoding the incoming data.*/
	CStoreUtilities* 					iStoreUtilities;
	CImapSettings& 						iImapSettings;
	TInt 								iLogId;
	/** The message store class to store received chunks.*/
	CMsvPlainBodyText* 					iPlainBodyText;
	CMsvStore* 							iStore;
	/** Observer to handle the receival of out of order chunks.*/
	MChunkOutOfOrderObserver& 			iChunkObserver;
	/** Indicates if the chunk is written to store so that it can be removed from the buffer array.*/
	TBool 								iReadyToRemoveChunk;
	/** Buffer which contains the chunk */
	HBufC8* 				iDecodedData;
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)		
	/** Flag to check if BINARY capability is set */
	TBool iBinaryCap;
	/** For message body download using Binary Fetch, this value indicates the extra Fetch Request given
	which is not required.
	*/
	TInt iExtraFetchRequestCount;
#endif
	};
	
#endif//_CMSVPLAINBODYTEXTQUEUEENTRY_H__
