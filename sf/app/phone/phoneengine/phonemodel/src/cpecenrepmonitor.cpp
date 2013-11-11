/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Contains phone engine central repository monitor base class
*
*/


// INCLUDE FILES
#include "cpecenrepmonitor.h"
#include <centralrepository.h>
#include <pepanic.pan>
#include <talogger.h>

// -----------------------------------------------------------------------------
// CPECenRepMonitor::CPECenRepMonitor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPECenRepMonitor::CPECenRepMonitor(
        TUint32 aMonitorSetting
        ) : CActive( EPriorityStandard ),
            iMonitorSetting( aMonitorSetting )
    {
    CActiveScheduler::Add( this );
    }

// Destructor
CPECenRepMonitor::~CPECenRepMonitor(
        // None
        )
    {
    Cancel();
    delete iRepository;
    }

// -----------------------------------------------------------------------------
// CPECenRepMonitor::BaseConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPECenRepMonitor::BaseConstructL(
        TUid aRepositoryUid
        )
    {
    // Create repository instance
    iRepository = CRepository::NewL( aRepositoryUid );

    // Store current value
    UpdateL();

    // Start monitoring
    SubmitNotifyRequestL();
    }

// -----------------------------------------------------------------------------
// CPECenRepMonitor::Get
// -----------------------------------------------------------------------------
//
TInt CPECenRepMonitor::Get(
        TInt& aValue
        )
    {
    return ( iRepository->Get( iMonitorSetting, aValue ) );
    }

// -----------------------------------------------------------------------------
// CPECenRepMonitor::SubmitNotifyRequestL
// -----------------------------------------------------------------------------
//
void CPECenRepMonitor::SubmitNotifyRequestL(
        // None
        )
    {
    __ASSERT_DEBUG( iRepository, Panic( EPEPanicNoRepository ) );
    __ASSERT_DEBUG( !IsActive(), Panic( EPEPanicRepositoryAlreadyActive ) );
    iRepository->NotifyRequest( iMonitorSetting, iStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CPECenRepMonitor::DoCancel
// -----------------------------------------------------------------------------
//
void CPECenRepMonitor::DoCancel(
        // None
        )
    {
    __ASSERT_DEBUG( iRepository, Panic( EPEPanicNoRepository ) );
    iRepository->NotifyCancel( iMonitorSetting );
    }

// -----------------------------------------------------------------------------
// CPECenRepMonitor::RunError
// -----------------------------------------------------------------------------
//
TInt CPECenRepMonitor::RunError(
        #ifdef _DEBUG
        TInt aError // Log the leavecode from RunL
        #else
        TInt /*aError*/
        #endif
        )
    {
    #ifdef _DEBUG
    TEFLOGSTRING3( KTAERROR, "CPECenRepMonitor::RunError %d MonitorSetting: %d", aError, iMonitorSetting );
    #endif
    return ( KErrNone );
    }

// -----------------------------------------------------------------------------
// CPECenRepMonitor::RunL
// -----------------------------------------------------------------------------
//
void CPECenRepMonitor::RunL(
        // None
        )
    {
    TEFLOGSTRING3( KTAREQEND, "CPECenRepMonitor::RunL, status: %d, monitored setting: %d", iStatus.Int(), iMonitorSetting );

    // Don't resubmit the request on error
    // Central repositry completes notifications with id of setting
    // so check only that value of iStatus is negative
    User::LeaveIfError( iStatus.Int() < KErrNone ); 
    SubmitNotifyRequestL();
    UpdateL();
    }
    
// End of file
