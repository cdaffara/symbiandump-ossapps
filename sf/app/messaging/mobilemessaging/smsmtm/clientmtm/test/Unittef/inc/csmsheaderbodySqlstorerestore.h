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
// file  CTestHeaderStoreReStore.h
// This contains Ut for Sms header migration.
// 

#include <test\tefunit.h>
#include <test\testexecutestepbase.h>
#include "smstestutils.h"
#include <es_sock.h>

class EntryObserver : public MMsvEntryObserver
	{
	public:
		virtual void HandleEntryEventL(TMsvEntryEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);		
	};
	
class SessionObserver : public MMsvSessionObserver
	{
	public:
		virtual void HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/){};
	};

class CTestHeaderStoreReStore : public  CActiveTestFixture
// WARNING: Despite being a C class, CActiveTestFixture is NOT derrived from CBase
// ... so don't expect your data members to be zero'ed
	{
public:
	CTestHeaderStoreReStore(); 
	virtual ~CTestHeaderStoreReStore();
	void SetupL();
	void TearDownL();
	
		
	static CTestSuite* CreateSuiteL(const TDesC& aName);

	void TestStoreHeaderL();
	void TestReStoreHeaderL();
	void InitialiseTestL();
	void SetRecipientsL(CSmsHeader& aHeader);
	
	void TestStoreReStoreSMSHeaderL();
	void TestReStoreSMSHeaderL();
	
	void SetEmptyRecipientsL(CSmsHeader& aHeader);
	
	void TestStoreReStoreEOSSMSHeaderL();
	void TestReStoreEOSHeaderL();
	
	
	void TestStoreReStoreEmptyEOSSMSHeaderL();
	void TestReStoreEmptyEOSHeaderL();

	void TestReStoreMultipuleEOSRecipientSMSHeaderL();
	void TestMultipuleEOSRecipientSMSHeaderL();
	
	void TestEmptyEOSSubjectSMSHeaderL();
	void TestReStoreEmptySubjectSMSHeaderL();
	
private:
	CSmsTestUtils* iTestUtils;
	EntryObserver* iEntryObserver;
	CMsvSession* iSession;
	SessionObserver* iSessionObserver;
	CMsvEntry* iEntry;
	CActiveScheduler* iScheduler;
	CTestUtilsWatcherStarter* iWatchers;
	CMsvOperation* iOperation;
	TMsvId iMessageId;
	CTestActive* iTestActive;
	CSmsHeader* iheader;
	CSmsHeader* iReciveheader;
	


};
