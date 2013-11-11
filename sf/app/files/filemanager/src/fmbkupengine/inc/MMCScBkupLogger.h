/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*       Macro definition file for logging.
*
*
*/

#ifndef __MMCSCBKUPLOGGER_H__
#define __MMCSCBKUPLOGGER_H__

// System includes
#include <e32std.h>

// User includes
#include "MMCScBkupConfig.h"
#include "TMMCScBkupOwnerDataType.h"

class MMCScBkupLogger
    {
    public:
        static void LogRaw( const TDesC& aData );
        static void Log( TRefByValue<const TDesC> aFmt, ... );
        static void LogFile( TRefByValue<const TDesC> aFmt, ... );
        static const TDesC& DataType( TMMCScBkupOwnerDataType aType );
        static const TDesC& FriendlyNameForSID( TSecureId aSID );
    };


// Logging Macros
#if defined(__MMCSCBKUPLOGGING_ENABLED__)

    _LIT( KMMCScBkupLoggingFullPath, "C:\\Logs\\MMCScBkup\\" );

    #define __LOG(aFmt)                             { _LIT(tempLogDes, aFmt); MMCScBkupLogger::Log(tempLogDes); }
    #define __LOG1(aFmt, A)                         { _LIT(tempLogDes, aFmt); MMCScBkupLogger::Log(tempLogDes, A); }
    #define __LOG2(aFmt, A, B)                      { _LIT(tempLogDes, aFmt); MMCScBkupLogger::Log(tempLogDes, A, B); }
    #define __LOG3(aFmt, A, B, C)                   { _LIT(tempLogDes, aFmt); MMCScBkupLogger::Log(tempLogDes, A, B, C); }
    #define __LOG4(aFmt, A, B, C, D)                { _LIT(tempLogDes, aFmt); MMCScBkupLogger::Log(tempLogDes, A, B, C, D); }
    #define __LOG5(aFmt, A, B, C, D, E)             { _LIT(tempLogDes, aFmt); MMCScBkupLogger::Log(tempLogDes, A, B, C, D, E); }
    #define __LOG6(aFmt, A, B, C, D, E, F)          { _LIT(tempLogDes, aFmt); MMCScBkupLogger::Log(tempLogDes, A, B, C, D, E, F); }
    #define __LOG7(aFmt, A, B, C, D, E, F, G)       { _LIT(tempLogDes, aFmt); MMCScBkupLogger::Log(tempLogDes, A, B, C, D, E, F, G); }
    #define __LOG8(aFmt, A, B, C, D, E, F, G, H)    { _LIT(tempLogDes, aFmt); MMCScBkupLogger::Log(tempLogDes, A, B, C, D, E, F, G, H); }

    // Path for backup log containing possible errors encountered during backup
    // or restore. This is needed because all errors won't interrupt operation.
    _LIT( KMMCScBkupLoggingFullPathAndName, "C:\\Private\\101F84EB\\BURLog\\bur.txt" );

    #define __LOGFILE(aFmt)                         { _LIT(tempLogDes, aFmt); MMCScBkupLogger::LogFile(tempLogDes); }
    #define __LOGFILE1(aFmt, A)                     { _LIT(tempLogDes, aFmt); MMCScBkupLogger::LogFile(tempLogDes, A); }
    #define __LOGFILE2(aFmt, A, B)                  { _LIT(tempLogDes, aFmt); MMCScBkupLogger::LogFile(tempLogDes, A, B); }
    #define __LOGFILE3(aFmt, A, B, C)               { _LIT(tempLogDes, aFmt); MMCScBkupLogger::LogFile(tempLogDes, A, B, C); }

#else

    #define __LOG(aFmt)
    #define __LOG1(aFmt, A)
    #define __LOG2(aFmt, A, B)
    #define __LOG3(aFmt, A, B, C)
    #define __LOG4(aFmt, A, B, C, D)
    #define __LOG5(aFmt, A, B, C, D, E)
    #define __LOG6(aFmt, A, B, C, D, E, F)
    #define __LOG7(aFmt, A, B, C, D, E, F, G)
    #define __LOG8(aFmt, A, B, C, D, E, F, G, H)

    #define __LOGFILE(aFmt)
    #define __LOGFILE1(aFmt, A)
    #define __LOGFILE2(aFmt, A, B)
    #define __LOGFILE3(aFmt, A, B, C)

#endif

#endif // __MMCSCBKUPLOGGER_H__
