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
#include "rsatservice.h"

class TSatIconInfo{
    
};

class RSatSession{

};

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RSatService::RSatService
// -----------------------------------------------------------------------------
//
RSatService::RSatService(  )
    //:
    //RSubSessionBase( /**/ )
    {
    
    }


// -----------------------------------------------------------------------------
// RSatService::OpenL
// -----------------------------------------------------------------------------
//
void RSatService::OpenL( 
        const RSatSession & aSession )
    {
    SMC_MOCK_METHOD1( void, const RSatSession &, aSession )
    }


// -----------------------------------------------------------------------------
// RSatService::Close
// -----------------------------------------------------------------------------
//
void RSatService::Close(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// RSatService::NotifySetupIdleModeTextChange
// -----------------------------------------------------------------------------
//
TInt RSatService::NotifySetupIdleModeTextChange( 
        TRequestStatus & aStatus )
    {
    SMC_MOCK_METHOD1( TInt, TRequestStatus &, aStatus )
    }


// -----------------------------------------------------------------------------
// RSatService::NotifySetupIdleModeTextChangeCancel
// -----------------------------------------------------------------------------
//
void RSatService::NotifySetupIdleModeTextChangeCancel(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// RSatService::GetSetupIdleModeTextL
// -----------------------------------------------------------------------------
//
TBool RSatService::GetSetupIdleModeTextL( 
        HBufC * & aText,
        TSatIconQualifier & aIconQualifier,
        TUint8 & aRecordNumber )
    {
    aText = HBufC::NewL(15);
    SMC_MOCK_METHOD3( TBool, 
            HBufC* , aText,
            TSatIconQualifier &, aIconQualifier,
            TUint8 &, aRecordNumber )
    }


// -----------------------------------------------------------------------------
// RSatService::GetIconInfoL
// -----------------------------------------------------------------------------
//
void RSatService::GetIconInfoL( 
        TUint8 aRecordNumber,
        RIconEf & aIconEf ) const
    {
    SMC_MOCK_METHOD2( void, TUint8, aRecordNumber, 
        RIconEf &, aIconEf )
    }


// -----------------------------------------------------------------------------
// RSatService::GetIconL
// -----------------------------------------------------------------------------
//
CFbsBitmap * RSatService::GetIconL( 
        const TSatIconInfo & aIconInfo ) const
    {
    SMC_MOCK_METHOD1( CFbsBitmap *, const TSatIconInfo &, aIconInfo )
    }


// -----------------------------------------------------------------------------
// RSatService::SetIdleModeTextResponse
// -----------------------------------------------------------------------------
//
void RSatService::SetIdleModeTextResponse( 
        const TSATIdleResult & aResult ) const
    {
    SMC_MOCK_METHOD1( void, const TSATIdleResult &, aResult )
    }



