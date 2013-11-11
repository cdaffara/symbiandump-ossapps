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
* Description:  Contains the implementation of class CSPForwardProvider
*
*/


#include <mccpforwardprovider.h>
#include <mccpforwardobserver.h>
#include <etelmm.h>
#include <badesca.h>    // CDesC8ArrayFlat

#include "cspforwardprovider.h"
#include "csplogger.h"


// ---------------------------------------------------------------------------
// Two phased construction
// ---------------------------------------------------------------------------
//
CSPForwardProvider* CSPForwardProvider::NewL ( )
    {
    CSPForwardProvider* self = new ( ELeave ) CSPForwardProvider( );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;    
    }
    
// ---------------------------------------------------------------------------
// Destructor of the object.
// ---------------------------------------------------------------------------
//
CSPForwardProvider::~CSPForwardProvider()
    {
    delete iEmptyArray;
    iObservers.Close();
    }

// ---------------------------------------------------------------------------
// CSPCall::NotifyForwardEventOccurred
// Notifies observers about call events
// ---------------------------------------------------------------------------
//
void CSPForwardProvider::NotifyForwardEventOccurred( 
    MCCPForwardObserver::TCCPForwardEvent aEvent )
    {
    CSPLOGSTRING2( CSPREQOUT, 
        "CSPForwardProvider::ForwardEventOccurred: event: %d", aEvent );

    for ( TInt i = 0; i < iObservers.Count(); i++ )
        {
        MCCPForwardObserver *obs = iObservers[i];
        if ( obs )
            {                
            obs->ForwardEventOccurred( aEvent );
            }
        }
    }

// ---------------------------------------------------------------------------
// Forward address choice array.
// ---------------------------------------------------------------------------
//  
const CDesC8Array& CSPForwardProvider::GetForwardAddressChoicesL()
    {
    return *iEmptyArray;
    }

// ---------------------------------------------------------------------------
// Forward to address. 
// Not supported. 
// ---------------------------------------------------------------------------
//  
void CSPForwardProvider::ForwardToAddressL( const TInt /*aIndex*/ )
    {
    User::Leave( KErrNotSupported );
    return;
    }
        
// ---------------------------------------------------------------------------
// Adds observer to array
// ---------------------------------------------------------------------------
//
void CSPForwardProvider::AddObserverL( const MCCPForwardObserver& aObserver )
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
TInt CSPForwardProvider::RemoveObserver( const MCCPForwardObserver& aObserver )
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
// Default C++ constructor
// ---------------------------------------------------------------------------
//
CSPForwardProvider::CSPForwardProvider( )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPForwardProvider::CSPForwardProvider()" );
    }

// ---------------------------------------------------------------------------
// Constructing 2nd phase
// ---------------------------------------------------------------------------
//
void CSPForwardProvider::ConstructL()
    {
    CSPLOGSTRING(CSPOBJECT, "CSPForwardProvider::ConstructL()" );
    iEmptyArray = new (ELeave) CDesC8ArrayFlat( 1 );
    }

