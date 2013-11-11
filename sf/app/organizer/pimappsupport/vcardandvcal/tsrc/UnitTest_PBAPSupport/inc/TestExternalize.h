// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TESTEXTERNALIZE_H__
#define __TESTEXTERNALIZE_H__

#include "TestUtils.h"

class CTests : public CBase
	{
public:
	static void DoTestsL();
		
private:
	CTests();
	~CTests();
	void ConstructL();
	void UnitTestsL();
	void TestsL();
	
	//Unit Tests
	void LineWrappingTestL();
	void EncodingTestL();
	void CharsetTestL();
	void EscapingTestL();
	void ReplaceCRLFTestL();
		
private:
	CUtils* iUtil;
	RTest iTest;
	};
		
#endif
