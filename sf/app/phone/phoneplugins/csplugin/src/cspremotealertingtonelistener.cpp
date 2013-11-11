/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Listens remote alerting tone status changes
*
*/


#include "cspremotealertingtonelistener.h"
#include "mcspremotealertingtoneobserver.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CSPRemoteAlertingToneListener::CSPRemoteAlertingToneListener(
    RMmCustomAPI& aCustomAPI,
    MCSPRemoteAlertingToneObserver& aObserver ) : 
    CActive( CActive::EPriorityStandard ),
    iCustomAPI( aCustomAPI ),
    iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Static constructor
// ---------------------------------------------------------------------------
//
CSPRemoteAlertingToneListener* CSPRemoteAlertingToneListener::NewL( 
    RMmCustomAPI& aCustomAPI,
    MCSPRemoteAlertingToneObserver& aObserver )
    {
    CSPRemoteAlertingToneListener* self = 
        new( ELeave ) CSPRemoteAlertingToneListener( 
            aCustomAPI, aObserver );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSPRemoteAlertingToneListener::~CSPRemoteAlertingToneListener()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// Starts listening the remote alerting tone changes, if not already started
// ---------------------------------------------------------------------------
//
void CSPRemoteAlertingToneListener::StartListening()
    {
    if ( !IsActive() )
        {
        iCustomAPI.NotifyRemoteAlertingToneStatusChange( iStatus, iRATStatus );
        SetActive();
        }
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Cancels the oustanding request.
// ---------------------------------------------------------------------------
//
void CSPRemoteAlertingToneListener::DoCancel()
    {
    iCustomAPI.CancelAsyncRequest(
        ECustomNotifyRemoteAlertingToneStatusChangeIPC );
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Starts listening next change and notifies the observer if no errors 
// in request completion.
// ---------------------------------------------------------------------------
//
void CSPRemoteAlertingToneListener::RunL()
    {
    const TInt error( iStatus.Int() );
    if ( error != KErrNotSupported )
        {
        StartListening();
        }
    if ( error == KErrNone )
        {
        iObserver.RemoteAlertingToneStatusChanged( iRATStatus );
        }
    }
