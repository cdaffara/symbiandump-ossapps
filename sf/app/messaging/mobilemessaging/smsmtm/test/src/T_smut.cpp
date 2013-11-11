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
//

#include "T_smut.h"
#include <smss.rsg>
#include <etelmm.h>
#include <smutset.h>
#include <csmsaccount.h>

#include <csmsemailfields.h>

#include <csmsgetdetdescinterface.h>


const TUid KUidMsvSmsEmailFieldsStream	= {0x10204C9D};

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

LOCAL_C void doMainL()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	CSmsTestUtils* smsTest = CSmsTestUtils::NewL(test);
	CleanupStack::PushL(smsTest);
	TInt nextTest = 0;
	
	CSmutTest* SmcmTest = CSmutTest::NewLC(*smsTest, KNullDesC, nextTest);

	SmcmTest->StartL();

	CleanupStack::PopAndDestroy(3, scheduler);
	}

CSmutTest::CSmutTest(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest)
: CSmsTestBase(aSmsTest, aScriptFile, aNextTest)
	{
	}

void CSmutTest::RunAutoL()
	{
	iNextTest = 0;
	
	// Testing TMsvSmsEntry
	iSmsTest.TestStart(++iNextTest, _L("TMsvSmsEntry"));
	TestSmsEntryL();
	iSmsTest.TestFinish(iNextTest);


	// Testing settings (CSmsSettings)
	iSmsTest.TestStart(++iNextTest, _L("CSmsSettings"));  
	TestSettingsL();
	iSmsTest.TestFinish(iNextTest);

	// Testing number class (CSmsNumber)
	iSmsTest.TestStart(++iNextTest, _L("CSmsNumber"));  
	TestNumberL();
	iSmsTest.TestFinish(iNextTest);
	
	// Testing email fields class (CSmsEmailFields)
	iSmsTest.TestStart(++iNextTest, _L("CSmsEmailFields"));  
	TestEmailFieldsL();
	iSmsTest.TestFinish(iNextTest);

	// Testing with header (CSmsHeader)
	iSmsTest.TestStart(++iNextTest, _L("CSmsHeader"));
	TestHeaderL();
	iSmsTest.TestFinish(iNextTest);

	// Testing with utility class (TSmsUtilities)
	iSmsTest.TestStart(++iNextTest, _L("TSmsUtilities"));
	TestSmsUtilitiesL();

	// Testing with ECOM Plugin class (CSmsGetDetDescInterface)
	iSmsTest.TestStart(++iNextTest, _L("CSmsGetDetDescInterface"));
	TestSmsUtilitiesBL();

	iSmsTest.TestFinish(iNextTest);

	iSmsTest.TestStart(++iNextTest, _L("EMS Extensions"));
	TestSmsEmsExtensionsL();

	iSmsTest.TestFinish(iNextTest);

	}

CSmutTest::~CSmutTest()
	{
	delete iSmsHeader;
	delete iRichText;
	delete iParaLayer;
	delete iCharLayer;
	delete iDBSession;
	
	}

void CSmutTest::ConstructL()
	{
	iSmsTest.SetLogToFile();

	iSelection = new (ELeave) CMsvEntrySelection();
	iTimer = CTestTimer::NewL();

	iParaLayer =CParaFormatLayer::NewL();
	iCharLayer = CCharFormatLayer::NewL();
	iRichText = CRichText::NewL(iParaLayer, iCharLayer, CEditableText::EFlatStorage,256);
	iDBSession = CMDBSession::NewL(CMDBSession::LatestVersion());	

	CActiveScheduler::Add(this);
	}

CSmutTest* CSmutTest::NewLC(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest)
	{
	CSmutTest* self = new (ELeave) CSmutTest(aSmsTest, aScriptFile, aNextTest);
	CleanupStack::PushL(self);

	self->ConstructL();

	return self;
	}

void CSmutTest::ShowMenuL()
	{
	iSmsTest.ResetMenu();

	iSmsTest.AppendToMenuL(_L("Start"));
	iSmsTest.AppendToMenuL(_L("Sms Utilities"));
	iSmsTest.AppendToMenuL(_L("Sms Settings"));

	TInt result = iSmsTest.DisplayMenu(_L("SMS Client MTM Test"));

	if (result <= 0)
		return;

	switch (result)
		{
		case 1:
			RunAutoL();
			break;
		case 2:
			TestSmsUtilitiesL();
			break;
		case 3:
			TestSettingsL();
			break;
		default:
			User::Leave(KErrArgument);
			break;
		}

	if (!iSmsTest.RunAuto())
		{
		iSmsTest.Printf(_L("Press any key to continue...\n"));
		iSmsTest.Test().Getch();
		}

	ShowMenuL();
	}

void CSmutTest::TestSettingsL()
	{
	iSmsTest.Printf(_L("Testing Create Settings...\n"));

	iSmsTest.SetEntryL(iSmsTest.iSmsServiceId);

// First we test all the features of the class CSmsSettings
// We give them different values and compare if it works

	CSmsSettings* settings = CSmsSettings::NewL();
	CleanupStack::PushL(settings);
	iSmsTest(CompareSettings(*settings, *settings));
	TestSettingsMembersL(*settings);
	TestMessageSettingsMembersL(*settings);

// Lets copy the values to a different instance
	CSmsSettings* smsSettings=CSmsSettings::NewL();
	CleanupStack::PushL(smsSettings);
	smsSettings->CopyL(*settings);
	iSmsTest(CompareSettings(*smsSettings, *settings));
	CleanupStack::PopAndDestroy(smsSettings);
	smsSettings = NULL;
	iSmsTest.Printf(_L("Test CSmsSettings::CopyL() passed\n"));


//	Lets create a sms service in the file system with these values
	TestStoreRestoreSettingsL(*settings);

// Lets make the previous test again; Lets give a settings instance some
// values, store it, restore it and see that the values are still the same
	CSmsSettings* set2=CSmsSettings::NewL();
	CleanupStack::PushL(set2);
	set2->AddServiceCenterL(_L("Radiolinja0"),_L("+358508771010"));
	set2->AddServiceCenterL(_L("Radiolinja1"),_L("+358508771010"));
	set2->AddServiceCenterL(_L("Radiolinja2"),_L("+358508771010"));
	set2->AddServiceCenterL(_L("Radiolinja3"),_L("+358508771010"));
	set2->AddServiceCenterL(_L("Radiolinja4"),_L("+358508771010"));
	set2->SetDefaultServiceCenter(1);
	iSmsTest(set2->DefaultServiceCenter()==1);
// Validity Period	
	set2->SetValidityPeriod(ESmsVPSixHours);
	iSmsTest(set2->ValidityPeriod().Int()==ESmsVPSixHours);
// Reply Quoted
	set2->SetReplyQuoted(EFalse);
	iSmsTest(!set2->ReplyQuoted());
// Reject Duplicate 
	set2->SetRejectDuplicate(ETrue);
	iSmsTest(set2->RejectDuplicate());
// Delivery Method
	set2->SetDelivery(ESmsDeliveryUponRequest);
	iSmsTest(set2->Delivery()==ESmsDeliveryUponRequest);
// Delivery Report
	set2->SetDeliveryReport(ETrue);
	iSmsTest(set2->DeliveryReport());
// Reply Path
	set2->SetReplyPath(EFalse);
	iSmsTest(!set2->ReplyPath());
// Message Conversion
	set2->SetMessageConversion(ESmsConvPIDNone);
	iSmsTest(set2->MessageConversion()==ESmsConvPIDNone);
// Can Concatenate
	set2->SetCanConcatenate(ETrue);
	iSmsTest(set2->CanConcatenate());
// Character Set
	set2->SetCharacterSet(TSmsDataCodingScheme::ESmsAlphabet8Bit);
	iSmsTest(set2->CharacterSet()==TSmsDataCodingScheme::ESmsAlphabet8Bit);
// Validity period format
	set2->SetValidityPeriodFormat(TSmsFirstOctet::ESmsVPFSemiOctet);
	iSmsTest(set2->ValidityPeriodFormat()==TSmsFirstOctet::ESmsVPFSemiOctet);
// Status Report Handling
	set2->SetStatusReportHandling(CSmsSettings::EMoveReportToInboxVisible);
	iSmsTest(set2->StatusReportHandling()==CSmsSettings::EMoveReportToInboxVisible);
// Special Message Handling
	set2->SetSpecialMessageHandling(CSmsSettings::EMoveReportToInboxVisible);
	iSmsTest(set2->SpecialMessageHandling()==CSmsSettings::EMoveReportToInboxVisible);

	TestStoreRestoreSettingsL(*set2);

	set2->SetClass2Folder(123);
	TestStoreRestoreSettingsL(*set2);

	CleanupStack::PopAndDestroy(set2);

	iSmsTest.iServiceSettings->CopyL(*settings);  // Lets copy the current setting to the service settings

	iSmsTest(CompareSettings(*iSmsTest.iServiceSettings, *settings));

	iSmsTest.Printf(_L("CSmsSettings::CopyL() 2\n"));

	CleanupStack::PopAndDestroy(settings);

	iSmsTest.Printf(_L("Test Create Settings Passed\n"));

	TestSettingsStoreToCommDbL();

	iSmsTest.Printf(_L("TestSettingsStoreToCommDbL Passed\n"));

	}


TBool CSmutTest::CompareSettingsFiles(
	RFs& aFs, const TDesC& aCompare1, const TDesC& aCompare2
	)
	{
	TBool identical = ETrue;

	RFileReadStream stream1;
	RFileReadStream stream2;

	// Open each stream for binary read.
	stream1.Open(aFs, aCompare1, EFileStream | EFileRead);
	stream2.Open(aFs, aCompare2, EFileStream | EFileRead);

	TInt ret1 = KErrNone;
	TInt ret2 = KErrNone;
	TBool finished = EFalse;
	TUint8 byte1 = 0;
	TUint8 byte2 = 0;

	while (!finished)
		{
		TRAP(ret1, byte1 = stream1.ReadUint8L());
		TRAP(ret2, byte2 = stream2.ReadUint8L());

		if (byte1 != byte2 || ret1 == KErrEof || ret2 == KErrEof)
			{
			// Either file data or lengths differ.
			if ((byte1 != byte2) || (ret1 != ret2))
				identical = EFalse;
			finished = ETrue;
			}
		}

	stream1.Close();
	stream2.Close();

	return identical;
	}

void CSmutTest::TestStoreRestoreSettingsL(const CSmsSettings& aSettings)
	{
	StoreSettingsL(aSettings);

// Lets read the service and see it the values are still ok
	CSmsSettings* set1=CSmsSettings::NewL();
	CleanupStack::PushL(set1);

	RestoreSettingsL(*set1);

	iSmsTest(CompareSettings(*set1, aSettings));

	CleanupStack::PopAndDestroy(set1);
	}

void CSmutTest::TestSettingsMembersL(CSmsSettings& aSettings)
	{
// Status Report Handling (what should be done to report messages)
	aSettings.SetStatusReportHandling(CSmsSettings::EMoveReportToInboxInvisible);
	iSmsTest(aSettings.StatusReportHandling()==CSmsSettings::EMoveReportToInboxInvisible);
	aSettings.SetStatusReportHandling(CSmsSettings::EMoveReportToInboxVisible);
	iSmsTest(aSettings.StatusReportHandling()==CSmsSettings::EMoveReportToInboxVisible);
	aSettings.SetStatusReportHandling(CSmsSettings::EDiscardReport);
	iSmsTest(aSettings.StatusReportHandling()==CSmsSettings::EDiscardReport);
	aSettings.SetStatusReportHandling(CSmsSettings::EMoveReportToInboxInvisibleAndMatch);
	iSmsTest(aSettings.StatusReportHandling()==CSmsSettings::EMoveReportToInboxInvisibleAndMatch);
	aSettings.SetStatusReportHandling(CSmsSettings::EMoveReportToInboxVisibleAndMatch);
	iSmsTest(aSettings.StatusReportHandling()==CSmsSettings::EMoveReportToInboxVisibleAndMatch);
	aSettings.SetStatusReportHandling(CSmsSettings::EDiscardReportAndMatch);
	iSmsTest(aSettings.StatusReportHandling()==CSmsSettings::EDiscardReportAndMatch);
	iSmsTest.Printf(_L("Test Status Report Handling Passed\n"));

// SC handling
	aSettings.AddServiceCenterL(_L("Radiolinja0"),_L("+358508771010"));
	aSettings.AddServiceCenterL(_L("Radiolinja1"),_L("+358508771010"));
	aSettings.AddServiceCenterL(_L("Radiolinja2"),_L("+358508771010"));
	aSettings.AddServiceCenterL(_L("Radiolinja3"),_L("+358508771010"));
	aSettings.AddServiceCenterL(_L("Radiolinja4"),_L("+358508771010"));
	iSmsTest(aSettings.GetServiceCenter(0).Name()==_L("Radiolinja0"));
	aSettings.RemoveServiceCenter(aSettings.ServiceCenterCount()-3);
	aSettings.RemoveServiceCenter(aSettings.ServiceCenterCount()-2);
	iSmsTest(aSettings.GetServiceCenter(1).Name()==_L("Radiolinja1"));
	iSmsTest(aSettings.GetServiceCenter(2).Name()==_L("Radiolinja4"));
	iSmsTest(aSettings.GetServiceCenter(2).Address()==_L("+358508771010"));
	aSettings.RemoveServiceCenter(aSettings.ServiceCenterCount()-1);
	aSettings.RemoveServiceCenter(aSettings.ServiceCenterCount()-1);
	aSettings.RemoveServiceCenter(aSettings.ServiceCenterCount()-1);
//	aSettings.RemoveServiceCenter(aSettings.ServiceCenterCount()-1); // this would be -1
	aSettings.AddServiceCenterL(_L("Radiolinja0"),_L("+358508771010"));
	aSettings.AddServiceCenterL(_L("Radiolinja1"),_L("+358508771010"));
	aSettings.AddServiceCenterL(_L("Radiolinja2"),_L("+358508771010"));
	aSettings.SetDefaultServiceCenter(1);
	iSmsTest(aSettings.DefaultServiceCenter()==1);
	aSettings.SetDefaultServiceCenter(2);
	iSmsTest(aSettings.DefaultServiceCenter()==2);
	iSmsTest.Printf(_L("Test Default SC Passed\n"));

// Special Message Handling (what should be done to special messages)
	aSettings.SetSpecialMessageHandling(CSmsSettings::EMoveReportToInboxInvisible);
	iSmsTest(aSettings.SpecialMessageHandling()==CSmsSettings::EMoveReportToInboxInvisible);
	aSettings.SetSpecialMessageHandling(CSmsSettings::EMoveReportToInboxVisible);
	iSmsTest(aSettings.SpecialMessageHandling()==CSmsSettings::EMoveReportToInboxVisible);
	aSettings.SetSpecialMessageHandling(CSmsSettings::EDiscardReport);
	iSmsTest(aSettings.SpecialMessageHandling()==CSmsSettings::EDiscardReport);
	aSettings.SetSpecialMessageHandling(CSmsSettings::EMoveReportToInboxInvisibleAndMatch);
	iSmsTest(aSettings.SpecialMessageHandling()==CSmsSettings::EMoveReportToInboxInvisibleAndMatch);
	aSettings.SetSpecialMessageHandling(CSmsSettings::EMoveReportToInboxVisibleAndMatch);
	iSmsTest(aSettings.SpecialMessageHandling()==CSmsSettings::EMoveReportToInboxVisibleAndMatch);
	aSettings.SetSpecialMessageHandling(CSmsSettings::EDiscardReportAndMatch);
	iSmsTest(aSettings.SpecialMessageHandling()==CSmsSettings::EDiscardReportAndMatch);
	iSmsTest.Printf(_L("Test Special Message Handling Passed\n"));

//CommDbAction
	aSettings.SetCommDbAction(CSmsSettings::ENone);
	iSmsTest(aSettings.CommDbAction() == CSmsSettings::ENone);
	aSettings.SetCommDbAction(CSmsSettings::EStoreToCommDb);
	iSmsTest(aSettings.CommDbAction() == CSmsSettings::EStoreToCommDb);
	iSmsTest.Printf(_L("Test CommDb Action Passed\n"));

//SmsBearerAction
	aSettings.SetSmsBearerAction(CSmsSettings::ENone);
	iSmsTest(aSettings.SmsBearerAction() == CSmsSettings::ENone);
	aSettings.SetSmsBearerAction(CSmsSettings::EStoreToCommDb);
	iSmsTest(aSettings.SmsBearerAction() == CSmsSettings::EStoreToCommDb);
	iSmsTest.Printf(_L("Test SMS Bearer Action Passed\n"));

//SmsBearer
	aSettings.SetSmsBearer(CSmsSettings::ESmsBearerPacketOnly);
	iSmsTest(aSettings.SmsBearer() == CSmsSettings::ESmsBearerPacketOnly);
	aSettings.SetSmsBearer(CSmsSettings::ESmsBearerPacketOnly);
	iSmsTest(aSettings.SmsBearer() == CSmsSettings::ESmsBearerPacketOnly);
	aSettings.SetSmsBearer(CSmsSettings::ESmsBearerCircuitOnly);
	iSmsTest(aSettings.SmsBearer() == CSmsSettings::ESmsBearerCircuitOnly);
	aSettings.SetSmsBearer(CSmsSettings::ESmsBearerPacketPreferred);
	iSmsTest(aSettings.SmsBearer() == CSmsSettings::ESmsBearerPacketPreferred);
	aSettings.SetSmsBearer(CSmsSettings::ESmsBearerCircuitPreferred);
	iSmsTest(aSettings.SmsBearer() == CSmsSettings::ESmsBearerCircuitPreferred);
	iSmsTest.Printf(_L("Test SMS Bearer Passed\n"));

// Reply Quoted
	aSettings.SetReplyQuoted(EFalse);
	iSmsTest(!aSettings.ReplyQuoted());
	aSettings.SetReplyQuoted(ETrue);
	iSmsTest(aSettings.ReplyQuoted());
	iSmsTest.Printf(_L("Test Reply Quoted Passed\n"));

// Delivery Method
	aSettings.SetDelivery(ESmsDeliveryUponRequest);
	iSmsTest(aSettings.Delivery()==ESmsDeliveryUponRequest);
	aSettings.SetDelivery(ESmsDeliveryScheduled);
	iSmsTest(aSettings.Delivery()==ESmsDeliveryScheduled);
	aSettings.SetDelivery(ESmsDeliveryImmediately);
	iSmsTest(aSettings.Delivery()==ESmsDeliveryImmediately);
	iSmsTest.Printf(_L("Test Delivery Passed\n"));

// Class2Folder
	iSmsTest(aSettings.Class2Folder() == KMsvGlobalInBoxIndexEntryId);
	aSettings.SetClass2Folder(1);
	iSmsTest(aSettings.Class2Folder() == 1);
	iSmsTest.Printf(_L("Test Class2Folder Passed\n"));	

// Description length
	iSmsTest(aSettings.DescriptionLength() == KSmsDescriptionLength);
	aSettings.SetDescriptionLength(100);
	iSmsTest(aSettings.DescriptionLength() == 100);
	iSmsTest.Printf(_L("Test DescriptionLength Passed\n"));	
	}

void CSmutTest::TestMessageSettingsMembersL(CSmsMessageSettings& aSettings)
	{

// Reject Duplicate 
	aSettings.SetRejectDuplicate(ETrue);
	iSmsTest(aSettings.RejectDuplicate());
	aSettings.SetRejectDuplicate(EFalse);
	iSmsTest(!aSettings.RejectDuplicate());
	iSmsTest.Printf(_L("Test Reject Duplicate Passed\n"));

// Delivery Report
	aSettings.SetDeliveryReport(ETrue);
	iSmsTest(aSettings.DeliveryReport());
	aSettings.SetDeliveryReport(EFalse);
	iSmsTest(!aSettings.DeliveryReport());
	iSmsTest.Printf(_L("Test Delivery Report Passed\n"));

// Reply Path
	aSettings.SetReplyPath(EFalse);
	iSmsTest(aSettings.ReplyPath()==EFalse);
	aSettings.SetReplyPath(ETrue);
	iSmsTest(aSettings.ReplyPath());
	iSmsTest.Printf(_L("Test Reply Path Passed\n"));

// Validity period format
	aSettings.SetValidityPeriodFormat(TSmsFirstOctet::ESmsVPFNone);
	iSmsTest(aSettings.ValidityPeriodFormat()==TSmsFirstOctet::ESmsVPFNone);
	aSettings.SetValidityPeriodFormat(TSmsFirstOctet::ESmsVPFEnhanced);
	iSmsTest(aSettings.ValidityPeriodFormat()==TSmsFirstOctet::ESmsVPFEnhanced);
	aSettings.SetValidityPeriodFormat(TSmsFirstOctet::ESmsVPFInteger);
	iSmsTest(aSettings.ValidityPeriodFormat()==TSmsFirstOctet::ESmsVPFInteger);
	aSettings.SetValidityPeriodFormat(TSmsFirstOctet::ESmsVPFSemiOctet);
	iSmsTest(aSettings.ValidityPeriodFormat()==TSmsFirstOctet::ESmsVPFSemiOctet);
	iSmsTest.Printf(_L("Test Validity Period Format Passed\n"));

// Message Conversion (following conversions are supported)
	aSettings.SetMessageConversion(ESmsConvFax);
	iSmsTest(aSettings.MessageConversion()==ESmsConvFax);
	aSettings.SetMessageConversion(ESmsConvX400);
	iSmsTest(aSettings.MessageConversion()==ESmsConvX400);
	aSettings.SetMessageConversion(ESmsConvPaging);
	iSmsTest(aSettings.MessageConversion()==ESmsConvPaging);
	aSettings.SetMessageConversion(ESmsConvMail);
	iSmsTest(aSettings.MessageConversion()==ESmsConvMail);
	aSettings.SetMessageConversion(ESmsConvErmes);
	iSmsTest(aSettings.MessageConversion()==ESmsConvErmes);
	aSettings.SetMessageConversion(ESmsConvSpeech);
	iSmsTest(aSettings.MessageConversion()==ESmsConvSpeech);
	aSettings.SetMessageConversion(ESmsConvPIDNone);
	iSmsTest(aSettings.MessageConversion()==ESmsConvPIDNone);
	iSmsTest.Printf(_L("Test Message Conversion Passed\n"));

// Validity Period	
	aSettings.SetValidityPeriod(ESmsVPHour);
	iSmsTest(aSettings.ValidityPeriod().Int()==ESmsVPHour);
	aSettings.SetValidityPeriod(ESmsVPSixHours);
	iSmsTest(aSettings.ValidityPeriod().Int()==ESmsVPSixHours);
	aSettings.SetValidityPeriod(ESmsVP24Hours);
	iSmsTest(aSettings.ValidityPeriod().Int()==ESmsVP24Hours);
	aSettings.SetValidityPeriod(ESmsVPMaximum);
	iSmsTest(aSettings.ValidityPeriod().Int()==ESmsVPMaximum);
	aSettings.SetValidityPeriod(ESmsVPWeek);
	iSmsTest(aSettings.ValidityPeriod().Int()==ESmsVPWeek);
	iSmsTest.Printf(_L("Test Validity Period Passed\n"));

// Can Concatenate (write messages with more than 160 characters)
	aSettings.SetCanConcatenate(ETrue);
	iSmsTest(aSettings.CanConcatenate());
	aSettings.SetCanConcatenate(EFalse);
	iSmsTest(aSettings.CanConcatenate()==EFalse);
	iSmsTest.Printf(_L("Test Can Concatenate Passed\n"));

// Character Set (following character sets are supported)
	aSettings.SetCharacterSet(TSmsDataCodingScheme::ESmsAlphabet7Bit);
	iSmsTest(aSettings.CharacterSet()==TSmsDataCodingScheme::ESmsAlphabet7Bit);
	aSettings.SetCharacterSet(TSmsDataCodingScheme::ESmsAlphabetUCS2);
	iSmsTest(aSettings.CharacterSet()==TSmsDataCodingScheme::ESmsAlphabetUCS2);
	aSettings.SetCharacterSet(TSmsDataCodingScheme::ESmsAlphabet8Bit);
	iSmsTest(aSettings.CharacterSet()==TSmsDataCodingScheme::ESmsAlphabet8Bit);
	iSmsTest.Printf(_L("Test Character Set Passed\n"));
	}

TBool CSmutTest::CompareSettings(const CSmsSettings& aLeft, const CSmsSettings& aRight, TBool aTestSC)
	{
	TCompare compare(CompareMessageSettings(aLeft, aRight));

	compare(aLeft.CommDbAction() == aRight.CommDbAction());
	compare(aLeft.SmsBearerAction() == aRight.SmsBearerAction());
	compare(aLeft.SmsBearer() == aRight.SmsBearer());
	compare(aLeft.Delivery() == aRight.Delivery());
	compare(CompareBools(aLeft.ReplyQuoted(), aRight.ReplyQuoted()));
	compare(aLeft.SpecialMessageHandling() == aRight.SpecialMessageHandling());
	compare(aLeft.StatusReportHandling() == aRight.StatusReportHandling());

	if (aTestSC)
		{
		compare(aLeft.DefaultServiceCenter() == aRight.DefaultServiceCenter());
		compare(aLeft.ServiceCenterCount() == aRight.ServiceCenterCount());

		TInt count = aLeft.ServiceCenterCount();

		while (count--)
			{
			compare(CompareServiceCenters(aLeft.GetServiceCenter(count), aRight.GetServiceCenter(count)));
			}
		}		

	compare(aLeft.Class2Folder() == aRight.Class2Folder());
	compare(aLeft.DescriptionLength() == aRight.DescriptionLength());

	return compare;
	}

TBool CSmutTest::CompareMessageSettings(const CSmsMessageSettings& aLeft, const CSmsMessageSettings& aRight)
	{
	TCompare compare(CompareBools(aLeft.CanConcatenate(), aRight.CanConcatenate()));
	compare(aLeft.CharacterSet() == aRight.CharacterSet());
	compare(CompareBools(aLeft.DeliveryReport(), aRight.DeliveryReport()));
	compare(aLeft.MessageConversion() == aRight.MessageConversion());
	compare(CompareBools(aLeft.RejectDuplicate(), aRight.RejectDuplicate()));
	compare(CompareBools(aLeft.ReplyPath(), aRight.ReplyPath()));
	compare(aLeft.ValidityPeriod() == aRight.ValidityPeriod());
	compare(aLeft.ValidityPeriodFormat() == aRight.ValidityPeriodFormat());
	return compare;
	}

void CSmutTest::TestSettingsRestoreDefectL()
	{
	//Test fix for defects EDNAALR-4KTKTD (CSmsSettings::RestoreL() should leave if stream not present)
	//and EDNAALR-4KTKLP (CSmsSettings::InternalizeL() causes memory leak)

	CSmsSettings* settings = CSmsSettings::NewL();
	CleanupStack::PushL(settings);

	//Test defect fix for EDNAALR-4KTKTD (CSmsSettings::RestoreL() should leave if stream not present)
	const TMsvId id = iSmsTest.CreateDummyMessageToSendL();
	Session().CleanupEntryPushL(id);
	iSmsTest.SetEntryL(id);

	iSmsTest.Printf(_L("TestSettingsRestoreDefectL Created dummy message\n"));

	TRAPD(err, RestoreSettingsL(*settings));

	iSmsTest.Printf(_L("TestSettingsRestoreDefectL RestoreL 1 with error %d\n"), err);
	iSmsTest.Printf(_L("Expected %d\n"), KErrNotFound);
	iSmsTest(err == KErrNotFound);

	//Test defect fix for EDNAALR-4KTKLP (CSmsSettings::InternalizeL() causes memory leak)
//	iSmsTest.SetEntryL(id);
	StoreSettingsL(*settings);

	iSmsTest.Printf(_L("TestSettingsRestoreDefectL StoreL 1\n"));

	settings->AddServiceCenterL(_L("Unknown"), _L("447785016005"));

	RestoreSettingsL(*settings);

	Session().CleanupEntryPop();
	Session().RemoveEntry(id);

	CleanupStack::PopAndDestroy(settings);
	iSmsTest.Printf(_L("TestSettingsRestoreDefectL RestoreL 2\n"));
	}


void CSmutTest::TestSettingsStoreToCommDbL()
	{
	iSmsTest.Test().Next(_L("Storing CSmsSettings to CommDb\n"));
	iSmsTest.Printf(_L("Test Store Settings to CommDb\n"));

	CSmsSettings* settings1 = CSmsSettings::NewL();
	CleanupStack::PushL(settings1);

	CSmsSettings* settings2 = CSmsSettings::NewL();
	CleanupStack::PushL(settings2);

	iSmsTest(CompareSettings(*settings1, *settings2));

	settings1->SetCommDbAction(CSmsSettings::EStoreToCommDb);
	settings1->AddServiceCenterL(_L("Vodafone"), _L("+447785016005"));
	settings1->SetDeliveryReport(ETrue);
	settings1->SetValidityPeriod(1000);

	iSmsTest.SetEntryL(iSmsTest.iSmsServiceId);

	StoreSettingsL(*settings1);
	iSmsTest.Printf(_L("\tNew Settings...\n"));
	iSmsTest(CompareSettingsAndCommDbL(*settings1));

	settings1->GetServiceCenter(settings1->DefaultServiceCenter()).SetAddressL(_L("+3854354433"));

	StoreSettingsL(*settings1);
	iSmsTest.Printf(_L("\tSC Address Changed...\n"));
	iSmsTest(CompareSettingsAndCommDbL(*settings1));
	
	settings1->SetDeliveryReport(EFalse);

	StoreSettingsL(*settings1);
	iSmsTest.Printf(_L("\tDelivery Report Changed...\n"));
	iSmsTest(CompareSettingsAndCommDbL(*settings1));

	settings1->SetValidityPeriod(2000);

	StoreSettingsL(*settings1);
	iSmsTest.Printf(_L("\tValidity Period Changed...\n"));
	iSmsTest(CompareSettingsAndCommDbL(*settings1));

	settings1->GetServiceCenter(settings1->DefaultServiceCenter()).SetAddressL(_L("+48996655888"));
	settings1->SetDeliveryReport(ETrue);
	settings1->SetValidityPeriod(3000);

	StoreSettingsL(*settings1);
	iSmsTest.Printf(_L("\tAll Settings Changed...\n"));
	iSmsTest(CompareSettingsAndCommDbL(*settings1));

	settings1->AddServiceCenterL(_L("Radio"), _L("+385669988"));
	settings1->SetDefaultServiceCenter(settings1->ServiceCenterCount()-1);

	StoreSettingsL(*settings1);
	iSmsTest.Printf(_L("\tDefaultSC Changed...\n"));
	iSmsTest(CompareSettingsAndCommDbL(*settings1));

	settings1->SetCommDbAction(CSmsSettings::ENone);
	settings1->GetServiceCenter(settings1->DefaultServiceCenter()).SetAddressL(_L("+123456789"));
	settings1->SetDeliveryReport(EFalse);
	settings1->SetValidityPeriod(4000);

	iSmsTest.Printf(_L("\tTesting Setting Different from CommDb...\n"));
	iSmsTest(!CompareSettingsAndCommDbL(*settings1));

	TInt count = settings1->ServiceCenterCount();

	while (count--)
		{
		settings1->RemoveServiceCenter(count);
		}

	iSmsTest.Printf(_L("\tTesting Setting Different from CommDb...\n"));
	iSmsTest(!CompareSettingsAndCommDbL(*settings1));

	iSmsTest.Printf(_L("\tRestore settings from store...\n"));
	RestoreSettingsL(*settings2);
	iSmsTest(CompareSettingsAndCommDbL(*settings2));

//Test SMS Bearer

	iSmsTest.Printf(_L("\tTest Store SMS Bearer...\n"));

//Test 1

	settings1->SetSmsBearerAction(CSmsSettings::EStoreToCommDb);
	settings1->SetSmsBearer(CSmsSettings::ESmsBearerPacketOnly);

	StoreSettingsL(*settings1);
	iSmsTest(CompareSettingsAndSmsBearerL(*settings1));
	
//Test 2

	settings1->SetSmsBearer(CSmsSettings::ESmsBearerCircuitOnly);
	iSmsTest(!CompareSettingsAndSmsBearerL(*settings1));

//Test 3

	StoreSettingsL(*settings1);
	iSmsTest(CompareSettingsAndSmsBearerL(*settings1));

	RestoreSettingsL(*settings2);
	iSmsTest(CompareSettingsAndSmsBearerL(*settings2));

//Test 4

	settings1->SetSmsBearerAction(CSmsSettings::ENone);
	StoreSettingsL(*settings1);
	iSmsTest(CompareSettingsAndSmsBearerL(*settings1));

//Test 5

	settings1->SetSmsBearer(CSmsSettings::ESmsBearerCircuitPreferred);
	iSmsTest(!CompareSettingsAndSmsBearerL(*settings1));
	StoreSettingsL(*settings1);
	iSmsTest(!CompareSettingsAndSmsBearerL(*settings1));

	CleanupStack::PopAndDestroy(2); //settings1, settings2
	}

TBool CSmutTest::CompareSettingsAndCommDbL(const CSmsSettings& aSettings) const
	{
	TCompare compare = ETrue;
	TBuf<256> scAddress;
	CMDBRecordSet<CCDGlobalSettingsRecord> globalSettingsRecord(KCDTIdGlobalSettingsRecord);
	TRAPD(err, globalSettingsRecord.LoadL(*iDBSession));
	if(err != KErrNone)
		{
		User::Leave(KErrNotFound);	
		}
	CCDModemBearerRecord *modemBearerRecord = static_cast<CCDModemBearerRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdModemBearerRecord));
	CleanupStack::PushL(modemBearerRecord);
	modemBearerRecord->SetRecordId(((CCDGlobalSettingsRecord*)globalSettingsRecord.iRecords[0])->iModemForPhoneServicesAndSMS); 
	modemBearerRecord->LoadL(*iDBSession);
	scAddress = modemBearerRecord->iMessageCentreNumber;

	if (aSettings.DefaultServiceCenter() < 0 || !aSettings.ServiceCenterCount())
		{
		compare(!scAddress.Length());
		}
	else
		{
		compare(scAddress == aSettings.GetServiceCenter(aSettings.DefaultServiceCenter()).Address());
		}
	
	TUint32 valPeriod = 0;
	valPeriod = modemBearerRecord->iMessageValidityPeriod;
	compare(valPeriod == (TUint32) aSettings.ValidityPeriod().Int());
	
	TBool delReport = EFalse;
	delReport = modemBearerRecord->iMessageDeliveryReport;
	compare(delReport ? aSettings.DeliveryReport() : !aSettings.DeliveryReport());

	CleanupStack::PopAndDestroy(modemBearerRecord);	

	return compare;
	}

TBool CSmutTest::CompareSettingsAndSmsBearerL(const CSmsSettings& aSettings) const
	{
	CMDBRecordSet<CCDGlobalSettingsRecord> globalSettingsRecord(KCDTIdGlobalSettingsRecord);
	globalSettingsRecord.LoadL(*iDBSession);
	TUint32 smsBearer = ((CCDGlobalSettingsRecord*)globalSettingsRecord.iRecords[0])->iSMSBearer;		
	TCompare compare(smsBearer == (TUint32) aSettings.SmsBearer());
	return compare;
	}

void CSmutTest::StoreNumberL(const CSmsNumber& aNumber, TUid aStreamId)
	{
	CMsvStore* store = iSmsTest.EditStoreL(); // lets edit the entry
	CleanupStack::PushL(store);

	RMsvWriteStream out;
	out.AssignLC(*store, aStreamId);		// file stream id ok?
	aNumber.ExternalizeL(out);				// lets extrenalise the instance
	out.CommitL();
	store->CommitL();

	CleanupStack::PopAndDestroy(2, store);
	}

void CSmutTest::RestoreNumberL(CSmsNumber& rNumber, TUid aStreamId)
	{
	CMsvStore* store = iSmsTest.ReadStoreL(); // prepare the store for reading
	CleanupStack::PushL(store);

	RMsvReadStream in;
	in.OpenLC(*store, aStreamId);		// lets open the store for 
	rNumber.InternalizeL(in);				// internalising the address/name

	CleanupStack::PopAndDestroy(2, store);
	}

void CSmutTest::StoreHeaderL(const CSmsHeader& aHeader)
	{
	CMsvStore* store = iSmsTest.EditStoreL();
	CleanupStack::PushL(store);

	aHeader.StoreL(*store);
	store->CommitL();

	CleanupStack::PopAndDestroy(store);
	}

void CSmutTest::RestoreHeaderL(CSmsHeader& aHeader)
	{
	CMsvStore* store = iSmsTest.ReadStoreL();
	CleanupStack::PushL(store);

	aHeader.RestoreL(*store);

	CleanupStack::PopAndDestroy(store);
	}

void CSmutTest::StoreSettingsL(const CSmsSettings& aSettings)
	{
	CSmsAccount* account = CSmsAccount::NewLC();
	account->SaveSettingsL(aSettings);
	CleanupStack::PopAndDestroy(account);    
	}

void CSmutTest::RestoreSettingsL(CSmsSettings& aSettings)
	{
	CSmsAccount* account = CSmsAccount::NewLC();
	account->LoadSettingsL(aSettings);
	CleanupStack::PopAndDestroy(account);    
	}

TBool CSmutTest::CompareServiceCenters(const CSmsServiceCenter& aLeft, const CSmsServiceCenter& aRight)
	{
	TCompare compare(aLeft.Name() == aRight.Name());
	compare(aLeft.Address() == aRight.Address());
	return compare;	
	}

TBool CSmutTest::CompareNumbers(const CSmsNumber& aLeft, const CSmsNumber& aRight)
	{
	TCompare compare(aLeft.Name() == aRight.Name());
	compare(aLeft.Address() == aRight.Address());
	compare(aLeft.LogId() == aRight.LogId());
	compare(aLeft.AckStatus(ESmsAckTypeDelivery) == aRight.AckStatus(ESmsAckTypeDelivery));	
	return compare;
	}

void CSmutTest::TestNumberL()
	{
	iSmsTest.Test().Next(_L("Testing CSmsNumber"));
	iSmsTest.Printf(_L("Preparing test CSmsNumber"));

	TMsvId id=iSmsTest.CreateDummyMessageToSendL();
	Session().CleanupEntryPushL(id);
	iSmsTest.SetEntryL(id);

	CSmsNumber* number0=CSmsNumber::NewL();  
	CleanupStack::PushL(number0);

	CSmsNumber* number=CSmsNumber::NewL();  // create the instance
	CleanupStack::PushL(number);

	// Testing the features of CSmsNumber
	// The max length for a name is 256 characters
	// The max length for a number is 20 characters

// Lets take an ordinary number, test it, then store and restore it
// and see if the values are still ok.

	number->SetNameL(_L("Radiolinja1"));	
	number->SetAddressL(_L("+358508771010")); 
	number->SetLogId(123456789);	
	number->SetAckStatus(ESmsAckTypeDelivery, CSmsNumber::EAckSuccessful);

	iSmsTest(number->Name()==_L("Radiolinja1")); // name ok?
	iSmsTest(number->Address()==_L("+358508771010")); // number ok?
	iSmsTest(number->LogId() == 123456789);
	iSmsTest(number->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EAckSuccessful);

	StoreNumberL(*number);
	RestoreNumberL(*number0);
	iSmsTest(CompareNumbers(*number0, *number));
	
// Lets try the max length of the telephone address, store and
// restore it and see if it works

	number->SetNameL(_L("Whatsthemaximumlengthforthis?Whatsthemaximumlengthforthis?Whatsthemaximumlengthforthis?"));
	number->SetAddressL(_L("+0123456789012345678"));
	number->SetLogId(987654321);	
	number->SetAckStatus(ESmsAckTypeDelivery, CSmsNumber::EAckError);

	iSmsTest(number->Name()==_L("Whatsthemaximumlengthforthis?Whatsthemaximumlengthforthis?Whatsthemaximumlengthforthis?"));
	iSmsTest(number->Address()==_L("+0123456789012345678"));
	iSmsTest(number->LogId() == 987654321);
	iSmsTest(number->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EAckError);

	StoreNumberL(*number);
	RestoreNumberL(*number0);
	iSmsTest(CompareNumbers(*number, *number0));

// Lets use special Scandinavian letters both in the name and
// address, store and restore and see if they are ok
	number->SetNameL(_L("ÖÄÅåöä"));
	number->SetAddressL(_L("åäöÅÄÖ"));
	number->SetLogId(123456789);	
	number->SetAckStatus(ESmsAckTypeDelivery, CSmsNumber::EAckSuccessful);

	iSmsTest(number->Name()==_L("ÖÄÅåöä"));
	iSmsTest(number->Address()==_L("åäöÅÄÖ"));
	iSmsTest(number->LogId() == 123456789);
	iSmsTest(number->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EAckSuccessful);

	StoreNumberL(*number);
	RestoreNumberL(*number0);
	iSmsTest(CompareNumbers(*number, *number0));

//  Lets try if both the number and the name can be empty,
//	store and restore it and see if they still match
	number->SetNameL(_L(""));
	number->SetAddressL(_L(""));
	number->SetLogId(987654321);	
	number->SetAckStatus(ESmsAckTypeDelivery, CSmsNumber::EAckError);

	iSmsTest(number->Name()==_L(""));
	iSmsTest(number->Address()==_L(""));
	iSmsTest(number->LogId() == 987654321);
	iSmsTest(number->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EAckError);

	StoreNumberL(*number);
	RestoreNumberL(*number0);
	iSmsTest(CompareNumbers(*number, *number0));

//  Lets try [alt+255] as both the name and number, store and	
//  restore it and see if it works
	number->SetNameL(_L(" ")); //alt+255
	number->SetAddressL(_L(" "));
	number->SetLogId(123456789);	
	number->SetAckStatus(ESmsAckTypeDelivery, CSmsNumber::EAckSuccessful);

	iSmsTest(number->Name()==_L(" "));
	iSmsTest(number->Address()==_L(" "));
	iSmsTest(number->LogId() == 123456789);
	iSmsTest(number->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EAckSuccessful);

	StoreNumberL(*number);
	RestoreNumberL(*number0);
	iSmsTest(CompareNumbers(*number, *number0));

// Test CSmsNumber::CopyL()

	number->SetNameL(_L("DavidCuando"));	
	number->SetAddressL(_L("+447747065627")); 
	number->SetLogId(118888);	
	number->SetAckStatus(ESmsAckTypeDelivery, CSmsNumber::EPendingAck);

	iSmsTest(number->Name()==_L("DavidCuando")); 
	iSmsTest(number->Address()==_L("+447747065627")); 
	iSmsTest(number->LogId() == 118888);
	iSmsTest(number->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck);

	number0->CopyL(*number);
	iSmsTest(CompareNumbers(*number0, *number));

	CleanupStack::PopAndDestroy(2, number0);

// Test NewL(CSmsNumber&)

	number=CSmsNumber::NewL();  // create the instance
	CleanupStack::PushL(number);

	number->SetNameL(_L("DavidCuando"));	
	number->SetAddressL(_L("+447747065627")); 
	number->SetLogId(118888);	
	number->SetAckStatus(ESmsAckTypeDelivery, CSmsNumber::EPendingAck);

	iSmsTest(number->Name()==_L("DavidCuando")); 
	iSmsTest(number->Address()==_L("+447747065627")); 
	iSmsTest(number->LogId() == 118888);
	iSmsTest(number->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck);

	number0=CSmsNumber::NewL(*number);  
	CleanupStack::PushL(number0);

	iSmsTest(CompareNumbers(*number0, *number));
	CleanupStack::PopAndDestroy(2, number);

	//Test for defect EDNMOHN-4LDDK8
	number=CSmsNumber::NewL();  // create the instance
	CleanupStack::PushL(number);

	StoreNumberL(*number);
	RestoreNumberL(*number);

	number->SetAddressL(_L("+4477676545"));
	//End Test for defect  EDNMOHN-4LDDK8

	CleanupStack::PopAndDestroy(number);

	Session().CleanupEntryPop();
	Session().RemoveEntry(id);
	}
	
void CSmutTest::StoreEmailFieldsL(const CSmsEmailFields& aEmailFields)
	{
	CMsvStore* store = iSmsTest.EditStoreL(); 
	CleanupStack::PushL(store);

	aEmailFields.StoreL(*store);
	store->CommitL();

	CleanupStack::PopAndDestroy(store);
	}

void CSmutTest::RestoreEmailFieldsL(CSmsEmailFields& aEmailFields)
	{
	CMsvStore* store = iSmsTest.ReadStoreL(); // prepare the store for reading
	CleanupStack::PushL(store);
	
	aEmailFields.RestoreL(*store);

	CleanupStack::PopAndDestroy(store);
	}
	
TBool CSmutTest::CompareEmailFields(const CSmsEmailFields& aFields1, const CSmsEmailFields& aFields2)
	{
	TBool same = CompareEmailFieldsAddresses(aFields1.Addresses(), aFields2.Addresses());
	if( same )
		same = (aFields2.Subject().Compare(aFields1.Subject()) == 0);
	if( same )
		same = (!aFields2.HasAddress()&&!aFields1.HasAddress() || aFields2.HasAddress()&&aFields1.HasAddress());
	if( same )
		same = (aFields2.Length() == aFields1.Length());
	return same;
	}
	
TBool CSmutTest::CompareEmailFieldsAddresses(const MDesCArray& aAddresses1, const MDesCArray& aAddresses2)
	{
	TBool same = (aAddresses1.MdcaCount() == aAddresses2.MdcaCount());
	if( same )
		{
		TInt count = aAddresses1.MdcaCount();
		while( same && count-- > 0)
			{
			same = (aAddresses2.MdcaPoint(count).Compare(aAddresses1.MdcaPoint(count)) == 0);
			}
		}		
	return same;
	}
	
void CSmutTest::TestEmailFieldsL()
	{
	iSmsTest.Test().Next(_L("Testing CSmsEmailFields"));
	iSmsTest.Printf(_L("Preparing test CSmsEmailFields"));

	TMsvId id=iSmsTest.CreateDummyMessageToSendL();
	Session().CleanupEntryPushL(id);
	iSmsTest.SetEntryL(id);

	_LIT(KTestAddress, "support@symbian.com");
	_LIT(KTestSubject, "Happy Days!");
	
	// Test CSmsEmailFields::NewL()
	CSmsEmailFields* fields1 = CSmsEmailFields::NewL();
	CleanupStack::PushL(fields1);
	
	iSmsTest(fields1->Addresses().MdcaCount()==0);
	iSmsTest(fields1->Subject().Compare(KNullDesC()) == 0);
	iSmsTest(!fields1->HasAddress());
	iSmsTest(fields1->Length() == 0);
	
	// Test address and subject accessors, has-address, length and reset methods
	fields1->AddAddressL(KNullDesC);	// empty address - does nothing!
	iSmsTest(fields1->Addresses().MdcaCount()==0);
	iSmsTest(fields1->Subject().Compare(KNullDesC()) == 0);
	iSmsTest(!fields1->HasAddress());
	iSmsTest(fields1->Length() == 0);
	
	fields1->AddAddressL(KTestAddress);
	iSmsTest(fields1->Addresses().MdcaCount()==1);	
	iSmsTest(fields1->Addresses().MdcaPoint(0).Compare(KTestAddress) == 0);
	iSmsTest(fields1->HasAddress());
	iSmsTest(fields1->Length() == KTestAddress().Length() + 1);	

	fields1->SetSubjectL(KTestSubject);
	iSmsTest(fields1->HasAddress());
	iSmsTest(fields1->Length() == KTestAddress().Length() + KTestSubject().Length() + 2);
	
	fields1->Reset();
	iSmsTest(fields1->Addresses().MdcaCount()==0);
	iSmsTest(fields1->Subject().Compare(KNullDesC()) == 0);
	iSmsTest(!fields1->HasAddress());
	iSmsTest(fields1->Length() == 0);
	
	fields1->SetSubjectL(KTestSubject);
	iSmsTest(fields1->Subject().Compare(KTestSubject) == 0);
	iSmsTest(!fields1->HasAddress());
	iSmsTest(fields1->Length() == KTestSubject().Length() + 2);
	
	fields1->AddAddressL(KTestAddress);
	iSmsTest(fields1->Addresses().MdcaCount()==1);	
	iSmsTest(fields1->Addresses().MdcaPoint(0).Compare(KTestAddress) == 0);
	iSmsTest(fields1->HasAddress());
	iSmsTest(fields1->Length() == KTestAddress().Length() + KTestSubject().Length() + 2);

	// Test multiple addresses - add empty address -> not added	
	fields1->AddAddressL(KNullDesC);
	iSmsTest(fields1->Addresses().MdcaCount()==1);
	
	_LIT(KTestAddress2, "support2@symbian.com");
	fields1->AddAddressL(KTestAddress2);
	iSmsTest(fields1->Addresses().MdcaCount()==2);	
	iSmsTest(fields1->Addresses().MdcaPoint(1).Compare(KTestAddress2) == 0);
	iSmsTest(fields1->HasAddress());
	// NOTE - length check; 3 -> 2 for subject delimiters, 1 for address comma separtor.
	iSmsTest(fields1->Length() == KTestAddress().Length() + KTestAddress2().Length() + KTestSubject().Length() + 3);
	
	// Test copy factory c'tor
	CSmsEmailFields* fields2 = CSmsEmailFields::NewL(*fields1);
	CleanupStack::PushL(fields2);

	iSmsTest(CompareEmailFields(*fields2, *fields1));
	
	// Test store and restore
	StoreEmailFieldsL(*fields1);
	fields2->Reset();
	RestoreEmailFieldsL(*fields2);

	iSmsTest(CompareEmailFields(*fields2, *fields1));
	
	fields1->Reset();
	StoreEmailFieldsL(*fields1);
	RestoreEmailFieldsL(*fields2);

	iSmsTest(CompareEmailFields(*fields2, *fields1));
	
	CleanupStack::PopAndDestroy(2, fields1);
	
	// Test parsing - expected data.
	_LIT(KTestBodyData, "Some body text#( )#");

	_LIT(KTestParseAddress1, "user1@symbian.com");
	_LIT(KTestParseSubject1, "");
	_LIT(KTestEmail1, "user1@symbian.com Some body text#( )#");
	TestEmailFieldsParsePassL(KTestEmail1, KTestParseAddress1, KTestParseSubject1, KTestBodyData);

	_LIT(KTestParseFullAddress1, "Some User <user1@symbian.com>");
	_LIT(KTestEmail2, "Some User <user1@symbian.com> Some body text#( )#");
	TestEmailFieldsParsePassL(KTestEmail2, KTestParseFullAddress1, KTestParseSubject1, KTestBodyData);

	_LIT(KTestParseAddress2, "user2@symbian.com");
	_LIT(KTestParseSubject2, "subject (2)");
	_LIT(KTestEmail3, "user2@symbian.com##subject (2)#Some body text#( )#");
	TestEmailFieldsParsePassL(KTestEmail3, KTestParseAddress2, KTestParseSubject2, KTestBodyData);

	_LIT(KTestParseFullAddress2, "Some User <user2@symbian.com>");
	_LIT(KTestEmail4, "Some User <user2@symbian.com>##subject (2)#Some body text#( )#");
	TestEmailFieldsParsePassL(KTestEmail4, KTestParseFullAddress2, KTestParseSubject2, KTestBodyData);

	_LIT(KTestParseAddress3, "user3@symbian.com");
	_LIT(KTestParseSubject3, "subject #3");
	_LIT(KTestEmail5, "user3@symbian.com(subject #3)Some body text#( )#");
	TestEmailFieldsParsePassL(KTestEmail5, KTestParseAddress3, KTestParseSubject3, KTestBodyData);

	_LIT(KTestParseFullAddress3, "Some User <user3@symbian.com>");
	_LIT(KTestEmail6, "Some User <user3@symbian.com>(subject #3)Some body text#( )#");
	TestEmailFieldsParsePassL(KTestEmail6, KTestParseFullAddress3, KTestParseSubject3, KTestBodyData);
	
	_LIT(KTestParseAddress4, "user4@symbian.com");
	_LIT(KTestParseSubject4, "");
	_LIT(KTestEmail7, "user4@symbian.com()Some body text#( )#");
	TestEmailFieldsParsePassL(KTestEmail7, KTestParseAddress4, KTestParseSubject4, KTestBodyData);

	_LIT(KTestParseFullAddress4, "Some User <user4@symbian.com>");
	_LIT(KTestEmail8, "Some User <user4@symbian.com>()Some body text#( )#");
	TestEmailFieldsParsePassL(KTestEmail8, KTestParseFullAddress4, KTestParseSubject4, KTestBodyData);

	_LIT(KTestParseAddress5, "user5@symbian.com");
	_LIT(KTestParseSubject5, "");
	_LIT(KTestEmail9, "user5@symbian.com###Some body text#( )#");
	TestEmailFieldsParsePassL(KTestEmail9, KTestParseAddress5, KTestParseSubject5, KTestBodyData);

	_LIT(KTestParseFullAddress5, "Some User <user5@symbian.com>");
	_LIT(KTestEmail10, "Some User <user5@symbian.com>###Some body text#( )#");
	TestEmailFieldsParsePassL(KTestEmail10, KTestParseFullAddress5, KTestParseSubject5, KTestBodyData);

	// Test parsing - corrupt but parse-able data.
	_LIT(KTestCorruptAddress1,	"user1@symbian.com");
	_LIT(KTestCorruptSubject1,	"smiley ;");
	_LIT(KTestCorruptBody1,		" subject)Some body text");
	_LIT(KTestCorruptEmail1,	"user1@symbian.com(smiley ;) subject)Some body text");
	TestEmailFieldsParsePassL(KTestCorruptEmail1, KTestCorruptAddress1, KTestCorruptSubject1, KTestCorruptBody1);
	
	_LIT(KTestCorruptAddress2,	"user1@symbian.com");
	_LIT(KTestCorruptSubject2,	"");
	_LIT(KTestCorruptBody2,		"(bad subject)Some body text");
	_LIT(KTestCorruptEmail2,	"user1@symbian.com (bad subject)Some body text");
	TestEmailFieldsParsePassL(KTestCorruptEmail2, KTestCorruptAddress2, KTestCorruptSubject2, KTestCorruptBody2);

	_LIT(KTestCorruptAddress3,	"user1@symbian.com");
	_LIT(KTestCorruptSubject3,	"");
	_LIT(KTestCorruptBody3,		"bad subject##Some body text");
	_LIT(KTestCorruptEmail3,	"user1@symbian.com###bad subject##Some body text");
	TestEmailFieldsParsePassL(KTestCorruptEmail3, KTestCorruptAddress3, KTestCorruptSubject3, KTestCorruptBody3);
	
	// Test parsing - corrupt and fail.
	_LIT(KTestFailEmail1,	"user#domain1.domain2 Some body text");
	TestEmailFieldsParseFailL(KTestFailEmail1);
	
	_LIT(KTestFailEmail2,	"user@domain1.domain2+Some+body+text");
	TestEmailFieldsParseFailL(KTestFailEmail2);

	_LIT(KTestFailEmail3,	"user@domain1.domain2##(subject)Some+body+text");
	TestEmailFieldsParseFailL(KTestFailEmail3);

	_LIT(KTestFailEmail4,	"user@domain1.domain2(##subject#Some+body+text");
	TestEmailFieldsParseFailL(KTestFailEmail4);

	_LIT(KTestFailEmail5,	"user1@symbian.com#bad subject#Some body text");
	TestEmailFieldsParseFailL(KTestFailEmail5);
	
	// Test composing - ok
	_LIT(KTestComposeAddress1,	"user1@domain1.domain2");
	_LIT(KTestComposeSubject1,	"");
	_LIT(KTestComposeFields1,	"user1@domain1.domain2 ");
	TestEmailFieldsComposeL(KTestComposeFields1, KTestComposeAddress1, KNullDesC, KTestComposeSubject1);
	
	_LIT(KTestComposeAddress2,	"user2@domain1.domain2");
	_LIT(KTestComposeSubject2,	"a subject");
	_LIT(KTestComposeFields2,	"user2@domain1.domain2(a subject)");
	TestEmailFieldsComposeL(KTestComposeFields2, KTestComposeAddress2, KNullDesC, KTestComposeSubject2);

	_LIT(KTestComposeFields3,	"user1@domain1.domain2,user2@domain1.domain2 ");
	TestEmailFieldsComposeL(KTestComposeFields3, KTestComposeAddress1, KTestComposeAddress2, KTestComposeSubject1);
	
	_LIT(KTestComposeFields4,	"user2@domain1.domain2,user1@domain1.domain2(a subject)");
	TestEmailFieldsComposeL(KTestComposeFields4, KTestComposeAddress2, KTestComposeAddress1, KTestComposeSubject2);

	// Testing composing - fail, no address.
	CSmsEmailFields* fields3 = CSmsEmailFields::NewL();
	CleanupStack::PushL(fields3);
	
	fields3->AddAddressL(KNullDesC);
	fields3->SetSubjectL(KTestSubject);

	TRAPD(err, fields3->ComposeLC());
	
	iSmsTest(err==KErrCorrupt);
	
	CleanupStack::PopAndDestroy(fields3);

	Session().CleanupEntryPop();
	Session().RemoveEntry(id);
	}
	
void CSmutTest::TestEmailFieldsParsePassL(const TDesC& aMessage, const TDesC& aAddress, const TDesC& aSubject, const TDesC& aBody)
	{
	CSmsEmailFields* fields = CSmsEmailFields::NewL();
	CleanupStack::PushL(fields);
	
	TInt pos = fields->ParseL(aMessage);
	
	TPtrC body(aMessage.Mid(pos));
	iSmsTest(fields->HasAddress());
	iSmsTest(fields->Addresses().MdcaCount()==1);
	iSmsTest(fields->Addresses().MdcaPoint(0).Compare(aAddress) == 0);
	iSmsTest(fields->Subject().Compare(aSubject) == 0);
	iSmsTest(body.Compare(aBody) == 0);
	
	CleanupStack::PopAndDestroy(fields);
	}

void CSmutTest::TestEmailFieldsParseFailL(const TDesC& aMessage)
	{
	CSmsEmailFields* fields = CSmsEmailFields::NewL();
	CleanupStack::PushL(fields);
	
	TInt pos = fields->ParseL(aMessage);
	
	iSmsTest(pos == KErrCorrupt);
	
	CleanupStack::PopAndDestroy(fields);	
	}

void CSmutTest::TestEmailFieldsComposeL(const TDesC& aFields, const TDesC& aAddress1, const TDesC& aAddress2, const TDesC& aSubject)
	{
	CSmsEmailFields* fields = CSmsEmailFields::NewL();
	CleanupStack::PushL(fields);
	
	fields->AddAddressL(aAddress1);
	fields->AddAddressL(aAddress2);
	fields->SetSubjectL(aSubject);
	HBufC* buffer = fields->ComposeLC();	

	iSmsTest(aFields.Compare(*buffer) == 0);
	
	CleanupStack::PopAndDestroy(2, fields);	// fields, buffer
	}

void CSmutTest::TestHeaderL()
	{

// Testing class CSmsHeader
	iSmsTest.Test().Next(_L("Testing CSmsHeader"));
	
// A CSmsHeader can be of 6 different types
// First we test constructing ESmsDeliver
	delete iSmsHeader; // deleting the previous one
	iSmsHeader = NULL;
	iSmsHeader=CSmsHeader::NewL(CSmsPDU::ESmsDeliver,*iRichText); // constructing
	iSmsTest(iSmsHeader->Type()==CSmsPDU::ESmsDeliver);	// type ok?

	CSmsDeliver& deliver = iSmsHeader->Deliver();  // lets get the CSmsPDU of the CSmsHeader
	const CSmsDeliver& deliver2 = iSmsHeader->Deliver();

	iSmsTest(deliver.Type() == CSmsPDU::ESmsDeliver); // Are the types ok?
	iSmsTest(deliver2.Type() == CSmsPDU::ESmsDeliver);

// Type ESmsCommand
	delete iSmsHeader; // deleting previous
	iSmsHeader=CSmsHeader::NewL(CSmsPDU::ESmsCommand,*iRichText);
	iSmsTest(iSmsHeader->Type()==CSmsPDU::ESmsCommand);

	CSmsCommand& command=iSmsHeader->Command();
	const CSmsCommand& command2=iSmsHeader->Command();

	iSmsTest(command.Type() == CSmsPDU::ESmsCommand);
	iSmsTest(command2.Type() == CSmsPDU::ESmsCommand);

// Type ESmsStatusReport
	delete iSmsHeader; // deleting the previous one
	iSmsHeader=CSmsHeader::NewL(CSmsPDU::ESmsStatusReport,*iRichText);
	iSmsTest(iSmsHeader->Type()==CSmsPDU::ESmsStatusReport);

	CSmsStatusReport& statusreport=iSmsHeader->StatusReport();
	const CSmsStatusReport& statusreport2=iSmsHeader->StatusReport();

	iSmsTest(statusreport.Type() == CSmsPDU::ESmsStatusReport);
	iSmsTest(statusreport2.Type() == CSmsPDU::ESmsStatusReport);

// Type ESmsSubmit
	delete iSmsHeader; // deleting the previous one
	iSmsHeader=CSmsHeader::NewL(CSmsPDU::ESmsSubmit,*iRichText);
	iSmsTest(iSmsHeader->Type()==CSmsPDU::ESmsSubmit);

	CSmsSubmit& submit=iSmsHeader->Submit();
	const CSmsSubmit& submit2=iSmsHeader->Submit();

	iSmsTest(submit.Type() == CSmsPDU::ESmsSubmit);
	iSmsTest(submit2.Type() == CSmsPDU::ESmsSubmit);

// Lets access the CSmsMessage of our header
	CSmsMessage& message=iSmsHeader->Message();
	const CSmsMessage& message2=iSmsHeader->Message();

	iSmsTest(message.Type() == CSmsPDU::ESmsSubmit);
	iSmsTest(message2.Type() == CSmsPDU::ESmsSubmit);

// Lets test the features of CSmsHeader with different values
	iSmsHeader->SetReplyPathProvided(ETrue);
	iSmsTest(iSmsHeader->ReplyPathProvided());
	iSmsHeader->SetReplyPathProvided(EFalse);
	iSmsTest(!iSmsHeader->ReplyPathProvided());

	iSmsHeader->SetBioMsgIdType(EBioMsgIdUnknown);
	iSmsTest(iSmsHeader->BioMsgIdType()==EBioMsgIdUnknown);
	iSmsHeader->SetBioMsgIdType(EBioMsgIdIana);
	iSmsTest(iSmsHeader->BioMsgIdType()==EBioMsgIdIana);
	iSmsHeader->SetBioMsgIdType(EBioMsgIdNbs);
	iSmsTest(iSmsHeader->BioMsgIdType()==EBioMsgIdNbs);
	iSmsHeader->SetBioMsgIdType(EBioMsgIdWap);
	iSmsTest(iSmsHeader->BioMsgIdType()==EBioMsgIdWap);
	iSmsHeader->SetBioMsgIdType(EBioMsgIdWapSecure);
	iSmsTest(iSmsHeader->BioMsgIdType()==EBioMsgIdWapSecure);

// Lets set the service center number
	iSmsHeader->SetServiceCenterAddressL(_L("+350508771010"));
	iSmsTest(iSmsHeader->ServiceCenterAddress()==_L("+350508771010"));

// Lets set one recipient for the message
	CArrayPtrFlat<CSmsNumber>& recipients = iSmsHeader->Recipients();
	CSmsNumber* number=CSmsNumber::NewL();
	CleanupStack::PushL(number);
	number->SetAddressL(_L("+358503367709"));
	recipients.AppendL(number);
	CleanupStack::Pop();

// Setting settings of the message
	CSmsSettings* smsset = CSmsSettings::NewL();
	CleanupStack::PushL(smsset);
	smsset->AddServiceCenterL(_L("Hippo"), _L("+358508771010"));
	smsset->SetCharacterSet(TSmsDataCodingScheme::ESmsAlphabet7Bit);
	smsset->SetDelivery(ESmsDeliveryImmediately);
	smsset->SetDeliveryReport(EFalse);
	smsset->SetMessageConversion(ESmsConvPIDNone);
	smsset->SetRejectDuplicate(ETrue);
	smsset->SetReplyPath(EFalse);
	smsset->SetValidityPeriod(ESmsVP24Hours);
	smsset->SetValidityPeriodFormat(TSmsFirstOctet::ESmsVPFSemiOctet);
// And copying them to the CSmsHeader
	iSmsHeader->SetSmsSettingsL(*smsset);
	CleanupStack::PopAndDestroy(smsset);

	CSmsSettings* smsset2 = CSmsSettings::NewL();
	CleanupStack::PushL(smsset2);
// Lets get the same setting from the CSmsHeader
	iSmsHeader->GetSmsSettingsL(*smsset2);
// And compare that they are ok!
	iSmsTest(smsset2->CharacterSet()==TSmsDataCodingScheme::ESmsAlphabet7Bit);
	iSmsTest(!smsset2->DeliveryReport());
	iSmsTest(smsset2->MessageConversion()==ESmsConvPIDNone);
	iSmsTest(smsset2->RejectDuplicate());
	iSmsTest(!smsset2->ReplyPath());
	iSmsTest(smsset2->ValidityPeriod().Int()==ESmsVP24Hours);
	iSmsTest(smsset2->ValidityPeriodFormat()==TSmsFirstOctet::ESmsVPFSemiOctet);
	CleanupStack::PopAndDestroy(smsset2);

//Test Set/FromAddress()
	iSmsHeader->SetFromAddressL(_L("+358503367709"));
	iSmsTest(iSmsHeader->FromAddress()==_L("+358503367709"));
	
//Test Set/EmailFields()
	_LIT(KTestAddress, "user@domain1.domain2");
	_LIT(KTestSubject, "a subject");

	CSmsEmailFields* fields1 = CSmsEmailFields::NewL();
	CleanupStack::PushL(fields1);
	
	fields1->AddAddressL(KTestAddress);
	fields1->SetSubjectL(KTestSubject);

	// Test the PID field value - should be ESmsConvPIDNone...
	CSmsSettings* smsset3 = CSmsSettings::NewL();
	CleanupStack::PushL(smsset3);

	iSmsHeader->GetSmsSettingsL(*smsset3);
	iSmsTest(smsset3->MessageConversion()==ESmsConvPIDNone);

	// Set the email fields - check the email fields and the PID value (should 
	// be ESmsConvMail).
	iSmsHeader->SetEmailFieldsL(*fields1);
	const CSmsEmailFields& fields2 = iSmsHeader->EmailFields();
	iSmsHeader->GetSmsSettingsL(*smsset3);
	
	iSmsTest(CompareEmailFields(*fields1, fields2));
	iSmsTest(smsset3->MessageConversion()==ESmsConvMail);
	
	// Reset the fields and then set back in the header - should reset the PID
	// value to ESmsConvPIDNone
	fields1->Reset();
	iSmsHeader->SetEmailFieldsL(*fields1);
	
	const CSmsEmailFields& fields3 = iSmsHeader->EmailFields();
	iSmsHeader->GetSmsSettingsL(*smsset3);
	
	iSmsTest(CompareEmailFields(*fields1, fields3));
	iSmsTest(smsset3->MessageConversion()==ESmsConvPIDNone);
	
	// Set the reply email fields - check email fields (both address and subject)
	// and the PID value (should be ESmsConvMail)
	_LIT(KEmailReplySubjectFormat,	"Re: %S");
	_LIT(KEmailReplySubject,		"Re: a subject");

	// First test with no subject...
	fields1->AddAddressL(KTestAddress);
	
	iSmsHeader->SetReplyEmailFieldsL(*fields1, KEmailReplySubjectFormat);
	const CSmsEmailFields& replyFields1 = iSmsHeader->EmailFields();
	iSmsHeader->GetSmsSettingsL(*smsset3);
	
	iSmsTest(CompareEmailFieldsAddresses(replyFields1.Addresses(), fields1->Addresses()));
	iSmsTest(replyFields1.Subject().Compare(KNullDesC) == 0);
	iSmsTest(smsset3->MessageConversion()==ESmsConvMail);
	
	// Now set the subject...
	fields1->SetSubjectL(KTestSubject);

	iSmsHeader->SetReplyEmailFieldsL(*fields1, KEmailReplySubjectFormat);
	const CSmsEmailFields& replyFields2 = iSmsHeader->EmailFields();
	iSmsHeader->GetSmsSettingsL(*smsset3);
	
	iSmsTest(CompareEmailFieldsAddresses(replyFields2.Addresses(), fields1->Addresses()));
	iSmsTest(replyFields2.Subject().Compare(KEmailReplySubject()) == 0);
	iSmsTest(smsset3->MessageConversion()==ESmsConvMail);
	
	// Repeat - test for (lack of) Re: chain.
	fields1->SetSubjectL(KEmailReplySubject);
	
	iSmsHeader->SetReplyEmailFieldsL(*fields1, KEmailReplySubjectFormat);
	const CSmsEmailFields& replyFields3 = iSmsHeader->EmailFields();
	iSmsHeader->GetSmsSettingsL(*smsset3);
	
	iSmsTest(CompareEmailFieldsAddresses(replyFields3.Addresses(), fields1->Addresses()));
	iSmsTest(replyFields3.Subject().Compare(KEmailReplySubject()) == 0);
	iSmsTest(smsset3->MessageConversion()==ESmsConvMail);
	
	// Set the forward email fields - check email fields (both address and subject)
	// and the PID value (should be ESmsConvMail)
	_LIT(KEmailForwardSubjectFormat,	"Fw: %S");
	_LIT(KEmailForwardSubject,			"Fw: a subject");
	
	// First test with no subject...
	fields1->Reset();
	fields1->AddAddressL(KTestAddress);

	iSmsHeader->SetForwardEmailFieldsL(*fields1, KEmailForwardSubjectFormat);
	const CSmsEmailFields& forwardFields1 = iSmsHeader->EmailFields();
	iSmsHeader->GetSmsSettingsL(*smsset3);
	
	iSmsTest(!forwardFields1.HasAddress());
	iSmsTest(forwardFields1.Addresses().MdcaCount()==0);
	iSmsTest(forwardFields1.Subject().Compare(KNullDesC) == 0);
	iSmsTest(smsset3->MessageConversion()==ESmsConvMail);

	// Now set the subject...
	fields1->SetSubjectL(KTestSubject);

	iSmsHeader->SetForwardEmailFieldsL(*fields1, KEmailForwardSubjectFormat);
	const CSmsEmailFields& forwardFields2 = iSmsHeader->EmailFields();
	iSmsHeader->GetSmsSettingsL(*smsset3);
	
	iSmsTest(!forwardFields2.HasAddress());
	iSmsTest(forwardFields2.Addresses().MdcaCount()==0);
	iSmsTest(forwardFields2.Subject().Compare(KEmailForwardSubject()) == 0);
	iSmsTest(smsset3->MessageConversion()==ESmsConvMail);
	
	// Repeat - test for (lack of) Fw: chain.
	fields1->SetSubjectL(KEmailForwardSubject);
	
	iSmsHeader->SetForwardEmailFieldsL(*fields1, KEmailForwardSubjectFormat);
	const CSmsEmailFields& forwardFields3 = iSmsHeader->EmailFields();
	iSmsHeader->GetSmsSettingsL(*smsset3);
	
	iSmsTest(!forwardFields3.HasAddress());
	iSmsTest(forwardFields3.Addresses().MdcaCount()==0);
	iSmsTest(forwardFields3.Subject().Compare(KEmailForwardSubject()) == 0);
	iSmsTest(smsset3->MessageConversion()==ESmsConvMail);

	// Reset the fields and set back in the header.
	fields1->Reset();
	iSmsHeader->SetEmailFieldsL(*fields1);

	CleanupStack::PopAndDestroy(2, fields1);

//Lets store and restore the CSmsHeader and test that the values are right
	TMsvId id = iSmsTest.CreateDummyMessageToSendL();
	Session().CleanupEntryPushL(id);
	iSmsTest.SetEntryL(id);
	
	StoreHeaderL(*iSmsHeader);
	
	// Check that the email fields stream is not present, as there is no email
	// fields data.
	CMsvStore* store1 = iSmsTest.ReadStoreL();
	CleanupStack::PushL(store1);
	iSmsTest(!store1->IsPresentL(KUidMsvSmsEmailFieldsStream));
	CleanupStack::PopAndDestroy(store1);

	CSmsHeader* header1 = CSmsHeader::NewL(CSmsPDU::ESmsSubmit,*iRichText);
	CleanupStack::PushL(header1);

	RestoreHeaderL(*header1);

//Values ok?
	iSmsTest(iSmsHeader->Type()==header1->Type());
	iSmsTest(!header1->ReplyPathProvided());
	iSmsTest(header1->ServiceCenterAddress()==_L("+350508771010"));
	iSmsTest(iSmsHeader->Recipients().Count()==header1->Recipients().Count());
	iSmsTest(iSmsHeader->BioMsgIdType()==header1->BioMsgIdType());
	iSmsTest(iSmsHeader->FromAddress()==header1->FromAddress());

// Lets get the setting of the message
	CSmsSettings* smsset4 = CSmsSettings::NewL();
	CleanupStack::PushL(smsset4);
	header1->GetSmsSettingsL(*smsset4);

// And compare that they are the original values
	iSmsTest(smsset4->CharacterSet()==TSmsDataCodingScheme::ESmsAlphabet7Bit);
	iSmsTest(!smsset4->DeliveryReport());
	iSmsTest(smsset4->MessageConversion()==ESmsConvPIDNone);
	iSmsTest(smsset4->RejectDuplicate());
	iSmsTest(!smsset4->ReplyPath());
	iSmsTest(smsset4->ValidityPeriod().Int()==ESmsVP24Hours);
	iSmsTest(smsset4->ValidityPeriodFormat()==TSmsFirstOctet::ESmsVPFSemiOctet);
	
	CleanupStack::PopAndDestroy(2, header1);
	
	// Test with email fields data.
	CSmsEmailFields* fields8 = CSmsEmailFields::NewL();
	CleanupStack::PushL(fields8);
	fields8->AddAddressL(KTestAddress);
	fields8->SetSubjectL(KTestSubject);
	iSmsHeader->SetEmailFieldsL(*fields8);
	
	StoreHeaderL(*iSmsHeader);

	// Check that the email fields stream is  present, as there is now email
	// fields data.
	CMsvStore* store2 = iSmsTest.EditStoreL();
	CleanupStack::PushL(store2);
	iSmsTest(store2->IsPresentL(KUidMsvSmsEmailFieldsStream));
	CleanupStack::PopAndDestroy(store2);

	CSmsHeader* header2 = CSmsHeader::NewL(CSmsPDU::ESmsSubmit,*iRichText);
	CleanupStack::PushL(header2);

	RestoreHeaderL(*header2);

	iSmsTest(iSmsHeader->Type()==header2->Type());
	iSmsTest(!header2->ReplyPathProvided());
	iSmsTest(header2->ServiceCenterAddress()==_L("+350508771010"));
	iSmsTest(iSmsHeader->Recipients().Count()==header2->Recipients().Count());
	iSmsTest(iSmsHeader->BioMsgIdType()==header2->BioMsgIdType());
	iSmsTest(iSmsHeader->FromAddress()==header2->FromAddress());

	CSmsSettings* smsset5 = CSmsSettings::NewL();
	CleanupStack::PushL(smsset5);
	header2->GetSmsSettingsL(*smsset5);

	iSmsTest(smsset5->CharacterSet()==TSmsDataCodingScheme::ESmsAlphabet7Bit);
	iSmsTest(!smsset5->DeliveryReport());
	iSmsTest(smsset5->MessageConversion()==ESmsConvMail);
	iSmsTest(smsset5->RejectDuplicate());
	iSmsTest(!smsset5->ReplyPath());
	iSmsTest(smsset5->ValidityPeriod().Int()==ESmsVP24Hours);
	iSmsTest(smsset5->ValidityPeriodFormat()==TSmsFirstOctet::ESmsVPFSemiOctet);
	
	const CSmsEmailFields& fields9 = iSmsHeader->EmailFields();
	iSmsTest(CompareEmailFields(*fields8, fields9));
	
	CleanupStack::PopAndDestroy(3, fields8);
	
//Test NewL that takes a CSmsMessage - pretend it's an email message.
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	// Test un-corrupt email - add an appropriate email message
	_LIT(KTestEmailAddress1, 	"user@domain1.domain2");
	_LIT(KTestEmailSubject1, 	"a subject");
	_LIT(KTestEmailBody1,		"some body text");
	_LIT(KTestEmailMessage1,	"user@domain1.domain2##a subject#some body text");

	CSmsMessage* email1 = CSmsMessage::NewL(fs, CSmsPDU::ESmsDeliver, CSmsEditorBuffer::NewL(*iRichText), EFalse);
	CleanupStack::PushL(email1);
	
	CSmsBufferBase& buffer1 = email1->Buffer();
	buffer1.Reset();
	buffer1.InsertL(0, KTestEmailMessage1());
	
	// Set the PID for email.
	CSmsPDU& pdu1 = email1->SmsPDU();
	if( pdu1.ProtocolIdentifierPresent() )
		{
		pdu1.SetPIDType(TSmsProtocolIdentifier::ESmsPIDTelematicInterworking);
		pdu1.SetTelematicDeviceIndicator(TSmsProtocolIdentifier::ESmsTelematicDevice);
		pdu1.SetTelematicDeviceType(TSmsProtocolIdentifier::ESmsInternetElectronicMail);
		}

	// create a new SMS header
	delete iSmsHeader;
	iSmsHeader = NULL;
	iSmsHeader = CSmsHeader::NewL(email1); 
	CleanupStack::Pop(email1);

	// Check that the header is correct...
	// Email fields...
	const CSmsEmailFields& fields10 = iSmsHeader->EmailFields();
	iSmsTest(fields10.HasAddress());
	iSmsTest(fields10.Addresses().MdcaCount()==1);
	iSmsTest(fields10.Addresses().MdcaPoint(0).Compare(KTestEmailAddress1) == 0);
	iSmsTest(fields10.Subject().Compare(KTestEmailSubject1) == 0);
	
	// Message body...
	CSmsBufferBase& body1 = iSmsHeader->Message().Buffer();
	TInt length1 = body1.Length();
	HBufC* buf1 = HBufC::NewLC(length1);
	TPtr bufPtr1(buf1->Des());
	body1.Extract(bufPtr1, 0, length1);
	
	iSmsTest(bufPtr1.Compare(KTestEmailBody1) == 0);
	
	CleanupStack::PopAndDestroy(buf1);

	// Test corrupt email.
	_LIT(KTestEmailBody2,		"user@domain1.domain2##a subjectsome body text");
	_LIT(KTestEmailMessage2,	"user@domain1.domain2##a subjectsome body text");

	CSmsMessage* email2 = CSmsMessage::NewL(fs, CSmsPDU::ESmsDeliver, CSmsEditorBuffer::NewL(*iRichText), EFalse);
	CleanupStack::PushL(email2);
	
	CSmsBufferBase& buffer2 = email2->Buffer();
	buffer2.Reset();
	buffer2.InsertL(0, KTestEmailMessage2());
	
	// Set the PID for email.
	CSmsPDU& pdu2 = email2->SmsPDU();
	if( pdu2.ProtocolIdentifierPresent() )
		{
		pdu2.SetPIDType(TSmsProtocolIdentifier::ESmsPIDTelematicInterworking);
		pdu2.SetTelematicDeviceIndicator(TSmsProtocolIdentifier::ESmsTelematicDevice);
		pdu2.SetTelematicDeviceType(TSmsProtocolIdentifier::ESmsInternetElectronicMail);
		}

	// create a new SMS header
	delete iSmsHeader;
	iSmsHeader = NULL;
	iSmsHeader = CSmsHeader::NewL(email2); 
	CleanupStack::Pop(email2);

	// Check that the header is correct...
	// Email fields...
	const CSmsEmailFields& fields11 = iSmsHeader->EmailFields();
	iSmsTest(!fields11.HasAddress());
	iSmsTest(fields11.Addresses().MdcaCount()==0);
	iSmsTest(fields11.Subject().Compare(KNullDesC) == 0);
	iSmsTest(fields11.Length() == 0);
	
	// Message body...
	CSmsBufferBase& body2 = iSmsHeader->Message().Buffer();
	TInt length2 = body2.Length();
	HBufC* buf2 = HBufC::NewLC(length2);
	TPtr bufPtr2(buf2->Des());
	body2.Extract(bufPtr2, 0, length2);
	
	iSmsTest(bufPtr2.Compare(KTestEmailBody2) == 0);
	
	CleanupStack::PopAndDestroy(buf2);
	
	CleanupStack::PopAndDestroy(&fs);

	Session().CleanupEntryPop();
	Session().RemoveEntry(id);
	}

_LIT(KGetDescriptionTestMessage, "Finland to win the ice hockey world champs");

#include <emsuserpromptie.h>
#include <emssoundie.h>
_LIT8(KUserMelody, 	"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
	  "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"); // Melody file

_LIT(KTestEmsVariousMsg1,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

void CSmutTest::TestSmsEmsExtensionsL()
// Test the modifications for EMS
	{
	// Set up a standard sms message first
	iRichText->Reset();
	iRichText->InsertL(0, KTestEmsVariousMsg1);
	// Make a Deliver so we can get at the protocol identifier
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsDeliver,*iRichText);
	CleanupStack::PushL(header);

	// Create 2 EMS Melody objects followed by a User Prompt Indicator

	// Create first Melody EMS object
	CEmsSoundIE* object = CEmsSoundIE::NewL(KUserMelody());
	CleanupStack::PushL(object);	
	// Insert the sound object after the 5th byte of the message
	object->SetStartPosition(5);
	// Add it to the CSmsMessage
	header->Message().AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);	

	// Create a second Melody EMS object 
	CEmsSoundIE* object2 = CEmsSoundIE::NewL(KUserMelody());
	CleanupStack::PushL(object2);	
	// Insert the sound object after the 5th byte of the message
	object2->SetStartPosition(5);
	// Add it to the CSmsMessage
	header->Message().AddEMSInformationElementL(*object2);
	CleanupStack::PopAndDestroy(object2);

	// Create a user prompt indicator and tell it there are 2 EMS objects
	CEmsUserPrompt* prompt = CEmsUserPrompt::NewL(2);
	CleanupStack::PushL(prompt);	
	// Add it after the 5th byte of the message
	prompt->SetStartPosition(5);
	// Add it to the CSmsMessage
	header->Message().AddEMSInformationElementL(*prompt);
	CleanupStack::PopAndDestroy(prompt);
 
	// Test the TMsvSmsEntry derived class
	TMsvSmsEntry entry;
	// Check the Existing sms message for User Prompt indicator
	const RPointerArray<const CEmsInformationElement>&  emsElements= header->Message().GetEMSInformationElementsL();
	// Loop through the array checking for a user prompt indicator element
	TInt i(0);
	for(;i<emsElements.Count();i++)
		{
		if(emsElements[i]->Identifier() == CSmsInformationElement::ESmsEnhancedUserPromptIndicator)
			{
			// Set user prompt indicator bit in iMtmData2
			entry.SetUserPromptIndicator(ETrue);
			break;
			}
		}
	// Check the user prompt indicator is set
	if(!entry.UserPromptIndicator())
		User::Leave(KErrCorrupt);
	// Check we can clear it
	entry.SetUserPromptIndicator(EFalse);
	if(entry.UserPromptIndicator())
		User::Leave(KErrCorrupt);

	// Convenient reference to PDU
	CSmsDeliver& pdu = STATIC_CAST(CSmsDeliver&,header->Message().SmsPDU());
	// Set up the Protocol Identifier to read back
	pdu.SetPIDType(TSmsProtocolIdentifier::ESmsPIDShortMessageType);
	pdu.SetShortMessageType(TSmsProtocolIdentifier::ESmsReplaceShortMessageType1);

	// Read from the pdu and set the TMsvSmsEntry copy
	entry.SetProtocolIdentifier(*pdu.ProtocolIdentifier());
	// Read it back and check the value
	if(entry.ProtocolIdentifier() != *pdu.ProtocolIdentifier())
		{
		User::Leave(KErrCorrupt);
		}

	CleanupStack::PopAndDestroy(); //header

	}

void CSmutTest::TestSmsUtilitiesL()
	{
	iSmsTest.Test().Next(_L("Testing TSmsUtilities"));

	iRichText->Reset();
	iRichText->InsertL(0, KGetDescriptionTestMessage);
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsSubmit,*iRichText);
	CleanupStack::PushL(header);

	TestSmsUtilities1L(*header);
	TestSmsUtilities2L(*header);
	TestSmsUtilities3L(*header);

	CleanupStack::PopAndDestroy(); //header

	TestSmsUtilitiesTimeStampsL();
	}

void CSmutTest::TestSmsUtilitiesBL()
	{
	iSmsTest.Test().Next(_L("Testing ECOM Plugin for SMS GetDetDesc interface"));

	iRichText->Reset();
	iRichText->InsertL(0, KGetDescriptionTestMessage);
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsSubmit,*iRichText);
	CleanupStack::PushL(header);

	TestSmsUtilities4L(*header);
	TestSmsUtilities5L(*header);
	TestSmsUtilities3L(*header);

	CleanupStack::PopAndDestroy(); //header
	}

void CSmutTest::TestSmsUtilities1L(CSmsHeader& aHeader)
/**
 * @test Tests TSmsUtilities::GetDescription()
 */
	{
	iSmsTest.Printf(_L("Testing TSmsUtilities::GetDescription() - use varying values to test trucation\n"));

	RResourceFile resFile;
	iSmsTest.OpenResourceFileL(KSmsResourceFile, resFile);
	CleanupClosePushL(resFile);

//Test GetDescription()

	TPtrC buf(KGetDescriptionTestMessage);
	const TInt maxLength = buf.Length();
	HBufC* desBuf = HBufC::NewLC(maxLength);
	TPtr desc = desBuf->Des();

	for( TInt length=1; length <= maxLength; ++length )
		{
		iSmsTest.Printf(_L("-> trucating to length %d\n"), length);

		desc.Zero();
		User::LeaveIfError(TSmsUtilities::GetDescription(aHeader.Message(), desc, length));

		iSmsTest.Printf(_L("-> GetDescription() : %S\n"), &desc);
		
		iSmsTest(desc.Length()<=length);
		iSmsTest(desc == buf.Left(desc.Length()) ); //this won't work if buf contains CEditableText::EParagraphDelimiter
		}
	desc.Zero();

//Test Special Message Indication

	CSmsPDU& pdu = aHeader.Message().SmsPDU();
	CSmsUserData& userData = pdu.UserData();
	TBuf8<2> msgInd;
	msgInd.Copy(_L("AA"));

	//Add information element
	msgInd[0] = TSmsUtilities::EEmailMessageWaiting;
	msgInd[1] = 0x00000001; //1 message

	iSmsTest.Printf(_L("\tAdding information element (Email) to UD\n"));
	userData.AddInformationElementL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication, msgInd);
	User::LeaveIfError(TSmsUtilities::GetDescription(aHeader.Message(), desc));
	CheckDescriptionL(resFile, R_MESSAGE_INDICATION_EMAIL_ONE, desc, 2);

	// Set the maximum length of desc to be less than the description.
	// The description should truncate to fit into desc.
	User::LeaveIfError(TSmsUtilities::GetDescription(aHeader.Message(), desc, 10));
	iSmsTest(desc.Length() == 10); //Check that the Length of desc is the specified max length.

	TestSpecialSmsMessageIndicationPduL(resFile, TSmsUtilities::EVoiceMessageWaiting, R_MESSAGE_INDICATION_VOICEMAIL_ONE);
	TestSpecialSmsMessageIndicationPduL(resFile, TSmsUtilities::EFaxMessageWaiting, R_MESSAGE_INDICATION_FAX_ONE);
	TestSpecialSmsMessageIndicationPduL(resFile, TSmsUtilities::EEmailMessageWaiting, R_MESSAGE_INDICATION_EMAIL_ONE);
	TestSpecialSmsMessageIndicationPduL(resFile, TSmsUtilities::EOtherMessageWaiting, R_MESSAGE_INDICATION_OTHER_ONE);

	CleanupStack::PopAndDestroy(2, &resFile);

	iSmsTest.Printf(_L("Passed\n"));
	}

CSmsMessage* CSmutTest::CreateSmsMessageLC(const TDesC8& aHexPdu) const
	{
	HBufC8* binaryPdu = ConvertToBinaryLC(aHexPdu);
	TGsmSms pdu;
	pdu.SetPdu(*binaryPdu);
	CleanupStack::PopAndDestroy(binaryPdu);
	CSmsMessage* message = CSmsMessage::NewL(iSmsTest.FileSession(), pdu, CSmsBuffer::NewL());
	CleanupStack::PushL(message);
	return message;
	}

HBufC8* CSmutTest::ConvertToBinaryLC(const TDesC8& aHex) const
	{
	const TInt len = aHex.Length();
	if (len % 2 != 0)
		User::Leave(KErrArgument);

	HBufC8* pdu = HBufC8::NewLC(len / 2);
	TPtr8 ptr(pdu->Des());

	for (TInt i = 0; i < len; i += 2)
		{
		TLex8 lex(aHex.Mid(i, 2));
		TUint8 val;
		User::LeaveIfError(lex.Val(val, EHex));
		ptr.Append(val);
		}

	return pdu;
	}

_LIT(KGetDetailsResult, "\t\tNum In: \"%S\" Name Out: \"%S\"\n");

void CSmutTest::TestSmsUtilities2L(CSmsHeader& aHeader)
	{
//Test GetDetails

	iSmsTest.Printf(_L("Test TSmsUtilities::GetDetails()\n"));

	_LIT(KLongTel, "+378458932945284754456302945754938539");

	_LIT(KMikkoTel, "+358503367709");
	_LIT(KMikkoFirst, "Mikko");
	_LIT(KMikkoLast, "Rintala");
	_LIT(KMikkoEmail, "mikko@nokia.com");

	_LIT(KUnqualifiedTel, "8503367710");
	_LIT(KUnqualifiedTel1, "+358503367710");
	_LIT(KUnqualifiedFirst, "Unq");
	_LIT(KUnqualifiedLast, "Wallified");
	_LIT(KUnqualifiedEmail, "unqalified@diysurgery.com");

//	_LIT(KAnthonyTel, " + 4 "); //is valid
	_LIT(KAnthonyTel, "+ 7 "); //is valid
	_LIT(KAnthonyFirst, "Anthony");
	_LIT(KAnthonyLast, "Alexander");
	_LIT(KAnthonyEmail, "test@symbian.com");

	_LIT(KHarriTel, "+8"); //is valid
	_LIT(KHarriFirst, "HarriHarriHarriHarriHarriHarriHarri");
	_LIT(KHarriLast, "RäisänenRäisänenRäisänenRäisänenRäisänen");
	_LIT(KHarriEmail, "harri@nokia.com");

	_LIT(KDupTel, "+44207435676");
	_LIT(KDupFirst1, "Keval");
	_LIT(KDupLast1, "Pindoria");
	_LIT(KDupFirst2, "Jackie");
	_LIT(KDupLast2, "Chan");
	
	_LIT(KBadTel1, "This is a bad Telephone");
	_LIT(KBadTel2, " 5  ");
	_LIT(KBadTel3, "5+");
	_LIT(KBadTel4, "*5565434556");

	_LIT(KBadFirst, "BadFirstName");
	_LIT(KBadLast, "BadLastName");

	_LIT(KNoNameTel, "+449005434");

	_LIT(KDummy, "+44444");

	TRAPD(err, iSmsTest.DeleteContactsFileL());
	iSmsTest(err == KErrNotFound || err == KErrNone);

	iSmsTest.Printf(_L("\tTesting without contacts\n"));

	//Simple test
	TPtrC testTel(KMikkoTel);
	TBuf<KSmsDetailsLength> details;

	aHeader.Message().SetToFromAddressL(testTel);
	TSmsUtilities::GetDetails(iSmsTest.FileSession(), aHeader.Message(), details);
	iSmsTest.Printf(KGetDetailsResult, &testTel, &details);
	iSmsTest(testTel.FindF(details) != KErrNotFound); //details should contain testTel as there is no contact.

	TestInvalidContact(testTel, details);

	testTel.Set(KAnthonyTel);
	aHeader.Message().SetToFromAddressL(testTel);
	err = TSmsUtilities::GetDetails(iSmsTest.FileSession(), aHeader.Message(), details);
	iSmsTest.Printf(KGetDetailsResult, &testTel, &details);
	iSmsTest(testTel.FindF(details) != KErrNotFound); //details should contain testTel as there is no contact.
	iSmsTest(err == KErrNone);

	TestInvalidContact(testTel, details);

	// Testing with long telephone number. 
	// The telephone number should truncate to fit into details.
	TestInvalidContact(KLongTel, details);
	iSmsTest(details.Length() == KSmsDetailsLength); //Length of details should be the default max length.

	//Add contacts

	iSmsTest.Printf(_L("\tAdding Contacts\n"));

		//Adding good telephone numbers

		TContactItemId contId = iSmsTest.CreateContactL(KAnthonyFirst, KAnthonyLast, KAnthonyTel, KAnthonyEmail);
		iSmsTest(contId != KNullContactId);

		contId = iSmsTest.CreateContactL(KMikkoFirst, KMikkoLast, KMikkoTel, KMikkoEmail);
		iSmsTest(contId != KNullContactId);

		contId = iSmsTest.CreateContactL(KUnqualifiedFirst, KUnqualifiedLast, KUnqualifiedTel, KUnqualifiedEmail);
		iSmsTest(contId != KNullContactId);

		contId = iSmsTest.CreateContactL(KHarriFirst, KHarriLast, KHarriTel, KHarriEmail);
		iSmsTest(contId != KNullContactId);

		//Adding a duplicate telephone

		contId = iSmsTest.CreateContactL(KDupFirst1, KDupLast1, KDupTel, KNullDesC);
		iSmsTest(contId != KNullContactId);

		contId = iSmsTest.CreateContactL(KDupFirst2, KDupLast2, KDupTel, KNullDesC);
		iSmsTest(contId != KNullContactId);

		//Adding bad telephone numbers

		contId = iSmsTest.CreateContactL(KBadFirst, KBadLast, KBadTel1, KNullDesC);
		iSmsTest(contId != KNullContactId);

		contId = iSmsTest.CreateContactL(KBadFirst, KBadLast, KBadTel2, KNullDesC);
		iSmsTest(contId != KNullContactId);

		contId = iSmsTest.CreateContactL(KBadFirst, KBadLast, KBadTel3, KNullDesC);
		iSmsTest(contId != KNullContactId);

		contId = iSmsTest.CreateContactL(KBadFirst, KBadLast, KBadTel4, KNullDesC);
		iSmsTest(contId != KNullContactId);

		//Adding no name contact

		contId = iSmsTest.CreateContactL(KNullDesC, KNullDesC, KNoNameTel, KNullDesC);
		iSmsTest(contId != KNullContactId);

	iSmsTest.Printf(_L("\tTesting with valid telephone numbers\n"));

		TestValidTel(KAnthonyTel, KAnthonyFirst, KAnthonyLast, KNullDesC, details);
		TestValidTel(KMikkoTel, KMikkoFirst, KMikkoLast, KNullDesC, details);
		// Test below was added for INC040050. The number of digits matched
		// against in GetDetails changed and resulted in undesirable behaviour
		// when searching for an unqualified number with a fully qualified number.
		TestValidTel(KUnqualifiedTel1, KUnqualifiedFirst, KUnqualifiedLast, KNullDesC, details);
		TestValidTel(KHarriTel, _L("Räisänen"), KNullDesC, _L("Harri"), details);

	iSmsTest.Printf(_L("\tTesting with invalid telephone numbers\n"));

		TestInvalidTelephone(KBadTel1, details);
		TestInvalidTelephone(KBadTel2, details);
		TestInvalidTelephone(KBadTel3, details);
		TestInvalidTelephone(KBadTel4, details);

	iSmsTest.Printf(_L("\tTesting with duplicate telephone numbers\n"));

		TestDuplicateContact(KDupTel, details);

	iSmsTest.Printf(_L("\tTesting No Names telephone number\n"));

		TestInvalidContact(KNoNameTel, details);
		TestInvalidContact(KDummy, details);

	iSmsTest.Printf(_L("Passed\n"));
	}

void CSmutTest::TestValidTel(const TDesC& aTel, const TDesC& aExpect1, const TDesC& aExpect2, const TDesC& aNotExpect, TDes& rDetails)
	{
	TInt err = TSmsUtilities::GetDetails(iSmsTest.FileSession(), aTel, rDetails);
	iSmsTest.Printf(KGetDetailsResult, &aTel, &rDetails);

	if (aExpect1.Length())
		iSmsTest(rDetails.FindF(aExpect1) != KErrNotFound);

	if (aExpect2.Length())
		iSmsTest(rDetails.FindF(aExpect2) != KErrNotFound);

	if (aNotExpect.Length())
		iSmsTest(rDetails.FindF(aNotExpect) == KErrNotFound);

	iSmsTest(err == KErrNone);
	}

void CSmutTest::TestInvalidTelephone(const TDesC& aTel, TDes& aDetails)
	{
	TInt err = TSmsUtilities::GetDetails(iSmsTest.FileSession(), aTel, aDetails);
	iSmsTest.Printf(KGetDetailsResult, &aTel, &aDetails);
	iSmsTest(aTel.FindF(aDetails) != KErrNotFound);	//aDetails should contain aTel as aTel is invalid.
	iSmsTest(err == KErrNone);
	}	

void CSmutTest::TestInvalidContact(const TDesC& aTel, TDes& aDetails)
	{
	TInt err = TSmsUtilities::GetDetails(iSmsTest.FileSession(), aTel, aDetails);
	iSmsTest.Printf(KGetDetailsResult, &aTel, &aDetails);
	iSmsTest(aTel.FindF(aDetails) != KErrNotFound); //aDetails should contain aTel as the contact is invalid.
	iSmsTest(err == KErrNone);
	}

void CSmutTest::TestDuplicateContact(const TDesC& aTel, TDes& aDetails)
	{
	TInt err = TSmsUtilities::GetDetails(iSmsTest.FileSession(), aTel, aDetails);
	iSmsTest.Printf(KGetDetailsResult, &aTel, &aDetails);
	iSmsTest(aTel.FindF(aDetails) != KErrNotFound); //aDetails should contain aTel as there is duplicate contact.
	iSmsTest(err == KErrNone);
	}

void CSmutTest::TestSmsUtilities3L(CSmsHeader& aHeader)
	{
	iSmsTest.Printf(_L("Test TSmsUtilities::ServiceIdL()\n"));

	TMsvId id2 = KMsvNullIndexEntryId;
// First tested function is ServiceIdL()
	TSmsUtilities::ServiceIdL(*iSmsTest.iMsvEntry, id2);
	iSmsTest(id2 == iSmsTest.iSmsServiceId);
	iSmsTest.Printf(_L("\tPassed\n"));

	iSmsTest.Printf(_L("Test TSmsUtilities::PopulateMsgEntry()\n"));
	TMsvEntry entry;
	TSmsUtilities::PopulateMsgEntry(entry,aHeader.Message(), iSmsTest.iSmsServiceId);
// todo More tests
	iSmsTest.Printf(_L("\tPassed\n"));
	}

void CSmutTest::TestSmsUtilitiesTimeStampsL()
	{
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsDeliver,*iRichText);
	CleanupStack::PushL(header);

	header->Message().SetTime(1);
	CSmsDeliver& deliver = header->Deliver();
	deliver.SetServiceCenterTimeStamp(2);

	CSmsSettings* settings = CSmsSettings::NewL();
	CleanupStack::PushL(settings);
	RestoreSettingsL(*settings);

	// Default settings should be to use the time that the SMS message time (the time
	// that the SMS arrives at the phone) as opposed to the time it arrives at the
	// service centre.

	iSmsTest.Printf(_L("Test PopulateMsgEntry() with settings to use SMS message time\n"));

	TMsvEntry entry;
	TSmsUtilities::PopulateMsgEntry(entry, header->Message(), iSmsTest.iSmsServiceId, *settings);
	iSmsTest(entry.iDate == 1);
	iSmsTest.Printf(_L("\tPassed\n"));

	// Now test that we use the service centre timestamp if the SMS setting flag
	// requires us to do so.
	iSmsTest.Printf(_L("Test PopulateMsgEntry() with settings to use service center timestamp\n"));

	settings->SetUseServiceCenterTimeStampForDate(ETrue);

	TSmsUtilities::PopulateMsgEntry(entry, header->Message(), iSmsTest.iSmsServiceId, *settings);
	iSmsTest(entry.iDate == 2);
	iSmsTest.Printf(_L("\tPassed\n"));
	
	CleanupStack::PopAndDestroy(2, header);
	}

void CSmutTest::TestSmsUtilities4L(CSmsHeader& aHeader)
/**
 * @test Tests CSmsGetDetDescInterface::GetDescription() default implementation
 *
 * This is a modified version of TestSmsUtilities1L(CSmsHeader& aHeader), modified to use the 
 * CSmsGetDetDescInterface ECOM plugin interface instead of the TSmsUtilies
 * interface for ::GetDescription
 */
	{
	iSmsTest.Printf(_L("Testing CSmsGetDetDescInterface::GetDescription() - use varying values to test trucation\n"));

	RResourceFile resFile;
	iSmsTest.OpenResourceFileL(KSmsResourceFile, resFile);
	CleanupClosePushL(resFile);
	
//Test GetDescription()

	TPtrC buf(KGetDescriptionTestMessage);
	const TInt maxLength = buf.Length();
	HBufC* desBuf = HBufC::NewLC(maxLength); // on the CleanupStack
	TPtr desc = desBuf->Des();

	// load ECOM interface used to create SMS details and description values.
	iSmsTest.Printf(_L("Loading ECOM Plugin...\n"));
	CSmsGetDetDescInterface* getDetDesc = CSmsGetDetDescInterface::NewL();
	CleanupStack::PushL(getDetDesc);
	iSmsTest.Printf(_L("... Plugin loaded\n"));
	
	for( TInt length=1; length <= maxLength; ++length )
		{
		iSmsTest.Printf(_L("-> trucating to length %d\n"), length);

		desc.Zero();
		User::LeaveIfError(getDetDesc->GetDescription(aHeader.Message(), desc, length));

		iSmsTest.Printf(_L("-> GetDescription() : %S\n"), &desc);
		
		iSmsTest(desc.Length()<=length);
		iSmsTest(desc == buf.Left(desc.Length()) ); //this won't work if buf contains CEditableText::EParagraphDelimiter
		}
	desc.Zero();

//Test Special Message Indication

	CSmsPDU& pdu = aHeader.Message().SmsPDU();
	CSmsUserData& userData = pdu.UserData();
	TBuf8<2> msgInd;
	msgInd.Copy(_L("AA"));

	//Add information element
	msgInd[0] = TSmsUtilities::EEmailMessageWaiting;
	msgInd[1] = 0x00000001; //1 message

	iSmsTest.Printf(_L("\tAdding information element (Email) to UD\n"));
	userData.AddInformationElementL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication, msgInd);
	User::LeaveIfError(getDetDesc->GetDescription(aHeader.Message(), desc));
	CheckDescriptionL(resFile, R_MESSAGE_INDICATION_EMAIL_ONE, desc, 2);

	// Set the maximum length of desc to be less than the description.
	// The description should truncate to fit into desc.
	User::LeaveIfError(getDetDesc->GetDescription(aHeader.Message(), desc, 10));
	iSmsTest(desc.Length() == 10); //Check that the Length of desc is the specified max length.

	TestSpecialSmsMessageIndicationPduL(resFile, TSmsUtilities::EVoiceMessageWaiting, R_MESSAGE_INDICATION_VOICEMAIL_ONE);
	TestSpecialSmsMessageIndicationPduL(resFile, TSmsUtilities::EFaxMessageWaiting, R_MESSAGE_INDICATION_FAX_ONE);
	TestSpecialSmsMessageIndicationPduL(resFile, TSmsUtilities::EEmailMessageWaiting, R_MESSAGE_INDICATION_EMAIL_ONE);
	TestSpecialSmsMessageIndicationPduL(resFile, TSmsUtilities::EOtherMessageWaiting, R_MESSAGE_INDICATION_OTHER_ONE);
	
	CleanupStack::PopAndDestroy(getDetDesc);
	REComSession::FinalClose();

	CleanupStack::PopAndDestroy(2, &resFile);

	iSmsTest.Printf(_L("Passed\n"));
	}

void CSmutTest::TestSmsUtilities5L(CSmsHeader& aHeader)
/**
 * @test Tests CSmsGetDetDescInterface::GetDetails() default implementation
 *
 * This is a modified version of TestSmsUtilities2L(CSmsHeader& aHeader), modified to use the 
 * CSmsGetDetDescInterface ECOM plugin interface instead of the TSmsUtilies
 * interface for ::GetDetails
 */
	{
//Test GetDetails

	iSmsTest.Printf(_L("Test CSmsGetDetDescInterface::GetDetails()\n"));

	_LIT(KLongTel, "+378458932945284754456302945754938539");

	_LIT(KMikkoTel, "+358503367709");
	_LIT(KMikkoFirst, "Mikko");
	_LIT(KMikkoLast, "Rintala");
	_LIT(KMikkoEmail, "mikko@nokia.com");

	_LIT(KUnqualifiedTel, "8503367710");
	_LIT(KUnqualifiedTel1, "+358503367710");
	_LIT(KUnqualifiedFirst, "Unq");
	_LIT(KUnqualifiedLast, "Wallified");
	_LIT(KUnqualifiedEmail, "unqalified@diysurgery.com");

	_LIT(KAnthonyTel, "+ 7 "); //is valid
	_LIT(KAnthonyFirst, "Anthony");
	_LIT(KAnthonyLast, "Alexander");
	_LIT(KAnthonyEmail, "test@symbian.com");

	_LIT(KHarriTel, "+8"); //is valid
	_LIT(KHarriFirst, "HarriHarriHarriHarriHarriHarriHarri");
	_LIT(KHarriLast, "RäisänenRäisänenRäisänenRäisänenRäisänen");
	_LIT(KHarriEmail, "harri@nokia.com");

	_LIT(KDupTel, "+44207435676");
	_LIT(KDupFirst1, "Keval");
	_LIT(KDupLast1, "Pindoria");
	_LIT(KDupFirst2, "Jackie");
	_LIT(KDupLast2, "Chan");
	
	_LIT(KBadTel1, "This is a bad Telephone");
	_LIT(KBadTel2, " 5  ");
	_LIT(KBadTel3, "5+");
	_LIT(KBadTel4, "*5565434556");

	_LIT(KBadFirst, "BadFirstName");
	_LIT(KBadLast, "BadLastName");

	_LIT(KNoNameTel, "+449005434");

	_LIT(KDummy, "+44444");
	
	// load ECOM interface used to create SMS details and description values.
	iSmsTest.Printf(_L("Loading ECOM Plugin...\n"));
	CSmsGetDetDescInterface* getDetDesc = CSmsGetDetDescInterface::NewL();
	CleanupStack::PushL(getDetDesc);
	iSmsTest.Printf(_L("... Plugin loaded\n"));


	TRAPD(err, iSmsTest.DeleteContactsFileL());
	iSmsTest(err == KErrNotFound || err == KErrNone);

	iSmsTest.Printf(_L("\tTesting without contacts\n"));

	//Simple test
	TPtrC testTel(KMikkoTel);
	TBuf<KSmsDetailsLength> details;

	aHeader.Message().SetToFromAddressL(testTel);
	getDetDesc->GetDetails(iSmsTest.FileSession(), aHeader.Message(), details);
	iSmsTest.Printf(KGetDetailsResult, &testTel, &details);
	iSmsTest(testTel.FindF(details) != KErrNotFound); //details should contain testTel as there is no contact.

	TestInvalidContact(testTel, details);

	testTel.Set(KAnthonyTel);
	aHeader.Message().SetToFromAddressL(testTel);
	err = getDetDesc->GetDetails(iSmsTest.FileSession(), aHeader.Message(), details);
	iSmsTest.Printf(KGetDetailsResult, &testTel, &details);
	iSmsTest(testTel.FindF(details) != KErrNotFound); //details should contain testTel as there is no contact.
	iSmsTest(err == KErrNone);

	TestInvalidContact(testTel, details);

	// Testing with long telephone number. 
	// The telephone number should truncate to fit into details.
	TestInvalidContact(KLongTel, details);
	iSmsTest(details.Length() == KSmsDetailsLength); //Length of details should be the default max length.

	//Add contacts

	iSmsTest.Printf(_L("\tAdding Contacts\n"));

		//Adding good telephone numbers

		TContactItemId contId = iSmsTest.CreateContactL(KAnthonyFirst, KAnthonyLast, KAnthonyTel, KAnthonyEmail);
		iSmsTest(contId != KNullContactId);

		contId = iSmsTest.CreateContactL(KMikkoFirst, KMikkoLast, KMikkoTel, KMikkoEmail);
		iSmsTest(contId != KNullContactId);

		contId = iSmsTest.CreateContactL(KUnqualifiedFirst, KUnqualifiedLast, KUnqualifiedTel, KUnqualifiedEmail);
		iSmsTest(contId != KNullContactId);

		contId = iSmsTest.CreateContactL(KHarriFirst, KHarriLast, KHarriTel, KHarriEmail);
		iSmsTest(contId != KNullContactId);

		//Adding a duplicate telephone

		contId = iSmsTest.CreateContactL(KDupFirst1, KDupLast1, KDupTel, KNullDesC);
		iSmsTest(contId != KNullContactId);

		contId = iSmsTest.CreateContactL(KDupFirst2, KDupLast2, KDupTel, KNullDesC);
		iSmsTest(contId != KNullContactId);

		//Adding bad telephone numbers

		contId = iSmsTest.CreateContactL(KBadFirst, KBadLast, KBadTel1, KNullDesC);
		iSmsTest(contId != KNullContactId);

		contId = iSmsTest.CreateContactL(KBadFirst, KBadLast, KBadTel2, KNullDesC);
		iSmsTest(contId != KNullContactId);

		contId = iSmsTest.CreateContactL(KBadFirst, KBadLast, KBadTel3, KNullDesC);
		iSmsTest(contId != KNullContactId);

		contId = iSmsTest.CreateContactL(KBadFirst, KBadLast, KBadTel4, KNullDesC);
		iSmsTest(contId != KNullContactId);

		//Adding no name contact

		contId = iSmsTest.CreateContactL(KNullDesC, KNullDesC, KNoNameTel, KNullDesC);
		iSmsTest(contId != KNullContactId);

	iSmsTest.Printf(_L("\tTesting with valid telephone numbers\n"));

		TestValidTel(*getDetDesc, KAnthonyTel, KAnthonyFirst, KAnthonyLast, KNullDesC, details);
		TestValidTel(*getDetDesc, KMikkoTel, KMikkoFirst, KMikkoLast, KNullDesC, details);
		// Test below was added for INC040050. The number of digits matched
		// against in GetDetails changed and resulted in undesirable behaviour
		// when searching for an unqualified number with a fully qualified number.
		TestValidTel(*getDetDesc, KUnqualifiedTel1, KUnqualifiedFirst, KUnqualifiedLast, KNullDesC, details);
		TestValidTel(*getDetDesc, KHarriTel, _L("Räisänen"), KNullDesC, _L("Harri"), details);

	iSmsTest.Printf(_L("\tTesting with invalid telephone numbers\n"));

		TestInvalidTelephone(*getDetDesc, KBadTel1, details);
		TestInvalidTelephone(*getDetDesc, KBadTel2, details);
		TestInvalidTelephone(*getDetDesc, KBadTel3, details);
		TestInvalidTelephone(*getDetDesc, KBadTel4, details);

	iSmsTest.Printf(_L("\tTesting with duplicate telephone numbers\n"));

		TestDuplicateContact(*getDetDesc, KDupTel, details);

	iSmsTest.Printf(_L("\tTesting No Names telephone number\n"));

		TestInvalidContact(*getDetDesc, KNoNameTel, details);
		TestInvalidContact(*getDetDesc, KDummy, details);
	
	CleanupStack::PopAndDestroy(getDetDesc);
	REComSession::FinalClose();

	iSmsTest.Printf(_L("Passed\n"));
	}

void CSmutTest::TestValidTel(CSmsGetDetDescInterface& aGetDetDesc, const TDesC& aTel, const TDesC& aExpect1, const TDesC& aExpect2, const TDesC& aNotExpect, TDes& rDetails)
	{
	TInt err = aGetDetDesc.GetDetails(iSmsTest.FileSession(), aTel, rDetails);
	iSmsTest.Printf(KGetDetailsResult, &aTel, &rDetails);

	if (aExpect1.Length())
		iSmsTest(rDetails.FindF(aExpect1) != KErrNotFound);

	if (aExpect2.Length())
		iSmsTest(rDetails.FindF(aExpect2) != KErrNotFound);

	if (aNotExpect.Length())
		iSmsTest(rDetails.FindF(aNotExpect) == KErrNotFound);

	iSmsTest(err == KErrNone);
	}

void CSmutTest::TestInvalidTelephone(CSmsGetDetDescInterface& aGetDetDesc, const TDesC& aTel, TDes& aDetails)
	{
	TInt err = aGetDetDesc.GetDetails(iSmsTest.FileSession(), aTel, aDetails);
	iSmsTest.Printf(KGetDetailsResult, &aTel, &aDetails);
	iSmsTest(aTel.FindF(aDetails) != KErrNotFound);	//aDetails should contain aTel as aTel is invalid.
	iSmsTest(err == KErrNone);
	}	

void CSmutTest::TestInvalidContact(CSmsGetDetDescInterface& aGetDetDesc, const TDesC& aTel, TDes& aDetails)
	{
	TInt err = aGetDetDesc.GetDetails(iSmsTest.FileSession(), aTel, aDetails);
	iSmsTest.Printf(KGetDetailsResult, &aTel, &aDetails);
	iSmsTest(aTel.FindF(aDetails) != KErrNotFound); //aDetails should contain aTel as the contact is invalid.
	iSmsTest(err == KErrNone);
	}

void CSmutTest::TestDuplicateContact(CSmsGetDetDescInterface& aGetDetDesc, const TDesC& aTel, TDes& aDetails)
	{
	TInt err = aGetDetDesc.GetDetails(iSmsTest.FileSession(), aTel, aDetails);
	iSmsTest.Printf(KGetDetailsResult, &aTel, &aDetails);
	iSmsTest(aTel.FindF(aDetails) != KErrNotFound); //aDetails should contain aTel as there is duplicate contact.
	iSmsTest(err == KErrNone);
	}

void CSmutTest::TestSpecialSmsMessageIndicationPduL(RResourceFile& aResFile, TSmsUtilities::TSmsUtilitiesSpecialMessageType aMessageType, TInt aId)
	{
	iSmsTest.Printf(_L("\tTestSpecialSmsMessageIndicationPduL Type=%d..."), aMessageType);

	const TUint8 KMaxCount = 0xFF;
	for (TUint8 i = 0; i < KMaxCount; i++)
		{
		TInt resId = aId;
		if (i != 1)
			resId++;

		TestSpecialSmsMessageIndicationPduL(aResFile, STATIC_CAST(TUint8, aMessageType), i, resId);
		TestSpecialSmsMessageIndicationPduL(aResFile, STATIC_CAST(TUint8, aMessageType | 0x80), i, resId);
		}

	iSmsTest.Printf(_L("Passed\n"));
	}

void CSmutTest::TestSpecialSmsMessageIndicationPduL(RResourceFile& aResFile, TUint8 aMessageType, TUint8 aMessageCount, TInt aId)
	{
	_LIT8(KFaxIndicationPdu, "440B916407586704F900002010011153710406040102810200");
	const TInt KIndicationPosition = 44;
	TBuf<KSmsDescriptionLength> desc;

	//Replace the Message Indication in this PDU
	RMobileSmsMessaging::TMobileSmsGsmTpdu miPdu(KFaxIndicationPdu);
	_LIT8(KFormat, "%02X%02X");
	TPtr8 ptr(&miPdu[KIndicationPosition], 0, 4);
	ptr.AppendFormat(KFormat, aMessageType, aMessageCount);


	CSmsMessage* msg = CreateSmsMessageLC(miPdu);
	User::LeaveIfError(TSmsUtilities::GetDescription(*msg, desc));
	CleanupStack::PopAndDestroy(msg);

	if (aMessageCount != 1)
		CheckDescriptionL(aResFile, aId, desc, aMessageCount);
	else
		CheckDescriptionL(aResFile, aId, desc);
	}

void CSmutTest::CheckDescriptionL(RResourceFile& aResFile, TInt aId, TRefByValue<const TDesC> aDesc, ...)
	{
	const TDesC& ref = aDesc;

	TBuf<256> resDesc;
	TBuf<256> resString;

	VA_LIST list;
	VA_START(list, aDesc);

	iSmsTest.ReadResourceStringL(aResFile, aId, resString);
	resDesc.AppendFormatList(resString, list);

	if (ref != resDesc)
		{
		iSmsTest.Printf(_L("\tMISMATCH Desc \"%S\" Resource \"%S\"\n"), &ref, &resDesc);
		iSmsTest(EFalse);
		}
	}

TBool CSmutTest::CheckSmsEntry(
							const TMsvSmsEntry& aEntry, 
							TBool aClassExpected, 
							TSmsDataCodingScheme::TSmsClass aClass, 
							TUint8 aPID, 
							TBool aUPI, 
							TMsvSmsEntry::TMsvSmsEntryAckSummary aDeliverySummary, 
							TBool aValidMessageId, 
							TInt32 aMessageId) const
	{
	TCompare retVal(ETrue);

	TSmsDataCodingScheme::TSmsClass dummyClass;

	retVal((aEntry.Class(dummyClass) && aClassExpected) || (!aEntry.Class(dummyClass) && !aClassExpected));

	if (aClassExpected)
		retVal(dummyClass == aClass);

	retVal(aEntry.ProtocolIdentifier() == aPID);
	retVal((aEntry.UserPromptIndicator() && aUPI) || (!aEntry.UserPromptIndicator() && !aUPI));

	retVal(aEntry.AckSummary(ESmsAckTypeDelivery) == aDeliverySummary);

	TInt32 dummyMessageId;

	retVal( (aEntry.MessageId(dummyMessageId) && aValidMessageId) || (!aEntry.MessageId(dummyMessageId) && !aValidMessageId) );
	if( aValidMessageId )
		retVal( dummyMessageId == aMessageId );

	return retVal;
	}

void CSmutTest::TestSmsEntryL()
	{
	iSmsTest.Test().Next(_L("Testing TMsvSmsEntry"));

	TMsvSmsEntry entry;
	iSmsTest(CheckSmsEntry(entry, EFalse, TSmsDataCodingScheme::ESmsClass0, 0, EFalse, TMsvSmsEntry::ENoAckSummary, EFalse, 0));

	entry.SetUserPromptIndicator(ETrue);
	iSmsTest(CheckSmsEntry(entry, EFalse, TSmsDataCodingScheme::ESmsClass0, 0, ETrue, TMsvSmsEntry::ENoAckSummary, EFalse, 0));

	entry.SetUserPromptIndicator(EFalse);
	iSmsTest(CheckSmsEntry(entry, EFalse, TSmsDataCodingScheme::ESmsClass0, 0, EFalse, TMsvSmsEntry::ENoAckSummary, EFalse, 0));

	entry.SetProtocolIdentifier(123);
	iSmsTest(CheckSmsEntry(entry, EFalse, TSmsDataCodingScheme::ESmsClass0, 123, EFalse, TMsvSmsEntry::ENoAckSummary, EFalse, 0));

	entry.SetProtocolIdentifier(100);
	entry.SetUserPromptIndicator(ETrue);
	iSmsTest(CheckSmsEntry(entry, EFalse, TSmsDataCodingScheme::ESmsClass0, 100, ETrue, TMsvSmsEntry::ENoAckSummary, EFalse, 0));

	entry.SetClass(ETrue, TSmsDataCodingScheme::ESmsClass0);
	iSmsTest(CheckSmsEntry(entry, ETrue, TSmsDataCodingScheme::ESmsClass0, 100, ETrue, TMsvSmsEntry::ENoAckSummary, EFalse, 0));

	entry.SetClass(ETrue, TSmsDataCodingScheme::ESmsClass1);
	iSmsTest(CheckSmsEntry(entry, ETrue, TSmsDataCodingScheme::ESmsClass1, 100, ETrue, TMsvSmsEntry::ENoAckSummary, EFalse, 0));

	entry.SetClass(ETrue, TSmsDataCodingScheme::ESmsClass2);
	iSmsTest(CheckSmsEntry(entry, ETrue, TSmsDataCodingScheme::ESmsClass2, 100, ETrue, TMsvSmsEntry::ENoAckSummary, EFalse, 0));
	
	entry.SetClass(ETrue, TSmsDataCodingScheme::ESmsClass3);
	iSmsTest(CheckSmsEntry(entry, ETrue, TSmsDataCodingScheme::ESmsClass3, 100, ETrue, TMsvSmsEntry::ENoAckSummary, EFalse, 0));

	entry.SetClass(EFalse);
	iSmsTest(CheckSmsEntry(entry, EFalse, TSmsDataCodingScheme::ESmsClass3, 100, ETrue, TMsvSmsEntry::ENoAckSummary, EFalse, 0));

	entry.SetUserPromptIndicator(EFalse);
	iSmsTest(CheckSmsEntry(entry, EFalse, TSmsDataCodingScheme::ESmsClass3, 100, EFalse, TMsvSmsEntry::ENoAckSummary, EFalse, 0));

	entry.SetClass(ETrue, TSmsDataCodingScheme::ESmsClass0);
	
	TInt classCounter, upiCounter, classDef, deliverySummary, messageIdValid;
	TSmsDataCodingScheme::TSmsClass tempClass;
	TMsvSmsEntry::TMsvSmsEntryAckSummary tempDelivery;
	TUint8 pidCounter;
	TInt32 messageId = 123456789;

	for (classCounter = 0; classCounter < 4; classCounter++)
		{
		for (pidCounter = 0; pidCounter < 128; pidCounter++)
			{
			for (upiCounter = 0; upiCounter < 2; upiCounter++)
				{
				for (classDef = 0; classDef < 2; classDef++)
					{
					for( deliverySummary = 0; deliverySummary < 5; ++deliverySummary )
						{
						for( messageIdValid = 0; messageIdValid < 2; ++messageIdValid)
							{
							entry = TMsvSmsEntry();
							tempClass = static_cast<TSmsDataCodingScheme::TSmsClass>(classCounter);
							tempDelivery = static_cast<TMsvSmsEntry::TMsvSmsEntryAckSummary>(deliverySummary);
							entry.SetClass(classDef, tempClass);
							entry.SetProtocolIdentifier(pidCounter);
							entry.SetUserPromptIndicator(upiCounter);
							entry.SetAckSummary(ESmsAckTypeDelivery, tempDelivery);
							entry.SetMessageId(messageId, messageIdValid);
							iSmsTest(CheckSmsEntry(entry, classDef, tempClass, pidCounter, upiCounter, tempDelivery, messageIdValid, messageId));
							}
						}
					}
				}
			}
		}
	
	for (classCounter = 0; classCounter < 4; classCounter++)
		{
		for (pidCounter = 0; pidCounter < 128; pidCounter++)
			{
			for (upiCounter = 0; upiCounter < 2; upiCounter++)
				{
				for (classDef = 0; classDef < 2; classDef++)
					{
					for( deliverySummary = 0; deliverySummary < 5; ++deliverySummary )
						{
						for( messageIdValid = 0; messageIdValid < 2; ++messageIdValid)
							{
							tempClass = static_cast<TSmsDataCodingScheme::TSmsClass>(classCounter);
							tempDelivery = static_cast<TMsvSmsEntry::TMsvSmsEntryAckSummary>(deliverySummary);
							entry.SetClass(classDef, tempClass);
							entry.SetProtocolIdentifier(pidCounter);
							entry.SetUserPromptIndicator(upiCounter);
							entry.SetAckSummary(ESmsAckTypeDelivery, tempDelivery);
							entry.SetMessageId(messageId, messageIdValid);
							iSmsTest(CheckSmsEntry(entry, classDef, tempClass, pidCounter, upiCounter, tempDelivery, messageIdValid, messageId));
							}
						}
					}
				}
			}
		}
	}
