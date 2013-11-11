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


#include "cimapcopy.h"
#include "moutputstream.h"
#include "imappaniccodes.h"

// IMAP COPY command
_LIT8(KCommandCopy, "%d UID COPY %S %S\r\n");

CImapCopy*  CImapCopy::NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId, const TDesC8& aSequenceSet, const TDesC& aMailboxName)
	{
	CImapCopy* self = new (ELeave) CImapCopy(aSelectedFolderData, aLogId);
	CleanupStack::PushL(self);
	self->ConstructL(aSequenceSet, aMailboxName);
	CleanupStack::Pop(self);
	return self;
	}
		
CImapCopy::CImapCopy(CImapFolderInfo* aSelectedFolderData, TInt aLogId)
	: CImapCommandEx(aSelectedFolderData, aLogId)	
	{}
	
CImapCopy::~CImapCopy()
	{
	delete iSequenceSet;
	delete iMailboxName;
	}
	
void CImapCopy::ConstructL(const TDesC8& aSequenceSet, const TDesC& aMailboxName)
	{
	iSequenceSet = aSequenceSet.AllocL();
	iMailboxName = EncodeMailboxNameForSendL(aMailboxName);
	}
/** 
Formats and sends the IMAP COPY command.
@param aTagId Command sequence id which will be sent along with the IMAP command.
@param aStream A wrapper for the outbound stream of a connected socket, using which 
the command will be send to the server
*/
void CImapCopy::SendMessageL(TInt aTagId, MOutputStream& aStream)
	{
	iTagId = aTagId;
	
	TInt bufLength = KCommandCopy().Length();	
	bufLength += TagLength(aTagId);
	bufLength += iSequenceSet->Length();
	bufLength += iMailboxName->Length();
	
	__ASSERT_DEBUG(iOutputBuffer==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandOutputBufferNotNull));
	iOutputBuffer = HBufC8::NewL(bufLength);	
	iOutputBuffer->Des().Format(KCommandCopy, aTagId, iSequenceSet, iMailboxName);	
	
	//send the command to the server
	aStream.SendDataReq(*iOutputBuffer);
		
	}
