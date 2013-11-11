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
#include "RSatSession.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RSatSession::RSatSession
// -----------------------------------------------------------------------------
//
RSatSession::RSatSession(  )
    {
    
    }


// -----------------------------------------------------------------------------
// RSatSession::ConnectL
// -----------------------------------------------------------------------------
//
void RSatSession::ConnectL(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// RSatSession::Version
// -----------------------------------------------------------------------------
//
TVersion RSatSession::Version(  ) const
    {
    SMC_MOCK_METHOD0( TVersion )
    }


