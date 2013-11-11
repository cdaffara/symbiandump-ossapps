/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*       CMsgErrorStartupObserver implementation file
*
*/



// INCLUDE FILES
#include <e32base.h>

#include <startupdomainpskeys.h>

#include  "msgerrorwatcher_p.h"
#include  "msgerrorstartupobserver.h"
#include "debugtraces.h"


// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMsgErrorStartupObserver::CMsgErrorStartupObserver
//
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CMsgErrorStartupObserver::CMsgErrorStartupObserver( CMsgErrorWatcherPrivate&  aWatcher )
    : CActive( CActive::EPriorityStandard ),
    iWatcher( aWatcher )
    {
    QDEBUG_WRITE("CMsgErrorStartupObserver:CMsgErrorStartupObserver : Enter");
    CActiveScheduler::Add( this );
    QDEBUG_WRITE("CMsgErrorStartupObserver:CMsgErrorStartupObserver : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorStartupObserver::ConstructL()
//
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CMsgErrorStartupObserver::ConstructL()
    {
    QDEBUG_WRITE("CMsgErrorStartupObserver:ConstructL : Enter");
    User::LeaveIfError( iStartupProperty.Attach( KPSUidStartup, KPSGlobalSystemState ) );
    // Complete self
    iStatus = KRequestPending;
    TRequestStatus* pStatus = &iStatus;
    SetActive();
    User::RequestComplete( pStatus, KErrNone );
    QDEBUG_WRITE("CMsgErrorStartupObserver:ConstructL : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorStartupObserver::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
CMsgErrorStartupObserver* CMsgErrorStartupObserver::NewL( CMsgErrorWatcherPrivate&  aWatcher )
    {
    QDEBUG_WRITE("CMsgErrorStartupObserver:NewL : Enter");
    CMsgErrorStartupObserver* self = new ( ELeave )
        CMsgErrorStartupObserver( aWatcher );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    QDEBUG_WRITE("CMsgErrorStartupObserver:NewL : Exit");
    return self;
    }

// ---------------------------------------------------------
// CMsgErrorStartupObserver::~CMsgErrorStartupObserver
//
// Destructor
// ---------------------------------------------------------
//
CMsgErrorStartupObserver::~CMsgErrorStartupObserver()
    {
    QDEBUG_WRITE("CMsgErrorStartupObserver:~CMsgErrorStartupObserver : Enter");
    Cancel();
    iStartupProperty.Close();
    QDEBUG_WRITE("CMsgErrorStartupObserver:~CMsgErrorStartupObserver : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorStartupObserver::DoCancel
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorStartupObserver::DoCancel()
    {
    QDEBUG_WRITE("CMsgErrorStartupObserver:DoCancel : Enter");
    iStartupProperty.Cancel();
    QDEBUG_WRITE("CMsgErrorStartupObserver:DoCancel : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorStartupObserver::RunL
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorStartupObserver::RunL()
    {
    QDEBUG_WRITE("CMsgErrorStartupObserver:RunL : Enter");
    TInt startupState = 0;
    iStartupProperty.Get( startupState );

    if ( startupState == ESwStateNormalRfOn ||
        startupState == ESwStateNormalRfOff ||
        startupState == ESwStateNormalBTSap )
        {
        TRAP_IGNORE( iWatcher.HandleStartupReadyL() );
        }
    else
        {
        iStatus = KRequestPending;
        iStartupProperty.Subscribe( iStatus );
        SetActive();
        }
    QDEBUG_WRITE("CMsgErrorStartupObserver:RunL : Exit");
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  
