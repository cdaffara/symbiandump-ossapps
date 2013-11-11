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
* Description:  Logger macros.
*
*/


#ifndef _DEBUG_LOG_MACROS_H_
#define _DEBUG_LOG_MACROS_H_

// =============================================================================
//
// DESCRIPTION:
//
// The macros in this header file provide a consistent, centralized way of
// logging debug traces.  The intent is for all libraries to use these macros so
// that debug tracing can be more efficient while being more maintainable.
//
// USAGE:
//
//  CONFIGURING:
//
//      - add to .mmp file
//
//          // [Optional] Override default tracing options. By default, error and
//          // info will be sent to trace file.  See below for a more detailed
//          // description of the logging options
//          MACRO DEBUG_USE_PROJECT_LOGGING_OPTIONS
//          MACRO DEBUG_LOG_OPTION_CONSTRUCT
//
//          // The following library is for Debug log tracing.
//          DEBUGLIBRARY DebugLog.lib
//
//      - make the directory: C:\data\logs and the appropriate subdirectories,
//        as indicated in the __LOG_CONSTRUCT macro below.
//        
//        For a given class, each thread that uses the class will have a separate
//        log file within the directory.  The log file will be named based on the
//        name of the process and the name of the thread.
//
//  PRINTING: (NOTE: The following is just a reference example)
//
//      // Include the debug logging header file
//      #include "DebugLogMacros.h"
//
//      class CTestClass
//          {
//          // MANDATORY: This must be within the class declaration
//          __LOG_DECLARATION
//          };
//
//      CTestClass::CTestClass()
//          {
//          // MANDATORY: Provide the subdirectory with C:\data\logs and the class name.
//          __LOG_CONSTRUCT( "subdirectory", "CTestClass" )
//
//          // OPTIONAL: Do this if you want to close the log file between writes.
//          //           (Similar to static RFlogger calls).
//          __LOG_CLOSE_BETWEEN_WRITES
//          }
//
//      CTestClass::~CTestClass()
//          {
//          // MANDATORY: Cleanup the log data.
//          __LOG_DESTRUCT
//          }
//
//      CTestClass::DoSomething( TInt aParam )
//      {
//          // Always log entering a method if other tracing is
//          // will be present in the method.  Use
//          // __LOG_ENTER_SUPPRESS(...) if you don't want the
//          // enter log to be traced even when
//          // DEBUG_LOG_OPTION_ENTER is enabled.
//          __LOG_ENTER( "DoSomething" )
//
//          // Log a simple trace (no formatting)
//          __LOG_WRITE_INFO( "This is a simple info trace" )
//
//          // Log a formatted trace. For each level of trace
//          // up to 4 variables may be formatted.  FORMAT1
//          // macros should be used for 1 paramter, FORMAT2
//          // should be used for 2 paramters, etc.
//          __LOG_WRITE_FORMAT1_DEBUG1( "aParam =%d", aParam )
//
//          // Optionally log method exit
//          __LOG_EXIT
//      }
// 
// =============================================================================
//
// =============================================================================
//
// OVERRIDING DEFAULT LOGGING OPTIONS:
//
// You can close all log files between writes for a given .mmp by defining the
// MACRO DEBUG_LOG_CLOSE_BETWEEN_WRITES within the .mmp file.
// 
// A project can override the default logging options by defining the MACRO
// DEBUG_USE_PROJECT_LOGGING_OPTIONS in its .mmp file.  After defining this MACRO
// any one the following option MACROS may be defined in the .mmp file:
//
// NAME: DEBUG_LOG_NONE
// DESCRIPTION: Disables all traces.
//
// NAME:        DEBUG_LOG_OPTION_CONSTRUCT
// DESCRIPTION: Enables constructor traces. (i.e. __LOG_CONSTRUCT )
//
// NAME:        DEBUG_LOG_OPTION_DESTRUCT
// DESCRIPTION: Enables destructor traces. (i.e. __LOG_DESTRUCT )
//
// NAME:        DEBUG_LOG_OPTION_ENTER
// DESCRIPTION: Enables method enter traces. (i.e. __LOG_ENTER(a) )
//
// NAME:        DEBUG_LOG_OPTION_EXIT
// DESCRIPTION: Enables method exit traces. (i.e. __LOG_EXIT )
//
// NAME:        DEBUG_LOG_OPTION_ERRORS
// DESCRIPTION: Enables error traces. (i.e. __LOG_WRITE_[...]_ERROR() )
//
// NAME:        DEBUG_LOG_OPTION_INFO
// DESCRIPTION: Enables info traces. (i.e. __LOG_WRITE_[...]_INFO() )
//
// NAME:        DEBUG_LOG_OPTION_DEBUG1
// DESCRIPTION: Enables level 1 debug traces. (i.e. __LOG_WRITE_[...]_DEBUG1() )
//
// NAME:        DEBUG_LOG_OPTION_DEBUG2
// DESCRIPTION: Enables level 2 debug traces. (i.e. __LOG_WRITE_[...]_DEBUG2() )
//
// NAME:        DEBUG_LOG_OPTION_DEBUG3
// DESCRIPTION: Enables level 3 debug traces. (i.e. __LOG_WRITE_[...]_DEBUG3() )
//
// NAME:        DEBUG_LOG_OPTION_ALL
// DESCRIPTION: Enables all traces. (WARNING - The trace file can quickly get
//              quite large).
//
// **********
// EXAMPLE 1: To enable constructor, destructor, and INFO traces:
// **********
//
//  .
//  .
//  .
//  MACRO DEBUG_USE_PROJECT_LOGGING_OPTIONS
//  MACRO DEBUG_LOG_OPTION_CONSTRUCT
//  MACRO DEBUG_LOG_OPTION_DESTRUCT
//  MACRO DEBUG_LOG_OPTION_INFO
//  .
//  .
//  .
//
// **********
// EXAMPLE 2: Disable all traces (in a debug build):
// **********
//
//  .
//  .
//  .
//  MACRO DEBUG_USE_PROJECT_LOGGING_OPTIONS
//  MACRO DEBUG_LOG_NONE
//  .
//  .
//  .
// 
// =============================================================================

#ifdef _DEBUG

    // ===========
    // DEBUG BUILD
    // ===========

    #include "DebugLogConst.h"

    // =====================================================================
    // Utility Logging Macros
    // =====================================================================
    
    // MANDATORY MACROS FOR CLASS-ORIENTED LOGGING
    #define __LOG_DECLARATION          __INTERNAL_LOG_DECLARATION
    #define __LOG_CONSTRUCT(a,b)       __INTERNAL_LOG_CONSTRUCT(a,b)
    #define __LOG_DESTRUCT             __INTERNAL_LOG_DESTRUCT
    
    // MANDATORY MACROS FOR STATIC FUNCTION LOGGING    
    #define __LOG_STATIC_ENTER(a,b)          __INTERNAL_LOG_STATIC_ENTER(a,b)
    #define __LOG_STATIC_EXIT                __INTERNAL_LOG_STATIC_EXIT
    #define __LOG_STATIC_ENTER_SUPPRESS(a,b) __INTERNAL_LOG_STATIC_ENTER_SUPPRESS(a,b)
    #define __LOG_STATIC_EXIT_SUPPRESS       __INTERNAL_LOG_STATIC_EXIT_SUPPRESS
    
    // OPTIONAL MACROS    
    #define __LOG_CLOSE_BETWEEN_WRITES __INTERNAL_LOG_CLOSE_BETWEEN_WRITES
    #define __LOG_ENTER(a)             __INTERNAL_LOG_ENTER(a)
    #define __LOG_ENTER_SUPPRESS(a)    __INTERNAL_LOG_ENTER_SUPPRESS(a)
    #define __LOG_EXIT                 __INTERNAL_LOG_EXIT
    #define __LOG_EXIT_FORMAT1(fmt,a)  __INTERNAL_LOG_EXIT_FORMAT1(fmt,a)
    #define __LOG_EXIT8_FORMAT1(fmt,a) __INTERNAL_LOG_EXIT8_FORMAT1(fmt,a)    

    // =====================================================================
    // ERROR Level Logging Macros
    // =====================================================================
    #define __LOG_WRITE_ERROR(a)                         __INTERNAL_LOG_WRITE_ERROR(a)

    #define __LOG_WRITE_FORMAT1_ERROR(a,b)               __INTERNAL_LOG_WRITE_FORMAT1_ERROR(a,b)
    #define __LOG_WRITE_FORMAT2_ERROR(a,b,c)             __INTERNAL_LOG_WRITE_FORMAT2_ERROR(a,b,c)
    #define __LOG_WRITE_FORMAT3_ERROR(a,b,c,d)           __INTERNAL_LOG_WRITE_FORMAT3_ERROR(a,b,c,d)
    #define __LOG_WRITE_FORMAT4_ERROR(a,b,c,d,e)         __INTERNAL_LOG_WRITE_FORMAT4_ERROR(a,b,c,d,e)
    #define __LOG_WRITE_FORMAT5_ERROR(a,b,c,d,e,f)       __INTERNAL_LOG_WRITE_FORMAT5_ERROR(a,b,c,d,e,f)
    #define __LOG_WRITE_FORMAT6_ERROR(a,b,c,d,e,f,g)     __INTERNAL_LOG_WRITE_FORMAT6_ERROR(a,b,c,d,e,f,g)
    #define __LOG_WRITE_FORMAT7_ERROR(a,b,c,d,e,f,g,h)   __INTERNAL_LOG_WRITE_FORMAT7_ERROR(a,b,c,d,e,f,g,h)
    #define __LOG_WRITE_FORMAT8_ERROR(a,b,c,d,e,f,g,h,i) __INTERNAL_LOG_WRITE_FORMAT8_ERROR(a,b,c,d,e,f,g,h,i)

    // 8-bit formatted strings
    #define __LOG_WRITE8_FORMAT1_ERROR(a,b)               __INTERNAL_LOG_WRITE8_FORMAT1_ERROR(a,b)
    #define __LOG_WRITE8_FORMAT2_ERROR(a,b,c)             __INTERNAL_LOG_WRITE8_FORMAT2_ERROR(a,b,c)
    #define __LOG_WRITE8_FORMAT3_ERROR(a,b,c,d)           __INTERNAL_LOG_WRITE8_FORMAT3_ERROR(a,b,c,d)
    #define __LOG_WRITE8_FORMAT4_ERROR(a,b,c,d,e)         __INTERNAL_LOG_WRITE8_FORMAT4_ERROR(a,b,c,d,e)
    #define __LOG_WRITE8_FORMAT5_ERROR(a,b,c,d,e,f)       __INTERNAL_LOG_WRITE8_FORMAT5_ERROR(a,b,c,d,e,f)
    #define __LOG_WRITE8_FORMAT6_ERROR(a,b,c,d,e,f,g)     __INTERNAL_LOG_WRITE8_FORMAT6_ERROR(a,b,c,d,e,f,g)
    #define __LOG_WRITE8_FORMAT7_ERROR(a,b,c,d,e,f,g,h)   __INTERNAL_LOG_WRITE8_FORMAT7_ERROR(a,b,c,d,e,f,g,h)
    #define __LOG_WRITE8_FORMAT8_ERROR(a,b,c,d,e,f,g,h,i) __INTERNAL_LOG_WRITE8_FORMAT8_ERROR(a,b,c,d,e,f,g,h,i)

    #define __LOG_HEX_DUMP_ERROR(a,buffer)                __INTERNAL_LOG_HEX_DUMP_ERROR(a,buffer)

    // =====================================================================
    // INFO Level Logging Macros
    // =====================================================================
    #define __LOG_WRITE_INFO(a)                         __INTERNAL_LOG_WRITE_INFO(a)

    #define __LOG_WRITE_FORMAT1_INFO(a,b)               __INTERNAL_LOG_WRITE_FORMAT1_INFO(a,b)
    #define __LOG_WRITE_FORMAT2_INFO(a,b,c)             __INTERNAL_LOG_WRITE_FORMAT2_INFO(a,b,c)
    #define __LOG_WRITE_FORMAT3_INFO(a,b,c,d)           __INTERNAL_LOG_WRITE_FORMAT3_INFO(a,b,c,d)
    #define __LOG_WRITE_FORMAT4_INFO(a,b,c,d,e)         __INTERNAL_LOG_WRITE_FORMAT4_INFO(a,b,c,d,e)
    #define __LOG_WRITE_FORMAT5_INFO(a,b,c,d,e,f)       __INTERNAL_LOG_WRITE_FORMAT5_INFO(a,b,c,d,e,f)
    #define __LOG_WRITE_FORMAT6_INFO(a,b,c,d,e,f,g)     __INTERNAL_LOG_WRITE_FORMAT6_INFO(a,b,c,d,e,f,g)
    #define __LOG_WRITE_FORMAT7_INFO(a,b,c,d,e,f,g,h)   __INTERNAL_LOG_WRITE_FORMAT7_INFO(a,b,c,d,e,f,g,h)
    #define __LOG_WRITE_FORMAT8_INFO(a,b,c,d,e,f,g,h,i) __INTERNAL_LOG_WRITE_FORMAT8_INFO(a,b,c,d,e,f,g,h,i)

    // 8-bit formatted strings
    #define __LOG_WRITE8_FORMAT1_INFO(a,b)               __INTERNAL_LOG_WRITE8_FORMAT1_INFO(a,b)
    #define __LOG_WRITE8_FORMAT2_INFO(a,b,c)             __INTERNAL_LOG_WRITE8_FORMAT2_INFO(a,b,c)
    #define __LOG_WRITE8_FORMAT3_INFO(a,b,c,d)           __INTERNAL_LOG_WRITE8_FORMAT3_INFO(a,b,c,d)
    #define __LOG_WRITE8_FORMAT4_INFO(a,b,c,d,e)         __INTERNAL_LOG_WRITE8_FORMAT4_INFO(a,b,c,d,e)
    #define __LOG_WRITE8_FORMAT5_INFO(a,b,c,d,e,f)       __INTERNAL_LOG_WRITE8_FORMAT5_INFO(a,b,c,d,e,f)
    #define __LOG_WRITE8_FORMAT6_INFO(a,b,c,d,e,f,g)     __INTERNAL_LOG_WRITE8_FORMAT6_INFO(a,b,c,d,e,f,g)
    #define __LOG_WRITE8_FORMAT7_INFO(a,b,c,d,e,f,g,h)   __INTERNAL_LOG_WRITE8_FORMAT7_INFO(a,b,c,d,e,f,g,h)
    #define __LOG_WRITE8_FORMAT8_INFO(a,b,c,d,e,f,g,h,i) __INTERNAL_LOG_WRITE8_FORMAT8_INFO(a,b,c,d,e,f,g,h,i)

    #define __LOG_HEX_DUMP_INFO(a,buffer)                __INTERNAL_LOG_HEX_DUMP_INFO(a,buffer)

    // =====================================================================
    // DEBUG1 Level Logging Macros
    // =====================================================================
    #define __LOG_WRITE_DEBUG1(a)                         __INTERNAL_LOG_WRITE_DEBUG1(a)

    #define __LOG_WRITE_FORMAT1_DEBUG1(a,b)               __INTERNAL_LOG_WRITE_FORMAT1_DEBUG1(a,b)
    #define __LOG_WRITE_FORMAT2_DEBUG1(a,b,c)             __INTERNAL_LOG_WRITE_FORMAT2_DEBUG1(a,b,c)
    #define __LOG_WRITE_FORMAT3_DEBUG1(a,b,c,d)           __INTERNAL_LOG_WRITE_FORMAT3_DEBUG1(a,b,c,d)
    #define __LOG_WRITE_FORMAT4_DEBUG1(a,b,c,d,e)         __INTERNAL_LOG_WRITE_FORMAT4_DEBUG1(a,b,c,d,e)
    #define __LOG_WRITE_FORMAT5_DEBUG1(a,b,c,d,e,f)       __INTERNAL_LOG_WRITE_FORMAT5_DEBUG1(a,b,c,d,e,f)
    #define __LOG_WRITE_FORMAT6_DEBUG1(a,b,c,d,e,f,g)     __INTERNAL_LOG_WRITE_FORMAT6_DEBUG1(a,b,c,d,e,f,g)
    #define __LOG_WRITE_FORMAT7_DEBUG1(a,b,c,d,e,f,g,h)   __INTERNAL_LOG_WRITE_FORMAT7_DEBUG1(a,b,c,d,e,f,g,h)
    #define __LOG_WRITE_FORMAT8_DEBUG1(a,b,c,d,e,f,g,h,i) __INTERNAL_LOG_WRITE_FORMAT8_DEBUG1(a,b,c,d,e,f,g,h,i)

    // 8-bit formatted strings
    #define __LOG_WRITE8_FORMAT1_DEBUG1(a,b)               __INTERNAL_LOG_WRITE8_FORMAT1_DEBUG1(a,b)
    #define __LOG_WRITE8_FORMAT2_DEBUG1(a,b,c)             __INTERNAL_LOG_WRITE8_FORMAT2_DEBUG1(a,b,c)
    #define __LOG_WRITE8_FORMAT3_DEBUG1(a,b,c,d)           __INTERNAL_LOG_WRITE8_FORMAT3_DEBUG1(a,b,c,d)
    #define __LOG_WRITE8_FORMAT4_DEBUG1(a,b,c,d,e)         __INTERNAL_LOG_WRITE8_FORMAT4_DEBUG1(a,b,c,d,e)
    #define __LOG_WRITE8_FORMAT5_DEBUG1(a,b,c,d,e,f)       __INTERNAL_LOG_WRITE8_FORMAT5_DEBUG1(a,b,c,d,e,f)
    #define __LOG_WRITE8_FORMAT6_DEBUG1(a,b,c,d,e,f,g)     __INTERNAL_LOG_WRITE8_FORMAT6_DEBUG1(a,b,c,d,e,f,g)
    #define __LOG_WRITE8_FORMAT7_DEBUG1(a,b,c,d,e,f,g,h)   __INTERNAL_LOG_WRITE8_FORMAT7_DEBUG1(a,b,c,d,e,f,g,h)
    #define __LOG_WRITE8_FORMAT8_DEBUG1(a,b,c,d,e,f,g,h,i) __INTERNAL_LOG_WRITE8_FORMAT8_DEBUG1(a,b,c,d,e,f,g,h,i)

    #define __LOG_HEX_DUMP_DEBUG1(a,buffer)                __INTERNAL_LOG_HEX_DUMP_DEBUG1(a,buffer)

    // =====================================================================
    // DEBUG2 Level Logging Macros
    // =====================================================================
    #define __LOG_WRITE_DEBUG2(a)                         __INTERNAL_LOG_WRITE_DEBUG2(a)

    #define __LOG_WRITE_FORMAT1_DEBUG2(a,b)               __INTERNAL_LOG_WRITE_FORMAT1_DEBUG2(a,b)
    #define __LOG_WRITE_FORMAT2_DEBUG2(a,b,c)             __INTERNAL_LOG_WRITE_FORMAT2_DEBUG2(a,b,c)
    #define __LOG_WRITE_FORMAT3_DEBUG2(a,b,c,d)           __INTERNAL_LOG_WRITE_FORMAT3_DEBUG2(a,b,c,d)
    #define __LOG_WRITE_FORMAT4_DEBUG2(a,b,c,d,e)         __INTERNAL_LOG_WRITE_FORMAT4_DEBUG2(a,b,c,d,e)
    #define __LOG_WRITE_FORMAT5_DEBUG2(a,b,c,d,e,f)       __INTERNAL_LOG_WRITE_FORMAT5_DEBUG2(a,b,c,d,e,f)
    #define __LOG_WRITE_FORMAT6_DEBUG2(a,b,c,d,e,f,g)     __INTERNAL_LOG_WRITE_FORMAT6_DEBUG2(a,b,c,d,e,f,g)
    #define __LOG_WRITE_FORMAT7_DEBUG2(a,b,c,d,e,f,g,h)   __INTERNAL_LOG_WRITE_FORMAT7_DEBUG2(a,b,c,d,e,f,g,h)
    #define __LOG_WRITE_FORMAT8_DEBUG2(a,b,c,d,e,f,g,h,i) __INTERNAL_LOG_WRITE_FORMAT8_DEBUG2(a,b,c,d,e,f,g,h,i)

    // 8-bit formatted strings
    #define __LOG_WRITE8_FORMAT1_DEBUG2(a,b)               __INTERNAL_LOG_WRITE8_FORMAT1_DEBUG2(a,b)
    #define __LOG_WRITE8_FORMAT2_DEBUG2(a,b,c)             __INTERNAL_LOG_WRITE8_FORMAT2_DEBUG2(a,b,c)
    #define __LOG_WRITE8_FORMAT3_DEBUG2(a,b,c,d)           __INTERNAL_LOG_WRITE8_FORMAT3_DEBUG2(a,b,c,d)
    #define __LOG_WRITE8_FORMAT4_DEBUG2(a,b,c,d,e)         __INTERNAL_LOG_WRITE8_FORMAT4_DEBUG2(a,b,c,d,e)
    #define __LOG_WRITE8_FORMAT5_DEBUG2(a,b,c,d,e,f)       __INTERNAL_LOG_WRITE8_FORMAT5_DEBUG2(a,b,c,d,e,f)
    #define __LOG_WRITE8_FORMAT6_DEBUG2(a,b,c,d,e,f,g)     __INTERNAL_LOG_WRITE8_FORMAT6_DEBUG2(a,b,c,d,e,f,g)
    #define __LOG_WRITE8_FORMAT7_DEBUG2(a,b,c,d,e,f,g,h)   __INTERNAL_LOG_WRITE8_FORMAT7_DEBUG2(a,b,c,d,e,f,g,h)
    #define __LOG_WRITE8_FORMAT8_DEBUG2(a,b,c,d,e,f,g,h,i) __INTERNAL_LOG_WRITE8_FORMAT8_DEBUG2(a,b,c,d,e,f,g,h,i)

    #define __LOG_HEX_DUMP_DEBUG2(a,buffer)                __INTERNAL_LOG_HEX_DUMP_DEBUG2(a,buffer)

    // =====================================================================
    // DEBUG3 Level Logging Macros
    // =====================================================================
    #define __LOG_WRITE_DEBUG3(a)                         __INTERNAL_LOG_WRITE_DEBUG3(a)

    #define __LOG_WRITE_FORMAT1_DEBUG3(a,b)               __INTERNAL_LOG_WRITE_FORMAT1_DEBUG3(a,b)
    #define __LOG_WRITE_FORMAT2_DEBUG3(a,b,c)             __INTERNAL_LOG_WRITE_FORMAT2_DEBUG3(a,b,c)
    #define __LOG_WRITE_FORMAT3_DEBUG3(a,b,c,d)           __INTERNAL_LOG_WRITE_FORMAT3_DEBUG3(a,b,c,d)
    #define __LOG_WRITE_FORMAT4_DEBUG3(a,b,c,d,e)         __INTERNAL_LOG_WRITE_FORMAT4_DEBUG3(a,b,c,d,e)
    #define __LOG_WRITE_FORMAT5_DEBUG3(a,b,c,d,e,f)       __INTERNAL_LOG_WRITE_FORMAT5_DEBUG3(a,b,c,d,e,f)
    #define __LOG_WRITE_FORMAT6_DEBUG3(a,b,c,d,e,f,g)     __INTERNAL_LOG_WRITE_FORMAT6_DEBUG3(a,b,c,d,e,f,g)
    #define __LOG_WRITE_FORMAT7_DEBUG3(a,b,c,d,e,f,g,h)   __INTERNAL_LOG_WRITE_FORMAT7_DEBUG3(a,b,c,d,e,f,g,h)
    #define __LOG_WRITE_FORMAT8_DEBUG3(a,b,c,d,e,f,g,h,i) __INTERNAL_LOG_WRITE_FORMAT8_DEBUG3(a,b,c,d,e,f,g,h,i)

    // 8-bit formatted strings
    #define __LOG_WRITE8_FORMAT1_DEBUG3(a,b)               __INTERNAL_LOG_WRITE8_FORMAT1_DEBUG3(a,b)
    #define __LOG_WRITE8_FORMAT2_DEBUG3(a,b,c)             __INTERNAL_LOG_WRITE8_FORMAT2_DEBUG3(a,b,c)
    #define __LOG_WRITE8_FORMAT3_DEBUG3(a,b,c,d)           __INTERNAL_LOG_WRITE8_FORMAT3_DEBUG3(a,b,c,d)
    #define __LOG_WRITE8_FORMAT4_DEBUG3(a,b,c,d,e)         __INTERNAL_LOG_WRITE8_FORMAT4_DEBUG3(a,b,c,d,e)
    #define __LOG_WRITE8_FORMAT5_DEBUG3(a,b,c,d,e,f)       __INTERNAL_LOG_WRITE8_FORMAT5_DEBUG3(a,b,c,d,e,f)
    #define __LOG_WRITE8_FORMAT6_DEBUG3(a,b,c,d,e,f,g)     __INTERNAL_LOG_WRITE8_FORMAT6_DEBUG3(a,b,c,d,e,f,g)
    #define __LOG_WRITE8_FORMAT7_DEBUG3(a,b,c,d,e,f,g,h)   __INTERNAL_LOG_WRITE8_FORMAT7_DEBUG3(a,b,c,d,e,f,g,h)
    #define __LOG_WRITE8_FORMAT8_DEBUG3(a,b,c,d,e,f,g,h,i) __INTERNAL_LOG_WRITE8_FORMAT8_DEBUG3(a,b,c,d,e,f,g,h,i)

    #define __LOG_HEX_DUMP_DEBUG3(a,buffer)                __INTERNAL_LOG_HEX_DUMP_DEBUG3(a,buffer)

    #define __LOG_LEAKED_OBJECTS                           __INTERNAL_LOG_LEAKED_OBJECTS

    // Allow logging only definitions and declarations
    #define __LOG_BLOCK(a)                          a
#else // _DEBUG

    // =============
    // RELEASE BUILD
    // =============

    #define __LOG_DECLARATION
    #define __LOG_CONSTRUCT(a,b)
    #define __LOG_CLOSE_BETWEEN_WRITES
    #define __LOG_DESTRUCT
    #define __LOG_ENTER(a)
    #define __LOG_ENTER_SUPPRESS(a)
    #define __LOG_STATIC_ENTER(a,b)
    #define __LOG_STATIC_ENTER_SUPPRESS(a,b)
    #define __LOG_EXIT
    #define __LOG_EXIT_FORMAT1(fmt,a)
    #define __LOG_EXIT8_FORMAT1(fmt,a)
    #define __LOG_STATIC_EXIT
    #define __LOG_STATIC_EXIT_SUPPRESS
    #define __LOG_WRITE_ERROR(a)
    #define __LOG_WRITE_FORMAT1_ERROR(a,b)
    #define __LOG_WRITE_FORMAT2_ERROR(a,b,c)
    #define __LOG_WRITE_FORMAT3_ERROR(a,b,c,d)
    #define __LOG_WRITE_FORMAT4_ERROR(a,b,c,d,e)
    #define __LOG_WRITE_FORMAT5_ERROR(a,b,c,d,e,f)
    #define __LOG_WRITE_FORMAT6_ERROR(a,b,c,d,e,f,g)
    #define __LOG_WRITE_FORMAT7_ERROR(a,b,c,d,e,f,g,h)
    #define __LOG_WRITE_FORMAT8_ERROR(a,b,c,d,e,f,g,h,i)
    #define __LOG_WRITE8_FORMAT1_ERROR(a,b)
    #define __LOG_WRITE8_FORMAT2_ERROR(a,b,c)
    #define __LOG_WRITE8_FORMAT3_ERROR(a,b,c,d)
    #define __LOG_WRITE8_FORMAT4_ERROR(a,b,c,d,e)
    #define __LOG_WRITE8_FORMAT5_ERROR(a,b,c,d,e,f)
    #define __LOG_WRITE8_FORMAT6_ERROR(a,b,c,d,e,f,g)
    #define __LOG_WRITE8_FORMAT7_ERROR(a,b,c,d,e,f,g,h)
    #define __LOG_WRITE8_FORMAT8_ERROR(a,b,c,d,e,f,g,h,i)
    #define __LOG_HEX_DUMP_ERROR(a,buffer)
    #define __LOG_WRITE_INFO(a);
    #define __LOG_WRITE_FORMAT1_INFO(a,b)
    #define __LOG_WRITE_FORMAT2_INFO(a,b,c)
    #define __LOG_WRITE_FORMAT3_INFO(a,b,c,d)
    #define __LOG_WRITE_FORMAT4_INFO(a,b,c,d,e)
    #define __LOG_WRITE_FORMAT5_INFO(a,b,c,d,e,f)
    #define __LOG_WRITE_FORMAT6_INFO(a,b,c,d,e,f,g)
    #define __LOG_WRITE_FORMAT7_INFO(a,b,c,d,e,f,g,h)
    #define __LOG_WRITE_FORMAT8_INFO(a,b,c,d,e,f,g,h,i)
    #define __LOG_WRITE8_FORMAT1_INFO(a,b)
    #define __LOG_WRITE8_FORMAT2_INFO(a,b,c)
    #define __LOG_WRITE8_FORMAT3_INFO(a,b,c,d)
    #define __LOG_WRITE8_FORMAT4_INFO(a,b,c,d,e)
    #define __LOG_WRITE8_FORMAT5_INFO(a,b,c,d,e,f)
    #define __LOG_WRITE8_FORMAT6_INFO(a,b,c,d,e,f,g)
    #define __LOG_WRITE8_FORMAT7_INFO(a,b,c,d,e,f,g,h)
    #define __LOG_WRITE8_FORMAT8_INFO(a,b,c,d,e,f,g,h,i)
    #define __LOG_HEX_DUMP_INFO(a,buffer)
    #define __LOG_WRITE_DEBUG1(a);
    #define __LOG_WRITE_FORMAT1_DEBUG1(a,b)
    #define __LOG_WRITE_FORMAT2_DEBUG1(a,b,c)
    #define __LOG_WRITE_FORMAT3_DEBUG1(a,b,c,d)
    #define __LOG_WRITE_FORMAT4_DEBUG1(a,b,c,d,e)
    #define __LOG_WRITE_FORMAT5_DEBUG1(a,b,c,d,e,f)
    #define __LOG_WRITE_FORMAT6_DEBUG1(a,b,c,d,e,f,g)
    #define __LOG_WRITE_FORMAT7_DEBUG1(a,b,c,d,e,f,g,h)
    #define __LOG_WRITE_FORMAT8_DEBUG1(a,b,c,d,e,f,g,h,i)
    #define __LOG_WRITE8_FORMAT1_DEBUG1(a,b)
    #define __LOG_WRITE8_FORMAT2_DEBUG1(a,b,c)
    #define __LOG_WRITE8_FORMAT3_DEBUG1(a,b,c,d)
    #define __LOG_WRITE8_FORMAT4_DEBUG1(a,b,c,d,e)
    #define __LOG_WRITE8_FORMAT5_DEBUG1(a,b,c,d,e,f)
    #define __LOG_WRITE8_FORMAT6_DEBUG1(a,b,c,d,e,f,g)
    #define __LOG_WRITE8_FORMAT7_DEBUG1(a,b,c,d,e,f,g,h)
    #define __LOG_WRITE8_FORMAT8_DEBUG1(a,b,c,d,e,f,g,h,i)
    #define __LOG_HEX_DUMP_DEBUG1(a,buffer)
    #define __LOG_WRITE_DEBUG2(a);
    #define __LOG_WRITE_FORMAT1_DEBUG2(a,b)
    #define __LOG_WRITE_FORMAT2_DEBUG2(a,b,c)
    #define __LOG_WRITE_FORMAT3_DEBUG2(a,b,c,d)
    #define __LOG_WRITE_FORMAT4_DEBUG2(a,b,c,d,e)
    #define __LOG_WRITE_FORMAT5_DEBUG2(a,b,c,d,e,f)
    #define __LOG_WRITE_FORMAT6_DEBUG2(a,b,c,d,e,f,g)
    #define __LOG_WRITE_FORMAT7_DEBUG2(a,b,c,d,e,f,g,h)
    #define __LOG_WRITE_FORMAT8_DEBUG2(a,b,c,d,e,f,g,h,i)
    #define __LOG_WRITE8_FORMAT1_DEBUG2(a,b)
    #define __LOG_WRITE8_FORMAT2_DEBUG2(a,b,c)
    #define __LOG_WRITE8_FORMAT3_DEBUG2(a,b,c,d)
    #define __LOG_WRITE8_FORMAT4_DEBUG2(a,b,c,d,e)
    #define __LOG_WRITE8_FORMAT5_DEBUG2(a,b,c,d,e,f)
    #define __LOG_WRITE8_FORMAT6_DEBUG2(a,b,c,d,e,f,g)
    #define __LOG_WRITE8_FORMAT7_DEBUG2(a,b,c,d,e,f,g,h)
    #define __LOG_WRITE8_FORMAT8_DEBUG2(a,b,c,d,e,f,g,h,i)
    #define __LOG_HEX_DUMP_DEBUG2(a,buffer)
    #define __LOG_WRITE_DEBUG3(a);
    #define __LOG_WRITE_FORMAT1_DEBUG3(a,b)
    #define __LOG_WRITE_FORMAT2_DEBUG3(a,b,c)
    #define __LOG_WRITE_FORMAT3_DEBUG3(a,b,c,d)
    #define __LOG_WRITE_FORMAT4_DEBUG3(a,b,c,d,e)
    #define __LOG_WRITE_FORMAT5_DEBUG3(a,b,c,d,e,f)
    #define __LOG_WRITE_FORMAT6_DEBUG3(a,b,c,d,e,f,g)
    #define __LOG_WRITE_FORMAT7_DEBUG3(a,b,c,d,e,f,g,h)
    #define __LOG_WRITE_FORMAT8_DEBUG3(a,b,c,d,e,f,g,h,i)
    #define __LOG_WRITE8_FORMAT1_DEBUG3(a,b)
    #define __LOG_WRITE8_FORMAT2_DEBUG3(a,b,c)
    #define __LOG_WRITE8_FORMAT3_DEBUG3(a,b,c,d)
    #define __LOG_WRITE8_FORMAT4_DEBUG3(a,b,c,d,e)
    #define __LOG_WRITE8_FORMAT5_DEBUG3(a,b,c,d,e,f)
    #define __LOG_WRITE8_FORMAT6_DEBUG3(a,b,c,d,e,f,g)
    #define __LOG_WRITE8_FORMAT7_DEBUG3(a,b,c,d,e,f,g,h)
    #define __LOG_WRITE8_FORMAT8_DEBUG3(a,b,c,d,e,f,g,h,i)
    #define __LOG_HEX_DUMP_DEBUG3(a,buffer)
    #define __LOG_LEAKED_OBJECTS
    // Allow logging only definitions and declarations
    #define __LOG_BLOCK(a)

#endif  // _DEBUG

#endif  // _DEBUG_LOG_MACROS_H_

// END FILE DebugLogMacros.h

