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
* Description: Implementation of TPhoneCmdParamString class.
*
*/


// INCLUDE FILES
#include "tphonecmdparamstring.h"   
#include "phoneui.pan"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// TPhoneCmdParamString::TPhoneCmdParamString
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TPhoneCmdParamString::TPhoneCmdParamString() :
    TPhoneCommandParam(),
    iString( NULL )
    {
    iParamId = EPhoneParamIdString;
    }

// ---------------------------------------------------------
// TPhoneCmdParamString::SetString
// Sets the string
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamString::SetString( TPtr* aString )
    {
    __ASSERT_DEBUG( aString, Panic( EPhoneUtilsParameterNotInitialized ) );
    iString = aString;   
    }

// ---------------------------------------------------------
// TPhoneCmdParamString::String
// Returns the string
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TPtr* TPhoneCmdParamString::String()
    {
    __ASSERT_DEBUG( iString, Panic( EPhoneUtilsInvariant ) );
    return iString;
    }

//  End of File
