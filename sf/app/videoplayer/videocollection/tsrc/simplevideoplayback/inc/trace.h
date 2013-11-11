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
* Description:  Tracing macros, 2nd generation
*
*/

#ifndef TRACE_H
#define TRACE_H

#include <QDebug>

//-----------------------------------------------------------------------------
// Trace definitions
//-----------------------------------------------------------------------------
//

/**
* Error trace enabled
*/
#ifdef _DEBUG
    #define ERROR_TRACE
    #define INFO_TRACE
    #define TIMESTAMP_TRACE
    #define FUNC_TRACE
#else
    #define ERROR_TRACE
    #define INFO_TRACE
    #define TIMESTAMP_TRACE
    #define FUNC_TRACE
#endif

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
//

/**
* Trace prefixes for macros with component name.
*/
#define _TRACE_PREFIX "[SVPB]:"

/**
* Prefix error trace
*/
#define _ERROR_PREFIX _TRACE_PREFIX " [ERROR]:"

/**
* Prefix info trace.
*/
#define _INFO_PREFIX _TRACE_PREFIX " [INFO]:"

/**
* Prefix timestamp trace.
*/
#define _TIMESTAMP_PREFIX _TRACE_PREFIX " [TIMESTAMP]:"

//-----------------------------------------------------------------------------
// Error trace macros
// Usage: wrap traces in ERROR() macro to allow disabling them in release builds.
// Use normal stream object operations.
// Examples:
// ERROR( "xxx failed" );
// ERROR( "Test trace arg =" << 999 << "arg2 =" << title() );
//-----------------------------------------------------------------------------
//
#ifdef ERROR_TRACE
    #define ERROR(trace) {qDebug() << _ERROR_PREFIX << trace;}
    #define ERROR_PARAM(param) param
    #define HANDLE_ERROR(trace) {ERROR(trace); __BREAKPOINT();}
    #define HANDLE_ERROR_BOOL(x) {if (!x) {ERROR(#x << "is false"); __BREAKPOINT();}}
    #define HANDLE_ERROR_NULL(x) {if (!x) {ERROR(#x << "is NULL"); __BREAKPOINT();}}
    #define HANDLE_ERROR_NEG(x) {if (x < 0) {ERROR(#x << "=" << x << "File:" << __FILE__ << ", line:" << __LINE__); __BREAKPOINT();}}
    #define CHECK_ERROR(err, trace) {if (err < 0) ERROR(trace << err);}
#else //ERROR_TRACE not defined
    #define ERROR(trace)
    #define ERROR_PARAM(param)
    #define HANDLE_ERROR(trace)
    #define HANDLE_ERROR_BOOL(x)
    #define HANDLE_ERROR_NULL(x)
    #define HANDLE_ERROR_NEG(x)
    #define CHECK_ERROR(err,trace)
#endif //ERROR_TRACE

//-----------------------------------------------------------------------------
// Info trace macros
// Usage: wrap traces in INFO() macro to allow disabling them in release builds.
// Use normal stream object operations.
// Examples:
// INFO( "Test trace" );
// INFO( "Test trace arg =" << 999 << "arg2 =" << title() );
//-----------------------------------------------------------------------------
//
#ifdef INFO_TRACE
    #define INFO(trace) {qDebug() << _INFO_PREFIX << trace;}
    #define INFO_PARAM(param) param
#else //INFO_TRACE not defined
    #define INFO(trace)
    #define INFO_PARAM(param)
#endif //INFO_TRACE

//-----------------------------------------------------------------------------
// Function trace macros
//-----------------------------------------------------------------------------
//
#ifdef FUNC_TRACE

    class FuncLog
    {
    public:
    inline FuncLog( const char* func ) : m_func( func )
        { qDebug() << _TRACE_PREFIX << m_func << "-START"; }
    inline ~FuncLog() { qDebug() << _TRACE_PREFIX << m_func << "-END"; }
    private: // Data
        QString m_func;
    };

    #define FUNC_LOG FuncLog _fl( __PRETTY_FUNCTION__ );
#else //FUNC_TRACE not defined
    #define FUNC_LOG
#endif //FUNC_TRACE

//-----------------------------------------------------------------------------
// Timestamp trace macros
//-----------------------------------------------------------------------------
//
#ifdef TIMESTAMP_TRACE
    #include <QTime>
    #define TIMESTAMP(trace)\
        {qDebug() << _TIMESTAMP_PREFIX << "(" << \
            QTime::currentTime().toString("HH:mm:ss:zzz") << ")" << trace;}
#else //TIMESTAMP_TRACE not defined
    #define TIMESTAMP(trace)
#endif //TIMESTAMP_TRACE

#endif // TRACE_H
