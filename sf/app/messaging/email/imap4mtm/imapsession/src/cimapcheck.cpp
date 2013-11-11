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


#include "cimapcheck.h"
#include "moutputstream.h"
#include "cimapsessionconsts.h"
#include "imappaniccodes.h"

// IMAP CHECK command
_LIT8(KCommandCheck, "%d CHECK\r\n");

CImapCheck* CImapCheck::NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId)
	{
	CImapCheck* self = new (ELeave) CImapCheck(aSelectedFolderData, aLogId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;		
	}	

CImapCheck::CImapCheck(CImapFolderInfo* aSelectedFolderData, TInt aLogId) 
	: CImapCommandEx(aSelectedFolderData, aLogId)
	{
	}
	
void CImapCheck::ConstructL()
	{
	}
	
CImapCheck::~CImapCheck()
	{
	
	}

/** 
Formats and sends the IMAP CHECK command.
@param aTagId Command sequence id which will be send along with the IMAP command.
@param aStream A wrapper for the outbound stream of a connected socket, using which 
the command will be send to the server
*/
void CImapCheck::SendMessageL(TInt aTagId, MOutputStream& aStream)
	{
	iTagId = aTagId;
		
	__ASSERT_DEBUG(iOutputBuffer==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandOutputBufferNotNull));
	iOutputBuffer = HBufC8::NewL(KCommandCheck().Length() + TagLength(aTagId));	
	iOutputBuffer->Des().Format(KCommandCheck, aTagId);	
	
	//send the command to the server
	aStream.SendDataReq(*iOutputBuffer);
	}

