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



#ifndef __TEST_CALINTERIMAPI_PARSER_MAIN_H__
#define __TEST_CALINTERIMAPI_PARSER_MAIN_H__

#include "TestCalInterimApiParserBase.h"

#include <calentry.h>
#include <caltime.h>

/**
The main parser that parses the BEGIN, END commands etc.
*/
class CTestCalInterimApiParserMain : public CTestCalInterimApiParserBase
	{
public:
	CTestCalInterimApiParserMain(CTestStep& aTestStep, MParserCallback& aParserCallback);
	virtual ~CTestCalInterimApiParserMain();

	//	CTestCalInterimParserBase implementation
	virtual TInt		ParseL(const TDesC& aCommand, const TDesC& aData);

private:
	void				ProcessBeginL(const TDesC& aData);
	void				ProcessMethod(const TDesC& aData);
	void 				ProcessTimeMode(const TDesC& aData);
	void				ProcessUidL(const TDesC& aData);
	void				ProcessRecurrenceIdL(const TDesC& aData);
	void				ProcessRecurrenceRangeL(const TDesC& aData);
	void				ProcessEndL(const TDesC& aData);
	void				doAllocTestL(const TDesC& aData);
	CCalEntry::TType	ConvertToEntryType(const TDesC& aData);
	CCalEntry::TMethod	ConverToMethod(const TDesC& aData);
	CalCommon::TRecurrenceRange	ConvertToRecurrenceRange(const TDesC& aData);

private:
	CCalEntry*			iEntry;
	TCalTime::TTimeMode	iTimeMode;
	CCalEntry::TType	iEntryType;
	CCalEntry::TMethod	iMethod;
	CalCommon::TRecurrenceRange	iRecurrenceRange;
	TCalTime			iRecurrenceId;
	TBool				iHasBegin;
	TBool				iHasMethod;
	TBool				iHasRecurrenceId;
	TBool				iHasRecurrenceRange;
	TBool				iAllocTest;
	RFs*				iFs;
	};

#endif
