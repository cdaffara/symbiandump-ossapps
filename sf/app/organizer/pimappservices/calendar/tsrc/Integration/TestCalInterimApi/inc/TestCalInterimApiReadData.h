/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __TEST_CALINTERIMAPI_READ_DATA_H__
#define __TEST_CALINTERIMAPI_READ_DATA_H__

#include "TestCalInterimApiSuiteStepBase.h"
#include "TestCalInterimApiParserMain.h"

#include <calentry.h>

class CCalEntryView;

/**
 * Teststep that creates or opens the specified calendar file, reads data from the specified
 * data file, invokes necessary parsers to parse the data specified by the data file and adds
 * the parsed calendar entries to the calendar file
 */
class CTestCalInterimApiReadData : public CTestCalInterimApiSuiteStepBase, private CTestCalInterimApiParserMain::MParserCallback
	{
public:
	CTestCalInterimApiReadData();
	~CTestCalInterimApiReadData();

	virtual TVerdict	doTestStepL();
protected:
	//	CTestCalInterimParserBase::MParserCallback implementation
	virtual void		ParserCreatedL(CTestCalInterimApiParserBase* aNewParser);
	virtual void		ProcessEntryL(CCalEntry* aEntry);
	void				FetchEntryL(const TPtrC8& aUid, CCalEntry* aEntry);
	void 				CompareGeoL(CCalEntry* aEntry, CCalEntry* aStoredEntry);
	void 				OpenAndAddEntriesL();
	void				OpenAgendaDataFileL(const TDesC& aDataFile);
	void				ReadAddEntriesL();
	void				doAllocTestL();
	void				GetNumberOfParentChildEntriesL(RPointerArray<CCalEntry> aEntriesToFetch);
	CCalEntry* 			GetNewlyAddedEntryL(CCalEntry* aEntry, RPointerArray<CCalEntry>& aEntries);
	void 				CheckTheNumOfParentChildEntryL(RPointerArray<CCalEntry> aEntriesToFetch);


public:
	CCalEntryView*								iEntryView;
	CCalEntry*									iEntry;
	RFile										iDataFile;
	TBool										iDataFileOpened;
	TFileText									iAgendaData;
	CTestCalInterimApiParserBase*				iCurrentParser;
	RPointerArray<CTestCalInterimApiParserBase>	iParsers;
	};

_LIT(KTestCalInterimApiReadData,	"TestCalInterimApiReadData");

#endif
