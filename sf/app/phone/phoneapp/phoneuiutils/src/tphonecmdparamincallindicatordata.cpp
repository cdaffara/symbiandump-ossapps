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
* Description: Implementation of TPhoneCmdParamIncallIndicatorData class.
*
*/


// INCLUDE FILES
#include "tphonecmdparamincallindicatordata.h"   

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// TPhoneCmdParamIncallIndicatorData::TPhoneCmdParamIncallIndicatorData
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C 
TPhoneCmdParamIncallIndicatorData::TPhoneCmdParamIncallIndicatorData() :
    iCallState( 0 ),
    iMode( 0 ),
    iMute( EFalse ),
    iCiphering( EFalse ),
    iEmergency( EFalse ),
    iLine2( EFalse ),
    iLittleBubbleVisible( ETrue ), 
    iCipheringIndicatorAllowed( ETrue )
    {
    iParamId = EPhoneParamIdIncallIndicatorData;
    }

// ---------------------------------------------------------
// TPhoneCmdParamIncallIndicatorData::SetCallState
// Sets the incall indicator call state
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamIncallIndicatorData::SetCallState( 
   TInt aCallState )
   {
   iCallState = aCallState;
   }

// ---------------------------------------------------------
// TPhoneCmdParamIncallIndicatorData::SetMode
// Sets the incall indicator mode
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamIncallIndicatorData::SetMode( TInt aMode )
   {
   iMode = aMode;
   }

// ---------------------------------------------------------
// TPhoneCmdParamIncallIndicatorData::SetMute
// Sets the incall indicator mute status
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamIncallIndicatorData::SetMute( 
   TBool aMute )
   {
   iMute = aMute;
   }

// ---------------------------------------------------------
// TPhoneCmdParamIncallIndicatorData::SetCiphering
// Sets the incall indicator ciphering status
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamIncallIndicatorData::SetCiphering( 
   TBool aCiphering )
   {
   iCiphering = aCiphering;
   }

// ---------------------------------------------------------
// TPhoneCmdParamIncallIndicatorData::SetEmergency
// Sets the incall indicator emergency status
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamIncallIndicatorData::SetEmergency( 
   TBool aEmergency )
   {
   iEmergency = aEmergency;
   }

// ---------------------------------------------------------
// TPhoneCmdParamIncallIndicatorData::SetMute
// Sets the incall indicator mute status
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamIncallIndicatorData::SetLine2( 
   TBool aLine2 )
   {
   iLine2 = aLine2;
   }

// ---------------------------------------------------------
// TPhoneCmdParamIncallIndicatorData::SetLittleBubbleVisible
// Sets the little bubble visible or hidden
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamIncallIndicatorData::SetLittleBubbleVisible( 
   TBool aVisible )
   {
   iLittleBubbleVisible = aVisible;
   }
   
// ---------------------------------------------------------
// TPhoneCmdParamIncallIndicatorData::CallState
// Returns the incall indicator call state
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt TPhoneCmdParamIncallIndicatorData::CallState() const
   {
   return iCallState;
   }

// ---------------------------------------------------------
// TPhoneCmdParamIncallIndicatorData::Mode
// Returns the incall indicator mode
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt TPhoneCmdParamIncallIndicatorData::Mode() const
   {
   return iMode;
   }

// ---------------------------------------------------------
// TPhoneCmdParamIncallIndicatorData::Mute
// Returns the incall indicator mute
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TBool TPhoneCmdParamIncallIndicatorData::Mute() const
   {
   return iMute;
   }

// ---------------------------------------------------------
// TPhoneCmdParamIncallIndicatorData::Ciphering
// Returns the incall indicator ciphering status
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TBool TPhoneCmdParamIncallIndicatorData::Ciphering() const
   {
   return iCiphering;
   }

// ---------------------------------------------------------
// TPhoneCmdParamIncallIndicatorData::Emergency
// Returns the incall indicator emergency status
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TBool TPhoneCmdParamIncallIndicatorData::Emergency() const
   {
   return iEmergency;
   }

// ---------------------------------------------------------
// TPhoneCmdParamIncallIndicatorData::Line2
// Returns the incall indicator line2
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TBool TPhoneCmdParamIncallIndicatorData::Line2() const
   {
   return iLine2;
   }

// ---------------------------------------------------------
// TPhoneCmdParamIncallIndicatorData::LittleBubbleVisible
// Returns the little bubble visible status
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TBool TPhoneCmdParamIncallIndicatorData::LittleBubbleVisible() const
   {
   return iLittleBubbleVisible;
   }

// ---------------------------------------------------------
// TPhoneCmdParamIncallIndicatorData::SetCipheringIndicatorAllowed
// Sets the ciphering indicator allowed status
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamIncallIndicatorData::SetCipheringIndicatorAllowed( 
    TBool aAllowed )
    {
    iCipheringIndicatorAllowed = aAllowed;
    }

// ---------------------------------------------------------
// TPhoneCmdParamIncallIndicatorData::CipheringIndicatorAllowed
// Returns the ciphering indicator allowed status
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TBool TPhoneCmdParamIncallIndicatorData::CipheringIndicatorAllowed() const    
    {
    return iCipheringIndicatorAllowed;
    }
    

//  End of File
