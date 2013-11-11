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
* Description:  CS Utils side class to hold conversation entry.
 *
*/

#ifndef __C_CS_CONVERSATION_ENTRY_H__
#define __C_CS_CONVERSATION_ENTRY_H__

// SYSTEM INCLUDES

//USER INCLUDES
#include <ccsdefs.h>

// CLASS DECLARATION
/**
 * CCsConversationEntry
 * This is a utils side class to hold the conversation entry
 *
 * @remarks Lib: csutils.lib
 * @since S60 v5.0
 */
class CCsConversationEntry: public CBase
    {
public:

    /**
     * NewL
     * Two phase construction
     * @return Instance of CCsConEntry
     */
    IMPORT_C static CCsConversationEntry* NewL();

    /**
     * Destructor
     */
    //TODO check the convention and how to make this funct non-sharable
    IMPORT_C ~CCsConversationEntry();

public:

    /**
     * EntryId
     * Provides the unique identifier of conversation
     * entry
     *
     * @return unique identifier of conversation
     * entry
     */
    IMPORT_C TInt EntryId() const;

    /**
     * SetEntryId
     * Sets the unique identifier of this object
     *
     * @param aEnryId The unique identifier
     */
    IMPORT_C void SetEntryId(const TInt aEnryId);

    /**
     * TimeStamp
     * Provides the Conversation TimeStamp of this object
     *
     * @return Instance of Conversation TimeStamp of this object
     */
    IMPORT_C TInt64 TimeStamp() const;

    /**
     * SetTimeStampL
     * Sets the Conversation TimeStamp of this object
     *
     * @param aTimeStamp Instance of Conversation TimeStamp
     */
    IMPORT_C void SetTimeStampL(const TInt64 aTimeStamp);

    /**
     * Contact
     * Provides the Conversation contact of this object
     *
     * @return Instance of Conversation contact number of this object
     */
    IMPORT_C HBufC* Contact() const;

    /**
     * SetContactL
     * Sets the Conversation contact of this object
     *
     * @param aContact Instance of Conversation contact
     */
    IMPORT_C void SetContactL(const TDesC& aContact);

    /**
     * ConversationDir
     * Provides the Conversation direction of this object
     *
     * @return Conversation direction of this object
     */
    IMPORT_C TCsDirection ConversationDir() const;

    /**
     * SetConversationDir
     * Sets the Conversation direction of this object
     *
     * @param aConversationDir Conversation direction
     */
    IMPORT_C void SetConversationDir(
            const TCsDirection aConversationDir);

    /**
     * Description
     * Provides the whole Description/body of this conversation
     *
     * @return Instance of Conversation contact of this object
     */
    IMPORT_C HBufC* Description() const;

    /**
     * SetDescriptionL
     * Sets the Description/body of this object
     *
     * @param aDescription Instance of Description of 
     *        this object
     */
    IMPORT_C void SetDescriptionL(
            const TDesC& aDescription);

    /**
     * GetType
     * this shall return the conversation flags for the conversation
     *
     * @return TCsType conversation flag
     */
    IMPORT_C TCsType GetType() const;

    /**
     * SetType
     * Sets the Conversation flags
     *
     * @param aCsConversationType shall be enum flags
     */
    IMPORT_C void SetType(const TCsType& aCsConversationType);

    /**
     * GetSendState
     * this shall return the conversation status
     *
     * @return  TCsSendState conversation status
     */
    IMPORT_C TCsSendState GetSendState() const;

    /**
     * SetSendState
     * Sets the Conversation status
     *
     * @param aCsConversationStatus shall be enum flags
     */
    IMPORT_C void SetSendState(const TCsSendState& aCsConversationStatus);

    /**
     * ChangeAttributes
     * Sets or Clears the given attribute(s).
     * Fields to change are specified using a bitmask of TCsAttribute values.
     * @param aSetAttributes A bitmask of the fields to set.
     * @param aClearAttributes A bitmask of the fields to clear.
     * @see TCsAttribute
     * @example
     * - To set ECsAttributeDraft attribute:
     *      ChangeAttributes( ECsAttributeDraft, ECsAttributeNone );
     * - To clear ECsAttributeDraft attribute:
     *      ChangeAttributes( ECsAttributeNone, ECsAttributeDraft );
     * - To set ECsAttributeNew & ECsAttributeDraft attribute and clear
     *   ECsAttributeAttachment & ECsAttributePriority attribute:
     *      ChangeAttributes( ECsAttributeNew | ECsAttributeDraft,
     *                        ECsAttributeAttachment | ECsAttributePriority );
     */
    IMPORT_C void ChangeAttributes( TUint16 aSetAttributes,
            TUint16 aClearAttributes );

    /**
     * IsAttributeSet
     * Checks if the give attribute is set.
     * @param aAttribute One of the TCsAttribute values to be checked.
     * @return ETrue if given attribute is set else EFalse.
     * @attention Two or more attributes cannot be ORed and sent.
     * @see TCsAttribute
     */
    IMPORT_C TBool IsAttributeSet( TCsAttribute aAttribute ) const;

    /**
     * CloneL
     * Gets the clone of the object
     *
     * @return Pointer to CCsConversationEntry object
     */
    IMPORT_C CCsConversationEntry* CloneL() const;

    /**
     * ExternalizeL
     * Writes 'this' to the stream
     *
     * @param aStream WriteStream with externalized contents
     */
    IMPORT_C virtual void ExternalizeL(RWriteStream& aStream) const;

    /**
     * InternalizeL
     * Initializes 'this' from stream
     *
     * @param aStream ReadStream with data contents to be internalized
     */
    IMPORT_C virtual void InternalizeL(RReadStream& aStream);

    /**
     * Compare
     * Compare to conversation entry object based on timestamp
     *
     * @param aFirst first conversationentry object
     * @param aSecond second conversationentry object
     *
     * return -1 if aFirst.iTimeStamp < aSecond.iTimeStamp
     *         1 if aFirst.iTimeStamp > aSecond.iTimeStamp
     *         0 if equal
     */
    IMPORT_C static TInt Compare(
            const CCsConversationEntry& aFirst, 
            const CCsConversationEntry& aSecond);
    
    /**
     * CompareById
     * Compare to conversation entry object based on Entry Id
     *
     * @param aFirst first conversationentry object
     * @param aSecond second conversationentry object
     *
     * return ETrue if aFirst.EntryId == aSecond.EntryId
     *        EFalse Otherwise
     */
    IMPORT_C static TBool CompareById(
            const CCsConversationEntry& aFirst, 
            const CCsConversationEntry& aSecond);

    /**
     * CompareByUnreadAttrib
     * Compare to conversation entry object based on their unread attribute.
     *
     * @param aFirst first conversationentry object
     * @param aSecond second conversationentry object
     *
     * return ETrue if aFirst.IsAttributeSet(ECsAttributeUnread) ==
     * aSecond.IsAttributeSet(ECsAttributeUnread)
     *        EFalse Otherwise
     */
    IMPORT_C static TBool CompareByUnreadAttrib(
            const CCsConversationEntry& aFirst,
            const CCsConversationEntry& aSecond);

private:    // functions

    /**
     * Default Constructor
     */
    CCsConversationEntry();

    /**
     * Second phase constructor
     */
    void ConstructL();

private:	// data members

    /**
     * iTimeStamp
     * Conversation TimeStamp
     * use to log the timestamp of the conversation entry.
     * Own.
     */
    TInt64 iTimeStamp;

    /**
     * iContact
     * Conversation Contact
     * use to identify the contact number of conversation entry.
     * Own.
     */
    HBufC*  iContact;

    /**
     * iDescription
     * Conversation  description
     * this shall be use to store whole description/body
     * of the conversation entry.
     * Own.
     */
    HBufC*  iDescription;

    /**
     * iEntryID
     * unique d fetched from log ingine
     * this corresponds to each call log
     * Own.
     */
    TInt iEntryID;

    /**
     * iConversationAttrib
     * Used to denote msg attributes like unread, sent, drafts etc.
     * Own.
     * @see TCsAttribute
     */
    TCsAttribute iConversationAttrib;

    /**
     * iConversationDir
     * Conversation Direction
     * corresponds to the Incoming , outgoing , missed  etc
     * Own.
     */
    TCsDirection iConversationDir;

    /**
     * iConversationType
     * this shall be use to denote the read/unread/draft/attachment
     * conversation entry.
     * Own.
     */
    TCsType  iConversationType;

    /**
     * iConversationStatus
     * this shall be use to denote the read/unread/draft/attachment
     * conversation entry.
     * Own.
     */
    TCsSendState  iConversationStatus;
    };

#endif // __C_CS_CON_ENTRY_H__

// End of File
