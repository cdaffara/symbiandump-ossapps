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
* Description:  Client-server preset notifier
*
*/


#include "pscommon.h"
#include "psserv.h"
#include "psservnotifier.h"

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
RPSServNotifier::RPSServNotifier( RPSServ& aServer, TInt aId )
    : iServer( aServer ), iData(), iId( aId )
    {
    // Initialize the last handled notification identifier to zero as the default constructor doesn't do it for us.
    iData().iNotificationId = 0; 
    }

// ---------------------------------------------------------------------------
// Closes the notifier.
// ---------------------------------------------------------------------------
//
void RPSServNotifier::Close()
    {
    CancelNotify();
    }

// ---------------------------------------------------------------------------
// Requests a notify when a preset changes.
// ---------------------------------------------------------------------------
//
void RPSServNotifier::Notify( TRequestStatus& aStatus )
    {
    iServer.SendReceive( EPSOpNotify, TIpcArgs( reinterpret_cast<TInt>( this ), iId, &iData ), aStatus );
    }

// ---------------------------------------------------------------------------
// Cancels a pending notify.
// ---------------------------------------------------------------------------
//
void RPSServNotifier::CancelNotify()
    {
    iServer.SendReceive( EPSOpCancelNotify, TIpcArgs( reinterpret_cast<TInt>( this ) ) );
    }

// ---------------------------------------------------------------------------
// Returns the id of the preset that was changed.
// ---------------------------------------------------------------------------
//
TInt RPSServNotifier::Id() const
    {
    return iData().iId;
    }

// ---------------------------------------------------------------------------
// Returns the data handler of the preset that was changed.
// ---------------------------------------------------------------------------
//
TUid RPSServNotifier::DataHandler() const
    {
    return iData().iDataHandler;
    }

// ---------------------------------------------------------------------------
// Returns the reason for the change.
// ---------------------------------------------------------------------------
//
MPSPresetObserver::TPSReason RPSServNotifier::Reason() const
    {
    return iData().iReason;
    }

// ======== GLOBAL FUNCTIONS ========
