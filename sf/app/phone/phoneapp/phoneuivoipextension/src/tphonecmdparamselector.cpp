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
* Description: Implementation of TPhoneCmdParamSelector class.
*
*/


// INCLUDE FILES
#include "tphonecmdparamselector.h"   


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// TPhoneCmdParamSelector::TPhoneCmdParamSelector
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TPhoneCmdParamSelector::TPhoneCmdParamSelector() :
    TPhoneCommandParam(),
    iServiceId( NULL ),
    iCallType( EPhoneCallTypeNotFound ),
    iEnabled( EFalse )
    {
    iParamId = (TPhoneCommandParam::TPhoneParamId)
                    TPhoneCommandParamVoip::EPhoneParamIdSelector;
    }

// ---------------------------------------------------------
// TPhoneCmdParamSelector::SetPhoneNumber
// Sets the service id.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamSelector::SetServiceId( const TUint aServiceId )
    {
    iServiceId = aServiceId;
    }

// ---------------------------------------------------------
// TPhoneCmdParamSelector::ServiceId
// Returns the service id.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TUint TPhoneCmdParamSelector::ServiceId()
    {
    return iServiceId;
    }

// ---------------------------------------------------------
// TPhoneCmdParamSelector::SetCallType
// Sets the call type.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamSelector::SetCallType( TPhoneSsCallType aCallType )
   {
   iCallType = aCallType;   
   }

// ---------------------------------------------------------
// TPhoneCmdParamSelector::NumberType
// Returns the call type.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TPhoneSsCallType TPhoneCmdParamSelector::CallType()
   {
   return iCallType;
   }

// ---------------------------------------------------------
// TPhoneCmdParamSelector::SetLocation
// Sets the registration status.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamSelector::SetRegStatus( TBool aEnabled )
    {
    iEnabled = aEnabled;   
    }

// ---------------------------------------------------------
// TPhoneCmdParamSelector::RegStatus
// Returns the registration status.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TBool TPhoneCmdParamSelector::RegStatus()
    {
    return iEnabled;
    }

//  End of File
