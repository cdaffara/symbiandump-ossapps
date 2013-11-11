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


#include "cimapselect.h"
#include "moutputstream.h"
#include "cimapfolderinfo.h"
#include "cimapsessionconsts.h"
#include "cimaplogger.h"

_LIT8(KTxtSelectFormat, "%d SELECT %S\r\n");
const TInt KSelectFormatEscapeCharCount = 4; // for "%d" and %S
_LIT8(KReadWriteTxt,"READ-WRITE");
_LIT8(KReadOnlyTxt,"READ-ONLY");

/**
The factory constructor. Part of two phased construction.
*/
CImapSelect* CImapSelect::NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId)
	{
	CImapSelect* self = new(ELeave) CImapSelect(aSelectedFolderData, aLogId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Second phase constructor.
*/
void CImapSelect::ConstructL()
	{
	}
/**
Constructor.
*/
CImapSelect::CImapSelect(CImapFolderInfo* aSelectedFolderData, TInt aLogId)
	: CImapCommandEx(aSelectedFolderData, aLogId)
	{
	__ASSERT_DEBUG(aSelectedFolderData != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ESelectFolderDataIsNull));
	__ASSERT_DEBUG(aSelectedFolderData->Name().Length() > 0, TImapServerPanic::ImapPanic(TImapServerPanic::ESelectMailboxNameIsEmpty));
	
	// Default to "read-write"
	aSelectedFolderData->SetReadWrite(ETrue);
	}

/**
Destructor.
*/
CImapSelect::~CImapSelect()
	{
	}

/**
Responsible for sending the IMAP select command to the remote server to
perform the desired action the IMAP client wishes. The data will be sent
to the remote server on the output stream provided.
It is assumed the output stream has already been set up and ready to use.

@param aTagId	Used to generate the IMAP tag that identifies the message.
@param aStream	The output stream on which the message will be sent.
*/
void CImapSelect::SendMessageL(TInt aTagId, MOutputStream& aStream)
	{
	HBufC8* mailbox = EncodeMailboxNameForSendL(iSelectedFolderData->Name());
	CleanupStack::PushL(mailbox);
	
	iTagId = aTagId;
	TInt bufferLength = KTxtSelectFormat().Length() - KSelectFormatEscapeCharCount + TagLength(aTagId) + mailbox->Length();
	
	__ASSERT_DEBUG(iOutputBuffer==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandOutputBufferNotNull));
	iOutputBuffer = HBufC8::NewL(bufferLength);
	iOutputBuffer->Des().Format(KTxtSelectFormat, iTagId, mailbox);
	
	CleanupStack::PopAndDestroy(mailbox);
	
	// Send the data on the output stream
	aStream.SendDataReq(*iOutputBuffer);
	}

/**
Parses the tagged response, looking for response text data.
Records the response status code (OK/NO/BAD)
And checks that the incoming tag id matches the sent tag id.
@param aTagId The value of the incoming tag.
@return ETrue - indicating that the command is completed by this tagged response.
*/
TBool CImapSelect::ParseTaggedResponseL(TInt aTagId)
	{
	// Check the tag id and fetche the response code
	BaseParseTaggedResponseL(aTagId);

	// Check the response code
	if (iResponseCode == EImapResponseNone)
		{
		// Was expecting one of OK/NO/BAD, but didn't get it.  This is a parse error.
		CorruptDataL();
		}

	if (iResponseCode == EImapResponseOk)
		{
		// [READ-ONLY] or [READ-WRITE] are always sent in the tagged OK response (see p33 of RFC3501)
		// The server SHOULD or MUST send this information depending on circumstances.
		// This means it is possible that the server might not send this information.
		// So the default value is set in the constructor of the Select and Examine command object.
		
		TPtrC8 responseTextCode = GetResponseTextCodeL(); //  e.g. [READ-ONLY]
		
		if(responseTextCode.CompareF(KReadWriteTxt) == 0)
			{
			iSelectedFolderData->SetReadWrite(ETrue);
			}
		else if(responseTextCode.CompareF(KReadOnlyTxt) == 0)
			{
			iSelectedFolderData->SetReadWrite(EFalse);
			}
		else if (responseTextCode.Length() > 0)
			{
			// Not expecting alternative response code here.
			__LOG_TEXT(iLogId, "CImapSelect - ignoring unknown response text code");
			}
		}
		
	return ETrue;
	}
	
CImapCommand::TParseBlockResult CImapSelect::ParseUntaggedResponseL()
	{
	TParseBlockResult result = ENotRecognised;
	
	TResponseCode untaggedResponseCode = GetResponseStateCode();
	
	if (untaggedResponseCode == EImapResponseOk)
		{
		ParseUntaggedOkL();
		result = ECompleteUntagged;
		}
	else if (untaggedResponseCode == EImapResponseNone)
		{
		TPtrC8 nextPart = GetNextPart();
		if (nextPart.CompareF(KImapTxtFlags) == 0)
			{
			TPtrC8 remainder = Remainder();
			ParseFlagsL(remainder);
			result = ECompleteUntagged;
			}
		else
			{
			result = ENotRecognised;
			}
		}
	
	// Untagged NO or untagged BAD are ignored (i.e. ENotRecognised)
	// There is no need to treat these as errors.
	//
	// Servers that support IMAP Extensions may return these responses even if we 
	// have not asked for them.
	// For example, RFC 4315 defines the "UIDPLUS" extension.
	// This allows a server to return...
	//
	//     * NO [UIDNOTSTICKY] Non-persistent UIDs
	//
	// ... as one of the untagged responses to a SELECT command.
	// 
	// We don't currently support UIDPLUS, so we can safely ignore this response.
	// But if we did support UIDPLUS, then the untagged NO would not need to be 
	// treated as an error (as is the case with tagged NO), but would merely cause
	// the code to store a flag to record that certain features of UIDPLUS are 
	// not available on this mailbox.
		
	return result;
	}
	
/**
Looks for and parses the response text code that should be found
in an untagged OK response.
*/
void CImapSelect::ParseUntaggedOkL()
	{
	TPtrC8 responseTextCode = GetResponseTextCodeL(); //  e.g. [UIDNEXT 1234]

	if (responseTextCode.Length() > 1)
		{
		TInt offset = responseTextCode.Locate(' ');
		if (offset > 0)
			{
			TPtrC8 textCode(responseTextCode.Left(offset));
			
			responseTextCode.Set(responseTextCode.Mid(offset + 1));
			
		 	if (textCode == KImapTxtUidNext) 
				{
				TLex8 desToInt(responseTextCode);
				TInt uidNext=0;
				TInt err = desToInt.Val(uidNext);
				if(err != KErrNone)
					{
					CorruptDataL();
					}
				iSelectedFolderData->SetUidNext(uidNext);
				}
			else if (textCode == KImapTxtUnseen)
				{
				TLex8 desToInt(responseTextCode);
				TInt unseen=0;
				TInt err = desToInt.Val(unseen);
				if(err != KErrNone)
					{
					CorruptDataL();
					}
				iSelectedFolderData->SetUnseen(unseen);
				}
				
			else if (textCode == KImapTxtUidValidity)
				{
				TLex8 desToInt(responseTextCode);
				TUint uidValidity=0;
				TInt err = desToInt.Val(uidValidity);
				if(err != KErrNone)
					{
					CorruptDataL();
					}
				iSelectedFolderData->SetUidValidity(uidValidity);
				}
			else
				{
				__LOG_TEXT(iLogId, "CImapSelect::ParseUntaggedOkL() - ignoring response text code");
				}
			}
		else
			{
			__LOG_TEXT(iLogId, "CImapSelect::ParseUntaggedOkL() - ignoring response text code (no spaces)");
			}			
		}
	else
		{
		// was expecting a response text code after the untagged OK
		CorruptDataL();	
		}
	}
	
void CImapSelect::ParseFlagsL(TDesC8& aFlags)
	{
	// formal definition of the FLAGS response is
	//
	// mailbox-data = "FLAGS" SP flag-list
	//
	// flag-list = "(" [flag *(SP flag)] ")"
	//
	// flag = "\Answered" / "\Flagged" / "\Deleted" / "\Seen" / "\Draft" / flag-keyword / flag-extension
	//
	// flag-extension = "\" atom
	// flag-keyword = atom
	//
	
	RDesParts flags;
	CleanupClosePushL(flags);
	TPtrC8 flagsString(aFlags);
	
	TInt start = aFlags.Locate('(');
	TInt end = aFlags.Locate(')');
	
	if(start == KErrNotFound || end == KErrNotFound)
		{
		CorruptDataL();	
		}
	else if (start > end)
		{
		CorruptDataL();
		}
	else
		{
		// lose the brackets
		flagsString.Set(aFlags.Mid(start+1, end-start-1));
		}
	
	GetDelimitedPartsL(' ', flagsString, flags);
	TInt flagsCount = flags.Count();
	for (TInt i = 0; i < flagsCount; ++i)
		{
		TPtrC8 flag = flags[i];

		if(flag.CompareF(KImapTxtFlagDeleted) == 0)
			{
			iSelectedFolderData->SetFlag(CImapFolderInfo::EDeleted, ETrue);				
			}
		else if (flag.CompareF(KImapTxtFlagSeen) == 0)
			{
			iSelectedFolderData->SetFlag(CImapFolderInfo::ESeen, ETrue);	
			}
		else if(flag.CompareF(KImapTxtFlagFlagged) == 0)
			{
			iSelectedFolderData->SetFlag(CImapFolderInfo::EFlagged, ETrue);
			}
		else if(flag.CompareF(KImapTxtFlagAnswered) == 0)
			{
			iSelectedFolderData->SetFlag(CImapFolderInfo::EAnswered, ETrue);	
			}
		else if(flag.CompareF(KImapTxtFlagDraft) == 0)
			{
			iSelectedFolderData->SetFlag(CImapFolderInfo::EDraft, ETrue);	
			}
		}

	CleanupStack::PopAndDestroy(&flags);
	}
