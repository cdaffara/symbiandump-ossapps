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
* Description:  Contains phone engine base central repository monitor class 
*
*/


// INCLUDE FILES
#include "cpecalldurationdisplaysettingmonitor.h"
#include <LogsDomainCRKeys.h>
#include <talogger.h>

// -----------------------------------------------------------------------------
// CPECallDurationDisplaySettingMonitor::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPECallDurationDisplaySettingMonitor* CPECallDurationDisplaySettingMonitor::NewL(
        // None
        )
    {
    TEFLOGSTRING(KTAOBJECT, "CPECallDurationDisplaySettingMonitor::NewL");
    CPECallDurationDisplaySettingMonitor* me = new ( ELeave ) CPECallDurationDisplaySettingMonitor();
    CleanupStack::PushL( me );
    me->ConstructL();
    CleanupStack::Pop( me );
    return ( me );
    }

// Destructor
CPECallDurationDisplaySettingMonitor::~CPECallDurationDisplaySettingMonitor()
    {
    // Base class object cancels outstanding request and deletes the repository
    }

// -----------------------------------------------------------------------------
// CPECallDurationDisplaySettingMonitor::CPECallDurationDisplaySettingMonitor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPECallDurationDisplaySettingMonitor::CPECallDurationDisplaySettingMonitor()
    : CPECenRepMonitor( KLogsShowCallDuration )
    {
    }

// -----------------------------------------------------------------------------
// CPECallDurationDisplaySettingMonitor::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPECallDurationDisplaySettingMonitor::ConstructL()
    {
    TEFLOGSTRING( KTAOBJECT, "CPECallDurationDisplaySettingMonitor::ConstructL" );
    BaseConstructL( KCRUidLogs );
    }

// -----------------------------------------------------------------------------
// CPECallDurationDisplaySettingMonitor::UpdateL
// Hardcoded values according to LogsDomainCRKeys.h
// 0 (call duration not shown)
// 1 (call duration shown)
// -----------------------------------------------------------------------------
//
void CPECallDurationDisplaySettingMonitor::UpdateL()
    {
    // Retrieve current value for call duration display
    iError = Get( iDisplayCallDuration );
    User::LeaveIfError( iError );
    TEFLOGSTRING2( KTAINT, "CPECallDurationDisplaySettingMonitor::UpdateL %d", iDisplayCallDuration );
    }

// -----------------------------------------------------------------------------
// CPECallDurationDisplaySettingMonitor::GetValue
// -----------------------------------------------------------------------------
//
TInt CPECallDurationDisplaySettingMonitor::GetValue(TInt& aValue)
    {
    aValue = (TInt)iDisplayCallDuration;
    return iError;
    }
    
// End of file
