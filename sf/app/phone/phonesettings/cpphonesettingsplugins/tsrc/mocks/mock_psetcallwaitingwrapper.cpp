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
#include "psetcallwaitingwrapper.h"

class PSetCallWaitingWrapperPrivate {
public:
    PSetCallWaitingWrapperPrivate() {}
    ~PSetCallWaitingWrapperPrivate() {}
};

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PSetCallWaitingWrapper::PSetCallWaitingWrapper
// -----------------------------------------------------------------------------
//
PSetCallWaitingWrapper::PSetCallWaitingWrapper( 
        CPsetContainer & psetContainer,
        QObject * parent ) : QObject( parent )
    {
    Q_UNUSED(psetContainer);
    }


// -----------------------------------------------------------------------------
// PSetCallWaitingWrapper::~PSetCallWaitingWrapper
// -----------------------------------------------------------------------------
//
PSetCallWaitingWrapper::~PSetCallWaitingWrapper(  )
    {

    }


// -----------------------------------------------------------------------------
// PSetCallWaitingWrapper::setCallWaiting
// -----------------------------------------------------------------------------
//
void PSetCallWaitingWrapper::setCallWaiting( 
        PsCallWaitingCommand aSetting,
        int aBasicServiceGroup )
    {
    SMC_MOCK_METHOD2( void, PsCallWaitingCommand, aSetting, 
        int, aBasicServiceGroup )
    }


// -----------------------------------------------------------------------------
// PSetCallWaitingWrapper::getCallWaitingStatus
// -----------------------------------------------------------------------------
//
void PSetCallWaitingWrapper::getCallWaitingStatus(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// PSetCallWaitingWrapper::cancelProcess
// -----------------------------------------------------------------------------
//
void PSetCallWaitingWrapper::cancelProcess(  )
    {
    SMC_MOCK_METHOD0( void )
    }
