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

#ifndef __MSOCKETCONTROLLERFACTORY_H__
#define __MSOCKETCONTROLLERFACTORY_H__

#include <e32std.h>

// Forward declarations
class CSocket;
class CSocketController;

/**
The MSocketControllerFactory API provides support to create socket controller
objects that encapsulate a connected socket. Also provides an API to place 
socket controllers in a store.

@internalTechnology
@prototype
*/
class MSocketControllerFactory
	{
public:
	/**
	Creates a socket controller object to encapsulate the connected socket. 
	Ownership of the connected socket is transferred to the socket controller. 
	The pointer to the created object is left on the cleanup stack.
	@param aConnectedSocket The connected socket.
	@return A pointer to the created socket controller object, which has 
			also been left on the cleanup stack.
	@panic	EInvariantFalse	The connected socket pointer was NULL
	*/
	virtual CSocketController* CreateSocketControllerLC(CSocket* aConnectedSocket) =0;
	/**
	Requests that the socket controller be added to the socket controller store.
	Ownership of the socket controller is transferred on calling this function.
	@param aSocketController The socket controller object to be placed in the socket controller store.
	*/
	virtual void AddToStoreL(CSocketController* aSocketController) =0;
	};

#endif	// __MSOCKETCONTROLLERFACTORY_H__
