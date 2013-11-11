/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __TEST_CAL_INTERIM_API_FILTER_H__
#define __TEST_CAL_INTERIM_API_FILTER_H__

#include "TestCalInterimApiSuiteStepBase.h"

#include <calcommon.h>

class CCalInstanceView;
class CCalInstance;
class CCalEntry;

/**
Test step that does a FindInstanceL search based on the data passed from ini
and checks the instances returned against the expected instances in ini if any
*/
class CTestCalInterimApiFilter : public CTestCalInterimApiSuiteStepBase
	{
public:

	CTestCalInterimApiFilter();
	virtual ~CTestCalInterimApiFilter();

	virtual TVerdict	doTestStepL();

private:
	void	FilterEntriesL(CalCommon::TCalViewFilter aFilter, TInt aExpectedCount, TBool aAllocTest, const TPtrC& aExpectedInstances);
	void 	CheckInstancesL(RPointerArray<CCalInstance>& aInstanceArray, RArray<TPtrC>& aSections);
	void 	PrintAllInstancesL(RPointerArray<CCalInstance>& aInstanceArray);
	void 	PerformSearchL(RPointerArray<CCalInstance>& aInstanceArray, CalCommon::TCalViewFilter aFilter, CalCommon::TCalTimeRange aTimeRange, const TPtrC& aSearchText, CalCommon::TCalSearchBehaviour aBehaviour);
	CalCommon::TCalSearchBehaviour GetBehaviourL(const TPtrC aSearchBehaviour);
	void 	TestEntryRefCountFeatureL(RPointerArray<CCalInstance>& aInstanceArray);
	void 	TestEntryPropertiesL(CCalEntry* entry1, CCalEntry* entry2);
	void	PerformanceTestInstanceViewL(TReal32& aMaxTimeLimit);

private:

	/** Start time for calculating the time range for a search */
	TCalTime						iStartDate;

	/** End time for calculating the time range for a search */
	TCalTime						iEndDate;

	/** Instance View */
	CCalInstanceView*				iInstanceView;

	};

_LIT(KTestCalInterimApiFilter,"TestCalInterimApiFilter");

#endif
