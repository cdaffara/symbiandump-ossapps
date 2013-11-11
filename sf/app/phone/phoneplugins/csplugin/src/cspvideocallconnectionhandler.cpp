/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Contains the implementation of class CSPDataCallConnectionHandler
*
*/


#include <etelmm.h>
#include <mccpcallobserver.h>
#include <e32property.h>

#include "cspvideocallconnectionhandler.h"
#include "cspvideocall.h"
#include "csplogger.h"
#include "cspconsts.h"


// ---------------------------------------------------------------------------
// Constructs the connection handler via two phased constructing.
// ---------------------------------------------------------------------------
//
CSPVideoCallConnectionHandler* CSPVideoCallConnectionHandler::NewL(
            CSPVideoCall& aObserver,
            RMobileCall& aCall ) 
    {
    CSPLOGSTRING(CSPOBJECT, "CSPDataCallConnectionHandler::NewL()" );
    CSPVideoCallConnectionHandler* self = new (ELeave) CSPVideoCallConnectionHandler( 
                                        aObserver, aCall );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// Destructor of the object.
// ---------------------------------------------------------------------------
//
CSPVideoCallConnectionHandler::~CSPVideoCallConnectionHandler( )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPVideoCallConnectionHandler::~CSPDataCallConnectionHandler()" );
    ReleaseConnection();
    Cancel();
    CloseConnection();
    }

// ---------------------------------------------------------------------------
// CSPVideoCallConnectionHandler::InitializeConnection
// ---------------------------------------------------------------------------
//
TInt CSPVideoCallConnectionHandler::InitializeConnection( )
    {    
    TInt err = CreateSocketConnection();
    if ( !err )
        {        
        err = LoanDataPort();
        }    
    
    CSPLOGSTRING2(CSPOBJECT, 
        "CSPVideoCallConnectionHandler::InitializeConnection err=%i", err );
        
    return err;
    }
    
// ---------------------------------------------------------------------------
// CSPVideoCallConnectionHandler::ReleaseConnection
// ---------------------------------------------------------------------------
//
TInt CSPVideoCallConnectionHandler::ReleaseConnection( )
    {    
    CloseConnection();
    TInt err = iCall.RecoverDataPort();
    CSPLOGSTRING2(CSPOBJECT, 
        "CSPVideoCallConnectionHandler::ReleaseConnection err=%i", err );
    iConnectionInitialized = EFalse;

    return err;
    }
    
// ---------------------------------------------------------------------------
// CSPVideoCallConnectionHandler::InitializeConnection
// ---------------------------------------------------------------------------
//
TBool CSPVideoCallConnectionHandler::ConnectionInitialized( )
    {
    return iConnectionInitialized;
    }

// -----------------------------------------------------------------------------
// Creates socket connection.
// Enumerates connections and save Access Point Name of CSD connection.
// -----------------------------------------------------------------------------
//
TInt CSPVideoCallConnectionHandler::CreateSocketConnection()
    {
    CSPLOGSTRING( CSPINT, "CSPVideoCallConnectionHandler::CreateSocketConnection() start");
    // Socket server connection to get Access Point Name
    TInt errorCode = iSocketServer.Connect();
    CSPLOGSTRING2( CSPREQOUT,
      "CSPVideoCallConnectionHandler::CreateSocketConnection: RSocketServ::Connect, error code: %d",
       errorCode );
      
    if ( KErrNone == errorCode )
        {
        errorCode = iConnection.Open( iSocketServer );
        CSPLOGSTRING2( CSPREQOUT,
         "CSPVideoCallConnectionHandler::CreateSocketConnection: RConnection::Open, errorcode: %d",
           errorCode );
        if ( errorCode )
            {
            iSocketServer.Close();
            }
        }
    return errorCode;
    }

// ---------------------------------------------------------------------------
// CSPVideoCallConnectionHandler::LoanDataPort
// ---------------------------------------------------------------------------
//
TInt CSPVideoCallConnectionHandler::LoanDataPort( )
    {
    CSPLOGSTRING(CSPINT, "CSPVideoCallConnectionHandler::LoanDataPort");
    TInt ret( KErrNone ); 
    if ( !IsActive() )
        {
        SetActive();
        iCall.LoanDataPort( iStatus, iCommPort );    
        }
    else
        {
        ret = KErrInUse;
        CSPLOGSTRING2(CSPERROR, 
            "CSPVideoCallConnectionHandler::LoanDataPort Error!: %d", 
            ret);
        }
    
    return ret; 
    }

// -----------------------------------------------------------------------------
// Close Socket Server connection, when the call is on Idle state.
// -----------------------------------------------------------------------------
//
void CSPVideoCallConnectionHandler::CloseConnection()
    {
    CSPLOGSTRING( CSPINT, "CSPVideoCallConnectionHandler::CloseConnection" );
    iConnection.Close();
    iSocketServer.Close();
    }

// ---------------------------------------------------------------------------
// CSPVideoCallConnectionHandler::RunL
// ---------------------------------------------------------------------------
//
void CSPVideoCallConnectionHandler::RunL()
    {
    // from CActive
    CSPLOGSTRING2( CSPREQEND, 
        "CSPVideoCallConnectionHandler::RunL: status: %d", iStatus.Int() );
    
    if ( iStatus == KErrNone )
        {
        CSPLOGSTRING( CSPINT, 
            "CSPVideoCallConnectionHandler::RunL: LoanDataPort request OK" );
        iConnectionInitialized = ETrue;
        iObserver.ConnectionReady( iCommPort.iPort );        
        }
    else
        {
        // Error situations         
        TInt err = iStatus.Int();
        CSPLOGSTRING2( CSPERROR, "CSPVideoCallConnectionHandler::RunL: request \
                                completed with error: %d", err );

        iObserver.ConnectionEstablishingFailed( err );        
        }    
    }

// ---------------------------------------------------------------------------
// CSPVideoCallConnectionHandler::DoCancel
// ---------------------------------------------------------------------------
//
void CSPVideoCallConnectionHandler::DoCancel()
    {
    CSPLOGSTRING( CSPREQOUT, "CSPVideoCallConnectionHandler::DoCancel" );
    if ( IsActive() )
        {
        iCall.LoanDataPortCancel();
        }
    else
        {
        CSPLOGSTRING( CSPERROR, 
            "CSPVideoCallConnectionHandler::DoCancel: Already active" );
        }
    }

// ---------------------------------------------------------------------------
// Constructs the requester.
// ---------------------------------------------------------------------------
//
CSPVideoCallConnectionHandler::CSPVideoCallConnectionHandler(
            CSPVideoCall& aObserver,
            RMobileCall& aCall ) : 
                CActive( EPriorityStandard ), 
                iObserver( aObserver ), 
                iCall( aCall ), 
                iConnectionInitialized( EFalse )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPVideoCallConnectionHandler::CSPVideoCallConnectionHandler" );
    CActiveScheduler::Add( this );
    }
    
// ---------------------------------------------------------------------------
// Constructing in the second phase.
// ---------------------------------------------------------------------------
//
void CSPVideoCallConnectionHandler::ConstructL()
    {
    // Empty implementation 
    }


// End of File

