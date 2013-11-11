// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name of test harness: T_MIUT05
// Component: IMCM
// Owner: KP
// Brief description of test harness:
// Tests storing/retrieving data from CImMimeHeader - most of the 
// functionality is tested - some new functions may not be tested!
// Detailed description of test harness:
// As above.
// Input files required to run test harness:
// None
// Intermediate files produced while running test harness:
// <DRIVE>:\msgtest\T_MIUT05\T_IMCM.log
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_MIUT05.<PLATFORM>.<VARIANT>.LOG.txt
// Description of how to build test harness:
// cd \msg\imcm\
// bldmake bldfiles
// abld test build
// Description of how to run test harness:
// The following instructions are the same for all platforms:
// 1. Build T_DB test harness from COMMDB component:
// cd \commdb\group
// bldmake bldfiles
// abld test build t_db
// 2. Build the test utilities:
// cd \msg\testutils\group\
// bldmake bldfiles
// abld build
// WINS running instructions:
// 1. \epoc32\release\wins\<VARIANT>\T_MIUT05.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_MIUT05.exe onto the other platform
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MSVTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\EMAILTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 4. Run T_MIUT05.exe on the other platform
// 
//

#include "emailtestutils.h"
#include "miuthdr.h"    // CImHeader
#include "miutlog.h" 
#include <msvstd.h>
#include <msvids.h>
#include <cemailaccounts.h>
#include <iapprefs.h>

// local variables etc //

RTest test(_L("MIUT05 Test CImMimeHeader class"));
LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D CImMimeHeader* imMimeHeader;
LOCAL_D CImMimeHeader* imMimeHeaderVerify;
LOCAL_D TMsvEntry* entry; 
LOCAL_D CImLog* log;
LOCAL_D CEmailTestUtils* testUtils;


//

#define KImRelativePathString _L8("..\\..\\")
#define KImailHeader1 _L8("TEXT")
#define KImailHeader2 _L8("PLAIN")
#define KImailHeader3 _L8("CHARSET")
#define KImailHeader4 _L8("US-ASCII")

#define KImailHeader5 _L8("<kjashd@jsdkjd.com")
#define KImailHeader6 _L("http:\\\\abc\\def\\hij\\klmn.html")
#define KImailHeader7 _L8("93245ihdflkjsd098325lksdf@09835.psion.com")
#define KImailHeader8 _L8("US-ASCII")
#define KImailHeader9 _L8("FILE")
#define KImailHeader10 _L8("ATTACHMENT")
#define KImailHeader11 _L8("CALENDAR")

#define KImailParam1 _L8("charset")
#define KImailParam2 _L8("ISO-8859-1")
#define KImailParam3 _L8("filename")
#define KImailParam4 _L8("ohio state.jpeg")
#define KImailParam5 _L8("START")
#define KImailParam6 _L8("0x1003")
#define KImailParam7 _L8("method")
#define KImailParam8 _L8("request")
#define KImailParam9 _L8("component")
#define KImailParam10 _L8("vevent")
#define KImailParam11 _L8("event.ics")
#define KMaxImMimeFieldLengthLong KMaxImMimeFieldLength+1

//

LOCAL_C void ResultOfTest(TBool aResult,TInt aTestNo)
	{
	if (aResult)
		{
		test.Printf(TRefByValue<const TDesC>_L("Passed test %d\n"),aTestNo);
		}
	else
		{
		test.Printf(TRefByValue<const TDesC>_L("\nFailed test %d\n"),aTestNo);
		}
	}

LOCAL_C void StoreMessageSettingsL(CImMimeHeader* aimMimeHeader)
	{
	// create a new TmsvEntry in the index
	CMsvStore* fileStore = testUtils->iMsvEntry->EditStoreL();
	CleanupStack::PushL(fileStore);
	aimMimeHeader->StoreL(*fileStore);
	fileStore->CommitL();
	CleanupStack::PopAndDestroy(); // filestore
	log->AppendComment(_L8("\tStored Message Settings"));
	}

LOCAL_C void RestoreMessageSettingsL(CImMimeHeader* aimMimeHeader)
	{
	aimMimeHeader->Reset();
	CMsvStore* fStore=testUtils->iMsvEntry->ReadStoreL();
	CleanupStack::PushL(fStore);
	aimMimeHeader->RestoreL(*fStore);
	CleanupStack::PopAndDestroy(); //fStore
	log->AppendComment(_L8("\tRestored Message Settings"));
	}

//

LOCAL_C void CreateFilledMimeHeader()
	{
	imMimeHeader->SetContentTypeL(KImailHeader1);
	imMimeHeader->SetContentSubTypeL(KImailHeader2);
	imMimeHeader->SetContentDispositionL(KImailHeader9);
	imMimeHeader->SetContentDescriptionL(KImailHeader1);
	imMimeHeader->SetContentBaseL(KImailHeader3);
	imMimeHeader->SetContentLocationL(KImailHeader6);
	imMimeHeader->SetContentIDL(KImailHeader4);
	imMimeHeader->SetRelativePathL(KImRelativePathString);
	// test the string arrays
	CDesC8Array& array=imMimeHeader->ContentTypeParams();
	array.AppendL(KImailParam1);
	array.AppendL(KImailParam2);
	array.AppendL(KImailParam3);
	array.AppendL(KImailParam4);

	CDesC8Array& arrayB=imMimeHeader->ContentDispositionParams();
	arrayB.AppendL(KImailParam3);
	arrayB.AppendL(KImailParam4);
	log->AppendComment(_L8("\tCreated Filled Mime Header"));
	}

LOCAL_C void CreateLongMimeHeader()
	{
	// a bunch of data for the header object....
	// all the fields are set to a length greater than 1000 i.e 1001

	TBuf8<KMaxImMimeFieldLengthLong> longValue;
	TBuf16<KMaxImMimeFieldLengthLong> longValue1;
	longValue.Fill('x',KMaxImMimeFieldLengthLong);
	longValue1.Fill('x',KMaxImMimeFieldLengthLong);

	imMimeHeader->SetContentTypeL(longValue);
	imMimeHeader->SetContentSubTypeL(longValue);
	imMimeHeader->SetContentDispositionL(longValue);
	imMimeHeader->SetContentDescriptionL(longValue);
	imMimeHeader->SetContentBaseL(longValue);
	imMimeHeader->SetContentLocationL(longValue1);
	imMimeHeader->SetContentIDL(longValue);
	imMimeHeader->SetRelativePathL(longValue);
	// test the string arrays
	CDesC8Array& array=imMimeHeader->ContentTypeParams();
	array.AppendL(longValue);
	array.AppendL(longValue);
	array.AppendL(longValue);
	array.AppendL(longValue);

	CDesC8Array& arrayB=imMimeHeader->ContentDispositionParams();
	arrayB.AppendL(longValue);
	arrayB.AppendL(longValue);
	log->AppendComment(_L8("\tCreated Long Mime Header"));
	}

LOCAL_C void CreateICalendarMimeHeader()
	{
	imMimeHeader->SetContentTypeL(KImailHeader1);
	imMimeHeader->SetContentSubTypeL(KImailHeader11);
	imMimeHeader->SetContentDispositionL(KImailHeader10);
	imMimeHeader->SetContentDescriptionL(KImailHeader1);
	imMimeHeader->SetContentBaseL(KImailHeader3);
	imMimeHeader->SetContentLocationL(KImailHeader6);
	imMimeHeader->SetContentIDL(KImailHeader4);
	imMimeHeader->SetRelativePathL(KImRelativePathString);
	// test the string arrays
	CDesC8Array& array=imMimeHeader->ContentTypeParams();
	array.AppendL(KImailParam7);
	array.AppendL(KImailParam8);
	array.AppendL(KImailParam9);
	array.AppendL(KImailParam10);
	array.AppendL(KImailParam1);
	array.AppendL(KImailParam2);

	CDesC8Array& arrayB=imMimeHeader->ContentDispositionParams();
	arrayB.AppendL(KImailParam3);
	arrayB.AppendL(KImailParam11);
	log->AppendComment(_L8("\tCreated ICalendar Mime Header"));
	}

LOCAL_C TBool CompareFilledMimeHeader(TInt aTest)
	{
	TBool result = ETrue;
	log->AppendComment(_L8("\tComparing Filled Mime Header"));
	TBool result1=((imMimeHeader->ContentType())==(imMimeHeaderVerify->ContentType()));
	if (!result1)
		log->AppendError(_L8("\t\tContent Types not identical"),-1);
	result&= result1;
	TBool result2=((imMimeHeader->ContentSubType())==(imMimeHeader->ContentSubType()));
	if (!result2)
		log->AppendError(_L8("\t\tContent SubTypes not identical"),-1);
	result&= result2;
	TBool result3=((imMimeHeader->ContentDisposition())==(imMimeHeader->ContentDisposition()));
	if (!result3)
		log->AppendError(_L8("\t\tContent Dispositions not identical"),-1);
	result&= result3;
	TBool result4=((imMimeHeader->ContentDescription())==(imMimeHeader->ContentDescription()));
	if (!result4)
		log->AppendError(_L8("\t\tContent Descriptions not identical"),-1);
	result&= result4;
	TBool result5=((imMimeHeader->ContentBase())==(imMimeHeader->ContentBase()));
	if (!result5)
		log->AppendError(_L8("\t\tContent Bases not identical"),-1);
	result&= result5;
	TBool result6=((imMimeHeader->ContentID())==(imMimeHeader->ContentID()));
	if (!result6)
		log->AppendError(_L8("\t\tContentIDs not identical"),-1);
	result&= result6;

	TBool result8=((imMimeHeader->ContentLocation())==(imMimeHeader->ContentLocation()));
	if (!result8)
		log->AppendError(_L8("\t\tContent Locations not identical"),-1);
	result&= result8;
	TBool result9=((imMimeHeader->RelativePath())==(imMimeHeader->RelativePath()));
	if (!result9)
		log->AppendError(_L8("\t\tRelative Paths not identical"),-1);
	result&=result9;

	TInt ii;
	TBufC8<100> oldStr;
	TBufC8<100>newStr;
	TBool result11=ETrue;
	for ( ii=0; ii<imMimeHeader->ContentTypeParams().Count(); ii++)
		{
		if (imMimeHeader->ContentTypeParams().Count()==imMimeHeaderVerify->ContentTypeParams().Count())
			{
			oldStr=imMimeHeader->ContentTypeParams()[ii];
			newStr=imMimeHeaderVerify->ContentTypeParams()[ii];
			result11&=((imMimeHeader->ContentTypeParams()[ii])==(imMimeHeaderVerify->ContentTypeParams()[ii]));
			}
		else
			result11&=EFalse;
		}
	if (!result11)
		log->AppendError(_L8("\t\tContent Type Parameters not identical"),-1);
	result&=result11;
	TBool result12=ETrue;
	for ( ii=0; ii<imMimeHeader->ContentDispositionParams().Count(); ii++)
		{
		if (imMimeHeader->ContentDispositionParams().Count()==imMimeHeaderVerify->ContentDispositionParams().Count())
			result12&=((imMimeHeader->ContentDispositionParams()[ii])==(imMimeHeaderVerify->ContentDispositionParams()[ii]));
		else
			result12&=EFalse;
		}
	if (!result12)
		log->AppendError(_L8("\t\tContent Disposition Parameters not identical"),-1);
	result&=result12;

	ResultOfTest(result,aTest);
	return result;
	}

LOCAL_C TBool CompareBlankMimeHeader(TInt aTest)
	{
	// need to check each of the fields of the stored header (imMimeHeaderVerify) are 
	// the same as that originally entered (imMimeHeader).  Here the length should be 
	// 0 so we don't actually need to compare!

	TBool result = ETrue;
	log->AppendComment(_L8("\tComparing Filled Mime Header"));
	TBool result1=((imMimeHeader->ContentType().Length()==0)==(imMimeHeaderVerify->ContentType().Length()==0));
	if (!result1)
		log->AppendError(_L8("\t\tContent Types not identical"),-1);
	result&=result1;
	TBool result2=((imMimeHeader->ContentSubType().Length()==0)==(imMimeHeader->ContentSubType().Length()==0));
	if (!result2)
		log->AppendError(_L8("\t\tContent SubTypes not identical"),-1);
	result&=result2;
	TBool result3=((imMimeHeader->ContentDisposition().Length()==0)==(imMimeHeader->ContentDisposition().Length()==0));
	if (!result3)
		log->AppendError(_L8("\t\tContent Dispositions not identical"),-1);
	result&=result3;
	TBool result4=((imMimeHeader->ContentDescription().Length()==0)==(imMimeHeader->ContentDescription().Length()==0));
	if (!result4)
		log->AppendError(_L8("\t\tContent Descriptions not identical"),-1);
	result&=result4;
	TBool result5=((imMimeHeader->ContentBase().Length()==0)==(imMimeHeader->ContentBase().Length()==0));
	if (!result5)
		log->AppendError(_L8("\t\tContent Bases not identical"),-1);
	result&=result5;
	TBool result6=((imMimeHeader->ContentID().Length()==0)==(imMimeHeader->ContentID().Length()==0));
	if (!result6)
		log->AppendError(_L8("\t\tContentIDs not identical"),-1);
	result&=result6;

	TBool result8=((imMimeHeader->ContentLocation().Length()==0)==(imMimeHeader->ContentLocation().Length()==0));
	if (!result8)
		log->AppendError(_L8("\t\tContent Locations not identical"),-1);
	result&=result8;
	TBool result9=((imMimeHeader->RelativePath().Length()==0)==(imMimeHeader->RelativePath().Length()==0));
	if (!result9)
		log->AppendError(_L8("\t\tRelative Paths not identical"),-1);
	result&=result9;

	TBool result10=((imMimeHeader->ContentTypeParams().Count())==(imMimeHeaderVerify->ContentTypeParams().Count()));
	if (!result10)
		log->AppendError(_L8("\t\tContent Type Parameters not identical"),-1);
	result&=result10;
	TBool result11=((imMimeHeader->ContentDispositionParams().Count())==(imMimeHeaderVerify->ContentDispositionParams().Count()));
	if (!result11)
		log->AppendError(_L8("\t\tContent Disposition Parameters not identical"),-1);
	result&=result11;
	
	ResultOfTest(result,aTest);

	return result;
	}


LOCAL_C TBool TestContentTypeParamCode(TInt aTest)
	{
	// search for an existing entry
	TPtrC8 param1 = imMimeHeader->GetContentTypeValue(KImailParam3);
	TBool result= KErrNone==param1.Compare(KImailParam4);

	TPtrC8 param2 = imMimeHeader->GetContentTypeValue(KImailParam1);
	result &= KErrNone==param2.Compare(KImailParam2);
	
	TPtrC8 unknownParam = imMimeHeader->GetContentTypeValue(KImailParam6);	// matches value, not a parameter
	result &= KErrNone != unknownParam.Compare(KImailParam2);

	
	TPtrC8 unknown2 = imMimeHeader->GetContentTypeValue(_L8("I don't exist"));	// parameter text is not in array
	result &= KErrNone != unknown2.Compare(KImailParam2);

	ResultOfTest(result,aTest);
	return result;
	}
	
LOCAL_C TBool TestICalendarContentTypeParamCode(TInt aTest)
	{
	// search for an existing entry
	TPtrC8 param1 = imMimeHeader->GetContentTypeValue(KImailParam7);
	TBool result= KErrNone==param1.Compare(KImailParam8);

	TPtrC8 param2 = imMimeHeader->GetContentTypeValue(KImailParam9);
	result &= KErrNone==param2.Compare(KImailParam10);
	
	TPtrC8 param3 = imMimeHeader->GetContentTypeValue(KImailParam1);
	result &= KErrNone==param3.Compare(KImailParam2);
	
	
	TPtrC8 unknownParam = imMimeHeader->GetContentTypeValue(KImailParam6);	// matches value, not a parameter
	result &= KErrNone != unknownParam.Compare(KImailParam2);

	TPtrC8 unknown2 = imMimeHeader->GetContentTypeValue(_L8("I don't exist"));	// parameter text is not in array
	result &= KErrNone != unknown2.Compare(KImailParam2);

	ResultOfTest(result,aTest);
	return result;
	}


LOCAL_C TBool CompareLongMimeHeader(TInt aTest)
	{
	TBool result = ETrue;
	log->AppendComment(_L8("\tComparing Long Mime Header"));
	TBool result1=((imMimeHeader->ContentType().Left(KMaxImMimeFieldLength))==(imMimeHeaderVerify->ContentType()));
	if (!result1)
		log->AppendError(_L8("\t\tContent Types not identical"),-1);
	result&=result1;
	TBool result2=((imMimeHeader->ContentSubType().Left(KMaxImMimeFieldLength))==(imMimeHeaderVerify->ContentSubType()));
	if (!result2)
		log->AppendError(_L8("\t\tContent SubTypes not identical"),-1);
	result&=result2;
	TBool result3=((imMimeHeader->ContentDisposition().Left(KMaxImMimeFieldLength))==(imMimeHeaderVerify->ContentDisposition()));
	if (!result3)
		log->AppendError(_L8("\t\tContent Dispositions not identical"),-1);
	result&=result3;
	TBool result4=((imMimeHeader->ContentDescription().Left(KMaxImMimeFieldLength))==(imMimeHeaderVerify->ContentDescription()));
	if (!result4)
		log->AppendError(_L8("\t\tContent Descriptions not identical"),-1);
	result&=result4;
	TBool result5=((imMimeHeader->ContentBase().Left(KMaxImMimeFieldLength))==(imMimeHeaderVerify->ContentBase()));
	if (!result5)
		log->AppendError(_L8("\t\tContent Bases not identical"),-1);
	result&=result5;
	TBool result6=((imMimeHeader->ContentID().Left(KMaxImMimeFieldLength))==(imMimeHeaderVerify->ContentID()));
	if (!result6)
		log->AppendError(_L8("\t\tContentIDs not identical"),-1);
	result&=result6;

	TBool result8=((imMimeHeader->ContentLocation().Left(KMaxImMimeFieldLength))==(imMimeHeaderVerify->ContentLocation()));
	if (!result8)
		log->AppendError(_L8("\t\tContent Locations not identical"),-1);
	result&=result8;
	TBool result9=((imMimeHeader->RelativePath().Left(KMaxImMimeFieldLength))==(imMimeHeaderVerify->RelativePath()));
	if (!result9)
		log->AppendError(_L8("\t\tRelative Paths not identical"),-1);
	result&=result9;

	TBool result10 = ETrue;
	TInt ii;
	TBuf8<KMaxImMimeFieldLengthLong> contentTypeParamsBuffer;
	for ( ii=0; ii<imMimeHeader->ContentTypeParams().Count(); ii++)
		{
		if (imMimeHeader->ContentTypeParams().Count()==imMimeHeaderVerify->ContentTypeParams().Count())
			{
			contentTypeParamsBuffer=(imMimeHeader->ContentTypeParams()[ii]);
			contentTypeParamsBuffer.SetLength(KMaxImMimeFieldLength);
			result10&=((contentTypeParamsBuffer)==(imMimeHeaderVerify->ContentTypeParams()[ii].Left(KMaxImMimeFieldLength)));
			}
		else
			result10&=EFalse;
		}
	if (!result10)
		log->AppendError(_L8("\t\tContent Type Parameters not identical"),-1);
	result&=result10;

	TBuf8<KMaxImMimeFieldLengthLong> contentDispositionParamsBuffer;
	TBool result11 = ETrue;
	for ( ii=0; ii<imMimeHeader->ContentDispositionParams().Count(); ii++)
		{
		if (imMimeHeader->ContentDispositionParams().Count()==imMimeHeaderVerify->ContentDispositionParams().Count())
			{
			contentDispositionParamsBuffer=(imMimeHeader->ContentDispositionParams()[ii]);
			contentDispositionParamsBuffer.SetLength(KMaxImMimeFieldLength);
			result11&=((contentDispositionParamsBuffer)==(imMimeHeaderVerify->ContentDispositionParams()[ii].Left(KMaxImMimeFieldLength)));
			}
		else
			result11&=EFalse;
		}
	if (!result11)
		log->AppendError(_L8("\t\tContent Disposition Parameters not identical"),-1);
	result&=result11;

	ResultOfTest(result,aTest);

	return result;
	}
	
LOCAL_C TBool CompareICalendarMimeHeader(TInt aTest)
	{
	TBool result = ETrue;
	log->AppendComment(_L8("\tComparing ICalendar Mime Header"));
	TBool result1=((imMimeHeader->ContentType())==(imMimeHeaderVerify->ContentType()));
	if (!result1)
		log->AppendError(_L8("\t\tContent Types not identical"),-1);
	result&= result1;
	TBool result2=((imMimeHeader->ContentSubType())==(imMimeHeaderVerify->ContentSubType()));
	if (!result2)
		log->AppendError(_L8("\t\tContent SubTypes not identical"),-1);
	result&= result2;
	TBool result3=((imMimeHeader->ContentDisposition())==(imMimeHeaderVerify->ContentDisposition()));
	if (!result3)
		log->AppendError(_L8("\t\tContent Dispositions not identical"),-1);
	result&= result3;
	TBool result4=((imMimeHeader->ContentDescription())==(imMimeHeaderVerify->ContentDescription()));
	if (!result4)
		log->AppendError(_L8("\t\tContent Descriptions not identical"),-1);
	result&= result4;
	TBool result5=((imMimeHeader->ContentBase())==(imMimeHeaderVerify->ContentBase()));
	if (!result5)
		log->AppendError(_L8("\t\tContent Bases not identical"),-1);
	result&= result5;
	TBool result6=((imMimeHeader->ContentID())==(imMimeHeaderVerify->ContentID()));
	if (!result6)
		log->AppendError(_L8("\t\tContentIDs not identical"),-1);
	result&= result6;

	TBool result8=((imMimeHeader->ContentLocation())==(imMimeHeaderVerify->ContentLocation()));
	if (!result8)
		log->AppendError(_L8("\t\tContent Locations not identical"),-1);
	result&= result8;
	TBool result9=((imMimeHeader->RelativePath())==(imMimeHeaderVerify->RelativePath()));
	if (!result9)
		log->AppendError(_L8("\t\tRelative Paths not identical"),-1);
	result&=result9;

	TInt ii;
	TBufC8<100> oldStr;
	TBufC8<100>newStr;
	TBool result11=ETrue;
	for ( ii=0; ii<imMimeHeader->ContentTypeParams().Count(); ii++)
		{
		if (imMimeHeader->ContentTypeParams().Count()==imMimeHeaderVerify->ContentTypeParams().Count())
			{
			oldStr=imMimeHeader->ContentTypeParams()[ii];
			newStr=imMimeHeaderVerify->ContentTypeParams()[ii];
			result11&=((imMimeHeader->ContentTypeParams()[ii])==(imMimeHeaderVerify->ContentTypeParams()[ii]));
			}
		else
			result11&=EFalse;
		}
	if (!result11)
		log->AppendError(_L8("\t\tContent Type Parameters not identical"),-1);
	result&=result11;
	TBool result12=ETrue;
	for ( ii=0; ii<imMimeHeader->ContentDispositionParams().Count(); ii++)
		{
		if (imMimeHeader->ContentDispositionParams().Count()==imMimeHeaderVerify->ContentDispositionParams().Count())
			{
			oldStr=imMimeHeader->ContentDispositionParams()[ii];
			newStr=imMimeHeaderVerify->ContentDispositionParams()[ii];
			result12&=((imMimeHeader->ContentDispositionParams()[ii])==(imMimeHeaderVerify->ContentDispositionParams()[ii]));
			}
		else
			result12&=EFalse;
		}
	if (!result12)
		log->AppendError(_L8("\t\tContent Disposition Parameters not identical"),-1);
	result&=result12;

	ResultOfTest(result,aTest);
	return result;
	}

//

LOCAL_C TBool FilledMimeHeader(TInt aTest)
	{
	log->AppendComment(_L8("\tFilled Mime Header Test (Test 1)"));
	
	entry = new(ELeave) TMsvEntry;
	entry->iDescription.Set(_L("Description"));
	entry->iDetails.Set(_L("Details"));
	entry->iType = KUidMsvMessageEntry;
	entry->iMtm = KUidMsgTypeIMAP4;
	entry->iServiceId = KMsvLocalServiceIndexEntryId;
	
	testUtils->iMsvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);
	testUtils->iMsvEntry->CreateL(*entry);
	TRAPD(error,testUtils->iMsvEntry->SetEntryL(entry->Id()));
	
	imMimeHeader=CImMimeHeader::NewLC();
	imMimeHeaderVerify=CImMimeHeader::NewLC();
	CreateFilledMimeHeader();
	StoreMessageSettingsL(imMimeHeader);
	RestoreMessageSettingsL(imMimeHeaderVerify);
	
	TBool result=CompareFilledMimeHeader(aTest);
	result &= TestContentTypeParamCode(aTest);

	delete entry;
	CleanupStack::PopAndDestroy(2);	//imMimeHeader, imMimeHeaderVerify

	return result;
	}

LOCAL_C TBool BlankMimeHeader(TInt aTest)
	{
	log->AppendComment(_L8("\tBlank Mime Header Test (Test 2)"));
	entry = new(ELeave) TMsvEntry;
	entry->iDescription.Set(_L("Description"));
	entry->iDetails.Set(_L("Details"));
	entry->iType = KUidMsvServiceEntry;
	imMimeHeader=CImMimeHeader::NewLC();
	imMimeHeaderVerify=CImMimeHeader::NewLC();
	StoreMessageSettingsL(imMimeHeader);
	RestoreMessageSettingsL(imMimeHeaderVerify);
	
	TBool result=CompareBlankMimeHeader(aTest);
	delete entry;
	CleanupStack::PopAndDestroy(2);	//imMimeHeader, imMimeHeaderVerify

	return result;
	}

LOCAL_C TBool LongMimeHeader(TInt aTest)
	{
	log->AppendComment(_L8("\tLong Mime Header Test (Test 3)"));
	entry = new(ELeave) TMsvEntry;
	entry->iDescription.Set(_L("Description"));
	entry->iDetails.Set(_L("Details"));
	entry->iType = KUidMsvMessageEntry;
	entry->iMtm = KUidMsgTypeIMAP4;
	entry->iServiceId = KMsvLocalServiceIndexEntryId;
	
	testUtils->iMsvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);
	testUtils->iMsvEntry->CreateL(*entry);
	TRAPD(error,testUtils->iMsvEntry->SetEntryL(entry->Id()));
	
	imMimeHeader=CImMimeHeader::NewLC();
	imMimeHeaderVerify=CImMimeHeader::NewLC();
	CreateLongMimeHeader();
	StoreMessageSettingsL(imMimeHeader);
	RestoreMessageSettingsL(imMimeHeaderVerify);
	
	TBool result=CompareLongMimeHeader(aTest);
	delete entry;
	CleanupStack::PopAndDestroy(2);	//imMimeHeader, imMimeHeaderVerify

	return result;
	}
	
LOCAL_C TBool ICalendarHeader(TInt aTest)
	{
	log->AppendComment(_L8("\tICalendar Mime Header Test (Test 1)"));
	entry = new(ELeave) TMsvEntry;
	entry->iDescription.Set(_L("Description"));
	entry->iDetails.Set(_L("Details"));
	entry->iType = KUidMsvServiceEntry;
	imMimeHeader=CImMimeHeader::NewLC();
	imMimeHeaderVerify=CImMimeHeader::NewLC();
	CreateICalendarMimeHeader();
	StoreMessageSettingsL(imMimeHeader);
	RestoreMessageSettingsL(imMimeHeaderVerify);
	
	TBool result=CompareICalendarMimeHeader(aTest);
	result &= TestICalendarContentTypeParamCode(aTest);

	delete entry;
	CleanupStack::PopAndDestroy(2);	//imMimeHeader, imMimeHeaderVerify

	return result;
	}

LOCAL_C TBool TestContentTransferEncoding(TInt aTest)
	{
	log->AppendComment(_L8("\tTesting Content Transfer Encoding (Test 4)"));

	CImMimeHeader* mimeHeader = CImMimeHeader::NewLC();
	CImMimeHeader* mimeHeaderVerify=CImMimeHeader::NewLC();
	TBool result = ETrue;
	
	mimeHeader->SetContentTransferEncodingL(_L8("base64"));
	StoreMessageSettingsL(mimeHeader);
	RestoreMessageSettingsL(mimeHeaderVerify);
	TBool result1 = (mimeHeader->ContentTransferEncoding() == mimeHeaderVerify->ContentTransferEncoding());
	if (!result1)
		log->AppendError(_L8("\t\tContent Transfer Encoding values not identical"),-1);
	result &= result1;
	mimeHeader->SetContentTransferEncodingL(_L8("quoted-printable"));
	StoreMessageSettingsL(mimeHeader);
	RestoreMessageSettingsL(mimeHeaderVerify);
	TBool result2 = (mimeHeader->ContentTransferEncoding() == mimeHeaderVerify->ContentTransferEncoding());
	if (!result2)
		log->AppendError(_L8("\t\tContent Transfer Encoding values not identical"),-1);
	result &= result2;
	mimeHeader->SetContentTransferEncodingL(_L8("7bit"));
	StoreMessageSettingsL(mimeHeader);
	RestoreMessageSettingsL(mimeHeaderVerify);
	TBool result3 = (mimeHeader->ContentTransferEncoding() == mimeHeaderVerify->ContentTransferEncoding());
	if (!result3)
		log->AppendError(_L8("\t\tContent Transfer Encoding values not identical"),-1);
	result &= result3;
	mimeHeader->SetContentTransferEncodingL(_L8("8bit"));
	StoreMessageSettingsL(mimeHeader);
	RestoreMessageSettingsL(mimeHeaderVerify);
	TBool result4 = (mimeHeader->ContentTransferEncoding() == mimeHeaderVerify->ContentTransferEncoding());
	if (!result4)
		log->AppendError(_L8("\t\tContent Transfer Encoding values not identical"),-1);
	result &= result4;
	mimeHeader->SetContentTransferEncodingL(_L8("binary"));
	StoreMessageSettingsL(mimeHeader);
	RestoreMessageSettingsL(mimeHeaderVerify);
	TBool result5 = (mimeHeader->ContentTransferEncoding() == mimeHeaderVerify->ContentTransferEncoding());
	if (!result5)
		log->AppendError(_L8("\t\tContent Transfer Encoding values not identical"),-1);
	result &= result5;
	mimeHeader->SetContentTransferEncodingL(_L8("uu"));
	StoreMessageSettingsL(mimeHeader);
	RestoreMessageSettingsL(mimeHeaderVerify);
	TBool result6 = (mimeHeader->ContentTransferEncoding() == mimeHeaderVerify->ContentTransferEncoding());
	if (!result6)
		log->AppendError(_L8("\t\tContent Transfer Encoding values not identical"),-1);
	result &= result6;

	CleanupStack::PopAndDestroy(2); // mimeHeader, mimeHeaderVerify

	ResultOfTest(result,aTest);

	return result;
	}

//

LOCAL_C void InitL()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);

	testUtils = CEmailTestUtils::NewLC(test);
	testUtils->CreateAllTestDirectories();
	testUtils->FileSession().SetSessionPath(_L("C:\\"));
	testUtils->CleanMessageFolderL();
	testUtils->GoServerSideL();

	log = CImLog::NewL(_L("c:\\logs\\email\\T_IMCM.log"), EAppend);
	CleanupStack::PushL(log);
	log->AppendComment(_L8("********** T_MIUT05 Test CImMimeHeader class **********"));
	TBuf8<80> buf;

#if defined(__WINS__)	
	buf.Append(_L8("WINS "));
#else
	buf.Append(_L8("MARM "));
#endif
#if defined(_UNICODE)
	buf.Append(_L8("U"));
#endif
#if defined(_DEBUG)
	buf.Append(_L8("DEB"));
#else
	buf.Append(_L8("REL"));
#endif
	log->AppendComment(buf);
	}
	
LOCAL_C void Closedown()
	{
	log->AppendComment(_L8("**********    T_MIUT05 Tests Complete    **********"));
	log->AppendComment(_L8(""));

	CleanupStack::PopAndDestroy(3);  //testUtils, log, scheduler
	}


#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
LOCAL_C void CreateAccount()
	{
	log->AppendComment(_L8("\tCreating email accounts..."));
	CEmailAccounts* accounts = CEmailAccounts::NewLC();
	CImImap4Settings* imapSetting = new (ELeave) CImImap4Settings;
	CImIAPPreferences* iap = CImIAPPreferences::NewLC();

	_LIT(KIMAPAccountName, "ImapAccount");
	accounts->PopulateDefaultImapSettingsL(*imapSetting, *iap);
	TImapAccount imapAccount = accounts->CreateImapAccountL(KIMAPAccountName, *imapSetting, *iap, EFalse);
	CleanupStack::PopAndDestroy(2);
	delete imapSetting;
	log->AppendComment(_L8("\tAccount Created succesfully."));
	}
	
#endif


//

LOCAL_C void doMainL()
	{
	InitL();
	testUtils->GoClientSideL();
	test.Printf(_L("Performing Header Tests"));

#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
	CreateAccount();
#endif
	
	TBool result = ETrue;
	testUtils->TestStart(1);
	TBool result1 = FilledMimeHeader(1);
	if (result1)
		log->AppendComment(_L8("PASSED Test 1"));
	else
		log->AppendError(_L8("FAILED Test 1"),-1);
	result&=result1;
	testUtils->TestFinish(1);

	testUtils->TestStart(2);
	TBool result2 = BlankMimeHeader(2);
	if (result2)
		log->AppendComment(_L8("PASSED Test 2"));
	else
		log->AppendError(_L8("FAILED Test 2"),-1);
	result&=result2;
	testUtils->TestFinish(2);

	testUtils->TestStart(3);
	TBool result3 = LongMimeHeader(3);
	if (result3)
		log->AppendComment(_L8("PASSED Test 3"));
	else
		log->AppendError(_L8("FAILED Test 3"),-1);
	result&=result3;
	testUtils->TestFinish(3);

	testUtils->TestStart(4);
	TBool result4 = TestContentTransferEncoding(4);
	if (result4)
		log->AppendComment(_L8("PASSED Test 4"));
	else
		log->AppendError(_L8("FAILED Test 4"),-1);
	result&=result4;
	testUtils->TestFinish(4);
	
	testUtils->TestStart(5);
	TBool result5 = ICalendarHeader(5);
	if (result5)
		log->AppendComment(_L8("PASSED Test 5"));
	else
		log->AppendError(_L8("FAILED Test 5"),-1);
	result&=result5;
	testUtils->TestFinish(5);

	if (result)
		{
		log->AppendComment(_L8("PASSED All tests Correctly"));
		test.Printf(_L("PASSED all Tests"));
		testUtils->TestHarnessCompleted();
		}
	else
		{
		test.Printf(_L("\nFAILED Tests"));
		log->AppendError(_L8("FAILED At Least one test"),-1);
		testUtils->TestHarnessFailed(KErrGeneral);
		}
	Closedown();
	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("MIUT05 Test CImMimeHeader class"));
	theCleanup=CTrapCleanup::New();
	TRAPD(ret,doMainL());		
	test(ret==KErrNone);
	delete theCleanup;	
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}
