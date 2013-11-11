/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Logger constants.
*
*/


#ifndef _DEBUG_LOG_CONST_H_
#define _DEBUG_LOG_CONST_H_

// ========
// INCLUDES
// ========

#include <e32std.h>
#include <e32def.h>
#include "DebugLog.h"

// ========
// LITERALS
// ========

// The base directory for debug logs.
_LIT( KDebugLogsBaseDirectory, "c:\\data\\logs\\" );

// The name of the engine log file
_LIT( KDebugLogFileExt, ".txt" );

// The prefix for the class destructor method
_LIT8( KDebugLogDestructorPrefix, "~" );

// Entering object method log
_LIT8( KStLogEnter, "Enter" );

// Exiting object method log
#define __EXIT_STRING "Exit"
_LIT8( KStLogExit, __EXIT_STRING );

// Object constructor log
_LIT8( KStLogConstruct, "++(0x%x)" );

// Object destructor log
_LIT8( KStLogDestruct, "--(0x%x)" );

// =============
// HELPER MACROS
// =============

// The following logging options are supported:
#define __DEBUG_LOG_NONE              0x0000
#define __DEBUG_LOG_CONSTRUCT         0x0002
#define __DEBUG_LOG_DESTRUCT          0x0004
#define __DEBUG_LOG_ENTER             0x0008
#define __DEBUG_LOG_EXIT              0x0010
#define __DEBUG_LOG_ERRORS            0x0020
#define __DEBUG_LOG_INFO              0x0040
#define __DEBUG_LOG_DEBUG1            0x0080
#define __DEBUG_LOG_DEBUG2            0x0100
#define __DEBUG_LOG_DEBUG3            0x0200
#define __DEBUG_LOG_ALL               0xFFFF

#ifdef DEBUG_USE_PROJECT_LOGGING_OPTIONS

	#ifdef DEBUG_LOG_OPTION_CONSTRUCT
		#undef DEBUG_LOG_OPTION_CONSTRUCT
		#define DEBUG_LOG_OPTION_CONSTRUCT __DEBUG_LOG_CONSTRUCT
	#else
		#define DEBUG_LOG_OPTION_CONSTRUCT 0
	#endif

	#ifdef DEBUG_LOG_OPTION_DESTRUCT
		#undef DEBUG_LOG_OPTION_DESTRUCT
		#define DEBUG_LOG_OPTION_DESTRUCT __DEBUG_LOG_DESTRUCT
	#else
		#define DEBUG_LOG_OPTION_DESTRUCT 0
	#endif

	#ifdef DEBUG_LOG_OPTION_ENTER
		#undef DEBUG_LOG_OPTION_ENTER
		#define DEBUG_LOG_OPTION_ENTER __DEBUG_LOG_ENTER
	#else
		#define DEBUG_LOG_OPTION_ENTER 0
	#endif

	#ifdef DEBUG_LOG_OPTION_EXIT
		#undef DEBUG_LOG_OPTION_EXIT
		#define DEBUG_LOG_OPTION_EXIT __DEBUG_LOG_EXIT
	#else
		#define DEBUG_LOG_OPTION_EXIT 0
	#endif

	#ifdef DEBUG_LOG_OPTION_ERRORS
		#undef DEBUG_LOG_OPTION_ERRORS
		#define DEBUG_LOG_OPTION_ERRORS __DEBUG_LOG_ERRORS
	#else
		#define DEBUG_LOG_OPTION_ERRORS 0
	#endif

	#ifdef DEBUG_LOG_OPTION_INFO
		#undef DEBUG_LOG_OPTION_INFO
		#define DEBUG_LOG_OPTION_INFO __DEBUG_LOG_INFO
	#else
		#define DEBUG_LOG_OPTION_INFO 0
	#endif
	
	#ifdef DEBUG_LOG_OPTION_DEBUG1
		#undef DEBUG_LOG_OPTION_DEBUG1
		#define DEBUG_LOG_OPTION_DEBUG1 __DEBUG_LOG_DEBUG1
	#else
		#define DEBUG_LOG_OPTION_DEBUG1 0
	#endif

	#ifdef DEBUG_LOG_OPTION_DEBUG2
		#undef DEBUG_LOG_OPTION_DEBUG2
		#define DEBUG_LOG_OPTION_DEBUG2 __DEBUG_LOG_DEBUG2
	#else
		#define DEBUG_LOG_OPTION_DEBUG2 0
	#endif

	#ifdef DEBUG_LOG_OPTION_DEBUG3
		#undef DEBUG_LOG_OPTION_DEBUG3
		#define DEBUG_LOG_OPTION_DEBUG3 __DEBUG_LOG_DEBUG3
	#else
		#define DEBUG_LOG_OPTION_DEBUG3 0
	#endif

	#ifdef DEBUG_LOG_OPTION_ALL
		#undef DEBUG_LOG_OPTION_ALL
		#define DEBUG_LOG_OPTION_ALL __DEBUG_LOG_ALL
	#else
		#define DEBUG_LOG_OPTION_ALL 0
	#endif

	#define __DEBUG_LOGGING_OPTIONS ( DEBUG_LOG_OPTION_CONSTRUCT|\
	                                  DEBUG_LOG_OPTION_DESTRUCT|\
	                                  DEBUG_LOG_OPTION_ENTER|\
	                                  DEBUG_LOG_OPTION_EXIT|\
	                                  DEBUG_LOG_OPTION_ERRORS|\
	                                  DEBUG_LOG_OPTION_INFO|\
	                                  DEBUG_LOG_OPTION_DEBUG1|\
	                                  DEBUG_LOG_OPTION_DEBUG2|\
	                                  DEBUG_LOG_OPTION_DEBUG3|\
	                                  DEBUG_LOG_OPTION_ALL )
#else
	#ifndef __DEBUG_LOGGING_OPTIONS

		// If project has not defined its own log level, set the default log level
		// which will include info and errors only.
		#define __DEBUG_LOGGING_OPTIONS (__DEBUG_LOG_ERRORS|__DEBUG_LOG_INFO)

	#endif // __DEBUG_LOGGING_OPTIONS

	// Finally, allow the logging options to be overriden at the global level.
	#include "GlobalDebugLoggingOptions.h"

#endif // DEBUG_USE_PROJECT_LOGGING_OPTIONS

// Helper macro to use an argument and prevent compiler warnings about unused
// method and stack arguments.
#define DEBUG_USE_VAR(var) if ( &var ) {};

// These are the basic logging macros - they are NOT intended to be called directly.
// Instead use the logging MACROS defined in DebugLogMacros.h
#define __INTERNAL_LOG_DECLARATION  RDebugLog iLogger;

#define __INTERNAL_LOG_CLOSE_BETWEEN_WRITES  \
                    iLogger.SetCloseBetweenWrites( ETrue ); \
                    iLogger.Close();
                    
#ifdef DEBUG_LOG_CLOSE_BETWEEN_WRITES
    #define __INTERNAL_LOG_AFTER_OPEN iLogger.SetCloseBetweenWrites( ETrue );
#else
#ifdef DEBUG_LOG_DO_NOT_CLOSE_BETWEEN_WRITES
    #define __INTERNAL_LOG_AFTER_OPEN iLogger.SetCloseBetweenWrites( EFalse );
#else
    #define __INTERNAL_LOG_AFTER_OPEN 
#endif    
#endif                    

// =========================
// Constructor logging MACRO
// =========================
#if (__DEBUG_LOGGING_OPTIONS & __DEBUG_LOG_CONSTRUCT)
	#define __INTERNAL_LOG_CONSTRUCT(a,b) \
	                _LIT( KLogDirectory, a ); \
	                _LIT8( KClassName8, b ); \
                    iLogger.Open( KLogDirectory, KClassName8, this ); \
	                __INTERNAL_LOG_AFTER_OPEN \
					iLogger.WriteFormat( KClassName8, KStLogConstruct, this );
#else
	#define __INTERNAL_LOG_CONSTRUCT(a,b) \
	                _LIT( KLogDirectory, a ); \
	                _LIT8( KClassName8, b ); \
                    iLogger.Open( KLogDirectory, KClassName8, this ); \
	                __INTERNAL_LOG_AFTER_OPEN
#endif // __DEBUG_LOG_CONSTRUCT

// =========================
// Destructor logging MACRO
// =========================
#if (__DEBUG_LOGGING_OPTIONS & __DEBUG_LOG_DESTRUCT)
	#define __INTERNAL_LOG_DESTRUCT \
				    iLogger.WriteFormat( KDebugLogDestructorPrefix, KStLogDestruct, this ); \
					iLogger.Close();
#else
	#define __INTERNAL_LOG_DESTRUCT \
	                iLogger.Close();
#endif // __DEBUG_LOG_DESTRUCT

// ==================
// Static ENTER MACRO
// ==================

// : USE 
#if (__DEBUG_LOGGING_OPTIONS & __DEBUG_LOG_ENTER)

	#define __INTERNAL_LOG_STATIC_ENTER_SUPPRESS(a,b) \
	                _LIT( KLogDirectory, a ); \
	                _LIT8( KFuncName8, b ); DEBUG_USE_VAR(KFuncName8); \
	                RDebugLog iLogger; \
	                iLogger.CleanupClosePushL(); \
                    iLogger.Open( KLogDirectory, TPtrC8((const TText8 *)__FILE__), 0 );

	#define __INTERNAL_LOG_STATIC_ENTER(a,b) \
	                __INTERNAL_LOG_STATIC_ENTER_SUPPRESS(a,b) \
					iLogger.Write( KFuncName8, KStLogEnter );
#else

	#define __INTERNAL_LOG_STATIC_ENTER_SUPPRESS(a,b) \
	                _LIT( KLogDirectory, a ); \
	                _LIT8( KFuncName8, b ); DEBUG_USE_VAR(KFuncName8); \
	                RDebugLog iLogger; \
	                iLogger.CleanupClosePushL(); \
                    iLogger.Open( KLogDirectory, TPtrC8((const TText8 *)__FILE__), 0 );

	#define __INTERNAL_LOG_STATIC_ENTER(a,b) __INTERNAL_LOG_STATIC_ENTER_SUPPRESS(a,b)

#endif // __DEBUG_LOG_ENTER

// =================
// Static EXIT MACRO
// =================

#if (__DEBUG_LOGGING_OPTIONS & __DEBUG_LOG_EXIT)

	#define __INTERNAL_LOG_STATIC_EXIT_SUPPRESS \
			CleanupStack::PopAndDestroy( &iLogger );

	#define __INTERNAL_LOG_STATIC_EXIT \
			iLogger.Write( KFuncName8, KStLogExit ); \
				__INTERNAL_LOG_STATIC_EXIT_SUPPRESS
#else
	#define __INTERNAL_LOG_STATIC_EXIT_SUPPRESS \
					CleanupStack::PopAndDestroy( &iLogger );
	#define __INTERNAL_LOG_STATIC_EXIT __INTERNAL_LOG_STATIC_EXIT_SUPPRESS
#endif // __DEBUG_LOG_EXIT

// ===================
// ENTER logging MACRO
// ===================
#if (__DEBUG_LOGGING_OPTIONS & __DEBUG_LOG_ENTER)
	#define __INTERNAL_LOG_ENTER(a) \
				    _LIT8( KFuncName8, a ); \
				    iLogger.Write( KFuncName8, KStLogEnter );
#else
	#define __INTERNAL_LOG_ENTER(a) \
	                _LIT8( KFuncName8, a ); DEBUG_USE_VAR(KFuncName8);
#endif
#define __INTERNAL_LOG_ENTER_SUPPRESS(a) \
                    _LIT8( KFuncName8, a ); DEBUG_USE_VAR(KFuncName8); 
                    
// ==================
// EXIT logging MACRO
// ==================
#if (__DEBUG_LOGGING_OPTIONS & __DEBUG_LOG_EXIT)
	#define __INTERNAL_LOG_EXIT \
	                iLogger.Write( KFuncName8, KStLogExit );
	#define __INTERNAL_LOG_EXIT_FORMAT1(a,b) \
//	                { \
// JCC - Figure out later why this MACRO doesn't work correctly
//	                _LIT( KFormatString, __EXIT_STRING "(" a ")" ); \
//	                iLogger.WriteFormat( KFuncName8, KFormatString, b ); \
	                iLogger.Write( KFuncName8, KStLogExit );
//	                } 
	#define __INTERNAL_LOG_EXIT8_FORMAT1(a,b) \
//	                { \
// JCC - Figure out later why this MACRO doesn't work correctly
//	                _LIT8( KFormatString, __EXIT_STRING "(" a ")" ); \
//	                iLogger.WriteFormat( KFuncName8, KFormatString, b ); \
	                iLogger.Write( KFuncName8, KStLogExit );
//	                }
#else
	#define __INTERNAL_LOG_EXIT
	#define __INTERNAL_LOG_EXIT_FORMAT1(a,b)
	#define __INTERNAL_LOG_EXIT8_FORMAT1(a,b)
#endif // __DEBUG_LOG_EXIT

// =========================
// Internal write log MACROs
// =========================
#define __INTERNAL_LOG_WRITE(a) \
                    { \
                    _LIT8( KString, a ); \
                    iLogger.Write( KFuncName8, KString ); \
                    }
#define __INTERNAL_LOG_WRITE_FORMAT1(a,b) \
                    { \
                    _LIT( KFormatString, a ); \
                    iLogger.WriteFormat( KFuncName8, KFormatString, b ); \
                    }
#define __INTERNAL_LOG_WRITE8_FORMAT1(a,b) \
                    { \
                    _LIT8( KFormatString, a ); \
                    iLogger.WriteFormat( KFuncName8, KFormatString, b ); \
                    }
#define __INTERNAL_LOG_WRITE_FORMAT2(a,b,c) \
                    { \
                    _LIT( KFormatString, a ); \
                    iLogger.WriteFormat( KFuncName8, KFormatString, b, c ); \
                    }
#define __INTERNAL_LOG_WRITE8_FORMAT2(a,b,c) \
                    { \
                    _LIT8( KFormatString, a ); \
                    iLogger.WriteFormat( KFuncName8, KFormatString, b, c ); \
                    }
#define __INTERNAL_LOG_WRITE_FORMAT3(a,b,c,d) \
                    { \
                    _LIT( KFormatString, a ); \
                    iLogger.WriteFormat( KFuncName8, KFormatString, b, c, d ); \
                    }
#define __INTERNAL_LOG_WRITE8_FORMAT3(a,b,c,d) \
                    { \
                    _LIT8( KFormatString, a ); \
                    iLogger.WriteFormat( KFuncName8, KFormatString, b, c, d ); \
                    }
#define __INTERNAL_LOG_WRITE_FORMAT4(a,b,c,d,e) \
                    { \
                    _LIT( KFormatString, a ); \
                    iLogger.WriteFormat( KFuncName8, KFormatString, b, c, d, e ); \
                    }
#define __INTERNAL_LOG_WRITE8_FORMAT4(a,b,c,d,e) \
                    { \
                    _LIT8( KFormatString, a ); \
                    iLogger.WriteFormat( KFuncName8, KFormatString, b, c, d, e ); \
                    }
#define __INTERNAL_LOG_WRITE_FORMAT5(a,b,c,d,e,f) \
                    { \
                    _LIT( KFormatString, a ); \
                    iLogger.WriteFormat( KFuncName8, KFormatString, b, c, d, e, f ); \
                    }
#define __INTERNAL_LOG_WRITE8_FORMAT5(a,b,c,d,e,f) \
                    { \
                    _LIT8( KFormatString, a ); \
                    iLogger.WriteFormat( KFuncName8, KFormatString, b, c, d, e, f ); \
                    }
#define __INTERNAL_LOG_WRITE_FORMAT6(a,b,c,d,e,f,g) \
                    { \
                    _LIT( KFormatString, a ); \
                    iLogger.WriteFormat( KFuncName8, KFormatString, b, c, d, e, f, g ); \
                    }
#define __INTERNAL_LOG_WRITE8_FORMAT6(a,b,c,d,e,f,g) \
                    { \
                    _LIT8( KFormatString, a ); \
                    iLogger.WriteFormat( KFuncName8, KFormatString, b, c, d, e, f, g ); \
                    }
#define __INTERNAL_LOG_WRITE_FORMAT7(a,b,c,d,e,f,g,h) \
                    { \
                    _LIT( KFormatString, a ); \
                    iLogger.WriteFormat( KFuncName8, KFormatString, b, c, d, e, f, g, h ); \
                    }
#define __INTERNAL_LOG_WRITE8_FORMAT7(a,b,c,d,e,f,g,h) \
                    { \
                    _LIT8( KFormatString, a ); \
                    iLogger.WriteFormat( KFuncName8, KFormatString, b, c, d, e, f, g, h ); \
                    }
#define __INTERNAL_LOG_WRITE_FORMAT8(a,b,c,d,e,f,g,h,i) \
                    { \
                    _LIT( KFormatString, a ); \
                    iLogger.WriteFormat( KFuncName8, KFormatString, b, c, d, e, f, g, h, i ); \
                    }
#define __INTERNAL_LOG_WRITE8_FORMAT8(a,b,c,d,e,f,g,h,i) \
                    { \
                    _LIT8( KFormatString, a ); \
                    iLogger.WriteFormat( KFuncName8, KFormatString, b, c, d, e, f, g, h, i ); \
                    }
#define __INTERNAL_LOG_HEX_DUMP(a,b) \
                    { \
                    _LIT( KString, a ); \
                    iLogger.Write( KFuncName8, KString ); \
    			    iLogger.HexDump( KFuncName8, b ); \
                    }

// ===============================
// ERROR level internal log MACROs
// ===============================
#if (__DEBUG_LOGGING_OPTIONS & __DEBUG_LOG_ERRORS)
	#define __INTERNAL_LOG_WRITE_ERROR(a)                          __INTERNAL_LOG_WRITE(a)
	#define __INTERNAL_LOG_WRITE_FORMAT1_ERROR(a,b)                __INTERNAL_LOG_WRITE_FORMAT1(a,b)
	#define __INTERNAL_LOG_WRITE_FORMAT2_ERROR(a,b,c)              __INTERNAL_LOG_WRITE_FORMAT2(a,b,c)
	#define __INTERNAL_LOG_WRITE_FORMAT3_ERROR(a,b,c,d)            __INTERNAL_LOG_WRITE_FORMAT3(a,b,c,d)
	#define __INTERNAL_LOG_WRITE_FORMAT4_ERROR(a,b,c,d,e)          __INTERNAL_LOG_WRITE_FORMAT4(a,b,c,d,e)
	#define __INTERNAL_LOG_WRITE_FORMAT5_ERROR(a,b,c,d,e,f)        __INTERNAL_LOG_WRITE_FORMAT5(a,b,c,d,e,f)
	#define __INTERNAL_LOG_WRITE_FORMAT6_ERROR(a,b,c,d,e,f,g)      __INTERNAL_LOG_WRITE_FORMAT6(a,b,c,d,e,f,g)
	#define __INTERNAL_LOG_WRITE_FORMAT7_ERROR(a,b,c,d,e,f,g,h)    __INTERNAL_LOG_WRITE_FORMAT7(a,b,c,d,e,f,g,h)
	#define __INTERNAL_LOG_WRITE_FORMAT8_ERROR(a,b,c,d,e,f,g,h,i)  __INTERNAL_LOG_WRITE_FORMAT8(a,b,c,d,e,f,g,h,i)
	#define __INTERNAL_LOG_WRITE8_FORMAT1_ERROR(a,b)               __INTERNAL_LOG_WRITE8_FORMAT1(a,b)
	#define __INTERNAL_LOG_WRITE8_FORMAT2_ERROR(a,b,c)             __INTERNAL_LOG_WRITE8_FORMAT2(a,b,c)
	#define __INTERNAL_LOG_WRITE8_FORMAT3_ERROR(a,b,c,d)           __INTERNAL_LOG_WRITE8_FORMAT3(a,b,c,d)
	#define __INTERNAL_LOG_WRITE8_FORMAT4_ERROR(a,b,c,d,e)         __INTERNAL_LOG_WRITE8_FORMAT4(a,b,c,d,e)
	#define __INTERNAL_LOG_WRITE8_FORMAT5_ERROR(a,b,c,d,e,f)       __INTERNAL_LOG_WRITE8_FORMAT5(a,b,c,d,e,f)
	#define __INTERNAL_LOG_WRITE8_FORMAT6_ERROR(a,b,c,d,e,f,g)     __INTERNAL_LOG_WRITE8_FORMAT6(a,b,c,d,e,f,g)
	#define __INTERNAL_LOG_WRITE8_FORMAT7_ERROR(a,b,c,d,e,f,g,h)   __INTERNAL_LOG_WRITE8_FORMAT7(a,b,c,d,e,f,g,h)
	#define __INTERNAL_LOG_WRITE8_FORMAT8_ERROR(a,b,c,d,e,f,g,h,i) __INTERNAL_LOG_WRITE8_FORMAT8(a,b,c,d,e,f,g,h,i)
	#define __INTERNAL_LOG_HEX_DUMP_ERROR(a,b)                     __INTERNAL_LOG_HEX_DUMP(a,b)
#else
	#define __INTERNAL_LOG_WRITE_ERROR(a)
	#define __INTERNAL_LOG_WRITE_FORMAT1_ERROR(a,b)
	#define __INTERNAL_LOG_WRITE_FORMAT2_ERROR(a,b,c)
	#define __INTERNAL_LOG_WRITE_FORMAT3_ERROR(a,b,c,d)
	#define __INTERNAL_LOG_WRITE_FORMAT4_ERROR(a,b,c,d,e)
	#define __INTERNAL_LOG_WRITE_FORMAT5_ERROR(a,b,c,d,e,f)
	#define __INTERNAL_LOG_WRITE_FORMAT6_ERROR(a,b,c,d,e,f,g)
	#define __INTERNAL_LOG_WRITE_FORMAT7_ERROR(a,b,c,d,e,f,g,h)
	#define __INTERNAL_LOG_WRITE_FORMAT8_ERROR(a,b,c,d,e,f,g,h,i)
	#define __INTERNAL_LOG_WRITE8_FORMAT1_ERROR(a,b)
	#define __INTERNAL_LOG_WRITE8_FORMAT2_ERROR(a,b,c)
	#define __INTERNAL_LOG_WRITE8_FORMAT3_ERROR(a,b,c,d)
	#define __INTERNAL_LOG_WRITE8_FORMAT4_ERROR(a,b,c,d,e)
	#define __INTERNAL_LOG_WRITE8_FORMAT5_ERROR(a,b,c,d,e,f)
	#define __INTERNAL_LOG_WRITE8_FORMAT6_ERROR(a,b,c,d,e,f,g)
	#define __INTERNAL_LOG_WRITE8_FORMAT7_ERROR(a,b,c,d,e,f,g,h)
	#define __INTERNAL_LOG_WRITE8_FORMAT8_ERROR(a,b,c,d,e,f,g,h,i)
	#define __INTERNAL_LOG_HEX_DUMP_ERROR(a,b)    
#endif // __DEBUG_LOG_ERRORS

// ==============================
// INFO level internal log MACROs
// ==============================
#if (__DEBUG_LOGGING_OPTIONS & __DEBUG_LOG_INFO)
	#define __INTERNAL_LOG_WRITE_INFO(a)                          __INTERNAL_LOG_WRITE(a)
	#define __INTERNAL_LOG_WRITE_FORMAT1_INFO(a,b)                __INTERNAL_LOG_WRITE_FORMAT1(a,b)
	#define __INTERNAL_LOG_WRITE_FORMAT2_INFO(a,b,c)              __INTERNAL_LOG_WRITE_FORMAT2(a,b,c)
	#define __INTERNAL_LOG_WRITE_FORMAT3_INFO(a,b,c,d)            __INTERNAL_LOG_WRITE_FORMAT3(a,b,c,d)
	#define __INTERNAL_LOG_WRITE_FORMAT4_INFO(a,b,c,d,e)          __INTERNAL_LOG_WRITE_FORMAT4(a,b,c,d,e)
	#define __INTERNAL_LOG_WRITE_FORMAT5_INFO(a,b,c,d,e,f)        __INTERNAL_LOG_WRITE_FORMAT5(a,b,c,d,e,f)
	#define __INTERNAL_LOG_WRITE_FORMAT6_INFO(a,b,c,d,e,f,g)      __INTERNAL_LOG_WRITE_FORMAT6(a,b,c,d,e,f,g)
	#define __INTERNAL_LOG_WRITE_FORMAT7_INFO(a,b,c,d,e,f,g,h)    __INTERNAL_LOG_WRITE_FORMAT7(a,b,c,d,e,f,g,h)
	#define __INTERNAL_LOG_WRITE_FORMAT8_INFO(a,b,c,d,e,f,g,h,i)  __INTERNAL_LOG_WRITE_FORMAT8(a,b,c,d,e,f,g,h,i)
	#define __INTERNAL_LOG_WRITE8_FORMAT1_INFO(a,b)               __INTERNAL_LOG_WRITE8_FORMAT1(a,b)
	#define __INTERNAL_LOG_WRITE8_FORMAT2_INFO(a,b,c)             __INTERNAL_LOG_WRITE8_FORMAT2(a,b,c)
	#define __INTERNAL_LOG_WRITE8_FORMAT3_INFO(a,b,c,d)           __INTERNAL_LOG_WRITE8_FORMAT3(a,b,c,d)
	#define __INTERNAL_LOG_WRITE8_FORMAT4_INFO(a,b,c,d,e)         __INTERNAL_LOG_WRITE8_FORMAT4(a,b,c,d,e)
	#define __INTERNAL_LOG_WRITE8_FORMAT5_INFO(a,b,c,d,e,f)       __INTERNAL_LOG_WRITE8_FORMAT5(a,b,c,d,e,f)
	#define __INTERNAL_LOG_WRITE8_FORMAT6_INFO(a,b,c,d,e,f,g)     __INTERNAL_LOG_WRITE8_FORMAT6(a,b,c,d,e,f,g)
	#define __INTERNAL_LOG_WRITE8_FORMAT7_INFO(a,b,c,d,e,f,g,h)   __INTERNAL_LOG_WRITE8_FORMAT7(a,b,c,d,e,f,g,h)
	#define __INTERNAL_LOG_WRITE8_FORMAT8_INFO(a,b,c,d,e,f,g,h,i) __INTERNAL_LOG_WRITE8_FORMAT8(a,b,c,d,e,f,g,h,i)
	#define __INTERNAL_LOG_HEX_DUMP_INFO(a,b)                     __INTERNAL_LOG_HEX_DUMP(a,b)
#else
	#define __INTERNAL_LOG_WRITE_INFO(a)
	#define __INTERNAL_LOG_WRITE_FORMAT1_INFO(a,b)
	#define __INTERNAL_LOG_WRITE_FORMAT2_INFO(a,b,c)
	#define __INTERNAL_LOG_WRITE_FORMAT3_INFO(a,b,c,d)
	#define __INTERNAL_LOG_WRITE_FORMAT4_INFO(a,b,c,d,e)
	#define __INTERNAL_LOG_WRITE_FORMAT5_INFO(a,b,c,d,e,f)
	#define __INTERNAL_LOG_WRITE_FORMAT6_INFO(a,b,c,d,e,f,g)
	#define __INTERNAL_LOG_WRITE_FORMAT7_INFO(a,b,c,d,e,f,g,h)
	#define __INTERNAL_LOG_WRITE_FORMAT8_INFO(a,b,c,d,e,f,g,h,i)
	#define __INTERNAL_LOG_WRITE8_FORMAT1_INFO(a,b)
	#define __INTERNAL_LOG_WRITE8_FORMAT2_INFO(a,b,c)
	#define __INTERNAL_LOG_WRITE8_FORMAT3_INFO(a,b,c,d)
	#define __INTERNAL_LOG_WRITE8_FORMAT4_INFO(a,b,c,d,e)
	#define __INTERNAL_LOG_WRITE8_FORMAT5_INFO(a,b,c,d,e,f)
	#define __INTERNAL_LOG_WRITE8_FORMAT6_INFO(a,b,c,d,e,f,g)
	#define __INTERNAL_LOG_WRITE8_FORMAT7_INFO(a,b,c,d,e,f,g,h)
	#define __INTERNAL_LOG_WRITE8_FORMAT8_INFO(a,b,c,d,e,f,g,h,i)
	#define __INTERNAL_LOG_HEX_DUMP_INFO(a,b)
#endif // __DEBUG_LOG_INFO

// ================================
// DEBUG1 level internal log MACROs
// ================================
#if (__DEBUG_LOGGING_OPTIONS & __DEBUG_LOG_DEBUG1)
	#define __INTERNAL_LOG_WRITE_DEBUG1(a)                          __INTERNAL_LOG_WRITE(a)
	#define __INTERNAL_LOG_WRITE_FORMAT1_DEBUG1(a,b)                __INTERNAL_LOG_WRITE_FORMAT1(a,b)
	#define __INTERNAL_LOG_WRITE_FORMAT2_DEBUG1(a,b,c)              __INTERNAL_LOG_WRITE_FORMAT2(a,b,c)
	#define __INTERNAL_LOG_WRITE_FORMAT3_DEBUG1(a,b,c,d)            __INTERNAL_LOG_WRITE_FORMAT3(a,b,c,d)
	#define __INTERNAL_LOG_WRITE_FORMAT4_DEBUG1(a,b,c,d,e)          __INTERNAL_LOG_WRITE_FORMAT4(a,b,c,d,e)
	#define __INTERNAL_LOG_WRITE_FORMAT5_DEBUG1(a,b,c,d,e,f)        __INTERNAL_LOG_WRITE_FORMAT5(a,b,c,d,e,f)
	#define __INTERNAL_LOG_WRITE_FORMAT6_DEBUG1(a,b,c,d,e,f,g)      __INTERNAL_LOG_WRITE_FORMAT6(a,b,c,d,e,f,g)
	#define __INTERNAL_LOG_WRITE_FORMAT7_DEBUG1(a,b,c,d,e,f,g,h)    __INTERNAL_LOG_WRITE_FORMAT7(a,b,c,d,e,f,g,h)
	#define __INTERNAL_LOG_WRITE_FORMAT8_DEBUG1(a,b,c,d,e,f,g,h,i)  __INTERNAL_LOG_WRITE_FORMAT8(a,b,c,d,e,f,g,h,i)
	#define __INTERNAL_LOG_WRITE8_FORMAT1_DEBUG1(a,b)               __INTERNAL_LOG_WRITE8_FORMAT1(a,b)
	#define __INTERNAL_LOG_WRITE8_FORMAT2_DEBUG1(a,b,c)             __INTERNAL_LOG_WRITE8_FORMAT2(a,b,c)
	#define __INTERNAL_LOG_WRITE8_FORMAT3_DEBUG1(a,b,c,d)           __INTERNAL_LOG_WRITE8_FORMAT3(a,b,c,d)
	#define __INTERNAL_LOG_WRITE8_FORMAT4_DEBUG1(a,b,c,d,e)         __INTERNAL_LOG_WRITE8_FORMAT4(a,b,c,d,e)
	#define __INTERNAL_LOG_WRITE8_FORMAT5_DEBUG1(a,b,c,d,e,f)       __INTERNAL_LOG_WRITE8_FORMAT5(a,b,c,d,e,f)
	#define __INTERNAL_LOG_WRITE8_FORMAT6_DEBUG1(a,b,c,d,e,f,g)     __INTERNAL_LOG_WRITE8_FORMAT6(a,b,c,d,e,f,g)
	#define __INTERNAL_LOG_WRITE8_FORMAT7_DEBUG1(a,b,c,d,e,f,g,h)   __INTERNAL_LOG_WRITE8_FORMAT7(a,b,c,d,e,f,g,h)
	#define __INTERNAL_LOG_WRITE8_FORMAT8_DEBUG1(a,b,c,d,e,f,g,h,i) __INTERNAL_LOG_WRITE8_FORMAT8(a,b,c,d,e,f,g,h,i)
	#define __INTERNAL_LOG_HEX_DUMP_DEBUG1(a,b)                     __INTERNAL_LOG_HEX_DUMP(a,b)
#else
	#define __INTERNAL_LOG_WRITE_DEBUG1(a)
	#define __INTERNAL_LOG_WRITE_FORMAT1_DEBUG1(a,b)
	#define __INTERNAL_LOG_WRITE_FORMAT2_DEBUG1(a,b,c)
	#define __INTERNAL_LOG_WRITE_FORMAT3_DEBUG1(a,b,c,d)
	#define __INTERNAL_LOG_WRITE_FORMAT4_DEBUG1(a,b,c,d,e)
	#define __INTERNAL_LOG_WRITE_FORMAT5_DEBUG1(a,b,c,d,e,f)
	#define __INTERNAL_LOG_WRITE_FORMAT6_DEBUG1(a,b,c,d,e,f,g)
	#define __INTERNAL_LOG_WRITE_FORMAT7_DEBUG1(a,b,c,d,e,f,g,h)
	#define __INTERNAL_LOG_WRITE_FORMAT8_DEBUG1(a,b,c,d,e,f,g,h,i)
	#define __INTERNAL_LOG_WRITE8_FORMAT1_DEBUG1(a,b)
	#define __INTERNAL_LOG_WRITE8_FORMAT2_DEBUG1(a,b,c)
	#define __INTERNAL_LOG_WRITE8_FORMAT3_DEBUG1(a,b,c,d)
	#define __INTERNAL_LOG_WRITE8_FORMAT4_DEBUG1(a,b,c,d,e)
	#define __INTERNAL_LOG_WRITE8_FORMAT5_DEBUG1(a,b,c,d,e,f)
	#define __INTERNAL_LOG_WRITE8_FORMAT6_DEBUG1(a,b,c,d,e,f,g)
	#define __INTERNAL_LOG_WRITE8_FORMAT7_DEBUG1(a,b,c,d,e,f,g,h)
	#define __INTERNAL_LOG_WRITE8_FORMAT8_DEBUG1(a,b,c,d,e,f,g,h,i)
	#define __INTERNAL_LOG_HEX_DUMP_DEBUG1(a,b)
#endif // __DEBUG_LOG_DEBUG1

// ================================
// DEBUG2 level internal log MACROs
// ================================
#if (__DEBUG_LOGGING_OPTIONS & __DEBUG_LOG_DEBUG2)
	#define __INTERNAL_LOG_WRITE_DEBUG2(a)                          __INTERNAL_LOG_WRITE(a)
	#define __INTERNAL_LOG_WRITE_FORMAT1_DEBUG2(a,b)                __INTERNAL_LOG_WRITE_FORMAT1(a,b)
	#define __INTERNAL_LOG_WRITE_FORMAT2_DEBUG2(a,b,c)              __INTERNAL_LOG_WRITE_FORMAT2(a,b,c)
	#define __INTERNAL_LOG_WRITE_FORMAT3_DEBUG2(a,b,c,d)            __INTERNAL_LOG_WRITE_FORMAT3(a,b,c,d)
	#define __INTERNAL_LOG_WRITE_FORMAT4_DEBUG2(a,b,c,d,e)          __INTERNAL_LOG_WRITE_FORMAT4(a,b,c,d,e)
	#define __INTERNAL_LOG_WRITE_FORMAT5_DEBUG2(a,b,c,d,e,f)        __INTERNAL_LOG_WRITE_FORMAT5(a,b,c,d,e,f)
	#define __INTERNAL_LOG_WRITE_FORMAT6_DEBUG2(a,b,c,d,e,f,g)      __INTERNAL_LOG_WRITE_FORMAT6(a,b,c,d,e,f,g)
	#define __INTERNAL_LOG_WRITE_FORMAT7_DEBUG2(a,b,c,d,e,f,g,h)    __INTERNAL_LOG_WRITE_FORMAT7(a,b,c,d,e,f,g,h)
	#define __INTERNAL_LOG_WRITE_FORMAT8_DEBUG2(a,b,c,d,e,f,g,h,i)  __INTERNAL_LOG_WRITE_FORMAT8(a,b,c,d,e,f,g,h,i)
	#define __INTERNAL_LOG_WRITE8_FORMAT1_DEBUG2(a,b)               __INTERNAL_LOG_WRITE8_FORMAT1(a,b)
	#define __INTERNAL_LOG_WRITE8_FORMAT2_DEBUG2(a,b,c)             __INTERNAL_LOG_WRITE8_FORMAT2(a,b,c)
	#define __INTERNAL_LOG_WRITE8_FORMAT3_DEBUG2(a,b,c,d)           __INTERNAL_LOG_WRITE8_FORMAT3(a,b,c,d)
	#define __INTERNAL_LOG_WRITE8_FORMAT4_DEBUG2(a,b,c,d,e)         __INTERNAL_LOG_WRITE8_FORMAT4(a,b,c,d,e)
	#define __INTERNAL_LOG_WRITE8_FORMAT5_DEBUG2(a,b,c,d,e,f)       __INTERNAL_LOG_WRITE8_FORMAT5(a,b,c,d,e,f)
	#define __INTERNAL_LOG_WRITE8_FORMAT6_DEBUG2(a,b,c,d,e,f,g)     __INTERNAL_LOG_WRITE8_FORMAT6(a,b,c,d,e,f,g)
	#define __INTERNAL_LOG_WRITE8_FORMAT7_DEBUG2(a,b,c,d,e,f,g,h)   __INTERNAL_LOG_WRITE8_FORMAT7(a,b,c,d,e,f,g,h)
	#define __INTERNAL_LOG_WRITE8_FORMAT8_DEBUG2(a,b,c,d,e,f,g,h,i) __INTERNAL_LOG_WRITE8_FORMAT8(a,b,c,d,e,f,g,h,i)
	#define __INTERNAL_LOG_HEX_DUMP_DEBUG2(a,b)                     __INTERNAL_LOG_HEX_DUMP(a,b)
#else
	#define __INTERNAL_LOG_WRITE_DEBUG2(a)
	#define __INTERNAL_LOG_WRITE_FORMAT1_DEBUG2(a,b)
	#define __INTERNAL_LOG_WRITE_FORMAT2_DEBUG2(a,b,c)
	#define __INTERNAL_LOG_WRITE_FORMAT3_DEBUG2(a,b,c,d)
	#define __INTERNAL_LOG_WRITE_FORMAT4_DEBUG2(a,b,c,d,e)
	#define __INTERNAL_LOG_WRITE_FORMAT5_DEBUG2(a,b,c,d,e,f)
	#define __INTERNAL_LOG_WRITE_FORMAT6_DEBUG2(a,b,c,d,e,f,g)
	#define __INTERNAL_LOG_WRITE_FORMAT7_DEBUG2(a,b,c,d,e,f,g,h)
	#define __INTERNAL_LOG_WRITE_FORMAT8_DEBUG2(a,b,c,d,e,f,g,h,i)
	#define __INTERNAL_LOG_WRITE8_FORMAT1_DEBUG2(a,b)
	#define __INTERNAL_LOG_WRITE8_FORMAT2_DEBUG2(a,b,c)
	#define __INTERNAL_LOG_WRITE8_FORMAT3_DEBUG2(a,b,c,d)
	#define __INTERNAL_LOG_WRITE8_FORMAT4_DEBUG2(a,b,c,d,e)
	#define __INTERNAL_LOG_WRITE8_FORMAT5_DEBUG2(a,b,c,d,e,f)
	#define __INTERNAL_LOG_WRITE8_FORMAT6_DEBUG2(a,b,c,d,e,f,g)
	#define __INTERNAL_LOG_WRITE8_FORMAT7_DEBUG2(a,b,c,d,e,f,g,h)
	#define __INTERNAL_LOG_WRITE8_FORMAT8_DEBUG2(a,b,c,d,e,f,g,h,i)
	#define __INTERNAL_LOG_HEX_DUMP_DEBUG2(a,b)
#endif // __DEBUG_LOG_DEBUG2

// ================================
// DEBUG3 level internal log MACROs
// ================================
#if (__DEBUG_LOGGING_OPTIONS & __DEBUG_LOG_DEBUG3)
	#define __INTERNAL_LOG_WRITE_DEBUG3(a)                          __INTERNAL_LOG_WRITE(a)
	#define __INTERNAL_LOG_WRITE_FORMAT1_DEBUG3(a,b)                __INTERNAL_LOG_WRITE_FORMAT1(a,b)
	#define __INTERNAL_LOG_WRITE_FORMAT2_DEBUG3(a,b,c)              __INTERNAL_LOG_WRITE_FORMAT2(a,b,c)
	#define __INTERNAL_LOG_WRITE_FORMAT3_DEBUG3(a,b,c,d)            __INTERNAL_LOG_WRITE_FORMAT3(a,b,c,d)
	#define __INTERNAL_LOG_WRITE_FORMAT4_DEBUG3(a,b,c,d,e)          __INTERNAL_LOG_WRITE_FORMAT4(a,b,c,d,e)
	#define __INTERNAL_LOG_WRITE_FORMAT5_DEBUG3(a,b,c,d,e,f)        __INTERNAL_LOG_WRITE_FORMAT5(a,b,c,d,e,f)
	#define __INTERNAL_LOG_WRITE_FORMAT6_DEBUG3(a,b,c,d,e,f,g)      __INTERNAL_LOG_WRITE_FORMAT6(a,b,c,d,e,f,g)
	#define __INTERNAL_LOG_WRITE_FORMAT7_DEBUG3(a,b,c,d,e,f,g,h)    __INTERNAL_LOG_WRITE_FORMAT7(a,b,c,d,e,f,g,h)
	#define __INTERNAL_LOG_WRITE_FORMAT8_DEBUG3(a,b,c,d,e,f,g,h,i)  __INTERNAL_LOG_WRITE_FORMAT8(a,b,c,d,e,f,g,h,i)
	#define __INTERNAL_LOG_WRITE8_FORMAT1_DEBUG3(a,b)               __INTERNAL_LOG_WRITE8_FORMAT1(a,b)
	#define __INTERNAL_LOG_WRITE8_FORMAT2_DEBUG3(a,b,c)             __INTERNAL_LOG_WRITE8_FORMAT2(a,b,c)
	#define __INTERNAL_LOG_WRITE8_FORMAT3_DEBUG3(a,b,c,d)           __INTERNAL_LOG_WRITE8_FORMAT3(a,b,c,d)
	#define __INTERNAL_LOG_WRITE8_FORMAT4_DEBUG3(a,b,c,d,e)         __INTERNAL_LOG_WRITE8_FORMAT4(a,b,c,d,e)
	#define __INTERNAL_LOG_WRITE8_FORMAT5_DEBUG3(a,b,c,d,e,f)       __INTERNAL_LOG_WRITE8_FORMAT5(a,b,c,d,e,f)
	#define __INTERNAL_LOG_WRITE8_FORMAT6_DEBUG3(a,b,c,d,e,f,g)     __INTERNAL_LOG_WRITE8_FORMAT6(a,b,c,d,e,f,g)
	#define __INTERNAL_LOG_WRITE8_FORMAT7_DEBUG3(a,b,c,d,e,f,g,h)   __INTERNAL_LOG_WRITE8_FORMAT7(a,b,c,d,e,f,g,h)
	#define __INTERNAL_LOG_WRITE8_FORMAT8_DEBUG3(a,b,c,d,e,f,g,h,i) __INTERNAL_LOG_WRITE8_FORMAT8(a,b,c,d,e,f,g,h,i)
	#define __INTERNAL_LOG_HEX_DUMP_DEBUG3(a,b)                     __INTERNAL_LOG_HEX_DUMP(a,b)
#else
	#define __INTERNAL_LOG_WRITE_DEBUG3(a)
	#define __INTERNAL_LOG_WRITE_FORMAT1_DEBUG3(a,b)
	#define __INTERNAL_LOG_WRITE_FORMAT2_DEBUG3(a,b,c)
	#define __INTERNAL_LOG_WRITE_FORMAT3_DEBUG3(a,b,c,d)
	#define __INTERNAL_LOG_WRITE_FORMAT4_DEBUG3(a,b,c,d,e)
	#define __INTERNAL_LOG_WRITE_FORMAT5_DEBUG3(a,b,c,d,e,f)
	#define __INTERNAL_LOG_WRITE_FORMAT6_DEBUG3(a,b,c,d,e,f,g)
	#define __INTERNAL_LOG_WRITE_FORMAT7_DEBUG3(a,b,c,d,e,f,g,h)
	#define __INTERNAL_LOG_WRITE_FORMAT8_DEBUG3(a,b,c,d,e,f,g,h,i)
	#define __INTERNAL_LOG_WRITE8_FORMAT1_DEBUG3(a,b)
	#define __INTERNAL_LOG_WRITE8_FORMAT2_DEBUG3(a,b,c)
	#define __INTERNAL_LOG_WRITE8_FORMAT3_DEBUG3(a,b,c,d)
	#define __INTERNAL_LOG_WRITE8_FORMAT4_DEBUG3(a,b,c,d,e)
	#define __INTERNAL_LOG_WRITE8_FORMAT5_DEBUG3(a,b,c,d,e,f)
	#define __INTERNAL_LOG_WRITE8_FORMAT6_DEBUG3(a,b,c,d,e,f,g)
	#define __INTERNAL_LOG_WRITE8_FORMAT7_DEBUG3(a,b,c,d,e,f,g,h)
	#define __INTERNAL_LOG_WRITE8_FORMAT8_DEBUG3(a,b,c,d,e,f,g,h,i)
	#define __INTERNAL_LOG_HEX_DUMP_DEBUG3(a,b)
#endif // __DEBUG_LOG_DEBUG3

#define __INTERNAL_LOG_LEAKED_OBJECTS RDebugLog::LogLeakedObjects();

#endif // _DEBUG_LOG_CONST_H_

// END FILE DebugLogConst.h
