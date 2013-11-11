/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Publish and subscribe settings listener.
*
*/


// INCLUDE FILES
#include    "CVTEngPubSubsListener.h"
#include    "MVtEngSettingPSObserver.h"
#include    <cvtlogger.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngPubSubsListener::CVtEngPubSubsListener
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngPubSubsListener::CVtEngPubSubsListener(
    const TUid aUid, const TInt aKey, MVtEngSettingPSObserver* aObserver ) :
        CActive( CActive::EPriorityStandard ),
        iUid( aUid ), iId( aKey ), iCallback( aObserver )
    {
    __VTPRINT( DEBUG_CONSTRUCT, "CVtEngPubSubsListener C++" )
    }

// -----------------------------------------------------------------------------
// CVtEngPubSubsListener::~CVtEngPubSubsListener
// Destructor.
// -----------------------------------------------------------------------------
//
CVtEngPubSubsListener::~CVtEngPubSubsListener()
    {
    __VTPRINTENTER( "PSLis.~" )
    Cancel();
    iProperty.Close();
    __VTPRINTEXIT( "PSLis.~" )
    }

// -----------------------------------------------------------------------------
// CVtEngPubSubsListener::RunL
// From CActive.
// -----------------------------------------------------------------------------
//
void CVtEngPubSubsListener::RunL()
    {
    __VTPRINTENTER( "PSLis.RunL" )
    const TRequestStatus status( iStatus );
    StartListening();
    iCallback->HandleNotifyPSL( iUid, iId, status );
    __VTPRINTEXIT( "PSLis.RunL" )
    }

// -----------------------------------------------------------------------------
// CVtEngPubSubsListener::DoCancel
// From CActive.
// -----------------------------------------------------------------------------
//
void CVtEngPubSubsListener::DoCancel()
    {
    __VTPRINTENTER( "PSLis.DoCancel" )
    iProperty.Cancel();
    __VTPRINTEXIT( "PSLis.DoCancel" )
    }

// -----------------------------------------------------------------------------
// CVtEngPubSubsListener::RunError
// From CActive.
// -----------------------------------------------------------------------------
//
TInt CVtEngPubSubsListener::RunError( TInt /*aError*/ )
    {
    __VTPRINTENTER( "PSLis.RunError" )
    __VTPRINTEXIT( "PSLis.RunError" )
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVtEngPubSubsListener::NewL
// -----------------------------------------------------------------------------
//
CVtEngPubSubsListener* CVtEngPubSubsListener::NewL( const TUid aUid, 
    const TInt aKey, MVtEngSettingPSObserver* aObserver )
    {
    __VTPRINTENTER( "PSLis.NewL" )
    CVtEngPubSubsListener* self = new( ELeave ) 
        CVtEngPubSubsListener( aUid, aKey, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    __VTPRINTEXIT( "PSLis.NewL" )
    return self;
    }

// -----------------------------------------------------------------------------
// CVtEngPubSubsListener::StartListening
// -----------------------------------------------------------------------------
//
void CVtEngPubSubsListener::StartListening() 
    {
    iProperty.Subscribe( iStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CVtEngPubSubsListener::ConstructL
// -----------------------------------------------------------------------------
//
void CVtEngPubSubsListener::ConstructL()
    {
    CActiveScheduler::Add( this );
    
    User::LeaveIfError ( iProperty.Attach( iUid, iId, EOwnerThread ) );
    StartListening();
    }

// -----------------------------------------------------------------------------
// CVtEngPubSubsListener::Get
// Read integer value.
// -----------------------------------------------------------------------------
//
TInt CVtEngPubSubsListener::Get( TInt& aVal )
    {
    return iProperty.Get( iUid, iId, aVal );
    }

// -----------------------------------------------------------------------------
// CVtEngPubSubsListener::Get
// Read binary value.
// -----------------------------------------------------------------------------
//
TInt CVtEngPubSubsListener::Get( TDes8& aVal )
    {
    return iProperty.Get( iUid, iId, aVal );
    }
  
// -----------------------------------------------------------------------------
// CVtEngPubSubsListener::Get
// Read string value.
// -----------------------------------------------------------------------------
//      
TInt CVtEngPubSubsListener::Get( TDes16& aVal )
    {
    return iProperty.Get( iUid, iId, aVal );
    }

// End of File
