/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Contains the implementation of class CSPDTMFProvider
*
*/


#include <etelmm.h>
#include <mccpdtmfprovider.h>
#include <mccpdtmfobserver.h>
#include <rmmcustomapi.h>

#include "cspdtmfprovider.h"
#include "csplogger.h"
#include "cspeteldtmfmonitor.h"
#include "cspeteldtmfstopmonitor.h"

CSPDTMFProvider* CSPDTMFProvider::NewL( RMobilePhone& aPhone,
                                        RMmCustomAPI& aMmCustom )
    {
    CSPDTMFProvider* self = new ( ELeave ) CSPDTMFProvider( aPhone,
                                                            aMmCustom );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor of the object.
// ---------------------------------------------------------------------------
//
CSPDTMFProvider::~CSPDTMFProvider( )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPDTMFProvider::~CSPDTMFProvider()" );
    Cancel();
    iObservers.Close();
    delete iMonitor;
    delete iStopMonitor;
    }

// ---------------------------------------------------------------------------
// Notifies observers about a DTMF event
// ---------------------------------------------------------------------------
//
void CSPDTMFProvider::NotifyDTMFEvent( const MCCPDTMFObserver::TCCPDtmfEvent aEvent,
                      const TInt aError,
                      const TChar aTone )
    {
    CSPLOGSTRING2(CSPINT, "CSPDTMFProvider::NotifyDTMFEvent %d", aEvent );
    for ( TInt i = 0; i < iObservers.Count(); i++ )
        {
        MCCPDTMFObserver *obs = iObservers[i];
        if ( obs )
            {        
            iObservers[i]->HandleDTMFEvent( aEvent, aError, aTone );
            }
        }    
    }

    
// ---------------------------------------------------------------------------
// Cancel DTMF string sending
// ---------------------------------------------------------------------------
//
TInt CSPDTMFProvider::CancelDtmfStringSending()
    {
    CSPLOGSTRING(CSPREQIN, "CSPDTMFProvider::CancelDtmfStringSending" );
    if ( IsActive() )
        {
        Cancel();
        }
    else
        {
        return KErrAlreadyExists; // Is not active
        }
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Starts DTMF string sending
// ---------------------------------------------------------------------------
//
TInt CSPDTMFProvider::StartDtmfTone( const TChar aTone )
    {
    CSPLOGSTRING(CSPREQIN, "CSPDTMFProvider::StartDtmfTone" );
    TInt ret(KErrAlreadyExists);
    if (!IsActive())
        {
        ret = iPhone.StartDTMFTone( aTone );
        }    
    return ret;
    }

// ---------------------------------------------------------------------------
// Stop DTMF tone
// ---------------------------------------------------------------------------
//
TInt CSPDTMFProvider::StopDtmfTone()
    {
    CSPLOGSTRING(CSPREQIN, "CSPDTMFProvider::StopDtmfTone" );
    TInt ret(KErrAlreadyExists);
    if (!IsActive())
        {
        ret = iPhone.StopDTMFTone();
        }
    return ret;    
    }

// ---------------------------------------------------------------------------
// Send DTMF string
// ---------------------------------------------------------------------------
//
TInt CSPDTMFProvider::SendDtmfToneString( const TDesC& aString )
    {
    CSPLOGSTRING(CSPREQIN, "CSPDTMFProvider::SendDtmfToneString" );
    TInt ret = KErrInUse;
    
    if ( !IsActive() )
        {            
        iPhone.SendDTMFTones( iStatus, aString );
        SetActive();
        ret = KErrNone;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// Continue DTMF string sending
// ---------------------------------------------------------------------------
//
TInt CSPDTMFProvider::ContinueDtmfStringSending( const TBool aContinue )
    {
    CSPLOGSTRING(CSPREQIN, "CSPDTMFProvider::ContinueDtmfStringSending" );
    return iPhone.ContinueDTMFStringSending( aContinue );
    }
    
// ---------------------------------------------------------------------------
// Adds observer.
// ---------------------------------------------------------------------------
//
void CSPDTMFProvider::AddObserverL( const MCCPDTMFObserver& aObserver )
    {
    if ( iObservers.Find( &aObserver ) == KErrNotFound )
        {
        iObservers.AppendL( &aObserver );
        }
    }

// ---------------------------------------------------------------------------
// Removes given observer.
// ---------------------------------------------------------------------------
//
TInt CSPDTMFProvider::RemoveObserver( const MCCPDTMFObserver& aObserver )
    {
    TInt ret = KErrNotFound;
    
    TInt found = iObservers.Find( &aObserver );
    if ( found != KErrNotFound )
        {
        iObservers.Remove( found );
        ret = KErrNone;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// From CActive.
// Handles request completion.
// ---------------------------------------------------------------------------
//
void CSPDTMFProvider::RunL()
    {
    CSPLOGSTRING2( CSPREQIN, 
        "CSPDTMFProvider::RunL: status: %d", iStatus.Int() );
    
    if ( iStatus == KErrNone )
        {
        MCCPDTMFObserver::TCCPDtmfEvent event = 
            MCCPDTMFObserver::ECCPDtmfStringSendingCompleted;
        NotifyDTMFEvent( event, KErrNone, NULL );
        }
    else
        {
        TInt error = iStatus.Int();
        if ( error != KErrCancel )
            {
            MCCPDTMFObserver::TCCPDtmfEvent event = 
                MCCPDTMFObserver::ECCPDtmfStringSendingCompleted;
            NotifyDTMFEvent( event, error, NULL );
            }
        else
            {
            // Cancel is not notified
            CSPLOGSTRING( CSPREQEND, "CSPDTMFProvider::RunL Cancel." );
            }
        }
                
    CSPLOGSTRING( CSPREQEND, "CSPDTMFProvider::RunL End of RunL." );
    }

// ---------------------------------------------------------------------------
// From CActive
// Canceling functionality.
// ---------------------------------------------------------------------------
//
void CSPDTMFProvider::DoCancel()
    {
    if ( iStatus == KRequestPending )
        {
        CSPLOGSTRING( CSPINT, 
            "CSPDTMFProvider::DoCancel Canceling pending request.." );
        iPhone.CancelAsyncRequest( EMobilePhoneSendDTMFTones );
        }
    }
            
// ---------------------------------------------------------------------------
// Constructs the requester.
// ---------------------------------------------------------------------------
//
CSPDTMFProvider::CSPDTMFProvider( RMobilePhone& aPhone, 
                                  RMmCustomAPI& aMmCustom ):
                    CActive( EPriorityStandard ),
                    iPhone( aPhone ),
                    iMmCustom( aMmCustom )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPDTMFProvider::CSPDTMFProvider()" );
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Constructs the requester in the second phase.
// ---------------------------------------------------------------------------
//
void CSPDTMFProvider::ConstructL( ) 
    {
    if ( !iPhone.SubSessionHandle() || !iMmCustom.SubSessionHandle() )
        {
        User::Leave( KErrArgument );
        }
    iMonitor = CSPEtelDtmfMonitor::NewL( *this, iMmCustom );
    iMonitor->StartMonitoring();
    iStopMonitor = CSPEtelDtmfStopMonitor::NewL( *this, iPhone );
    iStopMonitor->StartMonitoring();
    }

//  End of File
