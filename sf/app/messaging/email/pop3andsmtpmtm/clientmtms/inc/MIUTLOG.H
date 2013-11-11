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
//

#if !defined(__MIUTLOG_H__)
#define __MIUTLOG_H__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#if !defined(__F32FILE_H__)
#include <f32file.h>
#endif

_LIT(KMiutLogFile, "c:\\logs\\Email\\ImLog%d");
_LIT(KMiutLogFileExtension, ".txt");
_LIT(KMiutLogUnopenedFileFormatString, "%S(%u).txt");
_LIT8(KMiutLogOutputString, ">> %S");
_LIT8(KMiutLogResponseString, "<< %S");
_LIT8(KMiutLogErrorString, "** %S %d");
_LIT8(KMiutLogCommentString, "** %S");
_LIT8(KMiutLogCR, "\r\n");
_LIT(KMiutLogDateFormatString, "%D%M%*Y%1%/1%2%/2%3 %H%:1%T%:2%S ");


// Setting to select whether a new log file will be created, 
// or existing one will be appended to...

/**
@internalTechnology
@released
*/
enum TImLogOpenMode
	{ 
	EReplace, 
	EAppend
	};

class CImLog : public CBase
/**
@internalTechnology
@released
*/
	{
public:
	IMPORT_C static CImLog* NewL(const TDesC& aFullFilePath, TImLogOpenMode aImLogOpenMode);
	IMPORT_C static CImLog* NewL(TInt aPortNum);
	IMPORT_C ~CImLog();
	
	IMPORT_C void AppendOut(const TDesC8 &outPut);
	IMPORT_C void AppendResponse(const TDesC8 &theResponse);
	IMPORT_C void AppendComment(const TDesC8 &theError);
	IMPORT_C void AppendError(const TDesC8 &theError, TInt anErrorCode);
private:
	CImLog();
	void ConstructL(TInt aPortNum);
	void ConstructL(const TDesC& aFullFilePath, TImLogOpenMode aImLogOpenMode);

	void BuildLogString(const TDesC8& aFormat,const TDesC8 &theOutPut, TBool aWriteCrLf = EFalse);
	void WriteToLog(TRefByValue<const TDesC8> aFmt,...);
private:
	RFile	iFile;
	RFs		iFs;
	};

#endif
