/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This shall hold all the contact data associated with a
*                 conversation like name, numbers and contact link .
 *
*/


// SYSTEM INCLUDE FILES
#include <miutpars.h>

// USER INCLUDE FILES
#include "ccsconversation.h"
#include "ccsconversationcontact.h"
#include "ccsdebug.h"

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CCsConversationContact::CCsConversationContact
// Default constructor
// ----------------------------------------------------------------------------
CCsConversationContact::CCsConversationContact()
    {
    }

// ----------------------------------------------------------------------------
// CCsConversationContact::ConstructL
// Two phase construction
// ----------------------------------------------------------------------------
void
CCsConversationContact::ConstructL()
    {
    iDisplayName = NULL;
    iContactId = KErrNotFound;
    iPhoneNumberList = new (ELeave) RPointerArray<HBufC> ();
    }

// ----------------------------------------------------------------------------
// CCsConversationContact::NewL
// Two Phase constructor
// ----------------------------------------------------------------------------
CCsConversationContact*
CCsConversationContact::NewL()
    {
    CCsConversationContact* self = new (ELeave) CCsConversationContact();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// CCsConversationContact::~CCsConversationContact
// Destructor
// ----------------------------------------------------------------------------
CCsConversationContact::~CCsConversationContact()
    {
    if(iDisplayName)
        {
        delete iDisplayName;
        iDisplayName = NULL;
        }

    if (iPhoneNumberList)
        {
        iPhoneNumberList->ResetAndDestroy();
        iPhoneNumberList->Close();
        delete iPhoneNumberList;
        iPhoneNumberList = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CCsConversationContact::GetDisplayName
// get the display name of the conversation
// ----------------------------------------------------------------------------
HBufC*
CCsConversationContact::GetDisplayName() const
    {
    return iDisplayName;
    }

// ----------------------------------------------------------------------------
// CCsConversationContact::SetDisplayNameL
// Sets the display name of the conversation
// ----------------------------------------------------------------------------
void
CCsConversationContact::SetDisplayNameL(
        const TDesC& aDisplayName)
    {
        TRAPD(error, iDisplayName = aDisplayName.AllocL());
        if(error != KErrNone)
            {
            // handle error
            // call panic
            PRINT1 ( _L("CCsConversationContact::SetFirstNameL - Error:%d"),
                    error );
            }
  
    }

// ----------------------------------------------------------------------------
// CCsConversationContact::GetContactId
// get the phonebook Contact Id link for the conversation
// ----------------------------------------------------------------------------
TInt32
CCsConversationContact::GetContactId() const
    {
    return iContactId;
    }

// ----------------------------------------------------------------------------
// CCsConversationContact::SetContactLink
// Sets the phonebook Contact Id for the conversation
// ----------------------------------------------------------------------------
void
CCsConversationContact::SetContactId(
        TInt32 aContactId)
    {
    iContactId = aContactId;
    }

// ----------------------------------------------------------------------------
// CCsConversationContact::AddPhoneNumber
// Sets the phone number inside contact list
// ----------------------------------------------------------------------------
void
CCsConversationContact::AddPhoneNumberL(
        TDesC& aPhoneNumber)
    {
    HBufC* phoneNumber = aPhoneNumber.AllocL();
    iPhoneNumberList->AppendL(phoneNumber);
    }

// ----------------------------------------------------------------------------
// CCsConversationContact::GetPhoneNumberList
// Returns a list of associated phone numbers
// ----------------------------------------------------------------------------
void
CCsConversationContact::GetPhoneNumberList(
        RPointerArray<TDesC>& aPhoneNumbers)
    {
    for ( int index = 0; index < iPhoneNumberList->Count(); index++ )
        {
        HBufC* value = (*iPhoneNumberList)[index];
        aPhoneNumbers.Append(value);
        }
    }

// ----------------------------------------------------------------------------
// CCsConversationContact::MatchPhoneNumber
// Check if the input phone number (aPhoneNumber) is associated with this
// contact. Compares for aNumDigits.
// ----------------------------------------------------------------------------
TBool
CCsConversationContact::MatchPhoneNumber(
        TDesC& aPhoneNumber, TInt aNumDigits) const
    {
    if (aPhoneNumber.Length() == 0)
        {
        return EFalse;
        }

    TInt useDigits = aNumDigits;
    
    // Check if phone number is an email address
    TImMessageField email;
    if ( email.ValidInternetEmailAddress(aPhoneNumber) )
        {
        useDigits = aPhoneNumber.Length();
        }
    
    TInt count = iPhoneNumberList->Count();
    for ( TInt i = 0 ; i < count; i++)
        {
        HBufC* pno = (*iPhoneNumberList)[i];
        
        // Compare only last useDigits dgits of the number/ address
        if (aPhoneNumber.Right(useDigits).CompareF(pno->Des().
                Right(useDigits)) == 0)
            {
            return ETrue;
            }
        }
    return EFalse;
    }


// EOF
