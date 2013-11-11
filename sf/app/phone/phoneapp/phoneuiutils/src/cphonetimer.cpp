/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*   See header.
*
*/


// INCLUDE FILES
#include "cphonetimer.h"
#include "phoneui.pan"
#include "phonelogger.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CPhoneTimer::CPhoneTimer
// ---------------------------------------------------------
//
CPhoneTimer::CPhoneTimer( TInt aPriority ) :
    CTimer( aPriority)
    {
    __LOGMETHODSTARTEND(EPhoneUIUtils, "CPhoneTimer::CPhoneTimer() ");
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CPhoneTimer::ConstructL
// ---------------------------------------------------------
//
void CPhoneTimer::ConstructL()
    {
    __LOGMETHODSTARTEND(EPhoneUIUtils, "CPhoneTimer::ConstructL() ");
    CTimer::ConstructL();
    }

// ---------------------------------------------------------
// CPhoneTimer::NewL
// ---------------------------------------------------------
//
EXPORT_C CPhoneTimer* CPhoneTimer::NewL( TInt aPriority )
    {
    __LOGMETHODSTARTEND(EPhoneUIUtils, "CPhoneTimer::NewL() ");
    CPhoneTimer* self = new (ELeave) CPhoneTimer( aPriority );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
// ---------------------------------------------------------
// CPhoneTimer::~CPhoneTimer
// ---------------------------------------------------------
//
EXPORT_C CPhoneTimer::~CPhoneTimer()
    {
    __LOGMETHODSTARTEND(EPhoneUIUtils, "CPhoneTimer::~CPhoneTimer() ");
    Cancel();    
    }

// ---------------------------------------------------------
// CPhoneTimer::RunL()
// ---------------------------------------------------------
//
void CPhoneTimer::RunL()
    {
    __PHONELOG1( EBasic, EPhoneUIUtils, 
        "CPhoneTimer::RunL iStatus(%d)", 
        iStatus.Int() );
    
    if ( iStatus != KErrNone )
        {
        //error code is ignored, as CPeriodic. 
        return;
        }
   
    if ( !iTimerObserver )
        {
        __PHONELOG( EBasic, EPhoneUIUtils, 
            "CPhoneTimer::RunL CallBack" );
        iCallBack.CallBack();
        }
    else
        {
        __PHONELOG( EBasic, EPhoneUIUtils, 
            "CPhoneTimer::RunL HandleTimeOutL" );
        iTimerObserver->HandleTimeOutL();
        }
    }

// ---------------------------------------------------------
// CPhoneTimer::After()
// ---------------------------------------------------------
//
EXPORT_C void CPhoneTimer::After( 
    TTimeIntervalMicroSeconds32 anInterval, 
    TCallBack aCallBack )
    {
    __LOGMETHODSTARTEND(EPhoneUIUtils, "CPhoneTimer::After() ");
    
    if ( IsActive() )
        {
        Cancel();
        }
    iTimerObserver = NULL;
    iCallBack = aCallBack;
    CTimer::After( anInterval );
    }

// ---------------------------------------------------------
// CPhoneTimer::After()
// ---------------------------------------------------------
//

EXPORT_C void CPhoneTimer::After( 
    TTimeIntervalMicroSeconds32 anInterval, 
    MPhoneTimer* aObserver )
    {
    __LOGMETHODSTARTEND(EPhoneUIUtils, "CPhoneTimer::After() ");
    
    __ASSERT_DEBUG( aObserver, Panic( EPhoneUtilsParameterNotInitialized ) );
    
    if ( IsActive() )
        {
        Cancel();
        }
    iTimerObserver = aObserver;
    CTimer::After( anInterval );
    }

// ---------------------------------------------------------
//  CPhoneTimer::CancelTimer
// ---------------------------------------------------------
//
EXPORT_C void CPhoneTimer::CancelTimer()
    {
    __LOGMETHODSTARTEND(EPhoneUIUtils, "CPhoneTimer::CancelTimer() ");
    Cancel();
    }

// ---------------------------------------------------------
//  CPhoneTimer::DoCancel
// ---------------------------------------------------------
//
void CPhoneTimer::DoCancel()
    {
    __LOGMETHODSTARTEND(EPhoneUIUtils, "CPhoneTimer::DoCancel() ");
    iTimerObserver = NULL;
    CTimer::DoCancel();
    }

//  End of File  
