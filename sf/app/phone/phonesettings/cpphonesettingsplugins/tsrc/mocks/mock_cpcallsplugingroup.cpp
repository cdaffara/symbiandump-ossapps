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
#include "cpcallsplugingroup.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CpCallsPluginGroup::CpCallsPluginGroup
// -----------------------------------------------------------------------------
//
CpCallsPluginGroup::CpCallsPluginGroup( 
        CpItemDataHelper &helper )
    :CpSettingFormItemData(HbDataFormModelItem::GroupItem, hbTrId("Calls"),0),
    m_helper(helper)
    {
    
    }


// -----------------------------------------------------------------------------
// CpCallsPluginGroup::~CpCallsPluginGroup
// -----------------------------------------------------------------------------
//
CpCallsPluginGroup::~CpCallsPluginGroup(  )
    {
    
    }


// -----------------------------------------------------------------------------
// CpCallsPluginGroup::showCallDurationStateChanged
// -----------------------------------------------------------------------------
//
void CpCallsPluginGroup::showCallDurationStateChanged( )
    {

    }


// -----------------------------------------------------------------------------
// CpCallsPluginGroup::ownVideoInReceivedCallStateChanged
// -----------------------------------------------------------------------------
//
void CpCallsPluginGroup::ownVideoInReceivedCallStateChanged( 
        int index )
    {
    Q_UNUSED( index )
    }


// -----------------------------------------------------------------------------
// CpCallsPluginGroup::softRejectTextChanged
// -----------------------------------------------------------------------------
//
void CpCallsPluginGroup::softRejectTextChanged(  )
    {
    
    }


// -----------------------------------------------------------------------------
// CpCallsPluginGroup::callWaitingCurrentIndexChanged
// -----------------------------------------------------------------------------
//
void CpCallsPluginGroup::callWaitingCurrentIndexChanged( )
    {

    }


// -----------------------------------------------------------------------------
// CpCallsPluginGroup::cliCurrentIndexChanged
// -----------------------------------------------------------------------------
//
void CpCallsPluginGroup::cliCurrentIndexChanged( int index )
    {
    Q_UNUSED( index )
    }


// -----------------------------------------------------------------------------
// CpCallsPluginGroup::handleCallWaitingGetStatus
// -----------------------------------------------------------------------------
//
void CpCallsPluginGroup::handleCallWaitingGetStatus( 
        const PSetCallWaitingWrapper::PsCallWaitingStatus status,
        const QList<unsigned char> &basicServiceGroupIds )
    {
    Q_UNUSED( status )
    Q_UNUSED( basicServiceGroupIds )
    }


// -----------------------------------------------------------------------------
// CpCallsPluginGroup::handleCallWaitingChanged
// -----------------------------------------------------------------------------
//
void CpCallsPluginGroup::handleCallWaitingChanged( 
        const PSetCallWaitingWrapper::PsCallWaitingCommand setting,
        const int aResult )
    {
    Q_UNUSED( setting )
    Q_UNUSED( aResult )
    }


// -----------------------------------------------------------------------------
// CpCallsPluginGroup::handleCallWaitingRequesting
// -----------------------------------------------------------------------------
//
void CpCallsPluginGroup::handleCallWaitingRequesting( 
        bool ongoing,
        bool interrupted )
    {
    Q_UNUSED( ongoing )
    Q_UNUSED( interrupted )
    }


// -----------------------------------------------------------------------------
// CpCallsPluginGroup::handleCallWaitingError
// -----------------------------------------------------------------------------
//
void CpCallsPluginGroup::handleCallWaitingError( int aError )
    {
    Q_UNUSED( aError )
    }

