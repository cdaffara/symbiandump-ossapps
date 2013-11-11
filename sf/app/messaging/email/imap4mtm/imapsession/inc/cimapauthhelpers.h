// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// IMAPAUTHHELPERS.H
// This file specifies the "helper" classes that generate the Client responses 
// for a particular Imap Auth mechanism. The last Server message is required 
// to generate the Client response
// 
//

#if !defined(_IMAPAUTHHELPERS_H__)
#define _IMAPAUTHHELPERS_H__

#include <imcvcodc.h>
#include <imapset.h>
#include "cimapsettings.h"
#include <hash.h>				//hashing functions for Cramm Md5
#include <imcvcodc.h>
#include <timrfc822datefield.h>
#include "cimapcommand.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "timrfc822datefield.h"	
#endif

class MOutputStream;

/**
CImapAuthMechanismHelper is the base class for IMAP4 AUTH command.
Using this class SASL (Simple Authentication and Security Layer) mechanisms 
can be exteneded.
Right now CRAM-MD5, PLAIN and LOGIN authentication mechanisms are supported.

@internalComponent
@prototype
*/
NONSHARABLE_CLASS ( CImapAuthMechanismHelper) : public CImapCommandEx
{
public:
	enum TImapAuthProfileFlag
		{
		ENoProfile=0x00,
		EPlain=0x01,
		ELogin=0x02,
		ECramMD5=0x04,
		EUndefined=0x08, // must be last flag
		};
protected:
	CImapAuthMechanismHelper(const CImapSettings& aSettings,CImapFolderInfo* aSelectedFolderData, TInt aLogId);
	const CImapSettings& iSettings;
	TBuf8<KImMailMaxBufferSize> iNextClientMessage;
	};

/**
CImapAuthPlainMechanismHelper class is extended to support the AUTH command as 
specified in RFC2195, to indicate PLAIN authentication mechanism to the server.

@internalComponent
@prototype
*/
NONSHARABLE_CLASS ( CImapAuthPlainMechanismHelper) : public CImapAuthMechanismHelper

	{
public:
	static CImapAuthPlainMechanismHelper* NewL(const CImapSettings& aSettings,CImapFolderInfo* aSelectedFolderData, TInt aLogId);
public:
	void SendMessageL(TInt aTagId, MOutputStream& aStream);
	void ParseContinuationResponseL();
private:
	CImapAuthPlainMechanismHelper(const  CImapSettings& aSettings,CImapFolderInfo* aSelectedFolderData, TInt aLogId);
	void ConstructL();
	TImCodecB64 iEncoder;
	MOutputStream* iStream;
	};

/**
CImapAuthLoginMechanismHelper class is extended to support the AUTH command as 
specified in RFC2195, to indicate LOGIN authentication mechanism to the server.

@internalComponent
@prototype
*/
NONSHARABLE_CLASS (CImapAuthLoginMechanismHelper) : public CImapAuthMechanismHelper

	{
public:
	static CImapAuthLoginMechanismHelper* NewL(const CImapSettings& aSettings,CImapFolderInfo* aSelectedFolderData, TInt aLogId);
	~CImapAuthLoginMechanismHelper();
public:
	void SendMessageL(TInt aTagId, MOutputStream& aStream);
	void ParseContinuationResponseL();
private:
	CImapAuthLoginMechanismHelper(const  CImapSettings& aSettings,CImapFolderInfo* aSelectedFolderData, TInt aLogId);
	void ConstructL();
	
private:
	enum TLoginState
		{
		ESendingLoginName=0,
		ESendingPassword,
		};
	TLoginState iState;
	TImCodecB64 iEncoder;
	HBufC8* iBase64LoginName;
	HBufC8* iBase64Password;
	MOutputStream* iStream;
	};

/**
CImapAuthCramMd5MechanismHelper class is extended to support the AUTH command as 
specified in RFC2195, to indicate CRAM-MD5 authentication mechanism to the server.

@internalComponent
@prototype
*/
NONSHARABLE_CLASS ( CImapAuthCramMd5MechanismHelper) : public CImapAuthMechanismHelper
{
public:
	static CImapAuthCramMd5MechanismHelper* NewL(const CImapSettings& aSettings,CImapFolderInfo* aSelectedFolderData, TInt aLogId);
	~CImapAuthCramMd5MechanismHelper();
public:
	void SendMessageL(TInt aTagId, MOutputStream& aStream);
	void ParseContinuationResponseL();
private:
	CImapAuthCramMd5MechanismHelper(const CImapSettings& aSettings,CImapFolderInfo* aSelectedFolderData, TInt aLogId);
	void ConstructL();
private:
	TPtr8 FormSharedSecret(TPtr8 aPassword);
private:
	TImCodecB64 iEncoder;
	CMD5* iMd5Hash;
	MOutputStream* iStream;
	};
#endif

