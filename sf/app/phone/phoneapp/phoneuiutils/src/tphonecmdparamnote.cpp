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
* Description: Implementation of TPhoneCmdParamNote class.
*
*/


// INCLUDE FILES
#include "tphonecmdparamnote.h" 
#include "phoneui.pan"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// TPhoneCmdParamNote::TPhoneCmdParamNote
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TPhoneCmdParamNote::TPhoneCmdParamNote():
    TPhoneCommandParam(),
    iType( EPhoneNoteCustom ),
    iResourceId( 0 ),
    iTextResourceId( 0 ),
    iText( KNullDesC ),
    iTone( CAknNoteDialog::ENoTone ),
    iTimeout( CAknNoteDialog::ENoTimeout )
    {
    iParamId = EPhoneParamIdNote;
    }

// ---------------------------------------------------------
// TPhoneCmdParamNote::SetType
// Sets the note type
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamNote::SetType( TPhoneNoteType aType )
   {
   iType = aType;   
   }

// ---------------------------------------------------------
// TPhoneCmdParamNote::SetResourceId
// Sets the note resource ID
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamNote::SetResourceId( TInt aResourceId )
   {
   __ASSERT_DEBUG( aResourceId, Panic( EPhoneUtilsParameterNotInitialized ) );
   iResourceId = aResourceId;   
   }

// ---------------------------------------------------------
// TPhoneCmdParamNote::SetTextResourceId
// Sets the note text resource ID
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamNote::SetTextResourceId( TInt aTextResourceId )
    {
    __ASSERT_DEBUG( aTextResourceId,
        Panic( EPhoneUtilsParameterNotInitialized ) );
    iTextResourceId = aTextResourceId;   
    }

// ---------------------------------------------------------
// TPhoneCmdParamNote::SetText
// Sets the note text
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamNote::SetText( const TDesC& aText )
   {
   iText.Set( aText );   
   }

// ---------------------------------------------------------
// TPhoneCmdParamNote::SetTone
// Sets the note tone
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamNote::SetTone( CAknNoteDialog::TTone aTone )
   {
   iTone = aTone;   
   }

// ---------------------------------------------------------
// TPhoneCmdParamNote::SetTimeout
// Sets the note timeout
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamNote::SetTimeout( 
   CAknNoteDialog::TTimeout aTimeout )
   {
   iTimeout = aTimeout;   
   }

// ---------------------------------------------------------
// TPhoneCmdParamNote::Type
// Returns the note type
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TPhoneNoteType TPhoneCmdParamNote::Type() const
   {
   return iType;
   }

// ---------------------------------------------------------
// TPhoneCmdParamNote::ResourceId
// Returns the note resource ID
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt TPhoneCmdParamNote::ResourceId() const
    {
    return iResourceId;
    }

// ---------------------------------------------------------
// TPhoneCmdParamNote::TextResourceId
// Returns the note text resource ID
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt TPhoneCmdParamNote::TextResourceId() const
    {
    return iTextResourceId;
    }

// ---------------------------------------------------------
// TPhoneCmdParamNote::Text
// Returns the note text
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C const TDesC& TPhoneCmdParamNote::Text() const
   {
   return iText;
   }

// ---------------------------------------------------------
// TPhoneCmdParamNote::Tone
// Returns the note tone
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C CAknNoteDialog::TTone TPhoneCmdParamNote::Tone() const
   {
   return iTone;
   }

// ---------------------------------------------------------
// TPhoneCmdParamNote::Timeout
// Returns the note timeout
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C CAknNoteDialog::TTimeout TPhoneCmdParamNote::Timeout() const
   {
   return iTimeout;
   }

//  End of File  
