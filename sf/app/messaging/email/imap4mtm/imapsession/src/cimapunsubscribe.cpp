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


#include "cimapunsubscribe.h"
#include "moutputstream.h"
#include "imappaniccodes.h"

//Unsubscribe command
_LIT8(KCommandUnsubscribe, "%d UNSUBSCRIBE %S\r\n");

CImapUnsubscribe* CImapUnsubscribe::NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId, const TDesC& aMailboxName)
//static method	first phase construction
	{
	CImapUnsubscribe* self = new (ELeave) CImapUnsubscribe(aSelectedFolderData, aLogId);
	CleanupStack::PushL(self);
	self->ConstructL(aMailboxName);
	CleanupStack::Pop();
	return self;
	}
		
CImapUnsubscribe::CImapUnsubscribe(CImapFolderInfo* aSelectedFolderData, TInt aLogId)
	: CImapCommandEx(aSelectedFolderData, aLogId)
	{	
	}
	
void CImapUnsubscribe::ConstructL(const TDesC& aMailboxName)
	{
	iMailboxName = EncodeMailboxNameForSendL(aMailboxName);
	}
	
CImapUnsubscribe::~CImapUnsubscribe()
	{
	delete iMailboxName;
	}

/** 
Formats and sends the IMAP UNSUBSCRIBE command.
@param aTagId Command sequence id which will be sent along with the IMAP command.
@param aStream A wrapper for the outbound stream of a connected socket, using which 
the command will be send to the server
*/
void CImapUnsubscribe::SendMessageL(TInt aTagId, MOutputStream& aStream)
	{
	iTagId = aTagId;
	
	TInt bufLength = KCommandUnsubscribe().Length();	
	bufLength += TagLength(aTagId);
	bufLength += iMailboxName->Length();
	
	__ASSERT_DEBUG(iOutputBuffer==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandOutputBufferNotNull));
	iOutputBuffer = HBufC8::NewL(bufLength);	
	iOutputBuffer->Des().Format(KCommandUnsubscribe, aTagId, iMailboxName);	
	
	//send the command to the server
	aStream.SendDataReq(*iOutputBuffer);	
	}
