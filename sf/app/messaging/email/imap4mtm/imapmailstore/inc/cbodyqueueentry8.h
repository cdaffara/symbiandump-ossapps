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

#ifndef _CBODYQUEUEENTRY8_H__
#define _CBODYQUEUEENTRY8_H__

#include <miuthdr.h>
#include "cqueueentrybase.h"
#include "cstoreutilities.h"

class CBufBase;
class CMsvBodyText;
/**
Used to hold chunk info when sorting chunks into the correct order.
@internalComponent
@prototype
*/
class CBodyChunk8Info : public CBase
	{
public:
	CBodyChunk8Info(TInt aChunkNumber,HBufC8* aData);
	~CBodyChunk8Info();
public:
	TInt iChunkNumber;
	HBufC8* iData;	
	};

/**
Used for requests to store body text.
@internalComponent
@prototype
*/
class CBodyQueueEntry8 : public CQueueEntryBase
	{
public:		
	static CBodyQueueEntry8* NewL(TInt aTotalChunks,CImapMailStore& aParent,CMsvServerEntry& aServerEntry,CImapSettings& aImapSettings,CFetchBodyInfo& aFetchBodyInfo,TInt aLogId,MMailStoreObserver& aObserver, TBool aBinaryCap);
	~CBodyQueueEntry8();
	void AddChunkL(HBufC8* aData,TInt aChunkNumber, TInt aExtraFetchRequestCount);
protected:
	virtual void RunL();
	virtual TInt RunError(TInt aError);
private:
	void ConstructL();
	CBodyQueueEntry8(TInt aTotalChunks,CImapMailStore& aParent,CMsvServerEntry& aServerEntry,CImapSettings& aImapSettings,CFetchBodyInfo& aFetchBodyInfo,TInt aLogId,MMailStoreObserver& aObserver, TBool aBinaryCap);
	static TInt CompareChunks(const CBodyChunk8Info& aChunkBodyInfo1,const CBodyChunk8Info& aChunkBodyInfo2);
private:	
	CBufBase* iBodyBuf;
	CMsvBodyText* iBodyText;
	TInt iTotalChunks;
	TInt iNextExpectedChunk;
	TImEncodingType iEncoding;
	TUint iCharsetId;
	//for a partial fetch this is the amount of data not being fetched
	TInt32  iBodyPartRemainingSize;
	RPointerArray<CBodyChunk8Info> iDataArray;
	CStoreUtilities* iStoreUtilities;
	CImConvertCharconv*	iCharConv;
	CCnvCharacterSetConverter* iCharacterConverter;
	CImapSettings& iImapSettings;
	TInt iLogId;
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)		
	/** Flag to check if BINARY capability is set */
	TBool iBinaryCap;
	/** For message body download using Binary Fetch, this value indicates the extra Fetch Request given
	which is not required.
	*/
	TInt iExtraFetchRequestCount;
#endif
	};
	
#endif// _CBODYQUEUEENTRY8_H__