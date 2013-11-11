// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Phonebook Sync Test server header file.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __TE_CNTSYNCSERVER_H__
#define __TE_CNTSYNCSERVER_H__
#include "te_cntsyncbase.h"
#include <test/testexecuteserverbase.h>

class CCntSyncTestServer : public CTestServer
	{
public:
	~CCntSyncTestServer();

	static CCntSyncTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	};

#endif // __TE_CNTSYNCSERVER_H__
