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

#ifndef __CSOCKET_H__
#define __CSOCKET_H__

#include <e32base.h>
#include <in_sock.h>

// Forward declarations
class MCommsInfoProvider;
class CSecureSocketController;
class TCertInfo;

/**
The CSocket class provides an implementation to encapsulate a socket.
It provides a limited version of the RSocket API. 

@internalTechnology
@prototype
*/
class CSocket: public CBase
	{
public:
	/**	
	The TSocketType enumerates the type of socket, e.g if the socket should be
	initiased for a specified protocol. 	
	*/	
	enum TSocketType
		{
		EProtocolSocket	= 0,
		EBlankSocket,
		};

public:
	static CSocket* NewL(MCommsInfoProvider& aCommsInfoProvider, TSocketType aSocketType);
	virtual ~CSocket();

	void Connect(TInetAddr& aAddr, TRequestStatus& aStatus);
	void CancelConnect();
	void RecvOneOrMore(TDes8& aBuffer, TRequestStatus& aStatus);
	void CancelRecv();
	void Send(const TDesC8& aBuffer, TRequestStatus& aStatus);
	void CancelSend();
	void RemoteName(TInetAddr& aAddr);
	void LocalName(TInetAddr& aAddr);
	void UpgradeToSecureL(TRequestStatus& aStatus, const TDesC8& aSSLDomainName);
	void CancelUpgradeToSecure();

private:
	CSocket(MCommsInfoProvider& aCommsInfoProvider);
	void ConstructL(TSocketType aSocketType);

private:
	MCommsInfoProvider&		iCommsInfoProvider;
	RSocket					iSocket;
	TSockXfrLength			iBytesReceived;
	CSecureSocketController* iSecureSocketController;
	};

#endif	// __CSOCKET_H__
