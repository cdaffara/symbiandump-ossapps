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

#ifndef __CTCPTRANSPORTLAYER_H__
#define __CTCPTRANSPORTLAYER_H__

#include <e32base.h>
#include <in_sock.h>

#include "msocketfactory.h"
#include "msocketcontrollerfactory.h"
#include "msocketconnectorstore.h"
#include "msocketcontrollerstore.h"
#include "mcommsinfoprovider.h"

// Forward declarations
class CSocket;
class CSocketConnector;
class CSocketController;
class MConnectionPrefsProvider;

/**
The CTcpTransportLayer class provides an implementation of the TCP transport 
layer. This implementation uses a TCP/IP transport to provide input and output 
streams to connected remote hosts.

The CTcpTransportLayer class also provides the MSocketFactory, MSocketControllerFactory,
MSocketConnectorStore, MSocketControllerStore, MCommsInfoProvider API's. These API's are
used to supply input and output streams to remote hosts.

@see MSocketFactory
@see MSocketControllerFactory
@see MSocketConnectorStore
@see MSocketControllerStore
@see MCommsInfoProvider
@internalTechnology
@prototype
*/
class CTcpTransportLayer : public CBase,
						   public MSocketFactory,
						   public MSocketControllerFactory,
						   public MSocketConnectorStore,
						   public MSocketControllerStore,
						   public MCommsInfoProvider
	{						 
public:
	static CTcpTransportLayer* NewL(MConnectionPrefsProvider& aConnectionPrefsProvider);
	virtual ~CTcpTransportLayer();
	
private:
	
	CTcpTransportLayer(MConnectionPrefsProvider& aConnectionPrefsProvider);
	void ConstructL();
	
	MSocketControllerFactory& SocketControllerFactory();
	MSocketConnectorStore& SocketConnectorStore();
	MSocketControllerStore& SocketControllerStore();
	MCommsInfoProvider& CommsInfoProvider();

	// from MSocketFactory
	virtual MSocketConnector& ConnectL(MSocketConnectObserver& aObserver, const TDesC8& aRemoteHost, TUint16 aRemotePort);

	// from MSocketControllerFactory
	virtual CSocketController* CreateSocketControllerLC(CSocket* aConnectedSocket);
	virtual void AddToStoreL(CSocketController* aSocketController);

	// from MSocketConnectorStore
	virtual void ConnectionCompleted(CSocketConnector& aOrphanedSocketConnector);

	// from MSocketControllerStore
	virtual void SocketControllerShutdown(CSocketController& aOrphanedSocketController);

	// from MCommsInfoProvider
	virtual RSocketServ& SocketServer();
	virtual TProtocolDesc& ProtocolDescription();
	virtual RConnection& Connection();
	virtual void SecurityPreferences(TBool& aDialogPrompt);
	virtual TBool OwnsConnection();

private:
	/** The connection prefs provider */
	MConnectionPrefsProvider&			iConnectionPrefsProvider;
	/** The socket server session. */	
	RSocketServ							iSocketServer;
	/** The protocol description. */
	TProtocolDesc						iProtocolDescription;
	/** The network connection. */
	RConnection*						iConnection;
	/** Does transport layer own SockServ session. */
	TBool								iOwnsConnection;
	/** The socket connector store. */
	RPointerArray<CSocketConnector>		iConnectorStore;
	/** The socket controller store. */
	RPointerArray<CSocketController>	iControllerStore;
	};

#endif //	__CTCPTRANSPORTLAYER_H__