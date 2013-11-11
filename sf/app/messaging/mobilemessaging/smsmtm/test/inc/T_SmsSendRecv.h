// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "smstestbase.h"

#include <msvapi.h>
#include <smsuaddr.h>

_LIT(KSmsSendRecvTestName, "Send and Receive");

class CEntryObserver : public CBase, public MMsvEntryObserver
	{
public:
	void HandleEntryEventL(TMsvEntryEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);
	~CEntryObserver();
public:

	static CEntryObserver* NewL(CSmsTestUtils& aTestUtils, const TInt& aCurrentMessageNum);
	const CMsvEntrySelection& Received() const {return *iReceived;}
	TMsvId Received(TInt aItem) const {return iReceived->At(aItem);}
	TInt Count() const {return iReceived->Count();}
	TMsvId LastReceived() const {return Count() ? Received(Count() - 1) : KErrNotFound;}
	TMsvId ItemReceived(TInt aItem) const {return Count() ? Received(aItem) : KErrNotFound;}

private:

	void CheckReceivedMessageL(TMsvId aId);
	CEntryObserver(CSmsTestUtils& aTestUtils, const TInt& aCurrentMessageNum);
	
	CMsvEntrySelection* iReceived;			//< An array containing the received messages
	const TInt& iCurrentMessageNum;			//< Number of the current message
	CSmsTestUtils& iSmsTest;				//< Sms Test Util
	};

const TTimeIntervalMicroSeconds32 KWaitForReceiveIteration = 2000000;
const TTimeIntervalMicroSeconds32 KMaxWaitForMsg = 90000000;
const TTimeIntervalMicroSeconds32 KWaitForWatchersToStart = 20000000;

const TTimeIntervalMicroSeconds32 KWaitForWatchersToStartDisplayInterval = 5000000;
const TTimeIntervalMicroSeconds32 KWaitForWatchersToDie = 2000000;

//Command Line Switches
_LIT(KParseAfterReceive, "/p");
_LIT(KSaveAfterReceive, "/s");
_LIT(KWaitForever, "/w");
_LIT(KTestBifs, "/b");
_LIT(KRunEmsTests, "/e"); // if on, uses ems script instead

//Test Bifs
_LIT(KSendRecvScript, "sendrecv.script");
_LIT(KSendRecvEmsScript, "sendrecv_ems.script");
_LIT(KTestBif1, "Koala.rsc");
_LIT(KTestBif2, "Giraffe.rsc");
_LIT(KBifDir, "c:\\system\\bif\\"); 
const TTimeIntervalMicroSeconds32 KBifWait = 4000000;

class CSmsSendRecvTest : public CSmsTestBase
	{
public:
	static CSmsSendRecvTest* NewLC(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest);
	~CSmsSendRecvTest();

private:
	CSmsSendRecvTest(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest);

	TBool WatchersAlreadyRunningL();

	void TestSendingL();
	TBool SendNextMessageL();
	void StartWatcherL();
	void TestWaitForReceiveL();
	void TestParseL();
	void DoRunTestParseL();

	void TestBifsL();
	void TestAddBifL(const TDesC& aBifName);
	void TestRemoveBifL(const TDesC& aBifName);

	void ConstructL();

	void RunL();
	void DoCancel();

	TInt RunError(TInt aError);
	void RunComplete();

	void ShowMenuL();
	void RunAutoL();

	void DoRunSendingL();
	void DoRunWaitingToReceiveL();
	void DoWaitL();

	void WaitForExtraL();
	void DoRunWaitForExtraL();

	TInt CountRecipientsL(TMsvId aId);
	void StoreMessageL(TMsvId aId, const CSmsHeader& aHeader);

	void SendMessageIndicationL(TUint8 aMessageType = TSmsUtilities::EVoiceMessageWaiting, TUint8 aMessageCount=1);
	void SendReplaceTypeMessageL();
	TInt GetIndexOfSectionL(const TDesC& aSectionName);
	void TestReplyL();
 	void SetSmsSettingsL();


	CEntryObserver* iEntryObserver;			//< An Entry Observer

	TInt iSent;								//< Total number of messages sent
	TInt iTotal;							//< Total number of messages received

	TInt iCountWaits;						//< Count to wait 
	
	CTestUtilsWatcherStarter* iWatchers;	//< A Watcher
	CMsvEntry* iEntryForObserver;			//< A Msv Entry
	TInt iCurrentMessageNum;				//< The number of the current message
	TBool iParse;							//< A boolean
	TBool iSaveToFile;						//< A boolean
	TBool iWaitForever;						//< A boolean
	TInt iMsgVCalenderIndex;				//< store the index of vCalender received message
	
	enum TSmsSendTestState
	/**	@enum	TSmsSendTestState
	 *	Enum defining the states of this test harness
	 */
		{
		EStateWaiting,
		EStateSending,
		EStateWaitingToReceive,
		EStateParsing,
		EStateMessageReceived,
		EStateWaitingToReceiveAnyExtra
		} iState;

	RSocketServ iSocketServ;	//< A Socket Server
	RSocket iSocket;			//< A Socket
	TBool iTestBifs;			
	};
