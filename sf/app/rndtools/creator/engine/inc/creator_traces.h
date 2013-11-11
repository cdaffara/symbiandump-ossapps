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





#ifndef __CREATOR_TRACES_H__
#define __CREATOR_TRACES_H__

#include <e32def.h>


// ---------------------------------------------------------------------------
// You change these logging method values below! Recompile the application to take changes effect.

    // logging methods
    // 0 = No logging
    // 1 = Flogger
    // 2 = RDebug
    // 3 = Flogger and RDebug
    
    #ifndef _DEBUG
        
        // Logging method for UREL builds:
        #define CREATOR_LOGGING_METHOD  3

    #else

        // Logging method for UDEB builds:
        #define CREATOR_LOGGING_METHOD  3

    #endif    
    


// ---------------------------------------------------------------------------
// Do not make any changes to lines below...

    #if CREATOR_LOGGING_METHOD == 1 || CREATOR_LOGGING_METHOD == 3

        #include <flogger.h>
        _LIT(KLogFolder,"Creator");
        _LIT(KLogFile,"Creator_Trace.txt");

    #endif

    #if CREATOR_LOGGING_METHOD == 2 || CREATOR_LOGGING_METHOD == 3

        #include <e32debug.h>

    #endif


    #if CREATOR_LOGGING_METHOD == 0
    
        #define LOGTEXT(AAA)
        #define LOGSTRING(AAA)
        #define LOGSTRING2(AAA,BBB)
        #define LOGSTRING3(AAA,BBB,CCC)
        #define LOGSTRING4(AAA,BBB,CCC,DDD)
    
    
    #elif CREATOR_LOGGING_METHOD == 1
    
        #define LOGTEXT(AAA)                RFileLogger::Write(KLogFolder(),KLogFile(),EFileLoggingModeAppend, AAA)
        #define LOGSTRING(AAA)              do { _LIT(tempLogDes,AAA); RFileLogger::Write(KLogFolder(),KLogFile(),EFileLoggingModeAppend,tempLogDes()); } while (0)
        #define LOGSTRING2(AAA,BBB)         do { _LIT(tempLogDes,AAA); RFileLogger::WriteFormat(KLogFolder(),KLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempLogDes()),BBB); } while (0)
        #define LOGSTRING3(AAA,BBB,CCC)     do { _LIT(tempLogDes,AAA); RFileLogger::WriteFormat(KLogFolder(),KLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempLogDes()),BBB,CCC); } while (0)
        #define LOGSTRING4(AAA,BBB,CCC,DDD) do { _LIT(tempLogDes,AAA); RFileLogger::WriteFormat(KLogFolder(),KLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempLogDes()),BBB,CCC,DDD); } while (0)
   
    #elif CREATOR_LOGGING_METHOD == 2
    
        #define LOGTEXT(AAA)                RDebug::Print(AAA)
        #define LOGSTRING(AAA)              do { _LIT(tempLogDes,AAA); RDebug::Print(tempLogDes); } while (0)
        #define LOGSTRING2(AAA,BBB)         do { _LIT(tempLogDes,AAA); RDebug::Print(tempLogDes, BBB); } while (0)
        #define LOGSTRING3(AAA,BBB,CCC)     do { _LIT(tempLogDes,AAA); RDebug::Print(tempLogDes, BBB, CCC); } while (0)
        #define LOGSTRING4(AAA,BBB,CCC,DDD) do { _LIT(tempLogDes,AAA); RDebug::Print(tempLogDes, BBB, CCC, DDD); } while (0)
    
    #elif CREATOR_LOGGING_METHOD == 3
    
        #define LOGTEXT(AAA)                RDebug::Print(AAA); RFileLogger::Write(KLogFolder(),KLogFile(),EFileLoggingModeAppend, AAA)
        #define LOGSTRING(AAA)              do { _LIT(tempLogDes,AAA); RDebug::Print(tempLogDes); RFileLogger::Write(KLogFolder(),KLogFile(),EFileLoggingModeAppend,tempLogDes()); } while (0)
        #define LOGSTRING2(AAA,BBB)         do { _LIT(tempLogDes,AAA); RDebug::Print(tempLogDes, BBB); RFileLogger::WriteFormat(KLogFolder(),KLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempLogDes()),BBB); } while (0)
        #define LOGSTRING3(AAA,BBB,CCC)     do { _LIT(tempLogDes,AAA); RDebug::Print(tempLogDes, BBB, CCC); RFileLogger::WriteFormat(KLogFolder(),KLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempLogDes()),BBB,CCC); } while (0)
        #define LOGSTRING4(AAA,BBB,CCC,DDD) do { _LIT(tempLogDes,AAA); RDebug::Print(tempLogDes, BBB, CCC, DDD); RFileLogger::WriteFormat(KLogFolder(),KLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempLogDes()),BBB,CCC,DDD); } while (0)

    #endif

// ---------------------------------------------------------------------------

#endif // __LAUNCHER_TRACES_H__

