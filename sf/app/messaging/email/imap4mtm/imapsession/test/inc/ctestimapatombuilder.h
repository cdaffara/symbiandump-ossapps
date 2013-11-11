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

#ifndef __CTESTIMAPATOMBUILDER_H__
#define __CTESTIMAPATOMBUILDER_H__

#include <test/tefunit.h>


// Forward Declarations
class CImapAtomParser;
class CImapAtom;
class CImapBodyStructureBuilder;
class CImapFetchResponse;

/**
This test fixture tests CImapAtomParser and CImapAtomWalker.
@internalTechnology
@prototype
*/
class CTestImapAtomBuilder : public CTestFixture
// Note that all test suites must begin with "CTest"
// WARNING: Despite being a C class, CTestFixture is NOT derrived from CBase
// ... so don't expect your data members to be zero'ed
	{
public:
	// Constructor & Destructor (explicitly virtual because CTestFixture is NOT derrived from CBase)
	CTestImapAtomBuilder();
	virtual ~CTestImapAtomBuilder();
	
	// SetUp and TearDown code (optional)
	virtual void SetupL();
	virtual void TearDownL();

	// Tests
	void TestAtomParserL();
	void TestWalkerL();
			
	static CTestSuite* CreateSuiteL(const TDesC& aName);
	
private:
	void DumpAtomTreeL(CImapAtom* aRoot);
	
private:
	CImapAtomParser* iAtomParser;
	};
	
#endif __CTESTIMAPATOMBUILDER_H__
