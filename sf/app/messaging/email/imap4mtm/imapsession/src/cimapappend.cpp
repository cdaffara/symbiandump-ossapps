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

#include "cimapappend.h"

#include <imcvsend.h>

#include "cimapsessionconsts.h"
#include "moutputstream.h"
#include "cimaplogger.h"
#include "cimapsession.h"

_LIT8(KTxtAppendFormat, "%d APPEND %S {%d}\r\n");

// TCallBack return values
static const TInt KDontCallAgain = 0;
static const TInt KCallAgain = 1;

/**
The factory constructor. Part of two phased construction
*/
CImapAppend* CImapAppend::NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId, CImSendMessage& aSource, const TDesC& aDestinationMailboxName, CImapSession& aParentSession)
// static method
	{
	CImapAppend* self = new(ELeave) CImapAppend(aSelectedFolderData, aLogId, aSource, aParentSession);
	CleanupStack::PushL(self);
	self->ConstructL(aDestinationMailboxName);
	CleanupStack::Pop(self);
	return self;
	}

void CImapAppend::ConstructL( const TDesC& aDestinationMailboxName)
	{
	iLineBufferForSend = HBufC8::NewL(KImMailMaxBufferSize);
	iDestinationMailboxName = EncodeMailboxNameForSendL(aDestinationMailboxName);
	iBackgroundCalculationStepper = CIdle::NewL(CActive::EPriorityIdle);
	}

/**
Constructor.
*/
CImapAppend::CImapAppend(CImapFolderInfo* aSelectedFolderData, TInt aLogId, CImSendMessage& aSource, CImapSession& aParentSession)
	: CImapCommandEx(aSelectedFolderData, aLogId)
	, iSource(aSource)
	, iParentSession(aParentSession)
	, iAppendCommandSendState(EAppendWaitingToStart)
	{	
	}

/**
Destructor.
*/
CImapAppend::~CImapAppend()
	{
	delete iBackgroundCalculationStepper;
	delete iLineBufferForSend;
	delete iDestinationMailboxName;
	}
	
/**
Formats and sends the IMAP APPEND command.
@param aTagId Command sequence id which will be sent along with the IMAP command.
@param aStream A wrapper for the outbound stream of a connected socket, using which
the command will be send to the server
*/	
void CImapAppend::SendMessageL(TInt aTagId, MOutputStream& aStream)
	{
	// From RFC3501
	//	append = "APPEND" SP mailbox [SP flag-list] [SP date-time] SP literal
	//	
	// we have chosen not to supply the optional flag-list or date-time

	__ASSERT_DEBUG(iAppendCommandSendState==EAppendWaitingToStart, TImapServerPanic::ImapPanic(TImapServerPanic::EAppendInvalidState));

	iTagId = aTagId;
	iStream = &aStream;
				
	// Calculate the size of the message
	__LOG_TEXT(iLogId, "CImapAppend::SendMessageL() - Start calculating message size");
	iAppendCommandSendState = EAppendCalculatingSize;
	iBackgroundCalculationStepper->Start(TCallBack(MessageSizeCalculationStep, this));
	}

/**
Static callback method that was requested through iBackgroundCalculationStepper.
It calls the equivalent MessageSizeCalculationStep on the supplied CImapAppend object
which performs a single step of the message size calculation.
@param aSelf The CImapAppend object that requested the asynchronous callback.
@return Wheher this method needs to be called again, to calculate the next step.
*/
TInt CImapAppend::MessageSizeCalculationStep(TAny* aSelf)
// static method
	{
	CImapAppend* self = reinterpret_cast<CImapAppend*>(aSelf);
	TInt callAgain = KDontCallAgain;
	TRAPD(err, callAgain = self->MessageSizeCalculationStepL());
	
	if (err)
		{
		self->iParentSession.AsyncSendFailed(err);
		}
	
	return callAgain;
	}
	
/**
Performs a single step of the message size calculation.
@return whether this method needs to be called again, to calculate the next step.
*/
TInt CImapAppend::MessageSizeCalculationStepL()
	{
	__ASSERT_DEBUG(iAppendCommandSendState==EAppendCalculatingSize, TImapServerPanic::ImapPanic(TImapServerPanic::EAppendInvalidState));
	
	TInt callAgain = KDontCallAgain;
	
	TPtr8 nextLine = iLineBufferForSend->Des();	
	TInt status = iSource.NextLineL(nextLine, iPadCount);
	
	iTotalMessageSize += iLineBufferForSend->Length();

#ifdef __IMAP_LOGGING
	{
		// Log the first few bytes only
		TPtrC8 truncatedData = iLineBufferForSend->Left(60);
		// only log up to the first \r\n
		TInt pos = truncatedData.Find(KImapTxtCrlf);
		if (pos >= 0)
			{
			truncatedData.Set(truncatedData.Left(pos));
			}
		else 
			{
			// \r\n may have been split by the intial truncation,
			// so check for the \r
			pos = truncatedData.Locate('\r');
			if (pos >= 0)
				{
				truncatedData.Set(truncatedData.Left(pos));
				}
			}

		__LOG_FORMAT((iLogId, "CImapAppend::MessageSizeCalculationStepL() - %3d ==> %3d [%S]", iLineBufferForSend->Length(), iTotalMessageSize, &truncatedData));
	}
#endif //__IMAP_LOGGING
	
	if (status == KImCvFinished)
		{
		iSource.Reset();
		iPadCount = 0;
		nextLine.Zero();
		
		SendAppendCommandWithSizeL();
		}
	else
		{
		callAgain = KCallAgain;
		}
		
	return callAgain;
	}
	
/**
Sends the IMAP APPEND command.
This method is called after the size of the message has been calculated.
*/
void CImapAppend::SendAppendCommandWithSizeL()
	{
	TInt bufferLength = KTxtAppendFormat().Length() + TagLength(iTagId) + iDestinationMailboxName->Length() + TagLength(iTotalMessageSize);
	
	delete iOutputBuffer;
	iOutputBuffer=NULL;
	
	iOutputBuffer = HBufC8::NewL(bufferLength);
	iOutputBuffer->Des().Format(KTxtAppendFormat, iTagId, iDestinationMailboxName, iTotalMessageSize);
	
	// Send the data on the output stream
	iAppendCommandSendState = EAppendWaitingForContinuationResponse;
	iStream->SendDataReq(*iOutputBuffer);
	}

/**
Sends the first line of the message upon reciept of a continuation response from server.
The next line will be sent SendDataCnfL() is called, indicating that the first line has been succesfully sent.
*/
void CImapAppend::ParseContinuationResponseL() 
	{
	if (iAppendCommandSendState != EAppendWaitingForContinuationResponse)
		{
		CorruptDataL();
		}
		
	iAppendCommandSendState = EAppendSendingMessageData;		
	SendNextMessageLineL();
	}

/**
This is called when the previous line has been succesfully sent.
If there are more lines of the message to send, then the next line is sent now.
@return ECompleteUntagged
*/
void CImapAppend::SendDataCnfL()
	{
	if (iAppendCommandSendState == EAppendSendingMessageData)
		{
		SendNextMessageLineL();
		}
	else if (iAppendCommandSendState == EAppendWaitingForContinuationResponse)
		{
		__LOG_TEXT(iLogId, "CImapAppend::SendDataCnfL() - Initial Command Sent");
		}
	else
		{
		__ASSERT_DEBUG(iAppendCommandSendState == EAppendSendingLastMessageData, TImapServerPanic::ImapPanic(TImapServerPanic::EAppendInvalidState));
		__LOG_TEXT(iLogId, "CImapAppend::SendDataCnfL() - Message Data Sent");
		
		iAppendCommandSendState = EAppendFinishedSending;
		}
	}
	
/**
Sends the first or next line of the message to the server.
SendDataCnfL() will be called when the line has been succefully sent.
*/
void CImapAppend::SendNextMessageLineL()
	{	
	__ASSERT_DEBUG(iAppendCommandSendState == EAppendSendingMessageData, TImapServerPanic::ImapPanic(TImapServerPanic::EAppendInvalidState));
	
	// Receive the next line to be sent, by passing in a modifyable descriptor.
	// Note that iPadCount receives the padding count, which will be passed into the next call to NextLineL()
	TPtr8 nextLine = iLineBufferForSend->Des();
	TInt sendStatus = iSource.NextLineL(nextLine, iPadCount);
	
	__LOG_TEXT(iLogId, "CImapAppend::SendNextMessageLineL()");
	
	if (sendStatus == KImCvFinished)
		{
		// As well as sending the last part of the literal block,
		// we need to send a CRLF to terminate the APPEND command.
		// The CRLF is not part of the message data.
		
		// Is there enough room to append the CRLF?		
		TInt lengthWithCrlf = nextLine.Length() + KImapTxtCrlf().Length();		
		if (nextLine.MaxLength() < lengthWithCrlf)
			{
			// Make enough space for the CRLF
			iLineBufferForSend = iLineBufferForSend->ReAllocL(lengthWithCrlf);
			nextLine.Set(iLineBufferForSend->Des());
			}
		
		__LOG_TEXT(iLogId, "CImapAppend::SendNextMessageLineL() - appending command terminator");
		nextLine.Append(KImapTxtCrlf());
		
		// Indicate that this is the final line to be sent
		iAppendCommandSendState = EAppendSendingLastMessageData;
		}

	// Send the data, using a non-modifyable descriptor.
	iStream->SendDataReq(*iLineBufferForSend);
	}

/**
Overrides the default implementation of Cancel.
If the command is still calculating the message size, then the calculation is cancelled
*/
void CImapAppend::Cancel()
	{
	__LOG_TEXT(iLogId, "CImapAppend::Cancel()"); // Overrides CImapCommand::Cancel()
	
	iBackgroundCalculationStepper->Cancel();
	
	EnterFlushingState();
	}

/**
Overrides the default implementation of CanCompleteFlushNow.
If the command has not yet sent any data, then it can be completed now
as it is not yet expecting to receive any response data.
@return EFalse if data has been sent.
		ETrue if still at the message size calculation stage
*/
TBool CImapAppend::CanCompleteFlushNow() const 
	{
	// Any command that has completed should have been destroyed before this
	// method is called.
	__ASSERT_DEBUG(ParseState() != ECommandComplete, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandInvalidParseState1));
	__ASSERT_DEBUG(Flushing(), TImapServerPanic::ImapPanic(TImapServerPanic::ECommandNotFlushing));

	return (iAppendCommandSendState <= EAppendCalculatingSize) ? ETrue: EFalse;
	}
