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

#include "testframeparse.h"


_LIT(KTestErrorUnknown, "No more details available");
_LIT(KInvalidNumberOfArguments, "[%4d] Invalid number of arguments, actual: %d, expected: %d");
_LIT(KGeneralParseError, "[%4d] Error in command: %S, section: %S");
_LIT(KNotParsedError, "[%4d] Command not recognised: %S, in section: %S");

//
//
// CTestBaseCommandParser
//

EXPORT_C CBaseCommandParser::~CBaseCommandParser()
	{
	delete iArgumentList;
	delete iSupportedCommands;
	delete iCurrentCommand;
	delete iLastError;
	}

EXPORT_C void CBaseCommandParser::ConstructL()
	{
	iArgumentList = new (ELeave) CDesCArrayFlat(3);
	iSupportedCommands = new (ELeave) CDesCArrayFlat(1);
	iLastError = HBufC::NewL(0);
	iCurrentCommand = 0;
	}

// TODO: this function should really use TLex!
EXPORT_C void CBaseCommandParser::ParseL(const TDesC& aCommand, TTestDebugInfo aDebugInfo)
// Extract the command and any argments from the given string
	{
	iDebugInfo = aDebugInfo;
	iArgumentList->Reset();

	delete iCurrentCommand;
	iCurrentCommand = 0;
	TInt commandIter = 0;
	TInt commandStart = 0;
	TBool inQuotes = EFalse;
	while (commandIter <= aCommand.Length())
		{
		TBool commandFound = EFalse;

		if (commandIter < aCommand.Length())
			{
			if (aCommand[commandIter] == '"')
				{
				// We assume that quotes only occur at the beginning and end
				// of arguments. Hence, if we've just found a new quote, we should
				// not make it part of the argument.
				if (!inQuotes)
					{
					inQuotes = ETrue;
					commandStart++;
					}
				else
					{
					// Assume we've got to the end of the argument and skip the
					// following space.
					commandFound = ETrue;
					commandIter++;
					}
				}
			else if ((aCommand[commandIter] == ' ') && (!inQuotes))
				{
				commandFound = ETrue;
				}
			}
		else
			{
			commandFound = ETrue;
			}
		
		if (commandFound)
			{
			if (commandIter > commandStart)
			// Command or argument found
				{
				TInt commandLength = commandIter - commandStart;

				// If the argument ended in quotes, modify the length to exclude them.
				if (inQuotes)
					{
					inQuotes = EFalse;
					commandLength--;
					}
				
				if (iCurrentCommand == NULL)
				// This is the main command
					{
					iCurrentCommand = HBufC::NewL(commandLength);
					(*iCurrentCommand) = aCommand.Mid(commandStart, commandLength);
					}
				else
				// This is a command argument
					{
					iArgumentList->AppendL(aCommand.Mid(commandStart, commandLength));
					}

				// Set the start of the next argument.
				commandStart = commandIter + 1;
				}
			}

		commandIter++;
		}

	ProcessL();
	}

EXPORT_C TBool CBaseCommandParser::CanParse(const TDesC& aCommand) const
	{
	TBool canParse = EFalse;

	TInt commandCounter = iSupportedCommands->Count();
	while ((commandCounter-- > 0) && (!canParse))
	// Scan through each of the supported commands
		{
		TInt commandSize = (*iSupportedCommands)[commandCounter].Length();
		if (commandSize <= aCommand.Length())
		// The given command must be at least as long as the one it is being compared to
			{
			if (aCommand.Left(commandSize) == (*iSupportedCommands)[commandCounter])
			// If the first character match
				{
				// Check that there are no more characters or that the next one is a whitespace
				if (commandSize < aCommand.Length())
					{
					if (aCommand[commandSize] == ' ')
						{
						canParse = ETrue;
						}
					}
				else
					{
					canParse = ETrue;
					}
				}
			}
		}

	return canParse;
	}

EXPORT_C CBaseCommandParser::CBaseCommandParser()
	{
	}

EXPORT_C void CBaseCommandParser::AddCommandL(const TDesC& aCommand)
	{
	iSupportedCommands->AppendL(aCommand);
	}


EXPORT_C TDesC& CBaseCommandParser::Error()
	{
	if (iLastError->Length() == 0)
		{
		delete iLastError;
		iLastError = 0;
		iLastError = HBufC::NewL(KTestErrorUnknown().Length());
		(*iLastError) = KTestErrorUnknown;
		}

	return *iLastError;
	}

EXPORT_C void CBaseCommandParser::CheckNumberOfArgumentsL(TInt aNumberOfArguments)
	{
	if (iArgumentList->Count() != aNumberOfArguments)
		{
		TBuf<128> errorString;
		errorString.Format(KInvalidNumberOfArguments, iDebugInfo.LineNumber(), iArgumentList->Count(), aNumberOfArguments);
		SetErrorL(errorString);
		User::Leave(KErrArgument);
		}
	}

EXPORT_C void CBaseCommandParser::SetErrorL(const TDesC& aError)
	{
	delete iLastError;
	iLastError = 0;
	iLastError = HBufC::NewL(aError.Length());
	(*iLastError) = aError;
	}


//
//
// CBaseSectionParser
//

void CBaseSectionParser::SetSectionL(const TDesC& aSectionName)
	{
	delete iSection;
	iSection = 0;
	iSection = iScript.GetSectionL(aSectionName);
	}

EXPORT_C void CBaseSectionParser::ParseL()
	{
	// Go through each command in the section and parse it
	TBool parsed = EFalse;
	TBuf<512> currentCommand;
	TBool commandNotRecognised = EFalse;
	TInt lineNumber = 0;

	while (((lineNumber = iSection->GetCurrentCommand(currentCommand)) > 0) && (!commandNotRecognised))
		{
		TInt commandParserIndex = iCommandParsers->Count();
		parsed = EFalse;
		while (commandParserIndex--)
			// Look for a command parser that can parse the current command
			{
			if ((*iCommandParsers)[commandParserIndex]->CanParse(currentCommand))
				{
				TTestDebugInfo debugInfo(iScript, iSection->SectionPosition(), iSection->CurrentCommandPosition(), lineNumber);
				TRAPD(err, (*iCommandParsers)[commandParserIndex]->ParseL(currentCommand, debugInfo));
				if (err != KErrNone)
					{
					TBuf<128> errorString;
					errorString.Format(KGeneralParseError, lineNumber, &currentCommand, &(iSection->SectionName()));
					LogCommentL(errorString);
					LogCommentL((*iCommandParsers)[commandParserIndex]->Error());
					User::Leave(err);
					}
				parsed = ETrue;
				}
			}

		if (!parsed)
			{
			commandNotRecognised = ETrue;
			TBuf<256> aError;
			aError.Format(KNotParsedError, lineNumber, &currentCommand, &(iSection->SectionName()));
			LogCommentL(aError);
			User::Leave(KErrNotFound);
			}

		iSection->NextCommand();
		}
	}

EXPORT_C CBaseSectionParser::~CBaseSectionParser()
	{
	iCommandParsers->ResetAndDestroy();
	delete iCommandParsers;
	delete iSection;
	}

EXPORT_C CBaseSectionParser::CBaseSectionParser(CTestScript& aScript) : iScript(aScript)
	{
	}

EXPORT_C void CBaseSectionParser::AddCommandParserL(CBaseCommandParser* aParser)
	{
	TRAPD(err, iCommandParsers->AppendL(aParser));
	// If the parser can't be appended then delete it.
	// Then leave with the original error.
	if (err != KErrNone)
		{
		delete aParser;
		User::Leave(err);
		}
	}

EXPORT_C void CBaseSectionParser::ConstructL(const TDesC& aSectionName)
	{
	iCommandParsers = new (ELeave) CArrayPtrFlat<CBaseCommandParser>(5);
	SetSectionL(aSectionName);
	}

