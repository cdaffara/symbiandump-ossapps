// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file specifies the "helper" classes that generate the Client responses 
// for a particular SMTP Auth mechanism. The last Server message is required 
// to generate the Client response
// 
//

 
#if !defined(_SMTPAUTHHELPERS_H__)
#define _SMTPAUTHHELPERS_H__

#include <imcvcodc.h>
#include <smtpset.h>
#include "csmtpsettings.h"
#include <hash.h>				//hashing functions for Cramm Md5
#include <imcvcodc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "timrfc822datefield.h"
#endif

// Base class for SMTP Auth "helper" mechanism
class CSmtpAuthMechanismHelper : public CBase
/**
@internalComponent
@released
*/
	{
public:
	enum TSmtpAuthProfileFlag
		{
		ENoProfile=0x00,
		EPlain=0x01,
		ELogin=0x02,
		ECramMD5=0x04,
		EUndefined=0x08, // must be last flag
		};
protected:
	CSmtpAuthMechanismHelper(const CSmtpSettings& aSettings);
public:
	virtual void GetNextClientMessageL(TDes8& aNextMessage)=0;
	virtual void SetLastServerMessageL(const TDesC8& aLastMessage, TBool aIsMultiLineResponse)=0;
protected:
	const CSmtpSettings& iSettings;
	TPtrC8 iLastServerMessage;
	TBuf8<KImMailMaxBufferSize> iNextClientMessage;
	};

// Smtp Auth 'PLAIN mechanism helper
class CSmtpAuthPlainMechanismHelper : public CSmtpAuthMechanismHelper
/**
@internalComponent
@released
*/
	{
public:
	CSmtpAuthPlainMechanismHelper(const CSmtpSettings& aSettings);
public:
	void GetNextClientMessageL(TDes8& aNextMessage);
	void SetLastServerMessageL(const TDesC8& aLastMessage, TBool aIsMultiLineResponse);
private:
	TImCodecB64 iEncoder;
	};

// Smtp Auth 'LOGIN' mechanism helper
class CSmtpAuthLoginMechanismHelper : public CSmtpAuthMechanismHelper
/**
@internalComponent
@released
*/
	{
public:
	static CSmtpAuthLoginMechanismHelper* NewL(const CSmtpSettings& aSettings);
	~CSmtpAuthLoginMechanismHelper();
public:
	void GetNextClientMessageL(TDes8& aNextMessage);
	void SetLastServerMessageL(const TDesC8& aLastMessage, TBool aIsMultiLineResponse);
private:
	CSmtpAuthLoginMechanismHelper(const CSmtpSettings& aSettings);
	void ConstructL();
private:
	enum TLoginState
		{
		ESendingAuth=0,
		ESendingLoginName,
		ESendingPassword,
		};
	TLoginState iState;
	TImCodecB64 iEncoder;
	HBufC8* iBase64LoginName;
	HBufC8* iBase64Password;
	};

// Smtp Auth 'CRAM-MD5' mechanism helper
class CSmtpAuthCramMd5MechanismHelper : public CSmtpAuthMechanismHelper
/**
@internalComponent
@released
*/
	{
public:
	static CSmtpAuthCramMd5MechanismHelper* NewL(const CSmtpSettings& aSettings);
	~CSmtpAuthCramMd5MechanismHelper();
public:
	void GetNextClientMessageL(TDes8& aNextMessage);
	void SetLastServerMessageL(const TDesC8& aLastMessage, TBool aIsMultiLineResponse);
private:
	CSmtpAuthCramMd5MechanismHelper(const CSmtpSettings& aSettings);
	void ConstructL();
private:
	TPtr8 FormSharedSecret(TPtr8 aPassword);
private:
	TBool iInProgress;
	TImCodecB64 iEncoder;
	CMD5* iMd5Hash;
	};

GLREF_C CSmtpAuthMechanismHelper* CreateSMTPAuthHelperL(CSmtpAuthMechanismHelper::TSmtpAuthProfileFlag aType, const CSmtpSettings& aSettings); //factory function

#endif
