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
* Description:
*
*/
#include <QDebug>
#include <smcmockclassincludes.h>
#include <hbicon.h>
#include "cpnetworkstatus.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CpNetworkStatus::CpNetworkStatus
// -----------------------------------------------------------------------------
//
CpNetworkStatus::CpNetworkStatus(  )
    //:
    //QObject( /**/ )
    {
    
    }


// -----------------------------------------------------------------------------
// CpNetworkStatus::~CpNetworkStatus
// -----------------------------------------------------------------------------
//
CpNetworkStatus::~CpNetworkStatus(  )
    {
    
    }


// -----------------------------------------------------------------------------
// CpNetworkStatus::statusText
// -----------------------------------------------------------------------------
//
QString CpNetworkStatus::statusText(  ) const
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// CpNetworkStatus::statusIcon
// -----------------------------------------------------------------------------
//
HbIcon CpNetworkStatus::statusIcon(  ) const
    {
    SMC_MOCK_METHOD0( HbIcon )
    }


// -----------------------------------------------------------------------------
// CpNetworkStatus::statusIconLocicalName
// -----------------------------------------------------------------------------
//
QString CpNetworkStatus::statusIconLocicalName(  ) const
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// CpNetworkStatus::setSettingFormItemData
// -----------------------------------------------------------------------------
//
void CpNetworkStatus::setSettingFormItemData( 
        CpSettingFormItemData * data )
    {
    //SMC_MOCK_METHOD1( void, CpSettingFormItemData *, data )
    }


// -----------------------------------------------------------------------------
// CpNetworkStatus::networkNameChanged
// -----------------------------------------------------------------------------
//
void CpNetworkStatus::networkNameChanged( 
        QSystemNetworkInfo::NetworkMode mode,
        const QString & netName )
    {
    //SMC_MOCK_METHOD2( void, QSystemNetworkInfo::NetworkMode, mode, 
    //    const QString &, netName )
    }


// -----------------------------------------------------------------------------
// CpNetworkStatus::networkStatusChanged
// -----------------------------------------------------------------------------
//
void CpNetworkStatus::networkStatusChanged( 
        QSystemNetworkInfo::NetworkMode mode,
        QSystemNetworkInfo::NetworkStatus status )
    {
    //SMC_MOCK_METHOD2( void, QSystemNetworkInfo::NetworkMode, mode, 
    //    QSystemNetworkInfo::NetworkStatus, status )
    }


