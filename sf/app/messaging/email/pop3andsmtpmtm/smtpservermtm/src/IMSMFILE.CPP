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
// File Mailer for Internet SMTP Transport Driver
// 
//

#include <mentact.h>		// CMsgActive
#include <msvstd.h>			// TMsvEntry, CMsvEntrySelection
#include <msventry.h>		// CMsvServerEntry
#include <msvstore.h>		// CMsvStore
#include <miutpars.h>		// TImMessageField
#include <miuthdr.h>		// CImHeader
#include <imcvsend.h>		// CImSendMessage

#include "SMTS.H"
#include "IMSM.H"			// KSmtpFilePriority
#include "IMSMSEND.H"		// CImSmtpFile
#include "SMTSUTIL.H"		// forward declarations for utility fns

const TInt KMsvSuspendCheckGranuality=15;

//As per RFC-2821(Section: 4.5.3.2 Timeouts) Maximum time out for Data termination is 10 minutes. 
const TInt KSmtpDataTerminationTimeout=600;

CImSmtpFile* CImSmtpFile::NewL(CImTextServerSession& aSocket,  
							   CMsvServerEntry& aServerEntry,
							   TTime& aTime,
							   TBuf8<KImMailMaxBufferSize>& aSmtpBuffer,
							   CSmtpSettings& aSettings,TBool aIsBccRcpt)
	{
	CImSmtpFile* self= new (ELeave) CImSmtpFile(aSocket,aSmtpBuffer,aServerEntry);
	CleanupStack::PushL(self);
	self->ConstructL(aTime,aSettings,aIsBccRcpt);
	CleanupStack::Pop();	                                
	return self;
	}


CImSmtpFile::CImSmtpFile(CImTextServerSession& aSocket,TBuf8<KImMailMaxBufferSize>& aSmtpBuffer,CMsvServerEntry &aServerEntry) 
			: CMsgActive(KImSmtpFilePriority),
			  iSocket(aSocket),
			  iServerEntry(aServerEntry),
			  iSmtpBuffer(aSmtpBuffer)															 

	{
	__DECLARE_NAME(_S("CImSmtpFile"));
	}

void CImSmtpFile::ConstructL(TTime& aTimeNow,CSmtpSettings& aSettings,TBool aIsBccRcpt) 

	{
	iSendCopyToSelf=aSettings.SendCopyToSelf();

	iRecipientType = aIsBccRcpt? ERcptBcc : ERcptTo;

	// Create and Initialise the CImSendMessage object..
	iSendMessage = CImSendMessage::NewL(iServerEntry);

	// Create objects to store header and body form the message file
	// Read contents of message file into containers
	iHeader = CImHeader::NewLC();		//  PushL(iHeader)
	CleanupStack::Pop();			    //  iHeader

	iEntryId=iServerEntry.Entry().Id();

	CMsvStore* store = iServerEntry.EditStoreL();
	CleanupStack::PushL(store);

	// The message store entry always uses UTC, so set it now because the call to
	// GetHeaderFromStoreL updates the time on that entry.
	aTimeNow.UniversalTime();
	GetHeaderFromStoreL(*store,iServerEntry, aTimeNow);	// Read ImHeader object from message file
	CleanupStack::PopAndDestroy();//store

	//  Sets iBodyType (TImBodyConvAlgorithm).
	SelectBodyEncodingTypeL(aSettings.BodyEncoding(), iHeader->BodyEncoding());  

	// Create the message to send.
	// Note that when sending the message, we use the HomeTime as opposed to UniversalTime.
	// This is because RFC822 mandates that the time in emails should be local time.
	TTime homeTime;
	homeTime.HomeTime();
	iSendMessage->InitialiseL(iEntryId,iBodyType, homeTime, aSettings.ServerAddress(),
									  aSettings.DefaultMsgCharSet().iUid, aSettings.SendCopyToSelf());

	CActiveScheduler::Add(this);	// Add SmtpFile to scheduler's queue
	}


void CImSmtpFile::SetBytesToSend(TInt aNumBytes)
	{
	// Set the number of Bytes to send
	iBytesToSend = aNumBytes;
	}

TInt CImSmtpFile::BytesToSend() const
	{
	return iBytesToSend;
	}

TInt CImSmtpFile::TotalMsgSizeL()
// Return the Size of this Message
	{
	TInt msgSize = 0;
	if (iSendMessage)
		{
		// factor of 4/3 is here to attempt to take account of the increase in size that will happen
		// when the message is encoded
		msgSize = TInt((4.0/3)*(iSendMessage->HeaderSize() + iSendMessage->BodySizeL()));
		}

	return msgSize;
	}


void CImSmtpFile::GetHeaderFromStoreL(CMsvStore& aStore, CMsvServerEntry& aServerEntry, TTime& aTimeNow)
	{
	// Retrieve the CImHeader object 
	iHeader->RestoreL(aStore);

	TMsvEntry entry = aServerEntry.Entry();
	entry.iDate=aTimeNow;
	entry.SetSendingState(KMsvSendStateSending);
	entry.SetFailed(EFalse);
	aServerEntry.ChangeEntry(entry);
	} 


void CImSmtpFile::StartL(TRequestStatus& aSessionStatus)
	{
	// Start sending the message file, by initialising the state machine
	iCompleted=KErrNone;
	iRecipientArray=NULL;
	iState=EResettingSmtp;

	ChangeStateL();			// Start the state machine
	Queue(aSessionStatus);
	}


void CImSmtpFile::GetProgress(TImImailFileProgress& aFileProgress)
    {
	// Return info about how much of message has been sent so far
	aFileProgress.iBytesToSend = iBytesToSend;
	aFileProgress.iBytesSent = iBytesSent;
	aFileProgress.iSessionState = (TSmtpSessionState)iState;
	}


void CImSmtpFile::DoCancel()
    {
	// Cancel any pending socket call
	iSocket.Cancel();
	CMsgActive::DoCancel();		// MUST be the last statement in DoCancel();
    }


TInt CImSmtpFile::State()
	{
	// return current state of state machine
	return iState;
	}


TBool CImSmtpFile::NextRecipientL()
	{
	// Return True if there is another recipient identified in imheader;
	//	...if True, then "iRecipient" is set to point to next email address descriptor.
	//
	// This function scans through empty "To" and "Cc" fields
	// if necessary to locate another recipient email address.

	//Precondition: 'Bcc' recipients are dealt with in CImSmtpSession 
	if(iRecipientType==ERcptBcc)
		return EFalse;

	if (!iRecipientArray)
		{
		// Nextrecipient has been called for the 1st time, so get "To:" list from CImHeader
		iRecipientArray = &iHeader->ToRecipients();
		if (iSendCopyToSelf==ESendCopyAsToRecipient)
			{
			if ( iHeader->ReceiptAddress().Length() )
				iRecipientArray->AppendL(iHeader->ReceiptAddress());
			else if ( iHeader->ReplyTo().Length() )
				iRecipientArray->AppendL(iHeader->ReplyTo());
			}
		iRecipientIndex = 0;
		iRecipientType = ERcptTo;
		}

	while ((iRecipientIndex >= iRecipientArray->Count()) && (iRecipientType <= ERcptCc))
		{
		// Find the next Email address (if it exists)
		iRecipientArray = &iHeader->CcRecipients();
		if (iSendCopyToSelf==ESendCopyAsCcRecipient)
			{
			if ( iHeader->ReceiptAddress().Length() )
				iRecipientArray->AppendL(iHeader->ReceiptAddress());
			else if ( iHeader->ReplyTo().Length() )
				iRecipientArray->AppendL(iHeader->ReplyTo());
			}
		iRecipientIndex=0;
		iRecipientType++;
		}

	if (iRecipientType <= ERcptCc)
		{
		iRecipient.Set((*iRecipientArray)[iRecipientIndex++]);
		return ETrue;	// Found an email address; iRecipient points to it now.
		}
	return EFalse;	// iRecipientType>ERcptCc, so tell caller there are no more Email addresses left
	}


void CImSmtpFile::DoRunL()
    {
	iCompleted=iStatus.Int();	// remember completion code - which is >= KErrNone

	if (iState!=ESendData && iCompleted==KErrNone)
		{
		TInt SmtpCode = ESmtpNoReturnCode;
		TBool CommandAccepted = GetCurrentTextLine();
		if (CommandAccepted)
			{
			SmtpCode = SmtpResponseCode(iSmtpBuffer,iSmtpMultiLineResponse,iSmtpLastMultiLineResponse);			// parse out SMTP code from text response
			CommandAccepted = LastSmtpCommandAccepted(SmtpCode,SmtpFilePositiveResponse(iState));	// was response accepted by remote server?
			}

		// "RSET", "DATA" and "." SMTP commands should never return a multi line response		
		__ASSERT_DEBUG(iState!=ESendData,gPanic(EImsmBodyTextNotHandledHere));

		if ((!CommandAccepted) && (iCompleted==KErrNone))
			{
			// Record an SMTP error only if no other error has been signalled.
			iCompleted = SmtpFileError(SmtpCode);
			}
		}

	if ((iState==EEndData) || (iCompleted!=KErrNone))
		{
		//  I have completed last state, or an error has occurred... 
		//  so don't requeue state machine
		return;
		}

	// Normal situation... choose the next state...
	iState=SelectNextStateL();
	ChangeStateL();	
    }


TInt CImSmtpFile::SmtpFilePositiveResponse(TInt aState)
	{
	// Utility function returns 1st digit of 3 digit response code which
	// I expect the remote SMTP server to return if last command succeeded.
	TInt ExpectedFirstDigit;

	if (aState==EBeginData)
		{
		ExpectedFirstDigit = 3;		// SMTP server should repond to "DATA" command with code "324"
		}
	else
		{
		ExpectedFirstDigit = 2;		// SMTP server should normally respond with code "250"
		}
	return ExpectedFirstDigit;
	}


TInt CImSmtpFile::SelectNextStateL()
	{
	// Choose the next state for the state machine
	TInt NextState;

	switch (iState)
		{
	case ERcptToSmtp:	// is there another recipient address?
		NextState = (NextRecipientL()) ? ERcptToSmtp : EBeginData;
		break;
	case ESendData:		// is there any more data to send?
		NextState = iMoreRfc822Data ? ESendData : EEndData;
		break;
	default:
		NextState = iState+1;	
		break;
		}

	return NextState;
	}


void CImSmtpFile::ChangeStateL()
    {
    //
    // State machine of the whole SMTP File mailer.
	//	
	TImMessageField emailField;

    switch (iState)
        {
    case EResettingSmtp:
		SendAndQueueRead(KSmtpResetCommand);
        break;

    case EMailFromSmtp:
		__ASSERT_DEBUG(iSmtpBuffer.Length(),gPanic(EImsmNoFromAddress));
		if (emailField.ValidInternetEmailAddress(iHeader->From()))
			{
    		iSmtpBuffer = KSmtpMailFromCommand;
	    	iSmtpBuffer.Append(emailField.GetValidInternetEmailAddressFromString(iHeader->From()));
		    iSmtpBuffer.Append(KSmtpCloseAngleBracket);
			SendAndQueueRead(iSmtpBuffer);
			}
		else
			{
			// no Email address for return path, so refuse to send this message
			RequestComplete(iStatus,0-KSmtpNoMailFromErr);
			}
        break;

    case ERcptToSmtp: 
		if(iRecipientType==ERcptBcc) 
			{
			iRecipientArray = &iHeader->BccRecipients();
			iRecipient.Set((*iRecipientArray)[0]); // The should be only one Bcc recipient
			}
		else //'To' and 'Cc' recipients
			{
		    if(!iRecipientArray)
				NextRecipientL();//	state was EMailFromSmtp, so initialise iRecipient
			}
				

        //  BUG_FIX SW1-659.. check with new build..
        //  Prepend the recipient address... into iSmtpBuffer and insert the command seq.
        iSmtpBuffer.Copy(emailField.GetValidInternetEmailAddressFromString(iRecipient));
        iSmtpBuffer.Insert(0, KSmtpRcptToCommand);
		iSmtpBuffer.Append(KSmtpCloseAngleBracket);

        SendAndQueueRead(iSmtpBuffer);
        break;

    case EBeginData:
        SendAndQueueRead(KSmtpDataCommand);
        break;

    case ESendData:  // send RFC822 message now
        SendOneLineOfData();
        break;

    case EEndData:
		// The bytes sent count is NOT accurate, so make sure this value
		// doesn't exceed the number to send
		iBytesSent = iBytesToSend;
		// If smtp server did not respond within 10 mins after sending ".", the socket is timed out
		iSocket.SendQueueReceiveWithTimeout(iStatus, KSmtpDataTerminationTimeout, KSmtpEndOfDataCommand);
		break;

    default:    // Unknown state
        gPanic(EImsmBadFileState);
        break;
        }
	SetActive();
    }


void CImSmtpFile::SendOneLineOfData()
	{
	TInt paddedBytes=0;

	if((iSuspendCheck++ % KMsvSuspendCheckGranuality) == 0)
		{
		// save the id, reset to top entry, check suspended state, fail if suspended
		TMsvId savedId=iServerEntry.Entry().Id();
		// if we can't get to the parent entry, just keep sending
		TInt error=iServerEntry.SetEntry(iEntryId);
		__ASSERT_DEBUG(error==KErrNone,gPanic(EImsmUnableToSetServerEntryToMessage));
		if(error==KErrNone)
			{
			const TUint sendState=iServerEntry.Entry().SendingState();
			// we would be in a bad way if we can't get back to the entry.
			// but there isn't anything we can do about it
			error=iServerEntry.SetEntry(savedId);
			__ASSERT_ALWAYS(error==KErrNone,gPanic(EImsmUnableToSetServerEntryBack));

			if(sendState==KMsvSendStateSuspended)
				{
				RequestComplete(iStatus,KErrCancel);
				return;
				}			
			}
		}

	//->>Bug- SMTS was expecting a KImCvFinished (=-1) when IMCM finishes with the msg header and boby
	// instead IMCM returns a KImCvAdvance (=1) to indicate the end of msg.
	// this is fixed temporarily by using KImCvAdvance instead of KImCvFinished.
	TRAPD(error,iMoreRfc822Data=(KImCvFinished!= iSendMessage->NextLineL(iSmtpBuffer,paddedBytes)));	// "iMoreRfc822Data" flag signals if this is last line of RFC822 body
	if (!error)
		{
		__ASSERT_DEBUG(iSmtpBuffer.Length(),gPanic(EImsmBadSmtpBuffer));

		// Checking for end of data = <CRLF>.

		if (iSmtpBuffer[0]=='.')
			{
			paddedBytes++;
			iSmtpBuffer.Insert(0,KStuffDot);
			}

		__ASSERT_DEBUG(iSmtpBuffer.Match(KSmtpMatchCrLf)==iSmtpBuffer.Length()-2,gPanic(EImsmNoCrLfTerminator));
		iSocket.Send(iStatus,iSmtpBuffer);	// send line of text to TCP/IP socket

		// update progress information
		iBytesSent+=iSmtpBuffer.Length();	// NB counting terminating "\r\n" at end of each line
		
		// The bytes sent count is NOT accurate, so make sure the progress 
		// does NOT exceed the amount to Send
		if(iBytesSent > iBytesToSend)
			iBytesSent = iBytesToSend;
		}
	else
		{
		RequestComplete(iStatus,error);
		}
	}


TInt CImSmtpFile::SmtpFileError(TInt aSmtpErrorCode)
	{
	// Try to identify specific error condition from state and error code
	// if unable to do so, return SMTP code.

	switch (iState)
        {
	case EMailFromSmtp:
		if (aSmtpErrorCode==ESmtpSyntaxError ||aSmtpErrorCode==ESmtpUserNotLocal)
			return(0-KSmtpBadMailFromAddress);

		// If we receive one of the authentication error codes in response to our mail
		// from command, then this indicates that the initial login was refused, but
		// we decided to go on and try to do the send anyway.
		// Return the KSmtpLoginRefused error code to indicate this.
		// Note that the ESmtpTempAuthenticationFailure failure code should not be
		// received in response to a mail from command, but some servers seem to use it.
		if (aSmtpErrorCode == ESmtpTempAuthenticationFailure ||
		    aSmtpErrorCode == ESmtpAuthenticationRequired)
			{
			return (0 - KSmtpLoginRefused);
			}
		break;

	case ERcptToSmtp:
		if (aSmtpErrorCode==ESmtpSyntaxError || aSmtpErrorCode==ESmtpMailboxNoAccess
			||aSmtpErrorCode==ESmtpMailboxName)
			return (0-KSmtpBadRcptToAddress);
		break;

	case EResettingSmtp:
	case EBeginData:
	case ESendData:
	case EEndData:
		break;

    default:     // Unknown state
        gPanic(EImsmBadFileState);
        break;
		}

	TInt error=IdentifySmtpError(aSmtpErrorCode);
	return (error ? error : 0-KSmtpUnknownErr);	// return SMTP error code, if recognised
	}	


void CImSmtpFile::DoComplete(TInt& aStatus)
	{
	if (iCompleted)
		{
		// override completion code with my own (positive) error value
		aStatus=iCompleted;
		}
	}
	

void CImSmtpFile::SelectBodyEncodingTypeL(TMsgOutboxBodyEncoding anSettingsValue, 
										  TMsgOutboxBodyEncoding anHeaderValue)
	{
	if (anHeaderValue!=EMsgOutboxDefault)
		iBodyType= anHeaderValue==EMsgOutboxNoAlgorithm ? ESendAsSimpleEmail:ESendAsMimeEmail;
	else if (anSettingsValue!=EMsgOutboxDefault)
		iBodyType= anSettingsValue==EMsgOutboxNoAlgorithm ? ESendAsSimpleEmail:ESendAsMimeEmail;
	else
		iBodyType=ESendAsMimeEmail; // Default setting.
	}

//   Creating library \EPOC32\RELEASE\WINS\UDEB\IMUT.LIB and object \EPOC32\RELEASE\WINS\UDEB\IMUT.exp


void CImSmtpFile::SendAndQueueRead(const TDesC8& aDesc)
	{
	// Send SMTP command string to remote SMTP server -
	// and then queues asynch read request for a response from the remote SMTP server.
	iSocket.SendQueueReceive(iStatus,aDesc);
	}


TBool CImSmtpFile::GetCurrentTextLine()
	{
	TInt result=iSocket.GetCurrentTextLine(iSmtpBuffer);
	__ASSERT_DEBUG(result==ECRLFTerminated,gPanic(EImsmBadSmtpBuffer));
	return (result==ECRLFTerminated);
	}


CImSmtpFile::~CImSmtpFile()
	{
	// Destructor. Must NOT destroy iSocket as it is owned by iSmtpSession, not by iSmtpFile.
	Cancel();

	delete iHeader;
	delete iSendMessage;
	}
