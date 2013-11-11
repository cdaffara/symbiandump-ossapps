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

#include "TestCalInterimApiParserAttendee.h"
#include "TestCalInterimApiSuiteStepBase.h"

_LIT(KCmdCommonName, 		"COMMONNAME");
_LIT(KCmdSentBy, 			"SENTBY");
_LIT(KCmdRole,				"ROLE");
_LIT(KCmdAttendeeStatus,	"ATTENDEESTATUS");
_LIT(KCmdResponseRequested,	"RESPONSEREQUESTED");

_LIT(KWarnInvalidRole,		"Invalid ROLE %d");

_LIT(KErrMultipleCmnName,	"Multiple COMMONNAME");
_LIT(KErrMultipleSentBy,	"Multiple SENTBY");
_LIT(KErrMultipleRole,		"Multiple ROLE");
_LIT(KErrAttendeeStatus,	"Multiple ATTENDEESTATUS");

CTestCalInterimApiParserUser::CTestCalInterimApiParserUser(CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallback)
: CTestCalInterimApiParserBase(aTestStep, aParserCallback)
, iEntry(aEntry)
, iHasSentBy(EFalse)
, iHasCommonName(EFalse)
	{
	}

CTestCalInterimApiParserUser::~CTestCalInterimApiParserUser()
	{
	delete iAddress;
	delete iSentByAddress;
	delete iCommonName;
	}

CTestCalInterimApiParserUser* CTestCalInterimApiParserUser::NewLC(const TDesC& aData, CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallback)
	{
	CTestCalInterimApiParserUser* self = new (ELeave) CTestCalInterimApiParserUser(aEntry, aTestStep, aParserCallback);
	CleanupStack::PushL(self);
	self->ConstructL(aData);
	return self;
	}

CTestCalInterimApiParserUser* CTestCalInterimApiParserUser::NewL(const TDesC& aData, CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallback)
	{
	CTestCalInterimApiParserUser* self = CTestCalInterimApiParserUser::NewLC(aData, aEntry, aTestStep, aParserCallback);
	CleanupStack::Pop(self);
	return self;
	}

void CTestCalInterimApiParserUser::ConstructL(const TDesC& aData)
	{
	iAddress = aData.AllocL();
	}

TInt CTestCalInterimApiParserUser::ParseL(const TDesC& aCommand, const TDesC& aData)
	{
	TInt	ret=KErrNone;

	if ( aCommand == KCmdCommonName )
		{
		ProcessCommonNameL(aData);
		}
	else if ( aCommand == KCmdSentBy )
		{
		ProcessSentByL(aData);
		}
	else
		{
		//	Set the organizer.
		SetOrganizerL();
		ret=KErrNotFound;
		}

	return ret;
	}

void CTestCalInterimApiParserUser::ProcessCommonNameL(const TDesC& aData)
	{
	if(!iCommonName)
		{
		iCommonName = aData.AllocL();
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrMultipleCmnName);
		iTestStep.SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiParserUser::ProcessSentByL(const TDesC& aData)
	{
	if(!iSentByAddress)
		{
		iSentByAddress = aData.AllocL();
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrMultipleSentBy);
		iTestStep.SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiParserUser::SetOrganizerL()
	{
	CCalUser* user = NULL;
	if(iSentByAddress)
		{
		user = CCalUser::NewL(*iAddress, *iSentByAddress);
		}
	else
		{
		user = CCalUser::NewL(*iAddress);
		}

	CleanupStack::PushL(user);

	if(iCommonName)
		{
		user->SetCommonNameL(*iCommonName);
		}

	iEntry.SetOrganizerL(user);
	iEntry.SetPhoneOwnerL(user);

	CleanupStack::Pop(user);
	}


//
// CTestCalInterimParserAttendee implementation
//

CTestCalInterimApiParserAttendee*	CTestCalInterimApiParserAttendee::NewLC(const TDesC& aData, CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallback)
	{
	CTestCalInterimApiParserAttendee*	ret=new (ELeave) CTestCalInterimApiParserAttendee(aEntry, aTestStep, aParserCallback);
	CleanupStack::PushL(ret);
	ret->ConstructL(aData);
	return ret;
	}

CTestCalInterimApiParserAttendee*	CTestCalInterimApiParserAttendee::NewL(const TDesC& aData, CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallback)
	{
	CTestCalInterimApiParserAttendee*	ret=CTestCalInterimApiParserAttendee::NewLC(aData, aEntry, aTestStep, aParserCallback);
	CleanupStack::Pop(ret);
	return ret;
	}

CTestCalInterimApiParserAttendee::CTestCalInterimApiParserAttendee(CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallback)
: CTestCalInterimApiParserUser(aEntry, aTestStep, aParserCallback)
, iCalRole(CCalAttendee::EReqParticipant)
, iCalStatus(CCalAttendee::ENeedsAction)
, iRsvp(EFalse)
, iHasRole(EFalse)
, iHasAttendeeStatus(EFalse)
, iHasRsvp(EFalse)
	{
	}

CTestCalInterimApiParserAttendee::~CTestCalInterimApiParserAttendee()
	{
	}

TInt CTestCalInterimApiParserAttendee::ParseL(const TDesC& aCommand, const TDesC& aData)
	{
	TInt	ret=KErrNone;

	if ( aCommand == KCmdCommonName )
		{
		ProcessCommonNameL(aData);
		}
	else if ( aCommand == KCmdSentBy )
		{
		ProcessSentByL(aData);
		}
	else if ( aCommand==KCmdRole )
		{
		ProcessRoleL(aData);
		}
	else if ( aCommand==KCmdAttendeeStatus )
		{
		ProcessAttendeeStatusL(aData);
		}
	else if ( aCommand==KCmdResponseRequested)
		{
		ProcessResponseRequestedL(aData);
		}
	else
		{
		//	Add the attendee
		CCalAttendee* attendee = CreateAttendeeLC();
		iEntry.AddAttendeeL(attendee);

		GetAttendeeInfoL(attendee);

		// iEntry taken ownership
		CleanupStack::Pop(attendee);

		ret=KErrNotFound;
		}

	return ret;
	}

void CTestCalInterimApiParserAttendee::ProcessRoleL(const TDesC& aData)
	{
	if ( !iHasRole )
		{
		TLex	lex(aData);
		TUint	result;
		lex.Val(result);
		iHasRole=ETrue;
		iCalRole = static_cast<CCalAttendee::TCalRole>(result);
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrMultipleRole);
		iTestStep.SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiParserAttendee::ProcessAttendeeStatusL(const TDesC& aData)
	{
	if ( !iHasAttendeeStatus )
		{
		TLex	lex(aData);
		TUint	result;
		lex.Val(result);
		iHasAttendeeStatus=ETrue;
		iCalStatus = static_cast<CCalAttendee::TCalStatus>(result);
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrAttendeeStatus);
		iTestStep.SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiParserAttendee::ProcessResponseRequestedL(const TDesC& aData)
	{
	if ( !iHasRsvp )
		{
		iHasRsvp=ETrue;
		iRsvp = CTestCalInterimApiSuiteStepBase::ConvertStringToBool(aData);
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrAttendeeStatus);
		iTestStep.SetTestStepResult(EFail);
		}
	}

CCalAttendee* CTestCalInterimApiParserAttendee::CreateAttendeeLC()
	{
	CCalAttendee* attendee = NULL;

	if(iSentByAddress)
		{
		attendee = CCalAttendee::NewL(*iAddress, *iSentByAddress);
		}
	else
		{
		attendee = CCalAttendee::NewL(*iAddress);
		}

	CleanupStack::PushL(attendee);

	if(iCommonName)
		{
		attendee->SetCommonNameL(*iCommonName);
		}

	TRAPD(err, attendee->SetRoleL(iCalRole));

	if ( err == KErrNotFound )
		{// due to restriction of existing test cases, we cant leave here
		iTestStep.WARN_PRINTF2(KWarnInvalidRole, iCalRole);
		}
	else
		{// Propogate leave if some other error
		User::LeaveIfError(err);
		}

	attendee->SetStatusL(iCalStatus);
	attendee->SetResponseRequested(iRsvp);

	return attendee;
	}

void CTestCalInterimApiParserAttendee::GetAttendeeInfoL(CCalAttendee* aAttendee)
	{
	iTestStep.INFO_PRINTF1(_L("To check the attendee attributes"));
	CCalAttendee::TCalRole attendeeRole = aAttendee->RoleL();
	iTestStep.INFO_PRINTF2(_L("Attendee Role %d"), attendeeRole);
	if(iCalRole >= CCalAttendee::EReqParticipant && iCalRole <= CCalAttendee::EChair && attendeeRole != iCalRole )
		{
		iTestStep.ERR_PRINTF1(_L("The Role is not set correctly"));
		iTestStep.SetTestStepResult(EFail);
		}
	CCalAttendee::TCalStatus attendeeStatus = aAttendee->StatusL();
	iTestStep.INFO_PRINTF2(_L("Attendee Status %d"), attendeeStatus);
	if( attendeeStatus != iCalStatus )
		{
		iTestStep.ERR_PRINTF1(_L("The Status is not set correctly"));
		iTestStep.SetTestStepResult(EFail);
		}
	}

