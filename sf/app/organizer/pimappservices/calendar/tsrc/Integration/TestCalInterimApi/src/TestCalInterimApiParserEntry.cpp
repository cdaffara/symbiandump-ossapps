// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TestCalInterimApiParserEntry.h"
#include "TestCalInterimApiParserAttendee.h"
#include "TestCalInterimApiParserRRule.h"
#include "TestCalInterimApiParserAlarm.h"
#include "TestCalInterimApiParserAttachment.h"
#include "TestCalInterimApiSuiteStepBase.h"

#include <f32file.h>
#include <bautils.h>
#include <calentry.h>
#include <calcategory.h>
#include <calalarm.h>
#include <caluser.h>
#include <e32test.h>
#include <calattachment.h>

_LIT(KAllocTest,				"alloctest");
_LIT(KCmdDtStart,				"DTSTART");
_LIT(KCmdDtEnd,					"DTEND");
_LIT(KCmdDue,					"DUE");
_LIT(KCmdCompleted,				"COMPLETED");
_LIT(KCmdSummary,				"SUMMARY");
_LIT(KCmdLocation,				"LOCATION");
_LIT(KCmdCategories,			"CATEGORIES");
_LIT(KCmdPriority,				"PRIORITY");
_LIT(KCmdStatus,				"STATUS");
_LIT(KCmdDescription,			"DESCRIPTION");
_LIT(KCmdAlarm,					"ALARM");
_LIT(KCmdOrganizer,				"ORGANIZER");
_LIT(KCmdAttendee,				"ATTENDEE");
_LIT(KCmdRRule,					"RRULE");
_LIT(KCmdClearRepeat,			"CLEARREPEAT");
_LIT(KCmdRDate,					"RDATE");
_LIT(KCmdExDate,				"EXDATE");
_LIT(KCmdTransp,				"TRANSP");
_LIT(KCmdGEOLat,				"GEOLAT");
_LIT(KCmdGEOLong,				"GEOLONG");
_LIT(KCmdDTStamp,				"DTSTAMP");
_LIT(KCmdReplicationStatus,		"REPLICATIONSTATUS");
_LIT(KCmdLocalId,				"LOCALID");
_LIT(KCmdAttachType,			"ATTACHTYPE");
_LIT(KCmdEntryAttachment,		"ENTRYATTACH");
_LIT(KStatusTentative,			"ETentative");
_LIT(KStatusConfirmed,			"EConfirmed");
_LIT(KStatusCancelled,			"ECancelled");
_LIT(KStatusTodoNeedsAction,	"ETodoNeedsAction");
_LIT(KStatusTodoCompleted,		"ETodoCompleted");
_LIT(KStatusTodoInProcess,		"ETodoInProcess");
_LIT(KFile,						"EFile");
_LIT(KUri,						"EURI");

_LIT(KStatusPrivate,			"private");
_LIT(KStatusRestricted,			"restricted");

_LIT(KInfoSetStatus,			"SetStatus %d");

_LIT(KErrMultipleDtStart,		"Multiple DTSTART");
_LIT(KErrMultipleDtEnd,			"Multiple DTEND");
_LIT(KErrMultipleDue,			"Multiple DUE");
_LIT(KErrMultipleCompleted,		"Multiple COMPLETED");
_LIT(KErrMultipleSummary,		"Multiple SUMMARY");
_LIT(KErrMultipleLocation,		"Multiple LOCATION");
_LIT(KErrMultiplePriority,		"Multiple PRIORITY");
_LIT(KErrMultipleStatus,		"Multiple STATUS");
_LIT(KErrUnknowStatus,			"Unknow status %S");
_LIT(KErrMultipleDescription,	"Multiple DESCRIPTION");
_LIT(KErrMultipleAlarm,			"Multiple ALARM");
_LIT(KErrMultipleOrganizer,		"Multiple ORGANIZER");
_LIT(KErrMultipleRRule,			"Multiple RRULE");
_LIT(KErrMultipleDTStamp,		"Multiple DTSTAMP");
_LIT(KErrMultipleReplicationStatus,"Multiple REPLICATION STATUS");
_LIT(KErrMultipleLocalId,		"Multiple LOCALUID");
_LIT(KErrInvalidDateTime,		"Invalid date and time");
_LIT(KErrNoAttachType,			"No Attachment Type");

_LIT(KWarnExceptionDate,		"Exception date cannot be set for a non repeating entry");
_LIT(KWarnCompleted,			"CompletedL Should not be called for entry types other than TODO");
_LIT(KWarnTimeMode,				"It doesn't make sense for the completed time to be floating");
_LIT(KWarnStatusNotSupported,	"Status is not supported");
_LIT(KWarnSetPriority,			"Priority greater than 255 and less than 0 is not allowed");


CTestCalInterimApiParserEntry::CTestCalInterimApiParserEntry(CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallback, TCalTime::TTimeMode aTimeMode, RFs* aFs)
:	CTestCalInterimApiParserBase(aTestStep, aParserCallback)
,	iEntry(aEntry)
,	iTimeMode(aTimeMode)
,	iAttachType(CCalAttachment::EFile)
,	iFs(aFs)
,	iHasDtStart(EFalse)
,	iHasDtEnd(EFalse)
,	iHasDue(EFalse)
,	iHasCompleted(EFalse)
,	iHasSummary(EFalse)
,	iHasLocation(EFalse)
,	iHasPriority(EFalse)
,	iHasStatus(EFalse)
,	iHasDescription(EFalse)
,	iHasAlarm(EFalse)
,	iHasOrganizer(EFalse)
,	iHasRRule(EFalse)
,	iHasTransp(EFalse)
,	iGEOLat(0)
,	iHasGEOLat(EFalse)
,	iHasGEOLong(EFalse)
,	iHasDTStamp(EFalse)
,	iHasReplicationStatus(EFalse)
,	iHasLocalId(EFalse)
,	iHasAttachType(EFalse)
	{
	}

CTestCalInterimApiParserEntry::~CTestCalInterimApiParserEntry()
	{
	iRDateArray.Close();
	iExDateArray.Close();
	}

TInt CTestCalInterimApiParserEntry::ParseL(const TDesC& aCommand, const TDesC& aData)
	{
	TInt	ret = KErrNone;

	if ( aCommand == KCmdDtStart )
		{
		ProcessDtStartL(aData);
		}
	else if ( aCommand == KCmdDtEnd )
		{
		ProcessDtEndL(aData);
		}
	else if ( aCommand == KCmdDue )
		{
		ProcessDueL(aData);
		}
	else if ( aCommand == KCmdDTStamp )
		{
		ProcessDTStampL(aData);
		}
	else if ( aCommand == KCmdCompleted)
		{
		ProcessCompletedL(aData);
		}
	else if ( aCommand == KCmdSummary )
		{
		ProcessSummaryL(aData);
		}
	else if ( aCommand == KCmdLocation )
		{
		ProcessLocationL(aData);
		}
	else if ( aCommand == KCmdCategories )
		{
		ProcessCategoriesL(aData);
		}
	else if ( aCommand == KCmdPriority )
		{
		ProcessPriorityL(aData);
		}
	else if ( aCommand == KCmdLocalId )
		{
		ProcessLocalIdL(aData);
		}
	else if ( aCommand == KCmdStatus )
		{
		ProcessStatusL(aData);
		}
	else if ( aCommand == KCmdReplicationStatus )
		{
		ProcessReplicationStatusL(aData);
		}
	else if ( aCommand == KCmdDescription )
		{
		ProcessDescriptionL(aData);
		}
	else if ( aCommand == KCmdAlarm )
		{
		ProcessAlarmL(aData);
		}
	else if ( aCommand == KCmdOrganizer )
		{
		ProcessOrganizerL(aData);
		}
	else if ( aCommand == KCmdAttendee )
		{
		ProcessAttendeeL(aData);
		}
	else if ( aCommand == KCmdRRule )
		{
		ProcessRRuleL(aData);
		}
	else if ( aCommand == KCmdRDate )
		{
		ProcessRDateL(aData);
		}
	else if ( aCommand == KCmdExDate )
		{
		ProcessExDateL(aData);
		}
	else if ( aCommand == KCmdTransp )
		{
		ProcessTranspL(aData);
		}
	else if ( aCommand == KCmdGEOLat )
		{
		ProcessGEOLatL(aData);
		}
	else if ( aCommand == KCmdGEOLong )
		{
		ProcessGEOLongL(aData);
		}
	else if ( aCommand == KCmdClearRepeat )
		{
		ProcessClearRepeatPropertyL(aData);
		}
	else if ( aCommand == KCmdAttachType )
		{
		SetAttachType(aData);
		}
	else if ( aCommand == KCmdEntryAttachment )
		{
		ProcessEntryAttachmentL(aData);
		}
	else
		{
		ret = KErrNotFound;
		}

	return ret;
	}

void CTestCalInterimApiParserEntry::ProcessDtStartL(const TDesC& aData)
	{
	if ( !iHasDtStart )
		{
		TTime time(aData);
		TCalTime calTime;
		CTestCalInterimApiSuiteStepBase::SetCalTimeL(time, calTime, iTimeMode);
		iHasDtStart = ETrue;
		iEntry.SetStartAndEndTimeL(calTime, calTime);
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrMultipleDtStart);
		iTestStep.SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiParserEntry::ProcessDtEndL(const TDesC& aData)
	{
	if ( !iHasDtEnd )
		{
		TTime time(aData);
		TCalTime calTime;
		CTestCalInterimApiSuiteStepBase::SetCalTimeL(time, calTime, iTimeMode);
		TCalTime	startTime = iEntry.StartTimeL();
		iHasDtEnd = ETrue;
		TRAPD( err, iEntry.SetStartAndEndTimeL(startTime, calTime));
		if ( err != KErrNone )
			{
			iTestStep.ERR_PRINTF1(KErrInvalidDateTime);
			iTestStep.SetTestStepResult(EFail);
			}
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrMultipleDtEnd);
		iTestStep.SetTestStepResult(EFail);
		}
	}
void CTestCalInterimApiParserEntry::ProcessDueL(const TDesC& aData)
	{
	if ( !iHasDue )
		{
		TTime time(aData);
		TCalTime calTime;
		CTestCalInterimApiSuiteStepBase::SetCalTimeL(time, calTime, iTimeMode);
		TCalTime	startTime = iEntry.StartTimeL();
		iHasDtEnd = ETrue;
		iEntry.SetStartAndEndTimeL(startTime, calTime);
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrMultipleDue);
		iTestStep.SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiParserEntry::ProcessDTStampL(const TDesC& aData)
	{
	// The value MUST be specified in the UTC time format
	if ( !iHasDTStamp )
		{
		TTime time(aData);
		TCalTime calTime;
		calTime.SetTimeUtcL(time);
		iHasDTStamp = ETrue;
		iEntry.SetDTStampL(calTime);
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrMultipleDTStamp);
		iTestStep.SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiParserEntry::ProcessCompletedL(const TDesC& aData)
	{
	if ( !iHasCompleted )
		{
		TTime time(aData);
		TCalTime calTime;
		CTestCalInterimApiSuiteStepBase::SetCalTimeL(time, calTime, iTimeMode);
		iHasCompleted = ETrue;
		TRAPD( err, iEntry.SetCompletedL(ETrue, calTime) );
		
		// due to restriction of existing test cases, we cant leave here
		if ( err == KErrArgument )
			{ 
			iTestStep.WARN_PRINTF1(KWarnTimeMode);
			}
		else if ( err == KErrNotSupported )
			{
			iTestStep.WARN_PRINTF1(KWarnCompleted);
			}
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrMultipleCompleted);
		iTestStep.SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiParserEntry::ProcessSummaryL(const TDesC& aData)
	{
	if ( !iHasSummary )
		{
		iHasSummary = ETrue;
		iEntry.SetSummaryL(aData);
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrMultipleSummary);
		iTestStep.SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiParserEntry::ProcessLocationL(const TDesC& aData)
	{
	if ( !iHasLocation )
		{
		iHasLocation = ETrue;
		iEntry.SetLocationL(aData);
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrMultipleLocation);
		iTestStep.SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiParserEntry::ProcessCategoriesL(const TDesC& aData)
	{
	CCalCategory*	catergory = CCalCategory::NewL(aData);
	CleanupStack::PushL(catergory);
	iEntry.AddCategoryL(catergory);
	CleanupStack::Pop(catergory);
	}

void CTestCalInterimApiParserEntry::ProcessPriorityL(const TDesC& aData)
	{
	if ( !iHasPriority )
		{
		TLex	lex(aData);
		TUint	result;
		lex.Val(result);
		iHasPriority = ETrue;
		TRAPD( err, iEntry.SetPriorityL(result) );
		if ( err == KErrArgument )
			{ // due to the restriction of existing test cases, can't leave here.
			iTestStep.WARN_PRINTF1(KWarnSetPriority);
			}
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrMultiplePriority);
		iTestStep.SetTestStepResult(EFail);
		}
	}
	
void CTestCalInterimApiParserEntry::ProcessLocalIdL(const TDesC& aData)
	{
	if ( !iHasLocalId )
		{
		TLex	lex(aData);
		TUint	result;
		lex.Val(result);
		iHasLocalId = ETrue;
		iEntry.SetLocalUidL(result);
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrMultipleLocalId);
		iTestStep.SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiParserEntry::ProcessStatusL(const TDesC& aData)
	{
	if ( !iHasStatus )
		{
		CCalEntry::TStatus	status = CCalEntry::ETentative;
		TBool	validData = ETrue;

		if ( aData == KStatusTentative )
			{
			status = CCalEntry::ETentative;
			}
		else if ( aData == KStatusConfirmed )
			{
			status = CCalEntry::EConfirmed;
			}
		else if ( aData == KStatusCancelled )
			{
			status = CCalEntry::ECancelled;
			}
		else if ( aData == KStatusTodoNeedsAction )
			{
			status = CCalEntry::ETodoNeedsAction;
			}
		else if ( aData == KStatusTodoCompleted )
			{
			status = CCalEntry::ETodoCompleted;
			}
		else if ( aData == KStatusTodoInProcess )
			{
			status = CCalEntry::ETodoInProcess;
			}
		else
			{
			validData = EFalse;
			}

		if ( validData )
			{
			iTestStep.INFO_PRINTF2(KInfoSetStatus, (TInt)status);
			iHasStatus = ETrue;
			TRAPD( err, iEntry.SetStatusL(status) );

			// due to restriction of existing test cases, we cant leave here
			if ( err == KErrNotSupported )
				{
				iTestStep.WARN_PRINTF1(KWarnStatusNotSupported);
				}
			}
		else
			{
			iTestStep.ERR_PRINTF2(KErrUnknowStatus, &aData);
			iTestStep.SetTestStepResult(EFail);
			}
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrMultipleStatus);
		iTestStep.SetTestStepResult(EFail);
		}
	}
	
void CTestCalInterimApiParserEntry::ProcessReplicationStatusL(const TDesC& aData)
	{
	if ( !iHasReplicationStatus )
		{
		CCalEntry::TReplicationStatus replicationStatus = CCalEntry::EOpen;
		
		if ( aData == KStatusPrivate )
			{
			replicationStatus = CCalEntry::EPrivate;
			}
		else if ( aData == KStatusRestricted )
			{
			replicationStatus = CCalEntry::ERestricted;
			}
		else
			{
			replicationStatus = CCalEntry::EPrivate;
			}
		iEntry.SetReplicationStatusL(replicationStatus);
		iHasReplicationStatus = ETrue;
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrMultipleReplicationStatus);
		iTestStep.SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiParserEntry::ProcessDescriptionL(const TDesC& aData)
	{
	if ( !iHasDescription )
		{
		iHasDescription = ETrue;
		iEntry.SetDescriptionL(aData);
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrMultipleDescription);
		iTestStep.SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiParserEntry::ProcessAlarmL(const TDesC& aData)
	{
	if ( !iHasAlarm )
		{
		CTestCalInterimApiParserAlarm*	alarmParser = CTestCalInterimApiParserAlarm::NewLC(aData, iEntry, iTestStep, iParserCallback);
		iParserCallback.ParserCreatedL(alarmParser);
		CleanupStack::Pop(alarmParser);
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrMultipleAlarm);
		iTestStep.SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiParserEntry::ProcessOrganizerL(const TDesC& aData)
	{
	if ( !iHasOrganizer )
		{
		CTestCalInterimApiParserUser*	userParser = CTestCalInterimApiParserUser::NewLC(aData, iEntry, iTestStep, iParserCallback);
		iParserCallback.ParserCreatedL(userParser);
		CleanupStack::Pop(userParser);
		iHasOrganizer = ETrue;
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrMultipleOrganizer);
		iTestStep.SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiParserEntry::ProcessAttendeeL(const TDesC& aData)
	{
	CTestCalInterimApiParserAttendee*	attendeeParser = CTestCalInterimApiParserAttendee::NewLC(aData, iEntry, iTestStep, iParserCallback);
	iParserCallback.ParserCreatedL(attendeeParser);
	CleanupStack::Pop(attendeeParser);
	}

void CTestCalInterimApiParserEntry::ProcessRRuleL(const TDesC& aData)
	{
	if ( !iHasRRule )
		{
		CTestCalInterimApiParserRRule*	rruleParser = CTestCalInterimApiParserRRule::NewLC(aData, iEntry, iTestStep, iParserCallback);
		iParserCallback.ParserCreatedL(rruleParser);
		CleanupStack::Pop(rruleParser);
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrMultipleRRule);
		iTestStep.SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiParserEntry::ProcessRDateL(const TDesC& aData)
	{
	TTime time(aData);
	TCalTime calTime;
	CTestCalInterimApiSuiteStepBase::SetCalTimeL(time, calTime, iTimeMode);
	iRDateArray.Append(calTime);
	iEntry.SetRDatesL(iRDateArray);
	}

void CTestCalInterimApiParserEntry::ProcessExDateL(const TDesC& aData)
	{
	TTime time(aData);
	TCalTime calTime;
	CTestCalInterimApiSuiteStepBase::SetCalTimeL(time, calTime, iTimeMode);
	iExDateArray.Append(calTime);
	TRAPD(err, iEntry.SetExceptionDatesL(iExDateArray));
	if ( err == KErrNotFound )
		{
		iTestStep.WARN_PRINTF1(KWarnExceptionDate);
		}
	}

void CTestCalInterimApiParserEntry::ProcessTranspL(const TDesC& aData)
	{
	if ( !iHasTransp )
		{
		TInt err = KErrNone;
		iHasTransp=ETrue;
		
		TBool allocTest = EFalse;
		iTestStep.GetBoolFromConfig(iTestStep.ConfigSection(), KAllocTest, allocTest);

		TLex lex(aData);
		TInt result = 0;
		User::LeaveIfError(lex.Val(result));
		
		if (allocTest)
			{
			err = OOMSetTimeTransparencyL(result);
			}
		else
			{
			TRAP(err, iEntry.SetTimeTransparencyL(CCalEntry::TTransp(result)));	
			}
		
		if (result < 0)
			{
			iTestStep.INFO_PRINTF2(_L("SetBusyStatus Error Code %d"), err);
			iTestStep.TESTL(err == KErrArgument);
			
			TRAP(err, iEntry.SetTimeTransparencyL(CCalEntry::TTransp(KDefaultTranspStatus)));
			iTestStep.TESTL(err == KErrNone);
			}
		else
			{
			iTestStep.INFO_PRINTF2(_L("SetBusyStatus Error Code %d"), err);
			iTestStep.TESTL(err == KErrNone);
			}
		}
	}

void CTestCalInterimApiParserEntry::ProcessGEOLatL(const TDesC& aData)
	{
	if ( !iHasGEOLat )
		{
		iHasGEOLat = ETrue;
		TLex lex(aData);
		TReal result = 0.0;
		User::LeaveIfError(lex.Val(result));
		iGEOLat = result;
		}
	}

void CTestCalInterimApiParserEntry::ProcessGEOLongL(const TDesC& aData)
	{
	if ( !iHasGEOLong )
		{
		iHasGEOLong=ETrue;
		
		TBool allocTest = EFalse;
		iTestStep.GetBoolFromConfig(iTestStep.ConfigSection(), KAllocTest, allocTest);

		TLex lex(aData);
		TReal result = 0.0;
		User::LeaveIfError(lex.Val(result));
		
		CCalGeoValue* geoValue = CCalGeoValue::NewL();
		CleanupStack::PushL(geoValue);
	
		TInt err = KErrNone;	
		if (allocTest)
			{
			err = OOMSetLatLongL(*geoValue, result);
			}
		else
			{
			TRAP(err, geoValue->SetLatLongL(iGEOLat, result));
			}
		
		if ( iGEOLat < KMinGEOLat || iGEOLat > KMaxGEOLat || result < KMinGEOLong || result > KMaxGEOLong )
			{
			iTestStep.INFO_PRINTF2(_L("SetLatLongL Error Code %d"), err);
			iTestStep.TESTL(err == KErrArgument);
			}
		else
			{
			iTestStep.INFO_PRINTF2(_L("SetLatLongL Error Code %d"), err);
			iTestStep.TESTL(err == KErrNone);
			
			err = KErrNone;
			if (allocTest)
				{
				err = OOMSetGeoValueL(*geoValue);
				}
			else
				{
				TRAP(err, iEntry.SetGeoValueL(*geoValue));
				}
		
			if (err != KErrNone)
				{
				iTestStep.ERR_PRINTF2(_L("SetGeoValueL Error Code %d"), err);
				iTestStep.SetTestStepResult(EFail);
				iTestStep.SetTestStepError(err);
				}
			}
			
		CleanupStack::PopAndDestroy(geoValue);	
		}
	}

void CTestCalInterimApiParserEntry::SetAttachType(const TDesC& aData)
	{
	iHasAttachType = ETrue;
	iAttachType = ConvertToAttachType(aData);
	}

void CTestCalInterimApiParserEntry::ProcessEntryAttachmentL(const TDesC& aData)
	{
	if ( iHasAttachType )
		{
		// If disposition is Inline, then check if the data is file attachment or binary data
		if ( iAttachType == CCalAttachment::EFile )
			{
			// Check if the file exists in the file system
			TBool	ret = ValidateFileL(aData);
			// If file exists, open the file
			if ( !ret )
				{
				iBinaryData = ETrue;
				}	
			}
			
		CTestCalInterimApiParserAttachment*	attachmentParser = CTestCalInterimApiParserAttachment::NewLC(aData, iEntry, iTestStep, iParserCallback, iAttachType, *iFs, iBinaryData);
		iParserCallback.ParserCreatedL(attachmentParser);
		CleanupStack::Pop(attachmentParser);
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrNoAttachType);
		iTestStep.SetTestStepResult(EFail);
		}
	}

CCalAttachment::TType CTestCalInterimApiParserEntry::ConvertToAttachType(const TDesC& aData)
	{
	if ( aData.Compare(KFile) == 0 )
		{
		iAttachType = CCalAttachment::EFile;
		}
	else if ( aData.Compare(KUri) == 0 )
		{
		iAttachType = CCalAttachment::EUri;
		}
	return iAttachType;
	}
	
TBool CTestCalInterimApiParserEntry::ValidateFileL(const TDesC& aData)
	{
	User::LeaveIfError(iFs->Connect());
	User::LeaveIfError(iFs->ShareProtected());
	// Check if the file exists in the file system
	TBool	ret = BaflUtils::FileExists(*iFs, aData);
	return ret;
	}

void CTestCalInterimApiParserEntry::ProcessClearRepeatPropertyL(const TDesC& aData)
	{
	TLex	lex(aData);
	TInt	result = 0;
	lex.Val(result);
	if(result)
		{
		iEntry.ClearRepeatingPropertiesL();
		}
	}

TInt CTestCalInterimApiParserEntry::OOMSetLatLongL(CCalGeoValue& aGeoValue, const TReal& aResult)
	{
	TInt tryCount = 1;
 	TInt err = KErrNone;
 	for ( ;; )
 		{
 		__UHEAP_SETFAIL(RHeap::EDeterministic, tryCount);
 		TRAP(err, aGeoValue.SetLatLongL(iGEOLat, aResult));
 		if ( err==KErrNone || err==KErrArgument)
 			{
 			__UHEAP_RESET;
 			iTestStep.INFO_PRINTF1(_L("OOM testing of CCalGeoValue::SetLatLongL Api is done"));
 			break;
 			}
 				
 		if ( err != KErrNoMemory )
 			{
 			__UHEAP_RESET;
 			iTestStep.SetTestStepResult(EFail);
 			break;
 			}
 		tryCount++;
 		__UHEAP_RESET;
 		}
 	return err;
	}
	
TInt CTestCalInterimApiParserEntry::OOMSetGeoValueL(CCalGeoValue& aGeoValue)
	{
	TInt tryCount = 1;
 	TInt err = KErrNone;
 	for ( ;; )
 		{
 		__UHEAP_SETFAIL(RHeap::EDeterministic, tryCount);
 		TRAP(err, iEntry.SetGeoValueL(aGeoValue));
 		if ( err==KErrNone)
 			{
 			__UHEAP_RESET;
 			iTestStep.INFO_PRINTF1(_L("OOM testing of CCalEntry::SetGeoValueL Api is done"));
 			break;
 			}
 				
 		if ( err != KErrNoMemory )
 			{
 			__UHEAP_RESET;
 			iTestStep.SetTestStepResult(EFail);
 			break;
 			}
 		tryCount++;
 		__UHEAP_RESET;
 		}
 	return err;
	}
	
TInt CTestCalInterimApiParserEntry::OOMSetTimeTransparencyL(const TReal& aResult)
	{
	TInt tryCount = 1;
 	TInt err = KErrNone;
 	for ( ;; )
 		{
 		__UHEAP_SETFAIL(RHeap::EDeterministic, tryCount);
 		TRAP(err, iEntry.SetTimeTransparencyL(CCalEntry::TTransp(aResult)));
 		if ( err==KErrNone || err==KErrArgument)
 			{
 			__UHEAP_RESET;
 			iTestStep.INFO_PRINTF1(_L("OOM testing of CCalEntry::SetTimeTransparencyL Api is done"));
 			break;
 			}
 				
 		if ( err != KErrNoMemory )
 			{
 			__UHEAP_RESET;
 			iTestStep.SetTestStepResult(EFail);
 			break;
 			}
 		tryCount++;
 		__UHEAP_RESET;
 		}
 	return err;
	}
