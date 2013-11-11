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

#include "csocketreader.h"

#include "csocket.h"
#include "msocketcontroller.h"
#include "minputstreamobserver.h"
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
CSocketReader* CSocketReader::NewL(CSocket& aSocket, MSocketController& aController)
	{
	 CSocketReader* self = new(ELeave) CSocketReader(aSocket, aController);
	 CleanupStack::PushL(self);
	 self->ConstructL();
	 CleanupStack::Pop(self);
	 return self;
	}

void CSocketReader::ConstructL()
	{
	iTimer = CImapObservableTimer::NewL(*this);
	}	
/**
Destructor.
*/
CSocketReader::~CSocketReader()
	{
	__ASSERT_DEBUG( iObserver == NULL || iState == EClosed, TImapServerPanic::ImapPanic(TImapServerPanic::EBadInputStreamState) );

	// Cancel any outstanding requests
	Cancel();
	delete iTimer;
	}

/**
Constructor.

@param	aSocket		The connected socket. This owned by the observer.
@param	aController	The socket controller that owns the socket.
*/
CSocketReader::CSocketReader(CSocket& aSocket, MSocketController& aController)
	: CActive(CActive::EPriorityStandard), iSocket(aSocket), iController(aController)
	{
	CActiveScheduler::Add(this);
	}

/**
Requests that the input stream complete itself. This will caused the RunL()
to be called by the scheduler at the next opportunity.

@pre	The input stream is not active.
@post	The input stream object is active and the request has been
		completed.
*/
void CSocketReader::CompleteSelf()
	{
	__LOG_TEXT(iLogId, "CSocketReader::CompleteSelf");
	TRequestStatus* pStat = &iStatus;
	User::RequestComplete(pStat, KErrNone);
	SetActive();
	}

/**
Notifies the input stream that the socket is closed. The input stream 
observer is notified that the stream is closed. No more data can be received
from the socket.

@param	aError	The error code explaining why the socket has closed. A
				value of KErrNone indicates that the output stream 
				observer requested that the socket be closed.
@pre	None.
@post	The input stream is in the Closed state.
*/
void CSocketReader::SocketClosed(TInt aError)
	{
	// Cancel any outstanding requests
	Cancel();

	// The socket has shutdown. Inform the input stream observer that the input
	// stream is closed.
	if( iObserver )
		{
		iObserver->InputStreamCloseInd(aError);
		}		

	// Move to the Closed state
	SetState(EClosed);
	}

/**
Notifies the input stream that the socket is to be suspended.
*/
void CSocketReader::Suspend()
	{
	__ASSERT_DEBUG( !iSuspended, TImapServerPanic::ImapPanic(TImapServerPanic::EInputStreamAlreadySuspended) );

	// Cancel any outstanding requests
	Cancel();

#if defined (_DEBUG) && defined (_LOGGING)
	TBuf8<KIpv6MaxAddrSize> ip;
	TUint16	remotePort;
	TUint16 localPort;
	iController.ConnectionInfo(ip, remotePort, localPort);

	__FLOG_0(_T8("!! Suspending input stream"));
	__FLOG_3(_T8("-> on local port %d with %S, remote port %d"), localPort, &ip, remotePort);
#endif

	// Check to see a read requests had been issued.
	if( iState == EReceivedData )
		{
		// Yep - change to the Read state so that read request is re-issued when
		// the stream resumes.
		SetState(ERead);
		}

	// Stream is now suspended.
	iSuspended = ETrue;
	}

/**
Notifies the input stream that the socket is to be resumed.
*/
void CSocketReader::Resume()
	{
	__ASSERT_DEBUG( iSuspended, TImapServerPanic::ImapPanic(TImapServerPanic::EInputStreamNotSuspended) );

#if defined (_DEBUG) && defined (_LOGGING)
	TBuf8<KIpv6MaxAddrSize> ip;
	TUint16	remotePort;
	TUint16 localPort;
	iController.ConnectionInfo(ip, remotePort, localPort);

	__FLOG_0(_T8("!! Resumimng input stream"));
	__FLOG_3(_T8("-> on local port %d with %S, remote port %d"), localPort, &ip, remotePort);
#endif

	// Check to see a read requests needs to be issued.
	if( iState == ERead )
		{
		if (!iBufferDataDelivered && iBuffer.Length() > 0)
			{
			// We have undelivered data in the buffer (probably caused by the Cancel() in Suspend())
			// Deliver it now - rather than requesting more.
			SetState(EReceivedData);
			}		
		
		// Self complete to get the read request issued or to deliver the undelivered data.
		CompleteSelf();
		}

	// Stream is no longer suspended.
	iSuspended = EFalse;
	}

/**
@see MInputStream
*/
void CSocketReader::Bind(MInputStreamObserver& aObserver, TInt aLogId)
	{
	__ASSERT_DEBUG( iState == EIdle, TImapServerPanic::ImapPanic(TImapServerPanic::EBadInputStreamState) );

	// Bind to the input stream observer
	iObserver = &aObserver;
	
	iLogId = aLogId;
	}

/**
@see MInputStream
*/
void CSocketReader::ReadReq(TInt aIdleTime)
	{
	// Do we have an observer to report the incoming data to?
	__ASSERT_DEBUG( iObserver != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EInputStreamNotBound) );
	// Are we already reading?
	__ASSERT_DEBUG( iState == EIdle, TImapServerPanic::ImapPanic(TImapServerPanic::EBadInputStreamState) );
	if (aIdleTime > 0)
		{
		iIdleTime = aIdleTime;
		}
	
	// The input stream observer has requested some data.
	// Check whether the data in the buffer has already been delivered to an observer
	if (iBufferDataDelivered)
		{
		// We don't want to deliver it twice, so zero the buffer.
		iBuffer.Zero();
		}

	if (iSuspended)
		{
		// Set the start to read so that data will be requested/delivered when Resume() is called.
		SetState(ERead);
		}
	else
		{
		// If we still have data left to deliver, then deliver it now.
		// This can happen after a CancelRecv(), as the buffer will have been populated with data received 
		// at the time of cancel, but will not yet have been delivered.
		if (iBuffer.Length() > 0)
			{
			__LOG_TEXT(iLogId, "CSocketReader - Delivering data received prior to cancel");
			SetState(EReceivedData);
			}
		// Otherwise read some more from the socket
		else
			{
			SetState(ERead);
			}
			
		CompleteSelf();
		}	
	}
	
/**
@see MInputStream
*/
TBool CSocketReader::IsReading()
	{
	return (iState == ERead || iState == EReceivedData);
	}

/**
@see MInputStream
*/
void CSocketReader::CancelReadReq()
	{
	__ASSERT_DEBUG( iState == EIdle || iState == ERead || iState == EReceivedData, TImapServerPanic::ImapPanic(TImapServerPanic::EBadInputStreamState) );
	
	iTimer->Cancel();
	Cancel();	
	
	SetState(EIdle);
	}

/*
 *	Methods from CActive
 */

/**
The request servicing function. Behaviour depends on the state of the input
stream.

@leave	The function will leave if the request status is an error.
*/
void CSocketReader::RunL()
	{
	__ASSERT_DEBUG( iObserver != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EInputStreamNotBound) );

	__LOG_FORMAT((iLogId, "CSocketReader::RunL - State %d, Status %d", iState, iStatus.Int()));

	iTimer->Cancel();
	// Leave if the socket reported an error
	User::LeaveIfError(iStatus.Int());

	switch( iState )
		{
	case ERead:
		{
		// Request a read from the socket
		__ASSERT_DEBUG( iBuffer.Length() == 0, TImapServerPanic::ImapPanic(TImapServerPanic::EInputStreamBufferNotEmpty) );
		iBufferDataDelivered = EFalse;
		iSocket.RecvOneOrMore(iBuffer, iStatus);

		// Move to the Received state and go active - wait for read to complete
		SetState(EReceivedData);
		SetActive();		
		if(iIdleTime > 0)
			{
			// The recieve from socket should complete in iIdleTime seconds.
			iTimer->After(iIdleTime*KMicroSecondsToSeconds);
			iIdleTime = 0;
			}
		} break;
	case EReceivedData:
		{
		// Received data - move to the PendingAck state and wait for observer to 
		// notify that it has finished with the buffer.
		SetState(EIdle);

		__LOG_DATA_IN(iLogId, iBuffer);

		// Inform the observer of the received data.
		iBufferDataDelivered = ETrue;
		iObserver->ReceivedDataIndL(iBuffer);

#if defined (_DEBUG) && defined (_LOGGING)
		TBuf8<KIpv6MaxAddrSize> ip;
		TUint16	remotePort;
		TUint16 localPort;
		iController.ConnectionInfo(ip, remotePort, localPort);
	
		__FLOG_4(_T8("Received %d bytes on local port %d from %S, remote port %d"), iBuffer.Length(), localPort, &ip, remotePort);
		__FLOG_0(_T8("----------"));
		__FLOG_HEXDUMP(iBuffer.Ptr(), iBuffer.Length());
		__FLOG_0(_T8("----------"));
#endif
		} break;
	case EClosing:
		{
		// The socket has shutdown - move to the Closed state
		SetState(EClosed);

		// Inform the observer that the input stream is closed.
		iObserver->InputStreamCloseInd(KErrNone);

		// Inform the socket controller that the input stream is now shut
		iController.StreamClosed(KErrNone, MSocketController::EInputStream);
		} break;
	case EClosed:
	case EIdle:
	default:
		TImapServerPanic::ImapPanic(TImapServerPanic::EBadInputStreamError);
		break;
		}
	}

/**
The asynchronous request cancel function. If the input stream has requested a 
read from the socket (ie it is in the ReceivedData state) it cancels the read
request.
*/	
void CSocketReader::DoCancel()
	{
	iTimer->Cancel();
	if( iState == EReceivedData )
		{
		// There is a pending read request - cancel it
		iSocket.CancelRecv();
		}
	}

/**
The asynchronous request error handler. If this has been called then the 
read request or the socket shutdown request has failed. The socket can no 
longer be used. The input stream observer is notified that the stream is 
closed. No more data can be received from the socket.
*/	
TInt CSocketReader::RunError(TInt aError)
	{
	CloseStreams(aError);

	return KErrNone;
	}

void CSocketReader::SetState(TInputState aInputState)
	{
#ifdef __CSOCKETREADER_VERBOSE_LOGGING
#ifdef __IMAP_LOGGING

	_LIT8(KTxtInputStateIdle, "EIdle");
	_LIT8(KTxtInputStateRead, "ERead");
	_LIT8(KTxtInputStateReceivedData, "EReceivedData");
	_LIT8(KTxtInputStateClosing, "EClosing");
	_LIT8(KTxtInputStateClosed, "EClosed");
	_LIT8(KTxtInputStateUnknown, "Unknown");
				
	TPtrC8 ptrOldState(KTxtInputStateUnknown);
	TPtrC8 ptrNewState(KTxtInputStateUnknown);
	
	switch(iState)
		{
		case EIdle: 		ptrOldState.Set(KTxtInputStateIdle); 			break;
		case ERead:			ptrOldState.Set(KTxtInputStateRead); 			break;
		case EReceivedData:	ptrOldState.Set(KTxtInputStateReceivedData);	break;
		case EClosing:		ptrOldState.Set(KTxtInputStateClosing); 		break;
		case EClosed:		ptrOldState.Set(KTxtInputStateClosed); 			break;
		}
		
	switch(aInputState)
		{
		case EIdle: 		ptrNewState.Set(KTxtInputStateIdle); 			break;
		case ERead:			ptrNewState.Set(KTxtInputStateRead); 			break;
		case EReceivedData:	ptrNewState.Set(KTxtInputStateReceivedData);	break;
		case EClosing:		ptrNewState.Set(KTxtInputStateClosing); 		break;
		case EClosed:		ptrNewState.Set(KTxtInputStateClosed); 			break;
		}
	
	_LIT8(KLogFormat, "CSocketReader::iState %S ==> %S");
	__LOG_FORMAT((iLogId, KLogFormat, &ptrOldState, &ptrNewState));
	
#endif //__IMAP_LOGGING
#endif //__CSOCKETREADER_VERBOSE_LOGGING
	
	iState = aInputState;
	}
void CSocketReader::CloseStreams(TInt aError)
	{
	__ASSERT_DEBUG( iObserver != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EInputStreamNotBound) );

#if defined (_DEBUG) && defined (_LOGGING)
	TBuf8<KIpv6MaxAddrSize> ip;
	TUint16	remotePort;
	TUint16 localPort;
	iController.ConnectionInfo(ip, remotePort, localPort);

	__FLOG_1(_T8("!! Input stream error : %d"), aError);
	__FLOG_3(_T8("-> Connection on local port %d with %S, remote port %d closed"), localPort, &ip, remotePort);
#endif

	// Move to the Closed state
	SetState(EClosed);

	// The socket request has failed - the socket connection is broken. Need
	// to inform the input stream observer that the input stream is closed.
	
	if (aError == KErrAbort || aError == KErrEof )
		{
		aError = KErrDisconnected;
		}
	iObserver->InputStreamCloseInd(aError);

	// Inform the socket controller that the input stream is closed
	iController.StreamClosed(aError, MSocketController::EInputStream);
	}


#ifdef _DEBUG
void CSocketReader::OnTimerL(const CImapObservableTimer& aSourceTimer)
#else
void CSocketReader::OnTimerL(const CImapObservableTimer& /*aSourceTimer*/)
#endif
	{
	__ASSERT_DEBUG(&aSourceTimer == iTimer, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionUnknownTimer));
	Cancel();
	CloseStreams(KErrTimedOut);
	}
