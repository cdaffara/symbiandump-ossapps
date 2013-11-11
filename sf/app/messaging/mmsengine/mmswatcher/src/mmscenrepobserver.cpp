/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     This class observes given cenrep value.
*
*/



// INCLUDE FILES
//#include    <es_sock.h>
#include <centralrepository.h>

#include "mmscenrepobserver.h"
#include "mmswatcherdebuglogging.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------
//
CMmsCenRepObserver::CMmsCenRepObserver()
    :CActive ( EPriorityStandard),
    iRequester ( NULL )
    {
    }

// ---------------------------------------------------------
// ConstructL
// ---------------------------------------------------------
//
void CMmsCenRepObserver::ConstructL()
    {
    LOG( _L("CMmsCenRepObserver::ConstructL") );
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// Symbian two-phased constructor
// ---------------------------------------------------------
//
CMmsCenRepObserver* CMmsCenRepObserver::NewL()
    {
    CMmsCenRepObserver* self = new ( ELeave ) CMmsCenRepObserver();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CMmsCenRepObserver::~CMmsCenRepObserver()
    {
    Cancel();
    if( iRepository )
        {
        delete iRepository;
        }
    }

// ---------------------------------------------------------
// SubscribeNotification
// ---------------------------------------------------------
//
void CMmsCenRepObserver::SubscribeNotification(
    const TUid aRepositoryUid,
    const TInt32 aKey, 
    MMmsCenRepNotifyHandler* aRequester )
    {
    LOG( _L("CMmsCenRepObserver::SubscribeNotification") );
    
    // Store parameters
    iRepositoryUid = aRepositoryUid;
    iKey = aKey;
    iRequester = ( MMmsCenRepNotifyHandler* )aRequester;
    
    // Connect CenRep
    TRAPD( err, iRepository = CRepository::NewL( aRepositoryUid ) );
    if( err != KErrNone )
        {
        LOG( _L("- ERROR connecting repository! Not observing..") );
        return;
        }
    
    // Subscribe
    iRepository->NotifyRequest( iKey, iStatus );
    SetActive();
    }

// ---------------------------------------------------------
// RunL
// ---------------------------------------------------------
//
void CMmsCenRepObserver::RunL()
    {
    LOG( _L("CMmsCenRepObserver::RunL()") );

    // Get the value
    TInt value = 0;
    iRepository->Get( iKey, value );

    // Re-subscribe before notifying The Master
    iRepository->NotifyRequest( iKey, iStatus );
    SetActive();

    // Notify requester
    iRequester->HandleCenRepNotificationL(
        iRepositoryUid,
        iKey, 
        value );
    }

// ---------------------------------------------------------
// DoCancel
// ---------------------------------------------------------
//
void CMmsCenRepObserver::DoCancel()
    {
    LOG( _L("CMmsCenRepObserver::DoCancel") );
    iRepository->NotifyCancel( iKey );
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  

