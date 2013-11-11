// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// GT Messaging Test Harness T_SmsSendRecv
// Test Harness: T_SmsSendRecv
// Component: SMS
// Owner: AA
// Brief Description: Sends and waits to receive SMS messages specified in sendrecv.script
// Detailed Description: As Above
// Input Files REQUIRED:
// <DRIVE>:\msgtest\sms\defaults.script
// <DRIVE>:\msgtest\sms\sendrecv.script
// Intermediate Files Produced: None
// Output files produced: <DRIVE>:\msglogs\T_SmsSendRecv.<PLATFORM>.<VARIANT>.log
// Building Instructions:
// cd \msg\smcm
// bldmake bldfiles
// abld test build
// Running Instructions:
// All Platforms:
// 1. Build T_DB test harness from COMMDB component:
// cd \commdb\group
// bldmake bldfiles
// abld test build t_db
// 2. Run T_DB and select the your modem (probably GSM Mobile Phone)
// 3. Build the test utilities:
// cd \msg\testutils\group\
// bldmake bldfiles
// abld test build
// 4. Edit \epoc32\wins\c\msgtest\sms\defaults.script. Replace the telephone number ("Recipients") with your mobile's number. You may also need to change the service centre ("SC") number (the default is for Vodafone UK).
// WINS Only:
// 1. Run T_DB.exe (if it has not already been run) and select your modem (GSM mobile phone).
// 2. \epoc32\release\wins\<VARIANT>\T_SmsSendRecv.exe can be used at the command prompt or executable can be run from Windows Explorer.
// Platform other than WINS:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_SmsSendRecv.exe onto the other platform (or onto the CF card)
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_DB.exe onto the other platform (or onto the CF card)
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MsvTestUtils.DLL into <DRIVE>:\system\libs on the other platform (or onto the CF card)
// 4. Copy \epoc32\release\<PLATFORM>\<VARIANT>\SmsTestUtils.DLL into <DRIVE>:\system\libs on the other platform (or onto the CF card)
// 5. Copy \epoc32\wins\c\msgtest\sms\sendrecv.script and defaults.script into <DRIVE>:\msgtest\sms on the other platform (or onto the CF card)
// 6. Run T_DB.exe on the other platform (if it has not already been run) and select your modem (GSM mobile phone).
// 7. Run T_SmsSendRecv.exe on the other platform.
// 
//

#include "T_SmsSendRecv.h"
#include <watcher.h>
#include <smuthdr.h>
#include <txtrich.h>
#include <bioscmds.h>
#include "biosmtm.h"
#include <smss.rsg>
#include <msgtestscripts.h>
#include <smsclnt.h>
#include <csmsaccount.h>


RTest test(_L("T_SmsSendRecv Testrig"));
CTrapCleanup* theCleanup;

const TInt KTestDescriptionLength	= 15;

LOCAL_C void doMainL()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install( scheduler );

	CSmsTestUtils* smsTest = CSmsTestUtils::NewL(test);
	CleanupStack::PushL(smsTest);
	TInt nextTest = 0;
	
	smsTest->NotifySaPhoneOnL();

	CSmsSendRecvTest* recvTest = CSmsSendRecvTest::NewLC(*smsTest, KSendRecvScript, nextTest);

	smsTest->WaitForInitializeL();

	recvTest->StartL();

	CleanupStack::PopAndDestroy(3); //SmsTest, scheduler
	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("Setup"));
	theCleanup = CTrapCleanup::New();
	TRAPD(ret,doMainL());		
	test(ret==KErrNone);
	delete theCleanup;	
	test.Console()->SetPos(0, 13);
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}

CEntryObserver* CEntryObserver::NewL(CSmsTestUtils& aTestUtils, const TInt& aCurrentMessageNum)
	{
	CEntryObserver* self = new (ELeave) CEntryObserver(aTestUtils, aCurrentMessageNum);
	CleanupStack::PushL(self);
	self->iReceived = new (ELeave) CMsvEntrySelection();
	CleanupStack::Pop(self);
	return self;
	}

CEntryObserver::CEntryObserver(CSmsTestUtils& aTestUtils, const TInt& aCurrentMessageNum)
: CBase(), iCurrentMessageNum(aCurrentMessageNum), iSmsTest(aTestUtils)
	{
	}


CEntryObserver::~CEntryObserver() 
/**
	destructor
 */
	{
	delete iReceived;
	}

void CEntryObserver::CheckReceivedMessageL(TMsvId aId)
/**
	Checks received message

	@param		aId			A message Id
 */
	{
	// Restore SMS settings
	iSmsTest.SetEntryL(iSmsTest.iSmsServiceId);

	CSmsAccount* account = CSmsAccount::NewLC();
	account->LoadSettingsL(*iSmsTest.iServiceSettings);
	CleanupStack::PopAndDestroy(account);

	iSmsTest.Printf(_L("Check last received message...\n"));

	iSmsTest.DisplayMessageL(aId);
	iSmsTest.SetEntryL(aId);

	CSmsHeader* header = iSmsTest.GetHeaderLC(aId);

	if (header->Type() == CSmsPDU::ESmsDeliver)
		{
 		CSmsDeliver& pdu = header->Deliver();

		TBool found = EFalse;
		TInt destination, originator, is16Bit;
		destination = originator = is16Bit = 0;
		if (pdu.UserDataPresent())
			{
			const CSmsUserData& userData = pdu.UserData();
			TInt count = userData.NumInformationElements();
			iSmsTest.Printf(_L("\tPdu contains %d information elements\n"), count);
			while (!found && count--)
				{
				CSmsInformationElement& nextIE=userData.InformationElement(count);
				switch(nextIE.Identifier())
					{
					case CSmsInformationElement::ESmsIEIApplicationPortAddressing8Bit:
					case CSmsInformationElement::ESmsIEIApplicationPortAddressing16Bit:
						{
						pdu.ApplicationPortAddressing(destination, originator, &is16Bit);
						found = ETrue;
						break;
						}
					default:
						break;
					}
				}
			}
		else
			iSmsTest.Printf(_L("\tPdu contains NO user data header\n"));

		if (found)
			iSmsTest.Printf(_L("\tApplication Port Addressing: Dest %d, Origin %d, is16Bit %d\n"), destination, originator, is16Bit);
		else
			iSmsTest.Printf(_L("\tApplication Port Addressing: Not Found\n"));
		}

	//Check the description
	const TMsvEntry& entry = iSmsTest.Entry();

	if (entry.iBioType == 0)
		{
		//Test description
		TInt descriptionLength = iSmsTest.iServiceSettings->DescriptionLength();
		HBufC* buf = HBufC::NewLC(descriptionLength);
		TPtr desc = buf->Des();
		User::LeaveIfError(TSmsUtilities::GetDescription(header->Message(), desc, descriptionLength));

		iSmsTest.Printf(_L("\tiDescription=\"%S\" desc=\"%S\"\n"), &entry.iDescription, &desc);
		iSmsTest(desc.CompareF(entry.iDescription) == 0);
		
		if (header->Type() == CSmsPDU::ESmsStatusReport)
			{
			iSmsTest.ReadResourceStringL(KSmsResourceFile,  R_TYPE_STATUS_REPORT, desc);
			iSmsTest.Printf(_L("\tiDescription=\"%S\" Res=\"%S\"\n"), &entry.iDescription, &desc);
			iSmsTest(desc.CompareF(entry.iDescription) == 0);
			}
		CleanupStack::PopAndDestroy(buf);
		}


	CleanupStack::PopAndDestroy(header);
	}


void CEntryObserver::HandleEntryEventL(TMsvEntryEvent aEvent, TAny* aArg1, TAny* /*aArg2*/, TAny* /*aArg3*/)
/**
	Handles entry event, used by MMsvEntryObserver

	@param		aEvent		Msv entry event
	@param		aArg1		Argument 1
	@param		aArg2		Argument 2 
	@param		aArg3		Argument 3 
 */
	{
	if (aEvent == EMsvNewChildren && iCurrentMessageNum > -1)
		{
		CMsvEntrySelection* msgs = (CMsvEntrySelection*)aArg1;

		TInt count = msgs->Count();

		TInt msgRecv = 0;

		while (count--)
			{
			TMsvId id = (*msgs)[count];

			CheckReceivedMessageL(id);
			CSmsHeader* header = iSmsTest.GetHeaderLC(id);

			if (header->Type() == CSmsPDU::ESmsDeliver)
				{
				iReceived->AppendL(id);
				msgRecv++;
				}

			CleanupStack::PopAndDestroy(header);
			}

		if (msgRecv)	
			iSmsTest.Printf(_L("%d Messages Received in inbox\n"), msgRecv);
		}
	}

CSmsSendRecvTest::~CSmsSendRecvTest()
/**
	destructor
 */
	{
	delete iEntryObserver;
	delete iEntryForObserver;
	delete iWatchers;

	iSocket.Close();
	iSocketServ.Close();
	}

TBool CSmsSendRecvTest::WatchersAlreadyRunningL()
/**
	Checks if the watchers are already running
 */
	{
	RSocketServ server;
	RSocket socket;

	User::LeaveIfError(server.Connect());
	CleanupClosePushL(server);

		// test if it's there... 
	TProtocolDesc protoInfo;
	TProtocolName protocolname;
	protocolname.Copy(KSmsDatagram);
	User::LeaveIfError(server.FindProtocol(protocolname,protoInfo));

	User::LeaveIfError(socket.Open(server,
										protoInfo.iAddrFamily,
										protoInfo.iSockType, 
										protoInfo.iProtocol));
	CleanupClosePushL(socket);

	TSmsAddr addr;
	addr.SetSmsAddrFamily(ESmsAddrRecvAny);
	TInt err = socket.Bind(addr);

	CleanupStack::Pop(2); //server, socket
	socket.CancelAll();
	socket.Close();
//	server.Close();
	return (err == KErrAlreadyExists);
	}

void CSmsSendRecvTest::StartWatcherL()
/**
	Starts the watcher if it is not running
 */
	{
	iSmsTest.Test().Next(_L("Start the Watcher"));

	if (WatchersAlreadyRunningL())
		{
		iSmsTest.Printf(_L("Watchers are already running\n"));
		return;
		}

	iWatchers = CTestUtilsWatcherStarter::NewL();

	TTimeIntervalMicroSeconds32 wait = KWaitForWatchersToStart;
	while (wait.Int() > 0)
		{
		iSmsTest.Printf(_L("Waiting %d secs for watchers to start...\n"), wait.Int() / 1000000);
		wait = wait.Int() - KWaitForWatchersToStartDisplayInterval.Int();
		User::After(KWaitForWatchersToStartDisplayInterval);
		}
	}

CSmsSendRecvTest* CSmsSendRecvTest::NewLC(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest)
	{
	CSmsSendRecvTest* self = new (ELeave) CSmsSendRecvTest(aSmsTest, aScriptFile, aNextTest);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


void CSmsSendRecvTest::TestSendingL()
/**
	Tests sending: Creates messages as defined in the script file
 */
	{
	iSmsTest.Test().Next(_L("Sending"));

	TTime now;
	now.HomeTime();
	now += (TTimeIntervalSeconds) 5;

	iSelection->Reset();

	iSmsTest.DeleteSmsMessagesL(KMsvGlobalOutBoxIndexEntryId);

	TBool read = EFalse;
	iSmsTest.Printf(_L("Creating msgs in outbox from script: %S\n"), &iScriptFile);
	TRAPD(err, read = iSmsTest.ReadScriptL(iScriptFile, KMsvGlobalOutBoxIndexEntryId, *iSelection, now));

	iSmsTest.Test()(!err && read);

	iSmsTest.Printf(_L("Send and Receive %d messages...\n"), iSelection->Count());

	TInt count = iSelection->Count();
	iTotal = 0;
	iSent = 0;
	while (count--)
		{
		iTotal += CountRecipientsL(iSelection->At(count));
		}

	iCurrentMessageNum = -1;
	}

TInt CSmsSendRecvTest::CountRecipientsL(TMsvId aId)
/**
	Counts the number receipients in aId

	@param		aId		A message Id 
 */
	{
	CSmsHeader* header = iSmsTest.GetHeaderLC(aId);
	const TInt count = header->Recipients().Count();
	CleanupStack::PopAndDestroy(header);
	return count;
	}

TBool CSmsSendRecvTest::SendNextMessageL()
/**
	Sends the next message 
 */
	{
	iCurrentMessageNum++;
	const TInt count = iSelection->Count();

	if (iCurrentMessageNum < count)
		{
		iSmsTest.Printf(_L("Sending message %d of %d\n"), iCurrentMessageNum + 1, count);

		delete iOperation;
		iOperation = NULL;

		CMsvEntrySelection* sel = new (ELeave) CMsvEntrySelection();
		CleanupStack::PushL(sel);
		const TMsvId id = iSelection->At(iCurrentMessageNum);
		
		TInt replace_msg_one_index = GetIndexOfSectionL(_L("ReplaceTypeSMSOne"));
		TInt replace_msg_two_index = GetIndexOfSectionL(_L("ReplaceTypeSMSTwo"));
		TInt msg_indication_index = GetIndexOfSectionL(_L("SMSMessageIndication"));

		TInt vCalendar_index = GetIndexOfSectionL(_L("WAP:vCalendar"));
 		// storing the id of vCalender message , to use it to test for replying bio messages
 		if(vCalendar_index != KErrNotFound && vCalendar_index == (iCurrentMessageNum+1))
 			iMsgVCalenderIndex = iCurrentMessageNum;

		if(replace_msg_one_index != KErrNotFound && replace_msg_one_index == (iCurrentMessageNum+1))
			SendReplaceTypeMessageL();
					
		if(replace_msg_two_index != KErrNotFound && replace_msg_two_index == (iCurrentMessageNum+1))
			{
			SendReplaceTypeMessageL();
			iSmsTest.Printf(_L("This Message replaces previous Replace Type Message"));
			}
				
		if(msg_indication_index != KErrNotFound && msg_indication_index == (iCurrentMessageNum+1))
			SendMessageIndicationL();
		
		iSmsTest.DisplayMessageL(id);
		sel->AppendL(id);
		iSent += CountRecipientsL(id);
		iSmsTest.SetEntryL(KMsvGlobalOutBoxIndexEntryId);
		iOperation = MsvEntry().CopyL(*sel, iSmsTest.iSmsServiceId, iStatus);
		CleanupStack::PopAndDestroy(); //sel
	
		iState = EStateSending;
		SetActive();
		CActiveScheduler::Start();
		return ETrue;
		}

	return EFalse;
	}

TInt CSmsSendRecvTest::GetIndexOfSectionL(const TDesC& aSectionName)
/**
 *  Returns back the index of the section
 */
	{
	CScriptFile* script = CScriptFile::NewLC(iSmsTest, KSmsComponent);
	script->ReadScriptL(KSendRecvScript);
		
	TInt index = KErrNotFound;
	TInt count = iSelection->Count();

	for (TInt i = 0; i <= count; i++) //order important
		{
		CScriptSection& tempsection = *script->Sections()[i];
		if (tempsection.SectionName().CompareF(aSectionName) == 0)
			index = i;
		}

	CleanupStack::PopAndDestroy(script);
	
	return index;
	}

void CSmsSendRecvTest::SendReplaceTypeMessageL()
/**
 *  Handles the replace type sms messages in sendrecv.script
 */
	{	
	iSmsTest.Printf(_L("\nSend Replace Type Message\n"));
	const TMsvId id = iSelection->At(iCurrentMessageNum);
	CSmsHeader* header = iSmsTest.GetHeaderLC(id);
	CSmsSubmit& submit = header->Submit();
	submit.SetPIDType(TSmsProtocolIdentifier::ESmsPIDShortMessageType);
	submit.SetShortMessageType(TSmsProtocolIdentifier::ESmsReplaceShortMessageType1);
	StoreMessageL(id, *header);
	CleanupStack::PopAndDestroy(header);
	}

void CSmsSendRecvTest::SendMessageIndicationL(TUint8 aMessageType, TUint8 aMessageCount)
/**
 * Sends an SMS message with Special SMS Message Indication set in the user data header.
 * 
 * @param aMessageType The type of the Special SMS Message Indication
 * @param aMessageCount The number of "messages" waiting
 */
	{
	iSmsTest.Printf(_L("\nSendMessageIndicationL\n"));
	const TMsvId id = iSelection->At(iCurrentMessageNum);
	CSmsHeader* header = iSmsTest.GetHeaderLC(id);
	TBuf8<2> data;
	data.SetLength(2);
				
	data[0] = aMessageType;
	data[1] = aMessageCount;

	header->Submit().UserData().AddInformationElementL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication, data);
	StoreMessageL(id, *header);
	CleanupStack::PopAndDestroy(header);
	}

void CSmsSendRecvTest::StoreMessageL(TMsvId aId, const CSmsHeader& aHeader)
/**
 * Stores the header against a message
 *
 * @param aId ID of the message
 * @param aHeader The CSmsHeader to store against the message
 */
	{
	iSmsTest.SetEntryL(aId);
	CMsvStore* store = iSmsTest.EditStoreL();
	CleanupStack::PushL(store);
	aHeader.StoreL(*store);
	store->StoreBodyTextL(*iSmsTest.iRichText);
	store->CommitL();
	CleanupStack::PopAndDestroy(store);
	}

void CSmsSendRecvTest::TestWaitForReceiveL()
/**
	Waits to receive message
 */
	{
	iSmsTest.Test().Next(_L("Waiting to receive message"));
	iState = EStateWaitingToReceive;
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

CSmsSendRecvTest::CSmsSendRecvTest(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest)
: CSmsTestBase(aSmsTest, aScriptFile, aNextTest), iCurrentMessageNum(-1), iState(EStateWaiting)
/**
	Constructor
 */
	{
	TBuf<256> command;
		User::CommandLine(command);
	
	command.Trim();
	iParse = (command.FindF(KParseAfterReceive) != KErrNotFound);
	iSaveToFile = (command.FindF(KSaveAfterReceive) != KErrNotFound);
	iTestBifs = (command.FindF(KTestBifs) != KErrNotFound);
	iWaitForever = (command.FindF(KWaitForever) != KErrNotFound);

	iMsgVCalenderIndex = KErrNotFound;
	// check to see if we want to run EMS tests
	if (command.FindF(KRunEmsTests) != KErrNotFound)
		{
		iScriptFile = KSendRecvEmsScript;
		}
	}

void CSmsSendRecvTest::RunAutoL()
/**
	Runs tests in sequence, checks iStatus
 */
	{	
	iSmsTest.TestStart(++iNextTest, _L("Start Watcher"));
	StartWatcherL();
	User::LeaveIfError(iStatus.Int());
	iSmsTest.TestFinish(iNextTest, KErrNone);
	
	if (!WatchersAlreadyRunningL())
		{
		iSmsTest.Printf(_L("WARNING: NBS Watcher has not started yet\n"));
		}

	if (!iSmsTest.iSmokeTest && iTestBifs)
		{
		iSmsTest.TestStart(++iNextTest, _L("Add Remove Bifs"));
		TestBifsL();
		User::LeaveIfError(iStatus.Int());
		iSmsTest.TestFinish(iNextTest, KErrNone);
		}

	if (!WatchersAlreadyRunningL())
		{
		iSmsTest.Printf(_L("WARNING: NBS Watcher has not started yet\n"));
		}

	// Change the description length to be shorter then the default - actual
	// value is not important only that it is different to the default of
	// KSmsDescriptionLength (32).

	iSmsTest.Printf(_L("\nChanging the description length to %d\n"), KTestDescriptionLength);
	
	// Restore settings first...	
	CSmsAccount* account = CSmsAccount::NewLC();
	account->LoadSettingsL(*iSmsTest.iServiceSettings);

	// Set the description length to 1
	iSmsTest.iServiceSettings->SetDescriptionLength(KTestDescriptionLength);

	// Store the sms settings
	account->SaveSettingsL(*iSmsTest.iServiceSettings);
	CleanupStack::PopAndDestroy(account);

	// Ok, now start real tests...

	iSmsTest.TestStart(++iNextTest, _L("Read Script"));
	TestSendingL();
	User::LeaveIfError(iStatus.Int());
	iSmsTest.TestFinish(iNextTest, KErrNone);

	FOREVER
		{
		iSmsTest.TestStart(++iNextTest, _L("Send Message"));
		TBool sendNext = SendNextMessageL();
		if (sendNext)
			{
			User::LeaveIfError(iStatus.Int());
			iSmsTest.TestFinish(iNextTest, KErrNone);
			}
		else
			{
			iSmsTest.Printf(_L("No more messages to send!"));
			iSmsTest.TestFinish(iNextTest, KErrNone);
 			// Testing Reply to Bio-Messages
  			TestReplyL();
			break;
			}
		}

	iSmsTest.TestStart(++iNextTest, _L("Wait to receive extra message(s)"));
	WaitForExtraL();
	User::LeaveIfError(iStatus.Int());
	iSmsTest.TestFinish(iNextTest, KErrNone);
	iSmsTest.Printf(_L("\nDisplay Final Log:\n"));
	DisplayLogL();
	}

void CSmsSendRecvTest::SetSmsSettingsL()
 	{
 	MsvEntry().SetEntryL(iSmsTest.iSmsServiceId);
 
// Lets create sms settings to set it for reply
 	CSmsSettings* settings = CSmsSettings::NewL();
 	CleanupStack::PushL(settings);
 	
 	settings->SetReplyQuoted(ETrue);
 
 	CSmsAccount* account = CSmsAccount::NewLC();
 	account->SaveSettingsL(*settings);
 	CleanupStack::PopAndDestroy(account);
 
 	iSmsTest.iServiceSettings->CopyL(*settings);
 	CleanupStack::PopAndDestroy();  //settings
 	}
 
// Now testing replying to received messages
void CSmsSendRecvTest::TestReplyL()
 	{
 	// Test cannot be performed as no vCalender message received in the inbox
 	if(iMsgVCalenderIndex == KErrNotFound)
 		return;
 	
	iSmsTest.TestStart(++iNextTest, _L("CSmsClientMtm Reply"));
	iSmsTest.Printf(_L("This test is to test Reply for bio-messages(vCalender)\n"));
		
 	SetSmsSettingsL();
 
 	iSmsTest.InstantiateClientMtmsL();
 
 	const TMsvId id = iEntryObserver->ItemReceived(iMsgVCalenderIndex);

	if(id == KErrNotFound)
		{
		iSmsTest.TestFinish(iNextTest, KErrNotFound);
		return;
		}
 	iSmsTest.SetEntryL(id);
 
 	Client().SwitchCurrentEntryL(id); // Lets point at the received message
 	Client().LoadMessageL();
	iSmsTest.Printf(_L("Display vCalender Message\n"));
	iSmsTest.DisplayMessageL(id);
 	
	TMsvPartList part=0;
 	CMsvOperationWait* wait = CMsvOperationWait::NewLC();
 	// Replying the vCalender message
 	CMsvOperation* op=Client().ReplyL(KMsvDraftEntryId, part, wait->iStatus);
 	CleanupStack::PushL(op);
 	wait->Start();
 	CActiveScheduler::Start();
 
 	TPckgBuf<TMsvId> pkg;
 	pkg.Copy(op->ProgressL());
 	TMsvId progress = pkg();
 
 	CleanupStack::PopAndDestroy(2); //op, wait

	CSmsHeader* header = iSmsTest.GetHeaderLC(progress);

	// extract the replying message
	iSmsTest.SetEntryL(progress);
	CMsvStore* store = iSmsTest.ReadStoreL();
	CleanupStack::PushL(store);
	header->RestoreL(*store);
	CleanupStack::PopAndDestroy(); //store

	const CSmsBufferBase& buffer = header->Message().Buffer();
	
	// let's check to see if the reply message has any body in it
	TInt bufLen=buffer.Length();
	CleanupStack::PopAndDestroy(); //header
	
 	if (bufLen)
 		{
 		iSmsTest.Printf(_L("vCalender Reply Message having junk data\n"));
		iSmsTest.TestFinish(iNextTest, KErrNotFound);
		}
 	else
		{
 		iSmsTest.Printf(_L("vCalender Reply Message is a Empty SMS message\n"));
		Client().SwitchCurrentEntryL(progress);
		Client().LoadMessageL();
  		Client().Body().InsertL(0,_L("This is reply"));
 		Client().SaveMessageL();
		iSmsTest.TestFinish(iNextTest, KErrNone);
  		}
  	}


void CSmsSendRecvTest::ConstructL()
	{
	SetTestNameL(KSmsSendRecvTestName);
	iSmsTest.SetLogToFile();

	TBuf<256> command;	
		User::CommandLine(command);
	
	command.Trim();
	iSmsTest.Printf(_L("Command Line: \"%S\" (Parse %d, Save %d, Wait %d, Bifs %d)\n"), &command, iParse, iSaveToFile, iWaitForever, iTestBifs);

	iSelection = new (ELeave) CMsvEntrySelection();
	iTimer = CTestTimer::NewL();
	CActiveScheduler::Add(this);

	iSmsTest.SetCommDbServiceCenterFromScriptL(iScriptFile);

	iEntryObserver = CEntryObserver::NewL(iSmsTest, iCurrentMessageNum);

	iEntryForObserver = Session().GetEntryL(KMsvGlobalInBoxIndexEntryId);
	iEntryForObserver->AddObserverL(*iEntryObserver);

	ClearLogL();

	User::LeaveIfError(iSocketServ.Connect());
	TProtocolDesc protoinfo;
	TProtocolName protocolname(KSmsDatagram);
	User::LeaveIfError(iSocketServ.FindProtocol(protocolname,protoinfo));
	User::LeaveIfError(iSocket.Open(iSocketServ,protoinfo.iAddrFamily,protoinfo.iSockType,protoinfo.iProtocol));
	
	TSmsAddr smsaddr;
	smsaddr.SetSmsAddrFamily(ESmsAddrSendOnly);
	User::LeaveIfError(iSocket.Bind(smsaddr));
	}

void CSmsSendRecvTest::ShowMenuL()
	{
	iSmsTest.ResetMenu();

	iSmsTest.AppendToMenuL(_L("Start"));
	iSmsTest.AppendToMenuL(_L("Display Log"));
	iSmsTest.AppendToMenuL(_L("Clear Log"));
	iSmsTest.AppendToMenuL(_L("Navigate Message Store"));

	TInt result = iSmsTest.DisplayMenu(_L("SMS Send Receive Test"));

	if (result <= 0)
		return;

	switch (result)
		{
		case 1:
			RunAutoL();
			return;
		case 2:
			DisplayLogL();
			break;
		case 3:
			ClearLogL();
			break;
		case 4:
			iSmsTest.NavigateMessageStoreL();
			break;
		default:
			User::Leave(KErrArgument);
			break;
		}

	ShowMenuL();
	}

void CSmsSendRecvTest::RunComplete()
	{
	if (!IsActive())
		CActiveScheduler::Stop();
	}

void CSmsSendRecvTest::RunL()
/**
	Handles completed async operations
 */
	{
	switch (iState)
		{
		case EStateSending:
			{
			DoRunSendingL();
			delete iOperation;
			iOperation = NULL;
			break;
			}
		case EStateWaitingToReceive:
			{
			DoRunWaitingToReceiveL();
			break;
			}
		case EStateWaitingToReceiveAnyExtra:
			{
			DoRunWaitForExtraL();
			break;
			}
		case EStateMessageReceived:
			{
			TestParseL();
			break;
			}
		case EStateParsing:
			{
			DoRunTestParseL();
			break;
			}
		default:
			{
			break;
			}
		}

	RunComplete();
	}

TInt CSmsSendRecvTest::RunError(TInt aError)
	{
	aError = CSmsTestBase::RunError(aError);
	RunComplete();
	return aError;
	}

void CSmsSendRecvTest::DoRunTestParseL()
/**
	Checks status
 */
	{
	TBioProgress progress;
	TBioProgressBuf progressBuf;
	progressBuf.Copy(iOperation->ProgressL());
	progress = progressBuf();

	if (iStatus == KErrNone)
		iStatus = progress.iErrorCode;

	delete iOperation;
	iOperation = NULL;
	}

void CSmsSendRecvTest::TestParseL()
/**
	Parses BIO messages
 */
	{
	const TMsvId id = iEntryObserver->LastReceived();

	iSmsTest.SetEntryL(id);

	if (iSaveToFile)
		{
		iSmsTest.Printf(_L("\tSaving BIO message to file\n"));
		iSmsTest.SaveBodyToFileL(id);
		}

	if (!iParse)
		return;

	if (!iSmsTest.Entry().iBioType)
		return;

	iState = EStateParsing;
	
	iSmsTest.Printf(_L("Parsing BIO message\n"));

	CMsvEntrySelection* sel = new (ELeave) CMsvEntrySelection();
	CleanupStack::PushL(sel);

	sel->AppendL(id);

	delete iOperation;
	iOperation = NULL;
	
	iOperation = Session().TransferCommandL(*sel, KBiosMtmParse, KNullDesC8, iStatus);
	SetActive();

	CleanupStack::PopAndDestroy(sel);
	}

void CSmsSendRecvTest::DoCancel()
	{
	if (iOperation)
		{
		iOperation->Cancel();
		iSmsTest.Printf(_L("Operation Cancelled!\n"));
		}
	delete iOperation;
	iOperation = NULL;
	}

void CSmsSendRecvTest::DoRunSendingL()
/**
	Checks the status and displays the sending states
 */
	{
	if (iOperation)
		iSmsTest.SetProgressL(*iOperation);

	TInt count = iSelection->Count();
	TBool wait = EFalse;

	while (iStatus == KErrNone && count--)
		{
		TRAPD(err, iSmsTest.SetEntryL(iSelection->At(count)));

		if (err == KErrNone)
			{
			TMsvEntry entry = MsvEntry().Entry();
			iSmsTest.SetEntryL(KMsvGlobalOutBoxIndexEntryId);

			TBuf<0x100> output;
			TBuf<0x50>  nowBuf;

			switch (entry.SendingState())
				{
				case KMsvSendStateSending:
					output.AppendFormat(_L("Message %d Sending with error %d\n"), entry.Id(), entry.iError);
					wait = ETrue;
					break;
				case KMsvSendStateResend:
					output.AppendFormat(_L("Message %d Resend with error %d at "), entry.Id(), entry.iError);
					entry.iDate.FormatL(nowBuf, _L("%-B%:0%J%:1%T%:2%S%:3%+B"));
					output.Append(nowBuf);
					output.Append(_L("\n"));
					wait = ETrue;
					break;
				case KMsvSendStateScheduled:
					output.AppendFormat(_L("Message %d Scheduled at "), entry.Id());					entry.iDate.FormatL(nowBuf, _L("%-B%:0%J%:1%T%:2%S%:3%+B"));
					entry.iDate.FormatL(nowBuf, _L("%-B%:0%J%:1%T%:2%S%:3%+B"));
					output.Append(nowBuf);
					output.Append(_L("\n"));
					wait = ETrue;
					break;
				case KMsvSendStateFailed:
					output.AppendFormat(_L("Message %d Failed with error %d\n"), entry.Id(), entry.iError);
					if (entry.iError)
						iStatus = entry.iError;
					else
						iStatus = KErrGeneral;

					break;
				case KMsvSendStateWaiting:
					output.AppendFormat(_L("Message %d Waiting with error %d\n"), entry.Id(), entry.iError);
					wait = ETrue;
					break;
				case KMsvSendStateSent:
				default:
					break;
					//nothing
				}

			iSmsTest.Printf(output);
			}
		}

	if (wait)
		{
		iTimer->AfterReq(10000000, iStatus);
		SetActive();
		iState = EStateSending;
//		CActiveScheduler::Start();
		}
	else
		{
		iSmsTest.Printf(_L("Sending completed with %d\n"), iStatus);
		
		if (iStatus == KErrNone)
			{
			TestWaitForReceiveL();
			}
		else
			{
			iState = EStateWaiting;
			}
		}
	}

void CSmsSendRecvTest::WaitForExtraL()
/**
	Waits for any extra messages to arrive 
 */
	{
	iSmsTest.Printf(_L("Waiting for any extra to arrive...\n"));
	iWaitForever = EFalse;
	iCountWaits = 0;
	iState = EStateWaitingToReceiveAnyExtra;
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	CActiveScheduler::Start();
	}

void CSmsSendRecvTest::DoRunWaitForExtraL()
/**
	Checks if it has received all messages sent
 */
	{
	iCountWaits++;

	TInt recvCount = iEntryObserver->Count();

	if (recvCount > iSent)
		{
		iSmsTest.Printf(_L("Too many messages received:\nSent %d Rcvd %d\n"), iSent, recvCount);
		iStatus = KErrOverflow;
		}
	else
		DoWaitL();
	}

void CSmsSendRecvTest::DoRunWaitingToReceiveL()
/**
	Checks if it has received all messages sent
 */
	{
	iCountWaits++;

	const TInt recvCount = iEntryObserver->Count();

	if (recvCount == iSent)
		{
		//Message received
		if (!iSmsTest.AreMessagesSameTextL((*iSelection)[iCurrentMessageNum], iEntryObserver->Received(iSent - 1), ETrue))
			{
			//Do something?
			}
		}

	if (recvCount > iSent)
		{
		iSmsTest.Printf(_L("Too many messages received:\nSent %d Rcvd %d\n"), iSent, recvCount);
		iStatus = KErrOverflow;
		}
	else 
		{
		if (iTotal == recvCount || recvCount == iSent)
			{
			if (iTotal == recvCount)
				iSmsTest.Printf(_L("All Messages Received!!\n"));

			//Message received
			iState = EStateMessageReceived;
			iCountWaits = 0;
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrNone);
			SetActive();
			}
		else
			{
			DoWaitL();
			}
		}
	}
	
void CSmsSendRecvTest::DoWaitL()
/**
	Waits to recive the messages sent
 */
	{
	TInt recvCount = iEntryObserver->Count();
	
	TInt maxCount = KMaxWaitForMsg.Int() / KWaitForReceiveIteration.Int();
	TInt waitSecs = KWaitForReceiveIteration.Int() * (maxCount - iCountWaits) / 1000000;
	
	if (iWaitForever && iCountWaits <= 1)
		{
		iSmsTest.Printf(_L("Waiting forever to recv msg\n"));
		}
	else if (!iWaitForever)
		{
		iSmsTest.Printf(_L("Waiting %d secs to recv msg\n"), waitSecs);
		}
	
	if (iWaitForever || iCountWaits < maxCount)
		{
		iTimer->AfterReq(KWaitForReceiveIteration, iStatus);
		SetActive();
		}
	else
		{
		if (iSent != recvCount)
			{
			iSmsTest.Printf(_L("Time Out: Message NOT Received\n"));
			iStatus = KErrTimedOut;
			}
		}
	}
	
void CSmsSendRecvTest::TestBifsL()
/**
	todo Comments
 */
	{
	iSmsTest.Printf(_L("Remove 2 Bifs if already exist\n"));	
	TestRemoveBifL(KTestBif1);
	TestRemoveBifL(KTestBif2);
	
	//Test add bif
	iSmsTest.Printf(_L("Test Add 2 Bifs\n"));	
	TestAddBifL(KTestBif1);
	TestAddBifL(KTestBif2);

	//Test remove bif
	iSmsTest.Printf(_L("Test Remove 2 Bifs\n"));	
	TestRemoveBifL(KTestBif1);
	TestRemoveBifL(KTestBif2);

	//Test change bif
	iSmsTest.Printf(_L("Test Change 2 Bifs\n"));	
	TestAddBifL(KTestBif1);
	TestAddBifL(KTestBif2);
	TestAddBifL(KTestBif1);
	TestAddBifL(KTestBif2);

	//Test remove bif
	iSmsTest.Printf(_L("Test Remove 2 Bifs\n"));	
	TestRemoveBifL(KTestBif1);
	TestRemoveBifL(KTestBif2);
	}

void CSmsSendRecvTest::TestAddBifL(const TDesC& aBifName)
/**
	Comments
 */
	{
	iSmsTest.Printf(_L("\tAdd BIF: %S\n"), &aBifName);	
	
	RFs& fs = iSmsTest.FileSession();
	TInt err = fs.MkDir(KBifDir);

	if (err != KErrAlreadyExists)
		User::LeaveIfError(err);

	TParse fileOut;
	err = iSmsTest.ResolveFile(KSmsComponent, aBifName, fileOut);

	if (err)
		{
		iSmsTest.Printf(_L("Test BIF %S not found! Continuing without this test\n"), &aBifName);
		return;
		}

	CFileMan* fileMan = CFileMan::NewL(fs);
	CleanupStack::PushL(fileMan);

	err = fileMan->Copy(fileOut.FullName(), KBifDir);

	if (err != KErrAlreadyExists)
		User::LeaveIfError(err);

	CleanupStack::PopAndDestroy(fileMan);

	User::After(KBifWait);
	}

void CSmsSendRecvTest::TestRemoveBifL(const TDesC& aBifName)
/**
	todo Comments
 */
	{
	iSmsTest.Printf(_L("\tRemove Bif: %S\n"), &aBifName);	

	RFs& fs = iSmsTest.FileSession();
	TFileName fileName(KBifDir);
	fileName.Append(aBifName);

	TInt err = fs.Delete(fileName);

	if (err)
		{
		iSmsTest.Printf(_L("\t\tfs.Delete() returned %d\n"), err);	
		
		if (err != KErrNotFound && err != KErrPathNotFound)
			User::Leave(err);
		}

	User::After(KBifWait);
	}

