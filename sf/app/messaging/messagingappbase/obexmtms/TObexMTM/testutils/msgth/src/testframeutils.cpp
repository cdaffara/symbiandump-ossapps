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
//

#include "testframeutils.h"

_LIT(KCantOpenFile, "Could not open file: %S");
_LIT(KSectionNotFound, "Could not find section: %S");
_LIT(KMiscFileError, "File error");

_LIT(KInfoTestFinished, "[%4d] Test finished: %S");
_LIT(KInfoTestStarted, "[%4d] Starting test: %S");
_LIT(KInfoTestError, "[%4d] Test failed in section: %S, command %S, reason %d");


//
//
// TTestDebugInfo
//

EXPORT_C TInt TTestDebugInfo::LineNumber()
	{
	return iLineNumber;
	}

EXPORT_C HBufC* TTestDebugInfo::TestStartStringLC()
	{
	TBuf<256> startString;
	HBufC* sectionName = TestHarnessNameLC();
	startString.Format(KInfoTestStarted, iLineNumber, sectionName);
	CleanupStack::PopAndDestroy(sectionName);
	HBufC* startStringHBufC = HBufC::NewLC(startString.Length());
	(startStringHBufC->Des()).Copy(startString);
	return startStringHBufC;
	}

EXPORT_C HBufC* TTestDebugInfo::TestCompleteStringLC()
	{
	TBuf<256> startString;
	HBufC* sectionName = TestHarnessNameLC();
	startString.Format(KInfoTestFinished, iLineNumber, sectionName);
	CleanupStack::PopAndDestroy(sectionName);
	HBufC* startStringHBufC = HBufC::NewLC(startString.Length());
	(startStringHBufC->Des()).Copy(startString);
	return startStringHBufC;
	}

EXPORT_C HBufC* TTestDebugInfo::TestFailedStringL(TInt aReason)
	{
	TBuf<256> failedString;
	HBufC* sectionName = SectionNameLC();
	HBufC* commandName = CommandNameLC();
	failedString.Format(KInfoTestError, iLineNumber, sectionName, commandName, aReason);
	CleanupStack::PopAndDestroy(2, sectionName);
	HBufC* failedStringHBufC = HBufC::NewL(failedString.Length());
	(failedStringHBufC->Des()).Copy(failedString);
	return failedStringHBufC;
	}

EXPORT_C void TTestDebugInfo::SetTestHarnessName(TInt aTestHarnessPosition)
	{
	iTestHarnessPosition = aTestHarnessPosition;
	}

EXPORT_C HBufC* TTestDebugInfo::SectionNameLC()
	{
	TBuf8<128> sectionName;
	iScriptFile->GetLineL(sectionName, iSectionPosition);
	HBufC* sectionNameHBufC = HBufC::NewLC(sectionName.Length());
	sectionNameHBufC->Des().Copy(sectionName);
	return sectionNameHBufC;
	}

EXPORT_C HBufC* TTestDebugInfo::TestHarnessNameLC()
	{
	TBuf8<128> sectionName;
	iScriptFile->GetLineL(sectionName, iTestHarnessPosition);
	HBufC* sectionNameHBufC = HBufC::NewLC(sectionName.Length());
	sectionNameHBufC->Des().Copy(sectionName);
	return sectionNameHBufC;
	}

EXPORT_C HBufC* TTestDebugInfo::CommandNameLC()
	{
	TBuf8<128> commandName;
	iScriptFile->GetLineL(commandName, iCommandPosition);
	HBufC* commandNameHBufC = HBufC::NewLC(commandName.Length());
	commandNameHBufC->Des().Copy(commandName);
	return commandNameHBufC;
	}

EXPORT_C TTestDebugInfo::TTestDebugInfo(CTestScript& aTestScript, TInt aSectionPosition, TInt aCommandPosition, TInt aLineNumber) : iScriptFile(&aTestScript), iSectionPosition(aSectionPosition), iCommandPosition(aCommandPosition), iLineNumber(aLineNumber)
	{
	}	

EXPORT_C TTestDebugInfo::TTestDebugInfo(CTestScript& aTestScript, TInt aSectionPosition, TInt aCommandPosition) : iScriptFile(&aTestScript), iSectionPosition(aSectionPosition), iCommandPosition(aCommandPosition), iLineNumber(0)
	{
	}

EXPORT_C TTestDebugInfo::TTestDebugInfo() :  iScriptFile(0), iSectionPosition(0), iCommandPosition(0), iLineNumber(0)
	{
	}


//
//
// CTestScript
//

EXPORT_C CTestScript::~CTestScript()
	{
	delete iSectionPositions;
	iFile.Close();
	}

EXPORT_C CTestScript* CTestScript::NewL(RFs& aFs)
	{
	CTestScript* self = new (ELeave) CTestScript(aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CTestScript::ConstructL()
	{
	iSectionPositions = new (ELeave) CArrayFixFlat<TSectionPosition>(10);
	}

EXPORT_C TBool CTestScript::LoadFileL(const TDesC& aFileName)
	{
	iFile.Close();
	TBool loaded = EFalse;

	iSectionPositions->ResizeL(0);
	iEndOfFile = EFalse;

	TTestScriptString line;

	TInt err = iFile.Open(iFs, aFileName, EFileShareAny);

	if (err == KErrNone)
	// Scan the file for file sections, eg. [section1]
		{
		TInt lineNumber = 0;
		TSectionPosition section;
		section.iSectionPosition = 0;
		while (GetLineL(line))
			{
			lineNumber++;
			if (IsSection(line))
			// If the line is the start of a section then add it to the list.
				{
				// Strip the '[' and ']' characters
				line = line.Mid(1, line.Size() - 2);

				// Store the section name and the position
				section.iSectionName = line;
				section.iFilePosition = 0;
				section.iLineNumber = lineNumber;
				err = iFile.Seek(ESeekCurrent, section.iFilePosition);
				iSectionPositions->AppendL(section);
				}
			section.iSectionPosition = 0;
			err = iFile.Seek(ESeekCurrent, section.iSectionPosition);
			}

		loaded = ETrue;
		}
	else
		{
		iLastError.Format(KCantOpenFile, &aFileName);
		}

	return loaded;
	}

EXPORT_C CTestSection* CTestScript::GetSectionL(const TDesC& aSectionName)
	{
	CTestSection* newSection = 0;

	TSectionPosition section;
	TTestScriptString command;
	TBool sectionFound = EFalse;

	// Look for the section name in the array
	TInt sectionIndex = iSectionPositions->Count();
	TBuf<128> tempSectionNameU;
	while ((sectionIndex--) && (!sectionFound))
		{
		tempSectionNameU.Copy(((*iSectionPositions)[sectionIndex]).iSectionName);
		if (tempSectionNameU == aSectionName)
			{
			section = (*iSectionPositions)[sectionIndex];
			sectionFound = ETrue;
			}
		}

	if (!sectionFound)
		{
		iLastError.Format(KSectionNotFound, aSectionName);
		}
	else
		{
		newSection = CTestSection::NewL(section.iSectionPosition, tempSectionNameU, section.iLineNumber);
		CleanupStack::PushL(newSection);
		TInt err = iFile.Seek(ESeekStart, section.iFilePosition);
		if (err == KErrNone)
			{
			TInt commandLineNumber = section.iLineNumber;
			TInt commandPosition = 0;
			iFile.Seek(ESeekCurrent, commandPosition);
			while (GetNextCommandInSectionL(command, commandPosition, commandLineNumber))
				{
				newSection->AddCommandL(command, commandPosition, commandLineNumber);
				commandPosition = 0;
				iFile.Seek(ESeekCurrent, commandPosition);
				}
			}
		else
			{
			iLastError = KMiscFileError;
			}
		CleanupStack::Pop(newSection);
		}

	return newSection;
	}

CTestScript::CTestScript(RFs& aFs) : iFs(aFs)
	{
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
			// Look for the cr/lf pair or the end of the line
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
			if (bytesOnLine+1 < dataFromFile.Size())
				{
				if (dataFromFile[bytesOnLine+1] == 0x0a)
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
					|| (aLine[whiteSpaceIndex] == '\t'))
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

TBool CTestScript::IsSection(const TDesC8& aLine) const
	{
	TBool isSection = EFalse;

	if (aLine.Size() > 2)
		{
		if (aLine[0] == '[' && (aLine[aLine.Size() - 1] == ']'))
			{
			isSection = ETrue;
			}
		}

	return isSection;
	}

TBool CTestScript::IsSectionEnd(const TDesC8& aLine) const
	{
	TBool isSectionEnd = EFalse;

	if (aLine.Length() == 1 && aLine[0] == '.')
		{
		isSectionEnd = ETrue;
		}

	return isSectionEnd;
	}


// MU 20-11-00 New command to determine if the aLine is a comment.
TBool CTestScript::IsComment(const TDesC8& aLine) const
	{
	TBool isComment = EFalse;

	if (aLine.Size() >= 2)
		{
		if (aLine[0] == '/' && aLine[1] == '/')
			{
			isComment = ETrue;
			}
		}

	return isComment;
	}


// MU 20-11-00 Changed this function to get the next "valid" command in section.
//              This now allows the user to have comments within sections.
//              Note:  the first blank line in a section denotes the end of a section.

// MMS Test Harness: This function now takes the command position as a parameter.
// This needs to be updated when a comment is read from the file, in order
// to set the next command's position correctly.
TBool CTestScript::GetNextCommandInSectionL(TTestScriptString& aCommand, TInt& aPosition, TInt& aLineNumber)
	{
	TBool commandFound;

	do 	{
			GetLineL(aCommand);
			aLineNumber++;
			// if the command is a comment, update the next command's
			// position in the file (including the cr/lf)
			if ((aCommand.Size() > 0) && (IsComment(aCommand)))
				aPosition += aCommand.Size() + 2;
		}
	while (aCommand.Size() > 0 && IsComment(aCommand));

	if(aCommand.Size() > 0 && !IsSection(aCommand) && !IsSectionEnd(aCommand))
		commandFound = ETrue;
	else
		commandFound = EFalse;
	
	return commandFound;
	}

EXPORT_C TBool CTestScript::GetLineL(TTestScriptString& aLine, TInt aPos)
	{
	TBool lineFound = EFalse;
	TInt err = iFile.Seek(ESeekStart, aPos);
	if (err == KErrNone)
		{
		lineFound = GetLineL(aLine);
		}

	return lineFound;
	}



//
//
// CTestSection
//
EXPORT_C CTestSection* CTestSection::NewL(TInt aSectionPosition, const TDesC& aSectionName, TInt aLineNumber)
	{
	CTestSection* self = new (ELeave) CTestSection(aSectionPosition, aLineNumber);
	CleanupStack::PushL(self);
	self->ConstructL();
	self->iSectionName = aSectionName;
	CleanupStack::Pop(self);
	return self;
	}


// Returns current command line number if found
EXPORT_C TInt CTestSection::GetCurrentCommand(TDes& aCurrentCommand) const
	{
	TInt currentCommand = 0;

	if (iCommandIndex < iCommandList->Count())
		{
		aCurrentCommand.Copy((*iCommandList)[iCommandIndex].iCommand);
		currentCommand = (*iCommandList)[iCommandIndex].iLineNumber;
		}
	
	return currentCommand;
	}

EXPORT_C TBool CTestSection::NextCommand()
	{
	TBool nextCommandFound = EFalse;

	if ((iCommandIndex + 1) < iCommandList->Count())
		{
		nextCommandFound = ETrue;
		}

	iCommandIndex++;

	return nextCommandFound;
	}

EXPORT_C void CTestSection::AddCommandL(const CTestScript::TTestScriptString& aCommand, TInt aCommandPosition, TInt aLineNumber)
	{
	TCommandInfo command;
	command.iCommand = aCommand;
	command.iCommandPosition = aCommandPosition;
	command.iLineNumber = aLineNumber;
	iCommandList->AppendL(command);
	}

CTestSection::CTestSection(TInt aSectionPosition, TInt aLineNumber) : iSectionPosition(aSectionPosition), iLineNumber(aLineNumber)
	{
	}


void CTestSection::ConstructL()
	{
	iCommandList = new (ELeave) CArrayFixFlat<TCommandInfo>(10);
	}

EXPORT_C CTestSection::~CTestSection()
	{
	delete iCommandList;
	}

EXPORT_C TInt CTestSection::SectionPosition() const
	{
	return iSectionPosition;
	}

EXPORT_C TInt CTestSection::CurrentCommandPosition() const
	{
	return (*iCommandList)[iCommandIndex].iCommandPosition;
	}

EXPORT_C const TDesC& CTestSection::SectionName() const
	{
	return iSectionName;
	}
