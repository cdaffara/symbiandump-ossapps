/*
* Copyright (c) 2004 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Video telephony engine debugging support.
*
*/



#ifndef CVTLOGGER_H
#define CVTLOGGER_H

//  INCLUDES
#include <e32base.h>
#include <flogger.h>
#include <f32file.h>
#include "vtlogger.hrh"

// CONSTANTS
const TInt KVtDebugBufferSize = 100;
const TInt KVtDebugReturnValNotApplicable = -1;

// MACROS

#define DEBUG_CONSTRUCT CVtLogger::ELogConstruction
#define DEBUG_DESTRUCT CVtLogger::ELogDestruction
#define DEBUG_MEDIA CVtLogger::ELogMedia
#define DEBUG_AUDIO CVtLogger::ELogAudio
#define DEBUG_SESSION CVtLogger::ELogSession
#define DEBUG_DETAIL CVtLogger::ELogDetails
#define DEBUG_ALL CVtLogger::ELogLogEverything
#define DEBUG_GEN CVtLogger::ELogGeneric
#define DEBUG_RETURN CVtLogger::ELogReturn

#define DEBUG_FLAG1 CVtLogger::ELogApp1
#define DEBUG_FLAG2 CVtLogger::ELogApp2
#define DEBUG_FLAG3 CVtLogger::ELogApp3
#define DEBUG_FLAG4 CVtLogger::ELogApp4

#if defined(VTDEBUG) && defined(__CS_VIDEO_TELEPHONY)
/**
* Write a text buffer to log.
*/
#define __VTPRINT(level,x) CVtLogger::Print(level, _L(x));

/**
* Write a text buffer and integer value to log.
*/
#define __VTPRINT2(level,x,y) CVtLogger::Print(level, _L(x), y);

/**
* Write a text buffer and two integer values to log.
*/
#define __VTPRINT3(level,x,y,z) CVtLogger::Print(level, _L(x), y, z);

#define __VTPRINTENTER(x) CVtLogger::Print( CVtLogger::ELogGeneric, ETrue, _L(x));

#define __VTPRINTEXIT(x) CVtLogger::Print( CVtLogger::ELogGeneric, EFalse, _L(x) );

#define __VTPRINTEXITR(x,y) CVtLogger::Print( CVtLogger::ELogGeneric, EFalse, _L(x), y );

/**
* Initialize VT logging, must be called before any __VTPRINT's are called.
*/
#define VTLOGINIT CVtLogger::Initialize();

/**
* Uninitializes VT logging, must be called client using logging is deleted.
*/
#define VTLOGUNINIT CVtLogger::Uninitialize();

/**
* Returns logging and configuration flags
*/
#define VTFLAGS CVtLogger::Flags();

/**
* check if baseband loopback is enabled
*/
#define DEBUG_CFG_BB_LOOPBACK ( CVtLogger::Flags() & CVtLogger::ECfgBBLoopbackEnabled )

/**
* check if PV engine loopback is enabled
*/
#define DEBUG_CFG_PV_LOOPBACK ( CVtLogger::Flags() & CVtLogger::ECfgPVLoopbackEnabled )

/**
* Disables passing of audio source to PV
*/
#define DEBUG_CFG_NO_AUDIO_SOURCE ( CVtLogger::Flags() & CVtLogger::ECfgNoAudioSource )

/**
* Disables passing of audio source to PV
*/
#define DEBUG_CFG_NO_AUDIO_SINK ( CVtLogger::Flags() & CVtLogger::ECfgNoAudioSink )

/**
* Disables passing of video sink to PV
*/
#define DEBUG_CFG_NO_VIDEO_SINK ( CVtLogger::Flags() & CVtLogger::ECfgNoVideoSink )

/**
* Disables passing of video source to PV
*/
#define DEBUG_CFG_NO_VIDEO_SOURCE ( CVtLogger::Flags() & CVtLogger::ECfgNoVideoSource )

#else

/**
* Disables VT Debug mode if __CS_VIDEO_TELEPHONY is undefined
*/
#undef VTDEBUG

#define __VTPRINT(level,x) 
#define __VTPRINT2(level,x,y) 
#define __VTPRINT3(level,x,y,z) 
#define __VTPRINTENTER(x)
#define __VTPRINTEXIT(x)
#define __VTPRINTEXITR(x,y)
#define VTLOGINIT
#define VTLOGUNINIT
#define VTFLAGS 0
#define DEBUG_CFG_BB_LOOPBACK 0
#define DEBUG_CFG_PV_LOOPBACK 0
#define DEBUG_CFG_NO_AUDIO_SINK 0
#define DEBUG_CFG_NO_AUDIO_SOURCE 0
#define DEBUG_CFG_NO_VIDEO_SINK 0
#define DEBUG_CFG_NO_VIDEO_SOURCE 0

#endif // VTDEBUG

// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
*  Debbuging support.
*  To enable logging (either with RDebug or RFileLogger) follow steps below:
*  1. Create folder c:\Logs\VT
*  2. Create file VTLOGCONF.txt
*  3. Open the file and define logging level by hexadecimal digit, 
*     see TLogLevel in class declaration below. Example: 2007 means that
*     constructors (0x1), destructors (0x2 ) in media subsystem (0x4) are 
*     logged to file (0x2000), i.e. the values are bitwise or'ed. Log file
*     name is "<thread name>.txt" where <thread name> is name of the thread
*     where logging takes places.
*
*  @lib videoteleng
*  @since 2.6
*/
class CVtLogger : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CVtLogger* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CVtLogger();

    public: // New functions
        
        /**
        * Writes to log file or RDebug.
        * @param aDes text output
        */
        virtual void Print( TDesC& aDes );

        /**
        * Log level defining logging behavior.
        */
        enum TLogLevel {
            // Constructors are logged
            ELogConstruction  = 0x00000001,
            // Destructors are logged
            ELogDestruction   = 0x00000002,
            // Media subsystem methods are logged
            ELogMedia         = 0x00000004,
            // Session subsystem methods are logged
            ELogSession       = 0x00000008,
            // Audio subsystem methods are logged
            ELogAudio         = 0x00000010,
            // Details in methods are logged
            ELogDetails       = 0x00000020,
            // Generic framework methods are logged
            ELogGeneric       = 0x00000040,
            // Log return from function
            ELogReturn        = 0x00000080,
            // Everything is logged, as if all above were set.
            ELogLogEverything = 0x0000007F,
            // Insert time at the beginning of each log entry
            // For file logging time is always written because
            // it is automatically done by RFileLogger.
            ELogTime          = 0x00001000,
            // Log to file instead of using RDebug
            ELogToFile        = 0x00002000,
            // Application specific flag 1
            ELogApp1          = 0x00004000,
            // Application specific flag 2
            ELogApp2          = 0x00008000,
            // Application specific flag 3
            ELogApp3          = 0x00010000,
            // Application specific flag 4
            ELogApp4          = 0x00020000,

            // Configuration flags

            /** Loopback configuration (set only
            *   one of the following flags
            */
            // Baseband loopback enabled
            ECfgBBLoopbackEnabled = 0x00100000,
            // PV Engine loopback enabled
            ECfgPVLoopbackEnabled = 0x00200000,

            // Disable passing of audio source
            ECfgNoAudioSink       = 0x00400000,

            // Disable passing of audio sink
            ECfgNoAudioSource     = 0x00800000,

            // Disable passing of video source
            ECfgNoVideoSource     = 0x01000000,

            // Disable passing of video sink
            ECfgNoVideoSink      = 0x02000000
            };

        /**
        * Writes buffer to log.
        * @param aLevel combination of TLogLevel values
        * @param aBuffer buffer to log
        */
        IMPORT_C static void Print( 
            TInt aLevel, 
            TPtrC aBuffer );

        /**
        * Writes buffer to log.
        * @param aLevel combination of TLogLevel values
        * @param aBuffer buffer to log
        * @param aValue value appended to buffer
        */
        IMPORT_C static void Print( 
            TInt aLevel, 
            TPtrC aBuffer, 
            TInt aValue );
        
        /**
        * Writes buffer to log.
        * @param aLevel combination of TLogLevel values
        * @param aBuffer buffer to log
        * @param aValue1 first value appended to buffer
        * @param aValue2 second value appended to buffer
        */
        IMPORT_C static void Print( 
            TInt aLevel, 
            TPtrC aBuffer, 
            TInt aValue1,
            TInt aValue2 );

        /**
        * Writes buffer to log. This overloaded method is used
        * for printing entry or exit from a function.
        * @since Series60 2.8
        * @param aLevel combination of TLogLevel values
        * @param aEntry if ETrue "<" is appended to end of string
        *               if EFalse ">" is appended to end of string
        * @param aBuffer buffer to log
        * @param aRetVal return value of the function. If this value
        * is present (i.e. other than KVtDebugReturnValNotApplicable
        * is passed) then aBuffer should include format tag '%d' where
        * aRetValue should be placed.
        */
        IMPORT_C static void Print(
            TInt aLevel,
            const TBool aEntry,
            TPtrC aBuffer,
            TInt aRetVal = KVtDebugReturnValNotApplicable );
            
        /**
        * Initializes logging. This must be called before
        * any use of __VTPRINT macros.
        */
        IMPORT_C static void Initialize();

        /**
        * Uninitializes logging. This must be called when
        * client of logging service is deleted.
        */
        IMPORT_C static void Uninitialize();

        /**
        * Returns flags in configuration file
        * @return flags
        */
        IMPORT_C static TInt Flags();

    private:
    
        /**
        * Returns logger.
        * @return logger
        */
        static CVtLogger* Logger();

        /**
        * Returns if certain log levels are set.
        * @param aLevels bitmap of TLogLevel values
        * @return ETrue if all aLevels are set.
        */
        TBool CheckLevels( TInt aLevels ) const;

        /**
        * Reads configuration file and starts
        * monitoring changes on it.
        */
        void ReadCongigFile();

    private: // from CActive
        
        /**
        * Handles file change notification
        */
        void RunL();

        /**
        * Cancels monitoring file change
        */
        void DoCancel();

    private:

        /**
        * C++ default constructor.
        */
        CVtLogger();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( );

    private:    // Data        
    
        // Buffer size for printed lines
        enum 
            {
            ETempBufLength = 80
            };
            
        TFileName               iFileName;
        RFs                     iFs;
        TInt                    iLogLevel;
        TInt                    iAccessCount;
        TBuf<ETempBufLength>    iBuffer;
        TBuf<ETempBufLength>    iTempBuf;
    };


#endif      // CVTLOGGER_H
            
// End of File
