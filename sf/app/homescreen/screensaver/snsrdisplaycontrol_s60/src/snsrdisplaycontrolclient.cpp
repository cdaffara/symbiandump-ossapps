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

#include "snsrdisplaycontrolclient.h"
#include "snsrdisplaycontrolcommon.h"

// =========== CONSTANTS ===========


// ======== LOCAL FUNCTIONS ========

// -----------------------------------------------------------------------------
// CreateServerProcess
// Static function to create the server process.
// -----------------------------------------------------------------------------
//
static TInt CreateServerProcess()
    {
    TInt err( KErrNone );
    RProcess server;
    err = server.Create( KSnsrDispCtrlSrvName, KNullDesC, EOwnerThread );
    if ( !err )
        {
        TRequestStatus status;
        server.Rendezvous( status );
        if ( status != KRequestPending )
            {
            server.Kill( 0 ); // Abort startup.
            }
        else
            {
            server.Resume(); // Logon OK - start the server.
            }
        User::WaitForRequest( status ); // Wait for start or death.
        // We can't use the 'exit reason' if the server panicked as this
        // is the panic 'reason' and may be '0' which cannot be distinguished
        // from KErrNone.
        err = ( server.ExitType() == EExitPanic ) ? KErrGeneral
                : status.Int();
        server.Close();
        }
    return err;
    }

// -----------------------------------------------------------------------------
// StartServer
// Static function to start the server.
// -----------------------------------------------------------------------------
//
static TInt StartServer()
    {
    TInt result;

    TFindServer findServer( KSnsrDispCtrlSrvName );
    TFullName name;

    result = findServer.Next( name );
    if ( result != KErrNone )
        {
        // Server not running
        result = CreateServerProcess();
        }
    return result;
    }



// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C RSnsrDisplayControlClient::RSnsrDisplayControlClient() :
    RSessionBase(), iError( KErrNotReady )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSnsrDisplayControlClient::Open()
    {
    iError = StartServer();
    
    if ( !iError )
        {
        iError = CreateSession( KSnsrDispCtrlSrvName, Version() );
        }
    
    if ( !iError )
        {
        iError = ShareAuto();
        }
    
    return iError;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void RSnsrDisplayControlClient::Close()
    {
    RSessionBase::Close();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C TVersion RSnsrDisplayControlClient::Version() const
    {
    return TVersion( KSnsrDispCtrlSrvVerMajor, 
                     KSnsrDispCtrlSrvVerMinor, 
                     KSnsrDispCtrlSrvVerBuild );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void RSnsrDisplayControlClient::SetDisplayFullPower()
    {
    if ( !iError )
        {
        TRequestStatus status( KRequestPending );
        SendReceive( ESnsrDispCtrlSrvDisplayFullPower, TIpcArgs(), status );
        User::WaitForRequest( status );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void RSnsrDisplayControlClient::SetDisplayLowPower( TInt aStartRow, TInt aEndRow )
    {
    if ( !iError )
        {
        TRequestStatus status( KRequestPending );
        TIpcArgs args( aStartRow, aEndRow );
        
        SendReceive( ESnsrDispCtrlSrvDisplayLowPower, args, status );
        
        User::WaitForRequest( status );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void RSnsrDisplayControlClient::SetDisplayOff()
    {
    if ( !iError )
        {
        TRequestStatus status( KRequestPending );
        SendReceive( ESnsrDispCtrlSrvDisplayOff, TIpcArgs(), status );
        User::WaitForRequest( status );
        }
    }

