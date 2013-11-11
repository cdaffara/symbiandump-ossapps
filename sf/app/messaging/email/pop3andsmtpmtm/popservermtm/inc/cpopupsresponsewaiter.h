// cpopupsresponsewaiter.h

// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CPOPUPSRESPONSEWAITER_H__
#define __CPOPUPSRESPONSEWAITER_H__

#include <e32base.h>
#include <mentact.h>

#include <ups/upsclient.h>
using namespace UserPromptService;

class CImPop3Settings;

/**
An class that is used for connect to the UPS server and check if the client is authorised
to connect to the remote server.
@internalComponent
@released
*/

NONSHARABLE_CLASS (CPopUpsResponseWaiter) : public CMsgActive
	{
public:
	static CPopUpsResponseWaiter* NewL();
	~CPopUpsResponseWaiter();
	void AuthoriseAndConnectL(CImPop3Settings* aPopSettings, TThreadId aClientThreadId, TBool aHasCapability, TRequestStatus& aStatus);

protected:
	CPopUpsResponseWaiter();

	// from CMsgActive;
	void DoRunL();
	void DoCancel();
	
private:
	CImPop3Settings* 	iPopSettings;
	/** For interaction with UPS server */
	RUpsSession 		iUpsSession;
	RUpsSubsession 		iUpsSubsession;
	TUpsDecision 		iDecision;
	};

#endif// __CPOPUPSRESPONSEWAITER_H__
