/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  PC Connectivity client
*
*/



// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <e32base.h>
#include <s32mem.h>

// Client - server 
#include "sconpcconnclient.h"
#include "sconpcconnclientserver.h"
#include "debug.h"

// -----------------------------------------------------------------------------
// RSConPCConnSession::RSConPCConnSession()
// Default constructor
// -----------------------------------------------------------------------------
//
EXPORT_C RSConPCConnSession::RSConPCConnSession()
    {
    TRACE_FUNC;
    }

// -----------------------------------------------------------------------------
// RSConPCConnSession::~RSConPCConnSession()
// Default destructor
// -----------------------------------------------------------------------------
//
EXPORT_C RSConPCConnSession::~RSConPCConnSession()
    {
    TRACE_FUNC;
    }

// -----------------------------------------------------------------------------
// RSConPCConnSession::Connect()
// Creates a chunk and server, starts the server connection
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSConPCConnSession::Connect()
    {
    TRACE_FUNC_ENTRY;
 	
    TInt ret ( KErrNone );

    ret = CreateSession(KSConPCConnServerName,Version());
    
    if ( ret == KErrNotFound || ret == KErrServerTerminated )
    	{
    	ret = LaunchServer();
    	
    	if ( ret == KErrNone || ret == KErrAlreadyExists )
    		{
    		ret = CreateSession( KSConPCConnServerName, Version() );
    		}
    	}
    	
    if ( ret == KErrNone )
    	{
    	ret = CreateAndSendChunkHandle();
    	if ( ret != KErrNone )
    	    {
    	    LOGGER_WRITE("CreateAndSendChunkHandle failed, close session");
    	    Close();
    	    }
    	}
    
    LOGGER_WRITE_1( "RSConPCConnSession::Connect() : returned %d", ret );
    return ret; 
    }

// -----------------------------------------------------------------------------
// RSConPCConnSession::Close()
// Closes the server connection
// -----------------------------------------------------------------------------
//
EXPORT_C void RSConPCConnSession::Close()
    {
    TRACE_FUNC_ENTRY;
    iChunk.Close();
    RSessionBase::Close();
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// RSConPCConnSession::Version()
// Returns the version of the client
// -----------------------------------------------------------------------------
//	
TVersion RSConPCConnSession::Version(void) const
    {
    return (TVersion(KSConPCConnServerVersionMajor, 
                               KSConPCConnServerVersionMinor, 
                               KSConPCConnServerVersionBuild));
    }

// -----------------------------------------------------------------------------
// RSConPCConnSession::Version()
// Forwards a PUT -message to the server
// -----------------------------------------------------------------------------
//	
EXPORT_C TInt RSConPCConnSession::PutPacketL( 
    const TDesC& aNameHeader, const TDesC8& aTypeHeader, CBufFlat*& aBuffer )
    {
    TRACE_FUNC_ENTRY;

    if ( !aBuffer )
        {
        return KErrArgument;
        }

    if ((aBuffer->Size() + aTypeHeader.Size() + 
         aNameHeader.Size()) > KSConPCConnChunkMaxSize)
        {
        LOGGER_WRITE( "RSConPCConnSession::PutPacketL() :\
         WBXML document too big" );
        return KErrTooBig;
        }
	
    TInt ret (KErrNone);
    iChunk.Adjust( KSConPCConnChunkMaxSize );
    RMemWriteStream buf( iChunk.Base(), iChunk.Size() );
	
    buf.WriteInt32L( aNameHeader.Size() );
    buf.WriteL( aNameHeader );

    buf.WriteInt32L( aTypeHeader.Size() );
    buf.WriteL( aTypeHeader );
	
    // WBXML document	
    buf.WriteInt32L( aBuffer->Size() );
    buf.WriteL( aBuffer->Ptr(0) );
    buf.CommitL();
    buf.Close();
    
    TIpcArgs args;
    ret = SendReceive ( EPutMessage, args );

    LOGGER_WRITE_1( "RSConPCConnSession::PutPacketL() : returned %d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// RSConPCConnSession::Version()
// Forwards a GET -message to the server
// -----------------------------------------------------------------------------
//	
EXPORT_C TInt RSConPCConnSession::GetPacketL( 
    const TDesC& aNameHeader, const TDesC8& aTypeHeader, CBufFlat*& aBuffer )
    {
    TRACE_FUNC_ENTRY;

    if ( !aBuffer )
        {
        return KErrArgument;
        }

    TInt ret (KErrNone);
	
    iChunk.Adjust( KSConPCConnChunkMaxSize);
    RMemWriteStream writeBuf( iChunk.Base(), iChunk.Size() );
    writeBuf.WriteInt32L( aNameHeader.Size() );
    writeBuf.WriteL( aNameHeader );
    
    writeBuf.WriteInt32L( aTypeHeader.Size() );
    writeBuf.WriteL( aTypeHeader );

    writeBuf.CommitL();
    writeBuf.Close();
    
    TIpcArgs args;
    ret = SendReceive ( EGetMessage, args );

    if ( ret != KErrNone) 
        {
        LOGGER_WRITE_1( "RSConPCConnSession::GetPacketL() :\
         Send Receive failed with code %d", ret );
        return ret;
        };
	
    // copy data from the chunk
    RMemReadStream readBuf( iChunk.Base(), iChunk.Size() );
    TInt32 length ( 0 );

    aBuffer->Reset();
    length = readBuf.ReadInt32L();
	
    HBufC8* data = HBufC8::NewLC( length );
    TPtr8 dataPtr = data->Des();
	
    readBuf.ReadL( dataPtr, length );
    aBuffer->ExpandL( 0, length );
    aBuffer->Write ( 0, dataPtr );
	
    readBuf.Close();
	
    CleanupStack::PopAndDestroy(); // data;

    LOGGER_WRITE_1( "RSConPCConnSession::GetPacketL()\
     : returned %d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// RSConPCConnSession::Version()
// Forwards a Reset -message to the server
// -----------------------------------------------------------------------------
//	
EXPORT_C TInt RSConPCConnSession::ResetServer()
    {
    TRACE_FUNC_ENTRY;
    TInt ret ( KErrNone );
    TIpcArgs args;
    ret = SendReceive ( EResetMessage, args );
    LOGGER_WRITE_1( "RSConPCConnSession::ResetServerL(): ret %d", ret );
    return ret;
    }
    
// -----------------------------------------------------------------------------
// RSConPCConnSession::LaunchServer()
// Launches the server
// -----------------------------------------------------------------------------
//	
TInt RSConPCConnSession::LaunchServer()
	{
	TRACE_FUNC_ENTRY;
	TInt ret( KErrNone );
    
    // Create server
    RProcess p;
    ret = p.Create(KSConPCConnServerExe, KNullDesC);
    
    if ( ret != KErrNone )
        {
        // Loading failed.
        LOGGER_WRITE_1( "RProcess::Create failed, err: %d", ret );
        return ret;
        }
    
    
    TRequestStatus status;
    p.Rendezvous( status );
    
    if ( status != KRequestPending )
        {
        p.Kill(0);		// abort startup
        p.Close();
        LOGGER_WRITE("Abort startup, return KErrGeneral");
        return KErrGeneral;   
        }
    else
        {
        p.Resume();	// logon OK - start the server
        }
    
    User::WaitForRequest(status);
    
    p.Close();
    
	LOGGER_WRITE_1( "RSConPCConnSession::LaunchServerL(): returned %d", status.Int() );
    return status.Int();
	}
	
// -----------------------------------------------------------------------------
// RSConPCConnSession::CreateAndSendChunkHandle()
// Creates a chunk and sends a handle to server
// -----------------------------------------------------------------------------
//	
TInt RSConPCConnSession::CreateAndSendChunkHandle()
	{
	TRACE_FUNC_ENTRY;
	TInt err = iChunk.CreateGlobal( KNullDesC, 
                                    KSConPCConnChunkSize, 
                                    KSConPCConnChunkMaxSize );
    if ( err != KErrNone )
        {
        LOGGER_WRITE_1("iChunk.CreateGlobal failed, err: %d", err);
        return err;
        }                           
    TIpcArgs args;
 	args.Set( 0, iChunk );
 	err = SendReceive( EChunkMessage, args );
    
    LOGGER_WRITE_1("RSConPCConnSession::CreateAndSendChunkHandle : returned: %d", err);
    return err;
	}
	
// End of file
