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
// T_IMUT_PARSE
// Component: IMUT
// Owner: Ibrahim Rahman
// Brief description of test harness:
// Main regression test for receving email messages.
// Detailed description of test harness:
// Reads in emails messages stored in RFC files. Parse/Stores messages. 
// Then creates five log files detailing information about messages stored in mail directory.
// Does a comparison of log files with files containing expected results.
// Failures/Success of test dependent on outcome of comparison.
// Input files required to run test harness:
// in directory : /msgtest/imut/imcv/parse/
// Set of 100+ rfc*.txt files, should be in correct directory.
// Intermediate files produced while running test harness:
// in directory : /msglogs/t_imut_parse/
// Entry_822Headers.txt
// Entry_Files.txt
// Entry_MimeHeaders.txt
// Entry_RichTextBodies.txt
// Entry_Structure.txt
// Output files produced by running test harness:
// \msglogs\t_imut_parse*
// Description of how to build test harness:
// Description of how to run test harness:
// Testing Recieving of messages
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

// File paths used by test program
#define KComponent				_L("IMUT")

#define KMIMEOutputFile		_L("ImcvSendOutputText_MIME.txt")
#define KOutputFile			_L("ImcvSendOutputText.txt")

_LIT8(KDate, "Date");
_LIT8(KPlus, "+");
_LIT8(KMinus, "-");


#define KMailFolder	KMsvGlobalOutBoxIndexEntryId


#define KOkFileDir				_L("imcv\\parse\\")
#define KTestMessageDir			_L("rfc822\\parse\\")

LOCAL_D RTest test(_L("T_IMUT_PARSE test harness"));
LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D CEmailTestUtils* testUtils;

LOCAL_C CTestActive* active;
LOCAL_D	CDesCArrayFlat* subjectArray;
LOCAL_D	CDesCArrayFlat* fromArray;

LOCAL_D TBool StartTimeSet=EFalse;
LOCAL_D TTime StartTime;
LOCAL_D TInt  KnownDateTimeIndex=0;
LOCAL_D TUint DayLightSavingBackup;
LOCAL_C void SetKnownDateAndTime()
	{
	TDateTime aDateTimes[]={
				TDateTime(2002,EJune,18,10,30,0,0),
				TDateTime(2003,EJanuary,1,21,0,0,0),
				TDateTime(2001,EDecember,18,2,15,0,0),
				TDateTime(2004,EMay,18,17,50,0,0)
				};
	if (!StartTimeSet)
		{
		StartTime.UniversalTime();
		StartTimeSet=ETrue;
		}
	TTime aFakeTime(aDateTimes[KnownDateTimeIndex++]);
	KnownDateTimeIndex&=3;
	User::SetHomeTime(aFakeTime);
	}
LOCAL_C void RestoreDateAndTime()
	{
	if (StartTimeSet)
		{
		TTime aCurrentTime;
		aCurrentTime.UniversalTime();
		TTimeIntervalSeconds aDuration;
		if (!aCurrentTime.SecondsFrom(StartTime,aDuration))
			StartTime+=aDuration;
		User::SetHomeTime(StartTime);
		StartTimeSet=EFalse;
		}
	}
	
LOCAL_C void UnsetDayLightSaving()
// Make sure time is not offset by system daylight saving clock adjustments.
	{
	DayLightSavingBackup = User::UTCOffset().Int();
	User::SetUTCOffset(0);
	}
	
LOCAL_C void RestoreDayLightSaving()
// Restore daylight saving clock adjustments.
	{
	User::SetUTCOffset(DayLightSavingBackup);
	}

//----------------------------------------------------------------------------------------
LOCAL_C void WriteToFile(RFile file, TDesC8& aInputLine)
//----------------------------------------------------------------------------------------
	{
	file.Write(aInputLine);
	}


//----------------------------------------------------------------------------------------
LOCAL_C void WriteDataL( CImSendMessage* sendMsg, TImSendMethod anAlgorithm, 
						 TInt& count, TInt& aTotalSize, const TDesC& aFileDir)
//----------------------------------------------------------------------------------------
	{
	RFile file;	
	RFs& theFs =testUtils->FileSession();

	TFileName filename(aFileDir);
	TParse parsedLogFileName;
	
	if (anAlgorithm == ESendAsSimpleEmail)
		{
		filename.Append(KOutputFile);
		testUtils->ResolveLogFile(filename, parsedLogFileName);
		}
	else
		{
		filename.Append(KMIMEOutputFile);
		testUtils->ResolveLogFile(filename, parsedLogFileName);
		}
	 
	theFs.MkDirAll(parsedLogFileName.DriveAndPath());
	TInt err=file.Open(theFs, parsedLogFileName.FullName(), EFileStreamText|EFileWrite|EFileShareAny);
    if (err==KErrNotFound) // file does not exist - create it
		test(file.Create(theFs, parsedLogFileName.FullName(), EFileStreamText|EFileWrite|EFileShareAny)==KErrNone);
 
	TInt offset=0;
	test(file.Seek(ESeekEnd, offset)==KErrNone);

	TBuf8<10> num;

	file.Write(_L8("\r\n---------------------- Sending Email "));
	num.Num(count+1); file.Write(num);
	file.Write(_L8(" size="));
	num.Num(aTotalSize); file.Write(num);
	file.Write(_L8(" ------------------------\r\n\r\n"));
	
	TBuf8<1024> line;
	TInt paddingCount;

	// write the data from the SendMessage object into the segmented buffer
	TInt errorCode = KErrNone;
	do {
	    line.FillZ();
	    line.SetLength(0);
		errorCode = sendMsg->NextLineL( line, paddingCount );

		__ASSERT_DEBUG( line.Length(), User::Panic(_L("Empty line returned from SendMessage"),1));
		__ASSERT_DEBUG( line[line.Length()-2]==0x0D && line[line.Length()-1]==0x0A, User::Panic(_L("Line not CRLF terminated"),1));

        // Reversing the TimeZone Value to match GMT std.
        
        TInt timezoneOffsetPos;
        
		if(line.Find(KDate) != KErrNotFound)
	    {		
			if((timezoneOffsetPos = line.Find(KPlus)) != KErrNotFound || (timezoneOffsetPos = line.Find(KMinus)) != KErrNotFound)
			{
				TInt timezoneOffsetPriorPos = timezoneOffsetPos;
				TPtrC8 newChar = line.Mid(++timezoneOffsetPos,2);
				timezoneOffsetPos +=2;
				TPtrC8 newoneChar = line.Mid(timezoneOffsetPos,2);
		
				TInt32 newoffsetHours;
				TLex8 changechar(newChar);
				TInt8 errorcode01 = changechar.Val(newoffsetHours);
				
				TInt32 newoffsetMins;
				TLex8 changenewonechar(newoneChar);		
				TInt8 errorcode02 = changenewonechar.Val(newoffsetMins);
					
				TTimeIntervalSeconds offsetTemp=User::UTCOffset();
				TInt32 offset = offsetTemp.Int();
			
				TBool negative = (offset < 0);
				if (negative)
					offset = -offset;
			
				TInt32 offsetHours = offset / 3600;
				if(newoffsetHours == 0)
					newoffsetHours = newoffsetHours;
				else
					newoffsetHours -= offsetHours; // Actual TimeZone Hours
				
				TInt32 offsetMins = (offset - offsetHours * 3600) / 60;
				if(newoffsetMins == 0)
					newoffsetMins = newoffsetMins;
				else
					newoffsetMins -= offsetMins;  // Actual TimeZone Mins
				
				TBuf8<5> offsetString;
				
				if (negative)
				{
					offsetString.Format(KMiutTimeZoneNeg, newoffsetHours, newoffsetMins);
				}	
				else
				{
					offsetString.Format(KMiutTimeZonePos, newoffsetHours, newoffsetMins);
				}
					
				line.Replace(timezoneOffsetPriorPos,5,offsetString);
			}
	   	}
		
		WriteToFile(file, line);
	  }
	while(errorCode == KErrNone);

	file.Close();
	}

//----------------------------------------------------------------------------------------
LOCAL_C void WriteEmailL(TImSendMethod anAlgorithm, const TDesC& aFileDir)
//----------------------------------------------------------------------------------------
	{
	TBuf<10> domainName = _L("psion.com");
	test.Next(_L("Write Test"));

	TDateTime dtime;
	dtime.Set(1996, EDecember, 1, 23, 59, 59, 0);
	TTime time(dtime);

	CMsvServerEntry* sEntry=testUtils->iServerEntry;

	sEntry->SetEntry(KMailFolder);

	CMsvEntrySelection* children = new(ELeave) CMsvEntrySelection;
	sEntry->GetChildren(*children);
	TInt count = children->Count();
	
	for (TInt i=0; i < count; i++)
		{
		sEntry->SetEntry((*children)[i]);
 
		CImSendMessage* sendMsg = CImSendMessage::NewL(*sEntry);
		CleanupStack::PushL(sendMsg);

		TUint charset = KCharacterSetIdentifierIso88591; // KCharacterSetIdentifierUtf7;
		sendMsg->InitialiseL( sEntry->Entry().Id(), anAlgorithm, time, domainName, charset, ESendNoCopy);
		TInt size = sendMsg->HeaderSize();
		size += sendMsg->BodySizeL();

		if (anAlgorithm == ESendAsSimpleEmail)
			test.Printf(TRefByValue<const TDesC>_L("Sending Non-MIME Email,%d size=%d\n"), i, size);
		else
			test.Printf(TRefByValue<const TDesC>_L("Sending MIME Email,%d size=%d\n"), i, size);

 		WriteDataL( sendMsg, anAlgorithm, i, size, aFileDir);

		CleanupStack::PopAndDestroy(); // sendMsg
		}
		
	delete children;
	}


//----------------------------------------------------------------------------------------
void ReadDataL(CImRecvConvert* recvConvert, TFileName fileName)
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
void TestReadL(const TDesC& aDir, TFileName filename)
//----------------------------------------------------------------------------------------
	{
	CImRecvConvert* recvConvert = CImRecvConvert::NewLC(testUtils->FileSession(), testUtils->iServerEntry, 
		KUidMsgTypePOP3, KMsvLocalServiceIndexEntryId);

	recvConvert->SetMsvId(KMsvGlobalOutBoxIndexEntryId); //Set the TMsvServerEntry to correct context 

	filename.Insert(0, aDir);

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
void ResetL()
//----------------------------------------------------------------------------------------
	{
//	CleanupStack::PopAndDestroy(active);

	testUtils->Reset();

	// wait a couple of seconds
	CTestTimer* timer = CTestTimer::NewL();
	timer->After(5000000);
	CActiveScheduler::Start();
	delete timer;

	testUtils->CleanMessageFolderL();
	testUtils->GoServerSideL();
/*	active = new (ELeave) CTestActive();
	CleanupStack::PushL(active);
	active->StartL();
*/		
	}

//----------------------------------------------------------------------------------------
void DeleteLogDir(TPtrC aFolder)
//----------------------------------------------------------------------------------------
	{
	TFileName name;
	TParse parsedFileName;

	name.Copy(aFolder);
	name.Append(KFileNameHeaders);
	testUtils->ResolveLogFile(name, parsedFileName);
	testUtils->FileSession().Delete(parsedFileName.FullName());

	name.Copy(aFolder);
	name.Append(KFileNameMimeHeaders);
	testUtils->ResolveLogFile(name, parsedFileName);
	testUtils->FileSession().Delete(parsedFileName.FullName());

	name.Copy(aFolder);
	name.Append(KFileNameBodies);
	testUtils->ResolveLogFile(name, parsedFileName);
	testUtils->FileSession().Delete(parsedFileName.FullName());

	name.Copy(aFolder);
	name.Append(KFileNameFiles);
	testUtils->ResolveLogFile(name, parsedFileName);
	testUtils->FileSession().Delete(parsedFileName.FullName());

	name.Copy(aFolder);
	name.Append(KFileNameEntryStructure);
	testUtils->ResolveLogFile(name, parsedFileName);
	testUtils->FileSession().Delete(parsedFileName.FullName());

	name.Copy(aFolder);
	name.Append(KMIMEOutputFile);
	testUtils->ResolveLogFile(name, parsedFileName);
	testUtils->FileSession().Delete(parsedFileName.FullName());

	name.Copy(aFolder);
	name.Append(KOutputFile);
	testUtils->ResolveLogFile(name, parsedFileName);
	testUtils->FileSession().Delete(parsedFileName.FullName());
	
	}


//----------------------------------------------------------------------------------------
void UpL()
//----------------------------------------------------------------------------------------
	{
	subjectArray = new (ELeave) CDesCArrayFlat(100);
	fromArray = new (ELeave) CDesCArrayFlat(100);
	
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);

	TFileName name;
	TParse parsedFileName;

	testUtils = CEmailTestUtils::NewLC(test);

	testUtils->ResolveLogFile(name, parsedFileName);
	testUtils->FileSession().MkDirAll(parsedFileName.DriveAndPath());

	name.Copy(KTestMessageDir);
	testUtils->ResolveFile(KComponent, name, parsedFileName);

	testUtils->CreateAllTestDirectories();
	testUtils->FileSession().SetSessionPath(parsedFileName.FullName());

	testUtils->CleanMessageFolderL();
	testUtils->GoServerSideL();

	testUtils->SpecifyRfc822Dir(parsedFileName.FullName());
	
/*
	active = new (ELeave) CTestActive();
	CleanupStack::PushL(active);
	active->StartL();	
*/
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
void PrintTextL( const TDesC& aBuf, TInt err)
//----------------------------------------------------------------------------------------
	{
	HBufC* buff= HBufC::NewLC(100);

	(buff->Des()).Format(aBuf, err);
	testUtils->WriteComment(buff->Des());

	CleanupStack::PopAndDestroy();  // buff
	}

//----------------------------------------------------------------------------------------
void AppendFilenames(CDesCArray* correctFile, CPtrCArray* resultFile, const TDesC& aFileDir)
//----------------------------------------------------------------------------------------
	{
	TFileName name;
	TParse parsedFileName;

	name.Copy(KOkFileDir);
	name.Append(aFileDir);
	name.Append(KFileNameHeaders);
	testUtils->ResolveFile(KComponent, name, parsedFileName);
	correctFile->AppendL(parsedFileName.FullName());

	name.Copy(KOkFileDir);
	name.Append(aFileDir);
	name.Append(KFileNameMimeHeaders);
	testUtils->ResolveFile(KComponent, name, parsedFileName);
	correctFile->AppendL(parsedFileName.FullName());

	name.Copy(KOkFileDir);
	name.Append(aFileDir);
	name.Append(KFileNameBodies);
	testUtils->ResolveFile(KComponent, name, parsedFileName);
	correctFile->AppendL(parsedFileName.FullName());

	name.Copy(KOkFileDir);
	name.Append(aFileDir);
	name.Append(KFileNameFiles);
	testUtils->ResolveFile(KComponent, name, parsedFileName);
	correctFile->AppendL(parsedFileName.FullName());

	name.Copy(KOkFileDir);
	name.Append(aFileDir);
	name.Append(KFileNameEntryStructure);
	testUtils->ResolveFile(KComponent, name, parsedFileName);
	correctFile->AppendL(parsedFileName.FullName());

	name.Copy(KOkFileDir);
	name.Append(aFileDir);
	name.Append(KOutputFile);
	testUtils->ResolveFile(KComponent, name, parsedFileName);
	correctFile->AppendL(parsedFileName.FullName());

	name.Copy(KOkFileDir);
	name.Append(aFileDir);
	name.Append(KMIMEOutputFile);
	testUtils->ResolveFile(KComponent, name, parsedFileName);
	correctFile->AppendL(parsedFileName.FullName());

	// ---

	resultFile->AppendL(KFileNameHeaders);
	resultFile->AppendL(KFileNameMimeHeaders);
	resultFile->AppendL(KFileNameBodies);
	resultFile->AppendL(KFileNameFiles);
	resultFile->AppendL(KFileNameEntryStructure);
	resultFile->AppendL(KOutputFile);
	resultFile->AppendL(KMIMEOutputFile);
	}

//----------------------------------------------------------------------------------------
TInt CompareLogs(const TDesC& aFileDir)
//----------------------------------------------------------------------------------------
	{
	TInt testHarnessError = 0;

	TFileName filepath(aFileDir);
	TParse parsedFileName;
	testUtils->ResolveLogFile(filepath, parsedFileName);

	test.Console()->ClearScreen();
	test.Next(_L("Comparing:"));

	CDesCArray* correctFile = new (ELeave) CDesCArrayFlat(6);
	CPtrCArray* resultFile = new (ELeave) CPtrCArray(6);
	
	AppendFilenames(correctFile, resultFile, aFileDir);

	RFileReadStream file1;
	RFileReadStream file2;
	HBufC8* line1Buffer = HBufC8::NewLC(1024);
	TPtr8 line1 = line1Buffer->Des();
	TBuf8<1> aChar1;

	HBufC8* line2Buffer = HBufC8::NewLC(1024);
	TPtr8 line2 = line2Buffer->Des();
	TBuf8<1> aChar2;

	TBool aSendingLog=EFalse;
	for(TInt i=0; i<correctFile->MdcaCount(); i++)
		{
		if (i==5)
			aSendingLog=ETrue;

		testUtils->TestStart(i+1);

		TBool errorOccured = EFalse;

		TFileName file1Name = correctFile->MdcaPoint(i);

		TFileName file2Name = parsedFileName.FullName();
		file2Name.Append(resultFile->MdcaPoint(i));

		TInt error1=KErrNone;
		TInt error2=KErrNone;
		
		test.Printf(_L("\n%s"), file2Name.PtrZ());

		error1 = file1.Open(testUtils->FileSession(), file1Name, EFileShareAny);
		testHarnessError += error1;
		if(error1!=KErrNone)
			{
			PrintTextL( _L("\nERROR %d opening file %s"), error1, file1Name);
			errorOccured = ETrue;
			}
		else
			{
			error2 = file2.Open(testUtils->FileSession(), file2Name, EFileShareAny);
			testHarnessError += error2;
			if(error2!=KErrNone)
				{
				PrintTextL( _L("\nERROR %d opening file %s"), error2, file2Name);
				errorOccured = ETrue;
				}
			else
				PrintTextL( _L("\nTest %d, Opening file %s"), i+1, file1Name);

			}
		
		if((error1==KErrNone)&&(error2==KErrNone))
			{
			// read the file into the conversion object
			TInt file1LineCounter = 0;
	
			TBool finished = EFalse;
			TBool eofFile1 = EFalse;
			TBool eofFile2 = EFalse;
	
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
						eofFile1 = ETrue;
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
						eofFile2 = ETrue;
						break;
						}
					// stop at the end of line or no more data
					}
				while((aChar2[0]!=0x0A)&&(line2.Length()<1024));
				
				if (eofFile1 || eofFile2)
					{
					// Check if the file lengths are different.
					if (eofFile1 != eofFile2)
						{
						testUtils->WriteComment(_L("\nERROR - File lengths are different "));
						errorOccured = ETrue;
						}
					finished = ETrue;
					}
				else
					{
					line1.TrimRight();
					line2.TrimRight();

					if(aChar1[0]==0x0A) 
						file1LineCounter++;
					if(aSendingLog && line1.Match(_L8("Message-ID*")) == 0)
						continue;
					if(aSendingLog && line1.Find(_L8("EPOC32-")) != KErrNotFound)
						continue;
					if(aSendingLog && line1.Match(_L8("boundary*")) == 0 )
						continue;
					// if we are checking the file data log file, ignore differences
					// between c:\system\mail\ and c:\private\1000484b\ .
					

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
			testHarnessError += errorOccured;
			test.Printf(_L(" - FAILED"));
			}
		else
			{
			testUtils->TestFinish(i+1, KErrNone);
			test.Printf(_L(" - OK"));
			}
		}
	
	test.Printf(_L("\n"));

	CleanupStack::PopAndDestroy(2);  //line 1 , line 2
	delete correctFile;
	delete resultFile;

	return testHarnessError;
	}

//----------------------------------------------------------------------------------------
void Down()
//----------------------------------------------------------------------------------------
	{
	CleanupStack::PopAndDestroy(2);  // active testutils, scheduler
	
	subjectArray->Reset();
	delete subjectArray;

	fromArray->Reset();
	delete fromArray;

	}



//----------------------------------------------------------------------------------------
void ReadEmail(const TDesC& aDirName)
//----------------------------------------------------------------------------------------
	{
	TInt counter=0;
	TInt numberOfFiles=0;

	TFileName name;
	TParse parsedFileName;

	name.Copy(KTestMessageDir);
	name.Append(aDirName);
	testUtils->ResolveFile(KComponent, name, parsedFileName);
	
	CDir* rfcFileList;
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

				TestReadL(parsedFileName.FullName(), filename);
				counter++;
				}
			while(counter<numberOfFiles);
			}
		else
			test.Printf(_L("WARNING - There are no RFC files in test file source directory\n"));
		}
	else
		test.Printf(TRefByValue<const TDesC>_L("\r\nFile error %d"), error);

	delete rfcFileList;
	}

//----------------------------------------------------------------------------------------
TInt RunTests(TPtrC filedir, TParse* parsed)
//----------------------------------------------------------------------------------------
	{
	DeleteLogDir(filedir);
	ReadEmail(filedir);
	testUtils->ResolveLogFile(filedir, *parsed);
		
	testUtils->FindChildrenL(KMsvRootIndexEntryId, parsed->FullName());
	WriteEmailL(ESendAsSimpleEmail, filedir);
 	WriteEmailL(ESendAsMimeEmail, filedir);
	RestoreDateAndTime();
	return CompareLogs(filedir);
	}

//----------------------------------------------------------------------------------------
LOCAL_C void doMainL()
//----------------------------------------------------------------------------------------
	{
	UpL();
	TInt testHarnessError=0;

	test.Console()->ClearScreen();
	test.Console()->SetPos(0, 0);
	test.Printf(_L("\n---------------------"));
	test.Printf(_L("\n   T_IMCV_PARSE Regression Test   "));
	test.Printf(_L("\n---------------------\n"));
	test.Printf(_L("\n"));

	TParse* parsed = new (ELeave) TParse;
	HBufC* filedir= HBufC::NewL(256);
	SetKnownDateAndTime();
	
	// Make sure the daylight saving offset is set to a known state
	// since this is also output on the parsed files.
	UnsetDayLightSaving();

	// SIMPLE messages
	testHarnessError+=RunTests(_L("A.Simple\\"), parsed);
	ResetL();

	// MIME messages
	testHarnessError+=RunTests(_L("B.Mime\\"), parsed);
	ResetL();

	// Multipart messages
	testHarnessError+=RunTests(_L("C.Multipart\\"), parsed);
	ResetL();
	
	// Encoding messages
	testHarnessError+=RunTests(_L("D.Encoding\\"), parsed);
	ResetL();

	// Charsets messages
	testHarnessError+=RunTests(_L("E.Charsets\\"), parsed);
	ResetL();

	// Header messages
	testHarnessError+=RunTests(_L("F.Header\\"), parsed);
	ResetL();


	// MHTML messages
	testHarnessError+=RunTests(_L("G.MHTML\\"), parsed);
	ResetL();


	// Embedded messages
	testHarnessError+=RunTests(_L("H.Embedded\\"), parsed);

	RestoreDayLightSaving();

	if (testHarnessError==0)
		testUtils->TestHarnessCompleted(); 
	else
		testUtils->TestHarnessFailed(testHarnessError);

	
	delete filedir;
	delete parsed;
	Down();
  }

//----------------------------------------------------------------------------------------
GLDEF_C TInt E32Main()
//----------------------------------------------------------------------------------------
	{	
	__UHEAP_MARK;
	test.Start(_L("T_IMCV00 Test harness"));
	theCleanup=CTrapCleanup::New();
	TRAPD(ret,doMainL());
	test(ret==KErrNone);
	RestoreDateAndTime();
	delete theCleanup;
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}
