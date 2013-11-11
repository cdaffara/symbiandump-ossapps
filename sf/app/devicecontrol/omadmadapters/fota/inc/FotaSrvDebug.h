/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   This file defines logging macros 
*
*/



#ifndef FOTADEBUG_H
#define FOTADEBUG_H

    // IMPORT_C static TInt Print(TRefByValue<const TDesC> aFmt,...);
#if defined (__WINS__)
    #define _FOTA_DEBUG
  //  #define _FOTA_DEBUG_RDEBUG
#else
    #define _FOTA_DEBUG
#endif
    //#endif
    #ifdef _FOTA_DEBUG


        #include <e32std.h>
        #include <f32file.h>
        #include <flogger.h>
        #include <e32svr.h>

        _LIT( KLogFile, "fumo.log" );
        _LIT( KLogDir, "fota" );

        inline void FWrite (TRefByValue<const TDesC> aFmt,...)
            {
            VA_LIST list;
            VA_START( list, aFmt );
            RFileLogger::WriteFormat( KLogDir,KLogFile,EFileLoggingModeAppend ,TPtrC(aFmt) ,list );
            }

        #ifdef _FOTA_DEBUG_RDEBUG 
            #ifndef __GNUC__          
                #define FLOG            RDebug::Print
            #else   // __GNUC__       
                #define FLOG(arg...)    RDebug::Print(arg);
            #endif // __GNUC__        
        #else // _FOTA_DEBUG_RDEBUG
            #ifndef __GNUC__          
                #define FLOG            FWrite
            #else   // __GNUC__       
                #define FLOG(arg...)    FWrite(arg);
            #endif // __GNUC__        
        #endif // _FOTA_DEBUG_RDEBUG

    #else   // _FOTA_DEBUG    
        #ifndef __GNUC__
            #define FLOG
        #else
            #define FLOG(arg...)
        #endif // __GNUC__
    #endif // _FOTA_DEBUG

#endif      // FOTADEBUG_H
            
// End of File
