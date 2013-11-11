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

#include "cimapfetchbodystructurebase.h"
#include "cimapfetchresponse.h"
#include "cimapbodystructurebuilder.h"
#include "cimaprfc822headerfieldsparser.h"
#include "moutputstream.h"
#include "cimapsessionconsts.h"
#include "imappaniccodes.h"
#include "cimaplogger.h"

_LIT8(KCommandFetch, "%d UID FETCH %S (UID FLAGS BODYSTRUCTURE BODY.PEEK[HEADER.FIELDS (%S)])\r\n");
const TInt KCommandFetchFormatLength=6;

CImapFetchBodyStructureBase::CImapFetchBodyStructureBase(CImapFolderInfo* aSelectedFolderInfo, TInt aLogId, const TDesC8& aHeaderFields)
	: CImapCommand(aSelectedFolderInfo, aLogId)
	, iHeaderFields(aHeaderFields)
	, iState(EStateDataItemLine)
	{
	}

CImapFetchBodyStructureBase::~CImapFetchBodyStructureBase()
	{
	delete iBodyStructureBuilder;
	delete iHeaderFieldsParser;
	}

/**
@param aTagId Command sequence id which will be send along with the IMAP command.
@param aStream A wrapper for the outbound stream of a connected socket, using which 
the command will be send to the server
*/
void CImapFetchBodyStructureBase::SendMessageL(TInt aTagId, MOutputStream& aStream)
	{
	iTagId = aTagId;

	TInt bufLen = KCommandFetch().Length() - KCommandFetchFormatLength;
	bufLen += TagLength(aTagId);
	bufLen += iSequenceSet->Length();
	bufLen += iHeaderFields.Length();

	__ASSERT_DEBUG(iOutputBuffer==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandOutputBufferNotNull));
	iOutputBuffer = HBufC8::NewL(bufLen);
	iOutputBuffer->Des().Format(KCommandFetch, aTagId, iSequenceSet, &iHeaderFields);

	//send the command to the server
	aStream.SendDataReq(*iOutputBuffer);
	}

/**
@param aData Will contain a single line of response from the server for LOGIN command without \r\n.
@return will be any one of this
	1) If the next expected chunk is a literal block, ParseMessageL() will return the size of the block it expects.
	2) If the next expected chunk is a line, ParseMessageL() will return 0, and Result() will return EImapResponseNone.
	3) If no further data is expected (e.g. the OK or error tag has been received) then ParseMessageL() will return 0, 
	   and Result() will return one of EImapResponseOk, EImapResponseNo or EImapResponseBad.
*/
CImapCommand::TParseBlockResult CImapFetchBodyStructureBase::DoParseBlockL(const TDesC8& aData)
	{
	CImapCommand::TParseBlockResult resultCode(ECompleteUntagged);

	switch (iState)
		{
		case EStateDataItemLine:
			{
			// We are the beginning of a new response, so we can't have found any UID data items yet.
			// So we need to reset the flag here.
			iUidDataItemFoundInResponse = EFalse;
			resultCode = ProcessStartL();
			
			__ASSERT_DEBUG(iState == EStateDataItemLine, TImapServerPanic::ImapPanic(TImapServerPanic::EFetchBodyStructureUnexpectedState));
			
			// If we get EResponseIncomplete then allow the current EStateDataItemLine state to
			// drop through to ProcessDataItems()
			// otherwise, EStateComplete will take us straight to the return.
			if (resultCode != EResponseIncomplete)
				{
				iState = EStateComplete;
				}
			}
			break;
		case EStateBodyStructureLiteral:
		case EStateBodyStructureLine:
			{
			ProcessBodyStructureL(aData);
			resultCode = EResponseIncomplete; // always expect more data after bodystructure.
			}
			break;
		case EStateHeaderFieldsLiteral:
			{
			ProcessHeaderFieldsL(aData);
			resultCode = EResponseIncomplete; // always expect more data after header field literal.
			}
			break;
		case EStateFetchNextDataItemLine:
			{
			// Fetch is over.  Get ready to process next data item.
			iUnparsedData.Set(aData);
			if(GetAndStoreNextPart())
				{
						iState = EStateDataItemLine;
				}
			}
			break;
		default:
			{
			// unexpected state
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::EFetchBodyStructureUnexpectedState));
			}
		}

	// The ProcessXxxL() methods above can change the state.
	// Now we need to check if there are data items to process...
	if (iState == EStateDataItemLine)
		{
		resultCode = ProcessDataItemsL();
		}
	else if (iState == EStateComplete)
		{
		// this response is complete, so the next data (if any) will be a data item line,
		iState = EStateDataItemLine;
		}

	// For complete untagged responses, check whether the UID data item was received.
	// If it was, then this was a genuine response to the UID FETCH command.
	// If it was not received, then this was an unsolicited server event, and the data should be discarded.
	if (resultCode == ECompleteUntagged)
		{
		if (iUidDataItemFoundInResponse)
			{
			// Genuine UID FETCH response - copy UID and FLAG info into the response object
			iFetchResponse->SetMessageFlagInfo(iMessageFlagInfo);
			
			// Note: iUidDataItemFoundInResponse is NOT reset here as its value is needed by CImapFetchMultiBodyStructures::DoParseBlockL()
			// 		 Instead iUidDataItemFoundInResponse is set to EFalse just prior to calling ProcessStartL() - i.e. at the start of a new response.
			}
		else
			{
			// Record that an unsolicited FETCH was received - so that a sync will be triggered after this command.
			__LOG_TEXT(iLogId, "CImapFetchBodyStructureBase::DoParseBlockL() - Found unsolicited FETCH FLAGS");
			SetMessageFlagsChanged();
			}
		}

	return resultCode;
	}

CImapCommand::TParseBlockResult CImapFetchBodyStructureBase::ProcessStartL()
	{
	TParseBlockResult result = ENotRecognised;

	TInt tagId = 0;	
	TTagType tagged = GetTagTypeL(tagId);
	switch(tagged)
		{
		case ETagged:
			{
			// Check the tag id
	 		if (tagId != iTagId)
	 			{
	 			//Unexpected Tagid	
	 			CorruptDataL();
	 			}
	 		
 			// Fetch and check the response code
			iResponseCode = GetResponseStateCode();
			if (iResponseCode == EImapResponseNone)
				{
				// Was expecting one of OK/NO/BAD, but didn't get it.  This is a parse error.
				CorruptDataL();
				}
	 		
			result = ECompleteTagged;	
			}
			break;			
		case EUntagged:
			{
			// Is this a FETCH response?
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
					if (GetAndStoreNextPart())
						{
						if (iCurrentPart[0] == '(')
							{
							iCurrentPart.Set(iCurrentPart.Mid(1));
							}
						else
							{
							// was expecting a bracket, got something else
							CorruptDataL();
							}
						}
					else
						{
						// was expecting a bracket, got nothing
						CorruptDataL();
						}

					result = EResponseIncomplete;
					}
				}
			}
			break;
		case EContinuation:
		default:
			{
			CorruptDataL();
			}
			break;
		}

	// result will be ENotRecognised if tagged not found or untagged FETCH not found.
	return result;
	}

CImapCommand::TParseBlockResult CImapFetchBodyStructureBase::ProcessDataItemsL()
	{
	CImapCommand::TParseBlockResult resultCode = EResponseIncomplete;
	
	TBool foundPart = ETrue;
	while (iState == EStateDataItemLine && foundPart)
		{
		if (iCurrentPart.CompareF(KImapTxtUid) == 0)
			{
			ProcessUidL();
			}
		else if (iCurrentPart.CompareF(KImapTxtFlags) == 0)
			{
			ProcessFlagsL();
			}
		else if (iCurrentPart.CompareF(KImapTxtBodyStructure) == 0)
			{
			ProcessBodyStructureL();
			}
		else if (iCurrentPart.CompareF(KImapTxtBodyHeaderFields) == 0)
			{
			ProcessHeaderFieldsL();
			}
		// Ignore anything else at the moment until we reach 
		// a valid dataItem on the next line.
				
		
		// Only fetch the next part if we're still searching for data items.
		if (iState == EStateDataItemLine)
			{
			foundPart = GetAndStoreNextPart();
			}
		}
		
	if (!foundPart && iState == EStateDataItemLine)
		{
		resultCode = ECompleteUntagged;
		}
		
	return resultCode;
	}

void CImapFetchBodyStructureBase::ProcessFlagsL()
	{
	iUnparsedData.Set(iMessageFlagInfo.ParseFlagsL(iUnparsedData));
	}
	
void CImapFetchBodyStructureBase::ProcessUidL()
	{
	if (GetAndStoreNextPart())
		{
		TInt err = iMessageFlagInfo.SetMessageUid(iCurrentPart);
		if (err == KErrNone)
			{
			iUidDataItemFoundInResponse = ETrue;
			}
		else
			{
			// expected iCurrentPart to be a number representing a UID.
			// but we did not get a number.
			CorruptDataL();
			}
		}
	}
	
void CImapFetchBodyStructureBase::ProcessBodyStructureL()
	{
	iBodyStructureBuilder = CImapBodyStructureBuilder::NewL(*iFetchResponse, iLogId);

	ProcessBodyStructureL(iUnparsedData);
	}

void CImapFetchBodyStructureBase::ProcessBodyStructureL(const TDesC8& aData)
	{
	TBool needsMore = iBodyStructureBuilder->ProcessBlockL(aData);

	if (iState == EStateBodyStructureLiteral)
		{
		// Bodystructure should always request a line after a literal
		__ASSERT_DEBUG(needsMore, TImapServerPanic::ImapPanic(TImapServerPanic::EFetchBodyStructureExpectedRequestForLine));
		
		iState = EStateBodyStructureLine;
		}		
	else if (needsMore)
		{
		// Check the previous state was one we expected
		__ASSERT_DEBUG(iState == EStateDataItemLine || iState == EStateBodyStructureLine, TImapServerPanic::ImapPanic(TImapServerPanic::EFetchBodyStructureUnexpectedState));
		
		iState = EStateBodyStructureLiteral;
		}
	else
		{
		// Check the previous state was one we expected
		__ASSERT_DEBUG(iState == EStateDataItemLine || iState == EStateBodyStructureLine, TImapServerPanic::ImapPanic(TImapServerPanic::EFetchBodyStructureUnexpectedState));

		iState = EStateDataItemLine;
		
		iUnparsedData.Set(iBodyStructureBuilder->UnparsedData());
		
		delete iBodyStructureBuilder;
		iBodyStructureBuilder = NULL;		
		}
	}

void CImapFetchBodyStructureBase::ProcessHeaderFieldsL()
	{
	// Skip past the field titles
	while (GetAndStoreNextPart())
		{
		if (iCurrentPart[iCurrentPart.Length() - 1] == ']')
			{
			break;
			}
		}

	iHeaderFieldsParser = CImapRfc822HeaderFieldsParser::NewL(*iFetchResponse, iLogId);

	return ProcessHeaderFieldsL(iUnparsedData);
	}

void CImapFetchBodyStructureBase::ProcessHeaderFieldsL(const TDesC8& aData)
	{
	TBool needsMore = iHeaderFieldsParser->ProcessBlockL(aData);

	if (iState == EStateDataItemLine)
		{
		// Always expect a literal for the Header Fields data item
		if (!needsMore)
			{
			CorruptDataL();
			}
		
		iState = EStateHeaderFieldsLiteral;
		}
	else if (iState == EStateHeaderFieldsLiteral)
		{
		// Parser should not be expecting more data after a literal
		if (needsMore)
			{
			CorruptDataL();
			}
		
		iState = EStateFetchNextDataItemLine;
		
		iUnparsedData.Set(iHeaderFieldsParser->UnparsedData());
		__ASSERT_DEBUG(iUnparsedData.Length() == 0, TImapServerPanic::ImapPanic(TImapServerPanic::EFetchBodyStructureUnparsedDataExists));
		
		delete iHeaderFieldsParser;
		iHeaderFieldsParser = NULL;		
		}
	else
		{
		// Recovery from this would depend on the state
		__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::EFetchBodyStructureUnexpectedState));
		}
	}
 
/**
Move to the next part
@return whether a part was found
*/
TBool CImapFetchBodyStructureBase::GetAndStoreNextPart()
	{
	iCurrentPart.Set(GetNextPart());
	return (iCurrentPart.Length() > 0) ? ETrue : EFalse;
	}


TBool CImapFetchBodyStructureBase::UidDataItemFoundInResponse()
	{
	return iUidDataItemFoundInResponse;
	}
