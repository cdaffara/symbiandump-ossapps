// CMSVPLAINBODYTEXTENTRY.H

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

#ifndef __CMSVPLAINBODYTEXTENTRY_H__
#define __CMSVPLAINBODYTEXTENTRY_H__

#include <e32base.h>

class CMsvPlainBodyText;
class CMsvServerEntry;
class CMsvStore;

/** 
Size of buffer to temporarily hold the data passed from CImRecvConvert.CImRecvConvert
recieves data of size less than or equal to 1K from the socket.
*/
const TInt KPlainBodyTextCacheSizeBytes = 2002;


/**
Used for handling the asynchronous storage of plainbody text chunks.
@internalTechnology
@released
*/	
class CMsvPlainBodyTextEntry : public CBase
	{
public:
	static CMsvPlainBodyTextEntry* NewL(TInt aIs8Bit, CMsvServerEntry& aServerEntry, TUint aCharset, TUint aDefaultCharset);
	~CMsvPlainBodyTextEntry();
	
	void AddChunkL(const TDesC8& aLineOfData);
	void TryCommitL();

private:
	CMsvPlainBodyTextEntry();
	void ConstructL(TInt aIs8Bit, CMsvServerEntry& aServerEntry, TUint aCharset, TUint aDefaultCharset);

private:
	CMsvStore* 				iStore;
	/** 
	Cache to store incoming chunks.
	*/
	HBufC8*					iPlainBodyTextCache;
	/**
	The store class for handling chunk storage.
	*/
	CMsvPlainBodyText*		iPlainBodyText;
	/**
	Enum to indicate whether data has been committed to store.
	*/
	enum TCommitData
		{
		ENoData,
		EUnCommittedData,
		ECommittedData
		} iCommittedData;
	};
	
#endif//__CMSVPLAINBODYTEXTENTRY_H__
