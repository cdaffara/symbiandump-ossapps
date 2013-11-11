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
#include <smcmockclassincludes.h>
#include "mock_mpecallsettersif.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPECallSettersIFMock::SetKeyCode
// -----------------------------------------------------------------------------
//
void CPECallSettersIFMock::SetKeyCode( 
        const TChar & aKeyCode )
    {
    SMC_MOCK_METHOD1( void, const TUint &, static_cast<TUint>(aKeyCode) )
    }


// -----------------------------------------------------------------------------
// CPECallSettersIFMock::SetPhoneNumber
// -----------------------------------------------------------------------------
//
void CPECallSettersIFMock::SetPhoneNumber( 
        const TPEPhoneNumber & aPhoneNumber )
    {
    SMC_MOCK_METHOD1( void, const TPEPhoneNumber &, aPhoneNumber )
    }


// -----------------------------------------------------------------------------
// CPECallSettersIFMock::SetCallTypeCommand
// -----------------------------------------------------------------------------
//
void CPECallSettersIFMock::SetCallTypeCommand( 
        const TPECallType & aCallType )
    {
    SMC_MOCK_METHOD1( void, const TPECallType &, aCallType )
    }


// -----------------------------------------------------------------------------
// CPECallSettersIFMock::SetContactId2
// -----------------------------------------------------------------------------
//
void CPECallSettersIFMock::SetContactId2( 
        const TInt aContactId )
    {
    SMC_MOCK_METHOD1( void, const TInt, aContactId )
    }


// -----------------------------------------------------------------------------
// CPECallSettersIFMock::SetServiceIdCommand
// -----------------------------------------------------------------------------
//
void CPECallSettersIFMock::SetServiceIdCommand( 
        TUint32 aServiceId )
    {
    SMC_MOCK_METHOD1( void, TUint32, aServiceId )
    }


