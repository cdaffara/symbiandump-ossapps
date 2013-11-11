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

#ifndef __LOGGER_H__
#define __LOGGER_H__

#ifdef _DEBUG
    #define LOGGING_ENABLED
#endif
#ifdef LOGGING_ENABLED          // This must be enabled to use logging system

#define LOGGER_LOGGING          // Log to Logger



#ifdef LOGGER_LOGGING

//  INCLUDES
#include <flogger.h>

// LOG SETTINGS 
_LIT( KPnpLogFolder, "PnP" );
_LIT( KPnpLogFile, "PnPUtil.TXT" );

#endif

// CONSTANTS  
// None.

// MACROS
/*
-----------------------------------------------------------------------------

    INTERNAL MACROs. 

    DO NOT USE THESE DIRECTLY !!! 
    SEE EXTERNAL MACROS

-----------------------------------------------------------------------------
*/

#ifdef LOGGER_LOGGING

_LIT( KTextFormat, "%S%S" );
_LIT( KPnpUtil, "PnpUtil| " );

#define INTRLOGTEXT( AAA )                                                                                        \
    {                                                                                                            \
    RFileLogger::Write( KPnpLogFolder(), KPnpLogFile(), EFileLoggingModeAppend, AAA );            \
    }
#define INTRLOGSTRING( AAA )                                                                                    \
    {                                                                                                            \
    _LIT( tempLogDes, AAA );                                                                                    \
    RFileLogger::Write( KPnpLogFolder(), KPnpLogFile(), EFileLoggingModeAppend, tempLogDes() );    \
    }
// 20 chars is left for the formatted item
#define INTRLOGSTRING2( AAA, BBB )                                                                                \
    {                                                                                                            \
    _LIT( tempLogDes, AAA );                                                                                    \
    RFileLogger::WriteFormat( KPnpLogFolder(), KPnpLogFile(), EFileLoggingModeAppend, TRefByValue<const TDesC>( tempLogDes()), BBB ); \
    }
// 40 chars is left for the formatted items
#define INTRLOGSTRING3( AAA, BBB, CCC )                                                                            \
    {                                                                                                            \
    _LIT( tempLogDes, AAA );                                                                                    \
    RFileLogger::WriteFormat( KPnpLogFolder(), KPnpLogFile(), EFileLoggingModeAppend, TRefByValue<const TDesC>( tempLogDes()), BBB, CCC ); \
    }
#else
#define INTRLOGTEXT( AAA )
#define INTRLOGSTRING( AAA )
#define INTRLOGSTRING2( AAA, BBB )
#define INTRLOGSTRING3( AAA, BBB, CCC )
#endif

/*
-----------------------------------------------------------------------------

    EXTERNAL MACROs

    USE THESE MACROS IN YOUR CODE !

-----------------------------------------------------------------------------
*/


#define LOGTEXT( AAA )                { \
                                    INTRLOGTEXT( AAA ); \
                                    }   // Example: LOGTEXT( own_desc );

#define LOGSTRING( AAA )            { \
                                    INTRLOGSTRING( AAA ); \
                                    }   // Example: LOGSTRING( "Test" );

#define LOGSTRING2( AAA, BBB )        { \
                                    INTRLOGSTRING2( AAA, BBB ); \
                                    }  // Example: LOGSTRING( "Test %i", aValue );

#define LOGSTRING3( AAA, BBB, CCC )    { \
                                    INTRLOGSTRING3( AAA, BBB, CCC ); \
                                    } // Example: LOGSTRING( "Test %i %i", aValue1, aValue2 );


#else   // LOGGING_ENABLED

#define LOGTEXT( AAA ) 
#define LOGSTRING( AAA ) 
#define LOGSTRING2( AAA, BBB )    
#define LOGSTRING3( AAA, BBB, CCC )    

#endif  // LOGGING_ENABLED

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
// None.

// CLASS DECLARATION
// None.

#endif  // __LOGGER_H__
