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
#include "phoneapplauncher.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PhoneAppLauncher::PhoneAppLauncher
// -----------------------------------------------------------------------------
//
PhoneAppLauncher::PhoneAppLauncher( 
        QObject * parent )
    //:
    //QObject( /*parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// PhoneAppLauncher::~PhoneAppLauncher
// -----------------------------------------------------------------------------
//
PhoneAppLauncher::~PhoneAppLauncher(  )
    {
    
    }


// -----------------------------------------------------------------------------
// PhoneAppLauncher::launchMessaging
// -----------------------------------------------------------------------------
//
void PhoneAppLauncher::launchMessaging( 
        const QString & toField,
        const QString & name,
        const QString & messageBody )
    {
    SMC_MOCK_METHOD3( void, const QString &, toField, 
        const QString &, name, 
        const QString &, messageBody )
    }


// -----------------------------------------------------------------------------
// PhoneAppLauncher::launchContacts
// -----------------------------------------------------------------------------
//
void PhoneAppLauncher::launchContacts(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// PhoneAppLauncher::launchLogs
// -----------------------------------------------------------------------------
//
void PhoneAppLauncher::launchLogs( 
        int activatedView,
        bool showDialpad,
        const QString & dialpadText )
    {
    SMC_MOCK_METHOD3( void, int, activatedView, 
        bool, showDialpad, 
        const QString &, dialpadText )
    }


