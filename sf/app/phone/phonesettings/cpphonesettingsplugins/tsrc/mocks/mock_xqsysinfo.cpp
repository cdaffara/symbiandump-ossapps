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
#include "xqsysinfo.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// XQSysInfo::XQSysInfo
// -----------------------------------------------------------------------------
//
XQSysInfo::XQSysInfo( 
        QObject * parent )
    //:
    //QObject( /*parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// XQSysInfo::~XQSysInfo
// -----------------------------------------------------------------------------
//
XQSysInfo::~XQSysInfo(  )
    {
    
    }


// -----------------------------------------------------------------------------
// XQSysInfo::currentLanguage
// -----------------------------------------------------------------------------
//
XQSysInfo::Language XQSysInfo::currentLanguage(  ) const
    {
    
    XQSysInfo::Language variable;
    return variable;
    }


// -----------------------------------------------------------------------------
// XQSysInfo::imei
// -----------------------------------------------------------------------------
//
QString XQSysInfo::imei(  ) const
    {
    
    QString variable;
    return variable;
    }


// -----------------------------------------------------------------------------
// XQSysInfo::model
// -----------------------------------------------------------------------------
//
QString XQSysInfo::model(  ) const
    {
    
    QString variable;
    return variable;
    }


// -----------------------------------------------------------------------------
// XQSysInfo::manufacturer
// -----------------------------------------------------------------------------
//
QString XQSysInfo::manufacturer(  ) const
    {
    
    QString variable;
    return variable;
    }


// -----------------------------------------------------------------------------
// XQSysInfo::softwareVersion
// -----------------------------------------------------------------------------
//
QString XQSysInfo::softwareVersion(  ) const
    {
    
    QString variable;
    return variable;
    }


// -----------------------------------------------------------------------------
// XQSysInfo::batteryLevel
// -----------------------------------------------------------------------------
//
uint XQSysInfo::batteryLevel(  ) const
    {
    
    return 0;
    }


// -----------------------------------------------------------------------------
// XQSysInfo::imsi
// -----------------------------------------------------------------------------
//
QString XQSysInfo::imsi(  ) const
    {
    
    QString variable;
    return variable;
    }


// -----------------------------------------------------------------------------
// XQSysInfo::signalStrength
// -----------------------------------------------------------------------------
//
int XQSysInfo::signalStrength(  ) const
    {
    
    return 0;
    }


// -----------------------------------------------------------------------------
// XQSysInfo::diskSpace
// -----------------------------------------------------------------------------
//
qlonglong XQSysInfo::diskSpace( 
        XQSysInfo::Drive drive ) const
    {
    Q_UNUSED( drive )
    qlonglong variable;
    return variable;
    }


// -----------------------------------------------------------------------------
// XQSysInfo::isDiskSpaceCritical
// -----------------------------------------------------------------------------
//
bool XQSysInfo::isDiskSpaceCritical( 
        XQSysInfo::Drive drive ) const
    {
    Q_UNUSED( drive )
    bool variable;
    return variable;
    }


// -----------------------------------------------------------------------------
// XQSysInfo::isNetwork
// -----------------------------------------------------------------------------
//
bool XQSysInfo::isNetwork(  ) const
    {
    
    bool variable;
    return variable;
    }


// -----------------------------------------------------------------------------
// XQSysInfo::memory
// -----------------------------------------------------------------------------
//
int XQSysInfo::memory(  ) const
    {
    
    return 0;
    }


// -----------------------------------------------------------------------------
// XQSysInfo::browserVersion
// -----------------------------------------------------------------------------
//
QString XQSysInfo::browserVersion(  ) const
    {
    
    QString variable;
    return variable;
    }


// -----------------------------------------------------------------------------
// XQSysInfo::isSupported
// -----------------------------------------------------------------------------
//
bool XQSysInfo::isSupported( 
        int featureId )
    {
    SMC_MOCK_METHOD1( bool, int, featureId )
    }


// -----------------------------------------------------------------------------
// XQSysInfo::error
// -----------------------------------------------------------------------------
//
XQSysInfo::Error XQSysInfo::error(  ) const
    {
    
    XQSysInfo::Error variable;
    return variable;
    }

