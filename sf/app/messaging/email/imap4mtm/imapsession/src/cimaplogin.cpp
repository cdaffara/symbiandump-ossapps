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

#include "cimaplogin.h"
#include "moutputstream.h"
#include "cimapsessionconsts.h"
#include "imappaniccodes.h"

// IMAP LOGIN command
_LIT8(KCommandLogin, "%d LOGIN %S %S\r\n");
_LIT8(KCommandLoginUsernameLength, "%d LOGIN {%d}\r\n");
_LIT8(KCommandLoginUsernamePasswordLength, "%d LOGIN %S {%d}\r\n");

_LIT8(KCommandContinueUsernameLitPassword, "%S %S\r\n");
_LIT8(KCommandContinueUsernameLitPasswordLength, "%S {%d}\r\n");
_LIT8(KCommandContinuePasswordLit, "%S\r\n");


CImapLogin* CImapLogin::NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId, const TDesC8& aUserName, const TDesC8& aPassword, CImapCapabilityInfo& aCapabilityInfo)
// static method first phase construction
	{		
	CImapLogin* self = new (ELeave) CImapLogin(aSelectedFolderData, aLogId, aCapabilityInfo);
	CleanupStack::PushL(self);
	self->ConstructL(aUserName, aPassword);
	CleanupStack::Pop();
	return self;
	}
		
CImapLogin::CImapLogin(CImapFolderInfo* aSelectedFolderData, TInt aLogId, CImapCapabilityInfo& aCapabilityInfo)
	: CImapCapability(aSelectedFolderData, aLogId, aCapabilityInfo)
	{				
	}
		
void CImapLogin::ConstructL(const TDesC8& aUserName, const TDesC8& aPassword)
	{
	iUserName =	aUserName.AllocL();
	iPassword = aPassword.AllocL();
	}

CImapLogin::~CImapLogin()
	{
	delete iUserName;
	delete iPassword;
	}

/**
Check whether the username or/and password needs to be sent as literal data.
The data needs to be sent as a literal if it contains non ASCII characters
or it contains special characters
*/
void CImapLogin::CheckLiteralInUserNameAndPassword()
	{
	iLiteralUsername = EFalse;
	TPtr8 userName = iUserName->Des();
	for(TInt a=0;a<iUserName->Length();a++)
		{
		if (userName[a]<=32  || userName[a]>=127 || userName[a]=='\"' || userName[a]=='%'  ||
		    userName[a]=='(' || userName[a]==')' || userName[a]=='*'  || userName[a]=='\\' ||
		    userName[a]=='{' || userName[a]=='}' )
			{
			iLiteralUsername=ETrue;
			break;
			}
		}

	iLiteralPassword=EFalse;
	TPtr8 password = iPassword->Des();
	for(TInt a=0;a<iPassword->Length();a++)
		{
		if (password[a]<=32  || password[a]>=127 || password[a]=='\"' || password[a]=='%'  ||
		    password[a]=='(' || password[a]==')' || password[a]=='*'  || password[a]=='\\' ||
		    password[a]=='{' || password[a]=='}' )
			{
			iLiteralPassword=ETrue;
			break;
			}
		}
	}
/** 
Formats and sends the IMAP LOGIN command.
@param aTagId Command sequence id which will be send along with the IMAP command.
@param aStream A wrapper for the outbound stream of a connected socket, using which 
the command will be send to the server
*/
void CImapLogin::SendMessageL(TInt aTagId, MOutputStream& aStream)
	{
	iTagId = aTagId;
	iStream = &aStream;
		
	CheckLiteralInUserNameAndPassword();

	__ASSERT_DEBUG(iOutputBuffer==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandOutputBufferNotNull));		
	if (!iLiteralUsername && !iLiteralPassword)
		{
		// send username and password now
		TInt bufLength = KCommandLogin().Length() + TagLength(aTagId) + iUserName->Length() + iPassword->Length();
		iOutputBuffer = HBufC8::NewL(bufLength);
		
		iOutputBuffer->Des().Format(KCommandLogin, aTagId, iUserName, iPassword);		
		}
	else
		{	
		if (iLiteralUsername)
			{
			// send the username's length
			TInt bufLength = KCommandLoginUsernameLength().Length() + TagLength(aTagId) + TagLength(iUserName->Length());
			iOutputBuffer = HBufC8::NewL(bufLength);
			
			iOutputBuffer->Des().Format(KCommandLoginUsernameLength(), aTagId, iUserName->Length());
			iSentUserNameLength = ETrue;	
			}
		else 
			{
			// send the username and the passwords's length
			TInt bufLength = KCommandLoginUsernamePasswordLength().Length() + TagLength(aTagId) + iUserName->Length() + TagLength(iPassword->Length());
			iOutputBuffer = HBufC8::NewL(bufLength);
			iOutputBuffer->Des().Format(KCommandLoginUsernamePasswordLength(), aTagId, iUserName, iPassword->Length());
			}
		}
	
	//send the command to the server
	aStream.SendDataReq(*iOutputBuffer);
	}

void CImapLogin::ParseContinuationResponseL()
	{
	__ASSERT_DEBUG(iStream != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandOutputStreamIsNull));
	
	delete iOutputBuffer;
	iOutputBuffer = NULL;
	
	if(iLiteralUsername && !iLiteralPassword)
		{
		// send the username's literal data and then password.
		TInt bufLength = KCommandContinueUsernameLitPassword().Length() + iUserName->Length() + iPassword->Length();
		iOutputBuffer = HBufC8::NewL(bufLength);				
		
		iOutputBuffer->Des().Format(KCommandContinueUsernameLitPassword, iUserName, iPassword);
		}
	else if(iSentUserNameLength && iLiteralPassword)
		{
		// send the username's literal data and then password's length.
		TInt bufLength = KCommandContinueUsernameLitPasswordLength().Length() + iUserName->Length() + TagLength(iPassword->Length());
		iOutputBuffer = HBufC8::NewL(bufLength);
		
		iOutputBuffer->Des().Format(KCommandContinueUsernameLitPasswordLength, iUserName, iPassword->Length());
		iSentUserNameLength = EFalse;
		}
	else
		{
		// send the password as literal data
		TInt bufLength = KCommandContinuePasswordLit().Length() + iPassword->Length();
		iOutputBuffer = HBufC8::NewL(bufLength);
		
		iOutputBuffer->Des().Format(KCommandContinuePasswordLit, iPassword);
		}
		
	iStream->SendDataReq(*iOutputBuffer);
	}

	
/**
Calls the base class for default parsing,
then checks for and processes the CAPABILITY response text code if it has been sent.
E.g. The tagged response may have been something like
123 OK [CAPABILITY IMAP4rev1 STARTTLS AUTH=PLAIN LOGINDISABLED]
@param aTagId The incoming tag id.
@return ETrue - indicating that the command is completed by this tagged response.
*/
TBool CImapLogin::ParseTaggedResponseL(TInt aTagId)
	{
	BaseParseTaggedResponseL(aTagId);
		
	TPtrC8 responseTextCode = GetResponseTextCodeL();
	if (responseTextCode.Length() > 0)
		{
		// If this is a CAPABILITY response text code, then process it now
		iUnparsedData.Set(responseTextCode);
		TPtrC8 part = GetNextPart();
		
		if (part.CompareF(KTxtImapCapability) == 0)
			{
			ParseCapabilityDataL(iUnparsedData);
			}
		}
		
	return ETrue;
	}

