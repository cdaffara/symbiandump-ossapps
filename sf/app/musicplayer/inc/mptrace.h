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


#ifndef MPTX_H
#define MPTX_H

#include <QDebug>   // QDebug
#include <QtGlobal> // qDebug()

/*!
    \def TX_UNUSED(name)
    \brief Declares a single variable as unused when tracing is disabled.

    TX_UNUSED allows variables (usually method parameters) to be declared as used only if
    tracing is enabled. Without this variables that are included in trace macros, but not otherwise
    used, would cause unused variable warnings on compilation. If tracing is enabled, TX_UNUSED
    has no effect.

    Consider the following class method where the parameter number is not used at all, except for
    tracing its value on entry:

    \code
    #include <mptrace.h>

    void MyClass::myMethod(int number)
    {
        TX_UNUSED(number)
        TX_ENTRY("number =" << number)

        // ...some more code where the parameter number is not used.

        TX_EXIT
    }
    \endcode

    Compiling this method with tracing completely disabled at compile time would cause an unused
    variable warning to be issued unless the TX_UNUSED macro is used to declare the variable as
    trace-only.

    \param name The name of the variable to declare as unused if tracing is disabled. To mark
    several variables as trace-only, add a separate TX_UNUSED statement for each.
    \sa TX_STATIC_ENTRY_ARGS(args), TX_ENTRY_ARGS(args).
*/

/*!
    \def TX_STATIC_ENTRY
    \brief A method entry trace macro for static class methods or global functions.

    Invoking TX_STATIC_ENTRY outputs a timestamp followed by the scope in which the macro
    was invoked and the word "entry".

    TX_STATIC_ENTRY is intended to be used as the first line of static class methods or
    global functions. There is a corresponding exit macro, TX_EXIT.

    The following example shows proper usage of the TX_STATIC_ENTRY macro. Assuming a class
    has been declared with a static method that is implemented like this:

    \code
    #include <mptrace.h>

    void MyClass::myStaticMethod()
    {
        TX_STATIC_ENTRY

        int i = 1;
        i++;

        TX_EXIT
    }
    \endcode

    calling MyClass::myStaticMethod() generates output lines of the following format:

    \code
    2009-03-25 11:00:50.171 : static void MyClass::myStaticMethod() entry
    2009-03-25 11:00:50.171 : static void MyClass::myStaticMethod() exit
    \endcode

    \sa TX_STATIC_ENTRY_ARGS(args), TX_EXIT.
*/

/*!
    \def TX_STATIC_ENTRY_ARGS(args)
    \brief A method entry trace macro with arguments for static class methods or global functions.

    TX_STATIC_ENTRY_ARGS(args) is similar to TX_STATIC_ENTRY but it allows arguments to be
    output on the same line without needing to resort to a separate TX_LOG_ARGS call. This is
    especially handy for outputting the parameters of the method call.

    The following example shows proper usage of the TX_STATIC_ENTRY_ARGS(args) macro. Assuming
    a class has been declared with a static method that is implemented like this:

    \code
    #include <QString>
    #include <mptrace.h>

    void MyClass::myStaticMethod(const QString &text, int number)
    {
        TX_STATIC_ENTRY_ARGS("text =" << text << "number =" << number);

        int i = 1;
        i++;

        TX_EXIT
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
    \sa TX_STATIC_ENTRY.
*/

/*!
    \def TX_ENTRY
    \brief A method entry trace macro for class methods.

    Invoking TX_ENTRY outputs a timestamp followed by the scope in which the macro
    was invoked, the word "entry" and the this pointer value of the instance invoking the
    macro.

    The this pointer value included in the debug output can help make the output more readable, as
    it allows different instances of the same class to be distinguished from each other.

    TX_ENTRY is intended to be used as the first line of class methods. There is a corresponding
    exit macro, TX_EXIT.

    The following example shows proper usage of the TX_ENTRY macro. Assuming a class has been
    declared with a non-static method that is implemented like this:

    \code
    #include <mptrace.h>

    void MyClass::myMethod()
    {
        TX_ENTRY

        int i = 1;
        i++;

        TX_EXIT
    }
    \endcode

    calling myMethod() on an instance of MyClass generates output lines of the following format:

    \code
    2009-03-25 11:00:50.171 : void MyClass::myMethod() this 0x6cdab90 entry
    2009-03-25 11:00:50.171 : void MyClass::myMethod() exit
    \endcode

    \sa TX_ENTRY_ARGS(args), TX_EXIT.
*/

/*!
    \def TX_ENTRY_ARGS(args)
    \brief A method entry trace macro with arguments for class methods.

    TX_ENTRY_ARGS(args) is similar to TX_ENTRY but it allows arguments to be output on the
    same line without needing to resort to a separate TX_LOG_ARGS call. This is especially
    handy for outputting the parameters of the method call.

    The following example shows proper usage of the TX_ENTRY_ARGS(args)) macro. Assuming a
    class has been declared with a non-static method that is implemented like this:

    \code
    #include <QString>
    #include <mptrace.h>

    void MyClass::myMethod(const QString &text, int number)
    {
        TX_ENTRY_ARGS("text =" << text << "number =" << number);

        int i = 1;
        i++;

        TX_EXIT
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
    \sa TX_ENTRY, TX_EXIT.
*/

/*!
    \def TX_EXIT
    \brief A method exit trace macro for class methods or global functions.

    Invoking TX_EXIT outputs a timestamp followed by the scope in which the macro
    was invoked and the word "exit".

    TX_EXIT is intended to be used as the last line of class methods and global functions,
    just before the return statement, if any. There are two corresponding entry macros,
    TX_ENTRY and TX_STATIC_ENTRY, depending on whether the method being traced is a
    non-static or a static class method. TX_EXIT makes no distinction between these two types
    of methods and is to be used for both.

    See TX_ENTRY or TX_STATIC_ENTRY for an example of how to use TX_EXIT.

    \sa TX_EXIT_ARGS(args), TX_ENTRY, TX_STATIC_ENTRY.
*/

/*!
    \def TX_EXIT_ARGS(args)
    \brief A method exit trace macro with arguments for class methods or global functions.

    TX_EXIT_ARGS(args) is similar to TX_EXIT but it allows arguments to be output on the
    same line without needing to resort to a separate TX_LOG_ARGS call. This is especially
    handy for outputting the return value of the method call.

    The following example shows proper usage of the TX_EXIT_ARGS(args) macro. Assuming a
    class has been declared with a static method that is implemented like this:

    \code
    #include <QString>
    #include <mptrace.h>

    int MyClass::myStaticMethod(const QString &text)
    {
        TX_STATIC_ENTRY_ARGS("text =" << text);

        int length = text.length();

        TX_EXIT_ARGS("length" << length);

        return length;
    }
    \endcode

    calling MyClass::myStaticMethod(QString("foo")) generates output lines of the following format:

    \code
    2009-03-25 13:20:36.448 : static int MyClass::myStaticMethod(const QString&) entry, text = "foo"
    2009-03-25 13:20:36.448 : static int MyClass::myStaticMethod(const QString&) exit, length 3
    \endcode

    Although the example above is a static method, TX_EXIT_ARGS(args) works identically for
    non-static class methods and global functions.

    \param args Any number of arguments that can be streamed into a QTextStream, joined together
    by the streaming operator <<.
    \sa TX_EXIT
*/

/*!
    \def TX_LOG
    \brief A trace macro for class methods or global functions.

    Invoking TX_LOG outputs a timestamp followed by the scope in which the macro
    was invoked and the this pointer value of the instance invoking the
    macro.

    TX_LOG is similar to TX_ENTRY but it is especially handy for marking calls to methods that
    cannot fail, such as an empty constructor, without needing to resort to a separate TX_EXIT call.

    The following example shows proper usage of the TX_LOG(args) macro. Assuming a
    class has been declared with a static method that is implemented like this:

    \code
    #include <QString>
    #include <mptrace.h>

    MyClass::MyClass()
    {
        TX_LOG
    }
    \endcode

    calling new MyClass() generates output lines of the following format:

    \code
    2009-03-25 13:20:36.448 : MyClass::MyClass() this 0x6cdab90
    \endcode

    \sa TX_LOG_ARGS
*/

/*!
    \def TX_LOG_ARGS(args)
    \brief A generic trace macro with arguments for class methods or global functions.

    The following example shows how to produce arbitrary debug output:

    \code
    #include <QString>
    #include <mptrace.h>

    void MyClass::myMethod()
    {
        TX_ENTRY

        QString myString("This is a string.");
        int myValue = 109;

        TX_LOG_ARGS("this is a debug message, myString =" << myString << "myValue =" << myValue)

        TX_EXIT
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


#ifdef _DEBUG
    #define TX_UNUSED(name)
    #define TX_STATIC_ENTRY qDebug() << __PRETTY_FUNCTION__ << "entry";
    #define TX_STATIC_ENTRY_ARGS(args) qDebug() << __PRETTY_FUNCTION__ << "entry," << args;
    #define TX_ENTRY qDebug() << __PRETTY_FUNCTION__ << "this" << (void *)this << "entry";
    #define TX_ENTRY_ARGS(args) qDebug() << __PRETTY_FUNCTION__ << "this" << (void *)this << "entry," << args;
    #define TX_EXIT qDebug() << __PRETTY_FUNCTION__ << "exit";
    #define TX_EXIT_ARGS(args) qDebug() << __PRETTY_FUNCTION__ << "exit," << args;
    #define TX_LOG qDebug() << __PRETTY_FUNCTION__ << "this" << (void *)this;
    #define TX_LOG_ARGS(args) qDebug() << __PRETTY_FUNCTION__ << args;
#else
    #define TX_UNUSED(name) Q_UNUSED(name)
    #define TX_STATIC_ENTRY
    #define TX_STATIC_ENTRY_ARGS(args)
    #define TX_ENTRY
    #define TX_ENTRY_ARGS(args)
    #define TX_EXIT
    #define TX_EXIT_ARGS(args)
    #define TX_LOG
    #define TX_LOG_ARGS(args)
#endif // _DEBUG

#endif // MPTX_H
