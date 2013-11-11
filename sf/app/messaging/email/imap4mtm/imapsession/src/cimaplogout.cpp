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


#include "cimaplogout.h"

#include "moutputstream.h"
#include "cimapsessionconsts.h"
#include "imappaniccodes.h"

_LIT8(KTxtLogoutFormat, "%d LOGOUT\r\n");
const TInt KLogoutFormatEscapeCharCount = 2; // for %d

// Construction and Destruction
CImapLogout* CImapLogout::NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId)
// static method
	{
	CImapLogout* self = new(ELeave)CImapLogout(aSelectedFolderData, aLogId);
	return self;
	}
	
CImapLogout::CImapLogout(CImapFolderInfo* aSelectedFolderData, TInt aLogId)
	: CImapCommandEx(aSelectedFolderData, aLogId)
	{
	}
	
CImapLogout::~CImapLogout()
	{
	}


/**
Responsible for sending the IMAP logout command to the remote server.
The data will be sent to the remote server on the output stream provided.
It is assumed the output stream has already been set up and ready to use.

@param aTagId	Used to generate the IMAP tag that identifies the message.
@param aStream	The output stream on which the message will be sent.
*/
void CImapLogout::SendMessageL(TInt aTagId, MOutputStream& aStream)
// Send the line "<tag> LOGOUT"
	{
	iTagId = aTagId;
	TInt bufferLength = KTxtLogoutFormat.iTypeLength - KLogoutFormatEscapeCharCount + TagLength(aTagId);

	__ASSERT_DEBUG(iOutputBuffer==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandOutputBufferNotNull));
	iOutputBuffer = HBufC8::NewL(bufferLength);
	iOutputBuffer->Des().Format(KTxtLogoutFormat, iTagId);
	
	// Send the data on the output stream
	aStream.SendDataReq(*iOutputBuffer);
	}
