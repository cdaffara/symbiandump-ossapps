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
* Description:  PointSec monitor implementation.
*
*/


// INCLUDE FILES
#include "ImsPointsecMonitor.h"
#include "ImsPointsecObserver.h"

// CONSTANTS
// Variables that can be read from the System Agent by clients

// KPsUidPointsec
const TUid KPsUidPointsec = { 0x200020c6 };

// The Pointsec API version
const TUint KUidPointsecAPIVersion = { 0x102070e4 };

// The Pointsec status as defined by TPointsecStatus
const TUint KUidPointsecStatus = { 0x102070e3 };


// ================= MEMBER FUNCTIONS =======================

EXPORT_C  CImsPointsecMonitor* CImsPointsecMonitor::NewL( MImsPointsecObserver& aObserver )
    {
    CImsPointsecMonitor * self = new ( ELeave ) CImsPointsecMonitor( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );

    return self;
    }

EXPORT_C TBool CImsPointsecMonitor::IsServiceAllowed() const
	{
	__LOG_ENTER_SUPPRESS( "IsServiceAllowed" )
	__LOG_WRITE_FORMAT1_INFO( "pointsec monitor service allowed=%i", iServiceAllowed )
	return iServiceAllowed;
}


CImsPointsecMonitor::CImsPointsecMonitor( MImsPointsecObserver& aObserver )
: CActive( CActive::EPriorityHigh ), iObserver( &aObserver ), iServiceAllowed( EFalse )
    {
    CActiveScheduler::Add( this ); // Add to active scheduler
    __LOG_CONSTRUCT( "common", "CImsPointsecMonitor" )
    }

void CImsPointsecMonitor::ConstructL()
    {
    __LOG_ENTER( "ConstructL" )
    
    TInt version( -1 );
    iProperty.Get( KPsUidPointsec, KUidPointsecAPIVersion, version );
    __LOG_WRITE_FORMAT1_INFO("pointsec version=%x", version);

    User::LeaveIfError( iProperty.Attach( KPsUidPointsec, KUidPointsecStatus ) );
    
    //WMT: It is important for the FS Message Store to know the enable/disabled state right after
    //constructing this object, it can not wait until next active scheduler cycle
    //so we call UpdateStatus() to get the status, but do not notify observer 
    UpdateStatus();

    iProperty.Subscribe( iStatus );
    SetActive();

    __LOG_EXIT
    }


CImsPointsecMonitor::~CImsPointsecMonitor()
    {
    Cancel(); // AO's destructor always calls Cancel()
    iProperty.Close();
    __LOG_DESTRUCT
    }


void CImsPointsecMonitor::HandleEvent( TPointsecStatus aStatus )
    {
    __LOG_ENTER("HandleEvent")

    switch ( aStatus )
        {
        case EPointsecRecognizerStarted:
        case EPointsecInitializing:
        case EPointsecLocked:
        case EPointSecEncryptionStarting:
            SetServiceAllowed( EFalse );
            break;

        case EPointsecUnlocked:
            SetServiceAllowed( ETrue );
            break;

        default:
            break;
        }

    __LOG_EXIT
    }


void CImsPointsecMonitor::SetServiceAllowed( TBool aValue )
    {
    iServiceAllowed = aValue;
    }


void CImsPointsecMonitor::NotifyObserverIfNeeded( TBool aOldServiceAllowes )
    {
    __LOG_ENTER( "NotifyObserverIfNeeded" )
    
    if ( iServiceAllowed != aOldServiceAllowes )
        {
        if( iObserver )
            {
            if ( iServiceAllowed )
                {
                iObserver->PointSecLockEnded( );
                }
            else
                {
                iObserver->PointSecLockStarted( );
                }
            }
        }
    
    __LOG_EXIT
    }


void CImsPointsecMonitor::RunL()
    {
    __LOG_ENTER( "RunL" )
    
    TBool oldServiceAllowed = iServiceAllowed;
    
    UpdateStatus();

    NotifyObserverIfNeeded( oldServiceAllowed );

    iProperty.Subscribe( iStatus );
    SetActive();

    __LOG_EXIT
    }

void CImsPointsecMonitor::UpdateStatus()
    {
    __LOG_ENTER( "UpdateStatus" )
    // read the new pointsec status value from the system agent
    TInt status;
    
    TInt error = iProperty.Get( KPsUidPointsec, KUidPointsecStatus, status );
    __LOG_WRITE_FORMAT1_INFO("property get error=%i", error)
    
    if ( !error )
        {
        __LOG_WRITE_FORMAT1_INFO("poitsec status =%i", status)
        HandleEvent( static_cast<TPointsecStatus>( status ) );
        }
    else
        {
        // can't get pointsec status, assume connection is ok
        SetServiceAllowed( ETrue );
        }
    __LOG_EXIT
    }

void CImsPointsecMonitor::DoCancel()
    {
    iProperty.Cancel();
    }


