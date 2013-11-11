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

#ifndef __TESTFRAMEPARSE_H
#define __TESTFRAMEPARSE_H

#include "testframeutils.h"

#include <badesca.h>

//
//
// CBaseCommandParser
//

class MBaseTestState;

class CBaseCommandParser : public CBase
	{
public:
	IMPORT_C virtual ~CBaseCommandParser();

	IMPORT_C void ParseL(const TDesC& aCommand, TTestDebugInfo aDebugInfo);
	IMPORT_C TBool CanParse(const TDesC& aCommand) const;
	IMPORT_C TDesC& Error();

protected:
	IMPORT_C virtual void ProcessL() = 0;
	IMPORT_C virtual void ConstructL();
	IMPORT_C CBaseCommandParser();
	IMPORT_C void AddCommandL(const TDesC& aCommand);
	IMPORT_C void CheckNumberOfArgumentsL(TInt aNumberOfArguments);
	IMPORT_C void SetErrorL(const TDesC& aError);

protected:
	HBufC* iCurrentCommand;
	CDesCArrayFlat* iArgumentList;
	TTestDebugInfo iDebugInfo;
	HBufC* iLastError;

private:
	CDesCArrayFlat* iSupportedCommands;
	};



//
//
// CTestBaseSectionParser
//

class CBaseSectionParser : public CBase
	{
public:
	IMPORT_C void ParseL();
	IMPORT_C virtual ~CBaseSectionParser();

protected:
	IMPORT_C virtual void ConstructL(const TDesC& aSectionName);
	IMPORT_C virtual void LogCommentL(const TDesC& aComment) = 0;
	IMPORT_C CBaseSectionParser(CTestScript& aScript);
	IMPORT_C void AddCommandParserL(CBaseCommandParser* aParser);

protected:
	CTestScript& iScript;
	CTestSection* iSection;
	CArrayPtrFlat<CBaseCommandParser>* iCommandParsers;

private:
	void SetSectionL(const TDesC& aSectionName);
	};



#endif
