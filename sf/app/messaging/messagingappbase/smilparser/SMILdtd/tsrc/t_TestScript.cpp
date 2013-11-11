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
// Basic test script handling.
// 
//


#include "t_TestScript.h"


_LIT(KCantOpenFile, "Could not open file: %S");
_LIT(KMiscFileError, "File error");

_LIT(KInfoTestFinished, "[%4d] Test finished: %S");
_LIT(KInfoTestStarted, "[%4d] Starting test: %S");


//
//
// CTestScript
//

CTestScript::~CTestScript()
	{
	iFile.Close();
	}


CTestScript* CTestScript::NewL(RFs& aFs)
	{
	CTestScript* self = new (ELeave) CTestScript(aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


CTestScript::CTestScript(RFs& aFs)
: iFs(aFs)
	{
	}


void CTestScript::ConstructL()
	{
	}


TBool CTestScript::LoadFileL(const TDesC& aFileName)
	{
	iFile.Close();
	iEndOfFile = EFalse;
	TBool loaded = EFalse;

	TInt err = iFile.Open(iFs, aFileName, EFileShareAny);

	if (err == KErrNone)
		{
		loaded = ETrue;
		}
	else
		{
		iLastError.Format(KCantOpenFile, aFileName);
		}

	return loaded;
	}


TBool CTestScript::GetLineL(TTestScriptString& aLine)
	{
	TBool noError = EFalse;
	TInt startPosition = 0;

	// Save the current position
	TInt err = iFile.Seek(ESeekCurrent, startPosition);

	if (err == KErrNone)
		{
		// Read a block of data
		TTestScriptString dataFromFile;
		err = iFile.Read(dataFromFile);

		if (err != KErrNone)
			{
			iLastError.Copy(KMiscFileError);
			}
		else
			{
			// Look for the cr/lf pair, or the end of the line
			TInt bytesOnLine = 0;
			TBool finished = EFalse;
			while (!finished)
				{
				if (bytesOnLine == dataFromFile.Size())
					{
					finished = ETrue;
					}
				else if (dataFromFile[bytesOnLine] == 0x0d)
					{
					finished = ETrue;
					}
				else
					{
					bytesOnLine++;
					}
				}

			TInt startOfNextLine = startPosition + bytesOnLine + 1;

			if (bytesOnLine + 1 < dataFromFile.Size())
				{
				if (dataFromFile[bytesOnLine + 1] == 0x0a)
					{
					startOfNextLine++;
					}
				}

			// Seek to after the cr/lf pair
			iFile.Seek(ESeekStart, startOfNextLine);

			// Set the line
			aLine.Copy(dataFromFile.Left(bytesOnLine));

			// Get rid of any trailing white-space
			TInt whiteSpaceIndex = aLine.Size();
			TBool noMoreWhiteSpace = EFalse;
			while ((whiteSpaceIndex--) && (!noMoreWhiteSpace))
				{
				if ((aLine[whiteSpaceIndex] == ' ')
				||  (aLine[whiteSpaceIndex] == '\t'))
					{
					aLine.SetLength(whiteSpaceIndex);
					}
				else
					{
					noMoreWhiteSpace = ETrue;
					}
				}

			if (dataFromFile.Size() != 0)
				{
				noError = ETrue;
				}
			}
		}

	return noError;
	}

