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

#ifndef __MINPUTSTREAMOBSERVER_H__
#define __MINPUTSTREAMOBSERVER_H__

#include <e32std.h>

/**
The MInputStreamObserver class encapsulates an observer on 
the input stream.
@internalTechnology
@prototype
*/
class MInputStreamObserver
	{
public:	// methods
	/**
	Indicates to the observer that data has been received from the connected host.
	The supplied buffer will remain valid until the input stream is notified
	that it is no longer needed.
	@param aBuffer A buffer containing the received data.
	@post The input stream is in the PendingAck state.
	*/
	virtual void ReceivedDataIndL(const TDesC8& aBuffer) =0;

	/**
	Confirms to the observer that a request to upgrade to a secure connection has
	completed.
	*/
	virtual void SecureServerCnf() =0;

	/**
	Indicates to the observer that the socket connection with the
	remote host has been closed. The input stream is no longer valid.
	@param aError 	The error code explaining why the socket connection has
				  	closed. A value of KErrNone indicates that either the
				  	input or output stream observer requested that the 
					socket connection be closed.
	@post The input stream is in the Closed state and is no longer valid.
	*/
	virtual void InputStreamCloseInd(TInt aError) =0;
	};

#endif	// __MINPUTSTREAMOBSERVER_H__
