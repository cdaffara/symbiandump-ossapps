/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MYLOCATIONLOGGER_H
#define MYLOCATIONLOGGER_H
#include <e32base.h> 
#include <e32std.h> 
#include <e32des8.h>
#include <e32svr.h>
#include "mylocloggingconfiguration.h"
_LIT( KLogsDir, "Mylocation");
_LIT( KLogFileName, "Mylocation.log");

/*
 We have 3 possible logging methods:

 set MYLOC_LOGGING_METHOD to 0 to get no logging at all
 set MYLOC_LOGGING_METHOD to 1 to get logging via RDebug::Print
 set MYLOC_LOGGING_METHOD to 2 to get logging to log file

 Switching on/off is done in the Configuration file (MyLocLoggingConfiguration.h)
 */

#if ( defined (_MYLOCLOGGING) )
#if ( defined (_MYLOC_LOGGING_TO_FILE))
#define MYLOC_LOGGING_METHOD 2
#else
#define MYLOC_LOGGING_METHOD 1
#endif
#else
#define MYLOC_LOGGING_METHOD 0
#endif

#if (MYLOC_LOGGING_METHOD==0)
#define MYLOCLOGSTRING(C)
#define MYLOCLOGSTRING1(C, X)
#define MYLOCLOGSTRING2(C, X, Y)
#define MYLOCLOGSTRING3(C, X, Y, Z)
#define MYLOCLOGSTRING4(C, X, Y, Z, A)
#endif
#if (MYLOC_LOGGING_METHOD==1)
#define MYLOCLOGSTRING(C)                RDebug::Print(_L(C));
#define MYLOCLOGSTRING1(C, X)            RDebug::Print(_L(C),X);
#define MYLOCLOGSTRING2(C, X, Y)         RDebug::Print(_L(C),X, Y);
#define MYLOCLOGSTRING3(C, X, Y, Z)      RDebug::Print(_L(C),X, Y, Z);
#define MYLOCLOGSTRING4(C, X, Y, Z, A)   RDebug::Print(_L(C),X, Y, Z, A);
#endif
#if (MYLOC_LOGGING_METHOD==2)
#ifndef __COMMSDEBUGUTILITY_H__
#define __COMMSDEBUGUTILITY_H__
#include <flogger.h>
#endif


#define MYLOCLOGSTRING(C)                {_LIT(tempLogDes,C);RFileLogger::WriteFormat(KLogsDir(),KLogFileName(), EFileLoggingModeAppend, tempLogDes());}
#define MYLOCLOGSTRING1(C, X)            {_LIT(tempLogDes,C);RFileLogger::WriteFormat(KLogsDir(),KLogFileName(), EFileLoggingModeAppend, TRefByValue<const TDesC>(tempLogDes()),X);}
#define MYLOCLOGSTRING2(C, X, Y)         {_LIT(tempLogDes,C);RFileLogger::WriteFormat(KLogsDir(),KLogFileName(), EFileLoggingModeAppend, TRefByValue<const TDesC>(tempLogDes()),X,Y);}
#define MYLOCLOGSTRING3(C, X, Y, Z)      {_LIT(tempLogDes,C);RFileLogger::WriteFormat(KLogsDir(),KLogFileName(), EFileLoggingModeAppend, TRefByValue<const TDesC>(tempLogDes()),X,Y,Z);}
#define MYLOCLOGSTRING4(C, X, Y, Z, A)   {_LIT(tempLogDes,C);RFileLogger::WriteFormat(KLogsDir(),KLogFileName(), EFileLoggingModeAppend, TRefByValue<const TDesC>(tempLogDes()),X,Y,Z,A);}
#endif

/*
 *Trace implementation
 * Description:
 * Macro definition file for logging.
 */

#if !(MYLOC_LOGGING_METHOD == 0)
#ifdef __cplusplus

// define the following macro to enable call stack trace.
#define FUNCTION_ETRY_EXIT

const TInt KMaxLogLength = 256;
class CLogFunctionEntryExit : public CBase
{
public:
    CLogFunctionEntryExit(const char* aFunctionName, const TInt aLineNo) :
        iFunctionName(aFunctionName)
    {
    #if (MYLOC_LOGGING_METHOD == 2)
            iBuffer = HBufC8::New(KMaxLogLength);
            if (iBuffer != NULL)
            {
                TPtr8 buf = iBuffer->Des();
                buf.Copy((const TUint8 *) aFunctionName);
                _LIT8(KFormat,"=> %S [%02d])");
                RFileLogger::WriteFormat(KLogsDir(), KLogFileName(),
                        EFileLoggingModeAppend, TRefByValue<const TDesC8> (
                                KFormat()), &buf, aLineNo);
            }
    #else                
            RDebug::Printf("=> %s [%02d])", iFunctionName, aLineNo);
    #endif                
        }
        ~CLogFunctionEntryExit()
        {
    #if (MYLOC_LOGGING_METHOD == 2)
            TPtr8 buf = iBuffer->Des();
            _LIT8(KFormat,"<= %S");
            RFileLogger::WriteFormat(KLogsDir(), KLogFileName(),
                    EFileLoggingModeAppend, TRefByValue<const TDesC8> (KFormat()),
                    &buf);
            delete iBuffer;
    #else            
            RDebug::Printf("<= %s", iFunctionName);
    #endif            
        }
private:
        const char* iFunctionName;
    #if (MYLOC_LOGGING_METHOD == 2)
        HBufC8* iBuffer;
    #endif
};

#ifdef FUNCTION_ETRY_EXIT
// track
#pragma message ("MyLocation Call stack trace - ENABLE")
#define __TRACE_CALLSTACK CLogFunctionEntryExit functionEtryExit(__PRETTY_FUNCTION__,__LINE__)
#endif
#endif // __cplusplus
#endif //
#ifndef __TRACE_CALLSTACK
// don't track function enter and exit
#pragma message ("MyLocation Call stack trace - DISABLE")
#define __TRACE_CALLSTACK
#endif

#endif //MYLOCATIONLOGGER_H
