/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Monitors device lock state
*
*/



// INCLUDE FILES
#include    "CVtEngDeviceLockMonitor.h"
#include    "CVTEngPubSubsListener.h"
#include    "CVtEngEventManager.h"
#include    "cvtlogger.h"

#include <coreapplicationuisdomainpskeys.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngDeviceLockMonitor::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVtEngDeviceLockMonitor* CVtEngDeviceLockMonitor::NewL()
    {
    CVtEngDeviceLockMonitor* self = new ( ELeave ) CVtEngDeviceLockMonitor();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CVtEngDeviceLockMonitor::~CVtEngDeviceLockMonitor()
    {
    delete iAutolockListener;
    }

// -----------------------------------------------------------------------------
// CVtEngDeviceLockMonitor::NotifyState
// Notifies UI about device lock state
// -----------------------------------------------------------------------------
//
void CVtEngDeviceLockMonitor::NotifyState() const
    {
    __VTPRINTENTER( "LockMonitor.NotifyState" )

    TInt state = EAutolockStatusUninitialized;

	// Get autolock state
	TInt err = iAutolockListener->Get( state );

	__VTPRINT2( DEBUG_GEN, "LockMonitor Get err: %d", err );
	__VTPRINT2( DEBUG_GEN, "LockMonitor state: %d", state );

	if( err == KErrNone )
		{
		const TInt event =
        ( state <= EAutolockOff ) ?
                KVtEngDeviceLockOff : KVtEngDeviceLockOn;
        CVtEngEventManager::NotifyEvent( event );
        }		
			
    __VTPRINTEXIT( "LockMonitor.NotifyState" )
    }

// -----------------------------------------------------------------------------
// CVtEngDeviceLockMonitor::HandleNotifyPSL
// Callback for autolock state change
// -----------------------------------------------------------------------------
//
void CVtEngDeviceLockMonitor::HandleNotifyPSL(
    const TUid aUid,
    const TInt& aKey,
    const TRequestStatus& aStatus )
    {
    if ( aStatus == KErrNone &&
         aUid == KPSUidCoreApplicationUIs &&
         aKey == KCoreAppUIsAutolockStatus )
        {
        NotifyState();
        }
    }

// -----------------------------------------------------------------------------
// CVtEngDeviceLockMonitor::CVtEngDeviceLockMonitor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngDeviceLockMonitor::CVtEngDeviceLockMonitor()
    {
    }

// -----------------------------------------------------------------------------
// CVtEngDeviceLockMonitor::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVtEngDeviceLockMonitor::ConstructL()
    {
    __VTPRINTENTER( "LockMonitor.ConstructL" )
    // Listener for autolock state
    iAutolockListener = CVtEngPubSubsListener::NewL(
        KPSUidCoreApplicationUIs, KCoreAppUIsAutolockStatus, this );
    __VTPRINTEXIT( "LockMonitor.ConstructL" )
    }

//  End of File
