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

#include "TestCalInterimApiSetTz.h"

_LIT(KTimeZone, "timezone");
_LIT(KInfoNewTz, "New timezone = %S");

/**
 * Destructor
 */
CTestCalInterimApiSetTz::~CTestCalInterimApiSetTz()
	{
	}

/**
 * Constructor
 */
CTestCalInterimApiSetTz::CTestCalInterimApiSetTz()
	{
	SetTestStepName(KTestCalInterimApiSetTz);
	}

/**
Base class pure virtual
Sets the current timezone to a value passed from the ini
*/
TVerdict CTestCalInterimApiSetTz::doTestStepL() 
	{
	TPtrC	timeZone;
	GetStringFromConfig(ConfigSection(), KTimeZone, timeZone);
	
	HBufC8*	timeZone8 = HBufC8::NewLC(timeZone.Length());
	timeZone8->Des().Copy(timeZone);
	
	// Set the time zone
	GetPIMTestServerL().SetTimeZoneL(*timeZone8);
	
	TBuf8<256>	newTimeZone8;
	GetPIMTestServerL().GetTimeZoneL(newTimeZone8);
	
	// INFO_PRINTF understands only 16-bit
	HBufC*	newTimeZone16 = HBufC::NewLC(newTimeZone8.Length());
	newTimeZone16->Des().Copy(newTimeZone8);
	INFO_PRINTF2(KInfoNewTz, newTimeZone16);
	
	TESTL(timeZone8->Des() == newTimeZone8);
		
	CleanupStack::PopAndDestroy(2, timeZone8); // timeZone8, newTimeZone16
	
	PrintHomeTimeL();
	
	return TestStepResult();
	}

