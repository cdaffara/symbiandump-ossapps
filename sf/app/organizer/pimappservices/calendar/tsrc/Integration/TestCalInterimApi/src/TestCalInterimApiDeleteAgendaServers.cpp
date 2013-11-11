// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TestCalInterimApiDeleteAgendaServers.h"


/**
 * Destructor
 */
CTestCalInterimApiDeleteAgendaServers::~CTestCalInterimApiDeleteAgendaServers()
	{
	}

/**
 * Constructor
 */
CTestCalInterimApiDeleteAgendaServers::CTestCalInterimApiDeleteAgendaServers()
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestCalInterimApiDeleteAgendaServers);
	}

/**
 * To bypass base class implementation 
 */
TVerdict CTestCalInterimApiDeleteAgendaServers::doTestStepPreambleL()
	{
	return TestStepResult();
	}

/**
Base class pure virtual
Deletes the agenda server if it has been started. 
*/
TVerdict CTestCalInterimApiDeleteAgendaServers::doTestStepL() 
	{
	_LIT(KAgendaServer,"agsvexe*");
	TRAPD(err, GetPIMTestServerL().KillProcessL(KAgendaServer));
	if((err != KErrNone) && (err != KErrNotFound))
		{
		User::Leave(err);
		}

	return TestStepResult();
	}

