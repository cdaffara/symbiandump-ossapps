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
//

#include "smtpauthhelpers.h"
#include "SMTSUTIL.H"			// forward declarations for utility fns

#include <e32des8.h>

_LIT8(KPlainMechanism, "PLAIN "); //Space intentional, may find a better way of doing this.
_LIT8(KSmtpAuthLoginCommand, "AUTH LOGIN\r\n");
_LIT8(KCramMD5Mechanism, "CRAM-MD5");

_LIT8(KSmtpAuthBase64StringUsername,"username*"); // To match a folded 'username:' or 'username'
_LIT8(KSmtpAuthBase64StringPassword,"password*"); // To match a folded 'password:' or 'password'

const TInt KMaxLengthOfPlainMessageComponent = 255;
const TInt KMd5BlockLength = 64;

#define BASE64LEN(x) ((x*4)/3) // Every 3 bytes will be turned into 4 bytes

const TInt KPreambleLength = 18; //"AUTH LOGIN\r\nPLAIN "
const TInt KMaxLengthOfPlainMessage=	KPreambleLength + 1/*NUL*/ + KMaxLengthOfPlainMessageComponent/*Username*/ + 1/*NUL*/ + KMaxLengthOfPlainMessageComponent/*Password*/ + 2/* /r/n */;
const TInt KMaxLengthOfPlainMessageBase64= BASE64LEN(KMaxLengthOfPlainMessage);



//
//CSmtpAuthMechanismHelper
//

CSmtpAuthMechanismHelper::CSmtpAuthMechanismHelper(const CSmtpSettings& aSettings) : iSettings(aSettings)
	{
	}
	
//
//CSmtpAuthPlainMechanismHelper
//


CSmtpAuthPlainMechanismHelper::CSmtpAuthPlainMechanismHelper(const CSmtpSettings& aSettings) : CSmtpAuthMechanismHelper(aSettings)
	{
	}

void CSmtpAuthPlainMechanismHelper::GetNextClientMessageL(TDes8& aNextMessage)
	{
	// Build up the un-encoded authorisation string in parts
 	HBufC8* authString = HBufC8::NewMaxLC(KMaxLengthOfPlainMessage); 
 	TPtr8 authStringPtr = authString->Des();

	TBuf8<1> nul;
	nul.SetLength(1);
	nul[0] = 0;
	authStringPtr = nul; // Start with the NUL

	TPtrC8 loginName = iSettings.LoginName();
	TInt length= Min(loginName.Length(),KMaxLengthOfPlainMessageComponent);
	authStringPtr.Append(loginName.Left(length));

	authStringPtr.Append(nul); // Separator between login and password

	length= Min(iSettings.Password().Length(), KMaxLengthOfPlainMessageComponent);
	authStringPtr.Append(iSettings.Password().Left(length));
 
	// Auth string is now built, encode it
	HBufC8* encodedBuf = HBufC8::NewMaxLC(KMaxLengthOfPlainMessageBase64); 
	TPtr8 encodedBufPtr = encodedBuf->Des();
 

	iEncoder.Encode(authStringPtr,encodedBufPtr);


	// Now build the message
	iNextClientMessage.Zero();
 	iNextClientMessage.Append(KSmtpAuthCommand);
 	iNextClientMessage.Append(KPlainMechanism);
	iNextClientMessage.Append(encodedBufPtr);
 	iNextClientMessage.Append(KSmtpCrLf);

 	aNextMessage = iNextClientMessage;

	CleanupStack::PopAndDestroy(encodedBuf);
	CleanupStack::PopAndDestroy(authString);		
	}

void CSmtpAuthPlainMechanismHelper::SetLastServerMessageL(const TDesC8& /*aLastMessage*/, TBool /*aIsMultiLineResponse*/)
	{
	//iLastServerMessage.Set(aLastMessage);
	}


//
//CSmtpAuthLoginMechanismHelper
//

CSmtpAuthLoginMechanismHelper* CSmtpAuthLoginMechanismHelper::NewL(const CSmtpSettings& aSettings)  
	{
	CSmtpAuthLoginMechanismHelper* self= new (ELeave) CSmtpAuthLoginMechanismHelper(aSettings);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();	                                
	return self;
	}

CSmtpAuthLoginMechanismHelper::CSmtpAuthLoginMechanismHelper(const CSmtpSettings& aSettings) : CSmtpAuthMechanismHelper(aSettings)
	{
	}

CSmtpAuthLoginMechanismHelper::~CSmtpAuthLoginMechanismHelper()
	{
	delete iBase64LoginName;
	delete iBase64Password;
	}

void CSmtpAuthLoginMechanismHelper::ConstructL()
	{
	TInt len=(((iSettings.LoginName().Length()/3)+1)*4)+2; // length of LoginName in base 64 + 2 for CRLF
	iBase64LoginName=HBufC8::NewL(len);
	TPtr8 authString(iBase64LoginName->Des());
	iEncoder.Encode(iSettings.LoginName(),authString);
	authString.Append(KSmtpCrLf);
	len=(((iSettings.Password().Length()/3)+1)*4)+2; // length of Password in base 64 + 2 for CRLF
	iBase64Password=HBufC8::NewL(len);
	authString.Set(iBase64Password->Des());
	iEncoder.Encode(iSettings.Password(),authString);
	authString.Append(KSmtpCrLf);
	}

void CSmtpAuthLoginMechanismHelper::GetNextClientMessageL(TDes8& aNextMessage)
	{
	iNextClientMessage.Zero();
	switch (iState)
		{
		case ESendingAuth:
			iNextClientMessage=KSmtpAuthLoginCommand;
			break;
		case ESendingLoginName:
			iNextClientMessage=*iBase64LoginName;
			break;
		case ESendingPassword:
			iNextClientMessage=*iBase64Password;
			break;
		default:
	        gPanic(EImsmSmtpAuthLoginBadState);
			break;
		}
	aNextMessage=iNextClientMessage;
	}

void CSmtpAuthLoginMechanismHelper::SetLastServerMessageL(const TDesC8& aLastMessage, TBool /*aIsMultiLineResponse*/)
	{
	HBufC8* decodedMessage = NULL;
	TInt lastMessageLength = aLastMessage.Length();
    // Decode the SMTP response from the Base64 original
    if (lastMessageLength > 0)
        {
        decodedMessage = HBufC8::NewLC(lastMessageLength); // already on stack
        // Get pointer to response minus the "334 " at the start
        TPtrC8 response = aLastMessage.Right(lastMessageLength - 4);
        TPtr8  destination = decodedMessage->Des();
        iEncoder.Decode(response, destination);
        }
	switch (iState)
		{
		case ESendingAuth:
            if (decodedMessage)
                {
                // Match against 'username*'
                if (decodedMessage->Des().MatchF(KSmtpAuthBase64StringUsername) == KErrNotFound)
					{
                    User::Leave(KErrNotSupported);
					}
				//Some SMTP server don't follow the RFC 2554 completely, so at the end of 334 replies it appends information
				//string which is not BASE64 encoded. In such cases, after decoding the first 334 reply, it sometimes 
				//leaves BASE64 decoder in incorrect state. So the second reply isn't decoded properly, to avoid 
				//this we need to initialise the encoder
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
                if (decodedMessage->Des().MatchF(KSmtpAuthBase64StringPassword) == KErrNotFound)
					{
                    User::Leave(KErrNotSupported);
					}
				//Some SMTP server don't follow the RFC 2554 completely, so at the end of 334 replies it appends information
				//string which is not BASE64 encoded. In such cases, after decoding the first 334 reply, it sometimes 
				//leaves BASE64 decoder in incorrect state. So the second reply isn't decoded properly, to avoid 
				//this we need to initialise the encoder
				iEncoder.Initialise();
                iState = ESendingPassword;
                }
			else
                {
				User::Leave(KErrNotSupported);
				}
			break;
		case ESendingPassword: // the helper shouldn't be handling the 235 response
		default:
			User::Leave(KErrNotSupported);
			break;
		}
	if (decodedMessage)
		{
		CleanupStack::PopAndDestroy(decodedMessage);
		}
	}

//
//CSmtpAuthCramMd5MechanismHelper
//

CSmtpAuthCramMd5MechanismHelper* CSmtpAuthCramMd5MechanismHelper::NewL(const CSmtpSettings& aSettings)  
	{
	CSmtpAuthCramMd5MechanismHelper* self= new (ELeave) CSmtpAuthCramMd5MechanismHelper(aSettings);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();	                                
	return self;
	}

CSmtpAuthCramMd5MechanismHelper::CSmtpAuthCramMd5MechanismHelper(const CSmtpSettings& aSettings) : CSmtpAuthMechanismHelper(aSettings)
	{
	}


void CSmtpAuthCramMd5MechanismHelper::ConstructL()
	{
	iMd5Hash = CMD5::NewL();
	}

CSmtpAuthCramMd5MechanismHelper::~CSmtpAuthCramMd5MechanismHelper()
	{
	delete iMd5Hash;
	}


void CSmtpAuthCramMd5MechanismHelper::GetNextClientMessageL(TDes8& aNextMessage)
	{
	iNextClientMessage.Zero();

	if(iInProgress)
		{
		HBufC8* authbuffer = HBufC8::NewMaxLC(KImMailMaxBufferSize);//? 
		TPtr8 authbufferptr = authbuffer->Des();
		HBufC8* authbuffer2 = HBufC8::NewMaxLC(KImMailMaxBufferSize);//? 
		TPtr8 authbufferptr2 = authbuffer2->Des();
		HBufC8* authbuffer3 = HBufC8::NewMaxLC(KImMailMaxBufferSize);//? 
		TPtr8 authbufferptr3 = authbuffer3->Des();
		
		authbufferptr = iLastServerMessage;
		authbufferptr.Delete(0,4); //remove 334 from the begining of server response
		iEncoder.Decode(authbufferptr,authbufferptr2);
		//timestamp info now in authbufferptr2

		authbufferptr.Zero();
		authbufferptr3.Zero();
		authbufferptr = iSettings.Password();
		authbufferptr3 = FormSharedSecret(authbufferptr);
		authbufferptr.Zero();
		authbufferptr = authbufferptr3;  //now authbufferptr and authbufferptr3 contain the shared secret null padded to 64 bytes.
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
		iNextClientMessage.Append(KSmtpCrLf);
		CleanupStack::PopAndDestroy(3);	// authbufferptr3 ,authbufferptr2, authbufferptr
		aNextMessage = iNextClientMessage;
		}
	else
		{
		iNextClientMessage.Append(KSmtpAuthCommand);
		iNextClientMessage.Append(KCramMD5Mechanism);
		iNextClientMessage.Append(KSmtpCrLf);
		iInProgress=ETrue;
		aNextMessage = iNextClientMessage;
		}
	}

TPtr8 CSmtpAuthCramMd5MechanismHelper::FormSharedSecret(TPtr8 aPassword)
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
		//do we need to call this function again now to pad out the 16 byte result?
		}

	return secret;
	}

void CSmtpAuthCramMd5MechanismHelper::SetLastServerMessageL(const TDesC8& aLastMessage, TBool /*aIsMultiLineResponse*/)
	{
	iLastServerMessage.Set(aLastMessage);
	}


//
//CSmtpAuthMechanismHelper factory function
//
GLDEF_C CSmtpAuthMechanismHelper* CreateSMTPAuthHelperL(CSmtpAuthMechanismHelper::TSmtpAuthProfileFlag aType, const CSmtpSettings& aSettings)
	{
	//Factory function that returns relevant Helper corresponding to aType
	switch(aType)
		{
	case CSmtpAuthMechanismHelper::EPlain:
		{
		CSmtpAuthMechanismHelper* helper;
		helper = new (ELeave) CSmtpAuthPlainMechanismHelper(aSettings);
		return helper;
		}
	case CSmtpAuthMechanismHelper::ELogin:
		{
		CSmtpAuthLoginMechanismHelper* smtpAuthLogin = CSmtpAuthLoginMechanismHelper::NewL(aSettings);
		return smtpAuthLogin;
		}
	case CSmtpAuthMechanismHelper::ECramMD5:
		{
		CSmtpAuthCramMd5MechanismHelper* smtpAuthCramMd5 = CSmtpAuthCramMd5MechanismHelper::NewL(aSettings);
		return smtpAuthCramMd5;
		}
	default:
	    User::Leave(KErrNotSupported); //gPanic(EImsmBadSmtpAuthProfile2);
		return NULL;
		}
	}
