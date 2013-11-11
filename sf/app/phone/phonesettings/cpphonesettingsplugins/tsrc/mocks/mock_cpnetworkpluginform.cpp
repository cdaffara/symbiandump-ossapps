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
#include <cpsettingformitemdata.h>
#include <psetwrapper.h>
#include "cpnetworkpluginform.h"

class CellularDataSettings
{
public:
    CellularDataSettings();
};
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CpNetworkPluginForm::CpNetworkPluginForm
// -----------------------------------------------------------------------------
//
CpNetworkPluginForm::CpNetworkPluginForm( 
        QGraphicsItem * /*parent*/ )
    //:
    //HbDataForm( /*parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// CpNetworkPluginForm::~CpNetworkPluginForm
// -----------------------------------------------------------------------------
//
CpNetworkPluginForm::~CpNetworkPluginForm(  )
    {
    
    }


// -----------------------------------------------------------------------------
// CpNetworkPluginForm::searchAvailableNetworks
// -----------------------------------------------------------------------------
//
void CpNetworkPluginForm::searchAvailableNetworks(  )
    {
    SMC_MOCK_METHOD0( void )
    }

// -----------------------------------------------------------------------------
// CpNetworkPluginForm::cellularDataUseHomeStateChanged
// -----------------------------------------------------------------------------
//
void CpNetworkPluginForm::cellularDataUseHomeStateChanged( 
        int index )
    {
    SMC_MOCK_METHOD1( void, int, index )
    }


// -----------------------------------------------------------------------------
// CpNetworkPluginForm::cellularDataUseRoamStateChanged
// -----------------------------------------------------------------------------
//
void CpNetworkPluginForm::cellularDataUseRoamStateChanged( 
        int index )
    {
    SMC_MOCK_METHOD1( void, int, index )
    }


// -----------------------------------------------------------------------------
// CpNetworkPluginForm::networkModeStateChanged
// -----------------------------------------------------------------------------
//
void CpNetworkPluginForm::networkModeStateChanged( 
        int index )
    {
    SMC_MOCK_METHOD1( void, int, index )
    }


// -----------------------------------------------------------------------------
// CpNetworkPluginForm::operatorSelectionStateChanged
// -----------------------------------------------------------------------------
//
void CpNetworkPluginForm::operatorSelectionStateChanged(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// CpNetworkPluginForm::networkAccessModeGot
// -----------------------------------------------------------------------------
//
void CpNetworkPluginForm::networkAccessModeGot( 
        int mode )
    {
    SMC_MOCK_METHOD1( void, int, mode )
    }


// -----------------------------------------------------------------------------
// CpNetworkPluginForm::availableNetworksGot
// -----------------------------------------------------------------------------
//
void CpNetworkPluginForm::availableNetworksGot( 
        QList<PSetNetworkWrapper::NetworkInfo *> &  )
    {
//    SMC_MOCK_METHOD1( void, QList<PSetNetworkWrapper::NetworkInfo *> &, m_networkInfoList )
    }


// -----------------------------------------------------------------------------
// CpNetworkPluginForm::networkReqestFailed
// -----------------------------------------------------------------------------
//
void CpNetworkPluginForm::networkReqestFailed( 
        PSetNetworkWrapper::ErrorCode ,
        PSetNetworkWrapper::RequestType  )
    {
//    SMC_MOCK_METHOD2( void, PSetNetworkWrapper::ErrorCode, error, 
//        PSetNetworkWrapper::RequestType, type )
    }


// -----------------------------------------------------------------------------
// CpNetworkPluginForm::userCancel
// -----------------------------------------------------------------------------
//
void CpNetworkPluginForm::userCancel(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// CpNetworkPluginForm::handleSearchingNetworks
// -----------------------------------------------------------------------------
//
void CpNetworkPluginForm::handleSearchingNetworks( 
        PSetNetworkWrapper::RequestType &  )
    {
//    SMC_MOCK_METHOD1( void, PSetNetworkWrapper::RequestType &, type )
    }


// -----------------------------------------------------------------------------
// CpNetworkPluginForm::handleRequestingSelectedNetwork
// -----------------------------------------------------------------------------
//
void CpNetworkPluginForm::handleRequestingSelectedNetwork( 
        bool ongoing )
    {
    SMC_MOCK_METHOD1( void, bool, ongoing )
    }


// -----------------------------------------------------------------------------
// CpNetworkPluginForm::handleNetworkChanged
// -----------------------------------------------------------------------------
//
void CpNetworkPluginForm::handleNetworkChanged( 
        PSetNetworkWrapper::NetworkInfo & ,
        PSetNetworkWrapper::RegistrationStatus &  )
    {
//    SMC_MOCK_METHOD2( void, PSetNetworkWrapper::NetworkInfo &, currentInfo, 
//        PSetNetworkWrapper::RegistrationStatus &, status )
    }

// -----------------------------------------------------------------------------
// CpNetworkPluginForm::finishedManualSelectiondialog
// -----------------------------------------------------------------------------
//
void CpNetworkPluginForm::finishedManualSelectiondialog( 
        HbAction* /*action*/  )
    {
   // SMC_MOCK_METHOD1( void, HbAction *, action )
    }


// -----------------------------------------------------------------------------
// CpNetworkPluginForm::chageVisbilityOfManualNetworkSelection
// -----------------------------------------------------------------------------
//
void CpNetworkPluginForm::chageVisbilityOfManualNetworkSelection( 
        bool visible )
    {
    SMC_MOCK_METHOD1( void, bool, visible )
    }
