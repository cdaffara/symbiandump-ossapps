/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __TEST_CALINTERIMAPI_DELETE_ALARMS_H__
#define __TEST_CALINTERIMAPI_DELETE_ALARMS_H__

#include "TestCalInterimApiSuiteStepBase.h"

/**
 * Teststep that connects to the alarm server, and deletes all the Calendar
 * alarms queued with it. Can be used for cleanup purposes before and/or after
 * an alarm test
 */
class CTestCalInterimApiDeleteAlarms: public CTestCalInterimApiSuiteStepBase
	{
public:
	CTestCalInterimApiDeleteAlarms();
	~CTestCalInterimApiDeleteAlarms();
	
	TVerdict 		doTestStepPreambleL();
	TVerdict 		doTestStepL();

private:	

public:
	};

_LIT(KTestCalInterimApiDeleteAlarms,"TestCalInterimApiDeleteAlarms");

#endif // __TEST_CALINTERIMAPI_DELETE_ALARMS_H__
