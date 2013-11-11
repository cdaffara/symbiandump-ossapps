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

#include "csocketcontroller.h"

#include "csocket.h"
#include "csocketreader.h"
#include "csocketwriter.h"
#include "msocketcontrollerstore.h"
#include "mconnectionprefsprovider.h"
#include "imappaniccodes.h"
#include "imaptransportmanagercommon.h"

/**
The factory constructor. Ownership of the socket is transferred on calling 
the factory constructor.

@param	aSocket	The connected socket.
@return	A pointer to a fully constructed object.
*/
CSocketController* CSocketController::NewL(CSocket* aSocket, MConnectionPrefsProvider& aConnectionPrefsProvider)
	{
	// Ownership of socket has been transferred - place on cleanup stack.
	CleanupStack::PushL(aSocket);
	
	// Transfer ownership of the socket to the socket controller.
	CSocketController* self = new (ELeave) CSocketController(aSocket, aConnectionPrefsProvider);

	// Ownership of socket with the socket controller - pop-off the cleanup stack
	CleanupStack::Pop(aSocket);

	// Continue with initialisation of the socket controller
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.
*/
CSocketController::~CSocketController()
	{
	// Clean-up...
	delete iSocketReader;
	delete iSocketWriter;
	delete iSocket;
	}

/**
Constructor.

@param	aStore	The socket controller store.
@param	aSocket	The connected socket.
*/
CSocketController::CSocketController(CSocket* aSocket, MConnectionPrefsProvider& aConnectionPrefsProvider)
: CBase(), iSocket(aSocket), iConnectionPrefsProvider(aConnectionPrefsProvider)
	{
	__ASSERT_DEBUG( iSocket, User::Invariant() );
	}

/**
Second phase constructor. Initialises the object. Ownership of the connected
socket is passed to this class if this function completes.
*/
void CSocketController::ConstructL()
	{
	// Create the socket reader and writer
	// Writer is created (and added to AS) first so that it has a 'higher priority' in AS.
	// Must keep this order since it guarantees that we receive the acknowledgement of data being
	// sent out before receiving the response, which is something we rely on.
	iSocketWriter = CSocketWriter::NewL(*iSocket, *this);	
	iSocketReader = CSocketReader::NewL(*iSocket, *this);
	}

/**
Notifier that this object is now owned by the socket controller store.
	
@param	aStore	The socket controller store.
*/
void CSocketController::NotifyInStore(MSocketControllerStore& aStore)
	{
	iStore = &aStore;
	}

/**
The access function for the input stream.

@return	The input stream object.
*/
MInputStream& CSocketController::InputStream() const
	{
	return *iSocketReader;
	}

/**
The access function for the output stream.

@return	The output stream object.
*/
MOutputStream& CSocketController::OutputStream() const
	{
	return *iSocketWriter;
	}

/*
 *	Methods from MSocketController
 */

/**
@see MSocketController
*/
void CSocketController::StreamClosed(TInt aError, MSocketController::TStreamType aStreamType)
	{
	// Notify the other stream...
	switch( aStreamType )
		{
	case EInputStream:
		{
		// The input stream has closed the socket - inform the socket writer
		iSocketWriter->SocketClosed(aError);
		} break;
	case EOutputStream:
		{
		// The output stream has closed the socket - inform the socket reader
		iSocketReader->SocketClosed(aError);
		} break;
	default:
		User::Invariant();
		break;
		}

	// Both the input and output streams should be in the Closed state - this
	// socket controller is no longer useful.
	if( iStore )
		{
		// This socket controller is in the socket controller store - need to 
		// remove it from there.
		iStore->SocketControllerShutdown(*this);
		
		// This object is now ownerless - it should be deleted.
		delete this;
		}
	// This socket controller is not owned by the socket controller store - the
	// owner (probably the cleanup stack) will delete it.
	}

/**
@see MSocketController
*/
void CSocketController::StreamSuspended(MSocketController::TStreamType aSuspendedStream)
	{
	// Suspend the other stream...
	switch( aSuspendedStream )
		{
	case EInputStream:
		{
		// This is not supported yet!
		User::Invariant();
		} break;
	case EOutputStream:
		{
		// The output stream has suspended the socket - inform the socket reader
		iSocketReader->Suspend();
		} break;
	default:
		User::Invariant();
		break;
		}
	}

/**
@see MSocketController
*/
void CSocketController::StreamResumed(MSocketController::TStreamType aResumedStream)
	{
	// Resume the other stream...
	switch( aResumedStream )
		{
	case EInputStream:
		{
		// This is not supported yet!
		User::Invariant();
		} break;
	case EOutputStream:
		{
		// The output stream has resumed the socket - inform the socket reader
		iSocketReader->Resume();
		} break;
	default:
		User::Invariant();
		break;
		}
	}

/**
@see MSocketController
*/
#ifdef	_DEBUG
void CSocketController::ConnectionInfo(TDes8& aRemoteHost, TUint16& aRemotePort, TUint16& aLocalPort)
#else
void CSocketController::ConnectionInfo(TDes8& /*aRemoteHost*/, TUint16& /*aRemotePort*/, TUint16& /*aLocalPort*/)
#endif
	{
#ifdef _DEBUG
	__ASSERT_DEBUG( aRemoteHost.MaxLength() >= KIpv6MaxAddrSize, User::Invariant() );

	TInetAddr addr;
	iSocket->RemoteName(addr);
	
	TBuf<KIpv6MaxAddrSize> ip16bit;
	addr.Output(ip16bit);

	aRemoteHost.Copy(ip16bit);
	aRemotePort = static_cast<TUint16>(addr.Port());

	TInetAddr local;
	iSocket->LocalName(local);
	aLocalPort = static_cast<TUint16>(local.Port());
#else
	User::Invariant();
#endif
	}
