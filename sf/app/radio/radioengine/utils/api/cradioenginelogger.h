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

#ifndef C_RADIOENGINELOGGER_H
#define C_RADIOENGINELOGGER_H

// System includes
#include <e32base.h>

// User includes
#include "mradioenginelogger.h"

/**
 * Main logging macro.
 * When the macro is defined, logging is on
 */
#ifdef _DEBUG
    #define LOGGING_ENABLED
#endif

#define LOGGING_LEVEL 1

/**
 * Logging level macro definitions
 * Level 1 - No level macro needed. The trace is shown when logging is on.
 * Level 2 - Wrap log line with LEVEL2 macro. Traces of level 1 and 2 are shown.
 * Level 3 - Wrap log line with LEVEL3 macro. Traces of level 1, 2 and 3 are shown.
 */
#if LOGGING_LEVEL == 2
#   define LEVEL2( expr ) expr
#   define LEVEL3( expr )
#elif LOGGING_LEVEL == 3
#   define LEVEL2( expr ) expr
#   define LEVEL3( expr ) expr
#else
#   define LEVEL2( expr )
#   define LEVEL3( expr )
#endif

#ifdef LOGGING_ENABLED

/**
 * Debug var macros for all logging levels
 */
#if LOGGING_LEVEL == 2
#   define DEBUGVAR( var ) var
#   define DEBUGVAR2( var ) var
#   define DEBUGVAR3( var )
#elif LOGGING_LEVEL == 3
#   define DEBUGVAR( var ) var
#   define DEBUGVAR2( var ) var
#   define DEBUGVAR3( var ) var
#else
#   define DEBUGVAR( var ) var
#   define DEBUGVAR2( var )
#   define DEBUGVAR3( var )
#endif

/**
 * Log file path.
 * If the path points to memory card and the card is not present or is read only,
 * the log is written to C: drive.
 */
_LIT( KLogFile, "c:\\logs\\radio\\radioenginelog.txt" );

/**
 * Logging buffer length. Should be big enough to hold one line of text
 */
const TInt KLogBufferLength = 512;


// ===========================================================================
// ===========================================================================
//
// BEGIN LOGGING MACROS. Use these to write log prints
//
// ===========================================================================
// ===========================================================================

// All macros except LOG_METHOD and LOG_METHOD_RET are enclosed in do { }while( 0 );
// statements to make them form a complete code block that can be used anywhere.

/**
 * Function entry, exit and leave logging.
 *
 *  @code
 *   LOG_METHOD( "CSomeClass::SomeFunctionL" );
 *  @endcode
 */
#define LOG_METHOD( func ) TRadioMethodLogger ___methodLogger( _S( func ), _S( "" ) )

/**
 * Function entry, exit and leave logging.
 * Gets the function name automatically
 *
 *  @code
 *   LOG_METHOD_AUTO;
 *  @endcode
 */
// Substract 1 from MaxLength() because PtrZ() adds the zero terminator
#define LOG_METHOD_AUTO \
    TPtrC8 __ptr8( (const TUint8*)__PRETTY_FUNCTION__ ); \
    TBuf<150> __buf; \
    __buf.Copy( __ptr8.Left( __buf.MaxLength() - 1 ) ); \
    TRadioMethodLogger ___methodLogger( __buf.PtrZ(), _S( "" ) )

/**
 * Function entry, exit and leave logging. Logs also returned value
 * NOTE! At the moment return value is only logged in emulator builds.
 *
 *  @code
 *   // Function returns an integer value
 *   LOG_METHOD_RET( "CSomeClass::SomeFunctionL", "Error: %d" );
 *
 *   // Function returns an HBufC pointer
 *   LOG_METHOD_RET( "CSomeClass::SomeFunctionL", "HBufC: %S" );
 *  @endcode
 */
#define LOG_METHOD_RET( func,fmt ) TRadioMethodLogger ___methodLogger( _S( func ), _S( fmt ) )

/**
 * Function entry, exit and leave logging. Logs also returned value
 * Gets the function name automatically
 * NOTE! At the moment return value is only logged in emulator builds.
 *
 *  @code
 *   // Function returns an integer value
 *   LOG_METHOD_RET( "CSomeClass::SomeFunctionL", "Error: %d" );
 *
 *   // Function returns an HBufC pointer
 *   LOG_METHOD_RET( "CSomeClass::SomeFunctionL", "HBufC: %S" );
 *  @endcode
 */
// Substract 1 from MaxLength() because PtrZ() adds the zero terminator
#define LOG_METHOD_AUTO_RET( fmt ) \
    TPtrC8 __ptr8( (const TUint8*)__PRETTY_FUNCTION__ ); \
    TBuf<150> __buf; \
    __buf.Copy( __ptr8.Left( __buf.MaxLength() - 1 ) ); \
    TRadioMethodLogger ___methodLogger( __buf.PtrZ(), _S( fmt ) )

/**
 * Simple assert macro that works like normal asserts. The expression ( expr ) is
 * expected to be true in which case nothing is done. If the expression is false the
 * statement ( stmt ) is executed.
 *
 * Note! As with normal asserts, do not put a function call as the expression because
 * it will not be executed when logging is off!
 *
 *  @code
 *   // Log error code if it is not KErrNone
 *   TRAPD( err, SomeFunctionL() );
 *   MTV_ASSERT( !err, LOG_FORMAT( "Operation failed. error: %d", err ) );
 *  @endcode
 */
#define LOG_ASSERT( expr, stmt ) do { if ( !( expr )) { stmt; } }while( 0 )

/**
 * Writes a formatted string to log. Accepts variable number of parameters
 *
 *  @code
 *   LOG_FORMAT( "Integer: %d", 1 );
 *   LOG_FORMAT( "Integer: %d Desc %S", 1, &desc );
 *   LOG_FORMAT( "Integer: %d Desc %S hex 0x%X", 1, &desc, 0xC0FFEE );
 *  @endcode
 */
#define LOG_FORMAT( fmt,args...) do{MRadioEngineLogger::Logger()->AddIndent( KMarkerEngine()).AddFormat( _L( fmt ),args ).Commit();}while( 0 )

/**
 * Writes current time to log. Example: Time: 13.9.2007 13:23:25.383750
 */
#define LOG_CURRENT_TIME() do{MRadioEngineLogger::Logger()->AddIndent( KMarkerEngine()).AddTimestamp().Commit();}while( 0 )

/**
 * Writes a time to log. Accepts a TTime as parameter
 *
 *  @code
 *   TTime now;
 *   now.HomeTime();
 *   LOG_TIME( now );
 *  @endcode
 */
#define LOG_TIME( x ) do{MRadioEngineLogger::Logger()->AddIndent( KMarkerEngine()).AddTimestamp( x ).Commit();}while( 0 )

/**
 * Writes a time to log. Accepts a string and a TTime as parameter
 *
 *  @code
 *   TTime now;
 *   now.HomeTime();
 *   LOG_TIME2( "Current time", now );
 *  @endcode
 */
#define LOG_TIME2( desc, time ) do{MRadioEngineLogger::Logger()->AddIndent( KMarkerEngine()).Add( desc ).Add( ": " ).Add( time ).Commit();}while( 0 )

/**
 * Clear the log contents
 */
#define LOG_CLEAR() do{MRadioEngineLogger::Logger()->ClearLog();}while( 0 )

/**
 * Construct a log line one item at a time. No newline at end
 */
//#define LOG( x ) do{CRadioEngineLogger::Logger()->Add( x );}while( 0 )

/**
 * Write a newline to log
 */
#define LOG_NEWLINE() do{MRadioEngineLogger::Logger()->Commit();}while( 0 )

/**
 * Macro that behaves like User::LeaveIfError except it logs the leave
 * The line "TInt __error_code = expr;" makes sure the expression is evaluated only once
 */
#define LEAVEIFERROR( expr )                                                \
    do {                                                                    \
        TInt __err = expr;                                                  \
        if ( __err )                                                        \
            {                                                               \
            _LIT( KExpression, #expr );                                     \
            LOG_FORMAT( "%S leaving with code %d!", &KExpression, __err );  \
            User::Leave( __err );                                           \
            }                                                               \
        } while( 0 )

/**
 * Macro that behaves like User::Leave except it logs the leave
 * The line "TInt __error_code = expr;" makes sure the expression is evaluated only once
 */
#define LEAVE( expr )                                   \
    do {                                                \
        TInt __err = expr;                              \
        LOG_FORMAT( "Leaving with code %d!", __err );   \
        User::Leave( __err );                           \
        } while( 0 )

/**
 * Macro that behaves like TRAP except it logs the possible error
 */
#define TRAP_AND_LOG_ERR( err, expr )                               \
    TRAP( err, expr );                                              \
    if ( err )                                                      \
        {                                                           \
        _LIT( KExpression, #expr );                                 \
        LOG_FORMAT( "%S failed with err %d", &KExpression, err );   \
        }                                                           \

/**
 * Macro that behaves like TRAPD except it logs the possible error
 */
#define TRAPD_AND_LOG_ERR( err, expr )  \
    TInt err = 0;                       \
    TRAP_AND_LOG_ERR( err, expr )


/**
 * Macro that behaves like TRAP_IGNORE except it logs the possible error
 */
#define TRAP_IGNORE_AND_LOG_ERR( expr )     \
    do {                                    \
        TRAPD_AND_LOG_ERR( __err, expr );     \
        } while ( 0 )

/**
 * Write a single log line.
 */
#define LOGGER_WRITE( x ) do{MRadioEngineLogger::Logger()->AddIndent( KMarkerEngine ).Add( _L8( x ) ).Commit();}while( 0 )
#define LOG( x ) do{MRadioEngineLogger::Logger()->AddIndent( KMarkerEngine ).Add( x ).Commit();}while( 0 )
//#define LOG2( x1, x2 ) do{CRadioEngineLogger::Logger()->AddIndent().Add( x1 ).Add( x2 ).Commit();}while( 0 )
//#define LOG3( x1, x2, x3 ) do{CRadioEngineLogger::Logger()->AddIndent().Add( x1 ).Add( x2 ).Add( x3 ).Commit();}while( 0 )
//#define LOG4( x1, x2, x3, x4 ) do{CRadioEngineLogger::Logger()->AddIndent().Add( x1 ).Add( x2 ).Add( x3 ).Add( x4 ).Commit();}while( 0 )
//#define LOG5( x1, x2, x3, x4, x5 ) do{CRadioEngineLogger::Logger()->AddIndent().Add( x1 ).Add( x2 ).Add( x3 ).Add( x4 ).Add( x5 ).Commit();}while( 0 )

// ===========================================================================
// ===========================================================================
//
// END LOGGING MACROS. Do not use anything below this line directly!
//
// ===========================================================================
// ===========================================================================


#include <f32file.h>
#include <e32debug.h>

class TRadioMethodLogger;

const TInt KTimestampLength = 40;
typedef TBuf8<KTimestampLength> TTimestamp;

/**
 * Logger for writing log to a file and RDebug
 *
 *  @lib mtvcommon.lib
 *  @since Live TV UI v1.0
 */
NONSHARABLE_CLASS( CRadioEngineLogger ) : public CBase
                                        , public MRadioEngineLogger
                                        , public TDes16Overflow
    {
    friend class CRadioEngineTls;
public:

// from base class MRadioEngineLogger

    MRadioEngineLogger& ClearLog();
    MRadioEngineLogger& Add( const TDesC8& aMsg );
    MRadioEngineLogger& Add( const TDesC& aMsg );
    MRadioEngineLogger& Add( TInt aInt );
    MRadioEngineLogger& Add( const TReal& aReal );
    MRadioEngineLogger& Add( const char* aText );
    MRadioEngineLogger& Add( const TAny* aPtr );
    MRadioEngineLogger& Add( const TTime& aTime );
    MRadioEngineLogger& AddTimestamp();
    MRadioEngineLogger& AddFormat( TRefByValue<const TDesC> aFmt, ... );
    MRadioEngineLogger& AddIndent( const TDesC& aMarker );
    MRadioEngineLogger& AddIndentClear( const TDesC& aMarker );
    MRadioEngineLogger& IncIndent();
    MRadioEngineLogger& DecIndent();
    MRadioEngineLogger& Commit( TBool aNewLine = ETrue );

// from base class TDes16Overflow

    /**
     * Handles the overflow from AppendFormatList()
     *
     * @since Live TV UI v1.0
     * @param aDes The 16-bit modifiable descriptor whose overflow results in the
     *             call to this overflow handler.
     */
    void Overflow( TDes16 &aDes );

private:

    static CRadioEngineLogger* NewL( RFs& aFs );

    ~CRadioEngineLogger();

    CRadioEngineLogger( RFs& aFs );

    void ConstructL();

    /**
     * Returns the amount of characters that still fit in the buffer
     *
     * @since Live TV UI v1.0
     * @return Amount of space available
     */
    TInt Available() const;

    /**
     * Templated function to add either 8-bit or 16-bit descriptor to buffer
     *
     * @since Live TV UI v1.0
     * @param aDesc Descriptor to add
     */
    template<class PTR, class DESC>
    void AddDesC( const DESC& aDesc );

    /**
     * Parses the timestamp from the given TTime
     *
     * @since Live TV UI v1.0
     * @param aTime Time to parse
     * @param aTimestamp On return, the parsed timestamp
     */
    void ParseTimestamp( const TTime& aTime, TTimestamp& aTimestamp );

private: // data

    /**
     * Reference to file server session.
     */
    RFs&    iFs;

    /**
     * File handle
     */
    RFile   iFile;

    /**
     * Name of the log file.
     * Own.
     */
    HBufC*  iFileName;

    /**
     * Logging buffer. holds one log line at a time
     */
    TBuf8<KLogBufferLength> iBuf8;

    /**
     * Indentation
     */
    TInt    iIndent;

    /**
     * Flag to indicate whether or not timestamp has already been written to the beginning of line
     */
    TBool   iTimeStampWritten;

    };

/**
 * Helper class for method entry, exit and leave logging.
 * Used as automatic variable declared by LOG_METHOD() macro.
 */
NONSHARABLE_CLASS( TRadioMethodLogger )
    {
public:

    /**
     * Constructor. Writes method entry log.
     */
    IMPORT_C TRadioMethodLogger( const TText* aFunc, const TText* aRetFormat );

    /**
     * Destructor. Writes method exit log.
     */
    IMPORT_C ~TRadioMethodLogger();

private: // data

    /**
     * Pointer descriptor to function signature that is to be logged.
     */
    TPtrC iFunc;

    /**
     * Formatting string used to print the function return value
     */
    TPtrC iRetFormat;

    };

#else // LOGGING_ENABLED

// Empty macro definitions for non logging build.

#define LOG_METHOD_AUTO
#define LOG_METHOD_AUTO_RET( fmt )

#define LOG_METHOD( func )
#define LOG_METHOD_RET( func,fmt )
#define LOG_FORMAT( fmt,args...)
#define LOG_ASSERT( expr, stmt )
#define DEBUGVAR( var )
#define DEBUGVAR2( var )
#define DEBUGVAR3( var )

#define LOG_CURRENT_TIME()
#define LOG_TIME( x )
#define LOG_TIME2( desc, time )

#define LOG_CLEAR()
#define LOG( x )
#define LOG_NEWLINE()
#define LOG1( x )
#define LOG2( x1, x2 )
#define LOG3( x1, x2, x3 )
#define LOG4( x1, x2, x3, x4 )
#define LOG5( x1, x2, x3, x4, x5 )

// Default implementations for leave macros.
// Note! These can NOT be empty macros because the errors would not be checked when logging is off
#define LEAVEIFERROR( expr )    User::LeaveIfError( expr )
#define LEAVE( expr )           User::Leave( expr )

// Default implementation for trap macros
// Note! As with leave macros, these can not be empty
#define TRAP_AND_LOG_ERR( err,stmt )      TRAP( err, stmt )
#define TRAPD_AND_LOG_ERR( err,stmt )     TRAPD( err, stmt )
#define TRAP_IGNORE_AND_LOG_ERR( stmt )   TRAP_IGNORE( stmt )

#define LOGGER_WRITE( x )

// ===========================================================================
// Dummy classes to keep the exports unchanged when logging is turned off
// These can not be used.
// ===========================================================================
class RFs;
NONSHARABLE_CLASS( CRadioEngineLogger ) : public CBase
    {
    friend class CRadioEngineImp;
private:
    static void InitializeL( RFs& aFs );
    static void Release();
    };

NONSHARABLE_CLASS( TRadioMethodLogger )
    {
public:
    IMPORT_C TRadioMethodLogger( const TText* /*aFunc*/, const TText* /*aRetFormat*/ );
    IMPORT_C ~TRadioMethodLogger();
    };

#endif // LOGGING_ENABLED

#endif // C_RADIOENGINELOGGER_H
