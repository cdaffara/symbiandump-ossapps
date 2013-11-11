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

#ifndef __MCONNECTIONPREFSPROVIDER_H__
#define __MCONNECTIONPREFSPROVIDER_H__

#include <e32std.h>

// Forward declarations
class RConnection;

/**
The MConnectionPrefsProvider class provides accessors to
connection preferences.

@internalTechnology
@prototype
*/
class MConnectionPrefsProvider
	{
public:
	/**
	Supply the comms connection
	@param aSocketServerHandle the handle to the socket server.
	@param aConnectionPtr The network connection
	@return Boolean value stating whether comms connection supplied.
	*/
	virtual TBool SupplyCommsConnection(TInt& aSocketServerHandle, RConnection*& aConnectionPtr) =0;
	/**
	Set the comms connection.
	@param aSocketServerHandle The handle to the socket server.
	@param aConnectionPtr The network connection.
	*/
	virtual void SetCommsConnectionL(TInt aSocketServerHandle, RConnection*& aConnectionPtr) =0;
	/**
	Provide security preferences
	@param aDialogPrompt The dialog prompt for security dialogs.
	*/
	virtual void GetSecurityPrefs(TBool& aDialogPrompt) =0;
	};

#endif	// __MCONNECTIONPREFSPROVIDER_H__

