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
* Description: Implementation of TPhoneCmdParamInteger class.
*
*/


// INCLUDE FILES
#include "tphonecmdparaminteger.h"    

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// TPhoneCmdParamInteger::TPhoneCmdParamInteger
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TPhoneCmdParamInteger::TPhoneCmdParamInteger():
    TPhoneCommandParam(),
    iInteger( 0 )
    {
    iParamId = EPhoneParamIdInteger;
    }

// ---------------------------------------------------------
// TPhoneCmdParamInteger::SetInteger
// Sets the integer
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamInteger::SetInteger( TInt aInteger )
   {
   iInteger = aInteger;   
   }

// ---------------------------------------------------------
// TPhoneCmdParamInteger::Integer
// Returns the integer 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt TPhoneCmdParamInteger::Integer() const
   {
   return iInteger;
   }

//  End of File  
