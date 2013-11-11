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
// Implementation file for class CImStmpSession -- representing
// a connection to a remote SMTP server.
// 
//

#include <miutpars.h>           // TImMessageField
#include <imcvsend.h>			// CImSendConvert
#include <logwrap.h>
#include <logwraplimits.h>

#include "SMTS.H"
#include "IMSM.H"				// CImSmtpSession
#include "IMSMSEND.H"			// CImSmtpFile
#include "SMTSUTIL.H"			// forward declarations for utility fns
#include <imutcon.h>
#include "mobilitytestmtmapi.h"

const TInt KSmtpAuthCodeSuccessful=235;
const TInt KSmtpAuthCodeFailed=535;
const TInt KSmtpUnableToAuthAtPresent=435;
const TInt KSmtpAuthCodeReadyResponse=334;
const TInt KSmtpBadParameter=501;

/**
Factory constructor

@param aServerEntry Server entry
@param aSettings SMTP settings
@param aServ Socket server
@param aConnect Network connection
@return Constructed class
*/
CImSmtpSession* CImSmtpSession::NewL(CMsvServerEntry& aServerEntry, CSmtpSettings& aSettings, RSocketServ& aServ, CImConnect& aConnect, TMsvId aServiceId)
	{
	CImSmtpSession* self = new (ELeave) CImSmtpSession(aServerEntry, aSettings, aServiceId);
	CleanupStack::PushL(self);
	self->ConstructL(aServ, aConnect);
	CleanupStack::Pop(self);
	return self;
	}

/**
Connect the session to the server

@param aClientStatus Client status to be completed
*/
void CImSmtpSession::ConnectL(TRequestStatus& aClientStatus)
	{
	__ASSERT_DEBUG(!iSocketIsConnected, gPanic(EImsmSocketAlreadyConnected));

	if (!iSocketIsConnected)
		{
		if(iSettings.SecureSockets())
			iThisSession = ESecureSession;		 // if the user has chosen to use TLS then we have to use ESMTP+TLS
		else if(iSettings.SMTPAuth())
			iThisSession = EEnhancedSmtpSession; // if the user has chosen to use AUTH, when we have to use ESMTP
		else
			iThisSession = ESmtpSession;		 // use SMTP session for now

		iCompleted = KErrNone;
		iState = EConnectingToSmtp;		// Initialise to 1st state of state machine
		DoStateL();						// Here we go...
		Queue(aClientStatus);		
		}
	else
		{
		// Nothing to do so just complete the caller
		RequestComplete(aClientStatus, KErrNone);
		}
	}

/**
Send all the emails to the server

@param aSendFiles Files to send
@param aClientStatus Client status to be completed
*/
void CImSmtpSession::SendFilesL(CMsgImOutboxSend& aSendFiles, TRequestStatus& aClientStatus)
	{
	__ASSERT_DEBUG(iSocketIsConnected, gPanic(EImsmSocketNotConnected1));
	
	TInt error = aSendFiles.NextFile();
	if (iSocketIsConnected &&  error == KErrNone)
		{
		iSendFiles = &aSendFiles;
		iState = ESendingImail;
		DoStateL();
		Queue(aClientStatus);
		}
	else if(error == KErrNotFound)
		{
		// Nothing to do so just complete the caller
		RequestComplete(aClientStatus, KErrNotFound);
		}
	else
		{
		// Nothing to do so just complete the caller
		RequestComplete(aClientStatus, KErrNone);
		}
	}

/**
Quit from the server

@param aClientStatus Client status to be completed
*/
void CImSmtpSession::QuitL(TRequestStatus& aClientStatus)
	{
	__ASSERT_DEBUG(iSocketIsConnected, gPanic(EImsmSocketNotConnected2));
	
	if (iSocketIsConnected)
		{
		iState = EClosingSmtp;
		DoStateL();
		Queue(aClientStatus);
		}
	else
		{
		// Nothing to do so just complete the caller
		RequestComplete(aClientStatus, KErrNone);
		}
	}

/**
Constructor

@param aServerEntry Server entry
@param aSettings SMTP settings
*/
CImSmtpSession::CImSmtpSession(CMsvServerEntry& aServerEntry, CSmtpSettings& aSettings, TMsvId aServiceId)
			  :	CMsgActive(KImSmtpSessionPriority),
			    iServerEntry(aServerEntry),
				iSettings(aSettings),
				iServiceId(aServiceId),
				iCurrentAuthProfile(CSmtpAuthMechanismHelper::EUndefined) // always start with undefined
	{
	__DECLARE_NAME(_S("CImSmtpSession"));
	}

/**
Second phase constructor

@param aServ Socket server
@param aConnect Network connection
*/
void CImSmtpSession::ConstructL(RSocketServ& aServ, CImConnect& aConnect)
	{
	iSocket = CImTextServerSession::NewL(aServ, aConnect);
	//if log fails, ignore the error and continue.
	TRAP_IGNORE(iLogMessage = CImLogMessage::NewL(iServerEntry.FileSession()));
	CActiveScheduler::Add(this);					// Add SmtpSession to scheduler's queue
	}

/**
Destructor
*/
CImSmtpSession::~CImSmtpSession()
	{
	Cancel();
	if (iSocketIsConnected)
		{
		iSocket->Disconnect();
		}
	delete iSmtpFile;			// should be dead already
	delete iSocket;
	delete iSmtpAuthHelper;
	delete iLogMessage;
	}

//
//  CreateFormattedAddressL()
//
//  Creates a new HDesC* containing the concateneatd alias and address strings
//
HBufC* CImSmtpSession::CreateFormattedAddressLC(const TDesC& aString, const TDesC& aAlias)
    {
    TImMessageField msgField;

    //  Make a buffer..
    TInt strLen     = aString.Length();
    TInt aliasLen   = aAlias.Length();

    HBufC* outString = HBufC::NewLC(strLen+aliasLen+5);
    HBufC* buffer = HBufC::NewL(strLen);

    *buffer = msgField.GetValidInternetEmailAddressFromString(aString);

	if (aliasLen)
        {
		outString->Des().Format(KSmtpEmailAddressAndAliasFmtStr,&aAlias,buffer);
        }
    else
        {
        *outString = *buffer;	        // just copy email address; as there's no alias to be added
        }

	delete buffer;
    return outString;
    }


TImImailFileProgress CImSmtpSession::FileProgress()
	{
	// Return info about how much of message has been sent so far
	TImImailFileProgress progress;

	if (iSmtpFile && (iState==ESendingImail))
		{
		// get progress from CImSmtpFile object as it is doing the work at the moment
        iSmtpFile->GetProgress(progress);

		// If there is more than 1 message being sent(ie when sending to BCC's), then
		// update the Progress to take account of the Total Num of Messages sent
		if (iNumMsgsToSend > 1)
			{
			// Calc the Total number of Bytes to send
			TInt fileByteSize = iSmtpFile->BytesToSend();
			TInt bytesSentBefore = (iNumMsgsSent-1)*fileByteSize;  // iNumMsgsSent-1, as this msg is not yet complete
			TInt totalBytesSent = bytesSentBefore + progress.iBytesSent;

			// Update the Progress
			progress.iBytesToSend = iTotalBytesToSend;
			progress.iBytesSent = totalBytesSent;
			}
		}
	else
		{
		progress.iSessionState = EClosingSmtp;
		progress.iBytesSent = 0;
		progress.iBytesToSend = 0;
		}
	return progress;
	}


void CImSmtpSession::SendFileL()
	{
	// Start the 'file mailer' object
	__ASSERT_DEBUG(!iSmtpFile,gPanic(EImsmSmtpFileObjectAlreadyExists));
	// log the info of the message to be sent
	if (iLogMessage)
		iLogMessage->Reset();
	//  Update the header info for this message..
	CImHeader* header = CImHeader::NewLC();
	CMsvStore* store = iServerEntry.EditStoreL();
	CleanupStack::PushL(store);
	header->RestoreL(*store);

	TBool isBccRcpt = EFalse;
	CDesCArray* rcptArray = NULL;
	TBool sendBccNow = iToRcptHeaderUpdated;

	if(!iToRcptHeaderUpdated)
		{
		//  Set the new info...
		//set from address
		HBufC* formatedAddress = CreateFormattedAddressLC(iSettings.EmailAddress(), iSettings.EmailAlias());//Format the address fields
		header->SetFromL(*formatedAddress);
		CleanupStack::PopAndDestroy(formatedAddress);
		formatedAddress = NULL;

		// set ReceiptAddress if one exists in the settings
		if( header->ReceiptAddress().Length() == 0 && iSettings.ReceiptAddress().Length() > 0 )
			{
			formatedAddress=CreateFormattedAddressLC(iSettings.ReceiptAddress(),iSettings.EmailAlias());//Format the address fields
			header->SetReceiptAddressL(*formatedAddress);
			CleanupStack::PopAndDestroy(formatedAddress);
			formatedAddress = NULL;
			}

		if(header->ToRecipients().Count()==0 && header->CcRecipients().Count()==0)
			{
			sendBccNow=ETrue;
			}

		// ReplyToAddress
		if ((header->ReplyTo().Length() == 0 || header->ReplyTo() == iSettings.ReplyToAddress()) &&
		    (iSettings.ReplyToAddress().Length() > 0))
			{
			formatedAddress= CreateFormattedAddressLC(iSettings.ReplyToAddress(),iSettings.EmailAlias());//Format the address fields
			header->SetReplyToL(*formatedAddress);
			CleanupStack::PopAndDestroy(formatedAddress);
			formatedAddress = NULL;
			}
		
		//make a copy of 'Bcc' recipients then remove from Msg header
		rcptArray= &(header->BccRecipients());
		if (iSettings.SendCopyToSelf()==ESendCopyAsBccRecipient)
			{
			if ( header->ReceiptAddress().Length() )
				rcptArray->AppendL(header->ReceiptAddress());
			else if ( header->ReplyTo().Length() )
				rcptArray->AppendL(header->ReplyTo());
			}

		if(rcptArray->Count() > 0)
			{
			iBccRcptFound=ETrue; // 'Bcc' recipients exists
			iBccRcptIndex=0;	  //reset the counter
			iFinishedWithBccRcpts=EFalse;

			iSendFiles->ResetBccRcptArrayL();

			TInt numberRcpts = rcptArray->Count();
			if(numberRcpts)
			{
			iSendFiles->BccRcptArray().AppendL((*rcptArray)[0]);
			CDesCArray& tempArr = iSendFiles->BccRcptArray();
			for(TInt counter=1; counter<numberRcpts; counter++)
				{
				TInt aPos = 0;
				tempArr.Find((*rcptArray)[counter],aPos,ECmpFolded16);			
				if(aPos > 0)	
				iSendFiles->BccRcptArray().AppendL((*rcptArray)[counter]);
				}
				}
			rcptArray->Reset();
			}

		iToRcptHeaderUpdated=ETrue;
		isBccRcpt=EFalse;
		}

	// Reset the number of Sent Messages, if all previous messages have been sent
	if (iNumMsgsSent >= iNumMsgsToSend)
		{
		iNumMsgsSent = 0;
		}

	// Calc the num of messages we're sending. If there is a To or CC,
	// recipient then there will be a message sent
	if(header->ToRecipients().Count()>0 || header->CcRecipients().Count()>0)
		iNumMsgsToSend = 1;
	else
		iNumMsgsToSend = 0;

	// If there are BCC recipients, then we will be sending a message to each
	if (iBccRcptFound)
		iNumMsgsToSend += iSendFiles->BccRcptArray().Count();

	// Make sure there are recipients to send to.
	__ASSERT_ALWAYS(iNumMsgsToSend,gPanic(EImsmZeroRecipientsInMessage));

	// sending Msg to 'Bcc' recipients so update header
	if (sendBccNow && iBccRcptFound)
		{
		rcptArray= &(header->BccRecipients());
		rcptArray->Reset();
		header->BccRecipients().AppendL(iSendFiles->BccRcptArray()[iBccRcptIndex]);

		++iBccRcptIndex;//for the next recipient
		if(iBccRcptIndex < iSendFiles->BccRcptArray().Count())
			iFinishedWithBccRcpts=EFalse;
		else
			iFinishedWithBccRcpts=ETrue;

		isBccRcpt=ETrue;
		}

	header->StoreL(*store);
	store->CommitL();
	if (iLogMessage)
		{
		iLogMessage->LogEvent().SetEventType(KLogMailEventTypeUid);
		if ((header->ToRecipients().Count()
 			+ header->CcRecipients().Count()
 			+ header->BccRecipients().Count()) > 1)
 		// If there are multiple recipients then set the recipient to 'multiple'
 			{
 			TBuf<KLogMaxSharedStringLength> multipleRecipientsString;
 			iLogMessage->GetString(multipleRecipientsString, R_LOG_REMOTE_MULTIPLE);
 			iLogMessage->LogEvent().SetRemoteParty(multipleRecipientsString);
 			}
 		else
 		// If there is only one recipient then set the recipient name
 			{
			if (header->ToRecipients().Count())
				iLogMessage->LogEvent().SetRemoteParty(header->ToRecipients()[0]);

			else if (header->CcRecipients().Count())
				iLogMessage->LogEvent().SetRemoteParty(header->CcRecipients()[0]);

			else if (header->BccRecipients().Count())
				iLogMessage->LogEvent().SetRemoteParty(header->BccRecipients()[0]);
			}

		iLogMessage->LogEvent().SetSubject(header->Subject());

		TBuf<KLogMaxSharedStringLength> outString;
		iLogMessage->GetString(outString, R_LOG_DIR_OUT);
		iLogMessage->LogEvent().SetDirection(outString);

		iLogMessage->LogEvent().SetLink(iServerEntry.Entry().Id());
		}

	CleanupStack::PopAndDestroy(2, header);

	iTimeNow.UniversalTime();	// Set time/date of sending this message file
	if (iLogMessage)
		iLogMessage->LogEvent().SetTime(iTimeNow);
	TRAPD(error,iSmtpFile = CImSmtpFile::NewL(	*iSocket,
                                                iServerEntry,
												iTimeNow,
												iSmtpBuffer,
												iSettings,
												isBccRcpt)); // Create the file mailing object

	if(error == KErrNone)
		{
		// If this is the first message to send, then calc the total bytes to send
		if (iNumMsgsSent == 0)
			{
			iTotalBytesToSend =  iSmtpFile->TotalMsgSizeL() * iNumMsgsToSend;
			}

		// Tell the file how many bytes it will be sending.  This is just a
		// rough estimate for progress
		iSmtpFile->SetBytesToSend(iTotalBytesToSend/iNumMsgsToSend);

		// Increment the number of messages sent
		++iNumMsgsSent;

		iSmtpFile->StartL(iStatus);		// start sending the message
		MOBILITY_TEST_MTM_STATE(iServiceId, KMobilityTestMtmStateSmtpSendFile);
		}
	else
		{
		iCompleted=error;
		RequestComplete(iStatus,0-error);	// Failed to create File object, but don't want to stop session prematurely, so negate error code
		}
	}

void CImSmtpSession::DoCancel()
	{
	if (iSmtpFile)
		{
		iSmtpFile->Cancel();	//which calls iSocket->Cancel()
		delete iSmtpFile;		// this is expected to be deleted
		iSmtpFile=NULL;
		}
	else if (iSocket)
		{
		iSocket->Cancel();
		}

	if (iLogMessage)
		{
		iLogMessage->Cancel();
		}
	CMsgActive::DoCancel();		// this MUST be the last statement in this function
	}


void CImSmtpSession::DoRunL()
	{
	// Received a KErrNone signal completing last pending asynchronous operation.
	// So now decide what to do next...

	delete iSmtpFile;			// delete mailing object; (new one created later if need to mail another message)
	iSmtpFile=NULL;

	if (iStatus.Int() != KErrNone && iState!=ELogDataEvent)
		{
		iCompleted=iStatus.Int();	// retain any non-zero completion code for future reference when DoComplete() is called
		}

	iState = NextStateL();
		
	if (!iOperationComplete)
		{
		if (iSmtpMultiLineResponse)
			{
			// Don't change state, just read next line of multi-line
			// response from remote SMTP server...
			iSmtpLastMultiLineResponse=EFalse;
			iSocket->QueueReceiveNextTextLine(iStatus);
			SetActive();
			}
		else
			{
			DoStateL();		// move to state that was chosen by this function
			}
		}
	else
		{
		iOperationComplete = EFalse;
		}
	}



void CImSmtpSession::DoComplete(TInt& aStatus)
	{
	if (aStatus==KErrDisconnected && iState==EClosingSmtp)
		{
		// ignore sudden disconnect if in closing state
		aStatus = KErrNone;
		}

	if (iCompleted && aStatus==KErrNone)		// last non-zero error in aStatus takes preference over older error stored in iCompleted
		{
		aStatus = iCompleted;
		}

	if (aStatus>KErrNone)
		{
		switch (aStatus)
			{
			case ESmtpMailboxNoAccess:
			case ESmtpMailboxName:
			case ESmtpTransactionFailed:
				aStatus=KSmtpLoginRefused;
				break;
			case -KSmtpBadMailFromAddress:
			case -KSmtpBadRcptToAddress:
			case -KSmtpNoMailFromErr:
			case -KSmtpLoginRefused:
				// Negate the status because it was negated earlier so that it wouldn't be fatal
				aStatus = -aStatus;
				break;
			default:
				aStatus=KSmtpUnknownErr;
			}
		}
	}


TInt CImSmtpSession::NextStateL()
// this returns the next state according to the current state and the response text received from the server
// if the text has not been accepted by the server, then we close our session with it, i.e. return EClosingSmtp.
	{
	switch(iState)
		{
		case EConnectingToSmtp:
			return EWaitingForReply;

		case EWaitingForReply:
			return NextStateFromWaitingForReply();

		case EAuthorisingSmtp:
			return NextStateFromAuthorisingSmtpL();

		case ESendingStarttls:
			return NextStateFromSendStarttls();

		case ESettingSecurity:
			return NextStateFromSettingSecurityL();

		case ESendingImail:
			if (iBccRcptFound && !iFinishedWithBccRcpts)
				{
				return ESendingImail;
				}
			else
				{
				// 'Bcc' recipients are dealt with at this point
				iSendFiles->SetLastMessageStatusL(iTimeNow,(TSmtpSessionError)iStatus.Int());
				// ... which unlocks the server entry (except the first time)

				// now move on to the next Email message
				iToRcptHeaderUpdated=EFalse; // for the new msg
				iBccRcptFound=EFalse;
				iFinishedWithBccRcpts=ETrue;
				return ELogDataEvent;
				}

		case ELogDataEvent:
			return NextStateFromNextFile();

		case EClosingSmtp:
			{
			iOperationComplete = ETrue;
			return iState;
			}

		case EAuthInProgress:
			// for SMTP auth, call specific function, to stop cluttering up this one
			return NextStateFromAuthInProgressL();

		case EResetSmtp:
			return NextStateFromResetSmtp();

		default:
			return iState;
		}
	}

TInt CImSmtpSession::NextStateFromWaitingForReply()
	{
	if (CommandAccepted())
		{
		if (!iEsmtpSpokenHere)
			{
			iEsmtpSpokenHere = (iSmtpBuffer.Match(KEsmtpMatchString) != KErrNotFound);
			if (iEsmtpSpokenHere && iThisSession == ESmtpSession)
				iThisSession = EEnhancedSmtpSession;
			}

		if (iSmtpMultiLineResponse)
			{
			// If this is not last reply of a mullti-line response... request next
			// line and don't change state
			return EWaitingForReply;
			}
		return EAuthorisingSmtp;
		}
	else
		{
		// SMTP server didn't return "220" to "HELO" command, so quit session
		// without sending any Emails.
		return  EClosingSmtp;
		}
	}

TInt CImSmtpSession::NextStateFromAuthorisingSmtpL()
	{
	if (CommandAccepted())
		{
		UpdateAuthorisingInfo();

		if (iSmtpMultiLineResponse)
			{
			// If this is not last reply of a mullti-line response... request next
			// line and don't change state.
			return EAuthorisingSmtp;
			}

		if (iThisSession==ESecureSession)
			{
			if(iStartTlsAcceptedHere)
				{
				return ESendingStarttls;
				}
			else
				{
				// The SMTP server doesn't support TLS.
				// Save the error and close down the session.
				iCompleted = KErrSmtpTLSNegotiateFailed;
				return EClosingSmtp;
				}
			}
		else if (iThisSession==EEnhancedSmtpSession)
			{
			if (iSettings.SMTPAuth() && SelectNextSMTPAuthProfileL())
				{
				// if possible, perform SMTP AUTH
				return EAuthInProgress;
				}
			
			iOperationComplete = ETrue;
			return iState;
			}
		else
			{
			__ASSERT_DEBUG(iThisSession==ESmtpSession, gPanic(EImsmBadSessionState));
			iOperationComplete = ETrue;
			return iState;
			}
		}
	else
		{
		// EHLO or HELO rejected by server
		if( iSettings.SecureSockets() )
			{
			// Cannot continue if we're trying for secure?
			iCompleted = KErrSmtpTLSNegotiateFailed;
			return EClosingSmtp;
			}
		else if((iLastSmtpCode == KSmtpBadParameter) && (iRetryAuthWithHostname==EFalse))
			{
			// The server did not like the supplied IP parameter, reset and retry with a named
			// string as paramter using the same EHLO or HELO command if it hasn't already been
			// attempted
			iRetryAuthWithHostname = ETrue;
			iCompleted = KErrNone;
			return EResetSmtp;
			}
		else if(iThisSession==EEnhancedSmtpSession)
			{
			// EHLO has failed, reset and retry with HELO
			iCompleted = KErrNone;
			iRetryAuthWithHostname = EFalse;
			iThisSession = ESmtpSession;
			return EResetSmtp;
			}
		else
			{
			// Every attempt has failed, iCompleted has been set with an appropriate
			// error code in the CommandAccepted() call, so simply close the smtp session
			return EClosingSmtp;
			}
		}
	}

TInt CImSmtpSession::NextStateFromSendStarttls()
	{
	if (CommandAccepted() && iSmtpMultiLineResponse)
		{
		// If this is not last reply of a mullti-line response... request next
		// line and don't change state
		return ESendingStarttls;
		}

	if(iCompleted)
		return EClosingSmtp;
	else
		return ESettingSecurity;
	}

TInt CImSmtpSession::NextStateFromSettingSecurityL()
	{
	if ( CommandAccepted() )
		{
		UpdateAuthorisingInfo();

		if (iSmtpMultiLineResponse)
			{
			// If this is not last reply of a mullti-line response... request next
			// line and don't change state
			return ESettingSecurity;
			}
		}

	if(iCompleted)
		return EClosingSmtp;

	if (iSettings.SMTPAuth() && SelectNextSMTPAuthProfileL())
		return EAuthInProgress;
	else
		{
		iOperationComplete = ETrue;
		return iState;
		}
	}

TInt CImSmtpSession::NextStateFromAuthInProgressL()
	{
	TInt smtpCode=ESmtpNoReturnCode;
	TBool commandAccepted=GetCurrentTextLine();
	if (commandAccepted)
		{
		// parse out SMTP code from text response
		smtpCode=SmtpResponseCode(iSmtpBuffer,iSmtpMultiLineResponse,iSmtpLastMultiLineResponse);
		if (iSmtpMultiLineResponse)
			{
			// Check for response code 535
			if (smtpCode == KSmtpAuthCodeFailed)
				{
				User::Leave(KSmtpLoginRefused);
				}
			iSmtpAuthHelper->SetLastServerMessageL(iSmtpBuffer,iSmtpMultiLineResponse);
			return iState;
			}
		if(smtpCode==KSmtpAuthCodeSuccessful || smtpCode==KSmtpAuthCodeFailed || smtpCode==KSmtpUnableToAuthAtPresent)
			{
			//if KSmtpAuthCodeSuccessful then SmtpAuth concluded - move onto next state.
			//if KSmtpAuthCodeFailed then SmtpAuth failed, but try to send anyway
			iOperationComplete = ETrue;
			return iState;
			}
		else if(smtpCode==ESmtpParamNotImplemented)
			{
			// this mechanism not recognised by the server.
			delete iSmtpAuthHelper;
			iSmtpAuthHelper=NULL;
			if (SelectNextSMTPAuthProfileL())
				{
				return EAuthInProgress;
				}
			else
				{
				// all AUTH attempts fail, try to send anyway
				iOperationComplete = ETrue;
				return iState;
				}
			}
		else if(smtpCode==KSmtpAuthCodeReadyResponse)
			{
			iSmtpAuthHelper->SetLastServerMessageL(iSmtpBuffer,iSmtpMultiLineResponse);
			return EAuthInProgress;
			}
			// if here, we will execute default error case, unless someone adds
			// a clause not resulting in a "return" statement
		}
	// default error case
	iCompleted=SmtpSessionError(smtpCode);
	return EClosingSmtp;

	}

TInt CImSmtpSession::NextStateFromResetSmtp()
	{
	if( CommandAccepted() )
		{
		if (iSmtpMultiLineResponse)
			{
			// If this is not last reply of a mullti-line response... request next
			// line and don't change state
			return EResetSmtp;
			}
		// The reset was accepted - send the HELO command.
		return EAuthorisingSmtp;
		}

	// The reset was not accepted - didn't get a 2xx response. Might as
	// well end the SMTP session.
	return EClosingSmtp;
	}

void CImSmtpSession::DoStateL()
// performs the operation required by iState
    {
    switch (iState)
        {
		case EConnectingToSmtp:
			iEsmtpSpokenHere = EFalse;
			i8BitMimeAcceptedHere = EFalse;
			iSizeAcceptedHere = EFalse;
			iStartTlsAcceptedHere = EFalse;
			if(iSettings.SSLWrapper())
				{
				// Open secure socket on port 465 for SMTP session
				iSocket->SSLQueueConnectL(iStatus, iSettings.ServerAddress(), iSettings.Port(), iSettings.IapPrefs(), iSettings.TlsSslDomain());
				}
			else
				{
				// Open socket on port 25 for SMTP session
				iSocket->QueueConnectL(iStatus, iSettings.ServerAddress(), iSettings.Port(), iSettings.IapPrefs(), iSettings.TlsSslDomain());
				}

			MOBILITY_TEST_MTM_STATE(iServiceId, KMobilityTestMtmStateSmtpConnectingToSmtp);
			break;

		case EWaitingForReply:
			iSocketIsConnected=ETrue;
			iSocket->QueueReceiveNextTextLine(iStatus);
			MOBILITY_TEST_MTM_STATE(iServiceId, KMobilityTestMtmStateSmtpWaitingForReply);
			break;

		case EAuthorisingSmtp:
			{
			TBuf8<KImskIPAddressLen> address;
			GetIpAddress(address);
			
			if(iRetryAuthWithHostname)
				{
				// Try sending a text string hostname instead of IP address
				_LIT(KDefaultNameString, "localhost");
				address.Copy(KDefaultNameString);
				}

			if (iThisSession!=ESmtpSession)
				iSmtpBuffer.Format(KSmtpEhloCommand,&address);
			else
				iSmtpBuffer.Format(KSmtpHeloCommand,&address);

			iSocket->SendQueueReceive(iStatus, iSmtpBuffer);
			MOBILITY_TEST_MTM_STATE(iServiceId, KMobilityTestMtmStateSmtpAuthorisingSmtp);
			break;
			}
		case EAuthInProgress:
			{
			iSmtpAuthHelper->GetNextClientMessageL(iSmtpBuffer);
			iSocket->SendQueueReceive(iStatus, iSmtpBuffer);
			MOBILITY_TEST_MTM_STATE(iServiceId, KMobilityTestMtmStateSmtpAuthInProgress);
			break;
			}
		case ESendingStarttls:
			{
			// User has chosen to use Tls - send the STARTTLS cmd.
			iSocket->SetSSLTLSResponseL(KSmtpTlsResponse);
			iSocket->SendQueueReceive(iStatus, KSmtpStartTlsCommand());
			MOBILITY_TEST_MTM_STATE(iServiceId, KMobilityTestMtmStateSmtpSendingStartTls);
			break;
			}
		case ESettingSecurity:
			{
			// Reset the Auth profiles and flags.
			i8BitMimeAcceptedHere	= EFalse;
			iSizeAcceptedHere		= EFalse;
			iStartTlsAcceptedHere	= EFalse;
			iSupportedAuthProfiles	= ENone;
			
			//  Set Security, send a NOOP
			//	and then queues asynch read request for a response
			//	from the remote SMTP server.
			TBuf8<KImskIPAddressLen> address;
			GetIpAddress(address);

			iSmtpBuffer.Format(KSmtpEhloCommand, &address);
			iSocket->SendQueueReceive(iStatus,iSmtpBuffer);
			MOBILITY_TEST_MTM_STATE(iServiceId, KMobilityTestMtmStateSmtpSettingSecurity);
			break;
			}
		case ESendingImail:	// send one Imail message
 			SendFileL();
			break;

		case ELogDataEvent:
			{
			if (iLogMessage)
				{
				const TInt logError = iServerEntry.Entry().iError;
				if (logError == KErrNone)
					{
					TBuf<KLogMaxStatusLength> status;
					if (iLogMessage->GetString(status, R_LOG_DEL_SENT) == KErrNone)
						iLogMessage->LogEvent().SetStatus(status);
					}
				iLogMessage->Start(logError, iStatus);
				}
			else
				{
				TRequestStatus* status = &iStatus;
				User::RequestComplete(status, KErrNone);
				}
			break;
			}
		case EResetSmtp:
			iSocket->SendQueueReceive(iStatus, KSmtpResetCommand());
			MOBILITY_TEST_MTM_STATE(iServiceId, KMobilityTestMtmStateSmtpResetSmtp);
			break;

		case EClosingSmtp:	//finished running - end SMTP session
			iSocket->SendQueueReceive(iStatus, KSmtpQuitCommand());
			MOBILITY_TEST_MTM_STATE(iServiceId, KMobilityTestMtmStateSmtpClosingSmtp);
			break;

		default:     // Unknown state
			gPanic(EImsmBadSessionState);
			break;
        }
 	SetActive();
    }


TInt CImSmtpSession::SmtpSessionError(const TInt aSmtpErrorCode)
	{
	// This function is called when no socket/memory error occurred whilst completing
	// last state, but the remote SMTP server did not return the expected 3 digit
	// completion code.
	// Select an appropriate error code determined from the current state of CImSmtpSession.
	switch (iState)
		{
		case EAuthorisingSmtp:
			if (aSmtpErrorCode==ESmtpSyntaxError)
				return (KSmtpLoginRefused);
			break;

		case EWaitingForReply:
			// assume that ANY failure in this state means I have been refused connection to SMTP server
			return (KSmtpLoginRefused);

		case ESendingStarttls:
		case ESettingSecurity:
		case EConnectingToSmtp:
		case ESendingImail:
		case EResetSmtp:
		case EClosingSmtp:
			break;

		case EAuthInProgress:
			if (aSmtpErrorCode==KSmtpAuthCodeFailed || aSmtpErrorCode==KSmtpUnableToAuthAtPresent)
				return (KSmtpLoginRefused);
			break;

		default:     // Unknown state
			gPanic(EImsmBadSessionState);
			break;
        }

	return IdentifySmtpError(aSmtpErrorCode);
	}

TBool CImSmtpSession::GetCurrentTextLine()
	{
	TInt result=iSocket->GetCurrentTextLine(iSmtpBuffer);
	__ASSERT_DEBUG(result==ECRLFTerminated,gPanic(EImsmBadSmtpBuffer));

	if(result==ECRLFTerminated)
		return ETrue;
	else
		return EFalse;
	}

void CImSmtpSession::GetIpAddress(TDes8& aAddress)
/** Gets the local IP address from the socket without any trailing scope identifiers
@param aAddress Buffer in which the IP address of the socket will be returned */
	{
    aAddress.Copy(iSocket->LocalName());

    // Remove the %nn scope identifier if present Eg. x.x.x.x.x.x%1 becomes x.x.x.x.x.x
	//  This only occurs with IPv6 and was a requirement of multi-homing.
	TInt pos = aAddress.Locate(TChar('%'));
	if (pos>0)
		{
		aAddress.SetLength(pos);
		}
	}

/*
From knowledge of the last profile used, and the profiles available, select a next profile
to try, create the helper class
otherwise return false
*/
TBool CImSmtpSession::SelectNextSMTPAuthProfileL()
	{
	CSmtpAuthMechanismHelper::TSmtpAuthProfileFlag nextProfile=NextSMTPAuthProfile(iCurrentAuthProfile);
	while (!(nextProfile&iSupportedAuthProfiles) && nextProfile!=CSmtpAuthMechanismHelper::ENoProfile)	// while "nextProfile" is not supported
																				// and there are profiles to try
		nextProfile=NextSMTPAuthProfile(nextProfile); // try the next one
	iCurrentAuthProfile=nextProfile;
	if (iCurrentAuthProfile==CSmtpAuthMechanismHelper::ENoProfile)  // run out of profiles to try
		return EFalse;
	else
		{
		__ASSERT_DEBUG(!iSmtpAuthHelper, gPanic(EImsmSmtpAuthHelperAlreadyExists));
		iSmtpAuthHelper=CreateSMTPAuthHelperL(iCurrentAuthProfile,iSettings);
		return ETrue;
		}
	}

// Get the value of the connecting IAP from CImTextServerSession
//  Returns the value of the currently connecting IAP or a system-wide error code.
TInt CImSmtpSession::GetConnectionIAP()
	{
	TUint32 iap;
	TInt err = iSocket->GetIAPValue(iap);
	if (err == KErrNone)
		{
		return iap;
		}
	else
		{
		return err;
		}
	}

CSmtpAuthMechanismHelper::TSmtpAuthProfileFlag  CImSmtpSession::NextSMTPAuthProfile(CSmtpAuthMechanismHelper::TSmtpAuthProfileFlag aPreviousProfile)
	{
	// we try the following profiles in this order, CRAM-MD5, LOGIN, PLAIN
	switch(aPreviousProfile)
		{
		case CSmtpAuthMechanismHelper::EUndefined:
			return CSmtpAuthMechanismHelper::ECramMD5;

		case CSmtpAuthMechanismHelper::ECramMD5:
			return CSmtpAuthMechanismHelper::ELogin;

		case CSmtpAuthMechanismHelper::ELogin:
			return CSmtpAuthMechanismHelper::EPlain;

		case CSmtpAuthMechanismHelper::EPlain:
			return CSmtpAuthMechanismHelper::ENoProfile;

		default:
			gPanic(EImsmBadSmtpAuthProfile1);
		}
	return CSmtpAuthMechanismHelper::ENoProfile;
	}

TInt CImSmtpSession::NextStateFromNextFile()
	{
	if (iSendFiles->NextFile() == KErrNone)
		{
		return ESendingImail;
		}
	else
		{
		iOperationComplete = ETrue;
		return iState;
		}
	}

TBool CImSmtpSession::CommandAccepted()
	{
	iLastSmtpCode = ESmtpNoReturnCode;
	TBool commandAccepted = GetCurrentTextLine();
	if (commandAccepted)
		{
		iLastSmtpCode = SmtpResponseCode(iSmtpBuffer, iSmtpMultiLineResponse, iSmtpLastMultiLineResponse);   // parse out SMTP code from text response

		commandAccepted = LastSmtpCommandAccepted(iLastSmtpCode, 2);      // was response accepted by remote server?
		}

	if (!commandAccepted)
		{
		// overrride completion code with suitable error number
		iCompleted = SmtpSessionError(iLastSmtpCode);
		}
	return commandAccepted;
	}

void CImSmtpSession::UpdateAuthorisingInfo()
	{
	if (!i8BitMimeAcceptedHere)
		{
		// i.e. don't do test again if flag is already set
		i8BitMimeAcceptedHere = (iSmtpBuffer.Match(K8BitMimeMatchString) != KErrNotFound);
		}
	if (!iSizeAcceptedHere)
		{
		iSizeAcceptedHere = (iSmtpBuffer.Match(KSizeMatchString) != KErrNotFound);
		// ESMTP: V1.1 parse string to extract max message size from string here...
		}
	if (!iStartTlsAcceptedHere)
		{
		iStartTlsAcceptedHere = (iSmtpBuffer.Match(KStartTlsMatchString) != KErrNotFound);
		}
	//Support added for Domino server.
	if (iSmtpBuffer.Match(KAuthMatchString) != KErrNotFound || 
    				iSmtpBuffer.Match(KAuthDominoMatchString) != KErrNotFound) 
		{
		// check for each supported profile
		if (iSmtpBuffer.Match(KAuthPlainMatchString) != KErrNotFound)
			iSupportedAuthProfiles|=CSmtpAuthMechanismHelper::EPlain;
		if (iSmtpBuffer.Match(KAuthLoginMatchString) != KErrNotFound)
			iSupportedAuthProfiles|=CSmtpAuthMechanismHelper::ELogin;
		if (iSmtpBuffer.Match(KAuthCramMD5MatchString) != KErrNotFound)
			iSupportedAuthProfiles|=CSmtpAuthMechanismHelper::ECramMD5;
   		// Special case for Domino servers tagging "=" after "AUTH" 
   		if (iSmtpBuffer.Match(KAuthDominoPlainMatchString) != KErrNotFound) 
       		iSupportedAuthProfiles|=CSmtpAuthMechanismHelper::EPlain; 
   		if (iSmtpBuffer.Match(KAuthDominoLoginMatchString) != KErrNotFound) 
       		iSupportedAuthProfiles|=CSmtpAuthMechanismHelper::ELogin; 
   		if (iSmtpBuffer.Match(KAuthDominoCramMD5MatchString) != KErrNotFound) 
   			iSupportedAuthProfiles|=CSmtpAuthMechanismHelper::ECramMD5;

 		}
	}
