/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/


#include <e32svr.h>

#include "clog.h"

#ifdef ENABLE_LOGGING

#ifdef LOG_TO_FILE

#include <flogger.h>
/// Folder where the log resides
_LIT( KLogFolder, "ip_log" );
/// The name of the log file
_LIT( KLogFileName, "imageprintlog.txt" );
/// The format in which the time is formatted in log
_LIT( KLogTimeFormat, "%02d.%02d:%02d:%06d ");
_LIT8( KLogTimeFormat8, "%02d.%02d:%02d:%06d ");
/// The length of the string produced by KLogTimeFormat
const TInt KLogTimeFormatLength = 16;

#endif // LOG_TO_FILE

/// How many characters a log line can contain
const TInt KLogLineLength = 256;

#endif // ENABLE_LOGGING       

#ifdef ENABLE_LOGGING

void Log::DoLog( TRefByValue<const TDesC> aText, ... )
    {
    VA_LIST args;
    VA_START( args, aText );

    TBuf<KLogLineLength> buf;
    buf.FormatList( aText, args );

#ifdef LOG_TO_FILE
	RFileLogger logger;
	TInt ret = logger.Connect();
	if (ret==KErrNone)
		{
		logger.SetDateAndTime( EFalse,EFalse );
		logger.CreateLog( KLogFolder, KLogFileName, EFileLoggingModeAppend );		
		TBuf<KLogTimeFormatLength> timeStamp;
		TTime now;
		now.HomeTime();
		TDateTime dateTime;
		dateTime = now.DateTime();
		timeStamp.Format( KLogTimeFormat, 
            dateTime.Hour(), dateTime.Minute(),
            dateTime.Second(), dateTime.MicroSecond() );
		buf.Insert( 0, timeStamp );

		logger.Write(buf);
		}

	logger.Close();
#else
    RDebug::Print( buf );
#endif // LOG_TO_FILE
    VA_END( args );
    }

void Log::DoLog8( TRefByValue<const TDesC8> aText, ... )
    {
    VA_LIST args;
    VA_START( args, aText );

    TBuf8<KLogLineLength> buf;
    buf.FormatList( aText, args );
#ifdef LOG_TO_FILE
	RFileLogger logger;
	TInt ret = logger.Connect();
	if (ret==KErrNone)
		{
		logger.SetDateAndTime( EFalse,EFalse );
		logger.CreateLog( KLogFolder, KLogFileName, EFileLoggingModeAppend );
		TBuf8<KLogTimeFormatLength> timeStamp;
		TTime now;
		now.HomeTime();
		TDateTime dateTime;
		dateTime = now.DateTime();
		timeStamp.Format( KLogTimeFormat8,
            dateTime.Hour(), dateTime.Minute(),
            dateTime.Second(), dateTime.MicroSecond() );
		buf.Insert( 0, timeStamp );

		logger.Write(buf);
		}

	logger.Close();
#else
    TBuf<KLogLineLength> buf16;
    buf16.Copy(buf);
    RDebug::Print( buf16 );
#endif // LOG_TO_FILE
    VA_END( args );
    }

#endif // ENABLE_LOGGING
// End of File
