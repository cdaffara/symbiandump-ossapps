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
* Description:  Presence Cache trace utilities
*
*/

#ifndef PRESTRACE_H__
#define PRESTRACE_H__

#include <e32std.h>


// CONSTANTS
_LIT( KPresenceTraceLogDir, "presence" );
_LIT( KPresenceTraceLogFile, "presence.log" );


 //TRACE INDIRECTION
#ifdef _DEBUG

    /**
     * Trace handlers. Different statements with 
     * different names for handling different parameter amounts.
     *
     * Statements produce traces only in debug builds.
     * In hardware debug builds traces go to file defined
     * KPresenceTraceLogDir and KPresenceTraceLogFile.
     */
    #define TRACE( aStatement ) PresenceTrace::Trace( aStatement )
    #define TRACE_1( aStatement, aP1 ) PresenceTrace::Trace( aStatement, aP1 )
    #define TRACE_2( aStatement, aP1, aP2 ) PresenceTrace::Trace( aStatement, aP1, aP2 )
    #define TRACE_3( aStatement, aP1, aP2, aP3 ) PresenceTrace::Trace( aStatement, aP1, aP2, aP3 )
    #define TRACE_4( aStatement, aP1, aP2, aP3, aP4 ) PresenceTrace::Trace( aStatement, aP1, aP2, aP3, aP4 )
    #define TRACE_5( aStatement, aP1, aP2, aP3, aP4, aP5 ) PresenceTrace::Trace( aStatement, aP1, aP2, aP3, aP4, aP5 )
    #define TRACE_6( aStatement, aP1, aP2, aP3, aP4, aP5, aP6 ) PresenceTrace::Trace( aStatement, aP1, aP2, aP3, aP4, aP5, aP6 )


    /**
     * Block (and thus also function) trace handlers. These 
     * mark block enter, exit and also non local exit to traces.
     *
     * Example:
     * void CSomeClass::SomeMethod()
     *      {
     *      TRACE_BLOCK_ENTER( "CSomeClass::SomeMethod" );
     *
     *      Do your stuff here. If there happens a leave
     *      or return before "TRACE_BLOCK_EXIT()" statement,
     *      it is marked to traces with "CSomeClass::SomeMethod - nonlocal exit"
     *
     *      TRACE_BLOCK_EXIT();
     *      }
     */
    //#define TRACE_BLOCK_ENTER( aBlockName ) TPresenceBlockTraceHandler __blockTraceHandler( _S( aBlockName ) );
    //#define TRACE_BLOCK_EXIT() __blockTraceHandler.NormalExit();



#else //NOT _DEBUG

    #define TRACE( aStatement )
    #define TRACE_1( aStatement, aP1 )
    #define TRACE_2( aStatement, aP1, aP2 )
    #define TRACE_3( aStatement, aP1, aP2, aP3 )
    #define TRACE_4( aStatement, aP1, aP2, aP3, aP4 )
    #define TRACE_5( aStatement, aP1, aP2, aP3, aP4, aP5 )
    #define TRACE_6( aStatement, aP1, aP2, aP3, aP4, aP5, aP6 )
    #define TRACE_BLOCK_ENTER( aBlockName )
    #define TRACE_BLOCK_EXIT()

#endif



/**
 * trace sink.
 */
class PresenceTrace
    {
public:

    /**
     * Trace sink.
     *
     * @param aFmt A format list to print to trace.
     */
    IMPORT_C static void Trace( TRefByValue<const TDesC> aFmt,... );



private: //Prohibited construtor / destructor
    PresenceTrace();
    ~PresenceTrace();
    };



//  Commented out since not used. Can be uncommented if needed later
/*  
//  Block trace handler.
// 

NONSHARABLE_CLASS( TPresenceBlockTraceHandler )
    {
public:

    // Block trace handler constructor.
    // @param aBlockName Block name.
    
    TPresenceBlockTraceHandler( const TText* aBlockName );


    // Block trace handler destructor.
     ~TPresenceBlockTraceHandler();


    // Marks that block has reached its normal exit.
    void NormalExit();


private:
    //Block name
    const TPtrC   iBlockName;

    //Normal exit marker
    TBool iBlockNormalExit;
};

*/
#endif  // PRESTRACE_H__

