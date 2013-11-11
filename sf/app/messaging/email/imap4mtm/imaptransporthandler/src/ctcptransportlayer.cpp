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

#include <commdbconnpref.h>

#include "ctcptransportlayer.h"
#include "csocketconnector.h"
#include "csocketcontroller.h"
#include "imappaniccodes.h"
#include "mconnectionprefsprovider.h"

#include "csocketreader.h"
#include "csocketwriter.h"

_LIT(KTcpProtName, "tcp");

/**
The factory constructor.
@param	aConnectionPrefsProvider	Provides accessors to a connected socket server 
									session and the protocol description for a particular 
									protocol. 	
*/
CTcpTransportLayer* CTcpTransportLayer::NewL(MConnectionPrefsProvider& aConnectionPrefsProvider)
	{
	CTcpTransportLayer* self = new(ELeave) CTcpTransportLayer(aConnectionPrefsProvider);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.
*/
CTcpTransportLayer::~CTcpTransportLayer()
	{
	// Delete the socket connectors
	iConnectorStore.ResetAndDestroy();

	// Delete the socket controllers
	iControllerStore.ResetAndDestroy();

	// Close the socket server session if owned
	if( iOwnsConnection )
		{
		if( iConnection )
			{
			iConnection->Close();
			delete iConnection;
			}
		iSocketServer.Close();
		}	
	}

/**
Constructor.

@param	aConnectionPrefsProvider	The connected server session and the protocol description
									for a particular protocol.
*/
CTcpTransportLayer::CTcpTransportLayer(MConnectionPrefsProvider& aConnectionPrefsProvider)
	: iConnectionPrefsProvider(aConnectionPrefsProvider)
	{

	}
	
/**
Second phase constructor. Will connect to the socket server and obtain the
protocol description.	
*/
void CTcpTransportLayer::ConstructL()
	{
	// Callback on the observer for a Comms connection, otherwise create our own
	TInt socketServerHandle = 0;
	if (iConnectionPrefsProvider.SupplyCommsConnection(socketServerHandle, iConnection))
		{
		__ASSERT_ALWAYS(iConnection!=NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EExpectedConnectionNotSupplied));

		iOwnsConnection = EFalse;
		iSocketServer.SetHandle(socketServerHandle);
		}
	else
		{
		iOwnsConnection = ETrue;

		// Connect to the socket server
		User::LeaveIfError(iSocketServer.Connect());
		iConnection = new(ELeave) RConnection();
		User::LeaveIfError(iConnection->Open(iSocketServer));
		User::LeaveIfError(iConnection->Start());  //using default CommDb Settings
		iConnectionPrefsProvider.SetCommsConnectionL(iSocketServer.Handle(), iConnection);
		}

	// Get the protocol description
	User::LeaveIfError(iSocketServer.FindProtocol(KTcpProtName(), iProtocolDescription));	
	}
	
/**
Provides support to create socket controller objects that encapsulate 
a connected socket

@return	Reference to the socket controller object.
*/
MSocketControllerFactory& CTcpTransportLayer::SocketControllerFactory()
	{
	return *this;
	}

/**
Provides the socket connector store.

@return	Reference to the socket connector store.
*/	
MSocketConnectorStore& CTcpTransportLayer::SocketConnectorStore()
	{
	return *this;
	}

/**
Provides the socket controller store.
*/	
MSocketControllerStore& CTcpTransportLayer::SocketControllerStore()
	{
	return *this;
	}

/**
Provides the connected server session and the protocol description for 
a particular protocol.

@return	Reference to the comms info provider.
*/	
MCommsInfoProvider& CTcpTransportLayer::CommsInfoProvider()
	{
	return *this;
	}

/**
Requests that a connection is attempted with the specified port on 
the specified remote host. The IP address can be either a numeric address 
or a host name.

@param	aObserver  		The observer object for this connection.
@param	aRemoteHost  	The host name/IP address of the remote client.
@param	aRemotePort  	The port number of the remote client.
@see MSocketFactory
*/ 
MSocketConnector& CTcpTransportLayer::ConnectL(MSocketConnectObserver& aObserver, const TDesC8& aRemoteHost, TUint16 aRemotePort)
	{
	// Create the socket connector
	CSocketConnector* socketConnector = CSocketConnector::NewL(SocketConnectorStore(), SocketControllerFactory(), CommsInfoProvider());

	CleanupStack::PushL(socketConnector);

#if defined (_DEBUG) && defined (_LOGGING)
	socketConnector->__logger__ = this->__logger__;
#endif

	// Start connection to remote host
	socketConnector->ConnectL(aObserver, aRemoteHost, aRemotePort);

	// Add to the connector store
	User::LeaveIfError(iConnectorStore.Append(socketConnector));

	// Ownership of the socket connector now transferred to the store.
	CleanupStack::Pop(socketConnector);

	return *socketConnector;
	}
	

/**
Creates a socket controller object to encapsulate the connected socket. 
Ownership of the connected socket is transferred to the socket controller. 
The pointer to the created object is left on the cleanup stack. 

@param	aConnectedSocket 	The connected socket.
@see MSocketControllerFactory
*/
CSocketController* CTcpTransportLayer::CreateSocketControllerLC(CSocket* aConnectedSocket)
	{
	__ASSERT_DEBUG( aConnectedSocket, User::Invariant() );

	// A connection has been established on the given socket - create a socket
	// controller object to own the connected socket. 
	CSocketController* socketController = CSocketController::NewL(aConnectedSocket, iConnectionPrefsProvider);
	CleanupStack::PushL(socketController);

	return socketController;
	}

/**
Requests that the socket controller be added to the socket controller store. 
Ownership of the socket controller is transferred on calling this function.

@param	aSocketController  	The socket controller object to be placed in the socket 
							controller store. 
*/
void CTcpTransportLayer::AddToStoreL(CSocketController* aSocketController)
	{
	__ASSERT_DEBUG( aSocketController, User::Invariant() );

	// Ownership has been transferred - place on cleanup stack.
	CleanupStack::PushL(aSocketController);

	// Append to the store
	User::LeaveIfError(iControllerStore.Append(aSocketController));

	// The store now owns the socket controller
	CleanupStack::Pop(aSocketController);

	// Notify the socket controller that it's in the store
	aSocketController->NotifyInStore(SocketControllerStore());
	}


/**
Notifies the store that the socket connector has completed. The specified socket 
connector should be removed from the store. The socket connector will self delete 
after this call has completed as it has been orphaned. 

@param	aOrphanedSocketConnector	The socket connector to be removed from the store. 
									EInvariantFalse The orphaned socket connector is not 
									in the store.
@see MSocketConnectorStore
*/
void CTcpTransportLayer::ConnectionCompleted(CSocketConnector& aOrphanedSocketConnector)
	{
	// Find the appropriate index for this connector
	TInt index = iConnectorStore.Count();
	TBool found = EFalse;
	while(!found && index>0)
		{
		found = &aOrphanedSocketConnector == iConnectorStore[--index];
		}
	__ASSERT_DEBUG(found, User::Invariant());
	
	// Remove the socket connector from the store and compress the store.
	iConnectorStore.Remove(index);
	iConnectorStore.Compress();
	}

/**
Remove the socket controller object from the store. The socket controller is removed 
from the store - the store no longer has ownership of the socket controller.

@param	aOrphanedSocketController	The socket controller to be removed from the store.
@see MSocketControllerStore
*/
void CTcpTransportLayer::SocketControllerShutdown(CSocketController& aOrphanedSocketController)
	{
	// Find the appropriate index for this connector
	TInt index = iControllerStore.Count();
	TBool found = EFalse;
	while(!found && index>0)
		{
		found = &aOrphanedSocketController == iControllerStore[--index];
		}
	__ASSERT_DEBUG(found, User::Invariant());
	
	// Remove the socket controller from the store and compress the store.
	iControllerStore.Remove(index);
	iControllerStore.Compress();
	}


/**
Accessor to the socket server session

@return	A reference to the connected socket server session.
@see MCommsInfoProvider
*/
RSocketServ& CTcpTransportLayer::SocketServer()
	{
	return iSocketServer;
	}

/**
Accessor to the protocol description.

@return	A reference to the protocol description.
*/
TProtocolDesc& CTcpTransportLayer::ProtocolDescription()
	{
	return iProtocolDescription;
	}

/**
Accessor to the network connection

@return	A reference to the network connection.
*/
RConnection& CTcpTransportLayer::Connection()
	{
	return *iConnection;
	}

/**
Get the security preferences, dialog prompt and security policy 

@param	aDialogPrompt	The dialog prompt.
*/
void CTcpTransportLayer::SecurityPreferences(TBool& aDialogPrompt)
	{
	iConnectionPrefsProvider.GetSecurityPrefs(aDialogPrompt);
	}

/**
Is the SockServ session owned. 

@return	A boolean value stating whether SockServ session is owned.
*/
TBool CTcpTransportLayer::OwnsConnection()
	{
	return iOwnsConnection;
	}
