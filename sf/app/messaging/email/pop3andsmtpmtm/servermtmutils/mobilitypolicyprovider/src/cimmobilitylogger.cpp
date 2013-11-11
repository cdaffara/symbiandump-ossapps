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

#include "cimmobilitylogger.h"
#include <utf.h>
#include <bautils.h>

#ifdef __IM_MOBILITY_LOGGING

_LIT(KLogDir, "c:\\logs\\email");
_LIT(KBackupLogDir, "c:\\logs\\email\\backup");

_LIT(KDefaultLogFileFormat, "%S\\%S.txt");
_LIT(KStandardLogFileFormat, "%S\\%S_%d.txt");
_LIT(KBackupLogFileFormat, "%S\\%S_%d_%d.txt");

_LIT8(KTxtCRLF, "\r\n");
_LIT8(KTxtOverflowCRLF, "... Overflow... \r\n");
_LIT8(KTxtFormatText, "%S");
_LIT8(KTxtFormatDataIn, "<<(%3d)<< %S");
_LIT8(KTxtFormatDataOut, ">>(%3d)>> %S");
_LIT(KTxtLogDateFormat, "%D%M%*Y%1%/1%2%/2%3 %H%:1%T%:2%S ");

_LIT8(KTxtFormatStandardLogStarted, "Started Log: %S");

const TInt KDateFormatLength = 18;
const TInt KLogBufferSize = 2560;
const TInt KMaxBackupOffset = 6;

#endif //__IM_MOBILITY_LOGGING

CImMobilityLogger* CImMobilityLogger::NewL()
	{
	CImMobilityLogger* self = new (ELeave) CImMobilityLogger();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CImMobilityLogger::CImMobilityLogger()
	{
	}

void CImMobilityLogger::ConstructL()
	{
#ifdef __IM_MOBILITY_LOGGING
	User::LeaveIfError(iFs.Connect());

	iLogBuffer = HBufC8::NewL(KLogBufferSize);

	// Add an entry for the default log file
	TLogFileInfo info;
	info.iLoggingOn = ETrue;
	iFileList.AppendL(info);
#endif
	}

CImMobilityLogger::~CImMobilityLogger()
	{
#ifdef __IM_MOBILITY_LOGGING
	delete iLogBuffer;

	for (TInt pos = 0; pos < iFileList.Count(); ++pos)
		{
		iFileList[pos].iFile.Close();
		}

	iFileList.Reset();
	iFs.Close();
#endif
	}

#ifdef __IM_MOBILITY_LOGGING
EXPORT_C void CImMobilityLogger::CreateDefaultLog(const TDesC& aFilePartName)
#else //__IM_MOBILITY_LOGGING
EXPORT_C void CImMobilityLogger::CreateDefaultLog(const TDesC& /*aFilePartName*/)
#endif //__IM_MOBILITY_LOGGING
	{
#ifdef __IM_MOBILITY_LOGGING
	TRAP_IGNORE(DoCreateDefaultLogL(aFilePartName));
#endif
	}

#ifdef __IM_MOBILITY_LOGGING
void CImMobilityLogger::DoCreateDefaultLogL(const TDesC& aFilePartName)
#else //__IM_MOBILITY_LOGGING
void CImMobilityLogger::DoCreateDefaultLogL(const TDesC& /*aFilePartName*/)
#endif //__IM_MOBILITY_LOGGING
	{
#ifdef __IM_MOBILITY_LOGGING
	if (iDefaultLogFileInUse)
		{
		return;
		}

	TFileName filename;
	filename.Format(KDefaultLogFileFormat, &KLogDir(), &aFilePartName);

	TInt err = iFileList[KDefaultLog].iFile.Replace(iFs, filename, EFileWrite | EFileShareAny);

	if (err == KErrNone)
		{
		iDefaultLogFileInUse = ETrue;
		}
#endif //__IM_MOBILITY_LOGGING
	}

#ifdef __IM_MOBILITY_LOGGING
EXPORT_C void CImMobilityLogger::CreateStandardLog(const TDesC& aFilePartName, TInt& aLogId)
#else //__IM_MOBILITY_LOGGING
EXPORT_C void CImMobilityLogger::CreateStandardLog(const TDesC& /*aFilePartName*/, TInt& /*aLogId*/)
#endif //__IM_MOBILITY_LOGGING
	{
#ifdef __IM_MOBILITY_LOGGING
	TRAP_IGNORE(DoCreateStandardLogL(aFilePartName, aLogId));
#endif //__IM_MOBILITY_LOGGING
	}

#ifdef __IM_MOBILITY_LOGGING
void CImMobilityLogger::DoCreateStandardLogL(const TDesC& aFilePartName, TInt& aLogId)
#else //__IM_MOBILITY_LOGGING
void CImMobilityLogger::DoCreateStandardLogL(const TDesC& /*aFilePartName*/, TInt& /*aLogId*/)
#endif //__IM_MOBILITY_LOGGING
	{
#ifdef __IM_MOBILITY_LOGGING
	aLogId = KDefaultLog;

	TBool fileOpen(ETrue);
	TFileName filename;
	TInt err;
	TInt fileOffset(0);

	do
		{
		++fileOffset;
		filename.Format(KStandardLogFileFormat, &KLogDir(), &aFilePartName, fileOffset);

		err = iFs.IsFileOpen(filename, fileOpen);
		if (err == KErrNotFound)
			{
			err = KErrNone;
			fileOpen = EFalse;
			}

		User::LeaveIfError(err);
		}
	while (fileOpen);

	// See if we should take a backup of the log file
	if (BaflUtils::FolderExists(iFs, KBackupLogDir))
		{
		TInt backupOffset(KMaxBackupOffset);
		TFileName source;
		TFileName dest;

		dest.Format(KBackupLogFileFormat, &KBackupLogDir(), &aFilePartName, fileOffset, backupOffset);

		while (backupOffset > 1)
			{
			--backupOffset;
			source.Format(KBackupLogFileFormat, &KBackupLogDir(), &aFilePartName, fileOffset, backupOffset);
			BaflUtils::CopyFile(iFs, source, dest);
			dest = source;
			}

		BaflUtils::CopyFile(iFs, filename, dest);
		}

	TLogFileInfo info;
	info.iLoggingOn = ETrue;
	err = info.iFile.Replace(iFs, filename, EFileWrite | EFileShareAny);

	if (err != KErrNone)
		{
		return;
		}

	CleanupClosePushL(info.iFile);
	iFileList.AppendL(info);
	CleanupStack::Pop(&info.iFile);

	HBufC8* buf;
	TRAP(err, buf = CnvUtfConverter::ConvertFromUnicodeToUtf8L(filename));
	if (err == KErrNone)
		{
		const TDesC8& bufRef = *buf;
		LogFormat(KDefaultLog, KTxtFormatStandardLogStarted, &bufRef);
		delete buf;
		}

	aLogId = iFileList.Count() - 1;
#endif //__IM_MOBILITY_LOGGING
	}

#ifdef __IM_MOBILITY_LOGGING
EXPORT_C void CImMobilityLogger::CloseLog(TInt aFilePos)
#else //__IM_MOBILITY_LOGGING
EXPORT_C void CImMobilityLogger::CloseLog(TInt /*aFilePos*/)
#endif //__IM_MOBILITY_LOGGING
	{
#ifdef __IM_MOBILITY_LOGGING
	if (aFilePos < iFileList.Count())
		{
		iFileList[aFilePos].iFile.Close();
		
		if (aFilePos == KDefaultLog)
			{
			iDefaultLogFileInUse = EFalse;
			}
		}
#endif //__IM_MOBILITY_LOGGING
	}

#ifdef __IM_MOBILITY_LOGGING
EXPORT_C void CImMobilityLogger::LogText(TInt aFilePos, char const* aText)
#else //__IM_MOBILITY_LOGGING
EXPORT_C void CImMobilityLogger::LogText(TInt /*aFilePos*/, char const* /*aText*/)
#endif //__IM_MOBILITY_LOGGING
	{
#ifdef __IM_MOBILITY_LOGGING

	TPtrC8 buf(reinterpret_cast<TUint8*>(const_cast<char*>(aText)));
	LogFormat(aFilePos, KTxtFormatText, &buf);

#endif //__IM_MOBILITY_LOGGING
	}

#ifdef __IM_MOBILITY_LOGGING
EXPORT_C void CImMobilityLogger::LogText(TInt aFilePos, const TDesC8& aText)
#else //__IM_MOBILITY_LOGGING
EXPORT_C void CImMobilityLogger::LogText(TInt /*aFilePos*/, const TDesC8& /*aText*/)
#endif //__IM_MOBILITY_LOGGING
	{
#ifdef __IM_MOBILITY_LOGGING
	LogFormat(aFilePos, KTxtFormatText, &aText);
#endif //__IM_MOBILITY_LOGGING
	}

#ifdef __IM_MOBILITY_LOGGING
EXPORT_C void CImMobilityLogger::LogDataIn(TInt aFilePos, const TDesC8& aText)
#else //__IM_MOBILITY_LOGGING
EXPORT_C void CImMobilityLogger::LogDataIn(TInt /*aFilePos*/, const TDesC8& /*aText*/)
#endif //__IM_MOBILITY_LOGGING
	{
#ifdef __IM_MOBILITY_LOGGING
	LogData(aFilePos, aText, KTxtFormatDataIn);
#endif //__IM_MOBILITY_LOGGING
	}

#ifdef __IM_MOBILITY_LOGGING
EXPORT_C void CImMobilityLogger::LogDataOut(TInt aFilePos, const TDesC8& aText)
#else //__IM_MOBILITY_LOGGING
EXPORT_C void CImMobilityLogger::LogDataOut(TInt /*aFilePos*/, const TDesC8& /*aText*/)
#endif //__IM_MOBILITY_LOGGING
	{
#ifdef __IM_MOBILITY_LOGGING
	LogData(aFilePos, aText, KTxtFormatDataOut);
#endif //__IM_MOBILITY_LOGGING
	}

#ifdef __IM_MOBILITY_LOGGING
void CImMobilityLogger::LogData(TInt aFilePos, const TDesC8& aText, const TDesC8& aFmt)
#else //__IM_MOBILITY_LOGGING
void CImMobilityLogger::LogData(TInt /*aFilePos*/, const TDesC8& /*aText*/, const TDesC8& /*aFmt*/)
#endif //__IM_MOBILITY_LOGGING
	{
#ifdef __IM_MOBILITY_LOGGING

	TPtrC8 text(aText);
	TPtrC8 line;
	TInt crlfPos(0);

	for(;;)
		{
		crlfPos = text.Find(KTxtCRLF);

		if (crlfPos == KErrNotFound)
			{
			if (text.Length() > 0)
				{
				LogFormat(aFilePos, aFmt, text.Length(), &text);
				}

			break;
			}

		line.Set(text.Left(crlfPos));
		LogFormat(aFilePos, aFmt, crlfPos + 2, &line);

		if (text.Length() > crlfPos + 2)
			{
			text.Set(text.Mid(crlfPos + 2));
			}
		else
			{
			break;
			}
		}
#endif //__IM_MOBILITY_LOGGING
	}

#ifdef __IM_MOBILITY_LOGGING
EXPORT_C void CImMobilityLogger::LogFormat(TInt aFilePos, char const* aFmt, ...)
#else //__IM_MOBILITY_LOGGING
EXPORT_C void CImMobilityLogger::LogFormat(TInt /*aFilePos*/, char const* /*aFmt*/, ...)
#endif //__IM_MOBILITY_LOGGING
	{
#ifdef __IM_MOBILITY_LOGGING
	VA_LIST list;
	VA_START(list, aFmt);

	TPtrC8 fmt(reinterpret_cast<TUint8*>(const_cast<char*>(aFmt)));

	LogFormat(aFilePos, list, fmt);
#endif //__IM_MOBILITY_LOGGING	
	}

#ifdef __IM_MOBILITY_LOGGING
EXPORT_C void CImMobilityLogger::LogFormat(TInt aFilePos, TRefByValue<const TDesC8> aFmt, ...)
#else //__IM_MOBILITY_LOGGING
EXPORT_C void CImMobilityLogger::LogFormat(TInt /*aFilePos*/, TRefByValue<const TDesC8> /*aFmt*/, ...)
#endif //__IM_MOBILITY_LOGGING
	{
#ifdef __IM_MOBILITY_LOGGING
	VA_LIST list;
	VA_START(list, aFmt);

	LogFormat(aFilePos, list, aFmt);
#endif //__IM_MOBILITY_LOGGING
	}

#ifdef __IM_MOBILITY_LOGGING
EXPORT_C void CImMobilityLogger::LogOn(TInt aFilePos, TBool aLoggingOn)
#else //__IM_MOBILITY_LOGGING
EXPORT_C void CImMobilityLogger::LogOn(TInt /*aFilePos*/, TBool /*aLoggingOn*/)
#endif //__IM_MOBILITY_LOGGING
	{
#ifdef __IM_MOBILITY_LOGGING
	if (iFileList.Count() > aFilePos)
		{
		if (!aLoggingOn)
			{
			LogText(aFilePos, "Logging Off");
			}
		iFileList[aFilePos].iLoggingOn = aLoggingOn;
		if (aLoggingOn)
			{
			LogText(aFilePos, "Logging On");
			}
		}
#endif //__IM_MOBILITY_LOGGING
	}

#ifdef __IM_MOBILITY_LOGGING
void CImMobilityLogger::LogFormat(TInt aFilePos, VA_LIST& aList, TRefByValue<const TDesC8> aFmt)
#else //__IM_MOBILITY_LOGGING
void CImMobilityLogger::LogFormat(TInt /*aFilePos*/, VA_LIST& /*aList*/, TRefByValue<const TDesC8> /*aFmt*/)
#endif //__IM_MOBILITY_LOGGING
	{
#ifdef __IM_MOBILITY_LOGGING

	if (iFileList.Count() == 0)
		{
		return;
		}

	if (aFilePos >= iFileList.Count())
		{
		aFilePos = KDefaultLog;
		}

	if ((aFilePos == KDefaultLog) && (iDefaultLogFileInUse == EFalse))
		{
		return;
		}

	if (!iFileList[aFilePos].iLoggingOn)
		{
		return;
		}

	iFile = &iFileList[aFilePos].iFile;

	TPtr8 buf = iLogBuffer->Des();
	buf.Zero();

	TTime date;
	date.UniversalTime();
	TBuf<KDateFormatLength> dateString;
	TRAP_IGNORE(date.FormatL(dateString, KTxtLogDateFormat));
	buf.Copy(dateString);

	buf.AppendFormatList(aFmt, aList, this);
	iFile->Write(buf);
	iFile->Write(KTxtCRLF());

#endif //__IM_MOBILITY_LOGGING
	}

#ifdef __IM_MOBILITY_LOGGING
void CImMobilityLogger::Overflow(TDes8& aDes)
#else //__IM_MOBILITY_LOGGING
void CImMobilityLogger::Overflow(TDes8& /*aDes*/)
#endif //__IM_MOBILITY_LOGGING
	{
#ifdef __IM_MOBILITY_LOGGING
	iFile->Write(aDes);
	iFile->Write(KTxtOverflowCRLF());
#endif //__IM_MOBILITY_LOGGING
	}
