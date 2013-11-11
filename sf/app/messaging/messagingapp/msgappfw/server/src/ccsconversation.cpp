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
* Description:  This class encapsulates a contact and list of conversation-entries
*                from the conatct.
*
*/

// INCLUDE FILES
#include <ccsconversationentry.h>
#include <ccsdefs.h>

// USER INCLUDE FILES
#include "ccsconversationcontact.h"
#include "ccsconversation.h"
#include "ccsdebug.h"

//Costant Declaration

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CCsConversation::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CCsConversation*
CCsConversation::NewL()
{
    PRINT ( _L("Enter CCsConversation::NewL") );

    CCsConversation* self = new (ELeave) CCsConversation;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    PRINT ( _L("End CCsConversation::NewL") );

    return self;
}

// ----------------------------------------------------------------------------
// CCsConversation::CCsConversation
// Constructor
// ----------------------------------------------------------------------------
CCsConversation::CCsConversation()
{
}

// ----------------------------------------------------------------------------
// CCsConversation::ConstructL
// Second phase constructor
// ----------------------------------------------------------------------------
void
CCsConversation::ConstructL()
{
    iConversationID = 0;
	iUnreadMessagesCount = 0;
	iContact = CCsConversationContact::NewL();
	iDeleted = EFalse;
	
	// initialize the iCsConversationEntryList
	iEntryList = new (ELeave) RPointerArray<CCsConversationEntry> ();
}

// ----------------------------------------------------------------------------
// CCsConversation::~CCsConversation
// Destructor
// ----------------------------------------------------------------------------
CCsConversation::~CCsConversation()
    {
    PRINT ( _L("Enter CCsConversation::~CCsConversation") );

    if (iEntryList)
        {
        iEntryList->ResetAndDestroy();
        iEntryList->Close();
        delete iEntryList;
        iEntryList = NULL;
        }
    // delete the details object
    if (iContact)
        {
        delete iContact;
        iContact = NULL;
        }

    PRINT ( _L("End CCsConversation::~CCsConversation") );
    }

// ----------------------------------------------------------------------------
// CCsConversation::GetEntryId
// Constructor
// ----------------------------------------------------------------------------
TCsConversationEntryID
CCsConversation::GetConversationId() const
    {
    return iConversationID;
    }

// ----------------------------------------------------------------------------
// CCsConversation::PluginInterface
// ----------------------------------------------------------------------------
void
CCsConversation::SetConversationId(
        TCsConversationEntryID aEntryId)
    {
    iConversationID = aEntryId;
    }

// ----------------------------------------------------------------------------
// Get the Conversation Entry Display name from Coversation Detail class
// ----------------------------------------------------------------------------
HBufC*
CCsConversation::GetDisplayName()const
    {
    HBufC* fn = iContact->GetDisplayName();
    if ( fn && fn->Length() > 0 )
       return fn;

	RPointerArray<TDesC> pnoList;
	iContact->GetPhoneNumberList(pnoList);
	if(pnoList.Count())
		{
		return static_cast<HBufC*>(pnoList[0]);
		}

    return NULL;
    }

// ----------------------------------------------------------------------------
// CCsConversation::GetContactId
// Get the Conversation Entry Contact Id from Coversation Detail class
// ----------------------------------------------------------------------------
TInt32 
CCsConversation::GetContactId()const
    {
    return iContact->GetContactId();
    }

// ----------------------------------------------------------------------------
// CCsConversation::GetLatestEntryL
// TThis function shall return the latest or topmost Conversation Entry
// of Conversation class
// ----------------------------------------------------------------------------
CCsConversationEntry* 
CCsConversation::GetLatestEntryL()const
    {
    CCsConversationEntry* ConversationEntry = NULL;
    if(iEntryList->Count() > 0)
        {
        ConversationEntry =
        (static_cast<CCsConversationEntry*>((*iEntryList)[iEntryList->Count()-1]));
        }
    return ConversationEntry;
    }

// ----------------------------------------------------------------------------
// CCsConversation::GetEntryL
// This function returns the conversation entry at a specified index.
// ----------------------------------------------------------------------------
CCsConversationEntry* CCsConversation::GetEntryL ( TInt aIndex ) 
    {
    return static_cast<CCsConversationEntry*>((*iEntryList)[aIndex]);
    }

// ----------------------------------------------------------------------------
// CCsConversation::GetLatestUnreadEntryL
// TThis function shall return the latest or topmost Conversation Entry
// of Conversation class
// ----------------------------------------------------------------------------
CCsConversationEntry*
CCsConversation::GetLatestUnreadEntryL() const
    {
    CCsConversationEntry* ConversationEntry = NULL;
    if (iEntryList->Count() > 0)
        {
        TInt index = KErrNotFound;
        index = FindUnreadEntry();

        if (index != KErrNotFound)
            {
            ConversationEntry = (*iEntryList)[index];
            }
        }
    return ConversationEntry;
    }

// ----------------------------------------------------------------------------
// CCsConversation::GetEntryListL
// This function shall return all the Conversation Entry
// of ConversationEntry
// ----------------------------------------------------------------------------
void
CCsConversation::GetEntryListL (
        RPointerArray<CCsConversationEntry>* aConversationEntryList)
    {
    TInt EntryCount = iEntryList->Count();
    if (EntryCount > 0)
        {
        // loop through each entry make a clone and add it to aConversationEntryList class
        for (TInt index=EntryCount-1 ; index>=0 ; index--)
            {
            CCsConversationEntry* conEntry =
                (static_cast<CCsConversationEntry*>(
                    (*iEntryList)[index]))->CloneL();
            aConversationEntryList->Append(conEntry);
            }
        }
    }

// ----------------------------------------------------------------------------
// CCsConversation::GetEntryListL
// This function shall return segment of Conversation Entry
// of ConversationEntry based on aStartIndex and aEndIndex
// ----------------------------------------------------------------------------
void
CCsConversation::GetEntryListL (
        RPointerArray<CCsConversationEntry>* aConversationEntryList,
        TInt aStartIndex,
        TInt aEndIndex)
    {
    TInt EntryCount = iEntryList->Count();
    if (EntryCount > 0 && aStartIndex>=0 && aEndIndex<=EntryCount)
        {
        // loop through each entry make a clone and add 
        // it to aConversationEntryList class
        for (TInt index=aEndIndex-1 ; index>=aStartIndex ; index--)
            {
            CCsConversationEntry* conEntry =
                (static_cast<CCsConversationEntry*>(
                    (*iEntryList)[index]))->CloneL();
            aConversationEntryList->Append(conEntry);
            }
        }
    }


// ----------------------------------------------------------------------------
// CCsConversation::AddEntryL
// Add a entry to this conversation
// ----------------------------------------------------------------------------
void
CCsConversation::AddEntryL(
        CCsConversationEntry* aCsConversationEntry)
    {
    PRINT ( _L("Enter CCsConversation::AddConversationEntryL") );
    // Update Unread message count
    if( aCsConversationEntry->IsAttributeSet( ECsAttributeUnread ) )
        {
        iUnreadMessagesCount++;
        }
    // first add the conversation details and then
    // add the conversation entry into array

    // get the clone of entry class and then add into
    // the entry list array
    CCsConversationEntry* ConversationEntry = aCsConversationEntry->CloneL();

    iEntryList->InsertInOrderAllowRepeats(ConversationEntry,
            CCsConversationEntry::Compare);

    PRINT ( _L("End CCsConversation::AddConversationEntryL") );
    }

// ----------------------------------------------------------------------------
// CCsConversation::AddContactDetails
// Add contact details for the conversation
// this will be display name, number and contact Id
// ----------------------------------------------------------------------------
void
CCsConversation::AddContactDetailsL(
        TInt32  aContactId,
        const HBufC* aDisplayName)
    {
    if(aDisplayName)
    {
        iContact->SetDisplayNameL(*aDisplayName);    
    }
    iContact->SetContactId (aContactId);    
    }

// ----------------------------------------------------------------------------
// CCsConversation::AddContactDetails
// Add contact number for the conversation
// ----------------------------------------------------------------------------
void
CCsConversation::AddContactDetailsL(
        TDesC& aContactNumber,
        TInt32 aContactID)
    {
    iContact->AddPhoneNumberL(aContactNumber);
    iContact->SetContactId(aContactID);
    }

// ----------------------------------------------------------------------------
// CCsConversation::UpdateEntryL
// Update the existing conversation entry if match found,
// otherwise add as new entry
// return aEvent as update/new conversation event
// ----------------------------------------------------------------------------
void
CCsConversation::UpdateEntryL(
        CCsConversationEntry* aCsConversationEntry,
        TUint32& aEvent)
    {
    PRINT ( _L("Enter CCsConversation::UpdateConversationEntryL") );

    TInt index = KErrNotFound;
    index = FindEntry (aCsConversationEntry);

    if (index != KErrNotFound)
        {
        CCsConversationEntry* CoversationEntry =
        (*iEntryList)[index];
        // Check if the status flag is changed
        // When flag is changed from Unread -> Read
        if(!CoversationEntry->IsAttributeSet(ECsAttributeUnread)
                && aCsConversationEntry->IsAttributeSet(ECsAttributeUnread))
            {
            iUnreadMessagesCount++;
            }
        // When flag is changed from Read -> Unread
        else if(CoversationEntry->IsAttributeSet(ECsAttributeUnread)
                && !aCsConversationEntry->IsAttributeSet(ECsAttributeUnread))
            {
            iUnreadMessagesCount--;
            }
        iEntryList->Remove(index);
        delete CoversationEntry;
        
        // mark aEvent as update
        aEvent = KConversationEventUpdate;
        }
    else
        {
        if(aCsConversationEntry->IsAttributeSet(ECsAttributeUnread) )
            {
            iUnreadMessagesCount++;
            }
        
        aEvent = KConversationEventNew;
        }

    CCsConversationEntry* CoversationEntry = aCsConversationEntry->CloneL();

    iEntryList->InsertInOrderAllowRepeats(
            CoversationEntry,CCsConversationEntry::Compare);

    PRINT ( _L("End CCsConversation::UpdateConversationEntryL") );
    }

// ----------------------------------------------------------------------------
// CCsConversation::DeleteEntryL
// Delete an entry at given index
// ----------------------------------------------------------------------------
void
CCsConversation::DeleteEntryL(
        TInt aindexDeletion)
    {
    CCsConversationEntry* CoversationEntry =
            (*iEntryList)[aindexDeletion];
    if(CoversationEntry->IsAttributeSet(ECsAttributeUnread))
        {
        iUnreadMessagesCount--;
        }
    iEntryList->Remove(aindexDeletion);
    delete CoversationEntry;

    PRINT1 ( _L("End CCsConversation::DeleteConversationEntryL - Unread Count:%d"),
            iUnreadMessagesCount );
    }

// ----------------------------------------------------------------------------
// CCsConversation::FindEntry
// this function shall find the entry
// ----------------------------------------------------------------------------
TInt
CCsConversation::FindEntry(
        CCsConversationEntry* aCsConversationEntry)
    {
    TInt index = KErrNotFound;
    
    index = iEntryList->Find(
                aCsConversationEntry,
                CCsConversationEntry::CompareById);

    return index;
    }

// ----------------------------------------------------------------------------
// CCsConversation::FindUnreadEntry
// this function shall find an unread entry
// ----------------------------------------------------------------------------
TInt
CCsConversation::FindUnreadEntry() const
    {
    TInt index = KErrNotFound;

    CCsConversationEntry *unreadEntry = CCsConversationEntry::NewL();
    unreadEntry->ChangeAttributes(ECsAttributeUnread, ECsAttributeNone);
    CleanupStack::PushL(unreadEntry);

    index = iEntryList->FindReverse(
                unreadEntry,
                CCsConversationEntry::CompareByUnreadAttrib);

    CleanupStack::PopAndDestroy(unreadEntry);

    return index;
    }

// ----------------------------------------------------------------------------
// CCsConversation::GetEntryCount
// returns Total Conversation Entries
// ----------------------------------------------------------------------------
TInt
CCsConversation::GetEntryCount()
    {
    return (iEntryList->Count());
    }
// ----------------------------------------------------------------------------
// CCsConversation::GetUnreadMessageCount
// returns total count of unread messages
// ----------------------------------------------------------------------------
TUint16
CCsConversation::GetUnreadMessageCount() const
    {
    return iUnreadMessagesCount;
    }

// ----------------------------------------------------------------------------
// CCsConversation::GetContact
// Return the contact object associated with this conversation
// ----------------------------------------------------------------------------
CCsConversationContact*
CCsConversation::GetContact()const
    {
    return iContact;
    }

// ----------------------------------------------------------------------------
// CCsConversation::IsSpecialConversation
// Returns true if it is a conversation for unknown drafts .
// ----------------------------------------------------------------------------
TBool
CCsConversation::IsSpecialConversation()
    {
    if (iConversationID == KUnknownConversationId 
            ||iConversationID == KBluetoothMsgsConversationId
            ||iConversationID == KInfraRedMsgsConversationId)
        {
    	return ETrue;
        }
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CCsConversation::MarkDeleted
// ----------------------------------------------------------------------------
void CCsConversation::MarkDeleted(TBool aDeleted)
    {
    iDeleted = aDeleted;
    }

// ----------------------------------------------------------------------------
// CCsConversation::IsDeleted
// ----------------------------------------------------------------------------
TBool CCsConversation::IsDeleted() const
    {
    return iDeleted;
    }

//EOF
