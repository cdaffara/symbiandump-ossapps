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

#include <psetcontainer.h>
#include <psetcallbarring.h>
#include <psetcallbarringwrapper.h>
#include <QDebug>
#include <smcmockclassincludes.h>
#include "psetcallbarringwrapper.h"

class PSetCallBarringWrapperPrivate {
public:
    PSetCallBarringWrapperPrivate() {}
    ~PSetCallBarringWrapperPrivate() {}
};


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PSetCallBarringWrapper::PSetCallBarringWrapper
// -----------------------------------------------------------------------------
//
PSetCallBarringWrapper::PSetCallBarringWrapper( 
        CPsetContainer & psetContainer,
        QObject * parent )
    :
    QObject(parent)
    {
    Q_UNUSED(psetContainer)
    }


// -----------------------------------------------------------------------------
// PSetCallBarringWrapper::~PSetCallBarringWrapper
// -----------------------------------------------------------------------------
//
PSetCallBarringWrapper::~PSetCallBarringWrapper(  )
    {
    
    }


// -----------------------------------------------------------------------------
// PSetCallBarringWrapper::barringStatus
// -----------------------------------------------------------------------------
//
void PSetCallBarringWrapper::barringStatus( 
        PsServiceGroup serviceGroup,
        BarringType barringType )
    {
    SMC_MOCK_METHOD2( void, int/*PsServiceGroup*/, static_cast<int>(serviceGroup), 
        BarringType, barringType )
    }


// -----------------------------------------------------------------------------
// PSetCallBarringWrapper::enableBarring
// -----------------------------------------------------------------------------
//
void PSetCallBarringWrapper::enableBarring(
    int basicServiceGroup,
    PsServiceGroup serviceGroup,
    BarringType barringType,
    QString barringPassword)
    {
    SMC_MOCK_METHOD4( void, 
        int, basicServiceGroup, 
        int/*PsServiceGroup*/, static_cast<int>(serviceGroup), 
        BarringType, barringType, QString, barringPassword )
    }


// -----------------------------------------------------------------------------
// PSetCallBarringWrapper::disableBarring
// -----------------------------------------------------------------------------
//
void PSetCallBarringWrapper::disableBarring(
    int basicServiceGroup,
    PsServiceGroup serviceGroup,
    BarringType barringType,
    QString barringPassword)
    {
    SMC_MOCK_METHOD4( void, 
        int, basicServiceGroup, 
        int/*PsServiceGroup*/, static_cast<int>(serviceGroup), 
        BarringType, barringType, QString, barringPassword )
    }

// -----------------------------------------------------------------------------
// PSetCallBarringWrapper::changeBarringPassword
// -----------------------------------------------------------------------------
//
void PSetCallBarringWrapper::changeBarringPassword(
    const QString &oldPassword, 
    const QString &newPassword,
    const QString &verifiedPassword)
{
    SMC_MOCK_METHOD3( void, 
        const QString &, oldPassword, 
        const QString &, newPassword, 
        const QString &, verifiedPassword )
}
