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

#include "cimapservergreeting.h"

#include "cimapservergreetinginfo.h"
#include "moutputstream.h"
#include "cimapsessionconsts.h"
#include "imappaniccodes.h"

// Construction and Destruction
CImapServerGreeting* CImapServerGreeting::NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId, CImapServerGreetingInfo& aServerGreetingInfo)
// static method
	{
	CImapServerGreeting* self = new(ELeave)CImapServerGreeting(aSelectedFolderData, aLogId, aServerGreetingInfo);
	return self;
	}
	
CImapServerGreeting::CImapServerGreeting(CImapFolderInfo* aSelectedFolderData, TInt aLogId, CImapServerGreetingInfo& aServerGreetingInfo)
	: CImapCommandEx(aSelectedFolderData, aLogId)
	, iServerGreetingInfo(aServerGreetingInfo)
	{
	// override the default CImapCommand value
	iCompleteOnAnyResponse = ETrue;
	}
	
CImapServerGreeting::~CImapServerGreeting()
	{
	}

/**
The server sends its greeting upon connection.
No message needs to be sent in order for this to happen.
So this method is an empty stub that allows the server greeting object
to fit into the CImapCommand framework.
@param aTagId	Ignored in this implementation of CImapCommand.
@param aStream	Ignored in this implementation of CImapCommand.
*/
void CImapServerGreeting::SendMessageL(TInt /*aTagId*/, MOutputStream& /*aStream*/)
	{
	__ASSERT_DEBUG(iOutputBuffer==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandOutputBufferNotNull));
	}

/**
Server Greeting is a single untagged response with the response code OK/PREAUTH/BYE
Because iCompleteOnAnyResponse is ETrue, the command will complete on receipt of this response.
@return	ECompleteUntagged
*/
CImapCommand::TParseBlockResult CImapServerGreeting::ParseUntaggedResponseL()
	{
	// From RFC3501 section 9...
	//
	//	greeting = "*" SP (resp-cond-auth / resp-cond-bye) CRLF
	//	resp-cond-auth = ("OK" / "PREAUTH") SP resp-text
	//	resp-cond-bye = "BYE" SP resp-text
	//
	// resp-text is ignored by this implementation
	
	TPtrC8 serverGreetingTag = GetNextPart();
	
	if (serverGreetingTag.CompareF(KImapTxtOk) == 0)
		{
		iServerGreetingInfo.SetResponseTag(CImapServerGreetingInfo::ETagOk);
		}
	else if (serverGreetingTag.CompareF(KImapTxtPreAuth) == 0)
		{
		iServerGreetingInfo.SetResponseTag(CImapServerGreetingInfo::ETagPreAuth);
		}
	else if (serverGreetingTag.CompareF(KImapTxtBye) == 0)
		{
		iServerGreetingInfo.SetResponseTag(CImapServerGreetingInfo::ETagBye);
		}
	else
		{
		CorruptDataL();
		}
	
	return ECompleteUntagged;
	}
