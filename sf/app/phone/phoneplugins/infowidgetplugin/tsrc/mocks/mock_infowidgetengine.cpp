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
#include "infowidgetnetworkhandler.h"
#include "infowidgetsathandler.h"
#include "infowidgetengine.h"
#include "infowidgetpreferences.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// InfoWidgetEngine::InfoWidgetEngine
// -----------------------------------------------------------------------------
//
InfoWidgetEngine::InfoWidgetEngine( 
        QObject * parent )
    :QObject( parent )
    {
    
    }


// -----------------------------------------------------------------------------
// InfoWidgetEngine::~InfoWidgetEngine
// -----------------------------------------------------------------------------
//
InfoWidgetEngine::~InfoWidgetEngine(  )
    {
    
    }


// -----------------------------------------------------------------------------
// InfoWidgetEngine::modelData
// -----------------------------------------------------------------------------
//
const InfoWidgetEngine::ModelData & InfoWidgetEngine::modelData(  ) const
    {
    SMC_MOCK_METHOD0( const InfoWidgetEngine::ModelData & )
    }


// -----------------------------------------------------------------------------
// InfoWidgetEngine::updateNetworkDataToModel
// -----------------------------------------------------------------------------
//
void InfoWidgetEngine::updateNetworkDataToModel(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// InfoWidgetEngine::updateSatDataToModel
// -----------------------------------------------------------------------------
//
void InfoWidgetEngine::updateSatDataToModel(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// InfoWidgetEngine::updateLineDataToModel
// -----------------------------------------------------------------------------
//
void InfoWidgetEngine::updateLineDataToModel(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// InfoWidgetEngine::handleNetworkError
// -----------------------------------------------------------------------------
//
void InfoWidgetEngine::handleNetworkError( 
        int operation,
        int errorCode )
    {
    SMC_MOCK_METHOD2( void, int, operation, 
        int, errorCode )
    }


// -----------------------------------------------------------------------------
// InfoWidgetEngine::handleSatError
// -----------------------------------------------------------------------------
//
void InfoWidgetEngine::handleSatError( 
        int operation,
        int errorCode )
    {
    SMC_MOCK_METHOD2( void, int, operation, 
        int, errorCode )
    }


// -----------------------------------------------------------------------------
// InfoWidgetEngine::handleLineError
// -----------------------------------------------------------------------------
//
void InfoWidgetEngine::handleLineError( 
        int operation,
        int errorCode )
    {
    SMC_MOCK_METHOD2( void, int, operation, 
        int, errorCode )
    }


// -----------------------------------------------------------------------------
// InfoWidgetEngine::suspend
// -----------------------------------------------------------------------------
//
void InfoWidgetEngine::suspend(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// InfoWidgetEngine::resume
// -----------------------------------------------------------------------------
//
void InfoWidgetEngine::resume(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// InfoWidgetEngine::preferencesChanged
// -----------------------------------------------------------------------------
//
void InfoWidgetEngine::handlePreferencesChanged( 
        InfoWidgetPreferences::Options options)
    { 
    //SMC_MOCK_METHOD1( void, InfoWidgetPreferences::Options, options )
    }


