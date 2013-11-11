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

#include "TestCalInterimApiModifiedLocalIds.h"
#include <caltime.h>

_LIT(KNULL,				"NULL");
_LIT(KMIN,				"MIN");
_LIT(KMAX,				"MAX");

/* Class CTestCalInterimApiModifiedLocalIds  - This class verifies that the modified local ids returned are correct.
 The script TestCalModifiedLocalIds.script modifies parent and child entries. */
CTestCalInterimApiModifiedLocalIds::CTestCalInterimApiModifiedLocalIds()
:	iEntryView(NULL)
	{
	SetTestStepName(KTestCalInterimApiModifiedLocalIds);
	}
	
CTestCalInterimApiModifiedLocalIds::~CTestCalInterimApiModifiedLocalIds()
	{
	delete iEntryView;
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
@test
*/
TVerdict CTestCalInterimApiModifiedLocalIds::doTestStepL() 
	{
	TESTL(TestStepResult() == EPass);
	INFO_PRINTF1(_L("CTestCalInterimApiModifiedLocalIds::doTestStepL() start"));

	iField.Zero();
	iField.Append(KLocalIds);
	TESTL(GetIntFromConfig(ConfigSection(),iField,iIntLocalIds)); // gets local ids to be returned
	
	RArray<TCalLocalUid>	calLocalIds;
	CleanupClosePushL(calLocalIds);

	iEntryView = CCalEntryView::NewL(GetSession(), *this);
	CActiveScheduler::Start();
	
	iField.Zero();
	iField.Append(KSoak);
	GetStringFromConfig(ConfigSection(), iField, iPtrSoak); // determines wether to carry out soak tests

	iField.Zero();
	iField.Append(KSystemTime);
	TESTL(GetStringFromConfig(ConfigSection(), iField, iPtrSystemTime)); // gets the recorded system time

	// Soak testing
	if (iPtrSoak != KNullDesC)
		{
		iField.Zero();
		iField.Append(KNumberOfCalls);
		GetIntFromConfig(ConfigSection(), iField, iRecurCount); // determines wether to carry out soak tests	
		
		for (TInt index = 0; index < iRecurCount; index++)
			{
			calLocalIds.Reset();
			INFO_PRINTF2(_L("Call %d GetIdsModifiedSinceDateL"), index);
			TCalTime	calTime;
			calTime.SetTimeLocalL(FormatDateTime(iPtrSystemTime));
			iEntryView->GetIdsModifiedSinceDateL(calTime, calLocalIds);
			}
		}
	else
		{
		TCalTime	calTime;
		if (!iPtrSystemTime.Compare(KNULL()))
			{
			calTime.SetTimeLocalL(Time::NullTTime());
			}
		else if (!iPtrSystemTime.Compare(KMIN())) //TCalTime::minTime
			{
			TTime	mintime (TCalTime::MinTime());
			calTime.SetTimeLocalL(mintime);
			}
		else if (!iPtrSystemTime.Compare(KMAX()))
			{
			TTime	maxtime (TCalTime::MaxTime());
			calTime.SetTimeLocalL(maxtime);
			}
		else
			{
			calTime.SetTimeLocalL(FormatDateTime(iPtrSystemTime));
			}
		
		// Get the Local Ids modified since the specified date and time
		iEntryView->GetIdsModifiedSinceDateL(calTime, calLocalIds);
		}
		
	// Test if the number of localIds returned by the API is as expected
	TESTL(iIntLocalIds == calLocalIds.Count());

	CleanupStack::PopAndDestroy(&calLocalIds);
	
	INFO_PRINTF1(_L("CTestCalInterimApiModifiedLocalIds::doTestStepL() finish"));
	return TestStepResult();
	}

