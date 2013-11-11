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
* Description: Implementation of TPhoneCmdParamKeyCapture class.
*
*/


// INCLUDE FILES

#include "tphonecmdparamkeycapture.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
EXPORT_C TPhoneCmdParamKeyCapture::TPhoneCmdParamKeyCapture() :
    TPhoneCommandParam(),
    iKey( EStdKeyNull ),
    iKeyCode( EKeyNull ),
    iCaptureType( EPhoneKeyAllEvents )
    {
    iParamId = EPhoneParamIdKeyCapture;
    }

// ---------------------------------------------------------
// TPhoneCmdParamKeyCapture::SetKey
// Sets the key scan code
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamKeyCapture::SetKey( 
   TStdScanCode aKey )
   {
   iKey = aKey;   
   }

// ---------------------------------------------------------
// TPhoneCmdParamKeyCapture::SetKeyCode
// Sets the key code
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamKeyCapture::SetKeyCode( TKeyCode aKeyCode )
   {
   iKeyCode = aKeyCode;   
   }

// ---------------------------------------------------------
// TPhoneCmdParamKeyCapture::SetCaptureType
// Sets the key capture type
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamKeyCapture::SetCaptureType( 
   TPhoneCaptureType aCaptureType )
   {
   iCaptureType = aCaptureType;   
   }

// ---------------------------------------------------------
// TPhoneCmdParamKeyCapture::Key
// Returns the key scan code
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TStdScanCode TPhoneCmdParamKeyCapture::Key() const
   {
   return iKey;
   }

// ---------------------------------------------------------
// TPhoneCmdParamKeyCapture::KeyCode
// Returns the key code
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TKeyCode TPhoneCmdParamKeyCapture::KeyCode() const
   {
   return iKeyCode;
   }

// ---------------------------------------------------------
// TPhoneCmdParamKeyCapture::CaptureType
// Returns the key capture type
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TPhoneCaptureType TPhoneCmdParamKeyCapture::CaptureType() const
   {
   return iCaptureType;
   }

//  End of File  
