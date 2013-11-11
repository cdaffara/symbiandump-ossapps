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
// Name of test harness: T_MIUT06
// Component: IMCM
// Owner: KP
// Brief description of test harness:
// Tests CImLog and TImAttachmentFile.
// Detailed description of test harness:
// As above.
// Input files required to run test harness:
// None
// Intermediate files produced while running test harness:
// <DRIVE>:\msgtest\T_MIUT06\T_IMCM.log
// <DRIVE>:\msgtest\T_MIUT06\ImLog127.txt
// <DRIVE>:\msgtest\T_MIUT06\log.file
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_MIUT06.<PLATFORM>.<VARIANT>.LOG.txt
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
// 1. \epoc32\release\wins\<VARIANT>\T_MIUT06.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_MIUT06.exe onto the other platform
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MSVTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\EMAILTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 4. Run T_MIUT06.exe on the other platform
// 
//

#include "emailtestutils.h"
#include "MIUTATCH.H"	// TImAttachmentFile
#include "MIUTLOG.H"	// CImLog
#include <e32std.h>	// TTime
#include <msvids.h>
#include <e32math.h>

const TInt KMaxBuf = 1024;

LOCAL_D TBuf8<KMaxBuf+1> buffer(KMaxBuf+1);
LOCAL_D TBuf8<KMaxBuf+1> resultbuf;
LOCAL_D	TBuf8<KMaxBuf+1> temp;

LOCAL_D CImLog* log;
LOCAL_D CEmailTestUtils* testUtils;

RTest test(_L("MIUT06 Testrig for CImLog & TImAttachmentFile classes"));
LOCAL_D CTrapCleanup* theCleanup;

//

LOCAL_C void ResultOfTest(TBool aResult,TInt aTestNo)
	{
	if (aResult)
		{
		test.Printf(TRefByValue<const TDesC>_L("Passed test %d\n"),aTestNo);
		}
	else
		{
		test.Printf(TRefByValue<const TDesC>_L("Failed test %d\n"),aTestNo);
		}
	}

//

LOCAL_C TBool TestAttachmentClass1(TInt aTest)
	{
	// Read 10 chars
	log->AppendComment(_L8("\tAttachment Cache Test (Test 1)"));
	TBool result=ETrue;
	RFs s;
	TBool result1=(KErrNone==s.Connect());
	if (!result1)
		log->AppendError(_L8("\t\tError in connection"),-1);
	result&=result1;
	s.SetSessionPath(_L("c:\\"));

	TImAttachmentFile* filePtr=new (ELeave) TImAttachmentFile(s);
	CleanupDeletePushL(filePtr);
	TImAttachmentFile& file=*filePtr;


	s.Delete(_L("c:\\mailtest\\my test file1"));

	TBool result2=(KErrNone==file.CreateFile(_L("c:\\mailtest\\"),_L("my test file1")));
	if (!result2)
		log->AppendError(_L8("\t\tError creating file"),-1);
	TBuf8<10> buffer(10);
	result&=result2;
	buffer.Repeat(_L8("Hello World! "));

	TBool result3=(KErrNone==file.WriteFile(buffer));
	if (!result3)
		log->AppendError(_L8("\t\tError writing to file"),-1);
	result&=result3;
	TBool result4=(KErrNone==file.WriteFile(buffer));
	if (!result4)
		log->AppendError(_L8("\t\tError writing to file"),-1);
	result&=result4;
	file.CloseFile();

	TBool result5=(KErrNotFound==file.OpenFile(_L("c:\\mailtest\\"),_L("this file doesn't exist")));
	TBuf8<10> resultbuf;
	TBuf8<5> temp;
	result5&=(KErrNone==file.OpenFile(_L("c:\\mailtest\\"),_L("my test file1")));
	if (!result5)
		log->AppendError(_L8("\t\tError opening file"),-1);
	result&=result5;
	TBool result6=(KErrNone==file.ReadFile(resultbuf,5));
	if (!result6)
		log->AppendError(_L8("\t\tError reading file"),-1);
	result&=result6;
	TBool result7=(KErrNone==file.ReadFile(temp,5));
	if (!result7)
		log->AppendError(_L8("\t\tError reading file"),-1);
	result&=result7;

	resultbuf.Append(temp);
	TBool result8=(buffer.Compare(resultbuf)==KErrNone);
	if (!result8)
		log->AppendError(_L8("\t\tData read not identical to that written"),-1);
	result&=result8;

	file.CloseFile();
	file.DeleteAttachment(_L("c:\\mailtest\\"),_L("my test file1"));

	ResultOfTest(result,aTest);
	CleanupStack::PopAndDestroy(filePtr);
	return result;
	}


LOCAL_C TBool TestAttachmentClass2(TInt aTest)
	{
	// Read 511 chars
	log->AppendComment(_L8("\tAttachment Cache Test (Test 2)"));
	TBool result=ETrue;
	RFs s;
	TBool result1=(KErrNone==s.Connect());
	if (!result1)
		log->AppendError(_L8("\t\tError in connection"),-1);
	s.SetSessionPath(_L("c:\\"));
	result&=result1;

	TImAttachmentFile* filePtr=new (ELeave) TImAttachmentFile(s);
	CleanupDeletePushL(filePtr);
	TImAttachmentFile& file=*filePtr;
	
	s.Delete(_L("c:\\mailtest\\my test file2"));

	TBool result2=(KErrNone==file.CreateFile(_L("c:\\mailtest\\"),_L("my test file2")));
	if (!result2)
		log->AppendError(_L8("\t\tError creating file"),-1);
	result&=result2;

	buffer.Repeat(_L8("Hello World! "));
	
	TBool result3=(KErrNone==file.WriteFile(buffer));
	if (!result3)
		log->AppendError(_L8("\t\tError when writing to file"),-1);
	result&=result3;
	TBool result4=(KErrNone==file.WriteFile(buffer));
	if (!result4)
		log->AppendError(_L8("\t\tError when writing to file"),-1);
	result&=result4;

	file.CloseFile();

	TBool result5=(KErrNotFound==file.OpenFile(_L("c:\\mailtest\\"),_L("this file doesn't exist")));
	
	result5&=(KErrNone==file.OpenFile(_L("c:\\mailtest\\"),_L("my test file2")));
	if (!result5)
		log->AppendError(_L8("\t\tError opening file"),-1);
	result&=result5;
	TBool result6=(KErrNone==file.ReadFile(resultbuf,255));
	if (!result6)
		log->AppendError(_L8("\t\tError reading file"),-1);
	result&=result6;
	TBool result7=(KErrNone==file.ReadFile(temp,256));
	if (!result7)
		log->AppendError(_L8("\t\tError reading file"),-1);
	result&=result7;

	resultbuf.Append(temp);
	TBool result8=(buffer.Left(511).Compare(resultbuf)==KErrNone);
	if (!result8)
		log->AppendError(_L8("\t\tData read not identical to that written"),-1);
	result&=result8;
	file.CloseFile();

	file.DeleteAttachment(_L("c:\\mailtest\\"),_L("my test file2"));

	ResultOfTest(result,aTest);
	CleanupStack::PopAndDestroy(filePtr);
	return result;
	}

LOCAL_C TBool TestAttachmentClass3(TInt aTest)
	{
	// Read 512
	log->AppendComment(_L8("\tAttachment Cache Test (Test 3)"));
	// try writing data that is the same size as the cache (KDataLineBufferLength)
	TBool result=ETrue;
	RFs s;
	TBool result1=(KErrNone==s.Connect());
	if (!result1)
		log->AppendError(_L8("\t\tError in connection"),-1);
	result&=result1;
	s.SetSessionPath(_L("c:\\"));

	TImAttachmentFile* filePtr=new (ELeave) TImAttachmentFile(s);
	CleanupDeletePushL(filePtr);
	TImAttachmentFile& file=*filePtr;
	
	s.Delete(_L("c:\\mailtest\\my test file3"));

	TBool result2=(KErrNone==file.CreateFile(_L("c:\\mailtest\\"),_L("my test file3")));
	if (!result2)
		log->AppendError(_L8("\t\tError creating file"),-1);
	result&=result2;
	buffer.Repeat(_L8("Hello World! "));
	
	TBool result3=(KErrNone==file.WriteFile(buffer));
	if (!result3)
		log->AppendError(_L8("\t\tError writing to file"),-1);
	result&=result3;
	TBool result4=(KErrNone==file.WriteFile(buffer));
	if (!result4)
		log->AppendError(_L8("\t\tError writing to file"),-1);
	result&=result4;
	file.CloseFile();

	TBool result5=(KErrNotFound==file.OpenFile(_L("c:\\mailtest\\"),_L("this file doesn't exist")));

	result5&=(KErrNone==file.OpenFile(_L("c:\\mailtest\\"),_L("my test file3")));
	if (!result5)
		log->AppendError(_L8("\t\tError opening file"),-1);
	result&=result5;
	TBool result6=(KErrNone==file.ReadFile(resultbuf,256));
	if (!result6)
		log->AppendError(_L8("\t\tError reading file"),-1);
	result&=result6;
	TBool result7=(KErrNone==file.ReadFile(temp,256));
	if (!result7)
		log->AppendError(_L8("\t\tError reading file"),-1);
	result&=result7;

	resultbuf.Append(temp);
	TBool result8=(buffer.Left(512).Compare(resultbuf)==KErrNone);
	if (!result8)
		log->AppendError(_L8("\t\tData read not identical to that written"),-1);
	result&=result8;

	file.CloseFile();
	file.DeleteAttachment(_L("c:\\mailtest\\"),_L("my test file3"));

	ResultOfTest(result,aTest);
	CleanupStack::PopAndDestroy(filePtr);
	return result;
	}

LOCAL_C TBool TestAttachmentClass4(TInt aTest)
	{
	// Read 513
	log->AppendComment(_L8("\tAttachment Cache Test (Test 4)"));
	TBool result=ETrue;
	RFs s;
	TBool result1=(KErrNone==s.Connect());
	if (!result1)
		log->AppendError(_L8("\t\tError in connection"),-1);
	result&=result1;
	s.SetSessionPath(_L("c:\\"));

	TImAttachmentFile* filePtr=new (ELeave) TImAttachmentFile(s);
	CleanupDeletePushL(filePtr);
	TImAttachmentFile& file=*filePtr;
	
	s.Delete(_L("c:\\mailtest\\my test file4"));

	TBool result2=(KErrNone==file.CreateFile(_L("c:\\mailtest\\"),_L("my test file4")));
	if (!result2)
		log->AppendError(_L8("\t\tError creating file"),-1);
	result&=result2;
	buffer.Repeat(_L8("Hello World! "));
	
	TBool result3=(KErrNone==file.WriteFile(buffer));
	if (!result3)
		log->AppendError(_L8("\t\tError writing to file"),-1);
	result&=result3;
	TBool result4=(KErrNone==file.WriteFile(buffer));
	if (!result4)
		log->AppendError(_L8("\t\tError writing to file"),-1);
	result&=result4;
	file.CloseFile();

	TBool result5=(KErrNotFound==file.OpenFile(_L("c:\\mailtest\\"),_L("this file doesn't exist")));

	result5&=(KErrNone==file.OpenFile(_L("c:\\mailtest\\"),_L("my test file4")));
	if (!result5)
		log->AppendError(_L8("\t\tError opening file"),-1);
	result&=result5;
	TBool result6=(KErrNone==file.ReadFile(resultbuf,256));
	if (!result6)
		log->AppendError(_L8("\t\tError reading file"),-1);
	result&=result6;
	TBool result7=(KErrNone==file.ReadFile(temp,257));
	if (!result7)
		log->AppendError(_L8("\t\tError reading file"),-1);
	result&=result7;

	resultbuf.Append(temp);
	TBool result8=(buffer.Left(513).Compare(resultbuf)==KErrNone);
	if (!result8)
		log->AppendError(_L8("\t\tData read not identical to that written"),-1);
	result&=result8;

	file.CloseFile();
	file.DeleteAttachment(_L("c:\\mailtest\\"),_L("my test file4"));

	ResultOfTest(result,aTest);
	CleanupStack::PopAndDestroy(filePtr);
	return result;
	}

LOCAL_C TBool TestAttachmentClass5(TInt aTest)
	{
	//Read 1023
	log->AppendComment(_L8("\tAttachment Cache Test (Test 5)"));
	TBool result=ETrue;
	RFs s;
	TBool result1=(KErrNone==s.Connect());
	if (!result1)
		log->AppendError(_L8("\t\tError in connection"),-1);
	result&=result1;
	s.SetSessionPath(_L("c:\\"));

	TImAttachmentFile* filePtr=new (ELeave) TImAttachmentFile(s);
	CleanupDeletePushL(filePtr);
	TImAttachmentFile& file=*filePtr;
	
	s.Delete(_L("c:\\mailtest\\my test file5"));

	TBool result2=(KErrNone==file.CreateFile(_L("c:\\mailtest\\"),_L("my test file5")));
	if (!result2)
		log->AppendError(_L8("\t\tError creating file"),-1);
	result&=result2;
	buffer.Repeat(_L8("Hello World! "));
	
	TBool result3=(KErrNone==file.WriteFile(buffer));
	if (!result3)
		log->AppendError(_L8("\t\tError writing to file"),-1);
	result&=result3;
	TBool result4=(KErrNone==file.WriteFile(buffer));
	if (!result4)
		log->AppendError(_L8("\t\tError writing to file"),-1);
	result&=result4;
	file.CloseFile();

	TBool result5=(KErrNotFound==file.OpenFile(_L("c:\\mailtest\\"),_L("this file doesn't exist")));
	
	result5&=(KErrNone==file.OpenFile(_L("c:\\mailtest\\"),_L("my test file5")));if (!result)
		log->AppendError(_L8("\t\tError opening file"),-1);
	result&=result5;
	TBool result6=(KErrNone==file.ReadFile(resultbuf,511));
	if (!result6)
		log->AppendError(_L8("\t\tError reading file"),-1);
	result&=result6;
	TBool result7=(KErrNone==file.ReadFile(temp,512));
	if (!result7)
		log->AppendError(_L8("\t\tError reading file"),-1);
	result&=result7;

	resultbuf.Append(temp);
	TBool result8=(buffer.Left(1023).Compare(resultbuf)==KErrNone);
	if (!result8)
		log->AppendError(_L8("\t\tData read not identical to that written"),-1);
	result&=result8;

	file.CloseFile();
	file.DeleteAttachment(_L("c:\\mailtest\\"),_L("my test file5"));

	ResultOfTest(result,aTest);
	CleanupStack::PopAndDestroy(filePtr);
	return result;
	}

LOCAL_C TBool TestAttachmentClass6(TInt aTest)
	{
	log->AppendComment(_L8("\tAttachment Cache Test (Test 6)"));
	// try writing data that is twice the size of the cache 
	TBool result=ETrue;
	RFs s;
	TBool result1=(KErrNone==s.Connect());
	if (!result1)
		log->AppendError(_L8("\t\tError in connection"),-1);
	result&=result1;
	s.SetSessionPath(_L("c:\\"));

	TImAttachmentFile* filePtr=new (ELeave) TImAttachmentFile(s);
	CleanupDeletePushL(filePtr);
	TImAttachmentFile& file=*filePtr;
	
	s.Delete(_L("c:\\mailtest\\my test file6"));

	TBool result2=(KErrNone==file.CreateFile(_L("c:\\mailtest\\"),_L("my test file6")));
	if (!result2)
		log->AppendError(_L8("\t\tError creating file"),-1);
	result&=result2;
	buffer.Repeat(_L8("Hello World! "));
	
	TBool result3=(KErrNone==file.WriteFile(buffer));
	if (!result3)
		log->AppendError(_L8("\t\tError writing to file"),-1);
	result&=result3;
	TBool result4=(KErrNone==file.WriteFile(buffer));
	if (!result4)
		log->AppendError(_L8("\t\tError writing to file"),-1);
	result&=result4;
	file.CloseFile();

	TBool result5=(KErrNotFound==file.OpenFile(_L("c:\\mailtest\\"),_L("this file doesn't exist")));
	
	result5&=(KErrNone==file.OpenFile(_L("c:\\mailtest\\"),_L("my test file6")));
	if (!result5)
		log->AppendError(_L8("\t\tError opening file"),-1);
	result&=result5;
	TBool result6=(KErrNone==file.ReadFile(resultbuf,512));
	if (!result6)
		log->AppendError(_L8("\t\tError reading file"),-1);
	result&=result6;
	TBool result7=(KErrNone==file.ReadFile(temp,512));
	if (!result7)
		log->AppendError(_L8("\t\tError reading file"),-1);
	result&=result7;

	resultbuf.Append(temp);
	TBool result8=(buffer.Left(1024).Compare(resultbuf)==KErrNone);
	if (!result8)
		log->AppendError(_L8("\t\tData read not identical to that written"),-1);
	result&=result8;

	file.CloseFile();
	file.DeleteAttachment(_L("c:\\mailtest\\"),_L("my test file6"));

	ResultOfTest(result,aTest);
	CleanupStack::PopAndDestroy(filePtr);
	return result;
	}

LOCAL_C TBool TestAttachmentClass7(TInt aTest)
	{
	// Read 1024
	log->AppendComment(_L8("\tAttachment Cache Test (Test 7)"));
	TBool result=ETrue;
	RFs s;
	TBool result1=(KErrNone==s.Connect());
	if (!result1)
		log->AppendError(_L8("\t\tError in connection"),-1);
	result&=result1;
	s.SetSessionPath(_L("c:\\"));

	TImAttachmentFile* filePtr=new (ELeave) TImAttachmentFile(s);
	CleanupDeletePushL(filePtr);
	TImAttachmentFile& file=*filePtr;
	
	s.Delete(_L("c:\\mailtest\\my test file7"));

	TBool result2=(KErrNone==file.CreateFile(_L("c:\\mailtest\\"),_L("my test file7")));
	if (!result2)
		log->AppendError(_L8("\t\tError creating file"),-1);
	result&=result2;
	buffer.Repeat(_L8("Hello World! "));
	
	TBool result3=(KErrNone==file.WriteFile(buffer));
	if (!result3)
		log->AppendError(_L8("\t\tError writing to file"),-1);
	result&=result3;
	TBool result4=(KErrNone==file.WriteFile(buffer));
	if (!result4)
		log->AppendError(_L8("\t\tError writing to file"),-1);
	result&=result4;
	file.CloseFile();

	TBool result5=(KErrNotFound==file.OpenFile(_L("c:\\mailtest\\"),_L("this file doesn't exist")));
	
	result5&=(KErrNone==file.OpenFile(_L("c:\\mailtest\\"),_L("my test file7")));
	if (!result5)
		log->AppendError(_L8("\t\tError opening file"),-1);
	result&=result5;
	TBool result6=(KErrNone==file.ReadFile(resultbuf,24));
	if (!result6)
		log->AppendError(_L8("\t\tError reading file"),-1);
	result&=result6;
	TBool result7=(KErrNone==file.ReadFile(temp,1000));
	if (!result7)
		log->AppendError(_L8("\t\tError reading file"),-1);
	result&=result7;

	resultbuf.Append(temp);
	TBool result8=(buffer.Left(1024).Compare(resultbuf)==KErrNone);
	if (!result8)
		log->AppendError(_L8("\t\tData read not identical to that written"),-1);
	result&=result8;

	file.CloseFile();
	file.DeleteAttachment(_L("c:\\mailtest\\"),_L("my test file7"));

	ResultOfTest(result,aTest);
	CleanupStack::PopAndDestroy(filePtr);
	return result;
	}

LOCAL_C TBool TestAttachmentClass8(TInt aTest)
	{
	// Read 1024
	log->AppendComment(_L8("\tAttachment Cache Test (Test 8)"));
	TBool result=ETrue;
	RFs s;
	TBool result1=(KErrNone==s.Connect());
	if (!result1)
		log->AppendError(_L8("\t\tError in connection"),-1);
	result&=result1;
	s.SetSessionPath(_L("c:\\"));

	TImAttachmentFile* filePtr=new (ELeave) TImAttachmentFile(s);
	CleanupDeletePushL(filePtr);
	TImAttachmentFile& file=*filePtr;

	s.Delete(_L("c:\\mailtest\\my test file8"));

	TBool result2=(KErrNone==file.CreateFile(_L("c:\\mailtest\\"),_L("my test file8")));
	if (!result2)
		log->AppendError(_L8("\t\tError creating file"),-1);
	result&=result2;
	buffer.Repeat(_L8("Hello World! "));
	
	TBool result3=(KErrNone==file.WriteFile(buffer));
	if (!result3)
		log->AppendError(_L8("\t\tError writing to file"),-1);
	result&=result3;
	TBool result4=(KErrNone==file.WriteFile(buffer));
	if (!result4)
		log->AppendError(_L8("\t\tError writing to file"),-1);
	result&=result4;
	file.CloseFile();

	TBool result5=(KErrNotFound==file.OpenFile(_L("c:\\mailtest\\"),_L("this file doesn't exist")));
	
	result5&=(KErrNone==file.OpenFile(_L("c:\\mailtest\\"),_L("my test file8")));
	if (!result5)
		log->AppendError(_L8("\t\tError opening file"),-1);
	result&=result5;
	TBool result6=(KErrNone==file.ReadFile(resultbuf,1000));
	if (!result6)
		log->AppendError(_L8("\t\tError reading file"),-1);
	result&=result6;
	TBool result7=(KErrNone==file.ReadFile(temp,24));
	if (!result7)
		log->AppendError(_L8("\t\tError reading file"),-1);
	result&=result7;

	resultbuf.Append(temp);
	TBool result8=(buffer.Left(1024).Compare(resultbuf)==KErrNone);
	if (!result8)
		log->AppendError(_L8("\t\tData read not identical to that written"),-1);
	result&=result8;

	file.CloseFile();
	file.DeleteAttachment(_L("c:\\mailtest\\"),_L("my test file8"));

	ResultOfTest(result,aTest);
	CleanupStack::PopAndDestroy(filePtr);
	return result;
	}

LOCAL_C TBool TestAttachmentClass9(TInt aTest)
	{
	//Read 1025
	log->AppendComment(_L8("\tAttachment Cache Test (Test 9)"));
	TBool result=ETrue;
	RFs s;
	TBool result1=(KErrNone==s.Connect());
	if (!result1)
		log->AppendError(_L8("\t\tError in connection"),-1);
	result&=result1;
	s.SetSessionPath(_L("c:\\"));

	TImAttachmentFile* filePtr=new (ELeave) TImAttachmentFile(s);
	CleanupDeletePushL(filePtr);
	TImAttachmentFile& file=*filePtr;
	
	s.Delete(_L("c:\\mailtest\\my test file9"));

	TBool result2=(KErrNone==file.CreateFile(_L("c:\\mailtest\\"),_L("my test file9")));
	if (!result2)
		log->AppendError(_L8("\t\tError creating file"),-1);
	result&=result2;
	buffer.Repeat(_L8("Hello World! "));
	
	TBool result3=(KErrNone==file.WriteFile(buffer));
	if (!result3)
		log->AppendError(_L8("\t\tError writing to file"),-1);
	result&=result3;
	TBool result4=(KErrNone==file.WriteFile(buffer));
	if (!result4)
		log->AppendError(_L8("\t\tError writing to file"),-1);
	result&=result4;
	file.CloseFile();

	TBool result5=(KErrNotFound==file.OpenFile(_L("c:\\mailtest\\"),_L("this file doesn't exist")));
	
	result5&=(KErrNone==file.OpenFile(_L("c:\\mailtest\\"),_L("my test file9")));
	if (!result5)
		log->AppendError(_L8("\t\tError opening file"),-1);
	result&=result5;
	TBool result6=(KErrNone==file.ReadFile(resultbuf,1000));
	if (!result6)
		log->AppendError(_L8("\t\tError reading file"),-1);
	result&=result6;
	TBool result7=(KErrNone==file.ReadFile(temp,25));
	if (!result7)
		log->AppendError(_L8("\t\tError reading file"),-1);
	result&=result7;

	resultbuf.Append(temp);
	TBool result8=(buffer.Compare(resultbuf)==KErrNone);
	if (!result8)
		log->AppendError(_L8("\t\tData read not identical to that written"),-1);
	result&=result8;

	file.CloseFile();
	file.DeleteAttachment(_L("c:\\mailtest\\"),_L("my test file9"));

	ResultOfTest(result,aTest);
	CleanupStack::PopAndDestroy(filePtr);
	return result;
	}

LOCAL_C TBool TestAttachmentClass10(TInt aTest)
	{
	// Read 1025
	log->AppendComment(_L8("\tAttachment Cache Test (Test 10)"));
	TBool result=ETrue;
	RFs s;
	TBool result1=(KErrNone==s.Connect());
	if (!result1)
		log->AppendError(_L8("\t\tError in connection"),-1);
	result&=result1;
	s.SetSessionPath(_L("c:\\"));

	TImAttachmentFile* filePtr=new (ELeave) TImAttachmentFile(s);
	CleanupDeletePushL(filePtr);
	TImAttachmentFile& file=*filePtr;
	
	s.Delete(_L("c:\\mailtest\\my test file10"));

	TBool result2=(KErrNone==file.CreateFile(_L("c:\\mailtest\\"),_L("my test file10")));
	if (!result2)
		log->AppendError(_L8("\t\tError creating file"),-1);
	result&=result2;
	buffer.Repeat(_L8("Hello World! "));
	
	TBool result3=(KErrNone==file.WriteFile(buffer));
	if (!result3)
		log->AppendError(_L8("\t\tError writing to file"),-1);
	result&=result3;
	TBool result4=(KErrNone==file.WriteFile(buffer));
	if (!result4)
		log->AppendError(_L8("\t\tError writing to file"),-1);
	result&=result4;
	file.CloseFile();

	TBool result5=(KErrNotFound==file.OpenFile(_L("c:\\mailtest\\"),_L("this file doesn't exist")));
	
	result5&=(KErrNone==file.OpenFile(_L("c:\\mailtest\\"),_L("my test file10")));
	if (!result5)
		log->AppendError(_L8("\t\tError opening file"),-1);
	result&=result5;
	TBool result6=(KErrNone==file.ReadFile(resultbuf,25));
	if (!result6)
		log->AppendError(_L8("\t\tError reading file"),-1);
	result&=result6;
	TBool result7=(KErrNone==file.ReadFile(temp,1000));
	if (!result7)
		log->AppendError(_L8("\t\tError reading file"),-1);
	result&=result7;

	resultbuf.Append(temp);
	TBool result8=(buffer.Compare(resultbuf)==KErrNone);
	if (!result8)
		log->AppendError(_L8("\t\tData read not identical to that written"),-1);
	result&=result8;

	file.CloseFile();
	file.DeleteAttachment(_L("c:\\mailtest\\"),_L("my test file10"));

	ResultOfTest(result,aTest);
	CleanupStack::PopAndDestroy(filePtr);
	return result;
	}

LOCAL_C TBool TestAttachmentClass11(TInt aTest)
	{
	// Read 1025
	log->AppendComment(_L8("\tAttachment Cache Test (Test 11)"));
	TBool result=ETrue;
	RFs s;
	TBool result1=(KErrNone==s.Connect());
	if (!result1)
		log->AppendError(_L8("\t\tError in connection"),-1);
	result&=result1;
	s.SetSessionPath(_L("c:\\"));

	TImAttachmentFile* filePtr=new (ELeave) TImAttachmentFile(s);
	CleanupDeletePushL(filePtr);
	TImAttachmentFile& file=*filePtr;
	
	s.Delete(_L("c:\\mailtest\\my test file11"));

	TBool result2=(KErrNone==file.CreateFile(_L("c:\\mailtest\\"),_L("my test file11")));
	if (!result)
		log->AppendError(_L8("\t\tError creating file"),-1);
	result&=result2;
	buffer.Repeat(_L8("Hello World! "));
	
	TBool result3=(KErrNone==file.WriteFile(buffer));
	if (!result3)
		log->AppendError(_L8("\t\tError writing to file"),-1);
	result&=result3;
	TBool result4=(KErrNone==file.WriteFile(buffer));
	if (!result4)
		log->AppendError(_L8("\t\tError writing to file"),-1);
	result&=result4;
	file.CloseFile();

	TBool result5=(KErrNotFound==file.OpenFile(_L("c:\\mailtest\\"),_L("this file doesn't exist")));
	
	result5&=(KErrNone==file.OpenFile(_L("c:\\mailtest\\"),_L("my test file11")));
	if (!result5)
		log->AppendError(_L8("\t\tError opening file"),-1);
	TBool result6=(KErrNone==file.ReadFile(resultbuf, 513));
	if (!result6)
		log->AppendError(_L8("\t\tError reading file"),-1);
	result&=result6;
	TBool result7=(KErrNone==file.ReadFile(temp,512));
	if (!result7)
		log->AppendError(_L8("\t\tError reading file"),-1);
	result&=result7;

	resultbuf.Append(temp);
	TBool result8=(buffer.Compare(resultbuf)==KErrNone);
	if (!result8)
		log->AppendError(_L8("\t\tData read not identical to that written"),-1);
	result&=result8;

	file.CloseFile();
	file.DeleteAttachment(_L("c:\\mailtest\\"),_L("my test file11"));

	ResultOfTest(result,aTest);
	CleanupStack::PopAndDestroy(filePtr);
	return result;
	}

LOCAL_C TBool TestAttachmentClass12(TInt aTest)
	{
	log->AppendComment(_L8("\tAttachment Cache Test (Test 12)"));
	// try reading more than there actually exists on the file
	TBool result=ETrue;
	RFs s;
	TBool result1=(KErrNone==s.Connect());
	if (!result1)
		log->AppendError(_L8("\t\tError in connection"),-1);
	result&=result1;
	s.SetSessionPath(_L("c:\\"));

	TImAttachmentFile* filePtr=new (ELeave) TImAttachmentFile(s);
	CleanupDeletePushL(filePtr);
	TImAttachmentFile& file=*filePtr;
	
	s.Delete(_L("c:\\mailtest\\my test file12"));

	TBool result2=(KErrNone==file.CreateFile(_L("c:\\mailtest\\"),_L("my test file12")));
	if (!result2)
		log->AppendError(_L8("\t\tError creating file"),-1);
	result&=result;
	buffer.Repeat(_L8("Hello World! "));

	TBool result3=(KErrNone==file.WriteFile(buffer));
	if (!result3)
		log->AppendError(_L8("\t\tError writing to file"),-1);
	result&=result3;
	file.CloseFile();

	TBool result4=(KErrNotFound==file.OpenFile(_L("c:\\mailtest\\"),_L("this file doesn't exist")));
	
	result4&=(KErrNone==file.OpenFile(_L("c:\\mailtest\\"),_L("my test file12")));
	if (!result4)
		log->AppendError(_L8("\t\tError opening file"),-1);
	result&=result4;
	TBool result5=(KErrNone==file.ReadFile(resultbuf,KMaxBuf));
	if (!result5)
		log->AppendError(_L8("\t\tError reading file"),-1);
	result&=result5;
	
	TBool result6=(buffer.Left(KMaxBuf).Compare(resultbuf)==KErrNone);
	if (!result6)
		log->AppendError(_L8("\t\tData read not identical to that written"),-1);
	result&=result6;

	file.CloseFile();
	file.DeleteAttachment(_L("c:\\mailtest\\"),_L("my test file12"));

	ResultOfTest(result,aTest);
	CleanupStack::PopAndDestroy(filePtr);
	return result;
	}

LOCAL_C TBool TestAttachmentClass13(TInt aTest)
	{
	log->AppendComment(_L8("\tAttachment Cache Test (Test 13)"));
	// try reading more than there actually exists on the file
	TBool result=ETrue;
	RFs s;
	TBool result1=(KErrNone==s.Connect());
	if (!result1)
		log->AppendError(_L8("\t\tError in connection"),-1);
	result&=result1;
	s.SetSessionPath(_L("c:\\"));

	TImAttachmentFile* filePtr=new (ELeave) TImAttachmentFile(s);
	CleanupDeletePushL(filePtr);
	TImAttachmentFile& file=*filePtr;
	
	s.Delete(_L("c:\\mailtest\\my test file13"));

	TBool result2=(KErrNone==file.CreateFile(_L("c:\\mailtest\\"),_L("my test file13")));
	if (!result2)
		log->AppendError(_L8("\t\tError creating file"),-1);
	result&=result2;
	buffer.Repeat(_L8("Hello World! "));
	
	TBool result3=(KErrNone==file.WriteFile(buffer));
	if (!result3)
		log->AppendError(_L8("\t\tError writing to file"),-1);
	result&=result3;
	file.CloseFile();

	TBool result4=(KErrNotFound==file.OpenFile(_L("c:\\mailtest\\"),_L("this file doesn't exist")));

	result4&=(KErrNone==file.OpenFile(_L("c:\\mailtest\\"),_L("my test file13")));
	if (!result4)
		log->AppendError(_L8("\t\tError opening file"),-1);
	result&=result4;
	TBool result5=(KErrNone==file.ReadFile(resultbuf,600));
	if (!result5)
		log->AppendError(_L8("\t\tError reading file"),-1);
	result&=result5;
	TBool result6=(KErrNone==file.ReadFile(temp,424));
	if (!result6)
		log->AppendError(_L8("\t\tError reading file"),-1);
	result&=result6;

	resultbuf.Append(temp);
	TBool result7=(buffer.Left(1024).Compare(resultbuf)==KErrNone);
	if (!result7)
		log->AppendError(_L8("\t\tData read not identical to that written"),-1);
	result&=result7;

	file.CloseFile();
	file.DeleteAttachment(_L("c:\\mailtest\\"),_L("my test file13"));

	ResultOfTest(result,aTest);
	CleanupStack::PopAndDestroy(filePtr);
	return result;
	}

LOCAL_C TBool TestAttachmentClass14(TInt aTest)
	{
	log->AppendComment(_L8("\tAttachment Cache Test (Test 14)"));
	// try reading even for an empty file
	TBool result=ETrue;
	RFs s;
	TBool result1=(KErrNone==s.Connect());
	if (!result1)
		log->AppendError(_L8("\t\tError in connection"),-1);
	result&=result1;
	s.SetSessionPath(_L("c:\\"));

	TImAttachmentFile* filePtr=new (ELeave) TImAttachmentFile(s);
	CleanupDeletePushL(filePtr);
	TImAttachmentFile& file=*filePtr;
	
	s.Delete(_L("c:\\mailtest\\my test file14"));

	TBool result2=(KErrNone==file.CreateFile(_L("c:\\mailtest\\"),_L("my test file14")));
	if (!result2)
		log->AppendError(_L8("\t\tError creating file"),-1);
	result&=result2;
	file.CloseFile();

	TBool result3=(KErrNotFound==file.OpenFile(_L("c:\\mailtest\\"),_L("this file doesn't exist")));
	TBuf8<KMaxBuf> resultbuf;

	result3&=(KErrNone==file.OpenFile(_L("c:\\mailtest\\"),_L("my test file14")));
	if (!result3)
		log->AppendError(_L8("\t\tError opening file"),-1);
	result&=result3;
	TBool result4=(KErrNone==file.ReadFile(resultbuf,KMaxBuf));
	if (!result4)
		log->AppendError(_L8("\t\tError reading file"),-1);
	result&=result4;

	TBool result5=(resultbuf.Length()==0);	// nothing to read
	if (!result5)
		log->AppendError(_L8("\t\tError reading an empty file"),-1);
	result&=result5;

	file.CloseFile();
	file.DeleteAttachment(_L("c:\\mailtest\\"),_L("my test file14"));

	ResultOfTest(result,aTest);
	CleanupStack::PopAndDestroy(filePtr);
	return result;
	}

LOCAL_C TBool TestAttachmentClass15(TInt aTest)
	{
	log->AppendComment(_L8("\tAttachment Cache Test (Test 15)"));
	// Create a 'large file' (100K) for timing purposes
	TBool result=ETrue;
	RFs s;
	TBool result1=(KErrNone==s.Connect());
	if (!result1)
		log->AppendError(_L8("\t\tError in connection"),-1);
	result&=result1;
	s.SetSessionPath(_L("c:\\"));

	TImAttachmentFile* filePtr=new (ELeave) TImAttachmentFile(s);
	CleanupDeletePushL(filePtr);
	TImAttachmentFile& file=*filePtr;
	
	s.Delete(_L("c:\\mailtest\\my test file15"));

	TBool result2=(KErrNone==file.CreateFile(_L("c:\\mailtest\\"),_L("my test file15")));
	if (!result2)
		log->AppendError(_L8("\t\tError creating file"),-1);
	result&=result2;
	TBuf8<45> buffer(45);
	buffer.Repeat(_L8("Hello World! "));
	
	TBool result3 = ETrue;
	for (TInt i=0; i<22756; i++)
		result3&=(KErrNone==file.WriteFile(buffer));
	if (!result3)
		log->AppendError(_L8("\t\tError writing to file"),-1);
	result&=result3;
	file.CloseFile();

	TBool result4=(KErrNotFound==file.OpenFile(_L("c:\\mailtest\\"),_L("this file doesn't exist")));
	TBuf8<90> resultbuf;

	result4&=(KErrNone==file.OpenFile(_L("c:\\mailtest\\"),_L("my test file15")));
	if (!result4)
		log->AppendError(_L8("\t\tError opening file"),-1);
	result&=result4;
	TBool result5 = ETrue;
	for (TInt j=0; j<22756; j++)
		{
		result5&=(KErrNone==file.ReadFile(resultbuf,45));
		}
	if (!result5)
		log->AppendError(_L8("\t\tError reading file"),-1);
	result&=result5;

	file.CloseFile();
	file.DeleteAttachment(_L("c:\\mailtest\\"),_L("my test file15"));

	ResultOfTest(result,aTest);
	CleanupStack::PopAndDestroy(filePtr);
	return result;
	}

LOCAL_C TBool TestAttachmentClass16(TInt aTest)
	{
	log->AppendComment(_L8("\tAttachment Cache Test (Test 16)"));
	// Create a 'large file' (100K) for timing purposes
	TBool result=ETrue;
	RFs s;
	TBool result1=(KErrNone==s.Connect());
	if (!result1)
		log->AppendError(_L8("\t\tError in connection"),-1);
	result&=result1;
	s.SetSessionPath(_L("c:\\"));

	TImAttachmentFile* filePtr=new (ELeave) TImAttachmentFile(s);
	CleanupDeletePushL(filePtr);
	TImAttachmentFile& file=*filePtr;
	
	s.Delete(_L("c:\\mailtest\\my test file16"));

	TBool result2=(KErrNone==file.CreateFile(_L("c:\\mailtest\\"),_L("my test file16")));
	if (!result2)
		log->AppendError(_L8("\t\tError creating file"),-1);
	result&=result2;
	buffer.Repeat(_L8("Hello World! "));

	TBool result3 = ETrue;
	for (TInt i=0; i<1000; i++)
		result3&=(KErrNone==file.WriteFile(buffer));
	if (!result3)
		log->AppendError(_L8("\t\tError writing to file"),-1);
	result&=result3;
	file.CloseFile();

	TBool result4=(KErrNotFound==file.OpenFile(_L("c:\\mailtest\\"),_L("this file doesn't exist")));

	result4&=(KErrNone==file.OpenFile(_L("c:\\mailtest\\"),_L("my test file16")));
	if (!result4)
		log->AppendError(_L8("\t\tError opening file"),-1);
	result&=result4;
	TBool result5 = ETrue;
	for (TInt j=0; j<1000; j++)
		result5&=(KErrNone==file.ReadFile(resultbuf,KMaxBuf));
	if (!result5)
		log->AppendError(_L8("\t\tError reading file"),-1);
	result&=result5;

	file.CloseFile();
	file.DeleteAttachment(_L("c:\\mailtest\\"),_L("my test file16"));

	ResultOfTest(result,aTest);
	CleanupStack::PopAndDestroy(filePtr);
	return result;
	}

LOCAL_C TBool TestAttachmentClass17(TInt aTest)
	{
	log->AppendComment(_L8("\tAttachment Cache Test (Test 17)"));
	TBool result=ETrue;
	RFs s;
	TBool result1=(KErrNone==s.Connect());
	if (!result1)
		log->AppendError(_L8("\t\tError in connection"),-1);
	result&=result1;
	s.SetSessionPath(_L("c:\\"));

	TInt64 randSeed;
	TTime theTime;
	TUint randomValue = 0;
	TBuf8<52> validCharacterBuffer = _L8("bcdfghjklmnpqrstvwxyzBCDFGHJKLMNPQRSTVWXYZ0123456789");


	for (TInt i=1; i<=KMaxBuf+1; i++)
		{
		TImAttachmentFile* filePtr=new (ELeave) TImAttachmentFile(s);
		CleanupDeletePushL(filePtr);
		TImAttachmentFile& file=*filePtr;

		TBuf8<KMaxBuf+1> buffer(KMaxBuf+1);
		TBuf8<KMaxBuf+1> readbuffer(KMaxBuf+1);
		TBuf8<45> tempbuffer(45);
		buffer.Zero();
		readbuffer.Zero();
		s.Delete(_L("c:\\mailtest\\my test file17"));
		TBool result2=(KErrNone==file.CreateFile(_L("c:\\mailtest\\"),_L("my test file17")));
		if (!result2)
			log->AppendError(_L8("\t\tError creating file"),-1);
		result&=result2;

		// Fill in buffer with random characters
		theTime.UniversalTime(); 
		// Gets the current universal time to use as a random number seed
		randSeed = theTime.Int64();
		for (TInt j=0; j<i; j++)
			{
			randomValue = (Math::Rand(randSeed)%10)*10; 
			randomValue += Math::Rand(randSeed)%10;
			randomValue %= 52;
			buffer.AppendFormat(_L8("%c"),validCharacterBuffer[randomValue]);
			randomValue = 0;
			}
	
		// Write buffer to file in 45 byte segments
		TBool result3 = ETrue;
		if (i<=45)
			result3&=(KErrNone==file.WriteFile(buffer));
		else
			{
			// need to write 45 bytes at a time
			TInt counter = 0;
			while (counter <= i)
				{
				if (i-counter<45)
					result3&=(KErrNone==file.WriteFile(buffer.Mid(counter,(i-counter))));
				else
					result3&=(KErrNone==file.WriteFile(buffer.Mid(counter,45)));
				counter += 45;
				}
			}
		if (!result3)
			{
			log->AppendError(_L8("\t\tError writing to file"),-1);
			log->AppendError(_L8("\t\t\tIteration: "), i);
			}
		result&=result3;
		file.CloseFile();

		TBool result4=(KErrNone==file.OpenFile(_L("c:\\mailtest\\"),_L("my test file17")));
		if (!result4)
			log->AppendError(_L8("\t\tError opening file"),-1);
		result&=result4;
		
		// Read from file to buffer in 45 byte segments
		TBool result5 = ETrue;
		if (i<=45)
			result5&=(KErrNone==file.ReadFile(readbuffer,i));
		else
			{
			TInt counter = 0;
			while (counter <= i)
				{
				result5&=(KErrNone==file.ReadFile(tempbuffer,45));
				counter += 45;
				readbuffer.Append(tempbuffer);
				}
			}
		if (!result5)
			{
			log->AppendError(_L8("\t\tError reading file"),-1);
			log->AppendError(_L8("\t\t\tIteration: "), i);
			}

		result&=result5;

		// Compare data read is equal to data written
		TBool result6=(buffer.Compare(readbuffer)==KErrNone);
		if (!result6)
			{
			log->AppendError(_L8("\t\tData read not identical to that written"),-1);
			log->AppendError(_L8("\t\t\tIteration: "), i);
			}
		result&=result6;

		file.CloseFile();
	
		file.DeleteAttachment(_L("c:\\mailtest\\"),_L("my test file17"));
		CleanupStack::PopAndDestroy(filePtr);		
		}
	ResultOfTest(result,aTest);
	return result;
	}


LOCAL_C TBool TestLogClass(TInt aTest)
	{
	log->AppendComment(_L8("\tLog Test (Test 18)"));
	RFs s;
	TBool result=(KErrNone==s.Connect());
	if (!result)
		log->AppendError(_L8("\t\tError in connection"),-1);
	s.SetSessionPath(_L("c:\\"));
	s.MkDir(_L("c:\\logs\\"));
	s.MkDir(_L("c:\\logs\\email\\"));

	CImLog* log=CImLog::NewL(_L("c:\\mailtest\\log.file"),EReplace);
	log->AppendError(_L8("\t\tan error text"),KErrNone);
	log->AppendComment(_L8("This is a comment1"));
	delete log;

	log=CImLog::NewL(127);
	log->AppendOut(_L8("an error text"));
	log->AppendResponse(_L8("This is a response"));
	log->AppendComment(_L8("This is a comment"));
	log->AppendError(_L8("\t\tThis is an error"),KErrNotFound);	
	delete log;

	ResultOfTest(result,aTest);
	return result;
	}

LOCAL_C TBool TestLogClass2(TInt aTest)
	{
	log->AppendComment(_L8("\tLog Test (Test 19)"));
	RFs s;
	TBool result=(KErrNone==s.Connect());
	if (!result)
		log->AppendError(_L8("\t\tError in connection"),-1);
	s.SetSessionPath(_L("c:\\"));
	s.MkDir(_L("c:\\logs"));
	s.MkDir(_L("c:\\logs\\email"));

	CImLog* log=CImLog::NewL(_L("c:\\mailtest\\log.file"),EAppend);
	log->AppendComment(_L8("This should be appended!!"));
	delete log;

	ResultOfTest(result,aTest);
	return result;
	}

LOCAL_C void DoTests()
	{
	testUtils->TestStart(1);
	TBool result =TestAttachmentClass1(1);
	if (result)
		log->AppendComment(_L8("PASSED Test 1"));
	else
		log->AppendError(_L8("FAILED Test 1"),-1);
	testUtils->TestFinish(1);
	testUtils->TestStart(2);
	result &= TestAttachmentClass2(2);
	if (result)
		log->AppendComment(_L8("PASSED Test 2"));
	else
		log->AppendError(_L8("FAILED Test 2"),-1);
	testUtils->TestFinish(2);
	testUtils->TestStart(3);
	result &= TestAttachmentClass3(3);
	if (result)
		log->AppendComment(_L8("PASSED Test 3"));
	else
		log->AppendError(_L8("FAILED Test 3"),-1);
	testUtils->TestFinish(3);
	testUtils->TestStart(4);
	result &= TestAttachmentClass4(4);
	if (result)
		log->AppendComment(_L8("PASSED Test 4"));
	else
		log->AppendError(_L8("FAILED Test 4"),-1);
	testUtils->TestFinish(4);
	testUtils->TestStart(5);
	result &= TestAttachmentClass5(5);
	if (result)
		log->AppendComment(_L8("PASSED Test 5"));
	else
		log->AppendError(_L8("FAILED Test 5"),-1);
	testUtils->TestFinish(5);
	testUtils->TestStart(6);
	result &= TestAttachmentClass6(6);
	if (result)
		log->AppendComment(_L8("PASSED Test 6"));
	else
		log->AppendError(_L8("FAILED Test 6"),-1);
	testUtils->TestFinish(6);
	testUtils->TestStart(7);
	result &= TestAttachmentClass7(7);
	if (result)
		log->AppendComment(_L8("PASSED Test 7"));
	else
		log->AppendError(_L8("FAILED Test 7"),-1);
	testUtils->TestFinish(7);
	testUtils->TestStart(8);
	result &= TestAttachmentClass8(8);
	if (result)
		log->AppendComment(_L8("PASSED Test 8"));
	else
		log->AppendError(_L8("FAILED Test 8"),-1);
	testUtils->TestFinish(8);
	testUtils->TestStart(9);
	result &= TestAttachmentClass9(9);
	if (result)
		log->AppendComment(_L8("PASSED Test 9"));
	else
		log->AppendError(_L8("FAILED Test 9"),-1);
	testUtils->TestFinish(9);
	testUtils->TestStart(10);
	result &= TestAttachmentClass10(10);
	if (result)
		log->AppendComment(_L8("PASSED Test 10"));
	else
		log->AppendError(_L8("FAILED Test 10"),-1);
	testUtils->TestFinish(10);
	testUtils->TestStart(11);
	result &= TestAttachmentClass11(11);
	if (result)
		log->AppendComment(_L8("PASSED Test 11"));
	else
		log->AppendError(_L8("FAILED Test 11"),-1);
	testUtils->TestFinish(11);
	testUtils->TestStart(12);
	result &= TestAttachmentClass12(12);
	if (result)
		log->AppendComment(_L8("PASSED Test 12"));
	else
		log->AppendError(_L8("FAILED Test 12"),-1);
	testUtils->TestFinish(12);
	testUtils->TestStart(13);
	result &= TestAttachmentClass13(13);
	if (result)
		log->AppendComment(_L8("PASSED Test 13"));
	else
		log->AppendError(_L8("FAILED Test 13"),-1);
	testUtils->TestFinish(13);
	testUtils->TestStart(14);
	result &= TestAttachmentClass14(14);
	if (result)
		log->AppendComment(_L8("PASSED Test 14"));
	else
		log->AppendError(_L8("FAILED Test 14"),-1);
	testUtils->TestFinish(14);
 	testUtils->TestStart(15);
	result &= TestAttachmentClass15(15);
	if (result)
		log->AppendComment(_L8("PASSED Test 15"));
	else
		log->AppendError(_L8("FAILED Test 15"),-1);
	testUtils->TestFinish(15);
	testUtils->TestStart(16);
	result &= TestAttachmentClass16(16);
	if (result)
		log->AppendComment(_L8("PASSED Test 16"));
	else
		log->AppendError(_L8("FAILED Test 16"),-1);
	testUtils->TestFinish(16);
	testUtils->TestStart(17);
	result &= TestAttachmentClass17(17);
	if (result)
		log->AppendComment(_L8("PASSED Test 17"));
	else
		log->AppendError(_L8("FAILED Test 17"),-1);
	testUtils->TestFinish(17);
	testUtils->TestStart(18);
	result &= TestLogClass(18);
		if (result)
		log->AppendComment(_L8("PASSED Test 18"));
	else
		log->AppendError(_L8("FAILED Test 18"),-1);
	testUtils->TestFinish(18);
	testUtils->TestStart(19);
	result &= TestLogClass2(19);
	if (result)
		log->AppendComment(_L8("PASSED Test 19"));
	else
		log->AppendError(_L8("FAILED Test 19"),-1);
		testUtils->TestFinish(19);

	TTimeIntervalMicroSeconds intervalArray[5];
	TTimeIntervalMicroSeconds intervalArray1[5];
	for (TInt i = 0; i<5; i++)
		{
		TTime startTime;
		startTime.UniversalTime();
		TestAttachmentClass15(15);
		TTime finishTime;
		finishTime.UniversalTime();
		
		TTimeIntervalMicroSeconds interval;
		interval = finishTime.MicroSecondsFrom(startTime);
		intervalArray[i] = finishTime.MicroSecondsFrom(startTime);
		test.Printf(TRefByValue<const TDesC>_L("Interval is : %d \n"), interval.Int64());

		TTime startTime1;
		startTime1.UniversalTime();
		TestAttachmentClass16(16);
		TTime finishTime1;
		finishTime1.UniversalTime();
		
		TTimeIntervalMicroSeconds interval1;
		interval1 = finishTime1.MicroSecondsFrom(startTime1);
		intervalArray1[i] = finishTime1.MicroSecondsFrom(startTime1);
		test.Printf(TRefByValue<const TDesC>_L("Interval1 is : %d \n"), interval1.Int64());
		}

	for (TInt j=0; j<5; j++)
		{
		test.Printf(TRefByValue<const TDesC>_L("Interval[%d] is : %d \n"), j, intervalArray[j].Int64());
		test.Printf(TRefByValue<const TDesC>_L("Interval1[%d] is : %d \n"), j, intervalArray1[j].Int64());
		}
	
	if (result)
		{
		log->AppendComment(_L8("PASSED All tests Correctly"));
		test.Printf(_L("PASSED all Tests"));
		testUtils->TestHarnessCompleted();
		}
	else
		{
		test.Printf(_L("FAILED Tests"));
		log->AppendError(_L8("FAILED At Least one test"),-1);
		testUtils->TestHarnessFailed(KErrGeneral);
		}
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
	log->AppendComment(_L8("****** T_MIUT06 Test CImLog & TImAttachmentFile ******"));
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
	log->AppendComment(_L8("**********    T_MIUT06 Tests Complete    **********"));
	log->AppendComment(_L8(""));

	CleanupStack::PopAndDestroy(3);  //testUtils, log, scheduler
	}

//

LOCAL_C void doMainL()
	{
	InitL();
	testUtils->GoClientSideL();
	test.Printf(_L("Performing Tests"));

	DoTests();

	Closedown();
	}

GLDEF_C TInt E32Main()
	{	

	__UHEAP_MARK;
	test.Start(_L("MIUT06 Testrig for CImLog & TImAttachmentFile classes"));
	theCleanup=CTrapCleanup::New();
	TRAPD(ret,doMainL());		
	test(ret==KErrNone);
	delete theCleanup;	
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}

