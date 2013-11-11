/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  A plug-in for silencing the rigning tones.
*
*/


#ifndef LOGGER_H
#define LOGGER_H

#include <e32debug.h>  // RDebug::Print

/**
* Logging macros,
* see silenceactionpluginconst.hrh.
*/

#ifdef LOGGING_ENABLED

#ifndef LOGGING_MODE_FILE

#define LOG_1( a ) RDebug::Print( a )
#define LOG_2( a, b ) RDebug::Print( a, b )
#define LOG_3( a, b, c ) RDebug::Print( a, b, c )
#define LOG_RAW( a ) RDebug::RawPrint( a )
#define ENABLE_LOG

#else

#include <flogger.h>
#include <f32file.h>

_LIT( KFullPath, "c:\\logs\\silenceplugin\\" );

LOCAL_C void EnableLogging()
    {
    RFs fs;
    TInt err = fs.Connect();
    if( err == KErrNone )
        {
        fs.MkDirAll( KFullPath );
        }
    fs.Close();
    }

_LIT( KDir, "silenceplugin" );
_LIT( KFile, "silence.txt" );

#define LOG_1( a ) RFileLogger::Write( KDir, KFile, EFileLoggingModeAppend, a )
#define LOG_2( a, b ) RFileLogger::WriteFormat( KDir, KFile, EFileLoggingModeAppend, a, b )
#define LOG_3( a, b, c ) RFileLogger::WriteFormat( KDir, KFile, EFileLoggingModeAppend, a, b, c )
#define LOG_RAW( a ) RFileLogger::Write( KDir, KFile, EFileLoggingModeAppend, a )
#define ENABLE_LOG EnableLogging()

#endif // LOGGING_MODE_FILE

#else

#define LOG_1( a )
#define LOG_2( a, b )
#define LOG_3( a, b, c )
#define LOG_RAW( a )
#define ENABLE_LOG

#endif // LOGGING_ENABLED

#endif // LOGGER_H
