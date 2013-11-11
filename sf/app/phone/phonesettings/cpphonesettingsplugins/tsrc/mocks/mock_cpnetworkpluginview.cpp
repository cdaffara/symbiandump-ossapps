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
#include "cpnetworkpluginview.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CpNetworkPluginView::CpNetworkPluginView
// -----------------------------------------------------------------------------
//
CpNetworkPluginView::CpNetworkPluginView( 
        QGraphicsItem * /*parent*/ )
    :
    /*CpBaseSettingView(0, parent),*/
    m_networkSettingsForm(0)
    {
    
    }


// -----------------------------------------------------------------------------
// CpNetworkPluginView::~CpNetworkPluginView
// -----------------------------------------------------------------------------
//
CpNetworkPluginView::~CpNetworkPluginView(  )
    {
    
    }


// -----------------------------------------------------------------------------
// CpNetworkPluginView::SearchAvailableNetworks
// -----------------------------------------------------------------------------
//
void CpNetworkPluginView::searchAvailableNetworks(  )
    {
    SMC_MOCK_METHOD0( void )
    }


