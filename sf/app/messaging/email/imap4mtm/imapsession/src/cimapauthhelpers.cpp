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
// IMAPAUTHHELPERS.CPP
// 
//

#include "cimapauthhelpers.h"
#include "cimaputils.h"			// forward declarations for utility fns
#include "cimaplogger.h"
#include "moutputstream.h"
#include  "cimapsessionconsts.h"
#include <e32des8.h>


// IMAP LOGIN command
_LIT8(KCommandAuthenticateCrammd5, "%d AUTHENTICATE CRAM-MD5\r\n");
_LIT8(KCommandAuthenticatePlain, "%d AUTHENTICATE PLAIN ");
_LIT8(KCommandAuthenticateLogin, "%d AUTHENTICATE LOGIN\r\n");
_LIT8(KImapAuthBase64StringUsername,"username*"); // To match a folded 'username:' or 'username'
_LIT8(KImapAuthBase64StringPassword,"password*"); // To match a folded 'password:' or 'password'

const TInt KMaxLengthOfPlainMessageComponent = 255;
const TInt KMd5BlockLength = 64;

#define BASE64LEN(x) ((x*4)/3) // Every 3 bytes will be turned into 4 bytes

const TInt KPreambleLength = 18; //"AUTH LOGIN\r\nPLAIN "
const TInt KMaxLengthOfPlainMessage=	KPreambleLength + 1/*NUL*/ + KMaxLengthOfPlainMessageComponent/*Username*/ + 1/*NUL*/ + KMaxLengthOfPlainMessageComponent/*Password*/ + 2/* /r/n */;
const TInt KMaxLengthOfPlainMessageBase64= BASE64LEN(KMaxLengthOfPlainMessage);


 
 /**
Class constructor

@param aImapSettings Settings for IMAP service
@param aSelectedFolderData Imap Folder Info
@param aLogId logging info
*/
 CImapAuthMechanismHelper::CImapAuthMechanismHelper(const CImapSettings& aSettings,CImapFolderInfo* aSelectedFolderData, TInt aLogId) : CImapCommandEx(aSelectedFolderData, aLogId),iSettings(aSettings){}


/**
Static factory constructor. Part of two phased construction.

@param aImapSettings Settings for the Imap service
@param aSelectedFolderData Imap Folder Info
@param aLogId logging info
@return The constructed CImapAuthPlainMechanismHelper object
*/
CImapAuthPlainMechanismHelper* CImapAuthPlainMechanismHelper::NewL(const CImapSettings& aSettings,CImapFolderInfo* aSelectedFolderData, TInt aLogId)  
	{
	CImapAuthPlainMechanismHelper* self= new (ELeave) CImapAuthPlainMechanismHelper(aSettings,aSelectedFolderData,aLogId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();	                                
	return self;
	}


/**
Class constructor

@param aImapSettings Settings for IMAP service
@param aSelectedFolderData Imap Folder Info
@param aLogId logging info
*/
CImapAuthPlainMechanismHelper::CImapAuthPlainMechanismHelper(const CImapSettings& aSettings,CImapFolderInfo* aSelectedFolderData, TInt aLogId) : CImapAuthMechanismHelper(aSettings,aSelectedFolderData, aLogId){}

/**
Non-trival constructor. Part of two phased construction.
*/
void CImapAuthPlainMechanismHelper::ConstructL(){}


/** 
Formats and sends the IMAP AUTHENTICATE PLAIN command.
@param aTagId Command sequence id which will be send along with the IMAP command.
@param aStream A wrapper for the outbound stream of a connected socket, using which 
the command will be send to the server
*/
void CImapAuthPlainMechanismHelper::SendMessageL(TInt aTagId, MOutputStream& aStream)
	{
	iTagId = aTagId;
	iStream = &aStream;
	
	iNextClientMessage.Zero();
	
	// send Authenticate PLAIN command
	TInt bufLength = KCommandAuthenticatePlain().Length();
	bufLength += TagLength(aTagId);
	iNextClientMessage.Format(KCommandAuthenticatePlain, aTagId);
	
	// Build up the un-encoded authorisation string in parts
 	HBufC8* authString = HBufC8::NewMaxLC(KMaxLengthOfPlainMessage); 
 	TPtr8 authStringPtr = authString->Des();

	TBuf8<1> nul;
	nul.SetLength(1);
	nul[0] = 0;
	
	//Add Null to begining of auth string
	authStringPtr = nul; 
	
	//Get LoginName from central repository and append to auth string
	TPtrC8 loginName = iSettings.LoginName();
	TInt length= Min(loginName.Length(),KMaxLengthOfPlainMessageComponent);
	authStringPtr.Append(loginName.Left(length));

	authStringPtr.Append(nul); // Separator between login and password
	
	//Get Password from central repository and append to auth string
	length= Min(iSettings.Password().Length(), KMaxLengthOfPlainMessageComponent);
	authStringPtr.Append(iSettings.Password().Left(length));
 
	// Auth string is now built, 
	HBufC8* encodedBuf = HBufC8::NewMaxLC(KMaxLengthOfPlainMessageBase64); 
	TPtr8 encodedBufPtr = encodedBuf->Des();
 
	//Encode string 
	iEncoder.Encode(authStringPtr,encodedBufPtr);	
	
	// Now build the message
	iNextClientMessage.Append(encodedBufPtr);
 	iNextClientMessage.Append(KImapTxtCrlf);	
 	
 	//send the command to the server
 	aStream.SendDataReq(iNextClientMessage);
 	
 	CleanupStack::PopAndDestroy(2,authString);
	}


/** 
Respond  upon reciept of a continuation response from server.
*/
void CImapAuthPlainMechanismHelper::ParseContinuationResponseL(){}

/**
Static factory constructor. Part of two phased construction.

@param aImapSettings Settings for the Imap service
@param aSelectedFolderData Imap Folder Info
@param aLogId logging info
@return The constructed CImapAuthLoginMechanismHelper object
*/
CImapAuthLoginMechanismHelper* CImapAuthLoginMechanismHelper::NewL(const CImapSettings& aSettings,CImapFolderInfo* aSelectedFolderData, TInt aLogId)  
	{
	CImapAuthLoginMechanismHelper* self= new (ELeave) CImapAuthLoginMechanismHelper(aSettings,aSelectedFolderData,aLogId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();	                                
	return self;
	}
	
/**
Class constructor

@param aImapSettings Settings for IMAP service
@param aSelectedFolderData Imap Folder Info
@param aLogId logging info
*/
CImapAuthLoginMechanismHelper::CImapAuthLoginMechanismHelper(const CImapSettings& aSettings,CImapFolderInfo* aSelectedFolderData, TInt aLogId) : CImapAuthMechanismHelper(aSettings,aSelectedFolderData, aLogId){}

/**
Class destructor
*/
CImapAuthLoginMechanismHelper::~CImapAuthLoginMechanismHelper()
	{
	delete iBase64LoginName;
	delete iBase64Password;
	}
	
/**
Non-trival constructor. Part of two phased construction.
Creates Base64 Username and Password
*/
void CImapAuthLoginMechanismHelper::ConstructL()
	{
	//Get LoginName from central repository
	TInt len=(((iSettings.LoginName().Length()/3)+1)*4)+2; // length of LoginName in base 64 + 2 for CRLF
	iBase64LoginName=HBufC8::NewL(len);
	TPtr8 authString(iBase64LoginName->Des());
	
	//Encode LoginName
	iEncoder.Encode(iSettings.LoginName(),authString);
	//Append CRLF to authstring
	authString.Append(KImapTxtCrlf);
	
	//Get Password from central repository
	len=(((iSettings.Password().Length()/3)+1)*4)+2; // length of Password in base 64 + 2 for CRLF
	iBase64Password=HBufC8::NewL(len);
	authString.Set(iBase64Password->Des());
	
	//Encode Password Name
	iEncoder.Encode(iSettings.Password(),authString);
	//Append CRLF to authstring
	authString.Append(KImapTxtCrlf);
	}
	
	
/** 
Formats and sends the IMAP AUTHENTICATE LOGIN command.
@param aTagId Command sequence id which will be send along with the IMAP command.
@param aStream A wrapper for the outbound stream of a connected socket, using which 
the command will be send to the server
*/
void CImapAuthLoginMechanismHelper::SendMessageL(TInt aTagId, MOutputStream& aStream)
	{
	
	iTagId = aTagId;
	iStream = &aStream;
	
	TInt bufLength = KCommandAuthenticateLogin().Length();
	
	//Add tagId to buffer
	bufLength += TagLength(aTagId);
	
	// Create  Authenticate LOGIN command message
	iOutputBuffer = HBufC8::NewL(bufLength);
	iOutputBuffer->Des().Format(KCommandAuthenticateLogin, aTagId);
	
	//send the command to the server
	aStream.SendDataReq(*iOutputBuffer);
	}

/** 
Sends the Encoded Username  message using Login, upon reciept of a continuation response from server.
The next line Encoded Password  message  will be sent upon reciept of a continuation response from server, indicating that the first line has been succesfully sent.
*/

void CImapAuthLoginMechanismHelper::ParseContinuationResponseL()
	{
	
	iNextClientMessage.Zero();
	
	//Get next part of the response from server
	TPtrC8 part = GetNextPart();

	HBufC8* decodedMessage = NULL;
	
	decodedMessage = HBufC8::NewLC(KImMailMaxBufferSize);
	if (decodedMessage)
		{
		TPtr8 decodedMessagePtr = decodedMessage->Des();
		
		//Decode Server response 
		iEncoder.Decode(part, decodedMessagePtr);
		}    
        
	switch (iState)
		{
		case ESendingLoginName:
            if (decodedMessage)
                {
                // Match against 'username*'
                if (decodedMessage->Des().MatchF(KImapAuthBase64StringUsername) == KErrNotFound)
					{
                    User::Leave(KErrNotSupported);
					}
				//Initialise encoder	
				iEncoder.Initialise();
				
				//Copy Base64 LoginName to buffer
				iNextClientMessage=*iBase64LoginName;
				
				//Change state to ESendingPassword 
                iState = ESendingPassword;
                }
            else
                {
				User::Leave(KErrNotSupported);
				}
			break;
		case ESendingPassword:
			if (decodedMessage)
                {
                // Match against 'password*'
                if (decodedMessage->Des().MatchF(KImapAuthBase64StringPassword) == KErrNotFound)
					{
                    User::Leave(KErrNotSupported);
					}
					
				//Copy Base64 Password to buffer
				iNextClientMessage=*iBase64Password;
                }
			else
                {
				User::Leave(KErrNotSupported);
				}
			break;
		default:
			User::Leave(KErrNotSupported);
			break;
		}
	
	if (decodedMessage)
		{
		CleanupStack::PopAndDestroy(decodedMessage);
		}
		
	//send the command to the server
	iStream->SendDataReq(iNextClientMessage);
	}

/**
Static factory constructor. Part of two phased construction.

@param aImapSettings Settings for the Imap service
@param aSelectedFolderData Imap Folder Info
@param aLogId logging info
@return The constructed CImapAuthCramMd5MechanismHelper object
*/
CImapAuthCramMd5MechanismHelper* CImapAuthCramMd5MechanismHelper::NewL(const CImapSettings& aSettings,CImapFolderInfo* aSelectedFolderData, TInt aLogId)  
	{
	CImapAuthCramMd5MechanismHelper* self= new (ELeave) CImapAuthCramMd5MechanismHelper(aSettings,aSelectedFolderData,aLogId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();	                                
	return self;
	}

/**
Class constructor

@param aImapSettings Settings for IMAP service
@param aSelectedFolderData Imap Folder Info
@param aLogId logging info
*/
CImapAuthCramMd5MechanismHelper::CImapAuthCramMd5MechanismHelper(const CImapSettings& aSettings,CImapFolderInfo* aSelectedFolderData, TInt aLogId) :CImapAuthMechanismHelper(aSettings,aSelectedFolderData, aLogId){}

/**
Non-trival constructor. Part of two phased construction.
Constructs md5 
*/
void CImapAuthCramMd5MechanismHelper::ConstructL()
	{
	//Create Md5
	iMd5Hash = CMD5::NewL();
	}

CImapAuthCramMd5MechanismHelper::~CImapAuthCramMd5MechanismHelper()
	{
	//Delete Md5 object
	delete iMd5Hash;
	}

/** 
Formats and sends the IMAP AUTHENTICATE CRAM-MD5 command.
@param aTagId Command sequence id which will be send along with the IMAP command.
@param aStream A wrapper for the outbound stream of a connected socket, using which 
the command will be send to the server
*/
void CImapAuthCramMd5MechanismHelper::SendMessageL(TInt aTagId, MOutputStream& aStream)
	{
	
	iTagId = aTagId;
	iStream = &aStream;
	
	TInt bufLength = KCommandAuthenticateCrammd5().Length();
	
	//Add tagId to buffer
	bufLength += TagLength(aTagId);
	
	// Create  Authenticate CRAM-MD5 command message
	iOutputBuffer = HBufC8::NewL(bufLength);
	iOutputBuffer->Des().Format(KCommandAuthenticateCrammd5, aTagId);
	
	//send the command to the server		
	aStream.SendDataReq(*iOutputBuffer);
	}


/**Creates Shared secret Key between client and server 
@param aPassword
*/
TPtr8 CImapAuthCramMd5MechanismHelper::FormSharedSecret(TPtr8 aPassword)
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
		
		//Reset Md5
		iMd5Hash->Reset();
		
		//Hash Md5
		secret = (iMd5Hash->Hash(aPassword));
		}

	return secret;
	}


/** 
Sends the Encoded Username and password message using CRAM-MD5, upon reciept of a continuation response from server.
*/
void CImapAuthCramMd5MechanismHelper::ParseContinuationResponseL()
	{
	
	//Sets the length of the data to zero. 
	iNextClientMessage.Zero();
	
	//Get next part of the response from server
	TPtrC8 part = GetNextPart();
	__LOG_FORMAT((iLogId, "CImapCommand::ParseBlockL(): [%S] %d octets", &part, part.Length()));

	//Create Cram-md5 Base64 message
	HBufC8* authbuffer = HBufC8::NewMaxLC(KImMailMaxBufferSize);
	TPtr8 authbufferptr = authbuffer->Des();
	HBufC8* authbuffer2 = HBufC8::NewMaxLC(KImMailMaxBufferSize);
	TPtr8 authbufferptr2 = authbuffer2->Des();
	HBufC8* authbuffer3 = HBufC8::NewMaxLC(KImMailMaxBufferSize);
	TPtr8 authbufferptr3 = authbuffer3->Des();
	
	//copy server response to buffer
	authbufferptr = part;
	
	//Decode Server response,timestamp info now in authbufferptr2
	iEncoder.Decode(authbufferptr,authbufferptr2);
	
	//Sets the length of the data to zero. 
	authbufferptr.Zero();
	authbufferptr3.Zero();
	
	//Get Password from Central repository
	authbufferptr = iSettings.Password();
	
	//Create Shared secrete key 
	authbufferptr3 = FormSharedSecret(authbufferptr);
	
	//Sets the length of the data to zero.
	authbufferptr.Zero();
	
	// authbufferptr and authbufferptr3 contain the shared secret null padded to 64 bytes.
	authbufferptr = authbufferptr3;  
	TInt i=0;
	for (i=0; i<KMd5BlockLength; i++) 
		{
		authbufferptr[i] ^= 0x36; //ipad
		authbufferptr3[i] ^= 0x5c; //opad
		}
	
	//Append timestamp info to authebufferptr
	authbufferptr.Append(authbufferptr2);

	//Reset Md5
	iMd5Hash->Reset();                                                                                                                                                                                                                                                                                                                ;
	
	//Hash Md5                                                                                                                                                                                                                                                                                                                ;
	authbufferptr2 = iMd5Hash->Hash(authbufferptr);

	authbufferptr3.Append(authbufferptr2);
	authbufferptr.Zero();
	
	//Reset Md5
	iMd5Hash->Reset();
	
	//Hash  Md5
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
	
	//Get LoginName from Central repository
	authbufferptr3.Append(iSettings.LoginName());
	
	//Add Space to authbufferptr3
	authbufferptr3.Append(_L8(" "));
	
	//Append hexHash authbufferptr3
	authbufferptr3.Append(hexHash);
	
	//Encode Cram-md5 message
	iEncoder.Encode(authbufferptr3, iNextClientMessage);
	
	//Append CRLF to iNextClientMessage
	iNextClientMessage.Append(KImapTxtCrlf);
	
	CleanupStack::PopAndDestroy(3);	// authbufferptr3 ,authbufferptr2, authbufferptr
	
	//send the command to the server
	iStream->SendDataReq(iNextClientMessage);
	}
	
