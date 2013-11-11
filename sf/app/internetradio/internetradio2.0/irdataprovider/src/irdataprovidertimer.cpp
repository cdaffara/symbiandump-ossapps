/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/


#include "irdataprovidertimer.h"
#include "irdebug.h"

// ---------------------------------------------------------------------------
// CIRDataProviderTimer::NewL()
// Two phase constructor
// ---------------------------------------------------------------------------
//
CIRDataProviderTimer *CIRDataProviderTimer::NewL( const TInt aPriority,
                                     MIRDataProviderTimeoutNotifier &aNotify )
    {
    IRLOG_DEBUG( "CIRDataProviderTimer::NewL - Entering" );
    CIRDataProviderTimer *self = CIRDataProviderTimer::NewLC( aPriority, 
                                                              aNotify );
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRDataProviderTimer::NewL - Exiting" );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRDataProviderTimer::NewLC()
// ---------------------------------------------------------------------------
//    
CIRDataProviderTimer *CIRDataProviderTimer::NewLC( const TInt aPriority, 
                                     MIRDataProviderTimeoutNotifier &aNotify )
    {
    IRLOG_DEBUG( "CIRDataProviderTimer::NewLC - Entering" );
    CIRDataProviderTimer *self = new( ELeave ) CIRDataProviderTimer( 
                                                         aPriority, aNotify );
    CleanupStack::PushL( self );
    self->ConstructL();
    IRLOG_DEBUG( "CIRDataProviderTimer::NewLC - Exiting" );
    return self;
    }
 
// ---------------------------------------------------------------------------
// CIRDataProviderTimer::~CIRDataProviderTimer()
// Default C++ destructor
// ---------------------------------------------------------------------------
//    
CIRDataProviderTimer::~CIRDataProviderTimer()
    {
    IRLOG_DEBUG( "CIRDataProviderTimer::~CIRDataProviderTimer" );
    // Implementation not required
    }

// ---------------------------------------------------------------------------
// Derived from CTimer
// CIRDataProviderTimer::RunL()
// ---------------------------------------------------------------------------
//  
void CIRDataProviderTimer::RunL()
    {
    IRLOG_DEBUG( "CIRDataProviderTimer::RunL - Entering" );
    if ( iStatus == KErrNone )
        {
        iNotify.TimerExpired();
        }
    else
        {
        User::Leave( iStatus.Int() );
        }
    IRLOG_DEBUG( "CIRDataProviderTimer::RunL - Exiting" );
    }
    
// ---------------------------------------------------------------------------
// Derived from CTimer
// CIRDataProviderTimer::RunError()
// ---------------------------------------------------------------------------
//    
TInt CIRDataProviderTimer::RunError( TInt /*aError*/ )
    {
    IRLOG_DEBUG( "CIRDataProviderTimer::RunError" );
        return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CIRDataProviderTimer::CIRDataProviderTimer()
// ---------------------------------------------------------------------------
//        
CIRDataProviderTimer::CIRDataProviderTimer( const TInt aPriority, 
                                     MIRDataProviderTimeoutNotifier &aNotify )
 : CTimer( aPriority ),
   iNotify( aNotify )
    {
    IRLOG_DEBUG( "CIRDataProviderTimer::CIRDataProviderTimer" );
    }

// ---------------------------------------------------------------------------
// CIRDataProviderTimer::ConstructL()
// Second phase constructor
// ---------------------------------------------------------------------------
//    
void CIRDataProviderTimer::ConstructL()
    {
    IRLOG_DEBUG( "CIRDataProviderTimer::ConstructL - Entering" );
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    IRLOG_DEBUG( "CIRDataProviderTimer::ConstructL - Exiting" );
    }

