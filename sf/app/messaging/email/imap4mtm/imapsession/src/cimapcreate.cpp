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

#include "cimapcreate.h"
#include "moutputstream.h"
#include "cimapsessionconsts.h"
#include "imappaniccodes.h"

// IMAP CREATE command
_LIT8(KCommandCreate, "%d CREATE %S\r\n");

CImapCreate* CImapCreate::NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId, const TDesC& aMailboxName)
// static method first phase construction
	{
	CImapCreate* self = new (ELeave) CImapCreate(aSelectedFolderData, aLogId);
	CleanupStack::PushL(self);
	self->ConstructL(aMailboxName);
	CleanupStack::Pop();
	return self;
	}

CImapCreate::CImapCreate(CImapFolderInfo* aSelectedFolderData, TInt aLogId)
	: CImapCommandEx(aSelectedFolderData, aLogId)
	{	
	}

void CImapCreate::ConstructL(const TDesC& aMailboxName)
	{
	iMailboxName = EncodeMailboxNameForSendL(aMailboxName);
	}

CImapCreate::~CImapCreate()
	{
	delete iMailboxName;
	}

/**
Formats and sends the IMAP CREATE command.
@param aTagId Command sequence id which will be send along with the IMAP command.
@param aStream A wrapper for the outbound stream of a connected socket, using which
the command will be send to the server
*/
void CImapCreate::SendMessageL(TInt aTagId, MOutputStream& aStream)
	{
	iTagId = aTagId;

	TInt bufLength = KCommandCreate().Length();
	bufLength += iMailboxName->Length();
	bufLength += TagLength(aTagId);

	__ASSERT_DEBUG(iOutputBuffer==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandOutputBufferNotNull));
	iOutputBuffer = HBufC8::NewL(bufLength);
	iOutputBuffer->Des().Format(KCommandCreate, aTagId, iMailboxName);

	//send the command to the server
	aStream.SendDataReq(*iOutputBuffer);
	}
