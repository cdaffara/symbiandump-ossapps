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
*       CMsgErrorExtSmsDiskSpaceObserver implementation file
*
*
*/


// INCLUDE FILES
#include <e32base.h>

#include <e32property.h>

#include <smsuaddr.h>
#include "msgerrorwatcher_p.h"
#include "msgerrorextsmsdiskspaceobserver.h"
#include "debugtraces.h"
// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMsgErrorExtSmsDiskSpaceObserver::CMsgErrorExtSmsDiskSpaceObserver
//
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CMsgErrorExtSmsDiskSpaceObserver::CMsgErrorExtSmsDiskSpaceObserver( CMsgErrorWatcherPrivate& aWatcher )
    : CActive( CActive::EPriorityStandard ),
    iWatcher( aWatcher )
    {
    QDEBUG_WRITE("CMsgErrorExtSmsDiskSpaceObserver:CMsgErrorExtSmsDiskSpaceObserver : Enter");
    CActiveScheduler::Add( this );
    QDEBUG_WRITE("CMsgErrorExtSmsDiskSpaceObserver:CMsgErrorExtSmsDiskSpaceObserver : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorExtSmsDiskSpaceObserver::ConstructL()
//
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CMsgErrorExtSmsDiskSpaceObserver::ConstructL()
    {
    QDEBUG_WRITE("CMsgErrorExtSmsDiskSpaceObserver:ConstructL : Enter");            
    TInt success = iSmsDiskSpaceProperty.Attach( KUidSystemCategory, KUidPSDiskSpaceMonitorKeyType );
    QDEBUG_WRITE_FORMAT("iSmsDiskSpaceProperty.Attach: " ,success );
    User::LeaveIfError(success);        
    QDEBUG_WRITE( " AFTER ATTACH" );
    iStatus = KRequestPending;
    iSmsDiskSpaceProperty.Subscribe( iStatus );
    SetActive();
    QDEBUG_WRITE("CMsgErrorExtSmsDiskSpaceObserver:ConstructL : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorExtSmsDiskSpaceObserver::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
CMsgErrorExtSmsDiskSpaceObserver* CMsgErrorExtSmsDiskSpaceObserver::NewL( CMsgErrorWatcherPrivate& aWatcher)
    {
    QDEBUG_WRITE("CMsgErrorExtSmsDiskSpaceObserver:NewL : Enter");
    CMsgErrorExtSmsDiskSpaceObserver* self = new ( ELeave )
        CMsgErrorExtSmsDiskSpaceObserver( aWatcher);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    QDEBUG_WRITE("CMsgErrorExtSmsDiskSpaceObserver:NewL : Exit");
    return self;
    }

// ---------------------------------------------------------
// CMsgErrorExtSmsDiskSpaceObserver::~CMsgErrorExtSmsDiskSpaceObserver
//
// Destructor
// ---------------------------------------------------------
//
CMsgErrorExtSmsDiskSpaceObserver::~CMsgErrorExtSmsDiskSpaceObserver()
    {
    QDEBUG_WRITE("CMsgErrorExtSmsDiskSpaceObserver:~CMsgErrorExtSmsDiskSpaceObserver : Enter");
    Cancel();
    iSmsDiskSpaceProperty.Close();
    QDEBUG_WRITE("CMsgErrorExtSmsDiskSpaceObserver:~CMsgErrorExtSmsDiskSpaceObserver : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorExtSmsDiskSpaceObserver::DoCancel
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorExtSmsDiskSpaceObserver::DoCancel()
    {
    QDEBUG_WRITE("CMsgErrorExtSmsDiskSpaceObserver:DoCancel : Enter");
    iSmsDiskSpaceProperty.Cancel();
    QDEBUG_WRITE("CMsgErrorExtSmsDiskSpaceObserver:DoCancel : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorExtSmsDiskSpaceObserver::RunL
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorExtSmsDiskSpaceObserver::RunL()
    {
    QDEBUG_WRITE("CMsgErrorExtSmsDiskSpaceObserver:RunL : Enter");
    TInt messagesComingInLowDiskSpace(ESmsDiskSpaceUnknown);
    TInt success = iSmsDiskSpaceProperty.Get( messagesComingInLowDiskSpace );
    
   QDEBUG_WRITE_FORMAT("iSmsDiskSpaceProperty.Get returns: " ,success );
   QDEBUG_WRITE_FORMAT("iSmsDiskSpaceProperty.Get:  " ,messagesComingInLowDiskSpace );
        
    if ( messagesComingInLowDiskSpace == ESmsDiskSpaceFull )
        {
        QDEBUG_WRITE( "CMsgErrorExtSmsDiskSpaceObserver::RunL: ESmsDiskSpaceFull" );
        TRAP_IGNORE( iWatcher.HandleDiskSpaceEvent2L() );
        }

    iStatus = KRequestPending;
    iSmsDiskSpaceProperty.Subscribe( iStatus );
    SetActive();
    QDEBUG_WRITE("CMsgErrorExtSmsDiskSpaceObserver:RunL : Exit");
    }



// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  
