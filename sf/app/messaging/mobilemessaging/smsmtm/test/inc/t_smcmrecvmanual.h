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

class CSmcmRecvActive;
class CSmcmRecvOnly : public CSmsTestBase
/**
 * Manual test harness for receiving SMS messages
 * @note Also enables the user to also send messages to themselves. Defaults.script must have the correct telephone number
 */
	{
public:
	static CSmcmRecvOnly* NewLC(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest);
	~CSmcmRecvOnly();

private:
	CSmcmRecvOnly(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest);

	void ConstructL();

	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
	void Complete(TInt aStatus);

	void ShowMenuL();
	void MessageSubMenuL();

	void TestSendingL() {};
	void TestSchedulingL() {};

	void RunAutoL();
	void DoRunMainMenuL(TKeyCode aCode);
	void DoRunSendingMenuL(TKeyCode aCode);
	void DoRunSendingL();
	void DisplayAndRead(const TDesC& aText);
	void Read();
	void ReshowMenu();

	void SendShortMessageL();
	void SendLongMessageL();
	void SendReplaceTypeL();
	void SendMessageIndicationL(TUint8 aMessageType = TSmsUtilities::EVoiceMessageWaiting, TUint8 aMessageCount=1);
	void SendFromScriptL();
	void ScheduleSendL(); //< Sends all messages in iSelection
	void StoreMessageL(TMsvId aId, const CSmsHeader& aHeader);
	TMsvId CreateMessageLC(const TDesC& aBody);

private:

	enum TSmcmRecvOnlyState
		/**
		 *
		 */
		{
		EMainMenu,		//< Main menu currently displayed
		ESendingMenu,	//< Sending menu currently displayed
		ESending		//< Sending SMS messages
		} iState;

	CSmcmRecvActive* iRecvActive;			//< Active object that waits to receive SMS messages then displays them
	CTestUtilsWatcherStarter* iWatchers;	//< Start and stop the watchers
	HBufC* iRecipient;						//< Default recipient read from defaults.script
	HBufC* iServiceCenter;					//< Default service center read from defaults.script
	TInt iReplaceTypeCount;					//< Number of replace type messages sent
	};

class CSmcmRecvActive : public CActive, MMsvSessionObserver
/**
 * Active object that waits to receive SMS messages then displays them
 */
	{
	public:

		static CSmcmRecvActive* NewL(CSmsTestUtils& aSmsTest, TInt aPriority);
		~CSmcmRecvActive();
		void SentL(const CMsvEntrySelection& aSelection);
		TInt UnMatched() const {return iSent->Count() - iMatched->Count();}
		TInt Sent() const {return iSent->Count() + iMatched->Count();}
		TInt Received() const {return iReceived->Count() + iMatched->Count();}
		void DisplayUnmatched() const;

	private:

		CSmcmRecvActive(CSmsTestUtils& aSmsTest, TInt aPriority);
		void ConstructL();
		void RunL();
		void DoCancel();
		TInt RunError(TInt aError);
		void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);
		void PrintMessageL(TMsvId aId);
		void ProcessReceivedL();
		TBool IsMatchL(CMsvEntry& aLeft, CMsvEntry& aRight);
		TInt Find(const CMsvEntrySelection& aSelection, TMsvId aId) const;
		HBufC* GetBodyTextLC(CMsvEntry& aMessage);
		CSmsHeader* GetHeaderLC(CMsvEntry& aMessage) const;

	private:

		CSmsTestUtils& iSmsTest;
		RTimer iTimer;					//< Timer used before displaying received messages
		CMsvEntrySelection* iSelection;	//< Received messaged awaiting processing
		CMsvEntrySelection* iSent;		//< All sent messages
		CMsvEntrySelection* iReceived;	//< Received messages not matched with sent messages
		CMsvEntrySelection* iMatched;	//< Received messages matched with sent messages
	};
