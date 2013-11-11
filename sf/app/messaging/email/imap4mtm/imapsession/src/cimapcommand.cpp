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


#include "cimapcommand.h"
#include "cimapsessionconsts.h"
#include "cimapfolderinfo.h"
#include "cimaputils.h"
#include "cimapcharconv.h"
#include "cimaplogger.h"

static const TInt KDefaultNumberOfTaggedResponsesExpected = 1;

CImapCommand::CImapCommand(CImapFolderInfo* aSelectedFolderData, TInt aLogId)
	: iSelectedFolderData(aSelectedFolderData)
	, iLogId(aLogId)
	{}

CImapCommand::~CImapCommand()
	{
	delete iOutputBuffer;
	}

void CImapCommand::SendDataCnfL()
	{}

CImapCommand::TParseState CImapCommand::ParseBlockL(const TDesC8& aData)
	{
#ifdef __IMAP_LOGGING
	// Log the first few bytes only - but enough to tell the difference between untagged FETCH's
	TPtrC8 truncatedData = aData.Left(30);
	__LOG_FORMAT((iLogId, "CImapCommand::ParseBlockL(): [%S] %d octets", &truncatedData, aData.Length()));
#endif //__IMAP_LOGGING

	__ASSERT_DEBUG(iParseState != ECommandComplete, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandInvalidParseState2));
	iUnparsedData.Set(aData);
	iLiteralBytesExpected = 0; // Initialise to zero, and only peek in states that read a line.
	
	switch (iParseState)
		{
		case EWaitStartResponse:
			{
			if (Flushing())
				{
				ParseStartResponseFlushL(aData);
				}
			else
				{
				ParseLineL(aData);
				}
			break;
			}
		case EWaitLineParse:
			{
			ParseLineL(aData);
			}
			break;
		case EWaitLiteralParse:
			{
			ParseLiteralL(aData);
			}
			break;		
		case EWaitLiteralIngore:
			{
			__LOG_TEXT(iLogId, "CImapCommand Ignore Literal");
			// Ignore the literal.
			// A literal is always followed by a line (as part of the same response)
			SetParseState(EWaitLineIgnore);
			}
			break;
		case EWaitLineIgnore:
			{
			__LOG_TEXT(iLogId, "CImapCommand Ignore Line following Literal");
			// Ignore the line, but check whether it indicates that another literal is on its way.
			PeekLiteralRequest(aData);
			
			TParseState newParseState = (iLiteralBytesExpected > 0) 
				? EWaitLiteralIngore	// a literal has been indicated on the line.  We need to ignore that too!
				: EWaitStartResponse;	// we are not expecting any more data for this response, so wait for the next one.
			SetParseState(newParseState);
			}
			break;
		default:
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandInvalidParseState3));
			}
		}

	if (iCompleteOnAnyResponse && iParseState==EWaitStartResponse)
		{
		__LOG_TEXT(iLogId, "CImapCommand - Complete on Any Response");
		__ASSERT_DEBUG(iResponseCode == EImapResponseNone, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandInvalidResponseCode));
		
		// Simulate command completion so that the session closes this command.		
		iResponseCode = EImapResponseOk;
		SetParseState(ECommandComplete);
		}

	// check some post-conditions: is session getting consistent data?
	
	// (iParseState == ECommandComplete) implies (iResponseCode != EImapResponseNone)
	__ASSERT_DEBUG(iParseState == ECommandComplete ? iResponseCode != EImapResponseNone : ETrue, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandMismatchedParseStateAndResponseCode));
	
	// (iParseState != ECommandComplete) implies (iResponseCode == EImapResponseNone)
	__ASSERT_DEBUG(iParseState != ECommandComplete ? iResponseCode == EImapResponseNone : ETrue, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandMismatchedParseStateAndResponseCode));
				
	return iParseState;
	}

/**
Parses incoming line data.
@param aLine the line to parse.
*/
void CImapCommand::ParseLineL(const TDesC8& aLine)
	{

	// Should only be calling this method when parsing a line.
	__ASSERT_DEBUG(iParseState == EWaitStartResponse || iParseState == EWaitLineParse, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandInvalidParseState4));
	
	PeekLiteralRequest(aLine);
				
	TParseBlockResult parseBlockResult = DoParseBlockL(aLine);
	
	if(parseBlockResult ==  ECompleteUntagged && iLiteralBytesExpected != 0)
		return;
	
	switch (parseBlockResult)
		{
		case ECompleteTagged:
			// No more data expected
			__ASSERT_DEBUG(iLiteralBytesExpected == 0, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandResponseLiteralDataNotZero));
			__ASSERT_DEBUG(iResponseCode != EImapResponseNone, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandInvalidResponseCode));
			
			SetParseState(ECommandComplete);
			break;
		case ECompleteUntagged:
			// Wait for the next response line to be received
			__ASSERT_DEBUG(iLiteralBytesExpected == 0, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandResponseLiteralDataNotZero));
			
			SetParseState(EWaitStartResponse);
			break;
		case EResponseIncomplete:
			// Literal data is expected - this needs to be delivered to the subclass
			__ASSERT_DEBUG(iLiteralBytesExpected > 0, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandResponseLiteralDataNotNonZero));
			
			SetParseState(EWaitLiteralParse);
			break;
		case ENotRecognised:
			if (iParseState == EWaitLineParse)
				{
				__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandInvalidParseState5));
				}
			else // EWaitStartResponse
				{
				// The line was not understood by the subclass, so try the generic parsing
				parseBlockResult = ParseUnhandledBlockL(aLine);
				__ASSERT_DEBUG(parseBlockResult != EResponseIncomplete, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandInvalidParseBlockResult)); // Not expected or currently supported
				
				// If we're swallowing an unsupported response, then we may need to swallow a literal block next
				TParseState newParseState = (iLiteralBytesExpected > 0) 
					? EWaitLiteralIngore	// a literal has been indicated on the line.  We need to ignore that too!
					: EWaitStartResponse;	// we are not expecting any more data for this response	
				SetParseState(newParseState);
				}
			break;
		default:
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandInvalidParseBlockResult));
			break;
		}
	}
	
/**
Parses incoming literal data.
@param aLiteralBlock the literal block to parse.
*/
void CImapCommand::ParseLiteralL(const TDesC8& aLiteralBlock)
	{
	// Should only be calling this method when parsing a literal blcok.
	__ASSERT_DEBUG(iParseState == EWaitLiteralParse, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandInvalidParseState6));
	
	TParseBlockResult parseBlockResult = DoParseBlockL(aLiteralBlock);
	switch (parseBlockResult)
		{
		case ECompleteTagged:					
		case ECompleteUntagged:
			// Literals are always followed by a line, so the response can't be complete yet.
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandInvalidParseBlockResult));
			break;
		case EResponseIncomplete:
			SetParseState(EWaitLineParse);
			break;
		case ENotRecognised: 
			// Subclass asked for the literal block, so it ought to have parsed it.
			// Fall through to default __ASSERT_DEBUG below.
		default:
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandInvalidParseBlockResult));
			break;
		}
	}

/**
During cancel this method is called whenever the start of a new response is received.
It doesn't do any parsing, other than to detect the closing tagged response, 
which completes the command and means there is no more data to receive.
@param aLine the line containing the start of a new response.
*/
void CImapCommand::ParseStartResponseFlushL(const TDesC8& aLine)
	{
	__LOG_TEXT(iLogId, "CImapCommand::ParseStartResponseFlushL()");
	
	// should only call this method when parsing an incoming response but not cancelling
	__ASSERT_DEBUG(iParseState == EWaitStartResponse, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandInvalidParseState7));
	__ASSERT_DEBUG(iTaggedResponsesToFlush > 0, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandNoTaggedResponsesToFlush));

	TInt tagId = 0;
	switch (GetTagTypeL(tagId))
		{
		case ETagged:
			{
			--iTaggedResponsesToFlush;
			if (iTaggedResponsesToFlush == 0)
				{
				// Don't check the tagId during cancel.
				iResponseCode = GetResponseStateCode();
				SetParseState(ECommandComplete);	
				}
			else
				{
				__LOG_FORMAT((iLogId, "CImapCommand - Still have %d tagged response(s) to flush", iTaggedResponsesToFlush));
				}			
			}
			break;
		case EUntagged:
			{
			// Ignore the untagged response, but check whether it indicates that another literal is on its way.
			PeekLiteralRequest(aLine);
			TParseState newParseState = (iLiteralBytesExpected > 0) 
				? EWaitLiteralIngore	// a literal has been indicated on the line.  We need to ignore that too!
				: EWaitStartResponse;	// we are not expecting any more data for this response, so wait for the next one.
			SetParseState(newParseState);
			}
			break;
		case EContinuation:
			{
			// Not exepecting a continuation response
			CorruptDataL(); 
			}
			break;
		}
	}

/**
Looks for {nnn} at the end of the line, determine whether literal bytes are expected.
If literal bytes are expected, then iLiteralBytesExpected is updated to store the
number of bytes expected.
@return KErrNone if literal bytes are expected, otherwise a system-wide error code.
*/
TInt CImapCommand::PeekLiteralRequest(const TDesC8& aLine)
	{
	// Minimum of 3 chars needed for literal specifier: {n}
	TInt lineLength = aLine.Length();
	if (lineLength < 3)
		{
		return KErrNotFound; // no request found.
		}
		
	// Last char MUST be a }
	if (aLine[lineLength-1] != '}')
		{
		return KErrNotFound; // no request found.
		}
	
	// Reverse-Find matching '{'
	TPtrC8 ptrLiteralRequest;
	for (TInt i=lineLength-1; i>=0; --i)
		{
		if (aLine[i] == '{')
			{
			// Found matching brace.  Point at its contents only!
			ptrLiteralRequest.Set(aLine.Mid(i+1, lineLength-i-2));
			break; // from for loop
			}
		}
	
	if (ptrLiteralRequest.Length() == 0)
		{
		return KErrNotFound; // No matching brace found
		}
	
	// Extract the value
	TLex8 lex(ptrLiteralRequest);
	TInt err = lex.Val(iLiteralBytesExpected);
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)			
	//	If message body is downloaded using FETCH BINARY. The number of bytes received for previous fetch is recorded here.
	//	This valued is used to make sure that there are no extra body fetch command are issued 
	iPreviousLiteralBytesReceived = iLiteralBytesExpected;
#endif
	
#ifdef __IMAP_LOGGING
	if (iLiteralBytesExpected > 0)
		{
		__LOG_FORMAT((iLogId, "CImapCommand - Found Literal Request: %d", iLiteralBytesExpected));
		}	
#endif //__IMAP_LOGGING

	return err;
	}

/**
This is called when the session has processed all the blocks in its input buffer, but the 
command is not yet complete (so it is expecting more data).
This allows subclasses of CImapCommand to commit any bulk operations while waiting for the next set of data.
By default, this method does nothing.
*/
void CImapCommand::WaitingForMoreDataL()
	{}

/**
Call this method to stop fully parsing the command.
Instead, discard all incoming server data until no more data is expected for this command.
By default this means that the tagged response has been received.
This method can be overridden.
*/
void CImapCommand::Cancel()
	{
	__LOG_TEXT(iLogId, "CImapCommand::Cancel()");
	EnterFlushingState();
	}

void CImapCommand::EnterFlushingState()
	{
	iFlushing = ETrue;
	iTaggedResponsesToFlush = NumberOfTaggedResponsesExpected();
	
	__LOG_FORMAT((iLogId, "CImapCommand::EnterFlushingState() - %d tagged response(s) to flush", iTaggedResponsesToFlush));
	
	if (iParseState == EWaitLiteralParse)
		{
		SetParseState(EWaitLiteralIngore);
		}
	else if (iParseState == EWaitLineParse)
		{
		SetParseState(EWaitLineIgnore);
		}
	}

TBool CImapCommand::Flushing() const
	{
	return iFlushing;
	}

/**
This method tells the session whether the Flush operation is able to complete
the command straight away.
By default, a command that has not already been completed (and so destoyed)
cannot be completed early by Flush, as it is still waiting for the tagged
response.
This method can be overridden by commands that can be completed earlier, such
as commands that do not wait for a tagged response.
@return EFalse.
*/
TBool CImapCommand::CanCompleteFlushNow() const
	{
	// Any command that has completed should have been destroyed before this
	// method is called.
	__ASSERT_DEBUG(iParseState != ECommandComplete, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandInvalidParseState8));
	__ASSERT_DEBUG(Flushing(), TImapServerPanic::ImapPanic(TImapServerPanic::ECommandNotFlushing));
	
	return EFalse;
	}

/**
Returns the number of tagged responses that are currently expected.
This default implementation always returns one, as most commands on send one request
and expect one tagged response.
Pipelined commands that send more than one request at a time should override this method.
@return one.
*/
TInt CImapCommand::NumberOfTaggedResponsesExpected() const
	{
	return KDefaultNumberOfTaggedResponsesExpected;
	}

/**
This API will return the result returned by the remote server to the issued IMAP command.

@return The IMAP result code extracted from the incoming response or EImapResponseNone 
		if the response has not yet been parsed.
*/ 
CImapCommand::TResponseCode CImapCommand::ResponseCode() const
	{
	return iResponseCode;
	}

/**
@return the number of bytes 
*/
TInt CImapCommand::LiteralBytesExpected() const
	{
	return iLiteralBytesExpected;
	}

/**
Static method that should a be called by all classes internal to CImapSession whenever they
detect corrupt data - e.g. missing fields - that means that they can no longer process the 
incoming data.

This method will always leave with KErrCorrupt.
*/
#ifdef __IMAP_LOGGING
void CImapCommand::CorruptDataL(TInt aLogId)
#else
void CImapCommand::CorruptDataL(TInt /*aLogId*/)
#endif //__IMAP_LOGGING
// static method
	{
	__LOG_TEXT(aLogId, "CImapCommand::CorruptDataL() !!!");
	User::Leave(KErrImapCorrupt);
	}

/**
Non-static version of CorruptDataL() that provides the current log id
*/
void CImapCommand::CorruptDataL()
	{
	CorruptDataL(iLogId);
	}

/**
Static method that stores each part of the supplied data into an array based on 
the delimiter value supplied in the aDelimiter argument.

@param aDelimiter 	The character that delimits the end of one part and the begining of the next.
@param aData 		The data that is to be split.
@param aOutDelimitedPartsList Output array that recieves the list of parts.
@param aMaxParts 	If greater than zero, this stops the search when aOutDelimitedPartsList 
					has the specified number of parts.
*/
void CImapCommand::GetDelimitedPartsL(TChar aDelimiter, const TDesC8& aData, RDesParts& aOutDelimitedPartsList, TInt aMaxParts /*=0*/)
// static method
	{
	aOutDelimitedPartsList.Reset();
	TInt dataLength = aData.Length();
	
	if (dataLength == 0)
		{
		// no data to parse, so nothing to do.
		return;
		}
	
	TInt currentPos = 0;
	TPtrC8 currentSegment(aData);
	
	for(;;)
		{
		TInt found = currentSegment.Locate(aDelimiter);
		if (found == KErrNotFound)
			{
			aOutDelimitedPartsList.AppendL(currentSegment);
			break;
			}
		
		currentSegment.Set(currentSegment.Left(found));
		aOutDelimitedPartsList.AppendL(currentSegment);
		
		// Check if whether have enough parts yet
		if (aMaxParts > 0 && aOutDelimitedPartsList.Count() >= aMaxParts)
			{
			break;
			}
		
		// Move past the delimiter and set next segment
		currentPos += (found+1);
		if (currentPos<dataLength)
			{
			currentSegment.Set(aData.Mid(currentPos));
			}
		else
			{
			// No more data
			break;
			}	
		}
	}

/**
Static method that calculates the length in characters of the given tag id

@param aTagId The tag id whose length in characters is to be calculated
@return The length in characters of the given tag id
*/
TInt CImapCommand::TagLength(TInt aTagId)
// static method
	{
	// Count how many characters would be needed to represent aTagId as a string.
	// Do this by count how many times aTagId can be divided by 10.
	
	TInt tagLength = 1; // values 0 to 9 have a length of 1 character
	
	while (aTagId > 9)
		{
		aTagId = aTagId / 10;
		++tagLength;
		}
		
	return tagLength;
	}

/**
Encodes a Unicode mailbox name by:
	- Converting it into ImapUtf7 format
	- Quoting it if necessary
@param aMailboxName the mailbox name in Unicode format.
@return the mailbox name in ImapUtf7, quoted if necessary.
*/
HBufC8* CImapCommand::EncodeMailboxNameForSendL(const TDesC16& aMailboxName)
// static method
	{
	// According to section 9 of RFC3501, mailbox is an astring.
	// 	
	// That means that if it cannot be represented as an atom, then it must be 
	// represented as either a quoted string or literal.
	// We can rule out literal as explained below.
	//
	// An atom is a string that contains any 7-bit character other than "atom-specials"
	//
	// atom-specials consists of 
	//
	// 	"(" ")" "{" "%" "*" DQUOTE "\" "]" SP CTL
	//
	//
	// The ImapUtf7 encoding has the following characteristics (from RFC3501).
	//
	// "printable US-ASCII characters, except for "&", represent 
	// themselves; that is, characters with octet values 0x20-0x25 and 0x27-0x7e.
	// All other characters (octet values 0x00-0x1f and 0x7f-0xff) are
	// represented in modified BASE64, with a further modification from
	// [UTF-7] that "," is used instead of "/". Modified BASE64 MUST NOT be
	// used to represent any printing US-ASCII character which can represent
	// itself.""
	//
	// "&" means start "Modified BASE64" block
	// "-" means end "Modified BASE64" block
	// "&-" represents the & character
	//
	// "Modified BASE64" can contain the following characters
	// "a" to "z", "A" to "Z", "0" to "9" and then "+" "," and "=".  The last is the pad character.
	//
	//
	// So an Imap-UTF7 mailbox string can always be sent as quoted (and hence no need for literal)
	// because it conforms to the RFC3501 definition of a quoted string, which 
	// may only contain 7-bit US-ASCII characters other than NUL, CR and LF (which are encoded as Modified BASE64)
	//
	//
	// To decide whether to send the encoded mailbox as an atom or as quoted, we need to test whether
	// the encoded string contains printable atom-specials (CTL characters including CR and LF have 
	// already been dealt with by the ImapUtf7 encoding)
	//
	// Usefully, the set of atom-specials and the set of characters used in Modified BASE64 
	// DO NOT intersect.  That makes it safe to search for (and escape) atom-specials in an 
	// encoded ImapUtf7 string without inadvertantly finding or modifying a BASE64 encoding.
	//
	// Note that section 5.1 of RFC3501 allows mailbox names to include all the atom-specials, including
	// list wildcards such as "*" and "%".  So we need to search for all printable atom-specials, not a subset.

	if (aMailboxName.Length()==0)
	    {
	    // aMailboxName is an empty string so just return an empty string.
	    _LIT8(KEmptyString, "\"\"");
	    HBufC8* hBuf = KEmptyString().AllocL();
	    return hBuf;
	    }

	// Convert into ImapUtf7.
	HBufC8* mailboxNameUtf7 = CImapUtils::GetRef().Charconv().ConvertFromUnicodeToImapUtf7L(aMailboxName);
	CleanupStack::PushL(mailboxNameUtf7);
	
	// Search for printable atom-specials and quote the mailbox if any are found
	TInt countQuotedSpecials = 0;
	if (CheckForPrintableAtomSpecial(*mailboxNameUtf7, countQuotedSpecials))
		{		
		// Make enough room to escape each of the quoted-specials and to insert the start and end quote
		TInt mailboxNameLength = mailboxNameUtf7->Length() + countQuotedSpecials + 2;
		HBufC8 *expandedBuffer = mailboxNameUtf7->ReAllocL(mailboxNameLength);
		
		// Been moved due to realloc?
		if (expandedBuffer != mailboxNameUtf7)
			{
			// Update mailboxNameUtf7 with the new address
			mailboxNameUtf7 = expandedBuffer;
			expandedBuffer = NULL;
			
			// Update the cleanup stack with the new address of mailboxNameUtf7
			CleanupStack::Pop();
			CleanupStack::PushL(mailboxNameUtf7);
			}

		// Insert the opening DQUOTE
		mailboxNameUtf7->Des().Insert(0, KImapTxtDoubleQuote);
		
		// Perform escaping if there are any quoted-specials
		if (countQuotedSpecials > 0)
			{
			// Use the length of the newly expanded buffer, 
			// but do not include the first or last characters, which are non-escaped DQUOTE's
			// Hence start at index 1, and reduce the length by 2
			mailboxNameLength -= 2;
			for(TInt i=1; i<mailboxNameLength; ++i)
				{
				if ((*mailboxNameUtf7)[i]=='\\' || (*mailboxNameUtf7)[i]=='\"')
					{
					// We have found one of the quoted-specials, this needs to be escaped
					mailboxNameUtf7->Des().Insert(i, KImapTxtEscape);
					++i;
					}
				}	
			}
			
		// Append the closing DQUOTE
		mailboxNameUtf7->Des().Append(KImapTxtDoubleQuote);
		}
	
	CleanupStack::Pop(mailboxNameUtf7);
	return mailboxNameUtf7;
	}

/**
Checks whether the given string contains printable atom-specials.
An atom special is one of "(" ")" "{" "%" "*" DQUOTE "\" "]" SP
@param aString The string that is to be checked
@param aCountQuotedSpecials Output parameter that counts the number of quoted-specials characters that need escaping
@return ETrue is any of the atom-specials are present in the string
*/
TBool CImapCommand::CheckForPrintableAtomSpecial(const TDesC8& aString, TInt& aCountQuotedSpecials)
// static method
	{
	aCountQuotedSpecials = 0;
	TBool foundPrintableAtomSpecial = EFalse;

	TInt stringLength = aString.Length();
	for(TInt i=0; i<stringLength; ++i)
		{
		if (aString[i]=='(' || aString[i]==')' || aString[i]=='{' || aString[i]==' ' ||	// <<< specific printable atom-specials
			aString[i]=='%' || aString[i]=='*' || 										// <<< list-wildcards
			aString[i]==']')															// <<< resp-specials
			{
			foundPrintableAtomSpecial = ETrue;
			}
		else if (aString[i]=='\\' || aString[i]=='\"')									// <<< quoted-specials
			{
			foundPrintableAtomSpecial = ETrue;
			++aCountQuotedSpecials;
			}
		}
		
	return foundPrintableAtomSpecial;
	}

/**
Helper that is to be called by subclasses when they receive some data that is not specific to the class.
This indicates that the data is probably unilateral notification data, such as EXISTS and RECENT
This method will parse and handle the notification.
@param aLine The line to be parsed and handled (not including CRLF).
@return Whether the data was successfully parsed, recognised, etc
*/
CImapCommand::TParseBlockResult CImapCommand::ParseUnhandledBlockL(const TDesC8& aLine)
	{
	TParseBlockResult result = ENotRecognised;	
	
	iUnparsedData.Set(aLine);
	
	// Check the tag type.  This method should only be called for untagged resposnes.
	TInt tagId = 0;
	TTagType tagType = GetTagTypeL(tagId);
	__ASSERT_DEBUG(tagType == EUntagged, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandInvalidTagType));
	
	// Looking for one of
	//
	// number "EXISTS"
	// number "RECENT"
	// nz-number "EXPUNGE"
	//
	// Note that untagged "BYE" is ignored.
	// This can occur during LOGOUT or prior to the server terminating the connection.
	// For LOGOUT, we wait for the tagged OK.
	// And transport handler detects server termination - so we don't need to respond to the early warning.
	
	TPtrC8 numberPart = GetNextPart();
	TLex8 desToInt(numberPart);
	
	TInt messageCountOrId = 0; // This is a message count for all except EXPUNGE where it is a message id
	TInt err = desToInt.Val(messageCountOrId);
	
	if (err == KErrNone)
		{
		TPtrC8 secondPart = GetNextPart();
		
		if((secondPart.CompareF(KImapTxtExpunge) == 0))
			{
			__LOG_FORMAT((iLogId, "CImapCommand::ParseUnhandledBlockL() - Found EXPUNGE: %d", messageCountOrId));
			if (iSelectedFolderData != NULL && iSelectedFolderData->Exists())
				{
				iSelectedFolderData->AddExpungedMessageL(messageCountOrId);
				}
			result = ECompleteUntagged;
			}
		else if((secondPart.CompareF(KImapTxtExists) == 0))
			{
			__LOG_FORMAT((iLogId, "CImapCommand::ParseUnhandledBlockL() - Found EXISTS: %d", messageCountOrId));
			if (iSelectedFolderData != NULL)
				{
				iSelectedFolderData->SetExists(messageCountOrId);	
				}			
			result = ECompleteUntagged;
			}
		else if((secondPart.CompareF(KImapTxtRecent) == 0))
			{
			__LOG_FORMAT((iLogId, "CImapCommand::ParseUnhandledBlockL() - Found RECENT: %d", messageCountOrId));
			if (iSelectedFolderData != NULL)
				{
				iSelectedFolderData->SetRecent(messageCountOrId);
				}			
			result = ECompleteUntagged;
			}
		else if ((secondPart.CompareF(KImapTxtFetch) == 0))
			{
			// If a FETCH has been requested, then the FETCH response will be handled by the subclass.
			// So getting here means that the FETCH was unsolicited.
			// When means that it is LIKELY that this is a FETCH FLAGS response.
			// We don't need to parse the resposne fully (a complex task), but instead, 
			// just record the notification that flags have changed for a message.
			// This will cause a resync of flags when the Protocol Controller next attempts to go IDLE.
			__LOG_FORMAT((iLogId, "CImapCommand::ParseUnhandledBlockL() - Found unsolicited FETCH: %d", messageCountOrId));
			SetMessageFlagsChanged();
			result = ECompleteUntagged;
			}
		}
	
#ifdef __IMAP_LOGGING
	if (result == ENotRecognised)
		{
		__LOG_TEXT(iLogId, "CImapCommand::ParseUnhandledBlockL() - Ignoring Unhandled Response");
		}	
#endif //__IMAP_LOGGING

	return result;
	}

/**
Helper function to return the tag of the response
@param aTagId Will contain the Tag if response is tagged.
@return Returns the tag type as an enumeration.
*/
CImapCommand::TTagType CImapCommand::GetTagTypeL(TInt& aTagId)
	{
	TPtrC8 nextPart = GetNextPart() ;
	if(nextPart.Find(KImapTxtContinuation) >= 0)
	 	{
	 	__LOG_TEXT(iLogId, "CImapCommand::GetTagTypeL() - Found Continuation");
	 	return EContinuation ;
	 	}
	else if(nextPart.Find(KImapTxtUntagged) >= 0)
		{
		// Don't log this, as it's the most common, and causes bloat!
		return EUntagged;
		}
	else 
		{
		TLex8 desToInt(nextPart);
		if (desToInt.Val(aTagId) != KErrNone)
			{
			// We were expecting a numeric tag id here.
			__LOG_TEXT(iLogId, "CImapCommand::GetTagTypeL() - Non-numeric tag id");
			CorruptDataL();
			}
		__LOG_FORMAT((iLogId, "CImapCommand::GetTagTypeL() - Found Tagged: %d", aTagId));
		}
	return ETagged;
	}

/**
Helper method to return the next part of the response from server.
@return nextpart from the iUnparsedData
		KNullDesC8 if no more data to return.
*/
TPtrC8 CImapCommand::GetNextPart()
	{
	TPtrC8 nextPart = iUnparsedData;
	
	// If the first character is a space, then skip it.	
	if (iUnparsedData.Length() > 0)
		{
		if (iUnparsedData[0] == ' ')
			{
			iUnparsedData.Set(iUnparsedData.Mid(1));
			}
		}
		
	if (iUnparsedData.Length() > 0)
		{
		TInt length = iUnparsedData.Length();
		// Some server may send n FETCH (UID n BODY[1]<0> "Message Body" BODY[1.MIME] {nnn}
		// So extract the body from this response.
		if(iUnparsedData[0] == '"')
			{
			for (TInt i=length-1; i>=1; --i)
				{
				if (iUnparsedData[i] == '"')
					{
					// Found matching brace.  Point at its contents only!
					nextPart.Set(iUnparsedData.Mid(0, i + 1));
					iUnparsedData.Set(iUnparsedData.Mid(i+1));
					if(iUnparsedData.Length() > 0)
						{
						if (iUnparsedData[0] == ' ')
							{
							iUnparsedData.Set(iUnparsedData.Mid(1));
							}
						}
					break; // from for loop
					}
				}
			}
		else
			{
			TInt offset = iUnparsedData.Locate(' ');
			if (offset != KErrNotFound)
				{
				nextPart.Set(iUnparsedData.Left(offset));
				if(iUnparsedData.Length() > offset + 1)
					{
					iUnparsedData.Set(iUnparsedData.Mid(offset + 1));
					}
				else
					{
					iUnparsedData.Set(KNullDesC8);
					}
				}
			else
				{
				iUnparsedData.Set(KNullDesC8);
				}
			}
		}
	return 	nextPart;
	}

/**
Helper method to return the next part of the response from server.
The Unparsed Data pointer is not updated by this routine
@return nextpart from the iUnparsedData
		KNullDesC8 if no more data to return.
*/
TPtrC8 CImapCommand::PeekNextPart()
	{
	TPtrC8 nextPart = iUnparsedData;

	while ((nextPart.Length() > 0) && (nextPart[0] == ' '))
		{
		nextPart.Set(nextPart.Mid(1));
		}

	if (nextPart.Length() > 0)
		{
		TInt offset = nextPart.Locate(' ');
		if (offset != KErrNotFound)
			{
			nextPart.Set(nextPart.Left(offset));
			}
		}
	else
		{
		nextPart.Set(KNullDesC8);
		}

	return nextPart;
	}

/**
Remainder()
@return the remaining unparsed data
*/
TPtrC8 CImapCommand::Remainder()
	{
	return iUnparsedData;
	}

/**
Helper method to parse the "["resp-text-code"]" type response from server.
iUnparsedData is updated to next part of response only if [] is found.
@return resp-text-code without the square brackets([]).
        KNullDesC8 if "[" not found.
*/
TPtrC8 CImapCommand::GetResponseTextCodeL()
	{
	if(iUnparsedData.Length()==0)
		{
		iUnparsedData.Set(KNullDesC8);
		return iUnparsedData;	
		}
		
	TPtrC8 response = iUnparsedData;
	if(response[0] == '[')
		{
		TInt offset = iUnparsedData.Locate(']');
		// No closing "]" ,possibly data may be corrupt.
		if(offset== KErrNotFound)
			{
			__LOG_TEXT(iLogId, "CImapCommand::GetResponseTextCodeL() - No Closing ']'");
			CorruptDataL();
			}
		
		response.Set(iUnparsedData.Mid(1,offset-1));
		if (iUnparsedData.Length() > offset + 1)
			{
			// Move to next non-space part after "]".
			iUnparsedData.Set(iUnparsedData.Mid(offset+1));
			}
		else	
			{
			iUnparsedData.Set(KNullDesC8);
			}
			
		__LOG_FORMAT((iLogId, "CImapCommand::GetResponseTextCodeL() - Found %S", &response));
		}
		
		//Case where there is no [ ] for Server Response from servers like tuukka
		//(ie) * OK PERMANENTFLAGS (\seen \answered \flagged \deleted \draft \priority)  
		//we would be returning iUnparsedData, without editing the data

	return response;	
	}
	
/**
Helper method to return the state of the server response.
iUnparsedData is updated to next part of response only if OK/NO/BAD is found.
@return EImapResponseOk if OK is returned by server
		EImapResponseNo if NO is returned by server
		EImapResponseBad if BAD is returned by server
		EImapResponseNone in all other cases. 
*/	
CImapCommand::TResponseCode CImapCommand::GetResponseStateCode()	
	{
	TResponseCode result = EImapResponseNone;
	TInt offset = 0;
	TBool update = EFalse;
	offset = iUnparsedData.Locate(' ');

	TPtrC8 nextPart = iUnparsedData;
	if (offset != KErrNotFound)
		{
		nextPart.Set(iUnparsedData.Left(offset));
		}
		
	if(nextPart.CompareF(KImapTxtOk) == 0) 
		{
		__LOG_TEXT(iLogId, "CImapCommand::GetResponseStateCode() - Found \"OK\"");
	 	result = EImapResponseOk ;
	 	update = ETrue;
	 	}
	else if(nextPart.CompareF(KImapTxtNo) == 0) 
		{
		__LOG_TEXT(iLogId, "CImapCommand::GetResponseStateCode() - Found \"NO\"");
		result = EImapResponseNo;
		update = ETrue;
		}
	else if(nextPart.CompareF(KImapTxtBad) == 0) 
		{
		__LOG_TEXT(iLogId, "CImapCommand::GetResponseStateCode() - Found \"BAD\"");
	 	result = EImapResponseBad;
	 	update = ETrue;
		}
		
	if(update)
		{
		if(iUnparsedData.Length() > offset + 1)
			{
			iUnparsedData.Set(iUnparsedData.Mid(offset+1));		
			}
		else
			{
			iUnparsedData.Set(KNullDesC8);
			}
			
		}
	return result;
	}

void CImapCommand::SetParseState(TParseState aParseState)
	{
	if (aParseState == iParseState)
		{
		// Nothing to do.
		return;
		}
		
#ifdef __IMAP_LOGGING

	_LIT8(KTxtWaitStartResponse, "EWaitStartResponse");
	_LIT8(KTxtWaitLiteralParse, "EWaitLiteralParse");
	_LIT8(KTxtWaitLineParse, "EWaitLineParse");
	_LIT8(KTxtWaitLiteralIngore, "EWaitLiteralIngore");
	_LIT8(KTxtWaitLineIgnore, "EWaitLineIgnore");
	_LIT8(KTxtCommandComplete, "ECommandComplete");
	_LIT8(KTxtParseStateUnknown, "Unknown");
	
	TPtrC8 ptrOldCommandParseState(KTxtParseStateUnknown);
	TPtrC8 ptrNewCommandParseState(KTxtParseStateUnknown);
	
	switch(iParseState)
		{
		case EWaitStartResponse:	ptrOldCommandParseState.Set(KTxtWaitStartResponse);		break;
		case EWaitLiteralParse:		ptrOldCommandParseState.Set(KTxtWaitLiteralParse);		break;
		case EWaitLineParse:		ptrOldCommandParseState.Set(KTxtWaitLineParse);			break;
		case EWaitLiteralIngore:	ptrOldCommandParseState.Set(KTxtWaitLiteralIngore);		break;
		case EWaitLineIgnore:		ptrOldCommandParseState.Set(KTxtWaitLineIgnore);		break;
		case ECommandComplete:		ptrOldCommandParseState.Set(KTxtCommandComplete);		break;
		}
		
	switch(aParseState)
		{
		case EWaitStartResponse:	ptrNewCommandParseState.Set(KTxtWaitStartResponse);		break;
		case EWaitLiteralParse:		ptrNewCommandParseState.Set(KTxtWaitLiteralParse);		break;
		case EWaitLineParse:		ptrNewCommandParseState.Set(KTxtWaitLineParse);			break;
		case EWaitLiteralIngore:	ptrNewCommandParseState.Set(KTxtWaitLiteralIngore);		break;
		case EWaitLineIgnore:		ptrNewCommandParseState.Set(KTxtWaitLineIgnore);		break;
		case ECommandComplete:		ptrNewCommandParseState.Set(KTxtCommandComplete);		break;
		}
	
	_LIT8(KLogFormat, "CImapCommand::iParseState %S ==> %S");
	__LOG_FORMAT((iLogId, KLogFormat, &ptrOldCommandParseState, &ptrNewCommandParseState));
	
#endif //__IMAP_LOGGING

	iParseState = aParseState;
	}

CImapCommand::TParseState CImapCommand::ParseState() const
	{
	return iParseState;
	}

void CImapCommand::SetMessageFlagsChanged()
	{
	if (iSelectedFolderData != NULL)
		{
		__LOG_TEXT(iLogId, "CImapCommand - SetMessageFlagsChanged");
		iSelectedFolderData->SetMessageFlagsChanged(ETrue);
		}
	}

//
//
//

CImapCommandEx::CImapCommandEx(CImapFolderInfo* aSelectedFolderData, TInt aLogId)
	: CImapCommand(aSelectedFolderData, aLogId)
	{}

CImapCommand::TParseBlockResult CImapCommandEx::DoParseBlockL(const TDesC8& /*aData*/)
	{
	TParseBlockResult result = ENotRecognised;
	
	switch (ParseState())
		{
		case EWaitStartResponse:
			{
			result = ParseStartResponseL();
			}
			break;
		case EWaitLiteralParse:
			{
			__LOG_TEXT(iLogId, "CImapCommand - calling ParseLiteralBlockL()");
			ParseLiteralBlockL();
			result = EResponseIncomplete;
			}
			break;
		case EWaitLineParse:
			{
			__LOG_TEXT(iLogId, "CImapCommand - calling ParseLineFollowingLiteralL()");
			TBool wantMoreData = ParseLineFollowingLiteralL();
			result = wantMoreData ? EResponseIncomplete : ECompleteUntagged;
			}
			break;
		default:
			{
			// This method should not be called for any other parse state
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandInvalidParseState9));
			}
			break;
		}
		
	return result;
	}
	
CImapCommand::TParseBlockResult CImapCommandEx::ParseStartResponseL()
	{
	TParseBlockResult result = ENotRecognised;
	
	TInt tagId = 0;
	switch (GetTagTypeL(tagId))
		{
		case ETagged:
			{
			__LOG_TEXT(iLogId, "CImapCommand - calling ParseTaggedResponseL()");
			TBool commandComplete = ParseTaggedResponseL(tagId);
			
			// If this tagged response does not complete the command (e.g. during fetch body)
			// then treat it as if it were an untagged resopnse.
			result = commandComplete ? ECompleteTagged : ECompleteUntagged;
			}
			break;
		case EUntagged:
			{
			__LOG_TEXT(iLogId, "CImapCommand - calling ParseUntaggedResponseL()");
			result = ParseUntaggedResponseL();
			}
			break;
		case EContinuation:
			{
			__LOG_TEXT(iLogId, "CImapCommand - calling ParseContinuationResponseL()");
			ParseContinuationResponseL();
			result = ECompleteUntagged;
			}
			break;
		default:
			{
			// unexpected tag type value
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandInvalidTagType));
			CorruptDataL();
			break;
			}
		}
		
	return result;
	}
	
/**
Calls the Default handler with no further processing.
@see BaseParseTaggedResponseL()
@param aTagId The incoming tag id.
@return ETrue - indicating that the command is completed by this tagged response.
*/
TBool CImapCommandEx::ParseTaggedResponseL(TInt aTagId)
	{
	BaseParseTaggedResponseL(aTagId);
	return ETrue;
	}

/**
Default handler for a tagged response.
Records the response status code (OK/NO/BAD)
And checks that the incoming tag id matches the sent tag id.
Can be called by subclasses.
@param aTagId The incoming tag id.
*/	
void CImapCommandEx::BaseParseTaggedResponseL(TInt aTagId)
	{
	// Check the tag id
	if (aTagId != iTagId)
		{
		// Unexpected tag id
		CorruptDataL();
		}

	// Fetch and check the response code
	iResponseCode = GetResponseStateCode();
	if (iResponseCode == EImapResponseNone)
		{
		// Was expecting one of OK/NO/BAD, but didn't get it.  This is a parse error.
		CorruptDataL();
		}
	}

/**
Default handler for a continuation response.
Always leaves with KErrImapCorrupt, because by default commands do not expect continuation responses.
*/
void CImapCommandEx::ParseContinuationResponseL()
	{
	CorruptDataL();
	}

/**
Default handler for an untagged response.
Always treats the response as unrecognised, causing ParseUnhandledBlockL() to be called later.
This default is used by commands that expect no specific responses.
@return ENotRecognised
*/
CImapCommand::TParseBlockResult CImapCommandEx::ParseUntaggedResponseL()
	{	
	return ENotRecognised;
	}

/**
Default handler for a block of literal data.
Always leaves with KErrImapCorrupt, because by default commands do not expect literal data.
*/
void CImapCommandEx::ParseLiteralBlockL()
	{
	CorruptDataL();
	}

/**
Default handler for a line following a literal block.
Always leaves with KErrImapCorrupt, because by default commands do not expect literal data, let alone lines following literal data.
@return because this implementation of the method always leaves, no value can ever be returned.
*/
TBool CImapCommandEx::ParseLineFollowingLiteralL()
	{
	CorruptDataL();
	return EFalse;
	}

/***
To get the present TagId of the Command object.
@return the TagId of the Command object.
*/	
TInt CImapCommand::GetTagId() 
	{
	return iTagId ;
	}

