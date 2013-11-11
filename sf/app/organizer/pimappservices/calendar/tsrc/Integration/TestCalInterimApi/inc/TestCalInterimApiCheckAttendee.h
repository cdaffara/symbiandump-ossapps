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



#ifndef TEST_CALINTERIMAPI_CHECK_ATTENDEE_H
#define TEST_CALINTERIMAPI_CHECK_ATTENDEE_H

#include "TestCalInterimApiSuiteStepBase.h"

#include <calentry.h>
#include <caluser.h>

class CCalAttendee;
class CCalEntryView;

/**
This test step fetches the entry from the calendar file and checks the attendee properties
of the parent or child entry
*/
class CTestCalInterimApiCheckAttendee : public CTestCalInterimApiSuiteStepBase
	{
public:
	CTestCalInterimApiCheckAttendee();
	~CTestCalInterimApiCheckAttendee();

	virtual TVerdict	doTestStepL();

private:
	void	GetDataAndCheckAttendeeL();	
	void	FetchAndCheckAttendeesL(const TPtrC& aGuid, const TPtrC& aRecurrenceId);	
	void	CheckAttendeePropertiesL(CCalEntry* aEntry);
	void	GetDataAndCheckAttendeePropertiesL(RPointerArray<CCalAttendee> aEntryAttendeeList);
	
private:
	CCalEntryView*	iCalEntryView;
	TBuf<KMaxTestExecuteCommandLength>	iTempStore;
	TInt	iIndex;
	TInt	iAttendeeCount;
	};

_LIT(KTestCalInterimApiCheckAttendee, "TestCalInterimApiCheckAttendee");

#endif

