/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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

#ifndef VCXTESTLOG_H
#define VCXTESTLOG_H

// INCLUDES
#include <e32std.h>
#include <e32svr.h>

//#define FILE_LOGGING_IN_UREL_BUILD 1

// MACROS
#ifdef _DEBUG

// SELECT LOG TYPE FROM HERE
// 0 = RDebug
// 1 = file logging
// 2 = no logging
#ifdef __WINSCW__
    #define LOGTYPE 0
#else
    #define LOGTYPE 1
#endif __WINSCW__

#if LOGTYPE == 0
      
    #define VCXLOGLO1(s)               RDebug::Print(_L(#s))
    #define VCXLOGLO2(s, a)            RDebug::Print(_L(#s), a)
    #define VCXLOGLO3(s, a, b)         RDebug::Print(_L(#s), a, b)
    #define VCXLOGLO4(s, a, b, c)      RDebug::Print(_L(#s), a, b, c)
    #define VCXLOGLO5(s, a, b, c, d)   RDebug::Print(_L(#s), a, b, c, d)

#endif

#if LOGTYPE == 1
    
    #include <flogger.h>
    
    _LIT(KVCXLogFolder2,"Fusion");
    _LIT(KVCXLogFile2,"Fusion.txt");
    
    #define VCXLOGLO1(AAA)                  do { _LIT(tempVCXLogDes,AAA); RFileLogger::Write(KVCXLogFolder2(),KVCXLogFile2(),EFileLoggingModeAppend,tempVCXLogDes()); } while ( EFalse )
    #define VCXLOGLO2(AAA,BBB)              do { _LIT(tempVCXLogDes,AAA); RFileLogger::WriteFormat(KVCXLogFolder2(),KVCXLogFile2(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempVCXLogDes()),BBB); } while ( EFalse )
    #define VCXLOGLO3(AAA,BBB,CCC)          do { _LIT(tempVCXLogDes,AAA); RFileLogger::WriteFormat(KVCXLogFolder2(),KVCXLogFile2(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempVCXLogDes()),BBB,CCC); } while ( EFalse )
    #define VCXLOGLO4(AAA,BBB,CCC,DDD)      do { _LIT(tempVCXLogDes,AAA); RFileLogger::WriteFormat(KVCXLogFolder2(),KVCXLogFile2(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempVCXLogDes()),BBB,CCC,DDD); } while ( EFalse )
    #define VCXLOGLO5(AAA,BBB,CCC,DDD,EEE)  do { _LIT(tempVCXLogDes,AAA); RFileLogger::WriteFormat(KVCXLogFolder2(),KVCXLogFile2(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempVCXLogDes()),BBB,CCC,DDD,EEE); } while ( EFalse )
    
#endif

#if LOGTYPE == 2
   
    #define VCXLOGLO1(s)               
    #define VCXLOGLO2(s, a)           
    #define VCXLOGLO3(s, a, b)       
    #define VCXLOGLO4(s, a, b, c)     
    #define VCXLOGLO5(s, a, b, c, d)

#endif

#else // _DEBUG

    #ifdef FILE_LOGGING_IN_UREL_BUILD
    
    #include <flogger.h>
    
    _LIT(KVCXLogFolder2,"Fusion");
    _LIT(KVCXLogFile2,"Fusion.txt");
    
    #define VCXLOGLO1(AAA)                  do { _LIT(tempVCXLogDes,AAA); RFileLogger::Write(KVCXLogFolder2(),KVCXLogFile2(),EFileLoggingModeAppend,tempVCXLogDes()); } while ( EFalse )
    #define VCXLOGLO2(AAA,BBB)              do { _LIT(tempVCXLogDes,AAA); RFileLogger::WriteFormat(KVCXLogFolder2(),KVCXLogFile2(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempVCXLogDes()),BBB); } while ( EFalse )
    #define VCXLOGLO3(AAA,BBB,CCC)          do { _LIT(tempVCXLogDes,AAA); RFileLogger::WriteFormat(KVCXLogFolder2(),KVCXLogFile2(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempVCXLogDes()),BBB,CCC); } while ( EFalse )
    #define VCXLOGLO4(AAA,BBB,CCC,DDD)      do { _LIT(tempVCXLogDes,AAA); RFileLogger::WriteFormat(KVCXLogFolder2(),KVCXLogFile2(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempVCXLogDes()),BBB,CCC,DDD); } while ( EFalse )
    #define VCXLOGLO5(AAA,BBB,CCC,DDD,EEE)  do { _LIT(tempVCXLogDes,AAA); RFileLogger::WriteFormat(KVCXLogFolder2(),KVCXLogFile2(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempVCXLogDes()),BBB,CCC,DDD,EEE); } while ( EFalse )

    #else // FILE_LOGGING_IN_UREL_BUILD

    #define VCXLOGLO1(s)
    #define VCXLOGLO2(s, a)
    #define VCXLOGLO3(s, a, b)
    #define VCXLOGLO4(s, a, b, c)
    #define VCXLOGLO5(s, a, b, c, d)
  
    #endif // FILE_LOGGING_IN_UREL_BUILD

#endif // _DEBUG


#endif  // VCXTESTLOG_H

// End of File
