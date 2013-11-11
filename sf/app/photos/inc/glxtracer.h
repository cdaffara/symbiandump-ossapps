/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Logging macros
*
*/



#ifndef GLXTRACER_H
#define GLXTRACER_H
 
/**
 * This file has been copied from the forum nokia wiki and renamed to glxtracer.h
 * http://wiki.forum.nokia.com/index.php/Trace_Function_Enter%2C_Exit_and_Leave
 */

#include <e32base.h>
 
// Define tracer logging method
// 0    = Logging off
// 1    = Log to RDebug
// 2    = Log to file (RFileLogger)
#define TRACER_LOG_METHOD 0
 
// ============================================================================
 
// Logging off, define empty macros and skip all the rest
#if TRACER_LOG_METHOD == 0
 
    #define TRACER(func)    
    #define TRACER_RET(func,format)
 
#else                       // Logging on
 
    // Macro to print function entry, exit and leave. 
    // Example: TRACER("CMyClass::MyFunction");
    #define TRACER(func) TTracer function_tracer( _S(func), _S("") );
 
    // Macro to print function return value in addition to entry, exit 
    // and leave conditions Second parameter is a formatting string used 
    // to print the return value Example to print an integer return value:
    // TRACER_RET("CMyclass::MyFunction", "%d");
    #define TRACER_RET(func,format) TTracer func_tracer( _S(func), _S(format) );
 
    #if TRACER_LOG_METHOD == 1      // Print to RDebug
 
        #include <e32debug.h>
        #define TRACER_PRINT(a)         RDebug::Print(a,&iFunc);
        #define TRACER_PRINT_RET(a,b)   RDebug::Print(a,&iFunc,b);
 
    #elif TRACER_LOG_METHOD == 2    // Print to file
 
        #include <flogger.h>
        _LIT( KLogDir,  "tracer" );     // Log directory: C:\logs\tracer
        _LIT( KLogFile, "tracer.txt" ); // Log file: c:\logs\tracer\tracer.txt
        #define TRACER_PRINT(a)         RFileLogger::WriteFormat(KLogDir, \
                            KLogFile,EFileLoggingModeAppend,a,&iFunc);
        #define TRACER_PRINT_RET(a,b)   RFileLogger::WriteFormat(KLogDir, \
                            KLogFile,EFileLoggingModeAppend,a,&iFunc,b);
 
    #endif
 
    _LIT( KLogEnter,    "%S: ENTER" );
    _LIT( KLogExit,     "%S: EXIT" );
    _LIT( KLogLeave,    "%S: LEAVE!" );
    _LIT( KLogExitRet,  "%S: EXIT, Returning " );
 
    /**
     * Simple tracer class that logs function enter, exit or leave
     */
    class TTracer
        {
    public:
 
        /**
         * inline constructor to write log of entering a function
         */
        TTracer( const TText* aFunc, const TText* aRetFormat )
            : iFunc( aFunc )
            , iRetFormat( aRetFormat )
            {
            TRACER_PRINT( KLogEnter );
            }
 
        /**
         * inline destructor to write log of exiting a function 
         * normally or with a leave
         */
        ~TTracer()
            {
            if ( std::uncaught_exception() ) // Leave is an exception
                {
                // The function exited with a leave
                TRACER_PRINT( KLogLeave );
                }
            else
                {
                // The function exited normally
                if ( iRetFormat.Length() == 0 )
                    {
                    TRACER_PRINT( KLogExit );
                    }
                else
                    {
                    // Log the return value
                    #ifdef __WINS__
                        TInt32 retVal = 0;
 
                        // The assembly bit. This needs to be reimplemented
                        // for every target. 
                        _asm( mov retVal, ebx );
 
                        TBuf<100> format( KLogExitRet );
                        format.Append( iRetFormat );
                        TRACER_PRINT_RET( format, retVal );
                    #else
                        TRACER_PRINT( KLogExit );
                    #endif
                    }
                }
            }
 
    private:
 
        /** 
         * Pointer descriptor to function signature that is to be logged.
         */
        TPtrC iFunc;
 
        /**
         * Formatting string used to print the function return value
         */
        TPtrC iRetFormat;
 
        };
 
#endif // TRACER_LOG_METHOD == 0
 
#endif // GLXTRACER_H
