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
#include "psetcliwrapper.h"

class PSetCliWrapperPrivate {
public:
    PSetCliWrapperPrivate() {}
    ~PSetCliWrapperPrivate() {}
};

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PSetCliWrapper::PSetCliWrapper
// -----------------------------------------------------------------------------
//
PSetCliWrapper::PSetCliWrapper( 
        CPsetContainer & psetContainer,
        QObject * parent ) : QObject( parent )
    {
    Q_UNUSED(psetContainer);
    }


// -----------------------------------------------------------------------------
// PSetCliWrapper::~PSetCliWrapper
// -----------------------------------------------------------------------------
//
PSetCliWrapper::~PSetCliWrapper(  )
    {

    }


// -----------------------------------------------------------------------------
// PSetCliWrapper::getColpMode
// -----------------------------------------------------------------------------
//
void PSetCliWrapper::getColpMode(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// PSetCliWrapper::getClipMode
// -----------------------------------------------------------------------------
//
void PSetCliWrapper::getClipMode(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// PSetCliWrapper::getClirMode
// -----------------------------------------------------------------------------
//
void PSetCliWrapper::getClirMode(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// PSetCliWrapper::getColrMode
// -----------------------------------------------------------------------------
//
void PSetCliWrapper::getColrMode(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// PSetCliWrapper::getCnap
// -----------------------------------------------------------------------------
//
void PSetCliWrapper::getCnap(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// PSetCliWrapper::cancelAll
// -----------------------------------------------------------------------------
//
void PSetCliWrapper::cancelAll(  )
    {
    SMC_MOCK_METHOD0( void )
    }


