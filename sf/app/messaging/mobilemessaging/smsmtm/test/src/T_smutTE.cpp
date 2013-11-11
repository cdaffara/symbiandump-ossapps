// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <smss.rsg>
#include <csmsaccount.h>
#include <csmsemailfields.h>

#include "T_smutTE.h"
#include <gsmuieoperations.h>

const TUid KUidMsvSmsEmailFieldsStream	= {0x10204C9D};
const TInt KLogID1 = 123456789;
const TInt KLogID2 = 987654321;
const TInt KLogID3 = 118888;

CSmutTest::CSmutTest(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest,CTestStep& aTestStep)
: CSmsTestBase(aSmsTest, aScriptFile, aNextTest), iTestStep(aTestStep)
	{
	}

void CSmutTest::RunAutoL()
	{
	}
	
CSmutTest::~CSmutTest()
	{
	delete iSmsHeader;
	delete iRichText;
	delete iParaLayer;
	delete iCharLayer;
	iDBSession->Close();
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

CSmutTest* CSmutTest::NewLC(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest,CTestStep& aTestStep)
	{
	CSmutTest* self = new (ELeave) CSmutTest(aSmsTest, aScriptFile, aNextTest, aTestStep);
	CleanupStack::PushL(self);

	self->ConstructL();

	return self;
	}

void CSmutTest::ShowMenuL()
	{
	}

TVerdict CSmutTest::TestSettingsL()
	{
	iTestStep.INFO_PRINTF1(_L("Testing Create Settings..."));

	iSmsTest.SetEntryL(iSmsTest.iSmsServiceId);

	// First we test all the features of the class CSmsSettings
	// We give them different values and compare if it works

	CSmsSettings* settings = CSmsSettings::NewLC();
	if(!CompareSettings(*settings, *settings))
		{
		CleanupStack::PopAndDestroy(settings);
		return EFail;	
		}
	if(!TestSettingsMembersL(*settings))
		{
		CleanupStack::PopAndDestroy(settings);
		return EFail;	
		}
	TestMessageSettingsMembersL(*settings);

	// Lets copy the values to a different instance
	CSmsSettings* smsSettings=CSmsSettings::NewLC();
	smsSettings->CopyL(*settings);
	if(!CompareSettings(*smsSettings, *settings))
		{
		CleanupStack::PopAndDestroy(2,settings);
		return EFail;	
		}
	CleanupStack::PopAndDestroy(smsSettings);
	smsSettings = NULL;
	iTestStep.INFO_PRINTF1(_L("Test CSmsSettings::CopyL() passed"));

	//	Lets create a sms service in the file system with these values
	TestStoreRestoreSettingsL(*settings);

	// Lets make the previous test again; Lets give a settings instance some
	// values, store it, restore it and see that the values are still the same
	CSmsSettings* set2=CSmsSettings::NewLC();
	set2->AddServiceCenterL(_L("Radiolinja0"),_L("+358508771010"));
	set2->AddServiceCenterL(_L("Radiolinja1"),_L("+358508771010"));
	set2->AddServiceCenterL(_L("Radiolinja2"),_L("+358508771010"));
	set2->AddServiceCenterL(_L("Radiolinja3"),_L("+358508771010"));
	set2->AddServiceCenterL(_L("Radiolinja4"),_L("+358508771010"));
	set2->SetDefaultServiceCenter(1);
	iSmsTest(set2->DefaultServiceCenter()==1);
	// Validity Period	
	set2->SetValidityPeriod(ESmsVPSixHours);
	if(set2->ValidityPeriod().Int()!=ESmsVPSixHours)
		{
		CleanupStack::PopAndDestroy(2,settings);
		return EFail;	
		}
	// Reply Quoted
	set2->SetReplyQuoted(EFalse);
	if(set2->ReplyQuoted())
		{
		CleanupStack::PopAndDestroy(2,settings);
		return EFail;	
		}
	// Reject Duplicate 
	set2->SetRejectDuplicate(ETrue);
	if(!set2->RejectDuplicate())
		{
		CleanupStack::PopAndDestroy(2,settings);
		return EFail;	
		}
	// Delivery Method
	set2->SetDelivery(ESmsDeliveryUponRequest);
	if(set2->Delivery()!=ESmsDeliveryUponRequest)
		{
		CleanupStack::PopAndDestroy(2,settings);
		return EFail;	
		}
	// Delivery Report
	set2->SetDeliveryReport(ETrue);
	if(!set2->DeliveryReport())
		{
		CleanupStack::PopAndDestroy(2,settings);
		return EFail;	
		}
	// Reply Path
	set2->SetReplyPath(EFalse);
	if(set2->ReplyPath())
		{
		CleanupStack::PopAndDestroy(2,settings);
		return EFail;	
		}
	// Message Conversion
	set2->SetMessageConversion(ESmsConvPIDNone);
	if(set2->MessageConversion()!=ESmsConvPIDNone)
		{
		CleanupStack::PopAndDestroy(2,settings);
		return EFail;	
		}
	// Can Concatenate
	set2->SetCanConcatenate(ETrue);
	if(!set2->CanConcatenate())
		{
		CleanupStack::PopAndDestroy(2,settings);
		return EFail;	
		}
	// Character Set
	set2->SetCharacterSet(TSmsDataCodingScheme::ESmsAlphabet8Bit);
	if(set2->CharacterSet()!=TSmsDataCodingScheme::ESmsAlphabet8Bit)
		{
		CleanupStack::PopAndDestroy(2,settings);
		return EFail;	
		}
	// Validity period format
	set2->SetValidityPeriodFormat(TSmsFirstOctet::ESmsVPFSemiOctet);
	if(set2->ValidityPeriodFormat()!=TSmsFirstOctet::ESmsVPFSemiOctet)
		{
		CleanupStack::PopAndDestroy(2,settings);
		return EFail;	
		}
	// Status Report Handling
	set2->SetStatusReportHandling(CSmsSettings::EMoveReportToInboxVisible);
	if(set2->StatusReportHandling()!=CSmsSettings::EMoveReportToInboxVisible)
		{
		CleanupStack::PopAndDestroy(2,settings);
		return EFail;	
		}
	// Special Message Handling
	set2->SetSpecialMessageHandling(CSmsSettings::EMoveReportToInboxVisible);
	if(set2->SpecialMessageHandling()!=CSmsSettings::EMoveReportToInboxVisible)
		{
		CleanupStack::PopAndDestroy(2,settings);
		return EFail;	
		}

	if(!TestStoreRestoreSettingsL(*set2))
		{
		CleanupStack::PopAndDestroy(2,settings);
		return EFail;	
		}

	set2->SetClass2Folder(123);
	if(!TestStoreRestoreSettingsL(*set2))
		{
		CleanupStack::PopAndDestroy(2,settings);
		return EFail;	
		}

	CleanupStack::PopAndDestroy(set2);

	iSmsTest.iServiceSettings->CopyL(*settings);  // Lets copy the current setting to the service settings

	if(!CompareSettings(*iSmsTest.iServiceSettings, *settings))
		{
		CleanupStack::PopAndDestroy(settings);
		return EFail;	
		}

	iTestStep.INFO_PRINTF1(_L("CSmsSettings::CopyL() 2"));
	CleanupStack::PopAndDestroy(settings);
	iTestStep.INFO_PRINTF1(_L("Test Create Settings Passed"));
	TestSettingsStoreToCommDbL();
	iTestStep.INFO_PRINTF1(_L("Test Create Settings Passed"));
	return EPass;
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

TBool CSmutTest::TestStoreRestoreSettingsL(const CSmsSettings& aSettings)
	{
	StoreSettingsL(aSettings);

	// Lets read the service and see it the values are still ok
	CSmsSettings* set1=CSmsSettings::NewLC();

	RestoreSettingsL(*set1);

	if(!CompareSettings(*set1, aSettings))
		{
		CleanupStack::PopAndDestroy(set1);
		return EFalse;	
		}

	CleanupStack::PopAndDestroy(set1);
	
	return ETrue;
	}

TBool CSmutTest::TestSettingsMembersL(CSmsSettings& aSettings)
	{
	// Status Report Handling (what should be done to report messages)
	aSettings.SetStatusReportHandling(CSmsSettings::EMoveReportToInboxInvisible);
	if(aSettings.StatusReportHandling()!=CSmsSettings::EMoveReportToInboxInvisible)
		{
		return EFalse;
		}
	aSettings.SetStatusReportHandling(CSmsSettings::EMoveReportToInboxVisible);
	if(aSettings.StatusReportHandling()!=CSmsSettings::EMoveReportToInboxVisible)
		{
		return EFalse;	
		}
	aSettings.SetStatusReportHandling(CSmsSettings::EDiscardReport);
	if(aSettings.StatusReportHandling()!=CSmsSettings::EDiscardReport)
		{
		return EFalse;
		}
	aSettings.SetStatusReportHandling(CSmsSettings::EMoveReportToInboxInvisibleAndMatch);
	if(aSettings.StatusReportHandling()!=CSmsSettings::EMoveReportToInboxInvisibleAndMatch)
		{
		return EFalse;	
		}
	aSettings.SetStatusReportHandling(CSmsSettings::EMoveReportToInboxVisibleAndMatch);
	if(aSettings.StatusReportHandling()!=CSmsSettings::EMoveReportToInboxVisibleAndMatch)
		{
		return EFalse;	
		}
	aSettings.SetStatusReportHandling(CSmsSettings::EDiscardReportAndMatch);
	if(aSettings.StatusReportHandling()!=CSmsSettings::EDiscardReportAndMatch)
		{
		return EFalse;	
		}
	iTestStep.INFO_PRINTF1(_L("Test Status Report Handling Passed"));

	// SC handling
	aSettings.AddServiceCenterL(_L("Radiolinja0"),_L("+358508771010"));
	aSettings.AddServiceCenterL(_L("Radiolinja1"),_L("+358508771010"));
	aSettings.AddServiceCenterL(_L("Radiolinja2"),_L("+358508771010"));
	aSettings.AddServiceCenterL(_L("Radiolinja3"),_L("+358508771010"));
	aSettings.AddServiceCenterL(_L("Radiolinja4"),_L("+358508771010"));
	if(aSettings.GetServiceCenter(0).Name()!=_L("Radiolinja0"))
		{
		return EFalse;	
		}
	aSettings.RemoveServiceCenter(aSettings.ServiceCenterCount()-3);
	aSettings.RemoveServiceCenter(aSettings.ServiceCenterCount()-2);
	if(aSettings.GetServiceCenter(1).Name()!=_L("Radiolinja1"))
		{
		return EFalse;	
		}
	if(aSettings.GetServiceCenter(2).Name()!=_L("Radiolinja4"))
		{
		return EFalse;	
		}
	if(aSettings.GetServiceCenter(2).Address()!=_L("+358508771010"))
		{
		return EFalse;	
		}
	aSettings.RemoveServiceCenter(aSettings.ServiceCenterCount()-1);
	aSettings.RemoveServiceCenter(aSettings.ServiceCenterCount()-1);
	aSettings.RemoveServiceCenter(aSettings.ServiceCenterCount()-1);
	//	aSettings.RemoveServiceCenter(aSettings.ServiceCenterCount()-1); // this would be -1
	aSettings.AddServiceCenterL(_L("Radiolinja0"),_L("+358508771010"));
	aSettings.AddServiceCenterL(_L("Radiolinja1"),_L("+358508771010"));
	aSettings.AddServiceCenterL(_L("Radiolinja2"),_L("+358508771010"));
	aSettings.SetDefaultServiceCenter(1);
	if(aSettings.DefaultServiceCenter()!=1)
		{
		return EFalse;	
		}
	aSettings.SetDefaultServiceCenter(2);
	if(aSettings.DefaultServiceCenter()!=2)
		{
		return EFalse;	
		}
	iTestStep.INFO_PRINTF1(_L("Test Default SC Passed"));

	// Special Message Handling (what should be done to special messages)
	aSettings.SetSpecialMessageHandling(CSmsSettings::EMoveReportToInboxInvisible);
	if(aSettings.SpecialMessageHandling()!=CSmsSettings::EMoveReportToInboxInvisible)
		{
		return EFalse;	
		}
	aSettings.SetSpecialMessageHandling(CSmsSettings::EMoveReportToInboxVisible);
	if(aSettings.SpecialMessageHandling()!=CSmsSettings::EMoveReportToInboxVisible)
		{
		return EFalse;	
		}
	aSettings.SetSpecialMessageHandling(CSmsSettings::EDiscardReport);
	if(aSettings.SpecialMessageHandling()!=CSmsSettings::EDiscardReport)
		{
		return EFalse;	
		}
	aSettings.SetSpecialMessageHandling(CSmsSettings::EMoveReportToInboxInvisibleAndMatch);
	if(aSettings.SpecialMessageHandling()!=CSmsSettings::EMoveReportToInboxInvisibleAndMatch)
		{
		return EFalse;	
		}
	aSettings.SetSpecialMessageHandling(CSmsSettings::EMoveReportToInboxVisibleAndMatch);
	if(aSettings.SpecialMessageHandling()!=CSmsSettings::EMoveReportToInboxVisibleAndMatch)
		{
		return EFalse;	
		}
	aSettings.SetSpecialMessageHandling(CSmsSettings::EDiscardReportAndMatch);
	if(aSettings.SpecialMessageHandling()!=CSmsSettings::EDiscardReportAndMatch)
		{
		return EFalse;	
		}
	iTestStep.INFO_PRINTF1(_L("Test Special Message Handling Passed"));

	//CommDbAction
	aSettings.SetCommDbAction(CSmsSettings::ENone);
	if(aSettings.CommDbAction() != CSmsSettings::ENone)
		{
		return EFalse;	
		}
	aSettings.SetCommDbAction(CSmsSettings::EStoreToCommDb);
	if(aSettings.CommDbAction() != CSmsSettings::EStoreToCommDb)
		{
		return EFalse;	
		}
	iTestStep.INFO_PRINTF1(_L("Test CommDb Action Passed"));

	//SmsBearerAction
	aSettings.SetSmsBearerAction(CSmsSettings::ENone);
	if(aSettings.SmsBearerAction() != CSmsSettings::ENone)
		{
		return EFalse;	
		}
	aSettings.SetSmsBearerAction(CSmsSettings::EStoreToCommDb);
	if(aSettings.SmsBearerAction() != CSmsSettings::EStoreToCommDb)
		{
		return EFalse;	
		}
	iTestStep.INFO_PRINTF1(_L("Test SMS Bearer Action Passed"));

	//SmsBearer
	aSettings.SetSmsBearer(CSmsSettings::ESmsBearerPacketOnly);
	if(aSettings.SmsBearer() != CSmsSettings::ESmsBearerPacketOnly)
		{
		return EFalse;	
		}
	aSettings.SetSmsBearer(CSmsSettings::ESmsBearerPacketOnly);
	if(aSettings.SmsBearer() != CSmsSettings::ESmsBearerPacketOnly)
		{
		return EFalse;	
		}
	aSettings.SetSmsBearer(CSmsSettings::ESmsBearerCircuitOnly);
	if(aSettings.SmsBearer() != CSmsSettings::ESmsBearerCircuitOnly)
		{
		return EFalse;	
		}
	aSettings.SetSmsBearer(CSmsSettings::ESmsBearerPacketPreferred);
	if(aSettings.SmsBearer() != CSmsSettings::ESmsBearerPacketPreferred)
		{
		return EFalse;	
		}
	aSettings.SetSmsBearer(CSmsSettings::ESmsBearerCircuitPreferred);
	if(aSettings.SmsBearer() != CSmsSettings::ESmsBearerCircuitPreferred)
		{
		return EFalse;	
		}
	iTestStep.INFO_PRINTF1(_L("Test SMS Bearer Passed"));

	// Reply Quoted
	aSettings.SetReplyQuoted(EFalse);
	if(aSettings.ReplyQuoted())
		{
		return EFalse;	
		}
	aSettings.SetReplyQuoted(ETrue);
	if(!aSettings.ReplyQuoted())
		{
		return EFalse;	
		}
	iTestStep.INFO_PRINTF1(_L("Test Reply Quoted Passed"));

	// Delivery Method
	aSettings.SetDelivery(ESmsDeliveryUponRequest);
	if(aSettings.Delivery()!=ESmsDeliveryUponRequest)
		{
		return EFalse;	
		}
	aSettings.SetDelivery(ESmsDeliveryScheduled);
	if(aSettings.Delivery()!=ESmsDeliveryScheduled)
		{
		return EFalse;
		}
	aSettings.SetDelivery(ESmsDeliveryImmediately);
	if(aSettings.Delivery()!=ESmsDeliveryImmediately)
		{
		return EFalse;	
		}
	iTestStep.INFO_PRINTF1(_L("Test Delivery Passed"));

	// Class2Folder
	if(aSettings.Class2Folder() != KMsvGlobalInBoxIndexEntryId)
		{
		return EFalse;	
		}
	aSettings.SetClass2Folder(1);
	if(aSettings.Class2Folder() != 1)
		{
		return EFalse;
		}
	iTestStep.INFO_PRINTF1(_L("Test Class2Folder Passed"));	

	// Description length
	if(aSettings.DescriptionLength() != KSmsDescriptionLength)
		{
		return EFalse;
		}
	aSettings.SetDescriptionLength(100);
	if(aSettings.DescriptionLength() != 100)
		{
		return EFalse;
		}
	iTestStep.INFO_PRINTF1(_L("Test DescriptionLength Passed"));	
	
	return ETrue;
	}

TBool CSmutTest::TestMessageSettingsMembersL(CSmsMessageSettings& aSettings)
	{

	// Reject Duplicate 
	aSettings.SetRejectDuplicate(ETrue);
	if(!aSettings.RejectDuplicate())
		{
		return EFalse;	
		}
	aSettings.SetRejectDuplicate(EFalse);
	if(aSettings.RejectDuplicate())
		{
		return EFalse;	
		}
	
	iTestStep.INFO_PRINTF1(_L("Test Reject Duplicate Passed"));

	// Delivery Report
	aSettings.SetDeliveryReport(ETrue);
	if(!aSettings.DeliveryReport())
		{
		return EFalse;	
		}
	aSettings.SetDeliveryReport(EFalse);
	if(aSettings.DeliveryReport())
		{
		return EFalse;	
		}
	iTestStep.INFO_PRINTF1(_L("Test Delivery Report Passed"));

	// Reply Path
	aSettings.SetReplyPath(EFalse);
	if(aSettings.ReplyPath()!=EFalse)
		{
		return EFalse;	
		}
	aSettings.SetReplyPath(ETrue);
	if(!aSettings.ReplyPath())
		{
		return EFalse;	
		}
	iTestStep.INFO_PRINTF1(_L("Test Reply Path Passed"));

	// Validity period format
	aSettings.SetValidityPeriodFormat(TSmsFirstOctet::ESmsVPFNone);
	if(aSettings.ValidityPeriodFormat()!=TSmsFirstOctet::ESmsVPFNone)
		{
		return EFalse;	
		}
	aSettings.SetValidityPeriodFormat(TSmsFirstOctet::ESmsVPFEnhanced);
	if(aSettings.ValidityPeriodFormat()!=TSmsFirstOctet::ESmsVPFEnhanced)
		{
		return EFalse;	
		}
	aSettings.SetValidityPeriodFormat(TSmsFirstOctet::ESmsVPFInteger);
	if(aSettings.ValidityPeriodFormat()!=TSmsFirstOctet::ESmsVPFInteger)
		{
		return EFalse;	
		}
	aSettings.SetValidityPeriodFormat(TSmsFirstOctet::ESmsVPFSemiOctet);
	if(aSettings.ValidityPeriodFormat()!=TSmsFirstOctet::ESmsVPFSemiOctet)
		{
		return EFalse;	
		}
	iTestStep.INFO_PRINTF1(_L("Test Validity Period Format Passed"));

	// Message Conversion (following conversions are supported)
	aSettings.SetMessageConversion(ESmsConvFax);
	if(aSettings.MessageConversion()!=ESmsConvFax)
		{
		return EFalse;	
		}
	aSettings.SetMessageConversion(ESmsConvX400);
	if(aSettings.MessageConversion()!=ESmsConvX400)
		{
		return EFalse;	
		}
	aSettings.SetMessageConversion(ESmsConvPaging);
	if(aSettings.MessageConversion()!=ESmsConvPaging)
		{
		return EFalse;	
		}
	aSettings.SetMessageConversion(ESmsConvMail);
	if(aSettings.MessageConversion()!=ESmsConvMail)
		{
		return EFalse;	
		}
	aSettings.SetMessageConversion(ESmsConvErmes);
	if(aSettings.MessageConversion()!=ESmsConvErmes)
		{
		return EFalse;	
		}
	aSettings.SetMessageConversion(ESmsConvSpeech);
	if(aSettings.MessageConversion()!=ESmsConvSpeech)
		{
		return EFalse;	
		}
	aSettings.SetMessageConversion(ESmsConvPIDNone);
	if(aSettings.MessageConversion()!=ESmsConvPIDNone)
		{
		return EFalse;	
		}
	iTestStep.INFO_PRINTF1(_L("Test Message Conversion Passed"));

	// Validity Period	
	aSettings.SetValidityPeriod(ESmsVPHour);
	if(aSettings.ValidityPeriod().Int()!=ESmsVPHour)
		{
		return EFalse;	
		}
	aSettings.SetValidityPeriod(ESmsVPSixHours);
	if(aSettings.ValidityPeriod().Int()!=ESmsVPSixHours)
		{
		return EFalse;	
		}
	aSettings.SetValidityPeriod(ESmsVP24Hours);
	if(aSettings.ValidityPeriod().Int()!=ESmsVP24Hours)
		{
		return EFalse;	
		}
	aSettings.SetValidityPeriod(ESmsVPMaximum);
	if(aSettings.ValidityPeriod().Int()!=ESmsVPMaximum)
		{
		return EFalse;	
		}
	aSettings.SetValidityPeriod(ESmsVPWeek);
	if(aSettings.ValidityPeriod().Int()!=ESmsVPWeek)
		{
		return EFalse;	
		}
	iTestStep.INFO_PRINTF1(_L("Test Validity Period Passed"));

	// Can Concatenate (write messages with more than 160 characters)
	aSettings.SetCanConcatenate(ETrue);
	if(!aSettings.CanConcatenate())
		{
		return EFalse;	
		}
	aSettings.SetCanConcatenate(EFalse);
	if(aSettings.CanConcatenate()!=EFalse)
		{
		return EFalse;	
		}
	iTestStep.INFO_PRINTF1(_L("Test Can Concatenate Passed"));

	// Character Set (following character sets are supported)
	aSettings.SetCharacterSet(TSmsDataCodingScheme::ESmsAlphabet7Bit);
	if(aSettings.CharacterSet()!=TSmsDataCodingScheme::ESmsAlphabet7Bit)
		{
		return EFalse;	
		}
	aSettings.SetCharacterSet(TSmsDataCodingScheme::ESmsAlphabetUCS2);
	if(aSettings.CharacterSet()!=TSmsDataCodingScheme::ESmsAlphabetUCS2)
		{
		return EFalse;	
		}
	aSettings.SetCharacterSet(TSmsDataCodingScheme::ESmsAlphabet8Bit);
	if(aSettings.CharacterSet()!=TSmsDataCodingScheme::ESmsAlphabet8Bit)
		{
		return EFalse;	
		}
	iTestStep.INFO_PRINTF1(_L("Test Character Set Passed"));
	
	return ETrue;
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

	CSmsSettings* settings = CSmsSettings::NewLC();

	//Test defect fix for EDNAALR-4KTKTD (CSmsSettings::RestoreL() should leave if stream not present)
	const TMsvId id = iSmsTest.CreateDummyMessageToSendL();
	Session().CleanupEntryPushL(id);
	iSmsTest.SetEntryL(id);

	iTestStep.INFO_PRINTF1(_L("TestSettingsRestoreDefectL Created dummy message"));

	TRAPD(err, RestoreSettingsL(*settings));

	iTestStep.INFO_PRINTF2(_L("TestSettingsRestoreDefectL RestoreL 1 with error %d"), err);
	iTestStep.INFO_PRINTF2(_L("Expected %d"), KErrNotFound);
	iSmsTest(err == KErrNotFound);

	//Test defect fix for EDNAALR-4KTKLP (CSmsSettings::InternalizeL() causes memory leak)
	//	iSmsTest.SetEntryL(id);
	StoreSettingsL(*settings);

	iTestStep.INFO_PRINTF1(_L("TestSettingsRestoreDefectL StoreL 1"));

	settings->AddServiceCenterL(_L("Unknown"), _L("447785016005"));

	RestoreSettingsL(*settings);

	Session().CleanupEntryPop();
	Session().RemoveEntry(id);

	CleanupStack::PopAndDestroy(settings);
	iTestStep.INFO_PRINTF1(_L("TestSettingsRestoreDefectL RestoreL 2"));
	}


TBool CSmutTest::TestSettingsStoreToCommDbL()
	{
	iTestStep.INFO_PRINTF1(_L("Test Store Settings to CommDb"));
	
	CSmsSettings* settings1 = CSmsSettings::NewLC();
	CSmsSettings* settings2 = CSmsSettings::NewLC();

	if(!CompareSettings(*settings1, *settings2))
		{
		CleanupStack::PopAndDestroy(2,settings1);
		return EFalse;	
		}

	settings1->SetCommDbAction(CSmsSettings::EStoreToCommDb);
	settings1->AddServiceCenterL(_L("Vodafone"), _L("+447785016005"));
	settings1->SetDeliveryReport(ETrue);
	settings1->SetValidityPeriod(1000);

	iSmsTest.SetEntryL(iSmsTest.iSmsServiceId);

	StoreSettingsL(*settings1);
	iTestStep.INFO_PRINTF1(_L("\tNew Settings..."));
	if(!CompareSettingsAndCommDbL(*settings1))
		{
		CleanupStack::PopAndDestroy(2,settings1);
		return EFalse;	
		}

	settings1->GetServiceCenter(settings1->DefaultServiceCenter()).SetAddressL(_L("+3854354433"));

	StoreSettingsL(*settings1);
	iTestStep.INFO_PRINTF1(_L("\tSC Address Changed..."));
	if(!CompareSettingsAndCommDbL(*settings1))
		{
		CleanupStack::PopAndDestroy(2,settings1);
		return EFalse;	
		}
	
	settings1->SetDeliveryReport(EFalse);

	StoreSettingsL(*settings1);
	iTestStep.INFO_PRINTF1(_L("\tDelivery Report Changed..."));
	if(!CompareSettingsAndCommDbL(*settings1))
		{
		CleanupStack::PopAndDestroy(2,settings1);
		return EFalse;	
		}

	settings1->SetValidityPeriod(2000);

	StoreSettingsL(*settings1);
	iTestStep.INFO_PRINTF1(_L("\tValidity Period Changed..."));
	if(!CompareSettingsAndCommDbL(*settings1))
		{
		CleanupStack::PopAndDestroy(2,settings1);
		return EFalse;	
		}

	settings1->GetServiceCenter(settings1->DefaultServiceCenter()).SetAddressL(_L("+48996655888"));
	settings1->SetDeliveryReport(ETrue);
	settings1->SetValidityPeriod(3000);

	StoreSettingsL(*settings1);
	iTestStep.INFO_PRINTF1(_L("\tAll Settings Changed..."));
	if(!CompareSettingsAndCommDbL(*settings1))
		{
		CleanupStack::PopAndDestroy(2,settings1);
		return EFalse;	
		}

	settings1->AddServiceCenterL(_L("Radio"), _L("+385669988"));
	settings1->SetDefaultServiceCenter(settings1->ServiceCenterCount()-1);

	StoreSettingsL(*settings1);
	iTestStep.INFO_PRINTF1(_L("\tDefaultSC Changed..."));
	if(!CompareSettingsAndCommDbL(*settings1))
		{
		CleanupStack::PopAndDestroy(2,settings1);
		return EFalse;	
		}

	settings1->SetCommDbAction(CSmsSettings::ENone);
	settings1->GetServiceCenter(settings1->DefaultServiceCenter()).SetAddressL(_L("+123456789"));
	settings1->SetDeliveryReport(EFalse);
	settings1->SetValidityPeriod(4000);

	iTestStep.INFO_PRINTF1(_L("\tTesting Setting Different from CommDb..."));
	if(CompareSettingsAndCommDbL(*settings1))
		{
		CleanupStack::PopAndDestroy(2,settings1);
		return EFalse;	
		}

	TInt count = settings1->ServiceCenterCount();

	while (count--)
		{
		settings1->RemoveServiceCenter(count);
		}

	iTestStep.INFO_PRINTF1(_L("\tTesting Setting Different from CommDb..."));
	if(CompareSettingsAndCommDbL(*settings1))
		{
		CleanupStack::PopAndDestroy(2,settings1);
		return EFalse;	
		}

	iTestStep.INFO_PRINTF1(_L("\tRestore settings from store..."));
	RestoreSettingsL(*settings2);
	if(!CompareSettingsAndCommDbL(*settings2))
		{
		CleanupStack::PopAndDestroy(2,settings1);
		return EFalse;	
		}

	//Test SMS Bearer

	iTestStep.INFO_PRINTF1(_L("\tTest Store SMS Bearer..."));

	//Test 1

	settings1->SetSmsBearerAction(CSmsSettings::EStoreToCommDb);
	settings1->SetSmsBearer(CSmsSettings::ESmsBearerPacketOnly);

	StoreSettingsL(*settings1);
	if(!CompareSettingsAndSmsBearerL(*settings1))
		{
		CleanupStack::PopAndDestroy(2,settings1);
		return EFalse;	
		}
	
	//Test 2

	settings1->SetSmsBearer(CSmsSettings::ESmsBearerCircuitOnly);
	if(CompareSettingsAndSmsBearerL(*settings1))
		{
		CleanupStack::PopAndDestroy(2,settings1);
		return EFalse;	
		}

	//Test 3

	StoreSettingsL(*settings1);
	if(!CompareSettingsAndSmsBearerL(*settings1))
		{
		CleanupStack::PopAndDestroy(2,settings1);
		return EFalse;	
		}

	RestoreSettingsL(*settings2);
	if(!CompareSettingsAndSmsBearerL(*settings2))
		{
		return EFalse;	
		}

	//Test 4

	settings1->SetSmsBearerAction(CSmsSettings::ENone);
	StoreSettingsL(*settings1);
	if(!CompareSettingsAndSmsBearerL(*settings1))
		{
		CleanupStack::PopAndDestroy(2,settings1);
		return EFalse;	
		}

	//Test 5

	settings1->SetSmsBearer(CSmsSettings::ESmsBearerCircuitPreferred);
	if(CompareSettingsAndSmsBearerL(*settings1))
		{
		CleanupStack::PopAndDestroy(2,settings1);
		return EFalse;	
		}
	StoreSettingsL(*settings1);
	if(CompareSettingsAndSmsBearerL(*settings1))
		{
		CleanupStack::PopAndDestroy(2,settings1);
		return EFalse;	
		}

	CleanupStack::PopAndDestroy(2,settings1); // settings2
	
	return ETrue;
	}

TBool CSmutTest::CompareSettingsAndCommDbL(const CSmsSettings& aSettings) const
	{
	TCompare compare = ETrue;
	TBuf<256> scAddress;
	CMDBRecordSet<CCDGlobalSettingsRecord> globalSettingsRecord(KCDTIdGlobalSettingsRecord);
	globalSettingsRecord.LoadL(*iDBSession);
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
	TUint32 smsBearer=0;
	CMDBRecordSet<CCDGlobalSettingsRecord> globalSettingsRecord(KCDTIdGlobalSettingsRecord);
	TRAPD(err, globalSettingsRecord.LoadL(*iDBSession));
	if(err == KErrNone)
		{
		smsBearer = ((CCDGlobalSettingsRecord*)globalSettingsRecord.iRecords[0])->iSMSBearer;		
		}	
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

TVerdict CSmutTest::TestNumberL()
	{
	iTestStep.INFO_PRINTF1(_L("Testing Preparing test CSmsNumber..."));

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
	number->SetLogId(KLogID1);	
	number->SetAckStatus(ESmsAckTypeDelivery, CSmsNumber::EAckSuccessful);

	if(number->Name()!=_L("Radiolinja1")) // name ok?
		{
		CleanupStack::PopAndDestroy(2, number0);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	if(number->Address()!=_L("+358508771010")) // number ok?
		{
		CleanupStack::PopAndDestroy(2, number0);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	if(number->LogId() != KLogID1)
		{
		CleanupStack::PopAndDestroy(2, number0);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	if(number->AckStatus(ESmsAckTypeDelivery) != CSmsNumber::EAckSuccessful)
		{
		CleanupStack::PopAndDestroy(2, number0);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}

	StoreNumberL(*number);
	RestoreNumberL(*number0);
	if(!CompareNumbers(*number0, *number))
		{
		CleanupStack::PopAndDestroy(2, number0);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;
		}
	
// Lets try the max length of the telephone address, store and
// restore it and see if it works

	number->SetNameL(_L("Whatsthemaximumlengthforthis?Whatsthemaximumlengthforthis?Whatsthemaximumlengthforthis?"));
	number->SetAddressL(_L("+0123456789012345678"));
	number->SetLogId(KLogID2);	
	number->SetAckStatus(ESmsAckTypeDelivery, CSmsNumber::EAckError);

	if(number->Name()!=_L("Whatsthemaximumlengthforthis?Whatsthemaximumlengthforthis?Whatsthemaximumlengthforthis?"))
		{
		CleanupStack::PopAndDestroy(2, number0);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	if(number->Address()!=_L("+0123456789012345678"))
		{
		CleanupStack::PopAndDestroy(2, number0);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	if(number->LogId() != KLogID2)
		{
		CleanupStack::PopAndDestroy(2, number0);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	if(number->AckStatus(ESmsAckTypeDelivery) != CSmsNumber::EAckError)
		{
		CleanupStack::PopAndDestroy(2, number0);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}

	StoreNumberL(*number);
	RestoreNumberL(*number0);
	if(!CompareNumbers(*number, *number0))
		{
		CleanupStack::PopAndDestroy(2, number0);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
// Lets use special Scandinavian letters both in the name and
// address, store and restore and see if they are ok
	number->SetNameL(_L("ÖÄÅåöä"));
	number->SetAddressL(_L("åäöÅÄÖ"));
	number->SetLogId(KLogID1);	
	number->SetAckStatus(ESmsAckTypeDelivery, CSmsNumber::EAckSuccessful);

	if(number->Name()!=_L("ÖÄÅåöä"))
		{
		CleanupStack::PopAndDestroy(2, number0);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	if(number->Address()!=_L("åäöÅÄÖ"))
		{
		CleanupStack::PopAndDestroy(2, number0);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	if(number->LogId() != KLogID1)
		{
		CleanupStack::PopAndDestroy(2, number0);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;
		}
	if(number->AckStatus(ESmsAckTypeDelivery) != CSmsNumber::EAckSuccessful)
		{
		CleanupStack::PopAndDestroy(2, number0);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}

	StoreNumberL(*number);
	RestoreNumberL(*number0);
	iSmsTest(CompareNumbers(*number, *number0));

//  Lets try if both the number and the name can be empty,
//	store and restore it and see if they still match
	number->SetNameL(_L(""));
	number->SetAddressL(_L(""));
	number->SetLogId(KLogID2);	
	number->SetAckStatus(ESmsAckTypeDelivery, CSmsNumber::EAckError);

	if(number->Name()!=_L(""))
		{
		CleanupStack::PopAndDestroy(2, number0);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;
		}
	if(number->Address()!=_L(""))
		{
		CleanupStack::PopAndDestroy(2, number0);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	if(number->LogId() != KLogID2)
		{
		CleanupStack::PopAndDestroy(2, number0);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	if(number->AckStatus(ESmsAckTypeDelivery) != CSmsNumber::EAckError)
		{
		CleanupStack::PopAndDestroy(2, number0);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;
		}

	StoreNumberL(*number);
	RestoreNumberL(*number0);
	if(!CompareNumbers(*number, *number0))
		{
		CleanupStack::PopAndDestroy(2, number0);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}

//  Lets try [alt+255] as both the name and number, store and	
//  restore it and see if it works
	number->SetNameL(_L(" ")); //alt+255
	number->SetAddressL(_L(" "));
	number->SetLogId(KLogID1);	
	number->SetAckStatus(ESmsAckTypeDelivery, CSmsNumber::EAckSuccessful);

	if(number->Name()!=_L(" "))
		{
		CleanupStack::PopAndDestroy(2, number0);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;		
		}
	if(number->Address()!=_L(" "))
		{
		CleanupStack::PopAndDestroy(2, number0);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;		
		}
	if(number->LogId() != KLogID1)
		{
		CleanupStack::PopAndDestroy(2, number0);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;		
		}
	if(number->AckStatus(ESmsAckTypeDelivery) != CSmsNumber::EAckSuccessful)
		{
		CleanupStack::PopAndDestroy(2, number0);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;		
		}

	StoreNumberL(*number);
	RestoreNumberL(*number0);
	iSmsTest(CompareNumbers(*number, *number0));

// Test CSmsNumber::CopyL()

	number->SetNameL(_L("DavidCuando"));	
	number->SetAddressL(_L("+447747065627")); 
	number->SetLogId(KLogID3);	
	number->SetAckStatus(ESmsAckTypeDelivery, CSmsNumber::EPendingAck);

	if(number->Name()!=_L("DavidCuando"))
		{
		CleanupStack::PopAndDestroy(2, number0);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	if(number->Address()!=_L("+447747065627"))
		{
		CleanupStack::PopAndDestroy(2, number0);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;
		}
	if(number->LogId() != KLogID3)
		{
		CleanupStack::PopAndDestroy(2, number0);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;
		}
	if(number->AckStatus(ESmsAckTypeDelivery) != CSmsNumber::EPendingAck)
		{
		CleanupStack::PopAndDestroy(2, number0);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;
		}

	number0->CopyL(*number);
	if(!CompareNumbers(*number0, *number))
		{
		CleanupStack::PopAndDestroy(2, number0);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;
		}

	CleanupStack::PopAndDestroy(2, number0);

// Test NewL(CSmsNumber&)

	number=CSmsNumber::NewL();  // create the instance
	CleanupStack::PushL(number);

	number->SetNameL(_L("DavidCuando"));	
	number->SetAddressL(_L("+447747065627")); 
	number->SetLogId(KLogID3);	
	number->SetAckStatus(ESmsAckTypeDelivery, CSmsNumber::EPendingAck);

	if(number->Name()!=_L("DavidCuando"))
		{
		CleanupStack::PopAndDestroy(number);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;		
		}
	if(number->Address()!=_L("+447747065627"))
		{
		CleanupStack::PopAndDestroy(number);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);		
		return EFail;
		}
	if(number->LogId() != KLogID3)
		{
		CleanupStack::PopAndDestroy(number);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);		
		return EFail;
		}
	if(number->AckStatus(ESmsAckTypeDelivery) != CSmsNumber::EPendingAck)
		{
		CleanupStack::PopAndDestroy(number);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);		
		return EFail;
		}

	number0=CSmsNumber::NewL(*number);  
	CleanupStack::PushL(number0);

	if(!CompareNumbers(*number0, *number))
		{
		CleanupStack::PopAndDestroy(2, number);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;		
		}
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
	
	iTestStep.INFO_PRINTF1(_L("Test CSmsNumber Passed."));
	return EPass;
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
	
TVerdict CSmutTest::TestEmailFieldsL()
	{
	iTestStep.INFO_PRINTF1(_L("Preparing test CSmsEmailFields..."));

	TMsvId id=iSmsTest.CreateDummyMessageToSendL();
	Session().CleanupEntryPushL(id);
	iSmsTest.SetEntryL(id);

	_LIT(KTestAddress, "support@symbian.com");
	_LIT(KTestSubject, "Happy Days!");
	
	// Test CSmsEmailFields::NewL()
	CSmsEmailFields* fields1 = CSmsEmailFields::NewL();
	CleanupStack::PushL(fields1);
	
	if(fields1->Addresses().MdcaCount()!=0)
		{
		CleanupStack::PopAndDestroy(fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;
		}
	if(fields1->Subject().Compare(KNullDesC()) != 0)
		{
		CleanupStack::PopAndDestroy(fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	if(fields1->HasAddress())
		{
		CleanupStack::PopAndDestroy(fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	if(fields1->Length() != 0)
		{
		CleanupStack::PopAndDestroy(fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	
	// Test address and subject accessors, has-address, length and reset methods
	fields1->AddAddressL(KNullDesC);	// empty address - does nothing!
	if(fields1->Addresses().MdcaCount()!=0)
		{
		CleanupStack::PopAndDestroy(fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;		
		}
	if(fields1->Subject().Compare(KNullDesC()) != 0)
		{
		CleanupStack::PopAndDestroy(fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;		
		}
	if(fields1->HasAddress())
		{
		CleanupStack::PopAndDestroy(fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;		
		}
	if(fields1->Length() != 0)
		{
		CleanupStack::PopAndDestroy(fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;		
		}
	
	fields1->AddAddressL(KTestAddress);
	if(fields1->Addresses().MdcaCount()!=1)
		{
		CleanupStack::PopAndDestroy(fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	if(fields1->Addresses().MdcaPoint(0).Compare(KTestAddress) != 0)
		{
		CleanupStack::PopAndDestroy(fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	if(!fields1->HasAddress())
		{
		CleanupStack::PopAndDestroy(fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	if(fields1->Length() != KTestAddress().Length() + 1)
		{
		CleanupStack::PopAndDestroy(fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}

	fields1->SetSubjectL(KTestSubject);
	if(!fields1->HasAddress())
		{
		CleanupStack::PopAndDestroy(fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	if(fields1->Length() != KTestAddress().Length() + KTestSubject().Length() + 2)
		{
		CleanupStack::PopAndDestroy(fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	
	fields1->Reset();
	if(fields1->Addresses().MdcaCount()!=0)
		{
		CleanupStack::PopAndDestroy(fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	if(fields1->Subject().Compare(KNullDesC()) != 0)
		{
		CleanupStack::PopAndDestroy(fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	if(fields1->HasAddress())
		{
		CleanupStack::PopAndDestroy(fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	if(fields1->Length() != 0)
		{
		CleanupStack::PopAndDestroy(fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	
	fields1->SetSubjectL(KTestSubject);
	if(fields1->Subject().Compare(KTestSubject) != 0)
		{
		CleanupStack::PopAndDestroy(fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	if(fields1->HasAddress())
		{
		CleanupStack::PopAndDestroy(fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	if(fields1->Length() != KTestSubject().Length() + 2)
		{
		CleanupStack::PopAndDestroy(fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	
	fields1->AddAddressL(KTestAddress);
	if(fields1->Addresses().MdcaCount()!=1)
		{
		CleanupStack::PopAndDestroy(fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;		
		}
	if(fields1->Addresses().MdcaPoint(0).Compare(KTestAddress) != 0)
		{
		CleanupStack::PopAndDestroy(fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;		
		}
	if(!fields1->HasAddress())
		{
		CleanupStack::PopAndDestroy(fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;		
		}
	if(fields1->Length() != KTestAddress().Length() + KTestSubject().Length() + 2)
		{
		CleanupStack::PopAndDestroy(fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;		
		}

	// Test multiple addresses - add empty address -> not added	
	fields1->AddAddressL(KNullDesC);
	if(fields1->Addresses().MdcaCount()!=1)
		{
		CleanupStack::PopAndDestroy(fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;		
		}
	
	_LIT(KTestAddress2, "support2@symbian.com");
	fields1->AddAddressL(KTestAddress2);
	if(fields1->Addresses().MdcaCount()!=2)
		{
		CleanupStack::PopAndDestroy(fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;		
		}
	if(fields1->Addresses().MdcaPoint(1).Compare(KTestAddress2) != 0)
		{
		CleanupStack::PopAndDestroy(fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;		
		}
	if(!fields1->HasAddress())
		{
		CleanupStack::PopAndDestroy(fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;		
		}
	// NOTE - length check; 3 -> 2 for subject delimiters, 1 for address comma separtor.
	if(fields1->Length() != KTestAddress().Length() + KTestAddress2().Length() + KTestSubject().Length() + 3)
		{
		CleanupStack::PopAndDestroy(fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;		
		}
	
	// Test copy factory c'tor
	CSmsEmailFields* fields2 = CSmsEmailFields::NewL(*fields1);
	CleanupStack::PushL(fields2);

	if(!CompareEmailFields(*fields2, *fields1))
		{
		CleanupStack::PopAndDestroy(2,fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	
	// Test store and restore
	StoreEmailFieldsL(*fields1);
	fields2->Reset();
	RestoreEmailFieldsL(*fields2);

	if(!CompareEmailFields(*fields2, *fields1))
		{
		CleanupStack::PopAndDestroy(2,fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	
	fields1->Reset();
	StoreEmailFieldsL(*fields1);
	RestoreEmailFieldsL(*fields2);

	if(!CompareEmailFields(*fields2, *fields1))
		{
		CleanupStack::PopAndDestroy(2,fields1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	
	CleanupStack::PopAndDestroy(2, fields1);
	
	// Test parsing - expected data.
	_LIT(KTestBodyData, "Some body text#( )#");

	_LIT(KTestParseAddress1, "user1@symbian.com");
	_LIT(KTestParseSubject1, "");
	_LIT(KTestEmail1, "user1@symbian.com Some body text#( )#");
	if(!TestEmailFieldsParsePassL(KTestEmail1, KTestParseAddress1, KTestParseSubject1, KTestBodyData))
		{
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}

	_LIT(KTestParseFullAddress1, "Some User <user1@symbian.com>");
	_LIT(KTestEmail2, "Some User <user1@symbian.com> Some body text#( )#");
	if(!TestEmailFieldsParsePassL(KTestEmail2, KTestParseFullAddress1, KTestParseSubject1, KTestBodyData))
		{
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}

	_LIT(KTestParseAddress2, "user2@symbian.com");
	_LIT(KTestParseSubject2, "subject (2)");
	_LIT(KTestEmail3, "user2@symbian.com##subject (2)#Some body text#( )#");
	if(!TestEmailFieldsParsePassL(KTestEmail3, KTestParseAddress2, KTestParseSubject2, KTestBodyData))
		{
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}

	_LIT(KTestParseFullAddress2, "Some User <user2@symbian.com>");
	_LIT(KTestEmail4, "Some User <user2@symbian.com>##subject (2)#Some body text#( )#");
	if(!TestEmailFieldsParsePassL(KTestEmail4, KTestParseFullAddress2, KTestParseSubject2, KTestBodyData))
		{
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;
		}

	_LIT(KTestParseAddress3, "user3@symbian.com");
	_LIT(KTestParseSubject3, "subject #3");
	_LIT(KTestEmail5, "user3@symbian.com(subject #3)Some body text#( )#");
	if(!TestEmailFieldsParsePassL(KTestEmail5, KTestParseAddress3, KTestParseSubject3, KTestBodyData))
		{
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}

	_LIT(KTestParseFullAddress3, "Some User <user3@symbian.com>");
	_LIT(KTestEmail6, "Some User <user3@symbian.com>(subject #3)Some body text#( )#");
	if(!TestEmailFieldsParsePassL(KTestEmail6, KTestParseFullAddress3, KTestParseSubject3, KTestBodyData))
		{
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;
		}
	
	_LIT(KTestParseAddress4, "user4@symbian.com");
	_LIT(KTestParseSubject4, "");
	_LIT(KTestEmail7, "user4@symbian.com()Some body text#( )#");
	if(!TestEmailFieldsParsePassL(KTestEmail7, KTestParseAddress4, KTestParseSubject4, KTestBodyData))
		{
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;
		}

	_LIT(KTestParseFullAddress4, "Some User <user4@symbian.com>");
	_LIT(KTestEmail8, "Some User <user4@symbian.com>()Some body text#( )#");
	if(!TestEmailFieldsParsePassL(KTestEmail8, KTestParseFullAddress4, KTestParseSubject4, KTestBodyData))
		{
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}

	_LIT(KTestParseAddress5, "user5@symbian.com");
	_LIT(KTestParseSubject5, "");
	_LIT(KTestEmail9, "user5@symbian.com###Some body text#( )#");
	if(!TestEmailFieldsParsePassL(KTestEmail9, KTestParseAddress5, KTestParseSubject5, KTestBodyData))
		{
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}

	_LIT(KTestParseFullAddress5, "Some User <user5@symbian.com>");
	_LIT(KTestEmail10, "Some User <user5@symbian.com>###Some body text#( )#");
	if(!TestEmailFieldsParsePassL(KTestEmail10, KTestParseFullAddress5, KTestParseSubject5, KTestBodyData))
		{
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}

	// Test parsing - corrupt but parse-able data.
	_LIT(KTestCorruptAddress1,	"user1@symbian.com");
	_LIT(KTestCorruptSubject1,	"smiley ;");
	_LIT(KTestCorruptBody1,		" subject)Some body text");
	_LIT(KTestCorruptEmail1,	"user1@symbian.com(smiley ;) subject)Some body text");
	if(!TestEmailFieldsParsePassL(KTestCorruptEmail1, KTestCorruptAddress1, KTestCorruptSubject1, KTestCorruptBody1))
		{
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	
	_LIT(KTestCorruptAddress2,	"user1@symbian.com");
	_LIT(KTestCorruptSubject2,	"");
	_LIT(KTestCorruptBody2,		"(bad subject)Some body text");
	_LIT(KTestCorruptEmail2,	"user1@symbian.com (bad subject)Some body text");
	if(!TestEmailFieldsParsePassL(KTestCorruptEmail2, KTestCorruptAddress2, KTestCorruptSubject2, KTestCorruptBody2))
		{
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}

	_LIT(KTestCorruptAddress3,	"user1@symbian.com");
	_LIT(KTestCorruptSubject3,	"");
	_LIT(KTestCorruptBody3,		"bad subject##Some body text");
	_LIT(KTestCorruptEmail3,	"user1@symbian.com###bad subject##Some body text");
	if(!TestEmailFieldsParsePassL(KTestCorruptEmail3, KTestCorruptAddress3, KTestCorruptSubject3, KTestCorruptBody3))
		{
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	
	// Test parsing - corrupt and fail.
	_LIT(KTestFailEmail1,	"user#domain1.domain2 Some body text");
	if(!TestEmailFieldsParseFailL(KTestFailEmail1))
		{
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	
	_LIT(KTestFailEmail2,	"user@domain1.domain2+Some+body+text");
	if(!TestEmailFieldsParseFailL(KTestFailEmail2))
		{
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}

	_LIT(KTestFailEmail3,	"user@domain1.domain2##(subject)Some+body+text");
	if(!TestEmailFieldsParseFailL(KTestFailEmail3))
		{
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;
		}

	_LIT(KTestFailEmail4,	"user@domain1.domain2(##subject#Some+body+text");
	if(!TestEmailFieldsParseFailL(KTestFailEmail4))
		{
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}

	_LIT(KTestFailEmail5,	"user1@symbian.com#bad subject#Some body text");
	if(!TestEmailFieldsParseFailL(KTestFailEmail5))
		{
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	
	// Test composing - ok
	_LIT(KTestComposeAddress1,	"user1@domain1.domain2");
	_LIT(KTestComposeSubject1,	"");
	_LIT(KTestComposeFields1,	"user1@domain1.domain2 ");
	if(!TestEmailFieldsComposeL(KTestComposeFields1, KTestComposeAddress1, KNullDesC, KTestComposeSubject1))
		{
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	
	_LIT(KTestComposeAddress2,	"user2@domain1.domain2");
	_LIT(KTestComposeSubject2,	"a subject");
	_LIT(KTestComposeFields2,	"user2@domain1.domain2(a subject)");
	if(!TestEmailFieldsComposeL(KTestComposeFields2, KTestComposeAddress2, KNullDesC, KTestComposeSubject2))
		{
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}

	_LIT(KTestComposeFields3,	"user1@domain1.domain2,user2@domain1.domain2 ");
	if(!TestEmailFieldsComposeL(KTestComposeFields3, KTestComposeAddress1, KTestComposeAddress2, KTestComposeSubject1))
		{
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	
	_LIT(KTestComposeFields4,	"user2@domain1.domain2,user1@domain1.domain2(a subject)");
	if(!TestEmailFieldsComposeL(KTestComposeFields4, KTestComposeAddress2, KTestComposeAddress1, KTestComposeSubject2))
		{
			Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}

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
	
	iTestStep.INFO_PRINTF1(_L("Test EmailFields Passed"));
	return EPass;
	}
	
TBool CSmutTest::TestEmailFieldsParsePassL(const TDesC& aMessage, const TDesC& aAddress, const TDesC& aSubject, const TDesC& aBody)
	{
	CSmsEmailFields* fields = CSmsEmailFields::NewL();
	CleanupStack::PushL(fields);
	
	TInt pos = fields->ParseL(aMessage);
	
	TPtrC body(aMessage.Mid(pos));
	if(!fields->HasAddress())
		{
		CleanupStack::PopAndDestroy(fields);
		return EFalse;	
		}
	if(fields->Addresses().MdcaCount()!=1)
		{
		CleanupStack::PopAndDestroy(fields);
		return EFalse;	
		}
	if(fields->Addresses().MdcaPoint(0).Compare(aAddress) != 0)
		{
		CleanupStack::PopAndDestroy(fields);
		return EFalse;	
		}
	if(fields->Subject().Compare(aSubject) != 0)
		{
		CleanupStack::PopAndDestroy(fields);
		return EFalse;	
		}
	if(body.Compare(aBody) != 0)
		{
		CleanupStack::PopAndDestroy(fields);
		return EFalse;	
		}
	
	CleanupStack::PopAndDestroy(fields);
	return ETrue;
	}

TBool CSmutTest::TestEmailFieldsParseFailL(const TDesC& aMessage)
	{
	CSmsEmailFields* fields = CSmsEmailFields::NewL();
	CleanupStack::PushL(fields);
	
	TInt pos = fields->ParseL(aMessage);
	
	CleanupStack::PopAndDestroy(fields);	
	
	return (pos == KErrCorrupt);
	}

TBool CSmutTest::TestEmailFieldsComposeL(const TDesC& aFields, const TDesC& aAddress1, const TDesC& aAddress2, const TDesC& aSubject)
	{
	CSmsEmailFields* fields = CSmsEmailFields::NewL();
	CleanupStack::PushL(fields);
	
	fields->AddAddressL(aAddress1);
	fields->AddAddressL(aAddress2);
	fields->SetSubjectL(aSubject);
	HBufC* buffer = fields->ComposeLC();	
	
	TBool result = aFields.Compare(*buffer) == 0;
	CleanupStack::PopAndDestroy(2, fields);	// fields, buffer
		
	return result;
	}

TVerdict CSmutTest::TestHeaderL()
	{

	// Testing class CSmsHeader
	iTestStep.INFO_PRINTF1(_L("Testing CSmsHeader..."));
	
	// A CSmsHeader can be of 6 different types
	// First we test constructing ESmsDeliver
	delete iSmsHeader; // deleting the previous one
	iSmsHeader = NULL;
	iSmsHeader=CSmsHeader::NewL(CSmsPDU::ESmsDeliver,*iRichText); // constructing
	if(iSmsHeader->Type()!=CSmsPDU::ESmsDeliver)	// type ok?
		{
		return EFail;	
		}

	CSmsDeliver& deliver = iSmsHeader->Deliver();  // lets get the CSmsPDU of the CSmsHeader
	const CSmsDeliver& deliver2 = iSmsHeader->Deliver();

	if(deliver.Type() != CSmsPDU::ESmsDeliver)// Are the types ok?
		{
		return EFail;	
		}
	if(deliver2.Type() != CSmsPDU::ESmsDeliver)
		{
		return EFail;	
		}

		
	// Lets access the CSmsMessage of our header
	CSmsMessage& message=iSmsHeader->Message();
	//Get the operations
	CSmsReplyAddressOperations& operations = STATIC_CAST(CSmsReplyAddressOperations&,message.GetOperationsForIEL(CSmsInformationElement::ESmsReplyAddressFormat));		
	//Add address
	TPtrC replyAddress(_L("+447583927594"));
	operations.AddReplyAddressL(replyAddress);
	
	//get the address back
	if(iSmsHeader->ReplyAddressL()!=_L("+447583927594"))
		{
		return EFail;		
		}
		
	// Add a from address and make sure that the reply address is still used instead
	iSmsHeader->SetFromAddressL(_L("+358503367709"));
	//get the address back
	if(iSmsHeader->ReplyAddressL()!=_L("+447583927594"))
		{
		return EFail;		
		}
		
	// Type ESmsCommand
	delete iSmsHeader; // deleting previous
	iSmsHeader=CSmsHeader::NewL(CSmsPDU::ESmsCommand,*iRichText);
	if(iSmsHeader->Type()!=CSmsPDU::ESmsCommand)
		{
		return EFail;	
		}

	CSmsCommand& command=iSmsHeader->Command();
	const CSmsCommand& command2=iSmsHeader->Command();

	if(command.Type() != CSmsPDU::ESmsCommand)
		{
		return EFail;	
		}
	if(command2.Type() != CSmsPDU::ESmsCommand)
		{
		return EFail;	
		}

	// Type ESmsStatusReport
	delete iSmsHeader; // deleting the previous one
	iSmsHeader=CSmsHeader::NewL(CSmsPDU::ESmsStatusReport,*iRichText);
	if(iSmsHeader->Type()!=CSmsPDU::ESmsStatusReport)
		{
		return EFail;	
		}

	CSmsStatusReport& statusreport=iSmsHeader->StatusReport();
	const CSmsStatusReport& statusreport2=iSmsHeader->StatusReport();

	if(statusreport.Type() != CSmsPDU::ESmsStatusReport)
		{
		return EFail;	
		}
	if(statusreport2.Type() != CSmsPDU::ESmsStatusReport)
		{
		return EFail;	
		}

	// Type ESmsSubmit
	delete iSmsHeader; // deleting the previous one
	iSmsHeader=CSmsHeader::NewL(CSmsPDU::ESmsSubmit,*iRichText);
	if(iSmsHeader->Type()!=CSmsPDU::ESmsSubmit)
		{
		return EFail;	
		}

	CSmsSubmit& submit=iSmsHeader->Submit();
	const CSmsSubmit& submit2=iSmsHeader->Submit();

	if(submit.Type() != CSmsPDU::ESmsSubmit)
		{
		return EFail;	
		}
	if(submit2.Type() != CSmsPDU::ESmsSubmit)
		{
		return EFail;	
		}

	// Lets access the CSmsMessage of our header
	CSmsMessage& message1=iSmsHeader->Message();
	const CSmsMessage& message2=iSmsHeader->Message();

	if(message1.Type() != CSmsPDU::ESmsSubmit)
		{
		return EFail;		
		}
	if(message2.Type() != CSmsPDU::ESmsSubmit)
		{
		return EFail;		
		}

	// Lets test the features of CSmsHeader with different values
	iSmsHeader->SetReplyPathProvided(ETrue);
	if(!iSmsHeader->ReplyPathProvided())
		{
		return EFail;		
		}
	iSmsHeader->SetReplyPathProvided(EFalse);
	if(iSmsHeader->ReplyPathProvided())
		{
		return EFail;		
		}

	iSmsHeader->SetBioMsgIdType(EBioMsgIdUnknown);
	if(iSmsHeader->BioMsgIdType()!=EBioMsgIdUnknown)
		{
		return EFail;		
		}
	iSmsHeader->SetBioMsgIdType(EBioMsgIdIana);
	if(iSmsHeader->BioMsgIdType()!=EBioMsgIdIana)
		{
		return EFail;		
		}
	iSmsHeader->SetBioMsgIdType(EBioMsgIdNbs);
	if(iSmsHeader->BioMsgIdType()!=EBioMsgIdNbs)
		{
		return EFail;		
		}
	iSmsHeader->SetBioMsgIdType(EBioMsgIdWap);
	if(iSmsHeader->BioMsgIdType()!=EBioMsgIdWap)
		{
		return EFail;		
		}
	iSmsHeader->SetBioMsgIdType(EBioMsgIdWapSecure);
	if(iSmsHeader->BioMsgIdType()!=EBioMsgIdWapSecure)
		{
		return EFail;		
		}

	// Lets set the service center number
	iSmsHeader->SetServiceCenterAddressL(_L("+350508771010"));
	if(iSmsHeader->ServiceCenterAddress()!=_L("+350508771010"))
		{
		return EFail;		
		}

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
	if(smsset2->CharacterSet()!=TSmsDataCodingScheme::ESmsAlphabet7Bit)
		{
		CleanupStack::PopAndDestroy(smsset2);
		return EFail;	
		}
	if(smsset2->DeliveryReport())
		{
		CleanupStack::PopAndDestroy(smsset2);
		return EFail;	
		}
	if(smsset2->MessageConversion()!=ESmsConvPIDNone)
		{
		CleanupStack::PopAndDestroy(smsset2);
		return EFail;	
		}
	if(!smsset2->RejectDuplicate())
		{
		CleanupStack::PopAndDestroy(smsset2);
		return EFail;	
		}
	if(smsset2->ReplyPath())
		{
		CleanupStack::PopAndDestroy(smsset2);
		return EFail;	
		}
	if(smsset2->ValidityPeriod().Int()!=ESmsVP24Hours)
		{
		CleanupStack::PopAndDestroy(smsset2);
		return EFail;	
		}
	if(smsset2->ValidityPeriodFormat()!=TSmsFirstOctet::ESmsVPFSemiOctet)
		{
		CleanupStack::PopAndDestroy(smsset2);
		return EFail;	
		}
	CleanupStack::PopAndDestroy(smsset2);

	//Test Set/FromAddress()
	iSmsHeader->SetFromAddressL(_L("+358503367709"));
	if(iSmsHeader->ReplyAddressL()!=_L("+358503367709"))
		{
		return EFail;		
		}
	
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
	if(smsset3->MessageConversion()!=ESmsConvPIDNone)
		{
		CleanupStack::PopAndDestroy(2,smsset3);
		return EFail;	
		}

	// Set the email fields - check the email fields and the PID value (should 
	// be ESmsConvMail).
	iSmsHeader->SetEmailFieldsL(*fields1);
	const CSmsEmailFields& fields2 = iSmsHeader->EmailFields();
	iSmsHeader->GetSmsSettingsL(*smsset3);
	
	if(!CompareEmailFields(*fields1, fields2))
		{
		CleanupStack::PopAndDestroy(2,smsset3);
		return EFail;	
		}
	if(smsset3->MessageConversion()!=ESmsConvMail)
		{
		CleanupStack::PopAndDestroy(2,smsset3);
		return EFail;		
		}
	
	// Reset the fields and then set back in the header - should reset the PID
	// value to ESmsConvPIDNone
	fields1->Reset();
	iSmsHeader->SetEmailFieldsL(*fields1);
	
	const CSmsEmailFields& fields3 = iSmsHeader->EmailFields();
	iSmsHeader->GetSmsSettingsL(*smsset3);
	
	if(!CompareEmailFields(*fields1, fields3))
		{
		CleanupStack::PopAndDestroy(2,smsset3);
		return EFail;	
		}
	if(smsset3->MessageConversion()!=ESmsConvPIDNone)
		{
		CleanupStack::PopAndDestroy(2,smsset3);
		return EFail;	
		}
	
	// Set the reply email fields - check email fields (both address and subject)
	// and the PID value (should be ESmsConvMail)
	_LIT(KEmailReplySubjectFormat,	"Re: %S");
	_LIT(KEmailReplySubject,		"Re: a subject");

	// First test with no subject...
	fields1->AddAddressL(KTestAddress);
	
	iSmsHeader->SetReplyEmailFieldsL(*fields1, KEmailReplySubjectFormat);
	const CSmsEmailFields& replyFields1 = iSmsHeader->EmailFields();
	iSmsHeader->GetSmsSettingsL(*smsset3);
	
	if(!CompareEmailFieldsAddresses(replyFields1.Addresses(), fields1->Addresses()))
		{
		CleanupStack::PopAndDestroy(2,smsset3);
		return EFail;	
		}
	if(replyFields1.Subject().Compare(KNullDesC) != 0)
		{
		CleanupStack::PopAndDestroy(2,smsset3);
		return EFail;	
		}
	if(smsset3->MessageConversion()!=ESmsConvMail)
		{
		CleanupStack::PopAndDestroy(2,smsset3);
		return EFail;	
		}
	
	// Now set the subject...
	fields1->SetSubjectL(KTestSubject);

	iSmsHeader->SetReplyEmailFieldsL(*fields1, KEmailReplySubjectFormat);
	const CSmsEmailFields& replyFields2 = iSmsHeader->EmailFields();
	iSmsHeader->GetSmsSettingsL(*smsset3);
	
	if(!CompareEmailFieldsAddresses(replyFields2.Addresses(), fields1->Addresses()))
		{
		CleanupStack::PopAndDestroy(2,smsset3);
		return EFail;	
		}
	if(replyFields2.Subject().Compare(KEmailReplySubject()) != 0)
		{
		CleanupStack::PopAndDestroy(2,smsset3);
		return EFail;	
		}
	if(smsset3->MessageConversion()!=ESmsConvMail)
		{
		CleanupStack::PopAndDestroy(2,smsset3);
		return EFail;	
		}
	
	// Repeat - test for (lack of) Re: chain.
	fields1->SetSubjectL(KEmailReplySubject);
	
	iSmsHeader->SetReplyEmailFieldsL(*fields1, KEmailReplySubjectFormat);
	const CSmsEmailFields& replyFields3 = iSmsHeader->EmailFields();
	iSmsHeader->GetSmsSettingsL(*smsset3);
	
	if(!CompareEmailFieldsAddresses(replyFields3.Addresses(), fields1->Addresses()))
		{
		CleanupStack::PopAndDestroy(2,smsset3);
		return EFail;	
		}
	if(replyFields3.Subject().Compare(KEmailReplySubject()) != 0)
		{
		CleanupStack::PopAndDestroy(2,smsset3);
		return EFail;	
		}
	if(smsset3->MessageConversion()!=ESmsConvMail)
		{
		CleanupStack::PopAndDestroy(2,smsset3);
		return EFail;	
		}
	
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
	
	if(forwardFields1.HasAddress())
		{
		CleanupStack::PopAndDestroy(2,smsset3);
		return EFail;	
		}
	if(forwardFields1.Addresses().MdcaCount()!=0)
		{
		CleanupStack::PopAndDestroy(2,smsset3);
		return EFail;	
		}
	if(forwardFields1.Subject().Compare(KNullDesC) != 0)
		{
		CleanupStack::PopAndDestroy(2,smsset3);
		return EFail;	
		}
	if(smsset3->MessageConversion()!=ESmsConvMail)
		{
		CleanupStack::PopAndDestroy(2,smsset3);
		return EFail;	
		}

	// Now set the subject...
	fields1->SetSubjectL(KTestSubject);

	iSmsHeader->SetForwardEmailFieldsL(*fields1, KEmailForwardSubjectFormat);
	const CSmsEmailFields& forwardFields2 = iSmsHeader->EmailFields();
	iSmsHeader->GetSmsSettingsL(*smsset3);
	
	if(forwardFields2.HasAddress())
		{
		CleanupStack::PopAndDestroy(2,smsset3);
		return EFail;	
		}
	if(forwardFields2.Addresses().MdcaCount()!=0)
		{
		CleanupStack::PopAndDestroy(2,smsset3);
		return EFail;	
		}
	if(forwardFields2.Subject().Compare(KEmailForwardSubject()) != 0)
		{
		CleanupStack::PopAndDestroy(2,smsset3);
		return EFail;	
		}
	if(smsset3->MessageConversion()!=ESmsConvMail)
		{
		CleanupStack::PopAndDestroy(2,smsset3);
		return EFail;	
		}
	
	// Repeat - test for (lack of) Fw: chain.
	fields1->SetSubjectL(KEmailForwardSubject);
	
	iSmsHeader->SetForwardEmailFieldsL(*fields1, KEmailForwardSubjectFormat);
	const CSmsEmailFields& forwardFields3 = iSmsHeader->EmailFields();
	iSmsHeader->GetSmsSettingsL(*smsset3);
	
	if(forwardFields3.HasAddress())
		{
		CleanupStack::PopAndDestroy(2,smsset3);
		return EFail;	
		}
	if(forwardFields3.Addresses().MdcaCount()!=0)
		{
		CleanupStack::PopAndDestroy(2,smsset3);
		return EFail;	
		}
	if(forwardFields3.Subject().Compare(KEmailForwardSubject()) != 0)
		{
		CleanupStack::PopAndDestroy(smsset3);
		CleanupStack::PopAndDestroy(fields1);
		return EFail;	
		}
	if(smsset3->MessageConversion()!=ESmsConvMail)
		{
		CleanupStack::PopAndDestroy(2,smsset3);
		return EFail;	
		}

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
	if(store1->IsPresentL(KUidMsvSmsEmailFieldsStream))
		{
		CleanupStack::PopAndDestroy(store1);
		return EFail;	
		}
	CleanupStack::PopAndDestroy(store1);

	CSmsHeader* header1 = CSmsHeader::NewL(CSmsPDU::ESmsSubmit,*iRichText);
	CleanupStack::PushL(header1);

	RestoreHeaderL(*header1);

	//Values ok?
	if(iSmsHeader->Type()!=header1->Type())
		{
		CleanupStack::PopAndDestroy(header1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;
		}
	if(header1->ReplyPathProvided())
		{
		CleanupStack::PopAndDestroy(header1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;	
		}
	if(header1->ServiceCenterAddress()!=_L("+350508771010"))
		{
		CleanupStack::PopAndDestroy(header1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;	
		}
	if(iSmsHeader->Recipients().Count()!=header1->Recipients().Count())
		{
		CleanupStack::PopAndDestroy(header1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;	
		}
	if(iSmsHeader->BioMsgIdType()!=header1->BioMsgIdType())
		{
		CleanupStack::PopAndDestroy(header1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;	
		}
	if(iSmsHeader->ReplyAddressL()!=header1->ReplyAddressL())
		{
		CleanupStack::PopAndDestroy(header1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;	
		}

	// Lets get the setting of the message
	CSmsSettings* smsset4 = CSmsSettings::NewL();
	CleanupStack::PushL(smsset4);
	header1->GetSmsSettingsL(*smsset4);

	// And compare that they are the original values
	if(smsset4->CharacterSet()!=TSmsDataCodingScheme::ESmsAlphabet7Bit)
		{
		CleanupStack::PopAndDestroy(2,smsset4);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;	
		}
	if(smsset4->DeliveryReport())
		{
		CleanupStack::PopAndDestroy(2,smsset4);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;		
		}
	if(smsset4->MessageConversion()!=ESmsConvPIDNone)
		{
		CleanupStack::PopAndDestroy(2,smsset4);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;		
		}
	if(!smsset4->RejectDuplicate())
		{
		CleanupStack::PopAndDestroy(2,smsset4);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;		
		}
	if(smsset4->ReplyPath())
		{
		CleanupStack::PopAndDestroy(2,smsset4);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;		
		}
	if(smsset4->ValidityPeriod().Int()!=ESmsVP24Hours)
		{
		CleanupStack::PopAndDestroy(2,smsset4);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;		
		}
	if(smsset4->ValidityPeriodFormat()!=TSmsFirstOctet::ESmsVPFSemiOctet)
		{
		CleanupStack::PopAndDestroy(2,smsset4);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;		
		}
	
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
	if(!store2->IsPresentL(KUidMsvSmsEmailFieldsStream))
		{
		CleanupStack::PopAndDestroy(2,store2);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;	
		}
	CleanupStack::PopAndDestroy(store2);

	CSmsHeader* header2 = CSmsHeader::NewL(CSmsPDU::ESmsSubmit,*iRichText);
	CleanupStack::PushL(header2);

	RestoreHeaderL(*header2);

	if(iSmsHeader->Type()!=header2->Type())
		{
		CleanupStack::PopAndDestroy(2,store2);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;	
		}
	if(header2->ReplyPathProvided())
		{
		CleanupStack::PopAndDestroy(2,store2);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;	
		}
	if(header2->ServiceCenterAddress()!=_L("+350508771010"))
		{
		CleanupStack::PopAndDestroy(2,store2);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;	
		}
	if(iSmsHeader->Recipients().Count()!=header2->Recipients().Count())
		{
		CleanupStack::PopAndDestroy(2,store2);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;	
		}
	if(iSmsHeader->BioMsgIdType()!=header2->BioMsgIdType())
		{
		CleanupStack::PopAndDestroy(2,store2);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;	
		}
	if(iSmsHeader->ReplyAddressL()!=header2->ReplyAddressL())
		{
		CleanupStack::PopAndDestroy(2,store2);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;	
		}

	CSmsSettings* smsset5 = CSmsSettings::NewL();
	CleanupStack::PushL(smsset5);
	header2->GetSmsSettingsL(*smsset5);

	if(smsset5->CharacterSet()!=TSmsDataCodingScheme::ESmsAlphabet7Bit)
		{
		CleanupStack::PopAndDestroy(2,store2);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;	
		}
	if(smsset5->DeliveryReport())
		{
		CleanupStack::PopAndDestroy(2,store2);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;	
		}
	if(smsset5->MessageConversion()!=ESmsConvMail)
		{
		CleanupStack::PopAndDestroy(2,store2);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;	
		}
	if(!smsset5->RejectDuplicate())
		{
		CleanupStack::PopAndDestroy(2,store2);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;	
		}
	if(smsset5->ReplyPath())
		{
		CleanupStack::PopAndDestroy(2,store2);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;	
		}
	if(smsset5->ValidityPeriod().Int()!=ESmsVP24Hours)
		{
		CleanupStack::PopAndDestroy(2,store2);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;	
		}
	if(smsset5->ValidityPeriodFormat()!=TSmsFirstOctet::ESmsVPFSemiOctet)
		{
		CleanupStack::PopAndDestroy(2,store2);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;	
		}
	
	const CSmsEmailFields& fields9 = iSmsHeader->EmailFields();
	if(!CompareEmailFields(*fields8, fields9))
		{
		CleanupStack::PopAndDestroy(2,store2);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);
		return EFail;	
		}
	
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
	CleanupStack::Pop(email1);
	
	TRAPD(err,iSmsHeader = CSmsHeader::NewL(email1))
	if(err)
		{
		delete email1;	
		}

	// Check that the header is correct...
	// Email fields...
	const CSmsEmailFields& fields10 = iSmsHeader->EmailFields();
	if(!fields10.HasAddress())
		{
		CleanupStack::PopAndDestroy(&fs);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;
		}
	if(fields10.Addresses().MdcaCount()!=1)
		{
		CleanupStack::PopAndDestroy(&fs);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;	
		}
	if(fields10.Addresses().MdcaPoint(0).Compare(KTestEmailAddress1) != 0)
		{
		CleanupStack::PopAndDestroy(&fs);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;	
		}
	if(fields10.Subject().Compare(KTestEmailSubject1) != 0)
		{
		CleanupStack::PopAndDestroy(&fs);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;	
		}
	
	// Message body...
	CSmsBufferBase& body1 = iSmsHeader->Message().Buffer();
	TInt length1 = body1.Length();
	HBufC* buf1 = HBufC::NewLC(length1);
	TPtr bufPtr1(buf1->Des());
	body1.Extract(bufPtr1, 0, length1);
	
	if(bufPtr1.Compare(KTestEmailBody1) != 0)
		{
		CleanupStack::PopAndDestroy(2,buf1);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;	
		}
	
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
	if(fields11.HasAddress())
		{
		CleanupStack::PopAndDestroy(&fs);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;	
		}
	if(fields11.Addresses().MdcaCount()!=0)
		{
		CleanupStack::PopAndDestroy(&fs);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;	
		}
	if(fields11.Subject().Compare(KNullDesC) != 0)
		{
		CleanupStack::PopAndDestroy(&fs);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;	
		}
	if(fields11.Length() != 0)
		{
		CleanupStack::PopAndDestroy(&fs);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;	
		}
	
	// Message body...
	CSmsBufferBase& body2 = iSmsHeader->Message().Buffer();
	TInt length2 = body2.Length();
	HBufC* buf2 = HBufC::NewLC(length2);
	TPtr bufPtr2(buf2->Des());
	body2.Extract(bufPtr2, 0, length2);
	
	if(bufPtr2.Compare(KTestEmailBody2) != 0)
		{
		CleanupStack::PopAndDestroy(2,buf2);
		Session().CleanupEntryPop();
		Session().RemoveEntry(id);	
		return EFail;		
		}
	
//	delete email1;
	CleanupStack::PopAndDestroy(buf2);
	
	CleanupStack::PopAndDestroy(&fs);

	Session().CleanupEntryPop();
	Session().RemoveEntry(id);
	
	iTestStep.INFO_PRINTF1(_L("Test CSmsHeader Passed."));
	
	return EPass;
	}

_LIT(KGetDescriptionTestMessage, "Finland to win the ice hockey world champs");

#include <emsuserpromptie.h>
#include <emssoundie.h>
_LIT8(KUserMelody, 	"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
	  "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"); // Melody file

_LIT(KTestEmsVariousMsg1,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

TVerdict CSmutTest::TestSmsEmsExtensionsL()
// Test the modifications for EMS
	{
	
	iTestStep.INFO_PRINTF1(_L("Testing EMS Extensions..."));
	
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
	for(TInt i=0;i<emsElements.Count();i++)
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
		{
		iTestStep.ERR_PRINTF1(_L("Cannot set user prompt."));
		CleanupStack::PopAndDestroy(header);
		return EFail;
		}
	// Check we can clear it
	entry.SetUserPromptIndicator(EFalse);
	if(entry.UserPromptIndicator())
		{
		iTestStep.ERR_PRINTF1(_L("Cannot clear user prompt."));
		CleanupStack::PopAndDestroy(header);
		return EFail;	
		}

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
		iTestStep.ERR_PRINTF1(_L("TMsvSmsEntry::ProtocolIdentifier corrupt."));
		CleanupStack::PopAndDestroy(header);
		return EFail;
		}

	CleanupStack::PopAndDestroy(); //header
	
	iTestStep.INFO_PRINTF1(_L("Test EMS Extensions Passed."));
	
	return EPass;

	}

TVerdict CSmutTest::TestSmsUtilitiesL()
	{
	iTestStep.INFO_PRINTF1(_L("Testing TSmsUtilities...."));

	iRichText->Reset();
	iRichText->InsertL(0, KGetDescriptionTestMessage);
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsSubmit,*iRichText);
	CleanupStack::PushL(header);

	if(!TestSmsUtilities1L(*header))
		{
		CleanupStack::PopAndDestroy(header); //header
		return EFail;	
		}
		
	if(!TestSmsUtilities2L(*header))
		{
		CleanupStack::PopAndDestroy(header); //header
		return EFail;	
		}
		
	if(!TestSmsUtilities3L(*header))
		{
		CleanupStack::PopAndDestroy(header); //header
		return EFail;	
		}

	CleanupStack::PopAndDestroy(); //header
	
	iTestStep.INFO_PRINTF1(_L("Test TSmsUtilities Passed."));
	
	return EPass;
	}

TBool CSmutTest::TestSmsUtilities1L(CSmsHeader& aHeader)
/**
 * @test Tests TSmsUtilities::GetDescription()
 */
	{
	
	iTestStep.INFO_PRINTF1(_L("Testing TSmsUtilities::GetDescription() - use varying values to test trucation"));

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
		iTestStep.INFO_PRINTF2(_L("-> trucating to length %d"), length);

		desc.Zero();
		User::LeaveIfError(TSmsUtilities::GetDescription(aHeader.Message(), desc, length));

		iTestStep.INFO_PRINTF2(_L("-> GetDescription() : %S"), &desc);
		
		if(desc.Length()>length)
			{
			CleanupStack::PopAndDestroy(2,desBuf);
			return EFalse;	
			}
		if(desc != buf.Left(desc.Length()) ) //this won't work if buf contains CEditableText::EParagraphDelimiter
			{
			CleanupStack::PopAndDestroy(2,desBuf);
			return EFalse;		
			}
		}
	desc.Zero();

	//Test Special Message Indication

	iTestStep.INFO_PRINTF1(_L("Testing Special Message Indications..."));

	CSmsPDU& pdu = aHeader.Message().SmsPDU();
	CSmsUserData& userData = pdu.UserData();
	TBuf8<2> msgInd;
	msgInd.Copy(_L("AA"));

	//Add information element
	msgInd[0] = TSmsUtilities::EEmailMessageWaiting;
	msgInd[1] = 0x00000001; //1 message

	iTestStep.INFO_PRINTF1(_L("\tAdding information element (Email) to UD"));
	userData.AddInformationElementL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication, msgInd);
	User::LeaveIfError(TSmsUtilities::GetDescription(aHeader.Message(), desc));
	if(!CheckDescriptionL(resFile, R_MESSAGE_INDICATION_EMAIL_ONE, desc, 2))
		{
		CleanupStack::PopAndDestroy(2,desBuf);
		return EFalse;	
		}

	// Set the maximum length of desc to be less than the description.
	// The description should truncate to fit into desc.
	User::LeaveIfError(TSmsUtilities::GetDescription(aHeader.Message(), desc, 10));
	if(desc.Length() != 10) //Check that the Length of desc is the specified max length.
		{
		CleanupStack::PopAndDestroy(2,desBuf);
		return EFalse;	
		}

	if(!TestSpecialSmsMessageIndicationPduL(resFile, TSmsUtilities::EVoiceMessageWaiting, R_MESSAGE_INDICATION_VOICEMAIL_ONE))
		{
		CleanupStack::PopAndDestroy(2, &resFile);
		return EFalse;	
		}
	
	if(!TestSpecialSmsMessageIndicationPduL(resFile, TSmsUtilities::EFaxMessageWaiting, R_MESSAGE_INDICATION_FAX_ONE))
		{
		CleanupStack::PopAndDestroy(2, &resFile);
		return EFalse;	
		}
	if(!TestSpecialSmsMessageIndicationPduL(resFile, TSmsUtilities::EEmailMessageWaiting, R_MESSAGE_INDICATION_EMAIL_ONE))
		{
		CleanupStack::PopAndDestroy(2, &resFile);
		return EFalse;	
		}
	if(!TestSpecialSmsMessageIndicationPduL(resFile, TSmsUtilities::EOtherMessageWaiting, R_MESSAGE_INDICATION_OTHER_ONE))
		{
		CleanupStack::PopAndDestroy(2, &resFile);
		return EFalse;		
		}
			
	//Test video message waiting indication
	iTestStep.INFO_PRINTF1(_L("Testing video message waiting indications..."));	
	if(!TestVideoMessageIndicationPduL(resFile, aHeader, R_MESSAGE_INDICATION_VIDEOMESSAGE_ONE))
		{
		CleanupStack::PopAndDestroy(2, &resFile);
		return EFalse;		
		}
	
	//Test enhanced voice mail waiting indication
	iTestStep.INFO_PRINTF1(_L("Testing enhanced voice mail indications..."));	
	if(!TestEnhancedVoiceMailIndicationPduL(resFile, aHeader, R_MESSAGE_INDICATION_ENHANCED_VOICEMAIL_ONE))
		{
		CleanupStack::PopAndDestroy(2, &resFile);
		return EFalse;		
		}
	
	//Test multiple indications.
	iTestStep.INFO_PRINTF1(_L("Testing multiple indications..."));
	//Add a video message waiting notification (9 video messages)
	CSmsSpecialSMSMessageOperations& operations = STATIC_CAST(CSmsSpecialSMSMessageOperations&,aHeader.Message().GetOperationsForIEL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication));
	
	TBool toStore=ETrue;
	TUint8 messageCount=9;
	TUint8 totalMessageCount=messageCount;
	TSmsMessageIndicationType messageIndicationType=EGsmSmsExtendedMessageTypeWaiting;
	TExtendedSmsIndicationType extendedType=EGsmSmsVideoMessageWaiting;
	TSmsMessageProfileType messageProfileType=EGsmSmsProfileId1;
	
	operations.RemoveAllSpecialMessageIndicationsL();
	operations.AddSpecialMessageIndicationL(toStore,messageIndicationType,
								extendedType,messageProfileType,messageCount);
	//Add an enhanced voicemail notification (5 enhanced voice mails)
	CSmsEnhancedVoiceMailOperations& enhancedOperations = STATIC_CAST(CSmsEnhancedVoiceMailOperations&,aHeader.Message().GetOperationsForIEL(CSmsInformationElement::ESmsEnhanceVoiceMailInformation));
	enhancedOperations.RemoveEnhancedVoiceMailIEL();
	//Create CEnhancedVoiceMailNotification
	CEnhancedVoiceMailNotification* enhancedVoiceMailNotification=CEnhancedVoiceMailNotification::NewL();
	CleanupStack::PushL(enhancedVoiceMailNotification);
	messageCount=5;
	totalMessageCount+=messageCount;
	enhancedVoiceMailNotification->SetNumberOfVoiceMessages(messageCount);
	enhancedOperations.AddEnhancedVoiceMailIEL(*enhancedVoiceMailNotification);
	CleanupStack::PopAndDestroy(enhancedVoiceMailNotification);
	
	//Add an email message waiting notification (1 email)
	msgInd[0] = TSmsUtilities::EEmailMessageWaiting;
	msgInd[1] = 0x00000001; 
	userData.AddInformationElementL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication, msgInd);
	totalMessageCount++;
	
	//Add a fax waiting notification (2 faxes)
	msgInd[0] = TSmsUtilities::EFaxMessageWaiting;
	msgInd[1] = 0x00000002; 
	userData.AddInformationElementL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication, msgInd);
	totalMessageCount+=2;
	
	//Add a voice mail waiting notification (3 voice mails)
	msgInd[0] = TSmsUtilities::EVoiceMessageWaiting;
	msgInd[1] = 0x00000003; 
	userData.AddInformationElementL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication, msgInd);
	totalMessageCount+=3;
	
	User::LeaveIfError(TSmsUtilities::GetDescription(aHeader.Message(), desc));	
	if(!CheckDescriptionL(resFile, R_MESSAGE_INDICATION_OTHER_MANY, desc, totalMessageCount))
		{
		CleanupStack::PopAndDestroy(2, &resFile);
		return EFalse;	
		}
	iTestStep.INFO_PRINTF1(_L("Passed"));

	CleanupStack::PopAndDestroy(2, &resFile);
	
	return ETrue;
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

_LIT(KGetDetailsResult, "\t\tNum In: \"%S\" Name Out: \"%S\"");

TBool CSmutTest::TestSmsUtilities2L(CSmsHeader& aHeader)
	{
	//Test GetDetails

	iTestStep.INFO_PRINTF1(_L("Test TSmsUtilities::GetDetails()"));

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
	
	if(err != KErrNotFound && err != KErrNone)
		{
		return EFalse;	
		}

	iTestStep.INFO_PRINTF1(_L("\tTesting without contacts"));

	//Simple test
	TPtrC testTel(KMikkoTel);
	TBuf<KSmsDetailsLength> details;

	aHeader.Message().SetToFromAddressL(testTel);
	TSmsUtilities::GetDetails(iSmsTest.FileSession(), aHeader.Message(), details);
	iTestStep.INFO_PRINTF3(KGetDetailsResult, &testTel, &details);
	if(testTel.FindF(details) == KErrNotFound) //details should contain testTel as there is no contact.
		{
		return EFalse;	
		}

	if(!TestInvalidContact(testTel, details))
		{
		return EFalse;		
		}

	testTel.Set(KAnthonyTel);
	aHeader.Message().SetToFromAddressL(testTel);
	err = TSmsUtilities::GetDetails(iSmsTest.FileSession(), aHeader.Message(), details);
	iTestStep.INFO_PRINTF3(KGetDetailsResult, &testTel, &details);
	if(testTel.FindF(details) == KErrNotFound) //details should contain testTel as there is no contact.
		{
		return EFalse;		
		}
	if(err != KErrNone)
		{
		return EFalse;	
		}

	if(!TestInvalidContact(testTel, details))
		{
		return EFalse;		
		}

	// Testing with long telephone number. 
	// The telephone number should truncate to fit into details.
	if(!TestInvalidContact(KLongTel, details))
		{
		return EFalse;		
		}
	if(details.Length() != KSmsDetailsLength) //Length of details should be the default max length.
		{
		return EFalse;	
		}

	//Add contacts

	iTestStep.INFO_PRINTF1(_L("\tAdding Contacts"));

	//Adding good telephone numbers

	TContactItemId contId = iSmsTest.CreateContactL(KAnthonyFirst, KAnthonyLast, KAnthonyTel, KAnthonyEmail);
	if(contId == KNullContactId)
		{
		return EFalse;	
		}

	contId = iSmsTest.CreateContactL(KMikkoFirst, KMikkoLast, KMikkoTel, KMikkoEmail);
	if(contId == KNullContactId)
		{
		return EFalse;	
		}

	contId = iSmsTest.CreateContactL(KUnqualifiedFirst, KUnqualifiedLast, KUnqualifiedTel, KUnqualifiedEmail);
	if(contId == KNullContactId)
		{
		return EFalse;	
		}

	contId = iSmsTest.CreateContactL(KHarriFirst, KHarriLast, KHarriTel, KHarriEmail);
	if(contId == KNullContactId)
		{
		return EFalse;	
		}

	//Adding a duplicate telephone

	contId = iSmsTest.CreateContactL(KDupFirst1, KDupLast1, KDupTel, KNullDesC);
	if(contId == KNullContactId)
		{
		return EFalse;	
		}

	contId = iSmsTest.CreateContactL(KDupFirst2, KDupLast2, KDupTel, KNullDesC);
	if(contId == KNullContactId)
		{
		return EFalse;	
		}

	//Adding bad telephone numbers

	contId = iSmsTest.CreateContactL(KBadFirst, KBadLast, KBadTel1, KNullDesC);
	if(contId == KNullContactId)
		{
		return EFalse;	
		}

	contId = iSmsTest.CreateContactL(KBadFirst, KBadLast, KBadTel2, KNullDesC);
	if(contId == KNullContactId)
		{
		return EFalse;	
		}

	contId = iSmsTest.CreateContactL(KBadFirst, KBadLast, KBadTel3, KNullDesC);
	if(contId == KNullContactId)
		{
		return EFalse;	
		}

	contId = iSmsTest.CreateContactL(KBadFirst, KBadLast, KBadTel4, KNullDesC);
	if(contId == KNullContactId)
		{
		return EFalse;	
		}

	//Adding no name contact

	contId = iSmsTest.CreateContactL(KNullDesC, KNullDesC, KNoNameTel, KNullDesC);
	if(contId == KNullContactId)
		{
		return EFalse;	
		}

	iTestStep.INFO_PRINTF1(_L("\tTesting with valid telephone numbers"));

	if(!TestValidTel(KAnthonyTel, KAnthonyFirst, KAnthonyLast, KNullDesC, details))
		{
		return EFalse;	
		}
		
	if(!TestValidTel(KMikkoTel, KMikkoFirst, KMikkoLast, KNullDesC, details))
		{
		return EFalse;	
		}
	// Test below was added for INC040050. The number of digits matched
	// against in GetDetails changed and resulted in undesirable behaviour
	// when searching for an unqualified number with a fully qualified number.
	if(!TestValidTel(KUnqualifiedTel1, KUnqualifiedFirst, KUnqualifiedLast, KNullDesC, details))
		{
		return EFalse;	
		}
	if(!TestValidTel(KHarriTel, _L("Räisänen"), KNullDesC, _L("Harri"), details))
		{
		return EFalse;	
		}

	iTestStep.INFO_PRINTF1(_L("\tTesting with invalid telephone numbers"));

	if(!TestInvalidTelephone(KBadTel1, details))
		{
		return EFalse;	
		}
	if(!TestInvalidTelephone(KBadTel2, details))
		{
		return EFalse;		
		}
	if(!TestInvalidTelephone(KBadTel3, details))
		{
		return EFalse;		
		}
	if(!TestInvalidTelephone(KBadTel4, details))
		{
		return EFalse;		
		}

	iTestStep.INFO_PRINTF1(_L("\tTesting with duplicate telephone numbers"));

	if(!TestDuplicateContact(KDupTel, details))
		{
		return EFalse;	
		}

	iTestStep.INFO_PRINTF1(_L("\tTesting No Names telephone number"));

	if(!TestInvalidContact(KNoNameTel, details))
		{
		return EFalse;	
		}
	if(!TestInvalidContact(KDummy, details))
		{
		return EFalse;	
		}

	iTestStep.INFO_PRINTF1(_L("Passed"));
	
	return ETrue;
	}

TBool CSmutTest::TestValidTel(const TDesC& aTel, const TDesC& aExpect1, const TDesC& aExpect2, const TDesC& aNotExpect, TDes& rDetails)
	{
	TInt err = TSmsUtilities::GetDetails(iSmsTest.FileSession(), aTel, rDetails);
	iTestStep.INFO_PRINTF3(KGetDetailsResult, &aTel, &rDetails);

	if (aExpect1.Length())
		{
		if(rDetails.FindF(aExpect1) == KErrNotFound)
			{
			return EFalse;	
			}	
		}
		

	if (aExpect2.Length())
		{
		if(rDetails.FindF(aExpect2) == KErrNotFound)
			{
			return EFalse;	
			}
		}
		

	if (aNotExpect.Length())
		{
		if(rDetails.FindF(aNotExpect) != KErrNotFound)
			{
			return EFalse;		
			}	
		}
		
	if(err != KErrNone)
		{
		return EFalse;
		}
	
	return ETrue;
	}

TBool CSmutTest::TestInvalidTelephone(const TDesC& aTel, TDes& aDetails)
	{
	TInt err = TSmsUtilities::GetDetails(iSmsTest.FileSession(), aTel, aDetails);
	iTestStep.INFO_PRINTF3(KGetDetailsResult, &aTel, &aDetails);
	if(aTel.FindF(aDetails) == KErrNotFound)	//aDetails should contain aTel as aTel is invalid.
		{
		return EFalse;	
		}
	if(err != KErrNone)
		{
		return EFalse;	
		}
	
	return ETrue;
	}	

TBool CSmutTest::TestInvalidContact(const TDesC& aTel, TDes& aDetails)
	{
	TInt err = TSmsUtilities::GetDetails(iSmsTest.FileSession(), aTel, aDetails);
	iTestStep.INFO_PRINTF3(KGetDetailsResult, &aTel, &aDetails);
	
	if(aTel.FindF(aDetails) == KErrNotFound) //aDetails should contain aTel as the contact is invalid.
		{
		return EFalse;
		}
	if(err != KErrNone)
		{
		return EFalse;	
		}
	return ETrue;
	}

TBool CSmutTest::TestDuplicateContact(const TDesC& aTel, TDes& aDetails)
	{
	TInt err = TSmsUtilities::GetDetails(iSmsTest.FileSession(), aTel, aDetails);
	iTestStep.INFO_PRINTF3(KGetDetailsResult, &aTel, &aDetails);
	if(aTel.FindF(aDetails) == KErrNotFound) //aDetails should contain aTel as there is duplicate contact.
		{
		return EFalse;	
		}
	if(err != KErrNone)
		{
		return EFalse;	
		}
	
	return ETrue;
	}

TBool CSmutTest::TestSmsUtilities3L(CSmsHeader& aHeader)
	{
	iTestStep.INFO_PRINTF1(_L("Test TSmsUtilities::ServiceIdL()"));

	TMsvId id2 = KMsvNullIndexEntryId;
	// First tested function is ServiceIdL()
	TSmsUtilities::ServiceIdL(*iSmsTest.iMsvEntry, id2);
	if(id2 != iSmsTest.iSmsServiceId)
		{
		return EFalse;	
		}
	iTestStep.INFO_PRINTF1(_L("\tPassed"));

	iTestStep.INFO_PRINTF1(_L("Test TSmsUtilities::PopulateMsgEntry()"));
	TMsvEntry entry;
	TSmsUtilities::PopulateMsgEntry(entry,aHeader.Message(), iSmsTest.iSmsServiceId);
	// todo More tests
	iTestStep.INFO_PRINTF1(_L("\tPassed"));
	
	return ETrue;
	}

TBool CSmutTest::TestSpecialSmsMessageIndicationPduL(RResourceFile& aResFile, TSmsUtilities::TSmsUtilitiesSpecialMessageType aMessageType, TInt aId)
	{
	iTestStep.INFO_PRINTF2(_L("\tTestSpecialSmsMessageIndicationPduL Type=%d..."), aMessageType);

	const TUint8 KMaxCount = 0xFF;
	for (TUint8 i = 0; i < KMaxCount; i++)
		{
		TInt resId = aId;
		if (i != 1)
			resId++;

		if(!TestSpecialSmsMessageIndicationPduL(aResFile, STATIC_CAST(TUint8, aMessageType), i, resId))
			{
			return EFalse;	
			}
		if(!TestSpecialSmsMessageIndicationPduL(aResFile, STATIC_CAST(TUint8, aMessageType | 0x80), i, resId))
			{
			return EFalse;	
			}
		}

	iTestStep.INFO_PRINTF1(_L("Passed"));
	
	return ETrue;
	}

TBool CSmutTest::TestSpecialSmsMessageIndicationPduL(RResourceFile& aResFile, TUint8 aMessageType, TUint8 aMessageCount, TInt aId)
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
		return CheckDescriptionL(aResFile, aId, desc, aMessageCount);
	else
		return CheckDescriptionL(aResFile, aId, desc);
	}	
	
TBool CSmutTest::TestVideoMessageIndicationPduL(RResourceFile& aResFile, CSmsHeader& aHeader, TInt aId)
	{
	//remove any enhanced voice mail indications
	CSmsEnhancedVoiceMailOperations& enhancedOperations = STATIC_CAST(CSmsEnhancedVoiceMailOperations&,aHeader.Message().GetOperationsForIEL(CSmsInformationElement::ESmsEnhanceVoiceMailInformation));
	if(enhancedOperations.ContainsEnhancedVoiceMailIEL())
		{
		enhancedOperations.RemoveEnhancedVoiceMailIEL();	
		}

	const TUint8 KMaxCount = 0xFF;
	for (TUint8 i = 0; i < KMaxCount; i++)
		{
		TInt resId = aId;
		if (i != 1)
			resId++;

		if(!TestVideoMessageIndicationPduL(aResFile, i, aHeader, resId))
			{
			return EFalse;	
			}
		}

	iTestStep.INFO_PRINTF1(_L("Passed"));
	
	return ETrue;
	}
	

TBool CSmutTest::TestVideoMessageIndicationPduL(RResourceFile& aResFile, TUint8 aMessageCount, CSmsHeader& aHeader, TInt aId)
	{
	//Get the operations
	CSmsSpecialSMSMessageOperations& operations = STATIC_CAST(CSmsSpecialSMSMessageOperations&,aHeader.Message().GetOperationsForIEL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication));
	
	TBool toStore=ETrue;
	TSmsMessageIndicationType messageIndicationType=EGsmSmsExtendedMessageTypeWaiting;
	TExtendedSmsIndicationType extendedType=EGsmSmsVideoMessageWaiting;
	TSmsMessageProfileType messageProfileType=EGsmSmsProfileId1;
	TBuf<KSmsDescriptionLength> desc;
	
	operations.RemoveAllSpecialMessageIndicationsL();
	operations.AddSpecialMessageIndicationL(toStore,messageIndicationType,
								extendedType,messageProfileType,aMessageCount);
	
	User::LeaveIfError(TSmsUtilities::GetDescription(aHeader.Message(), desc));
	
	if(!CheckDescriptionL(aResFile, aId, desc, aMessageCount))
		{
		operations.RemoveAllSpecialMessageIndicationsL();
		return EFalse;	
		}
	operations.RemoveAllSpecialMessageIndicationsL();	
	return ETrue;
	}

TBool CSmutTest::TestEnhancedVoiceMailIndicationPduL(RResourceFile& aResFile, CSmsHeader& aHeader, TInt aId)
	{
	//remove any special message indications
	CSmsSpecialSMSMessageOperations& operations = STATIC_CAST(CSmsSpecialSMSMessageOperations&,aHeader.Message().GetOperationsForIEL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication));
	operations.RemoveAllSpecialMessageIndicationsL();
	
	const TUint8 KMaxCount = 0xFF;
	for (TUint8 i = 0; i < KMaxCount; i++)
		{
		TInt resId = aId;
		if (i != 1)
			resId++;

		if(!TestEnhancedVoiceMailIndicationPduL(aResFile, i, aHeader, resId))
			{
			return EFalse;	
			}
		}

	iTestStep.INFO_PRINTF1(_L("Passed"));
	
	return ETrue;
	}
	

TBool CSmutTest::TestEnhancedVoiceMailIndicationPduL(RResourceFile& aResFile, TUint8 aMessageCount, CSmsHeader& aHeader, TInt aId)
	{	
	//get the operations class
	CSmsEnhancedVoiceMailOperations& enhancedOperations = STATIC_CAST(CSmsEnhancedVoiceMailOperations&,aHeader.Message().GetOperationsForIEL(CSmsInformationElement::ESmsEnhanceVoiceMailInformation));
	//remove any enhanced voice mail indications
	if(enhancedOperations.ContainsEnhancedVoiceMailIEL())
		{
		enhancedOperations.RemoveEnhancedVoiceMailIEL();	
		}
	//Create CEnhancedVoiceMailNotification
	CEnhancedVoiceMailNotification* enhancedVoiceMailNotification=CEnhancedVoiceMailNotification::NewL();
	CleanupStack::PushL(enhancedVoiceMailNotification);

	//Set the properties
	TSmsMessageProfileType messageProfileType=EGsmSmsProfileId1;
	enhancedVoiceMailNotification->SetProfile(messageProfileType);
	enhancedVoiceMailNotification->SetStorage(ETrue);
	enhancedVoiceMailNotification->SetNumberOfVoiceMessages(aMessageCount);
	
	enhancedOperations.AddEnhancedVoiceMailIEL(*enhancedVoiceMailNotification);
	CleanupStack::PopAndDestroy(enhancedVoiceMailNotification);
	
	TBuf<KSmsDescriptionLength> desc;
	User::LeaveIfError(TSmsUtilities::GetDescription(aHeader.Message(), desc));
	if(!CheckDescriptionL(aResFile, aId, desc, aMessageCount))
		{
		return EFalse;	
		}
	
	
	return ETrue;
	}
	

TBool CSmutTest::CheckDescriptionL(RResourceFile& aResFile, TInt aId, TRefByValue<const TDesC> aDesc, ...)
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
		iTestStep.INFO_PRINTF3(_L("\tMISMATCH Desc \"%S\" Resource \"%S\""), &ref, &resDesc);
		return EFalse;
		}
		
	return ETrue;
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

TVerdict CSmutTest::TestSmsEntryL()
	{
	iTestStep.INFO_PRINTF1(_L("Testing TMsvSmsEntry..."));

	TMsvSmsEntry entry;
	if(!CheckSmsEntry(entry, EFalse, TSmsDataCodingScheme::ESmsClass0, 0, EFalse, TMsvSmsEntry::ENoAckSummary, EFalse, 0))
		{
		return EFail;	
		}

	entry.SetUserPromptIndicator(ETrue);
	if(!CheckSmsEntry(entry, EFalse, TSmsDataCodingScheme::ESmsClass0, 0, ETrue, TMsvSmsEntry::ENoAckSummary, EFalse, 0))
		{
		return EFail;	
		}
	entry.SetUserPromptIndicator(EFalse);
	if(!CheckSmsEntry(entry, EFalse, TSmsDataCodingScheme::ESmsClass0, 0, EFalse, TMsvSmsEntry::ENoAckSummary, EFalse, 0))
		{
		return EFail;	
		}
	entry.SetProtocolIdentifier(123);
	if(!CheckSmsEntry(entry, EFalse, TSmsDataCodingScheme::ESmsClass0, 123, EFalse, TMsvSmsEntry::ENoAckSummary, EFalse, 0))
		{
		return EFail;	
		}
	entry.SetProtocolIdentifier(100);
	entry.SetUserPromptIndicator(ETrue);
	if(!CheckSmsEntry(entry, EFalse, TSmsDataCodingScheme::ESmsClass0, 100, ETrue, TMsvSmsEntry::ENoAckSummary, EFalse, 0))
		{
		return EFail;	
		}
	entry.SetClass(ETrue, TSmsDataCodingScheme::ESmsClass0);
	if(!CheckSmsEntry(entry, ETrue, TSmsDataCodingScheme::ESmsClass0, 100, ETrue, TMsvSmsEntry::ENoAckSummary, EFalse, 0))
		{
		return EFail;	
		}
	entry.SetClass(ETrue, TSmsDataCodingScheme::ESmsClass1);
	if(!CheckSmsEntry(entry, ETrue, TSmsDataCodingScheme::ESmsClass1, 100, ETrue, TMsvSmsEntry::ENoAckSummary, EFalse, 0))
		{
		return EFail;	
		}
	entry.SetClass(ETrue, TSmsDataCodingScheme::ESmsClass2);
	if(!CheckSmsEntry(entry, ETrue, TSmsDataCodingScheme::ESmsClass2, 100, ETrue, TMsvSmsEntry::ENoAckSummary, EFalse, 0))
			{
		return EFail;	
		}
	entry.SetClass(ETrue, TSmsDataCodingScheme::ESmsClass3);
	if(!CheckSmsEntry(entry, ETrue, TSmsDataCodingScheme::ESmsClass3, 100, ETrue, TMsvSmsEntry::ENoAckSummary, EFalse, 0))
			{
		return EFail;	
		}
	entry.SetClass(EFalse);
	if(!CheckSmsEntry(entry, EFalse, TSmsDataCodingScheme::ESmsClass3, 100, ETrue, TMsvSmsEntry::ENoAckSummary, EFalse, 0))
		{
		return EFail;	
		}
		
	entry.SetUserPromptIndicator(EFalse);
	if(!CheckSmsEntry(entry, EFalse, TSmsDataCodingScheme::ESmsClass3, 100, EFalse, TMsvSmsEntry::ENoAckSummary, EFalse, 0))
			{
		return EFail;	
		}
	entry.SetClass(ETrue, TSmsDataCodingScheme::ESmsClass0);
	
	TInt classCounter, upiCounter, classDef, deliverySummary, messageIdValid;
	TSmsDataCodingScheme::TSmsClass tempClass;
	TMsvSmsEntry::TMsvSmsEntryAckSummary tempDelivery;
	TUint8 pidCounter;
	TInt32 messageId = KLogID1;

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
							if(!CheckSmsEntry(entry, classDef, tempClass, pidCounter, upiCounter, tempDelivery, messageIdValid, messageId))
								{
								return EFail;	
								}
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
							if(!CheckSmsEntry(entry, classDef, tempClass, pidCounter, upiCounter, tempDelivery, messageIdValid, messageId))
								{
								return EFail;	
								}
							}
						}
					}
				}
			}
		}

	iTestStep.INFO_PRINTF1(_L("Test TMsvSmsEntry Passed."));	
	return EPass;
	}
