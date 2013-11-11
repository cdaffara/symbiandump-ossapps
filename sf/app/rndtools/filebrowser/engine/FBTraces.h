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

#ifndef FILEBROWSER_TRACES_H
#define FILEBROWSER_TRACES_H

#include <e32def.h>


// ---------------------------------------------------------------------------
// You can change these logging method values below! Recompile the application to take changes effect.

    // logging methods
    // 0 = No logging
    // 1 = Flogger
    // 2 = RDebug
    // 3 = Flogger and RDebug
    
    #ifndef _DEBUG
        
        // Logging method for UREL/release builds:
        #define FILEBROWSER_LOGGING_METHOD  0

    #else

        // Logging method for UDEB/debug builds:
        #define FILEBROWSER_LOGGING_METHOD  2

    #endif    
    


// ---------------------------------------------------------------------------
// Do not make any changes to lines below...

    #if FILEBROWSER_LOGGING_METHOD == 1 || FILEBROWSER_LOGGING_METHOD == 3

        #include <flogger.h>
        _LIT(KLogFolder,"Launcher");
        _LIT(KLogFile,"Launcher_Trace.txt");

    #endif

    #if FILEBROWSER_LOGGING_METHOD == 2 || FILEBROWSER_LOGGING_METHOD == 3

        #include <e32debug.h>

    #endif


    #if FILEBROWSER_LOGGING_METHOD == 0
    
        #define LOGTEXT(AAA)
        #define LOGSTRING(AAA)
        #define LOGSTRING2(AAA,BBB)
        #define LOGSTRING3(AAA,BBB,CCC)
        #define LOGSTRING4(AAA,BBB,CCC,DDD)
    
    
    #elif FILEBROWSER_LOGGING_METHOD == 1
    
        #define LOGTEXT(AAA)                RFileLogger::Write(KLogFolder(),KLogFile(),EFileLoggingModeAppend, AAA)
        #define LOGSTRING(AAA)              do { _LIT(tempLogDes,AAA); RFileLogger::Write(KLogFolder(),KLogFile(),EFileLoggingModeAppend,tempLogDes()); } while (0)
        #define LOGSTRING2(AAA,BBB)         do { _LIT(tempLogDes,AAA); RFileLogger::WriteFormat(KLogFolder(),KLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempLogDes()),BBB); } while (0)
        #define LOGSTRING3(AAA,BBB,CCC)     do { _LIT(tempLogDes,AAA); RFileLogger::WriteFormat(KLogFolder(),KLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempLogDes()),BBB,CCC); } while (0)
        #define LOGSTRING4(AAA,BBB,CCC,DDD) do { _LIT(tempLogDes,AAA); RFileLogger::WriteFormat(KLogFolder(),KLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempLogDes()),BBB,CCC,DDD); } while (0)
   
    #elif FILEBROWSER_LOGGING_METHOD == 2
    
        #define LOGTEXT(AAA)                RDebug::Print(AAA)
        #define LOGSTRING(AAA)              do { _LIT(tempLogDes,AAA); RDebug::Print(tempLogDes); } while (0)
        #define LOGSTRING2(AAA,BBB)         do { _LIT(tempLogDes,AAA); RDebug::Print(tempLogDes, BBB); } while (0)
        #define LOGSTRING3(AAA,BBB,CCC)     do { _LIT(tempLogDes,AAA); RDebug::Print(tempLogDes, BBB, CCC); } while (0)
        #define LOGSTRING4(AAA,BBB,CCC,DDD) do { _LIT(tempLogDes,AAA); RDebug::Print(tempLogDes, BBB, CCC, DDD); } while (0)
    
    #elif FILEBROWSER_LOGGING_METHOD == 3
    
        #define LOGTEXT(AAA)                RDebug::Print(AAA); RFileLogger::Write(KLogFolder(),KLogFile(),EFileLoggingModeAppend, AAA)
        #define LOGSTRING(AAA)              do { _LIT(tempLogDes,AAA); RDebug::Print(tempLogDes); RFileLogger::Write(KLogFolder(),KLogFile(),EFileLoggingModeAppend,tempLogDes()); } while (0)
        #define LOGSTRING2(AAA,BBB)         do { _LIT(tempLogDes,AAA); RDebug::Print(tempLogDes, BBB); RFileLogger::WriteFormat(KLogFolder(),KLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempLogDes()),BBB); } while (0)
        #define LOGSTRING3(AAA,BBB,CCC)     do { _LIT(tempLogDes,AAA); RDebug::Print(tempLogDes, BBB, CCC); RFileLogger::WriteFormat(KLogFolder(),KLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempLogDes()),BBB,CCC); } while (0)
        #define LOGSTRING4(AAA,BBB,CCC,DDD) do { _LIT(tempLogDes,AAA); RDebug::Print(tempLogDes, BBB, CCC, DDD); RFileLogger::WriteFormat(KLogFolder(),KLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempLogDes()),BBB,CCC,DDD); } while (0)

    #endif

// ---------------------------------------------------------------------------

#endif
