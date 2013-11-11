/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __DEBUG_H__
#define __DEBUG_H__

/*
#ifndef _DEBUG

#define __TIME_MARK(_var)
#define __TIME_ENDMARK(_msg, _var)
#define __TIME(_msg, _code) _code;
#define __TICK(_msg)
#define __HEAP(_msg)
#define __PRINT(_fmt, args...)
#define __PRINT8(_fmt, args...)
#define __PRINTS(_str)
#define __PRINT_IF_ERROR(_stmt, _fmt) _stmt;
#define __TRAP_AND_PRINT_IF_ERROR(_stmt, _fmt) TRAP_IGNORE(_stmt);


#else
*/
// ========== INCLUDE FILES ===================================================
#include <flogger.h>
//#pragma message "Warning: Logging enabled, binary fotprint increased!!!"

// ========== CONSTANTS =======================================================
#ifndef D_LOG_DIR
#define D_LOG_DIR _L("cal")
#endif

//
// Log file for heap usage
//
#ifndef D_LOG_FILE
#define D_LOG_FILE _L("debug.log")
#endif


// ========== LOCAL FUNCTIONS =================================================

// ----------------------------------------------------------------------------
//
// Performance logging operation, give results in microseconds 10^-6
// The metricts are based on the measurment current value of microseconds in 
// the begining of operation and in the end. The time difference is actual result
//
// The logging operations take about 5ms, which should be considered if inner
// tag are used.
//
// Examples:
//
// __TIME_MARK(t)
// DoOperation(...);
// __TIME_ENDMARK(_L("Message 1"), t);
//
// __TIME(_L("Message 1"), DoOperation(...))
//
// ----------------------------------------------------------------------------
//

//
// Actual implementation of 
// Logs the microsecond difference between current value and stored earlier
// aMsg - textual message stored to the log file
// aStart - the start time, the value is set by previous macros
//
inline void __impl_time_endmark(const TDesC& aMsg, TTime aStart)
{
	TTime now;
	now.UniversalTime();
	TTimeIntervalMicroSeconds delta = now.MicroSecondsFrom(aStart);
	RFileLogger::WriteFormat(D_LOG_DIR, D_LOG_FILE, EFileLoggingModeAppend, _L("\t[T]\t%S\tTime:\t%Ld"), &aMsg, delta.Int64());	
}

//
// Prints the message and current ms value to the log file
//
inline void __impl_tick(const TDesC& aMsg)
{
	//Gets the nanokernel tick count. This is the current value of the machine's millisecond tick counter.
    TUint32 nTick = User::NTickCount();
	RFileLogger::WriteFormat(D_LOG_DIR, D_LOG_FILE, EFileLoggingModeAppend, _L("\t[T]\t%S\tMicro Sec:\t%d000"), &aMsg, nTick);	
}

//
// Stores the current microsecond value
// _var - A name, which will be declared as a TInt, and will receive the current time value.
// After the macro, _var remains in scope until the end of its enclosing block.
//
#define __TIME_MARK(_var) TTime _var;_var.UniversalTime();

//
// Logs the microsecond difference between current value and stored earlier
// _msg - textual message stored to the log file
// _var - the start time, the value is set by previous macros
//
#define __TIME_ENDMARK(_msg, _var) __impl_time_endmark(_L(_msg), _var);

//
// Measures the execution of code statement
// _msg - textual message stored to the log file
// _code - code statements to be measured
//
#define __TIME(_msg, _code) {TTime _time;_time.UniversalTime();_code;__impl_time_endmark(_L(_msg),_time);};

//
//  the message and current ms value to the log file
//
#define __TICK(_msg) __impl_tick(_L(_msg));
// ----------------------------------------------------------------------------
//
// Logs the amout of used heap to log file. Notes that the Heap value equals
// to the result provided by Goofy tool.
//
// ----------------------------------------------------------------------------
//
inline void __impl_heap(const TDesC& aMsg)
{
	//Log Memory	
    TInt size;
    User::Heap().AllocSize(size);
    RFileLogger::WriteFormat(D_LOG_DIR, D_LOG_FILE, EFileLoggingModeAppend, _L("\t[M]\t%S\tAlloc:\t%d\tHeap:\t%d"), &aMsg, size, User::Heap().Size());	
}

#define __HEAP(_msg) __impl_heap(_L(_msg));
// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
inline void __impl_print(TRefByValue<const TDesC16> aFmt, ...)
{
	VA_LIST list;
	VA_START(list,aFmt);
	
	//Log event
    RFileLogger::WriteFormat(D_LOG_DIR, D_LOG_FILE, EFileLoggingModeAppend, aFmt, list);
}

inline void __impl_print8(TRefByValue<const TDesC8> aFmt, ...)
{
	VA_LIST list;
	VA_START(list,aFmt);
	
	//Log event
    RFileLogger::WriteFormat(D_LOG_DIR, D_LOG_FILE, EFileLoggingModeAppend, aFmt, list);
}


inline void __impl_prints(const TDesC& aMsg)
{
	RFileLogger::WriteFormat(D_LOG_DIR, D_LOG_FILE, EFileLoggingModeAppend, _L("\t[I]\t%S"), &aMsg);	
}

// It is a correct approach to define function-like macroses but GCC give
// compilatoin error.
// #define __PRINT(_fmt, ...) __impl_print(_L(_fmt), __VA_ARGS__);
//#define __PRINT8(_fmt, ...) __impl_print8(_L8(_fmt), __VA_ARGS__);
//#define __PRINT(_fmt, args...) __impl_print(_L(_fmt), args);
//#define __PRINT8(_fmt, args...) __impl_print8(_L8(_fmt), args);
#define __PRINTS(_str) __impl_prints(_L(_str));

//#define __PRINT_IF_ERROR(_stmt, _fmt) { TInt _error = _stmt; if (_error != KErrNone) __PRINT(_fmt, _error); };
//#define __TRAP_AND_PRINT_IF_ERROR(_stmt, _fmt) { TRAPD(_error, _stmt); if (_error != KErrNone) __PRINT(_fmt, _error); };

//#endif //_DEBUG

#endif
