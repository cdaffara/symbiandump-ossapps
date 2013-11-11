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

#ifndef __CSECURESOCKETCONTROLLER_H__
#define __CSECURESOCKETCONTROLLER_H__

#include <e32base.h>
#include <securesocket.h>

// Forward declarations
class MCommsInfoProvider;

/**
The CSecureSocketController class provides an implementation of a secure 
socket controller for a socket to use.

@internalTechnology
@prototype
*/
class CSecureSocketController : public CBase
	{
public:
	static CSecureSocketController* NewL(RSocket& aSocket, MCommsInfoProvider& aCommsInfoProvider);
	virtual ~CSecureSocketController();

	void StartSecureHandshakeL(TRequestStatus& aStatus, const TDesC8& aSSLDomainName);
	void RecvOneOrMore(TDes8& aBuffer, TRequestStatus& aStatus, TSockXfrLength& aLength);
	void CancelRecv();
	void Send(const TDesC8& aBuffer, TRequestStatus& aStatus);
	void CancelSend();
	void CancelHandshake();

private:
	CSecureSocketController(RSocket& aSocket, MCommsInfoProvider& aCommsInfoProvider);

private:
	RSocket& iSocket;
	MCommsInfoProvider& iCommsInfoProvider;
	CSecureSocket* iTlsSocket;
	};

#endif	// __CSECURESOCKETCONTROLLER_H__
