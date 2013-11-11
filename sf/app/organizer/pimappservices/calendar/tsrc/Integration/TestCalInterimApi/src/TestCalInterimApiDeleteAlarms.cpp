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

#include "TestCalInterimApiDeleteAlarms.h"
#include <calalarm.h>
#include <asclisession.h>


/**
 * Destructor
 */
CTestCalInterimApiDeleteAlarms::~CTestCalInterimApiDeleteAlarms()
	{
	}

/**
 * Constructor
 */
CTestCalInterimApiDeleteAlarms::CTestCalInterimApiDeleteAlarms()
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestCalInterimApiDeleteAlarms);
	}

/**
 * To bypass base class implementation 
 */
TVerdict CTestCalInterimApiDeleteAlarms::doTestStepPreambleL()
	{
	return TestStepResult();
	}

/**
Base class pure virtual
Deletes all the calendar alarms queued with the alarm server
Can be used for cleanup before or after alarm tests
*/
TVerdict CTestCalInterimApiDeleteAlarms::doTestStepL() 
	{
	RASCliSession	alarmSession;
	CleanupClosePushL(alarmSession); 
	
	TESTL(alarmSession.Connect() == KErrNone);
	TESTL(alarmSession.AlarmDeleteAllByCategory(KUidAgendaModelAlarmCategory, EFalse) == KErrNone);
	TESTL(alarmSession.AlarmDeleteAllByCategory(TUid::Null(), EFalse) == KErrNone);
	CleanupStack::PopAndDestroy(&alarmSession);

	return TestStepResult();
	}

