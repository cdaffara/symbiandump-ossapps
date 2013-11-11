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
// POPAUTHHELPER.H
// 
//

#ifndef __CPOPSASLAUTHHELPER_H__
#define __CPOPSASLAUTHHELPER_H__

#include <imcvcodc.h>
#include <pop3set.h>
#include <hash.h>	
#include <imcvcodc.h>
#include <timrfc822datefield.h> // needed for KImMailMaxBufferSize 

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "timrfc822datefield.h"				
#endif

/**
CPopAuthMechanismHelper is the base class for POP3 AUTH command.
Using this class SASL (Simple Authentication and Security Layer) mechanisms 
can be exteneded.
Right now CRAM-MD5, PLAIN and LOGIN authentication mechanisms are supported.

@internalComponent
@prototype
*/
NONSHARABLE_CLASS (CPopAuthMechanismHelper) : public CBase
	{
public:
	enum TPopAuthProfileFlag
		{
		ENoProfile=0x00,
		EPlain=0x01,
		ELogin=0x02,
		ECramMD5=0x04,
		EUndefined=0x08, // must be last flag
		};

protected:
	CPopAuthMechanismHelper(const CImPop3Settings& aSettings);
public:
	virtual void GetNextClientMessageL(TDes8& aNextMessage)=0;
	virtual void SetLastServerMessageL(const TDesC8& aLastMessage, TBool aIsMultiLineResponse)=0;
protected:
	const CImPop3Settings& iSettings;
	TPtrC8 iLastServerMessage;
	TBuf8<KImMailMaxBufferSize> iNextClientMessage;

#if(defined SYMBIAN_SERVERMTM_UNIT_TESTCODE)
	friend class CTestPopSaslAuthHelper;
#endif	
	};


/**
CPopAuthCramMd5MechanismHelper class is extended to support the AUTH command as 
specified in RFC1734, to indicate CRAM-MD5 authentication mechanism to the server.

@internalComponent
@prototype
*/
NONSHARABLE_CLASS( CPopAuthCramMd5MechanismHelper ) : public CPopAuthMechanismHelper
	{
public:
	static CPopAuthCramMd5MechanismHelper* NewL(const CImPop3Settings& aSettings);
	~CPopAuthCramMd5MechanismHelper();
public:
	void GetNextClientMessageL(TDes8& aNextMessage);
	void SetLastServerMessageL(const TDesC8& aLastMessage, TBool aIsMultiLineResponse);
private:
	CPopAuthCramMd5MechanismHelper(const CImPop3Settings& aSettings);
	void ConstructL();
private:
	TPtr8 FormSharedSecret(TPtr8 aPassword);
private:
	TBool iInProgress;
	TImCodecB64 iEncoder;
	CMD5* iMd5Hash;

#if(defined SYMBIAN_SERVERMTM_UNIT_TESTCODE)
	friend class CTestPopSaslAuthHelper;
#endif	
	};


/**
CPopAuthPlainMechanismHelper class is extended to support the AUTH command as 
specified in RFC1734, to indicate PLAIN authentication mechanism to the server.

@internalComponent
@prototype
*/
NONSHARABLE_CLASS( CPopAuthPlainMechanismHelper ): public CPopAuthMechanismHelper
	{
public:
	static CPopAuthPlainMechanismHelper* NewL(const CImPop3Settings& aSettings);
	CPopAuthPlainMechanismHelper(const CImPop3Settings& aSettings);
public:
	void GetNextClientMessageL(TDes8& aNextMessage);
	void SetLastServerMessageL(const TDesC8& aLastMessage, TBool aIsMultiLineResponse);
private:
	TBool iInProgress;
	TImCodecB64 iEncoder;

#if(defined SYMBIAN_SERVERMTM_UNIT_TESTCODE)
	friend class CTestPopSaslAuthHelper;
#endif	

	};


/**
CPopAuthLoginMechanismHelper class is extended to support the AUTH command as 
specified in RFC1734, to indicate LOGIN authentication mechanism to the server.

@internalComponent
@prototype
*/
//class CPopAuthLoginMechanismHelper : public CPopAuthMechanismHelper
NONSHARABLE_CLASS( CPopAuthLoginMechanismHelper ): public CPopAuthMechanismHelper
	{
public:
	static CPopAuthLoginMechanismHelper* NewL(const CImPop3Settings& aSettings);
	~CPopAuthLoginMechanismHelper();
public:
	void GetNextClientMessageL(TDes8& aNextMessage);
	void SetLastServerMessageL(const TDesC8& aLastMessage, TBool aIsMultiLineResponse);
private:
	CPopAuthLoginMechanismHelper(const CImPop3Settings& aSettings);
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

#if(defined SYMBIAN_SERVERMTM_UNIT_TESTCODE)
	friend class CTestPopSaslAuthHelper;
#endif	

	};

#endif // __CPOPSASLAUTHHELPER_H


