/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:
*
*/

#ifndef MPHCNTMATCH_H
#define MPHCNTMATCH_H

//  INCLUDES
#include <cntdef.h>     // TContactItemId
#include <badesca.h>

class MVPbkContactLink;
class CPhCntContactId;

// CLASS DECLARATION

/**
*  Abstact match result. 
*
*  @lib PhoneCntFinder
*  @since 1.0
*/
class MPhCntMatch
    {
    public:
        
        /**
        * Enumerates CLI types.
        *
        * ECliEmpty - not set/found.
        * ECliNumber - phone number from contact.
        * ECliName - name from contact.
        * ECliCompany - company name from contact.
        */
        enum TCliType
            {
            ECliEmpty,  
            ECliNumber,
            ECliName,
            ECliCompany
            };
            
        enum TNumberType
            {
            ENone = 0,
            EMobileNumber,
            EStandardNumber,
            EFaxNumber,
            EPagerNumber,
            EVoipNumber,
            EVideoNumber,
            EAssistantNumber,
            ECarNumber,
            EHomeNumber,
            EWorkNumber            
            }; 

        /**
        * To destroy the item. Must be called when item is not needed
        * any more.
        */
        virtual void Release() = 0;

        /**
         * Contact id.
         * @return Contact finder contact id.
         */
        virtual CPhCntContactId* ContactId() const = 0;
        
        /**
        * Get the number type.
        * @return Phonebook number type.
        */
        virtual TNumberType NumberType() const = 0;

        /**
        * Get CLI value.
        * @param aCliText CLI is copied here. User must delete after usage.
        * @return CLI type.
        */
        virtual TCliType Cli( HBufC*& aCliText ) const = 0;

        /**
        * First name field from contact.
        * @return Field text. Empty if there isn't one specified.
        */
        virtual TPtrC FirstName() const = 0;

        /**
        * Last name field from contact.
        * @return Field text. Empty if there isn't one specified.
        */
        virtual TPtrC LastName() const = 0;

        /**
        * Company name field from contact.
        * @return Field text. Empty if there isn't one specified.
        */
        virtual TPtrC CompanyName() const = 0;

        /**
        * Group name field from contact.
        * @return Field text. Empty if there isn't one specified.
        */
        virtual TPtrC GroupName() const = 0;
        
        /**
        * Number field from contact (from matched field).
        * @return Field text. Empty if there isn't one specified.
        */
        virtual TPtrC Number() = 0;

        /**
        * Ringing tone field from contact.
        * @return Field text. Empty if there isn't one specified.
        */
        virtual TPtrC PersonalRingingTone() const = 0;

        /**
        * Find if contact belong to any contact groups given.
        * @param aGroupArray Compared contact groups.
        * @return ETrue is belogns at least one group from parameter.
        */
        virtual TBool BelongsToGroups( 
            const CArrayFix<TContactItemId>& aGroupArray ) const = 0;

        /**
        * Returns text to speech text for this contact. Used in
        * text to speech ringing tone.
        * @since Series 60 3.0
        * @return descriptor, ownership passed.
        */
        virtual HBufC* TextToSpeechTextL() const = 0;
        
        /**
        * Call text field from contact.
        * @since Series 60 3.1
        * @return Field text. Empty if there isn't one specified.
        */
        virtual TPtrC CallText() const = 0;

        /**
        * Call image field from contact.
        * @since Series 60 3.1
        * @return Field text. Empty if there isn't one specified.
        */
        virtual TPtrC CallImage() const = 0;
        
        /**
        * Can used to check if contact has a thumbnail image.
        * @since Series 60 3.1
        * @return ETrue if has thumbnail, otherwise EFalse.
        */
        virtual TBool HasThumbnailImage() const = 0;      
        
        /**
        * Returns all dtmf numbers which are found from
        * a single contact
        * @Since Series S60 3.2
        * @return array of DTMF numbers.
        */
        virtual CDesCArray& AllDtmfNumbers() const = 0;    

    };

#endif      // MPHCNTMATCH_H   
            
// End of File
