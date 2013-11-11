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

#ifndef __MSOCKETFACTORY_H__
#define __MSOCKETFACTORY_H__

#include <e32std.h>

// Forward declarations
class MSocketConnectObserver;
class MSocketConnector;

/**
The MSocketFactory class provides an API to obtain connected sockets. There
are two methods of obtaining these connected sockets. The first is via a
listening service on a specified local port that receives connections. The
second method is a connecting service that initiates a connection to a 
specified port on a specified remote host.

The connecting service also requires an observer - an MSocketConnectObserver.
There can be many connecting services active at any one time. Each one has
an associated socket connector - MSocketConnector. This API allows each 
connection request to be stopped at any time before the connection ends. The
connection is ended when the socket connect observer has been notified of 
either a successful connection or of an error occuring during the connection.
Attempting to stop the socket connector after either of these notifications
will cause an access violation - the socket connector is no longer valid.

@see MSocketConnectObserver
@see MSocketConnector
@internalTechnology
@prototype
*/
class MSocketFactory
	{
public:
	/**
	Requests that a connection is attempted with the specified port on the 
	specified remote host. The IP address can be either a numeric address or a
	host name.
	@param aObserver The observer object for this connection.
	@param aRemoteHost The host name/IP address of the remote client.
	@param aRemotePort The port number of the remote client.
	@return The socket connector that encapsulates the connection.
	@leave KErrNoMemory
	*/
	virtual MSocketConnector& ConnectL(MSocketConnectObserver& aObserver, const TDesC8& aRemoteHost, TUint16 aRemotePort) =0;
	};

#endif	// __MSOCKETFACTORY_H__
