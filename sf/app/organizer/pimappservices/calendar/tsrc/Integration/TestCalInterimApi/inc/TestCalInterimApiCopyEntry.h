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



#ifndef TESTCALINTERIMAPICOPYENTRY_H
#define TESTCALINTERIMAPICOPYENTRY_H

#include "TestCalInterimApiSuiteStepBase.h"

#include <calcommon.h>
#include <calinstance.h>
#include <calentry.h>
#include <calsession.h>
#include <calentryview.h>

/**
 * This test step fetches the entry from the file , copies the entry from one to another
 and verifies the local uid and Guid of the entry based on the copy Type flag given
 */
class CTestCalInterimApiCopyEntry : public CTestCalInterimApiSuiteStepBase
	{
public:
	CTestCalInterimApiCopyEntry();
	~CTestCalInterimApiCopyEntry();

	virtual TVerdict	doTestStepL();

private:
	void GetDataFromIniFile();
	CalCommon::TRecurrenceRange GetRecurrenceRange(const TPtrC& aRecurrenceRange);
	void BuildEntryViewL();
	void SetCopyType();
	void SetSourceEntryL();
	void SetDestinationEntryL();
	void CopyEntryL();
	void StoreCopiedEntryL();
	void CheckEntriesL();
	void CheckUidDetailsL();
	void CheckTransValueL();
	void CheckGEOPropertyL();
	void OOMTestForCopyL();
	void SetEntryAndCopyL();

private:
	CCalEntryView* 				iEntryView;
    CCalEntry::TCopyType		iCopyType;
    CCalEntry* 					iSourceEntry;
    CCalEntry* 					iDestinationEntry;
   	RPointerArray<CCalEntry>	iEntryArray;
	TPtrC 						iGuidSource;
	TPtrC 						iGuidDestination;
	TPtrC 						iSourceRecurrenceId;
	TPtrC						iDestinationRecurrenceId;
	TPtrC 						iDestinationRecurrenceRange;
	TPtrC 						iCopyTypeData;
	TBool						iAllocTest;
	};

_LIT(KTestCalInterimApiCopyEntry,"TestCalInterimApiCopyEntry");

#endif
