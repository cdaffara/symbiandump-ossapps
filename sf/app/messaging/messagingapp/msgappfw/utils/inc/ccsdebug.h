/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CS Server Debug macros
 *
*/

#ifndef __C_CS_DEBUG_H__
#define __C_CS_DEBUG_H__

// INCLUDE FILES

// SYSTEM INCLUDES
#include <e32svr.h>
#include <e32std.h>
#include <f32file.h>
#include <eikenv.h>
#include <flogger.h>

// USER INCLUDES

// FORWARD DECLARATIONS

/**
 * MAX_BUFF_LENGTH.
 * maximum buffer used for logging 
 * 
 */
#define MAX_BUFF_LENGTH     255

/**
 * TIME_FACTOR.
 * time factor used to convert latency time into milli seconds
 * 
 */
#define TIME_FACTOR     1000

/**
 * KLogDrive.
 * Log Drive
 */
_LIT(KLogDrive, "c");
/**
 * KLogDirFullName.
 * Log Directory Full Name
 */
_LIT(KLogDirFullName, "\\logs\\");

/**
 * KLogDir.
 * CS Log Directory 
 */
_LIT(KLogDir, "Cs");

/**
 * KLogDir.
 * CS Log Directory 
 */
_LIT(KLogFile, "CsServer.txt");

/**
 * KLogBanner.
 * CS Log Banner 
 */
_LIT( KLogBanner, "CONVERSATION SERVER");

/**
 * KCSTimeFormatString.
 * Time formatting string. 
 */
_LIT(KCSTimeFormatString,   "%H:%T:%S:%*C2");

/**
 * Debug Marcos 
 */
#ifdef _DEBUG

/**
 * Debug Marcos for WINS.
 */
#ifdef __WINS__

/**
 * PRINT.
 * Debug Marcos to print text.
 * 
 * @param txt Text to be printed on console
 */
#define PRINT( txt )                    RDebug::Print( txt );

/**
 * PRINT1.
 * Debug Marco to print text and arg1.
 * 
 * @param txt Text to be printed on console
 * @param arg1 Argument to be printed on console
 */
#define PRINT1( txt, arg1 )             RDebug::Print( txt, arg1 );

/**
 * PRINT2.
 * Debug Marco to print text, arg1 and arg2.
 * 
 * @param txt Text to be printed on console
 * @param arg1 Argument to be printed on console
 * @param arg2 Argument to be printed on console 
 */
#define PRINT2( txt, arg1, arg2 )       RDebug::Print( txt, arg1, arg2 );

/**
 * CONSERVER_CREATE.
 * Debug Marco to create folder and file to log debug txt.
 * Not needed for WINS.
 */
#define CONSERVER_CREATE

#else

/**
 * Debug Marcos for ARM
 */

/**
 * PRINT.
 * Debug Marco to log text.
 * 
 * @param txt Text to be logged in file
 */
#define PRINT( txt ) RFileLogger::Write(KLogDir,KLogFile, \
    EFileLoggingModeAppend,txt);

/**
 * PRINT1.
 * Debug Marco to log text and arg1
 * 
 * @param txt Text to be logged in file
 * @param arg1 Argument to be logged in file
 */
#define PRINT1( txt, arg1 )             RFileLogger::WriteFormat(KLogDir,     \
    KLogFile,EFileLoggingModeAppend,     \
    TRefByValue<const TDesC>(txt),arg1);

/**
 * PRINT2.
 * Debug Marco to log text, arg1 and arg2.
 * 
 * @param txt Text to be logged in file
 * @param arg1 Argument to be logged in file
 * @param arg2 Argument to be logged in file 
 */
#define PRINT2( txt, arg1, arg2 )       RFileLogger::WriteFormat(KLogDir,      \
    KLogFile,EFileLoggingModeAppend,   \
    TRefByValue<const TDesC>(txt),arg1,arg2);

/**
 * CONSERVER_CREATE.
 * Debug Marco to create folder and file to log debug txt.
 */
#define CONSERVER_CREATE                {FCsCreate( );}

/**
 * FCsCreate.
 * Function creates folder and file to log debug txt.
 */
inline void FCsCreate( )
    {
    TFileName path(KLogDrive);
    path.Append(_L(":"));
    path.Append(KLogDirFullName);
    path.Append(KLogDir);
    path.Append(_L("\\"));
    RFs& fs = CEikonEnv::Static()->FsSession();
    fs.MkDirAll(path);
    RFileLogger::WriteFormat(KLogDir, KLogFile, EFileLoggingModeOverwrite, KLogBanner);
    }
#endif // __WINS__

#else
/**
 * Empty Debug Marcos 
 */

/**
 * PRINT 
 * Do nothing
 */
#define PRINT( txt )

/**
 * PRINT1
 * Do nothing
 */
#define PRINT1( txt, arg1 )

/**
 * PRINT2 
 * Do nothing
 */
#define PRINT2( txt, arg1, arg2 )

/**
 * CONSERVER_CREATE 
 * Do nothing
 */

/**
 * CONSERVER_CREATE.
 * Debug Marco to create folder and file to log debug txt.
 */
#define CONSERVER_CREATE                {FCsCreate( );}

/**
 * FCsCreate.
 * Function creates folder and file to log debug txt.
 */
inline void FCsCreate( )
    {
    TFileName path(KLogDrive);
    path.Append(_L(":"));
    path.Append(KLogDirFullName);
    path.Append(KLogDir);
    path.Append(_L("\\"));
    RFs& fs = CEikonEnv::Static()->FsSession();
    fs.MkDirAll(path);
    RFileLogger::WriteFormat(KLogDir, KLogFile, EFileLoggingModeOverwrite, KLogBanner);
    }

#endif // _DEBUG

/**
 * __LATENCY_MARK.
 * Marco Marks the start time for latency measurement
 * 
 * @param str String to be logged
 */
#define __LATENCY_MARK( str )     CCsDebugWrapper::__LatencyMarkStartL(str);

/**
 * __LATENCY_MARKEND.
 * Marco Marks the end time for latency measurement
 * 
 * @param str String to be logged
 */
#define __LATENCY_MARKEND( str )  CCsDebugWrapper::__LatencyMarkEnd(str);

/**
 * PRINT_LATENCY.
 * Debug Marco to log text.
 * 
 * @param txt Text to be logged in file
 */
#define PRINT_LATENCY( txt ) RFileLogger::Write(KLogDir,KLogFile, \
    EFileLoggingModeAppend,txt);

/**
 * PRINT1.
 * Debug Marco to log text and arg1
 * 
 * @param txt Text to be logged in file
 * @param arg1 Argument to be logged in file
 */
#define PRINT_LATENCY1( txt, arg1 )             RFileLogger::WriteFormat(KLogDir,     \
    KLogFile,EFileLoggingModeAppend,     \
    TRefByValue<const TDesC>(txt),arg1);

/**
 * PRINT_LATENCY2.
 * Debug Marco to log text, arg1 and arg2.
 * 
 * @param txt Text to be logged in file
 * @param arg1 Argument to be logged in file
 * @param arg2 Argument to be logged in file 
 */
#define PRINT_LATENCY2( txt, arg1, arg2 )       RFileLogger::WriteFormat(KLogDir,      \
    KLogFile,EFileLoggingModeAppend,   \
    TRefByValue<const TDesC>(txt),arg1,arg2);

#define PRINT_TIMESTAMP(a) {_LIT(temp, a); \
                            TTime time; time.HomeTime(); \
                            TBuf<256> buffer; \
                            time.FormatL( buffer, KCSTimeFormatString ); \
                            buffer.Insert(0, temp); \
                            RFileLogger::WriteFormat(KLogDir,   \
                                    KLogFile,   \
                                    EFileLoggingModeAppend, \
                                    buffer); }

// CLASS DECLARATION

/**
 * CCsDebug
 * Debug Class
 * 
 * @remarks Lib: CsUtils.lib
 * @since S60 v5.0
 */
class CCsDebug : public CBase
    {

    public: // Constructor and New functions

        /**
         * NewL.
         * Two-phased constructor.
         *
         * @return A pointer to the created instance of CCsDebug.
         */
        IMPORT_C static CCsDebug* NewL();

        /**
         * Mark.
         * Marks the start time for latency measurement.
         */
        IMPORT_C void Mark(TRefByValue<const TDesC> str,...);

        /**
         * UnMark.
         * Unmarks the start time for latency measurement.
         */
        IMPORT_C void UnMark(TRefByValue<const TDesC> str,...);

    private: // Constructor

        /**
         * ConstructL.
         * Performs the second phase construction of a
         * CCsDebug object.
         */
        void ConstructL();

    private: // Data

        /**
         * startTime.
         * Start Time, when measurement is started
         * Own.
         */
        TTime startTime;

        /**
         * endTime.
         * End Time, when measurement is ended
         */
        TTime endTime;
    };

/**
 * CCsDebugArr
 * Debug Array
 * 
 * @remarks Lib: CsUtils.lib
 * @since S60 v5.0
 */
class CCsDebugArr : public CBase
    {

public: // Constructor and New functions

    /**
     * NewL.
     * Two-phased constructor.
     *
     * @return A pointer to the created instance of CCsDebugArr.
     */
    IMPORT_C static CCsDebugArr* NewL();

    /**
     * Push.
     * Push an element into the array
     *
     * @param dbg Reference to instance of CCsDebug.
     */
    IMPORT_C void Push(CCsDebug& dbg);

    /**
     * Pop.
     * Pop an element from the array
     *
     * @return A pointer to the instance of CCsDebug.
     */    
    IMPORT_C CCsDebug* Pop();

    /**
     * IsEmpty.
     * Check if array is empty.
     *
     * @return ETure if array is empty, otherwise EFalse.
     */    
    IMPORT_C TBool IsEmpty();

public: // Destructor

    /**
     * ~CCsDebugArr.
     * Destructor.
     * Destroys the object and release all memory objects.
     */
    ~CCsDebugArr();

private: // data

    /**
     * debugArray.
     * Array of pointer to CCsDebug objects.  
     * Own.
     */
    RPointerArray<CCsDebug> debugArray;
    };

/**
 * CCsDebugWrapper
 * Debug Wrapper
 * 
 * @remarks Lib: CsUtils.lib
 * @since S60 v5.0
 */
class CCsDebugWrapper : public CBase 
    {
    
public: //New functions
    
    /**
     * __LatencyMarkStartL.
     * Marks the start time for latency measurement.
     */
     IMPORT_C static void __LatencyMarkStartL(TRefByValue<const TDesC> str);
     
     /**
      * __LatencyMarkEnd.
      * Marks the end time for latency measurement.
      */
     IMPORT_C static void __LatencyMarkEnd(TRefByValue<const TDesC> str);
    };

#endif // __C_CS_DEBUG_H__

// End of File
