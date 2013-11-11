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

#include "TestCalInterimApiCheckAlarm.h"

#include <asshdalarm.h>
#include <calentryview.h>

_LIT(KGUID,					"guid");
_LIT(KInstanceTime,			"instancetime");

_LIT(KInfoGUID,				"GUID: %S");
_LIT(KInfoAlarmNotified,	"Instance's alarm notified as expected");
_LIT(KErrAlarmNotNotified,	"Instance's alarm NOT notified ");
_LIT(KAlarmCount,			"Alarm count = %D");
_LIT(KNoAlarmData,			"Alarm ID %D does not have Alarm Data!");
_LIT(KTestStepError,		"Error occured in Test Step %S: %D");
	
/**
 * Destructor 
 */
CTestCalInterimApiCheckAlarm::~CTestCalInterimApiCheckAlarm()
	{
	iAlarmServer.Close();
	}

/**
 * Constructor
 */
CTestCalInterimApiCheckAlarm::CTestCalInterimApiCheckAlarm()
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestCalInterimApiCheckAlarm);
	}

TVerdict CTestCalInterimApiCheckAlarm::doTestStepL()
	{
	TRAPD(err, DoTestsL());
	
	if(err != KErrNone)
		{
		ERR_PRINTF3(KTestStepError, &TestStepName(), err);
		SetTestStepError(err);
		}
		
	return TestStepResult();
	}

/**
Retreives the cal entry data stored by alarm server for the alarm
*/
CCalEntryId* CTestCalInterimApiCheckAlarm::GetEntryIdL(TAlarmId aAlarmId)	
	{
	HBufC8*	data8 = NULL;
	
	// Get the data stored with the alarm
	TInt err = iAlarmServer.GetAlarmData(aAlarmId, data8);
	
	User::LeaveIfError(err);	
	
	CleanupStack::PushL(data8);
	
	// Get the entry details of this alarm
	CCalEntryId*	entryId = CCalEntryId::NewL(data8); 
	
	// CCalEntryId takes ownership of data8.
	CleanupStack::Pop(data8);
	return entryId;
	}
	
void CTestCalInterimApiCheckAlarm::DoTestsL()
	{
	// Let agenda build the index, and in due course, queue the alarms
	CCalEntryView*	entryView = CCalEntryView::NewL(GetSession(), *this);
	CActiveScheduler::Start();
	delete entryView;
	
	User::After(6000000);
		
	TPtrC gUID;
	TPtrC instanceTime;
	TESTL(GetStringFromConfig(ConfigSection(), KGUID, gUID));
	TESTL(GetStringFromConfig(ConfigSection(), KInstanceTime, instanceTime));
	
	HBufC8*	gUID8 = HBufC8::NewLC(gUID.Length());
	gUID8->Des().Copy(gUID);
	
	// Connect to alarm server and wait for some time
	User::LeaveIfError(iAlarmServer.Connect());
	RArray<TAlarmId> alarmIds;
	
	iAlarmServer.GetAlarmIdListL(alarmIds);
	TInt	alarmIdsCount = alarmIds.Count();
	INFO_PRINTF2(KAlarmCount, alarmIdsCount);
	
	TBool	isExpectedInstance = EFalse;
	for(TInt count = 0; count < alarmIdsCount && !isExpectedInstance; ++count)
		{
		TASShdAlarm	alarm;
		TInt	err = iAlarmServer.GetAlarmDetails(alarmIds[count], alarm);
		User::LeaveIfError(err);
		TAlarmState	state = alarm.State();
		
		// Check the notifying alarm(s). See if one of them is what we are expecting
		if(state == EAlarmStateNotifying || state == EAlarmStateNotified 
		|| state == EAlarmStateSnoozed   || state == EAlarmStateWaitingToNotify)
			{// This alarm is a notified one, either showing or sleeping
			CCalEntryId*	entryId = NULL;
			TRAPD(err, entryId = GetEntryIdL(alarmIds[count]));
			
			if(err == KErrNotFound)
				{
				// It is possible that some other test step created
				// a non-calendar alarm. This will not have data, just
				// log it and continue
				INFO_PRINTF2(KNoAlarmData, alarmIds[count]);
				continue;
				}
			User::LeaveIfError(err);	
			
			CleanupStack::PushL(entryId);
			
			// Test if the entry details are what we expect
			isExpectedInstance  = TestInstanceL(entryId, *gUID8, instanceTime);
			
			if(isExpectedInstance)
				{
				// This alarm is the one we expected. 
				// We are done, delete this alarm, and make way for the others
				User::LeaveIfError(iAlarmServer.AlarmDelete(alarm.Id()));
				}
			CleanupStack::PopAndDestroy(entryId);
			}
		}
	CleanupStack::PopAndDestroy(gUID8);
	
	if (isExpectedInstance)
		{
		INFO_PRINTF1(KInfoAlarmNotified);
		}
	else
		{
		INFO_PRINTF1(KErrAlarmNotNotified);
		SetTestStepResult(EFail);
		}	
	}

/**
Checks the instance data corresponding the alarm, against the ini data
*/
TBool CTestCalInterimApiCheckAlarm::TestInstanceL(CCalEntryId* aEntryId, const TDesC8& aGUID8, const TDesC& aInstanceTime)
	{
	TTime	time1(aInstanceTime);	
	TTime	time2 = aEntryId->InstanceTimeL().TimeLocalL();
	
	// Log the instance whose alarm has notified.
	HBufC*	foundGUID = HBufC::NewLC(aEntryId->IdL().Length());
	foundGUID->Des().Copy(aEntryId->IdL());
	INFO_PRINTF2(KInfoGUID, foundGUID);
	CleanupStack::PopAndDestroy(foundGUID);
	PrintTimeL(time2);
		
	// Check
	if(!aEntryId)
		{
		return EFalse;
		}
	if(aEntryId->IdL() != aGUID8)
		{
		return EFalse;
		}
	if(time1 != time2)
		{
		return EFalse;
		}
		
	return ETrue;	
	}
