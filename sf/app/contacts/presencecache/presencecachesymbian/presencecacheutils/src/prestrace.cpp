/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  TPresence Cache trace implementation
*
*/

// INCLUDE FILES
#include "prestrace.h"

#include <e32svr.h>
#include <e32std.h>
#include <flogger.h>


/**
 * Handler used by trace implementation to truncate
 * the string rather than panic in case of buffer overflow.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( TPresenceOverflowTruncate ) : public TDes16Overflow
    {
    public:
        void Overflow( TDes16& /*aDes*/ ) {}
    };



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PresenceTrace::Trace()
// -----------------------------------------------------------------------------
//
EXPORT_C void PresenceTrace::Trace( TRefByValue<const TDesC> aFmt,... )
    {
    TBuf< 250 > buffer;
    buffer.Append( _L("[") );
    buffer.Append( RThread().Name() );
    buffer.Append( _L("] ") );

    TPresenceOverflowTruncate overflowHandler;

    VA_LIST list;
    VA_START( list, aFmt );
    buffer.AppendFormatList( aFmt, list, &overflowHandler );


//    #ifdef __WINS__
    RDebug::Print( _L("%S"), &buffer );
//    #endif // __WINS__ Pia
// Pia    #else

    RFileLogger logger;
    if( logger.Connect() == KErrNone )
        {
        logger.SetDateAndTime( EFalse, ETrue );
        logger.CreateLog( KPresenceTraceLogDir,
                          KPresenceTraceLogFile,
                          EFileLoggingModeAppend );

        logger.Write( buffer );
        logger.CloseLog();
        logger.Close();
        }

// Pia   #endif // __WINS__
   }


/* Commented out since not used as of now. Can be uncommented if needed

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TPresenceBlockTraceHandler::TPresenceBlockTraceHandler()
// -----------------------------------------------------------------------------
//
EXPORT_C TPresenceBlockTraceHandler::TPresenceBlockTraceHandler( const TText* aBlockName ) 
    : iBlockName( aBlockName ),
      iBlockNormalExit( EFalse )
    {
    TRACE_1( _L("%S - enter"), &iBlockName );  
    }   


// -----------------------------------------------------------------------------
// TPresenceBlockTraceHandler::~TPresenceBlockTraceHandler()
// -----------------------------------------------------------------------------
//
EXPORT_C TPresenceBlockTraceHandler::~TPresenceBlockTraceHandler() // CSI: 82 #
    {
    if( !iBlockNormalExit )
        {
        //Normal exit not recorded so far
        //Thus report here nonlocal exit
        TRACE_1( _L("%S - nonlocal exit"), &iBlockName );  
        }
    }


// -----------------------------------------------------------------------------
// TPresenceBlockTraceHandler::NormalExit()
// -----------------------------------------------------------------------------
//
EXPORT_C void TPresenceBlockTraceHandler::NormalExit() 
    {
    TRACE_1( _L("%S - exit"), &iBlockName );  
    iBlockNormalExit = ETrue;
    }
    

*/
