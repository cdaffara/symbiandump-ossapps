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

#ifndef __MOUTPUTSTREAM_H__
#define __MOUTPUTSTREAM_H__

#include <e32std.h>

// Forward declarations
class MOutputStreamObserver;
class MOutputStreamSecureObserver;
class TCertInfo;

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

@see MOutputStreamObserver
@internalTechnology
@prototype
*/
class MOutputStream
	{
public:
	/**
	This binds an observer to the output stream.
	@param aObserver An output stream observer.
	@param aLogId Log file identifier
	@pre		The output stream is either in the Idle or PendingSend state.
	@post		The output stream is in the PendingSend state.
	@panic		EBadOutputStreamState	The output stream is not in the Idle or
										PendingSend state.
	*/
	virtual void Bind(MOutputStreamObserver& aObserver, TInt aLogId) =0;

	/**
	This binds a secure socket notification observer to the output stream.
	@param aObserver An output stream secure observer.
	@pre		The output stream is either in the Idle or PendingSend state.
	@panic		EBadOutputStreamState	The output stream is not in the Idle or
										PendingSend state.
	*/
	virtual void BindSecure(MOutputStreamSecureObserver& aObserver) =0;

	/**
	Requests that the output stream send the supplied data to the connected host.
	The observer will be notified when the data has been successfully sent. The 
	data buffer must remain valid until this notification.
	@param aBuffer The buffer to send on the socket.
	@post		The output stream is in the SentData state.
	@panic		EOutputStreamNotBound	The output stream has no observer bound 
										to it.
	@panic		EBadOutputStreamState	The output stream is not in the 
										PendingSend state.
	*/
	virtual void SendDataReq(const TDesC8& aBuffer, TInt aIdleTime = 0) =0;
	/**
	Upgrade to a secure (client) connection
	@param aSSLDomainName The SSL domain name to use for certificate validation.
	*/
	virtual void SecureClientReq(const TDesC8& aSSLDomainName) = 0;
	/**
	Closes the output stream synchronously. The observer is not notified. The
	corresponding input stream is also closed and its observer notified.
	@pre		The output stream is not in the Closing or Closed state and an 
				observer has been bound to it.
	@post		The output stream is in the Closed state and no longer valid.
	@panic		EOutputStreamNotBound	The output stream has no observer bound 
										to it.
	@panic		EBadOutputStreamState	The output stream is in the Closing	or 
										Closed state.
	*/
	virtual void Close() =0;
	};

#endif	// __MOUTPUTSTREAM_H__
