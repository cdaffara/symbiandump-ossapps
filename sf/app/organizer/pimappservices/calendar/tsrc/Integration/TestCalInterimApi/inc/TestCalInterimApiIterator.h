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



#ifndef __TEST_CAL_INTERIM_API_ITERATOR_H__
#define __TEST_CAL_INTERIM_API_ITERATOR_H__

#include "TestCalInterimApiSuiteStepBase.h"

class CTestCalInterimApiIterator : public CTestCalInterimApiSuiteStepBase
	{
public:
	CTestCalInterimApiIterator();

	virtual TVerdict	doTestStepL();

private:
	//Ref 3.2.13 001, 002, 003, 005, 
	void	IterateEntriesL();
	void	doAllocTestL();
	};

_LIT(KTestCalInterimApiIterator,"TestCalInterimApiIterator");

#endif
