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
* Description: Implementation of TPhoneCmdParamRingTone class.
*
*/


// INCLUDE FILES
#include "tphonecmdparamringtone.h" 

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// TPhoneCmdParamRingTone::TPhoneCmdParamRingTone
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TPhoneCmdParamRingTone::TPhoneCmdParamRingTone() :
    TPhoneCommandParam(),
    iRingTone( KNullDesC ),
    iTextToSay( KNullDesC ),
    iType( EPhoneRingToneProfile ),
    iVolume( 0 ),
    iRingingType( 0 ),
    iIsCallerText( EFalse ),
    iIsCallerImage( EFalse )
    {
    iParamId = EPhoneParamIdRingTone;
    }

// ---------------------------------------------------------
// TPhoneCmdParamRingTone::SetType
// Sets the ring tone type
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamRingTone::SetType( 
   TPhoneRingToneType aType )
   {
   iType = aType;
   }

// ---------------------------------------------------------
// TPhoneCmdParamRingTone::SetVolume
// Sets the ring tone volume
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamRingTone::SetVolume( TInt aVolume )
   {
   iVolume = aVolume;
   }

// ---------------------------------------------------------
// TPhoneCmdParamRingTone::SetRingingType
// Sets the ring tone ringing type 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamRingTone::SetRingingType( TInt aRingingType )
   {
   iRingingType = aRingingType;
   }

// ---------------------------------------------------------
// TPhoneCmdParamRingTone::SetRingTone
// Sets the ring tone
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamRingTone::SetRingTone( const TDesC& aRingTone )
   {
   iRingTone.Set( aRingTone );
   }

// ---------------------------------------------------------
// TPhoneCmdParamRingTone::SetTextToSay
// Sets the text to say
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamRingTone::SetTextToSay( const TDesC& aTextToSay )
   {
   iTextToSay.Set( aTextToSay );
   }

// ---------------------------------------------------------
// TPhoneCmdParamRingTone::Type
// Returns the ring tone type
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TPhoneRingToneType TPhoneCmdParamRingTone::Type() const
   {
   return iType;
   }

// ---------------------------------------------------------
// TPhoneCmdParamRingTone::Volume
// Returns the ring tone volume
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt TPhoneCmdParamRingTone::Volume() const
   {
   return iVolume;
   }

// ---------------------------------------------------------
// TPhoneCmdParamRingTone::RingingType
// Returns the ring tone ringing type
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt TPhoneCmdParamRingTone::RingingType() const
   {
   return iRingingType;
   }

// ---------------------------------------------------------
// TPhoneCmdParamRingTone::RingTone
// Returns the ring tone
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C const TDesC& TPhoneCmdParamRingTone::RingTone() const
   {
   return iRingTone;
   }

// ---------------------------------------------------------
// TPhoneCmdParamRingTone::TextToSay
// Returns the text to say
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C const TDesC& TPhoneCmdParamRingTone::TextToSay() const
   {
   return iTextToSay;
   }

// ---------------------------------------------------------
// TPhoneCmdParamRingTone::SetCallerTextStatus
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamRingTone::SetCallerTextStatus( TBool aStatus )
    {
    iIsCallerText = aStatus;
    }
// ---------------------------------------------------------
// TPhoneCmdParamRingTone::IsCallerText
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TBool TPhoneCmdParamRingTone::IsCallerText()
    {
    return iIsCallerText;
    }

// ---------------------------------------------------------
// TPhoneCmdParamRingTone::SetCallerImageStatus
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamRingTone::SetCallerImageStatus( TBool aStatus )
    {
    iIsCallerImage = aStatus;
    }

// ---------------------------------------------------------
// TPhoneCmdParamRingTone::IsCallerImage
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TBool TPhoneCmdParamRingTone::IsCallerImage()
    {
    return iIsCallerImage;
    }


//  End of File
