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



#ifndef __TEST_CALINTERIMAPI_SET_SYS_TIME_H__
#define __TEST_CALINTERIMAPI_SET_SYS_TIME_H__

#include "TestCalInterimApiSuiteStepBase.h"

/**
 * Teststep that sets the system time to a time mentioned in the ini
 * Assumes that the time mentioned in the INI is local to the current
 * timezone
 */
class CTestCalInterimApiSetSysTime: public CTestCalInterimApiSuiteStepBase
	{
public:
	CTestCalInterimApiSetSysTime();
	~CTestCalInterimApiSetSysTime();
	TVerdict doTestStepPreambleL();
	TVerdict 		doTestStepL();
	};

_LIT(KTestCalInterimApiSetSysTime,"TestCalInterimApiSetSysTime");

#endif // __TEST_CALINTERIMAPI_SET_SYS_TIME_H__
