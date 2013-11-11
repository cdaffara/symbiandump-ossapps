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



#ifndef TEST_CALINTERIMAPI_CAL_PERFORMANCE_H
#define TEST_CALINTERIMAPI_CAL_PERFORMANCE_H

#include "TestCalInterimApiReadData.h"

#include <calentry.h>

class CCalEntryView;

/**
This test step logs the time taken for storing the entry with/ without attachments and fetching 
the entry with/ without attachments
*/
class CTestCalInterimApiCalPerformance: public CTestCalInterimApiReadData
	{
public:
	CTestCalInterimApiCalPerformance();
	~CTestCalInterimApiCalPerformance();

	virtual TVerdict	doTestStepL();

private:
	void		RetreiveIniDataL();
	void		ProcessEntryL(CCalEntry* aEntry);
	void		PerformanceOfAddEntriesL();
	void		PerformanceOfFetchEntriesL();
	void		PerformanceOfFetchingAttachmentL();
		
private:
	CCalEntryView*				iEntryView;
	RPointerArray<CCalEntry>	iEntryArray;
	TReal32						iMaxTimeLimit;
	};

_LIT(KTestCalInterimApiCalPerformance,"TestCalInterimApiCalPerformance");

#endif
