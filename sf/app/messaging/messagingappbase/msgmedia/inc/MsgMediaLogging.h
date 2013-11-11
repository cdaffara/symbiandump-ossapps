/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*     Logging macros for MsgMedia.dll
*
*/



// ========== LOGGING MACROS ===============================

#ifndef __MSGMEDIALOGGING_H
#define __MSGMEDIALOGGING_H

#ifdef __MSGMEDIA_DEBUG

#include <e32std.h>
#include <e32def.h>
#include <flogger.h>

/*  logging macros usage:

// replace MSGM with your own TLA (Three Letter Acronyme).

// set logging on in mmp file...
MACRO __MSGMEDIA_DEBUG
#ifdef WINS
MACRO USE_RDEBUG
#endif


void CTestAppUi::ConstructL()
{
    // creates log directory and log file on app startup.
    // prints starting banner...
    MSGMLOGGER_CREATE(iFs);

    // entering to function...
    MSGMLOGGER_ENTERFN("ConstructL()");

    TInt number = 42;
    TFileName file;
    file = "something" ....

    // print variable number of parameters...
    // Note: use _L macro for format string!
    MSGMLOGGER_WRITEF(_L("MSGM: number = %d, file = %S"), number, &file);

    // print just a string (no _L macro here!)
    MSGMLOGGER_WRITE("Hello world!");

    // leave from function...
    MSGMLOGGER_LEAVEFN("ConstructL()");
}

CTestAppUi::~CTestAppUi()
{
    // print app exit banner when application exits.
    MSGMLOGGER_DELETE;
}

*/

#ifdef USE_RDEBUG

_LIT(KMSGMLogBanner,             "MSGM - MsgMedia");
_LIT(KMSGMLogEnterFn,            "MSGM: >> %S");
_LIT(KMSGMLogLeaveFn,            "MSGM: << %S");
_LIT(KMSGMLogWrite,              "MSGM: %S");
_LIT(KMSGMLogTag,                "MSGM: ");
_LIT(KMSGMLogExit,               "MSGM: Application exit");
_LIT(KMSGMLogTimeFormatString,   "%H:%T:%S:%*C2");

#define MSGMLOGGER_CREATE(a)          {RDebug::Print(KMSGMLogBanner);}
#define MSGMLOGGER_DELETE             {RDebug::Print(KMSGMLogExit);}
#define MSGMLOGGER_ENTERFN(a)         {_LIT(temp, a); RDebug::Print(KMSGMLogEnterFn, &temp);}
#define MSGMLOGGER_LEAVEFN(a)         {_LIT(temp, a); RDebug::Print(KMSGMLogLeaveFn, &temp);}
#define MSGMLOGGER_WRITE(a)           {_LIT(temp, a); RDebug::Print(KMSGMLogWrite, &temp);}
#define MSGMLOGGER_WRITE_TIMESTAMP(a) {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KMSGMLogTimeFormatString ); buffer.Insert(0, temp); buffer.Insert(0, KMSGMLogTag); RDebug::Print(buffer); }
#define MSGMLOGGER_WRITEF             RDebug::Print

#else

_LIT( KMSGMLogDir,               "MSGM");
_LIT( KMSGMLogFile,              "MSGM.txt");
_LIT8(KMSGMLogBanner,            "MSGM - MsgMedia");
_LIT8(KMSGMLogEnterFn,           "MSGM: >> %S");
_LIT8(KMSGMLogLeaveFn,           "MSGM: << %S");
_LIT8(KMSGMLogExit,              "MSGM: Application exit");
_LIT( KMSGMLogTimeFormatString,  "%H:%T:%S:%*C2");

#define MSGMLOGGER_CREATE(a)          {MSGM_FCreate(a);}
#define MSGMLOGGER_DELETE             {RFileLogger::Write(KMSGMLogDir, KMSGMLogFile, EFileLoggingModeAppend, KMSGMLogExit);}
#define MSGMLOGGER_ENTERFN(a)         {_LIT8(temp, a); RFileLogger::WriteFormat(KMSGMLogDir, KMSGMLogFile, EFileLoggingModeAppend, KMSGMLogEnterFn, &temp);}
#define MSGMLOGGER_LEAVEFN(a)         {_LIT8(temp, a); RFileLogger::WriteFormat(KMSGMLogDir, KMSGMLogFile, EFileLoggingModeAppend, KMSGMLogLeaveFn, &temp);}
#define MSGMLOGGER_WRITE(a)           {_LIT(temp, a); RFileLogger::Write(KMSGMLogDir, KMSGMLogFile, EFileLoggingModeAppend, temp);}
#define MSGMLOGGER_WRITE_TIMESTAMP(a) {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KMSGMLogTimeFormatString ); buffer.Insert(0, temp); RFileLogger::Write(KMSGMLogDir, KMSGMLogFile, EFileLoggingModeAppend, buffer); }
#define MSGMLOGGER_WRITEF             MSGM_FPrint

inline void MSGM_FPrint(const TRefByValue<const TDesC> aFmt, ...)
{
    VA_LIST list;
    VA_START(list,aFmt);
    RFileLogger::WriteFormat(KMSGMLogDir, KMSGMLogFile, EFileLoggingModeAppend, aFmt, list);
}

inline void MSGM_FPrint(const TDesC& aDes)
{
    RFileLogger::WriteFormat(KMSGMLogDir, KMSGMLogFile, EFileLoggingModeAppend, aDes);
}

inline void MSGM_FHex(const TUint8* aPtr, TInt aLen)
{
    RFileLogger::HexDump(KMSGMLogDir, KMSGMLogFile, EFileLoggingModeAppend, 0, 0, aPtr, aLen);
}

inline void MSGM_FHex(const TDesC8& aDes)
{
    MSGM_FHex(aDes.Ptr(), aDes.Length());
}

inline void MSGM_FCreate(RFs& aFs)
{
    TFileName path(_L("c:\\logs\\"));
    path.Append(KMSGMLogDir);
    path.Append(_L("\\"));
    aFs.MkDirAll(path);
    RFileLogger::WriteFormat(KMSGMLogDir, KMSGMLogFile, EFileLoggingModeOverwrite, KMSGMLogBanner);
}

#endif // USE_RDEBUG

#else // __MSGMEDIA_DEBUG

// dummy inline MSGM_FPrint for MSGMLOGGER_WRITEF macro.
inline TInt MSGM_FPrint(const TRefByValue<const TDesC> /*aFmt*/, ...) { return 0; }

#define MSGMLOGGER_CREATE(a)
#define MSGMLOGGER_DELETE
#define MSGMLOGGER_ENTERFN(a)
#define MSGMLOGGER_LEAVEFN(a)
#define MSGMLOGGER_WRITE(a)
// if __MSGMEDIA_DEBUG not defined this call to FPrint gets optimized away.
#define MSGMLOGGER_WRITEF 1 ? 0 : MSGM_FPrint
#define MSGMLOGGER_WRITE_TIMESTAMP(a)

#endif // __MSGMEDIA_DEBUG

#endif // __MSGMEDIALOGGING_H

// =========================================================
