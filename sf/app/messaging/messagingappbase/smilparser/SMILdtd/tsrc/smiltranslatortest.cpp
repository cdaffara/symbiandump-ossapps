// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the Console Test Harness for testing the SMILTranslator .dll
// It carries this out by searching for all files of a certain type (as specified by 
// KWildName) that reside in a given set of directories (as specified by KInputPathList)
// It then passes each file through a Parser object, picks up the resultant Document
// object and passes this straight back to the composer object.  Output is a new XML
// file of the same name but different directory (same as initial directory but with 
// KTestFindStr replaced by KTestReplaceStr & KTestReplaceStr2) - these directories are  
// created automatically.  The application then reports any errors for each file before
// moving onto the next one.  These error reports are copied to the console screen (but 
// usually too fast to read); to the output window in developer studio; and to a flat
// file (named and located as specified in KErrorFileName)
// There are 3 types of test that may be run.  The Basic test merely
// translates all input files to output files.  The Performance test does the same thing
// multiple times (as specified by KPerformanceTestIterations), although the log file
// output will only relate to the last run.  Finally the Memory test utilises the Heap
// Allocation Failure tool to incrementally run and progressively fail at each and every
// attempt to allocate memory, it's finally run should complete successfully to prove
// that no memory leaks have occurred in spite of X hundreds of previously failed runs.
// Note that this finally test should be done with a much reduced number of files
// residing in the input directories.
// The application can test ASCII or Unicode input and output (4 combinations possible).
// To vary the input you must manually add ASCII or Unicode files to the input
// directories.
// The tests can be run automatically or interactively:
// For interactive tests, run with '-i as follows:
// SMILTRANSLATORTEST.EXE -i
// To run with out user interaction, please see the following examples:
// SMILTRANSLATORTEST.EXE -h                                                 # show command line help
// SMILTRANSLATORTEST.EXE                                                    # runs with default options
// SMILTRANSLATORTEST.EXE -file_type ascii -data_mode file -test_type basic  # same as above
// SMILTRANSLATORTEST.EXE -use_file_handles                                  # msgapi2 only
// SMILTRANSLATORTEST.EXE -use_full_paths                                    # msgapi2 only
// SMILTRANSLATORTEST.EXE -test_type performance -performance_iteratons 3    # performance test with 3 iterations
// 
//

/**
 @file
*/

//#define DEBUG_SMILTRANSLATORTEST_

#include <e32test.h>
#include <e32std.h>
#include <f32file.h>
#include <txtetext.h>
#include <gmxmlparser.h>
#include <gmxmlcomposer.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "smildtdenum.h"
#endif
#include <smildtd.h>
#include <bacline.h>

// Constants
_LIT(KErrorFileName,"c:\\msgtest\\SMIL_Test_Files\\ErrorLog.txt");
_LIT(KParseError,"    Parse File error = ");
_LIT(KComposeError,"    Compose File error = ");
_LIT(KOutputNewLine,"\r\n");
_LIT(KStartFile,"Starting test of file ");
_LIT(KTxtAsciiOrUnicode,"Unicode (F1), Ascii (F2) or Utf-8 (F3) output?");
_LIT(KTxtBufferOrFile,"\nTest file API (F1), test buffer API (F2)");
_LIT(KTxtChooseTest,"\nPress:\nF1 (or any other key) for Basic test\nF2 for Performance test\nF3 for Memory Allocation test");
_LIT(KSmilTranslatorTest,"Smil Translator");
_LIT(KInputPathList, "c:\\msgtest\\SMIL_Test_Files\\SMIL_Input\\Valid\\;c:\\msgtest\\SMIL_Test_Files\\MMS_Input\\Valid\\;c:\\msgtest\\SMIL_Test_Files\\SMIL_Input\\Invalid\\;c:\\msgtest\\SMIL_Test_Files\\MMS_Input\\Invalid\\;");
_LIT(KTestFindStr, "_Input\\");
_LIT(KTestReplaceStr, "_Output\\");
_LIT(KWildName, "*.txt");                         // read all file that match *.txt
_LIT(KOptionI, "-i");                             // unary: interactive mode
_LIT(KOptionH, "-h");
// note: traditional style args "--file-type" don't work with the CCommandLineArguments class
_LIT(KOptionFileType, "-file_type");               // binary: ascii, utf8, unicode
_LIT(KOptionDataMode, "-data_mode");               // binary: buffer, file
_LIT(KOptionTestType, "-test_type");               // binary: basic, performance, memory
_LIT(KOptionPerformanceIterations, "-performance_iterations"); // binary: <number of iterations>
_LIT(KOptionUseFileHandles, "-use_file_handles");   // unary: use file handles as input
_LIT(KOptionUseFullPaths, "-use_full_paths");       // unary: use full paths as input
_LIT(KTxtChooseFileInput, "Choose input file type: Using file path (F1), Using file handle (F2)?");

// Globals 
LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D CActiveScheduler* scheduler;
LOCAL_D RTest test(KSmilTranslatorTest);
LOCAL_D	RFs fileSystem;
class CTestConfig;
LOCAL_D CTestConfig* testConfig;

//
// TestConfig
class CTestConfig : public CBase
	{
public:
	typedef enum {EBasic, EPerformance, EMemory} TTestType;
	typedef enum {EFileData, EBufferData} TXMLDataMode;
	static CTestConfig* NewLC();
	~CTestConfig() {}	

	TInt ProcessCommandLineL();
	void InteractiveMode();

	// accessors
	TBool NeedsHelp() const {return iNeedsHelp; }
	TBool IsInteractive() const {return iIsInteractive; }
	TXMLFileType FileType() const {return iFileType; } 
	TXMLDataMode DataMode() const {return iDataMode; }
	TTestType TestType() const {return iTestType; }
	TInt PerformanceTestIterations() const {return iPerformanceTestIterations; }

	// display
	void DisplayHelp();
	void UseageErr();
	void Dump();

	TBool UseFileHandles() const {return iUseFileHandles; }


private:
	void ConstructL();
	TBool UnaryArgPresent(CCommandLineArguments* aArgs,TInt aArgCount, const TDesC& aOption);
	TInt ExtractBinaryArg(CCommandLineArguments* aArgs,TInt aArgCount, const TDesC& aOption,TBuf<32>& aBuf);

	TBool FileTypeFromStr(const TBuf<32>& aFileType);
	TBool DataModeFromStr(const TBuf<32>& aDataMode);
	TBool TestTypeFromStr(const TBuf<32>& aTestType);
private:
	TBool iNeedsHelp;
	TBool iIsInteractive;
	TXMLFileType iFileType;
	TTestType iTestType;
	TXMLDataMode iDataMode;
	TInt iPerformanceTestIterations;
	TBool iUseFileHandles;
	};

void CTestConfig::ConstructL()
	{
	iNeedsHelp=EFalse;
	iIsInteractive=EFalse;
	iFileType=EAscii;
	iTestType=EBasic; 
	iDataMode=EFileData;
	iPerformanceTestIterations=100; 
	iUseFileHandles = EFalse;
	}

CTestConfig* CTestConfig::NewLC()
	{
	CTestConfig* self=new(ELeave)CTestConfig();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

TBool CTestConfig::FileTypeFromStr(const TBuf<32>& aFileType)
	{
	TBool found=ETrue;
	if(aFileType.Compare(_L("ascii"))==0)
		{
		iFileType=EAscii;
		}
	else if(aFileType.Compare(_L("utf8"))==0)
		{
		iFileType=EUtf8;
		}
	else if (aFileType.Compare(_L("unicode"))==0)
		{
		iFileType=EUnicode;
		}
	else
		{
		found=EFalse;
		}
	return found;
	}

TBool CTestConfig::DataModeFromStr(const TBuf<32>& aDataMode)
	{
	TBool found=ETrue;
	if(aDataMode.Compare(_L("buffer"))==0)
		{
		iDataMode=EBufferData;	
		}
	else if(aDataMode.Compare(_L("file"))==0)
		{
		iDataMode=EFileData;
		}
	else 
		{
		found=EFalse;
		}
	return found;
	}

TBool CTestConfig::TestTypeFromStr(const TBuf<32>& aTestType)
	{
	TBool found=ETrue;
	if (aTestType.Compare(_L("basic"))==0) 
		{
		iTestType=EBasic;
		}
	else if (aTestType.Compare(_L("performance"))==0)
		{
		iTestType=EPerformance;
		}
	else if (aTestType.Compare(_L("memory"))==0)
		{
		iTestType=EMemory;
		}
	else 
		{
		found=EFalse;
		}
	return found;
	}

void CTestConfig::DisplayHelp()
	{
	test.Printf(KOptionH);
	test.Printf(_L("\thelp\r\n"));
	test.Printf(KOptionI);
	test.Printf(_L("\tinteractive mode\r\n"));
	test.Printf(KOptionFileType);
    test.Printf(_L("\tascii|utf8|unicode\r\n"));
	test.Printf(KOptionDataMode);
	test.Printf(_L("\tbuffer-data|file-data\r\n"));
	test.Printf(KOptionTestType);
	test.Printf(_L("\tbasic|performance|memory\r\n"));
	test.Printf(KOptionPerformanceIterations);
	test.Printf(_L(" N\tthe number of iterations to perform\r\n"));
	test.Printf(KOptionUseFileHandles);
	test.Printf(_L("\tuse file handles for the test\r\n"));
	test.Printf(KOptionUseFullPaths);
	test.Printf(_L("\tuse file paths for the test\r\n"));
	}

void CTestConfig::UseageErr()
	{
		test.Printf(_L("\r\nUseage Err:\r\n"));
		DisplayHelp();
	}

void CTestConfig::Dump() 
	{	
	RDebug::Print(_L("\r\n"));
	RDebug::Print(_L("SMILTRANSLATORTEST Config Settings:\r\n"));
	RDebug::Print(_L("===================================\r\n"));
	RDebug::Print(_L("iNeedsHelp                 = %d\r\n"),iNeedsHelp);
	RDebug::Print(_L("iIsInteractive             = %d\r\n"),iIsInteractive);
	RDebug::Print(_L("iFiletype                  = %d\r\n"),iFileType);
	RDebug::Print(_L("iTestType                  = %d\r\n"),iTestType);
	RDebug::Print(_L("iDataMode                  = %d\r\n"),iDataMode);  
	RDebug::Print(_L("iPerformanceTestIterations = %d\r\n"),iPerformanceTestIterations);
	RDebug::Print(_L("iUseFileHanldes            = %d\r\n"),iUseFileHandles);
	}

TBool CTestConfig::UnaryArgPresent(CCommandLineArguments *aArgs, TInt aArgCount, const TDesC& aOption)
	{
	TInt i = 1;
	while ((i<aArgCount)&&aArgs->Arg(i).Compare(aOption))
		{
		++i;
		}
	return i<aArgCount;
	}

TInt CTestConfig::ExtractBinaryArg(CCommandLineArguments *aArgs, TInt aArgCount, const TDesC& aOption,TBuf<32>& aBuf)
	{
		TInt err=KErrNotFound;
		TInt i=0; 

		while ((i<aArgCount)&&aArgs->Arg(i).Compare(aOption)) 
			{
			++i;
			}
		if (i<aArgCount) 
			{
			if (++i<aArgCount) 
				{
				err=KErrNone;
				aBuf=aArgs->Arg(i);
				}
			else
				{
				err=KErrArgument;
				}
			}
		return err;
	}

void CTestConfig::InteractiveMode()
	{
	TKeyCode choice;

	test.Printf(KTxtAsciiOrUnicode);
	choice=test.Getch();

	if(choice==EKeyHelp)
		{
		iFileType=EAscii;
		}
	else if(choice==EKeyDial)
		{
		iFileType=EUtf8;
		}
	else
		{
		iFileType=EUnicode;
		}
	test.Printf(KTxtBufferOrFile);
	choice=test.Getch();
	if(choice==EKeyHelp)
		{
		iDataMode=EBufferData;
		}
	else
		{
		iDataMode=EFileData;
		}
		
	// Get user's input on whether to use file paths or file handles for the input files
	test.Printf(KOutputNewLine());
	test.Printf(KTxtChooseFileInput());
	choice=test.Getch();
	iUseFileHandles=EFalse;
	if(choice==EKeyHelp)          // F2
		{
		iUseFileHandles = ETrue;
		}

	test.Printf(KTxtChooseTest);
	choice=test.Getch();
	if(choice==EKeyHelp)        // F2
		{
		iTestType=EPerformance;
		}
	else if (choice==EKeyDial)  // F3
		{
		iTestType=EMemory;
		}
	else                          // any key
		{
		iTestType=EBasic;  
		}
	}

TInt CTestConfig::ProcessCommandLineL()
	{
	// Handle command line arguments
	CCommandLineArguments* args=CCommandLineArguments::NewLC();
	
	TInt argCount=args->Count();

	// Search for: "-h"/help parameter
	if (UnaryArgPresent(args,argCount,KOptionH))
		{
			iNeedsHelp = ETrue;
			CleanupStack::Pop(args);
			delete args;
			return KErrNone;
		}

	// Search for: "-i"/interactive parameter
	if(UnaryArgPresent(args,argCount,KOptionI)) 
		{
		if (argCount==2)
			{
			iIsInteractive=ETrue;
			}
		else
			{
			UseageErr();
			}
		} 
	// Search for: "-usefilehandles" parameter
	if(UnaryArgPresent(args,argCount,KOptionUseFileHandles)) 
		{
		iUseFileHandles=ETrue;		
		}
	// Search for: "-usefullpaths" parameter
	if(UnaryArgPresent(args,argCount,KOptionUseFullPaths))
		{
		iUseFileHandles=EFalse;
		}

	// Search for: --file-type, --data-mode, --test-type
	TBuf<32> buf;
	TInt err;
	if (((err=ExtractBinaryArg(args,argCount,KOptionFileType,buf))==KErrNone)&&(!FileTypeFromStr(buf))||
		  err==KErrArgument)
		{
		UseageErr();
		CleanupStack::PopAndDestroy(args);
		return KErrArgument;
		}
	if (((err=ExtractBinaryArg(args,argCount,KOptionDataMode,buf))==KErrNone)&&(!DataModeFromStr(buf))||
		  err==KErrArgument)
		{
		UseageErr();			
		CleanupStack::PopAndDestroy(args);
		return KErrArgument;
		}
    if (((err=ExtractBinaryArg(args,argCount,KOptionTestType,buf))==KErrNone)&&(!TestTypeFromStr(buf))||
		 err==KErrArgument)
		{
		UseageErr();
		CleanupStack::PopAndDestroy(args);
		return KErrArgument;
		}
    if (((err=ExtractBinaryArg(args,argCount,KOptionPerformanceIterations,buf))==KErrNone))
		{
		TLex16 lexer(buf.Ptr());
		TInt iterations;
		lexer.Val(iterations);
		iPerformanceTestIterations = iterations;
		}
	else if (err==KErrArgument)
		{
		UseageErr();
		CleanupStack::PopAndDestroy(args);
		return KErrArgument;
		}
	CleanupStack::PopAndDestroy(args);
	return KErrNone;
	}

//

class CTestDataSupplier : public CBase, public MMDXMLParserDataProvider
	{
public:
	static CTestDataSupplier* NewL(RFs &aRFs, const TDesC& aFileName);
	~CTestDataSupplier();

	// From MMDXMLParserDataProvided
	void GetData(TPtrC8 &aPtr, TRequestStatus &aStatus);
	void Disconnect();

private:
	void ConstructL(RFs &aRFs, const TDesC& aFileName);

private:
	HBufC8* iCurrentChunk;
	RFile iFile;
	TInt iChunkSize;	// Start at 1, then increment for subsequent chunk
	};


CTestDataSupplier* CTestDataSupplier::NewL(RFs &aRFs, const TDesC& aFileName)
	{
	CTestDataSupplier* self = new (ELeave) CTestDataSupplier();
	CleanupStack::PushL(self);
	self->ConstructL(aRFs, aFileName);
	CleanupStack::Pop(self);
	return self;
	}

CTestDataSupplier::~CTestDataSupplier()
	{
	iFile.Close();
	delete iCurrentChunk;
	}

// From MMDXMLParserDataProvided
// TODO: Should GetData be a leaving function? Allows more flexibility to implementations of this funtion?
void CTestDataSupplier::GetData(TPtrC8 &aPtr, TRequestStatus &aStatus)
	{
	// Read the data into the descriptor
	delete iCurrentChunk;
	iCurrentChunk = NULL;
	iCurrentChunk = HBufC8::NewL(iChunkSize);
	TPtr8 chunk = iCurrentChunk->Des();
	iFile.Read(chunk, iChunkSize); // Ignore the error code, assume end of file if we haven't read any data.

	TDataProviderResults result;

	if (iCurrentChunk->Length() != 0)
		{
		aPtr.Set(*iCurrentChunk);
		result = KMoreData;
		}
	else
		{
		// Assume that if we haven't got any data then we're at the end of the stream.
		result = KDataStreamEnd;
		}

//	iChunkSize++;
	TRequestStatus *s = &aStatus;
	User::RequestComplete(s, (TInt)result);
	return;
	}

void CTestDataSupplier::Disconnect()
	{
	// Don't need to do anything here.
	}

void CTestDataSupplier::ConstructL(RFs &aRFs, const TDesC& aFileName)
	{
	iChunkSize = 1;
	
	// Open the file that will supply the data
	User::LeaveIfError(iFile.Open(aRFs, aFileName, EFileRead));
	}


//
// CSmilTranslatorTestUtils declaration
//

class CSmilTranslatorTestUtils : public CActive, public MMDXMLParserObserver, public MMDXMLComposerObserver
	{
public:
	static CSmilTranslatorTestUtils* NewLC();
	~CSmilTranslatorTestUtils();
	void ConstructL();
    void RunTestL();
	TInt FilesProcessed() const {return iFilesProcessed; }

public: // from CActive
	void DoCancel();
	void RunL();

public: // from MMDXMLParserObserver
	void ParseFileCompleteL();

public: // from MMDXMLComposerObserver
	void ComposeFileCompleteL();

private:
	CSmilTranslatorTestUtils();
	void SetOutputFileName();
	void AppendErrorStr(TInt aError, TDes& aOutputMsg);
	void AppendSeverityStr(TInt aSeverity, TDes& aOutputMsg);

private:
	enum TComposerState
		{
		EComposing,
		ESizing
		};

    RFs iSession;
	CMDXMLDocument* iXMLDoc;


	CMDXMLParser* iParser;
	CMDXMLComposer* iComposer;
	TBuf<255> iInputFileName;
	TBuf<255> iOutputFileName;
	TComposerState iComposerState;
	TInt iSize;
	RFile iErrorFile;
	CTestDataSupplier* iDataSupplier;

	// return list of found files used by TFileFinder class
	CDir* iFileList;
		
	// buffer for composing the error messages for output to the screen and error file
	TBuf<255> iOutputMsg;

	TFindFile *iFileFinder;

	TInt iState;
	TInt iErr;
	TInt iIndex;

	enum TSmilTestStates
		{
		KInit = 0x00,
		KParseFile,
		KCheckResults,
		KEnd
		};

	TTime iStartTime;
	TTime iStartComposeTime;
	TInt64 iComposeTime;
	TInt iFilesProcessed;

	TBool iUseFileHandle;
	};

//===================================================================================

//
// CSmilTranslatorTestUtils definition
//

CSmilTranslatorTestUtils* CSmilTranslatorTestUtils::NewLC()
	{
	CSmilTranslatorTestUtils* self = new (ELeave) CSmilTranslatorTestUtils();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

//===================================================================================

CSmilTranslatorTestUtils::~CSmilTranslatorTestUtils()
	{
	delete iFileList;
	delete iXMLDoc;
	delete iParser;
	delete iComposer;
	iSession.Close();
	}

//===================================================================================

CSmilTranslatorTestUtils::CSmilTranslatorTestUtils() : CActive(EPriorityStandard), iFileFinder()
	{
	}

//===================================================================================

void CSmilTranslatorTestUtils::ConstructL()
	{ 

    iSession.Connect();
	iComposer = CMDXMLComposer::NewL(this);
#define VALIDATE
#ifndef VALIDATE
	iParser = CMDXMLParser::NewL(this);
#else
	CSMILDtd* smil = CSMILDtd::NewLC();
	iParser = CMDXMLParser::NewL(this, smil);
	CleanupStack::Pop(smil);
#endif
	
	iUseFileHandle = testConfig->UseFileHandles();

	iState = KInit;
	iErr = KErrNone;
	iStartTime.UniversalTime();
	iFilesProcessed = 0;

	CActiveScheduler::Add(this);

	TRequestStatus *s = &iStatus;
	User::RequestComplete(s, KErrNone);
	SetActive();
	}

//===================================================================================

void CSmilTranslatorTestUtils::RunL()
	{
	RunTestL();
	}

//===================================================================================

void CSmilTranslatorTestUtils::DoCancel()
	{
	}

//===================================================================================

#define DES_AS_8_BIT(str) (TPtrC8((TText8*)((str).Ptr()), (str).Size()))

void CSmilTranslatorTestUtils::RunTestL()
	{
	TRequestStatus *s = &iStatus;

	// used to generate a leave if an out of memory error was encountered, specifically
	// during the memory test loop in E32Main().  This is necessary because leaves in
	// the .dll Active Object RunL() functions do not return to this application, so
	// have to be Trapped in the Active objects and translated into an error code.
	TBool memoryError = EFalse;

	switch(iState)
		{
	case KInit:
		{
		// Utility class for file manipulation
		iFileFinder = new TFindFile(iSession);

		TPtrC errorFileName(KErrorFileName);

		// create err dir if doesn't exist - this api ignores the file name (ignores everything after final '/')
		fileSystem.MkDirAll(KErrorFileName);

		// overwrite any existing file of this name
		iErr = iErrorFile.Replace(iSession, errorFileName, EFileWrite | EFileStreamText);

		if(iErr == KErrNone)
			{
			TBuf<1> bom;
			bom.Append(CEditableText::EByteOrderMark);
			iErrorFile.Write(DES_AS_8_BIT(bom));
			// will search multiple directories, but halt after completing current directory
			// if at least one match is made.  Remembers which directories have been searched
			// in order to continue search using .FindWild() function later
			iErr = iFileFinder->FindWildByPath(KWildName, &KInputPathList, iFileList);
			iIndex = 0;
			}

		if(iErr == KErrNone)
			{
			iState = KParseFile;
			}
		else
			{
			iState = KEnd;
			}
		User::RequestComplete(s, KErrNone);
		SetActive();
		}
		break;

	case KParseFile:
		{
		++iFilesProcessed;
		iErr = KErrNone;

		TParse fullEntry;
		fullEntry.Set((*iFileList)[iIndex++].iName,& iFileFinder->File(),NULL);
		iInputFileName = fullEntry.FullName(); // extract individual path + name from list
		SetOutputFileName(); // output name is based on input one

		iOutputMsg = KStartFile;
		iOutputMsg.Append(iInputFileName);		// display full path
		test.Start(iOutputMsg);				// print to console

		// test console automatically places output on a new line, for output
		// to error file we need to add white space ready for next line
		iOutputMsg.Append(KOutputNewLine);
		iErrorFile.Write(DES_AS_8_BIT(iOutputMsg));	// print to error file

		// schedule Parser active object for call to it's RunL function
		if (testConfig->DataMode() == CTestConfig::EBufferData)
			{
			// We're testing the buffering API...
			// Create a data supplier object and pass it in to the parser
			delete iDataSupplier;
			iDataSupplier = NULL;
			iDataSupplier = CTestDataSupplier::NewL(iSession, iInputFileName);
			iParser->ParseSource(iDataSupplier);
			}
		else
			{
			if( iUseFileHandle )
				{
				RFile file;
				User::LeaveIfError(file.Open(iSession, iInputFileName, EFileRead | EFileShareReadersOnly));
			// 	No function declaration of ParseFile() that take RFile Object parameter
			//	iParser->ParseFile(file);
				iParser->ParseFile(iSession, iInputFileName);
				}
			else
				{
				// We're testing the file mode so parse the file.
				iParser->ParseFile(iSession, iInputFileName);
				}
			}

		iState = KCheckResults;
		iStatus = KRequestPending;
		SetActive();
		}
		break;

	case KCheckResults:
		{
		// when execution begins again one parse followed by a compose would have
		// completed for the current file, handle any error messages generated here
		iErr = iParser->Error();
		TInt severity = iParser->ErrorSeverity();
		if(iErr != KErrNone)
			{
			iOutputMsg = KParseError;
			AppendErrorStr(iErr, iOutputMsg);
			AppendSeverityStr(severity, iOutputMsg);
			iOutputMsg.Append(KOutputNewLine);
			
			// IF there are no more errors for this file bung in an
			// extra line to make output more prominent
			if(iComposer->Error() == KErrNone)
				{
				iOutputMsg.Append(KOutputNewLine);
				}
			test.Printf(iOutputMsg);						// print to console
			iErrorFile.Write(DES_AS_8_BIT(iOutputMsg));	// print to error file
			
			if(iErr == KErrNoMemory)
				{
				memoryError = ETrue;
				}
			}

		iErr = iComposer->Error();
		severity = iComposer->ErrorSeverity();
		if(iErr != KErrNone)
			{
			iOutputMsg = KComposeError;
			AppendErrorStr(iErr, iOutputMsg);
			AppendSeverityStr(severity, iOutputMsg);
			iOutputMsg.Append(KOutputNewLine);
			iOutputMsg.Append(KOutputNewLine);								
			test.Printf(iOutputMsg);
			iErrorFile.Write(DES_AS_8_BIT(iOutputMsg));
			
			if(iErr == KErrNoMemory)
				{
				memoryError = ETrue;
				}
			}

		test.End();

		// if the OOM condition occured during Parsing or Composing
		if(memoryError)
			{
			User::Leave(KErrNoMemory);
			}

		iState = KParseFile;

		if(iIndex >= iFileList->Count())
			{
			// fileList must be deleted after each loop prior to being passed
			// back to fileFinder (unnecessary after KErrNotFound)
			delete iFileList;
			iFileList = 0; // Just in case it doesn't get set in the FindWild

			// continue wildcard search for next directory in list
			iErr = iFileFinder->FindWild(iFileList);
			iIndex = 0;
			if(iErr != KErrNone)
				iState = KEnd;
			}

		SetActive();
		User::RequestComplete(s, KErrNone);
		}
		break;

	default:
	case KEnd:
		{
		TTime endTime;
		TTimeIntervalSeconds interval;
		endTime.UniversalTime();
		endTime.SecondsFrom(iStartTime, interval);

		TBuf<100> time;
		_LIT(KComposeTime, "Total time for composing: %d microseconds\n");
		time.Format(KComposeTime, iComposeTime);
		iErrorFile.Write(DES_AS_8_BIT(time));

		_LIT(KTimeTaken, "Total time for tests: %d seconds");
		time.Format(KTimeTaken, interval.Int());
		iErrorFile.Write(DES_AS_8_BIT(time));


		delete iFileFinder;
		delete iDataSupplier;
		CActiveScheduler::Stop();
		}
		break;
		}
	}

//===================================================================================

void CSmilTranslatorTestUtils::AppendErrorStr(TInt aError, TDes& aOutputMsg)
	{
	switch(aError)
		{
		case KErrXMLBadAttributeValue:
			aOutputMsg.Append(_L("Bad Attribute Value")); break;

		case KErrXMLBadAttributeName:
			aOutputMsg.Append(_L("Bad Attribute Name")); break;

		case KErrXMLInvalidChild:
			aOutputMsg.Append(_L("Invalid Child")); break;

		case KErrXMLBadNesting:
			aOutputMsg.Append(_L("Bad Nesting")); break;

		case KErrXMLIncomplete:
			aOutputMsg.Append(_L("Incomplete")); break;

		case KErrXMLBadElementName:
			aOutputMsg.Append(_L("Bad Element Name")); break;

		case KErrXMLDuplicateDocTypeTags:
			aOutputMsg.Append(_L("Duplicate DocType Tags")); break;

		case KErrXMLDuplicateVersionTags:
			aOutputMsg.Append(_L("Duplicate Version Tags")); break;

		case KErrXMLDuplicateRootElements:
			aOutputMsg.Append(_L("Duplicate Root Elements")); break;

		case KErrXMLMissingDocTypeTag:
			aOutputMsg.Append(_L("Missing DocType Tag")); break;

		case KErrXMLMissingVersionTag:
			aOutputMsg.Append(_L("Missing Version Tag")); break;

		case KErrXMLDuplicateAttributeName:
			aOutputMsg.Append(_L("Duplicate Attribute Name")); break;

		case KErrXMLMultipleRootElements:
			aOutputMsg.Append(_L("Mulitiple Root Elements")); break;

		case KErrXMLCorruptFile:
			aOutputMsg.Append(_L("Corrupt File")); break;

		case KErrXMLIllegalCharacter:
			aOutputMsg.Append(_L("Illegal Character")); break;

		case KErrXMLBadEntity:
			aOutputMsg.Append(_L("Malformed Entity")); break;

		case KErrXMLInvalidElement:
			aOutputMsg.Append(_L("Invalid Element")); break;

		case KErrXMLInvalidAttribute:
			aOutputMsg.Append(_L("Invalid Attribute")); break;

		case KErrPathNotFound:
			aOutputMsg.Append(_L("File Path Not Found")); break;

		case KErrNoMemory:
			aOutputMsg.Append(_L("Memory Allocation Failure")); break;

		case KErrNotSupported:
			aOutputMsg.Append(_L("Not Supported")); break;

		default:
			aOutputMsg.Append(_L("Unknown Error")); break;
		}
	}

//===================================================================================
void CSmilTranslatorTestUtils::AppendSeverityStr(TInt aSeverity, TDes& aOutputMsg)
	{
	aOutputMsg.Append(_L(", Severity ")); 
	switch(aSeverity)
		{
		case EXMLFatal:
			aOutputMsg.Append(_L("Fatal")); 
			break;
		case EXMLIndeterminate:
			aOutputMsg.Append(_L("Indeterminate")); 
			break;
		case EXMLWorkable:
			aOutputMsg.Append(_L("Workable")); 
			break;
		case EXMLNone:
			aOutputMsg.Append(_L("None")); 
			break;
		default:
			aOutputMsg.Append(_L("Unknown"));
			break;
		}
	}
//===================================================================================

void CSmilTranslatorTestUtils::ParseFileCompleteL()
// call back function called from Parser::RunL()
	{
	// iXMLDoc ends up owned by this class, must delete off the old one on each pass.
	delete iXMLDoc;
	iXMLDoc = NULL;

	// get parsed file, don't worry about errors, composer should be robust enough to handle bad files
	iXMLDoc = iParser->DetachXMLDoc();

	iComposerState = EComposing;
	TRequestStatus *s = &iStatus;

	iStartComposeTime.UniversalTime();

	// create output dir if doesn't exist - this api ignores the file name (ignores everything after final '/')
	fileSystem.MkDirAll(iOutputFileName);

	// schedule composer active object for call to it's RunL function

	if( iUseFileHandle )
		{
		RFile file;
		TInt fileError = file.Replace(iSession, iOutputFileName, EFileWrite | EFileStream);

		if( fileError==KErrNone )
			{
		// 	No function declaration of ComposeFile() that take RFile Object parameter
//			iErr = iComposer->ComposeFile(file, iXMLDoc, filetype);
			iErr = iComposer->ComposeFile(iSession, iOutputFileName, iXMLDoc, testConfig->FileType());
			}
		else
			{
			// if a file error has occured, we need to set the internal error state
			// and can only do this by trying to compose again with the filepath so
			// it fails internally
			iErr = iComposer->ComposeFile(iSession, iOutputFileName, iXMLDoc, testConfig->FileType());
			}
		}
	else
		{
		iErr = iComposer->ComposeFile(iSession, iOutputFileName, iXMLDoc, testConfig->FileType());
		}

	// we are waiting on this event...
	User::RequestComplete(s, KErrNone);
	}

//===================================================================================

void CSmilTranslatorTestUtils::ComposeFileCompleteL()
// call back function called from Composer::RunL()
	{
	TTime timeNow;
	timeNow.UniversalTime();
	TTimeIntervalMicroSeconds timeForCompose = timeNow.MicroSecondsFrom(iStartComposeTime);
	iComposeTime += timeForCompose.Int64();

	if (iComposerState == ESizing)
		{
		// Check the size of the file that has been written against the size calulated by the
		// call to CMDXMLComposer::CalculateFileSize

		RFile outputXMLFile;
		outputXMLFile.Open(iSession, iOutputFileName, EFileRead);

		TInt actualSize;
		User::LeaveIfError(outputXMLFile.Size(actualSize));
		
		if (iSize != actualSize)
			{
			// The calculated file size doesn't match the real file size, this test has failed
			TBuf<255> outputMsg;

			outputMsg.Append(KOutputNewLine);
			outputMsg.Append(_L("Test Failed - The calculated file size doesn't match the actual size."));
			outputMsg.Append(KOutputNewLine);			
			
			test.Printf(outputMsg);						// print to console
			iErrorFile.Write(DES_AS_8_BIT(outputMsg));	// print to error file			
			}

		outputXMLFile.Close();

		// If we are sizing then stop the active scheduler. Once the scheduler is stopped
		// and this function exits, program control resumes where the scheduler was started
		// in RunTestL.
//		CActiveScheduler::Stop();
		}

	else if (iComposerState == EComposing)
		{
		// The XML file has been composed. Now we need to run the sizing function to check
		// that we can calculate the size correctly.

		// Set the state to sizing and run the sizing operation...
		iComposerState = ESizing;

		// Calculate the file size and wait for the callback to this function again.
		iComposer->CalculateFileSize(iSize, iXMLDoc, testConfig->FileType());
		}
	}

//===================================================================================

void CSmilTranslatorTestUtils::SetOutputFileName()
	{
	TInt offset;

	iOutputFileName = iInputFileName;
	if((offset = iOutputFileName.Find(KTestFindStr)) != KErrNotFound)
		{
		iOutputFileName.Replace(offset, TPtrC(KTestFindStr).Length(), KTestReplaceStr);
		}
	}

//===================================================================================

//
// TestHarness implementation
//



LOCAL_C TInt startTestL()
	{
	TInt err = KErrNone;

	// we may need to make some output dirs if they don't already exist
	fileSystem.Connect();

	scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install( scheduler );

	CSmilTranslatorTestUtils* ttu=CSmilTranslatorTestUtils::NewLC(); 
	
	// suspend execution until active object scheduler is finished
	CActiveScheduler::Start();

	if (ttu->FilesProcessed()==0)
		err=KErrNotFound;

	fileSystem.Close();
	CleanupStack::PopAndDestroy(2, scheduler); //scheduler, as well as the object
			                                   //placed on the stack by CSmilTranslatorTestUtils::NewLC();
	return err;
	}

LOCAL_C TInt doMainL()
	{
	testConfig = CTestConfig::NewLC();

	// set command line options
	TInt err=testConfig->ProcessCommandLineL();		
	if(err!=KErrNone)
		return err;

	// users specified -h: display help to console and abort
	if (testConfig->NeedsHelp())
		{
		testConfig->DisplayHelp();
		test.Getch();
		return KErrNone;
		}

	// user specified -i: let them override settings inside the console
	if (testConfig->IsInteractive())
		testConfig->InteractiveMode();

#ifdef DEBUG_SMILTRANSLATORTEST_
	testConfig->Dump();
#endif

	TInt returnCode = KErrNone;

	// performance Performance Test
	if (testConfig->TestType() == CTestConfig::EPerformance)
		{
		TInt loopFor = 0;
		do
			{
			loopFor++;
			returnCode=startTestL();	// Qualified: false leavescan error		
			}
		while(loopFor!=testConfig->PerformanceTestIterations() && returnCode == KErrNone);
		}
	else if (testConfig->TestType()==CTestConfig::EMemory)
		{
		TInt after = 0;
		do
			{
			after++;
			User::__DbgSetAllocFail(RHeap::EUser, RHeap::EDeterministic, after);
			returnCode=startTestL();
			}
		while(returnCode != KErrNone);
		}
	// Assume user wants a basic test
	else
		{
		returnCode=startTestL();										
		}	

	CleanupStack::Pop(testConfig);
	return returnCode;
	}
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	theCleanup=CTrapCleanup::New();
	test.Start(_L("Smil Translator"));
	TInt returnCode=KErrNone;
	TRAP(returnCode,returnCode=doMainL());
	test(returnCode==KErrNone);
	delete testConfig;
	delete theCleanup;	
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	User::Heap().Check();
	return(KErrNone);
	}

// End Of File
