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


#include "cimapservereventwait.h"
#include "moutputstream.h"
#include "cimapsessionconsts.h"
#include "imappaniccodes.h"

CImapServerEventWait* CImapServerEventWait::NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId)
	{
	CImapServerEventWait* self = new (ELeave) CImapServerEventWait(aSelectedFolderData, aLogId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;		
	}	

CImapServerEventWait::CImapServerEventWait(CImapFolderInfo* aSelectedFolderData, TInt aLogId) 
	: CImapCommandEx(aSelectedFolderData, aLogId)
	{
	// override the default CImapCommand value
	iCompleteOnAnyResponse = ETrue;
	}
	
void CImapServerEventWait::ConstructL()
	{
	}
	
CImapServerEventWait::~CImapServerEventWait()
	{
	}

/**
If CImapServerEventWait is waiting for the start of a new response
(i.e. it is not in the middle of processing a response) then the Flush
operation can complete the command straight away.
@return EFalse if processing a response.
		ETrue if waiting for the start of a new response.
*/
TBool CImapServerEventWait::CanCompleteFlushNow() const
	{
	// Any command that has completed should have been destroyed before this
	// method is called.
	__ASSERT_DEBUG(ParseState() != ECommandComplete, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandInvalidParseState10));
	__ASSERT_DEBUG(Flushing(), TImapServerPanic::ImapPanic(TImapServerPanic::ECommandNotFlushing));
	
	return (ParseState() == EWaitStartResponse) ? ETrue: EFalse;
	}

/**
This command does not send any command.
It just waits for any unsolicited responses that the server might send.
So this method is an empty stub that allows the server event wait object
to fit into the CImapCommand framework.
@param aTagId	Ignored in this implementation of CImapCommand.
@param aStream	Ignored in this implementation of CImapCommand.
*/
void CImapServerEventWait::SendMessageL(TInt /*aTagId*/, MOutputStream& /*aStream*/)
	{
	__ASSERT_DEBUG(iOutputBuffer==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandOutputBufferNotNull));
	}

/**
Always leaves with KErrImapCorrupt, because unsolicited tagged responses are not expected from the server.
@return because this implementation of the method always leaves, no value can ever be returned.
*/
TBool CImapServerEventWait::ParseTaggedResponseL(TInt /*aTagId*/)
	{
	CorruptDataL();
	return EFalse;
	}

