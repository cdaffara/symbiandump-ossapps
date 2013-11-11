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
* Description:  Contains the implementation of class CSPTransferProvider
*
*/


#include <mccptransferprovider.h>
#include <mccptransferobserver.h>
#include <etelmm.h>
#include <exterror.h>
#include <mccpcall.h>

#include "csptransferprovider.h"
#include "csplogger.h"
#include "mcspcallerrorobserver.h"
#include "mcspcommoninfo.h"


// ---------------------------------------------------------------------------
// Two phased construction
// ---------------------------------------------------------------------------
//
CSPTransferProvider* CSPTransferProvider::NewL ( 
    RMobileCall& aCall, 
    MCSPCallErrorObserver& aErrorObserver,
    MCSPCommonInfo& aCommonInfo )
    {
    CSPTransferProvider* self = 
        new ( ELeave ) CSPTransferProvider( aCall, aErrorObserver, aCommonInfo );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;    
    }

// ---------------------------------------------------------------------------
// Destructor of the object.
// ---------------------------------------------------------------------------
//
CSPTransferProvider::~CSPTransferProvider()
    {
    Cancel();
    iObservers.Close();
    }

// ---------------------------------------------------------------------------
// Transfers the call. CCE maps CS Transfer() to this method.
// (UnattendedTransfer and others are VoIP features)
// ---------------------------------------------------------------------------
//  
TInt CSPTransferProvider::AttendedTransfer( MCCPCall& aTransferTargetCall )
    {
    TInt err ( KErrNone );
    
    if ( !IsActive() )
        {    
        if ( iCall.SubSessionHandle() != 0 )
            {
            err = KErrNone;
            // send async request to Etel
            iCall.Transfer( iStatus );
            SetActive();
            if( aTransferTargetCall.State() == MCCPCallObserver::ECCPStateConnecting )
                {
                iCommonInfo.DontReportTerminationError();
                }
            }
        else
            {
            // transfer not allowed
            err = KErrNotSupported;
            }
         }
    else
        {
        err = KErrInUse;
        }
    CSPLOGSTRING2( CSPREQOUT, 
         "CSPTransferProvider::AttendedTransfer: err: %d", err );
    // return error value
    return err;
    }

// ---------------------------------------------------------------------------
// Transfers the call. CCE maps CS Transfer() to this method.
// (UnattendedTransfer and others are VoIP features)
// ---------------------------------------------------------------------------
//  
TInt CSPTransferProvider::AttendedTransfer( const TDesC& /*aTransferTarget*/ )
    {
    TInt err ( KErrNone );
    
    if ( !IsActive() )
        {    
        if ( iCall.SubSessionHandle() != 0 )
            {
            err = KErrNone;
            // send async request to Etel
            iCall.Transfer( iStatus );
            SetActive();
            }
        else
            {
            // transfer not allowed
            err = KErrNotSupported;
            }
         }
    else
        {
        err = KErrInUse;
        }
    CSPLOGSTRING2( CSPREQOUT, 
         "CSPTransferProvider::AttendedTransfer: err: %d", err );
    // return error value
    return err;
    }

// ---------------------------------------------------------------------------
// Not supported by CS Plug-in. This is a VoIP feature
// ---------------------------------------------------------------------------
//  
TInt CSPTransferProvider::UnattendedTransfer( const TDesC& /*aTransferTarget*/ )
    {
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// Not supported by CS Plug-in. This is a VoIP feature
// ---------------------------------------------------------------------------
//  
TInt CSPTransferProvider::AcceptTransfer( const TBool /*aAccept*/ )
    {
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// Not supported by CS Plug-in. This is a VoIP feature
// ---------------------------------------------------------------------------
//
const TDesC& CSPTransferProvider::TransferTarget() const
    {
    return KNullDesC;
    }

// ---------------------------------------------------------------------------
// Notify observers about an occurred transfer event
// ---------------------------------------------------------------------------
//
void CSPTransferProvider::TransferEventOccurred( 
                 const MCCPTransferObserver::TCCPTransferEvent aEvent )
    {
    CSPLOGSTRING2( CSPREQOUT, 
        "CSPTransferProvider::TransferEventOccurred: event: %d", aEvent );

    for ( TInt i = 0; i < iObservers.Count(); i++ )
        {
        MCCPTransferObserver *obs = iObservers[i];
        if ( obs )
            {        
            iObservers[i]->TransferEventOccurred( aEvent );
            }
        }
    }

// ---------------------------------------------------------------------------
// Adds observer to array
// ---------------------------------------------------------------------------
//
void CSPTransferProvider::AddObserverL( const MCCPTransferObserver& aObserver )
    {
    if ( iObservers.Find( &aObserver ) == KErrNotFound )
        {
        iObservers.AppendL( &aObserver );
        }
    }

// ---------------------------------------------------------------------------
// Removes observer from array
// ---------------------------------------------------------------------------
//
TInt CSPTransferProvider::RemoveObserver( const MCCPTransferObserver& aObserver )
    {
    TInt found = iObservers.Find( &aObserver );
    if ( found != KErrNotFound )
        {
        iObservers.Remove( found );
        return KErrNone;
        }
    return found;
    }

// ---------------------------------------------------------------------------
// From CActive.
// Handles request completion.
// ---------------------------------------------------------------------------
//
void CSPTransferProvider::RunL()
    {
    CSPLOGSTRING2( CSPREQEND, 
        "CSPTransferProvider::RunL: status: %d", iStatus.Int() );
    
    if ( iStatus == KErrNone )
        {
        MCCPTransferObserver::TCCPTransferEvent event = 
            MCCPTransferObserver::ECCPLocalTransfer;
        TransferEventOccurred( event );
        }
    else
        {
        CSPLOGSTRING2( CSPERROR, "CSPTransferProvider::RunL: Error \
                     %d", iStatus.Int() );

        TInt error = iStatus.Int();
        if ( error == KErrGsmCCFacilityRejected )
            {
            iErrorObserver.NotifyErrorOccurred( ECCPErrorRejected );
            }
        else if( error != KErrCancel )
            {
            iErrorObserver.NotifyErrorOccurred( ECCPTransferFailed );
            }
        }
    
    CSPLOGSTRING( CSPREQEND, "CSPTransferProvider::RunL End of RunL." );
    }
    
// ---------------------------------------------------------------------------
// Cancels the pending async request
// ---------------------------------------------------------------------------
//
void CSPTransferProvider::DoCancel()
    {
    if ( iStatus == KRequestPending )
        {
        CSPLOGSTRING( CSPINT, 
            "CSPTransferProvider::DoCancel Canceling pending request.." );
        // cancel request
        iCall.CancelAsyncRequest( EMobileCallTransfer );
        }
    }

// ---------------------------------------------------------------------------
// Default C++ constructor
// ---------------------------------------------------------------------------
//
CSPTransferProvider::CSPTransferProvider( 
    RMobileCall& aCall, 
    MCSPCallErrorObserver& aErrorObserver,
    MCSPCommonInfo& aCommonInfo )
    : CActive( EPriorityStandard ),
      iCall (aCall),
      iErrorObserver( aErrorObserver ),
      iCommonInfo( aCommonInfo )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPTransferProvider::CSPTransferProvider()" );
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Constructing 2nd phase
// ---------------------------------------------------------------------------
//
void CSPTransferProvider::ConstructL()
    {
    CSPLOGSTRING(CSPOBJECT, "CSPTransferProvider::ConstructL()" );
    }

// End of File

