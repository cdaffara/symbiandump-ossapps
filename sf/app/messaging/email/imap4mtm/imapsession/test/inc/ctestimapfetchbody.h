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

#ifndef CTESTIMAPFETCHBODY_H
#define CTESTIMAPFETCHBODY_H

#include "cfakeinputstream.h"
#include "cfakeoutputstream.h"
#include "cfetchbodyinfo.h"
#include "cimapmailstore.h"
#include <test/tefunit.h>

#include "cfakeinputstream.h"
#include "cactivewaiter.h"

// Forward Declarations
class CFakeOutputStream;
class CImapSession;
class CImapMailStore;
class CImapFetchBodyResponse;
class CImapSettings;
class CMsvServer;

/**
This test fixture tests CImapCreate.
@internalTechnology
@prototype
*/
class CTestImapFetchBody : public CActiveTestFixture
						 , public MFakeInputStreamIsEmptyObserver
						 , public MActiveWaiterObserver
// Note that all test suites must begin with "CTest"
// WARNING: Despite being a C class, CActiveTestFixture is NOT derrived from CBase
// ... so don't expect your data members to be zero'ed
	{
public:
	// Constructor & Destructor (explicitly virtual because CTestFixture is NOT derrived from CBase)
	CTestImapFetchBody();
	virtual ~CTestImapFetchBody();
	
	// SetUp and TearDown code (optional)
	virtual void SetupL();
	virtual void TearDownL();

	// Implement MFakeInputStreamIsEmptyObserver
	void OnInputStreamIsEmptyL();
	
	// Implement MActiveWaiterObserver
	void DoCancel();

	// Tests
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	void TestFetchBinaryBodyL();	
	void TestFetchBinaryBodyWithCancelL();		
#endif		
	void TestFetchBodyL();
	void TestFetchBodyWithCancelL();
	void TestFetchMultiChunkBodyL();
	void TestFetchBodyWithMimeL();
	void TestFetchBodyWithMimeZeroAngelBracketL();
	void TestFetchBodyWithMime2L();
	void TestFetchBodyWithNoBodyLengthInServerResponseL();
	void TestFetchBodyWithNilMimeL();
	void TestFetchBodyWithEmptyMimeL();
			
	static CTestSuite* CreateSuiteL(const TDesC& aName);

private:
	void CreateMessageStructureL(TMsvId aId);
private:
	CFakeInputStream* iInputStream;
	CFakeOutputStream* iOutputStream;
	CActiveWaiter* iActiveWaiter;
	CImapSession* iImapSession;
	CFetchBodyInfo* iFetchBodyInfo;
	TMsvId iMsvMessageEntry;
	TMsvId iAttMessageEntry;
	TMsvId iBodyMessageEntry;
	TMsvId iFolderId;
	CImapSettings* iImapSettings;
	CImapMailStore* iImapMailStore;
	CMsvServerEntry* iServerEntry;
	CImapFetchBodyResponse* iFetchBodyResponse;
	CMsvServer*	iMsvServer;
	};
	
#endif CTESTIMAPFETCHBODY_H
