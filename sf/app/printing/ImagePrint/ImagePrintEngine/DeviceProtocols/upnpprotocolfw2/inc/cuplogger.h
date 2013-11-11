/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares logging macros and CUPLogger class
*
*/


#ifndef CUPLOGGER_H
#define CUPLOGGER_H

#include "logdef.h"

/**
 *  NOTE: Link to flogger.lib in MMP file. Use DEBUGLIBRARY -keyword to avoid
 *        warnings in release builds.
 *        Example:  DEBUGLIBRARY    flogger.lib
 */

/**
 *  Usage:  LOG("[MODULE_NAME]\t Trace text here");
 *
 *          Trace target can be changed below (file logging needs directory c:\logs\upnp)
 *          #define __FLOGGING -row uncommented (default)  = File logging
 *          #define __CLOGGING -row uncommented            = Console logging
 */

#ifdef ENABLE_LOGGING
// Define one of these flags:
// FLOGGING = File logging
// CLOGGING = Console logging
#define __FLOGGING__
//#define __CLOGGING__
#endif



// Then actual definitions depending on the 
// flag values.

#ifdef ENABLE_LOGGING

    #include <e32std.h>
    #include <f32file.h>


    // Actual log file name
    _LIT(KLogFile,"UALog.txt");

    // Subdirectory under c:\logs -directory -- MUST EXIST.
    _LIT(KLogDir, "upnp");
    
	/// The format in which the time is formatted in log
	_LIT( KLogTimeFormat, "%02d.%02d:%02d:%06d ");
	_LIT8( KLogTimeFormat8, "%02d.%02d:%02d:%06d ");

	/// The length of the string produced by KLogTimeFormat
	const TInt KLogTimeFormatLength = 16;

	/// How many characters a log line can contain
	const TInt KLogLineLength = 256;
    

    #include <f32file.h>
    #include <flogger.h>


    // Define the top level macros
    #define LOGS(a1, a2) {\
    	TBuf<1000> log;\
    	log.Append(_L(a1));\
		log.Append(a2);\
		Print(log );}
    #define LOG8S(a1, a2) {\
    	TBuf8<1000> log;\
    	log.Append(_L8(a1));\
		log.Append(a2);\
		Print8(log );}

    #define LOG(a) {Print( _L(a) ) ;}
   	#define LOG1(s, v) {Print( _L(s), v );}
   	#define LOG2(s, v1, v2) {Print( _L(s), v1, v2 );}
   	
    #define LOG8(a) {Print8( _L(a) );}
   	#define LOG81(s, v) {Print8( _L8(s), v );}
   	#define LOG82(s, v1, v2) {Print8( _L8(s), v1, v2 );}

    #ifdef __FLOGGING__

        inline void Print( TRefByValue<const TDesC> aText, ... )
            {
		    VA_LIST args;
		    VA_START( args, aText );

		    TBuf<256> buf;
		    buf.FormatList( aText, args );

			RFileLogger logger;
			TInt ret = logger.Connect();
			if (ret==KErrNone)
				{
				logger.SetDateAndTime( EFalse,EFalse );
				logger.CreateLog( KLogDir, KLogFile, EFileLoggingModeAppend );		
				TBuf<KLogTimeFormatLength> timeStamp;
				TTime now;
				now.HomeTime();
				TDateTime dateTime;
				dateTime = now.DateTime();
				timeStamp.Format( KLogTimeFormat, 
		            dateTime.Hour(), dateTime.Minute(),
		            dateTime.Second(), dateTime.MicroSecond() );
				buf.Insert( 0, timeStamp );

				logger.Write(buf);
				}

			logger.Close();
		    VA_END( args );
            }

        inline void Print8( TRefByValue<const TDesC8> aText, ... )
            {
		    VA_LIST args;
		    VA_START( args, aText );

		    TBuf8<256> buf;
		    buf.FormatList( aText, args );

			RFileLogger logger;
			TInt ret = logger.Connect();
			if (ret==KErrNone)
				{
				logger.SetDateAndTime( EFalse,EFalse );
				logger.CreateLog( KLogDir, KLogFile, EFileLoggingModeAppend );		
				TBuf8<KLogTimeFormatLength> timeStamp;
				TTime now;
				now.HomeTime();
				TDateTime dateTime;
				dateTime = now.DateTime();
				timeStamp.Format( KLogTimeFormat8, 
		            dateTime.Hour(), dateTime.Minute(),
		            dateTime.Second(), dateTime.MicroSecond() );
				buf.Insert( 0, timeStamp );

				logger.Write(buf);
				}

			logger.Close();
		    VA_END( args );
            }
            
    #else
        // Console Logging on
        #define Print RDebug::Print

	    #define LOGS(a1, a2) {\
	    	TBuf<1000> log;\
	    	log.Append(_L(a1));\
			log.Append(a2);\
			Print(log );}

	    #define LOG8S(a1, a2) {\
	    	TBuf8<1000> log;\
	    	log.Append(_L8(a1));\
			log.Append(a2);\
			Print(log );}
    #endif  // __FLOGGING__

#else

    // DEBUG build is not on --> no logging at all
    #define LOG(a)
    #define LOG8S(a1, a2)

   	#define LOG1(s, v)
   	#define LOG2(s, v1, v2)
   	
    #define LOG8(a)
   	#define LOG81(s, v)
   	#define LOG82(s, v1, v2)

#endif  // ENABLE_LOGGING

#endif  // CUPLOGGER_H
            
// End of File
