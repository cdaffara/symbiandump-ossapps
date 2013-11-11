/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of the public preset notifier
*
*/


#include "pspresetnotifier.h"
#include "psserv.h"
#include "psservnotifier.h"

const TInt KPSDefaultNotifierPriority = CActive::EPriorityLow; // Priority of the non-preset notifiers.
const TInt KPSPresetNotifierPriority = CActive::EPriorityStandard; // Priority of the preset notifiers.

const TInt KPSFailedRequestDelay = 5000000; // The delay after which a failed notify request is reissued to the server.

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CPSPresetNotifier* CPSPresetNotifier::NewL( RPSServ& aServ, MPSPresetObserver& aObserver )
    {
    CPSPresetNotifier* self = new ( ELeave ) CPSPresetNotifier( aServ, aObserver, KPSDefaultNotifierPriority );
    CleanupStack::PushL( self );
    self->ConstructL( KErrNotFound );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Non-exported two-phased constructor.
// ---------------------------------------------------------------------------
//
CPSPresetNotifier* CPSPresetNotifier::NewL( RPSServ& aServ, MPSPresetObserver& aObserver, TInt aId )
    {
    CPSPresetNotifier* self = new ( ELeave ) CPSPresetNotifier( aServ, aObserver, KPSPresetNotifierPriority );
    CleanupStack::PushL( self );
    self->ConstructL( aId );
    CleanupStack::Pop( self );
    return self;
    }     

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CPSPresetNotifier::CPSPresetNotifier( RPSServ& aServ, MPSPresetObserver& aObserver, TInt aPriority )
    : CActive( aPriority ), iServ( aServ ), iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }
        
// ---------------------------------------------------------------------------
// Second-phase constructor.
// ---------------------------------------------------------------------------
//
void CPSPresetNotifier::ConstructL( TInt aId )
    {
    iTimer = CPeriodic::NewL( CActive::EPriorityStandard );

    iNotifier = new ( ELeave ) RPSServNotifier( iServ, aId );
    iNotifier->Notify( iStatus );

    SetActive();
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CPSPresetNotifier::~CPSPresetNotifier()
    {
    Cancel();

    delete iNotifier;
    delete iTimer;
    }

// ---------------------------------------------------------------------------
// Cancels the pending notify.
// ---------------------------------------------------------------------------
//
void CPSPresetNotifier::DoCancel()
    {
    if ( iNotifier )
        {
        iNotifier->CancelNotify();
        }
    
    if ( iTimer )
        {
        iTimer->Cancel();
        }
    }
    
// ---------------------------------------------------------------------------
// Invoked when a preset is changed.
// ---------------------------------------------------------------------------
//
void CPSPresetNotifier::RunL()
    {
    User::LeaveIfError( iStatus.Int() );
    iObserver.HandlePresetChangedL( iNotifier->Id(), iNotifier->DataHandler(), iNotifier->Reason() );

    // Requesting the new notification must be done after the observer has been
    // notified of the current one, as the request may complete synchronously on the 
    // server side, causing it to overwrite the existing notify package and thus notifying
    // the observer incorrectly.
    
    iNotifier->Notify( iStatus );
    SetActive();
    }

// ---------------------------------------------------------------------------
// Called when RunL leaves.
// ---------------------------------------------------------------------------
//
TInt CPSPresetNotifier::RunError( TInt /*aError*/ )
    {
    // After a notification fails for any reason, whether it is because the server side
    // code has caused a leave or the observer's implementation of preset handling
    // call back fails, we will request a new notification with a set delay. This
    // prevents the notifier from being in a state where it can no longer keep track
    // of its state.

    iTimer->Cancel();
    iTimer->Start( KPSFailedRequestDelay, KPSFailedRequestDelay, TCallBack( StaticDelayedNotifyCallBack, this ) );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Static notify call back that is used after a failed notification.
// ---------------------------------------------------------------------------
//
TInt CPSPresetNotifier::StaticDelayedNotifyCallBack( TAny* aSelf )
    {
    CPSPresetNotifier* self = static_cast<CPSPresetNotifier*>( aSelf );
    if ( self )
        {
        self->iTimer->Cancel();

        self->iNotifier->Notify( self->iStatus );
        self->SetActive();
        }

    return KErrNone;
    }

// ======== GLOBAL FUNCTIONS ========

