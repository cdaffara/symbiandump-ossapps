/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
/**
*
**/

#include "debug.h"

#define RD_CALENDAR_PREVIEW

#ifndef PIM_TRACE_H
#define PIM_TRACE_H

#if defined (_DEBUG) || defined (_PIM_FILE_LOG) 
    //Includes
    #include <e32base.h>
    #include <e32std.h>
    #include <flogger.h>
    #include <e32svr.h>
    #include <f32file.h>
	
    const TInt KMaxLogLineLength = 512;
#endif
#if defined( _DEBUG ) 
    #define PIM_TRAP_HANDLE( _err, _s ) \
    { \
        TRAP( _err, _s; ); \
        ASSERT( !_err ); \
    }
    #define PIM_TRAPD_HANDLE( _s ) \
    { \
        TRAPD( _err, _s; ); \
        ASSERT( !_err ); \
    }
    #define WARNING_NOTE( _s )
#else 
// urel
    #define PIM_TRAP_HANDLE( _err, _s )   TRAP_IGNORE( _s; );
    #define PIM_TRAPD_HANDLE( _s )  TRAP_IGNORE( _s; );
    #define WARNING_NOTE( _s )
#endif // _DEBUG

#if defined (_DEBUG) || defined (_PIM_FILE_LOG) 
    
   
    /********************************************************************************************************************/

    // MMP MACROS
    #if defined( TRACE_MODULE_AIAGENDAPLUGINENGINE )
        _LIT8( KModuleName8, "AIAgendaPluginEngine" );
    #elif defined( TRACE_MODULE_CALENDAR ) 
        _LIT8( KModuleName8, "calendar" );
    #elif defined( TRACE_MODULE_CALENGLOBALDATA ) 
        _LIT8( KModuleName8, "CalenGlobalData" );
    #elif defined( TRACE_MODULE_CALENCOMMONUI ) 
        _LIT8( KModuleName8, "CalenCommonUi" );
    #elif defined( TRACE_MODULE_CALENDEFAULTEDITORS ) 
        _LIT8( KModuleName8, "CalenDefaultEditors" );
    #elif defined( TRACE_MODULE_CALENLUNAR ) 
        _LIT8( KModuleName8, "CalenLunar" );
    #elif defined( TRACE_MODULE_CALENLUNARPLUGIN ) 
        _LIT8( KModuleName8, "CalenLunarPlugin" );
    #elif defined( TRACE_MODULE_CALENTHAIPLUGIN ) 
        _LIT8( KModuleName8, "CalenThaiPlugin" );
    #elif defined( TRACE_MODULE_CALENSVR ) 
        _LIT8( KModuleName8, "CalenSvr" );
    #elif defined( TRACE_MODULE_CALENSVRCLIENT ) 
        _LIT8( KModuleName8, "CalenSvrClient" );
    #else // default
        _LIT8( KModuleName8, "CalendarUI" );
    #endif

    // traces
    // [module_name]_ENTRY_POINT;[thread_id];[function_name];[filename]:[line_number];
    _LIT8( KFuncEntryFormat8, ";%S_ENTRY_POINT;%d;%S;%S:%d;");
    // [module_name]_EXIT_POINT;[thread_id];[function_name];[filename]:[line_number];
    _LIT8( KFuncExitFormat8,  ";%S_EXIT_POINT;%d;%S;%S:%d;");
    
    /********************************************************************************************************************/
    NONSHARABLE_CLASS(TOverflowTruncate8) : public TDes8Overflow
        {
        public:
            void Overflow(TDes8& /*aDes*/) {}
        };

    //Trace macro 8bit
    // Disable lint warning of function having a variable number of
    //  arguments 
    //lint -esym( 960, 69 )
    inline void TraceDebugPrint( TRefByValue<const TDesC8> aFmt, ...) 
        {
        VA_LIST list;
        
        // Disabled  lint warning of comma operator 
        //lint -esym( 960, 42 )
        VA_START( list, aFmt ); 
        
        TOverflowTruncate8 overflow;
        TBuf8<KMaxLogLineLength> buf8;
        buf8.AppendFormatList( aFmt, list, &overflow );
        TBuf16<KMaxLogLineLength> buf16( buf8.Length() );
        buf16.Copy( buf8 );
        TRefByValue<const TDesC> tmpFmt( _L("%S") );
 #if defined (_PIM_FILE_LOG)  
        _LIT( KLogDir, "CalenUi");
        _LIT( KLogDir2, "c:\\CalenUi");
        _LIT( KLogFile, "log.txt");
        _LIT( KDir, "c:\\logs\\CalenUi" );
        _LIT( KDir2, "c:\\logs\\CalenUi" );
        
        RFs fs;
        fs.Connect();
        fs.MkDirAll( KDir );
        
        RFileLogger fileLogger;
        fileLogger.Connect();
        fileLogger.CreateLog( KLogDir, KLogFile, EFileLoggingModeAppend );
        fileLogger.SetDateAndTime( EFalse, ETrue );
        fileLogger.WriteFormat( tmpFmt, &buf16 );
        fileLogger.CloseLog();
        fileLogger.Close();

        fs.MkDirAll( KDir2 );
        
        RFileLogger fileLogger2;
        fileLogger2.Connect();
        fileLogger2.CreateLog( KLogDir2, KLogFile, EFileLoggingModeAppend );
        fileLogger2.SetDateAndTime( EFalse, ETrue );
        fileLogger2.WriteFormat( tmpFmt, &buf16 );
        fileLogger2.CloseLog();
        fileLogger2.Close();
        fs.Close();
        
#else      
        RDebug::Print( tmpFmt, &buf16 );
#endif
        }
        
    _LIT8( KWarningFormat2, " error %d trapped line %d");
    inline void WriteTrapInfo( TInt aError, TInt aWarningNote )
        {
        if ( aError )
            {    
            TBuf8<128> text;
            text.Format( KWarningFormat2, aError, aWarningNote );
            
            TraceDebugPrint(text);
            }       
        }
        
        
    #undef TRAP_INSTRUMENTATION_LEAVE
    #define TRAP_INSTRUMENTATION_LEAVE(aResult) WriteTrapInfo( aResult, __LINE__);
        
    /********************************************************************************************************************/
    //Debug print
    #ifdef TRACE_ENTRY_EXIT_POINT // MACRO IN MMP FILE
        #ifdef __ARMCC__
            #define TRACE_FUNC_ENTRY_DEBUG_PRINT                                                                \
            {                                                                                                   \
                const TUint threadId( RThread().Id() );                                                         \
                TPtrC8 ptr8File( (TUint8*)__MODULE__ );                                                         \
                TPtrC8 ptr8PF( (TUint8*)__PRETTY_FUNCTION__ );                                                  \
                TraceDebugPrint( KFuncEntryFormat8, &KModuleName8, threadId, &ptr8PF, &ptr8File, __LINE__ );    \
            }
            #define TRACE_FUNC_EXIT_DEBUG_PRINT                                                                 \
            {                                                                                                   \
                const TUint threadId( RThread().Id() );                                                         \
                TPtrC8 ptr8File( (TUint8*)__MODULE__ );                                                         \
                TPtrC8 ptr8PF( (TUint8*)__PRETTY_FUNCTION__ );                                                  \
                TraceDebugPrint( KFuncExitFormat8, &KModuleName8, threadId, &ptr8PF, &ptr8File, __LINE__ );     \
            }
        #else // __WINS__
            #define TRACE_FUNC_ENTRY_DEBUG_PRINT                                                                \
            {                                                                                                   \
                const TUint threadId( RThread().Id() );                                                         \
                TPtrC8 ptr8File( (TUint8*)__FILE__ );                                                           \
                TPtrC8 ptr8PF( (TUint8*)__PRETTY_FUNCTION__ );                                                  \
                TraceDebugPrint( KFuncEntryFormat8, &KModuleName8, threadId, &ptr8PF, &ptr8File, __LINE__ );    \
            }
            #define TRACE_FUNC_EXIT_DEBUG_PRINT                                                                 \
            {                                                                                                   \
                const TUint threadId( RThread().Id() );                                                         \
                TPtrC8 ptr8File( (TUint8*)__FILE__ );                                                           \
                TPtrC8 ptr8PF( (TUint8*)__PRETTY_FUNCTION__ );                                                  \
                TraceDebugPrint( KFuncExitFormat8, &KModuleName8, threadId, &ptr8PF, &ptr8File, __LINE__ );     \
            }
        #endif
    #endif

    /********************************************************************************************************************/
    //Check if trace should be activated 
    #ifdef TRACE_ENTRY_EXIT_POINT // MACRO IN MMP FILE
        #define TRACE_ENTRY_POINT TRACE_FUNC_ENTRY_DEBUG_PRINT
        #define TRACE_EXIT_POINT  TRACE_FUNC_EXIT_DEBUG_PRINT
    #else
        #define TRACE_ENTRY_POINT
        #define TRACE_EXIT_POINT
    #endif
    
#else 
    #define TRACE_ENTRY_POINT
    #define TRACE_EXIT_POINT

#endif // defined (_DEBUG) || defined (_PIM_FILE_LOG) 

#endif // PIM_TRACE_H
