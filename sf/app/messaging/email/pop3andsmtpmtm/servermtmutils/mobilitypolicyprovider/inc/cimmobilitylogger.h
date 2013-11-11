// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CSMTMLOGGER_H__
#define __CSMTMLOGGER_H__

#include <e32base.h>
#include <f32file.h>

const TInt KDefaultLog = 0;

#ifdef _DEBUG
#define __IM_MOBILITY_LOGGING
#endif //_DEBUG

#ifdef __IM_MOBILITY_LOGGING

#define __LOG_CREATE_DEFAULT(a) CImMobilityPolicyProvider::GetRef().Logger().CreateDefaultLog(a)
#define __LOG_CREATE_STANDARD(a,b) CImMobilityPolicyProvider::GetRef().Logger().CreateStandardLog(a,b)
#define __LOG_CLOSE(a) CImMobilityPolicyProvider::GetRef().Logger().CloseLog(a)
#define __LOG_TEXT(a,b) CImMobilityPolicyProvider::GetRef().Logger().LogText(a,b)
#define __LOG_DATA_IN(a,b) CImMobilityPolicyProvider::GetRef().Logger().LogDataIn(a,b)
#define __LOG_DATA_OUT(a,b) CImMobilityPolicyProvider::GetRef().Logger().LogDataOut(a,b)
#define __LOG_FORMAT(a) CImMobilityPolicyProvider::GetRef().Logger().LogFormat a
#define __LOG_ON(a,b) CImMobilityPolicyProvider::GetRef().Logger().LogOn(a,b)

#else //__IM_MOBILITY_LOGGING

#define __LOG_CREATE_DEFAULT(a)
#define __LOG_CREATE_STANDARD(a,b)
#define __LOG_CLOSE(a)
#define __LOG_TEXT(a,b)
#define __LOG_DATA_IN(a,b)
#define __LOG_DATA_OUT(a,b)
#define __LOG_FORMAT(a)
#define __LOG_ON(a,b)

#endif //__IM_MOBILITY_LOGGING

/**
@internalTechnology
@prototype
*/
class CImMobilityLogger : public CBase, TDes8Overflow
	{
public:
	static CImMobilityLogger* NewL();
	~CImMobilityLogger();

	IMPORT_C void CreateDefaultLog(const TDesC& aFilePartName);
	IMPORT_C void CreateStandardLog(const TDesC& aFilePartName, TInt& aLogId);
	IMPORT_C void CloseLog(TInt aFilePos);
	IMPORT_C void LogText(TInt aFilePos, char const* aText);
	IMPORT_C void LogText(TInt aFilePos, const TDesC8& aText);
	IMPORT_C void LogDataIn(TInt aFilePos, const TDesC8& aText);
	IMPORT_C void LogDataOut(TInt aFilePos, const TDesC8& aText);
	IMPORT_C void LogFormat(TInt aFilePos, char const* aFmt, ...);
	IMPORT_C void LogFormat(TInt aFilePos, TRefByValue<const TDesC8> aFmt, ...);
	IMPORT_C void LogOn(TInt aFilePos, TBool aLoggingOn);

private:
	CImMobilityLogger();
	void ConstructL();

	void DoCreateDefaultLogL(const TDesC& aFilePartName);
	void DoCreateStandardLogL(const TDesC& aFilePartName, TInt& aLogId);
	void LogData(TInt aFilePos, const TDesC8& aText, const TDesC8& aFmt);
	void LogFormat(TInt aFilePos, VA_LIST& aList, TRefByValue<const TDesC8> aFmt);

	// from TDes8Overflow
	void Overflow(TDes8& aDes);

private:
	RFs iFs;
	struct TLogFileInfo
		{
		RFile iFile;
		TBool iLoggingOn;
		};
	RArray<TLogFileInfo> iFileList;

	RFile* iFile;

	HBufC8* iLogBuffer;
	
	TBool iDefaultLogFileInUse;
	};

#endif // __CSMTMLOGGER_H__
