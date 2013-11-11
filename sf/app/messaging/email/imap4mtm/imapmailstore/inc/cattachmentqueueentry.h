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
	
#ifndef __CATTACHMENTQUEUEENTRY_H__
#define __CATTACHMENTQUEUEENTRY_H__

#include "cqueueentrybase.h"
#include "cstoreutilities.h"

/**
Used to hold chunk info when sorting chunks into the correct order.
@internalComponent
@prototype
*/
class CChunkAttachmentInfo : public CBase
	{
friend class CAttachmentQueueEntry;
public:
	CChunkAttachmentInfo(TInt aChunkNumber,HBufC8* aData);
	~CChunkAttachmentInfo();
private:
	TInt iChunkNumber;
	HBufC8* iData;	
	};

/**
Used for requests to store attachments.
@internalComponent
@prototype
*/
class CAttachmentQueueEntry : public CQueueEntryBase
	{
public:
	static CAttachmentQueueEntry* NewL(TInt aTotalChunks,CImapMailStore& aParent,CMsvServerEntry& aServerEntry,CImapSettings& aImapSettings,CFetchBodyInfo& aFetchBodyInfo,TInt aLogId,MMailStoreObserver& aObserver);
	~CAttachmentQueueEntry();
	void AddChunkL(HBufC8* aData,TInt aChunkNumber);
	void CreateAttachmentL(CMsvServerEntry& iServerEntry);
	static void CreateAttachmentInfoL(CMsvServerEntry& aServerEntry,RFile& aFile);
	void CancelRequestL();
	void CancelRequest();
protected:
	void DoCancel();
	virtual void RunL();
	virtual TInt RunError(TInt aError);
private:
	void ConstructL();
	CAttachmentQueueEntry(TInt aTotalChunks,CImapMailStore& aParent,CMsvServerEntry& aServerEntry,CImapSettings& aImapSettings,CFetchBodyInfo& aFetchBodyInfo,TInt aLogId,MMailStoreObserver& aObserver);
	static TInt CompareChunks(const CChunkAttachmentInfo& aChunkAttachmentInfo1, const CChunkAttachmentInfo& aChunkAttachmentInfo2);
private:
	TInt iTotalChunks;
	TImEncodingType iEncoding;
	TInt iNextExpectedChunk;
	RFile iFile;
	TBool iReadyToRemoveChunk;
	RPointerArray<CChunkAttachmentInfo> iDataArray;
	CStoreUtilities* iStoreUtilities;
	CImapSettings& iImapSettings;
	//Content Access Framework Wrapper class, not owned by this class.
	CImCaf* iCaf;
	HBufC8* iDecodedData;
	TInt iLogId;
	};


#endif // __CATTACHMENTQUEUEENTRY_H__


