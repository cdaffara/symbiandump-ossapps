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
#include <psetwrapper.h>
#include <psetcliwrapper.h>
#include <psetcallwaitingwrapper.h> 
#include <psetcalldivertingwrapper.h>
#include <psetnetworkwrapper.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PSetWrapper::PSetWrapper
// -----------------------------------------------------------------------------
//
PSetWrapper::PSetWrapper( 
        QObject * parent ) : QObject( parent ) 
    {

    }


// -----------------------------------------------------------------------------
// PSetWrapper::~PSetWrapper
// -----------------------------------------------------------------------------
//
PSetWrapper::~PSetWrapper(  )
    {

    }


// -----------------------------------------------------------------------------
// PSetWrapper::cliWrapper
// -----------------------------------------------------------------------------
//
PSetCliWrapper & PSetWrapper::cliWrapper(  )
    {
    SMC_MOCK_METHOD0( PSetCliWrapper & )
    }


// -----------------------------------------------------------------------------
// PSetWrapper::callWaitingWrapper
// -----------------------------------------------------------------------------
//
PSetCallWaitingWrapper & PSetWrapper::callWaitingWrapper(  )
    {
    SMC_MOCK_METHOD0( PSetCallWaitingWrapper & )
    }


// -----------------------------------------------------------------------------
// PSetWrapper::callDivertingWrapper
// -----------------------------------------------------------------------------
//
PSetCallDivertingWrapper & PSetWrapper::callDivertingWrapper(  )
    {
    SMC_MOCK_METHOD0( PSetCallDivertingWrapper & )
    }


// -----------------------------------------------------------------------------
// PSetWrapper::networkWrapper
// -----------------------------------------------------------------------------
//
PSetNetworkWrapper & PSetWrapper::networkWrapper(  )
    {
    SMC_MOCK_METHOD0( PSetNetworkWrapper & )
    }


// -----------------------------------------------------------------------------
// PSetWrapper::callBarringWrapper
// -----------------------------------------------------------------------------
//
PSetCallBarringWrapper & PSetWrapper::callBarringWrapper(  )
    {
    SMC_MOCK_METHOD0( PSetCallBarringWrapper & )
    }
