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
#include "xqsettingsmanager.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// XQSettingsManager::XQSettingsManager
// -----------------------------------------------------------------------------
//
XQSettingsManager::XQSettingsManager( 
        QObject * parent )
    : QObject( parent )
    {
    
    }

// -----------------------------------------------------------------------------
// XQSettingsManager::~XQSettingsManager
// -----------------------------------------------------------------------------
//
XQSettingsManager::~XQSettingsManager(  )
    {
    
    }


// -----------------------------------------------------------------------------
// XQSettingsManager::readItemValue
// -----------------------------------------------------------------------------
//
QVariant XQSettingsManager::readItemValue( 
        const XQSettingsKey & key,
        XQSettingsManager::Type type )
    {
    SMC_MOCK_METHOD2( QVariant, const XQSettingsKey &, key, XQSettingsManager::Type, type )
    }


// -----------------------------------------------------------------------------
// XQSettingsManager::writeItemValue
// -----------------------------------------------------------------------------
//
bool XQSettingsManager::writeItemValue( 
        const XQSettingsKey & key,
        const QVariant & value )
    {
    SMC_MOCK_METHOD2( bool, const XQSettingsKey &, key, const QVariant &, value )
    }


// -----------------------------------------------------------------------------
// XQSettingsManager::startMonitoring
// -----------------------------------------------------------------------------
//
bool XQSettingsManager::startMonitoring( 
        const XQSettingsKey & key,
        XQSettingsManager::Type type )
    {
    Q_UNUSED( key )
    Q_UNUSED( type )
    bool variable;
    return variable;
    }


// -----------------------------------------------------------------------------
// XQSettingsManager::stopMonitoring
// -----------------------------------------------------------------------------
//
bool XQSettingsManager::stopMonitoring( 
        const XQSettingsKey & key )
    {
    Q_UNUSED( key )
    bool variable;
    return variable;
    }


// -----------------------------------------------------------------------------
// XQSettingsManager::error
// -----------------------------------------------------------------------------
//
XQSettingsManager::Error XQSettingsManager::error(  ) const
    {
    
    XQSettingsManager::Error variable;
    return variable;
    }



