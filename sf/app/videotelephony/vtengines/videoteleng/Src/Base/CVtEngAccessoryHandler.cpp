/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Accessory listener.
*
*/



// INCLUDE FILES
#include    "CVtEngAccessoryHandler.h"
#include    "MVtEngAccessoryObserver.h"
#include    <cvtlogger.h>
                                                                 
// CONSTANTS
const TInt KVtEngAccObserverGranularity = 1;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngAccessoryHandler::CVtEngAccessoryHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngAccessoryHandler::CVtEngAccessoryHandler() : 
    CActive( CActive::EPriorityStandard ),
    iAccessoryObservers( KVtEngAccObserverGranularity )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CVtEngAccessoryHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVtEngAccessoryHandler::ConstructL()
    {
    User::LeaveIfError( iAccessoryServer.Connect() );
    User::LeaveIfError( iAccessoryMode.CreateSubSession( iAccessoryServer ) );
    StartListening();
    }

// -----------------------------------------------------------------------------
// CVtEngAccessoryHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVtEngAccessoryHandler* CVtEngAccessoryHandler::NewL( )
    {
    __VTPRINTENTER( "AccHdr.NewL" )
    CVtEngAccessoryHandler* self = new( ELeave ) CVtEngAccessoryHandler;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    __VTPRINTEXIT( "AccHdr.NewL" )
    return self;
    }

// -----------------------------------------------------------------------------
// CVtEngAccessoryHandler::~CVtEngAccessoryHandler
// Destructor.
// -----------------------------------------------------------------------------
//
CVtEngAccessoryHandler::~CVtEngAccessoryHandler()
    {
    __VTPRINTENTER( "AccHdr.~" )
    Cancel();
    RemoveAllObservers();
    iAccessoryMode.CloseSubSession();    
    iAccessoryServer.Disconnect();
    __VTPRINTEXIT( "AccHdr.~" )
    }

// -----------------------------------------------------------------------------
// CVtEngAccessoryHandler::RemoveObserver
// Removes an accessory listener.
// -----------------------------------------------------------------------------
//
void CVtEngAccessoryHandler::RemoveObserver( 
    MVtEngAccessoryObserver* aObserver )
    {
    __VTPRINTENTER( "AccHdr.RemoveObserver" )
    TInt pos = iAccessoryObservers.Find( aObserver );
    if ( pos != KErrNotFound )
        {
        iAccessoryObservers.Remove( pos );
        }
    __VTPRINTEXIT( "AccHdr.RemoveObserver" )
    }

// -----------------------------------------------------------------------------
// CVtEngAccessoryHandler::RegisterObserverL
// New accessory listener.
// -----------------------------------------------------------------------------
//
void CVtEngAccessoryHandler::
    RegisterObserverL( MVtEngAccessoryObserver* aObserver )
    {
    __VTPRINTENTER( "AccHdr.RegisterObserverL" )
    TInt pos = iAccessoryObservers.Find( aObserver );
    if ( pos == KErrNotFound )
        {
        User::LeaveIfError( iAccessoryObservers.Append( aObserver ) );
        }
    __VTPRINTEXIT( "AccHdr.RegisterObserverL" )
    }

// -----------------------------------------------------------------------------
// CVtEngAccessoryHandler::RemoveAllObservers
// All observers off they go
// 
// -----------------------------------------------------------------------------
//    
void CVtEngAccessoryHandler::RemoveAllObservers()
    {
    iAccessoryObservers.ResetAndDestroy();
    iAccessoryObservers.Close();
    }

// -----------------------------------------------------------------------------
// CVtEngAccessoryHandler::StartListening
// -----------------------------------------------------------------------------
//
void CVtEngAccessoryHandler::StartListening() 
    {
    iAccessoryMode.NotifyAccessoryModeChanged( iStatus, iMode );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CVtEngAccessoryHandler::RunL
// From CActive.
// -----------------------------------------------------------------------------
//
void CVtEngAccessoryHandler::RunL()
    {
    __VTPRINTENTER( "AccHdr.RunL" )
    StartListening();
    NotifyObserversL();
    __VTPRINTEXIT( "AccHdr.RunL" )
    }

// -----------------------------------------------------------------------------
// CVtEngAccessoryHandler::DoCancel
// From CActive.
// -----------------------------------------------------------------------------
//
void CVtEngAccessoryHandler::DoCancel()
    {
    __VTPRINTENTER( "AccHdr.DoCancel" )
    iAccessoryMode.CancelNotifyAccessoryModeChanged();
    __VTPRINTEXIT( "AccHdr.DoCancel" )
    }

// -----------------------------------------------------------------------------
// CVtEngAccessoryHandler::RunError
// From CActive.
// -----------------------------------------------------------------------------
//
TInt CVtEngAccessoryHandler::RunError( TInt /*aError*/ )
    {
    __VTPRINTENTER( "AccHdr.RunError" )
    __VTPRINTEXIT( "AccHdr.RunError" )
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVtEngAccessoryHandler::NotifyObserversL.
// -----------------------------------------------------------------------------
//
void CVtEngAccessoryHandler::NotifyObserversL()
    {
    const TInt obsCount( iAccessoryObservers.Count() );
    for ( TInt i = 0; i < obsCount; i++ )
        {
        MVtEngAccessoryObserver* obs = iAccessoryObservers[ i ];
        if ( obs )
            {
            obs->AccessoryModeChanged( 
                iMode.iAccessoryMode, iMode.iAudioOutputStatus );
            }
        }
    }

// -----------------------------------------------------------------------------
// CVtEngAccessoryHandler::CMonitor::AccessoryMode
// -----------------------------------------------------------------------------
//
TInt CVtEngAccessoryHandler::
    AccessoryMode( TAccPolAccessoryMode& aAccessoryMode) const
    {
    return iAccessoryMode.GetAccessoryMode( aAccessoryMode );
    }

// end of file
