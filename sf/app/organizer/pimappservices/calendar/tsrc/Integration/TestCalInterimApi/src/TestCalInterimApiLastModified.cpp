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

#include "TestCalInterimApiLastModified.h"

// Class CTestCalInterimApiLastModified // This class fetched all entries from database, and verifies the Last Modified Date/Time
CTestCalInterimApiLastModified::CTestCalInterimApiLastModified()
	{
	SetTestStepName(KTestCalInterimApiLastModified);
	}

CTestCalInterimApiLastModified::~CTestCalInterimApiLastModified()
	{
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
@test
*/
TVerdict CTestCalInterimApiLastModified::doTestStepL()	
	{
	TESTL(TestStepResult() == EPass);
	INFO_PRINTF1(_L("CTestCalInterimApiLastModified::doTestStepL() start"));
	
	RPointerArray<CCalEntry> fetchEntryArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &fetchEntryArray));
	
	iEntryView = CCalEntryView::NewL(GetSession(), *this);
	
	CActiveScheduler::Start();

	FetchAllEntryFromDatabaseL(fetchEntryArray);

	for (TInt index = 0; index < fetchEntryArray.Count(); index++)
		{
		INFO_PRINTF2(_L("Checking Entry %d"), index+1);
		iField.Zero();
		iField.Append(KUidEntry);
		iField.AppendNum(index + 1);
		TESTL(GetStringFromConfig(ConfigSection(),iField,iPtrEntryUid));	 // gets the entries uid
	
		HBufC8* entryGuid = HBufC8::NewLC(iPtrEntryUid.Length());
		entryGuid->Des().Copy(iPtrEntryUid);
	
		iField.Zero();
		iField.Append(KExpectedLastModified);
		iField.AppendNum(index+1);
		TESTL(GetStringFromConfig(ConfigSection(),iField,iPtrExpectedLastModified));	 // gets expected last modified date/time of entry
	
		const TDesC8& entryUid1 = fetchEntryArray[index]->UidL();
		TESTL(!entryGuid->Des().Compare(entryUid1));
		
		TDateTime lastModifiedTime = fetchEntryArray[index]->LastModifiedDateL().TimeUtcL().DateTime();
		INFO_PRINTF7(_L("last-modified Date Year Of Stored Entry: %d, Month: %d, Day: %d, Time Hr: %d, Min: %d, Sec: %d "), lastModifiedTime.Year(), lastModifiedTime.Month(), lastModifiedTime.Day(), lastModifiedTime.Hour(), lastModifiedTime.Minute(), lastModifiedTime.Second());
		
		TTime expectedLastTime = FormatDateTime(iPtrExpectedLastModified);
		
		TDateTime expected = expectedLastTime.DateTime();
		INFO_PRINTF7(_L("expected last-modified Date Year Of Stored Entry: %d, Month: %d, Day: %d, Time Hr: %d, Min: %d, Sec: %d "), expected.Year(), expected.Month(), expected.Day(), expected.Hour(), expected.Minute(), expected.Second());
		
		TTimeIntervalSeconds secs;
		fetchEntryArray[index]->LastModifiedDateL().TimeUtcL().SecondsFrom(expectedLastTime, secs);
		
		//time delay 
		TESTL(secs.Int() < KTimeDelay && secs.Int() >= (KTimeDelay-KTimeDelay));
		
		INFO_PRINTF1(_L("Last Modified Date/Time of entry is correct"));
		
		CleanupStack::PopAndDestroy(entryGuid);
		}
	
	CleanupStack::PopAndDestroy(&fetchEntryArray);// entryArray.reset & entryArray.destroy;	
	INFO_PRINTF1(_L("CTestCalInterimApiLastModified::doTestStepL() finish"));
	return TestStepResult();
	}

