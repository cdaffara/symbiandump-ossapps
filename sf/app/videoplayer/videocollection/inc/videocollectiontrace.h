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
* Description:  tracing functionality for video collection 
*
*/

#ifndef VIDEOCOLLETIONTRACE_H
#define VIDEOCOLLETIONTRACE_H

/**
 * Set 1 to enable tracing in urel builds. Default is 0.
 */
#define UREL_TRACE 0

/**
 * Set to 1 to disable tracing. Default is 0.
 */
#define DISABLE_TRACE 0

/**
 * 0 = Tracing into RDebug in all builds. (Default)
 * 1 = Tracing into RDebug in winscw, file in ARM.
 * 2 = Tracing into file in all builds. 
 */
#define TRACE_OUTPUT 0

//-----------------------------------------------------------------------------
// Trace configuration
//-----------------------------------------------------------------------------
//
#if ( (defined(_DEBUG) || UREL_TRACE == 1) && DISABLE_TRACE == 0 )

    // Error trace
    #define ERROR_TRACE
    
    // Function trace
    #define FUNC_TRACE

    // Info trace
    #define INFO_TRACE
    
    // Timestamp trace
    #define TIMESTAMP_TRACE
    
    // Tracing current client process and thread
    #define CLIENT_TRACE

    // Enable tracing into file.
    #if ((TRACE_OUTPUT == 1 && !defined(__WINSCW__)) || TRACE_OUTPUT == 2)
        #define TRACE_INTO_FILE
    #endif

#else // TRACING DISABLED

    #undef ERROR_TRACE
    #undef INFO_TRACE
    #undef TIMESTAMP_TRACE
    #undef FUNC_TRACE
    #undef CLIENT_TRACE
    #undef TRACE_INTO_FILE

#endif // TRACING ENABLED

//-----------------------------------------------------------------------------
// INCLUDES
//-----------------------------------------------------------------------------
//
#include <qstring.h>
#include <e32debug.h> // RDebug

#if defined TRACE_INTO_FILE
    #include <flogger.h> // RFileLogger
#endif

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
//
/**
* Trace prefixes for macros with component name.
*/
#define _PREFIX_TRACE( a ) TPtrC( (const TText*) L"#Fu#Co# " L##a )
#define _PREFIX_TRACE_2( a, b ) TPtrC( (const TText*) L"#Fu#Co# " L##a L##b )
#define _PREFIX_TRACE8( a ) (const char*)( "#Fu#Co# " a )

/**
* Prefix error trace
*/
#define _PREFIX_ERROR( aMsg ) _PREFIX_TRACE( "[ERROR: %d]: " L##aMsg )

/**
* Define needed if TRACE_INTO_FILE macro in use
*/
#ifdef TRACE_INTO_FILE
    _LIT( KTracePath, "Fusion" );
    _LIT( KTraceFile, "videos.log" );
#endif

//-----------------------------------------------------------------------------
// Error trace macros
//-----------------------------------------------------------------------------
//
#ifdef ERROR_TRACE

    /**
    * Error trace definitions.
    */
    #ifdef TRACE_INTO_FILE

        #define ERROR( aErr, aMsg )\
            {\
            if( aErr < KErrNone )\
                {\
                RFileLogger::WriteFormat( KTracePath, KTraceFile,\
                    EFileLoggingModeAppend,\
                    _PREFIX_ERROR( aMsg ), aErr );\
                }\
            }
        #define ERROR_1( aErr, aMsg, aP1 )\
            {\
            if( aErr < KErrNone )\
                {\
                RFileLogger::WriteFormat( KTracePath, KTraceFile,\
                    EFileLoggingModeAppend,\
                    _PREFIX_ERROR( aMsg ), aErr, aP1 );\
                }\
            }
        #define ERROR_2( aErr, aMsg, aP1, aP2 )\
            {\
            if( aErr < KErrNone )\
                {\
                RFileLogger::WriteFormat( KTracePath, KTraceFile,\
                    EFileLoggingModeAppend,\
                    _PREFIX_ERROR( aMsg ), aErr, aP1, aP2 );\
                }\
            }
        #define ERROR_3( aErr, aMsg, aP1, aP2, aP3 )\
            {\
            if( aErr < KErrNone )\
                {\
                RFileLogger::WriteFormat( KTracePath, KTraceFile,\
                    EFileLoggingModeAppend,\
                    _PREFIX_ERROR( aMsg ), aErr, aP1, aP2, aP3 );\
                }\
            }

    #else//TRACE_INTO_FILE not defined

        #define ERROR( aErr, aMsg )\
            {\
            if( aErr < KErrNone )\
                {\
                RDebug::Print( _PREFIX_ERROR( aMsg ), aErr );\
                }\
            }
        #define ERROR_1( aErr, aMsg, aP1 )\
            {\
            if( aErr < KErrNone )\
                {\
                RDebug::Print( _PREFIX_ERROR( aMsg ), aErr, aP1 );\
                }\
            }
        #define ERROR_2( aErr, aMsg, aP1, aP2 )\
            {\
            if( aErr < KErrNone )\
                {\
                RDebug::Print( _PREFIX_ERROR( aMsg ), aErr, aP1, aP2 );\
                }\
            }
        #define ERROR_3( aErr, aMsg, aP1, aP2, aP3 )\
            {\
            if( aErr < KErrNone )\
                {\
                RDebug::Print( _PREFIX_ERROR( aMsg ), aErr, aP1, aP2, aP3 );\
                }\
            }

    #endif//TRACE_INTO_FILE

    #define ERROR_GEN( aMsg ) ERROR( KErrGeneral, aMsg )
    #define ERROR_GEN_1( aMsg, aP1 ) ERROR_1( KErrGeneral, aMsg, aP1 )
    #define ERROR_GEN_2( aMsg, aP1, aP2 ) ERROR_2( KErrGeneral, aMsg, aP1, aP2 )
    #define ERROR_GEN_3( aMsg, aP1, aP2, aP3 ) ERROR_3( KErrGeneral, aMsg, aP1, aP3 )

    #define ERROR_PARAM(_p) _p

    #define TRAPD_ERR( aErr, aStmt ) TRAPD( aErr, aStmt )
    #define TRAP_ERR( aErr, aStmt ) TRAP( aErr, aStmt )

    #define TRAP_AND_LEAVE(_s,_t) \
        { TRAPD(_e,_s); ERROR(_e,_t); User::LeaveIfError(_e); }

#else//ERROR_TRACE not defined

    #define ERROR( aErr, aMsg )
    #define ERROR_1( aErr, aMsg, aP1 )
    #define ERROR_2( aErr, aMsg, aP1, aP2 )
    #define ERROR_3( aErr, aMsg, aP1, aP2, aP3 )
    #define ERROR_GEN( aMsg )
    #define ERROR_GEN_1( aMsg, aP1 )
    #define ERROR_GEN_2( aMsg, aP1, aP2 )
    #define ERROR_GEN_3( aMsg, aP1, aP2, aP3 )

    #define ERROR_PARAM(_p)

    #define TRAPD_ERR( aErr, aStmt ) TRAP_IGNORE( aStmt )
    #define TRAP_ERR( aErr, aStmt )  TRAP_IGNORE( aStmt )

    #define TRAP_AND_LEAVE(_s,_t) { _s; }

#endif//ERROR_TRACE

//-----------------------------------------------------------------------------
// Info trace macros
//-----------------------------------------------------------------------------
//
#ifdef INFO_TRACE

    /**
    * Info log message definitions.
    */
    #ifdef TRACE_INTO_FILE

        #define INFO( aMsg )\
            {\
            RFileLogger::Write( KTracePath, KTraceFile,\
                EFileLoggingModeAppend,\
                _PREFIX_TRACE( aMsg ) );\
            }
        #define INFO_1( aMsg, aP1 )\
            {\
            RFileLogger::WriteFormat( KTracePath, KTraceFile,\
                EFileLoggingModeAppend,\
                _PREFIX_TRACE( aMsg ), aP1 );\
            }
        #define INFO_2( aMsg, aP1, aP2 )\
            {\
            RFileLogger::WriteFormat( KTracePath, KTraceFile,\
                EFileLoggingModeAppend,\
                _PREFIX_TRACE( aMsg ), aP1, aP2 );\
            }
        #define INFO_3( aMsg, aP1, aP2, aP3 )\
            {\
            RFileLogger::WriteFormat( KTracePath, KTraceFile,\
                EFileLoggingModeAppend,\
                _PREFIX_TRACE( aMsg ), aP1, aP2, aP3 );\
            }
        #define INFO_4( aMsg, aP1, aP2, aP3, aP4 )\
            {\
            RFileLogger::WriteFormat( KTracePath, KTraceFile,\
                EFileLoggingModeAppend,\
                _PREFIX_TRACE( aMsg ), aP1, aP2, aP3, aP4 );\
            }
        #define INFO_5( aMsg, aP1, aP2, aP3, aP4, aP5 )\
            {\
            RFileLogger::WriteFormat( KTracePath, KTraceFile,\
                EFileLoggingModeAppend,\
                _PREFIX_TRACE( aMsg ), aP1, aP2, aP3, aP4, aP5 );\
            }
        #define INFOQSTR_1( aMsg, aQStr )\
            {\
            TPtrC _qstringptrc(aQStr.utf16()); \
            RFileLogger::WriteFormat( KTracePath, KTraceFile,\
                EFileLoggingModeAppend,\
                _PREFIX_TRACE( aMsg ), &_qstringptrc );\
            }
        #define INFOQSTR_2( aMsg, aQStr1, aQStr2 )\
            {\
            TPtrC _qstringptrc1(aQStr1.utf16()); \
            TPtrC _qstringptrc2(aQStr2.utf16()); \
            RFileLogger::WriteFormat( KTracePath, KTraceFile,\
                EFileLoggingModeAppend,\
                _PREFIX_TRACE( aMsg ), &_qstringptrc1, &_qstringptrc2 );\
            }            

    #else//TRACE_INTO_FILE not defined

        #define INFO( aMsg )\
            {\
            RDebug::Print( _PREFIX_TRACE( aMsg ) );\
            }
        #define INFO_1( aMsg, aP1 )\
            {\
            RDebug::Print( _PREFIX_TRACE( aMsg ), aP1 );\
            }
        #define INFO_2( aMsg, aP1, aP2 )\
            {\
            RDebug::Print( _PREFIX_TRACE( aMsg ), aP1, aP2 );\
            }
        #define INFO_3( aMsg, aP1, aP2, aP3 )\
            {\
            RDebug::Print( _PREFIX_TRACE( aMsg ), aP1, aP2, aP3 );\
            }
        #define INFO_4( aMsg, aP1, aP2, aP3, aP4 )\
            {\
            RDebug::Print( _PREFIX_TRACE( aMsg ), aP1, aP2, aP3, aP4 );\
            }
        #define INFO_5( aMsg, aP1, aP2, aP3, aP4, aP5 )\
            {\
            RDebug::Print( _PREFIX_TRACE( aMsg ), aP1, aP2, aP3, aP4, aP5 );\
            }

        #define INFOQSTR_1( aMsg, aQStr )\
            {\
            TPtrC _qstringptrc(aQStr.utf16()); \
            RDebug::Print( _PREFIX_TRACE( aMsg ), &_qstringptrc );\
            }

        #define INFOQSTR_2( aMsg, aQStr1, aQStr2 )\
            {\
            TPtrC _qstringptrc1(aQStr1.utf16()); \
            TPtrC _qstringptrc2(aQStr2.utf16()); \
            RDebug::Print( _PREFIX_TRACE( aMsg ), &_qstringptrc1, &_qstringptrc2 );\
            }

    #endif//TRACE_INTO_FILE

    #define INFO_PARAM( aParam ) aParam

#else//INFO_TRACE not defined

    #define INFO( aMsg )
    #define INFO_1( aMsg, aP1 )
    #define INFO_2( aMsg, aP1, aP2 )
    #define INFO_3( aMsg, aP1, aP2, aP3 )
    #define INFO_4( aMsg, aP1, aP2, aP3, aP4 )
    #define INFO_5( aMsg, aP1, aP2, aP3, aP4, aP5 )
    #define INFO_PARAM( aParam )
    #define INFOQSTR_1( aMsg, aP1 );
    #define INFOQSTR_2( aMsg, aP1, aP2 );

#endif//INFO_TRACE

//-----------------------------------------------------------------------------
// Trace current client thread name and process id
//-----------------------------------------------------------------------------
//
#ifdef CLIENT_TRACE

    #define _PREFIX_CLIENT( aMsg ) _PREFIX_TRACE( "[CLIENT: sid [0x%x] thread [%S]] " L##aMsg )

    #ifdef TRACE_INTO_FILE

        #define CLIENT_TRACE_2( aMsg, aP1, aP2 )\
            {\
            RFileLogger::WriteFormat( KTracePath, KTraceFile,\
                EFileLoggingModeAppend, _PREFIX_CLIENT(aMsg), aP1, aP2 );\
            }

    #else//TRACE_INTO_FILE not defined

        #define CLIENT_TRACE_2( aMsg, aP1, aP2 )\
            {\
            RDebug::Print( _PREFIX_CLIENT(aMsg), aP1, aP2 );\
            }

    #endif//TRACE_INTO_FILE

    /**
    * Traces current secure id and thread
    */
    #define CURRENT_CLIENT( aMsg )\
        {\
        TName name( RThread().Name() );\
        TSecureId sid( RThread().SecureId() );\
        CLIENT_TRACE_2( aMsg, sid.iId, &name );\
        }

    /**
    * Traces secure id and thread from RMessage2
    */
    #define CLIENT( aRMessage2 )\
        {\
        RThread thread;\
        TInt err = aRMessage2.Client( thread );\
        if( err == KErrNone )\
            {\
            TName name( thread.Name() );\
            TSecureId sid( thread.SecureId() );\
            CLIENT_TRACE_2( "", sid.iId, &name );\
            }\
        thread.Close();\
        }

    /**
    * Traces secure id and thread from RMessage2
    */
    #define CLIENT_1( aRMessage2, aMsg )\
        {\
        RThread thread;\
        TInt err = aRMessage2.Client( thread );\
        if( err == KErrNone )\
            {\
            TName name( thread.Name() );\
            TSecureId sid( thread.SecureId() );\
            CLIENT_TRACE_2( aMsg, sid.iId, &name );\
            }\
        thread.Close();\
        }

#else

    #define CURRENT_CLIENT( aMsg )
    #define CLIENT( aRMessage2 )
    #define CLIENT_1( aRMessage2, aMsg )

#endif

//-----------------------------------------------------------------------------
// Trace current heap usage
//-----------------------------------------------------------------------------
//
#ifdef HEAP_TRACE

    #define _PREFIX_HEAP( aMsg ) _PREFIX_TRACE( "[HEAP: %d bytes] " L##aMsg )

    #ifdef TRACE_INTO_FILE

        #define HEAP( aMsg )\
            {\
            TInt totalAllocSpace = 0;\
            User::AllocSize( totalAllocSpace );\
            RFileLogger::WriteFormat( KTracePath, KTraceFile,\
                EFileLoggingModeAppend, _PREFIX_HEAP( aMsg ), totalAllocSpace );\
            }
        #define HEAP_1( aMsg, aP1 )\
            {\
            TInt totalAllocSpace = 0;\
            User::AllocSize( totalAllocSpace );\
            RFileLogger::WriteFormat( KTracePath, KTraceFile,\
                EFileLoggingModeAppend, _PREFIX_HEAP( aMsg ), totalAllocSpace, aP1 );\
            }
        #define HEAP_2( aMsg, aP1, aP2 )\
            {\
            TInt totalAllocSpace = 0;\
            User::AllocSize( totalAllocSpace );\
            RFileLogger::WriteFormat( KTracePath, KTraceFile,\
                EFileLoggingModeAppend, _PREFIX_HEAP( aMsg ), totalAllocSpace, aP1, aP2 );\
            }
        #define HEAP_3( aMsg, aP1, aP2, aP3 )\
            {\
            TInt totalAllocSpace = 0;\
            User::AllocSize( totalAllocSpace );\
            RFileLogger::WriteFormat( KTracePath, KTraceFile,\
                EFileLoggingModeAppend, _PREFIX_HEAP( aMsg ), totalAllocSpace, aP1, aP2, aP3 );\
            }
        #define HEAP_4( aMsg, aP1, aP2, aP3, aP4 )\
            {\
            TInt totalAllocSpace = 0;\
            User::AllocSize( totalAllocSpace );\
            RFileLogger::WriteFormat( KTracePath, KTraceFile,\
                EFileLoggingModeAppend, _PREFIX_HEAP( aMsg ), totalAllocSpace, aP1, aP2, aP3, aP4 );\
            }

    #else//TRACE_INTO_FILE not defined

        #define HEAP( aMsg )\
            {\
            TInt totalAllocSpace = 0;\
            User::AllocSize( totalAllocSpace );\
            RDebug::Print( _PREFIX_HEAP( aMsg ), totalAllocSpace );\
            }
        #define HEAP_1( aMsg, aP1 )\
            {\
            TInt totalAllocSpace = 0;\
            User::AllocSize( totalAllocSpace );\
            RDebug::Print( _PREFIX_HEAP( aMsg ), totalAllocSpace, aP1 );\
            }
        #define HEAP_2( aMsg, aP1, aP2 )\
            {\
            TInt totalAllocSpace = 0;\
            User::AllocSize( totalAllocSpace );\
            RDebug::Print( _PREFIX_HEAP( aMsg ), totalAllocSpace, aP1, aP2 );\
            }
        #define HEAP_3( aMsg, aP1, aP2, aP3 )\
            {\
            TInt totalAllocSpace = 0;\
            User::AllocSize( totalAllocSpace );\
            RDebug::Print( _PREFIX_HEAP( aMsg ), totalAllocSpace, aP1, aP2, aP3 );\
            }
        #define HEAP_4( aMsg, aP1, aP2, aP3, aP4 )\
            {\
            TInt totalAllocSpace = 0;\
            User::AllocSize( totalAllocSpace );\
            RDebug::Print( _PREFIX_HEAP( aMsg ), totalAllocSpace, aP1, aP2, aP3, aP4 );\
            }

    #endif//TRACE_INTO_FILE

#else//HEAP_TRACE not defined

    #define HEAP( aMsg )
    #define HEAP_1( aMsg, aP1 )
    #define HEAP_2( aMsg, aP1, aP2 )
    #define HEAP_3( aMsg, aP1, aP2, aP3 )
    #define HEAP_4( aMsg, aP1, aP2, aP3, aP4 )

#endif//HEAP_TRACE


//-----------------------------------------------------------------------------
// Function trace macros
//-----------------------------------------------------------------------------
//
#ifdef FUNC_TRACE

    #include <e32base.h> // TCleanupItem

    /**
    * Function logging definitions.
    */
    #ifdef TRACE_INTO_FILE

        #define FUNC( aMsg, aP1 )\
            {\
            TPtrC8 trace( _S8( aMsg ) );\
            RFileLogger::WriteFormat( KTracePath, KTraceFile,\
                EFileLoggingModeAppend, trace, aP1 );\
            }

        #define FUNC2( aMsg, aP1, aP2 )\
            {\
            TPtrC8 trace( _S8( aMsg ) );\
            RFileLogger::WriteFormat( KTracePath, KTraceFile,\
                EFileLoggingModeAppend, trace, aP1, aP2 );\
            }

        #define FUNC_2( aMsg, aP1, aP2, aP3 )\
            {\
            TPtrC8 trace( _S8( aMsg ) );\
            RFileLogger::WriteFormat( KTracePath, KTraceFile,\
                EFileLoggingModeAppend, trace, aP1, aP2, aP3 );\
            }

    #else//TRACE_INTO_FILE not defined

        #define FUNC( aMsg, aP1 )\
            {\
            RDebug::Printf( aMsg, aP1 );\
            }

        #define FUNC2( aMsg, aP1, aP2 )\
            {\
            RDebug::Printf( aMsg, aP1, aP2 );\
            }
        
        #define FUNC_2( aMsg, aP1, aP2, aP3 )\
            {\
            RDebug::Printf( aMsg, aP1, aP2, aP3 );\
            }

    #endif//TRACE_INTO_FILE

    /**
    * Function trace helper class.
    */
    const TText KFuncNameTerminator = '(';
    const TText KFuncLeaveIndicator = 'L';
    const TInt KFuncCanLeave = 0x1;
    const TInt KFuncLeft = 0x2;
    const TInt KFuncLogClient = 0x4;
    
    class TFuncLog
        {
    public:
        static void Cleanup( TAny* aPtr )
            {
            TFuncLog* self = static_cast< TFuncLog* >( aPtr );
            self->iFlags |= KFuncLeft;
            self->LogLeave();
            }
    
        inline void LogStart()
            {
            if ( iFlags & KFuncLogClient )
                {
                TBuf8<KMaxName> name8;
                name8.Copy( RThread().Name() );
                TSecureId sid( RThread().SecureId() );
                FUNC_2( _PREFIX_TRACE8("%S() -> ENTER(sid [0x%x] thread [%S])"),
                    &iFunc, sid.iId, &name8 );
                }
            else
                {
                if(!iAddr) 
                    {
                    FUNC( _PREFIX_TRACE8("%S() -> ENTER"), &iFunc );
                    }
                else
                    {
                    FUNC2( _PREFIX_TRACE8("%S() [0x%x] -> ENTER"), &iFunc, iAddr );
                    }
                }
            }

        inline void LogLeave()
            {
            if ( iFlags & KFuncLogClient )
                {
                TBuf8<KMaxName> name8;
                name8.Copy( RThread().Name() );
                TSecureId sid( RThread().SecureId() );
                FUNC_2( _PREFIX_TRACE8("%S() - LEAVE(sid [0x%x] thread [%S])"),
                    &iFunc, sid.iId, &name8 );
                }
            else
                {
                if(!iAddr)
                    {
                    FUNC( _PREFIX_TRACE8("%S() - LEAVE"), &iFunc );
                    }
                else
                    {
                    FUNC2( _PREFIX_TRACE8("%S() [0x%x] - LEAVE"), &iFunc, iAddr );
                    }
                }
            }
    
        inline void LogEnd()
            {
            if ( iFlags & KFuncLogClient )
                {
                TBuf8<KMaxName> name8;
                name8.Copy( RThread().Name() );
                TSecureId sid( RThread().SecureId() );
                FUNC_2( _PREFIX_TRACE8("%S() <- OUT(sid [0x%x] thread [%S])"),
                    &iFunc, sid.iId, &name8 );
                }
            else
                {
                if(!iAddr)
                    {
                    FUNC( _PREFIX_TRACE8("%S() <- OUT"), &iFunc );
                    }
                else
                    {
                    FUNC2( _PREFIX_TRACE8("%S() [0x%x] <- OUT"), &iFunc, iAddr );
                    }
                }
            }
    
        // For non leaving and L methods
        inline TFuncLog( const char* aFunc, TUint aFlags ) :
                iAddr(0), iFunc( aFunc ? _S8( aFunc ) : _S8("") ),
                iFlags( aFlags ),
                iCleanupItem( Cleanup, this ),
                iLCPtr( NULL )
            {
            TInt pos( iFunc.Locate( KFuncNameTerminator ) );
            if( pos >= 0 && pos < iFunc.Length() )
                {
                iFunc.Set( iFunc.Left( pos ) );
                TInt len( iFunc.Length() );
                if( len > 0 && iFunc[ len - 1 ] == KFuncLeaveIndicator )
                    {
                    iFlags |= KFuncCanLeave;
                    CleanupStack::PushL( iCleanupItem ); // Ignore warnings
                    }
                }
            LogStart();
            }

        // For non leaving and L methods
        inline TFuncLog( TUint aAddr, const char* aFunc, TUint aFlags ) :
                iAddr(aAddr), iFunc( aFunc ? _S8( aFunc ) : _S8("") ),
                iFlags( aFlags ),
                iCleanupItem( Cleanup, this ),
                iLCPtr( NULL )
            {
            TInt pos( iFunc.Locate( KFuncNameTerminator ) );
            if( pos >= 0 && pos < iFunc.Length() )
                {
                iFunc.Set( iFunc.Left( pos ) );
                TInt len( iFunc.Length() );
                if( len > 0 && iFunc[ len - 1 ] == KFuncLeaveIndicator )
                    {
                    iFlags |= KFuncCanLeave;
                    CleanupStack::PushL( iCleanupItem ); // Ignore warnings
                    }
                }
            LogStart();
            }
    
        // For LC methods only
        inline TFuncLog( const char* aFunc, TUint aFlags, TAny** aLCPtr ) :
                iAddr(0), iFunc( aFunc ? _S8( aFunc ) : _S8("") ),
                iFlags( aFlags ),
                iCleanupItem( Cleanup, this ),
                iLCPtr( NULL )
            {
            TInt pos( iFunc.Locate( KFuncNameTerminator ) );
            if( pos >= 0 && pos < iFunc.Length() )
                {
                iFunc.Set( iFunc.Left( pos ) );
                iFlags |= KFuncCanLeave;
                iLCPtr = aLCPtr;
                CleanupStack::PushL( iCleanupItem ); // Ignore warnings
                }
            LogStart();
            }
    
        inline ~TFuncLog()
            {
            if ( !( iFlags & KFuncLeft ) )
                {
                if ( iFlags & KFuncCanLeave )
                    {
                    if ( iLCPtr && *iLCPtr)
                        {
                        CleanupStack::Pop(); // Pop LC ptr first
                        CleanupStack::Pop( this ); // Pop the cleanup item
                        CleanupStack::PushL( *iLCPtr ); // Push LC ptr back
                        }
                    else
                        {
                        CleanupStack::Pop( this ); // Pop the cleanup item
                        }
                    }
                LogEnd(); // Normally finished
                }
            }
    
    private: // Data
        TPtrC8 iFunc;
        TUint iFlags;
        TCleanupItem iCleanupItem;
        TAny** iLCPtr;
        TUint iAddr;
        };

    /*
    * Use with non leaving and L methods
    *
    * CMyClass* CMyClass::DoL()
    *   {
    *   FUNC_LOG;
    *   CMyClass* ret = new (ELeave) CMyClass();
    *   ...
    *   return ret;
    *   }
    *
    * CMyClass* CMyClass::Do()
    *   {
    *   FUNC_LOG;
    *   return iMyClass;
    *   }
    *
    */
    #define FUNC_LOG TFuncLog _fl( __PRETTY_FUNCTION__, 0 );
    
    /**
     * Use this to log memory address. 
     */
    #define FUNC_LOG_ADDR(P1) TFuncLog _fl2( (int)P1, __PRETTY_FUNCTION__, 0 );

    /*
    * Use with LC methods
    *
    * CMyClass* CMyClass::DoLC()
    *   {
    *   CMyClass* ret = NULL;
    *   FUNC_LOG_LC( ret );
    *   ...
    *   return ret;
    *   }
    */
    #define FUNC_LOG_LC( aPtr ) TFuncLog _fl( __PRETTY_FUNCTION__, 0, (TAny**)&aPtr );

#ifdef CLIENT_TRACE

    /*
    * Used like FUNC_LOG. Prints client info useful for client side executed API methods.
    */
    #define FUNC_LOG_WITH_CLIENT TFuncLog _fl( __PRETTY_FUNCTION__, KFuncLogClient );

    /*
    * Used like FUNC_LOG_LC. Prints client info useful for client side executed API methods.
    */ 
    #define FUNC_LOG_WITH_CLIENT_LC( aPtr )\
        TFuncLog _fl( __PRETTY_FUNCTION__, KFuncLogClient, (TAny**)&aPtr );

#else //CLIENT_TRACE not defined

    #define FUNC_LOG_WITH_CLIENT FUNC_LOG
    #define FUNC_LOG_WITH_CLIENT_LC( aPtr ) FUNC_LOG_LC( aPtr )

#endif //CLIENT_TRACE

#else//FUNC_TRACE not defined

    #define FUNC_LOG
    #define FUNC_LOG_ADDR(P1)
    #define FUNC_LOG_LC( ptr )
    #define FUNC_LOG_WITH_CLIENT
    #define FUNC_LOG_WITH_CLIENT_LC( ptr )

#endif//FUNC_TRACE

//-----------------------------------------------------------------------------
// Timestamp trace macros
//-----------------------------------------------------------------------------
//
#ifdef TIMESTAMP_TRACE

    #define PREFIX_TIMESTAMP( aCaption )\
        _PREFIX_TRACE_2("[TIMESTAMP] (%d:%02d:%02d.%06d us) ",aCaption)
    #define CURRENT_TIME( aDt ) TDateTime aDt; { TTime t; t.HomeTime(); aDt = t.DateTime(); }
    #define EXTRACT_TIME( aDt ) aDt.Hour(), aDt.Minute(), aDt.Second(), aDt.MicroSecond()

    #ifdef TRACE_INTO_FILE

        #define TIMESTAMP( aCaption )\
            {\
            CURRENT_TIME( dt );\
            RFileLogger::WriteFormat( KTracePath, KTraceFile,\
                EFileLoggingModeAppend,\
                PREFIX_TIMESTAMP(aCaption),EXTRACT_TIME( dt ) );\
            }

        #define TIMESTAMP_1( aCaption, aP1 )\
            {\
            CURRENT_TIME( dt );\
            RFileLogger::WriteFormat( KTracePath, KTraceFile,\
                EFileLoggingModeAppend,\
                PREFIX_TIMESTAMP(aCaption),EXTRACT_TIME( dt ),aP1 );\
            }

        #define TIMESTAMP_2( aCaption, aP1, aP2 )\
            {\
            CURRENT_TIME( dt );\
            RFileLogger::WriteFormat( KTracePath, KTraceFile,\
                 EFileLoggingModeAppend,\
                PREFIX_TIMESTAMP(aCaption),EXTRACT_TIME( dt ),aP1,aP2 );\
            }

        #define TIMESTAMP_3( aCaption, aP1, aP2, aP3 )\
            {\
            CURRENT_TIME( dt );\
            RFileLogger::WriteFormat( KTracePath, KTraceFile,\
                EFileLoggingModeAppend,\
                PREFIX_TIMESTAMP(aCaption),EXTRACT_TIME( dt ),aP1,aP2,aP3 );\
            }

    #else//TRACE_INTO_FILE not defined

        #define TIMESTAMP( aCaption )\
            {\
            CURRENT_TIME( dt );\
            RDebug::Print( PREFIX_TIMESTAMP(aCaption),EXTRACT_TIME( dt ) );\
            }

        #define TIMESTAMP_1( aCaption, aP1 )\
            {\
            CURRENT_TIME( dt );\
            RDebug::Print( PREFIX_TIMESTAMP(aCaption),EXTRACT_TIME( dt ),aP1 );\
            }

        #define TIMESTAMP_2( aCaption, aP1, aP2 )\
            {\
            CURRENT_TIME( dt );\
            RDebug::Print( PREFIX_TIMESTAMP(aCaption),EXTRACT_TIME( dt ),aP1,aP2 );\
            }

        #define TIMESTAMP_3( aCaption, aP1, aP2, aP3 )\
            {\
            CURRENT_TIME( dt );\
            RDebug::Print( PREFIX_TIMESTAMP(aCaption),EXTRACT_TIME( dt ),aP1,aP2,aP3 );\
            }

    #endif//TRACE_INTO_FILE

#else//TIMESTAMP_TRACE not defined

    #define TIMESTAMP( aCaption )
    #define TIMESTAMP_1( aCaption, aP1 )
    #define TIMESTAMP_2( aCaption, aP1, aP2 )
    #define TIMESTAMP_3( aCaption, aP1, aP2, aP3 )

#endif//TIMESTAMP_TRACE

//-----------------------------------------------------------------------------
// Trace assert macros
//-----------------------------------------------------------------------------
//
#if ( (defined(_DEBUG) || UREL_TRACE == 1) && DISABLE_TRACE == 0 )

    #ifdef TRACE_INTO_FILE

        #define PANIC_TRACE_2( aMsg, aP1, aP2 )\
            {\
            TPtrC8 trace( _S8( aMsg ) );\
            RFileLogger::WriteFormat( KTracePath, KTraceFile,\
                EFileLoggingModeAppend, trace, aP1, aP2 );\
            }

    #else//TRACE_INTO_FILE not defined

        #define PANIC_TRACE_2( aMsg, aP1, aP2 )\
            {\
            RDebug::Printf( aMsg, aP1, aP2 );\
            }

    #endif //TRACE_INTO_FILE not define

    static void Panic( const TDesC8& aFileName, const TInt aLineNum )
        {
        PANIC_TRACE_2( "Assertion failed in file=%S, line=%d", &aFileName, aLineNum );
        User::Invariant();
        }

    #define ASSERT_ALWAYS_TRACE Panic( _L8(__FILE__), __LINE__ );
    #define ASSERT_TRACE( _s ) if ( !( _s ) ) { ASSERT_ALWAYS_TRACE; }

#else // TRACING DISABLED

    #define ASSERT_ALWAYS_TRACE
    #define ASSERT_TRACE( _s )

#endif // TRACING ENABLED


#endif // VIDEOCOLLETIONTRACE_H
