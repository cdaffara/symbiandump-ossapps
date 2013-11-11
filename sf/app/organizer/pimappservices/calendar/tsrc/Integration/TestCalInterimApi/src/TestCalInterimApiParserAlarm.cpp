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

#include "TestCalInterimApiParserAlarm.h"
#include "TestCalInterimApiSuiteStepBase.h"
#include <calentry.h>

_LIT(KCmdAlarmSoundName,			"ALARMSOUNDNAME");
_LIT(KCmdDisposition,				"DISPOSITION");
_LIT(KCmdMimeType,					"MIMETYPE");
_LIT(KCmdContent,					"CONTENT");

_LIT(KErrMultipleAlarmSoundName,	"Multiple ALARMSOUNDNAME");
_LIT(KErrMultipleDisposition,		"Multiple DISPOSITION");
_LIT(KErrMultipleMimeType,			"Multiple MIMETYPE");
_LIT(KErrMultipleContent,			"Multiple CONTENT");

_LIT(KErrWrongData,					"Wrong Data");
_LIT(KWarnAlarmOffset,				"Invalid Alarm offset");

CTestCalInterimApiParserAlarm::CTestCalInterimApiParserAlarm(CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallback)
:	CTestCalInterimApiParserBase(aTestStep, aParserCallback)
,	iEntry(aEntry)
,	iDisposition(CCalContent::EDispositionUnknown)
,	iHasAlarmSoundName(EFalse)
,	iHasDisposition(EFalse)
,	iHasMimeType(EFalse)
,	iHasContent(EFalse)
	{
	}

CTestCalInterimApiParserAlarm*	CTestCalInterimApiParserAlarm::NewLC(const TDesC& aData, CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallback)
	{
	CTestCalInterimApiParserAlarm*	ret = new (ELeave) CTestCalInterimApiParserAlarm(aEntry, aTestStep, aParserCallback);
	CleanupStack::PushL(ret);
	ret->ConstructL(aData);
	return ret;
	}

CTestCalInterimApiParserAlarm*	CTestCalInterimApiParserAlarm::NewL(const TDesC& aData, CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallback)
	{
	CTestCalInterimApiParserAlarm*	ret = CTestCalInterimApiParserAlarm::NewLC(aData, aEntry, aTestStep, aParserCallback);
	CleanupStack::Pop(ret);
	return ret;
	}

CTestCalInterimApiParserAlarm::~CTestCalInterimApiParserAlarm()
	{
	delete iCalAlarm;
	delete iAlarmContent;
	delete iMimeType;
	}


void CTestCalInterimApiParserAlarm::ConstructL(const TDesC& aData)
	{
	TLex lex(aData);
	TInt result;
	lex.Val(result);
	TTimeIntervalMinutes offset(result);
	iCalAlarm = CCalAlarm::NewL();
	iCalAlarm->SetTimeOffset(offset);
	}

TInt CTestCalInterimApiParserAlarm::ParseL(const TDesC& aCommand, const TDesC& aData)
	{
	TInt ret = KErrNone;

	if (aCommand == KCmdAlarmSoundName)
		{
		ProcessAlarmSoundNameL(aData);
		}
	else if (aCommand == KCmdDisposition)
		{
		ProcessDisposition(aData);
		}
	else if (aCommand == KCmdMimeType)
		{
		ProcessMimeTypeL(aData);
		}
	else if (aCommand == KCmdContent)
		{
		ProcessContentL(aData);
		}
	else
		{
		// Add alarm content if exists
		if(iAlarmContent != NULL)
			{
			iCalAlarm->SetAlarmAction(CTestCalInterimApiSuiteStepBase::CreateCalContentL(iAlarmContent, iMimeType, iDisposition));
			}

		// CCalContent takes ownership
		iAlarmContent	= NULL;
		iMimeType		= NULL;

		//	Add the alarm to the entry. Ownership lies with us
		TRAPD( err, iEntry.SetAlarmL(iCalAlarm) );
		if ( err == KErrNotSupported )
			{ // due to restriction of existing test cases, we cant leave here
			iTestStep.WARN_PRINTF1(KWarnAlarmOffset);
			}

		ret=KErrNotFound;
		}

	return ret;
	}

void CTestCalInterimApiParserAlarm::ProcessAlarmSoundNameL(const TDesC& aData)
	{
	if (!iHasAlarmSoundName)
		{
		iCalAlarm->SetAlarmSoundNameL(aData);
		iHasAlarmSoundName = ETrue;
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrMultipleAlarmSoundName);
		iTestStep.SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiParserAlarm::ProcessDisposition(const TDesC& aData)
	{
	if (!iHasDisposition)
		{
		TInt err = CTestCalInterimApiSuiteStepBase::ConvertToDisposition(iDisposition, aData);
		if(err == KErrNotFound)
			{
			iTestStep.ERR_PRINTF1(KErrWrongData);
			iTestStep.SetTestStepResult(EFail);
			}
		iHasDisposition = ETrue;
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrMultipleDisposition);
		iTestStep.SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiParserAlarm::ProcessMimeTypeL(const TDesC& aData)
	{
	if (!iHasMimeType)
		{
		iMimeType = HBufC8::NewL(aData.Length());
		iMimeType->Des().Copy(aData);
		iHasMimeType = ETrue;
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrMultipleMimeType);
		iTestStep.SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiParserAlarm::ProcessContentL(const TDesC& aData)
	{
	if (!iHasContent)
		{
		iAlarmContent = HBufC8::NewL(aData.Length());
		iAlarmContent->Des().Copy(aData);
		iHasContent = ETrue;
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrMultipleContent);
		iTestStep.SetTestStepResult(EFail);
		}
	}

