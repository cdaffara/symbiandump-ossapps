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

#include "cimapsearch.h"
#include "moutputstream.h"
#include "cimapsessionconsts.h"
#include "imappaniccodes.h"

// IMAP SEARCH command
_LIT8(KCommandSearch, "%d UID SEARCH %S\r\n");

CImapSearch* CImapSearch::NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId, const TDesC8& aSearchCriteria, RArray<TUint>& aMatchingMessageIds)
// static method first phase construction
	{
	CImapSearch* self = new (ELeave) CImapSearch(aSelectedFolderData, aLogId, aMatchingMessageIds);
	CleanupStack::PushL(self);
	self->ConstructL(aSearchCriteria);
	CleanupStack::Pop();
	return self;
	}

CImapSearch::CImapSearch(CImapFolderInfo* aSelectedFolderData, TInt aLogId, RArray<TUint>& aMatchingMessageIds)	
	: CImapCommandEx(aSelectedFolderData, aLogId)
	, iMatchingMessageIds(aMatchingMessageIds)
	{
	
	}

void CImapSearch::ConstructL(const TDesC8& aSearchCriteria)
	{
	iSearchCriteria = aSearchCriteria.AllocL();
	}

CImapSearch::~CImapSearch()
	{
	delete iSearchCriteria;
	}

/**
Formats and sends the IMAP SEARCH command.
@param aTagId Command sequence id which will be send along with the IMAP command.
@param aStream A wrapper for the outbound stream of a connected socket, using which
the command will be send to the server
*/
void CImapSearch::SendMessageL(TInt aTagId, MOutputStream& aStream)
	{
	// From RFC3501
	//	search = "SEARCH" [SP "CHARSET" SP astring] 1*(SP search-key)
	//
	// we have chosen not to send teh optional "CHARSET", which means we
	// don't need to check for a BADCHARSET response text code in the resposne.
	
	iTagId = aTagId;

	TInt bufLength = KCommandSearch().Length();
	bufLength += TagLength(aTagId);
	bufLength += iSearchCriteria->Length();
	
	__ASSERT_DEBUG(iOutputBuffer==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandOutputBufferNotNull));
	iOutputBuffer = HBufC8::NewL(bufLength);
	iOutputBuffer->Des().Format(KCommandSearch, aTagId, iSearchCriteria);

	//send the command to the server
	aStream.SendDataReq(*iOutputBuffer);
	}

/**
Checks if this is a SEARCH response.  If it is, then it is parsed.
@return ECompleteUntagged if the line is an untagged SEARCH response.
		ENotRecognised for any other response (i.e. one that is not known about by this command)
*/
CImapCommand::TParseBlockResult CImapSearch::ParseUntaggedResponseL()
	{	
	TParseBlockResult result = ENotRecognised;
	
	// Is this a search response?
	TPtrC8 part = GetNextPart(); // returns KNullDesC8 if there is no part available
	if(part.CompareF(KImapTxtSearch) == 0)
		{
		ParseSearchResponseL();
		result = ECompleteUntagged;
		}
	
	return result;
	}

/**
Parses the message uids from a previously identified untagged search response.
*/
void CImapSearch::ParseSearchResponseL()
	{
	// From RFC3501 section 9 (edited)
	//
	// mailbox-data = "SEARCH" *(SP nz-number)
	//
	// "SEARCH" has already been found, so the remainder of response 
	// should be space-separated numbers representing UIDS of messages.
	
	RDesParts messageUidList;
	CleanupClosePushL(messageUidList);

	GetDelimitedPartsL(' ', iUnparsedData, messageUidList);
		
	TUint messageUid = 0;
	
	TInt countMessageUids = messageUidList.Count(); 	 
	for(TInt i = 0; i < countMessageUids; ++i)
		{
		TLex8 desToInt(messageUidList[i]);
		
		TInt err = desToInt.Val(messageUid);
		if (err != KErrNone)
			{
			CorruptDataL();
			}
			
		iMatchingMessageIds.Append(messageUid);
		}
				
	CleanupStack::PopAndDestroy(&messageUidList);
	}
