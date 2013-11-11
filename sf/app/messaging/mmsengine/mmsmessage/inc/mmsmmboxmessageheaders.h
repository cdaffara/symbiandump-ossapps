/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   MMBox related headers present in message PDUs
*
*/




#ifndef MMSMMBOXMESSAGEHEADERS_H
#define MMSMMBOXMESSAGEHEADERS_H

//  INCLUDES
#include <e32base.h>
#include "mmsmmboxflags.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class RMsvReadStream;
class RMsvWriteStream;
class CDesC8Array;

// CLASS DECLARATION

/**
*  MMBox related headers present in message PDUs.
*
*  @lib mmsutil.lib
*  @since 2.6
*/
class CMmsMMBoxMessageHeaders : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CMmsMMBoxMessageHeaders* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CMmsMMBoxMessageHeaders();

    public: // New functions
        
        /**
        * Internalize.
        * @since 2.6
        * @param aStream opened read stream
        */
        void InternalizeL( RMsvReadStream& aStream );

        /**
        * Externalize.
        * @since 2.6
        * @param aStream opened write stream
        */
        void ExternalizeL( RMsvWriteStream& aStream ) const;

        /*
        * How much room this needs on disk
        * @since 2.6
        * @return amount of bytes the data in this item needs
        */
        TInt Size() const;

        /**
        * MMBox Store request accessor
        * @since 2.6
        * @return Should message be stored to Mmbox? (KMmsYes/KMmsNo)
        */
        inline TInt MmsStore() const;

        /**
        * MMBox Store request mutator
        * @since 2.6
        * @param aMmsStore Should message be stored to Mmbox? (KMmsYes/KMmsNo)
        */
        inline void SetMmsStore ( TInt aMmsStore );

        /**
        * Message MMBoxState accessor
        * @since 2.6
        * @return message MMBox State Draft/Sent/New/Retrieved/Forwarded
        */
        inline TInt MmsMMState() const;

        /**
        * Message MMBoxState mutator
        * @since 2.6
        * @param aMmsMMState message MMBox State Draft/Sent/New/Retrieved/Forwarded
        */
        inline void SetMMState( TInt aMmsMMState );

        /**
        * MMBox store state accessor
        * @since 2.6
        * @return Has MMS message been stored in MMBox? (KMmsYes/KMmsNo)
        */
        inline TInt MmsStored() const;

        /**
        * MMBox store state mutator - needed by decode
        * @since 2.6
        * @param aMmsStored Has MMS message been stored in MMBox? (KMmsYes/KMmsNo)
        */
        inline void SetMmsStored( TInt aMmsStored );

        /**
        * Status code from operation where message storing to MMBox was requested.
        * @since 2.6
        * @return Message MMBox Store status (Success or error code)
        */
        inline TInt MmsStoreStatus() const;

        /**
        * Set Status code from operation where message storing to MMBox was requested.
        * @since 2.6
        * @param aMmsStoreStatus Message MMBox Store status (Success or error code)
        */
        inline void SetMmsStoreStatus( TInt aMmsStoreStatus );

        /**
        * Store status text accessor.
        * @since 2.6
        * @return MMBox MmsStoreStatus text string.
        */
        IMPORT_C TPtrC MmsStoreStatusText() const;

        /**
        * Store status  text mutator.
        * @since 2.6
        * @param aText MMBox MmsStoreStatus text string,
        */
        IMPORT_C void SetMmsStoreStatusTextL( const TDesC& aText );

        /** 
        * Give access to the content location array
        * @return Content location array
        */
        inline CDesC8Array& ContentLocationList() const;

        /**
        * Keyword array accessor
        * @since 2.6
        * @return reference to the keyword array.
        */
        inline RPointerArray<CMmsMMBoxFlags>& KeywordArray();

        /**
        * Add item to keyword array
        * Items are always appended (no sorting order defined).
        * If item with specified keyword exists, token is updated.
        * @since 2.6
        * @param aToken MMBox Operation connected with keyword (Add/Remove/Filter)
        * @param aKeyword Keyword connected to object
        */
        IMPORT_C void AppendKeywordItemL( TInt aToken, const TDesC& aKeyword );

        /**
        * Remove keyword item from MMBox flags array
        * @since 2.6
        * @param aKeyword first item with this keyword is deleted.
        */
        IMPORT_C void DeleteKeywordItem( const TDesC& aKeyword );

        /**
        * Find item corresponding to given keyword from array
        * @since 2.6
        * @return index of item with keyword,
        *         KErr NotFound, if keyword not in array
        */
        IMPORT_C TInt FindItemWithKeyword( const TDesC& aKeyword );

    public: // Functions from base classes

    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

        /**
        * C++ default constructor.
        */
        CMmsMMBoxMessageHeaders();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Delete old data before internalizing
        */
        void Reset();

    public:     // Data
    
    protected:  // Data

    private:    // Data
        // MMBox store request
        TInt32   iMmsStore; // Yes/No
        // Message state in MMBox
        TInt32   iMmsMMState; // Draft/Sent/New/Retrieved/Forwarded
        // Message stored flag
        TInt32   iMmsStored; // Yes/No
        // MMBox store status
        TInt32   iMmsStoreStatus; // error code
        // Operator response text in case of error
        HBufC*   iMmsStoreStatusText;
        // Content location array to support PDUs that contain more than one
        // content location header
        CDesC8Array* iContentLocationArray;
        // MMS flags array, no ordering
        // collection keywords with add/remove/filter status
        RPointerArray<CMmsMMBoxFlags> iKeywordArray;
 
    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

    };

#include "mmsmmboxmessageheaders.inl"

#endif      // MMSMMBOXMESSAGEHEADERS_H   
            
// End of File
