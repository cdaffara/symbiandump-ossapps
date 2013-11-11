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


#include "cimapstarttls.h"

#include "moutputstream.h"
#include "imappaniccodes.h"

_LIT8(KTxtStartTlsFormat, "%d STARTTLS\r\n");
const TInt KStartTlsFormatEscapeCharCount = 2; // for "%d"

// Construction and Destruction
CImapStartTls* CImapStartTls::NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId)
// static method
	{
	CImapStartTls* self = new(ELeave)CImapStartTls(aSelectedFolderData, aLogId);
	return self;
	}
	
CImapStartTls::CImapStartTls(CImapFolderInfo* aSelectedFolderData, TInt aLogId)
	: CImapCommandEx(aSelectedFolderData, aLogId)
	{
	}
	
CImapStartTls::~CImapStartTls()
	{
	}


/**
Responsible for sending the IMAP starttls command to the remote server.
The data will be sent to the remote server on the output stream provided.
It is assumed the output stream has already been set up and ready to use.

@param aTagId	Used to generate the IMAP tag that identifies the message.
@param aStream	The output stream on which the message will be sent.
*/
void CImapStartTls::SendMessageL(TInt aTagId, MOutputStream& aStream)
// Send the line "<tag> STARTTLS"
	{
	iTagId = aTagId;
	TInt bufferLength = KTxtStartTlsFormat.iTypeLength - KStartTlsFormatEscapeCharCount + TagLength(aTagId);

	__ASSERT_DEBUG(iOutputBuffer==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandOutputBufferNotNull));
	iOutputBuffer = HBufC8::NewL(bufferLength);
	iOutputBuffer->Des().Format(KTxtStartTlsFormat, iTagId);
	
	// Send the data on the output stream
	aStream.SendDataReq(*iOutputBuffer);
	}

