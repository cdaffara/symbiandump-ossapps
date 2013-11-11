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
#include "infowidgetsathandler.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// InfoWidgetSatHandler::InfoWidgetSatHandler
// -----------------------------------------------------------------------------
//
InfoWidgetSatHandler::InfoWidgetSatHandler( 
        QObject * parent )
    {
    Q_UNUSED(parent);
    }


// -----------------------------------------------------------------------------
// InfoWidgetSatHandler::~InfoWidgetSatHandler
// -----------------------------------------------------------------------------
//
InfoWidgetSatHandler::~InfoWidgetSatHandler(  )
    {
    
    }


// -----------------------------------------------------------------------------
// InfoWidgetSatHandler::connect
// -----------------------------------------------------------------------------
//
void InfoWidgetSatHandler::connect( 
        bool connect )
    {
    SMC_MOCK_METHOD1( void, bool, connect )
    }


// -----------------------------------------------------------------------------
// InfoWidgetSatHandler::satDisplayText
// -----------------------------------------------------------------------------
//
const QString & InfoWidgetSatHandler::satDisplayText(  ) const
    {
    SMC_MOCK_METHOD0( const QString & )
    }


// -----------------------------------------------------------------------------
// InfoWidgetSatHandler::setSatDisplayText
// -----------------------------------------------------------------------------
//
void InfoWidgetSatHandler::setSatDisplayText( 
        const QString & displayText )
    {
    SMC_MOCK_METHOD1( void, const QString &, displayText )
    }


// -----------------------------------------------------------------------------
// InfoWidgetSatHandler::handleIdleModeTxtMessage
// -----------------------------------------------------------------------------
//
void InfoWidgetSatHandler::handleIdleModeTxtMessage( 
        int idleResult )
    {
    SMC_MOCK_METHOD1( void, int, idleResult )
    }


// -----------------------------------------------------------------------------
// InfoWidgetSatHandler::handleSatError
// -----------------------------------------------------------------------------
//
void InfoWidgetSatHandler::handleSatError( 
        int operation,
        int errorCode )
    {
    SMC_MOCK_METHOD2( void, int, operation, 
        int, errorCode )
    }


