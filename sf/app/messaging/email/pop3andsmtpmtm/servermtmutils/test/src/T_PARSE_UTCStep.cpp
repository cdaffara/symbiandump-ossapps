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
// The test is that the time-stamps used for the entries in the message store are taken from the appropriate header, i.e. the received header where present, the date header failing that and the UTC time failing that.
// 
//

/**
 @SYMTestCaseID MSG-POP-0243
 @SYMTestType UT
 @SYMTestPriority High
 @SYMPREQ 234
 @SYMTestCaseDependencies RFC2822 files rfc_0001.txt, rfc_0002.txt, rfc_0003.txt, rfc_0004.txt and rfc_Embedded.txt
 @SYMTestCaseDesc This is an offline test that reads in and parses rfc2822 files and stores them in the message store.
 @SYMTestActions  Reads and parses the RFC2822 files, then tests the message store entry time-stamps.
 @SYMTestExpectedResults The timestamps of the message entries should be the same as in the ini file.
*/

#include <test/testexecutelog.h>
#include "T_PARSE_UTCStep.h"


_LIT(KPath,"c:\\msgtest\\IMUT\\rfc822\\parse\\UTC\\");
_LIT(KTestMessageDir,"c:\\msgtest\\IMUT\\rfc822\\parse\\");
RTest test(_L("UTC Parse Test"));

void DummyObserver::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/){}
 
LOCAL_C void FormatDateTime(TDes& aBuffer, const TTime& aTime)
	{ 
	aBuffer.Zero();
	_LIT(KFormat4,"%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%:3%+B");
	aTime.FormatL(aBuffer,KFormat4); 
	}
			
void CTestUTCParse::PrintEntryTime(TTime* aTIme)
	{
	TBuf<45> dateTimeString;
	FormatDateTime(dateTimeString,*aTIme); 
	INFO_PRINTF2(_L("%S"),&dateTimeString);	
	}

CTestUTCParse::~CTestUTCParse()
	{
	iTimeList->Reset();
	if(iTimeList)
		{
		delete iTimeList;
		}
	if(iTestUtils)
		{
		delete iTestUtils;
		}
	if(iScheduler)
		{
		delete iScheduler;
		}
	}
 
CTestUTCParse::CTestUTCParse()
	{
	SetTestStepName(KTestUTCParse);
	iMessageEntryCount=0;
	}
 
void CTestUTCParse::ConnectToMessageServerL()
	{
	// Connect to the Message Server...
	iTestUtils->GoClientSideL();
	iObserver = new (ELeave) DummyObserver();
	iSession = CMsvSession::OpenSyncL(*iObserver);		
	}
 
void CTestUTCParse::WriteStoreL()
	{
	//create the entry_structure file
	TParse parsedFileName;
	TFileName name(KTestMessageDir);
	iTestUtils->ResolveLogFile(name, parsedFileName);
	iTestUtils->FindChildrenL(KMsvRootIndexEntryIdValue, parsedFileName.FullName(), ETrue, EFalse);	
	}

TVerdict CTestUTCParse::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);	
	
	INFO_PRINTF1(_L("Changing Locale to UTC -7 hours 30 minutes...\r\n"));
	
	const TInt KOneHour 	= 3600;
	const TInt KSevenHours 	= 7*KOneHour;
	User::SetUTCOffset(-(KSevenHours + KOneHour/2));
	
	//initialisation
	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	iTestUtils = CEmailTestUtils::NewL(test);
	iTestUtils->CreateAllTestDirectories();
	iTestUtils->FileSession().SetSessionPath(_L("C:\\"));
	iTestUtils->CleanMessageFolderL();
	iTestUtils->GoServerSideL();
	iTimeList=new(ELeave)CArrayFixFlat<TTime>(20);

	return TestStepResult();
	}
 	
void CTestUTCParse::ReadDataL(CImRecvConvert* aRecvConvert, TFileName aFileName)
	{
	// open the file
	RFile file;

	TInt error = file.Open(iTestUtils->FileSession(), aFileName, EFileShareAny);
	if(error != KErrNone)
		{
		ERR_PRINTF2(TRefByValue<const TDesC>_L("\r\nFile open error %d"), error);
		User::Leave(KErrNotFound);
		}

	INFO_PRINTF2(TRefByValue<const TDesC>_L("Data from %s...\r\n"), aFileName.PtrZ());		

	// read the file into the conversion object
	HBufC8* lineBuffer = HBufC8::NewLC(1024);
	TPtr8 line = lineBuffer->Des();

	TBuf8<1> theChar;
	TBool finished = EFalse;

	aRecvConvert->ResetL();
	// supply a new attachment path since previous one is discarded by call to Reset()

	do {
		line.FillZ();
		line.SetLength(0);
		// compile a line one char at a time
		do {
			file.Read(theChar, 1);
			if(theChar.Length())
				line.Append(theChar);
			else
				finished = ETrue;
			// stop at the end of line or no more data
			} while(theChar.Length() && theChar[0] != 0x0A);
			if(!line.Length())
				break;
			
			aRecvConvert->ParseNextField(line); // This function doesnt actually leave, any leaves are relayed to the MessageCompleteL() function

		} while(!finished);

	TRAPD(err, aRecvConvert->MessageCompleteL());
	if(err!=KErrNone)
		{
		// At this point the message should be deleted and CImRecvConvert should be Reset()
		ERR_PRINTF3(TRefByValue<const TDesC>_L("\r\nError %d occured during the conversion of Message %d"), err, aRecvConvert->EntryId());
		}
	CleanupStack::PopAndDestroy(lineBuffer); 

	file.Close();
	}
 	
void CTestUTCParse::TestReadL(const TDesC& aDir, TFileName aFilename)
	{
	
	CImRecvConvert* recvConvert = CImRecvConvert::NewLC(iTestUtils->FileSession(), iTestUtils->iServerEntry, 
		KUidMsgTypePOP3, KMsvLocalServiceIndexEntryId);
	
	//Set the TMsvServerEntry to correct context 
	recvConvert->SetMsvId(KMsvGlobalOutBoxIndexEntryId); 

	aFilename.Insert(0, aDir);

	TRAPD(error,ReadDataL(recvConvert, aFilename)); 
	if (error)
		{
		ERR_PRINTF2(TRefByValue<const TDesC>_L("\r\nReadData Failed, Error %d"), error);
		}
	CleanupStack::PopAndDestroy(recvConvert);  
	}

 
void CTestUTCParse::ReadEmailL()

	{
	TInt counter=0;
	TInt numberOfFiles=0;
	TFileName name;
	TParse parsedFileName;
		
	parsedFileName.Set(KPath,NULL,NULL);

	CDir* rfcFileList;
	// Loads the any test files into an EntryList
	TInt error=iTestUtils->FileSession().GetDir(parsedFileName.FullName(), KEntryAttNormal, ESortByName, rfcFileList);
	if(!error)
		{
		numberOfFiles = rfcFileList->Count();
		INFO_PRINTF2(_L("Number Of Files=%d   1\n"), numberOfFiles);

		if(numberOfFiles)
			{
			do	{
				TFileName filename = (*rfcFileList)[counter].iName;
				TestReadL(parsedFileName.FullName(), filename);
				counter++;
				}
			while(counter<numberOfFiles);
			}
		else
			WARN_PRINTF1(_L("WARNING - There are no RFC files in test file source directory\n"));
		}
	else
		ERR_PRINTF2(TRefByValue<const TDesC>_L("\r\nFile error %d"), error);
	if(rfcFileList)
		{
		delete rfcFileList;
		}	
	}
 	
TVerdict CTestUTCParse::TestTime(TInt aCount,TTime* aTime)
	{
	
	TVerdict result=EFail;
	TTime currentUtcTime;
	TTimeIntervalMicroSeconds difference;
	TTime time;
	
	switch(aCount)
		{
	case 0:
		//the expected timestamp is Wed, 30 Apr 1997 10:12:18 +0100
		_LIT(KDateCorrect, "19970329:091218.000000"); 
		time.Set(KDateCorrect);	
		result = (time==*aTime? EPass:EFail);
		if(result==EFail) ERR_PRINTF2(_L("Test %d failed\n"), aCount+1);
	break;
			
	case 1:
		//there is no received header in this test so the date field should be used insted
		//the expected timestamp is Wed, 30 Apr 1997 10:02:38 +0100 (BST)
		_LIT(KDateCorrect55, "19970329:090238.000000"); 
		time.Set(KDateCorrect55);
		result = (time==*aTime? EPass:EFail);	
		if(result==EFail) ERR_PRINTF2(_L("Test %d failed\n"), aCount+1);	
	break;
	
	case 2:
		//the UTC offset in this test is invalid (+2500) and should be ignored
		//expected time-stamp is Wed, 30 Apr 1997 10:12:18 +2500
		_LIT(KDateCorrect2, "19970329:101218.000000"); 
		time.Set(KDateCorrect2);	
		result = (time==*aTime? EPass:EFail);		
		if(result==EFail) ERR_PRINTF2(_L("Test %d failed\n"), aCount+1);	
	break;
	
	case 3:
		//the UTC offset is invalid in this test and the current UTC time should be used to 
		//time-stamp the entry instead
		currentUtcTime.UniversalTime();
		difference=currentUtcTime.MicroSecondsFrom(*aTime);
		//the difference should be less than 1 second
		result = (difference<5000000? EPass:EFail);	
		if(result==EFail) ERR_PRINTF2(_L("Test %d failed\n"), aCount+1);	
	break;
	
	case 4:  
		//expected time-stamp is Thu, 3 Mar 2005 15:28:50 +0000
		_LIT(KDateCorrect3, "20050202:152850.000000"); 
		time.Set(KDateCorrect3);
		result = (time==*aTime? EPass:EFail);		
		if(result==EFail) ERR_PRINTF2(_L("Test %d failed\n"), aCount+1);
	break;
	
	case 5:
		//This is the time-stamp of an embedded entry 
		//expected time-stamp is Thu, 3 Mar 2005 15:27:12 +0000
		_LIT(KDateCorrect4, "20050202:152712.000000"); 
		time.Set(KDateCorrect4);
		result = (time==*aTime? EPass:EFail);	
		if(result==EFail) ERR_PRINTF2(_L("Test %d failed\n"), aCount+1);
	break;
	
	default:
	break;
			
		}
		return result;	
	}

 	
TVerdict CTestUTCParse::TestParsing()
	{
		
	TVerdict result=EPass;
	TInt count=iTimeList->Count();
	const TInt KNumEmailMessages=6;
	
	//test that we have the right number of time-stamps
	if(count!=KNumEmailMessages)
		{
		ERR_PRINTF1(_L("Wrong number of entries in the store."));
		return EFail;
		}
	
	//test that each time-stamp is as expected
	for(TInt i=0;i<count;i++)
		{
		result=TestTime(i,&iTimeList->At(i));
		if(result==EFail)
			break;
		}

	return result;
		
	}
 	
void CTestUTCParse::TraverseL(const TMsvId aTMsvId)
	{
	//open the entry, dont sort
	TMsvSelectionOrdering ordering;	
	ordering.SetSorting(EMsvSortByNone);
	CMsvEntry* entry = CMsvEntry::NewL(*iSession, aTMsvId,ordering);
	CleanupStack::PushL(entry);
	
	//retrieve the entries children
	CMsvEntrySelection* msvEntrySelection;
	msvEntrySelection=entry->ChildrenL();
	CleanupStack::PushL(msvEntrySelection);
	TInt count = entry->Count();
	
	CMsvEntry* child;
	TMsvEntry childDetails;

	//for each child test if its a message entry
	for(TInt i=0;i<count;i++)
		{
		child=iSession->GetEntryL((*msvEntrySelection)[i]);
		childDetails=child->Entry();
		
		if(childDetails.iType==KUidMsvMessageEntry)
			{
			iMessageEntryCount++;
			iTimeList->AppendL(childDetails.iDate);
			PrintEntryTime(&childDetails.iDate);
			}
		//recursive call
		TraverseL(childDetails.Id());
		if(child)
			{
			delete child;
			}
		}
			
	CleanupStack::PopAndDestroy(msvEntrySelection);
	CleanupStack::PopAndDestroy(entry);
		
	}
	
TVerdict CTestUTCParse::doTestStepL()
	{
	TVerdict result=EPass;	
	// read rfc2822 messages into the store
	ReadEmailL();
	WriteStoreL();
		
	ConnectToMessageServerL();
	TraverseL(KMsvGlobalOutBoxIndexEntryId);
	
	//test the parsing
	result=TestParsing();
	
	SetTestStepResult(result);		
	return TestStepResult();
	}

TVerdict CTestUTCParse::doTestStepPostambleL()
	{		
	return TestStepResult();
	}
	
	


