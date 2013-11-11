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

#include "TestCalInterimApiParserMain.h"
#include "TestCalInterimApiParserEntry.h"
#include "TestCalInterimApiSuiteStepBase.h"

#include <calentryview.h>

_LIT(KCmdBegin,						"BEGIN");
_LIT(KCmdMethod,					"METHOD");
_LIT(KCmdTimeMode,					"TIMEMODE");
_LIT(KCmdUid,						"UID");
_LIT(KCmdRecurrenceId,				"RECURR-ID");
_LIT(KCmdRecurrenceRange,			"RECURRENCERANGE");
_LIT(KCmdEnd,						"END");

_LIT(KTimeModeFloating,				"EFloating");
_LIT(KFixedTimeZone,				"EFixedTimeZone");

_LIT(KTypeToDo,						"VTODO");
_LIT(KTypeEvent,					"VEVENT");
_LIT(KTypeReminder,					"VREMINDER");
_LIT(KTypeAppt,						"VAPPT");
_LIT(KTypeAnniv,					"VANNIV");

_LIT(KMethodAdd,					"EMethodAdd");
_LIT(KMethodNone,					"EMethodNone");
_LIT(KMethodPublish,				"EMethodPublish");
_LIT(KMethodRequest,				"EMethodRequest");
_LIT(KMethodReply,					"EMethodReply");
_LIT(KMethodCancel,					"EMethodCancel");
_LIT(KMethodRefresh,				"EMethodRefresh");
_LIT(KMethodCounter,				"EMethodCounter");
_LIT(KMethodDeclineCounter,			"EMethodDeclineCounter");
_LIT(KTypeThisOnly,					"ThisOnly");
_LIT(KTypeThisAndAll,				"ThisAndAll");
_LIT(KTypeThisAndFuture,			"ThisAndFuture");
_LIT(KTypeThisAndPrior,				"ThisAndPrior");

_LIT(KErrNestedBegin,				"Nested BEGIN");
_LIT(KErrNoEnd,						"No END");
_LIT(KErrNoBegin,					"No BEGIN");
_LIT(KErrMultipleMethod,			"Multiple Method");
_LIT(KErrMultipleRecurrenceID,		"Muliple RecurrenceId");
_LIT(KErrMultipleRecurrenceRange,	"Multiple RecurrenceRange");
_LIT(KErrEndTypeMismatch,			"End Type does not match Begin Type");
_LIT(KErrNoEntryCreated,			"No Entry created to End");
_LIT(KErrUnknownEntryType,			"Unknown Entry Type");
_LIT(KErrUnknownRecurrenceRange,	"Unknown Recurrence Range");
_LIT(KErrUnknownMethod,				"Unknown Method");

CTestCalInterimApiParserMain::CTestCalInterimApiParserMain(CTestStep& aTestStep, MParserCallback& aParserCallback)
:	CTestCalInterimApiParserBase(aTestStep, aParserCallback)
,	iTimeMode(TCalTime::EFixedUtc)
,	iEntryType(CCalEntry::EAppt)
,	iMethod(CCalEntry::EMethodAdd)
,	iRecurrenceRange(CalCommon::EThisOnly)
,	iHasBegin(EFalse)
,	iHasMethod(EFalse)
,	iHasRecurrenceId(EFalse)
	{
	}

CTestCalInterimApiParserMain::~CTestCalInterimApiParserMain()
	{
	iFs->Close();
	delete iEntry;
	}

TInt CTestCalInterimApiParserMain::ParseL(const TDesC& aCommand, const TDesC& aData)
	{
	TInt	ret = KErrNone;

	if ( aCommand == KCmdBegin )
		{
		ProcessBeginL(aData);
		}
	else if ( aCommand == KCmdMethod )
		{
		ProcessMethod(aData);
		}
	else if ( aCommand == KCmdTimeMode )
		{
		ProcessTimeMode(aData);
		}
	else if ( aCommand == KCmdUid )
		{
		ProcessUidL(aData);
		}
	else if ( aCommand == KCmdRecurrenceId )
		{
		ProcessRecurrenceIdL(aData);
		}
	else if ( aCommand == KCmdRecurrenceRange )
		{
		ProcessRecurrenceRangeL(aData);
		}
	else if ( aCommand == KCmdEnd )
		{
		ProcessEndL(aData);
		}
	else
		{
		ret = KErrNotFound;
		}

	return ret;
	}

void CTestCalInterimApiParserMain::ProcessBeginL(const TDesC& aData)
	{
	if ( !iHasBegin )
		{
		iHasBegin = ETrue;
		iEntryType = ConvertToEntryType(aData);
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrNestedBegin);
		iTestStep.SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiParserMain::ProcessMethod(const TDesC& aData)
	{
	if ( !iHasMethod )
		{
		iHasMethod = ETrue;
		iMethod = ConverToMethod(aData);
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrMultipleMethod);
		iTestStep.SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiParserMain::ProcessTimeMode(const TDesC& aData)
	{
	if (aData == KTimeModeFloating)
		{
		iTimeMode = TCalTime::EFloating;
		}
	else if(aData == KFixedTimeZone)
		{
		iTimeMode = TCalTime::EFixedTimeZone;
		}
	}

void CTestCalInterimApiParserMain::ProcessUidL(const TDesC& aData)
	{
	if ( iHasBegin )
		{
		if ( iEntry == NULL )
			{
			iFs = new(ELeave) RFs;
			HBufC8*	entryId = HBufC8::NewLC(aData.Length());
			entryId->Des().Copy(aData);
			// Recurrence Id for child entries.
			if ( iHasRecurrenceId )
				{
				iEntry = CCalEntry::NewL(iEntryType, entryId, iMethod, (TUint)0, iRecurrenceId, iRecurrenceRange);
				iHasRecurrenceId = EFalse;
				}
			else
				{
				iEntry=CCalEntry::NewL(iEntryType, entryId, iMethod, (TUint)0);
				}
			CleanupStack::Pop(entryId);

			CTestCalInterimApiParserEntry*	entryParser = new (ELeave) CTestCalInterimApiParserEntry(*iEntry, iTestStep, iParserCallback, iTimeMode, iFs);
			CleanupStack::PushL(entryParser);
			iParserCallback.ParserCreatedL(entryParser);
			CleanupStack::Pop(entryParser);
			}
		else
			{
			iTestStep.ERR_PRINTF1(KErrNoEnd);
			iTestStep.SetTestStepResult(EFail);
			}
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrNoBegin);
		iTestStep.SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiParserMain::ProcessRecurrenceIdL(const TDesC& aData)
	{
	if( !iHasRecurrenceId )
		{
		TTime		time(aData);
		iHasRecurrenceId = ETrue;
		CTestCalInterimApiSuiteStepBase::SetCalTimeL(time, iRecurrenceId, iTimeMode);
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrMultipleRecurrenceID);
		iTestStep.SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiParserMain::ProcessRecurrenceRangeL(const TDesC& aData)
	{
	if ( iHasRecurrenceId )
		{
		iRecurrenceRange = ConvertToRecurrenceRange(aData);
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrMultipleRecurrenceRange);
		iTestStep.SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiParserMain::ProcessEndL(const TDesC& aData)
	{
	if ( iEntry != NULL )
		{
		CCalEntry::TType entryType=ConvertToEntryType(aData);

		if ( entryType == iEntryType )
			{
			iParserCallback.ProcessEntryL(iEntry); // takes ownership
			}
		else
			{
			iTestStep.ERR_PRINTF1(KErrEndTypeMismatch);
			iTestStep.SetTestStepResult(EFail);
			}
		iEntry = NULL;
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrNoEntryCreated);
		iTestStep.SetTestStepResult(EFail);
		}
	iHasBegin = EFalse;
	iTimeMode = TCalTime::EFixedUtc;
	iHasMethod = EFalse;
	}

CCalEntry::TType CTestCalInterimApiParserMain::ConvertToEntryType(const TDesC& aData)
	{
	CCalEntry::TType	ret = CCalEntry::EAppt;
	if ( aData.Compare(KTypeToDo) == 0 )
		{
		ret = CCalEntry::ETodo;
		}
	else if ( aData.Compare(KTypeEvent) == 0 )
		{
		ret = CCalEntry::EEvent;
		} 
	else if ( aData.Compare(KTypeReminder) == 0 )
		{
		ret = CCalEntry::EReminder;
		}
	else if ( aData.Compare(KTypeAppt) == 0 )
		{
		ret = CCalEntry::EAppt;
		}
	else if ( aData.Compare(KTypeAnniv) == 0 )
		{
		ret = CCalEntry::EAnniv;
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrUnknownEntryType);
		iTestStep.SetTestStepResult(EFail);
		}

	return ret;
	}

CalCommon::TRecurrenceRange CTestCalInterimApiParserMain::ConvertToRecurrenceRange(const TDesC& aData)
	{
	CalCommon::TRecurrenceRange	retrecurrence = CalCommon::EThisOnly;
	if ( aData.Compare(KTypeThisOnly) == 0 )
		{
		retrecurrence = CalCommon::EThisOnly;
		}
	else if ( aData.Compare(KTypeThisAndAll) == 0 )
		{
		retrecurrence = CalCommon::EThisAndAll;
		}
	else if ( aData.Compare(KTypeThisAndFuture) == 0 )
		{
		retrecurrence = CalCommon::EThisAndFuture;
		}
	else if ( aData.Compare(KTypeThisAndPrior) == 0 )
		{
		retrecurrence = CalCommon::EThisAndPrior;
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrUnknownRecurrenceRange);
		iTestStep.SetTestStepResult(EFail);
		}

	return retrecurrence;
	}

CCalEntry::TMethod CTestCalInterimApiParserMain::ConverToMethod(const TDesC& aData)
	{
	CCalEntry::TMethod	retMethod = CCalEntry::EMethodAdd;
	if ( aData.Compare(KMethodAdd) == 0 )
		{
		retMethod = CCalEntry::EMethodAdd;
		}
	else if ( aData.Compare(KMethodNone) == 0 )
		{
		retMethod = CCalEntry::EMethodNone;
		}
	else if ( aData.Compare(KMethodPublish) == 0 )
		{
		retMethod = CCalEntry::EMethodPublish;
		}
	else if ( aData.Compare(KMethodRequest) == 0 )
		{
		retMethod = CCalEntry::EMethodRequest;
		}
	else if ( aData.Compare(KMethodReply) == 0 )
		{
		retMethod = CCalEntry::EMethodReply;
		}
	else if ( aData.Compare(KMethodCancel) == 0 )
		{
		retMethod = CCalEntry::EMethodCancel;
		}
	else if ( aData.Compare(KMethodRefresh) == 0 )
		{
		retMethod = CCalEntry::EMethodRefresh;
		}
	else if ( aData.Compare(KMethodCounter) == 0 )
		{
		retMethod = CCalEntry::EMethodCounter;
		}
	else if ( aData.Compare(KMethodDeclineCounter) == 0 )
		{
		retMethod = CCalEntry::EMethodDeclineCounter;
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrUnknownMethod);
		iTestStep.SetTestStepResult(EFail);
		}

	return retMethod;
	}
