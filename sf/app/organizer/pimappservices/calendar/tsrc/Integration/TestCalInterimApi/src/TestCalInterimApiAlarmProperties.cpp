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

#include "TestCalInterimApiAlarmProperties.h"
#include <calentryview.h>

_LIT(KGUID,					"guid");
_LIT(KRecurrenceId,			"recurrenceid");
_LIT(KAlarmSoundName,		"alarmsoundname");
_LIT(KDisposition,			"disposition");
_LIT(KMimeType,				"mimetype");
_LIT(KContent,				"content");
_LIT(KOffset,				"offset");

/**
Constructor. Sets the test step name. Testserver reference passed to make use 
of TEF's method of sharing data between test steps
*/
CTestCalInterimApiAlarmProperties::CTestCalInterimApiAlarmProperties() 
	{
	//Call base class method to set human readable name for test step
	SetTestStepName(KTestCalInterimApiAlarmProperties);
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
*/
TVerdict CTestCalInterimApiAlarmProperties::doTestStepL()
	{
	TPtrC	gUID;
	TPtrC	recurrenceId;
	TPtrC	alarmSoundName;
	TPtrC	disposition;
	TPtrC	mimeType;
	TPtrC	content;
	TInt	offset(0);
	TTime	recurrenceIdValue;
	
	TESTL(GetStringFromConfig(ConfigSection(), KGUID, gUID));
	
	GetStringFromConfig(ConfigSection(), KRecurrenceId, recurrenceId);
	GetStringFromConfig(ConfigSection(), KAlarmSoundName, alarmSoundName);
	GetStringFromConfig(ConfigSection(), KDisposition, disposition);
	GetStringFromConfig(ConfigSection(), KMimeType, mimeType);
	GetStringFromConfig(ConfigSection(), KContent, content);
	GetIntFromConfig   (ConfigSection(), KOffset, offset);
	
	CCalEntryView*	entryView = CCalEntryView::NewL(GetSession(), *this);
	CActiveScheduler::Start();
	CleanupStack::PushL(entryView);
	
	HBufC8*	gUID8 = HBufC8::NewLC(gUID.Length());
	gUID8->Des().Copy(gUID);
	
	RPointerArray<CCalEntry>	entriesToFetch;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entriesToFetch));
	entryView->FetchL(*gUID8, entriesToFetch);
	
	// Get the entry we are interested in
	TCalTime	recurrenceIdTime;
	if(recurrenceId == KNullDesC)
		{
		recurrenceIdTime.SetTimeUtcL(Time::NullTTime());
		}
	else
		{
		recurrenceIdValue.Set(recurrenceId);
		recurrenceIdTime.SetTimeUtcL(recurrenceIdValue);
		}
		
	CCalEntry*	entry = GetMatchingEntryL(entriesToFetch, recurrenceIdTime);
	
	TESTL(entry != NULL);
	
	TestAlarmPropertiesL(entry, offset, alarmSoundName, disposition, mimeType, content);
	
	CleanupStack::PopAndDestroy(3, entryView); // guid8, entryview, entriestofetch
	
	return TestStepResult();	
	}
	
/**
Checks the properties of an entry's alarm like,offset, alarmsoundname and attachment
*/
void CTestCalInterimApiAlarmProperties::TestAlarmPropertiesL(CCalEntry* aEntry, TInt aTimeOffset, const TDesC& aAlarmSoundName, const TDesC& aDisposition, const TDesC& aMimeType, const TDesC& aContent)
	{
	// Alarm fetched from the entry
	CCalAlarm*	alarm1 = aEntry->AlarmL();
	CleanupStack::PushL(alarm1);
	
	// We construct an alarm with data from the ini
	CCalAlarm*	alarm2 = CCalAlarm::NewL();
	CleanupStack::PushL(alarm2);
	alarm2->SetTimeOffset(aTimeOffset);
	alarm2->SetAlarmSoundNameL(aAlarmSoundName);
	
	if(aContent != KNullDesC)
		{
		HBufC8*	content8 = HBufC8::NewLC(aContent.Length());
		content8->Des().Copy(aContent);
		HBufC8*	mimeType8 = HBufC8::NewLC(aMimeType.Length());
		mimeType8->Des().Copy(aMimeType);
		CCalContent::TDisposition	disposition;
		User::LeaveIfError(ConvertToDisposition(disposition, aDisposition));
		alarm2->SetAlarmAction(CreateCalContentL(content8, mimeType8, disposition));
		CleanupStack::Pop(2, content8); // mimeType8, content8
		}
	
	// Compare both the alarms
	CompareAlarmsL(alarm1, alarm2);
	
	CleanupStack::PopAndDestroy(2, alarm1);	// alarm1, alarm2
	}


