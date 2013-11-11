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



#ifndef TEST_CALINTERIMAPI_FETCHENTRY_ANDCHECKDATA_H
#define TEST_CALINTERIMAPI_FETCHENTRY_ANDCHECKDATA_H

#include "TestCalInterimApiSuiteStepBase.h"

#include <calentry.h>

class CCalEntryView;

/**
This test step fetches the entry from the calendar file and check the data that 
is given in the structure which is declared in the base class.
*/
class CTestCalInterimApiFetchEntryAndCheckData : public CTestCalInterimApiSuiteStepBase
	{
public:
	CTestCalInterimApiFetchEntryAndCheckData();
	~CTestCalInterimApiFetchEntryAndCheckData();

	virtual TVerdict	doTestStepL();

private:
	void	FetchAndCheckDataL(const TPtrC& aGuid, const TInt& aExpectedCount, const TEntryProperties& aEntryProperties, const TPtrC& aTimeMode);
	void	CheckDataL(CCalEntry* aCalEntry, const TEntryProperties& aEntryProperties);
	void	CheckCategoriesL(CCalEntry* aCalEntry, const TDesC& aCategories);
	void    CheckGEOPropertyL(CCalEntry* aCalEntry, const TDesC& aGeoLat, const TDesC& aGeoLong);
	void	SoakTestsForFetchEntryL(const TPtrC& aGuid, const TBool& aSoakTest, const TInt& aSoakTestCount);
	void	CheckAttendeesL(CCalEntry* aCalEntry);
	void	CheckRDatesL(CCalEntry* aCalEntry, const TEntryProperties& aEntryProperties);
	void	CheckExDatesL(CCalEntry* aCalEntry,const TEntryProperties& aEntryProperties);

private:
	TCalTime		iRecurrenceId;
	CCalEntryView*	iCalEntryView;
	};

_LIT(KTestCalInterimApiFetchEntryAndCheckData, "TestCalInterimApiFetchEntryAndCheckData");

#endif
