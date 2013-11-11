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
// Phonebook Sync Multiple Phonebook Test server header file.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __TE_PHBKSYNCSERVER_H__
#define __TE_PHBKSYNCSERVER_H__

#include "TE_PhBkSyncBase.h"
#include <test/testexecuteserverbase.h>

class CPhBkSyncTestServer : public CTestServer
	{
public:
	~CPhBkSyncTestServer();

	static CPhBkSyncTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	};

#endif // __TE_PHBKSYNCSERVER_H__
