/*
* Copyright (c) 2003-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Structure to hold MMS encapsulation 1.1 version
*                previoulsy-sent-by and previously-sent-date information  
*
*/




#ifndef MMSSENDINGCHAIN_H
#define MMSSENDINGCHAIN_H

//  INCLUDES
#include <e32base.h>
#include <mtclbase.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Object for storing previously-sent-by objects.
*
*  @lib mmsutil.lib
*  @since 2.1
*/
class CMmsSendingChain : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CMmsSendingChain();

        /**
        * Destructor.
        */
        virtual ~CMmsSendingChain();

    public: // New functions
        
        /**
        * Ordering function.
        * @since 2.1
        * @param aFirst first item to be compared
        * @param aSecond second item to be compared
        * @return 1 if aFirst > aSecond, -1 if aFirst < aSecond, 0 if aFirst == aSecond
        */
        static TInt Compare( const CMmsSendingChain& aFirst, const CMmsSendingChain& aSecond );

        /**
        * Internalize.
        * @since 2.1
        * @param aStream opened read stream
        */
        void InternalizeL( RMsvReadStream& aStream );

        /**
        * Externalize.
        * @since 2.1
        * @param aStream opened write stream
        */
        void ExternalizeL( RMsvWriteStream& aStream ) const;

        /**
        * Order accessor.
        * @since 2.1
        * @return iOrder
        */
        inline TUint Order() const;

        /**
        * Order mutator.
        * @since 2.1
        * @param sets iOrder
        */
        inline void SetOrder( TUint aOrder );

        /**
        * Date accessor.
        * @since 2.1
        * @return date in seconds from 1970-01-01.
        */
        inline TInt64 Date() const;

        /**
        * Date mutator. 
        * @since 2.1
        * @param aDate date in seconds from 1970-01-01.
        */
        inline void SetDate( const TInt64 aDate );

        /**
        * Sender accessor.
        * @since 2.1
        * @return Real address of sender (for example phone number).
        */
        IMPORT_C TPtrC Sender() const;

        /**
        * Sender mutator.
        * @since 2.1
        * @param aSender
        */
        IMPORT_C void SetSenderL( const TDesC& aSender );

        /*
        * How much room this needs on disk
        * @since 2.1
        * @return amount of bytes the data in this item needs
        */
        TInt Size() const;

    public: // Functions from base classes

    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

    public:     // Data
    
    protected:  // Data

    private:    // Data
        
        // sending order
        TUint32  iOrder;
        // sending date as seconds from 1.1.1970
        TInt64  iDate;
        // the address of the sender
        HBufC*  iSender;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#include "mmssendingchain.inl"

#endif      // MMSSENDINGCHAIN_H   
            
// End of File
