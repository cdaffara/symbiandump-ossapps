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
 * Description:  This class holds complete conversations for a contact 
 *                The class basically has two parts. One header which has
 *                Unread count, Unique Id, contact information and the body
 *                will have all conversations as list of conversation entries
 *
 */

#ifndef __C_CS_CONVERSATION_H
#define __C_CS_CONVERSATION_H

// SYSTEM INCLUDE FILES
#include <ccsdefs.h>

// FORWARD DECLARATIONS
class CCsConversationEntry;
class CCsConversationContact;

// CLASS DECLARATION
/** 
 * This class holds complete conversations for a contact 
 * The class basically has two parts. One header which has
 * Unread count, Unique Id, contact information and the body
 * will have all conversations as list of conversation entries
 */
class CCsConversation : public CBase
{
public:

    /**
     * Two phase construction
     */
    static CCsConversation* NewL();

    /**
     * Destructor
     */
    ~CCsConversation();

    /**
     * GetConversationId
     * This function shall return the conversation id
     * for this conversation
     * 
     * @return TCsConversationEntryID conversation id
     */
    TCsConversationEntryID GetConversationId() const;

    /**
     * SetConversationId
     * This function shall set conversation id
     * for this conversation
     * 
     * @param  aCsConversationEntryID Id to be set
     */
    void SetConversationId(TCsConversationEntryID aCsConversationEntryID);

    /**
     * GetDisplayName
     * Returns the display name of the Conversation
     * 
     * @return First name of conversation
     */
    HBufC* GetDisplayName() const;

    /**
     * GetContactId
     * Returns the contact Id of this conversation,
     * from which the contact information can be fetched.
     * 
     * @return - integer type contact Id
     */
    
    TInt32 GetContactId() const;

    /**
     * GetLatestEntryL
     * This function shall return the latest Conversation Entry
     * of this Conversation 
     * 
     * @return - latest conversation entry
     */
    CCsConversationEntry* GetLatestEntryL() const;

    /**
     * GetLatestUnreadEntryL
     * This function shall return the latest Unread Conversation Entry
     * of this Conversation
     *
     * @return - latest unread conversation entry
     */
    CCsConversationEntry* GetLatestUnreadEntryL() const;

    /**
     * GetEntryListL
     * This function shall return all the entries this conversation holds
     * 
     * @param aConversationEntryList, list of conversation entries
     */
    void
            GetEntryListL(
                          RPointerArray<CCsConversationEntry>* aConversationEntryList);
    
    /**
     * GetEntryListL
     * This function shall return all the entries this conversation holds
     * 
     * @param aConversationEntryList, list of conversation entries
	 * @param aStartIndex, start index for conversation entries.
	 * @param aEndIndex, end index for conversation entries.
     */
    void GetEntryListL(
            RPointerArray<CCsConversationEntry>* aConversationEntryList,
            TInt aStartIndex,
            TInt aEndIndex);

    /**
     * GetEntryL
     * This function returns the conversation entry at a specified index.
     * 
     * @param aIndex, entry list index
     */
    CCsConversationEntry* GetEntryL(TInt aIndex);

    /**
     * AddEntryL
     * Adds an entry to this conversation
     * 
     * @param aCsConversationEntry Entry to be added
     */
    void AddEntryL(CCsConversationEntry* aCsConversationEntry);

    /**
     * AddContactDetailsL
     * Adds contact details for the conversation
     * 
     * @param aContactId Contact id
     * @param aFirstName first name
     * @param aLastName last name
     */
    void AddContactDetailsL(
            TInt32 aContactId, 
            const HBufC* aDisplayName);

    /**
     * AddContactDetailsL
     * Over loaded method 
     * Add contact Number for the conversation
     * 
     * @param aContactNumber Contact number
     * @param aContactId Contact id
     */
    void AddContactDetailsL(
            TDesC& aContactNumber,
            TInt32 aContactID);

    /**
     * UpdateEntryL
     * Update the details of an conversation entry
     * and set event as new/update
     * 
     * @param aCsConversationEntry Conversation entry
     * @param aEvent event set to new/update
     */
    void UpdateEntryL(CCsConversationEntry* aCsConversationEntry,
                      TUint32& aEvent);

    /**
     * DeleteEntryL
     * Deletes the conversation entry
     * 
     * @param aindexDeletion index of the entry to be deleted
     */
    void DeleteEntryL(TInt aindexDeletion);

    /**
     * 
     * FindEntry
     * Finds the conversation entry in the conversation
     * 
     * @param aCsConversationEntry entry to be found
     * @return index of the entry if found else KErrNotFound
     */
    TInt FindEntry(CCsConversationEntry* aCsConversationEntry);

    /**
     *
     * FindUnreadEntry
     * Finds the unread conversation entry in the conversation
     *
     * @return index of the entry if found else KErrNotFound
     */
    TInt FindUnreadEntry() const;

    /**
     * GetEntryCount
     * Return the number of Conversation Entries in the Conversation
     * 
     * @return Count of Entries for this conversation
     */
    TInt GetEntryCount();

    /**
     * GetUnreadMessageCount
     * Returns the number of Unread Conversation Entries in the Conversation
     * 
     * @return the count of unread messsages
     */
    TUint16 GetUnreadMessageCount() const;

    /**
     * GetContact
     * Returns the contact details of this conversation
     * 
     * @return  Conversation Contact Object
     */
    CCsConversationContact* GetContact() const;

    /**
     * IsSpecialConversation
     * Returns whether the conversation is special conversation,
     * may it be Drafts/Bluetooth/IrDa
     * 
     * @return  TBool  ETrue  if special conversation else EFalse
     */
    TBool IsSpecialConversation();

    /**
     * Mark deleted
     * @param aDeleted ETrue if conversation is being deleted else it is EFalse
     */
    void MarkDeleted(TBool aDeleted);

    /**
     * Delete state of this conversation
     * 
     * @return ETrue if delete is ongoing for this conversation
     */
    TBool IsDeleted() const;

private:

    /**
     * Constructor
     */
    CCsConversation();

    /**
     * Second phase constructor
     */
    void ConstructL();

private:

    /**
     * iEntryList
     * This is array of actual conversation-entries
     */
    RPointerArray<CCsConversationEntry>* iEntryList;

    /**
     * iContact
     * This is contact details for this conversation
     * The details shall include name and phonebook contact link
     */
    CCsConversationContact* iContact;

    /**
     * iConversationID
     * Unique entry id for each conversation
     * This shall be used b/w clent server request
     */
    TCsConversationEntryID iConversationID;

    /**
     * iUnreadMessagesCount
     * Unread message count in the conversation
     * @attention Range of unread message count is 0 to 65535.
     */
    TUint16 iUnreadMessagesCount;

    /**
     * iDeleted
     * Enabled when delete is started for this conversation.
     */
    TBool iDeleted;
};

#endif // __C_CS_CONVERSATION_H
