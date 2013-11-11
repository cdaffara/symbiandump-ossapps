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
// t_smtp_utc_server.cpp
// 
//

#ifndef __T_SMTP_UTC_SERVER_H__
#define __T_SMTP_UTC_SERVER_H__
 
#include <test/testexecuteserverbase.h>

//
// SortUTCServer
//

class CSchSendUTCServer : public CTestServer
	{
public:
	static CSchSendUTCServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	RFs& Fs();	
private:
	RFs iFs;
	};
#endif //__T_SMTP_UTC_SERVER_H__