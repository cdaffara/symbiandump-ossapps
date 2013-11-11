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
*       CMsgErrorCommDbObserver implementation file
*
*/



// INCLUDE FILES
#include    <e32base.h>
#include    <commdb.h>

#include    "msgerrorwatcher_p.h"
#include    "msgerrorcommdbobserver.h"
#include    "debugtraces.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMsgErrorCommDbObserver::CMsgErrorCommDbObserver
//
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CMsgErrorCommDbObserver::CMsgErrorCommDbObserver( CMsgErrorWatcherPrivate& aWatcher )
    : CActive( CActive::EPriorityStandard ),
    iWatcher( aWatcher )
    {
    QDEBUG_WRITE("CMsgErrorCommDbObserver:CMsgErrorCommDbObserver : Enter");
    QDEBUG_WRITE("CMsgErrorCommDbObserver:CMsgErrorCommDbObserver : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorCommDbObserver::ConstructL
//
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CMsgErrorCommDbObserver::ConstructL()
    {
    QDEBUG_WRITE("CMsgErrorCommDbObserver:ConstructL : Enter");
    iDb = CCommsDatabase::NewL( EDatabaseTypeIAP );
    CActiveScheduler::Add( this );
    Start();
    QDEBUG_WRITE("CMsgErrorCommDbObserver:ConstructL : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorCommDbObserver::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
CMsgErrorCommDbObserver* CMsgErrorCommDbObserver::NewL( CMsgErrorWatcherPrivate& aWatcher )
    {
    QDEBUG_WRITE("CMsgErrorCommDbObserver:NewL : Enter");
    CMsgErrorCommDbObserver* self = new ( ELeave )
        CMsgErrorCommDbObserver( aWatcher );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    QDEBUG_WRITE("CMsgErrorCommDbObserver:NewL : Exit");
    return self;
    }

    
// ---------------------------------------------------------
// CMsgErrorCommDbObserver::~CMsgErrorCommDbObserver
//
// Destructor
// ---------------------------------------------------------
//
CMsgErrorCommDbObserver::~CMsgErrorCommDbObserver()
    {
    QDEBUG_WRITE("CMsgErrorCommDbObserver:~CMsgErrorCommDbObserver : Enter");
    Cancel();
    delete iDb;
    QDEBUG_WRITE("CMsgErrorCommDbObserver:~CMsgErrorCommDbObserver : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorCommDbObserver::Restart
//
// (Re)starts the active object (public)
// ---------------------------------------------------------
//
void CMsgErrorCommDbObserver::Restart()
    {
    QDEBUG_WRITE("CMsgErrorCommDbObserver:Restart : Enter");
    Start();
    QDEBUG_WRITE("CMsgErrorCommDbObserver:Restart : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorCommDbObserver::Start
//
// Starts the active object
// ---------------------------------------------------------
//
void CMsgErrorCommDbObserver::Start()
    {
    QDEBUG_WRITE("CMsgErrorCommDbObserver:Start : Enter");
    if ( !IsActive() ) 
        {
        iDb->RequestNotification( iStatus );
        SetActive();       
        }
    QDEBUG_WRITE("CMsgErrorCommDbObserver:Start : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorCommDbObserver::DoCancel
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorCommDbObserver::DoCancel()
    {
    QDEBUG_WRITE("CMsgErrorCommDbObserver:DoCancel : Enter");
    if ( iDb )
        {
        iDb->CancelRequestNotification();
        }
    QDEBUG_WRITE("CMsgErrorCommDbObserver:DoCancel : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorCommDbObserver::RunL
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorCommDbObserver::RunL()
    {
    QDEBUG_WRITE("CMsgErrorCommDbObserver:RunL : Enter");
    if ( iStatus == RDbNotifier::ECommit )
        {
        TRAP_IGNORE( iWatcher.HandleCommDbEventL() );
        }
    else
        {
        Start();
        }
    QDEBUG_WRITE("CMsgErrorCommDbObserver:RunL : Exit");
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  
