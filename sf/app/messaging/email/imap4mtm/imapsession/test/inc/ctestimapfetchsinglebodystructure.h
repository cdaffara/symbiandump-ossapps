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

#ifndef CTESTIMAPFETCHSINGLEBODYSTRUCTURE_H
#define CTESTIMAPFETCHSINGLEBODYSTRUCTURE_H

#include "ctestimapcommandbase.h"
#include "cimapenvelope.h"
// Forward Declarations
class CFakeInputStream;
class CFakeOutputStream;
class CActiveWaiter;
class CImapSession;

/**
This test fixture tests CImapFetchSingleBodyStructure.
@internalTechnology
@prototype
*/
class CTestImapFetchSingleBodyStructure : public CTestImapCommandBase
// Note that all test suites must begin with "CTest"
// WARNING: Despite being a C class, CActiveTestFixture is NOT derrived from CBase
// ... so don't expect your data members to be zero'ed
	{
public:
	// Constructor & Destructor
	CTestImapFetchSingleBodyStructure();
	~CTestImapFetchSingleBodyStructure();
	
	TBool DescriptorsMatch(const TDesC8& aFirst, const TDesC8& aSecond);
	
	// Tests
	void TestTextBodyFetchL();
	void TestBasicBodyFetchL();
	void TestRFC822BodyFetchL();
	void TestMultipartL();
	void TestPipexFetchL();
	
	static CTestSuite* CreateSuiteL(const TDesC& aName);
	void CheckAddressL(const CImapEnvelope::TAddress& aAddress, const TDesC16& aExpectedAddressString);
private:

	};
	
#endif CTESTIMAPFETCHSINGLEBODYSTRUCTURE_H
