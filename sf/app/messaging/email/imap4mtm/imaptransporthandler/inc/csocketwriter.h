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

#ifndef __CSOCKETWRITER_H__
#define __CSOCKETWRITER_H__

#include <e32base.h>

#include "moutputstream.h"
#include "cimapobservabletimer.h"

// Forward declarations
class CSocket;
class MOutputSocketObserver;
class MSocketController;

/**
The CSocketWriter class encapsulates the writing functionality and behaviour
for a connected socket. It implements the MOutputStream API.

@see MOutputStream
@internalTechnology
@prototype
*/
class CSocketWriter : public CActive, public MOutputStream, public MImapTimerObserver
	{
public:
	static CSocketWriter* NewL(CSocket& aSocket, MSocketController& aController);
	virtual ~CSocketWriter();
	
	void SocketClosed(TInt aError);
	virtual void SecureClientReq(const TDesC8& aSSLDomainName);

private:
	CSocketWriter(CSocket& aSocket, MSocketController& aController);
	void ConstructL();
	void CompleteSelf();
	
	// from MOutputStream
	virtual void Bind(MOutputStreamObserver& aObserver, TInt aLogId);
	virtual void BindSecure(MOutputStreamSecureObserver& aObserver);
	virtual void SendDataReq(const TDesC8& aBuffer, TInt aIdleTime = 0);
	virtual void Close();

	// from CActive
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);
	void CloseStreams(TInt aError);
	// from MImapTimerObserver
	void OnTimerL(const CImapObservableTimer& aSourceTimer);

private:
	/** The state machine for the output stream. */
	enum TOutputState
		{
		/** The output stream is waiting for an observer to bind itself to it. */
		EIdle = 0,
		/** The output stream is ready to send data to the socket. It is waiting for the observer to request a data send. */
		EPendingSend,
		/** Data has been sent to the socket. The output stream is waiting for the socket to notify it that the write was successful. */
		ESentData,
		/** Start the secure handshake procedure. */
		EStartSecureHandshake,
		/** Secure handshake has completed. */
		ESecureHandshakeComplete,
		/**	The observer has asked the output stream to close the socket. */
		EClosing,
		/**	The socket has been closed - data can no longer be sent to it. */
		EClosed
		};

private:
	/** The connected socket.*/
	CSocket&					iSocket;
	/** The socket controller that owns the socket.*/
	MSocketController&			iController;
	/**	The state of the output stream. */
	TOutputState				iState;
	/**	The observer for the output stream. */
	MOutputStreamObserver*		iObserver;
	/**	The observer for the output stream for secure socket notifications. */
	MOutputStreamSecureObserver* iSecureObserver;
	/** SSL domain name to use for certificate validation. */
	TPtrC8 iSSLDomainName;
	/** Log file identifier */
	TInt iLogId;
	CImapObservableTimer* 		iTimer;
	};

#endif	// __CSOCKETWRITER_H__