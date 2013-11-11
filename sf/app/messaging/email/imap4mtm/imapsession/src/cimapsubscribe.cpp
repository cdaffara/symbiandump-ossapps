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


#include "cimapsubscribe.h"
#include "moutputstream.h"
#include "imappaniccodes.h"

//Subscribe command
_LIT8(KCommandSubscribe, "%d SUBSCRIBE %S\r\n");

CImapSubscribe* CImapSubscribe::NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId, const TDesC& aMailboxName)
//static method	first phase construction
	{
	CImapSubscribe* self = new (ELeave) CImapSubscribe(aSelectedFolderData, aLogId);
	CleanupStack::PushL(self);
	self->ConstructL(aMailboxName);
	CleanupStack::Pop();
	return self;
	}
		
CImapSubscribe::CImapSubscribe(CImapFolderInfo* aSelectedFolderData, TInt aLogId)
	: CImapCommandEx(aSelectedFolderData, aLogId)
	{	
	}
	
void CImapSubscribe::ConstructL(const TDesC& aMailboxName)
	{
	iMailboxName = EncodeMailboxNameForSendL(aMailboxName);
	}
	
CImapSubscribe::~CImapSubscribe()
	{
	delete iMailboxName;
	}

/** 
Formats and sends the IMAP SUBSCRIBE command.
@param aTagId Command sequence id which will be sent along with the IMAP command.
@param aStream A wrapper for the outbound stream of a connected socket, using which 
the command will be send to the server
*/
void CImapSubscribe::SendMessageL(TInt aTagId, MOutputStream& aStream)
	{
	iTagId = aTagId;
	
	TInt bufLength = KCommandSubscribe().Length();
	bufLength += TagLength(aTagId);
	bufLength += iMailboxName->Length();
	
	__ASSERT_DEBUG(iOutputBuffer==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandOutputBufferNotNull));
	iOutputBuffer = HBufC8::NewL(bufLength);	
	iOutputBuffer->Des().Format(KCommandSubscribe, aTagId, iMailboxName);	
	
	//send the command to the server
	aStream.SendDataReq(*iOutputBuffer);	
	}
