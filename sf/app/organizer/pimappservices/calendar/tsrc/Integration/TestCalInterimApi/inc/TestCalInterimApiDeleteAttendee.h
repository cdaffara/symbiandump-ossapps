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



#ifndef TEST_CALINTERIMAPI_DELETE_ATTENDEE_H
#define TEST_CALINTERIMAPI_DELETE_ATTENDEE_H

#include "TestCalInterimApiSuiteStepBase.h"

#include <calentry.h>
#include <caluser.h>

/**
This test step fetches the entry from the calendar file and then deletes the specified
attendee from the parent or child entry. 
*/

class CCalAttendee;
class CCalEntryView;
class CTestCalInterimApiDeleteAttendee : public CTestCalInterimApiSuiteStepBase
	{
public:
	CTestCalInterimApiDeleteAttendee();
	~CTestCalInterimApiDeleteAttendee();

	virtual TVerdict	doTestStepL();

private:
	void	FetchEntryAndDeleteAttendeeL(const TPtrC& aGuid, const TPtrC& aRecurrenceId, const TPtrC& aTimeMode);	
	void	PerformOOMForDeleteAttendeeL(CCalEntry* aEntry, const TInt aIndex);	
	void	DeleteAttendeeL(CCalEntry* aEntry);
	
private:
	CCalEntryView*	iEntryView;
	};

_LIT(KTestCalInterimApiDeleteAttendee, "TestCalInterimApiDeleteAttendee");

#endif

