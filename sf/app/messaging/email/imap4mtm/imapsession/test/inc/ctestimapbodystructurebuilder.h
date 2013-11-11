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

#ifndef __CTESTIMAPBODYSTRUCTUREBUILDER_H__
#define __CTESTIMAPBODYSTRUCTUREBUILDER_H__

#include <test/tefunit.h>
#include "cimapenvelope.h"

// Forward Declarations
class CImapBodyStructureBuilder;
class CImapFetchResponse;

/**
This test fixture tests CImapBodyStructureBuilder.
@internalTechnology
@prototype
*/
class CTestImapBodyStructureBuilder : public CTestFixture
// Note that all test suites must begin with "CTest"
// WARNING: Despite being a C class, CTestFixture is NOT derrived from CBase
// ... so don't expect your data members to be zero'ed
	{
public:
	// Constructor & Destructor (explicitly virtual because CTestFixture is NOT derrived from CBase)
	CTestImapBodyStructureBuilder();
	virtual ~CTestImapBodyStructureBuilder();
	
	// SetUp and TearDown code (optional)
	virtual void SetupL();
	virtual void TearDownL();

	void AssertDescriptorsEqual(const TDesC8& aFirst, const TDesC8& aSecond);

	// Tests
	void TestSimpleTextMessageL();
	void TestComplexTextMessageL();
	void TestRfc822MessageL();
	void TestMultipartTextMessageNestLeftL();
	void TestMultipartTextMessageNestRightL();
			
	static CTestSuite* CreateSuiteL(const TDesC& aName);
	
private:
	void CheckAddressL(const CImapEnvelope::TAddress& aAddress, const TDesC16& aExpectedAddressString);

private:
	CImapFetchResponse* iFetchResponse;
	CImapBodyStructureBuilder* iBodyStructureBuilder;
	};
	
#endif __CTESTIMAPBODYSTRUCTUREBUILDER_H__
