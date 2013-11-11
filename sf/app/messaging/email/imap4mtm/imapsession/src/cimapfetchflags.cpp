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

#include "cimapfetchflags.h"

#include "cimapsessionconsts.h"
#include "moutputstream.h"
#include "imappaniccodes.h"
#include "cimaplogger.h"

_LIT8(KCommandFetchFlags, "%d UID FETCH %S (UID FLAGS)\r\n");

CImapFetchFlags*  CImapFetchFlags::NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId, const TDesC8& aSequenceSet, RArrayMessageFlagInfo& aOutMessageFlagInfo)
	{
	CImapFetchFlags* self = new (ELeave) CImapFetchFlags(aSelectedFolderData, aLogId, aOutMessageFlagInfo);
	CleanupStack::PushL(self);
	self->ConstructL(aSequenceSet);
	CleanupStack::Pop(self);
	return self;
	}
		
CImapFetchFlags::CImapFetchFlags(CImapFolderInfo* aSelectedFolderData, TInt aLogId, RArrayMessageFlagInfo& aOutMessageFlagInfo) 
	: CImapCommandEx(aSelectedFolderData, aLogId)
	, iOutMessageFlagInfo(aOutMessageFlagInfo)
	{
	}
	
CImapFetchFlags::~CImapFetchFlags()
	{
	delete iSequenceSet;
	}
	
void CImapFetchFlags::ConstructL(const TDesC8& aSequenceSet)
	{
	iSequenceSet = aSequenceSet.AllocL();
	}
/** 
Formats and sends the IMAP FETCH FLAGS command.
@param aTagId Command sequence id which will be send along with the IMAP command.
@param aStream A wrapper for the outbound stream of a connected socket, using which 
the command will be send to the server
*/	
void CImapFetchFlags::SendMessageL(TInt aTagId, MOutputStream& aStream)
	{
	iTagId = aTagId;
	
	TInt bufLength = KCommandFetchFlags().Length();
	bufLength += TagLength(aTagId);
	bufLength += iSequenceSet->Length();
	
	__ASSERT_DEBUG(iOutputBuffer==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandOutputBufferNotNull));
	iOutputBuffer = HBufC8::NewL(bufLength);	
	iOutputBuffer->Des().Format(KCommandFetchFlags, aTagId, iSequenceSet);	
	
	//send the command to the server
	aStream.SendDataReq(*iOutputBuffer);		
	}

/**
Called after a line of untagged response data has been received. 
@param	aLine	The line to be parsed.
@return ECompleteUntagged if the line is a recognised untagged response
		ENotRecognised for any other response 
*/
CImapCommand::TParseBlockResult CImapFetchFlags::ParseUntaggedResponseL()
	{
	TParseBlockResult result = ENotRecognised;
	
	// Is this a fetch flags response?
	// Check for Sequence Number followed by "FETCH"
	
	TPtrC8 part1 = GetNextPart(); // returns KNullDesC8 if there is no part available
	TPtrC8 part2 = GetNextPart(); // returns KNullDesC8 if there is no part available
	
	// Is part1 a Sequence Number?
	TInt sequenceNumber = 0;
	TLex8 lex(part1);
	if (lex.Val(sequenceNumber) == KErrNone)
		{
		// part1 is a Sequence Number.  Now check part2 - is it "FETCH"?

		if(part2.CompareF(KImapTxtFetch) == 0)
			{
			ProcessFetchFlagsResponseL();
			result = ECompleteUntagged;
			}
		}
	
	// If ProcessFetchFlagsResponseL() is not called then result remains ENotRecognised			
	return result;
	}
	
void CImapFetchFlags::ProcessFetchFlagsResponseL()
	{
	// Each untagged response is of the form 
	// 		* 23 FETCH (FLAGS (\Seen) UID 4827313)
	// Where 23 is the message sequence number.  Discard this.
	// And 4827313 is the message uid.  Keep this.
	//
	
	TMessageFlagInfo messageFlagInfoEntry;

	// Strip the opening and closing brackets
	TInt remainderLength = iUnparsedData.Length();
	if (remainderLength < 2)
		{
		// expecting at least '(' and ')'
		CorruptDataL();
		}
	if (iUnparsedData[0] != '(' || iUnparsedData[remainderLength-1] != ')')
		{
		CorruptDataL();
		}
		
	iUnparsedData.Set(iUnparsedData.Mid(1, remainderLength-2));
	
	// The UID FETCH and UID STORE commands expect responses to contain 
	// both FLAGS and UID data items.
	//
	// If a FETCH response is received that does not contain the UID data item
	// then it is an unsolicited server event and should not be recorded in the result set.
	
	TBool foundFlags = EFalse;
	TBool foundUid = EFalse;
	
	while ( !(foundFlags && foundUid) )
		{
		TPtrC8 dataItem = GetNextPart();
				
		if (dataItem.CompareF(KImapTxtFlags) == 0)
			{
			iUnparsedData.Set( messageFlagInfoEntry.ParseFlagsL(Remainder()) );
			foundFlags = ETrue;
			}
		else if (dataItem.CompareF(KImapTxtUid) == 0)
			{
			TPtrC8 uidString = GetNextPart();
			
			TInt err = messageFlagInfoEntry.SetMessageUid(uidString);
			if (err != KErrNone)
				{
				CorruptDataL();
				}
			foundUid = ETrue;
			}
		else if (dataItem.Length() == 0)
			{
			// No more data items
			// 
			break; // from while loop
			}
		else
			{
			//
			__LOG_FORMAT((iLogId, "CImapFetchFlags::ProcessFetchFlagsResponseL() - Unexpected data item: %S", &dataItem));
			CorruptDataL();
			}
		}
		
	if (foundFlags && foundUid)
		{
		// A genuine response to the UID FETCH or UID STORE was received
		iOutMessageFlagInfo.AppendL(messageFlagInfoEntry);
		}
	else if (foundFlags)
		{
		// An unsolicited server event was received - record this so that a sync will be triggered 
		// after this command has completed.
		__LOG_TEXT(iLogId, "CImapFetchFlags::ProcessFetchFlagsResponseL() - Found unsolicited FETCH FLAGS");
		SetMessageFlagsChanged();
		}
	else
		{
		// Was expecting at least the FLAGS data item.
		CorruptDataL();
		}
	}
