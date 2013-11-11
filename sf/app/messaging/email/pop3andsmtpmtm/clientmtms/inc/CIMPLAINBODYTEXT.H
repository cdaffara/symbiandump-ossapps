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

#ifndef _CIMPLAINBODYTEXT_H_
#define _CIMPLAINBODYTEXT_H_

class CImEmailMessage;
class CImStoreMessagePart;
class CMsgActive;
/**
This class is a wrapper over CMsvPlainBodyText class of message server.
The pointer to this class can be obtained by calling OpenPlainBodyTextForReadL 
or OpenPlainBodyTextForWriteL API of the CImEmailMessage class.
This can be used to store/restore a email message created by using the 
CImEmailOperation class.

@see CImEmailMessage, CImEmailOperation
@publishedAll
@released
*/

class CImPlainBodyText: public CMsgActive
	{
public:
	static CImPlainBodyText* NewL(CImEmailMessage& aEmailMessage, CMsvEntry& aMsvEntry, CImEmailMessage::TImEmailEntryType aEntryType, TInt aChunkLength, TBool aEditMode);
	IMPORT_C ~CImPlainBodyText();
	
	IMPORT_C void StoreChunkL(const TDesC8& aChunk, TRequestStatus& aStatus);
	IMPORT_C void StoreChunkL(const TDesC8& aChunk);
	
	IMPORT_C void StoreChunkL(const TDesC16& aChunk, TRequestStatus& aStatus);
	IMPORT_C void StoreChunkL(const TDesC16& aChunk);
		
	IMPORT_C void StoreRichTextAsPlainTextL(CRichText& aRichText);
	
	IMPORT_C void CommitL(TRequestStatus& aStatus);
	IMPORT_C void CommitL(TRequestStatus& aStatus, CImMimeHeader& aMimeHeader);
	
	IMPORT_C void NextChunkL(TDes8& aChunk, TRequestStatus& aStatus);
	IMPORT_C void NextChunkL(TDes8& aChunk);
	
	IMPORT_C void NextChunkL(TDes16& aChunk, TRequestStatus& aStatus);		
	IMPORT_C void NextChunkL(TDes16& aChunk);
	
	IMPORT_C void PreviousChunkL(TDes8& aChunk, TRequestStatus& aStatus);
	IMPORT_C void PreviousChunkL(TDes8& aChunk);
		
	IMPORT_C void PreviousChunkL(TDes16& aChunk, TRequestStatus& aStatus);
	IMPORT_C void PreviousChunkL(TDes16& aChunk);
	
private:
	CImPlainBodyText(CImEmailMessage& aEmailMessage, CMsvEntry& aMsvEntry);
	void ConstructL(TInt aChunkLength, CImEmailMessage::TImEmailEntryType aEntryType, TBool aEditMode);
	void DoRunL(); // From CMsgActive
	void DoCancel(); 
private:
	CImEmailMessage& 		iEmailMessage;
	CImStoreMessagePart* 	iStoreMessagePart;
	/** The CMsvEntry associated with the message being stored/restored.*/
	CMsvEntry& 				iMsvEntry;
	/** The array of  framework/server class to store/restore body text in chunks.*/
	RPointerArray<CMsvPlainBodyText> iPlainTextArray;
	/** The id of the message. */
	TMsvId 					iMessageId;
	/** The Store associated with the message being stored/restored.*/
	CMsvStore* 				iStore;
	/** For indexing the plaintext array */
	TInt 					iIndex;
	/** The 16-bit pointer to a chunk that will be populated with data when asynchronous
		16-bit versions of NextChunkL / PreviousChunkL returns.*/
	TDes16*					iChunk16;
	/** The 8-bit pointer to a chunk that will be populated with data when asynchronous
		8-bit versions of NextChunkL / PreviousChunkL returns.*/
	TDes8*					iChunk8;
	
	/** Enum indicating which asynchronous chunk retrieving method resulted in a 
	   	call to DoRunl.*/
	enum TChunkRetrievalState 
		{
		EIdleState,
		ENextChunk8Bit,
		ENextChunk16Bit,
		EPreviousChunk8Bit,
		EPreviousChunk16Bit,
		} iChunkRetrievalState;
	};

#endif //_CIMPLAINBODYTEXT_H_


