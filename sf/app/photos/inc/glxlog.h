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



#ifndef GLXLOG_H
#define GLXLOG_H

// KH 3-August-2007, from now on only log on debug builds because of performance
#ifdef _DEBUG
#define GLX_LOGGING_ENABLED
#endif

#ifndef GLX_LOGGING_ENABLED

    // empty declarations to clean up code
    #define GLX_LOG_ENTRY_EXIT( s )
    #define GLX_LOG_ENTRY_EXIT_LEAVE_L( s )
    #define GLX_LOG_INFO( s )
    #define GLX_LOG_INFO1( s, i )
    #define GLX_LOG_INFO2( s, i, j )
    #define GLX_LOG_INFO3( s, i, j, k )
    #define GLX_LOG_INFO4( s, i, j, k, l )
    #define GLX_LOG_INFO5( s, i, j, k, l, m )
    #define GLX_LOG_WARNING( s )
    #define GLX_LOG_WARNING1( s, i )
    #define GLX_LOG_WARNING2( s, i, j )
    #define GLX_LOG_WARNING3( s, i, j, k )
    #define GLX_LOG_ERROR( s )
    #define GLX_LOG_ERROR1( s, i )
    #define GLX_LOG_ERROR2( s, i, j )
    #define GLX_LOG_ERROR3( s, i, j, k )
    #define GLX_FUNC( s )
    #define GLX_FUNCL( s )
    #define GLX_DEBUG1( s )
    #define GLX_DEBUG2( s, i )
    #define GLX_DEBUG3( s, i, j )
    #define GLX_DEBUG4( s, i, j, k )
    #define GLX_DEBUG5( s, i, j, k, l )

#else

// only include headers if logging is enabled
#include <e32std.h>
#include <e32base.h>
#include <glxlogger.h>


    /**
     * Helper macro for defining debug string with filename
     * Note! this is internal so dont use outside this header
     */
/*    #ifdef __arm__
        // use the module name instead as its the filename without path
        #define GLX_DEBUG_STR( cat, str ) __MODULE__ "(%d) : " ##cat " : " ##str
    #else
        // put full file paths off for __FILE__ macro to make the log entries shorter
        #pragma fullpath_file off*/
        #define GLX_DEBUG_STR( cat, str ) __FILE__ "(%d) : " cat " : " str
//    #endif

    /**
     * Helper macro for defining debug string with filename and linenumber
     * @param category the category string for the log entry
     * @param string the string for the log entry
     * Note! this is internal so dont use outside this header
     */
    #define GLX_DEBUG_LINE( category, string ) \
        TPtrC8((const TText8*) GLX_DEBUG_STR( category, string ) ), __LINE__

    /**
     * Output to glx logger
     * Note! this is internal so dont use outside this header
     */
    #define GLX_DEBUG_PRINT GlxLogger::WriteFormat

    // LOGGING MACROS
    /**
     * Logs the entry and exit point of the function
     * Does not log the filename nor linenumber as inlining does not work
     * if you want to log the filename and linenumber, 
     * use GLX_LOG_INFO macros instead
     * Example usage:
     * 	GLX_LOG_ENTRY_EXIT( "CFoo::Bar()" );
     * @param a string to be added to the log
     */
    #define GLX_LOG_ENTRY_EXIT( s ) TGlxFunctionTrace _tracer( _L8( s ) )

    /**
     * Logs the entry, exit and possible leave point of the function.
     * Note! cannot be used in a non leaving function or inside a LC function 
     * as this macro uses the cleanup stack
     * Does not log the filename nor linenumber as inlining does not work
     * if you want to log the filename and linenumber, 
     * use GLX_LOG_INFO macros instead
     * Example usage:
     * 	GLX_LOG_ENTRY_EXIT_LEAVE_L( "CFoo::Bar()" );
     * @param a string to be added to the log
     * @deprecated use GLX_LOG_ENTRY_EXIT instead as it also detects leave and does not require cleanupstack to work
     */
    #define GLX_LOG_ENTRY_EXIT_LEAVE_L( s ) \
        TGlxFunctionTraceWithLeaveDetection _tracer( _L8( s ) ); \
        CleanupClosePushL( _tracer )

    /**
     * Logs a single line of information and the filename and linenumber
     * Example usage:
     * 	GLX_LOG_INFO( "CFoo::Bar()" );
     * @param s, the string to be inserted to the log, plain compile time string, not a descriptor
     */
    #define GLX_LOG_INFO( s ) \
        GLX_DEBUG_PRINT( GLX_DEBUG_LINE( "info", s ) )

    /**
     * Logs a single line of information with one parameter 
     * and the filename and linenumber
      * NOTE! logging is done in 8 bit strings so you cannot format 16 bit descriptor
      * values.
     * For the parameter string, use %d for int, %f for float and %S for address of descriptor
     * Example usage:
     * 	GLX_LOG_INFO1( "CFoo::Bar(%d)", aInt );
     * @param s, the string to be inserted to the log, plain compile time string, not a descriptor
     * @param i, value to be inserted to the string, can be of any type
     */
    #define GLX_LOG_INFO1( s, i ) \
        GLX_DEBUG_PRINT( GLX_DEBUG_LINE( "info", s ), i )

    /**
     * Logs a single line of information with two parameters
     * along with the filename and linenumber
     * For the parameter string, use %d for int, %f for float and %S for address of descriptor
      * NOTE! logging is done in 8 bit strings so you cannot format 16 bit descriptor
      * values.
     * Example usage:
     * 	GLX_LOG_INFO2( "CFoo::Bar(%d,%f)", aInt, aFloat );
     * @param s, the string to be inserted to the log, plain compile time string, not a descriptor
     * @param i, value to be inserted to the string, can be of any type
     * @param j, value to be inserted to the string, can be of any type
     */
    #define GLX_LOG_INFO2( s, i, j ) \
        GLX_DEBUG_PRINT( GLX_DEBUG_LINE( "info", s ), i, j )

    /**
     * Logs a single line of information with three parameters
     * along with the filename and linenumber
     * For the parameter string, use %d for int, %f for float and %S for address of descriptor
      * NOTE! logging is done in 8 bit strings so you cannot format 16 bit descriptor
      * values.
     * Example usage:
     * 	GLX_LOG_INFO3( "CFoo::Bar(%d,%S,%f)", aInt, &aDesC, aFloat );
     * @param s, the string to be inserted to the log, plain compile time string, not a descriptor
     * @param i, value to be inserted to the string, can be of any type
     * @param j, value to be inserted to the string, can be of any type
     * @param k, value to be inserted to the string, can be of any type
     */
    #define GLX_LOG_INFO3( s, i, j, k ) \
        GLX_DEBUG_PRINT( GLX_DEBUG_LINE( "info", s ), i, j, k )

    /**
     * Logs a single line of information with four parameters
     * along with the filename and linenumber
     * For the parameter string, use %d for int, %f for float and %S for address of descriptor
      * NOTE! logging is done in 8 bit strings so you cannot format 16 bit descriptor
      * values.
     * Example usage:
     * 	GLX_LOG_INFO4( "CFoo::Bar(%d,%f,%S,%s)", aInt, aFloat, "string" );
     * @param s, the string to be inserted to the log, plain compile time string, not a descriptor
     * @param i, value to be inserted to the string, can be of any type
     * @param j, value to be inserted to the string, can be of any type
     * @param k, value to be inserted to the string, can be of any type
     * @param l, value to be inserted to the string, can be of any type
     */
    #define GLX_LOG_INFO4( s, i, j, k, l ) \
        GLX_DEBUG_PRINT( GLX_DEBUG_LINE( "info", s ), i, j, k, l )

    /**
     * Logs a single line of information with five parameters
     * along with the filename and linenumber
     * For the parameter string, use %d for int, %f for float and %S for address of descriptor
      * NOTE! logging is done in 8 bit strings so you cannot format 16 bit descriptor
      * values.
     * Example usage:
     * 	GLX_LOG_INFO5( 
     *       "CFoo::Bar(%.1f,%.2f,%.3f,%.4f,%.5f)", aFloat1, aFloat2, aFloat3, aFloat4, aFloat5 );
     * @param s, the string to be inserted to the log, plain compile time string, not a descriptor
     * @param i, value to be inserted to the string, can be of any type
     * @param j, value to be inserted to the string, can be of any type
     * @param k, value to be inserted to the string, can be of any type
     * @param l, value to be inserted to the string, can be of any type
     * @param m, value to be inserted to the string, can be of any type
     */
    #define GLX_LOG_INFO5( s, i, j, k, l, m ) \
        GLX_DEBUG_PRINT( GLX_DEBUG_LINE( "info", s ), i, j, k, l, m )

    /**
     * Logs a single line of warning and the filename and linenumber
     * Example usage:
     * 	GLX_LOG_WARNING( "Incorrect state, resetting state" );
     * @param s, the string to be inserted to the log, plain compile time string, not a descriptor
     */
    #define GLX_LOG_WARNING( s ) \
        GLX_DEBUG_PRINT( GLX_DEBUG_LINE( "warning", s ) )

    /**
     * Logs a single line of warning with one parameter 
     * and the filename and linenumber
     * For the parameter string, use %d for int, %f for float and %S for address of descriptor
      * NOTE! logging is done in 8 bit strings so you cannot format 16 bit descriptor
      * values.
     * Example usage:
     *  GLX_LOG_WARNING1( "Incorrect parameter %d", aInt );
     * @param s, the string to be inserted to the log, plain compile time string, not a descriptor
     * @param i, value to be inserted to the string, can be of any type
     */
    #define GLX_LOG_WARNING1( s, i ) \
        GLX_DEBUG_PRINT( GLX_DEBUG_LINE( "warning", s ), i )

    /**
     * Logs a single line of warning with two parameters
     * along with the filename and linenumber
     * For the parameter string, use %d for int, %f for float and %S for address of descriptor
      * NOTE! logging is done in 8 bit strings so you cannot format 16 bit descriptor
      * values.
     * Example usage:
     *  GLX_LOG_WARNING2( "Incorrect parameter %f, defaulting to %f", aInputFloat, KDefaultFloat );
     * @param s, the string to be inserted to the log, plain compile time string, not a descriptor
     * @param i, value to be inserted to the string, can be of any type
     * @param j, value to be inserted to the string, can be of any type
     */
    #define GLX_LOG_WARNING2( s, i, j ) \
        GLX_DEBUG_PRINT( GLX_DEBUG_LINE( "warning", s ), i , j )

    /**
     * Logs a single line of warning with three parameters
     * along with the filename and linenumber
     * For the parameter string, use %d for int, %f for float and %S for address of descriptor
      * NOTE! logging is done in 8 bit strings so you cannot format 16 bit descriptor
      * values.
     * Example usage:
     *  GLX_LOG_WARNING3( "Incorrect size (%d,%d), scale is %f", aWidth, aHeight, aScale );
     * @param s, the string to be inserted to the log, plain compile time string, not a descriptor
     * @param i, value to be inserted to the string, can be of any type
     * @param j, value to be inserted to the string, can be of any type
     * @param k, value to be inserted to the string, can be of any type
     */
    #define GLX_LOG_WARNING3( s, i, j, k ) \
        GLX_DEBUG_PRINT( GLX_DEBUG_LINE( "warning", s ), i, j, k )

    /**
     * Logs a single line of error and the filename and linenumber.
     * Use this macro to track your asserts and panics so you can see them in the log and 
     * have a trace to come back to later on.
      * NOTE! logging is done in 8 bit strings so you cannot format 16 bit descriptor
      * values.
     * Example usage:
     *  GLX_LOG_ERROR( "Incorrect state, going to panic" );
     * @param s, the string to be inserted to the log, plain compile time string, not a descriptor
     */
    #define GLX_LOG_ERROR( s ) GLX_DEBUG_PRINT( GLX_DEBUG_LINE( "error", s ) )

    /**
     * Logs a single line of error with one parameter and the filename and linenumber
     * Use this macro to track your asserts and panics so you can see them in the log and 
     * have a trace to come back to later on.
     * For the parameter string, use %d for int, %f for float and %S for address of descriptor
      * NOTE! logging is done in 8 bit strings so you cannot format 16 bit descriptor
      * values.
     * Example usage:
     *  GLX_LOG_ERROR1( "Incorrect parameter %d, panicing", aInt );
     * @param s, the string to be inserted to the log, plain compile time string, not a descriptor
     * @param i, value to be inserted to the string, can be of any type
     */
    #define GLX_LOG_ERROR1( s, i ) \
        GLX_DEBUG_PRINT( GLX_DEBUG_LINE( "error", s ), i )

    /**
     * Logs a single line of error with one parameter and the filename and linenumber
     * Use this macro to track your asserts and panics so you can see them in the log and 
     * have a trace to come back to later on.
     * For the parameter string, use %d for int, %f for float and %S for address of descriptor
      * NOTE! logging is done in 8 bit strings so you cannot format 16 bit descriptor
      * values.
     * Example usage:
     *  GLX_LOG_ERROR2( "Incorrect size (%d,%d), panicking", aWidth, aHeight );
     * @param s, the string to be inserted to the log, plain compile time string, not a descriptor
     * @param i, value to be inserted to the string, can be of any type
     * @param j, value to be inserted to the string, can be of any type
     */
    #define GLX_LOG_ERROR2( s, i, j ) \
        GLX_DEBUG_PRINT( GLX_DEBUG_LINE( "error", s ), i, j )

    /**
     * Logs a single line of error with one parameter and the filename and linenumber
     * Use this macro to track your asserts and panics so you can see them in the log and 
     * have a trace to come back to later on.
     * For the parameter string, use %d for int, %f for float and %S for address of descriptor
      * NOTE! logging is done in 8 bit strings so you cannot format 16 bit descriptor
      * values.
     * Example usage:
     *  GLX_LOG_ERROR3( "Incorrect state %d,%d,%f, panicking", aInt, aInt2, aFloat );
     * @param s, the string to be inserted to the log, plain compile time string, not a descriptor
     * @param i, value to be inserted to the string, can be of any type
     * @param j, value to be inserted to the string, can be of any type
     * @param k, value to be inserted to the string, can be of any type
     */
    #define GLX_LOG_ERROR3( s, i, j, k ) \
        GLX_DEBUG_PRINT( GLX_DEBUG_LINE( "error", s ), i, j, k )

    // Old macros for compatibility 
    ///@deprecated, use GLX_LOG_ENTRY_EXIT instead
    #define GLX_FUNC( s ) GLX_LOG_ENTRY_EXIT( s )
    ///@deprecated, use GLX_LOG_ENTRY_EXIT_LEAVE_L instead
    #define GLX_FUNCL( s )	GLX_LOG_ENTRY_EXIT_LEAVE_L( s )
    ///@deprecated, use GLX_LOG_INFO instead
    #define GLX_DEBUG1( s ) GLX_LOG_INFO( s )
    ///@deprecated, use GLX_LOG_INFO1 instead
    #define GLX_DEBUG2( s, i ) GLX_LOG_INFO1( s, i )
    ///@deprecated, use GLX_LOG_INFO2 instead
    #define GLX_DEBUG3( s, i, j ) GLX_LOG_INFO2( s, i, j )
    ///@deprecated, use GLX_LOG_INFO3 instead
    #define GLX_DEBUG4( s, i, j, k ) GLX_LOG_INFO3( s, i, j, k )
    ///@deprecated, use GLX_LOG_INFO4 instead
    #define GLX_DEBUG5( s, i, j, k, l ) GLX_LOG_INFO4( s, i, j, k, l )

    /**
     * TRAP instrumentation for Leave
     */
    #undef TRAP_INSTRUMENTATION_LEAVE
    #define TRAP_INSTRUMENTATION_LEAVE( aResult ) \
        GLX_LOG_INFO1( "Leave %d TRAPPED", aResult )

    /**
     * Helper class to track function entry and exit.
     */
    class TGlxFunctionTrace
        {
        public:
            inline TGlxFunctionTrace( TRefByValue< const TDesC8 > aName )
                : iStr( aName )
                {
                // not using GLX_DEBUG2 as this function does not inline
                GLX_DEBUG_PRINT( _L8( "Entry : %S" ), &iStr );
                }
            inline ~TGlxFunctionTrace()
                {
                if( std::uncaught_exception() )
                    {
                    // not using GLX_DEBUG2 as this function does not inline
                    GLX_DEBUG_PRINT( _L8( "Leave: %S" ), &iStr );
                    }
                else
                    {
                    // not using GLX_DEBUG2 as this function does not inline
                    GLX_DEBUG_PRINT( _L8( "Exit : %S" ), &iStr );
                    }
                }
        private:
            TPtrC8 iStr;
        };

    /**
     * Helper class to track function entry, exit and possible leave.
     * Never instantiate this class by itself.
     * Only ever user the macro GLX_FUNCL as this calls CleanupClosePushL
     * If you don't use the macro the destructor will panic in CleanupStack::Pop( this )
     * DO NOT use this in a LC method such as NewLC. 
     * Cleanup stack will be popped in the wrong order causing unwanted behaviour
     */
    class TGlxFunctionTraceWithLeaveDetection
        {
        public:
            inline TGlxFunctionTraceWithLeaveDetection( TRefByValue<const TDesC8> aName )
                : iStr( aName ), iLeave( EFalse )
                {
                // not using GLX_DEBUG2 as this function does not inline
                GLX_DEBUG_PRINT( _L8( "Entry : %S" ), &iStr );
                }
            inline ~TGlxFunctionTraceWithLeaveDetection()
                {
                // Did we leave
                if( iLeave )                         
                    {
                    // not using GLX_DEBUG2 as this function does not inline
                    GLX_DEBUG_PRINT( _L8( "Leave : %S" ), &iStr );
                    }
                else
                    {
                    // not using GLX_DEBUG2 as this function does not inline
                    GLX_DEBUG_PRINT( _L8( "Exit : %S" ), &iStr );
                    // Remove this from Cleanup. 
                    CleanupStack::Pop( this );
                    }
                }
            void Close() // This is only called if a leave happens.
                {
                iLeave = ETrue;
                }
        private:
            TPtrC8 iStr;
            TBool iLeave;
        };

#endif

#endif  // GLXLOG_H

// End of File
