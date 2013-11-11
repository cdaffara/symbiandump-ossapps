/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of TPhoneCmdParamTransEffect class.
*
*/


// INCLUDE FILES
#include "tphonecmdparamtranseffect.h" 
#include "phoneui.pan"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// TPhoneCmdParamTransEffect::TPhoneCmdParamTransEffect
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TPhoneCmdParamTransEffect::TPhoneCmdParamTransEffect():
    iType( EPhoneTransEffectNone )
    {
    }

// ---------------------------------------------------------
// TPhoneCmdParamTransEffect::SetType
// Sets the note type
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamTransEffect::SetType( TPhoneTransEffectType aType )
   {
   iType = aType;   
   }

 
// ---------------------------------------------------------
// TPhoneCmdParamTransEffect::Type
// Returns the note type
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TPhoneTransEffectType TPhoneCmdParamTransEffect::Type() const
   {
   return iType;
   }
  
//  End of File  
