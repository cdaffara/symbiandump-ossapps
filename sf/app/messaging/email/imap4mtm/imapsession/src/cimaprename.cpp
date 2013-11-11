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

#include "cimaprename.h"
#include "moutputstream.h"
#include "cimapsessionconsts.h"
#include "imappaniccodes.h"

// IMAP RENAME command
_LIT8(KCommandRename, "%d RENAME %S %S\r\n");

/**
The factory constructor.
*/
CImapRename* CImapRename::NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId, const TDesC& aOldMailboxName, const TDesC& aNewMailboxName)
// static method first phase construction
	{
	CImapRename* self = new (ELeave) CImapRename(aSelectedFolderData, aLogId);
	CleanupStack::PushL(self);
	self->ConstructL(aOldMailboxName, aNewMailboxName);
	CleanupStack::Pop();
	return self;
	}

/**
Constructor.
@param	aSelectedFolderData
*/
CImapRename::CImapRename(CImapFolderInfo* aSelectedFolderData, TInt aLogId)	
	: CImapCommandEx(aSelectedFolderData, aLogId)
	{
	}

/**
Second phase constructor.
Will convert the 16-bit maiboxname parameters to IMAP UTF-7.
@param aOldMailboxName mailbox whose name is to be changed
@param aNewMailboxName the new name for the mailbox
*/
void CImapRename::ConstructL(const TDesC& aOldMailboxName, const TDesC& aNewMailboxName)
	{
	iOldMailboxName = EncodeMailboxNameForSendL(aOldMailboxName);
	iNewMailboxName = EncodeMailboxNameForSendL(aNewMailboxName);
	}
/**
Destructor
*/
CImapRename::~CImapRename()
	{
	delete iOldMailboxName;
	delete iNewMailboxName;
	}

/**
Formats and sends the IMAP RENAME command.
@param aTagId Command sequence id which will be sent along with the IMAP command.
@param aStream A wrapper for the outbound stream of a connected socket, using which
the command will be send to the server
*/
void CImapRename::SendMessageL(TInt aTagId, MOutputStream& aStream)
	{
	iTagId = aTagId;

	TInt bufLength = KCommandRename().Length();
	bufLength += TagLength(aTagId);
	bufLength += iOldMailboxName->Length();
	bufLength += iNewMailboxName->Length();

	__ASSERT_DEBUG(iOutputBuffer==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandOutputBufferNotNull));
	iOutputBuffer = HBufC8::NewL(bufLength);
	iOutputBuffer->Des().Format(KCommandRename, aTagId, iOldMailboxName, iNewMailboxName);

	//send the command to the server
	aStream.SendDataReq(*iOutputBuffer);
	}
