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
* Description:  Trace macro declarations.
*
*/


#ifndef CNTDEBUG_H
#define CNTDEBUG_H

#include <QDebug>   // QDebug
#include <QtGlobal> // qDebug()

// #define TRACK_MEMORY_LEAKS
// #define TRACES
// #define TRACE2FILE

/*!
    \def CNT_UNUSED(name)
    \brief Declares a single variable as unused when tracing is disabled.

    CNT_UNUSED allows variables (usually method parameters) to be declared as used only if
    tracing is enabled. Without this variables that are included in trace macros, but not otherwise
    used, would cause unused variable warnings on compilation. If tracing is enabled, CNT_UNUSED
    has no effect.

    Consider the following class method where the parameter number is not used at all, except for
    tracing its value on entry:

    \code
    #include <cntdebug.h>

    void MyClass::myMethod(int number)
    {
        CNT_UNUSED(number)
        CNT_ENTRY("number =" << number)

        // ...some more code where the parameter number is not used.

        CNT_EXIT
    }
    \endcode

    Compiling this method with tracing completely disabled at compile time would cause an unused
    variable warning to be issued unless the CNT_UNUSED macro is used to declare the variable as
    trace-only.

    \param name The name of the variable to declare as unused if tracing is disabled. To mark
    several variables as trace-only, add a separate CNT_UNUSED statement for each.
    \sa CNT_STATIC_ENTRY_ARGS(args), CNT_ENTRY_ARGS(args).
*/

/*!
    \def CNT_STATIC_ENTRY
    \brief A method entry trace macro for static class methods or global functions.

    Invoking CNT_STATIC_ENTRY outputs a timestamp followed by the scope in which the macro
    was invoked and the word "entry".

    CNT_STATIC_ENTRY is intended to be used as the first line of static class methods or
    global functions. There is a corresponding exit macro, CNT_EXIT.

    The following example shows proper usage of the CNT_STATIC_ENTRY macro. Assuming a class
    has been declared with a static method that is implemented like this:

    \code
    #include <cntdebug.h>

    void MyClass::myStaticMethod()
    {
        CNT_STATIC_ENTRY

        int i = 1;
        i++;

        CNT_EXIT
    }
    \endcode

    calling MyClass::myStaticMethod() generates output lines of the following format:

    \code
    2009-03-25 11:00:50.171 : static void MyClass::myStaticMethod() entry
    2009-03-25 11:00:50.171 : static void MyClass::myStaticMethod() exit
    \endcode

    \sa CNT_STATIC_ENTRY_ARGS(args), CNT_EXIT.
*/

/*!
    \def CNT_STATIC_ENTRY_ARGS(args)
    \brief A method entry trace macro with arguments for static class methods or global functions.

    CNT_STATIC_ENTRY_ARGS(args) is similar to CNT_STATIC_ENTRY but it allows arguments to be
    output on the same line without needing to resort to a separate CNT_LOG_ARGS call. This is
    especially handy for outputting the parameters of the method call.

    The following example shows proper usage of the CNT_STATIC_ENTRY_ARGS(args) macro. Assuming
    a class has been declared with a static method that is implemented like this:

    \code
    #include <QString>
    #include <cntdebug.h>

    void MyClass::myStaticMethod(const QString &text, int number)
    {
        CNT_STATIC_ENTRY_ARGS("text =" << text << "number =" << number);

        int i = 1;
        i++;

        CNT_EXIT
    }
    \endcode

    calling MyClass::myStaticMethod(QString("foo"), 74) generates output lines of the following
    format:

    \code
    2009-03-25 11:00:50.171 : static void MyClass::myStaticMethod(const QString&, int) entry, text = "foo" number = 74
    2009-03-25 11:00:50.171 : static void MyClass::myStaticMethod(const QString&, int) exit
    \endcode

    \param args Any number of arguments that can be streamed into a QTextStream, joined together
    by the streaming operator <<.
    \sa CNT_STATIC_ENTRY.
*/

/*!
    \def CNT_ENTRY
    \brief A method entry trace macro for class methods.

    Invoking CNT_ENTRY outputs a timestamp followed by the scope in which the macro
    was invoked, the word "entry" and the this pointer value of the instance invoking the
    macro.

    The this pointer value included in the debug output can help make the output more readable, as
    it allows different instances of the same class to be distinguished from each other.

    CNT_ENTRY is intended to be used as the first line of class methods. There is a corresponding
    exit macro, CNT_EXIT.

    The following example shows proper usage of the CNT_ENTRY macro. Assuming a class has been
    declared with a non-static method that is implemented like this:

    \code
    #include <cntdebug.h>

    void MyClass::myMethod()
    {
        CNT_ENTRY

        int i = 1;
        i++;

        CNT_EXIT
    }
    \endcode

    calling myMethod() on an instance of MyClass generates output lines of the following format:

    \code
    2009-03-25 11:00:50.171 : void MyClass::myMethod() this 0x6cdab90 entry
    2009-03-25 11:00:50.171 : void MyClass::myMethod() exit
    \endcode

    \sa CNT_ENTRY_ARGS(args), CNT_EXIT.
*/

/*!
    \def CNT_ENTRY_ARGS(args)
    \brief A method entry trace macro with arguments for class methods.

    CNT_ENTRY_ARGS(args) is similar to CNT_ENTRY but it allows arguments to be output on the
    same line without needing to resort to a separate CNT_LOG_ARGS call. This is especially
    handy for outputting the parameters of the method call.

    The following example shows proper usage of the CNT_ENTRY_ARGS(args)) macro. Assuming a
    class has been declared with a non-static method that is implemented like this:

    \code
    #include <QString>
    #include <cntdebug.h>

    void MyClass::myMethod(const QString &text, int number)
    {
        CNT_ENTRY_ARGS("text =" << text << "number =" << number);

        int i = 1;
        i++;

        CNT_EXIT
    }
    \endcode

    calling myMethod(QString("foo"), 74) on an instance of MyClass generates output lines of the
    following format:

    \code
    2009-03-25 11:00:50.171 : void MyClass::myMethod(const QString&, int) this 0x6cdab90 entry, text = "foo" number = 74
    2009-03-25 11:00:50.171 : void MyClass::myMethod(const QString&, int) exit
    \endcode

    \param args Any number of arguments that can be streamed into a QTextStream, joined together
    by the streaming operator <<.
    \sa CNT_ENTRY, CNT_EXIT.
*/

/*!
    \def CNT_EXIT
    \brief A method exit trace macro for class methods or global functions.

    Invoking CNT_EXIT outputs a timestamp followed by the scope in which the macro
    was invoked and the word "exit".

    CNT_EXIT is intended to be used as the last line of class methods and global functions,
    just before the return statement, if any. There are two corresponding entry macros,
    CNT_ENTRY and CNT_STATIC_ENTRY, depending on whether the method being traced is a
    non-static or a static class method. CNT_EXIT makes no distinction between these two types
    of methods and is to be used for both.

    See CNT_ENTRY or CNT_STATIC_ENTRY for an example of how to use CNT_EXIT.

    \sa CNT_EXIT_ARGS(args), CNT_ENTRY, CNT_STATIC_ENTRY.
*/

/*!
    \def CNT_EXIT_ARGS(args)
    \brief A method exit trace macro with arguments for class methods or global functions.

    CNT_EXIT_ARGS(args) is similar to CNT_EXIT but it allows arguments to be output on the
    same line without needing to resort to a separate CNT_LOG_ARGS call. This is especially
    handy for outputting the return value of the method call.

    The following example shows proper usage of the CNT_EXIT_ARGS(args) macro. Assuming a
    class has been declared with a static method that is implemented like this:

    \code
    #include <QString>
    #include <cntdebug.h>

    int MyClass::myStaticMethod(const QString &text)
    {
        CNT_STATIC_ENTRY_ARGS("text =" << text);

        int length = text.length();

        CNT_EXIT_ARGS("length" << length);

        return length;
    }
    \endcode

    calling MyClass::myStaticMethod(QString("foo")) generates output lines of the following format:

    \code
    2009-03-25 13:20:36.448 : static int MyClass::myStaticMethod(const QString&) entry, text = "foo"
    2009-03-25 13:20:36.448 : static int MyClass::myStaticMethod(const QString&) exit, length 3
    \endcode

    Although the example above is a static method, CNT_EXIT_ARGS(args) works identically for
    non-static class methods and global functions.

    \param args Any number of arguments that can be streamed into a QTextStream, joined together
    by the streaming operator <<.
    \sa CNT_EXIT
*/

/*!
    \def CNT_LOG
    \brief A trace macro for class methods or global functions.

    Invoking CNT_LOG outputs a timestamp followed by the scope in which the macro
    was invoked and the this pointer value of the instance invoking the
    macro.

    CNT_LOG is similar to CNT_ENTRY but it is especially handy for marking calls to methods that
    cannot fail, such as an empty constructor, without needing to resort to a separate CNT_EXIT call.

    The following example shows proper usage of the CNT_LOG(args) macro. Assuming a
    class has been declared with a static method that is implemented like this:

    \code
    #include <QString>
    #include <cntdebug.h>

    MyClass::MyClass()
    {
        CNT_LOG
    }
    \endcode

    calling new MyClass() generates output lines of the following format:

    \code
    2009-03-25 13:20:36.448 : MyClass::MyClass() this 0x6cdab90
    \endcode

    \sa CNT_LOG_ARGS
*/

/*!
    \def CNT_LOG_ARGS(args)
    \brief A generic trace macro with arguments for class methods or global functions.

    The following example shows how to produce arbitrary debug output:

    \code
    #include <QString>
    #include <cntdebug.h>

    void MyClass::myMethod()
    {
        CNT_ENTRY

        QString myString("This is a string.");
        int myValue = 109;

        CNT_LOG_ARGS("this is a debug message, myString =" << myString << "myValue =" << myValue)

        CNT_EXIT
    }
    \endcode

    calling myMethod() on an instance of MyClass generates output lines of the following format:

    \code
    2009-03-25 13:45:22.083 : void MyClass::myMethod() this 0x6cdab90 entry
    2009-03-25 13:45:22.083 : void MyClass::myMethod() this is a debug message, myString = "This is a string." myValue = 109
    2009-03-25 13:45:22.083 : void MyClass::myMethod() exit
    \endcode

    Any number of arguments may be printed by chaining them together with the streaming operator
    <<. Notice that a single space character is automatically added between each streamed
    argument, hence the hardcoded strings in the example above, such as "myValue =", do not have
    a space at the beginning and end of the string. This automatic space addition is a feature
    of qDebug() streaming and cannot be disabled.

    \param args Any number of arguments that can be streamed into a QTextStream, joined together
    by the streaming operator <<.
*/

// traces for debug builds by default, or if
// TRACES macro is defined
#if defined (_DEBUG) && !defined (TRACES)
#define TRACES
#endif

#if defined (TRACES) || defined (TRACE2FILE)
    #define CNT_UNUSED(name)
    #define CNT_STATIC_ENTRY qDebug() << __PRETTY_FUNCTION__ << "entry";
    #define CNT_STATIC_ENTRY_ARGS(args) qDebug() << __PRETTY_FUNCTION__ << "entry," << args;
    #define CNT_ENTRY qDebug() << __PRETTY_FUNCTION__ << "this" << (void *)this << "entry";
    #define CNT_ENTRY_ARGS(args) qDebug() << __PRETTY_FUNCTION__ << "this" << (void *)this << "entry," << args;
    #define CNT_EXIT qDebug() << __PRETTY_FUNCTION__ << "exit";
    #define CNT_EXIT_ARGS(args) qDebug() << __PRETTY_FUNCTION__ << "exit," << args;
    #define CNT_LOG qDebug() << __PRETTY_FUNCTION__ << "this" << (void *)this;
    #define CNT_LOG_ARGS(args) qDebug() << __PRETTY_FUNCTION__ << args;
    #define CNT_WARNING(args) qWarning() << __PRETTY_FUNCTION__ << args;
    #define CNT_CRITICAL(args) qCritical() << __PRETTY_FUNCTION__ << args;
    #define CNT_FATAL(args) qFatal() << __PRETTY_FUNCTION__ << args;
#else
    #define CNT_UNUSED(name) Q_UNUSED(name)
    #define CNT_STATIC_ENTRY
    #define CNT_STATIC_ENTRY_ARGS(args)
    #define CNT_ENTRY
    #define CNT_ENTRY_ARGS(args)
    #define CNT_EXIT
    #define CNT_EXIT_ARGS(args)
    #define CNT_LOG
    #define CNT_LOG_ARGS(args)
    #define CNT_WARNING(args)
    #define CNT_CRITICAL(args)
    #define CNT_FATAL(args)
#endif // TRACES || TRACE2FILE
    
// for tracing memory leaks
#ifdef TRACK_MEMORY_LEAKS
    #include <hbapplication.h>

    #define CNT_TRACK_QOBJECTLIFE(obj) { new CntQObjectTracker(obj, __FILE__, __LINE__); }
    #define CNT_TRACK_QOBJECTLIVES(objects) { foreach (QObject* object, objects) new CntQObjectTracker(object, __FILE__, __LINE__); }

    class CntQObjectTracker : public QObject
    {
        Q_OBJECT
        public:
            CntQObjectTracker(QObject *obj, QString fileName, int fileLine) {
                objectName = obj->metaObject()->className();
                createdInFile = fileName;
                createdAtLine = fileLine;
                connect(obj, SIGNAL(destroyed()), this, SLOT(objectDestroyed()));
                connect(HbApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(reportMemoryLeak()));
            }
        public slots:
            void objectDestroyed() { delete this; }
        private slots:
            void reportMemoryLeak() { qDebug() << "MEMORY LEAK: The" << objectName << "object in" << createdInFile << "at line" << createdAtLine << "was never destroyed."; delete this; }
        private: // functions
            ~CntQObjectTracker() {}
        private: // data
            QString  objectName;
            QString  createdInFile;
            int      createdAtLine;
    };
#else
    #define CNT_TRACK_QOBJECTLIFE(obj)
    #define CNT_TRACK_QOBJECTLIVES(obj)
#endif

// filter phonebook app traces
#ifdef TRACE2FILE
    #include <QFile>
    #include <QTextStream>
    static void cntCustomLog2File(QtMsgType type, const char *msg)
    {   
        QFile logFile("c:/cnt_logs.log");
        if (!logFile.open(QIODevice::Append | QIODevice::Text))
        {
            qFatal("error opening c:/cnt_logs.log file");
            return;
        }

        QTextStream out(&logFile);
        switch (type)
        {
            case QtDebugMsg:
                out << "[CNT] Debug: " << msg;
                break;
            case QtWarningMsg:
                out << "[CNT] Warning: " << msg;
                break;
            case QtCriticalMsg:
                out << "[CNT] Critical: " << msg;
                break;
            case QtFatalMsg:
                out << "[CNT] Fatal: " << msg;
                abort();
                break;
            default:
                out << "[CNT] No Log Selection Type: " << msg;
                break;
        }
    }
    #define MSG_HANDLER cntCustomLog2File
#else
    #ifdef Q_OS_SYMBIAN
        #include <e32debug.h>
        static void cntCustomLog(QtMsgType type, const char *msg)
        {
            switch (type) {
                case QtDebugMsg:
                    RDebug::Printf("[CNT] Debug: %s\n", msg);
                    break;
                case QtWarningMsg:
                    RDebug::Printf("[CNT] Warning: %s\n", msg);
                    break;
                case QtCriticalMsg:
                    RDebug::Printf("[CNT] Critical: %s\n", msg);
                    break;
                case QtFatalMsg:
                    RDebug::Printf("[CNT] Fatal: %s\n", msg);
                    abort();
                    break;
                default:
                    break;
            }
        }
        #define MSG_HANDLER cntCustomLog
    #else
        #define MSG_HANDLER 0
    #endif  // Q_OS_SYMBIAN
#endif  // TRACE2FILE

#endif // CNTDEBUG_H
