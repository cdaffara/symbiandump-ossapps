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
// Internet SMTP Transport Driver - utility functions
// 
//

#include<e32std.h>
//Oyster includes
#include<msvstd.h>
//IMSK includes
#include<imsk.h>
//SMTS includes
#include"SMTS.H"
#include"IMSM.H"
#include"SMTSUTIL.H"

GLDEF_C void gPanic(TSmtsAssertError aReason)
	{
	User::Panic(_L("SMTS-DLL"),aReason);
	}


GLDEF_C void RequestComplete(TRequestStatus& aStatus,TInt aCompletion)
	{
	// Untility function which sends completion signal to client active object
	TRequestStatus* statusPtr=&aStatus;
	User::RequestComplete(statusPtr,aCompletion);
	}


GLDEF_C TBool LastSmtpCommandAccepted(TInt aSmtpCode,TInt aRequiredFirstDigit)
	{
	// Utility function: is 1st digit of 3-digit code returned by SMTP server
	// the digit we expected? Returns true or False.
	// Used as a simple test to determine whether last command sent to remote SMTP server
	// was accepted or not.
	//
	// E.g. aSmtpCode=250 ; test for aRequiredFirstDigit=2
	// or   aSmtpCode=354 ; test for aRequiredFirstDigit=3
	
	return ((aSmtpCode/100)==aRequiredFirstDigit);
	}


GLDEF_C TInt SmtpResponseCode(const TDesC8& aTextLine,TBool& aMultiLineResponse,TBool& aLastMultiLineResponse)
	{
	// This function examines the response from remote SMTP server to determine
	// whether the most recent SMTP command sent to that server by this transport driver
	// was accepted, or not. Also determines whether it is one of a multi-line response
	// from the server (as sent by ESMTP servers)
	//
	// SMTP return codes are always 3 digit numbers, where:
	//		"2xx" is a positive response (ie command was accepted)
	//		"354" is a positive response to the "DATA" command
	//		"4xx" is a negative reply - temporary error in remote SMTP server
	//		"5xx" is a negative reply  - error in commmand sent to remote SMPT server by this transport driver

	TLex8   parseResponse(aTextLine);
	TInt    SmtpCode;
	if (!parseResponse.Val(SmtpCode))
		{
		// remember state of iSmtpMultiLine flag; value is used by parseMultiLineResponse
		aLastMultiLineResponse = aMultiLineResponse;
		aMultiLineResponse = (parseResponse.Get().IsSpace()==EFalse);
		aLastMultiLineResponse &= !aMultiLineResponse;
		return (SmtpCode);
		}
	// If TLex failed, response from SMTP server was bogus - ie it contained no 3-digit return code.
	// So generate a fake one to ensure that state machine is notified of this error
	return (ESmtpNoReturnCode);	
	}


GLDEF_C TInt IdentifySmtpError(TInt aSmtpErrorCode)
	{
	// Error codes which may be returned in response to any SMTP command
	switch (aSmtpErrorCode)
		{
	case 421:
//		error=ESmtpServiceNotAvailable;		// service not available
	case 450:
//		error=ESmtpMailboxUnavailable;
	case 451: 
//		error=ESmtpActionAborted;			// processing error on remote SMTP server
	case 452:
//		error=ESmtpActionNotTaken;			// insufficient storage on remote SMTP server
	case 500:
//		error=ESmtpCommandUnrecognised;		// Bug in IMSM.DLL? ; sending illegal command to remote SMTP server
	case 501:
//		error=ESmtpSyntaxError;				// Bug in IMSM.DLL? ; incorrect syntax in command sent to remote SMTP server
	case 502:
//		error=ESmtpCommmandNotImplemented;	// Command sent to SMTP server not recognised by that server.
	case 503:
//		error=ESmtpBadSequence;				// Bug in IMSM's state machine? ; SMTP commands sent to server in wrong order
	case 504:
//		error=ESmtpParamNotImplemented;		// Bug in IMSM.DLL? ; SMTP server didn't expect parameter with this command
	case 550:
//		error=ESmtpMailboxNoAccess;			// Mailbox not found, or no access to mailbox on remote SMTP server
	case 551:
//		error=ESmtpUserNotLocal;			// Remote SMTP server expects a forward path
	case 552:
//		error=ESmtpExceededStorage;			// Insuffucient storage space on remote SMTP server for message
	case 553:
//		error=ESmtpMailboxName;				// Bug in IMSM.DLL? ; Mailbox name not allowed
	case 554:
//		error=ESmtpTransactionFailed;		// Transaction failed!

		// I recognised SMTP error code
		return aSmtpErrorCode;
		}

	// i.e. didn't recognise error code - so must be a socket/Epoc32 error
	return 0;
	}
