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

#ifndef __CBODYQUEUEENTRY16_H__
#define __CBODYQUEUEENTRY16_H__

#include "cqueueentrybase.h"
#include "cstoreutilities.h"

class CParaFormatLayer;
class CCharFormatLayer;

/**
Used to hold chunk info when sorting chunks into the correct order.
@internalComponent
@prototype
*/
class CBodyChunk16Info : public CBase
	{
public:
	CBodyChunk16Info(TInt aChunkNumber,HBufC8* aData);
	~CBodyChunk16Info();
public:
	TInt iChunkNumber;
	HBufC8* iData;	
	};

/**
Used for requests to store body text.
@internalComponent
@prototype
*/
class CBodyQueueEntry16 : public CQueueEntryBase
	{
public:                            
	static CBodyQueueEntry16* NewL(TInt aTotalChunks,CImapMailStore& aParent,CMsvServerEntry& aServerEntry,CImapSettings& aImapSettings,CFetchBodyInfo& aFetchBodyInfo,TInt aLogId,MMailStoreObserver& aObserver, TBool aBinaryCap);
	~CBodyQueueEntry16();
	void AddChunkL(HBufC8* aData,TInt aChunkNumber, TInt aExtraFetchRequestCount);
protected:
	virtual void RunL();
	virtual TInt RunError(TInt aError);
private:
	void ConstructL();
	CBodyQueueEntry16(TInt aTotalChunks,CImapMailStore& aParent,CMsvServerEntry& aServerEntry,CImapSettings& aImapSettings,CFetchBodyInfo& aFetchBodyInfo,TInt aLogId,MMailStoreObserver& aObserver, TBool aBinaryCap);
	static TInt CompareChunks(const CBodyChunk16Info& aChunkBodyInfo1, const CBodyChunk16Info& aChunkBodyInfo2);
private:	
	CRichText* iBodyText;
	CParaFormatLayer* iParaFormatLayer;
	CCharFormatLayer* iCharFormatLayer;
	TInt iTotalChunks;
	TInt iNextExpectedChunk;
	TImEncodingType iEncoding;
	TUint iCharsetId;
	//for a partial fetch this is the amount of data not being fetched
	TInt32  iBodyPartRemainingSize;
	RPointerArray<CBodyChunk16Info> iDataArray;
	CStoreUtilities* iStoreUtilities;
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
	
#endif// _CBODYQUEUEENTRY16_H__