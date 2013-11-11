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



#ifndef TEST_CALINTERIMAPI_DELETE_INSTANCES_H
#define TEST_CALINTERIMAPI_DELETE_INSTANCES_H

#include "TestCalInterimApiSuiteStepBase.h"

#include <calcommon.h>
#include <calinstanceview.h>
#include <calinstance.h>

class CCalInstanceView;

/**
This test step finds the instances of the entry and then deletes the instances
based upon the recurrence range of the entry
*/
class CTestCalInterimApiDeleteInstances : public CTestCalInterimApiSuiteStepBase 
	{
public:
	CTestCalInterimApiDeleteInstances();
	~CTestCalInterimApiDeleteInstances();

	virtual TVerdict	doTestStepL();

protected:
	void	FindTheInstancesAndDeleteL(const TInt& aExpectedCountOfInstances);
	void	CompareInstanceDateTimeAndDeleteL(RPointerArray<CCalInstance>& aInstanceArray);
	void	PrintInstancesDateTimeL(RPointerArray<CCalInstance>& aInstanceArray);
	void	PerformOOMTestsForDeleteInstancesL(CCalInstance* aInstance, CalCommon::TRecurrenceRange aRecurrenceRange);
	CalCommon::TRecurrenceRange	ConvertToRecurrenceRange(const TPtrC& aRecurrenceRange);

private:
	CCalInstanceView*	iInstanceView;
	TCalTime			iCalStartDateTime;
	TCalTime			iCalEndDateTime;
	TCalTime			iInstanceDateTime;
	TPtrC				iRecurrenceRange;
	TInt				iFilter;
	TBool				iAllocTest;
	};

_LIT(KTestCalInterimApiDeleteInstances, "TestCalInterimApiDeleteInstances");

#endif
