/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef EMAILTRACE_H
#define EMAILTRACE_H

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QAbstractFileEngine>

/*
 * The macros NM_COMMENT_TRACES, NM_ERROR_TRACES, NM_FUNCTION_TRACES, and
 * NM_TIMESTAMP_TRACES control which debug messages are printed. The trace
 * logging is controlled with the NM_LOG_TO_FILE macro, whereas the
 * NM_LOG_DIRECTORY macro defines which directory is to be used to store the
 * log files. The print_trace() helper function implements printing. Log
 * files are named according to process thread IDs. If NM_LOG_TO_FILE is zero
 * or a log file cannot be opened, the messages are printed to qDebug(). The
 * DSC2STR() function can be used to convert Symbian descriptors to QString
 * objects.
 * 
 * NOTICE that you can control the tracing system for a component tree by
 * using qmake. For example, if you only want to enable timestamps for a
 * component, you can invoke
 * 
 *     qmake "DEFINES += DEBUG NM_TIMESTAMP_TRACES=1" -r
 * 
 * before compiling the component. You can add as many macro definitions
 * between the quotation marks as you wish. For example, if you want to log
 * the trace messages to "c:/logs", add the definitions
 * 
 *     NM_LOG_TO_FILE=1 NM_LOG_DIRECTORY=\\\"c:/logs/\\\"
 * 
 * between the quotation marks. Do not forget to add the terminating slash
 * character and notice how the quotation marks around the directory path
 * have been escaped. This does not work if the log directory does not exist.
 */

// Timestamp traces always ON
#ifndef NM_TIMESTAMP_TRACES
#define NM_TIMESTAMP_TRACES 1
#endif

#ifndef NM_LOG_TO_FILE
#define NM_LOG_TO_FILE   1
#endif
#ifndef NM_LOG_DIRECTORY
#define NM_LOG_DIRECTORY "c:/data/logs/"
#endif

#if defined(DEBUG) || defined(_DEBUG)

#ifndef NM_COMMENT_TRACES
#define NM_COMMENT_TRACES   0
#endif
#ifndef NM_ERROR_TRACES
#define NM_ERROR_TRACES     0
#endif
#ifndef NM_FUNCTION_TRACES
#define NM_FUNCTION_TRACES  0
#endif

#if NM_COMMENT_TRACES || NM_ERROR_TRACES || NM_FUNCTION_TRACES

inline void print_trace(const QString& msg)
{
    static QFile file(NM_LOG_DIRECTORY+
                      QString("nmail_p%1.log").
                      arg(QCoreApplication::applicationPid()));
    if (NM_LOG_TO_FILE && !file.isOpen()) {
        file.open(QIODevice::Append | QIODevice::Text);
    }
    if (file.isWritable()) {
        QTextStream(&file) << msg << '\n';
    } else {
        qDebug("[Nmail] %s",msg.toLatin1().data());
    }
}

#endif

inline QString DSC2STR(const TDesC& dsc)
{
    return QString::fromRawData(reinterpret_cast<const QChar*>(dsc.Ptr()),
                                dsc.Length());
}

#else

#define DSC2STR(dsc)

#endif /* DEBUG */

/*
 * The function NM_COMMENT() prints a debug message. The INFO macros and the
 * NMLOG macro are provided for legacy compatibility. They are deprecated and
 * should not be used. If sprintf() type of formatting is desired, consider
 * using QString::arg() or QTextStream.
 */
#if NM_COMMENT_TRACES

inline void NM_COMMENT(const QString& msg)
{
    print_trace("COMMENT : " + msg);
}
#define INFO(msg) NM_COMMENT(msg)
#define INFO_1(msg,arg1)\
do {\
    QString __msg;\
    __msg.sprintf(msg,arg1);\
    NM_COMMENT(__msg);\
} while (0)
#define INFO_2(msg,arg1,arg2)\
do {\
    QString __msg;\
    __msg.sprintf(msg,arg1,arg2);\
    NM_COMMENT(__msg);\
} while (0)
#define INFO_3(msg,arg1,arg2,arg3)\
do {\
    QString __msg;\
    __msg.sprintf(msg,arg1,arg2,arg3);\
    NM_COMMENT(__msg);\
} while (0)
#define NMLOG(msg) NM_COMMENT(msg)

#else

#define NM_COMMENT(msg)
#define INFO(msg)
#define INFO_1(msg,arg1)
#define INFO_2(msg,arg1,arg2)
#define INFO_3(msg,arg1,arg2,arg3)
#define NMLOG(msg)

#endif /* NM_COMMENT_TRACES */

/*
 * The function NM_ERROR() prints its second argument if the first argument
 * is non-zero. The ERROR macros are provided for legacy compatibility. They
 * are deprecated and should not be used. If sprintf() type of formatting is
 * desired, consider using QString::arg() or QTextStream.
 */
#if NM_ERROR_TRACES

inline void NM_ERROR(int err, const QString& msg)
{
    if (err) {
        print_trace("ERROR : " + msg);
    }
}
#define ERROR(err,msg) NM_ERROR(err,msg)
#define ERROR_1(err,msg,arg1)\
do {\
    QString __msg;\
    __msg.sprintf(msg,arg1);\
    NM_ERROR(err,__msg);\
} while (0)
#define ERROR_2(err,msg,arg1,arg2)\
do {\
    QString __msg;\
    __msg.sprintf(msg,arg1,arg2);\
    NM_ERROR(err,__msg);\
} while(0)
#define ERROR_3(err,msg,arg1,arg2,arg3)\
do {\
    QString __msg;\
    __msg.sprintf(msg,arg1,arg2,arg3);\
    NM_ERROR(err,__msg);\
} while(0)
#define ERROR_GEN(msg) ERROR(KErrGeneral,msg)
#define ERROR_GEN_1(msg,arg1) ERROR_1(KErrGeneral,msg,arg1)

#else

#define NM_ERROR(err,msg)
#define ERROR(err,msg)
#define ERROR_1(err,msg,arg1)
#define ERROR_2(err,msg,arg1,arg2)
#define ERROR_3(err,msg,arg1,arg2,arg3)
#define ERROR_GEN(msg)
#define ERROR_GEN_1(msg,arg1)

#endif /* NM_ERROR_TRACES */

/*
 * The macro NM_FUNCTION, when used inside a function body, enables tracing
 * for a function. If used, it should be placed on the first line of the
 * function body. ENTER and RETURN messages are printed when entering into
 * and returning from a function, respectively. In case of an exception,
 * UNWIND (for stack unwinding) is printed instead of RETURN. The FUNC_LOG
 * macro is provided for legacy compatibility. It is deprecated and should
 * not be used.
 */
#if NM_FUNCTION_TRACES

class __ftracer
{
public:
    __ftracer(const QString& _fn)
    : fn(_fn)
    {
        print_trace("ENTER : " + fn);
    }
    ~__ftracer()
    {
        if (std::uncaught_exception()) {
            print_trace("UNWIND : " + fn);
        } else {
            print_trace("RETURN : " + fn);
        }
    }
private:
    QString fn;
};

#define NM_FUNCTION __ftracer __ft(__PRETTY_FUNCTION__)
#define FUNC_LOG NM_FUNCTION

#else

#define NM_FUNCTION
#define FUNC_LOG

#endif /* NM_FUNCTION_TRACES */

/*
 * The function NM_TIMESTAMP() prints a timestamp with a message. These traces do not
 * go to class specific log -files, but to one file called nmail_timestamps.log.
 */
#if NM_TIMESTAMP_TRACES

inline void NM_TIMESTAMP(const QString& msg)
{ 
    QString timestamp ("TIMESTAMP : " + 
                       QDateTime::currentDateTime().toUTC().toString("hh:mm:ss.zzz ") + 
                       msg);
    
    QDir directory("c:/data/");
            if (directory.exists()) {
                directory.mkdir("logs");
            }
            
    static QFile file(NM_LOG_DIRECTORY +
                      QString("nmail_timestamps.log"));
    if (NM_LOG_TO_FILE && !file.isOpen()) {
        file.open(QIODevice::Append | QIODevice::Text | QIODevice::ReadWrite);
    }
    if (file.isWritable()) {
        QTextStream(&file) << timestamp << '\n';
    } else {
        qDebug("[Nmail] %s",timestamp.toLatin1().data());
    }
    if(file.isOpen()) {
        file.close();
    }
}

#else

#define NM_TIMESTAMP(msg)

#endif /* NM_TIMESTAMP_TRACES */

#endif /* EMAILTRACE_H */
