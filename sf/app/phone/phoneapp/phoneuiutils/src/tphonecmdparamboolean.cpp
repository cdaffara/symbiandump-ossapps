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
* Description: Implementation of TPhoneCmdParamBoolean class.
*
*/


// INCLUDE FILES

#include "tphonecmdparamboolean.h"  

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// TPhoneCmdParamBoolean::TPhoneCmdParamBoolean
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TPhoneCmdParamBoolean::TPhoneCmdParamBoolean() :
    TPhoneCommandParam(),
    iBoolean( EFalse )
    {
    iParamId = EPhoneParamIdBoolean;
    }

// ---------------------------------------------------------
// TPhoneCmdParamBoolean::SetBoolean
// Sets the Boolean value
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamBoolean::SetBoolean( TBool aBoolean )
   {
   iBoolean = aBoolean;   
   }

// ---------------------------------------------------------
// TPhoneCmdParamBoolean::Boolean
// Returns the Boolean value
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TBool TPhoneCmdParamBoolean::Boolean() const
   {
   return iBoolean;
   }

//  End of File  
