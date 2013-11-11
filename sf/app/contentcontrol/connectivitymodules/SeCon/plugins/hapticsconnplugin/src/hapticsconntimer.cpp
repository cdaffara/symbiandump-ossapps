/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Connection shutdown timer for haptics server connection.
*
*/


#include "hapticsconntimer.h"

const TInt KHapticsConnectionTimeout = 30000000; // 30 sec.

// ---------------------------------------------------------------------------
// Two phased constructor.
// ---------------------------------------------------------------------------
//
CHapticsConnTimer* CHapticsConnTimer::NewL( MHapticsConnectionShutdown* aCallback )
    {
    CHapticsConnTimer* self = new ( ELeave ) CHapticsConnTimer(aCallback);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CHapticsConnTimer::~CHapticsConnTimer()
    {
    }

// ---------------------------------------------------------------------------
// Activates this timer.
// ---------------------------------------------------------------------------
//
void CHapticsConnTimer::Start()
    {
    After( KHapticsConnectionTimeout );
    }

// ---------------------------------------------------------------------------
// Constructor of the shutdown timer.
// ---------------------------------------------------------------------------
//
CHapticsConnTimer::CHapticsConnTimer( MHapticsConnectionShutdown* aCallback )
    : CTimer( EPriorityStandard ), iCallback(aCallback)
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CHapticsConnTimer::ConstructL()
    {
    CTimer::ConstructL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHapticsConnTimer::RunL()
    {
    iCallback->NotifyShutdown();
    }

//  End of File  
