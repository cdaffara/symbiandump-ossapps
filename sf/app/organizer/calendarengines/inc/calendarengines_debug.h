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
* Description:  Header file defining all debug related functions for calendar engine. Macros, funtions
*								defined here can be used to debug calendar engines.
*
*/

/**
*
**/
#ifndef PIM_TRACE_H
#define PIM_TRACE_H

#ifdef _DEBUG
    #define PIM_TRAPD_HANDLE(_s) TRAPD(leave,_s); ASSERT(!leave)
    #define PIM_ASSERT_DEBUG(_s) TInt err = _s; ASSERT(!err)
#else // _DEBUG
    #define PIM_TRAPD_HANDLE(_s) TRAP_IGNORE(_s)
    #define PIM_ASSERT_DEBUG(_s) _s   
#endif // _DEBUG


#ifdef _DEBUG
    /********************************************************************************************************************/
    //Inlude & define common data
    #include <e32base.h>
    #include <e32std.h>
    #include <flogger.h>
    #include <e32svr.h>
   
    const TInt KMaxLogLineLength = 512;
   
    /********************************************************************************************************************/

    #if defined( TRACE_MODULE_AGNVERSIT2 ) // MACRO IN MMP FILE
        _LIT8( KModuleName8, "AgnVersit2" );
    #elif defined( TRACE_ENTRY_CALENIMP ) // MACRO IN MMP FILE
        _LIT8( KModuleName8, "CalenImp" );
    #elif defined ( TRACE_ENTRY_CALENINTERIMUTILS2 )
        _LIT8( KModuleName8, "CalenInterimUtils2" );
    #elif defined ( TRACE_ENTRY_VERSIT2 )
        _LIT8( KModuleName8, "Versit2" );
    #else // default
        _LIT8( KModuleName8, "CalendarEngines" );
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
    inline void TraceDebugPrint( TRefByValue<const TDesC8> aFmt, ...)
        {
        VA_LIST list;
        VA_START( list, aFmt );
        
        TOverflowTruncate8 overflow;
        TBuf8<KMaxLogLineLength> buf8;
        buf8.AppendFormatList( aFmt, list, &overflow );
        TBuf16<KMaxLogLineLength> buf16( buf8.Length() );
        buf16.Copy( buf8 );
        TRefByValue<const TDesC> tmpFmt( _L("%S") );
        RDebug::Print( tmpFmt, &buf16 );
        }
        
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
        #define TRACE_EXIT_POINT TRACE_FUNC_EXIT_DEBUG_PRINT
    #else
        #define TRACE_ENTRY_POINT
        #define TRACE_EXIT_POINT
    #endif

#else
    #define TRACE_ENTRY_POINT
    #define TRACE_EXIT_POINT

#endif // _DEBUG

#endif // PIM_TRACE_H


// End of File
