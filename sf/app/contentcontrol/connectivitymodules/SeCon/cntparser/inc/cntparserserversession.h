/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  CCntParserServerSession header
*
*/


#ifndef __CNTPARSERSERVERSESSION_H__
#define __CNTPARSERSERVERSESSION_H__

#include "clientserver.h"
#include "cntparserclient.h"
#include "cntparserserver.h"
#include "cntparsercollector.h"

class CCntParserServer;

//**********************************
//CCntParserServerSession
//**********************************
class CCntParserServerSession : public CSession2
{
	public:
	static CCntParserServerSession* NewL(CCntParserServer* aServer);
	virtual ~CCntParserServerSession();
	void ConstructL(CCntParserServer* aServer);

	private:
	CCntParserServerSession();

	public:
	virtual void ServiceL(const RMessage2& aMessage);
	TInt DispatchMessageL(const RMessage2& aMessage);
	void CompletePhoneBookRequest(TInt aErr);

	protected:
	// panic the client
	void PanicClient(TInt aPanic) const;

	private:
	CCntParserServer* iContactsParserSrv;
	TBool iPhoneBookRequest;
	RMessage2 iPhoneBookRequestMessage;
};

#endif