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

#include "cimapfetchbody.h"
#include "moutputstream.h"
#include "cimapsession.h"
#include "cimapsessionconsts.h"
#include "cimapsettings.h"
#include "cimapmimeheaderfieldsparser.h"
#include "cimapmimeheaderfields.h"
#include "cfetchbodyinfo.h"
#include "cimapfetchbodyresponse.h"
#include "cimaplogger.h"
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
#include "cimapcapabilityinfo.h"
#endif

const TInt KMaxTagIdSize = 8;
const TInt KDefaultMaxFetchSize = 20480;
const TInt KOutstandingRequests = 2;
const TInt KOutstandingBinaryFetchRequests = 1;

_LIT8(KCommandFetch, "%S UID FETCH %d (BODY[%S]<%d.%d>)\r\n");
_LIT8(KCommandFetchWithMime, "%S UID FETCH %d (BODY[%S]<%d.%d> BODY[%S.MIME])\r\n");
_LIT8(KCommandFetchPeek, "%S UID FETCH %d (BODY.PEEK[%S]<%d.%d>)\r\n");
_LIT8(KCommandFetchPeekWithMime, "%S UID FETCH %d (BODY.PEEK[%S]<%d.%d> BODY.PEEK[%S.MIME])\r\n");
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
_LIT8(KCommandFetchBinary, "%S UID FETCH %d (BINARY[%S]<%d.%d>)\r\n");
_LIT8(KCommandFetchBinaryPeek, "%S UID FETCH %d (BINARY.PEEK[%S]<%d.%d>)\r\n");
#endif
_LIT8(KStartSection,"[");
_LIT8(KEndSection,"]");
_LIT8(KStartLiteral,"{");
_LIT8(KEndLiteral,"}");
_LIT8(KStartOrigin,"<");
_LIT8(KEndOrigin,">");
 
CImapFetchBody* CImapFetchBody::NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId, TUint aMessageUid,TBool aPeek, CFetchBodyInfo& aFetchBodyInfo, CImapFetchBodyResponse& aFetchBodyResponse, CImapSettings& aImapSettings, CImapMailStore& aImapMailStore, CImapSession& aParent)
	{
	CImapFetchBody* self = new (ELeave) CImapFetchBody(aSelectedFolderData, aLogId, aMessageUid, aPeek, aFetchBodyInfo, aFetchBodyResponse, aImapSettings, aImapMailStore, aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
CImapFetchBody::CImapFetchBody(CImapFolderInfo* aSelectedFolderData, TInt aLogId, TUint aMessageUid, TBool aPeek, CFetchBodyInfo& aFetchBodyInfo, CImapFetchBodyResponse& aFetchBodyResponse, CImapSettings& aImapSettings, CImapMailStore& aImapMailStore, CImapSession& aParent)
	: CImapCommand(aSelectedFolderData, aLogId),
	iMessageUid(aMessageUid),
	iPeek(aPeek),
	iSizeToFetch(aFetchBodyInfo.SizeToFetch()),
	iImapSettings(aImapSettings),
	iFetchBodyInfo(aFetchBodyInfo),
	iImapMailStore(aImapMailStore),
	iParent(aParent),
	iFetchBodyResponse(aFetchBodyResponse),
	iSendFetch(ETrue),
	iCancelled(EFalse)
	{
	}
	
CImapFetchBody::~CImapFetchBody()
	{
	// ensure that iImapMailStore will not call StoreOperationComplete() 
	// on this object now that it is being destoyed.
	
	delete iHeaderFieldsParser;
	delete iBuf;
	iTagIds.Reset();
	}

/**
Overrides CImapCommand::Cancel() by cancelling any outstanding mail store operation.
*/
void CImapFetchBody::Cancel()
	{
	__LOG_TEXT(iLogId, "CImapFetchBody::Cancel()"); // Overrides CImapCommand::Cancel()
	iCancelled = ETrue;
	iImapMailStore.CancelRequest(*this);
	CImapCommand::Cancel();
	}

void CImapFetchBody::StoreOperationComplete(TMsvId /*aId*/,TInt aErrorCode)
	{
	__LOG_FORMAT((iLogId, "CImapFetchBody::StoreOperationComplete aErrorCode = %d",aErrorCode));
	iStoreComplete = ETrue;
	
	// Complete early if there is an error
	if (aErrorCode != KErrNone)
		{
		__LOG_TEXT(iLogId, "CImapFetchBody::StoreOperationComplete - ERROR: Completing Early");
		iParent.FetchBodyOperationComplete(aErrorCode);
		}
	// Otherwise complete only if the last tagged OK has been received.
	else if(ParseState() == ECommandComplete)
		{
		// Call the session to let it know we are done
		__ASSERT_DEBUG(iRequestCount == iTotalRequests && iOutstandingRequests == 0, TImapServerPanic::ImapPanic(TImapServerPanic::EStoreOperationCompleteWithPendingRequest) );
		iParent.FetchBodyOperationComplete(aErrorCode);
		}
	}
	
TBool CImapFetchBody::IsStoreOperationComplete()
	{
	return iStoreComplete;
	}
	
void CImapFetchBody::ConstructL()
	{
	// obtain
	iImapSettings.GetTransportBufferSizesL(iMaxFetchSize, iMaxOutstandingRequests);

#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)		
	// check for BINARY capability
	const CImapCapabilityInfo& capabilityInfo = iParent.CapabilityInfo();
	TBool binaryCapExist = capabilityInfo.QueryFlag(CImapCapabilityInfo::EBinaryCap);		
#endif
	
	// if either parameter is undefined, resort to using the defaults.
	if(iMaxFetchSize==0)
		{
		iMaxFetchSize=KDefaultMaxFetchSize;	
		}
	
	if(iMaxOutstandingRequests==0)
		{
		iMaxOutstandingRequests=KOutstandingRequests;	
		}

	//calculate the number of chunks that will be needed
	iTotalRequests=TotalRequestsRequired(iSizeToFetch);
		
	__LOG_FORMAT((iLogId, "CImapFetchBody::CImapFetchBody iTotalRequests = %d",iTotalRequests));
	
	
	if(iFetchBodyInfo.IsText())
		{
		// check if chunk storage mechanism is enabled.
		if(iImapSettings.StorePlainText())
			{
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
			if(binaryCapExist && iFetchBodyInfo.IsText())
				{
				iImapMailStore.InitialiseStorePlainBodyTextL(iTotalRequests,iImapSettings,iFetchBodyInfo,iLogId,*this,*this, ETrue);
				}
			else
#endif
				{
				iImapMailStore.InitialiseStorePlainBodyTextL(iTotalRequests,iImapSettings,iFetchBodyInfo,iLogId,*this,*this);
				}			
			}
		else
			{
			if(iImapSettings.Store8BitData())
				{
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
				if(binaryCapExist && iFetchBodyInfo.IsText())
					{
					iImapMailStore.InitialiseStoreBody8L(iTotalRequests,iImapSettings,iFetchBodyInfo,iLogId,*this,ETrue);
					}
				else
#endif
					{
					iImapMailStore.InitialiseStoreBody8L(iTotalRequests,iImapSettings,iFetchBodyInfo,iLogId,*this);
					}						
				}
			else
				{
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
				if(binaryCapExist && iFetchBodyInfo.IsText())
					{
					iImapMailStore.InitialiseStoreBody16L(iTotalRequests,iImapSettings,iFetchBodyInfo,iLogId,*this,ETrue);		
					}
				else			
#endif
					{	
					iImapMailStore.InitialiseStoreBody16L(iTotalRequests,iImapSettings,iFetchBodyInfo,iLogId,*this);		
					}
				}
			}
		}
	else
		{
		iImapMailStore.InitialiseStoreAttachmentL(iTotalRequests,iImapSettings,iFetchBodyInfo,iLogId,*this);	
		}
	}

TInt CImapFetchBody::TotalRequestsRequired(TInt aSize)
	{
	
	TInt chunkNumber = aSize / iMaxFetchSize;
	
	if( (aSize % iMaxFetchSize)>0)
		{
		//add a chunk for the last bit of data
		++chunkNumber;
		}	

	return chunkNumber;	
	}


TInt CImapFetchBody::CalculateChunk(TInt aPos)
	{
	return aPos / iMaxFetchSize;
	}

/** 
Formats and sends the IMAP UID FETCH command.
@param aTagId Command sequence id which will be send along with the IMAP command.
@param aStream A wrapper for the outbound stream of a connected socket, using which 
the command will be send to the server
*/
void CImapFetchBody::SendMessageL(TInt aTagId, MOutputStream& aStream)
	{	
	iOutStream=&aStream;
	
	if(iCancelled)
	{
	   return; 
	}
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)		
	// check for BINARY capability
	const CImapCapabilityInfo& capabilityInfo = iParent.CapabilityInfo();
	TBool binaryCapExist = capabilityInfo.QueryFlag(CImapCapabilityInfo::EBinaryCap);		
	if(binaryCapExist && iFetchBodyInfo.IsText())
		{
		// if message body part is downloaded using FETCH BINARY, then there should be only one
		// Outstanding Request
		iOutstandingRequests = KOutstandingBinaryFetchRequests;
		}
	else
#endif
		{
		//iOutstandingRequests is the smaller of iMaxOutstandingRequests and the total chunk count
		iOutstandingRequests = iTotalRequests>iMaxOutstandingRequests ? iMaxOutstandingRequests : iTotalRequests;
		}
	// SendMessageL will increment the tag ID as the first thing it does, so we
	// should set it to 1 lower than the fist tag we want to send.
	iTagId = aTagId - 1;

	SendMessageL();	
	}
	
void CImapFetchBody::SendMessageL()
	{
	// Set the tag ID to use in the next message
	++iTagId;

	_LIT8(KTagFormatId, "%d");
	TBuf8<KMaxTagIdSize>tagIdBuffer;
	tagIdBuffer.Format(KTagFormatId,iTagId);
	
	iTagIds.InsertInOrder(iTagId);
	
	
	//create fetch command based on settings	
	//the offset from which we want to fetch data
	TInt offset = iRequestCount*iMaxFetchSize;
	
	// calclulate the size to fetch in this request,
	// default to max fetch size.
	TUint sizeToFetch = iMaxFetchSize;
	
	TInt bufLength = 0;
	bufLength += iFetchBodyInfo.RelativePath()->Length();
	bufLength += TagLength(offset);
	bufLength += TagLength(sizeToFetch);
	bufLength += tagIdBuffer.Length();

#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)	
	// check for BINARY capability
	// Issue binary fetch for plain/text part only
	const CImapCapabilityInfo& capabilityInfo = iParent.CapabilityInfo();
	TBool binaryCapExist = capabilityInfo.QueryFlag(CImapCapabilityInfo::EBinaryCap);	
#endif
	if (iRequestCount == 0)
		{
		bufLength += iFetchBodyInfo.RelativePath()->Length();
		
		if (iPeek)
			{
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)	
			if(binaryCapExist && iFetchBodyInfo.IsText())
				{
				bufLength += KCommandFetchBinaryPeek().Length();
				}
			else
#endif
				{
				bufLength += KCommandFetchPeekWithMime().Length();	
				}
			}
		else
			{
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)	
			if(binaryCapExist && iFetchBodyInfo.IsText())
				{	
				bufLength += KCommandFetchBinary().Length();
				}
			else
#endif
				{
				bufLength += KCommandFetchWithMime().Length();
				}			
			}
		}
	else
		{
		if(iPeek)
			{
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)	
			if(binaryCapExist && iFetchBodyInfo.IsText())
				{
				bufLength += KCommandFetchBinaryPeek().Length();
				}
			else
#endif
				{
				bufLength += KCommandFetchPeek().Length();	
				}			
			}
		else
			{
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)	
			if(binaryCapExist && iFetchBodyInfo.IsText())
				{
				bufLength += KCommandFetchBinary().Length();							
				}
			else
#endif
				{
				bufLength += KCommandFetch().Length();			
				}			
			}
		}

	//now create the command	
	HBufC8* buffer = HBufC8::NewL(bufLength);				
	delete iOutputBuffer;
	iOutputBuffer=buffer;

	if (iRequestCount == 0)
		{
		if(iPeek)
			{
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)	
			if(binaryCapExist && iFetchBodyInfo.IsText())
				{
				iOutputBuffer->Des().Format(KCommandFetchBinaryPeek, &tagIdBuffer, iMessageUid, iFetchBodyInfo.RelativePath(), offset, sizeToFetch, iFetchBodyInfo.RelativePath());				
				}
			else
#endif
				{
				iOutputBuffer->Des().Format(KCommandFetchPeekWithMime, &tagIdBuffer, iMessageUid, iFetchBodyInfo.RelativePath(), offset, sizeToFetch, iFetchBodyInfo.RelativePath());
				}			
			}
		else
			{
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)	
			if(binaryCapExist && iFetchBodyInfo.IsText())
				{
				iOutputBuffer->Des().Format(KCommandFetchBinary, &tagIdBuffer, iMessageUid, iFetchBodyInfo.RelativePath(), offset, sizeToFetch, iFetchBodyInfo.RelativePath());	
				}
			else
#endif
				{
				iOutputBuffer->Des().Format(KCommandFetchWithMime, &tagIdBuffer, iMessageUid, iFetchBodyInfo.RelativePath(), offset, sizeToFetch, iFetchBodyInfo.RelativePath());	
				}
			}
		}
	else
		{
		if(iPeek)
			{
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)	
			if(binaryCapExist && iFetchBodyInfo.IsText())
				{
				iOutputBuffer->Des().Format(KCommandFetchBinaryPeek, &tagIdBuffer, iMessageUid, iFetchBodyInfo.RelativePath(), offset, sizeToFetch);	
				}
			else
#endif
				{
				iOutputBuffer->Des().Format(KCommandFetchPeek, &tagIdBuffer, iMessageUid, iFetchBodyInfo.RelativePath(), offset, sizeToFetch);	
				}			
			}
		else
			{
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)	
			if(binaryCapExist && iFetchBodyInfo.IsText())
				{
				iOutputBuffer->Des().Format(KCommandFetchBinary, &tagIdBuffer, iMessageUid, iFetchBodyInfo.RelativePath(), offset, iMaxFetchSize);								
				}
			else
#endif
				{
				iOutputBuffer->Des().Format(KCommandFetch, &tagIdBuffer, iMessageUid, iFetchBodyInfo.RelativePath(), offset, iMaxFetchSize);				
				}			
			}
		}

	delete iBuf;
	iBuf = NULL;
	iReceivedMimeHeaders = EFalse;

	//send the command to the server
	iOutStream->SendDataReq(*iOutputBuffer);
	++iRequestCount;
	}

/**
@param aData Will contain a single line of response from the server for LOGIN command without \r\n.
@return will be any one of this
	1) If the next expected chunk is a literal block, ParseMessageL() will return the size of the block it expects.
	2) If the next expected chunk is a line, ParseMessageL() will return 0, and Result() will return EImapResponseNone.
	3) If no further data is expected (e.g. the OK or error tag has been received) then ParseMessageL() will return 0, 
	   and Result() will return one of EImapResponseOk, EImapResponseNo or EImapResponseBad.
*/
CImapCommand::TParseBlockResult CImapFetchBody::DoParseBlockL(const TDesC8& aData)
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
			
			ASSERT(iState == EStateDataItemLine);
			
			// If we get EResponseIncomplete then allow the current EStateDataItemLine state to
			// drop through to ProcessDataItems()
			// otherwise, EStateComplete will take us straight to the return.
			if (resultCode != EResponseIncomplete)
				{
				iState = EStateComplete;
				}
			}
			break;
		case EStateBodyLiteral:
			{
			// Bump progress: bytesdone is *encoded* length, so we just use the encoded length
			iFetchBodyInfo.IncrementBytesFetched(aData.Length());
			
			__ASSERT_DEBUG(aData.Length() == iLiteralSize, TImapServerPanic::ImapPanic(TImapServerPanic::ETParseBlockResultInvalidLiteralSize) );

			ProcessBodyLiteralL(aData);
			resultCode = EResponseIncomplete; // always expect more data after a literal
			}
			break;
		case EStateMime:
			{
			ProcessRestOfMimeL(iUnparsedData);
			resultCode = EResponseIncomplete;
			}
			break;
		case EStateFetchNextDataItemLine:
			{
			// Fetch is over.  Get ready to process next data item.
			iUnparsedData.Set(aData);
			GetAndStoreNextPart();
			
			iState = EStateDataItemLine;
			}
			break;
		default:
			{
			// unexpected state
			ASSERT(EFalse);
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
		//if we still have more requests to issue send the next one now
		
		if(resultCode == ECompleteTagged)
			{
			if (iResponseCode == EImapResponseOk)
				{
				++iResponseCount;
				// If we received some MIME headers, we may need to store them
				// with the CAF framework
				if ((iReceivedMimeHeaders) && (iFetchBodyInfo.Caf() != NULL) && (iFetchBodyInfo.Caf()->Registered()))
					{
					WriteMimeHeadersToCafL();
					}

				// Store the body data if we received it
				if (iBuf != NULL)
					{
					TInt extraFetchRequestCount = 0;
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)					
					// check for BINARY capability
					const CImapCapabilityInfo& capabilityInfo = iParent.CapabilityInfo();
					TBool binaryCapExist = capabilityInfo.QueryFlag(CImapCapabilityInfo::EBinaryCap);					
					if(binaryCapExist && iFetchBodyInfo.IsText() && iPreviousLiteralBytesReceived < KDefaultMaxFetchSize)
						{
						extraFetchRequestCount = iTotalRequests - iRequestCount;
						// iTotalRequests, iRequestCount and iResponseCount should be same at this stage
						// iResponseCount will be same as iRequestCount
						iTotalRequests=iRequestCount;
						//reset iPreviousLiteralBytesReceived to zero
						iPreviousLiteralBytesReceived=0;										
						}
#endif
						
					StoreBodyDataL(extraFetchRequestCount);										
					
					if (iRequestCount<iTotalRequests)
						{
						// if there are outstanding requests already then just add this request to the count
						++iOutstandingRequests;
						
						// If iOutstandingRequests is greater than one, this means a write operation is in progress
						// and the message will be sent when SendDataCnf() is called.
						if (iSendFetch && (iOutstandingRequests == 1))
							{
							SendMessageL();
							}

						resultCode=ECompleteUntagged;
						iResponseCode=EImapResponseNone;
						}
					// if there are remaining requests yet to be received then tell the session were not finished.
					else if(iResponseCount<iTotalRequests)
						{
						resultCode = ECompleteUntagged;	
						iResponseCode=EImapResponseNone;	
						}
					}
				// if there is no body part to be stored but server sent a OK responsethen set 
				// iStoreComplete to ETrue and Cancel the Request.If more than one FETCH was sent
				// then Flush the state so that all incoming server data is discarded.
				else
					{
					iStoreComplete = ETrue;
					iImapMailStore.CancelRequest(*this);	
					
					// Check the tag id
					if(iTagIds.Count())
						{
						EnterFlushingState();
						resultCode = ECompleteUntagged;
						iResponseCode = EImapResponseNone;
						}		
					}
				}
			else
				{
				iImapMailStore.CancelRequest(*this);
				}
			}
			
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
			iFetchBodyResponse.SetMessageFlagInfo(iMessageFlagInfo);
			
			// Note: iUidDataItemFoundInResponse is NOT reset here
			// 		 Instead iUidDataItemFoundInResponse is set to EFalse just prior to calling ProcessStartL() - i.e. at the start of a new response.
			}
		else
			{
			// Record that an unsolicited FETCH was received - so that a sync will be triggered after this command.
			__LOG_TEXT(iLogId, "CImapFetchBody::DoParseBlockL() - Found unsolicited FETCH FLAGS");
			SetMessageFlagsChanged();
			}
		}

	return resultCode;
	}

/**
Returns the number of tagged responses that are currently expected.
Because CImapFetchBody uses pipelining - with many simulataneous request running at once
it will be expect a tagged response for each request that is still running on the server.
@return the number of tagged responses that are currently expected.
*/
TInt CImapFetchBody::NumberOfTaggedResponsesExpected() const
	{
	return iTagIds.Count();
	}
	
CImapCommand::TParseBlockResult CImapFetchBody::ProcessStartL()
	{
	TParseBlockResult result = ENotRecognised;

	TInt tagId = 0;	
	TTagType tagged = GetTagTypeL(tagId);
	switch(tagged)
		{
		case ETagged:
			{
			// Check the tag id
			TInt tagPos=iTagIds.FindInOrder(tagId);
			if(tagPos!=KErrNotFound)
				{
				iTagIds.Remove(tagPos);	
				}
			else
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
				
			result =ECompleteTagged;		
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

CImapCommand::TParseBlockResult CImapFetchBody::ProcessDataItemsL()
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
	
		// check if the part starts 'BODY['	
		else if (iCurrentPart.Find(KImapTxtBody)==0)
			{
			//is it the body or the body.mime?
			if(iCurrentPart.Find(KImapTxtMime) != KErrNotFound )
				{
				ProcessStartOfMimeL();
				}
			else
				{
				ProcessBodyL();	
				}
			}
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
		// check if the part starts 'BINARY['	
		else if (iCurrentPart.Find(KImapTxtBinary)==0)
			{
			// no mime to process for fetch binary
			ProcessBodyL();	
			}
#endif
	
		// Only fetch the next part if we're still searching for data items.
		if (iState == EStateDataItemLine)
			{
			foundPart = GetAndStoreNextPart();
			}
		}
		
	if (!foundPart && iState == EStateDataItemLine)
		{
		if(iBuf && iUnexpectedFormat)
			{
			iState = EStateFetchNextDataItemLine;
			iUnexpectedFormat = EFalse;
			}
		resultCode = ECompleteUntagged;
		}
	iUnexpectedFormat = EFalse;	
	return resultCode;
	}


void CImapFetchBody::ProcessStartOfMimeL()
	{
	//look for the body section that is being returned 
	TInt secStart=iCurrentPart.Find(KStartSection);
	TInt secEnd=iCurrentPart.Find(KImapTxtMime);

	if(secStart==KErrNotFound || secEnd==KErrNotFound)
		{
		CorruptDataL();
		}

	TPtrC8 section = iCurrentPart.Mid(secStart + 1, secEnd - secStart - 1);

	//check the section is what we asked for
	if(section.CompareF(*iFetchBodyInfo.RelativePath())  != 0)
		{
		CorruptDataL();
		}

	// Peek the next part. We don't want to consume it as we may need
	// to pass it to the header fields parser.
	iCurrentPart.Set(PeekNextPart());
	if (iCurrentPart.Length() == 0)
		{
		CorruptDataL();
		}

	iReceivedMimeHeaders = ETrue;
	
	// If the last character is ')' then we're at the last data item in the list.
	// Consume the character so that the rest of the data item can be interpreted.
	if (iCurrentPart.Right(1).CompareF(KImapTxtCloseBracket) == 0)
		{
		iCurrentPart.Set(iCurrentPart.Left(iCurrentPart.Length() - 1));
		}
	
	// Check if data part is NIL or "" for empty string
	if (iCurrentPart.CompareF(KImapTxtNil) == 0 || iCurrentPart.CompareF(KImapTxtEmptyStringAsDoubleQuotePair) == 0)
		{
		// Consume the NIL part
		GetAndStoreNextPart();

		// Create empty MIME header fields
		CImapMimeHeaderFields* fields = CImapMimeHeaderFields::NewL();
		iFetchBodyResponse.SetHeaderFields(fields);

		// May be more data items coming up
		iState = EStateDataItemLine;
		}
	else
		{
		// Pass the rest of the line to the header fields parser
		iHeaderFieldsParser = CImapMimeHeaderFieldsParser::NewL(iFetchBodyResponse, iLogId);
		iState = EStateMime;
		ProcessRestOfMimeL(iUnparsedData);
		}
	}

void CImapFetchBody::ProcessRestOfMimeL(const TDesC8& aData)
	{
	TBool wantsMore = iHeaderFieldsParser->ProcessBlockL(aData);

	if (!wantsMore)
		{
		delete iHeaderFieldsParser;
		iHeaderFieldsParser = NULL;
		iState = EStateFetchNextDataItemLine;
		}
	}

void CImapFetchBody::ProcessBodyL()
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	{//BODY[1]<0> or BINARY[1]<0>
#else
	{//BODY[1]<0>
#endif	
	//look for the body section that is being returned 
	TInt secStart=iCurrentPart.Find(KStartSection);
	TInt secEnd=iCurrentPart.Find(KEndSection);
	
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	const CImapCapabilityInfo& capabilityInfo = iParent.CapabilityInfo();
	if(capabilityInfo.QueryFlag(CImapCapabilityInfo::EBinaryCap) && iFetchBodyInfo.IsText())
		{
		if(secStart!=KImapTxtBinary().Length() - 1  || secEnd <= secStart + 1)
			{
			CorruptDataL();
			}
		}
	else
#endif 
		{
		if(secStart!=KImapTxtBody().Length() - 1  || secEnd <= secStart + 1)
			{
			CorruptDataL();
			}						
		}
			
	TPtrC8 section=iCurrentPart.Mid(secStart+1,secEnd-secStart-1);
	
	//check the section is what we asked for
	if(section.CompareF(*iFetchBodyInfo.RelativePath())  != 0)
		{
		CorruptDataL();
		}
	
	//get the origin octet of the form <origin>, this may not exist, if its not
		//then the origin is at the start of the data
	TInt originStart=iCurrentPart.Find(KStartOrigin);
	if(originStart==KErrNotFound)
		{
		//the origin octet will be 0, the data will  go in the first chunk
		iCurrentChunk=0;		
		}
	else
		{
		if(originStart != secEnd + 1)
			{
			CorruptDataL();
			}
		TInt originEnd=iCurrentPart.Find(KEndOrigin);
		if(originEnd==KErrNotFound || originEnd != iCurrentPart.Length() - 1 )
			{
			CorruptDataL();	
			}
		if(originEnd <= originStart + 1)
			{
			CorruptDataL();	
			}
			
		TPtrC8 originPtr=iCurrentPart.Mid(originStart+1,originEnd-originStart-1);
		TLex8 originToInt(originPtr);
	
		TInt origin=0;
		TInt err = originToInt.Val(origin);
		if (err != KErrNone)
			{
			// Was expecting originPtr to be a number
			CorruptDataL();
			}
		
		//set the chunk number
		//if the origin was blank ie. <> then this is set to 0, the start of the data 
		iCurrentChunk = CalculateChunk(origin);
		}
		
	//now look for the size of the literal
	TBool foundPart = GetAndStoreNextPart();
	if(!foundPart)
		{
		CorruptDataL();	
		}
		
	TInt litStart=iCurrentPart.Find(KStartLiteral);
	TInt litEnd=iCurrentPart.Find(KEndLiteral);
	
	if(litStart==KErrNotFound && litEnd==KErrNotFound)
		{
		// This may be the data item
		ProcessBodyLiteralL(iCurrentPart);
		iState = EStateDataItemLine;
		iUnexpectedFormat = ETrue;
		}
	else
		{
		if(litStart==KErrNotFound || litEnd==KErrNotFound)
			{
			CorruptDataL();	
			}

		if(litEnd <= litStart + 1)
			{
			CorruptDataL();	
			}

		TPtrC8 litPtr=iCurrentPart.Mid(litStart+1,litEnd-litStart-1);
		TLex8 litSizeToInt(litPtr);
		
		TInt err = litSizeToInt.Val(iLiteralSize);
		if (err != KErrNone)
			{
			// Was expecting litPtr to be a number
			CorruptDataL();
			}
		
		if(GetAndStoreNextPart())
			{
			ProcessBodyLiteralL(iCurrentPart);
			iState = EStateDataItemLine;
			iUnexpectedFormat = ETrue;
			}
		else
			{
			//now wait for the litereral
			iState = EStateBodyLiteral;
			}
		}
	}
	
void CImapFetchBody::ProcessBodyLiteralL(const TDesC8& aData)
	{
	delete iBuf;
	iBuf = NULL;
	iBuf = aData.AllocL();
	
	//now wait for the line that always follows a literal
	iState = EStateFetchNextDataItemLine;
	}

/**
Move to the next part
@return whether a part was found
*/
TBool CImapFetchBody::GetAndStoreNextPart()
	{
	iCurrentPart.Set(GetNextPart());
	return (iCurrentPart.Length() > 0) ? ETrue : EFalse;
	}

void CImapFetchBody::ProcessFlagsL()
	{
	iUnparsedData.Set(iMessageFlagInfo.ParseFlagsL(iUnparsedData));
	}

void CImapFetchBody::ProcessUidL()
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

void CImapFetchBody::WriteMimeHeadersToCafL()
	{
	CImapMimeHeaderFields* fields = iFetchBodyResponse.HeaderFields();
	if (fields != NULL)
		{
		TPtrC8 name;
		TPtrC8 value;

		fields->RestartGetNextField();
		while (fields->GetNextField(name, value))
			{
			__LOG_FORMAT((iLogId, "Add CAF metadata: %S %S", &name, &value));
			iFetchBodyInfo.Caf()->AddToMetaDataL(name, value);
			}
		}
	}

void CImapFetchBody::StoreBodyDataL(TBool aExtraFetchRequestCount)
	{
	// We are going to pass the buffer to the mail store, so set our
	// buffer to NULL so that we don't try to delete it if the store
	// routine leaves.
	HBufC8* buf(iBuf);
	iBuf = NULL;

	if(iFetchBodyInfo.IsText())
		{
		if(iImapSettings.StorePlainText())
			{
			iSendFetch = iImapMailStore.StorePlainBodyTextL(buf,iFetchBodyInfo.PartId(),iCurrentChunk,aExtraFetchRequestCount);
			}
		else
			{
			if(iImapSettings.Store8BitData())
				{
				iImapMailStore.StoreBodyChunk8L(buf,iFetchBodyInfo.PartId(),iCurrentChunk,aExtraFetchRequestCount);
				}
			else
				{
				iImapMailStore.StoreBodyChunk16L(buf,iFetchBodyInfo.PartId(),iCurrentChunk,aExtraFetchRequestCount);	
				}
			}
		}
	else //attachments
		{
		iImapMailStore.StoreAttachmentChunkL(buf,iFetchBodyInfo.PartId(),iCurrentChunk);
		}
	}

/**
If pipelining is enabled then this method will send the next fetch request to the server after confirmation of the last request having been sent.
@return void
*/ 


void CImapFetchBody::SendDataCnfL()
	{
	ASSERT(iOutstandingRequests>0);
	--iOutstandingRequests;
	//if we want more requests outstanding then send the next one now
	if(iOutstandingRequests>0 && iSendFetch)
		{
		SendMessageL();	
		}
	}
/**
This method will enable the FETCH command to be send to the server if it was
disabled by CImapMailStore due to reciept of out-of-order chunks.
@return void
*/	
void CImapFetchBody::EnableSendFetch()
	{
	iSendFetch = ETrue;
	}
