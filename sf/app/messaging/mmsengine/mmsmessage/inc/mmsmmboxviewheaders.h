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
* Description:   Headers related to MMbox view
*
*/




#ifndef MMSMMBOXVIEWHEADERS_H
#define MMSMMBOXVIEWHEADERS_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class RMsvReadStream;
class RMsvWriteStream;

// CLASS DECLARATION

/**
*  Headers in MMBox view requests and view confirmations
*
*  @lib mmsutil.lib
*  @since 2.6
*/
class CMmsMMBoxViewHeaders : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CMmsMMBoxViewHeaders();

        /**
        * Destructor.
        */
        virtual ~CMmsMMBoxViewHeaders();

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
        * MmsStart accessor
        * @since 2.6
        * @return Index of first MM to have information returned
        */
        inline TUint MmsStart() const;

        /**
        * MmsStart mutator
        * @since 2.6
        * @param aMmsStart Index of first MM to have information returned
        */
        inline void SetMmsStart ( TUint aMmsStart );

        /**
        * MmsLimit accessor
        * @since 2.6
        * @return Maximum number of selected messages
        *     0 = No Message related information returned in MMS View confirmation
        *         ( Quotas and totals only )
        *     KMaxTUint32 = Information for all remaining messages SHALL be returned
        *         ( header will be absent from request )
        *     0 < limit < KMaxTUint32 = return info for specified number of messages
        */
        inline TUint MmsLimit() const;

        /**
        * MmsLimit mutator
        * @since 2.6
        * @param aMmsLimit Maximum number of selected messages
        *     0 = No Message related information returned in MMS View confirmation
        *         ( Quotas and totals only )
        *     KMaxTUint32 = Information for all remaining messages SHALL be returned
        *         ( header will be absent from request )
        *     0 < aMmsLimit < KMaxTUint32 = return info for specified number of messages
        */
        inline void SetMmsLimit ( TUint aMmsLimit );

        /**
        * MmsTotals accessor
        * @since 2.6
        * @return MMBox totals requested header (KMmsYes/KmmsNo)
        */
        inline TInt MmsTotals() const;

        /**
        * MmsTotals mutator
        * @since 2.6
        * @param aMmsTotals MMBox totals requested header (KMmsYes/KmmsNo)
        */
        inline void SetMmsTotals ( TInt aMmsTotals );

        /**
        * MmsQuotas accessor
        * @since 2.6
        * @return MMBox quota requested header (KMmsYes/KmmsNo)
        */
        inline TInt MmsQuotas() const;

        /**
        * MmsQuotas mutator
        * @since 2.6
        * @param aMmsQuotas MMBox quota requested header (KMmsYes/KmmsNo)
        */
        inline void SetMmsQuotas ( TInt aMmsQuotas );

        /**
        * MmsMessageCount accessor
        * @since 2.6
        * @return Number of messages in MMbox view confirmation PDU
        */
        inline TUint MmsMessageCount() const;

        /**
        * MmsMessageCount mutator
        * @since 2.6
        * @param aMmsMessageCount Number of messages in MMbox view confirmation PDU
        */
        inline void SetMmsMessageCount ( TUint aMmsMessageCount );

        /**
        * MMBoxTotalNumber accessor
        * @since 2.6
        * @return Total number of messages in user's MMBox
        */
        inline TUint MMBoxTotalNumber() const;

        /**
        * MMBoxTotalNumber mutator
        * @since 2.6
        * @param aMMBoxTotalNumber Total number of messages in user's MMBox
        */
        inline void SetMMBoxTotalNumber ( TUint aMMBoxTotalNumber );

        /**
        * MMBoxTotalSize accessor
        * @since 2.6
        * @return Total size of messages in user's MMBox
        */
        inline TUint MMBoxTotalSize() const;

        /**
        * MMBoxTotalSize mutator
        * @since 2.6
        * @param aMMBoxTotalSize Total size of messages in user's MMBox
        */
        inline void SetMMBoxTotalSize( TUint aMMBoxTotalSize );

        /**
        * MMBoxQuotaNumber accessor
        * @since 2.6
        * @return Quota defined for user's MMBox, number of messages.
        */
        inline TUint MMBoxQuotaNumber() const;

        /**
        * MMBoxQuotaNumber mutator
        * @since 2.6
        * @param aMMBoxQuotaNumber Quota defined for user's MMBox, number of messages.
        */
        inline void SetMMBoxQuotaNumber ( TUint aMMBoxQuotaNumber );

        /**
        * MMBoxQuotaSize accessor
        * @since 2.6
        * @return Quota defined for user's MMBox, total size.
        */
        inline TUint MMBoxQuotaSize() const;

        /**
        * MMBoxQuotaSize mutator
        * @since 2.6
        * @param aMMBoxQuotaSize Quota defined for user's MMBox, total size.
        */
        inline void SetMMBoxQuotaSize ( TUint aMMBoxQuotaSize );

        /**
        * Attribute array accessor
        * Use standard RArray functions to manipulate the array.
        * @since 2.6
        * @return reference to the information element array.
        *     The elements must be assigned numbers as defined in Table 21
        *     in MMS encapsulation document version 1.2 or later.
        *     (All values will have the high byte set)
        */
        inline RArray<TUint>& AttributeArray();

        /**
        * Message state array accessor.
        * These are states used to filter wanted messages in a MMbox view request
        * Use standard RArray functions to manipulate the array.
        * Entries are message MMBox States: Draft/Sent/New/Retrieved/Forwarded
        * @since 2.6
        * @return reference to the information element array.
        *     The elements must be assigned numbers:
        *     KMmsDraft, KMmsSent, KMmsNew, KMmsRetrieved, KMmsForwarded
        */
        inline RArray<TInt>& MMStateArray();


    public: // Functions from base classes

       
    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

    public:     // Data
   
    protected:  // Data

    private:    // Data
        // Index of first message in view request
        TUint32  iMmsStart;
        // Maximum number of message info PDUs to be returned
        TUint32  iMmsLimit;
        // Total number requested
        TInt32  iMmsTotals; // Yes/No, default No
        // MMBox Quota requested
        TInt32  iMmsQuotas; // Yes/No, default No
        // Message count in content of view confirmation PDU
        TUint32  iMmsMessageCount;
        // Total number of messages in MMBox
        TUint32  iMMBoxTotalNumber;
        // total size of messages in MMBox
        TUint32  iMMBoxTotalSize;
        // Message number quota in MMBox
        TUint32  iMMBoxQuotaNumber;
        // Message size quota in MMBox
        TUint32  iMMBoxQuotaSize;
        // Attributes array, list of information elements for each message
        // Use InsertInOrder(), FindInOrder() Remove(), and [] operator to access entries.
        // Call Reset() to clear the array, Count() to find number of items
        RArray<TUint> iAttributes;
        // State array. These values correspond to iMmsMMState member in
        // CMmsMmBoxMessageHeaders class, but in a view request or view confirmation 
        // the states form an array, and the corresponding header may appear multiple
        // times. Therefore an array is needed.
        // The single state should not be used when constructing MMBox view requests or
        // handling MMBox view responses
        // Use InsertInOrder(), FindInOrder() Remove(), and [] operator to access entries.
        // Call Reset() to clear the array, Count() to find number of items
        RArray<TInt> iMmsMMStates;

         
    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

    };

#include "mmsmmboxviewheaders.inl"

#endif      // MMSMMBOXVIEWHEADERS_H   
            
// End of File
