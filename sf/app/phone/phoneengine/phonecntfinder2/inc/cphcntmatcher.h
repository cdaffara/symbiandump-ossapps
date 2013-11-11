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

#ifndef CPHCNTMATCHER_H
#define CPHCNTMATCHER_H

//  INCLUDES
#include <e32base.h>
#include <cntdef.h>     // TContactItemId
#include <badesca.h>

#include "mphcntmatch.h"

// CONSTANTS
const TInt KPhCntInvalidFieldId = KErrNotFound;

// DATA TYPES
typedef TInt TFieldId;

// FORWARD DECLARATIONS

class MVPbkContactLink;

// CLASS DECLARATION

/**
*  Abstract class for making matches.
*
*  @lib PhoneCntFinder
*  @since 1.0
*/
class CPhCntMatcher 
    : public CBase
    {
    public:  // Constructors and destructor  

        /**
        * Match number to phonebook.
        * @param aMatch Found match, owership tranferred. NULL if not found any.
        * @param aTelNumber Number to match against.
        * @return Error code: KErrNone - one match found
        *                     KErrNotFound - no matches found
        *                     KErrAlreadyExists - several matches -> no match
        *                     KErrUnderflow - Number too short for matching
        *                     KErrAccessDenied - Can't get phonebook instance
        *                     other - normal Symbian OS error
        */
        virtual TInt MatchNumber( 
            MPhCntMatch*& aMatch, 
            const TDesC& aTelNumber ) = 0;

        /**
        * Match number to phonebook.
        * @param aMatch Found match, owership tranferred. NULL if not found any.
        * @param aTelNumber Number to match against.
        * @param aContactId Current contact.
        * @return Error code: KErrNone - one match found
        *                     KErrNotFound - no matches found
        *                     KErrAlreadyExists - several matches -> no match
        *                     KErrUnderflow - Number too short for matching
        *                     KErrAccessDenied - Can't get phonebook instance
        *                     other - normal Symbian OS error
        */
        virtual TInt MatchNumber(
            MPhCntMatch*& aMatch,
            const TDesC& aTelNumber,
            const CPhCntContactId& aContactLink ) = 0;


        /**
        * Match number to phonebook.
        * @param aMatch Found match, owership tranferred. NULL if not found any.
        * @param aNumber Number to match against.
        * @param aAllowUsernameMatch 
        *                        is ETrue if username is sufficient for match.
        * @param aCharsForMatching Length of string to be used in matching.
        *                       Starts from the end of VoIP number. Domain part
        *                       not included for matching.
        * @return Error code: KErrNone - one match found
        *                     KErrNotFound - no matches found
        *                     other - normal Symbian OS error
        */    
        virtual TInt MatchVoipNumber(
            MPhCntMatch*& aMatch,
            const TDesC& aNumber,
            const TBool aAllowUsernameMatch,
            TInt aCharsForMatching = 0 ) = 0;
        
         /**
        * Gets VoIP call contact info from phonebook
        * by contact id.
        * @since Series60 3.0
        * @param aMatch for found match, owership tranferred. Empty if not found.
        * @param aContactId for current contact.
        * @return Error code: KErrNone - VoIP call contact found
        *                     KErrNotFound - no VoIP call contact found
        *                     other - normal Symbian OS error
        */    
        virtual TInt MatchVoipNumber(
            MPhCntMatch*& aMatch,
            const CPhCntContactId& aContactId ) = 0;
            
        /**
         * Determines if contact has other type of numbers
         * than VoIP numbers.
         * 
         * @since S60 v3.2.
         * @param aContactId Id of the contact.
         * @return ETrue - CS numbers found from contact.
         *         EFalse - No CS numbers found from contact.
         */
        virtual TBool HasCSNumbers( 
            const CPhCntContactId& aContactId ) = 0;
            
        
        /**
        * Match number to phonebook.
        * @param aMatch Found match, owership tranferred. NULL if not found any.
        * @param aTelNumber Number to match against.
        * @param aContactId  a contact id (unigue UID)
        * @return Error code: KErrNone - one match found
        *                     KErrNotFound - no matches found
        *                     KErrAlreadyExists - several matches -> no match
        *                     KErrUnderflow - Number too short for matching
        *                     KErrAccessDenied - Can't get phonebook instance
        *                     other - normal Symbian OS error
        */
        virtual TInt MatchNumber( 
            MPhCntMatch*& aMatch, 
            const TDesC& aTelNumber,
            TInt aContactId ) = 0;

    };

#endif      // CPHCNTMATCHER_H  
            
// End of File
