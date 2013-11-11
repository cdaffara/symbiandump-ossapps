// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __T_IMAPNEWMSGDURINGSYNCIDLECANCELSERVER_H__
#define __T_IMAPNEWMSGDURINGSYNCIDLECANCELSERVER_H__

#include <testexecuteserverbase.h>

class CImapNewMsgDuringSyncIdleCancelServer : public CTestServer
	{
public:
	static CImapNewMsgDuringSyncIdleCancelServer* NewL();
	~CImapNewMsgDuringSyncIdleCancelServer();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	RFs& Fs();

private:
	CImapNewMsgDuringSyncIdleCancelServer();

private:
	RFs iFs;
	};

#endif  //__T_IMAPNEWMSGDURINGSYNCIDLECANCELSERVER_H__
