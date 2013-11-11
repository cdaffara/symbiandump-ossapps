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

#include "csocketwriter.h"

#include "csocket.h"
#include "msocketcontroller.h"
#include "moutputstreamobserver.h"
#include "moutputstreamsecureobserver.h"
#include "imappaniccodes.h"
#include "imaptransportmanagercommon.h"
#include "cimaplogger.h"

const TInt KMicroSecondsToSeconds = 1000000;
/**
The factory constructor.

@param		aSocket		The connected socket. This owned by the observer.
@param		aController	The socket controller that owns the socket.
@return		A pointer to a fully constructed object.
*/
CSocketWriter* CSocketWriter::NewL(CSocket& aSocket, MSocketController& aController)
	{
	CSocketWriter* self = new (ELeave) CSocketWriter(aSocket, aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CSocketWriter::ConstructL()
	{
	iTimer = CImapObservableTimer::NewL(*this);
	}

/**
Destructor.
*/
CSocketWriter::~CSocketWriter()
	{
	__ASSERT_DEBUG( iState == EIdle || iState == EClosed, TImapServerPanic::ImapPanic(TImapServerPanic::EBadOutputStreamState) );

	// Cancel any outstanding requests
	Cancel();
	delete iTimer;
	}

/**
Constructor.

@param	aSocket		The connected socket. This owned by the observer.
@param	aController	The socket controller that owns the socket.
*/
CSocketWriter::CSocketWriter(CSocket& aSocket, MSocketController& aController)
	: CActive(CActive::EPriorityStandard), iSocket(aSocket), iController(aController)
	{
	CActiveScheduler::Add(this);
	}

/**
Notifies the output stream that the socket is closed. The output stream 
observer is notified that the stream is closed. No more data can be sent to
the socket.

@param	aError	The error code explaining why the socket has closed. A
				value of KErrNone indicates that the input stream 
				observer requested that the socket be closed.
@pre	None.
@post	The output stream is in the Closed state.
*/
void CSocketWriter::SocketClosed(TInt aError)
	{
	// Cancel any outstanding requests
	Cancel();

	// The socket has shutdown. Inform the output stream observer that the 
	// output stream is closed.
	if( iObserver )
		iObserver->OutputStreamCloseInd(aError);

	// Move to the Closed state
	iState = EClosed;
	}

/*
 *	Methods from MOutputStream
 */

/**
@see MOutputStream
*/
void CSocketWriter::Bind(MOutputStreamObserver& aObserver, TInt aLogId)
	{
	__ASSERT_DEBUG( iState == EIdle || iState == EPendingSend, TImapServerPanic::ImapPanic(TImapServerPanic::EBadOutputStreamState) );

	// Bind to the output stream observer
	iObserver = &aObserver;

	iLogId = aLogId;

	// Move to the PendingSend state
	iState = EPendingSend;
	}

void CSocketWriter::BindSecure(MOutputStreamSecureObserver& aObserver)
	{
	__ASSERT_DEBUG( iState == EIdle || iState == EPendingSend, TImapServerPanic::ImapPanic(TImapServerPanic::EBadOutputStreamState) );

	// Bind to the output stream observer
	iSecureObserver = &aObserver;
	}

/**
@see MOutputStream
*/
void CSocketWriter::SendDataReq(const TDesC8& aBuffer, TInt aIdleTime)
	{
	
	if(iState == ESentData)
		return;

	__ASSERT_DEBUG( iObserver != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EOutputStreamNotBound) );
	__ASSERT_DEBUG( iState == EPendingSend, TImapServerPanic::ImapPanic(TImapServerPanic::EBadOutputStreamState) );

	__LOG_DATA_OUT(iLogId, aBuffer);

	// The output stream observer requests that the supplied buffer is sent to
	// the socket. Request a write to the socket
	iSocket.Send(aBuffer, iStatus);

	// Start the timer, send request must complete within aIdleTime, else there 
	// is a problem with the socket connection.
	if(aIdleTime > 0)
		{
		iTimer->After(aIdleTime*KMicroSecondsToSeconds);
		}
#if defined (_DEBUG) && defined (_LOGGING)
	TBuf8<KIpv6MaxAddrSize> ip;
	TUint16	remotePort;
	TUint16 localPort;
	iController.ConnectionInfo(ip, remotePort, localPort);

	__FLOG_4(_T8("Sending %d bytes on local port %d to %S, remote port %d"), aBuffer.Length(), localPort, &ip, remotePort);
	__FLOG_0(_T8("----------"));
	__FLOG_HEXDUMP(aBuffer.Ptr(), aBuffer.Length());
	__FLOG_0(_T8("----------"));
#endif

	// Move to the SentData and go active
	iState = ESentData;
	SetActive();
	}

/**
@see MOutputStream
*/
void CSocketWriter::SecureClientReq(const TDesC8& aSSLDomainName)
	{
	__ASSERT_DEBUG( iSecureObserver != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EOutputStreamNotBoundSecure) );
	__ASSERT_DEBUG( iState == EPendingSend, TImapServerPanic::ImapPanic(TImapServerPanic::EBadOutputStreamState) );

#if defined (_DEBUG) && defined (_LOGGING)
	TBuf8<KIpv6MaxAddrSize> ip;
	TUint16	remotePort;
	TUint16 localPort;
	iController.ConnectionInfo(ip, remotePort, localPort);

	__FLOG_0(_T8("!! Upgrading to secure (client) connection"));
	__FLOG_3(_T8("-> on local port %d with %S, remote port %d"), localPort, &ip, remotePort);
#endif

	// Notify the controller to suspend the input stream.
	iController.StreamSuspended(MSocketController::EOutputStream);
	
	// Store the host name as required for secure certificate domain name 
	// checking. Move to the StartSecureHandshake state and self complete.
	iSSLDomainName.Set(aSSLDomainName);
	iState = EStartSecureHandshake;
	CompleteSelf();
	}

/**
@see MOutputStream
*/
void CSocketWriter::Close()
	{
	__ASSERT_DEBUG( iObserver != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EOutputStreamNotBound) );
	__ASSERT_DEBUG( iState != EClosing || iState != EClosed, TImapServerPanic::ImapPanic(TImapServerPanic::EBadOutputStreamState) );

	// Cancel any outstanding requests
	Cancel();

#if defined (_DEBUG) && defined (_LOGGING)
	TBuf8<KIpv6MaxAddrSize> ip;
	TUint16	remotePort;
	TUint16 localPort;
	iController.ConnectionInfo(ip, remotePort, localPort);

	__FLOG_0(_T8("!! Closing output stream"));
	__FLOG_3(_T8("-> on local port %d with %S, remote port %d"), localPort, &ip, remotePort);
#endif

	// There is no need to do anything here - by informing the socket controller
	// that the output stream is closed it will notify the input stream and then
	// close the socket.

	// Move to the Closed state
	iState = EClosed;

	// Inform the socket controller that the output stream is closed
	iController.StreamClosed(KErrCancel, MSocketController::EOutputStream);
	}

/**
Completes this active object.
*/
void CSocketWriter::CompleteSelf()
	{
	TRequestStatus* pStat = &iStatus;
	User::RequestComplete(pStat, KErrNone);
	SetActive();
	}

/*
 * Methods from CActive
 */

/**
The request servicing function. Behaviour depends on the state of the output
stream.

@leave	The function will leave if the request status is an error.
*/
void CSocketWriter::RunL()
	{
	// Leave if the socket reported an error
	User::LeaveIfError(iStatus.Int());

	switch( iState )
		{
	case ESentData:
		{
		__ASSERT_DEBUG( iObserver != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EOutputStreamNotBound) );
		// Send complete without error so cancel the idle timer.
		iTimer->Cancel();

		// The data has successfully been written to the socket - move to the 
		// PendingSend state. 
		iState = EPendingSend;

		// Inform the observer that the data has been sent.
		iObserver->SendDataCnf();
		} break;
	case EClosing:
		{
		__ASSERT_DEBUG( iObserver != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EOutputStreamNotBound) );

		// The socket has shutdown - move to the Closed state
		iState = EClosed;

		// Inform the observer that the output stream is closed.
		iObserver->OutputStreamCloseInd(KErrNone);

		// Inform the socket controller that the output stream is closed
		iController.StreamClosed(KErrNone, MSocketController::EOutputStream);
		} break;
	case EStartSecureHandshake:
		{
		// Start the secure handshake
		iState = ESecureHandshakeComplete;
		iSocket.UpgradeToSecureL(iStatus, iSSLDomainName);
		SetActive();
		} break;
	case ESecureHandshakeComplete:
		{
		__ASSERT_DEBUG( iSecureObserver != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EOutputStreamNotBoundSecure) );

		// Secure handshake has completed successfully so inform the observer.
		iState = EPendingSend;

		iSecureObserver->SecureClientCnf(KErrNone);

#if defined (_DEBUG) && defined (_LOGGING)
		TBuf8<KIpv6MaxAddrSize> ip;
		TUint16	remotePort;
		TUint16 localPort;
		iController.ConnectionInfo(ip, remotePort, localPort);

		__FLOG_0(_T8("!! Upgrade to secure (client) connection successful"));
		__FLOG_3(_T8("-> on local port %d with %S, remote port %d"), localPort, &ip, remotePort);
#endif

		// Notify the controller to resume the input stream.
		iController.StreamResumed(MSocketController::EOutputStream);
		} break;
	case EPendingSend:
	case EClosed:
	case EIdle:
	default:
		TImapServerPanic::ImapPanic(TImapServerPanic::EBadOutputStreamState);
		break;
		}
	}

/**
The asynchronous request cancel function. If the output stream has requested
a write to the socket (ie it is in the SentData state) this function cancels
the write request.
*/
void CSocketWriter::DoCancel()
	{
	iTimer->Cancel();
	if( iState == ESentData )
		{
		// There is a pending write request - cancel it
		iSocket.CancelSend();
		}
	else if( iState == ESecureHandshakeComplete )
		{
		// Cancel the upgrade to secure connection.
		iSocket.CancelUpgradeToSecure();
		}
	}

/**
The error handler for when RunL() leaves. If this has been called then the 
write request or the socket shutdown request has failed. The socket can no 
longer be used. The output stream observer is notified that the stream is 
closed. No more data can be sent to the socket.

@return		A value of KErrNone indicating that the the error has been 
			handled.
*/
TInt CSocketWriter::RunError(TInt aError)
	{
	CloseStreams(aError);
	return KErrNone;
	}
#ifdef _DEBUG
void CSocketWriter::OnTimerL(const CImapObservableTimer& aSourceTimer)
#else
void CSocketWriter::OnTimerL(const CImapObservableTimer& /*aSourceTimer*/)
#endif
	{
	__ASSERT_DEBUG(&aSourceTimer == iTimer, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionUnknownTimer));
	Cancel();
	CloseStreams(KErrTimedOut);
	}
	
	
void CSocketWriter::CloseStreams(TInt aError)
	{
#if defined (_DEBUG) && defined (_LOGGING)
	TBuf8<KIpv6MaxAddrSize> ip;
	TUint16	remotePort;
	TUint16 localPort;
	iController.ConnectionInfo(ip, remotePort, localPort);

	__FLOG_1(_T8("!! Output stream error : %d"), aError);
	__FLOG_3(_T8("-> Connection on local port %d with %S, remote port %d closed"), localPort, &ip, remotePort);
#endif

	if (iState == ESecureHandshakeComplete)
		{
		__ASSERT_DEBUG( iSecureObserver != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EOutputStreamNotBoundSecure) );

		// The request to upgrade to a secure socket has failed. Inform the output
		// stream secure observer of the error.
		
		if (aError == KErrEof )
			{
			aError = KErrDisconnected;
			}
			
		iSecureObserver->SecureClientCnf(aError);

		// If an observer has bound, tell it that the stream is closed.
		// It is possible that the secure socket will be setup before anyone tries
		// to bind an output stream observer
		if (iObserver != NULL)
			{
			iObserver->OutputStreamCloseInd(aError);
			}
		}
	else
		{
		__ASSERT_DEBUG( iObserver != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EOutputStreamNotBound) );

		// The socket request has failed - the socket connection is broken. Need
		// to inform the output stream observer that the output stream is closed.

		iObserver->OutputStreamCloseInd(aError);
		}

	// Move to the Closed state
	iState = EClosed;

	// Inform the socket controller that the output stream is closed
	iController.StreamClosed(aError, MSocketController::EOutputStream);
	}
