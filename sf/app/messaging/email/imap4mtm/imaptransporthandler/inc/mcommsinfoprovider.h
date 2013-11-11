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

#ifndef __MCOMMSINFOPROVIDER_H__
#define __MCOMMSINFOPROVIDER_H__

#include <e32std.h>
#include <in_sock.h>


/**
The MCommsInfoProvider class provides accessors to a connected socket server
session and the protocol description for a particular protocol.

@internalTechnology
@prototype
*/
class MCommsInfoProvider
	{
public:
	/** 
	Access function to the socket server session.
	@return	A reference to the connected socket server session.
	*/
	virtual RSocketServ& SocketServer() =0;	
	/**
	Access function to the protocol description.
	@return	A reference to the protocol description.
	*/
	virtual TProtocolDesc& ProtocolDescription() =0;
	/**
	Access function to the network connection.
	@return A reference to the network connection.
	*/
	virtual RConnection& Connection() =0;
	/**
	Get the security preferences, dialog prompt and security policy
	@param aDialogPrompt 
	*/
	virtual void SecurityPreferences(TBool& aDialogPrompt) =0;
	/**
	Is the SockServ session owned.
	@return A boolean value stating whether SockServ session is owned.
	*/
	virtual TBool OwnsConnection() =0;	
	};

#endif	// __MCOMMSINFOPROVIDER_H__
