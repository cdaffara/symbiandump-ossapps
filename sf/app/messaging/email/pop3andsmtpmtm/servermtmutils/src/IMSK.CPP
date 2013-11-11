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
//

#include "IMSK.H"
#include <imutcon.h>
#include "IMSKSCR.H"
#include "IMUTDLL.H"
#include "cimsocketidletimer.h"

#include <e32hal.h>
#include <securesocket.h>

#include <bautils.h>
#include <barsread.h>

#include <es_enum.h>
#include <ssl_internal.h>


/*************************************************************
* Notes:                                           
*                                                  
* GetIAPBearer won't work if a new bearer is added,
* it will return KImskUnknownBearer                 
*                                                  
*************************************************************/

_LIT8(KImCarriageReturn,"\r\n");
_LIT(KSSLProtocol,"tls1.0");
_LIT8(KNullSSLDomainName, "");

const TInt KImutMicroSecondsToMinutes = 60000000;
const TInt KImutMicroSecondsToSeconds = 1000000;

#if defined(__IMSK_SIMULATION)
_LIT(KGPRSConfigFile, "c:\\logs\\email\\imutconf\\gprs.cfg");
_LIT(KFailIAPConfigFile, "c:\\logs\\email\\imutconf\\iapfail.cfg");
_LIT8(KStart, "start:");
_LIT8(KDuration, "duration:");
_LIT8(KRepeat, "repeat:");
_LIT8(KLogGPRSSimSuspended, "GPRS simulated suspend (%d)");
_LIT8(KLogGPRSSimUnSuspended, "GPRS returned from suspend");
#endif 	// __IMSK_SIMULATION

#ifndef _NO_MSG_LOGGING
#define __IMSK_LOGGING
#endif

#if defined(__IMSK_LOGGING)
#define __LOG_IN(text) (iLog?iLog->AppendResponse(text):void(0))
#define __LOG_COMMENT_OUT(text) (iLog?iLog->AppendComment(text):void(0))
#define __LOG_OUT(text) (iLog?iLog->AppendOut(text):void(0))
#define __LOG_ERR(text,err) (iLog?iLog->AppendError(text,err):void(0))
#else
#define __LOG_IN(text) (void(0))
#define __LOG_COMMENT_OUT(text) (void(0))
#define __LOG_OUT(text) (void(0))
#define __LOG_ERR(text,err) (void(0))
#endif


#if defined(__IMSK_LOGGING)
// IMSK Logging
_LIT8(KLogResolveCompleted, "Resolve completed");
_LIT8(KLogResolveQueued, "Resolve queued");
_LIT8(KLogConnectQueued, "Connect queued on port ");
_LIT8(KLogConnectCompleted, "Connect completed");
_LIT8(KLogSendReceiveCompleted, "SendReceive/Receiving completed");
_LIT8(KLogSendReceiveBinaryCompleted, "SendReceive/ReceivingBinaryData completed");
_LIT8(KLogServerConnectionError, "Server Connection error");
_LIT8(KLogReadCancelled, "Read cancelled");
_LIT8(KLogWriteCancelled, "Write cancelled");
_LIT8(KLogOverridefailed, "Overide failed with"); 
_LIT8(KLogTLSHandShake, "TLS handshake Started"); 
_LIT8(KLogTLSHandShakeCompleted, "TLS handshake completed"); 
_LIT8(KLogSocketClosed, "Closed socket"); 
_LIT8(KLogTLSResponse, "SSL/TLS response should be '%S'");
_LIT8(KLogSendReceiveTimedOut, "SendReceive has timed out");

#endif

//*********************************************************************************************/
//*                                                                                           */
//*                           Construction/Destruction functions                              */
//*                                                                                           */
//*********************************************************************************************/

LOCAL_C void RequestComplete(TRequestStatus& aStatus,TInt aError)
	{
	TRequestStatus* pStatus=&aStatus;
	User::RequestComplete(pStatus,aError);
	}

EXPORT_C CImTextServerSession  *CImTextServerSession::NewL()
	{
	CImTextServerSession* self=new (ELeave) CImTextServerSession();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

/**
@internalTechnology
@released
*/
EXPORT_C CImTextServerSession  *CImTextServerSession::NewL(RSocketServ& aSocketServ, CImConnect& aConnect)
	{
	CImTextServerSession* self=new (ELeave) CImTextServerSession(aSocketServ, aConnect);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

/**
Factory constructor that allows setting of send and receive idle times.

An idle time of zero implies that no timer should be set.

@param	aSendIdleTime
The idle time in minutes allowed for sending. A value of zero indicates that no timer 
should be set.

@param	aReceiveIdleTime
The idle time in minutes allowed for receiving. A value of zero indicates that no timer 
should be set.

@return
A pointer to the newly created CImTextServerSession object. 
*/
EXPORT_C CImTextServerSession  *CImTextServerSession::NewL(TInt aSendIdleTime, TInt aReceiveIdleTime)
	{
	CImTextServerSession* self=new (ELeave) CImTextServerSession(aSendIdleTime, aReceiveIdleTime);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

/**
@internalTechnology
@released
*/
EXPORT_C CImTextServerSession* CImTextServerSession::NewL(TInt aSendIdleTime, TInt aReceiveIdleTime, RSocketServ& aSocketServ, CImConnect& aConnect)
	{
	CImTextServerSession* self=new (ELeave) CImTextServerSession(aSendIdleTime, aReceiveIdleTime, aSocketServ, aConnect);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CImTextServerSession::CImTextServerSession()
	: CMsgActive(EActivePriorityHigh), 
	  iState(EImClosed),
	  iPerformLogging(ETrue)
	{
	}

CImTextServerSession::CImTextServerSession(RSocketServ& aSocketServ, CImConnect& aConnect)
	: CMsgActive(EActivePriorityHigh), 
	  iServ(aSocketServ),
	  iState(EImClosed),
	  iConnect(&aConnect),
	  iPerformLogging(ETrue),
	  iClientOwnsConnection(ETrue)
	{
	}

/**
Constructor that allows setting of the send and receive idle times.

An idle time of zero implies that no timer should be set.

@param	aSendIdleTime
The idle time in minutes allowed for sending. A value of zero indicates that no timer 
should be set.

@param	aReceiveIdleTime
The idle time in minutes allowed for receiving. A value of zero indicates that no timer 
should be set.
*/
CImTextServerSession::CImTextServerSession(TInt aSendIdleTime, TInt aReceiveIdleTime)
	: CMsgActive(EActivePriorityHigh), 
	  iState(EImClosed),
	  iPerformLogging(ETrue),
	  iSendIdleTime(aSendIdleTime*KImutMicroSecondsToMinutes),
	  iReceiveIdleTime(aReceiveIdleTime*KImutMicroSecondsToMinutes),iPrimaryTextServerSession(NULL)
	{
	}

CImTextServerSession::CImTextServerSession(TInt aSendIdleTime, TInt aReceiveIdleTime, RSocketServ& aSocketServ, CImConnect& aConnect)
	: CMsgActive(EActivePriorityHigh), 
	  iServ(aSocketServ),
	  iState(EImClosed),
	  iConnect(&aConnect),
	  iPerformLogging(ETrue),
	  iSendIdleTime(aSendIdleTime*KImutMicroSecondsToMinutes),
	  iReceiveIdleTime(aReceiveIdleTime*KImutMicroSecondsToMinutes),
	  iClientOwnsConnection(ETrue)
	{
	}

//
// 2nd stage of construction
//
void CImTextServerSession::ConstructL()
	{
	iBuffer = HBufC8::NewL(KImMailMaxBufferSize*2 +2);

	if (!iClientOwnsConnection)
		{
		User::LeaveIfError(iServ.Connect());
		}

#if defined(__IMSK_SIMULATION)

	// if gprs suspension file exists - use its contents to determine
	// the the time to delay TCP/IP traffic

	User::LeaveIfError(iFs.Connect());
	TInt err = iGprsFile.Open(iFs,KGPRSConfigFile,EFileShareAny);

	if (err != KErrNone && err != KErrPathNotFound && err != KErrNotFound)
		User::Leave(err);
	
	if (err == KErrNone)
		{
		iGprsConfigExists=ETrue;
		// read the first delay period
		ReadNextPeriod();

		// get baseline time
		if (SuspendPeriodSet())
			iLastSuspend.UniversalTime();
		
		User::LeaveIfError(iSuspendTimer.CreateLocal());
		}
	else
		iGprsConfigExists=EFalse;
#endif	// __IMSK_SIMULATION

	iSocketIdleTimer = CImSocketIdleTimer::NewL(*this);
	CActiveScheduler::Add(this); 
	}

//
// Destruction
//
CImTextServerSession::~CImTextServerSession()
	{
	Disconnect();
	delete iBuffer;
	if (!iClientOwnsConnection)
		{
		delete iConnect;
		iServ.Close();
		}
	delete iSentData;
	delete iTLSResponse;
#if defined(__IMSK_SIMULATION)
	iGprsFile.Close();
	delete iSendData;
	iFs.Close();
#endif // __IMSK_SIMULATION
	delete iSocketIdleTimer;
	delete iSSLDomainName;
	}


//*********************************************************************************************/
//*                                                                                           */
//*                           Connect/Disconnect functions                                    */
//*                                                                                           */
//*********************************************************************************************/

//
// Queue a standard or wrapped SSL connect (depending on state of iWrappedSocket) assuming the 
//  socket is successfully opened- RunL called on completion
//

void CImTextServerSession::QueueGenericConnectL(TRequestStatus &aStatus,const TDesC& anAddressDesc, TInt aPortNum, const CImIAPPreferences& aIAPPreferences, const TDesC8& aSSLDomainName)
	{
#if defined(__IMSK_LOGGING)
	CreateLogFile(aPortNum);  // Attempt to create the log file
#endif
	Queue(aStatus);

	iIAPPreferences=&aIAPPreferences;
	iPortNum=aPortNum;
	iAddressDesc.Set(anAddressDesc);

	delete iSSLDomainName;
	iSSLDomainName = NULL;
	if (aSSLDomainName.Length() > 0)
		{
		iSSLDomainName = aSSLDomainName.AllocL();
		}
	else
		{
		iSSLDomainName = HBufC8::NewL(iAddressDesc.Length());
		TPtr8 addr8ptr = iSSLDomainName->Des();
		addr8ptr.Copy(iAddressDesc);
		}

	__ASSERT_ALWAYS(iState==EImClosed,gPanic(EImskSocketOpen));

	iState = EImDialUsingOverride;
#if defined(__IMSK_SCRIPTING)
	if (!iScript)
		OpenScriptFile(iPortNum); // open the relevant script file if it exists

	if(iScript)
		RequestComplete(iStatus,KErrNone);
	else
#endif //(__IMSK_SCRIPTING)
		{
		// If we are not using the clients connection, create our own one now
		if (!iClientOwnsConnection)
			{
			delete iConnect;
			iConnect = 0;
			iConnect = CImConnect::NewL(*iIAPPreferences,*this);
			}
#if defined(__IMSK_SIMULATION)
		iConnect->SetIAPsToFail(ReadConfigNum(KFailIAPConfigFile));
#endif //(__IMSK_SIMULATION)

		// if local textseversession is active, then connect the session using existing RConnection
		if(iPrimaryTextServerSession)
			{
			// Attaching the existing RConnection.
			iConnect->SecondaryStartL(iPrimaryTextServerSession);			
			// Queue the connection
			DoQueueConnect();
			return;			
			}
		else
			{
			// If we are using the clients connection then it should have already been
			// started, so just self complete to keep the state machine going. If it is
			// our own connection, start it now.
			if (iClientOwnsConnection)
				{
				RequestComplete(iStatus, KErrNone);
				}
			else
				{
				iConnect->StartL(iStatus);	
				}
			}
		}
	SetActive();
	}

//
// Queue a connect assuming the socket is successfully opened- RunL called on completion
//

EXPORT_C void CImTextServerSession::QueueConnectL(TRequestStatus &aStatus,const TDesC& anAddressDesc, TInt aPortNum, const CImIAPPreferences& aIAPPreferences, TBool /*aEnableTimeout*/)
	{
		iWrappedSocket=EFalse;
		QueueGenericConnectL(aStatus, anAddressDesc, aPortNum,  aIAPPreferences, KNullSSLDomainName);
	}

/**
Queue a connect on a socket assuming the socket is successfully opened.

@param aStatus Asynchronous completion status
@param aAddressDesc Address of the server to connect to
@param aIAPPreferences IAP connection preferences to be used		
@param aPortNum Port number eg. 993, 465, 995.
@param aSSLDomainName SSL domain name to use for the connection
@pre   None
@post  Connection is ready to send and receive data
@since 9.5
*/
EXPORT_C void CImTextServerSession::QueueConnectL(TRequestStatus &aStatus,const TDesC& anAddressDesc, TInt aPortNum, const CImIAPPreferences& aIAPPreferences, const TDesC8& aSSLDomainName)
	{
	iWrappedSocket=EFalse;
	QueueGenericConnectL(aStatus, anAddressDesc, aPortNum,  aIAPPreferences, aSSLDomainName);
	}

//
// Queue a wrapped SSL connect assuming the socket is successfully opened- RunL called on completion
//

EXPORT_C void CImTextServerSession::SSLQueueConnectL(TRequestStatus &aStatus,const TDesC& anAddressDesc, TInt aPortNum, const CImIAPPreferences& aIAPPreferences, TBool /*aEnableTimeout*/)
	{
		iWrappedSocket=ETrue;
		QueueGenericConnectL(aStatus, anAddressDesc, aPortNum,  aIAPPreferences, KNullSSLDomainName);
	}

/**
Queue a wrapped SSL connect on a socket assuming the socket is successfully opened.

@param aStatus Asynchronous completion status
@param aAddressDesc Address of the server to connect to
@param aIAPPreferences IAP connection preferences to be used		
@param aPortNum Port number eg. 993, 465, 995.
@param aSSLDomainName SSL domain name to use for the connection
@pre   None
@post  Connection is ready to send and receive data
@since 9.5
*/
EXPORT_C void CImTextServerSession::SSLQueueConnectL(TRequestStatus &aStatus,const TDesC& anAddressDesc, TInt aPortNum, const CImIAPPreferences& aIAPPreferences, const TDesC8& aSSLDomainName)
	{
	iWrappedSocket=ETrue;
	QueueGenericConnectL(aStatus, anAddressDesc, aPortNum, aIAPPreferences, aSSLDomainName);
	}

//
// Public socket disconnection
//	
EXPORT_C void CImTextServerSession::Disconnect()
	{
	iIAPCached=EFalse;
	Cancel();
	Close();
	}


EXPORT_C void CImTextServerSession::Disconnect(TRequestStatus& aStatus)
	{
	Disconnect();
	RequestComplete(aStatus,KErrNone);
	}


//*********************************************************************************************/
//*                                                                                           */
//*                           Send/Recieve/Query functions                                    */
//*                                                                                           */
//*********************************************************************************************/

//
// Async send functions
//
EXPORT_C void CImTextServerSession::Send(TRequestStatus &aStatus, const TDesC8& aDesc)
	{
	if (iState!=EImSendReceive)
		{
		RequestComplete(aStatus,KErrDisconnected);
		return;
		}
	
	iSendShortIdleTime		= 0;
	iReceiveShortIdleTime	= 0;
	
	Queue(aStatus);

	iSendReceive=EImSending;
	RealSend(aDesc);
	}

//
// Async send function, specifying short idle timeouts should be used.
// aIdleTime specifies the send and next receive timeout to be used in seconds
//
EXPORT_C void CImTextServerSession::SendWithTimeout(TRequestStatus &aStatus, TInt aIdleTime, const TDesC8& aDesc)
	{
	if (iState!=EImSendReceive)
		{
		RequestComplete(aStatus,KErrDisconnected);
		return;
		}
	
	iSendShortIdleTime    = aIdleTime*KImutMicroSecondsToSeconds;
	iReceiveShortIdleTime = aIdleTime*KImutMicroSecondsToSeconds;

	Queue(aStatus);

	iSendReceive=EImSending;
	RealSend(aDesc);
	}
	
EXPORT_C void CImTextServerSession::SendQueueReceiveWithTimeout(TRequestStatus& aStatus, TInt aIdleTime, const TDesC8& aDesc) 
	{ 
	if (iState!=EImSendReceive)      
		{ 
		RequestComplete(aStatus, KErrDisconnected);
		return;
		} 
               
		iSendShortIdleTime    = aIdleTime*KImutMicroSecondsToSeconds; 
		iReceiveShortIdleTime = aIdleTime*KImutMicroSecondsToSeconds; 
    	Queue(aStatus); 
    
		iSendReceive=EImSendingQueueReceive; 
		RealSend(aDesc);
	} 	

//
// async. Send
//
EXPORT_C void CImTextServerSession::Send(TRequestStatus &aStatus, TRefByValue<const TDesC8> aFmt,...)
	{
	VA_LIST list;
	VA_START(list,aFmt);
	TBuf8<2*KImMailMaxBufferSize+2>aBuf;
	aBuf.AppendFormatList(aFmt,list);

	Send(aStatus, aBuf);
	}


//
// async Send and receive operation
//
EXPORT_C void CImTextServerSession::SendQueueReceive(TRequestStatus &aStatus, const TDesC8& aDesc)
	{
	if (iState!=EImSendReceive)
		{
		RequestComplete(aStatus,KErrDisconnected);
		return;
		}

	Queue(aStatus);

	iSendReceive=EImSendingQueueReceive;

	RealSend(aDesc);
	}


//
// async. SendQueueRecieve
//
EXPORT_C void CImTextServerSession::SendQueueReceive(TRequestStatus &aStatus, TRefByValue<const TDesC8> aFmt,...)
	{
	VA_LIST list;
	VA_START(list,aFmt);
	TBuf8<2*KImMailMaxBufferSize+2>aBuf;
	aBuf.AppendFormatList(aFmt,list);

	SendQueueReceive(aStatus, aBuf);
	}



//
// User queues a new request from the socket (unless there's a full line of data in buffer
// then signal user and there's no need to make a receive request)
//
EXPORT_C void CImTextServerSession::QueueReceiveNextTextLine(TRequestStatus& aStatus)
	{
	if (iState!=EImSendReceive)
		{
		RequestComplete(aStatus,KErrDisconnected);
		return;
		}

	iSendReceive=EImReceiving;


	Queue(aStatus);

	// Is there already a full text line in the buffer (ie is there a CRLF?)
	if(iBuffer->Find(KImCarriageReturn)==KErrNotFound)
		{
		RealReceive(iReceive);
		return;
		}
	// force calling active object to terminate
	Complete(KErrNone);
	}

//
// Return first full line of data received from socket to user
//
EXPORT_C TImLineType CImTextServerSession::GetCurrentTextLine(TDes8& aDesc)
	{
	__ASSERT_ALWAYS(this->IsActive()==EFalse,gPanic(EImskSocketStillActive));
	// we have two possibilites a CRLF term line or we've filled our buffer
	TInt bufLength=aDesc.MaxLength();
	TPtr8 bufPtr=iBuffer->Des();

	if(iBuffer->Length()==0)
		{
		aDesc.Zero();
		return EReceiveBufferEmpty;
		}

	TInt crLfPos=iBuffer->Find(KImCarriageReturn); // need to update 
	if(crLfPos!=KErrNotFound)
		{
		if(crLfPos<bufLength-1) // the user's buffer is big enough
			{
			bufLength=crLfPos+KCarriageLineFeedLength;
			iCurrentLineType=ECRLFTerminated;
			}
		else if(crLfPos==bufLength-1)
			{
			iCurrentLineType=EBufferTooSmall;
			--bufLength;
			}
		else // not big enough fill it and tell him there's more left incl. the CRLF
			{
			iCurrentLineType=EBufferTooSmall;
			}
		}
	else if(iCurrentLineType==EReceiveBufferFull) // buffer full - you sort it out
		{
		bufLength=Min(iBuffer->Length(),bufLength);
		}
	else
		{
		// gPanic coz this shouldn't happen
		gPanic(EImskUnknownState);
		}
	aDesc=iBuffer->Left(bufLength);
	bufPtr.Delete(0,bufLength);

	TInt left=iReceive.Length();
	if (left)
		{
		if (left>bufLength)
			left=bufLength;
		bufPtr.Append(iReceive.Left(left));
		iReceive.Delete(0,left);
		}
#if defined(__IMSK_SCRIPTING)
	if(iScript)
		__LOG_IN(aDesc);
#endif
	return iCurrentLineType;
	}



EXPORT_C void CImTextServerSession::ReceiveBinaryData(TRequestStatus& aStatus, TDes8& aDes, TInt aLen)
	{
	if (iState!=EImSendReceive)
		{
		RequestComplete(aStatus,KErrDisconnected);
		return;
		}
	

	Queue(aStatus);
	iSendReceive=EImReceivingBinaryData;
	iLen=aLen;
	RealReceive(aDes);
	}


// Returns the IAP we are connecting/connected with in aIAP or returns an error code
EXPORT_C TInt CImTextServerSession::GetIAPValue(TUint32 &aIAP)
	{
	TInt err=KErrNone;

	if(iIAPCached)
		// Return cached IAP value
		{
		aIAP=iCurrentIAPcache;
		}
	else
		// IAP not yet cached
		{
#if defined(__IMSK_SCRIPTING)
		if (iScript)
			{
			aIAP = 0;
			return KErrNotFound;
			}
		else
#endif //(__IMSK_SCRIPTING)
			{			
			err = iConnect->GetIAPValue(aIAP);
			if(err==KErrNone && !(iState == EImClosed || iState == EImDialUsingOverride))
				// Only cache if no error and connection is complete
				{
				iCurrentIAPcache=aIAP;
				iIAPCached=ETrue;
				}
			}
		}

	return(err);
	}
//Returns the Name of the RConnection in use.
EXPORT_C TInt CImTextServerSession::GetRConnectionName(TName &aName)
	{
#if defined(__IMSK_SCRIPTING)
	if (iScript)
		{
		_LIT(KNull,"");
		aName.Copy(KNull);
		return KErrNotFound;			
		}
	else		
#endif //(__IMSK_SCRIPTING)
		{
		return (iConnect->GetRConnectionName(aName));		
		}
	}

// Returns the bearer type we are connected to with in aBearer or returns an error code
EXPORT_C TInt CImTextServerSession::GetIAPBearer(TUint32 &aBearer)
	{
#if defined(__IMSK_SCRIPTING)
	if (iScript)
		{
		aBearer = 0;
		return KErrNotFound;
		}
	else
#endif //(__IMSK_SCRIPTING)
		{		
		return iConnect->GetIAPBearer(aBearer);
		}
	}

// Returns the last socket activity timeout value for the connection
EXPORT_C TInt CImTextServerSession::GetLastSocketActivityTimeout(TUint32& aTimeout)
	{
#if defined(__IMSK_SCRIPTING)
	if (iScript)
		{
		aTimeout = 0;
		return KErrNotFound;
		}
	else
#endif //(__IMSK_SCRIPTING)
		{		
		return iConnect->GetLastSocketActivityTimeout(aTimeout);
		}
	}

RSocketServ& CImTextServerSession::GetSocketServ()
	{
	return iServ;
	}

//*********************************************************************************************/
//*                                                                                           */
//*                           Other public functions                                          */
//*                                                                                           */
//*********************************************************************************************/


EXPORT_C const TDesC& CImTextServerSession::LocalName()
	{
   	return iLocalName;	// a text descriptor - containing the numeric IP address of this client, eg "194.255.242.34"
	}


EXPORT_C void CImTextServerSession::LogText(const TDesC8& aDesc)
	{
	__LOG_COMMENT_OUT(aDesc);
	}



EXPORT_C void CImTextServerSession::PerformLogging(TBool aLogging)
	{
	iPerformLogging = aLogging;
	}



EXPORT_C void CImTextServerSession::LogError(const TDesC8& aDesc, const TInt aNumber)
	{
	__LOG_ERR(aDesc,aNumber);
	}




EXPORT_C void CImTextServerSession::SetSSLTLSResponseL(const TDesC8& aDesc)
	{
	__ASSERT_ALWAYS( aDesc.Length(), gPanic(EImskNoTLSResponseString));
	iTLSResponse=aDesc.AllocL();
	if (iTLSResponse==NULL)
		User::Leave(KErrNoMemory);
	iReadTLSResponse=ETrue;
#if defined(__IMSK_LOGGING)
	TBuf8<1024> buf;
	buf.AppendFormat(KLogTLSResponse,&aDesc);
	__LOG_COMMENT_OUT(buf);
#endif
	}


void CImTextServerSession::SocketIdle()
	{
	__LOG_COMMENT_OUT(KLogSendReceiveTimedOut);

	// The socket has been idle too long - probably in a half open situation 
	// and server could well have disconnected the session and we'll never 
	// know about it. So disconnect and notify the observer.
	iSocketIdleTimeSet=ETrue;
	Disconnect();
	}


//*********************************************************************************************/
//*                                                                                           */
//*                           Private IMSK connect functions                                  */
//*                                                                                           */
//*********************************************************************************************/

	
//
// Queue a connect assuming the socket is successfully opened- RunL called on completion
//
void CImTextServerSession::ParseSSLTLSResponseL()
	{
	__LOG_COMMENT_OUT(*iBuffer);
	if (iBuffer->Find(*iTLSResponse)==KErrNotFound)
		{
		__LOG_ERR(_L8("ParseSSLTLSResponseL failed with "),KImskSSLTLSNegotiateFailed);
		Complete(KImskSSLTLSNegotiateFailed);
		return;
		}
	delete iTLSResponse;  //finished with TLSresponse.  free the memory
	iTLSResponse=NULL;
	iReadTLSResponse=EFalse;
	CreateSecureSocketL();
	}

void CImTextServerSession::CreateSecureSocketL()
	{
#if defined(__IMSK_SCRIPTING)
	if (iScript)
		{
		iState=EImTLSHandShakeStarted;
		RequestComplete(iStatus,KErrNone);
		SetActive();
		__LOG_COMMENT_OUT(KLogTLSHandShake);
		return;
		}
#endif
	iSecureSocket = CSecureSocket::NewL( iSocket, KSSLProtocol);
	__ASSERT_DEBUG(iSSLDomainName, gPanic(EPanicNoSSLDomainName));
	if (iSSLDomainName)
		{
		User::LeaveIfError(iSecureSocket->SetOpt(KSoSSLDomainName, KSolInetSSL, *iSSLDomainName));
		}
	iSecureSocket->StartClientHandshake( iStatus );
	iState=EImTLSHandShakeStarted;
	SetActive();
	__LOG_COMMENT_OUT(KLogTLSHandShake);
	}

void CImTextServerSession::SocketConnect()
	{

	iState=EImConnect;

#if defined(__IMSK_SCRIPTING)
	if (iScript)
		{
		SetActive();
		RequestComplete(iStatus,KErrNone);
		}
	else
#endif
		{
		__ASSERT_DEBUG(iScript==NULL,User::Invariant());

		iHostResolver.Close();
		TInt err;
		__ASSERT_ALWAYS(iPortNum>=0, gPanic(EImskInvalidPortNumber));
		
		TSockAddr& addr=iHostent().iAddr;
		addr.SetPort(iPortNum);
		LogText(addr);
		err=iSocket.Open(iServ,KAfInet,KSockStream, KProtocolInetTcp, iConnect->GetConnection());
		
		if(err==KErrNone)
			{
			iSocket.Connect(addr,iStatus);
			SetActive();
			return;
			}
		__LOG_ERR(KLogServerConnectionError,err);
		Complete(err);
		}
	}


// called from DoRunL after a connection has been opened
void CImTextServerSession::DoQueueConnect()
	{
#if defined(__IMSK_LOGGING)
	if (!iLog)
		CreateLogFile(iPortNum);  // Attempt to create the log file
#endif
	TInt err;
#if defined(__IMSK_SCRIPTING)
	if (iScript)
		{
		iState=EImResolve;
		RequestComplete(iStatus,KErrNone);
		SetActive();
		return;
		}
	else
#endif
		{
		iState=EImResolve;
		err = iHostResolver.Open(iServ, KAfInet, KProtocolInetTcp, iConnect->GetConnection());
		if(err==KErrNone)
			{
			iHostResolver.GetByName(iAddressDesc, iHostent,iStatus);
			SetActive();
			__LOG_COMMENT_OUT(KLogResolveQueued);
			return;
			}
		}
	Complete(err);
	}


// called from DoRunL after a socket has been connected to
void CImTextServerSession::DoConnectedToSocketL()
	{
#if defined(__IMSK_SCRIPTING)
	// Retrieve the local IP address of this client
	if(!iScript)
#endif
		{
		TInetAddr address;
		iSocket.LocalName(address);
		address.Output(iLocalName);
		}

	if(iWrappedSocket)
		{
		CreateSecureSocketL();
		}
	else
		{
		iState=EImSendReceive;
		iSendReceive=EImInactive;
		TPtr8 bufPtr=iBuffer->Des();
		bufPtr.Zero();
		}
	}


//
// Close all socket and resovler access. Restore to just created state.
//
void CImTextServerSession::Close()
	{
	iHostResolver.Close();
	if (iSecureSocket)
		{
		//Closing the Secure Connection & the Socket.
		//Since the Socket is closed, Shutdown shouldn't be called again, so making iSocketIdelTimeSet=EFalse
		iSecureSocket->Close();
		delete iSecureSocket;
		iSecureSocket=NULL;
		iSocketIdleTimeSet = EFalse;
		}
 	else if(iSocketIdleTimeSet) 
		{ 
		//For non Secure Socket, if idle time is set then shutdown the socket
		// Shutdown should be called only once, so make iSocketIdelTimeSet=EFalse 
		iSocketIdleTimeSet=EFalse; 
		iSocket.Shutdown(RSocket::EImmediate, iStatus); 
		iState = EImSendReceiveTimedOut; 
		SetActive();
		return; 
		} 
	else
		{
		//For non Secure Socket, if idle time is not set then close the socket
		iSocket.Close();	
		}

	iState=EImClosed;
	__LOG_COMMENT_OUT(KLogSocketClosed);
#if defined(__IMSK_SCRIPTING)
	delete iScript;
	iScript=NULL;
#endif
#if defined(__IMSK_LOGGING)
	delete iLog;
	iLog=NULL;
#endif

#if defined(__IMSK_SIMULATION)
	if (iGprsConfigExists)
		{
		// reset to begin of config file
		iCfgFilePos = 0;
		// read the first delay period
		ReadNextPeriod();

		if (SuspendPeriodSet())
			iLastSuspend.UniversalTime();
		}
#endif	// __IMSK_SIMULATION
	}
	

//*********************************************************************************************/
//*                                                                                           */
//*                           CActive private IMSK functions                                  */
//*                                                                                           */
//*********************************************************************************************/



//
// Current request has completed - deal with it
//
void CImTextServerSession::DoRunL()
	{
	delete iSentData;
	iSentData = 0;

	switch(iState)
		{
		case EImTLSHandShakeStarted:
			__LOG_COMMENT_OUT(KLogTLSHandShakeCompleted);
			iState=EImSendReceive;
			iSendReceive=EImInactive;
			iSecurityState=EImSecurityStateOn;
			break;
		case EImDialUsingOverride:
			DoQueueConnect();
			break;
		case EImResolve:   // we have a internet connection
			__LOG_COMMENT_OUT(KLogResolveCompleted);
			__LOG_ERR(KLogConnectQueued,iPortNum);
			
			SocketConnect();
			break;

		case EImConnect:  // we have a socket connected
			__LOG_COMMENT_OUT(KLogConnectCompleted);
			DoConnectedToSocketL();
			break;

		case EImSendReceive:
			{
			// Cancel the socket idle timer as the socket request has completed.
			iSocketIdleTimer->Cancel();

			if(iSendReceive==EImReceiving)
				{
				__LOG_COMMENT_OUT(KLogSendReceiveCompleted);
				__LOG_IN(iReceive);

	
				TPtr8 bufPtr=iBuffer->Des();
				TInt space=bufPtr.MaxLength()-bufPtr.Length();
				if(iReceive.Length()>space)
					{
					bufPtr.Append(iReceive.Left(space));
					iReceive.Delete(0,space);
					iSendReceive=EImInactive;
					iCurrentLineType=EReceiveBufferFull;
					}
				else
					{
					bufPtr.Append(iReceive);
					iReceive.Zero();
					// search for a CRLF in iBuffer
					if(iBuffer->Find(KImCarriageReturn)==KErrNotFound)
						{
						// Start up the socket idle timer.
						if(iReceiveShortIdleTime.Int() > 0 )
							{
							iSocketIdleTimer->After(iReceiveShortIdleTime);
							iReceiveShortIdleTime = 0;
							} 
						else if( iReceiveIdleTime.Int() > 0 )
							{
							iSocketIdleTimer->After(iReceiveIdleTime);
							}

						if (iSecurityState==EImSecurityStateOn)
							iSecureSocket->RecvOneOrMore(iReceive,iStatus,iLen);
						else
							iSocket.RecvOneOrMore(iReceive,NULL,iStatus,iLen);
						SetActive();
						}
					else
						{
						iSendReceive=EImInactive;
						iCurrentLineType=ECRLFTerminated;
						if (iReadTLSResponse)
							{
							ParseSSLTLSResponseL();
							return;
							}
						}
					}	
			}
			else if (iSendReceive==EImReceivingBinaryData)
				{
				// just pass raw binary data straight back to caller without any processing
				__LOG_COMMENT_OUT(KLogSendReceiveBinaryCompleted);
				__LOG_IN(*iReceiveData);
				}
			else if(iSendReceive==EImSendingQueueReceive)
				{
				iSendReceive=EImReceiving;
				// Is there already a full text line in the buffer (ie is there a CRLF?)
				if(iBuffer->Find(KImCarriageReturn)==KErrNotFound)
					{
					RealReceive(iReceive);
					return;
					}
				}
  			else if (iSendReceive==EImSending)
  				{
				if (iReadTLSResponse)
					{
					iSendReceive=EImReceiving;
					RealReceive(iReceive);
					return;
					}
				Complete(KErrNone);
				}
#if defined(__IMSK_SIMULATION)
			else if(iSendReceive==EImSuspended)
				{
				__LOG_COMMENT_OUT(KLogGPRSSimUnSuspended);

				iSendReceive=iSuspendedState;
				if(iSendReceive==EImSending || iSendReceive==EImSendingQueueReceive)
					{
					RealSend(*iSendData);
					delete iSendData;
					iSendData=NULL;
					}
				else
					RealReceive(*iReceiveData);
				}
#endif	// __IMSK_SIMULATION
			}
			break;
		case EImSendReceiveTimedOut: 
			{ 
			iSocket.Close();     
			iState=EImClosed; 
			Complete(KErrTimedOut); 
			break;               
			} 

		default:
			gPanic(EImskUnknownState);
		}

	}


//
// Cancel connect/receive if one is currently occurring
//
void CImTextServerSession::DoCancel()
	{
	// clear out receive buffer if we cancel
	TPtr8 bufPtr=iBuffer->Des();
	bufPtr.Zero();

	switch(iState)
		{
	case EImResolve:
		iHostResolver.Cancel();
		break;
	case EImConnect:
		iSocket.CancelConnect();
		break;
	case EImSendReceive:
		{
#if defined(__IMSK_SCRIPTING)
		if (!iScript)
#endif
			{
			// Cancel the socket idle timer as the socket request is also being
			// cancelled.
			iSocketIdleTimer->Cancel();

			if(iSendReceive==EImSending || iSendReceive==EImSendingQueueReceive)
				{
				__LOG_COMMENT_OUT(KLogWriteCancelled);
				if (iSecurityState==EImSecurityStateOn)
					iSecureSocket->CancelSend();
				else
					iSocket.CancelSend();
				}
			else if(iSendReceive==EImReceiving || iSendReceive==EImReceivingBinaryData)
				{
				__LOG_COMMENT_OUT(KLogReadCancelled);
				if (iSecurityState==EImSecurityStateOn)
					iSecureSocket->CancelRecv();
				else
					iSocket.CancelRead();
				}
			}
#if defined(__IMSK_SCRIPTING)
		else
			iScript->Cancel();
#endif//__IMSK_SCRIPTING
#if defined(__IMSK_SIMULATION)
		if(iSendReceive==EImSuspended) iSuspendTimer.Cancel();
#endif//__IMSK_SIMULATION
		}
		break;
	case EImTLSHandShakeStarted:
        if (iSecureSocket)
        	{
            iSecureSocket->CancelHandshake();
        	}
        break;
	case EImClosed:
	default:
		break;
	case EImDialUsingOverride:
		if (iConnect)
			iConnect->Cancel();
		break;
	case EImSendReceiveTimedOut: 
		iSocket.CancelAll(); 
		iSocket.Close();     
		iState=EImClosed; 
		break; 
		}
    
	delete iSentData;
	iSentData = 0;

	if(!iSocketIdleTimeSet)
		{
		CMsgActive::DoCancel();
		}
	}

//
// If it all goes wrong
//
void CImTextServerSession::DoComplete(TInt& aStatusValue)
	{
	TInt status=aStatusValue;
	//
	// test for KErrEof returns from socket.  This might indicate that there has been 
	// a TCPIP half close - with the remote end sending a FIN
	// If this occurs as response to QUIT treat as a remote disconnection
	//
	if (iState==EImDialUsingOverride)
		{
		__LOG_ERR(KLogOverridefailed,aStatusValue);
		Close();
		return;
		}
	if(status==KErrEof)
		{
		aStatusValue=KErrDisconnected;
		Close();
		return;
		}
	// don't close the socket if the ide timer timed out, or if asynch operation was cancelled...
	if((aStatusValue) && (aStatusValue!=KErrCancel) && (aStatusValue!=KErrTimedOut))
		{
		// there was an error so no need to keep on running these timers...
		if(iState==EImResolve)
			{
			if (aStatusValue==KErrNotFound)
				aStatusValue=KImskErrorDNSNotFound;
		
			if (aStatusValue==KErrLocked)
				aStatusValue=KImskErrorControlPanelLocked;
			}

		if(iState!=EImClosed)
			Close();
		}

	delete iSentData;
	iSentData = 0;
	}


//*********************************************************************************************/
//*                                                                                           */
//*                           Private IMSK send/receive functions                             */
//*                                                                                           */
//*********************************************************************************************/

// log & Send to a socket
void CImTextServerSession::RealSend(const TDesC8& aDesc)
	{
	// Cancel the socket idle timer as we are about to send some data	
	iSocketIdleTimer->Cancel();

#if defined(__IMSK_SIMULATION)
	if(IsSuspended())
		{
		iSuspendedState=iSendReceive;
		iSendReceive=EImSuspended;
		iSendData=aDesc.Alloc();
		if(iSendData==NULL)
		   {
		   TInt err=KErrNoMemory;
		   DoComplete(err);
		   }
		SetAfterTimer();
		}
	else
#endif
		{
		// Do the logging of the text if required
		if (iPerformLogging)
			__LOG_OUT(aDesc);

#if defined(__IMSK_SCRIPTING)
		if(iScript)
			RequestComplete(iStatus,KErrNone);
		else
#endif
			{
			if(iSecurityState==EImSecurityStateFailed) 
				RequestComplete(iStatus,KImskSecuritySettingsFailed);
			else
				{
				iSentData = aDesc.Alloc();
				
				if (iSentData==NULL)
					RequestComplete(iStatus, KErrNoMemory);
				else
					{
					// Start up the socket idle timer.
					if( iSendShortIdleTime.Int() > 0 )
						{
						iSocketIdleTimer->After(iSendShortIdleTime);						
						iSendShortIdleTime = 0;
						}
					else if( iSendIdleTime.Int() > 0 )
						{
						iSocketIdleTimer->After(iSendIdleTime);
						}

					if (iSecurityState==EImSecurityStateOn)
						iSecureSocket->Send(*iSentData,iStatus);
					else
						iSocket.Write(*iSentData,iStatus);
					}
				}
			}
		}
	SetActive();
	}

//
// Queue a recieve from a socket
//
void CImTextServerSession::RealReceive(TDes8& aDesc)
	{
	// Cancel the socket idle timer as we are about to receive some data
	iSocketIdleTimer->Cancel();

	iReceiveData=&aDesc;
#if defined(__IMSK_SIMULATION)
	if(IsSuspended())
		{
		iSuspendedState=iSendReceive;
		iSendReceive=EImSuspended;	
		SetAfterTimer();
		}
	else
#endif
		{
#if defined(__IMSK_SCRIPTING)

		if(iScript)
			{
			TPtr8 bufPtr=iBuffer->Des();
			iScript->RetrieveResponse(bufPtr, iStatus);
			TInt bufLength = aDesc.MaxLength();
			aDesc=iBuffer->Left(bufLength);
			bufPtr.Delete(0,bufLength);
			}
		else
#endif	
			{
			if(iSecurityState==EImSecurityStateFailed) 
				RequestComplete(iStatus,KImskSecuritySettingsFailed);
			else 
				{
				// Start up the socket idle timer.
				if(iReceiveShortIdleTime.Int() > 0 )
					{
					iSocketIdleTimer->After(iReceiveShortIdleTime);
					iReceiveShortIdleTime = 0;
					} 
				else if( iReceiveIdleTime.Int() > 0 )
					{
					iSocketIdleTimer->After(iReceiveIdleTime);
					}

				if (iSecurityState==EImSecurityStateOn)
					iSecureSocket->RecvOneOrMore(aDesc,iStatus,iLen);
				else
					iSocket.RecvOneOrMore(aDesc,NULL,iStatus,iLen);
				}
			}
		}
	SetActive();
	}



//*********************************************************************************************/
//*                                                                                           */
//*                           Private logging/scripting/suspend functions                     */
//*                                                                                           */
//*********************************************************************************************/




#if defined(__IMSK_LOGGING)
//
// Attempt to create a log file to record messages sent to/from socket
//
void CImTextServerSession::CreateLogFile(TInt aPortNum)
	{
	//attempt to create a log file
	TRAP_IGNORE(iLog = CImLog::NewL(aPortNum));
	}
#endif

#if defined(__IMSK_SCRIPTING)
//
// See if a script file exists for the type of session we're perforiming
// if it doesn't really try to connect
// if it does exits then load the iap and bearer we should pretend to be connected to
void CImTextServerSession::OpenScriptFile(TInt aPortNum)
	{
	TRAP_IGNORE(iScript=CImTextServerScript::NewL(aPortNum));
	}

#endif

#if defined(__IMSK_SIMULATION)

// *************************************************************************
// GPRS Suspend code
// *************************************************************************
//
// Read next suspension time from configuration file gprs.cfg
// The tokens start:, duration: and repeat: are located and their
// values used to setup the next suspension period.
// NB, The tokens are assumed (must be) to be paired as follows:
//
//	start: nn
//	duration: nn
//	
//	or
//
//	repeat:
//	start: nn
//	duration: nn
//	
//	or
//
//	duration: nn	(where start is assumed to be 0.

void CImTextServerSession::ReadNextPeriod()
	{
	ResetSuspendPeriod();

    //  Read into the buffer
    TBuf8<80> buffer;

    //  Get the current file position
	TInt pos;
	TInt err;

    iGprsFile.Seek(ESeekStart, iCfgFilePos);

	// look for the tokens

	while (! SuspendPeriodSet())
		{
		err = iGprsFile.Read(buffer);
    
		if (err != KErrNone)
			{
			ResetSuspendPeriod();
			return;
			}

		// quit on eof
		if (buffer.Length() == 0)
			break;

		//  Copy to the lfcr and then set the file pointer
		//  to the point after that...
		pos = buffer.Find(KImCarriageReturn);
		if (pos != KErrNotFound)
			{
			iCfgFilePos += pos + 2;
			buffer.Delete(pos,buffer.Length());
			}
		else
			iCfgFilePos += buffer.Length();

		iGprsFile.Seek(ESeekStart, iCfgFilePos);  
		              
		// check the line read in
		buffer.TrimLeft();

		if (buffer.FindF(KRepeat) != KErrNotFound)
			{
			iRepeat = ETrue;
			}
		else if (buffer.FindF(KStart) != KErrNotFound)
			{
			iStart = GetTokenValue(KStart.iTypeLength, buffer);
			}
		else if (buffer.FindF(KDuration) != KErrNotFound)
			{
			iDuration = GetTokenValue(KDuration.iTypeLength, buffer);
			}
		}
	}



TInt CImTextServerSession::ReadConfigNum(const TDesC& aName)
	{
	RFile	configFile;
	TInt err = configFile.Open(iFs,aName,EFileShareAny);
	TInt toreturn=0;
	if(err==KErrNone)
		{
	    TBuf8<20> buffer;    // we ignore more than 20 chars in the file
		configFile.Read(buffer,20);
		configFile.Close();
		
		TLex8 lexical(buffer);
		lexical.Val(toreturn);
		}
	return toreturn;
	}



TUint32 CImTextServerSession::GetTokenValue(TInt aTokenLen, const TPtrC8& aBuffer)
	{
	TUint32 num;
	TInt i = aTokenLen;

	TBuf<70> value;
	value.Copy(aBuffer);

	// remove leading token, space and tabs
	while(i < value.Length())
		{
		if (value[i] != ' ' && value[i] != '\t')
			break;
		i++;
		}
	value.Delete(0,i);

	TLex lex(value);

	if (lex.Val(num,EDecimal) != KErrNone)
		num = 0;

	return num;
	}

// determine if we are in a suspension period based on the last 
// time one took place. If we have passed over the last period
// get the next period from the config file.
TBool CImTextServerSession::IsSuspended()
	{
	if (iGprsConfigExists && SuspendPeriodSet())
		{
		TTime now;
		now.UniversalTime();		

		// suspend period has not yet started
		if (now < iLastSuspend + iStart)
			return EFalse;
		
		// we are in the suspend period
		if (now < iLastSuspend + iStart + iDuration)
			return ETrue;

		// we have moved out of the period so get the next one
		// if not required to repeat the last
		if (! iRepeat)
			ReadNextPeriod();
	
		// reset baseline time/send buffer len if one set
		if (SuspendPeriodSet())
			{
			iLastSuspend.UniversalTime();

			// check for immediate start
			if (iStart.Int() == 0)
				return ETrue;
			}
		}

	return EFalse;
	}

// determine if a suspend period has been defined.
// start can be immediate.
TBool CImTextServerSession::SuspendPeriodSet()
	{
	return iStart.Int() >= 0 && iDuration.Int() != 0 ? ETrue : EFalse;
	}

void CImTextServerSession::ResetSuspendPeriod()
	{
	iStart = 0;
	iDuration = 0;
	iRepeat = EFalse;
	}


// calls DoRunL after the delay period has expired
// NB, delay is expected to be < KMinTInt32 secs
// used to queue async calls
void CImTextServerSession::SetAfterTimer()
	{

		{
		TBuf8<1024> buf;
		buf.Format(KLogGPRSSimSuspended,iDuration.Int());
		__LOG_COMMENT_OUT(buf);
		}
	TTime now;		

	now.UniversalTime();
	
	TTimeIntervalMicroSeconds usecs(TInt64(0));

	TTime delayEnd = iLastSuspend + iStart + iDuration;
	if (now <= delayEnd)
		usecs=delayEnd.MicroSecondsFrom(now);
	TInt64 delay=usecs.Int64()+500000;
	iSuspendTimer.After(iStatus,I64INT(delay));
	}


#endif	// __IMSK_SIMULATION



// Depreciated functions - do not use.
EXPORT_C void CImTextServerSession::QueueConnect(TRequestStatus& ,const TDesC& , TInt , TBool )
	{
	__ASSERT_ALWAYS(EFalse, gPanic(EImskNotSupported));
	}
EXPORT_C void CImTextServerSession::QueueConnect(TRequestStatus&,const TDesC&, TInt, TCallBack,const TUint32,TInt, TBool)
	{
	__ASSERT_ALWAYS(EFalse, gPanic(EImskNotSupported));
	}
EXPORT_C void CImTextServerSession::QueueConnect(TRequestStatus &,const TDesC& , TInt ,const TUint32 , TInt , TBool )
	{
	__ASSERT_ALWAYS(EFalse, gPanic(EImskNotSupported));
	}
EXPORT_C void CImTextServerSession::QueueConnect(TRequestStatus& ,const TDesC& , TInt , TCallBack , TBool )
	{
	__ASSERT_ALWAYS(EFalse, gPanic(EImskNotSupported));
	}
EXPORT_C void CImTextServerSession::QueueConnect(TRequestStatus& ,const TDesC& , TInt , TCallBack , const CImIAPPreferences& ,TInt , TBool )
	{
	__ASSERT_ALWAYS(EFalse, gPanic(EImskNotSupported));
	}
EXPORT_C void CImTextServerSession::QueueConnect(TRequestStatus &,const TDesC& , TInt , const CImIAPPreferences& ,TInt , TBool )
	{
	__ASSERT_ALWAYS(EFalse, gPanic(EImskNotSupported));
	}
EXPORT_C TInt CImTextServerSession::Send(const TDesC8& )
	{
    __ASSERT_ALWAYS(EFalse, gPanic(EImskNotSupported));
	return 0;
	}
EXPORT_C TInt CImTextServerSession::Send(TRefByValue<const TDesC8>,...)
	{
	__ASSERT_ALWAYS(EFalse, gPanic(EImskNotSupported));
	return 0;
	}
EXPORT_C TInt CImTextServerSession::SendReceive(const TDesC8& )
	{
	__ASSERT_ALWAYS(EFalse, gPanic(EImskNotSupported));
	return 0;
	}
EXPORT_C TInt CImTextServerSession::Receive(TDes8&)
	{
	__ASSERT_ALWAYS(EFalse, gPanic(EImskNotSupported));
	return 0;
	}
EXPORT_C void CImTextServerSession::Receive(TRequestStatus &, TDes8&)
	{
	__ASSERT_ALWAYS(EFalse, gPanic(EImskNotSupported));
	}

EXPORT_C CImTextServerSession *CImTextServerSession::NewLC (TImOperationMode, RSocketServ &)
	{
	__ASSERT_ALWAYS(EFalse, gPanic(EImskNotSupported));
	return 0;
	}
EXPORT_C CImTextServerSession *CImTextServerSession::NewL(RSocketServ &)
	{
	__ASSERT_ALWAYS(EFalse, gPanic(EImskNotSupported));
	return 0;
	}

EXPORT_C TInt CImTextServerSession::SetSecurity(TBool, TBool)
	{
	__ASSERT_ALWAYS(EFalse, gPanic(EImskNotSupported));
	return 0;
	}

/**
	Intended Usage	:	Gets the stage of the connection process obtained from RConnection
	@since			7.0s
	@return			The current connection stage from RConnection or a system-wide error code.

	*/
EXPORT_C TInt CImTextServerSession::GetConnectionStage()
	{
#if defined(__IMSK_SCRIPTING)
	if (iScript)
		{
		return KErrNotFound;
		}
	else
#endif //(__IMSK_SCRIPTING)
		{	
		TNifProgress progress;
		TInt err = iConnect->Progress(progress);
		return (err == KErrNone) ? (progress.iStage) : (err);
		}
	}

// Setting of PrimaryTextServerSession, Going to be set on the secondary session.
EXPORT_C void CImTextServerSession::SetPrimaryTextServerSession(CImTextServerSession* aPrimaryTextServerSession)
	{
	iPrimaryTextServerSession=aPrimaryTextServerSession;
	}

// Return of current Connection
CImConnect* CImTextServerSession::GetCImConnect()
	{
	return iConnect;
	}
