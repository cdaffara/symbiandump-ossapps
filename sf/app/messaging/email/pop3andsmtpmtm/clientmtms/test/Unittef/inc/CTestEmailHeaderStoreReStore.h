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
// file  CTestEmailHeaderStoreReStore.h
// This contains Ut for email header migration.
// 

#include "emailtestutils.h"
#include "MIUTHDR.H"    // CImHeader
#include <pop3set.h>
#include <miutlog.h>

#include <msventry.h>
//#include "MSVSERV.H"
#include <msvids.h>
#include <miutconv.h>
#include <charconv.h>
#include <imcvcodc.h>

#include <test\tefunit.h>
#include <test\testexecutestepbase.h>
#include <es_sock.h>


class CObserver;

class CTestEmailHeaderStoreReStore: public  CActiveTestFixture
// WARNING: Despite being a C class, CActiveTestFixture is NOT derrived from CBase
// ... so don't expect your data members to be zero'ed
	{
public:
	CTestEmailHeaderStoreReStore(); 
	virtual ~CTestEmailHeaderStoreReStore();
	void SetupL();
	void TearDownL();
	
		
	static CTestSuite* CreateSuiteL(const TDesC& aName);

	void TestStoreRestoreHeaderL();
	void InitialiseTestL();
	void CreateMessagesL(TUid aMtm);
	void CreateAccountL();
	void TestReStoreHeaderL();
	void TestCreateEncodedMessageHeaderL();
	void TestReStoreEncodedHeaderL();
	void TestCreateMimeMessageHeaderL();
	void TestReStoreMimedHeaderL();
	void TestCreateMimeMessageEmptyHeaderL();
	void TestReStoreMimedEmptyHeaderL();
	void TestReStoreMimedReStoreHeaderL();
	void TestCreateMimeMessageReStoreHeaderL();

	void TestCreateMimeMessageReStoreTestHeaderL();
	void TestReStoreMimedReStoreTestHeaderL();
	
	void TestCreateMimeMessageEmptyXtypeTestHeaderL();
	void TestReStoreMimedEmptyXtypeTestHeaderL();
	
	void TestCreateMimeMessageEmptyMimeOneTestHeaderL();
	void TestReStoreMimedEmptyOneTestHeaderL();
	
	void TestCreateEmptytestEncodedMessageHeaderL();
	void TestReStoreEmptytestEncodedHeaderL();
	
	void TestCreateEmptytestEncodedMessageOneHeaderL();
	void TestReStoreEmptytestEncodedOneHeaderL();
	
	void TestCreateEmptytestEncodedMessageTwoHeaderL();
	void TestReStoreEmptytestEncodedTwoHeaderL();
	

private:
	CEmailTestUtils* iTestUtils;
	CMsvSession* iSession;
	CMsvEntry* iEntry;
	CActiveScheduler* iScheduler;
	CTestUtilsWatcherStarter* iWatchers;
	CMsvOperation* iOperation;
	TMsvId iMessageId;
	CTestActive* iTestActive;
	CObserver* iob;
	CImHeader* imHeader;
	CImHeader* imReciveHeader;
	CImHeader* iencodedHeader;
	CImHeader* imEncodedReciveHeader;
	CImMimeHeader* imMimeHeader;
	CImMimeHeader* imMimeReciveHeader;
	

};

class CObserver : public MMsvSessionObserver
	{
	public:
		void HandleSessionEvent(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);
		void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);
	};



