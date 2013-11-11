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
* Description:  CS Utils side class to hold latest conversation entry
 *                by timestamp
 *
*/


// SYSTEM INCLUDE FILES
#include <ccsconversationentry.h>
#include <ccsclientconversation.h>

// USER INCLUDE FILES
#include "ccsdebug.h"

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CCsClientConversation::CCsClientConversation
// Default constructor
// ----------------------------------------------------------------------------
CCsClientConversation::CCsClientConversation()
    {
    }

// ----------------------------------------------------------------------------
// CCsClientConversation::ConstructL
// Two phase construction
// ----------------------------------------------------------------------------
void CCsClientConversation::ConstructL()
    {
    iConversationEntryID = 0;
    iDisplayName = NULL;
    iContactId = KErrNotFound;
    iConversationEntry = NULL;
    iUnreadMessagesCount = 0;
    }

// ----------------------------------------------------------------------------
// CCsClientConversation::NewL
// Two Phase constructor
// ----------------------------------------------------------------------------
EXPORT_C CCsClientConversation* CCsClientConversation::NewL()
    {
    CCsClientConversation* self = new (ELeave) CCsClientConversation();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// CCsClientConversation::~CCsClientConversation
// Destructor
// ----------------------------------------------------------------------------
EXPORT_C CCsClientConversation::~CCsClientConversation()
    {
    if (iDisplayName)
        {
        delete iDisplayName;
        iDisplayName = NULL;
        }
    if (iConversationEntry)
        {
        delete iConversationEntry;
        iConversationEntry = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CCsClientConversation::GetConversationEntryIdList
// this shall return the conversation Entry Id
// ----------------------------------------------------------------------------
EXPORT_C TCsConversationEntryID
CCsClientConversation::GetConversationEntryId() const
    {
    return iConversationEntryID;
    }

// ----------------------------------------------------------------------------
// CCsClientConversation::AddConversationEntryId
// this function shall set conversation entry ID in the object
// ----------------------------------------------------------------------------
EXPORT_C void
CCsClientConversation::SetConversationEntryId(TCsConversationEntryID aEnryId)
    {
    iConversationEntryID = aEnryId;
    }

// ----------------------------------------------------------------------------
// CCsClientConversation::GetDisplayName
// this shall return the conversation name of the object
// ----------------------------------------------------------------------------
EXPORT_C HBufC* CCsClientConversation::GetDisplayName() const
    {
    return iDisplayName;
    }

// ----------------------------------------------------------------------------
// CCsClientConversation::SetDisplayNameL
// this function shall set conversation name in the object
// ----------------------------------------------------------------------------
EXPORT_C void
CCsClientConversation::SetDisplayNameL(
                           const HBufC* aDisplayName)
    {
    if (aDisplayName)
        {
        iDisplayName = aDisplayName->AllocL();
        }
    }

// ----------------------------------------------------------------------------
// CCsClientConversation::GetContactId
// this function shall return contact Id of the object
// ----------------------------------------------------------------------------
EXPORT_C TInt32
CCsClientConversation::GetContactId() const
    {
    return iContactId;
    }

// ----------------------------------------------------------------------------
// CCsClientConversation::SetContactId
// this function shall set the phonebook contact Id in the object
// ----------------------------------------------------------------------------
EXPORT_C void
CCsClientConversation::SetContactId(const TInt32 aContactId)
    {
    iContactId = aContactId;     
    }

// ----------------------------------------------------------------------------
// CCsClientConversation::GetConversationEntry
// this function shall return the latest conversation entry by timestamp of
// conversation identified by iConversationEntryID of the object
// ----------------------------------------------------------------------------
EXPORT_C CCsConversationEntry*
CCsClientConversation::GetConversationEntry() const
    {
    return iConversationEntry;
    }

// ----------------------------------------------------------------------------
// CCsClientConversation::SetConversationEntryL
// this function shall set the latest conversation entry by timestamp of
// conversation identified by iConversationEntryID of the object
// ----------------------------------------------------------------------------
EXPORT_C void
CCsClientConversation::SetConversationEntryL(
        const CCsConversationEntry* aCsConversationEntry)
    {
    if (aCsConversationEntry)
        {
        iConversationEntry = aCsConversationEntry->CloneL();
        }
    }

// ----------------------------------------------------------------------------
// CCsClientConversation::CloneL
// This function return the clone of object
// ----------------------------------------------------------------------------
EXPORT_C CCsClientConversation* CCsClientConversation::CloneL() const
    {
    CCsClientConversation* cloneObject = CCsClientConversation::NewL();
    CleanupStack::PushL(cloneObject);

    cloneObject->SetConversationEntryId(iConversationEntryID);
    cloneObject->SetDisplayNameL(iDisplayName);
    cloneObject->SetContactId(iContactId);
    cloneObject->SetConversationEntryL(iConversationEntry);
    cloneObject->SetUnreadMessageCount(iUnreadMessagesCount);
    
    CleanupStack::Pop(cloneObject);

    return cloneObject;
    }

// ----------------------------------------------------------------------------
// CCsClientConversation::ExternalizeL
// Writes CCsClientConversation object to the stream
// ----------------------------------------------------------------------------
EXPORT_C  void CCsClientConversation::ExternalizeL(RWriteStream& aStream) const
    {
    // write iConversationEntryID count into the stream
    aStream.WriteInt32L(iConversationEntryID);
    aStream.WriteUint16L(iUnreadMessagesCount);

    if (iDisplayName)
        {
        // now write the display name
        aStream.WriteInt8L(iDisplayName->Length());
        aStream << *iDisplayName;
        }
    else
        {
        aStream.WriteInt8L(0);
        }

    // phonebook contactId
    aStream.WriteInt32L(iContactId);
    
    // write iConversationEntry count into the stream
    if (iConversationEntry)
        {
        iConversationEntry->ExternalizeL(aStream);
        }
    }

// ----------------------------------------------------------------------------
// CCsClientConversation::InternalizeL
// Initializes CCsClientConversation from stream
// ----------------------------------------------------------------------------
EXPORT_C  void CCsClientConversation::InternalizeL(
        RReadStream& aStream)
    {
    // now read the conversation id

    iConversationEntryID = aStream.ReadInt32L();
    iUnreadMessagesCount = aStream.ReadUint16L();
    // read first name
    TInt lenName = aStream.ReadInt8L();
    if (lenName > 0)
        {
        iDisplayName = HBufC::NewL(aStream, lenName);
        }
        
    // read contact link
    iContactId = aStream.ReadInt32L();    
        
    //Conversation entry
    iConversationEntry = CCsConversationEntry::NewL();
    iConversationEntry->InternalizeL(aStream);
    }

// ----------------------------------------------------------------------------
// CCsClientConversation::CompareByTimeStamp
// Compares 2 CCsClientConversation objects based on the latest
// conversation entry by time stamp
// ----------------------------------------------------------------------------
EXPORT_C TInt CCsClientConversation::CompareByTimeStamp(
        const CCsClientConversation& obj1,
        const CCsClientConversation& obj2)
    {
    TInt64 timestamp1 = obj1.GetConversationEntry()->TimeStamp();
    TInt64 timestamp2 = obj2.GetConversationEntry()->TimeStamp();
    TInt result = -1;

    if ( timestamp1 == timestamp2 )
        {
        result = 0;
        }
    else if ( timestamp1 < timestamp2 )
        {
        result =  1;
        }
    
    return result;
    }

// ----------------------------------------------------------------------------
// CCsClientConversation::CompareByConversationEntryId
// Compares 2 CCsClientConversation objects based on ConversationEntryId.
// ----------------------------------------------------------------------------
EXPORT_C TBool CCsClientConversation::CompareByConversationEntryId(
        const CCsClientConversation& obj1,
        const CCsClientConversation& obj2)
    {
    TBool result = EFalse;

    if( obj1.GetConversationEntryId() == obj2.GetConversationEntryId() )
        {
        result =  ETrue;
        }
    
    return result;
    }
// ----------------------------------------------------------------------------
// CCsClientConversation::GetUnreadMessageCount
// returns the unread message count in the conversation
// ----------------------------------------------------------------------------
EXPORT_C TUint16 CCsClientConversation::GetUnreadMessageCount() const
    {
    return iUnreadMessagesCount;
    }
// ----------------------------------------------------------------------------
// CCsClientConversation::SetUnreadMessageCount
// Sets the unread message count
// ----------------------------------------------------------------------------
EXPORT_C void CCsClientConversation::SetUnreadMessageCount(TUint16 aCount)
    {
    iUnreadMessagesCount = aCount;
    }
// end of file
