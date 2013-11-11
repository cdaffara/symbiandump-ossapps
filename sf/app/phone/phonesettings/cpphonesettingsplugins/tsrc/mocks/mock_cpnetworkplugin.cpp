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
#include "cpnetworkplugin.h"
#include <cpsettingformitemdata.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CpNetworkPlugin::Q_INTERFACES
// -----------------------------------------------------------------------------
//
CpNetworkPlugin::CpNetworkPlugin()
    {

    }


// -----------------------------------------------------------------------------
// CpNetworkPlugin::~CpNetworkPlugin
// -----------------------------------------------------------------------------
//
CpNetworkPlugin::~CpNetworkPlugin(  )
    {
    
    }


// -----------------------------------------------------------------------------
// CpNetworkPlugin::createSettingFormItemData
// -----------------------------------------------------------------------------
//
QList <CpSettingFormItemData * > CpNetworkPlugin::createSettingFormItemData( 
        CpItemDataHelper & itemDataHelper ) const
    {
    Q_UNUSED( itemDataHelper )
    return QList<CpSettingFormItemData *> ();
    }


// -----------------------------------------------------------------------------
// CpNetworkPlugin::createSettingView
// -----------------------------------------------------------------------------
//
CpBaseSettingView * CpNetworkPlugin::createSettingView( 
        const QVariant & hint ) const
    {
    SMC_MOCK_METHOD1( CpBaseSettingView *, const QVariant &, hint )
    }


