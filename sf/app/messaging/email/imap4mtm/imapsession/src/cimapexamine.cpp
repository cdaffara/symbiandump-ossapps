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

#include "cimapexamine.h"
#include "moutputstream.h"
#include "cimapfolderinfo.h"
#include "imappaniccodes.h"

_LIT8(KTxtExamineFormat, "%d EXAMINE %S\r\n");
const TInt KExamineFormatEscapeCharCount = 4; // for "%d" and %S

/**
The factory constructor. Part of two phased construction.
*/
CImapExamine* CImapExamine::NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId)
	{
	CImapExamine* self = new(ELeave) CImapExamine(aSelectedFolderData, aLogId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Second phase constructor.
Calling the base class ConstructL().
*/
void CImapExamine::ConstructL()
	{
	CImapSelect::ConstructL();
	}

/**
Constructor.
*/
CImapExamine::CImapExamine(CImapFolderInfo* aSelectedFolderData, TInt aLogId)
	: CImapSelect(aSelectedFolderData, aLogId)
	{
	// Default to "read only"
	// This will override the setting made in CImapSelect's constructor.
	aSelectedFolderData->SetReadWrite(EFalse);
	}
	
/**
Destructor.
*/
CImapExamine::~CImapExamine()
	{}
	
/**
Formats and sends the IMAP EXAMINE command.
@param aTagId Command sequence id which will be sent along with the IMAP command.
@param aStream A wrapper for the outbound stream of a connected socket, using which
the command will be send to the server
*/	
void CImapExamine::SendMessageL(TInt aTagId, MOutputStream& aStream)
	{
	HBufC8* mailbox = EncodeMailboxNameForSendL(iSelectedFolderData->Name());
	CleanupStack::PushL(mailbox);
	
	iTagId = aTagId;
	TInt bufferLength = KTxtExamineFormat().Length() - KExamineFormatEscapeCharCount + TagLength(aTagId) + mailbox->Length();
	
	__ASSERT_DEBUG(iOutputBuffer==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandOutputBufferNotNull));
	iOutputBuffer = HBufC8::NewL(bufferLength);
	iOutputBuffer->Des().Format(KTxtExamineFormat, iTagId, mailbox);
	
	CleanupStack::PopAndDestroy(mailbox);
		
	// Send the data on the output stream
	aStream.SendDataReq(*iOutputBuffer);
	}


