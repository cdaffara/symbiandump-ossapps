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

#include "cimapdelete.h"
#include "moutputstream.h"
#include "cimapsessionconsts.h"
#include "imappaniccodes.h"

// IMAP DELETE command
_LIT8(KCommandDelete, "%d DELETE %S\r\n");

/**
The factory constructor.
*/
CImapDelete* CImapDelete::NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId, const TDesC& aMailboxName)
// static method first phase construction
	{
	CImapDelete* self = new (ELeave) CImapDelete(aSelectedFolderData, aLogId);
	CleanupStack::PushL(self);
	self->ConstructL(aMailboxName);
	CleanupStack::Pop();
	return self;
	}
/**
Constructor
*/
CImapDelete::CImapDelete(CImapFolderInfo* aSelectedFolderData, TInt aLogId)
	: CImapCommandEx(aSelectedFolderData, aLogId)
	{	
	}

/**
Second phase constructor.
Will convert the 16-bit maiboxname to UTF-7.
*/
void CImapDelete::ConstructL(const TDesC& aMailboxName)
	{
	iMailboxName = EncodeMailboxNameForSendL(aMailboxName);
	}

CImapDelete::~CImapDelete()
	{
	delete iMailboxName;
	}

/**
Formats and sends the IMAP DELETE command.
@param aTagId Command sequence id which will be send along with the IMAP command.
@param aStream A wrapper for the outbound stream of a connected socket, using which
the command will be send to the server
*/
void CImapDelete::SendMessageL(TInt aTagId, MOutputStream& aStream)
	{
	iTagId = aTagId;

	TInt bufLength = KCommandDelete().Length();
	bufLength += TagLength(aTagId);
	bufLength += iMailboxName->Length();

	__ASSERT_DEBUG(iOutputBuffer==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandOutputBufferNotNull));
	iOutputBuffer = HBufC8::NewL(bufLength);
	iOutputBuffer->Des().Format(KCommandDelete, aTagId, iMailboxName);

	//send the command to the server
	aStream.SendDataReq(*iOutputBuffer);
	}
