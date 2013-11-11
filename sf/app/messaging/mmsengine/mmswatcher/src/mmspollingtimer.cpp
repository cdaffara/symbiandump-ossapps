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
*     This creates an event at regular intervals.
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <msvapi.h>
#include <msvids.h>

#include "mmspollingtimer.h"
#include "mmswatcherdebuglogging.h"
#include "mmsconst.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
const TInt KMmsFirstPollingInterval = 5000000; // 5 seconds

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMmsPollingTimer::CMmsPollingTimer
// ---------------------------------------------------------
//
CMmsPollingTimer::CMmsPollingTimer()
    :CTimer ( EPriorityStandard )
    {   
    }

// ---------------------------------------------------------
// CMmsPollingTimer::ConstructL
// ---------------------------------------------------------
//
void CMmsPollingTimer::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CMmsPollingTimer::NewL
// ---------------------------------------------------------
//
CMmsPollingTimer* CMmsPollingTimer::NewL()
    {
    CMmsPollingTimer* self = new ( ELeave ) CMmsPollingTimer;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CMmsPollingTimer::~CMmsPollingTimer
// ---------------------------------------------------------
//
CMmsPollingTimer::~CMmsPollingTimer()
    {
    Cancel();        
    }

// ---------------------------------------------------------
// CMmsPollingTimer::Start
// ---------------------------------------------------------
//
void CMmsPollingTimer::Start( MMsvSessionObserver* aRequester, const TInt aTimerValue )
    {
    // Store reference to object that will be called back
    iRequester = (MMsvSessionObserver*) aRequester;
    // Store timeout value that will be used regularly
    iTimeout = aTimerValue;
    // First timeout however will occur after fixed 5 seconds in order to give time for
    // emulator statup
    After( KMmsFirstPollingInterval ); 
    }

// ---------------------------------------------------------
// CMmsPollingTimer::RunL()
// ---------------------------------------------------------
//
void CMmsPollingTimer::RunL()
    {
    LOG( _L("CMmsPollingTimer::RunL") );
    TMsvId tid = KMsvSentEntryId;
    iRequester->HandleSessionEventL( 
        MMsvSessionObserver::EMsvEntriesCreated,
        NULL, 
        &tid, 
        NULL );
    
    After( iTimeout );
    }

// ---------------------------------------------------------
// CMmsPollingTimer::DoCancel()
// ---------------------------------------------------------
//
void CMmsPollingTimer::DoCancel()
    {
    LOG( _L("CMmsPollingTimer::DoCancel") );
    CTimer::DoCancel();
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  

