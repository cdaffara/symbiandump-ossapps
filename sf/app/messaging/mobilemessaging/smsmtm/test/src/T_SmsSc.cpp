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

#include "T_SmsSc.h"
#include <smutsimparam.h>
#include <smsclnt.h>
#include <smsuaddr.h>

#include <testconfigfileparser.h>
#include <e32std.h>

// Autotest library header file
#include "autotest.h"

#include <simtsy.h>

// Global pointer for use by the autogTest.h macro's
CAutoTest* gAutoTest;


RTest gTest(_L("T_SmsSc Testrig"));
CTrapCleanup* theCleanup;


#define LOCAL_CHECKPOINT_CODE(a,b) a.TestCheckPointCodeL((b), (__FILE__), (__LINE__))
#define LOCAL_CHECKPOINT(a,b) a.TestCheckPointL((b), (__FILE__), (__LINE__))
#define LOCAL_CHECKPOINT_COMPARE(a,b,c,d) a.TestCheckPointCompareL((b),(c),(d),(__FILE__),(__LINE__))

_LIT(KTestScriptFile, "SimParameter.script");
_LIT(KTestScriptComponent, "sms");

_LIT8(KListSectionStart, "list");
_LIT8(KSmsParamEntry, "SmsParamEntry");
_LIT8(KSmsParamMaxEntries, "SmsParamMaxEntries");

const TInt KSmsParamEntryDel = ',';

enum TSmsParamEntry
/**	@enum	TSmsParamEntry
 *	Enum defining the SMS parameters
 */
	{
	ESmsParamIndex,
	ESmsParamName,
	ESmsParamPID,
	ESmsParamDCS,
	ESmsParamVP,
	ESmsParamDA,
	ESmsParamSCA
	};

/*
void SetTestNumberL(TInt aTestNumber)
  	{
  	gTest.Printf(_L("Changing the Test Number to %d\n"), aTestNumber);
	RSAVarChangeNotify notify;
	GLOBAL_CHECKPOINT_COMPARE(notify.Connect(), KErrNone, _L("Failed to connect to System Agent Notifier"));
	CleanupClosePushL(notify);
  	TRAPD(ret, notify.NotifySaVarChangeL(KUidSASimTsyTestNumber,aTestNumber));
	GLOBAL_CHECKPOINT_COMPARE(ret, KErrNone, _L("Failed to set test number"));
	CleanupStack::PopAndDestroy(&notify);
  	}
*/
CSmsScTest* NewScTestLCC(TInt aTestNumber)
/**
	Constructs and returns CSmsScTest object. 
	Notifies the system agent that the phone is on.

	@return		CSmsScTest*		A pointer to SmsSc test.  
 */
	{
	CSmsTestUtils* testUtils = CSmsTestUtils::NewL(gTest);
	CleanupStack::PushL(testUtils);

	testUtils->SetSimTsyTestNumberL(aTestNumber);

	TInt nextTest = 0;

	testUtils->NotifySaPhoneOnL();
	testUtils->WaitForInitializeL();
	return CSmsScTest::NewLC(*gAutoTest, *testUtils, KNullDesC, nextTest);
	}

RMobileSmsMessaging::TMobileSmspEntryV1 CreateEntry()
/**
	Creates and returns an entry. 

	@return		TMobileSmspEntryV1		A SIM parameter entry. 
 */
	{
	RMobileSmsMessaging::TMobileSmspEntryV1 entry;
	
	_LIT(KSmsServiceCenterNumber, "+44778501000000");
	entry.iServiceCentre.iTelNumber = KSmsServiceCenterNumber();

//	_LIT(KSmsNumber, "123456");
//	entry.iDestination.iTelNumber = KSmsNumber();

	_LIT(KSmsText, "Vodafone");
	entry.iText = KSmsText();

	entry.iValidParams = RMobileSmsMessaging::KSCAIncluded;

//	TUint8 reservedFiller = 'a';
//	entry.iReservedFiller = reservedFiller;

//	TUint8 validityPeriod = 'b';
//	entry.iValidityPeriod = validityPeriod;

//	TUint8 dcs = 'c';
//	entry.iDcs = dcs;

//	TUint8 protocolID = 'd';
//	entry.iProtocolId = protocolID;

	return entry;
	}

CMobilePhoneSmspList* NewListLC(const RMobileSmsMessaging::TMobileSmspEntryV1& aEntry, TInt aCount)
/**
	Return a new list with aCount entries of aEntry

	@param		aEntry						A SIM parameter entry.
	@param		aCount						An integer.
	@return		CMobilePhoneSmspList*		A list containing the SIM Parameters.
 */
	{
	CMobilePhoneSmspList* list = CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(list);
	RMobileSmsMessaging::TMobileSmspEntryV1 entry(aEntry);

	for (TInt i = 0; i < aCount; ++i)
		{
		entry.iIndex = i;
		list->AddEntryL(entry);
		}

	return list;
	}

void DoTest1L(CSmsScTest& aTest)
/**
	Reads and stores the initial SIM parameters

	@param		aTest		A reference to the SmsSc test
 */
	{
	CMsvOperationWait* waiter = CMsvOperationWait::NewLC();
	
	aTest.ReadInitialParamsL(waiter->iStatus);
	waiter->Start();
	CActiveScheduler::Start();
	GLOBAL_CHECKPOINT_COMPARE(waiter->iStatus.Int(), KErrNone, _L("ReadInitialParamsL completed with error"));

	CleanupStack::PopAndDestroy(waiter);
	}

void Test1L()
/**
	Tests Reading SMS parameters from SIM
 */
	{
	CSmsScTest* scTest = NewScTestLCC(0);

	DoTest1L(*scTest);

	CleanupStack::PopAndDestroy(scTest);
	CleanupStack::PopAndDestroy();
	}

void Test2L()
/**
	Tests Reading error
 */
	{
	// todo Request the tester on screen to remove the phone from serial/IR link

	CSmsScTest* scTest = NewScTestLCC(0);
	CMsvOperationWait* waiter = CMsvOperationWait::NewLC();
	
	scTest->ReadInitialParamsL(waiter->iStatus);
	waiter->Start();
	CActiveScheduler::Start();
	//GLOBAL_CHECKPOINT(waiter->iStatus != KErrNone);
	//GLOBAL_CHECKPOINT_COMPARE(waiter->iStatus != KErrNone, ETrue, _L("ReadInitialParamsL completed with error"));

	CleanupStack::PopAndDestroy(3); //waiter, scTest*2
	}

void DoTest3L(CSmsScTest& aTest, CMsvOperationWait& aWaiter, TInt aMaxNumber)
/**
	Performs round trip tests

	@param		aTest		A reference to SmsSc test
	@param		aWaiter		A reference to MsvOperationWait
 */
	{
	//const TInt maxNumber = aTest.OriginalOperation().SmspList().MaxNumberEntries();
	const RMobileSmsMessaging::TMobileSmspEntryV1 entry(CreateEntry());
	
	for (TInt i = 1; i <= aMaxNumber; ++i)
		{
		CMobilePhoneSmspList* list = NewListLC(entry, i);
		list->SetMaxNumberEntries(aMaxNumber);
		aTest.RoundTripTestL(*list, aWaiter.iStatus);
		aWaiter.Start();
		CActiveScheduler::Start();
		CleanupStack::PopAndDestroy(list);
		User::LeaveIfError(aWaiter.iStatus.Int());
		}
	}

void Test3L()
/**
	Tests Reading and Writing parameters (Round Trip Test)
 */
	{	
	CSmsScTest* scTest = NewScTestLCC(1);
	CMsvOperationWait* waiter = CMsvOperationWait::NewLC();
	scTest->ReadInitialParamsL(waiter->iStatus);
	waiter->Start();
	CActiveScheduler::Start();
	GLOBAL_CHECKPOINT_COMPARE(waiter->iStatus.Int(), KErrNone, _L("ReadInitialParamsL completed with error"));
	
	const TInt maxNumber = scTest->OriginalOperation().SmspList().MaxNumberEntries();
	TRAPD(err, DoTest3L(*scTest, *waiter, maxNumber));

	scTest->WriteOriginalParamsL(waiter->iStatus);
	waiter->Start();
	CActiveScheduler::Start();
	GLOBAL_CHECKPOINT_COMPARE(waiter->iStatus.Int(), KErrNone, _L("WriteOriginalParamsL completed with error"));
	User::LeaveIfError(err);

	CleanupStack::PopAndDestroy(3); //scTest*2, waiter
	}

void DoTest4L(CSmsScTest& aTest, CMsvOperationWait& aWaiter, TInt aMaxNumber)
	{
	const RMobileSmsMessaging::TMobileSmspEntryV1 entry(CreateEntry());

	CMobilePhoneSmspList* list = NewListLC(entry, aMaxNumber);
	list->SetMaxNumberEntries(aMaxNumber);
	aTest.RoundTripTestL(*list, aWaiter.iStatus);
	aWaiter.Start();
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(list);
	GLOBAL_CHECKPOINT(aWaiter.iStatus != KErrNone);
	}

void Test4L()
/**
	Tests Writing error
 */
	{
	CSmsScTest* scTest = NewScTestLCC(0);
	CMsvOperationWait* waiter = CMsvOperationWait::NewLC();
	
	scTest->ReadInitialParamsL(waiter->iStatus);
	waiter->Start();
	CActiveScheduler::Start();
	GLOBAL_CHECKPOINT_COMPARE(waiter->iStatus.Int(), KErrNone, _L("ReadInitialParamsL completed with error"));

	// Write too many parameters
	const TInt maxNumber = scTest->OriginalOperation().SmspList().MaxNumberEntries() + 1;

	TRAPD(err, DoTest4L(*scTest, *waiter, maxNumber));

	scTest->WriteOriginalParamsL(waiter->iStatus);
	waiter->Start();
	CActiveScheduler::Start();
	GLOBAL_CHECKPOINT_COMPARE(waiter->iStatus.Int(), KErrNone, _L("WriteOriginalParamsL completed with error"));
	User::LeaveIfError(err);

	CleanupStack::PopAndDestroy(3); //waiter, scTest*2
	}

TInt CountListsInScriptL(RFs& aFs, const TDesC& aComponent, const TDesC& aScriptFile)
/**
	Returns the number of lists in the script file. 

	@param		aFs							Handle to file server session 
	@param		aComponent					A descriptor
	@param		aScriptFile					Script file
	@return		TInt						Number of lists in the script file
 */
	{
	CTestConfig* file = CTestConfig::NewLC(aFs, aComponent, aScriptFile);

	TInt count = 0;

	FOREVER
		{
		TBuf8<32> sectionName(KListSectionStart);
		sectionName.AppendNum(count+1);
		const CTestConfigSection* section = file->Section(sectionName);

		if (section != NULL)
			count++;
		else
			break;
		}

	CleanupStack::PopAndDestroy(file);
	return count;
	}
	

CMobilePhoneSmspList* ReadFromScriptLC(RFs& aFs, const TDesC& aComponent, const TDesC& aScriptFile, TInt aListNumber, TInt aDefaultMaxNumber)
/**
	Reads and returns a list of parameters from the script file. 

	@param		aFs							Handle to file server session 
	@param		aComponent					A descriptor
	@param		aScriptFile					Script file
	@param		aListNumber					The list number in the script file 
	@param		aDefaultMaxNumber			Maximum number of entries supported by the SIM 
	@return		CMobilePhoneSmspList*		A list containing the parameters
 */
	{
	gTest.Printf(_L("ReadFromScriptLC [aComponent=%S aScript=%S aListNumber=%d"), &aComponent, &aScriptFile, aListNumber);

	CMobilePhoneSmspList* list = CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(list);

	CTestConfig* file = CTestConfig::NewLC(aFs, aComponent, aScriptFile);

	TBuf8<32> sectionName(KListSectionStart);
	sectionName.AppendNum(aListNumber);

	const CTestConfigSection* section = file->Section(sectionName);

	if (section == NULL)
		{
		gTest.Printf(_L("\tSection %d Not Found"), aListNumber);
		User::Leave(KErrNotFound);
		}

	const CTestConfigItem* item = section->Item(KSmsParamMaxEntries);

	TInt maxNumber = aDefaultMaxNumber;

	if (item == NULL)
		{
		gTest.Printf(_L("\tFailed to find KSmsParamMaxEntries in script section. Setting to %d\n"), aDefaultMaxNumber);
		}
	else
		{
		maxNumber = section->ItemValue(KSmsParamMaxEntries, aDefaultMaxNumber);
		}

	if (maxNumber > aDefaultMaxNumber)
		maxNumber = aDefaultMaxNumber;

	list->SetMaxNumberEntries(maxNumber);

	const TInt count = section->ItemCount(KSmsParamEntry);

	if (count == 0)
		{
		gTest.Printf(_L("\tFailed to find KSmsParamEntry in script section\n"));
		User::Leave(KErrNotFound);
		}
	
	for (TInt i = 0; i < count && i < aDefaultMaxNumber; ++i)
		{
		item = section->Item(KSmsParamEntry, i);
		__ASSERT_ALWAYS(item != NULL, User::Invariant());

		const TDesC8& value = item->Value();

		RMobileSmsMessaging::TMobileSmspEntryV1 entry = RMobileSmsMessaging::TMobileSmspEntryV1();
		TPtrC8 ptrOutput(KNullDesC8);
		TInt intOutput;
	
		if (CTestConfig::GetElement(value, KSmsParamEntryDel, ESmsParamIndex, intOutput) == KErrNone)
			entry.iIndex = intOutput;
		else
			entry.iIndex = i;

		if (CTestConfig::GetElement(value, KSmsParamEntryDel, ESmsParamName, ptrOutput) == KErrNone)
			{
			entry.iText.Copy(ptrOutput);
			}

		if (CTestConfig::GetElement(value, KSmsParamEntryDel, ESmsParamPID, intOutput) == KErrNone)
			{
			entry.iProtocolId = (TUint8)intOutput;
			entry.iValidParams |= RMobileSmsMessaging::KProtocolIdIncluded;
			}

		if (CTestConfig::GetElement(value, KSmsParamEntryDel, ESmsParamDCS, intOutput) == KErrNone)
			{
			entry.iDcs = (TUint8)intOutput;
			entry.iValidParams |= RMobileSmsMessaging::KDcsIncluded;
			}

		if (CTestConfig::GetElement(value, KSmsParamEntryDel, ESmsParamVP, intOutput) == KErrNone)
			{
			entry.iValidityPeriod = (TUint8)intOutput;
			entry.iValidParams |= RMobileSmsMessaging::KValidityPeriodIncluded;
			}

		if (CTestConfig::GetElement(value, KSmsParamEntryDel, ESmsParamDA, ptrOutput) == KErrNone)
			{
			entry.iDestination.iTelNumber.Copy(ptrOutput);
			entry.iValidParams |= RMobileSmsMessaging::KDestinationIncluded;
			}

		if (CTestConfig::GetElement(value, KSmsParamEntryDel, ESmsParamSCA, ptrOutput) == KErrNone)
			{
			entry.iServiceCentre.iTelNumber.Copy(ptrOutput);
			entry.iValidParams |= RMobileSmsMessaging::KSCAIncluded;
			}

		list->AddEntryL(entry);
		}
	
	CleanupStack::PopAndDestroy(file);

	return list;
	}

void Test5L()
/**
	Data test (Round Trip Test using script)
 */
	{
	CSmsScTest* scTest = NewScTestLCC(0);
	CMsvOperationWait* waiter = CMsvOperationWait::NewLC();

	scTest->ReadInitialParamsL(waiter->iStatus);
	waiter->Start();
	CActiveScheduler::Start();
	GLOBAL_CHECKPOINT_COMPARE(waiter->iStatus.Int(), KErrNone, _L("ReadInitialParamsL completed with error"));

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	const TInt maxNumber = scTest->OriginalOperation().SmspList().MaxNumberEntries();
	const TInt count = CountListsInScriptL(fs, KTestScriptComponent, KTestScriptFile);

	TBool roundTripTests = ETrue;
	for (TInt i = 1; i <= count; ++i)
		{
		CMobilePhoneSmspList* list = ReadFromScriptLC(fs, KTestScriptComponent, KTestScriptFile, i, maxNumber);

		GLOBAL_CHECKPOINT(list->MaxNumberEntries() <= maxNumber);

		scTest->RoundTripTestL(*list, waiter->iStatus);
		waiter->Start();
		CActiveScheduler::Start();

		if(waiter->iStatus != KErrNone)
			{
			roundTripTests = EFalse;
			break;
			}

		CleanupStack::PopAndDestroy(list);
		}

	scTest->WriteOriginalParamsL(waiter->iStatus);
	waiter->Start();
	CActiveScheduler::Start();
	GLOBAL_CHECKPOINT_COMPARE(waiter->iStatus.Int(), KErrNone, _L("WriteOriginalParamsL completed with error"));

	//GLOBAL_CHECKPOINT(roundTripTests);
	GLOBAL_CHECKPOINT_COMPARE(roundTripTests, ETrue, _L("RoundTripTestL failled"));

	CleanupStack::PopAndDestroy(4); //waiter, scTest*2, fs
	}

void Test6L()
/**
	OOM Test of CSmsClientMtm::ReadSimParamsL
 */
	{
	CSmsScTest* scTest = NewScTestLCC(0);

	TInt failCount = 0;
	TInt error = KErrNone;
	_LIT(KFailCountDesc, "DoTest1L still failing, count = %d\n");

	do
		{
		__UHEAP_FAILNEXT(failCount++);

		TRAP(error, DoTest1L(*scTest));

		__UHEAP_RESET;

		if (error != KErrNone)
			{
			if (error != KErrNoMemory)
				gTest.Printf(KFailCountDesc, failCount);

			GLOBAL_CHECKPOINT_COMPARE(error, KErrNoMemory, _L("Error not KErrNoMemory"));

			if (failCount % 100 == 0)
				gTest.Printf(KFailCountDesc, failCount);
			}
		}
	while (error != KErrNone);
	
	gTest.Printf(KFailCountDesc, failCount);

	CleanupStack::PopAndDestroy(2); //scTest*2
	}

void Test7L()
/**
	OOM Test of CSmsClientMtm::WriteSimParamsL
 */
	{
	CSmsScTest* scTest = NewScTestLCC(1);
	CMsvOperationWait* waiter = CMsvOperationWait::NewLC();
	scTest->ReadInitialParamsL(waiter->iStatus);
	waiter->Start();
	CActiveScheduler::Start();
	GLOBAL_CHECKPOINT_COMPARE(waiter->iStatus.Int(), KErrNone, _L("ReadInitialParamsL completed with error"));

	TInt failCount = 0;
	TInt error = KErrNone;

	do
		{
		__UHEAP_FAILNEXT(failCount++);

		const TInt maxNumber = 1;
		TRAP(error, DoTest3L(*scTest, *waiter, maxNumber));

		__UHEAP_RESET;

		if (error != KErrNone)
			{
			gTest.Printf(_L("DoTest3L still failing, count = %d\n"), failCount);
			GLOBAL_CHECKPOINT_COMPARE(error, KErrNoMemory, _L("Error not KErrNoMemory"));
			}
		}
	while (error != KErrNone);
	
	gTest.Printf(_L("DoTest3L Fail Count %d\n"), failCount);

	scTest->WriteOriginalParamsL(waiter->iStatus);
	waiter->Start();
	CActiveScheduler::Start();
	GLOBAL_CHECKPOINT_COMPARE(waiter->iStatus.Int(), KErrNone, _L("WriteOriginalParamsL completed with error"));

	CleanupStack::PopAndDestroy(3); //scTest*2, waiter
	}

LOCAL_C const TAutoTestCase SmsScTestCases[] =
/**
	Sets up an array of test functions
 */
	{
	{Test1L,_S("GT83-T_MESSS-001 Test Reading")},
	//{Test2L,_S("GT83-T_MESSS-002 Test Reading Error")},
	{Test3L,_S("GT83-T_MESSS-003 Test Reading and Writing (Round Trip Test)")},
	{Test4L,_S("GT83-T_MESSS-004 Test Writing Error")},
	{Test5L,_S("GT83-T_MESSS-005 Data test (Round Trip Test using script)")},
	{Test6L,_S("GT83-T_MESSS-006 OOM test of CSmsClientMtm::ReadSimParamsL")},
	{Test7L,_S("GT83-T_MESSS-007 OOM test of CSmsClientMtm::WriteSimParamsL")}
	};

LOCAL_C void doMainL()
/**
	Constructs sctive scheduler and starts off the tests
 */
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install( scheduler );

	// Copying the config file to the root directory

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	_LIT(KFileName2,"\\sms\\smssc_config.txt");
	
	CSmsTestUtils::CopyToSimTsyConfigFileL(fs, KFileName2);

	CleanupStack::PopAndDestroy(&fs);

	// Open the socket server

	RSocketServ serv;
	RSocket socket;

	User::LeaveIfError(serv.Connect());
	CleanupClosePushL(serv);
	
	TProtocolDesc protoinfo;
	TProtocolName protocolname(KSmsDatagram);
	User::LeaveIfError(serv.FindProtocol(protocolname,protoinfo));
	User::LeaveIfError(socket.Open(serv,protoinfo.iAddrFamily,protoinfo.iSockType,protoinfo.iProtocol));

	TSmsAddr smsaddr;
	smsaddr.SetSmsAddrFamily(ESmsAddrSendOnly);
	User::LeaveIfError(socket.Bind(smsaddr));

	CleanupClosePushL(socket);

	_LIT(KTestOutput,"smssc_testreport.txt");
	_LIT8(KComponentInfo,"smcm and smss");
	// Library defined macro
	// Uses a global pointer instance of CAutoTest
	GLOBAL_AUTOTEST_EXECUTE(SmsScTestCases, KTestOutput, KComponentInfo, gTest);

	CleanupStack::PopAndDestroy(&socket);
	CleanupStack::PopAndDestroy(&serv);
	CleanupStack::PopAndDestroy(scheduler);
	}

GLDEF_C TInt E32Main()
/**
	Calls doMainL and checks for error
 */
	{
	__UHEAP_MARK;

	gTest.Start(_L("Setup"));
	theCleanup = CTrapCleanup::New();


	TRAPD(ret,doMainL());

	if (ret != KErrNone)
		gTest.Printf(_L("Test completed with %d"), ret);

	delete theCleanup;	
	gTest.Console()->SetPos(0, 13);
	gTest.End();
	gTest.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}

CSmsScTest::~CSmsScTest()
/**
	destructor
 */
	{
	Cancel();
	delete iOriginalParams;
	}

void CSmsScTest::ReadInitialParamsL(TRequestStatus& aStatus)
/**
	Starts off the read async operation.

	@param		aStatus		A request status.
 */
	{
	iState = EStateReadInitialParams;

	delete iOperation;
	iOperation = NULL;

	iOperation = Client().ReadSimParamsL(iStatus);
	SetActive();
	Queue(aStatus);
	}

void CSmsScTest::RoundTripTestL(const CMobilePhoneSmspList& aListToWrite, TRequestStatus& aStatus)
/**
	Starts off the round trip test.

	@param		aListToWrite	A list to write to the SIM.
	@param		aStatus			Indicates the status of the operations.
 */
	{
	iListToWrite = &aListToWrite;

//	iSmsTest.Test().Printf(_L("Starting Round Trip Test\n"));

	WriteSimParamsL();
	Queue(aStatus);
	}

void CSmsScTest::WriteSimParamsL()
/**
	Starts off the write async operation.

	@param		aList		A list to write to the SIM.
 */
	{
	iState = EStateWriteSimParams;

//	PrintList(iSmsTest.Test(), *iListToWrite);

	delete iOperation;
	iOperation = NULL;

	iOperation = Client().WriteSimParamsL(*iListToWrite, iStatus);
	SetActive();
	}

void CSmsScTest::ReadAfterWriteL()
/**
	Reading after writing to the SIM
 */
	{
	iSmsTest.Test().Printf(_L("Reading after writing\n"));
	iState = EStateReadAfterWrite;

	delete iOperation;
	iOperation = NULL;

	iOperation = Client().ReadSimParamsL(iStatus);
	SetActive();
	}

void CSmsScTest::WriteOriginalParamsL(TRequestStatus& aStatus)
/**
	Writes the default parameters back to the SIM

	@param		aStatus		Indicates the status of the write operation.
 */
	{
	Queue(aStatus);

	iSmsTest.Test().Printf(_L("Writing original parameters back to Sim"));
	iState = EStateWriteOriginalParams;

	delete iOperation;
	iOperation = NULL;

	iOperation = Client().WriteSimParamsL(iOriginalParams->SmspList(), iStatus);
	SetActive();
	}

CSmsScTest* CSmsScTest::NewLC(CAutoTest& aAutoTest, CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest)
	{
	CSmsScTest* self = new (ELeave) CSmsScTest(aAutoTest, aSmsTest, aScriptFile, aNextTest);
	CleanupStack::PushL(self);

	self->ConstructL();
	return self;
	}

CSmsScTest::CSmsScTest(CAutoTest& aAutoTest, CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest)
: CSmsTestBase(aSmsTest, aScriptFile, aNextTest), iState(EStateWaiting), iAutoTest(aAutoTest)
	{
	CActiveScheduler::Add(this);
	}

void CSmsScTest::RunAutoL()
/**
	Required by CSmsTestBase.
 */
	{

	}

void CSmsScTest::ConstructL()
	{
	SetTestNameL(KSmsScTestName);
	iSmsTest.InstantiateClientMtmsL();

	iSmsTest.SetLogToFile();
	}

void CSmsScTest::ShowMenuL()
/**
	Required by CSmsTestBase.
 */
	{
	}

void CSmsScTest::RunL()
/**
	Handles completed async operations
 */
	{
	iSmsTest.Test().Printf(_L("CSmsScTest::RunL [State=%d Status=%d]\n"), iState, iStatus.Int());

	switch (iState)
		{
		case EStateReadInitialParams:
			{
			DoRunReadInitialParamsL();
			break;
			}
		case EStateWriteSimParams:
			{
			DoRunWriteSimParamsL();
			break;
			}
		case EStateReadAfterWrite:
			{
			DoRunReadAfterWriteL();
			break;
			}
		case EStateWriteOriginalParams:
			{
			DoRunWriteOriginalParamsL();
			break;
			}
		default:
			{
			break;
			}
		}

	if (!IsActive())
		Complete(iStatus.Int());
	}

void CSmsScTest::Complete(TInt aError)
/**
	Indicates that the operation is complete with aError.

	@param		aError		error returned by the operation
 */
	{
	iSmsTest.Test().Printf(_L("CSmsScTest::Complete [aError=%d]\n"), aError);

	if (iReport != NULL)
		User::RequestComplete(iReport, aError);
	}

TInt CSmsScTest::RunError(TInt aError)
/**

	@param		aError		error returned by the operation
	@return		TInt		Returns the KErrNone. 
 */
	{
	Complete(aError);
	return KErrNone;
	}

void CSmsScTest::Queue(TRequestStatus& aStatus)
/**

	@param		aStatus		Indicates the status of the operation.
 */
	{
	__ASSERT_DEBUG(iReport==NULL, User::Invariant());

	aStatus=KRequestPending;
	iReport=&aStatus;
	}

void CSmsScTest::PrintListL(RTest& aTest, const CMobilePhoneSmspList& aList)
/**
	Prints aList 

	@param		aTest		
	@param		aList		A list of SIM parameters	
 */
	{
	//Print MaxNumber
	aTest.Printf(_L("========= CMobilePhoneSmspList ========= \n"));

	aTest.Printf(_L("MaxNumber=%d\n"), aList.MaxNumberEntries());

	//Pirnt Num Entries
	TInt count = aList.Enumerate();
	aTest.Printf(_L("Number of entries: %d\n"), count);

	//Print each entry in aList
	for(TInt i = 0; i < count; ++i)
		{
		const RMobileSmsMessaging::TMobileSmspEntryV1& entry = aList.GetEntryL(i);
		PrintEntry(aTest, entry);
		}

	aTest.Printf(_L("======= End CMobilePhoneSmspList ======= \n"));
	}

void CSmsScTest::PrintEntry(RTest& aTest, const RMobileSmsMessaging::TMobileSmspEntryV1& aEntry)
/**
	Prints aEntry 

	@param		aTest		
	@param		aEntry		A parameter entry.	
 */
	{
	aTest.Printf(_L("\t========= TMobileSmspEntryV1 =========\n"));

	aTest.Printf(_L("\tIndex= %d\n\tValid= 0x%X\n"), aEntry.iIndex, aEntry.iValidParams);

	if (aEntry.iValidParams & RMobileSmsMessaging::KProtocolIdIncluded)
		aTest.Printf(_L("\tProtocolId = %d\n"), aEntry.iProtocolId);

	if (aEntry.iValidParams & RMobileSmsMessaging::KDcsIncluded)
		aTest.Printf(_L("\tDcs = %d\n"), aEntry.iDcs);
	
	if (aEntry.iValidParams & RMobileSmsMessaging::KValidityPeriodIncluded)
		aTest.Printf(_L("\tValidityPeriod= %d\n"), aEntry.iValidityPeriod);
	
	if (aEntry.iValidParams & RMobileSmsMessaging::KDestinationIncluded)
		aTest.Printf(_L("\tDestination = %S\n"), &aEntry.iDestination.iTelNumber);
	
	if (aEntry.iValidParams & RMobileSmsMessaging::KSCAIncluded)
		aTest.Printf(_L("\tSC = %S\n"), &aEntry.iServiceCentre.iTelNumber);
	
	if (aEntry.iValidParams & RMobileSmsMessaging::KSCAIncluded)
		aTest.Printf(_L("\tText = %S\n"), &aEntry.iText);
		
	aTest.Printf(_L("\t======= End TMobileSmspEntryV1 =======\n"));
	}

void CSmsScTest::DoRunReadInitialParamsL()
/**
	Checks the status, saves the operation and prints the list read.
 */
	{
	if (iStatus == KErrNone)
		{
		TIntBuf progressBuf;
		progressBuf.Copy(iOperation->ProgressL());
		iStatus = progressBuf();
		}

	if (iStatus == KErrNone)
		{
		delete iOriginalParams;
		iOriginalParams = (CSmsSimParamOperation*) iOperation;
		iOperation = NULL;

	    PrintListL(iSmsTest.Test(), iOriginalParams->SmspList());
		}

	delete iOperation;
	iOperation = NULL;
	}

void CSmsScTest::DoRunWriteSimParamsL()
/**
	Checks the status, saves the operation and calls ReadAfterWriteL.
 */
	{
	if (iStatus == KErrNone)
		{
		TIntBuf progressBuf;
		progressBuf.Copy(iOperation->ProgressL());
		iStatus = progressBuf();
		}

	iSmsTest.Test().Printf(_L("WriteSimParams completed with error %d\n"), iStatus);

	delete iOperation;
	iOperation = NULL;

	if (iStatus == KErrNone)
		{
		// Reading the new parameters from Sim
		ReadAfterWriteL();
		}
	}

void CSmsScTest::DoRunReadAfterWriteL()
/**
	Checks the status, parameter list and service center list.
 */
	{
	if (iStatus == KErrNone)
		{
		TIntBuf progressBuf;
		progressBuf.Copy(iOperation->ProgressL());
		iStatus = progressBuf();
		}

	iSmsTest.Test().Printf(_L("ReadSimParams completed with error %d\n"), iStatus);
	
	if (iStatus == KErrNone)
		{
		CSmsSimParamOperation* op = (CSmsSimParamOperation*) iOperation;
		
		const CMobilePhoneSmspList& list = op->SmspList();
		TInt count = list.Enumerate();

		iSmsTest.Test().Printf(_L("ReadSimParams found:\n\t%d SMS params on SIM\n"), count);

//		PrintList(iSmsTest.Test(), list);
		LOCAL_CHECKPOINT(iAutoTest, CompareListsL(*iListToWrite, list));
		}

	delete iOperation;
	iOperation = NULL;
	}

TBool CSmsScTest::CompareListsL(const CMobilePhoneSmspList& aWritten, const CMobilePhoneSmspList& aRead)
/**
	Compares two parameter lists 

	@param		aWritten	A list of SIM parameters
	@param		aRead		A list of SIM parameters	
	@return		TBool		A boolean. 
 */
	{
	const TInt count = aWritten.Enumerate();

	TInt ret = ETrue;

	if (count < aRead.Enumerate())
		ret = EFalse;

	for (TInt i=0; i<count && ret; ++i) 
		{   
		const RMobileSmsMessaging::TMobileSmspEntryV1& entry1 = aWritten.GetEntryL(i);
		const RMobileSmsMessaging::TMobileSmspEntryV1& entry2 = aRead.GetEntryL(i);

		ret = CompareEntries(entry1, entry2);
		}
	
	return ret;
	}

TBool CSmsScTest::CompareEntries(const RMobileSmsMessaging::TMobileSmspEntryV1& aWritten, const RMobileSmsMessaging::TMobileSmspEntryV1& aRead)
/**
	Compares two parameter entries 

	@param		aWritten	A parameter entry
	@param		aRead		A parameter entry	
	@return		TBool		A boolean. 
 */
	{
	TBool ret = ETrue;

	if ((aWritten.iValidParams & RMobileSmsMessaging::KProtocolIdIncluded) && (aRead.iValidParams & RMobileSmsMessaging::KProtocolIdIncluded) && aRead.iProtocolId != aWritten.iProtocolId)
		ret = EFalse;
	else if ((aWritten.iValidParams & RMobileSmsMessaging::KDcsIncluded) && (aRead.iValidParams & RMobileSmsMessaging::KDcsIncluded) && aWritten.iDcs != aRead.iDcs)
		ret = EFalse;
	else if ((aWritten.iValidParams & RMobileSmsMessaging::KValidityPeriodIncluded) && (aRead.iValidParams & RMobileSmsMessaging::KValidityPeriodIncluded) && aWritten.iValidityPeriod != aRead.iValidityPeriod)
		ret = EFalse;
	else if ((aWritten.iValidParams & RMobileSmsMessaging::KDestinationIncluded) && (aRead.iValidParams & RMobileSmsMessaging::KDestinationIncluded) && aWritten.iDestination.iTelNumber.CompareF(aRead.iDestination.iTelNumber) != 0)
		ret = EFalse;
	else if ((aWritten.iValidParams & RMobileSmsMessaging::KSCAIncluded) && (aRead.iValidParams & RMobileSmsMessaging::KSCAIncluded) && aWritten.iServiceCentre.iTelNumber.CompareF(aRead.iServiceCentre.iTelNumber) != 0)
		ret = EFalse;
	else if ((aWritten.iValidParams & RMobileSmsMessaging::KSCAIncluded) && (aRead.iValidParams & RMobileSmsMessaging::KSCAIncluded) && aWritten.iText.CompareF(aRead.iText) != 0)
		ret = EFalse;

	return ret;
	}

void CSmsScTest::DoRunWriteOriginalParamsL()
/**
	Checks the status
 */	
	{
	if (iStatus == KErrNone)
		{
		TIntBuf progressBuf;
		progressBuf.Copy(iOperation->ProgressL());
		iStatus = progressBuf();
		}

	iSmsTest.Test().Printf(_L("WriteSimParams completed with error %d\n"), iStatus);

	delete iOperation;
	iOperation = NULL;
	}

void CSmsScTest::DoCancel()
/**
	Cancels the async operation
 */
	{
	if (iOperation)
		{
		iOperation->Cancel();
		iSmsTest.Test().Printf(_L("Operation Cancelled!\n"));
		}

	delete iOperation;
	iOperation = NULL;
	}
