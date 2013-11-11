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

#ifndef __CTRANSPORTMANAGER_H__
#define __CTRANSPORTMANAGER_H__

#include <e32base.h>

#include "mconnectionprefsprovider.h"

// Forward declarations
class CTcpTransportLayer;
class MSocketFactory;
class RConnection;

/**
The CTransportManager class provides an implementation to manage the
transport layer.

@see MConnectionPrefsProvider
@see CTcpTransportLayer
@internalTechnology
@prototype
*/
class CTransportManager : public CBase, public MConnectionPrefsProvider
	{
public:
	IMPORT_C static CTransportManager* NewL(TInt aSocketServerHandle, RConnection& aConnection);
	IMPORT_C virtual ~CTransportManager();

	IMPORT_C MSocketFactory& SocketFactory();

private:
	CTransportManager(TInt aSocketServerHandle, RConnection& aConnection);
	void ConstructL();

	// from MConnectionPrefsProvider
	TBool SupplyCommsConnection(TInt& aSocketServerHandle, RConnection*& aConnectionPtr);
	void SetCommsConnectionL(TInt aSocketServerHandle, RConnection*& aConnection);
	void GetSecurityPrefs(TBool& aDialogPrompt);

private:
	/** The TCP transport layer. */
	CTcpTransportLayer* iTcpLayer;

	TInt iSocketServerHandle;
	
	// Pointer to the RConnection to use. This is not owned by this class, but
	// it cannot be a reference as it can be set from multiple public APIs
	RConnection* iConnection;
	};

#endif // __CTRANSPORTMANAGER_H__
