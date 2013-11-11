/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file defines logging interface macros
*
*/


#ifndef __LOGGER_H__
#define __LOGGER_H__

// Set logging on only for debug builds as Paos filter is a critical component
// when considering browser performance
#ifdef _DEBUG
    #define LOGGING_ENABLED
#endif
#ifdef LOGGING_ENABLED          // This must be enabled to use logging system

#define LOGGER_LOGGING          // Log to Logger



#ifdef LOGGER_LOGGING

//  INCLUDES
#include <flogger.h>

// LOG SETTINGS 
_LIT( KLogFolder, "PnP" );
_LIT( KLogFile, "ServicePlugin.TXT" );

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


#define INTRLOGTEXT( AAA )                                                                                        \
    {                                                                                                            \
    RFileLogger::Write( KLogFolder(), KLogFile(), EFileLoggingModeAppend, AAA );            \
    }
#define INTRLOGSTRING( AAA )                                                                                    \
    {                                                                                                            \
    _LIT( tempLogDes, AAA );                                                                                    \
    RFileLogger::Write( KLogFolder(), KLogFile(), EFileLoggingModeAppend, tempLogDes() );    \
    }
#define INTRLOGSTRING2( AAA, BBB )                                                                                \
    {                                                                                                            \
    _LIT( tempLogDes, AAA );                                                                                    \
    RFileLogger::WriteFormat( KLogFolder(), KLogFile(), EFileLoggingModeAppend, TRefByValue<const TDesC>( tempLogDes()), BBB ); \
    }
#define INTRLOGSTRING3( AAA, BBB, CCC )                                                                            \
    {                                                                                                            \
    _LIT( tempLogDes, AAA );                                                                                    \
    RFileLogger::WriteFormat( KLogFolder(), KLogFile(), EFileLoggingModeAppend, TRefByValue<const TDesC>( tempLogDes()), BBB, CCC ); \
    }

#define INTRLOGRSTRING( AAA, BBB ) \
    { \
    TPtrC8 string8 = BBB.DesC(); \
    HBufC* buf = HBufC::NewLC( string8.Length() ); \
    buf->Des().Copy( string8 ); \
    LOGSTRING2( AAA, buf ); \
    CleanupStack::PopAndDestroy( buf ); \
    }

#else
#define INTRLOGTEXT( AAA )
#define INTRLOGSTRING( AAA )
#define INTRLOGSTRING2( AAA, BBB )
#define INTRLOGSTRING3( AAA, BBB, CCC )
#define INTRLOGRSTRING( AAA, BBB )
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

#define LOGRSTRING( AAA, BBB )    { \
                                    INTRLOGRSTRING( AAA, BBB ); \
                                    } // Example: LOGRSTRING( "Test %i", RString1 );

#else   // LOGGING_ENABLED

#define LOGTEXT( AAA ) 
#define LOGSTRING( AAA ) 
#define LOGSTRING2( AAA, BBB )    
#define LOGSTRING3( AAA, BBB, CCC )    
#define LOGRSTRING( AAA, BBB )

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
