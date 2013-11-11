/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/



#ifndef __TESTCALINTERIMAPI_SERVER_H__
#define __TESTCALINTERIMAPI_SERVER_H__

#include <test/testexecuteserverbase.h>

class CTestCalInterimApiSuite : public CTestServer
	{
public:
	static CTestCalInterimApiSuite* NewL();
	// Base class pure virtual override
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	
	// Methods to start, check and stop the server
	TInt LaunchAgnServer(TPtrC aStartUpExeName, TPtrC aServerMode);
    void TerminateAgnServer();
    TBool CheckAgnServer(TExitType aExitTypeFromIni, TInt aExitReasonFromIni);

private:
    RProcess iStartUpProcess;
	};

#endif
