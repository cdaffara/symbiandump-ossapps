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
// T_IMCV_Transform_Receive
// Component: IMUT
// Owner: Ibrahim Rahman
// Brief description of test harness:
// Testing the System Default Charset setting in Messaging.
// This is set in the imcm resource file.
// This particular test runs the parsing code, 
// simulating effect of downloading messages.
// ( See also t_imcv_transform_send )
// Detailed description of test harness:
// Extract RFC data from files save to message store after conversion.
// Query the message store, saved info to set of output files. 
// Compare these files against expected results.
// Test is run with the ascii, utf7 and an unknown charset value.
// Intermediate files produced while running test harness:
// in directory : /msglogs/t_imcv_transform_receive/
// NoneEntry_822Headers.txt
// NoneEntry_Files.txt
// NoneEntry_MimeHeaders.txt
// NoneEntry_RichTextBodies.txt
// NoneEntry_Structure.txt
// UTF7Entry_822Headers.txt
// UTF7Entry_Files.txt
// UTF7Entry_MimeHeaders.txt
// UTF7Entry_RichTextBodies.txt
// UTF7Entry_Structure.txt
// UnknownEntry_822Headers.txt
// UnknownEntry_Files.txt
// UnknownEntry_MimeHeaders.txt
// UnknownEntry_RichTextBodies.txt
// UnknownEntry_Structure.txt
// Output files produced by running test harness:
// \msglogs\t_imcv_transform_receive*
// Testing encoding/charset setting during the receiving of messages.
// Test code is a version of t_imcv00
// 
//

#include <txtetext.h>
#include <txtrich.h>
#include <bautils.h>
#include <miuthdr.h>
#include <miutmsg.h>
#include <miutset.h>  //KUidMsgTypePOP3
#include <imutdll.h>	
#include <imcvtext.h>
#include <imcvrecv.h>
#include <imcvsend.h>

//Oyster includes
#include <msvstd.h>
#include <msvids.h>
#include <msvuids.h>
#include <msvruids.h>
#include <msvreg.h>

#include "msvindexadapter.h"

#include "MSVSERV.H"
#include <msventry.h> 
#include <msvapi.h>
#include <mtclbase.h>
#include "emailtestutils.h"
#include <s32strm.h>
#include <f32file.h>


#define KComponent				_L("IMUT")

// File paths used by test program
#define KTestMessageDir			_L("imcv\\Transform_Receive\\")
#define KTest822FilesDir		_L("rfc822\\Transform_Receive\\")
#define KTestLibsDir			_L("c:\\system\\data\\")

_LIT(KUnknownResource, "imcm.rsc.unknown");

LOCAL_D RTest test(_L("T_IMCV_Transform_Receive test harness"));
LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D CEmailTestUtils* testUtils;

LOCAL_C CTestActive* active;

//----------------------------------------------------------------------------------------
LOCAL_C void ReadDataL(CImRecvConvert* recvConvert, TFileName fileName)
//----------------------------------------------------------------------------------------
	{
	// open the file
	RFile file;

	TInt anError = file.Open(testUtils->FileSession(), fileName, EFileShareAny);
	if(anError != KErrNone)
		{
		test.Printf(TRefByValue<const TDesC>_L("\r\nFile open error %d"), anError);
		test.Getch();
		User::Leave(KErrNotFound);
		}

	test.Printf(TRefByValue<const TDesC>_L("Data from %s...\r\n"), fileName.PtrZ());		

	// read the file into the conversion object
	HBufC8* lineBuffer = HBufC8::NewLC(1024);
	TPtr8 line = lineBuffer->Des();

	TBuf8<1> aChar;
	TBool finished = FALSE;

	recvConvert->ResetL();
	// supply a new attachment path since previous one is discarded by call to Reset()

	do {
		line.FillZ();
		line.SetLength(0);
		// compile a line one char at a time
		do {
			file.Read(aChar, 1);
			if(aChar.Length())
				line.Append(aChar);
			else
				finished = TRUE;
			// stop at the end of line or no more data
			} while(aChar.Length() && aChar[0] != 0x0A);
			if(!line.Length())
				break;
			/***********************************************/
			/* -- This is where we actually do the work -- */
			/***********************************************/
			recvConvert->ParseNextField(line); // This function doesnt actually leave, any leaves are relayed to the MessageCompleteL() function

		} while(!finished);

	TRAPD(err, recvConvert->MessageCompleteL());
	if(err!=KErrNone)
		{
		// At this point the message should be deleted and CImRecvConvert should be Reset()
		test.Printf(TRefByValue<const TDesC>_L("\r\nError %d occured during the conversion of Message %d"), err, recvConvert->EntryId());
		}
	CleanupStack::PopAndDestroy(); // lineBuffer

	file.Close();
	}


//----------------------------------------------------------------------------------------
LOCAL_C void TestReadL(TFileName filename)
//----------------------------------------------------------------------------------------
	{
	CImRecvConvert* recvConvert = CImRecvConvert::NewLC(testUtils->FileSession(), testUtils->iServerEntry, 
		KUidMsgTypePOP3, KMsvLocalServiceIndexEntryId);

	recvConvert->SetMsvId(KMsvGlobalOutBoxIndexEntryId); //Set the TMsvServerEntry to correct context 

	//TRAPD(error,ReadDataWithOOMTestingL(recvConvert, filename)); // Do the stuff
	TRAPD(error,ReadDataL(recvConvert, filename)); // Do the stuff
	if (error)
		{
		test.Printf(TRefByValue<const TDesC>_L("\r\nReadData Failed, Error %d"), error);
		test.Getch();
		}
	CleanupStack::PopAndDestroy();  //recvConvert
	}


//----------------------------------------------------------------------------------------
LOCAL_C void UpL()
//----------------------------------------------------------------------------------------
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);

	testUtils = CEmailTestUtils::NewL(test);

	TParse parsedFileName;
	TFileName name;

	testUtils->ResolveLogFile(name, parsedFileName);

	testUtils->FileSession().RmDir(parsedFileName.DriveAndPath());

	if(!testUtils->FileSession().MkDirAll(parsedFileName.DriveAndPath()))
		{
		TFileName buffer(parsedFileName.FullName());
		test.Printf(TRefByValue<const TDesC>_L("Created %S directory\n"),&buffer);
		}

	testUtils->CreateAllTestDirectories();

	name.Copy(KTest822FilesDir);
	testUtils->ResolveFile(KComponent, name, parsedFileName);
	testUtils->FileSession().SetSessionPath(parsedFileName.FullName());
	testUtils->CleanMessageFolderL();
	testUtils->GoServerSideL();

	active = new (ELeave) CTestActive();
	active->StartL();	
	}


//----------------------------------------------------------------------------------------
void PrintTextL(const TDesC& aBuf, TInt err, TFileName& aFilename)
//----------------------------------------------------------------------------------------
	{
	HBufC* buff= HBufC::NewLC(100);

	(buff->Des()).Format(aBuf, err, aFilename.PtrZ());
	testUtils->WriteComment(buff->Des());
	test.Printf(buff->Des());

	CleanupStack::PopAndDestroy();  // buff
	}

//----------------------------------------------------------------------------------------
void PrintTextL(const TDesC& aBuf, TInt err)
//----------------------------------------------------------------------------------------
	{
	HBufC* buff= HBufC::NewLC(100);

	(buff->Des()).Format(aBuf, err);
	testUtils->WriteComment(buff->Des());

	CleanupStack::PopAndDestroy();  // buff
	}

//----------------------------------------------------------------------------------------
TInt CompareLogs(const TDesC& aText)
//----------------------------------------------------------------------------------------
	{
	TInt testHarnessError=0;

	test.Console()->ClearScreen();
	test.Next(_L("Comparing:"));

	CPtrCArray* file = new (ELeave) CPtrCArray(6);	
	file->AppendL(KFileNameHeaders);
	file->AppendL(KFileNameMimeHeaders);
	file->AppendL(KFileNameBodies);
	file->AppendL(KFileNameFiles);	
	file->AppendL(KFileNameEntryStructure);

	RFileReadStream file1;
	RFileReadStream file2;
	HBufC8* line1Buffer = HBufC8::NewLC(1024);
	TPtr8 line1 = line1Buffer->Des();
	TBuf8<1> aChar1;

	HBufC8* line2Buffer = HBufC8::NewLC(1024);
	TPtr8 line2 = line2Buffer->Des();
	TBuf8<1> aChar2;

	TParse parsedLogFileName;
	TParse parsedCompareFileName;

	for(TInt i=0; i<file->MdcaCount(); i++)
		{
		testUtils->TestStart(i+1);

		TBool errorOccured = EFalse;

		TFileName file1Name = file->MdcaPoint(i);
		file1Name.Insert(0, aText);
		testUtils->ResolveLogFile(file1Name, parsedLogFileName);

		TFileName file2Name = file->MdcaPoint(i);
		file2Name.Insert(0, aText);
		file2Name.Insert(0, KTestMessageDir);
		testUtils->ResolveFile(KComponent, file2Name, parsedCompareFileName);

		TInt error1=KErrNone;
		TInt error2=KErrNone;
		
		test.Printf(_L("\n%s"), file2Name.PtrZ());

		testHarnessError=error1=file1.Open(testUtils->FileSession(), parsedLogFileName.FullName(), EFileShareAny);
		if(error1!=KErrNone)
			{
			file1Name=parsedLogFileName.FullName();
			PrintTextL(_L("\nERROR %d opening file %s"), error1, file1Name);
			errorOccured = ETrue;
			}
		else
			{
			testHarnessError=error2=file2.Open(testUtils->FileSession(), parsedCompareFileName.FullName(), EFileShareAny);
			if(error2!=KErrNone)
				{
				file1Name=parsedLogFileName.FullName();
				PrintTextL( _L("\nERROR %d opening file %s"), error2, file1Name);
				errorOccured = ETrue;
				}
			else
				PrintTextL(_L("\nTest %d, Opening file %s"), i+1, file1Name);

			}
		
		if((error1==KErrNone)&&(error2==KErrNone))
			{
			// read the file into the conversion object
			TInt file1LineCounter = 0;
	
			TBool finished = EFalse;
	
			do	{
				line1.FillZ();
				line1.SetLength(0);
				// compile the lines one char at a time
				do	{
					TRAPD( error, file1.ReadL(aChar1, 1) ); 
					if (error!=KErrEof)
						line1.Append(aChar1);
					else
						{
						finished = ETrue;
						break;
						}
					// stop at the end of line or no more data
					}
				while((aChar1[0]!=0x0A)&&(line1.Length()<1024));

				line2.FillZ();
				line2.SetLength(0);
				do	{
					TRAPD( error, file2.ReadL(aChar2, 1) ); 
					if (error!=KErrEof)
						line2.Append(aChar2);
					else
						{
						finished = ETrue;
						break;
						}
					// stop at the end of line or no more data
					}
				while((aChar2[0]!=0x0A)&&(line2.Length()<1024));
				
				if(!finished)
					{
					line1.TrimRight();
					line2.TrimRight();

					if(aChar1[0]==0x0A) 
						file1LineCounter++;
					if(line1.Compare(line2))
						{
						PrintTextL(_L("\nERROR - Line %d is different "), file1LineCounter);
						errorOccured = ETrue;
						}
					}
				}
			while(!finished);
			}		

		file1.Close();
		file2.Close();


		if (errorOccured)
			{
			testUtils->TestFinish(i+1, KErrGeneral);
			testHarnessError=errorOccured;
			test.Printf(_L(" - FAILED"));
			}
		else
			{
			testUtils->TestFinish( i+1, KErrNone);
			test.Printf(_L(" - OK"));
			}
		}
	
	test.Printf(_L("\n"));

	CleanupStack::PopAndDestroy(2);  //line 1 , line 2
	delete file;
	return testHarnessError;
	}


//----------------------------------------------------------------------------------------
LOCAL_C void Down()
//----------------------------------------------------------------------------------------
	{
	delete testUtils;
	//CleanupStack::PopAndDestroy(testUtils); 
	CleanupStack::PopAndDestroy(); 
	delete active;
	}

//----------------------------------------------------------------------------------------
void SendFilesToMessageStore(const TDesC& aText)
//----------------------------------------------------------------------------------------
	{
	TInt counter=0;
	TInt numberOfFiles=0;	
	CDir* rfcFileList;

	TParse parsedFileName;
	TFileName name(KTest822FilesDir);
	//name.Append(aText);
	testUtils->ResolveFile(KComponent, name, parsedFileName);


	// Loads the any test files into an EntryList
	TInt error=testUtils->FileSession().GetDir(parsedFileName.FullName(), KEntryAttNormal, ESortByName, rfcFileList);
	if(!error)
		{
		numberOfFiles = rfcFileList->Count();
		test.Printf(_L("Number Of Files=%d   1\n"), numberOfFiles);
		TInt num=1;
		if(numberOfFiles)
			{
			do	{
				TFileName filename = (*rfcFileList)[counter].iName;
				PrintTextL(_L("\n%d Opening file %s\n"), num++, filename);

				filename.Insert(0, parsedFileName.FullName());
				TestReadL(filename);
				counter++;
				}
			while(counter<numberOfFiles);
			
			name.Copy(aText);
			testUtils->ResolveLogFile(name, parsedFileName);

			testUtils->FindChildrenL(KMsvRootIndexEntryId, parsedFileName.FullName());
			}
		else
			test.Printf(_L("WARNING - There are no RFC files in test file source directory\n"));
		}
	else
		test.Printf(TRefByValue<const TDesC>_L("\r\nFile error %d"), error);

	delete rfcFileList;
	}


LOCAL_C void DeleteMessageFolderL()
	{
	test.Console()->ClearScreen();
	testUtils->Reset();

	CleanupStack::PopAndDestroy(); //scheduler 

	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);

	testUtils->CreateAllTestDirectories();
	testUtils->FileSession().SetSessionPath(KTest822FilesDir);
	testUtils->CleanMessageFolderL();
	testUtils->GoServerSideL();
	}


//----------------------------------------------------------------------------------------
LOCAL_C void doMainL()
//----------------------------------------------------------------------------------------
	{
	TInt err;
	UpL();


	test.Console()->ClearScreen();
	test.Console()->SetPos(0, 0);
	test.Printf(_L("\n---------------------"));
	test.Printf(_L("\n  Charset Settings Test - Receive"));
	test.Printf(_L("\n---------------------\n"));
	test.Printf(_L("\n"));

//	SendFilesToMessageStore(_L("None"));
//	err=CompareLogs(_L("None"));
//	DeleteMessageFolderL();
		
//	testUtils->FileSession().SetSessionPath(KTestLibsDir);
//	testUtils->FileSession().Delete(KImEngineResourceFile); // IF IT EXISTS
//	testUtils->FileSession().Rename(KUtf7Resource, KImEngineResourceFile);
	SendFilesToMessageStore(_L("UTF7"));
	err=CompareLogs(_L("UTF7"));
	DeleteMessageFolderL();

	// Testing Unknown charset
//	testUtils->FileSession().SetSessionPath(KTestLibsDir);
//	testUtils->FileSession().Rename(KImEngineResourceFile, KUtf7Resource);
//	testUtils->FileSession().Rename(KUnknownResource, KImEngineResourceFile);
//	SendFilesToMessageStore(_L("Unknown"));
//	err=CompareLogs(_L("Unknown"));
//	DeleteMessageFolderL();

	testUtils->FileSession().Rename(KImEngineResourceFile, KUnknownResource);

	if (err==0)
		testUtils->TestHarnessCompleted(); 
	else
		testUtils->TestHarnessFailed(-2);

	Down();
  }

//----------------------------------------------------------------------------------------
GLDEF_C TInt E32Main()
//----------------------------------------------------------------------------------------
	{	
	__UHEAP_MARK;
	test.Start(_L("T_IMCV_Transform_Receive Test harness"));
	theCleanup=CTrapCleanup::New();
	TRAPD(ret,doMainL());
	test(ret==KErrNone);
	delete theCleanup;
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}
