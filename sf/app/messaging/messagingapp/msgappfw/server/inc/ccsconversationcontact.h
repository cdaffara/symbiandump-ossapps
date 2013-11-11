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
 * Description:  CS Server Conversation Contact class. 
 *                This shall hold all the contact data associated with a 
 *                conversation like name, numbers and contact link 
 *
 */

#ifndef __C_CS_CONVERSATIONCONTACT_H
#define __C_CS_CONVERSATIONCONTACT_H

// SYSTEM INCLUDE FILES
#include <ccsdefs.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION
/** 
 * CS Server Conversation Contact Details class
 * This shall hold all the details of contact associated with a 
 * conversation like name, numbers and contact id
 */
class CCsConversationContact : public CBase
{
public:

    /**
     * Two phase construction
     */
    static CCsConversationContact* NewL();

    /**
     * Destructor
     */
    ~CCsConversationContact();

    /**
     * GetDisplayName
     * Get the first name of contact
     *  
     * @return  returns the First name        
     */
    HBufC* GetDisplayName() const;

    /**
     * SetDisplayNameL
     * Set the first name of contact  
     * 
     * @param aDisplayName first name       
     */
    void SetDisplayNameL(const TDesC& aDisplayName);

    /**
     * GetContactId
     * Get the Contact Id of conversation
     * 
     * @return  returns the integer contact Id
     */
    TInt32 GetContactId() const;

    /**
     * SetContactId
     * Set the Contact Id of conversation
     * 
     * @param aContactId the phone book contact link Id
     */
    void SetContactId(TInt32 aContactId);

    /**
     * AddPhoneNumberL
     * Adds the contact number inside phone number list
     * 
     * @param aPhoneNumber phone number
     */
    void AddPhoneNumberL(TDesC& aPhoneNumber);

    /**
     * GetPhoneNumberList
     * Returns a list of associated phone numbers in this contact
     * 
     * @param aPhoneNumbers array of phone numbers
     */
    void GetPhoneNumberList(RPointerArray<TDesC>& aPhoneNumbers);

    /**
     * MatchPhoneNumber
     * Check if the input phone number (aPhoneNumber) is  
     * associated with this contact. Compares for aNumDigits
     * 
     * @param aPhoneNumber  phone number
     * @param aNumDigits number of digits to be compared
     * @return TBool - ETrue if match is found, else EFalse
     */
    TBool MatchPhoneNumber(TDesC& aPhoneNumber, TInt aNumDigits) const;

private:

    /**
     * Constructor
     */
    CCsConversationContact();

    /**
     * Second phase constructor
     */
    void ConstructL();

private:

    /**
     * iFirstName
     * Conversation first name
     * Own.
     */
    HBufC* iDisplayName;   

    /**
     * iContactId
     * Conversation phonebook contact Id
     */
    TInt32 iContactId;

    /**
     * iPhoneNumberList
     * Used to store all associated phone numbers for a conversation
     * Own
     */
    RPointerArray<HBufC>* iPhoneNumberList;
};

#endif // __C_CS_CONVERSATIONCONTACT_H
