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
#include "infowidgetsathandler_p.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// InfoWidgetSatHandlerPrivate::InfoWidgetSatHandlerPrivate
// -----------------------------------------------------------------------------
//
InfoWidgetSatHandlerPrivate::InfoWidgetSatHandlerPrivate( 
        InfoWidgetSatHandler * iwSatHandler,
        RSatService & iSatService )
    :CActive( 0 ),
        m_satService(iSatService)
    
    {
    Q_UNUSED(iwSatHandler)
    }


// -----------------------------------------------------------------------------
// InfoWidgetSatHandlerPrivate::~InfoWidgetSatHandlerPrivate
// -----------------------------------------------------------------------------
//
InfoWidgetSatHandlerPrivate::~InfoWidgetSatHandlerPrivate(  )
    {
    
    }


// -----------------------------------------------------------------------------
// InfoWidgetSatHandlerPrivate::startObserving
// -----------------------------------------------------------------------------
//
void InfoWidgetSatHandlerPrivate::startObserving(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// InfoWidgetSatHandlerPrivate::connect
// -----------------------------------------------------------------------------
//
bool InfoWidgetSatHandlerPrivate::connect(  )
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// InfoWidgetSatHandlerPrivate::disconnect
// -----------------------------------------------------------------------------
//
void InfoWidgetSatHandlerPrivate::disconnect(  )
    {
    SMC_MOCK_METHOD0( void )
    }
    
// -----------------------------------------------------------------------------
// InfoWidgetSatHandlerPrivate::DoCancel
// -----------------------------------------------------------------------------
//
void InfoWidgetSatHandlerPrivate::DoCancel( )
	 {
	 SMC_MOCK_METHOD0( void )	
	 }
	
	
// -----------------------------------------------------------------------------
// InfoWidgetSatHandlerPrivate::RunL
// -----------------------------------------------------------------------------
//
void InfoWidgetSatHandlerPrivate::RunL( )
	 {
	 SMC_MOCK_METHOD0( void )		
	 }
	
