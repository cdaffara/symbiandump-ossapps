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

#include "csocketconnector.h"

#include "csocket.h"
#include "msocketconnectobserver.h"
#include "msocketconnectorstore.h"
#include "msocketcontrollerfactory.h"
#include "mcommsinfoprovider.h"
#include "csocketcontroller.h"
#include "imappaniccodes.h"
#include "imaptransportmanagercommon.h"

#if defined (_DEBUG) && defined (_LOGGING)
const TInt KHostNameSize	= 126;
#endif

/**
The factory constructor.

@param 	aStore 						The socket connector store.
@param 	aSocketControllerFactory		The socket controller factory.
@param 	aCommsInfoProvider			The comms info provider.
@return	A pointer to a fully constructed object.	
*/
CSocketConnector* CSocketConnector::NewL(MSocketConnectorStore&	aStore, 
                                         MSocketControllerFactory& aSocketControllerFactory, 
                                         MCommsInfoProvider& aCommsInfoProvider)
	{
	return new (ELeave) CSocketConnector(aStore, aSocketControllerFactory, aCommsInfoProvider);
	}

/**
Destructor.
*/
CSocketConnector::~CSocketConnector()
	{
	// Cancel any outstanding requests
	Cancel();

	// Cleanup...
	delete iHost;
	delete iConnectingSocket;
	iHostResolver.Close();
	}

/**
Constructor.

@param	aStore						The socket connector store.
@param	aSocketControllerFactory	The socket controller factory.
@param	aCommsInfoProvider			The comms info provider.
*/
CSocketConnector::CSocketConnector(MSocketConnectorStore& aStore,
								   MSocketControllerFactory& aSocketControllerFactory,
								   MCommsInfoProvider& aCommsInfoProvider) : CActive(CActive::EPriorityStandard), iStore(aStore),
									   										 iSocketControllerFactory(aSocketControllerFactory),
									   										 iCommsInfoProvider(aCommsInfoProvider)
	{
	CActiveScheduler::Add(this);
	}

/**
Start connection to specified remote host. The socket connector starts 
connecting to the specified remote host on the specified port.

@param	aRemoteHost		The host name/IP address of the remote host
@param	aRemotePort		The port number of the remote host
@pre	The socket connector is in the Idle state.
@post	The socket connector is not in the Idle state.
*/
void CSocketConnector::ConnectL(MSocketConnectObserver& aObserver, const TDesC8& aRemoteHost, TUint16 aRemotePort)
	{
	__ASSERT_DEBUG( iState == EIdle, TImapServerPanic::ImapPanic(TImapServerPanic::EBadSocketConnectorState) );

	// Set the observer
	iObserver = &aObserver;

	__ASSERT_DEBUG(!iHost, User::Invariant());

	// Copy the remote host IP address and port
	iHost = HBufC::NewL(aRemoteHost.Length());
	iHost->Des().Copy(aRemoteHost);
	iPort = aRemotePort;

	// Move to the PendingDNSLookup state and self complete.
	iState = EPendingDNSLookup;
	CompleteSelf();
	}

/**
Requests that the socket connector complete itself. This will caused the 
RunL() to be called by the scheduler at the next opportunity.

@pre	The socket connector object is not active.
@post	The socket connector object is active and the request has been
		completed.
*/
void CSocketConnector::CompleteSelf()
	{
	TRequestStatus* pStat = &iStatus;
	User::RequestComplete(pStat, KErrNone);
	SetActive();
	}

/**
Commit a suicide on the socket connector.
*/
void CSocketConnector::Suicide()
	{
	// Inform store that connection is complete and then suicide.
	iStore.ConnectionCompleted(*this);
	delete this;
	}

/**
Stop the connect.

@see MSocketConnector
*/
void CSocketConnector::StopConnect()
	{
	// Cancel any outstanding requests.
	Cancel();

#if defined (_DEBUG) && defined (_LOGGING)
	TBuf8<KHostNameSize> host;
	host.Copy((*iHost).Left(KHostNameSize)); //just get the KHostNameSize characters

	__FLOG_0(_T8("!! Stopping connection"));
	__FLOG_1(_T8("-> connect to host %S stopped"), &host);
#endif

	Suicide();
	}

/*
 *	Methods from CActive
 */

/**
The request servicing function. Behaviour depends on the state of the socket
connector.

@leave	The function will leave if the request status is an error.
*/
void CSocketConnector::RunL()
	{
	// Leave if there has been an error
	User::LeaveIfError(iStatus.Int());

	switch( iState )
		{
	case EPendingDNSLookup:
		{
#if defined (_DEBUG) && defined (_LOGGING)
		TBuf8<KHostNameSize> host;
		host.Copy((*iHost).Left(KHostNameSize)); //just get the KHostNameSize characters

		__FLOG_1(_T8("Doing DNS lookup -> searching for host %S"), &host);
#endif

		// Open the host resolver session
		User::LeaveIfError(iHostResolver.Open(
											 iCommsInfoProvider.SocketServer(),
											 iCommsInfoProvider.ProtocolDescription().iAddrFamily, 
											 KProtocolInetTcp,
											 iCommsInfoProvider.Connection()
											 ));
		// Start the DNS lookup for the remote host name.
		iHostResolver.GetByName(*iHost, iHostDnsEntry, iStatus);

		// Move to the Connecting state and go active
		iState = EConnecting;
		SetActive();
		} break;
	case EConnecting:
		{
		// DNS lookup successful - form the internet address object
		iAddress = TInetAddr(iHostDnsEntry().iAddr);
		iAddress.SetPort(iPort);

#if defined (_DEBUG) && defined (_LOGGING)
		TBuf8<KHostNameSize> host;
		host.Copy((*iHost).Left(KHostNameSize)); //just get the KHostNameSize characters

		TBuf<KIpv6MaxAddrSize> ip16bit;
		iAddress.Output(ip16bit);

		TBuf8<KIpv6MaxAddrSize> ip;
		ip.Copy(ip16bit);
		
		__FLOG_2(_T8("DNS lookup complete -> host %S has IP address %S"), &host, &ip);
#endif

		// Create the connecting socket
		iConnectingSocket = CSocket::NewL(iCommsInfoProvider, CSocket::EProtocolSocket);

		// Start connecting to the remote client
		iConnectingSocket->Connect(iAddress, iStatus);

		// Move to the Connected state and go active
		iState = EConnected;
		SetActive();
		} break;
	case EConnected:
		{
#if defined (_DEBUG) && defined (_LOGGING)
		TBuf8<KHostNameSize> host;
		host.Copy((*iHost).Left(KHostNameSize)); //just get the KHostNameSize characters

		TInetAddr addr;
		iConnectingSocket->RemoteName(addr);

		TBuf<KIpv6MaxAddrSize> ip16bit;
		addr.Output(ip16bit);

		TBuf8<KIpv6MaxAddrSize> ip;
		ip.Copy(ip16bit);

		TInetAddr local;
		iConnectingSocket->LocalName(local);

		__FLOG_1(_T8("!! Connection with host %S established"), &host);
		__FLOG_3(_T8("-> on local port %d with %S, remote port %d"), local.Port(), &ip, addr.Port());
#endif

		// A connection has been made with the remote client - lose ownership of
		// the connecting socket.
		CSocket* connectedSocket = iConnectingSocket;
		iConnectingSocket = NULL;

		// Create a socket controller object to own the connected socket.
		CSocketController* socketController = iSocketControllerFactory.CreateSocketControllerLC(connectedSocket);

		// Inform the socket connect observer that a TCP connection is established - 
		// pass back the input and output stream objects.
		iObserver->ConnectionMadeL(socketController->InputStream(), socketController->OutputStream());

		// Remove socket controller from cleanup stack - transferring ownership
		// to the store.
		CleanupStack::Pop(socketController);

		// Add the socket controller in the store - ownership is transferred to
		// the store.
		iSocketControllerFactory.AddToStoreL(socketController);

		// Socket connector is finished - suicide.
		Suicide();
		} break;
	case EIdle:
	default:
		TImapServerPanic::ImapPanic(TImapServerPanic::EBadSocketConnectorState);
		break;
		}
	}

/**
The asynchronous request cancel.
*/
void CSocketConnector::DoCancel()
	{
	// Check state
	switch( iState )
		{
	case EConnecting:
		{
		// DNS lookup is pending - cancel
		iHostResolver.Cancel();
		} break;
	case EConnected:
		{
		if( iConnectingSocket )
			{
			// Connection is pending - cancel and delete the socket
			iConnectingSocket->CancelConnect();
			delete iConnectingSocket;
			iConnectingSocket = NULL;
			}
		} break;
	case EIdle:
	case EPendingDNSLookup:
	default:
		// Do nothing...
		break;
		}
	}

/**
The error handler for when RunL() leaves. This function does any necessary
cleanup. The socket connector is then set to suicide.

@param	aError	The leave code.
@return	A value of KErrNone.if the error has been handled, any other 
		value if it has not been handled.
@post	The socket connector is in the Suicide state.
*/
TInt CSocketConnector::RunError(TInt aError)
	{
#if defined (_DEBUG) && defined (_LOGGING)
	TBuf8<KHostNameSize> host;
	host.Copy((*iHost).Left(KHostNameSize)); //just get the KHostNameSize characters

	__FLOG_1(_T8("!! Error : %d"), aError);
	__FLOG_1(_T8("-> connect to host %S failed"), &host);
#endif

	TInt errorToPropagate = aError;
	TInt error = KErrNone;

	switch( iState )
		{
	case EPendingDNSLookup:
	case EConnecting:
		{
		// In EReadyForDNS or EReadyToConnect states, KErrNotReady may be 
		// returned by the comms call that require a connection. The RConnection
		// that is passed in MUST be started or this error will occur. If we own
		// the RConnection we should start the RConnection again, else if our
		// client own the RConnection, we just propagate the Comms error.
		if( iCommsInfoProvider.OwnsConnection() && aError == KErrNotReady )
			{
			// We own the RConnection and the error is KErrNotReady
			errorToPropagate = iCommsInfoProvider.Connection().Start();

			if( errorToPropagate == KErrNone )
				{
				// RConnection started successfully, try to connect again
				CompleteSelf();
				return KErrNone;
				}
			}
		// Opening the connecting socket has failed or the DNS lookup completed
		// with an error code - suicide after notifying the observer - do nothing 
		// except drop through to the Connected state case...
		}
	case EConnected:
		{
		// Either the socket controller factory left in AddToStoreL() or in
		// CreateSocketControllerLC(), or the observer left in ConnectionMadeL()
		// or the connect request completed with an error code. Inform the 
		// observer of the error.
		error = iObserver->HandleConnectError(errorToPropagate);
		} break;
	case EIdle:
	default:
		TImapServerPanic::ImapPanic(TImapServerPanic::EBadSocketConnectorState);
		break;
		}

	Suicide();

	return error;
	}
