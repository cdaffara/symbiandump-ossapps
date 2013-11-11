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



#ifndef __TEST_CALINTERIMAPI_PARSER_BASE_H__
#define __TEST_CALINTERIMAPI_PARSER_BASE_H__

#include <test/testexecutestepbase.h>

class CCalEntry;
class TCalRRule;

class CTestCalInterimApiParserBase :  public CBase
	{
public:
	class MParserCallback
		{
	public:
		virtual void	ParserCreatedL(CTestCalInterimApiParserBase* aNewParser) = 0;
		virtual void	ProcessEntryL(CCalEntry* aEntry) = 0;
		};

public:
	CTestCalInterimApiParserBase(CTestStep& aTestStep, MParserCallback& aParserCallback);
	virtual ~CTestCalInterimApiParserBase();

	// Parse the command read in
	// @return	KErrNone if processed
	//			KErrNotFound if the parser cannot process the command
	virtual TInt	ParseL(const TDesC& aCommand, const TDesC& aData) = 0;

protected:
	CTestStep&			iTestStep;
	MParserCallback&	iParserCallback;
	};

#endif
