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
*       CMsgErrorSmsDiskSpaceObserver implementation file
*
*/



// INCLUDE FILES
#include <e32base.h>

#include <e32property.h>

#include <smsuaddr.h>
#include  "msgerrorwatcher_p.h"
#include "msgerrorsmsdiskspaceobserver.h"
#include "debugtraces.h"
// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMsgErrorSmsDiskSpaceObserver::CMsgErrorSmsDiskSpaceObserver
//
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CMsgErrorSmsDiskSpaceObserver::CMsgErrorSmsDiskSpaceObserver( CMsgErrorWatcherPrivate&  aWatcher )
    : CActive( CActive::EPriorityStandard ),
    iWatcher( aWatcher )
    {
    QDEBUG_WRITE("CMsgErrorSmsDiskSpaceObserver:CMsgErrorSmsDiskSpaceObserver : Enter");
    CActiveScheduler::Add( this );
    QDEBUG_WRITE("CMsgErrorSmsDiskSpaceObserver:CMsgErrorSmsDiskSpaceObserver : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorSmsDiskSpaceObserver::ConstructL()
//
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CMsgErrorSmsDiskSpaceObserver::ConstructL()
    {
    QDEBUG_WRITE("CMsgErrorSmsDiskSpaceObserver:ConstructL : Enter");
    
    TInt success = iSmsDiskSpaceProperty.Attach( KUidPSSMSStackCategory, KUidPSSMSStackDiskSpaceMonitorKey );
    QDEBUG_WRITE_FORMAT("iSmsDiskSpaceProperty.Attach: " ,success );
    User::LeaveIfError(success);            
    iStatus = KRequestPending;
    iSmsDiskSpaceProperty.Subscribe( iStatus );
    SetActive();
    QDEBUG_WRITE("CMsgErrorSmsDiskSpaceObserver:ConstructL : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorSmsDiskSpaceObserver::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
CMsgErrorSmsDiskSpaceObserver* CMsgErrorSmsDiskSpaceObserver::NewL( CMsgErrorWatcherPrivate&  aWatcher)
    {
    QDEBUG_WRITE("CMsgErrorSmsDiskSpaceObserver:NewL : Enter");
    CMsgErrorSmsDiskSpaceObserver* self = new ( ELeave )
        CMsgErrorSmsDiskSpaceObserver( aWatcher);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    QDEBUG_WRITE("CMsgErrorSmsDiskSpaceObserver:NewL : Exit");
    return self;
    }

// ---------------------------------------------------------
// CMsgErrorSmsDiskSpaceObserver::~CMsgErrorSmsDiskSpaceObserver
//
// Destructor
// ---------------------------------------------------------
//
CMsgErrorSmsDiskSpaceObserver::~CMsgErrorSmsDiskSpaceObserver()
    {
    QDEBUG_WRITE("CMsgErrorSmsDiskSpaceObserver:~CMsgErrorSmsDiskSpaceObserver : Enter");
    Cancel();
    iSmsDiskSpaceProperty.Close();
    QDEBUG_WRITE("CMsgErrorSmsDiskSpaceObserver:~CMsgErrorSmsDiskSpaceObserver : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorSmsDiskSpaceObserver::DoCancel
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorSmsDiskSpaceObserver::DoCancel()
    {
    QDEBUG_WRITE("CMsgErrorSmsDiskSpaceObserver:DoCancel : Enter");
    iSmsDiskSpaceProperty.Cancel();
    QDEBUG_WRITE("CMsgErrorSmsDiskSpaceObserver:DoCancel : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorSmsDiskSpaceObserver::RunL
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorSmsDiskSpaceObserver::RunL()
    {
    QDEBUG_WRITE("CMsgErrorSmsDiskSpaceObserver:RunL : Enter");    
    TInt messagesComingInLowDiskSpace(ESmsDiskSpaceUnknown);
    TInt success = iSmsDiskSpaceProperty.Get( messagesComingInLowDiskSpace );
    
   QDEBUG_WRITE_FORMAT("iSmsDiskSpaceProperty.Get returns: " ,success );
   QDEBUG_WRITE_FORMAT("iSmsDiskSpaceProperty.Get: " ,messagesComingInLowDiskSpace );
        
    
    if ( messagesComingInLowDiskSpace == ESmsDiskSpaceFull )
        {
        QDEBUG_WRITE( "CMsgErrorSmsDiskSpaceObserver::RunL: ESmsDiskSpaceFull" );
        TRAP_IGNORE( iWatcher.HandleDiskSpaceEvent2L() );
        }

    iStatus = KRequestPending;
    iSmsDiskSpaceProperty.Subscribe( iStatus );
    SetActive();
    QDEBUG_WRITE("CMsgErrorSmsDiskSpaceObserver:RunL : Exit");
    }



// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  
