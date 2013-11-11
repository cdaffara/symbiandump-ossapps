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

#ifndef __MSOCKETCONTROLLER_H__
#define __MSOCKETCONTROLLER_H__

#include <e32std.h>

/**
The observer API for the socket reader and writer. It is used by the socket 
reader or writer to notify the socket controller of when either the input or
output stream has been closed. It also provides name and port info for the
remote host.
@internalTechnology
@prototype
*/
class MSocketController
	{
public:
	/** The TStreamType enumerates the types of streams. */
	enum TStreamType
		{
		/** The input stream. */
		EInputStream = 0,
		/** The output stream. */
		EOutputStream
		};	

public:
	/**
	This notifies the socket controller that either the input or the output 
	stream is closed. That stream is no longer valid.
	@param aError The error code explaining why the stream has closed.
				  A value of KErrNone indicates that the stream 
				  observer requested that the stream be closed.
	@param aStreamType The stream that has been closed.
	*/
	virtual void StreamClosed(TInt aError, MSocketController::TStreamType aStreamType) =0;
	/**
	This notifies the socket controller that either the input or the output
	stream has been suspended.
	@param aStreamType The stream that has been suspended.
	*/
	virtual void StreamSuspended(MSocketController::TStreamType aSuspendedStream) =0;
	/**
	This notifies the socket controller that either the input or the output
	stream has been resumed.
	@param aStreamType The stream that has been resumed.
	*/
	virtual void StreamResumed(MSocketController::TStreamType aResumedStream) =0;
	/**
	Provides connection information to the socket controller.
	@param aRemoteHost The remote host.
	@param aRemotePort The remote port number.
	@param aLocalPort The local port being used.
	*/
	virtual void ConnectionInfo(TDes8& aRemoteHost, TUint16& aRemotePort, TUint16& aLocalPort) =0;
	};

#endif	// __MSOCKETCONTROLLER_H__

