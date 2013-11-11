// csmtpupsresponsewaiter.h

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

#ifndef __CSMTPUPSRESPONSEWAITER_H__
#define __CSMTPUPSRESPONSEWAITER_H__

#include <e32base.h>
#include <mentact.h>

#include <ups/upsclient.h>
using namespace UserPromptService;

class CMsgActive;
class CImSmtpSettings;


/**
An class that is used for connect to the UPS server and check if the client is authorised
to connect to the remote server.
@internalComponent
@released
*/
NONSHARABLE_CLASS (CSmtpUpsResponseWaiter) : public CMsgActive
	{
public:
	static CSmtpUpsResponseWaiter* NewL();
	~CSmtpUpsResponseWaiter();
	
	void AuthoriseAndConnectL(const CImSmtpSettings& aSettings, TBool aHasCapability, TThreadId aClientThreadId, TRequestStatus& aStatus);

protected:
	CSmtpUpsResponseWaiter();

	// from CMsgActive;
	void DoRunL();
	void DoCancel();
	
private:
	/** For interaction with UPS server */
	RUpsSession 		iUpsSession;
	RUpsSubsession 		iUpsSubsession;
	TUpsDecision 		iDecision;
	};

#endif// __CSMTPUPSRESPONSEWAITER_H__
