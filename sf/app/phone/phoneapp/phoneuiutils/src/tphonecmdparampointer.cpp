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
* Description: Implementation of TPhoneCmdParamPointer class.
*
*/

// INCLUDE FILES
#include "tphonecmdparampointer.h"    

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// TPhoneCmdParamPointer::TPhoneCmdParamPointer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TPhoneCmdParamPointer::TPhoneCmdParamPointer():
    TPhoneCommandParam(),
    iPointer( NULL )
    {
    iParamId = EPhoneParamIdPointer;
    }

// ---------------------------------------------------------
// TPhoneCmdParamPointer::SetPointer
// Sets the address pointer
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamPointer::SetPointer( TAny* aPointer )
   {
   iPointer = aPointer;   
   }

// ---------------------------------------------------------
// TPhoneCmdParamPointer::Pointer
// Returns the address pointer
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TAny* TPhoneCmdParamPointer::Pointer() const
   {
   return iPointer;
   }

//  End of File  
