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
* Description:  Client side class to hold the conversation entry.
 *
*/


// SYSTEM INCLUDE FILES
#include <ccsconversationentry.h>

// USER INCLUDE FILES
#include "ccsdebug.h"

// CONSTANTS
const TInt KMaxDescrSize = 1500; // Message body restricted to this size

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CCsConEntry::CCsConEntry
// Default constructor
// ----------------------------------------------------------------------------
CCsConversationEntry::CCsConversationEntry()
    {
    }

// ----------------------------------------------------------------------------
// CCsConversationEntry::ConstructL
// Two phase construction
// ----------------------------------------------------------------------------
void CCsConversationEntry::ConstructL()
    {
    iEntryID = 0;
    iTimeStamp = 0;
    iContact = NULL;
    iConversationDir = ECsDirectionUnknown;
    iDescription = NULL;
    iConversationType = ECsUnknown;
    iConversationAttrib = ECsAttributeNone;
    iConversationStatus = ECsSendStateUnknown;
    }

// ----------------------------------------------------------------------------
// CCsConversationEntry::NewL
// Two Phase constructor
// ----------------------------------------------------------------------------
EXPORT_C CCsConversationEntry* CCsConversationEntry::NewL()
    {
    CCsConversationEntry* self = new (ELeave) CCsConversationEntry();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// CCsConversationEntry::~CCsConversationEntry
// Destructor
// ----------------------------------------------------------------------------
EXPORT_C CCsConversationEntry::~CCsConversationEntry()
    {
    // delete the iContact if its allocated
    if(iContact)
        {
        delete iContact;
        iContact = NULL;
        }
    
    // delete the iDescription if its allocated
    if(iDescription)
        {
        delete iDescription;
        iDescription = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CCsConEntry::EntryId
// Returns the EntryId of the object
// ----------------------------------------------------------------------------
EXPORT_C TInt CCsConversationEntry::EntryId() const
{
return iEntryID;
}

// ----------------------------------------------------------------------------
// CCsConversationEntry::SetEntryId
// Sets the entryID of this object
// ----------------------------------------------------------------------------
EXPORT_C void CCsConversationEntry::SetEntryId(const TInt aEnryId)
    {
    iEntryID = aEnryId;
    }

// ----------------------------------------------------------------------------
// CCsConversationEntry::TimeStamp
// Provides the Conversation TimeStamp of this object
// ----------------------------------------------------------------------------
EXPORT_C TInt64 CCsConversationEntry::TimeStamp() const
    {
    return iTimeStamp;
    }

// ----------------------------------------------------------------------------
// CCsConversationEntry::SetTimeStampL
// Sets the Conversation TimeStamp of this object
// ----------------------------------------------------------------------------
EXPORT_C void CCsConversationEntry::SetTimeStampL(const TInt64 aTimeStamp)
    {
    iTimeStamp = aTimeStamp;
    }

// ----------------------------------------------------------------------------
// CCsConversationEntry::Contact
// Provides the Conversation contact of this object
// ----------------------------------------------------------------------------
EXPORT_C HBufC* CCsConversationEntry::Contact() const
    {
    return iContact;
    }

// ----------------------------------------------------------------------------
// CCsConversationEntry::SetContactL
// Sets the Conversation contact of this object
// ----------------------------------------------------------------------------
EXPORT_C void CCsConversationEntry::SetContactL(const TDesC& aContact)
{
    iContact = aContact.AllocL();
}

// ----------------------------------------------------------------------------
// CCsConversationEntry::ConversationDir
// Provides the Conversation direction of this object
// ----------------------------------------------------------------------------
EXPORT_C TCsDirection CCsConversationEntry::ConversationDir() const
    {
    return iConversationDir;
    }

// ----------------------------------------------------------------------------
// CCsConversationEntry::SetConversationDir
// Sets the Conversation direction of this object
// ----------------------------------------------------------------------------
EXPORT_C void CCsConversationEntry::SetConversationDir(
        const TCsDirection aConversationDir)
    {
    iConversationDir = aConversationDir;
    }

// ----------------------------------------------------------------------------
// CCsConversationEntry::GetType
// Provides the conversation flags of the object
// ----------------------------------------------------------------------------
EXPORT_C TCsType CCsConversationEntry::GetType() const
    {
    return iConversationType;
    }

// ----------------------------------------------------------------------------
// CCsConversationEntry::SetType
// Sets the conversation flags of this object
// ----------------------------------------------------------------------------
EXPORT_C void CCsConversationEntry::SetType(const TCsType& aCsConversationType)
    {
    iConversationType = aCsConversationType;
    }

// ----------------------------------------------------------------------------
// CCsConversationEntry::GetSendState
// Provides the conversation status of this object
// ----------------------------------------------------------------------------
EXPORT_C TCsSendState CCsConversationEntry::GetSendState() const
    {
    return iConversationStatus;
    }

// ----------------------------------------------------------------------------
// CCsConversationEntry::SetSendState
// Sets the conversation status of this object
// ----------------------------------------------------------------------------
EXPORT_C void CCsConversationEntry::SetSendState(const TCsSendState& aCsConversationStatus)
    {
    iConversationStatus = aCsConversationStatus;
    }

// ----------------------------------------------------------------------------
// CCsConversationEntry::Description
// Provides the Conversation description(body) of this object
// ----------------------------------------------------------------------------
EXPORT_C HBufC* CCsConversationEntry::Description() const
    {
    return iDescription;
    }

// ----------------------------------------------------------------------------
// CCsConversationEntry::SetDescriptionL
// Sets the Conversation description of this object
// ----------------------------------------------------------------------------
EXPORT_C void CCsConversationEntry::SetDescriptionL(const TDesC& aDescription)
{

    TInt descrLength = aDescription.Length();
    if (descrLength > KMaxDescrSize)
        descrLength = KMaxDescrSize;
    iDescription = HBufC::NewL(descrLength);
    iDescription->Des().Copy(aDescription.Left(descrLength));

}

// ----------------------------------------------------------------------------
// CCsConversationEntry::ChangeAttributes
// Sets or Clears the given attribute(s).
// ----------------------------------------------------------------------------
EXPORT_C void CCsConversationEntry::ChangeAttributes( TUint16 aSetAttributes,
        TUint16 aClearAttributes )
    {
    // Set attribute(s)
    iConversationAttrib |= aSetAttributes;

    // Clear atttibute(s)
    iConversationAttrib &= ~aClearAttributes;
    }

// ----------------------------------------------------------------------------
// CCsConversationEntry::IsAttributeSet
// Checks if the give attribute is set.
// ----------------------------------------------------------------------------
EXPORT_C TBool CCsConversationEntry::IsAttributeSet(TCsAttribute aAttribute ) const
    {
    return ( iConversationAttrib & aAttribute );
    }

// ----------------------------------------------------------------------------
// CCsConversationEntry::CloneL
// Provides the clone of this object
// ----------------------------------------------------------------------------
EXPORT_C CCsConversationEntry* CCsConversationEntry::CloneL() const
    {
    CCsConversationEntry* cloneObject = CCsConversationEntry::NewL();
    CleanupStack::PushL(cloneObject);

    if (iContact)
        cloneObject->SetContactL(*iContact);

    cloneObject->SetConversationDir(iConversationDir);
    cloneObject->SetType(iConversationType);
    cloneObject->SetEntryId(iEntryID);
    cloneObject->SetSendState(iConversationStatus);
    cloneObject->ChangeAttributes(iConversationAttrib, ECsAttributeNone);
    if(iDescription)
        cloneObject->SetDescriptionL(*iDescription);
    cloneObject->SetTimeStampL(iTimeStamp);

    CleanupStack::Pop(cloneObject);
    return cloneObject;
    }

//------------------------------------------------------------------------------
// CCsConversationEntry::ExternalizeL
// Writes CCsConversationEntry variable to the stream
// ----------------------------------------------------------------------------
EXPORT_C  void CCsConversationEntry::ExternalizeL(RWriteStream& aStream) const
    {
    // write iEntryId into the stream
    aStream.WriteInt32L(iEntryID);

    // write iTimeStamp
    aStream.WriteReal64L(iTimeStamp);

    // write iConversationAttrib
    aStream.WriteUint16L(iConversationAttrib);

    if (iContact)
        {
        // write iContact length
        aStream.WriteInt8L(iContact->Length());
        // write iContact
        aStream << *iContact;
        }
    else
        {
        // write 0 contact len
        aStream.WriteInt8L(0);
        }

    // write iConDir length
    aStream.WriteUint8L(iConversationDir);

    if (iDescription)
        {
        // write iDescription length
        aStream.WriteUint16L(iDescription->Length());
        // write iDescription
        aStream << *iDescription;
        }
    else
        {
        // write 0 iDescription len
        aStream.WriteUint16L(0);
        }

    // write iConversationType
    aStream.WriteUint8L(iConversationType);

    // write iConversationStatus
    aStream.WriteUint8L(iConversationStatus);
    }

// ----------------------------------------------------------------------------
// CCsConversationEntry::InternalizeL
// Initializes CCsConversationEntry from stream
// ----------------------------------------------------------------------------
EXPORT_C  void CCsConversationEntry::InternalizeL(RReadStream& aStream)
    {
    // Read iEntryID length
    iEntryID = aStream.ReadInt32L();

    // read iTimeStamp
    iTimeStamp = aStream.ReadReal64L();

    // read iConversationAttrib
    iConversationAttrib = aStream.ReadUint16L();

    // Read iContact length
    TInt lenContact = aStream.ReadInt8L();

    if (lenContact > 0)
        {
        // read iContact
        iContact = HBufC::NewL(aStream, lenContact);
        }
    else
        iContact = NULL;

    // Read iConversationDir
    iConversationDir =
        static_cast<TCsDirection> (aStream.ReadUint8L());

    // Read iDescription length
    TInt lenDescription = aStream.ReadUint16L();

    if (lenDescription > 0)
        {
        // read iDescription
        iDescription = HBufC::NewL(aStream, lenDescription);
        }
    
    // Read iConversationType
    iConversationType =
        static_cast<TCsType> (aStream.ReadUint8L());

    // Read iConversationStatus
    iConversationStatus =
        static_cast<TCsSendState> (aStream.ReadUint8L());
    }

//-----------------------------------------------------------------------------
// CCsConversationEntry::Compare
// Compare to conversation entry object based on timestamp
//----------------------------------------------------------------------------
EXPORT_C TInt CCsConversationEntry::Compare(
        const CCsConversationEntry& aFirst,
        const CCsConversationEntry& aSecond)
    {

    if (aFirst.EntryId() < aSecond.EntryId())
        return -1;
    if (aFirst.EntryId() > aSecond.EntryId())
        return 1;
    return 0;
    }

//-----------------------------------------------------------------------------
// CCsConversationEntry::CompareById
// Compare to conversation entry object based on Entry Ids
//----------------------------------------------------------------------------
EXPORT_C TBool CCsConversationEntry::CompareById(
        const CCsConversationEntry& aFirst,
        const CCsConversationEntry& aSecond)
    {
    if (aFirst.EntryId() == aSecond.EntryId())
        return ETrue;
    
    return EFalse;
    }

//-----------------------------------------------------------------------------
// CCsConversationEntry::CompareByUnreadAttrib
// Compare to conversation entry object based on their unread attribute.
//----------------------------------------------------------------------------
EXPORT_C TBool CCsConversationEntry::CompareByUnreadAttrib(
        const CCsConversationEntry& aFirst,
        const CCsConversationEntry& aSecond)
    {
    if (aFirst.IsAttributeSet(ECsAttributeUnread) ==
            aSecond.IsAttributeSet(ECsAttributeUnread))
        return ETrue;

    return EFalse;
    }

//EOF
