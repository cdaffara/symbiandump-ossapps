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
//

#include "cpopsaslauthhelper.h"
#include <e32des8.h>
#include "POPS.PAN"   		// pop's panic codes

//string constants containing POP3 AUTH command
_LIT8(KPop3AuthCommand, "AUTH ");
_LIT8(KCramMD5Mechanism, "CRAM-MD5");
_LIT8(KPlainMechanism, "PLAIN ");
_LIT8(KPop3AuthLoginCommand, "AUTH LOGIN\r\n");
_LIT8(KPop3CrLf,"\r\n");

_LIT8(KPop3AuthBase64StringUsername,"username*"); // To match a folded 'username:' or 'username'
_LIT8(KPop3AuthBase64StringPassword,"password*"); // To match a folded 'password:' or 'password'

const TInt KMaxLengthOfPlainMessageComponent = 255;
const TInt KMd5BlockLength = 64;

#define BASE64LEN(x) ((x*4)/3) // Every 3 bytes will be turned into 4 bytes

const TInt KPreambleLength = 18; //"AUTH LOGIN\r\nPLAIN "
const TInt KMaxLengthOfPlainMessage=	KPreambleLength + 1/*NUL*/ + KMaxLengthOfPlainMessageComponent/*Username*/ + 1/*NUL*/ + KMaxLengthOfPlainMessageComponent/*Password*/ + 2/* /r/n */;
const TInt KMaxLengthOfPlainMessageBase64= BASE64LEN(KMaxLengthOfPlainMessage);


/**
Constructor for CPopAuthMechanismHelper
*/
CPopAuthMechanismHelper::CPopAuthMechanismHelper(const CImPop3Settings& aSettings) : iSettings(aSettings)
	{
	}

/** 
Allocates and constructs a CPopAuthCramMd5MechanismHelper object.
@param aSettings: Refence to CImPop3Settings 
@leave KErrNoMemory A memory allocation failed 
*/
CPopAuthCramMd5MechanismHelper* CPopAuthCramMd5MechanismHelper::NewL(const CImPop3Settings& aSettings)  
	{
	CPopAuthCramMd5MechanismHelper* self= new (ELeave) CPopAuthCramMd5MechanismHelper(aSettings);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();	                                
	return self;
	}

/**
Constructor for CPopAuthMechanismHelper
*/
CPopAuthCramMd5MechanismHelper::CPopAuthCramMd5MechanismHelper(const CImPop3Settings& aSettings) : CPopAuthMechanismHelper(aSettings)
	{
	}


/**
Second phase construction
*/
void CPopAuthCramMd5MechanismHelper::ConstructL()
	{
	iMd5Hash = CMD5::NewL();
	}

/**
Destructor
*/
CPopAuthCramMd5MechanismHelper::~CPopAuthCramMd5MechanismHelper()
	{
	delete iMd5Hash;
	}


/**
Function to form POP3 "AUTH CRAM-MD5" command and to handle base64 encoding of 
the challenges and responses as per RFC 2195
@param aNextMessage: contains response command
*/
void CPopAuthCramMd5MechanismHelper::GetNextClientMessageL(TDes8& aNextMessage)
	{
	iNextClientMessage.Zero();

	if(iInProgress)
		{
		HBufC8* authbuffer = HBufC8::NewMaxLC(KImMailMaxBufferSize); 
		TPtr8 authbufferptr = authbuffer->Des();
		HBufC8* authbuffer2 = HBufC8::NewMaxLC(KImMailMaxBufferSize);
		TPtr8 authbufferptr2 = authbuffer2->Des();
		HBufC8* authbuffer3 = HBufC8::NewMaxLC(KImMailMaxBufferSize);
		TPtr8 authbufferptr3 = authbuffer3->Des();
		
		authbufferptr = iLastServerMessage;
		// to remove "+ " from the begining of server response
		authbufferptr.Delete(0,2); 
		
		//timestamp info now in authbufferptr2
		iEncoder.Decode(authbufferptr,authbufferptr2);
		
		authbufferptr.Zero();
		authbufferptr3.Zero();
		authbufferptr = iSettings.Password();
		authbufferptr3 = FormSharedSecret(authbufferptr);
		authbufferptr.Zero();
		//now authbufferptr and authbufferptr3 contain the shared secret null padded to 64 bytes.
		authbufferptr = authbufferptr3;  
		
		TInt i=0;
		for (i=0; i<KMd5BlockLength; i++) 
			{
			authbufferptr[i] ^= 0x36; //ipad
			authbufferptr3[i] ^= 0x5c; //opad
			}

		authbufferptr.Append(authbufferptr2);

    	iMd5Hash->Reset();                                                                                                                                                                                                                                                                                                                ;
		authbufferptr2 = iMd5Hash->Hash(authbufferptr);

		authbufferptr3.Append(authbufferptr2);
		
		authbufferptr.Zero();
		iMd5Hash->Reset();
		authbufferptr = iMd5Hash->Hash(authbufferptr3);

		// MD5 algorithm ALWAYS returns 16 bytes of data - which will be converted into
		// 32 characters; each byte represented by a 2 character hex representation, 
		// eg 255="ff"
		TBuf<32> hexHash; 
		for (i=0;i<16;i++)
			{
			hexHash.AppendNumFixedWidth(authbufferptr[i],EHex,2);
			}

		authbufferptr3.Zero();
		authbufferptr3.Append(iSettings.LoginName());
		authbufferptr3.Append(_L8(" "));
		authbufferptr3.Append(hexHash);

		iEncoder.Encode(authbufferptr3, iNextClientMessage);
		iNextClientMessage.Append(KPop3CrLf);
		CleanupStack::PopAndDestroy(3);	// authbufferptr3 ,authbufferptr2, authbufferptr
		aNextMessage = iNextClientMessage;
		}
	else
		{
		iNextClientMessage.Append(KPop3AuthCommand);
		iNextClientMessage.Append(KCramMD5Mechanism);
		iNextClientMessage.Append(KPop3CrLf);
		iInProgress=ETrue;
		aNextMessage = iNextClientMessage;
		}
	}

/**
FormSharedSecret()
@param aPassword: password
*/
TPtr8 CPopAuthCramMd5MechanismHelper::FormSharedSecret(TPtr8 aPassword)
	{
	TPtr8 secret = aPassword;
	
	if (aPassword.Length() <= KMd5BlockLength)
		{
		//pad up to 64 bytes with zeros
		secret.SetLength(64);
		TInt origLen = aPassword.Length();
		TInt lengthToFill = KMd5BlockLength-aPassword.Length();
		for (TInt i = 0; i<lengthToFill; i++)
			{
			secret[origLen+i]=0x00;
			}
		}
	else
		{
		//make shared secret the Digest of the password
		secret.Zero();
		iMd5Hash->Reset();
		secret = (iMd5Hash->Hash(aPassword));
		}

	return secret;
	}


/**
To set the last message from server.
@param aPassword: password
*/
void CPopAuthCramMd5MechanismHelper::SetLastServerMessageL(const TDesC8& aLastMessage, TBool /*aIsMultiLineResponse*/)
	{
	iLastServerMessage.Set(aLastMessage);
	}


/**
Constructor
*/
CPopAuthPlainMechanismHelper::CPopAuthPlainMechanismHelper(const CImPop3Settings& aSettings) : CPopAuthMechanismHelper(aSettings)
	{
	}


/** 
Allocates and constructs a CPopAuthPlainMechanismHelper object.
@param aSettings: Refence to CImPop3Settings 
@leave KErrNoMemory A memory allocation failed 
*/
CPopAuthPlainMechanismHelper* CPopAuthPlainMechanismHelper::NewL(const CImPop3Settings& aSettings)  
	{
	CPopAuthPlainMechanismHelper* self= new (ELeave) CPopAuthPlainMechanismHelper(aSettings);
	return self;
	}

	
/**
Function to handle POP3 "AUTH PLIAN" mechanism.
Steps:
1. Get the LoginName from CImPop3Settings
2. Get the Password from CImPop3Settings
3. Add "LoginName","NUL Char" and "Password" in a Buffer
4. Convert the above buffer to BASE64
@param aNextMessage: Buffer contains BASE64 converted LoginName and Password.
*/
void CPopAuthPlainMechanismHelper::GetNextClientMessageL(TDes8& aNextMessage)
	{
	// Build up the un-encoded authorisation string in parts
 	HBufC8* authString = HBufC8::NewMaxLC(KMaxLengthOfPlainMessage); 
 	TPtr8 authStringPtr = authString->Des();

	TBuf8<1> nulChar;
	nulChar.SetLength(1);
	nulChar[0] = 0;
	authStringPtr = nulChar; // Start with the NUL

	TPtrC8 loginName = iSettings.LoginName();
	TInt length= Min(loginName.Length(),KMaxLengthOfPlainMessageComponent);
	authStringPtr.Append(loginName.Left(length));

	authStringPtr.Append(nulChar); // Separator between login and password

	length= Min(iSettings.Password().Length(), KMaxLengthOfPlainMessageComponent);
	authStringPtr.Append(iSettings.Password().Left(length));
 
	// Auth string is now built, encode it
	HBufC8* encodedBuf = HBufC8::NewMaxLC(KMaxLengthOfPlainMessageBase64); 
	TPtr8 encodedBufPtr = encodedBuf->Des();
 
	iEncoder.Encode(authStringPtr,encodedBufPtr);

	// Now build the message
	iNextClientMessage.Zero();
 	iNextClientMessage.Append(KPop3AuthCommand);
 	iNextClientMessage.Append(KPlainMechanism);
	iNextClientMessage.Append(encodedBufPtr);
 	iNextClientMessage.Append(KPop3CrLf);

 	aNextMessage = iNextClientMessage;

	CleanupStack::PopAndDestroy(2, authString); // encodedBuf, authString
	}

/**
To set the last message from server.
@param aPassword: password
*/
void CPopAuthPlainMechanismHelper::SetLastServerMessageL(const TDesC8& /*aLastMessage*/, TBool /*aIsMultiLineResponse*/)
	{
	//iLastServerMessage.Set(aLastMessage);
	}

/**
Constructor
*/
CPopAuthLoginMechanismHelper::CPopAuthLoginMechanismHelper(const CImPop3Settings& aSettings) : CPopAuthMechanismHelper(aSettings)
	{
	}

/** 
Allocates and constructs a CPopAuthLoginMechanismHelper object.
@param aSettings: Refence to CImPop3Settings 
@leave KErrNoMemory A memory allocation failed 
*/
CPopAuthLoginMechanismHelper* CPopAuthLoginMechanismHelper::NewL(const CImPop3Settings& aSettings)  
	{
	CPopAuthLoginMechanismHelper* self= new (ELeave) CPopAuthLoginMechanismHelper(aSettings);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();	                                
	return self;
	}

/**
Destructor
*/
CPopAuthLoginMechanismHelper::~CPopAuthLoginMechanismHelper()
	{
	delete iBase64LoginName;
	delete iBase64Password;
	}

/**
SecondPhase  construction
*/
void CPopAuthLoginMechanismHelper::ConstructL()
	{
	// length of LoginName in base 64 + 2 for CRLF
	TInt len=(((iSettings.LoginName().Length()/3)+1)*4)+2; 
	iBase64LoginName=HBufC8::NewL(len);
	TPtr8 authString(iBase64LoginName->Des());
	iEncoder.Encode(iSettings.LoginName(),authString);
	authString.Append(KPop3CrLf);
	
	// length of Password in base 64 + 2 for CRLF
	len=(((iSettings.Password().Length()/3)+1)*4)+2; 
	iBase64Password=HBufC8::NewL(len);
	authString.Set(iBase64Password->Des());
	iEncoder.Encode(iSettings.Password(),authString);
	authString.Append(KPop3CrLf);
	}


/**
Function to handle POP3 "AUTH LOGIN" mechanism.
@param aNextMessage: aNextMessage contains BASE64 converted LoginName/Password.
*/
void CPopAuthLoginMechanismHelper::GetNextClientMessageL(TDes8& aNextMessage)
	{
	iNextClientMessage.Zero();
	switch (iState)
		{
		case ESendingAuth:
			iNextClientMessage=KPop3AuthLoginCommand;
			break;
		case ESendingLoginName:
			iNextClientMessage=*iBase64LoginName;
			break;
		case ESendingPassword:
			iNextClientMessage=*iBase64Password;
			break;
		default:
			Panic(EPopInvalidState);
	        break;
		}
	aNextMessage=iNextClientMessage;
	}

void CPopAuthLoginMechanismHelper::SetLastServerMessageL(const TDesC8& aLastMessage, TBool /*aIsMultiLineResponse*/)
	{
	HBufC8* decodedMessage = NULL;
	TInt lastMessageLength = aLastMessage.Length();
    
    // Decode the POP response from the Base64 original
    if (lastMessageLength > 0)
        {
        decodedMessage = HBufC8::NewLC(lastMessageLength); // already on stack
        // Get pointer to response minus the "+ " at the start
        TPtrC8 response = aLastMessage.Right(lastMessageLength - 2);
        TPtr8  destination = decodedMessage->Des();
        iEncoder.Decode(response, destination);
        }
	switch (iState)
		{
		case ESendingAuth:
            if (decodedMessage)
                {
                // Match against 'username*'
                if (decodedMessage->Des().MatchF(KPop3AuthBase64StringUsername) == KErrNotFound)
					{
                    User::Leave(KErrNotSupported);
					}
				iEncoder.Initialise();
                iState = ESendingLoginName;
                }
            else
                {
				User::Leave(KErrNotSupported);
				}
			break;
		case ESendingLoginName:
			if (decodedMessage)
                {
                // Match against 'password*'
                if (decodedMessage->Des().MatchF(KPop3AuthBase64StringPassword) == KErrNotFound)
					{
                    User::Leave(KErrNotSupported);
					}
				iEncoder.Initialise();
                iState = ESendingPassword;
                }
			else
                {
				User::Leave(KErrNotSupported);
				}
			break;
		case ESendingPassword: 
		default:
			User::Leave(KErrNotSupported);
			break;
		}
		
	if (decodedMessage)
		{
		CleanupStack::PopAndDestroy(decodedMessage);
		}
	}
	
