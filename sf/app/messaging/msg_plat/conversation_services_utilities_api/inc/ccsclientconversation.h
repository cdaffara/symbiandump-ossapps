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
* Description:  This class shall be used for holding data required for
 *                server to client and vice versa. Also this shall be for
 *                one client many conversations 
 *
*/

#ifndef __C_CS_CLIENT_CONVERSATION_H__
#define __C_CS_CLIENT_CONVERSATION_H__

// SYSTEM INCLUDES

//USER INCLUDES
#include <ccsdefs.h>

// FORWARD CLASS
class CCsConversationEntry;

// CLASS DECLARATION
/**
 * CCsClientConversation
 * This class shall be used for holding data required for
 * server to client and vice versa. Also this shall be for
 * one client many conversations 
 * 
 * @remarks Lib: csutils.lib
 * @since S60 v5.0
 */
class CCsClientConversation: public CBase
    {
public:

    /**
     * 
     * NewL
     * Two phase construction
     * @return Instance of CCsClientConversation
     */
    IMPORT_C static CCsClientConversation* NewL();

    /**
     * Destructor
     */
    IMPORT_C ~CCsClientConversation();

public:

    /**
     * GetConversationEntryId
     * Provides the unique identifier of conversation 
     *
     * @return unique identifier of conversation
     * entry
     */
    IMPORT_C TCsConversationEntryID GetConversationEntryId() const;

    /**
     * SetConversationEntryId
     * Sets the unique identifier of this object 
     * 
     * @param aEnryId The unique identifier
     */
    IMPORT_C void SetConversationEntryId(
            TCsConversationEntryID aEnryId);

    /**
     * GetDisplayName
     * Provides the Conversation display name of this object
     *
     * @return Instance of Conversation display name of this object
     */
    IMPORT_C HBufC* GetDisplayName() const;

    /**
     * SetDisplayNameL
     * Sets the Conversation display name of this object
     *  
     * @param aDisplayName Instance of Conversation display name
     */
    IMPORT_C void SetDisplayNameL(
            const HBufC* aDisplayName);

    /**
     * GetContactId
     * Provides the Contact Id for the conversation
     *
     * @return Integer type iContactId
     */
    IMPORT_C TInt32 GetContactId() const;

    /**
     * SetContactId
     * Sets the phonebook Contact Id of the conversation 
     * 
     * @param aContactId to be set as contact id
     */
    IMPORT_C void SetContactId (const TInt32 aContactId);

    /**
     * GetConversationEntry
     * Provides the latest conversation Entry by 
     * timestamp of a conversation identified  by 
     * iConversationEntryID  of this object.
     *
     * @return Instance of Conversation Entry of this object
     */
    IMPORT_C CCsConversationEntry* GetConversationEntry() const;

    /**
     * SetConversationEntryL
     * Sets latest conversation Entry by timestamp of a conversation
     * identified  by iConversationEntryID  of this object.
     * 
     * @param aCsConversationEntry Instance of Conversation contact
     */
    IMPORT_C void SetConversationEntryL(
            const CCsConversationEntry* aCsConversationEntry);

    /**
     * CloneL
     * Gets the clone of the object 
     * 
     * @return Pointer to CCsClientConversation object
     */
    IMPORT_C CCsClientConversation* CloneL() const;

    /**
     * ExternalizeL
     * Writes 'this' object to the stream
     *
     * @param aStream WriteStream with externalized contents
     */
    IMPORT_C virtual void ExternalizeL(
            RWriteStream& aStream) const;

    /**
     * InternalizeL
     * Initializes/Read 'this' object from stream
     *
     * @param aStream ReadStream with data contents to be internalized
     */
    IMPORT_C virtual void InternalizeL(
            RReadStream& aStream);

    /**
     * CompareByTimeStamp
     * Compares 2 CCsClientConversation objects based 
     * on the latest event time stamp
     * 
     * @param obj1 CCsClientConversation object
     * @param obj2 CCsClientConversation object
     * 
     * @return -1 if 
     * aFirst.iConversationEntry->iTimeStamp > 
     *  aSecond.iConversationEntry->iTimeStamp
     * 1 if 
     * aFirst.iConversationEntry->iTimeStamp <
     *  aSecond.iConversationEntry->iTimeStamp
     * 0 if 
     * aFirst.iConversationEntry->iTimeStamp ==
     *  aSecond.iConversationEntry->iTimeStamp
     */
    IMPORT_C static TInt CompareByTimeStamp(
            const CCsClientConversation& obj1,
            const CCsClientConversation& obj2);

    /**
     * CompareByConversationEntryId
     * Compares 2 CCsClientConversation objects based 
     * on ConversationEntryId.
     * 
     * @param obj1 CCsClientConversation object
     * @param obj2 CCsClientConversation object
     * 
     * @return ETrue if 
     * aFirst.iConversationEntry->iEntryID ==
     *  aSecond.iConversationEntry->iEntryID
     * EFalse if 
     * aFirst.iConversationEntry->iEntryID !=
     *  aSecond.iConversationEntry->iEntryID
     */
    IMPORT_C static TBool CompareByConversationEntryId(
            const CCsClientConversation& obj1,
            const CCsClientConversation& obj2);
    
    /**
     * GetUnreadMessageCount
     * Returns the unread message count in the 
     * conversation
     * 
     * @return unread Messages count
     * @attention Range of unread message count is 0 to 65535.
     */
    IMPORT_C TUint16 GetUnreadMessageCount() const;
    
    /**
     * GetUnreadMessageCount
     * Returns the unread message count in the
     * conversation
     * 
     * @return unread Messages count
     * @attention Range of unread message count is 0 to 65535.
     */
    IMPORT_C void SetUnreadMessageCount(TUint16 aCount);


private:    // functions

    /**
     * Default Constructor
     */
    CCsClientConversation();

    /**
     * Second phase constructor
     */
    void ConstructL();

private:    // data members
    
    /**
     * iConversationEntryList
     * Latest converation entry
     */
    CCsConversationEntry* iConversationEntry;

    /**
     * iConversationEntryIDList
     * Unique id or index of the conversation
     */
    TCsConversationEntryID iConversationEntryID;

    /**
     * iDisplayName
     * Display name of conversation
     */
    HBufC*  iDisplayName;

    /**
     * iContactId
     * Contact Id of phonebook contact
     */
    TInt32 iContactId;
    
    /**
     * iUnreadMessagesCount
     * Count of unread messages in a conversation
     *
     * @attention Range of unread message count is 0 to 65535.
     */
    TUint16 iUnreadMessagesCount;
    };

#endif // __C_CS_CLIENT_CONVERSATION_H__

// End of File

