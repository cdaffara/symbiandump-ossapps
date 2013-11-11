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

#include "ctransportmanager.h"
#include "ctcptransportlayer.h"

/**
The factory constructor.

@return	A pointer to a fully constructed object.
*/
EXPORT_C CTransportManager* CTransportManager::NewL(TInt aSocketServerHandle, RConnection& aConnection)
	{
	CTransportManager* self = new(ELeave) CTransportManager(aSocketServerHandle, aConnection);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Constructor.
*/
CTransportManager::CTransportManager(TInt aSocketServerHandle, RConnection& aConnection) :
 iSocketServerHandle(aSocketServerHandle),
 iConnection(&aConnection)
	{
	}

/**
Second phase constructor. Sets up the TCP layer and
assigns the object to iTcpLayer.
*/
void CTransportManager::ConstructL()
	{
	iTcpLayer = CTcpTransportLayer::NewL(*this);
	}

/**
Destructor.
*/
EXPORT_C CTransportManager::~CTransportManager()
	{
	delete iTcpLayer;
	}

/**
Return the connected sockets to caller.

@return Obtain the connected sockets.
*/
EXPORT_C MSocketFactory& CTransportManager::SocketFactory()
	{
	return *iTcpLayer;
	}

/**
Supply the comms connection

@param 	aSocketServerHandle  	The handle to the socket server.  
@param	aConnectionPtr  		The network connection
@return	Boolean value stating whether comms connection supplied.
*/
TBool CTransportManager::SupplyCommsConnection(TInt& aSocketServerHandle, RConnection*& aConnectionPtr)
	{
	if (iSocketServerHandle != 0)
		{
		aSocketServerHandle = iSocketServerHandle;
		aConnectionPtr = iConnection;
		return ETrue;
		}

	return EFalse;
	}

/**
Set the comms connection

@param aSocketServerHandle The handle to the socket server. 
@param aConnectionPtr      The network connection.
*/
void CTransportManager::SetCommsConnectionL(TInt aSocketServerHandle, RConnection*& aConnectionPtr)
	{
	iSocketServerHandle = aSocketServerHandle;
	iConnection = aConnectionPtr;
	}

/**
Provide security preferences 

@param	aDialogPrompt  The dialog prompt for security dialogs.
*/	
void CTransportManager::GetSecurityPrefs(TBool& aDialogPrompt)
	{
	// Allow security dialogs
	aDialogPrompt = ETrue;
	}
