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

#ifndef __MINPUTSTREAM_H__
#define __MINPUTSTREAM_H__

#include <e32std.h>

// Forward declarations
class MInputStreamObserver;
class TCertInfo;

/**
The MInputStream class provides the API to receive data from a connected 
remote host. They encapsulate the inbound stream of a connected socket. 
	
The input socket observer must bind itself to the input stream before using
any of the other MInputStream functions. The MInputStream::Bind() API is 
used to do this binding. The input stream does not do its first read until 
an observer has been bound to it. When done for the first time the input 
stream moves from the Idle state to the Read state.
	
When the input stream is in the Read state, it requests a read from the 
socket. This moves the input stream into the ReceivedData state and it waits
for the socket to notify it when it receives any data from the remote host. 
When the input stream receives the notification it notifies its observer via
the MInputStreamObserver::ReceivedDataInd() API. The input stream moves into
the PendingAck state.

The buffer containing the received data remains valid until the observer 
notifies the input stream that it is no longer needed. The observer can do
this using the MInputStream::ReceivedDataRes() API. The input stream returns
to the Read state and issues another read request to the socket.

The input stream can only be re-bound to another observer when it is in the 
PendingAck state. This does not change the state of the input stream. The 
new observer is responsible for notifying the input stream when the received 
data buffer is no longer needed.
		  
The MInputSocket::Close() API closes the input stream synchronously. In 
this case the observer will not be notified. Once the the call completes 
the input stream is in the Closed state and is no longer valid. This
synchronous close should be used when an asynchronous shutdown is not 
appropriate, e.g. when deleting the observer object in error conditions.

Similar to the asynchronous shutdown the corresponding output stream is also 
shutdown and its observer notified.

@see MInputStreamObserver
@internalTechnology
@prototype
*/
class MInputStream
	{
public:
	/**
	This binds an observer to the input stream. The bound observer is responsible
	for notifying the input stream when the current received data (if any) is no
	longer needed.
	@param aObserver An input stream observer.
	@param aLogId Log file identifier
	@pre		The input stream is either in the Idle or PendingAck state.
	@post		The input stream is in the Read state if the previous state was
				the Idle state, or it remains in the PendingAck state.
	@panic		EBadInputStreamState	The input stream is not in the Idle or
										PendingAck state.
	*/
	virtual void Bind(MInputStreamObserver& aObserver, TInt aLogId) =0;

	/**
	Informs the input stream that the received data is no longer needed. The
	input stream can request a read from the socket.
	@pre		The input stream is in the PendingAck state and an observer has
				been bound to it.
	@post		The input stream is in the Read state.
	@panic		EInputStreamNotBound	The input stream has no observer bound 
										to it.
	@panic		EBadInputStreamState	The input stream is not in the EPendingAck
										state.
	*/
	virtual void ReadReq(TInt aIdleTime=0) =0;

	/**
	Cancels a read request
	@pre		The input stream is in the Idle, Read, or ReceivedData state
	@post		The input stream is in the Idle state.
	@panic	EBadInputStreamState	The input stream is not in the correct state
	*/
	virtual void CancelReadReq() =0;
	
	/**
	Returns whether a read request exists on the stream
	*/
	virtual TBool IsReading() =0;
	};

#endif	// __MINPUTSTREAM_H__
