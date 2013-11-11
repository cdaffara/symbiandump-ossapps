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
* Description:  Contact match class
*
*/

#ifndef CPECONTACTMATCH_H
#define CPECONTACTMATCH_H

// INCLUDES
#include <cntdef.h>
#include <e32base.h>
#include <pevirtualengine.h>

// CONSTANTS
// Number of languages that use lastname-firstname order
const TInt KPENumberOfSwappedLanguages = 7;

// Languages which use lastname-firstname order
const TLanguage KPESwappedLanguages[ KPENumberOfSwappedLanguages ] = 
    {
    ELangHungarian,       // 17
    ELangTaiwanChinese,   // 29
    ELangHongKongChinese, // 30
    ELangPrcChinese,      // 31
    ELangJapanese,        // 32
    ELangKorean,          // 65
    ELangVietnamese       // 96
    };

// FORWARD DECLARATIONS
class CPEContactHandling;
class CPhCntContactId;
class CPhCntFactory;
class MPEDataStore;
class MPhCntMatch;
class CPhCntMatcher;
class MPESimContactMatch;

// CLASS DECLARATION

/**
*  Container for all Contact Handling owned instances of Phone Contact Finder
*/
class CPEContactMatch
    : public CBase
    {
    public:  // Constructors and destructor      

        /**
        * Two-phased constructor.
        * @param aOwner, handle to contact handling interface / owner of this class
        * @param aDataStore, handle to internal engine info interface
        * @return an instance of class CPEContactMatch
        */
        static CPEContactMatch* NewL(
            CPEContactHandling& aOwner,
            MPEDataStore& aDataStore );
        
        /**
        * Destructor.
        */
        ~CPEContactMatch();

    public: // New functions

        /**
        * Synchronous method for finding contact data specified with aContactId
        * @param aCallId, the call this contact is associated with
        * @return None
        */
        void MatchWithContactIdL( const TInt aCallId ); 

        /**
        * Search contact information synchronously matching with aPhoneNumber
        * @param aCallId, the call this contact is associated with
        * @return None (leaves with KErrNotFound if no match found or db in use)
        */
        void MatchWithNumberL( const TInt aCallId );

        /**
        * Returns contact picture data and call id association
        * NOTE! Ownership of the picture is transfered to caller.
        * @return contact thumbnail, equals NULL if not found
        */
        CFbsBitmap* ContactThumbnail();

    private:

        /**
        * C++ constructor.
        */
        CPEContactMatch(
            CPEContactHandling& aOwner,
            MPEDataStore& aDataStore );
        
        /**
        * Symbian OS constructor.
        */
        void ConstructL();

        /**
        * Copies contact information to remote info struct
        * @param aMatch a contact item 
        * @param aCallId, the call this contact is associated with
        */
        void CopyContactFieldsDataL(
            const MPhCntMatch& aMatch,
            const TInt aCallId );

        /**
        * Gets remote party name
        * @param aMatch a contact item 
        * @param aName, Remote party name
        */
        void GetName( const MPhCntMatch& aMatch,
            TPEContactName& aName);

        /**
         * Creates contact id.
         */
        CPhCntContactId* CreateContactIdL( const TDesC8& aContactId );

        /**
         * Creates contact id.
         */
        CPhCntContactId* CreateContactIdL( TContactItemId aContactId );

        /**
        * Starts loading thumbnail image
        * @param aContactId contact item with the desired thumbnail
        * @param aCallId call id for identifying which thumbnail is loaded
        * @return Symbian OS error code
        */
        TInt LoadThumbnail( const CPhCntContactId& aContactId, const TInt aCallId );


    private:    // Data

        // Handle to contact handling interface / owner of this class
        CPEContactHandling& iOwner;

        // Handle to internal engine info interface
        MPEDataStore& iDataStore;

        // Owned: library
        RLibrary iLibrary;

        // Owned: contact factory
        CPhCntFactory* iContactFactory;

        // Owned: contact matcher
        CPhCntMatcher* iContactMatcher;

        // Owned: sim contact matcher
        MPESimContactMatch* iSimContactMatcher;


    };

#endif // CPECONTACTMATCH_H

// End of File
