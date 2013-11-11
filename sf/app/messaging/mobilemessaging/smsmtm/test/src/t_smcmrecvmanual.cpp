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

#include "t_smcmrecvmanual.h"
#include <msvuids.h>
#include <smuthdr.h>
#include <smscmds.h>
#include <msgtestscripts.h>
#include <txtrich.h>

_LIT(KSmcmRecvOnlyTestName, "Receive SMS Messages");
_LIT(KHelpText, "\n[1] Start Watchers\n[2] Stop Watchers\n[3] Send myself message\n[D] Display Unmatched Messages\ne[X]it\n");
_LIT(KMessageText, "\n[1] Short Message\n[2] Long Message\n[3] Message Indication\n[4] Replace Type\n[5] From sendrecv.script\n[D] Display Unmatched Messages\ne[X]it\n");
_LIT(KSmsScript, "sendrecv.script");
_LIT(KDefaultScript, "defaults.script");
_LIT(KRecipients, "Recipients");
_LIT(KServiceCenter, "SC");
const TInt KWaitAfterRecv = 100000; //< 0.1 second

RTest test(_L("T_SmcmRecvManual: Receive SMS Messages"));
CTrapCleanup* theCleanup;

LOCAL_C void doMainL()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install( scheduler );

	CSmsTestUtils* smsTest = CSmsTestUtils::NewL(test);
	CleanupStack::PushL(smsTest);
	TInt nextTest = 0;
	
	smsTest->NotifySaPhoneOnL();

	CSmcmRecvOnly* scTest = CSmcmRecvOnly::NewLC(*smsTest, KSmsScript, nextTest);

	scTest->StartL();

	CActiveScheduler::Start();

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

CSmcmRecvOnly::~CSmcmRecvOnly()
/**
 * CSmcmRecvOnly destructor
 */
	{
	Cancel();
	delete iRecvActive;
	delete iWatchers;
	delete iOperation;
	iOperation = NULL;

	delete iRecipient;
	delete iServiceCenter;

	delete iSelection;
	iSelection = NULL;
	}

CSmcmRecvOnly* CSmcmRecvOnly::NewLC(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest)
/**
 * CSmcmRecvOnly factory function
 */
	{
	CSmcmRecvOnly* self = new (ELeave) CSmcmRecvOnly(aSmsTest, aScriptFile, aNextTest);
	CleanupStack::PushL(self);

	self->ConstructL();
	return self;
	}

CSmcmRecvOnly::CSmcmRecvOnly(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest)
: CSmsTestBase(aSmsTest, aScriptFile, aNextTest)
/**
 * CSmcmRecvOnly constructor
 */
	{
	CActiveScheduler::Add(this);
	}

void CSmcmRecvOnly::DisplayAndRead(const TDesC& aText)
/**
 * Displays aText then issues a read on the console
 */
	{
	iSmsTest.Printf(aText);
	Read();
	}

void CSmcmRecvOnly::RunAutoL()
	{
	DisplayAndRead(KHelpText);
	}

void CSmcmRecvOnly::ConstructL()
	{
	iSmsTest.NotifySaPhoneOnL();
	SetTestNameL(KSmcmRecvOnlyTestName);
	iSmsTest.InstantiateClientMtmsL();

	iSmsTest.SetLogToFile();
	iRecvActive = CSmcmRecvActive::NewL(iSmsTest, Priority()-1);
	iSelection = new (ELeave) CMsvEntrySelection;

	User::After(1000000);

	CScriptFile* script = CScriptFile::NewLC(iSmsTest, KSmsComponent);
	script->ReadScriptL(KDefaultScript);

	iRecipient = script->ItemValue(KDefaults, KRecipients, KNullDesC).AllocL();
	iServiceCenter = script->ItemValue(KDefaults, KServiceCenter, KNullDesC).AllocL();

	CleanupStack::PopAndDestroy(script);

	User::After(1000000);

	iSmsTest.Printf(_L("Deleting SMS messages in outbox..."));
	iSmsTest.DeleteSmsMessagesL(KMsvGlobalOutBoxIndexEntryId);

	User::After(1000000);
	
	iSmsTest.Printf(_L("Done\nDeleting SMS messages in sent..."));
	iSmsTest.DeleteSmsMessagesL(KMsvSentEntryId);

	User::After(1000000);
	
	iSmsTest.Printf(_L("Done\nDeleting SMS messages in inbox..."));
	iSmsTest.DeleteSmsMessagesL(KMsvGlobalInBoxIndexEntryId);

	iSmsTest.Printf(_L("Done\n"));
	}

void CSmcmRecvOnly::ShowMenuL()
	{
	RunAutoL();
	}

void CSmcmRecvOnly::DoRunMainMenuL(TKeyCode aCode)
/**
 * Called by RunL when iState == EMainMenu, when there is a key press event.
 * Acts on the user's request.
 */
	{
	const TChar ch = TChar(aCode);
	const TDesC* text = &KHelpText();

	switch (ch)
		{
		case '1':

			iSmsTest.Printf(_L("\nStarting Watchers\n"));

			if (iWatchers == NULL)
				iWatchers = CTestUtilsWatcherStarter::NewL(Priority());
			else
				iSmsTest.Printf(_L("OOPS: Watchers already started!!\n"));

			break;

		case '2':

			iSmsTest.Printf(_L("\nStopping Watchers\n"));

			if (iWatchers == NULL)
				iSmsTest.Printf(_L("OOPS: Watchers not started yet!!\n"));
			else
				{
				delete iWatchers;
				iWatchers = NULL;
				}
			break;

		case '3':

			text = &KMessageText();
			iState = ESendingMenu;
			break;

		case 'D':
		case 'd':

			iRecvActive->DisplayUnmatched();
			break;

		case 'x':
		case 'X':
		case EKeyEscape:

			text = NULL;
			Complete(KErrNone);
			break;

		case EKeySpace:

			ReshowMenu();
			break;

		default:

			Read();
			break;
		}

	if (!IsActive() && text != NULL)
		DisplayAndRead(*text);
	}

void CSmcmRecvOnly::Read()
/**
 * Issues an asynchronous read on the console
 */
	{
	Cancel();
	iSmsTest.Test().Console()->Read(iStatus);
	SetActive();
	}

void CSmcmRecvOnly::SendReplaceTypeL()
	{
	iSmsTest.Printf(_L("\nSendReplaceTypeL\n"));
	_LIT(KShortMessage, "Replace Type %d");
	TBuf<64> buf;
	buf.AppendFormat(KShortMessage, iReplaceTypeCount++);

	const TMsvId id = CreateMessageLC(buf);

	CSmsHeader* header = iSmsTest.GetHeaderLC(id);

	CSmsSubmit& submit = header->Submit();
	submit.SetPIDType(TSmsProtocolIdentifier::ESmsPIDShortMessageType);
	submit.SetShortMessageType(TSmsProtocolIdentifier::ESmsReplaceShortMessageType1);
	StoreMessageL(id, *header);
	CleanupStack::PopAndDestroy(header);

	iSelection->Reset();
	iSelection->AppendL(id);
	ScheduleSendL();
	iSmsTest.iMsvSession->CleanupEntryPop(); //id
	}

void CSmcmRecvOnly::DoRunSendingMenuL(TKeyCode aCode)
/**
 * Called by RunL when iState == ESendingMenu, when there is a key press event.
 * Acts on the user's sending request.
 */
	{
	const TChar ch = TChar(aCode);

	switch (ch)
		{
		case '1':
			SendShortMessageL();
			break;
		case '2':
			SendLongMessageL();
			break;
		case '3':
			SendMessageIndicationL();
			break;

		case '4':
			SendReplaceTypeL();
			break;

		case '5':
			SendFromScriptL();
			break;

		case 'D':
		case 'd':

			iRecvActive->DisplayUnmatched();
			ReshowMenu();
			break;

		case 'x':
		case 'X':
		case EKeyEscape:

			DisplayAndRead(KHelpText);
			iState = EMainMenu;
			break;

		case EKeySpace:
			
			ReshowMenu();
			break;

		default:

			Read();
			break;
		}
	}

void CSmcmRecvOnly::ReshowMenu()
/**
 * Redisplays the current menu
 */
	{
	const TDesC* menu = NULL;

	switch (iState)
		{
		case EMainMenu:
			menu = &KHelpText();
			break;
		case ESendingMenu:
			menu = &KMessageText();
			break;

		default:
			break;
		}

	if (menu != NULL)
		{
		DisplayAndRead(*menu);
		}
	}

void CSmcmRecvOnly::SendFromScriptL()
/**
 * Sends SMS messages from sendrecv.script
 */
	{
	iSmsTest.Printf(_L("\nSendFromScriptL\n"));
	iSelection->Reset();
	TTime now;
	now.HomeTime();
	iSmsTest.ReadScriptL(iScriptFile, KMsvGlobalOutBoxIndexEntryId, *iSelection, now);
	ScheduleSendL();
	}

void CSmcmRecvOnly::SendLongMessageL()
/**
 * Sends a 2-part concatenated SMS message
 */
	{
	iSmsTest.Printf(_L("\nSendLongMessageL\n"));

	_LIT(KLongMessage, "LungoLungoLungoLungoLungoLungoLungoLungoLungoLungoLungoLungoLungoLungoLungoLungoLungoLungoLungoLungoLungoLungoLungoLungoLungoLungoLungoLungoLungoLungoLungoLungoLungoLungoLungo");
	const TMsvId id = CreateMessageLC(KLongMessage);
	iSelection->Reset();
	iSelection->AppendL(id);
	ScheduleSendL();
	iSmsTest.iMsvSession->CleanupEntryPop(); //id
	}

TMsvId CSmcmRecvOnly::CreateMessageLC(const TDesC& aBody)
/**
 * Creates a message in the outbox with body text aBody
 *
 * @param aBody Body text of the new message
 * @return ID of the new message
 */
	{
	const TMsvId id = iSmsTest.CreateDummyMessageToSendL();
	iSmsTest.iMsvSession->CleanupEntryPushL(id);

	CSmsHeader* header = iSmsTest.GetHeaderLC(id);

	header->Recipients().ResetAndDestroy();
	CSmsNumber* number = CSmsNumber::NewL();
	CleanupStack::PushL(number);
	number->SetAddressL(*iRecipient);
	number->SetNameL(*iRecipient);
	header->Recipients().AppendL(number);
	CleanupStack::Pop(number);

	header->Submit().SetServiceCenterAddressL(*iServiceCenter);

	TMsvEntry entry(iSmsTest.Entry());
	entry.iDate.HomeTime();
	iSmsTest.ChangeEntryL(entry);


	_LIT(KDateFormat, " %-B%:0%J%:1%T%:2%S%.%*C4%:3%+B %D%M%Y%/0%1%/1%2%/2%3%/3");
	TBuf<64> dateString;
	entry.iDate.FormatL(dateString, KDateFormat);
	iSmsTest.iRichText->Reset();
	iSmsTest.iRichText->InsertL(0, dateString);
	iSmsTest.iRichText->InsertL(0, aBody);

	StoreMessageL(id, *header);

	CleanupStack::PopAndDestroy(header);

	return id;
	}

void CSmcmRecvOnly::SendShortMessageL()
/**
 * Sends a single-part SMS message
 */
	{
	iSmsTest.Printf(_L("\nSendShortMessageL\n"));
	_LIT(KShortMessage, "ridere");
	const TMsvId id = CreateMessageLC(KShortMessage);
	iSelection->Reset();
	iSelection->AppendL(id);
	ScheduleSendL();
	iSmsTest.iMsvSession->CleanupEntryPop(); //id
	}

void CSmcmRecvOnly::StoreMessageL(TMsvId aId, const CSmsHeader& aHeader)
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

void CSmcmRecvOnly::ScheduleSendL()
/**
 * Sends all messages in iSelection
 * @note Messages are actually scheduled. Actual ending occurs a few seconds later.
 */
	{
	iState = ESending;
	delete iOperation;
	iOperation = NULL;
	iOperation = iSmsTest.iMsvSession->TransferCommandL(*iSelection, ESmsMtmCommandScheduleCopy, KNullDesC8, iStatus);
	SetActive();
	}

void CSmcmRecvOnly::SendMessageIndicationL(TUint8 aMessageType, TUint8 aMessageCount)
/**
 * Sends an SMS message with Special SMS Message Indication set in the user data header.
 *
 * @param aMessageType The type of the Special SMS Message Indication
 * @param aMessageCount The number of "messages" waiting
 */
	{
	iSmsTest.Printf(_L("\nSendMessageIndicationL\n"));
	_LIT(KShortMessage, "SMI");
	const TMsvId id = CreateMessageLC(KShortMessage);

	CSmsHeader* header = iSmsTest.GetHeaderLC(id);

	TBuf8<2> data;
	data.SetLength(2);
	data[0] = aMessageType;
	data[1] = aMessageCount;

	header->Submit().UserData().AddInformationElementL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication, data);
	StoreMessageL(id, *header);
	CleanupStack::PopAndDestroy(header);

	iSelection->Reset();
	iSelection->AppendL(id);
	ScheduleSendL();
	iSmsTest.iMsvSession->CleanupEntryPop(); //id
	}

void CSmcmRecvOnly::DoRunSendingL()
/**
 * Called by CSmcmRecvOnly::RunL() when sending has completed.
 * Displays the current state of all sent messages (in iSelection) and determines whether there was and error.
 */
	{
	iSmsTest.SetProgressL(*iOperation);
	delete iOperation;
	iOperation = NULL;

	iSmsTest.Printf(_L("\nDoRunSendingL [iStatus=%d Progress.iError=%d]\n"), iStatus.Int(), iSmsTest.iProgress.iError);

	iSmsTest.DisplaySendingStatesL(*iSelection);

	TInt error = KErrNone;
	const TBool sendingComplete = iSmsTest.SendingCompleteL(*iSelection, error);

	iSmsTest.Printf(_L("[sendingCompete=%d error=%d]\n"), sendingComplete, error);

	if (iStatus.Int() == KErrNone && iSmsTest.iProgress.iError == KErrNone && error == KErrNone)
		iRecvActive->SentL(*iSelection);

	iState = ESendingMenu;
	DisplayAndRead(KMessageText);
	}

void CSmcmRecvOnly::RunL()
	{
	User::LeaveIfError(iStatus.Int());

	const TKeyCode keyCode = iSmsTest.Test().Console()->KeyCode();

	switch (iState)
		{
		case EMainMenu:
			DoRunMainMenuL(keyCode);
			break;
		case ESendingMenu:
			DoRunSendingMenuL(keyCode);
			break;
		case ESending:
			DoRunSendingL();
			break;
		}
	}

TInt CSmcmRecvOnly::RunError(TInt aError)
	{
	Complete(aError);
	return KErrNone;
	}

void CSmcmRecvOnly::Complete(TInt aStatus)
	{
	iSmsTest.Printf(_L("CSmcmRecvOnly Completed with %d\n"), aStatus);
	CActiveScheduler::Stop();
	}

void CSmcmRecvOnly::DoCancel()
	{
	iSmsTest.Test().Console()->ReadCancel();
	if (iOperation != NULL)
		iOperation->Cancel();
	}

//

CSmcmRecvActive* CSmcmRecvActive::NewL(CSmsTestUtils& aSmsTest, TInt aPriority)
/**
 * CSmcmRecvActive factory function
 */
	{
	CSmcmRecvActive* self = new (ELeave) CSmcmRecvActive(aSmsTest, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CSmcmRecvActive::CSmcmRecvActive(CSmsTestUtils& aSmsTest, TInt aPriority)
: CActive(aPriority), iSmsTest(aSmsTest)
/**
 * CSmcmRecvActive constructor
 */
	{
	CActiveScheduler::Add(this);
	}

CSmcmRecvActive::~CSmcmRecvActive()
/**
 * CSmcmRecvActive destructor
 */
	{
	Cancel();
	iTimer.Close();
	iSmsTest.iMsvSession->RemoveObserver(*this);
	delete iSelection;
	delete iReceived;
	delete iSent;
	delete iMatched;
	}

void CSmcmRecvActive::ConstructL()
	{
	User::LeaveIfError(iTimer.CreateLocal());
	iSelection = new (ELeave) CMsvEntrySelection;
	iSent = new (ELeave) CMsvEntrySelection;
	iReceived = new (ELeave) CMsvEntrySelection;
	iMatched = new (ELeave) CMsvEntrySelection;
	iSmsTest.iMsvSession->AddObserverL(*this);
	}

void CSmcmRecvActive::HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* , TAny* )
/**
 * Called by CMsvSession when a messaging event has occurred. It is used here to find out if any new messages have been created.
 */
	{
	CMsvEntrySelection* entries = NULL;

	switch (aEvent)
		{
		case EMsvEntriesCreated:
			entries = STATIC_CAST(CMsvEntrySelection*, aArg1);
			break;
		default:
			break;
		}

	if (entries != NULL)
		{
		TInt count = entries->Count();

		while (count--)
			{
			const TMsvId id = (*entries)[count];
			iSmsTest.SetEntryL(id);

			const TMsvEntry& entry = iSmsTest.Entry();

			if ((entry.iMtm == KUidMsgTypeSMS || entry.iBioType != 0) && entry.iType == KUidMsvMessageEntry)
				iSelection->AppendL(id);
			}

		if (iSelection->Count() != 0)
			{
			Cancel();
			iTimer.After(iStatus, KWaitAfterRecv);
			SetActive();
			}
		}
	}

void CSmcmRecvActive::RunL()
/**
 * Called after the timer completes, which was started when a new message arrived (in CSmcmRecvActive::HandleSessionEventL()).
 * Displays all new messages then determines whether they match sent messages.
 */
	{
	const TInt recvCount = iReceived->Count();
	TInt count = iSelection->Count();

	while (count--)
		{
		const TMsvId id = (*iSelection)[0];
		CSmsHeader* header = iSmsTest.GetHeaderLC(id);

		if (header->Type() == CSmsPDU::ESmsDeliver)
			{
			if (Find(*iReceived, id) == KErrNotFound)
				iReceived->AppendL(id);

			PrintMessageL(id);
			}

		CleanupStack::PopAndDestroy(header);
		iSelection->Delete(0,1);
		}

	if (recvCount != iReceived->Count())
		ProcessReceivedL();
	}

void CSmcmRecvActive::ProcessReceivedL()
/**
 * Determines whether received messages match the sent messages.
 * If two messages in iSent and iReceived match, then it is moved from iReceived to iMatched and deleted from iSent.
 */
	{
	TInt sentCount = iSent->Count();

	while (sentCount--)
		{
		TInt recvCount = iReceived->Count();
		
		const TMsvId sentId = (*iSent)[sentCount];
		CMsvEntry* sentEntry = iSmsTest.iMsvSession->GetEntryL(sentId);
		CleanupStack::PushL(sentEntry);

		while (recvCount--)
			{
			const TMsvId recvId = (*iReceived)[recvCount];
			CMsvEntry* recvEntry = iSmsTest.iMsvSession->GetEntryL(recvId);
			CleanupStack::PushL(recvEntry);
			
			if (IsMatchL(*sentEntry, *recvEntry))
				{
				iMatched->AppendL(recvId);
				iSent->Delete(sentCount, 1);
				iReceived->Delete(recvCount, 1);
				}

			CleanupStack::PopAndDestroy(recvEntry);
			}

		CleanupStack::PopAndDestroy(sentEntry);
		}
	}

TBool CSmcmRecvActive::IsMatchL(CMsvEntry& aSent, CMsvEntry& aReceived)
/**
 * Compares the body text of aSent and aReceived
 *
 * @return Returns ETrue if the body text matches
 */
	{
	HBufC* sentBody = GetBodyTextLC(aSent);
	HBufC* recvBody = GetBodyTextLC(aReceived);
	TBool retVal = (*sentBody == *recvBody);
	CleanupStack::PopAndDestroy(recvBody);
	CleanupStack::PopAndDestroy(sentBody);

	// todo AA 28/2/2002 Is more checking required?

	return retVal;
	}

CSmsHeader* CSmcmRecvActive::GetHeaderLC(CMsvEntry& aMessage) const
/**
 * Returns the CSmsHeader restored from message aMessage.
 */
	{
	CSmsHeader* header = CSmsHeader::NewL(CSmsMessage::NewL(iSmsTest.FileSession(), CSmsPDU::ESmsDeliver, CSmsBuffer::NewL()));
	CleanupStack::PushL(header);

	CMsvStore* store = aMessage.ReadStoreL();
	CleanupStack::PushL(store);
	header->RestoreL(*store);
	CleanupStack::PopAndDestroy(store);

	return header;
	}

HBufC* CSmcmRecvActive::GetBodyTextLC(CMsvEntry& aMessage)
/**
 * @return The body text of message aMessage
 */
	{
	iSmsTest.iRichText->Reset();
	CMsvStore* store = aMessage.ReadStoreL();
	CleanupStack::PushL(store);
	store->RestoreBodyTextL(*iSmsTest.iRichText);
	CleanupStack::PopAndDestroy(store);
	
	const TInt len = iSmsTest.iRichText->DocumentLength();
	HBufC* buf = HBufC::NewLC(len);
	TPtr ptr(buf->Des());
	iSmsTest.iRichText->Extract(ptr);
	return buf;
	}

TInt CSmcmRecvActive::Find(const CMsvEntrySelection& aSelection, TMsvId aId) const
/**
 * Attempts to find a message in a selection (array).
 *
 * @param aSelection The selection of messages to search
 * @param aId The message to find in aSelection
 * @return The index of aId in aSelection or KErrNotFound if aId is not in aSelection
 */
	{
	TInt count = aSelection.Count();

	while (count--)
		{
		const TMsvId id = aSelection[count];
		if (id == aId)
			return count;
		}

	return KErrNotFound;
	}

void CSmcmRecvActive::PrintMessageL(TMsvId aId)
/**
 * Displays the received message aId
 */
	{
	iSmsTest.iMsvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);
	iSmsTest.Printf(_L("\nReceived Message #%d of %d!! Inbox Count=%d\n"), Received(), Sent(), iSmsTest.iMsvEntry->Count());
	iSmsTest.DisplayMessageL(aId);
	}

void CSmcmRecvActive::SentL(const CMsvEntrySelection& aSelection)
/**
 * Copies aSelection into iSent. One entry is added to iSent for every receipient of each message in aSelection.
 */
	{
	TInt count = aSelection.Count();
	while (count--)
		{
		const TMsvId id = aSelection[count];
		CSmsHeader* header = iSmsTest.GetHeaderLC(id);
		TInt recptCount = header->Recipients().Count();

		while (recptCount--)
			iSent->AppendL(id);

		CleanupStack::PopAndDestroy(header);
		}
	}

TInt CSmcmRecvActive::RunError(TInt aError)
	{
	iSmsTest.Printf(_L("WARNING: CSmcmRecvActive::RunL() left with %d\n"), aError);
	iTimer.After(iStatus, KWaitAfterRecv);
	SetActive();
	return KErrNone;
	}

void CSmcmRecvActive::DoCancel()
	{
	iTimer.Cancel();
	}

void CSmcmRecvActive::DisplayUnmatched() const
/**
 * Displays all messages that are yet to be matched
 */
	{
	_LIT(KDisplayFormat, "\nSent %d Received %d Matched %d Unmatched %d\n");

	if (iSent->Count() != 0)
		{
		iSmsTest.Printf(_L("\nUnmatched Messages:\n"));
		TRAPD(err, iSmsTest.DisplaySendingStatesL(*iSent));
		iSmsTest.Printf(_L("\n"));
		if (err != KErrNone)
			iSmsTest.Printf(_L("WARNING: DisplaySendingStatesL left with %d\n"), err);
		}

	iSmsTest.Printf(KDisplayFormat, Sent(), Received(), iMatched->Count(), iSent->Count());
	}
