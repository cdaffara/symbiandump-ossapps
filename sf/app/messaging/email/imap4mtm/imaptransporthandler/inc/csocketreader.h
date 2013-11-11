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

#ifndef __CSOCKETREADER_H__
#define __CSOCKETREADER_H__

#include <e32base.h>

#include "minputstream.h"
#include "cimapobservabletimer.h"

// Size of buffer to store received data.
// The size is chosen to try to minimise the number of reads required for a 
// large data block on high speed connections.
const TInt KDefaultBufferSize	= 6144;

// Forward declarations
class CSocket;
class MInputStreamObserver;
class MSocketController;

/**
The CSocketReader class encapsulates the reading functionality and behaviour
for a connected socket. It implements the MInputStream API.

@see MInputStream
@internalTechnology
@prototype
*/
class CSocketReader : public CActive,
					  public MInputStream,
					  public MImapTimerObserver
	{
private:
	/** The state machine for the input stream. */
	enum TInputState
		{
		/** The input stream is currently not servicing a request */
		EIdle = 0,
		
		/** The input stream has an observer and it requested a read from the socket. */
		ERead,
		
		/**	The input stream has received a buffer of data from the socket. */
		EReceivedData,
		
		/**	The observer has asked the input stream to close the socket. */
		EClosing,
		
		/**	The socket has been closed - data can no longer be received from it. */
		EClosed
		};
	
public:
	static CSocketReader* NewL(CSocket& aSocket, MSocketController& aController);
	virtual ~CSocketReader();

	void SocketClosed(TInt aError);
	void Suspend();
	void Resume();

private:
	CSocketReader(CSocket& aSocket, MSocketController& aController);
	void ConstructL();
	void CompleteSelf();

	// from MInputStream
	virtual void Bind(MInputStreamObserver& aObserver, TInt aLogId);

	virtual void ReadReq(TInt aIdleTime=0);
	virtual void CancelReadReq();
	virtual TBool IsReading();
	
	// from CActive
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);
	
	void SetState(TInputState aState);
	void OnTimerL(const CImapObservableTimer& aSourceTimer);
	void CloseStreams(TInt aError);
private:
	/** The connected socket. */
	CSocket&					iSocket;
	/** The socket controller that owns the socket.*/
	MSocketController&			iController;
	/** The buffer into which the received data is placed. */
	TBuf8<KDefaultBufferSize>	iBuffer;
	/** Whether the data in iBuffer has been delivered to an observer */
	TBool						iBufferDataDelivered;
	/**	The state of the input stream. */
	TInputState					iState;
	/** Is the socket suspended */
	TBool						iSuspended;
	/** The observer for the input stream. */
	MInputStreamObserver*		iObserver;
	/** Log file identifier */
	TInt iLogId;
	/**Imap Idle Timer. */
	CImapObservableTimer*		iTimer;
	TInt						iIdleTime;
	};

#endif	// __CSOCKETREADER_H__
