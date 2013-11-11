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
* Description: Implementation of TPhoneCmdParamBitmap class.
*
*/


// INCLUDE FILES

#include "tphonecmdparambitmap.h"
#include "phoneui.pan"  

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// TPhoneCmdParamBitmap::TPhoneCmdParamBitmap
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TPhoneCmdParamBitmap::TPhoneCmdParamBitmap() :
    TPhoneCommandParam(),
    iBitmap( NULL ),
    iMaskBitmap( NULL )
    {
    iParamId = EPhoneParamIdBitmap;
    }

// ---------------------------------------------------------
// TPhoneCmdParamBitmap::SetBitmap
// Sets the Bitmap
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamBitmap::SetBitmap( CFbsBitmap* aBitmap )
   {
   __ASSERT_DEBUG( aBitmap, Panic( EPhoneUtilsParameterNotInitialized ) );
   iBitmap = aBitmap;
   }

// ---------------------------------------------------------
// TPhoneCmdParamBitmap::Bitmap
// Returns the Bitmap
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C CFbsBitmap* TPhoneCmdParamBitmap::Bitmap() const
   {
   __ASSERT_DEBUG( iBitmap, Panic( EPhoneUtilsInvariant ) );
   return iBitmap;
   }

// ---------------------------------------------------------
// TPhoneCmdParamBitmap::SetMaskBitmap
// Sets the mask bitmap
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamBitmap::SetMaskBitmap( CFbsBitmap* aMaskBitmap )
   {
   __ASSERT_DEBUG( aMaskBitmap, Panic( EPhoneUtilsParameterNotInitialized ) );
   iMaskBitmap = aMaskBitmap;
   }

// ---------------------------------------------------------
// TPhoneCmdParamBitmap::MaskBitmap
// Returns the mask bitmap
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C CFbsBitmap* TPhoneCmdParamBitmap::MaskBitmap() const
   {
   return iMaskBitmap;
   }


//  End of File  
