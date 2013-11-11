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


#include "cimapidle.h"
#include "moutputstream.h"
#include "cimapsessionconsts.h"
#include "imappaniccodes.h"
#include "cimaplogger.h"

// IMAP IDLE command
_LIT8(KTxtIdleFormat, "%d IDLE\r\n");
const TInt KIdleFormatParamChars = -2; // 2 chars in "%d"

_LIT8(KTxtDoneCrlf, "DONE\r\n");

CImapIdle* CImapIdle::NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId)
	{
	CImapIdle* self = new (ELeave) CImapIdle(aSelectedFolderData, aLogId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;		
	}	

CImapIdle::CImapIdle(CImapFolderInfo* aSelectedFolderData, TInt aLogId) 
	: CImapCommandEx(aSelectedFolderData, aLogId)
	, iIdleState(iIdleState)
	{
	}
	
void CImapIdle::ConstructL()
	{
	}
	
CImapIdle::~CImapIdle()
	{
	}

/** 
Formats and sends the IMAP IDLE command.
@param aTagId Command sequence id which will be send along with the IMAP command.
@param aStream A wrapper for the outbound stream of a connected socket, using which 
the command will be send to the server
*/
void CImapIdle::SendMessageL(TInt aTagId, MOutputStream& aStream)
	{
	iTagId = aTagId;
	TInt bufferLength = KTxtIdleFormat().Length() - KIdleFormatParamChars + TagLength(aTagId);
	
	__ASSERT_DEBUG(iOutputBuffer==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandOutputBufferNotNull));
	iOutputBuffer = HBufC8::NewL(bufferLength);
	iOutputBuffer->Des().Format(KTxtIdleFormat, iTagId);
	
	//send the command to the server
	aStream.SendDataReq(*iOutputBuffer);
	}

/**
If waiting for a continuation, iIdleState is updated to waiting for untagged responses.
This state change will be detected by the session.
When not waiting for a continuation, then the method will leave with KErrImapCorrupt.
*/
void CImapIdle::ParseContinuationResponseL()
	{
	if (iIdleState == EIdleWaitContinue)
		{
		iIdleState = EIdleWaitResponse;
		}
	else
		{
		CorruptDataL();
		}			
	}

/**
Overrides the default version of cancel 
because we don't want to "flush" on cancel while in the EIdleWaitResponse state.
After CImapSession::Cancel() is called during EIdleWaitResponse, 
the user should call DoneL() next, not FlushCancelledCommand()
*/
void CImapIdle::Cancel()
	{
	__LOG_TEXT(iLogId, "CImapIdle::Cancel()"); // Overrides CImapCommand::Cancel()
	
	if (iIdleState != EIdleWaitResponse)
		{
		// When in EIdleWaitDone, a cancel would require a flush as normal, as we are now expecting a tagged OK.
		
		// When in EIdleWaitContinue, setting flush to true on cancel will cause a corrupt data error when the 
		// continuation character is received, and the session will have to be shut down.
		// This is reasonable as the command would otherwise have to send data to the server, which is not
		// what we want to do when flushing a cancel.
			
		EnterFlushingState();
		}	
	}
	
/**
Sends the "DONE" command to the server, which should reply with 
zero or more untagged status responses, followed by the tagged OK response.
*/
void CImapIdle::SendDone(MOutputStream& aStream)
	{
	__ASSERT_DEBUG(iIdleState == EIdleWaitResponse, TImapServerPanic::ImapPanic(TImapServerPanic::EIdleStateInvalid));
	
	//send the command to the server
	iIdleState = EIdleWaitDone;
	// DONE command is sent with a 10 seconds tineout.
	aStream.SendDataReq(KTxtDoneCrlf, 10);
	}

/**
@return The current state of the idle command.
*/
CImapIdle::TIdleState CImapIdle::IdleState()
	{
	return iIdleState;
	}
