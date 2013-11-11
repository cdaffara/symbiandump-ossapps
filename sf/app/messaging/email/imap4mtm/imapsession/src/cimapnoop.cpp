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


#include "cimapnoop.h"
#include "moutputstream.h"
#include "cimapsessionconsts.h"
#include "imappaniccodes.h"

// IMAP NOOP command
_LIT8(KCommandNoop, "%d NOOP\r\n");

CImapNoop* CImapNoop::NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId)
	{
	CImapNoop* self = new (ELeave) CImapNoop(aSelectedFolderData, aLogId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;		
	}	

CImapNoop::CImapNoop(CImapFolderInfo* aSelectedFolderData, TInt aLogId) 
	: CImapCommandEx(aSelectedFolderData, aLogId)
	{
	}
	
void CImapNoop::ConstructL()
	{
	}
	
CImapNoop::~CImapNoop()
	{
	}

/** 
Formats and sends the IMAP NOOP command.
@param aTagId Command sequence id which will be sent along with the IMAP command.
@param aStream A wrapper for the outbound stream of a connected socket, using which 
the command will be send to the server
*/
void CImapNoop::SendMessageL(TInt aTagId, MOutputStream& aStream)
	{
	iTagId = aTagId;
	
	__ASSERT_DEBUG(iOutputBuffer==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandOutputBufferNotNull));
	iOutputBuffer = HBufC8::NewL(KCommandNoop().Length() + TagLength(aTagId));	
	iOutputBuffer->Des().Format(KCommandNoop, aTagId);	
	
	//send the command to the server
	aStream.SendDataReq(*iOutputBuffer);
	}
