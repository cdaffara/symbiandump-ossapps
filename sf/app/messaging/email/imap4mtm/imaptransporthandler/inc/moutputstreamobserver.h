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

#ifndef __MOUTPUTSTREAMOBSERVER_H__
#define __MOUTPUTSTREAMOBSERVER_H__

#include <e32std.h>

/**
The MOutputStream and MOutputStreamObserver classes provide the API to send
data to a connected remote host. They encapsulate the outbound stream of a 
connected socket. 
	
The output socket observer must bind itself to the output stream before using
any of the other MOutputStream functions. The MOutputStream::Bind() API is 
used to do this binding. When done for the first time the output stream moves
from the Idle state to the PendingSend state.
	
Once an observer has been bound to the output stream data can be sent to the
connected host using the MOutputStream::SendDataReq() API. This can only be 
done when the output stream is in the PendingSend state otherwise a panic 
occurs. The MOutputStream::SendDataReq() API changes the output stream state
to the SentData state. 
	
The supplied data buffer must remain valid until the observer is notified 
that the send has been successful. This is done using the 
MOutputStreamObserver::SendDataCnf() API. The output stream moves back to 
the PendingSend state.

The output stream can only be re-bound to another observer when in the 
PendingSend state. The re-binding does not change the state of the output
stream.
		  
The MOutputSocket::Close() API closes the output stream synchronously. In 
this case the output stream observer will not be notified. Once the the call
completes the output stream is in the Closed state and is no longer valid. 
This synchronous close should be used when an asynchronous shutdown is not 
appropriate, e.g. when deleting the observer object in error conditions.

Similar to the asynchronous shutdown the corresponding input stream is also 
shutdown and its observer notified.

@see MOutputStream
@internalTechnology
@prototype
*/

class MOutputStreamObserver
	{
public:
	/**
	Confirms to the output stream observer that the data has been successfully
	sent to the remote host.
	@post		The output stream in the PendingSend state.
	*/
	virtual void SendDataCnf() =0;
	/**
	Indicates to the observer that the socket connection with the
	remote host has been closed. The output stream is no longer valid.
	@param 	aError  The error code explaining why the socket connection has
			closed. A value of KErrNone indicates that either the
			input or output stream observer requested that the 
			socket connection be closed.
	@post	The output stream is in the Closed state and is no longer valid.
	*/
	virtual void OutputStreamCloseInd(TInt aError) =0;
	};

#endif	// __MOUTPUTSTREAMOBSERVER_H__
