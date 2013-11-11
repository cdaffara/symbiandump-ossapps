/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of TPhoneCmdParamProgressNote class.
*
*/


// INCLUDE FILES
#include "tphonecmdparamprogressnote.h" 
#include "phoneui.pan"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// TPhoneCmdParamProgressNote::TPhoneCmdParamProgressNote
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TPhoneCmdParamProgressNote::TPhoneCmdParamProgressNote():
    TPhoneCommandParam(),
    iResourceId( 0 ),
    iWaitTime( 0 )
    {
    iParamId = EPhoneParamIdProgressNote;
    }

// ---------------------------------------------------------
// TPhoneCmdParamProgressNote::SetResourceId
// Sets the note resource ID
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamProgressNote::SetResourceId( TInt aResourceId )
    {
    __ASSERT_DEBUG( aResourceId, Panic( EPhoneUtilsParameterNotInitialized ) );
    iResourceId = aResourceId;   
    }

// ---------------------------------------------------------
// TPhoneCmdParamProgressNote::SetWaitTime
// Sets the note timeout
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamProgressNote::SetWaitTime( 
   TTimeIntervalMicroSeconds32 aWaitTime )
    {
    iWaitTime = aWaitTime;   
    }


// ---------------------------------------------------------
// TPhoneCmdParamProgressNote::ResourceId
// Returns the note resource ID
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt TPhoneCmdParamProgressNote::ResourceId() const
    {
    __ASSERT_DEBUG( iResourceId, Panic( EPhoneUtilsInvariant ) );
    return iResourceId;
    }

  // ---------------------------------------------------------
// TPhoneCmdParamProgressNote::WaitTime
// Returns the note timeout
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TTimeIntervalMicroSeconds32 TPhoneCmdParamProgressNote::WaitTime() const
    {
    return iWaitTime;
    } 

//  End of File  
