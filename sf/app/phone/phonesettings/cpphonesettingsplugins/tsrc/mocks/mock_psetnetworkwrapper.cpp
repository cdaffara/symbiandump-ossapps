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
#include <psetnetworkwrapper.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PSetNetworkWrapper::PSetNetworkWrapper
// -----------------------------------------------------------------------------
//
PSetNetworkWrapper::PSetNetworkWrapper( 
        CPsetContainer & psetContainer,
        QObject * parent ) : QObject( parent )
    {
    Q_UNUSED(psetContainer);
    }


// -----------------------------------------------------------------------------
// PSetNetworkWrapper::~PSetNetworkWrapper
// -----------------------------------------------------------------------------
//
PSetNetworkWrapper::~PSetNetworkWrapper(  )
    {
    
    }


// -----------------------------------------------------------------------------
// PSetNetworkWrapper::getAvailableNetworks
// -----------------------------------------------------------------------------
//
void PSetNetworkWrapper::getAvailableNetworks(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// PSetNetworkWrapper::getNetworkSelectionMode
// -----------------------------------------------------------------------------
//
PSetNetworkWrapper::ErrorCode PSetNetworkWrapper::getNetworkSelectionMode( 
        PSetNetworkWrapper::NetworkSelectionMode & mode ) const
    {
    SMC_MOCK_METHOD1( PSetNetworkWrapper::ErrorCode, PSetNetworkWrapper::NetworkSelectionMode &, mode )
    }


// -----------------------------------------------------------------------------
// PSetNetworkWrapper::selectNetwork
// -----------------------------------------------------------------------------
//
void PSetNetworkWrapper::selectNetwork( 
        const PSetNetworkWrapper::NetworkInfo & info )
    {
    Q_UNUSED(info);
    }


// -----------------------------------------------------------------------------
// PSetNetworkWrapper::cancelRequest
// -----------------------------------------------------------------------------
//
void PSetNetworkWrapper::cancelRequest(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// PSetNetworkWrapper::getNetworkAccessMode
// -----------------------------------------------------------------------------
//
void PSetNetworkWrapper::getNetworkAccessMode(  ) const
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// PSetNetworkWrapper::setNetworkAccessMode
// -----------------------------------------------------------------------------
//
void PSetNetworkWrapper::setNetworkAccessMode( 
        PSetNetworkWrapper::NetworkAccessMode mode )
    {
    SMC_MOCK_METHOD1( void, PSetNetworkWrapper::NetworkAccessMode, mode )
    }


// -----------------------------------------------------------------------------
// PSetNetworkWrapper::isManualNetworkSelectionSupported
// -----------------------------------------------------------------------------
//
bool PSetNetworkWrapper::isManualNetworkSelectionSupported(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }



