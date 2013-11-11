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
* Description: Implementation of TPhoneCmdParamGlobalNote class.
*
*/


// INCLUDE FILES
#include <avkon.hrh>
#include "tphonecmdparamglobalnote.h"
#include "phoneui.pan" 

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// TPhoneCmdParamGlobalNote::TPhoneCmdParamGlobalNote
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TPhoneCmdParamGlobalNote::TPhoneCmdParamGlobalNote():
    TPhoneCommandParam(),
    iType( EPhoneNotificationDialog ),
    iTextResourceId( 0 ),
    iText( KNullDesC ),
    iTimeout( 0 ),
    iNotificationDialog( EFalse ),
    iCauseCode( KErrNotFound ),
    iToneType( EPhoneNoTone )
    {
    iParamId = EPhoneParamIdGlobalNote;
    }

// ---------------------------------------------------------
// TPhoneCmdParamGlobalNote::SetType
// Sets the global note type
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamGlobalNote::SetType( /*TAknGlobalNoteType*/ TPhoneNotificationType aType )
   {
   iType = aType;   
   }

// ---------------------------------------------------------
// TPhoneCmdParamGlobalNote::SetTextResourceId
// Sets the global note resource ID
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamGlobalNote::SetTextResourceId( 
   TInt aTextResourceId )
   {
    __ASSERT_DEBUG( aTextResourceId, 
        Panic( EPhoneUtilsParameterNotInitialized ) );
   iTextResourceId = aTextResourceId;   
   }

// ---------------------------------------------------------
// TPhoneCmdParamGlobalNote::SetText
// Sets the global note text
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamGlobalNote::SetText( 
   const TDesC& aText )
   {
   iText.Set( aText );
   }

// ---------------------------------------------------------
// TPhoneCmdParamGlobalNote::SetTimeout
// Sets the timeout value for a note.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamGlobalNote::SetTimeout( TInt aTimeout )
    {
    iTimeout = aTimeout;
    }

// ---------------------------------------------------------
// TPhoneCmdParamGlobalNote::SetNotificationDialog
// Sets the notification dialog flag.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamGlobalNote::SetNotificationDialog( 
        TBool aNotificationDialog )
    {
    iNotificationDialog = aNotificationDialog;
    }

// ---------------------------------------------------------
// TPhoneCmdParamGlobalNote::SetCauseCode
// Sets the cause code.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamGlobalNote::SetCauseCode( 
        TInt aCauseCode )
    {
    iCauseCode = aCauseCode;
    }

// ---------------------------------------------------------
// TPhoneCmdParamGlobalNote::SetToneType
// Sets the tone type.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamGlobalNote::SetToneType( 
   TPhoneNotificationToneType aToneType )
    {
    iToneType = aToneType;
    }
            
// ---------------------------------------------------------
// TPhoneCmdParamGlobalNote::ToneType
// Returns the tone type.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TPhoneNotificationToneType TPhoneCmdParamGlobalNote::ToneType() const
    {
    return iToneType;
    }

// ---------------------------------------------------------
// TPhoneCmdParamGlobalNote::Type
// Returns the global note type
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C /*TAknGlobalNoteType*/ TPhoneNotificationType TPhoneCmdParamGlobalNote::Type() const
   {
   return iType;
   }

// ---------------------------------------------------------
// TPhoneCmdParamGlobalNote::TextResourceId
// Returns the global note text resource ID
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt TPhoneCmdParamGlobalNote::TextResourceId() const
   {
   return iTextResourceId;
   }

// ---------------------------------------------------------
// TPhoneCmdParamGlobalNote::Text
// Returns the global note text
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C const TDesC& TPhoneCmdParamGlobalNote::Text() const
   {
   return iText;
   }

// ---------------------------------------------------------
// TPhoneCmdParamGlobalNote::Timeout
// Returns the timeout value.
// ---------------------------------------------------------
//
EXPORT_C TInt TPhoneCmdParamGlobalNote::Timeout() const
    {
    return iTimeout;
    }

// ---------------------------------------------------------
// TPhoneCmdParamGlobalNote::NotificationDialog
// Returns the notification dialog flag.
// ---------------------------------------------------------
//
EXPORT_C TBool TPhoneCmdParamGlobalNote::NotificationDialog() const
    {
    return iNotificationDialog;
    }

// ---------------------------------------------------------
// TPhoneCmdParamGlobalNote::CauseCode
// Returns the cause code.
// ---------------------------------------------------------
//
EXPORT_C TInt TPhoneCmdParamGlobalNote::CauseCode() const
    {
    return iCauseCode;
    }

//  End of File  
