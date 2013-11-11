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
* Description:  Monitors device incoming call state
*
*/


// INCLUDE FILES
#include    "cvtengincomingcallmonitor.h"
#include    "CVTEngPubSubsListener.h"
#include    "CVtEngEventManager.h"
#include    "cvtlogger.h"

#include 	<ctsydomainpskeys.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngIncomingCallMonitor::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVtEngIncomingCallMonitor* CVtEngIncomingCallMonitor::NewL()
    {
    CVtEngIncomingCallMonitor* self = new ( ELeave ) CVtEngIncomingCallMonitor();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// 
// -----------------------------------------------------------------------------
// CVtEngIncomingCallMonitor::~
// Destructor.
// -----------------------------------------------------------------------------
//
CVtEngIncomingCallMonitor::~CVtEngIncomingCallMonitor()
    {
    delete iIncomingCallListener;
    }

// -----------------------------------------------------------------------------
// CVtEngIncomingCallMonitor::NotifyState
// Notifies UI about device incoming call state
// -----------------------------------------------------------------------------
//
void CVtEngIncomingCallMonitor::NotifyState() const
    {
    __VTPRINTENTER( "IncomingCall.NotifyState" )
	
	// Initialize state as first valuea of call state enum
    TInt state = EPSCTsyCallStateUninitialized;
	
	// Get incomig call state and send event to UI
    if ( iIncomingCallListener->Get( state ) == KErrNone ) 
    	{
    	__VTPRINT2( DEBUG_SESSION, "IncomingCall.NotifyState=%d", state )
        const TInt event = ( state == EPSCTsyCallStateRinging ) ?
                KVtEngSessionWaitingCallActive : KVtEngSessionWaitingCallInactive;
        CVtEngEventManager::NotifyEvent( event );
        }
    __VTPRINTEXIT( "IncomingCall.NotifyState" )
    }

// -----------------------------------------------------------------------------
// CVtEngIncomingCallMonitoronitor::HandleNotifyPSL
// Callback for incoming call state change
// -----------------------------------------------------------------------------
//
void CVtEngIncomingCallMonitor::HandleNotifyPSL(
    const TUid aUid,
    const TInt& aKey,
    const TRequestStatus& aStatus )
    {
    if ( aStatus == KErrNone &&
         aUid == KPSUidCtsyCallInformation &&
         aKey == KCTsyCallState )
        {
        NotifyState();
        }
    }

// -----------------------------------------------------------------------------
// CVtEngIncomingCallMonitor::CVtEngIncomingCallMonitor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngIncomingCallMonitor::CVtEngIncomingCallMonitor()
    {
    }

// -----------------------------------------------------------------------------
// CVtEngIncomingCallMonitor::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVtEngIncomingCallMonitor::ConstructL()
    {
    __VTPRINTENTER( "IncomingCall.ConstructL" )
    // Listener for incoming call state
    iIncomingCallListener = CVtEngPubSubsListener::NewL(
    	KPSUidCtsyCallInformation, KCTsyCallState, this );
    __VTPRINTEXIT( "IncomingCall.ConstructL" )
    }

//  End of File
