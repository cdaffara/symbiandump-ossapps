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
*          CMsgCenRepObserver implementation file
*
*/



// INCLUDE FILES
//#include    <es_sock.h>
#include <centralrepository.h>

#include "msgerrorwatcher_p.h"
#include "msgcenrepobserver.h"
 
#include "MmsEngineInternalCRKeys.h"
#include "debugtraces.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------
//
CMsgCenRepObserver::CMsgCenRepObserver(CMsgErrorWatcherPrivate& aWatcher)
    :CActive ( EPriorityStandard),
    iWatcher(aWatcher),
    iRepositoryUid( KCRUidMmsEngine ),
    iKey( KMmsAccesspointArrayBase/*KMmsEngineAccessPointCount*/ )
    {
    QDEBUG_WRITE("CMsgCenRepObserver:CMsgCenRepObserver : Enter");
    QDEBUG_WRITE("CMsgCenRepObserver:CMsgCenRepObserver : Exit");
    }

// ---------------------------------------------------------
// ConstructL
// ---------------------------------------------------------
//
void CMsgCenRepObserver::ConstructL()
    {
    QDEBUG_WRITE("CMsgCenRepObserver:ConstructL : Enter");
    CActiveScheduler::Add( this );

    // Connect CenRep
    TRAPD( err, iRepository = CRepository::NewL( iRepositoryUid ) );
    if( err != KErrNone )
        {

        QDEBUG_WRITE( "Error in connecting to CenRep!" );

        return;
        }
    QDEBUG_WRITE("CMsgCenRepObserver:ConstructL : Exit");  
    }

// ---------------------------------------------------------
// Symbian two-phased constructor
// ---------------------------------------------------------
//
CMsgCenRepObserver* CMsgCenRepObserver::NewL( CMsgErrorWatcherPrivate& aWatcher )
    {
    QDEBUG_WRITE("CMsgCenRepObserver:NewL : Enter");
    CMsgCenRepObserver* self = new ( ELeave ) CMsgCenRepObserver( aWatcher );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    QDEBUG_WRITE("CMsgCenRepObserver:NewL : Exit");
    return self;
    }
    
// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CMsgCenRepObserver::~CMsgCenRepObserver()
    {
    QDEBUG_WRITE("CMsgCenRepObserver:~CMsgCenRepObserver : Enter");
    Cancel();
    if( iRepository )
        {
        delete iRepository;
        }
    QDEBUG_WRITE("CMsgCenRepObserver:~CMsgCenRepObserver : Exit");
    }

// ---------------------------------------------------------
// SubscribeNotification
// ---------------------------------------------------------
//
void CMsgCenRepObserver::SubscribeNotification( )
    {
    QDEBUG_WRITE("CMsgCenRepObserver:SubscribeNotification : Enter");
    // Subscribe
    iRepository->NotifyRequest( iKey, iStatus );
    SetActive();
    QDEBUG_WRITE("CMsgCenRepObserver:SubscribeNotification : Exit");
    }

// ---------------------------------------------------------
// RunL
// ---------------------------------------------------------
//
void CMsgCenRepObserver::RunL()
    {
    QDEBUG_WRITE("CMsgCenRepObserver:RunL : Enter");
    iWatcher.HandleCenRepNotificationL();    
    QDEBUG_WRITE("CMsgCenRepObserver:RunL : Exit");
    }

// ---------------------------------------------------------
// RunError
// ---------------------------------------------------------
//
TInt CMsgCenRepObserver::RunError( TInt /*aError*/ )
    {
    QDEBUG_WRITE("CMsgCenRepObserver:RunError : Enter");
    QDEBUG_WRITE("CMsgCenRepObserver:RunError : Exit");
	return KErrNone;	
    }

// ---------------------------------------------------------
// DoCancel
// ---------------------------------------------------------
//
void CMsgCenRepObserver::DoCancel()
    {
    QDEBUG_WRITE("CMsgCenRepObserver:DoCancel : Enter");
    iRepository->NotifyCancel( iKey );
    QDEBUG_WRITE("CMsgCenRepObserver:DoCancel : Exit");
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  

