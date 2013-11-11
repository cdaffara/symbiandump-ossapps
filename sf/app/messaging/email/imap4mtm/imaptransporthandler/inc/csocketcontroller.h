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

#ifndef __CSOCKETCONTROLLER_H__
#define __CSOCKETCONTROLLER_H__

#include <e32base.h>

#include "msocketcontroller.h"

// Forward declarations
class CSocket;
class MInputStream;
class MOutputStream;
class CSocketReader;
class CSocketWriter;
class MSocketControllerStore;
class MConnectionPrefsProvider;

/**
The CSocketController class owns a connected socket. It provides input and
output stream objects that use that socket via the CSocketReader and 
CSocketWriter attributes.

A CSocketController object will self-delete once the socket it owns has been 
closed. The socket can be closed either by the remote client, the input stream
observer or the output stream observer.
	
@internalTechnology
@prototype
*/
class CSocketController: public CBase, public MSocketController
	{
public:
	static CSocketController* NewL(CSocket* aSocket, MConnectionPrefsProvider& aConnectionPrefsProvider);
	virtual ~CSocketController();

	void NotifyInStore(MSocketControllerStore& aStore);
	MInputStream& InputStream() const;
	MOutputStream& OutputStream() const;

private:
	CSocketController(CSocket* aSocket, MConnectionPrefsProvider& aConnectionPrefsProvider);
	void ConstructL();

	// from MSocketController
	virtual void StreamClosed(TInt aError, MSocketController::TStreamType aStreamType);
	virtual void StreamSuspended(MSocketController::TStreamType aSuspendedStream);
	virtual void StreamResumed(MSocketController::TStreamType aResumedStream);
	virtual void ConnectionInfo(TDes8& aRemoteHost, TUint16& aRemotePort, TUint16& aLocalPort);

private:
	/** The socket controller store. This is not owned.*/
	MSocketControllerStore*	iStore;
	/**	The connected socket.*/
	CSocket*				iSocket;
	/**	The socket reader. */
	CSocketReader*			iSocketReader;
	/**	The socket writer. */
	CSocketWriter*			iSocketWriter;
	/** The connection preference */
	MConnectionPrefsProvider& iConnectionPrefsProvider;
	};

#endif	// __CSOCKETCONTROLLER_H__
