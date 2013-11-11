// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name of test harness: T_SMIL_DOM
// Input file required to run test harness:
// \epoc32\wins\c\MsgTest\SMIL_DOM\script.txt
// Output file produced by running test harness:
// \epoc32\wins\c\MsgTest\SMIL_DOM\SmilDomLog.txt
// This script driven test harness tests the SMIL DOM classes. Tree navigation, and attribute
// getting and setting methods can be exercised by means of script commands. The test harness
// uses the concepts of the current element in a DOM tree, and the numeric position of a
// child element, in its child list. It logs results (including errors) for verification.
// The test harness may be used to perform the following tests:
// Exercise of each of the element creation and navigation routines. Each routine may be
// exercised in a valid way and also, where applicable, in invalid ways (for example, inserting
// before non-existent elements in a list).
// Simple attributes (e.g. id) can be used to check position during navigation. Elements should
// have their ids set when they are created, and various navigational paths taken to elements
// with known ids.
// Note that the harness is allowed to fail, if it is given a script containing errors; so not
// a lot of work has gone into handling errors.
// 
//


#include "t_TestScript.h"
#include "t_ScriptRunner.h"
#include <e32test.h>
#include <e32std.h>
#include <f32file.h>


// This is used if we can't find MsgLogs anywhere
_LIT(KMsvTestFileDefaultOutputBase, "C:\\MsgLogs\\");
_LIT(KMsvPathSep, "\\");
_LIT(KMsvTestFileInputBase, "MsgTest");
_LIT(KMsvTestFileOutputBase, "MsgLogs");


TInt ResolveFile(RFs& aFs, const TDesC& aComponent, const TDesC& aFileName, TParse& aParseOut)
	{
	TFileName* savedPath = new TFileName;
	TFileName* fileName = new TFileName;

	if ((savedPath == NULL) || (fileName == NULL))
		{
		return KErrNoMemory;
		}

	fileName->Append(KMsvPathSep);
	fileName->Append(KMsvTestFileInputBase);
	fileName->Append(KMsvPathSep);
	fileName->Append(aComponent);
	fileName->Append(KMsvPathSep);
	fileName->Append(aFileName);

	// File finder will look in the session drive first, then Y->A,Z
	// so set session drive to Y (save old, and restore it afterwards).
	aFs.SessionPath(*savedPath);
	_LIT(KTopDrive, "Y:\\");
	aFs.SetSessionPath(KTopDrive);
	TFindFile file_finder(aFs);
	TInt err = file_finder.FindByDir(*fileName, KNullDesC);

	if (err == KErrNone)
		{
		aParseOut.Set(file_finder.File(), NULL, NULL);
		}

	aFs.SetSessionPath(*savedPath);
	delete savedPath;
	delete fileName;
	return (err);
	}


//
//
// Main Test Harness
//

LOCAL_D CTrapCleanup* theCleanup;


void doMainL()
	{
	RTest test(_L("SMIL DOM tests"));
	RFs fs;
	fs.Connect();

	CTestScript* script = CTestScript::NewL(fs);
	CleanupStack::PushL(script);
	TParse scriptFileName;

	TInt err = ResolveFile(fs, _L("SMIL_DOM"), _L("script.txt"), scriptFileName);

	if (err != KErrNone)
		{
		// This should report unable to load file
		}

	script->LoadFileL(scriptFileName.FullName());

	CScriptRunner* scriptRunner = CScriptRunner::NewL();
	CleanupStack::PushL(scriptRunner);

	TBuf8<128> line;
	TBuf<128> uniLine;
	while (script->GetLineL(line))
		{
		uniLine.Copy(line);
		scriptRunner->ParseL(uniLine);
		}

	CleanupStack::PopAndDestroy(scriptRunner);
	CleanupStack::PopAndDestroy(script);

	fs.Close();
	test.Close();
	}


GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	theCleanup = CTrapCleanup::New();

	TRAPD(ret, doMainL());

	delete theCleanup;
	__UHEAP_MARKEND;
	User::Heap().Check();
	return (KErrNone);
	}

