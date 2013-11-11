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
* Description: Implementation of TPhoneCmdParamEmergencyCallHeaderData class.
*
*/


// INCLUDE FILES
#include "tphonecmdparamemergencycallheaderdata.h"   

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// TPhoneCmdParamEmergencyCallHeaderData::TPhoneCmdParamEmergencyCallHeaderData
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TPhoneCmdParamEmergencyCallHeaderData::
TPhoneCmdParamEmergencyCallHeaderData() :
    TPhoneCommandParam(),
    iLabelText( KNullDesC ),
    iHeaderText( KNullDesC ),
    iCiphering( EFalse ),
    iOwnPhoneNumberText( KNullDesC ),
    iCipheringIndicatorAllowed( ETrue )
    {
    iParamId = EPhoneParamIdEmergencyCallHeaderData;
    }

// ---------------------------------------------------------
// TPhoneCmdParamEmergencyCallHeaderData::SetLabelText
// Sets the call header label text
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamEmergencyCallHeaderData::SetLabelText( 
   const TDesC& aLabelText )
   {
   iLabelText.Set( aLabelText );
   }

// ---------------------------------------------------------
// TPhoneCmdParamEmergencyCallHeaderData::SetHeaderText
// Sets the call header text
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamEmergencyCallHeaderData::SetHeaderText( 
   const TDesC& aHeaderText )
   {
   iHeaderText.Set( aHeaderText );
   }

// ---------------------------------------------------------
// TPhoneCmdParamEmergencyCallHeaderData::SetCiphering
// Sets the call header ciphering status
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamEmergencyCallHeaderData::SetCiphering( 
   TBool aCiphering )
   {
   iCiphering = aCiphering;
   }

// ---------------------------------------------------------
// TPhoneCmdParamEmergencyCallHeaderData::SetOwnPhoneNumberText
// Sets the call header own phone number text
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamEmergencyCallHeaderData::SetOwnPhoneNumberText( 
   const TDesC& aOwnPhoneNumberText )
   {
   iOwnPhoneNumberText.Set( aOwnPhoneNumberText );
   }

// ---------------------------------------------------------
// TPhoneCmdParamEmergencyCallHeaderData::LabelText
// Returns the call header label text
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C const TDesC& TPhoneCmdParamEmergencyCallHeaderData::LabelText() const
   {
   return iLabelText;
   }

// ---------------------------------------------------------
// TPhoneCmdParamEmergencyCallHeaderData::HeaderText
// Returns the call header text
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C const TDesC& TPhoneCmdParamEmergencyCallHeaderData::HeaderText() const
   {
   return iHeaderText;
   }

// ---------------------------------------------------------
// TPhoneCmdParamEmergencyCallHeaderData::Ciphering
// Returns the call header ciphering
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TBool TPhoneCmdParamEmergencyCallHeaderData::Ciphering() const
   {
   return iCiphering;
   }

// ---------------------------------------------------------
// TPhoneCmdParamEmergencyCallHeaderData::OwnPhoneNumberText
// Returns the call header own phone number text
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C const TDesC& 
TPhoneCmdParamEmergencyCallHeaderData::OwnPhoneNumberText() const
   {
   return iOwnPhoneNumberText;
   }

// ---------------------------------------------------------
// TPhoneCmdParamEmergencyCallHeaderData::SetCipheringIndicatorAllowed
// Sets the ciphering indicator allowed status
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamEmergencyCallHeaderData::SetCipheringIndicatorAllowed( 
    TBool aAllowed )
    {
    iCipheringIndicatorAllowed = aAllowed;
    }

// ---------------------------------------------------------
// TPhoneCmdParamEmergencyCallHeaderData::CipheringIndicatorAllowed
// Returns the ciphering indicator allowed status
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TBool TPhoneCmdParamEmergencyCallHeaderData::CipheringIndicatorAllowed() const    
    {
    return iCipheringIndicatorAllowed;
    }

//  End of File
