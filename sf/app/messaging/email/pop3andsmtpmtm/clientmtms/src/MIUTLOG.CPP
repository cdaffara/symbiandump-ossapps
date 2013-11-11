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

#ifdef _DEBUG
#undef _MSG_NO_LOGGING
#endif

#include "MIUTLOG.H"


CImLog::CImLog()
	{
	__DECLARE_NAME(_S("CImLog"));
	}

EXPORT_C CImLog  *CImLog::NewL(TInt aPortNum)
	{
	CImLog* self = new (ELeave) CImLog();
	CleanupStack::PushL(self);
	self->ConstructL(aPortNum);
	CleanupStack::Pop();
	return self;
	}

//
// 2nd phase of construction
//
#ifndef _MSG_NO_LOGGING
void CImLog::ConstructL(TInt aPortNum)
#else
void CImLog::ConstructL(TInt /*aPortNum*/)
#endif
	{
#ifndef _MSG_NO_LOGGING

	User::LeaveIfError(iFs.Connect());
	TFileName logFileStem; 
	logFileStem.Format(KMiutLogFile,aPortNum);

	TBool fileOpen = TRUE;
	TFileName logFile = logFileStem;
	logFile.Append(KMiutLogFileExtension);
	TUint fileCounter = 1;
	TInt err;

// Keep trying until a suitable file name is found that isn't already open.
	while (fileOpen)
		{
		err = iFs.IsFileOpen(logFile, fileOpen);
		if (err == KErrNotFound)
			{
			err = KErrNone;
			fileOpen = FALSE;
			}

		User::LeaveIfError(err);
		
		if (fileOpen)
			{
			++fileCounter;
			logFile.Format(KMiutLogUnopenedFileFormatString, &logFileStem, fileCounter);
			}
		}

// If the log directory has not been created, no log will be created. OK
		User::LeaveIfError(iFile.Replace(iFs,logFile,EFileWrite|EFileShareAny));

#endif // _MSG_NO_LOGGING
	}

//
// Alternate NewL - user describes full file path and file name for the log file
//
EXPORT_C CImLog  *CImLog::NewL(const TDesC& aFullFilePath, TImLogOpenMode aImLogOpenMode)
	{
	CImLog* self = new (ELeave) CImLog();
	CleanupStack::PushL(self);
	self->ConstructL(aFullFilePath,aImLogOpenMode);
	CleanupStack::Pop();
	return self;
	}

//
// 2nd phase of construction
//
#ifndef _MSG_NO_LOGGING
void CImLog::ConstructL(const TDesC& aFullFilePath, TImLogOpenMode aImLogOpenMode)
#else
void CImLog::ConstructL(const TDesC& /*aFullFilePath*/, TImLogOpenMode /*aImLogOpenMode*/)
#endif
	{
#ifndef _MSG_NO_LOGGING

	User::LeaveIfError(iFs.Connect());
	TFileName logFile; 
	logFile.Append(aFullFilePath);

	if (aImLogOpenMode==EAppend)
		{
		TInt err=iFile.Open(iFs,logFile,EFileWrite|EFileShareAny);
		if (err==KErrNotFound)
			{
			User::LeaveIfError(iFile.Create(iFs,logFile,EFileWrite|EFileShareAny));
			}
		else if (err != KErrNone)
			{
			User::Leave(err);
			}			
		TInt position=1;
		User::LeaveIfError(iFile.Seek(ESeekEnd,position));
		}
	else
		{
		// If the log directory has not been created, no log will be created. OK
		User::LeaveIfError(iFile.Replace(iFs,logFile,EFileWrite|EFileShareAny));
		}

#endif // _MSG_NO_LOGGING
	}

EXPORT_C CImLog::~CImLog()
	{
#ifndef _MSG_NO_LOGGING

	iFile.Close();
	iFs.Close();

#endif // _MSG_NO_LOGGING
	}

//
// Log message sent to the socket
//
#ifndef _MSG_NO_LOGGING
EXPORT_C void CImLog::AppendOut(const TDesC8& outPut)
#else
EXPORT_C void CImLog::AppendOut(const TDesC8& /*outPut*/)
#endif
	{
#ifndef _MSG_NO_LOGGING
	BuildLogString(KMiutLogOutputString, outPut, ETrue);
#endif
	}

//
// Log message received from socket
//
#ifndef _MSG_NO_LOGGING
EXPORT_C void CImLog::AppendResponse(const TDesC8& theResponse)
#else
EXPORT_C void CImLog::AppendResponse(const TDesC8& /*theResponse*/)
#endif
	{
#ifndef _MSG_NO_LOGGING
	BuildLogString(KMiutLogResponseString, theResponse, ETrue);
#endif
	}

//
// Log an error message
//
#ifndef _MSG_NO_LOGGING
EXPORT_C void CImLog::AppendError(const TDesC8& theError, TInt anErrorCode)
#else
EXPORT_C void CImLog::AppendError(const TDesC8& /*theError*/, TInt /*anErrorCode*/)
#endif
	{
#ifndef _MSG_NO_LOGGING
	WriteToLog(KMiutLogErrorString, &theError, anErrorCode);
#endif
	}

//
// Log a comment
//
#ifndef _MSG_NO_LOGGING
EXPORT_C void CImLog::AppendComment(const TDesC8& theError)
#else
EXPORT_C void CImLog::AppendComment(const TDesC8& /*theError*/)
#endif
	{
#ifndef _MSG_NO_LOGGING
	WriteToLog(KMiutLogCommentString, &theError);
#endif
	}

//
// Construct a string with date, a message type indicator (out, in, error) sort out formatting
//
#ifndef _MSG_NO_LOGGING
void CImLog::BuildLogString(const TDesC8& aFormat,const TDesC8& theOutPut, TBool aWriteCrLf)
#else
void CImLog::BuildLogString(const TDesC8& /*aFormat*/,const TDesC8& /*theOutPut*/, TBool /*aWriteCrLf*/)
#endif
	{
#ifndef _MSG_NO_LOGGING

	TPtrC8 logString(theOutPut);

	for(;;)
		{
		TInt crLfPos=logString.Find(KMiutLogCR);
		if(crLfPos==KErrNotFound)
			break;
		else
			{
			TPtrC8 line;
			if (aWriteCrLf && (crLfPos == logString.Size() - 2))
				{
				line.Set(logString);
				}
			else
				{
				line.Set(logString.Left(crLfPos));
				}
			WriteToLog(TRefByValue<const TDesC8>(aFormat),&line);
			logString.Set(logString.Mid(crLfPos+2));
			}
		}

	if(logString.Length())
		WriteToLog(TRefByValue<const TDesC8>(aFormat),&logString);

#endif // _MSG_NO_LOGGING
	}

//
// Write information to the log file
//
#ifndef _MSG_NO_LOGGING
void CImLog::WriteToLog(TRefByValue<const TDesC8> aFmt,...)
#else
void CImLog::WriteToLog(TRefByValue<const TDesC8> /*aFmt*/,...)
#endif
	{
#ifndef _MSG_NO_LOGGING

	TBuf8<2560> buf;
	TTime date;
	date.UniversalTime();
	TBuf<18> dateString;
	TRAP_IGNORE(date.FormatL(dateString,(KMiutLogDateFormatString)));
	buf.Copy(dateString);
	
	VA_LIST list;
	VA_START(list,aFmt);

	buf.AppendFormatList(aFmt,list);
	buf.Append(KMiutLogCR);
	iFile.Write(buf);

#endif // _MSG_NO_LOGGING
	}
