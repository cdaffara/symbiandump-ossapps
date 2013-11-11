/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Video Source subsystem.
*
*/


// INCLUDE FILES
#include "cactivewait.h"

const TInt KFirstFrameIndx = 0;

// MACROS

#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif

// ============================ CVSActiveWait ===============================

// -----------------------------------------------------------------------------
// CVSActiveWait::~CVSActiveWait()
// -----------------------------------------------------------------------------
//
template <class R>
CVSActiveWait< R >::~CVSActiveWait()
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSActiveWait::~CVSActiveWait() >>"), RThread().Id().operator TUint()));
    Cancel();
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSActiveWait::~CVSActiveWait() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSActiveWait::RequestStatus()
// -----------------------------------------------------------------------------
//
template <class R>
TRequestStatus* CVSActiveWait< R >::RequestStatus()
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSActiveWait::RequestStatus() >>"), RThread().Id().operator TUint()));
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSActiveWait::RequestStatus() <<"), RThread().Id().operator TUint()));
    return &iStatus;
    }

// -----------------------------------------------------------------------------
// CVSActiveWait::Signal( TInt aError )
// -----------------------------------------------------------------------------
//
template <class R>
void CVSActiveWait< R >::Signal( TInt aError )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSActiveWait::Signal() >>"), RThread().Id().operator TUint()));
    if( IsActive() )
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, aError );
        }
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSActiveWait::Signal() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSActiveWait::RunL()
// -----------------------------------------------------------------------------
//
template <class R>
void CVSActiveWait< R >::RunL()
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSActiveWait::RunL() >>"), RThread().Id().operator TUint()));
    (iRequester->*iCallback)( iStatus.Int() );
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSActiveWait::RunL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSActiveWait::DoCancel()
// -----------------------------------------------------------------------------
//
template <class R>
void CVSActiveWait< R >::DoCancel()
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSActiveWait::DoCancel() >>"), RThread().Id().operator TUint()));
    TRequestStatus* pStatus = &iStatus;
    User::RequestComplete( pStatus, KErrCancel );
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSActiveWait::DoCancel() <<"), RThread().Id().operator TUint()));
    }
    
    