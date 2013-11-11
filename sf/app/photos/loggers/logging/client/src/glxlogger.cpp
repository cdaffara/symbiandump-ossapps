/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Logging utility for MC Photos
*
*/



// CLASS HEADER
#include "glxlogger.h"

// EXTERNAL INCLUDES
#include <e32debug.h>
#include <flogger.h>

// INTERNAL INCLUDES
//#include "glxlogchunk.h"

namespace GlxLogger
    {
    /// constants
    const TInt KMaxLoggingBufferLength = 256;
/*#ifndef __WINSCW__
    /// writeable buffer, this requires static data
    TBuf8< KMaxLoggingBufferLength > gWriteBuffer;
    _LIT( KGlxLoggingDirectory, "MCPhotos" );
    _LIT( KGlxLoggingFileName, "debug_%d.log" );
#endif*/

/**
 * TDes overflow handler
 * Default overflow handler panics and we dont want to do 
 * that in logging so need to implement our own handler.
 */ 
class TDes8OverFlowHandler : public TDes8Overflow
    {
    public: // from TDes8Overflow
        /// @ref TDes8Overflow::Overflow
        void Overflow( TDes8& /*aDes*/ )
            {
            // do nothing, we cant allocate so just eat the error
            }
    };

// -----------------------------------------------------------------------------
// GlxLogger::WriteFormat
// -----------------------------------------------------------------------------
EXPORT_C void WriteFormat( TRefByValue< const TDesC8 > aFmt, ... )
    {
    /// writeable buffer, in emulator this is in stack
    TBuf8< KMaxLoggingBufferLength > gWriteBuffer;
    // take the ellipsis parameter
    VA_LIST args;
    VA_START( args, aFmt );
    // create overflow handler
    TDes8OverFlowHandler overflowhandler;
    // clear old data by setting length to zero
    gWriteBuffer.SetLength( 0 );
    // append the parameters to the descriptor
    gWriteBuffer.AppendFormatList( aFmt, args, &overflowhandler );
    // end the ellipsis handling
    VA_END( args );
    
    TBuf16<256> buffer;
    buffer.Copy(gWriteBuffer);
    RDebug::Print(_L("MCPhotos:-%S"),&buffer);
    
    } // namespace
    
} // namespace
