// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Internet SMTP Transport Driver - forward declarations for utility functions
// "Assert" errors codes returned by IMSM.DLL
// 
//

#if !defined (__IMSMSUTIL_H)
#define __IMSMSUTIL_H

enum TSmtsAssertError
	{
	EImsmSmtpFileObjectAlreadyExists = 0,	// Tried to create another CImSmtpFile object when one existed already
	EImsmZeroRecipientsInMessage = 1,		// Message header contains zero To, Cc or Bcc addresses
	EImsmNoFromAddress = 2,					// No From() address in header
	EImsmNoFileStore = 3,					// iFileStore is not open
	EImsmBodyTextNotHandledHere = 4,		// Bug in code: should never be called if state is ESendData
	EImsmBadSmtpBuffer = 5,					// IMSK buffer is null, or full or has overflowed
	EImsmBadSessionState = 6,				// iState variable out of range in iSmtpSession object
	EImsmBadFileState = 7,					// iState variable out of range in iSmtpFile object
	EImsmNoCrLfTerminator = 8,				// "\r\n" character pair not found at end of SmtpBuffer
	EImsmBadBodyEncodingState = 9,			// illegal value for CImSendConvert()'s encoding parameter
    EImsmEmptyEmailAddress = 10,				// zero-length descriptor has been passed as the email address
    EImsmEmptyPostOffice = 11,				// zero-length descriptor has been passed as the  SMTP post office name
	EImsmEmptyRespath = 12,					// zero-length descriptor has been passed as the resource path
	EImsmNoServiceInfo = 13,					// service info was missing
	EImsmServerError = 14,					// error occured locking/releasing entries

	ESmtsBusy = 15,							// second request has been issued to Smts before first one has completed
	EEntryNotSet = 16,
	EImsmEntryIsNotaServer = 17,

	EImsmUnexpectedSmtpPositiveError = 18,
	EImsmUnableToSetServerEntryBack = 19,
	EImsmUnableToSetServerEntryToMessage = 20,

	EImsmBadSmtpAuthProfile1 = 21,
	EImsmBadSmtpAuthProfile2 = 22,
	EImsmSmtpAuthLoginBadState = 23,
	EImsmSmtpAuthHelperAlreadyExists = 24,

	EImsmSocketAlreadyConnected = 25,
	EImsmSocketNotConnected1 = 26,
	EImsmSocketNotConnected2 = 27,
	
	EImsmSessionManagerInvalidState = 28,

	EImsmUnexpectedState1 = 29,
	EImsmUnexpectedState2 = 30,
	EImsmUnexpectedState3 = 31,
	EImsmUnexpectedState4 = 32,
	EImsmUnexpectedState5 = 33,
	EImsmUnexpectedState6 = 34,
	EImsmSessionNotDefined = 35,
	EImsmUnexpectedMobilityAction = 36
	};


// SMTP session and file error codes...
enum TSmtpSessionError
	{
	ESmtpServiceNotAvailable=421,		// SMTP error code 421
	ESmtpMailboxUnavailable=450,		// SMTP error code 450
	ESmtpActionAborted=451,				// SMTP error code 451
	ESmtpActionNotTaken=452,			// SMTP error code 452
	ESmtpTempAuthenticationFailure=454,	// SMTP error code 454
	ESmtpCommandUnrecognised=500,		// SMTP error code 500
	ESmtpSyntaxError=501,				// SMTP error code 501
	ESmtpCommmandNotImplemented=502,	// SMTP error code 502
	ESmtpBadSequence=503,				// SMTP error code 503
	ESmtpParamNotImplemented=504,		// SMTP error code 504
	ESmtpAuthenticationRequired=530, // SMTP error code 530
	ESmtpAuthenticationFailed=535,		// SMTP error code 535
	ESmtpMailboxNoAccess=550,			// SMTP error code 550
	ESmtpUserNotLocal=551,				// SMTP error code 551
	ESmtpExceededStorage=552,			// SMTP error code 552
	ESmtpMailboxName=553,				// SMTP error code 553
	ESmtpTransactionFailed=554,			// SMTP error code 554
	ESmtpNoReturnCode=599,				// Fake SMTP error code - probably not talking to SMTP server at all!
	};


GLREF_C void RequestComplete(TRequestStatus& aStatus,TInt aCompletion);
GLREF_C TBool LastSmtpCommandAccepted(TInt SmtpCode,TInt aRequiredFirstDigit);

GLREF_C TInt SmtpResponseCode(const TDesC8& aTextLine,TBool& aMultiLineResponse,TBool& aLastMultiLineResponse);
GLREF_C TInt IdentifySmtpError(TInt aSmtpErrorCode);


GLREF_C void gPanic(TSmtsAssertError);

// string constants used whilst parsing response from SMTP server
_LIT8(KEsmtpMatchString,"*ESMTP *");
_LIT8(K8BitMimeMatchString,"*8-BITMIME *");
_LIT8(KSizeMatchString,"*SIZE *");
_LIT8(KStartTlsMatchString,"*STARTTLS*");
_LIT8(KAuthMatchString,"*AUTH *");
_LIT8(KAuthPlainMatchString,"* PLAIN*");
_LIT8(KAuthLoginMatchString,"* LOGIN*");
_LIT8(KAuthCramMD5MatchString,"* CRAM-MD5*");

// Special case for non-compliant Domino servers 
_LIT8(KAuthDominoMatchString,"*AUTH=*"); 
_LIT8(KAuthDominoPlainMatchString,"*=PLAIN*"); 
_LIT8(KAuthDominoLoginMatchString,"*=LOGIN*"); 
_LIT8(KAuthDominoCramMD5MatchString,"*=CRAM-MD5*"); 

//string constants containing SMTP command words
_LIT8(KSmtpHeloCommand,"HELO [%S]\r\n");
_LIT8(KSmtpEhloCommand,"EHLO [%S]\r\n");
_LIT8(KSmtpStartTlsCommand,"STARTTLS\r\n");
_LIT8(KSmtpTlsResponse,"220");
_LIT8(KSmtpResetCommand,"RSET\r\n");

_LIT8(KSmtpMailFromCommand,"MAIL FROM:<");
_LIT8(KSmtpRcptToCommand,"RCPT TO:<");
_LIT8(KSmtpDataCommand,"DATA\r\n");
_LIT8(KSmtpEndOfDataCommand,".\r\n");
_LIT8(KSmtpNoopCommand,"NOOP\r\n");
_LIT8(KSmtpQuitCommand,"QUIT\r\n");
_LIT8(KSmtpCloseAngleBracket,">\r\n");
_LIT8(KSmtpCrLf,"\r\n");

//string constants containing SMTP AUTH command words
_LIT8(KSmtpAuthCommand, "AUTH ");

#ifdef _UNICODE
 _LIT(KSmtpEmailAddressAndAliasFmtStr,"\"%S\" <%S>");
#else
 _LIT8(KSmtpEmailAddressAndAliasFmtStr,"\"%S\" <%S>");
#endif

// general purpose strings
_LIT8(KStuffDot,".");
_LIT8(KSmtpMatchCrLf,"*\r\n");				// used to detect CRLF terminator in outgoing buffer (only used in debug build)

#endif
