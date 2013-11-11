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



#ifndef __TEST_CALINTERIMAPI_PARSER_RRULE_H__
#define __TEST_CALINTERIMAPI_PARSER_RRULE_H__

#include "TestCalInterimApiParserBase.h"

#include <calrrule.h>

class CCalEntry;

class CTestCalInterimApiParserRRule : public CTestCalInterimApiParserBase
	{
public:
	static CTestCalInterimApiParserRRule*	NewLC(const TDesC& aData, CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallback);
	static CTestCalInterimApiParserRRule*	NewL(const TDesC& aData, CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallback);
	virtual ~CTestCalInterimApiParserRRule();

	//	CTestCalInterimParserBase implementation
	virtual TInt		ParseL(const TDesC& aCommand, const TDesC& aData);

protected:
	CTestCalInterimApiParserRRule(CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallback);
	void	ConstructL(const TDesC& aData);

private:
	void 				ProcessSetByMonthL(const TDesC& aData);
	void				ProcessSetByMonthlyDayL(const TDesC& aData);
	void				ProcessWeekInMonthL(const TDesC& aData);
	void				ProcessIntervalL(const TDesC& aData);
	void				ProcessCountL(const TDesC& aData);
	void				ProcessSetByDateL(const TDesC& aData);
	void 				ProcessUntilL(const TDesC& aData);

	static TInt			ConvertToDay(const TDesC& aData, TDay& aDay);
	void				StoreRRuleL();
	void				GetRRuleL();

private:
	CCalEntry&			iEntry;
	TCalRRule::TType	iType;
	TCalRRule*			iRRule;
	RArray<TDay>		iArrayDay;
	RArray<TInt>		iArrayWeekInMonth;
	RArray<TInt>		iArrayByDate;
	RArray<TMonth>		iArrayByMonth;
	TBool				iHasInterval;
	TBool				iHasCount;
	TBool				iHasUntil;
	};

#endif
