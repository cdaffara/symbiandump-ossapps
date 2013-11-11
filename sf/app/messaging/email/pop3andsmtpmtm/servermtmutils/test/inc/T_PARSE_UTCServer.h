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

#ifndef __T_PARSE_UTCSERVER_H__
#define __T_PARSE_UTCSERVER_H__

#include <test/testexecuteserverbase.h>

class CParseUTCServer : public CTestServer
	{
public:
	static CParseUTCServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	RFs& Fs(){return iFs;};	
private:
	RFs iFs;
	};
#endif