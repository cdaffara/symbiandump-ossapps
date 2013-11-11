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

#include "csecuresocketcontroller.h"
#include "mcommsinfoprovider.h"
#include "imappaniccodes.h"
#include <ssl_internal.h>

/**
The factory constructor.

@param	aSocket	Reference to the socket.
@param	aCommsInfoProvider	Reference to the comms info provider.
*/
CSecureSocketController* CSecureSocketController::NewL(RSocket& aSocket, MCommsInfoProvider& aCommsInfoProvider)
	{
	CSecureSocketController* self = new (ELeave) CSecureSocketController(aSocket, aCommsInfoProvider);
	return self;
	}

/**
Destructor.
*/
CSecureSocketController::~CSecureSocketController()
	{
	if( iTlsSocket )
		{
		iTlsSocket->Close();
		delete iTlsSocket;
		}
	else
		{
		iSocket.Close(); // Has ownership of socket if secure layer was not created
		}
	}

/**
Constructor.

@param	aSocket	Reference to the socket.
@param	aCommsInfoProvider	Reference to the comms info provider.	
*/
CSecureSocketController::CSecureSocketController(RSocket& aSocket, MCommsInfoProvider& aCommsInfoProvider)
: iSocket(aSocket), iCommsInfoProvider(aCommsInfoProvider)
	{
	}

/**
Used to start a tls handshake with the host.

@param	aStatus	The request status.
@param	aSSLDomainName SSL domain name 
*/
void CSecureSocketController::StartSecureHandshakeL(TRequestStatus& aStatus, const TDesC8& aSSLDomainName)
	{
	// Create the secure layer
	_LIT(KTxtTls, "tls1.0");
	if( !iTlsSocket )
		{
		iTlsSocket = CSecureSocket::NewL(iSocket, KTxtTls());
		}

	// Get the security preferences, dialog prompt and security policy
	TBool dialogPref;
	iCommsInfoProvider.SecurityPreferences(dialogPref);

	// Dialog preferences
	if( !dialogPref )
		{
		User::LeaveIfError(iTlsSocket->SetDialogMode(EDialogModeUnattended));
		}

	// Set an option on the socket to check the server certificate domain
	User::LeaveIfError(iTlsSocket->SetOpt(KSoSSLDomainName, KSolInetSSL, aSSLDomainName));
	
	iTlsSocket->StartClientHandshake(aStatus);
	}

/**
Receives data from a remote host and completes when data is available.
@param	aBuffer	A descriptor where data read will be placed.
@param	aStatus	On completion, KErrNone if successful, otherwise one of 
				the system wide error codes. Note that KErrEof indicates 
				either that a remote connection is closed, and that no more 
				data is available for reading, or the socket has been 
				shutdown with option RSocket::EStopInput. 
@param	aLength	For non-datagram sockets, on return, a length 
				which indicates how much data was read. This 
				is the same as length of the returned aDesc. 
				For datagram sockets, this parameter is not used.
*/
void CSecureSocketController::RecvOneOrMore(TDes8& aBuffer, TRequestStatus& aStatus, TSockXfrLength& aLength)
	{
	__ASSERT_DEBUG( iTlsSocket!=NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ETlsSocketNotStarted) );

	iTlsSocket->RecvOneOrMore(aBuffer, aStatus, aLength);
	}

/**
Cancels an outstanding Recv() operation.
*/
void CSecureSocketController::CancelRecv()
	{
	if( iTlsSocket )
		{
		iTlsSocket->CancelRecv();
		}
	}

/**
Sends data to a remote host on a connected socket with options set by protocol specific flags.
The length of the descriptor indicates the amount of data to be sent. The TSockXfrLength argument 
will return the amount of data actually sent.
A socket may only have one send operation in progress at any one time.
Send() should only be used with connected sockets.
If a protocol's information flag is marked with KSIUrgentData, then KSockWriteUrgent may be provided 
as a flag to Send(). All other flags are protocol specific.

@param	aBuffer	Reference to the data to send.
@param	aStatus	The request status.
*/
void CSecureSocketController::Send(const TDesC8& aBuffer, TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG( iTlsSocket!=NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ETlsSocketNotStarted) );
	
	iTlsSocket->Send(aBuffer, aStatus);
	}

/**
Cancels an outstanding Send() operation. Calling the function will cause any outstanding 
send operation to complete prematurely. The state of a socket after a send is cancelled 
is defined by the characteristics of the protocol.
*/
void CSecureSocketController::CancelSend()
	{
	if( iTlsSocket )
		{
		iTlsSocket->CancelSend();
		}
	}

/**
Cancels the handshake.
*/
void CSecureSocketController::CancelHandshake()
	{
	if( iTlsSocket )
		{
		iTlsSocket->CancelHandshake();
		}
	}
