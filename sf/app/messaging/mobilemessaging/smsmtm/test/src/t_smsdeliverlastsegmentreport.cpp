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


#include "t_smsdeliverlastsegmentreport.h"
#include <smss.rsg>
#include <etelmm.h>
#include "smstestutils.h"




GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("Setup"));
	theCleanup = CTrapCleanup::New();
	if(theCleanup == NULL)
		{
		return KErrNoMemory;			
		}
	TRAPD(ret,doMainL());
	delete theCleanup;	
	test.Printf(_L("Completed with return code %d"),ret);
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return ret;
	}

LOCAL_C void doMainL()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	CSmsTestUtils* smsTest = CSmsTestUtils::NewLC(test);
	TInt nextTest = 0;
	
	CSmsDeliverLastSegmentReport* lastSegReport = CSmsDeliverLastSegmentReport::NewLC(*smsTest, KNullDesC, nextTest);

	lastSegReport->StartL();

	CleanupStack::PopAndDestroy(3, scheduler);
	}

CSmsDeliverLastSegmentReport::CSmsDeliverLastSegmentReport(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest)
: CSmsTestBase(aSmsTest, aScriptFile, aNextTest)
	{
	}

void CSmsDeliverLastSegmentReport::RunAutoL()
	{
	// Testing settings (CSmsSettings)
	iSmsTest.TestStart(++iNextTest, _L("CSmsSettings"));  
	TestSmsSettingsL();
	iSmsTest.TestFinish(iNextTest);

	// Testing number class (CSmsHeader)
	iSmsTest.TestStart(++iNextTest, _L("CSmsHeader"));  
	TestSmsHeaderL();
	
	iSmsTest.TestFinish(iNextTest);

	}

CSmsDeliverLastSegmentReport::~CSmsDeliverLastSegmentReport()
	{
	delete iSmsHeader;
	delete iRichText;
	delete iParaLayer;
	delete iCharLayer;
	}


void CSmsDeliverLastSegmentReport::ConstructL()
	{
	iSmsTest.SetLogToFile();

	iSelection = new (ELeave) CMsvEntrySelection();
	iTimer = CTestTimer::NewL();

	iParaLayer =CParaFormatLayer::NewL();
	iCharLayer = CCharFormatLayer::NewL();
	iRichText = CRichText::NewL(iParaLayer, iCharLayer, CEditableText::EFlatStorage,256);
	
	CActiveScheduler::Add(this);
	}

CSmsDeliverLastSegmentReport* CSmsDeliverLastSegmentReport::NewLC(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest)
	{
	CSmsDeliverLastSegmentReport* self = new (ELeave) CSmsDeliverLastSegmentReport(aSmsTest, aScriptFile, aNextTest);
	CleanupStack::PushL(self);

	self->ConstructL();

	return self;
	}

void CSmsDeliverLastSegmentReport::ShowMenuL()
	{
	iSmsTest.ResetMenu();

	iSmsTest.AppendToMenuL(_L("Start"));
	RunAutoL();
	
	if (!iSmsTest.RunAuto())
		{
		iSmsTest.Printf(_L("Press any key to continue...\n"));
		iSmsTest.Test().Getch();
		}

	}

void CSmsDeliverLastSegmentReport::TestSmsSettingsL()
	{
	iSmsTest.Printf(_L("Testing Create Settings...\n"));

	iSmsTest.SetEntryL(iSmsTest.iSmsServiceId);
// First we test all the last segment delivery report feature of the class CSmsSettings
// We give it different values and compare if it works

	CSmsSettings* settings = CSmsSettings::NewLC();
		
	TestSettingLastSegmentDeliveryReport(*settings);

	iSmsTest.Printf(_L("Test Create Settings Passed\n"));

	CleanupStack::PopAndDestroy(settings);

}



void CSmsDeliverLastSegmentReport::TestSettingLastSegmentDeliveryReport(CSmsMessageSettings& aSettings)
	{

// Reject Duplicate 

// Delivery Report
	aSettings.SetLastSegmentDeliveryReport(ETrue); 
	iSmsTest(aSettings.LastSegmentDeliveryReport());
	aSettings.SetLastSegmentDeliveryReport(EFalse);
	iSmsTest(!aSettings.LastSegmentDeliveryReport());
	aSettings.SetLastSegmentDeliveryReport(ETrue);
	
	iSmsTest.Printf(_L("Test Last Segment Delivery Report Passed\n"));

// Can Concatenate (write messages with more than 160 characters)
	aSettings.SetCanConcatenate(ETrue);
	iSmsTest(aSettings.CanConcatenate());
	aSettings.SetCanConcatenate(EFalse);
	iSmsTest(aSettings.CanConcatenate()==EFalse);
	iSmsTest.Printf(_L("Test Can Concatenate Passed\n"));

	}




void CSmsDeliverLastSegmentReport::TestSmsHeaderL()
	{

// Testing class CSmsHeader
	iSmsTest.Test().Next(_L("Testing CSmsHeader"));
	
// First we test constructing ESmsDeliver
	delete iSmsHeader; // deleting the previous one
	iSmsHeader = NULL;
	iSmsHeader=CSmsHeader::NewL(CSmsPDU::ESmsDeliver,*iRichText); // constructing
	iSmsTest(iSmsHeader->Type()==CSmsPDU::ESmsDeliver);	// type ok?

	CSmsDeliver& deliver = iSmsHeader->Deliver();  // lets get the CSmsPDU of the CSmsHeader
	iSmsTest(deliver.Type() == CSmsPDU::ESmsDeliver); // Are the types ok?
	

// Type ESmsStatusReport
	delete iSmsHeader; // deleting the previous one
	iSmsHeader=CSmsHeader::NewL(CSmsPDU::ESmsStatusReport,*iRichText);
	iSmsTest(iSmsHeader->Type()==CSmsPDU::ESmsStatusReport);

	CSmsStatusReport& statusreport=iSmsHeader->StatusReport();
	iSmsTest(statusreport.Type() == CSmsPDU::ESmsStatusReport);

// Type ESmsSubmit
	delete iSmsHeader; // deleting the previous one
	iSmsHeader=CSmsHeader::NewL(CSmsPDU::ESmsSubmit,*iRichText);
	iSmsTest(iSmsHeader->Type()==CSmsPDU::ESmsSubmit);

	CSmsSubmit& submit=iSmsHeader->Submit();
	iSmsTest(submit.Type() == CSmsPDU::ESmsSubmit);

// Lets access the CSmsMessage of our header
	CSmsMessage& message=iSmsHeader->Message();
	iSmsTest(message.Type() == CSmsPDU::ESmsSubmit);

// Setting settings of the message
	CSmsSettings* smsSet = CSmsSettings::NewLC();
	smsSet->SetLastSegmentDeliveryReport(ETrue);
	
	// And copying them to the CSmsHeader
	iSmsHeader->SetSmsSettingsL(*smsSet);
	
	// Lets get the same setting from the CSmsHeader
	iSmsHeader->GetSmsSettingsL(*smsSet);
	
	// And compare that they are ok!
	iSmsTest(smsSet->LastSegmentDeliveryReport());
	
	CleanupStack::PopAndDestroy();
	}

