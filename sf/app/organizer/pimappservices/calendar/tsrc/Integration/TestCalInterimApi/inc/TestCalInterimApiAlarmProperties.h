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



#ifndef __TEST_CALINTERIMAPI_ALARM_PROPERTIES_H__
#define __TEST_CALINTERIMAPI_ALARM_PROPERTIES_H__


#include "TestCalInterimApiSuiteStepBase.h"

class CCalEntry;

/*@{*/
/// Literal constant defined for identifying step name
_LIT(KTestCalInterimApiAlarmProperties, "TestCalInterimApiAlarmProperties");
/*@}*/

/**
Test the properties of an alarm like, offset, soundname, attachment etc.
*/
class CTestCalInterimApiAlarmProperties : public CTestCalInterimApiSuiteStepBase
	{
public:
	CTestCalInterimApiAlarmProperties();

	// TEF pure virtual
	TVerdict 				doTestStepL();
	
private:
	void 					TestAlarmPropertiesL(CCalEntry* aEntry, TInt aTimeOffset, const TDesC& aAlarmSoundName, const TDesC& aDisposition, const TDesC& aMimeType, const TDesC& aContent);
	
	};

#endif		// __TEST_CALINTERIMAPI_ALARM_PROPERTIES_H__
