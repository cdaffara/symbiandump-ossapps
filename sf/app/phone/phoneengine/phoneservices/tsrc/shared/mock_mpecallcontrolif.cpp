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
#include "mock_mpecallcontrolif.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPECallControlIFMock::HandleDialServiceCall
// -----------------------------------------------------------------------------
//
TInt CPECallControlIFMock::HandleDialServiceCall( 
        const TBool aClientCall )
    {
    SMC_MOCK_METHOD1( TInt, const TBool, aClientCall )
    }


// -----------------------------------------------------------------------------
// CPECallControlIFMock::HandlePlayDTMFL
// -----------------------------------------------------------------------------
//
void CPECallControlIFMock::HandlePlayDTMFL(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// CPECallControlIFMock::HandleEndDTMF
// -----------------------------------------------------------------------------
//
TInt CPECallControlIFMock::HandleEndDTMF(  )
    {
    SMC_MOCK_METHOD0( TInt )
    }


