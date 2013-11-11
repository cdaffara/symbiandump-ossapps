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
* Description: Implementation of TPhoneCmdParamSfiData class.
*
*/


// INCLUDE FILES
#include "tphonecmdparamsfidata.h"   

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// TPhoneCmdParamSfiData::TPhoneCmdParamSfiData
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TPhoneCmdParamSfiData::TPhoneCmdParamSfiData() :
    TPhoneCommandParam(),
    iNumber( KNullDesC ),
    iName( KNullDesC )
    {
    iParamId = EPhoneParamIdSfiData;
    }

// ---------------------------------------------------------
// TPhoneCmdParamSfiData::SetLabelText
// Sets the call header label text
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamSfiData::SetNumber( 
   const TDesC& aNumber )
   {
   iNumber.Set( aNumber );
   }

// ---------------------------------------------------------
// TPhoneCmdParamSfiData::SetShortLabelText
// Sets the call header short label text
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamSfiData::SetName( 
   const TDesC& aName )
   {
   iName.Set( aName );
   }

// ---------------------------------------------------------
// TPhoneCmdParamSfiData::LabelText
// Returns the call header label text
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C const TDesC& TPhoneCmdParamSfiData::Number() const
   {
   return iNumber;
   }

// ---------------------------------------------------------
// TPhoneCmdParamSfiData::ShortLabelText
// Returns the call header short label text
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C const TDesC& TPhoneCmdParamSfiData::Name() const
   {
   return iName;
   }

//  End of File
