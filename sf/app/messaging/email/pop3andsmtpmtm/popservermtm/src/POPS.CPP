// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include "POPS.H"
#include "POPSOP.H"
#include <pop3set.h>

#include <iapprefs.h>
#include <hash.h>
#include <pop3set.h>
#include <e32std.h>
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
#include "cpopsaslauthhelper.h"
#endif
#include "POPS.PAN"  // impp's own panic codes

#define KPeriod 30000000	// period of timer

// Specifies how long a socket is allowed to be inactive before we close it
// down. This handles the situation where the mail server closes down the
// connection, but we don't receive any indication of that. It has been seen
// when connected using GPRS, and a long telephone call is then made.
const TInt KPopSendInactivityTimeMinutes = 30;
const TInt KPopReceiveInactivityTimeMinutes = 30;

// Utility functions used by both active objects...
GLDEF_C TBool CommandAccepted(TDesC8& aResponse)
	{
	// +OK signifies a successful command -ERR otherwise
	return (0==aResponse.Locate('+'));
	}
//
// My own panic command 
//
GLDEF_C void Panic(TPopsPanic aPanic)
	{
	_LIT(KPanicText,"Pop3 session");
	User::Panic(KPanicText, aPanic);
	}

CImPop3Session::CImPop3Session()
	: CMsgActive(KImPopSessionPriority)
	{
	__DECLARE_NAME(_S("CImPop3Session"));
	}


CImPop3Session* CImPop3Session::NewL(RSocketServ& aServ, CImConnect& aConnect)
	{
	CImPop3Session* self = new (ELeave) CImPop3Session();
	CleanupStack::PushL(self);
	self->ConstructL(aServ, aConnect);
	CleanupStack::Pop();
	return self;
	}


void CImPop3Session::ConstructL(RSocketServ& aServ, CImConnect& aConnect)
	{
	iSocket=CImTextServerSession::NewL(KPopSendInactivityTimeMinutes, KPopReceiveInactivityTimeMinutes, aServ, aConnect);
	iNoMessages=0;
	iIdTab=NULL;
	iSocketConnected=EFalse;
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	iSaslAuthLogin = EFalse;
#endif	

	CActiveScheduler::Add(this);	  // Add PopSession to scheduler's queue
	}


CImPop3Session::~CImPop3Session()
	{
	Cancel();

	delete iPopApop;
	delete iConnectReply;
	delete iSocket;
	delete [] iIdTab;
	delete iCaf;
	delete iPopCapabilities;
	}

//
// Connect to Pop3 with User and Pass
//
void CImPop3Session::ConnectL(CImPop3Settings* aPopSettings, const CImIAPPreferences& aPrefs, TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(!IsActive(),Panic(EImppAlreadyActive));
	// copy of pop settings owned by mtm
	iPopSettings = aPopSettings;
	
	iCompleted=KErrNone;
	iState=EConnectingToPop;	// Initialise to 1st state of state machine
	
	Queue(aStatus);

	if(iPopSettings->SSLWrapper())
		{
		iSocket->SSLQueueConnectL(iStatus, iPopSettings->ServerAddress(), aPopSettings->Port(), aPrefs, iPopSettings->TlsSslDomain());
		}
	else
		{
		iSocket->QueueConnectL(iStatus, iPopSettings->ServerAddress(), aPopSettings->Port(), aPrefs, iPopSettings->TlsSslDomain());
		}
	SetActive();
	}

//
// Queue a read if mailbox is in idle state
//
void CImPop3Session::Waiting(TRequestStatus& aStatus)
	{
	iState=EPopConnected;
	iSocket->QueueReceiveNextTextLine(iStatus);
	SetActive();
	Queue(aStatus);
	}
//
// Put a QUIT function 
//
void CImPop3Session::Quit(TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(!IsActive(),Panic(EImppAlreadyActive));

	iState=EStopPop;

	if(iSocketConnected)
		{
		_LIT8(KPop3CmdQuit, "QUIT\r\n");
		iSocket->SendQueueReceive(iStatus,KPop3CmdQuit());
		SetActive();
		Queue(aStatus);
		}
	else
		{
		TRequestStatus* pS=&iStatus;
		User::RequestComplete(pS,KErrNone);
		}
	}

//
// Pass the TMsg array to our Pop session
//
void CImPop3Session::SetMessageArray(TInt32* anIdArray, TUint aNoMessages)
	{
	iIdTab=anIdArray;
	iNoMessages=aNoMessages;
	}
	
TInt32* CImPop3Session::MessageArray()
	{
	return iIdTab;
	}

TInt CImPop3Session::GetNumMessages()
	{
	return iNoMessages;
	}

//
// Return Pop3 message no for a given message id
//
TInt CImPop3Session::MessageNo(TMsvId aMsgId)
// AF Using TInt reduces the available range of +ve numbers 
// but shouldn't be a problem should it?
	{
	TInt msgNo=KErrNotFound;
	TInt arrayCtr=0;

	if(iIdTab==NULL || iNoMessages==0)
		{
		Panic(EImppBabelMsgArrayNotDefined);
		}
	
	while(arrayCtr<iNoMessages && msgNo==KErrNotFound)
		{
		if(TInt32(*(iIdTab+arrayCtr))==aMsgId)
			{
			msgNo=arrayCtr+1;
			}
		arrayCtr++;
		}

	return msgNo;
	}

//
// Cancel any current operation
//
void CImPop3Session::DoCancel()
	{
	if(iState==EStopPop)
		{
		iSocket->Disconnect();
		iSocketConnected=EFalse;
		}
	
	if(iState == EPopCapabilities || iState == EWaitingForReply)
	    {
	    if(iPopCapabilities)
	        {
	        iPopCapabilities->Cancel();
	        }
	    }
	
	iSocket->Cancel();  // clear a pending socket call

	CMsgActive::DoCancel();
	}

//
// Result of last active call
//
void CImPop3Session::DoRunL()
    {
	if(iState==EStopPop)
		{
		iSocket->Disconnect();
		iSocketConnected=EFalse;
		}
	
	//get the response
	if(iState!=EConnectingToPop && iState!=EStopPop)
		{
		if(iState != EWaitingForReply)
			{
			TInt lineState=iSocket->GetCurrentTextLine(iResponseBuffer);
			__ASSERT_ALWAYS(lineState==ECRLFTerminated,User::Leave(EImppBufferNotTerminated));

			if(!CommandAccepted(iResponseBuffer))
				{
				User::Leave(GetPopError());
				}
			}
		
		if(iPopSettings->Apop() && iState==EWaitingForReply)
			{
			HBufC8* connectReply = HBufC8::NewL(KImMailMaxBufferSize);
			// Delete iConnectReply if not NULL & assign newly created
			delete iConnectReply;
			iConnectReply = connectReply;
			iConnectReply->Des().Copy(iResponseBuffer); 
			}
		}
			
	if(iState!=EStopPop) 
		{
		// successful response decide what to do next
		switch(iState)
			{
			case EPopCapabilities:
				GetCapabilitiesL();
				iState = EWaitingForReply;
				break;
				
			case EWaitingForReply:
				if(iPopSettings->SecureSockets())
					{
					iState = ERequestingTLS;
					}
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
				else if(iPopSettings->POP3Auth())
					{
					iState = ESaslAuthInProgress; 
					SelectAuthExtensionProfileL();
					}
#endif					
				else if(iPopSettings->Apop())
					{
					iState=EAuthorisingApop;
					}
				else 
					{
					iState++;
					}
				ChangeStateL();
				break;

#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)					
			case ESaslAuthInProgress:
				// To set the server response, accodring the response will be creating 
				// the authentication string.
				iPopAuthHelper->SetLastServerMessageL(iResponseBuffer, ETrue);
				iState++;
				ChangeStateL();
				break;
			
			case ESaslAuthIsDone:
				iSocketConnected=ETrue;
				break;
#endif
				
			case EConnectingToPop:
			case EAuthorisingUser:
				iState++;
				ChangeStateL();
				break;
				
			case EAuthorisingPass:
			case EAuthorisingApop:
				iSocketConnected=ETrue;
				break;
				
			// TLS request accepted
			case ERequestingTLS:
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)			
				if(iPopSettings->POP3Auth())
					{
					iState = ESaslAuthInProgress; 
					SelectAuthExtensionProfileL();
					}
				else
#endif
					{
					iPopSettings->Apop() ? iState = EAuthorisingApop : iState =EAuthorisingUser;
					}
				ChangeStateL();
				break;
			}
		}
   }

void CImPop3Session::DoComplete(TInt& aCompleteStatus)
 	{
	TInt status=aCompleteStatus;
	if (status==KErrNone)	// no error
		{
		return;
		}

	if(iSocketConnected && status==KErrCancel)	// Cancel() has been called
		{
		return;
		}

	// if we've connected but it all goes wrong send a synchronous quit kludged code
	switch (iState)
		{
	case EWaitingForReply:	// stay connected
	case EStopPop:			
		break;
	case EAuthorisingUser:
	case EAuthorisingPass:
	case EAuthorisingApop:
	case EPopConnected:     // idle read has returned POP server timed out
	case EConnectingToPop:	// lose connection
	case ERequestingTLS:
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	case ESaslAuthInProgress:
	case ESaslAuthIsDone:
#endif	
		iSocket->Disconnect();
		iSocketConnected=EFalse;
		break;
		}
	}

void CImPop3Session::ChangeStateL()
	{
	//
	// State machine of the POP mail session.
	// (But only handle states which can leave)
	// Identify state on entry, change to next state and then
	// start new operation associated with that new state.
	//
   
	__ASSERT_DEBUG(!IsActive(),Panic(EImppAlreadyActive));
   
	switch (iState)
		{
	case EPopCapabilities:
		iSocket->QueueReceiveNextTextLine(iStatus);
		break;

#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)	
	case ESaslAuthInProgress:
		iPopAuthHelper->GetNextClientMessageL(iResponseBuffer);
		iSocket->SendQueueReceive(iStatus, iResponseBuffer);
		iSocket->PerformLogging(ETrue);
		break;
	
	case ESaslAuthIsDone:
		iPopAuthHelper->GetNextClientMessageL(iResponseBuffer);
		iSocket->SendQueueReceive(iStatus, iResponseBuffer);
		iSocket->PerformLogging(ETrue);
		if(iSaslAuthLogin)
			{
			iState = ESaslAuthInProgress;
			iSaslAuthLogin = EFalse;	
			}
		break;
#endif
		
	case EAuthorisingUser:
	case EAuthorisingApop:
		{
		TPtrC8 loginName=iPopSettings->LoginName();
		if (iState == EAuthorisingApop)
			{
			TRAPD(apopErr,ConstructApopL());
			if(apopErr!=KErrNone)
				{
				User::Leave(KPop3CannotCreateApopLogonString);
				}
			iSocket->PerformLogging(EFalse);
			_LIT8(KPop3CmdApop, "APOP %S %S\r\n");
			iSocket->SendQueueReceive(iStatus, KPop3CmdApop(), &loginName, iPopApop);
			iSocket->PerformLogging(ETrue);
			delete iPopApop;
			iPopApop=NULL;
			delete iConnectReply;
			iConnectReply=NULL;
			}
		else
			{
			iSocket->PerformLogging(EFalse);
			_LIT8(KPop3CmdUser, "USER %S\r\n");
			iSocket->SendQueueReceive(iStatus, KPop3CmdUser(), &loginName);
			iSocket->PerformLogging(ETrue);
			}
		break;
		}
	case EAuthorisingPass:
		{
		TPtrC8 password=iPopSettings->Password();
		iSocket->PerformLogging(EFalse);
		_LIT8(KPop3CmdPass, "PASS %S\r\n");
		iSocket->SendQueueReceive(iStatus, KPop3CmdPass(), &password);
		iSocket->PerformLogging(ETrue);
		break;
		}   
	// new request to initiate TLS
	case ERequestingTLS:
		_LIT8(KPop3CmdOk, "+OK");
		iSocket->SetSSLTLSResponseL(KPop3CmdOk());
		_LIT8(KPop3CmdStls, "STLS\r\n");
		iSocket->SendQueueReceive(iStatus, KPop3CmdStls());
		break;
   
	   default: 	// Unknown state
		   Panic(EImppBadSessionState);
		   break;
		}
	
	SetActive();
	}

//
// return a pointer to our CImTextServerSession so CPop3Operations can use it
//
 CImTextServerSession* CImPop3Session::TextServerSession()
	{
	return iSocket;
	}

//
// Return POP3 error (based on what state the POP login is in
//
TInt CImPop3Session::GetPopError()
	{
	TInt anError=KErrGeneral;
	switch (iState)
		{
	case EWaitingForReply:
	case EPopCapabilities:
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
		if(iPopSettings->POP3Auth() && iState != EPopCapabilities)
			{
			anError=KPop3AuthenticationFailed;
			}
		else
#endif		
			{
			anError=KPop3CannotConnect;
			} 
		break;
	case EAuthorisingUser:
		anError=KPop3InvalidUser;
		break;
	case EAuthorisingPass:
		anError=KPop3InvalidLogin;
		break;
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	case ESaslAuthInProgress:
		anError=KPop3InvalidUser;
		break;
	case ESaslAuthIsDone:
		anError=KPop3InvalidLogin;
		break;
#endif		
	case EAuthorisingApop:
		anError=KPop3InvalidApopLogin;
		break;
	case EPopConnected:
		anError=KErrDisconnected; // assume that this is Pop time out
		break;
	case ERequestingTLS:
		anError = KErrPop3TLSNegotiateFailed;
	default:
		break;
		}
	return anError;
	}

//DMC
TInt CImPop3Session::MaxHeaders() 
	{
	return iPopSettings->InboxSynchronisationLimit();
	}

//
// Set and test state of iOpPending: is a pop operation currently underway?
//
void CImPop3Session::SetPending()
	{
	iOpPending=ETrue;
	}

void CImPop3Session::SetOpNotPending()
	{
	iOpPending=EFalse;
	}

TBool CImPop3Session::IsPending()
	{
	return iOpPending;
	}


TBool CImPop3Session::IsConnectedToInternet()
	{
	return iSocketConnected;
	}

//
// construct Apop string using timestamp and user password (and MD5 hash function)
//
void CImPop3Session::ConstructApopL()
	{
	TPtrC8 pass(iPopSettings->Password());
	TPtrC8 timeStamp;
	TInt timeStampEndPos=KErrNotFound;

	const TInt timeStampStartPos=iConnectReply->Des().Locate('<');
	if(timeStampStartPos==KErrNotFound)
		{
		User::Leave(KErrNotFound);
		}

	timeStampEndPos=iConnectReply->Des().Locate('>');
	if(timeStampEndPos!=KErrNotFound && (timeStampEndPos>timeStampStartPos))
		{
		TInt timeStampLength=(timeStampEndPos-timeStampStartPos)+1;
		timeStamp.Set(iConnectReply->Des().Mid(timeStampStartPos,timeStampLength));
		}
	else
		{
		User::Leave(KErrNotFound);
		}
		
	HBufC8* popBuf = HBufC8::NewLC(timeStamp.Length() + pass.Length());
	TPtr8 bufContent=popBuf->Des();
	bufContent=timeStamp;
	bufContent.Append(pass);

	// do MD5
	CMD5* doHash = CMD5::NewL();
	CleanupStack::PushL(doHash);
	HBufC8* popApop = HBufC8::NewL(2*doHash->HashSize());
	delete iPopApop;
	iPopApop = NULL;
	iPopApop = popApop;
	TPtr8 hashCont=iPopApop->Des();
	hashCont = doHash->Hash(*popBuf);	

	TBuf<32> hexHash;  
	// MD5 algorithm ALWAYS returns 16 bytes of data - which will be converted into
	// 32 characters; each byte represented by a 2 character hex representation, 
	// eg 255="ff"
	
	for (TInt j=0;j<16;j++)
		{
		hexHash.AppendNumFixedWidth(hashCont[j],EHex,2);
		}

	// copy Hex values to iPopApop so that it can be used with the APOP command
	iPopApop->Des().Copy(hexHash);
	
	CleanupStack::PopAndDestroy(2); //popBuf & doHash
	}

CImCaf* CImPop3Session::GetCafL(RFs& aFs)
	{
	if(!iCaf)
		{
		iCaf = new(ELeave) CImCaf(aFs);
		}
	return iCaf;
	}

//
// Use CAPA command to get server capabilities
//
void CImPop3Session::GetCapabilitiesL()
	{
	iPopCapabilities = CImPop3Capa::NewL(this);
	
	iPopCapabilities->Start(iStatus);
	SetActive();
	}
	
TBool CImPop3Session::PipeliningSupport()
	{
	return iPopCapabilities->PipeliningSupport();
	}


#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
/**
Function to select SASL Authentication Mechanism supported by email server.
If CRAM-MD5 Authentication is not supported by server and if Fallback flag is enabled,
will fallback to less secure authentication mechanism.
Fallback priority will be in a following order when server dosn't support CRAM-MD5.
1. APOP
2. PLAIN
3. LOGIN
4. USER/PASS
*/
void CImPop3Session::SelectAuthExtensionProfileL()
	{
	// get the supported SASL Capabilities
	TUint supportedAuthProfiles = iPopCapabilities->SaslAuthExtensionFlag();
			
	// to check whether CRAM-MD5 supported by the server
	if(supportedAuthProfiles & CPopAuthMechanismHelper::ECramMD5)
		{
		//AUTHENTICATE CRAM-MD5 implementation
		iPopAuthHelper = CPopAuthCramMd5MechanismHelper::NewL(*iPopSettings);
		}
	// if server doesn't support CRAM-MD5, check whether FallBack enabled from POP3 email settings.
	else if(iPopSettings->FallBack())
		{
		if(iPopSettings->Apop())
			{
			//APOP
			iState=EAuthorisingApop;
			}
		else if(supportedAuthProfiles & CPopAuthMechanismHelper::EPlain)
			{
			//AUTHENTICATE PLAIN implementation
			iPopAuthHelper = CPopAuthPlainMechanismHelper::NewL(*iPopSettings);
			iState = ESaslAuthIsDone;
			}
		else if(supportedAuthProfiles & CPopAuthMechanismHelper::ELogin)
			{
			//AUTHENTICATE LOGIN implementation
			iPopAuthHelper = CPopAuthLoginMechanismHelper::NewL(*iPopSettings);
			iSaslAuthLogin = ETrue;
			}
		else
			{
			//USER/PASS
			iState = EAuthorisingUser;	
			}
		}
	else
		{
		// if server doesn't support CRAM-MD5 & FallBack is Off, leave with error.
		iState = EWaitingForReply;
		User::Leave(GetPopError());
		}	 
	}

#endif

