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
* Description:  Implements the class CSPPubSubListener
*
*/


#include "csppubsublistener.h"
#include "mcsppubsubobserver.h"
#include "csplogger.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSPPubSubListener::CSPPubSubListener
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSPPubSubListener::CSPPubSubListener(
    const TUid aUid, const TInt aKey, MCSPPubSubObserver* aObserver ) :
        CActive( CActive::EPriorityStandard ),
        iUid( aUid ), iId( aKey ), iObserver( aObserver )
    {
    }

// -----------------------------------------------------------------------------
// CSPPubSubListener::~CSPPubSubListener
// Destructor.
// -----------------------------------------------------------------------------
//
CSPPubSubListener::~CSPPubSubListener()
    {
    CSPLOGSTRING( CSPINT, "CSPPubSubListener::~CSPPubSubListener" )
    Cancel();
    iProperty.Close();
    }

// -----------------------------------------------------------------------------
// CSPPubSubListener::RunL
// From CActive.
// -----------------------------------------------------------------------------
//
void CSPPubSubListener::RunL()
    {
    CSPLOGSTRING( CSPINT, "CSPPubSubListener::RunL" )
    const TRequestStatus status( iStatus );
    StartListening();
    iObserver->HandleNotifyPSL( iUid, iId, status );
    CSPLOGSTRING( CSPINT, "CSPPubSubListener::RunL end" )
    }

// -----------------------------------------------------------------------------
// CSPPubSubListener::DoCancel
// From CActive.
// -----------------------------------------------------------------------------
//
void CSPPubSubListener::DoCancel()
    {
    CSPLOGSTRING( CSPINT, "CSPPubSubListener::DoCancel")
    iProperty.Cancel();
    }

// -----------------------------------------------------------------------------
// CSPPubSubListener::RunError
// From CActive.
// -----------------------------------------------------------------------------
//
TInt CSPPubSubListener::RunError( TInt /*aError*/ )
    {
    CSPLOGSTRING( CSPINT, "CSPPubSubListener::RunError" )
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSPPubSubListener::NewL
// -----------------------------------------------------------------------------
//
CSPPubSubListener* CSPPubSubListener::NewL( const TUid aUid, 
    const TInt aKey, MCSPPubSubObserver* aObserver )
    {
    CSPLOGSTRING( CSPINT, "CSPPubSubListener::NewL" )
    CSPPubSubListener* self = new( ELeave ) 
        CSPPubSubListener( aUid, aKey, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CSPPubSubListener::StartListening
// -----------------------------------------------------------------------------
//
void CSPPubSubListener::StartListening() 
    {
    iProperty.Subscribe( iStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CSPPubSubListener::ConstructL
// -----------------------------------------------------------------------------
//
void CSPPubSubListener::ConstructL()
    {
    if ( iObserver )
        {
        CActiveScheduler::Add( this );
        }
        
    User::LeaveIfError ( iProperty.Attach( iUid, iId, EOwnerThread ) );

    if ( iObserver )
        {
        StartListening();
        }
    }

// -----------------------------------------------------------------------------
// CSPPubSubListener::Get
// Read integer value.
// -----------------------------------------------------------------------------
//
TInt CSPPubSubListener::Get( TInt& aVal )
    {
    return iProperty.Get( iUid, iId, aVal );
    }

// -----------------------------------------------------------------------------
// CSPPubSubListener::Get
// Read binary value.
// -----------------------------------------------------------------------------
//
TInt CSPPubSubListener::Get( TDes8& aVal )
    {
    return iProperty.Get( iUid, iId, aVal );
    }
  
// -----------------------------------------------------------------------------
// CSPPubSubListener::Get
// Read string value.
// -----------------------------------------------------------------------------
//      
TInt CSPPubSubListener::Get( TDes16& aVal )
    {
    return iProperty.Get( iUid, iId, aVal );
    }

// End of File
