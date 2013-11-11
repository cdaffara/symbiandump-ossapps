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

#if (!defined __T_CRSUITESERVER_H__)
#define __T_CRSUITESERVER_H__
#include <test/testexecuteserverbase.h>


class CTestCalInterimApiSuite : public CTestServer
	{
public:
	static CTestCalInterimApiSuite* NewL();
	// Base class pure virtual override
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	};

#endif // __T_CRSUITESERVER_H__
