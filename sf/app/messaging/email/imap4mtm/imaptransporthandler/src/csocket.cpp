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

#include "csocket.h"
#include "csecuresocketcontroller.h"
#include "mcommsinfoprovider.h"
#include "imappaniccodes.h"

// Flags to pass to protocol in RecvOneOrMore call for socket
const TInt KDefaultFlags = 0;

/**
The factory constructor.

@param		aCommsInfoProvider	The comms info provider object.
@param		aSocketType			The type of the socket.
@return		A pointer to a fully constructed object.
*/
CSocket* CSocket::NewL(MCommsInfoProvider& aCommsInfoProvider, TSocketType aSocketType)
	{
	CSocket* self = new (ELeave) CSocket(aCommsInfoProvider);
	CleanupStack::PushL(self);
	self->ConstructL(aSocketType);
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.
*/
CSocket::~CSocket()
	{
	// Close socket and notify the socket status observer
	if( iSecureSocketController )
		{
		delete iSecureSocketController;
		}
	else
		{
		iSocket.Close();
		}
	}

/**
Constructor.

@param	aCommsInfoProvider	The comms info provider object.
*/
CSocket::CSocket(MCommsInfoProvider& aCommsInfoProvider)
	: iCommsInfoProvider(aCommsInfoProvider)
	{
	}

/**
Second phase constructor.

@param		aSocketType	The type of the socket.
*/
void CSocket::ConstructL(TSocketType aSocketType)
	{
	switch( aSocketType )
		{
	case EProtocolSocket:
		{
		// Open a protocol socket
		User::LeaveIfError(iSocket.Open(
									iCommsInfoProvider.SocketServer(),
									iCommsInfoProvider.ProtocolDescription().iAddrFamily, 
									iCommsInfoProvider.ProtocolDescription().iSockType, 
									iCommsInfoProvider.ProtocolDescription().iProtocol,
									iCommsInfoProvider.Connection()
									));
		} break;
	case EBlankSocket:
		{
		// Open a blank socket
		User::LeaveIfError(iSocket.Open(iCommsInfoProvider.SocketServer()));
		} break;
	default:
		{
		User::Invariant();
		} break;
		}
	}

/**
Start asynchronous connect service. The address contains the IP address and
port with which a tcp connection should be established with. The request 
status is completed either when a connection has been established or an error
has occurred - this is reflected in the value of the request status.

@param	aAddr	The IP address and port of the remote host.
@param	aStatus	The request status that is completed when the connect 
		service completes.
*/
void CSocket::Connect(TInetAddr& aAddr, TRequestStatus& aStatus)
	{
	iSocket.Connect(aAddr, aStatus);
	}

/**
Cancel the connect service.
*/
void CSocket::CancelConnect()
	{
	iSocket.CancelConnect();
	}

/**
Receive data from socket asynchronously. Any data received by the socket is 
placed in the buffer supplied by aBuffer. The request status is completed 
either when data has been received or an error has occurred - this is 
reflected in the value of the request status.

@param		aBuffer	The buffer where any received data is placed.	
@param		aStatus	The request status that is completed when the receive 
					service completes.
*/
void CSocket::RecvOneOrMore(TDes8& aBuffer, TRequestStatus& aStatus)
	{
	if( iSecureSocketController )
		{
		// aFlags not used for secure sockets
		iSecureSocketController->RecvOneOrMore(aBuffer, aStatus, iBytesReceived);
		}
	else
		{
		iSocket.RecvOneOrMore(aBuffer, KDefaultFlags, aStatus, iBytesReceived);
		}
	}

/**
Cancel the receive service.
*/
void CSocket::CancelRecv()
	{
	if( iSecureSocketController )
		{
		iSecureSocketController->CancelRecv();
		}
	else
		{
		iSocket.CancelRecv();
		}
	}

/**
Send data to the socket asynchronously. The data in the supplied buffer is 
sent to the socket. The request status is completed either when data has 
been sent or an error has occurred - this is reflected in the value of the
request status.

@param		aBuffer	The buffer containing the data to be sent.
@param		aStatus	The request status that is completed when the send 
					service completes.
*/
void CSocket::Send(const TDesC8& aBuffer, TRequestStatus& aStatus)
	{
	if( iSecureSocketController )
		{
		iSecureSocketController->Send(aBuffer, aStatus);
		}
	else
		{
		iSocket.Write(aBuffer, aStatus);
		}
	}

/**
Cancel the send service.
*/
void CSocket::CancelSend()
	{
	if( iSecureSocketController )
		{
		iSecureSocketController->CancelSend();
		}
	else
		{
		iSocket.CancelWrite();
		}
	}

/**
Get the remote host name. The IP address and port of the remote host is set
in the output argument.

@param	aAddr	The output argument where the IP address and port of the
				remote host is placed.
*/
void CSocket::RemoteName(TInetAddr& aAddr)
	{
	iSocket.RemoteName(aAddr);
	}

/**
Get the local socket name. The IP address and port of the local socket is 
set in the output argument.

@param	aAddr	The output argument where the IP address and port of the
				local socket is placed.
*/
void CSocket::LocalName(TInetAddr& aAddr)
	{
	iSocket.LocalName(aAddr);
	}

/**
Upgrade the socket connection to a secure connection.

@param	aStatus		The request status.
@param	aSSLDomainName SSL domain name.
*/
void CSocket::UpgradeToSecureL(TRequestStatus& aStatus, const TDesC8& aSSLDomainName)
	{
	// Check that the socket is already a secure one.
	if( iSecureSocketController )
		{
		// The socket connection is already secure, simply complete the request
		aStatus=KRequestPending;
		TRequestStatus* pStat = &aStatus;
		User::RequestComplete(pStat, KErrNone);
		}
	else
		{
		// Create the secure socket controller and start the secure handshake
		iSecureSocketController = CSecureSocketController::NewL(iSocket, iCommsInfoProvider);
		iSecureSocketController->StartSecureHandshakeL(aStatus, aSSLDomainName);
		}
	}

/**
Cancel the upgrade to a secure socket.
*/
void CSocket::CancelUpgradeToSecure()
	{
	if( iSecureSocketController )
		{
		iSecureSocketController->CancelHandshake();
		}
	}
