// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// $Workfile: t_main.cpp $
// $Author: Stevep $
// $Revision: 6 $
// $Date: 27/03/02 11:13 $
// 
//


#include <e32std.h>
#include <c32comm.h>
#include <bacline.h>

#include "obextestutils.h"
#include "harness.h"
#include "parsers.h"
#include "obexparsers.h"


// Name of test harness.
_LIT(KObexTestName, "OBEX Mtm Tests");
_LIT(KObexTestExeName, "T_OBEXMTM");

//Name of the main script file.
_LIT(KScriptFileName, "c:\\msgtest\\obexsmoke\\scripts\\script.txt");

// Where the test harness places the log file.
_LIT(KMsvMsgLogsSrcPath, "c:\\MsgLogs\\");

// Device drivers.
#if defined (__WINS__)
#define PDD_NAME _L("ECDRV")
#define LDD_NAME _L("ECOMM")
#else  // __GCC32__
#define PDD_NAME _L("EUART2")
#define LDD_NAME _L("ECOMM")
#define FIR_PDD_NAME _L("DIFIR")	// Brutus FIR PDD
#define FIR_LDD_NAME _L("EFIR")		// Brutus FIR LDD
#endif


void AppendVariantName(TDes& aFileName)
	{
#if defined(__WINS__) && defined(__WINSCW__)
	aFileName.Append(_L(".WINSCW."));
#elif(__WINS__)
	aFileName.Append(_L(".WINS."));
#endif
#if (defined(__THUMB__) || defined(__MARM_THUMB__))
	aFileName.Append(_L(".THUMB."));
#endif
#if (defined(__ARMI__) || defined(__MARM_ARMI__))
	aFileName.Append(_L(".ARMI."));
#endif
#if (defined(__ARM4__) || defined(__MISA__) || defined(__MARM_ARM4__))
	aFileName.Append(_L(".ARM4."));
#endif

#if defined(_DEBUG)
	aFileName.Append(_L("DEB."));
#else
	aFileName.Append(_L("REL."));
#endif

	aFileName.Append(_L("LOG"));
	}


void MoveLogsL(RFs& aRfs, const TDesC& aTestScript)
	{
	//	Move the test log directory and file.
	//
	//	For example: test script "c:\msgtest\mmssmoke\scripts\MTM_Registering_Script.txt",
	//	under a WINS UDEB build, test harness name T_OBEXMTM.
	//
	//	dir src = c:\MsgLogs\T_OBEXMTM
	//	dir dst = c:\MsgLogs\T_OBEXMTM_MTM_Registering_Script
	//
	//	file src = c:\MsgLogs\T_OBEXMTM.WINS.DEB.LOG
	//	file dst = c:\MsgLogs\T_OBEXMTM_MTM_Registering_Script.WINS.DEB.LOG

	CFileMan*	fileMan = CFileMan::NewL(aRfs);
	CleanupStack::PushL(fileMan);

	//
	// Determine dir name for src.
	TFileName	msgLogsSrc;

	msgLogsSrc.Append(KMsvMsgLogsSrcPath);	// Add "c:\MsgLogs\".
	msgLogsSrc.Append(KObexTestExeName);	// Add "T_OBEXMTM".

	//
	// Determine dir name for dst.
	TFileName	msgLogsDst;
	TInt		fileErr;
	TInt		loc;
	TInt		len;

	msgLogsDst = msgLogsSrc;	// Add "c:\MsgLogs\T_OBEXMTM".
	msgLogsDst.Append('_');		// Add "_".

	// Add test name.
	len = aTestScript.Length();
	loc = aTestScript.LocateReverse('\\');
	msgLogsDst.Append(aTestScript.Right(len - loc - 1));

	// Remove the ".ext".
	msgLogsDst = msgLogsDst.Left(msgLogsDst.Locate('.'));

	//
	// Move the dir.
	fileErr = fileMan->Move(msgLogsSrc, msgLogsDst);

	// Move will not delete existing directories, so we have to do it ourself.
	// The documentation says it will delete directories !!!!
	if (fileErr == KErrAlreadyExists)
		{
		msgLogsDst.Append('\\');								// To ensure we delete the correct directory.
		fileErr = fileMan->RmDir(msgLogsDst);
		msgLogsDst = msgLogsDst.Left(msgLogsDst.Length() - 1);	// Remove '\' again.
		if (fileErr == KErrNone)
			fileErr = fileMan->Move(msgLogsSrc, msgLogsDst);
		}

	//
	// Now move the file. Append ".WINS.DEB.LOG" to src and dst.
	AppendVariantName(msgLogsSrc);
	AppendVariantName(msgLogsDst);

	fileErr = fileMan->Move(msgLogsSrc, msgLogsDst);

	CleanupStack::PopAndDestroy(fileMan);
	}


TInt RunTestL(RFs& aRfs, RTest& aTest, const TDesC& aTestScript)
	{
	TInt err = KErrNone;

	// Create the test script object, which will use the test script
	CTestScript* script = CTestScript::NewL(aRfs);
	CleanupStack::PushL(script);

	script->LoadFileL(aTestScript);

	// Create the main test harness, to which all test states will
	// be appended.
	CMainTestHarness* mainTest = CMainTestHarness::NewL(aTest);
	CleanupStack::PushL(mainTest);
	
	TTestDebugInfo debugInfo(*script, 0, 0);
	
	mainTest->SetDebugInfo(debugInfo);

	// Create a new section parser.
	CObexMainSectionParser* mainSectionParser = CObexMainSectionParser::NewL(*mainTest, *script);
	CleanupStack::PushL(mainSectionParser);

	// Attempt to parse the script
	TRAP(err, mainSectionParser->ParseL());
	if (err == KErrNone)
		{
		// If we've parsed the script OK, attempt to run the test.
		// The "waiter" object is provided just in case we don't
		// want to start and stop the active scheduler.
		CTestActive* waiter = new (ELeave) CTestActive;
		CleanupStack::PushL(waiter);

		waiter->StartL();
		mainTest->StartL(waiter->iStatus);
		CActiveScheduler::Start();
		err = waiter->iStatus.Int();
		
		CleanupStack::PopAndDestroy(waiter);
		}

	CleanupStack::PopAndDestroy(mainSectionParser);
	CleanupStack::PopAndDestroy(mainTest);
	CleanupStack::PopAndDestroy(script);

	return err;
	}


TInt doMainL()
	{
	TInt err = KErrNone;

	// Create the test object which we'll use for output etc.
	RTest test(KObexTestName);
	CleanupClosePushL(test);

	// Load Device drivers ... may have to move this...

#ifdef __EPOC32__
	StartC32();
#endif
	User::LoadPhysicalDevice(PDD_NAME);
    User::LoadLogicalDevice(LDD_NAME);
#ifdef __EPOC32__
	User::LoadPhysicalDevice(FIR_PDD_NAME);
	User::LoadLogicalDevice(FIR_LDD_NAME);
#endif	

	// Start the file system.
	RFs rfs;
	User::LeaveIfError(rfs.Connect());
	CleanupClosePushL(rfs);

	// Start a new active scheduler.
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);
	
	// Name of the actual script being executed.
	HBufC*	scriptFileName = HBufC::NewLC(KMaxFileName);
	TPtr	strPtr = scriptFileName->Des();

	_LIT(KInputScriptParameterId, "-i");
	_LIT(KScriptNotFoundErr, "%S not found!\n\n");

	// Handle command line arguments
	CCommandLineArguments*	args = CCommandLineArguments::NewLC();
	TInt					argCount = args->Count();
	TInt					currArg = 0;

	// Search for "-i" parameter.
	while ((currArg < argCount) && args->Arg(currArg).Compare(KInputScriptParameterId))
		{
		currArg++;
		}
	
	// Input script filename argument NOT present
	//
	// Run multiple test scripts, held in KScriptFileName.
	if ((currArg == argCount) || (currArg == (argCount - 1)))
		{
		CleanupStack::PopAndDestroy(args); // Done with arguments.
		
		// Load the main script file, used to determine which actual test scripts are run.
		CTestScript*	mainScript = CTestScript::NewL(rfs);
		CleanupStack::PushL(mainScript);

		if (mainScript->LoadFileL(KScriptFileName))
			{
			CTestScript::TTestScriptString		line;
			TBool								lineGot = false;
			TInt								linePos = 0;


			// For each valid line in the main script file, run the test.
			lineGot = mainScript->GetLineL(line, linePos);
			while (lineGot)
				{
				if (line.Length() > 0)
					{
					TInt	fileErr;
					TBool	dummy = TRUE;


					// Determine the test script file name and path.
					strPtr.Copy(line);
					strPtr.Insert(0, KScriptFileName().Left(KScriptFileName().LocateReverse('\\') + 1)); // inserts path

					// Ensure file exsits.
					fileErr = rfs.IsFileOpen(scriptFileName->Des(), dummy);
					if (fileErr == KErrNone)
						{
						// Start the test.
						err = RunTestL(rfs, test, scriptFileName->Des());

						// Move the output log.
						MoveLogsL(rfs, scriptFileName->Des());
						}
					}

				else
					{
					test.Printf(KScriptNotFoundErr, scriptFileName);
					}

				linePos += line.Length() + 2;	// 2 for cr/lf pair.
				lineGot = mainScript->GetLineL(line, linePos);
				}
			}

		else
			{
			test.Printf(KScriptNotFoundErr, &KScriptFileName);
			User::After(5000000);
			}
	
		CleanupStack::PopAndDestroy(mainScript);
		}
	
	// Run single test script.
	else
		{
		TInt	fileErr;
		TBool	dummy = TRUE;

		// Determine the test script file name and path.
		strPtr.Copy(args->Arg(currArg + 1));
		strPtr.Insert(0, KScriptFileName().Left(KScriptFileName().LocateReverse('\\') + 1)); // inserts path
		
		CleanupStack::PopAndDestroy(args);	// Done with arguments.
		
		// Ensure file exsits.
		fileErr = rfs.IsFileOpen(scriptFileName->Des(), dummy);
		if (fileErr == KErrNone)
			{
			// Start the test.
			err = RunTestL(rfs, test, scriptFileName->Des());

			// Move the output log.
			MoveLogsL(rfs, scriptFileName->Des());
			}
		else
			{
			test.Printf(KScriptNotFoundErr, scriptFileName);
			User::After(5000000);
			}
		}

	CleanupStack::PopAndDestroy(scriptFileName);
	CleanupStack::PopAndDestroy(scheduler);
	CleanupStack::PopAndDestroy();	// rfs
	CleanupStack::PopAndDestroy();	// test

	return err;
	}


LOCAL_D CTrapCleanup* theCleanup;

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	theCleanup=CTrapCleanup::New();

	TInt status = KErrNone;

	TRAPD(ret, status = doMainL());		

	delete theCleanup;	
	__UHEAP_MARKEND;
	User::Heap().Check();
	return status;
	}
