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

#include "cimapexpunge.h"
#include "moutputstream.h"
#include "cimapfolderinfo.h"
#include "cimapsessionconsts.h"
#include "imappaniccodes.h"

// IMAP EXPUNGE command
_LIT8(KCommandExpunge, "%d EXPUNGE\r\n");

CImapExpunge* CImapExpunge::NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId)
	{
	CImapExpunge* self = new (ELeave) CImapExpunge(aSelectedFolderData, aLogId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;		
	}	

CImapExpunge::CImapExpunge(CImapFolderInfo* aSelectedFolderData, TInt aLogId) 
	: CImapCommandEx(aSelectedFolderData, aLogId)
	{
	}
	
void CImapExpunge::ConstructL()
	{
	}
	
CImapExpunge::~CImapExpunge()
	{
	}

/** 
Formats and sends the IMAP EXPUNGE command.
@param aTagId Command sequence id which will be sent along with the IMAP command.
@param aStream A wrapper for the outbound stream of a connected socket, using which 
the command will be send to the server
*/
void CImapExpunge::SendMessageL(TInt aTagId, MOutputStream& aStream)
	{
	iTagId = aTagId;
		
	__ASSERT_DEBUG(iOutputBuffer==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandOutputBufferNotNull));
	iOutputBuffer = HBufC8::NewL(KCommandExpunge().Length() - 2 + TagLength(aTagId));// -2 for %d..
	iOutputBuffer->Des().Format(KCommandExpunge, aTagId);	
	//send the command to the server
	aStream.SendDataReq(*iOutputBuffer);
	}
