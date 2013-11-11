// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TestCalInterimApiSetSysTime.h"

#include <tz.h>

_LIT(KTime,	"time");

/**
 * Destructor
 */
CTestCalInterimApiSetSysTime::~CTestCalInterimApiSetSysTime()
	{
	}

/**
 * Constructor
 */
CTestCalInterimApiSetSysTime::CTestCalInterimApiSetSysTime()
	{
	SetTestStepName(KTestCalInterimApiSetSysTime);
	}

TVerdict CTestCalInterimApiSetSysTime::doTestStepPreambleL()
	{
	_LIT(KKillProcess, "killprocess");
	TBool killProcess = EFalse;
	GetBoolFromConfig(ConfigSection(), KKillProcess, killProcess);

	if(killProcess)
		{
		_LIT(KAgendaServer,"agsvexe*");
		TRAP_IGNORE(GetPIMTestServerL().KillProcessL(KAgendaServer));
		}
	return CTestCalInterimApiSuiteStepBase::doTestStepPreambleL();
	}

/**
 * Base class pure virtual
 * Sets the system time to a value passed from the ini
 */
TVerdict CTestCalInterimApiSetSysTime::doTestStepL() 
	{
	// Connect to the TZ server and set auto update to on, as needed
	// by the DST tests. ETZAutoDSTUpdateOn enables automatic DST update
	// RPIMTestServer currently does not provide this facility
	RTz	tz;
	CleanupClosePushL(tz);
	User::LeaveIfError(tz.Connect());
	tz.SetAutoUpdateBehaviorL(RTz::ETZAutoDSTUpdateOn);
	
	TPtrC	timeStr;
	TESTL(GetStringFromConfig(ConfigSection(), KTime, timeStr));
	TTime	time(timeStr);
	
	// Set the time. We already have a conncetion to TZ.
	TInt	err = tz.SetHomeTime(time);
	SetTestStepError(err);
	CleanupStack::PopAndDestroy(&tz);
	
	PrintHomeTimeL();
	
	return TestStepResult();
	}

