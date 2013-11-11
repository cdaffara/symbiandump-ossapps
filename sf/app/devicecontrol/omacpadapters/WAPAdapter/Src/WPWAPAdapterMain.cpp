/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DLL entry point
*
*/



// INCLUDE FILES
#include <e32debug.h>
#include <e32std.h>
#include <flogger.h>
#include "WPWAPDebug.h"

// CONSTANTS

#ifdef WAPDEBUG
#ifdef WINS
/// Folder where the log resides
_LIT( KLogFolder, "provisioning" );

/// The name of the log file
_LIT( KLogFileName, "wapadapter" );

/// The format in which the time is formatted in log
_LIT( KLogTimeFormat, "%02d.%02d:%02d:%06d ");

/// The length of the string produced by KLogTimeFormat
const TInt KLogTimeFormatLength = 16;

/// How many characters a log line can contain
const TInt KLogLineLength = 256;
#endif
#endif 

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// Debug
// -----------------------------------------------------------------------------
//
#ifdef WAPDEBUG
GLDEF_C void Debug( TRefByValue<const TDesC> aText, ... )
    {
    #ifdef WINS
    VA_LIST args;
    VA_START( args, aText );

    TBuf<KLogLineLength> buf;
    buf.FormatList( aText, args );

    RFileLogger logger;
    TInt ret=logger.Connect();
    if (ret==KErrNone)
        {
        logger.SetDateAndTime( EFalse,EFalse );
        logger.CreateLog( KLogFolder, KLogFileName, EFileLoggingModeAppend );       
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
    #else
    RDebug::Print(aText);
    #endif
    }
#else
GLDEF_C void Debug( TRefByValue<const TDesC> /*aText*/, ... )
    {
    }
#endif

//  End of File  
